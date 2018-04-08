#pragma once

#include "Common.h"
#include "UnitType.h"

class CCBot;
class Unit;

namespace Util
{
    CCRace          GetRaceFromString(const std::string & str);
	sc2::Point2DI	ToPoint2DI(sc2::Point2D point);
	sc2::Point2D	ToPoint2D(sc2::Point2DI point);
    CCTilePosition  GetTilePosition(const CCPosition & pos);
    CCPosition      GetPosition(const CCTilePosition & tile);
    std::string     GetStringFromRace(const CCRace & race);
    bool            UnitCanMetaTypeNow(const Unit & unit, const UnitType & type, CCBot & m_bot);
    UnitType        GetTownHall(const CCRace & race, CCBot & bot);
    UnitType        GetRefinery(const CCRace & race, CCBot & bot);
    UnitType        GetSupplyProvider(const CCRace & race, CCBot & bot);
    CCPosition      CalcCenter(const std::vector<Unit> & units);
    bool            IsZerg(const CCRace & race);
    bool            IsProtoss(const CCRace & race);
    bool            IsTerran(const CCRace & race);
	bool isBuilding(const sc2::UnitTypeID & type);
    CCPositionType  TileToPosition(float tile);

#ifdef SC2API
    sc2::BuffID     GetBuffFromName(const std::string & name, CCBot & bot);
    sc2::AbilityID  GetAbilityFromName(const std::string & name, CCBot & bot);
	int GetUnitTypeWidth(const sc2::UnitTypeID type, const sc2::Agent & bot);
	int GetUnitTypeHeight(const sc2::UnitTypeID type, const sc2::Agent & bot);
	sc2::AbilityID UnitTypeIDToAbilityID(const sc2::UnitTypeID & id);
#endif

    float Dist(const Unit & unit, const CCPosition & p2);
    float Dist(const Unit & unit1, const Unit & unit2);
    float Dist(const CCPosition & p1, const CCPosition & p2);
    CCPositionType DistSq(const CCPosition & p1, const CCPosition & p2);
	bool IsTownHallType(const sc2::UnitTypeID & type);
	bool IsTownHall(const sc2::Unit * unit);
	bool IsRefinery(const sc2::Unit * unit);
	bool IsRefineryType(const sc2::UnitTypeID & type);
};
