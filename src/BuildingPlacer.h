#pragma once

#include "Common.h"
#include "BuildingData.h"

class CCBot;
class BaseLocation;

class BuildingPlacer
{
    CCBot & m_bot;

    std::vector< std::vector<bool> > m_reserveMap;
	// Cache where a building is most likely going to be created. 
	// Useful for deciding if we should send a scv to go build. (accounting for current income)
	mutable std::map<sc2::UnitTypeID, sc2::Point2DI> build_location_cache_;

    // queries for various BuildingPlacer data
    bool isReserved(int x, int y) const;
    bool isInResourceBox(int x, int y) const;
    bool tileOverlapsBaseLocation(int x, int y, UnitType type) const;
	bool buildable(const int x, const int y, const sc2::UnitTypeID type) const;

public:

    BuildingPlacer(CCBot & bot);

    void onStart();

	// determines whether we can build at a given location
	bool canBuildHere(const int bx, const int by, const sc2::UnitTypeID type) const;
    bool canBuildHereWithSpace(int bx, int by, const Building & b, int buildDist) const;

	std::vector<sc2::Point2DI> getTilesForBuilding(const sc2::UnitTypeID type, const sc2::Point2DI pos) const;

    // returns a build location near a building's desired location
    CCTilePosition getBuildLocationNear(const Building & b, int buildDist) const;

	sc2::Point2DI getBuildLocationForType(const sc2::UnitTypeID type, sc2::Point2DI desired_loc = sc2::Point2DI(0, 0)) const;

	sc2::Point2DI getNextCoordinateToWallWithBuilding(const sc2::UnitTypeID building_type) const;

    void drawReservedTiles();

	void reserveTiles(sc2::UnitTypeID building_type, sc2::Point2DI building_location);
    void freeTiles(int x, int y, int width, int height);
	CCTilePosition getRefineryPosition() const;
};
