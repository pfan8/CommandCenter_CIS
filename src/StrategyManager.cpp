#include "StrategyManager.h"
#include "CCBot.h"
#include "JSONTools.h"
#include "Util.h"
#include "MetaType.h"
#include "Common.h"

Strategy::Strategy()
{

}

Strategy::Strategy(const std::string & name, const CCRace & race, const BuildOrder & buildOrder, const Condition & scoutCondition, const Condition & attackCondition, const int expandNum = 0)
    : m_name            (name)
    , m_race            (race)
    , m_buildOrder      (buildOrder)
    , m_wins            (0)
    , m_losses          (0)
    , m_scoutCondition  (scoutCondition)
    , m_attackCondition (attackCondition)
	, m_expandNum		(expandNum)
{

}

// constructor
StrategyManager::StrategyManager(CCBot & bot)
	: m_bot(bot)
	, m_bases_safe(false)
	, m_exbase_onbuild(false)
{
}

void StrategyManager::onStart()
{
    readStrategyFile(m_bot.Config().ConfigFileLocation);
}

void StrategyManager::onFrame()
{
	// Update variables that we will need later. 
	m_bases_safe = areBasesSafe();
	//RecalculateMacroGoal();
	checkExpand();
	checkExpandBunker();
	HandleUnitAssignments();

}

bool StrategyManager::areBasesSafe()
{
	for (const auto & enemy_unit : m_bot.UnitInfo().getUnits(Players::Enemy))
	{
		for (const auto & potential_base : m_bot.UnitInfo().getUnits(Players::Self))
		{
			if (Util::IsTownHall(potential_base.getUnitPtr())
				&& Util::DistSq(potential_base.getUnitPtr()->pos, enemy_unit.getUnitPtr()->pos) < (30 * 30))
			{
				return false;
			}
		}
	}
	return true;
}

const Strategy & StrategyManager::getCurrentStrategy() const
{
    auto strategy = m_strategies.find(m_bot.Config().StrategyName);

    BOT_ASSERT(strategy != m_strategies.end(), "Couldn't find Strategy corresponding to strategy name: %s", m_bot.Config().StrategyName.c_str());

    return (*strategy).second;
}

const BuildOrder & StrategyManager::getOpeningBookBuildOrder() const
{
    return getCurrentStrategy().m_buildOrder;
}

bool StrategyManager::scoutConditionIsMet() const
{
    return getCurrentStrategy().m_scoutCondition.eval();
}

bool StrategyManager::attackConditionIsMet() const
{
    return getCurrentStrategy().m_attackCondition.eval();
}

// assigns units to various managers
void StrategyManager::HandleUnitAssignments()
{
	//SetScoutUnits();

	// Repair any damaged supply depots. If our base is safe, lower the wall. Otherwise, raise the wall. 
	for (const auto & unit : m_bot.UnitInfo().getUnits(Players::Self))
	{
		// Find all the depots and perform some actions on them. 
		if (unit.getType().isSupplyProvider())
		{
			// If the depot may die, go repair it. 
			/*if (unit.isMaxHealthed())
				Micro::SmartRepairWithSCVCount(unit, 2, m.Info());*/

			if (m_bases_safe)
			{
				m_bot.Actions()->UnitCommand(unit.getUnitPtr(), sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
			}
			else
			{
				m_bot.Actions()->UnitCommand(unit.getUnitPtr(), sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_RAISE);
			}
		}
	}
}

void StrategyManager::checkExpand() const
{
	if (!m_exbase_onbuild)
	{
		auto & stratage = getCurrentStrategy();
		UnitType base_type;
		switch (stratage.m_race)
		{
		case sc2::Race::Terran:
			base_type = UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_bot);
			break;
		case sc2::Race::Zerg:
			base_type = UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_bot);
			break;
		case sc2::Race::Protoss:
			base_type = UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_bot);
			break;
		default:
			break;
		}
		if (m_bot.UnitInfo().getUnitTypeCount(Players::Self, base_type, false) < stratage.m_expandNum + 1
			&& shouldExpandNow())
		{
			m_bot.expand(base_type);
		}
	}
}

void StrategyManager::checkExpandBunker()
{
	for (auto & bm : m_bunkerMarines)
	{
		if (bm.second.second > 0)
		{
			m_bot.buildBunkerMarines(bm.first, bm.second.first);
		}
		else if (bm.second.first && bm.second.second <= 0)
		{
			const auto & it = std::find(m_bunkerMarines.begin(), m_bunkerMarines.end(), bm);
			m_bunkerMarines.erase(it);
		}
	}
	std::vector<Unit> units = m_bot.GetUnits();
	for (auto & bunker_unit : units)
	{
		if (bunker_unit.getType().getAPIUnitType() != sc2::UNIT_TYPEID::TERRAN_BUNKER)
		{
			continue;
		}

		for (auto & marine_unit : units)
		{
			if (bunker_unit.isMaxCargo())
			{
				break;
			}

			if (marine_unit.getType().getAPIUnitType() == sc2::UNIT_TYPEID::TERRAN_MARINE)
			{
				m_bot.Actions()->UnitCommand(marine_unit.getUnitPtr(), sc2::ABILITY_ID::SMART, bunker_unit.getUnitPtr());
			}
		}
	}
}

bool StrategyManager::shouldExpandNow() const
{
	if (m_bot.Observation()->GetMinerals() > 400 && m_bases_safe)
	{
		return true;
	}
    return false;
}

void StrategyManager::addStrategy(const std::string & name, const Strategy & strategy)
{
    m_strategies[name] = strategy;
}

