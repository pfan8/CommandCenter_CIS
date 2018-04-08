#include "Common.h"
#include "BuildingPlacer.h"
#include "CCBot.h"
#include "Building.h"
#include "Util.h"

BuildingPlacer::BuildingPlacer(CCBot & bot)
    : m_bot(bot)
{

}

void BuildingPlacer::onStart()
{
    m_reserveMap = std::vector< std::vector<bool> >(m_bot.Map().width(), std::vector<bool>(m_bot.Map().height(), false));
}

bool BuildingPlacer::isInResourceBox(int tileX, int tileY) const
{
    if (m_bot.Bases().getBaseAtLocation(tileX, tileY)) 
		return true;
	return false;
}

// makes final checks to see if a building can be built at a certain location
bool BuildingPlacer::canBuildHere(const int bx, const int by, const sc2::UnitTypeID type) const
{
	// Don't build to close to a base unless that building is a command center.
	if (isInResourceBox(bx, by) && !Util::IsTownHallType(type) && !Util::IsRefineryType(type))
		return false;

	// Will the starcraft engine allow the building to be built at the location?
	if (!buildable(bx, by, type))
		return false;

	// We are not allowed to build on any tile that we have reserved.
	for (const auto pos : getTilesForBuilding(type, sc2::Point2DI{ bx,by }))
	{
		if (!m_bot.Map().isOnMap(pos.x, pos.y)
			|| m_reserveMap[pos.x][pos.y])
			return false;
	}
	// If none of the above conditions failed, we must be allowed to build at the location.
	return true;
}

//returns true if we can build this type of unit here with the specified amount of space.
bool BuildingPlacer::canBuildHereWithSpace(int bx, int by, const Building & b, int buildDist) const
{
    UnitType type = b.type;

    //if we can't build here, we of course can't build here with space
    if (!canBuildHere(bx, by, type.getAPIUnitType()))
    {
        return false;
    }

    // height and width of the building
    int width  = type.tileWidth();
    int height = type.tileHeight();

    // TODO: make sure we leave space for add-ons. These types of units can have addons:

    // define the rectangle of the building spot
    int startx = bx - (width / 2) - buildDist;
    int starty = by - (height / 2) - buildDist;
    int endx   = bx + (width / 2) + buildDist;
    int endy   = by + (height / 2) + buildDist;

    // TODO: recalculate start and end positions for addons

    // if this rectangle doesn't fit on the map we can't build here
    if (startx < 0 
		|| starty < 0 
		|| endx > m_bot.Map().width() 
		//|| endx < bx + width 
		|| endy > m_bot.Map().height()
		|| buildDist < 0)
    {
        return false;
    }

	// Account for add-ons.
	sc2::UNIT_TYPEID sc2_type = type.getAPIUnitType();
	if (sc2_type == sc2::UNIT_TYPEID::TERRAN_BARRACKS
		|| sc2_type == sc2::UNIT_TYPEID::TERRAN_FACTORY
		|| sc2_type == sc2::UNIT_TYPEID::TERRAN_STARPORT)
	{
		endx += 3;

		// Make sure there is room to build the addon. 
		if (!buildable(bx + 3, by, sc2_type))
			return false;
	}

    // if we can't build here, or space is reserved, or it's in the resource box, we can't build here
    for (int x = startx; x < endx; x++)
    {
        for (int y = starty; y < endy; y++)
        {
			// Refineries can only be built in one spot, it does not matter what is next to them. 
			// Supply depots are usually part of a wall. They have to be built flush with the buildings next to them. 
            if (!type.isRefinery() && !type.isSupplyDepot())
            {
                if (!buildable(x, y, type.getAPIUnitType()) || m_reserveMap[x][y])
                {
                    return false;
                }
            }
        }
    }

    return true;
}

