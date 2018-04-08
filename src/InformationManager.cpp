#include <sc2api/sc2_api.h>

#include "InformationManager.h"
#include "unitInfo.h"
#include "Util.h"

InformationManager::InformationManager(sc2::Agent & bot)
	: m_bot(bot)
	// m_map must be be before m_bases in order to satisfy dependancies.
	, m_map(bot, *this)
	, m_bases(bot, m_map)
	, m_unit_info(bot, m_bases)
{ }

void InformationManager::OnStart()
{
	m_map.OnStart();
	m_unit_info.OnStart();
	m_bases.OnStart();

	// WARNING: Will only support two player games. 
	const auto player_id = m_bot.Observation()->GetPlayerID();
	for (auto & player_info : m_bot.Observation()->GetGameInfo().player_info)
	{
		if (player_info.player_id == player_id)
		{
			player_race_[sc2::Unit::Alliance::Self] = player_info.race_actual;
		}
		else
		{
			player_race_[sc2::Unit::Alliance::Enemy] = player_info.race_requested;
		}
	}
	dps_m_map = std::vector<std::vector<float>>{};
	dps_m_map.clear();
	for (int y = 0; y < m_map.TrueMapHeight(); ++y)
	{
		dps_m_map.push_back(std::vector<float>());
		for (int x = 0; x < m_map.TrueMapWidth(); ++x)
		{
			// There is an inherit "danger" for traveling through any square. 
			// Don't use 0, otherwise we won't find the "shortest and safest path"
			dps_m_map[y].push_back(1);
		}
	}
}

void InformationManager::OnUnitCreated(const sc2::Unit* unit)
{
}

void InformationManager::OnUnitDestroyed(const sc2::Unit* unit)
{
	m_unit_info.OnUnitDestroyed(unit);
}

void InformationManager::OnFrame()
{
	m_map.OnFrame();
	m_unit_info.OnFrame();
	m_bases.OnFrame(*this);

	// Reset dps_m_map
	for (const auto & unit : m_unit_info.GetUnits(sc2::Unit::Alliance::Enemy))
	{
		for (int y = 0; y < dps_m_map.size(); ++y)
		{
			for (int x = 0; x < dps_m_map[y].size(); ++x)
			{
				dps_m_map[y][x] = 1;
			}
		}
	}

	// Update dps_m_map
	for (const auto & m_unit_infopair : m_unit_info.GetUnitInfoMap(sc2::Unit::Alliance::Enemy))
	{
		const auto unit_info = m_unit_infopair.second;
		const int damage = Util::GetAttackDamage(unit_info.type, m_bot);
		if (damage == 0) continue;
		int range = static_cast<int>(Util::GetAttackRange(unit_info.type, m_bot)) + 1;
		//  Melee units are dangerous too.
		if (range == 0 && !Util::IsBuilding(unit_info.type)) range = 2;

		for (float y = 0; y < dps_m_map.size(); ++y)
		{
			for (float x = 0; x < dps_m_map[y].size(); ++x)
			{
				for (float falloff = 0; falloff < 25; ++falloff)
				{
					// Danger zone falloff only applies to army units. 
					if (!Util::IsWorker(unit_info.unit) && falloff > 1)
						if (Util::DistSq(sc2::Point2D{ x, y }, unit_info.lastPosition) <= static_cast<float>(range*range + falloff * falloff))
						{
							dps_m_map[y][x] += static_cast<float>(damage / (falloff * 2 + 1));
							break;
						}
				}
			}
		}
	}

	for (int y = 0; y < m_map.TrueMapHeight(); ++y)
	{
		for (int x = 0; x < m_map.TrueMapWidth(); ++x)
		{
			if (!m_map.IsWalkable(x, y))
				dps_m_map[y][x] = 999;
		}
	}
}

const BaseLocationManager & InformationManager::Bases() const
{
	return m_bases;
}

const MapTools & InformationManager::Map() const
{
	return m_map;
}

UnitInfoManager & InformationManager::UnitInfo()
{
	return m_unit_info;
}

const sc2::Race & InformationManager::GetPlayerRace(sc2::Unit::Alliance player) const
{
	return player_race_.at(player);
}

const ::UnitInfo * InformationManager::GetClosestUnitInfoWithJob(const sc2::Point2D reference_point, const UnitMission unit_mission) const
{
	const ::UnitInfo * closest_unit = nullptr;
	double closest_distance = std::numeric_limits<double>::max();

	for (auto & m_unit_infopair : m_unit_info.GetUnitInfoMap(sc2::Unit::Alliance::Self))
	{
		const ::UnitInfo & unit_info = m_unit_infopair.second;
		if (unit_info.mission == unit_mission)
		{
			const double distance = Util::DistSq(reference_point, unit_info.unit->pos);
			if (!closest_unit || distance < closest_distance)
			{
				closest_unit = &unit_info;
				closest_distance = distance;
			}
		}
	}

	return closest_unit;
}