const UnitPairVector StrategyManager::getBuildOrderGoal() const
{
    return std::vector<UnitPair>();
}

const UnitPairVector StrategyManager::getProtossBuildOrderGoal() const
{
    return std::vector<UnitPair>();
}

const UnitPairVector StrategyManager::getTerranBuildOrderGoal() const
{
    return std::vector<UnitPair>();
}

const UnitPairVector StrategyManager::getZergBuildOrderGoal() const
{
    return std::vector<UnitPair>();
}


void StrategyManager::onEnd(const bool isWinner)
{

}

void StrategyManager::readStrategyFile(const std::string & filename)
{
    CCRace race = m_bot.GetPlayerRace(Players::Self);
    std::string ourRace = Util::GetStringFromRace(race);

    std::ifstream file(filename);
    json j;
    file >> j;

#ifdef SC2API
    const char * strategyObject = "SC2API Strategy";
#else
    const char * strategyObject = "BWAPI Strategy";
#endif

    // Parse the Strategy Options
    if (j.count(strategyObject) && j[strategyObject].is_object())
    {
        const json & strategy = j[strategyObject];

        // read in the various strategic elements
        JSONTools::ReadBool("ScoutHarassEnemy", strategy, m_bot.Config().ScoutHarassEnemy);
        JSONTools::ReadString("ReadDirectory", strategy, m_bot.Config().ReadDir);
        JSONTools::ReadString("WriteDirectory", strategy, m_bot.Config().WriteDir);

        // if we have set a strategy for the current race, use it
        if (strategy.count(ourRace.c_str()) && strategy[ourRace.c_str()].is_string())
        {
            m_bot.Config().StrategyName = strategy[ourRace.c_str()].get<std::string>();
        }

        // check if we are using an enemy specific strategy
        JSONTools::ReadBool("UseEnemySpecificStrategy", strategy, m_bot.Config().UseEnemySpecificStrategy);
        if (m_bot.Config().UseEnemySpecificStrategy && strategy.count("EnemySpecificStrategy") && strategy["EnemySpecificStrategy"].is_object())
        {
            // TODO: Figure out enemy name
            const std::string enemyName = "ENEMY NAME";
            const json & specific = strategy["EnemySpecificStrategy"];

            // check to see if our current enemy name is listed anywhere in the specific strategies
            if (specific.count(enemyName.c_str()) && specific[enemyName.c_str()].is_object())
            {
                const json & enemyStrategies = specific[enemyName.c_str()];

                // if that enemy has a strategy listed for our current race, use it
                if (enemyStrategies.count(ourRace.c_str()) && enemyStrategies[ourRace.c_str()].is_string())
                {
                    m_bot.Config().StrategyName = enemyStrategies[ourRace.c_str()].get<std::string>();
                    m_bot.Config().FoundEnemySpecificStrategy = true;
                }
            }
        }

        // Parse all the Strategies
        if (strategy.count("Strategies") && strategy["Strategies"].is_object())
        {
            const json & strategies = strategy["Strategies"];
            for (auto it = strategies.begin(); it != strategies.end(); ++it) 
            {
                const std::string & name = it.key();
                const json & val = it.value();              
                
                BOT_ASSERT(val.count("Race") && val["Race"].is_string(), "Strategy is missing a Race string");
                CCRace strategyRace = Util::GetRaceFromString(val["Race"].get<std::string>());

				BOT_ASSERT(val.count("ExpandBaseNum") && val["ExpandBaseNum"].is_number(), "Strategy is missing a ExpandBaseNum number");
				int expandBaseNum = val["ExpandBaseNum"];
                
                BOT_ASSERT(val.count("OpeningBuildOrder") && val["OpeningBuildOrder"].is_array(), "Strategy is missing an OpeningBuildOrder array");
                BuildOrder buildOrder;
                const json & build = val["OpeningBuildOrder"];
                for (size_t b(0); b < build.size(); b++)
                {
                    if (build[b].is_string())
                    {
                        MetaType MetaType(build[b].get<std::string>(), m_bot);
                        buildOrder.add(MetaType);
                    }
                    else
                    {
                        BOT_ASSERT(false, "Build order item must be a string %s", name.c_str());
                        continue;
                    }
                }

                BOT_ASSERT(val.count("ScoutCondition") && val["ScoutCondition"].is_array(), "Strategy is missing a ScoutCondition array");
                Condition scoutCondition(val["ScoutCondition"], m_bot);
                
                BOT_ASSERT(val.count("AttackCondition") && val["AttackCondition"].is_array(), "Strategy is missing an AttackCondition array");
                Condition attackCondition(val["AttackCondition"], m_bot);

                addStrategy(name, Strategy(name, strategyRace, buildOrder, scoutCondition, attackCondition, expandBaseNum));
            }
        }
    }
}

void StrategyManager::activateExpandState(sc2::Point2DI exbase_position)
{
	BM new_bunker_marine(exbase_position, std::make_pair(false, 4));
	m_bunkerMarines.push_back(new_bunker_marine);
	m_exbase_onbuild = true;
}

void StrategyManager::resetExpandState()
{
	m_exbase_onbuild = false;
}

void StrategyManager::validateBM(sc2::Point2DI exbase_pos, BMBuildType type)
{
	for (auto & bm : m_bunkerMarines)
	{
		if (bm.first == exbase_pos)
		{
			switch (type)
			{
			case BMBuildType::BUNKER:
				bm.second.first = true;
				break;
			case BMBuildType::MARINE:
				bm.second.second--;
				break;
			default:
				break;
			}
		}
	}
}