std::vector<sc2::Point2DI> BuildingPlacer::getTilesForBuilding(const sc2::UnitTypeID type, const sc2::Point2DI pos) const
{
	assert(Util::isBuilding(type));
	std::vector<sc2::Point2DI> return_vector;

	// Remember to take add-ons into account!
	int space_for_add_on = 0;
	if (type == sc2::UNIT_TYPEID::TERRAN_BARRACKS
		|| type == sc2::UNIT_TYPEID::TERRAN_FACTORY
		|| type == sc2::UNIT_TYPEID::TERRAN_STARPORT)
		space_for_add_on = 4;

	const int building_width = Util::GetUnitTypeWidth(type, m_bot);
	const int building_height = Util::GetUnitTypeHeight(type, m_bot);
	const size_t rwidth = m_reserveMap.size();
	const size_t rheight = m_reserveMap[0].size();

	const int startx = pos.x - (building_width / 2);
	const int starty = pos.y - (building_height / 2);
	const int endx = pos.x + (building_width / 2);
	const int endy = pos.y + (building_height / 2);

	for (int y = starty; y < starty + building_height && y < rheight; y++)
	{
		for (int x = startx; x < startx + building_width + space_for_add_on && x < rwidth; x++)
		{
			return_vector.push_back(sc2::Point2DI{ x,y });
		}
	}
	return return_vector;
}

CCTilePosition BuildingPlacer::getBuildLocationNear(const Building & b, int buildDist) const
{
	/* Timer t;
	 t.start();*/

    // get the precomputed vector of tile positions which are sorted closes to this location
    auto & closestToBuilding = m_bot.Map().getClosestTilesTo(b.desiredPosition);

    //double ms1 = t.getElapsedTimeInMilliSec();

    // iterate through the list until we've found a suitable location
    for (size_t i(0);i < closestToBuilding.size() && i < 1000; i++)
    {
		auto & pos = closestToBuilding.at(i);
        if (canBuildHereWithSpace(pos.x, pos.y, b, buildDist))
        {
            //double ms = t.getElapsedTimeInMilliSec();
            //printf("Building Placer Took %d iterations, lasting %lf ms @ %lf iterations/ms, %lf setup ms\n", (int)i, ms, (i / ms), ms1);

            return pos;
        }
    }

    //double ms = t.getElapsedTimeInMilliSec();
    //printf("Building Placer Failure: %s - Took %lf ms\n", b.type.getName().c_str(), ms);

    return CCTilePosition(0, 0);
}

sc2::Point2DI BuildingPlacer::getBuildLocationForType(const sc2::UnitTypeID type) const
{
	if (build_location_cache_.find(type) != build_location_cache_.end())
	{
		return build_location_cache_.at(type);
	}

	sc2::Point2DI desired_loc;
	if (Util::IsRefineryType(type))
	{
		desired_loc = getRefineryPosition();
	}

	//else if (type == sc2::UNIT_TYPEID::TERRAN_BARRACKS)
	//{
	//	desired_loc = m_bot.GetProxyManager().GetProxyLocation();
	//}


	// Make a wall if necessary.
	else if (type == sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT && m_bot.UnitInfo().getNumDepots(sc2::Unit::Alliance::Self) < 3)
	{
		if (m_bot.GetMinerals() > 80)
		{
			desired_loc = getNextCoordinateToWallWithBuilding(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT);
			//std::cout << "desired_loc1:" << desired_loc.x << " + " << desired_loc.y << std::endl;
		}
	}

	// Find the next expansion location. 
	else if (Util::IsTownHallType(type))
	{
		CCTilePosition ccpos = m_bot.Bases().getNextExpansion(sc2::Unit::Alliance::Self);
		const sc2::Point2D next_expansion_location = sc2::Point2D(ccpos.x, ccpos.y);
		desired_loc = Util::ToPoint2DI(next_expansion_location);
	}
	// If no special placement code is required, get a position somewhere in our starting base.
	else
	{
		desired_loc = Util::ToPoint2DI(m_bot.GetStartLocation());
	}

	// Return a "null" point if the desired_loc was not on the map. 
	if (!m_bot.Map().isOnMap(desired_loc))
	{
		return sc2::Point2DI{ 0, 0 };
	}
	UnitType unit_type = UnitType(type, m_bot);
	Building b = Building(unit_type, desired_loc);
	//std::cout << "desired_loc2:" << desired_loc.x << " + " << desired_loc.y << std::endl;
	build_location_cache_[type] = getBuildLocationNear(b, m_bot.Config().BuildingSpacing);
	//build_location_cache_[type] = desired_loc;
	return build_location_cache_[type];
}