const sc2::Unit * InformationManager::GetClosestUnitWithJob(const sc2::Point2D reference_point, const UnitMission unit_mission) const
{
	const sc2::Unit * closest_unit = nullptr;
	double closest_distance = std::numeric_limits<double>::max();

	for (auto & m_unit_infopair : m_unit_info.GetUnitInfoMap(sc2::Unit::Alliance::Self))
	{
		const ::UnitInfo & unit_info = m_unit_infopair.second;
		if (unit_info.mission == unit_mission)
		{
			const double distance = Util::DistSq(reference_point, unit_info.unit->pos);
			if (distance < closest_distance)
			{
				closest_unit = unit_info.unit;
				closest_distance = distance;
			}
		}
	}

	return closest_unit;
}

const UnitInfo* InformationManager::GetClosestUnitInfoWithJob(const sc2::Point2D point,
	const std::vector<UnitMission> mission_vector) const
{
	const ::UnitInfo * closest_unit = nullptr;
	double closest_distance = std::numeric_limits<double>::max();

	for (auto & m_unit_infopair : m_unit_info.GetUnitInfoMap(sc2::Unit::Alliance::Self))
	{
		if (std::find(mission_vector.begin(), mission_vector.end(), m_unit_infopair.second.mission) != mission_vector.end())
		{
			const double distance = Util::DistSq(m_unit_infopair.second.unit->pos, point);
			if (distance < closest_distance)
			{
				closest_unit = &m_unit_infopair.second;
				closest_distance = distance;
			}
		}
	}

	return closest_unit;
}


const sc2::Unit* InformationManager::GetClosestUnitWithJob(const sc2::Point2D point,
	const std::vector<UnitMission> mission_vector) const
{
	const sc2::Unit * closest_unit = nullptr;
	double closest_distance = std::numeric_limits<double>::max();

	for (auto & m_unit_infopair : m_unit_info.GetUnitInfoMap(sc2::Unit::Alliance::Self))
	{
		const ::UnitInfo & unit_info = m_unit_infopair.second;
		// Buildings are part of the unit info map, but they do not have jobs. 
		if (!Util::IsWorkerType(unit_info.type)) continue;
		if (std::find(mission_vector.begin(), mission_vector.end(), unit_info.mission) != mission_vector.end())
		{
			const double distance = Util::DistSq(m_unit_infopair.second.unit->pos, point);
			if (distance < closest_distance)
			{
				closest_unit = unit_info.unit;
				closest_distance = distance;
			}
		}
	}

	return closest_unit;
}

const sc2::Unit* InformationManager::GetClosestUnitOfType(const sc2::Unit* reference_unit,
	const sc2::UnitTypeID reference_type_id) const
{
	const sc2::Unit* closest_unit = nullptr;
	double closest_distance = std::numeric_limits<double>::max();

	for (auto unit : m_unit_info.GetUnits(sc2::Unit::Alliance::Self))
	{
		if (unit->unit_type == reference_type_id)
		{
			const double distance = Util::DistSq(unit->pos, reference_unit->pos);
			if (!closest_unit || distance < closest_distance)
			{
				closest_unit = unit;
				closest_distance = distance;
			}
		}
	}

	return closest_unit;
}


const sc2::Unit* InformationManager::GetClosestMineralField(const sc2::Unit* reference_unit) const
{
	const sc2::Unit* closest_unit = nullptr;
	double closest_distance = std::numeric_limits<double>::max();

	for (auto unit : m_unit_info.GetUnits(sc2::Unit::Alliance::Neutral))
	{
		if (unit->mineral_contents)
		{
			const double distance = Util::DistSq(unit->pos, reference_unit->pos);
			if (!closest_unit || distance < closest_distance)
			{
				closest_unit = unit;
				closest_distance = distance;
			}
		}
	}

	return closest_unit;
}

const sc2::Unit* InformationManager::FindNeutralUnitAtPosition(const sc2::Point2DI point) const
{
	for (const auto unit : m_unit_info.GetUnits(sc2::Unit::Alliance::Neutral))
	{
		if (Util::ToPoint2DI(unit->pos) == point)
		{
			return unit;
		}
	}
	return nullptr;
}

std::vector<std::vector<float>> InformationManager::GetDPSMap() const
{
	return dps_m_map;
}