sc2::Point2DI BuildingPlacer::getNextCoordinateToWallWithBuilding(const sc2::UnitTypeID building_type) const
{
	sc2::Point2D closest_point(0, 0);
	double closest_distance = std::numeric_limits<double>::max();

	// Get the closest ramp to our starting base. 
	const sc2::Point2D base_location = m_bot.Bases().getPlayerStartingBaseLocation(Players::Self)->getPosition();

	const float base_height = m_bot.Map().terrainHeight(base_location.x, base_location.y);

	// No need to iterate through the edges of the map, as the edge can never be part of our wall. 
	// The smallest building is width 2, so shrink the iteration dimensions by that amount. 
	for (int y = 2; y < (m_bot.Map().height() - 2); ++y)
	{
		for (int x = 2; x < (m_bot.Map().width() - 2); ++x)
		{
			// If we can walk on it, but not build on it, it is most likely a ramp.
			// TODO: That is not actually correct, come up with a beter way to detect ramps. 
			if (m_bot.Map().IsAnyTileAdjacentToTileType(sc2::Point2DI{ x, y }, MapTileType::Ramp, sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT)
				&& canBuildHere(x, y, sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT))
			{
				// The first depot in a wall has to be next to, well, a wall. 
				// This allows the depot wall to be built correctly on AbyssalReefLE.
				if (m_bot.Config().MapName == "AbyssalReefLE" &&
					m_bot.UnitInfo().getNumDepots(sc2::Unit::Alliance::Self) < 1
					&& !(m_bot.Map().isTileCornerOfTileType(sc2::Point2DI{ x, y }, MapTileType::CantWalk)))
					continue;

				if (/*IsTileCornerReserved(sc2::Point2DI{x, y})
					|| */m_bot.Map().terrainHeight(x, y) < 10.5)
					continue;

				// Don't wall of at Proxima Station's pocket expansion.
				if (m_bot.Config().MapName == "ProximaStationLE"
					&& m_bot.UnitInfo().getNumDepots(sc2::Unit::Alliance::Self) < 3
					&& (y < 49 || y > 119))
					continue;

				// Don't wall of at Acolyte's pocket expansion.
				if (m_bot.Config().MapName == "AcolyteLE"
					&& m_bot.UnitInfo().getNumDepots(sc2::Unit::Alliance::Self) < 3
					&& (x < 38 || x > 130))
					continue;

				const sc2::Point2D point{ static_cast<float>(x),
					static_cast<float>(y) };
				const double distance = Util::DistSq(point, base_location);
				if (distance < closest_distance)
				{
					closest_point = point;
					closest_distance = distance;
				}
			}
		}
	}
	//std::cout << "closest_point:" << closest_point.x << " + " << closest_point.y << std::endl;
	//std::cout << "closest_distance:" << closest_distance << std::endl;
	return Util::ToPoint2DI(closest_point);
}

bool BuildingPlacer::tileOverlapsBaseLocation(int x, int y, UnitType type) const
{
    // if it's a resource depot we don't care if it overlaps
    if (type.isResourceDepot())
    {
        return false;
    }

    // dimensions of the proposed location
    int tx1 = x;
    int ty1 = y;
    int tx2 = tx1 + type.tileWidth();
    int ty2 = ty1 + type.tileHeight();

    // for each base location
    for (const BaseLocation * base : m_bot.Bases().getBaseLocations())
    {
        // dimensions of the base location
        int bx1 = (int)base->getDepotPosition().x;
        int by1 = (int)base->getDepotPosition().y;
        int bx2 = bx1 + Util::GetTownHall(m_bot.GetPlayerRace(Players::Self), m_bot).tileWidth();
        int by2 = by1 + Util::GetTownHall(m_bot.GetPlayerRace(Players::Self), m_bot).tileHeight();

        // conditions for non-overlap are easy
        bool noOverlap = (tx2 < bx1) || (tx1 > bx2) || (ty2 < by1) || (ty1 > by2);

        // if the reverse is true, return true
        if (!noOverlap)
        {
            return true;
        }
    }

    // otherwise there is no overlap
    return false;
}

bool BuildingPlacer::buildable(const int x, const int y, const sc2::UnitTypeID type) const
{
	if (!m_bot.Map().isOnMap(x, y) || !m_bot.Map().canBuildTypeAtPosition(x, y, type))
		return false;


	return true;
}

void BuildingPlacer::reserveTiles(sc2::UnitTypeID building_type, sc2::Point2DI building_location)
{
	// Reserve the tiles.
	for (const auto pos : getTilesForBuilding(building_type, building_location))
	{
		//std::cout << "reserved_point:" << pos.x << " + " << pos.y << std::endl;
		m_reserveMap[pos.x][pos.y] = true;
	}

	// Remove the placed building from the cache.
	const auto build_loc_iter = build_location_cache_.find(building_type);
	if (build_loc_iter != build_location_cache_.end())
	{
		build_location_cache_.erase(build_loc_iter);
	}
}

void BuildingPlacer::drawReservedTiles()
{
    if (!m_bot.Config().DrawReservedBuildingTiles)
    {
        return;
    }

    int rwidth = (int)m_reserveMap.size();
    int rheight = (int)m_reserveMap[0].size();

    for (int x = 0; x < rwidth; ++x)
    {
        for (int y = 0; y < rheight; ++y)
        {
            if (m_reserveMap[x][y]/* || isInResourceBox(x, y)*/)
            {
                m_bot.Map().drawTile(x, y, CCColor(127, 39, 195));
            }
        }
    }
}

void BuildingPlacer::freeTiles(int bx, int by, int width, int height)
{
    int rwidth = (int)m_reserveMap.size();
    int rheight = (int)m_reserveMap[0].size();

    for (int x = bx; x < bx + width && x < rwidth; x++)
    {
        for (int y = by; y < by + height && y < rheight; y++)
        {
            m_reserveMap[x][y] = false;
        }
    }
}

CCTilePosition BuildingPlacer::getRefineryPosition() const
{
    CCPosition closestGeyser(0, 0);
    double minGeyserDistanceFromHome = std::numeric_limits<double>::max();
    CCPosition homePosition = m_bot.GetStartLocation();

    for (auto & unit : m_bot.GetUnits())
    {
        if (!unit.getType().isGeyser())
        {
            continue;
        }

        CCPosition geyserPos(unit.getPosition());

        // check to see if it's next to one of our depots
        bool nearDepot = false;
        for (auto & unit : m_bot.UnitInfo().getUnits(Players::Self))
        {
            if (unit.getType().isResourceDepot() && Util::Dist(unit, geyserPos) < 10)
            {
                nearDepot = true;
            }
        }

        if (nearDepot)
        {
            double homeDistance = Util::Dist(unit, homePosition);

            if (homeDistance < minGeyserDistanceFromHome)
            {
                minGeyserDistanceFromHome = homeDistance;
                closestGeyser = unit.getPosition();
            }
        }
    }

    return CCTilePosition((int)closestGeyser.x, (int)closestGeyser.y);
}

bool BuildingPlacer::isReserved(int x, int y) const
{
    int rwidth = (int)m_reserveMap.size();
    int rheight = (int)m_reserveMap[0].size();
    if (x < 0 || y < 0 || x >= rwidth || y >= rheight)
    {
        return false;
    }

    return m_reserveMap[x][y];
}

