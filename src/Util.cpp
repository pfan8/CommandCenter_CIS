#include "Util.h"
#include "CCBot.h"
#include <iostream>

std::string Util::GetStringFromRace(const CCRace & race)
{
#ifdef SC2API
    if      (race == sc2::Race::Terran)  { return "Terran"; }
    else if (race == sc2::Race::Protoss) { return "Protoss"; }
    else if (race == sc2::Race::Zerg)    { return "Zerg"; }
    else if (race == sc2::Race::Random)  { return "Random"; }
#else
    if      (race == BWAPI::Races::Terran)  { return "Terran"; }
    else if (race == BWAPI::Races::Protoss) { return "Protoss"; }
    else if (race == BWAPI::Races::Zerg)    { return "Zerg"; }
    else if (race == BWAPI::Races::Unknown) { return "Unknown"; }
#endif
    BOT_ASSERT(false, "Unknown Race");
    return "Error";
}

CCRace Util::GetRaceFromString(const std::string & raceIn)
{
    std::string race(raceIn);
    std::transform(race.begin(), race.end(), race.begin(), ::tolower);

    if      (race == "terran")  { return sc2::Race::Terran; }
    else if (race == "protoss") { return sc2::Race::Protoss; }
    else if (race == "zerg")    { return sc2::Race::Zerg; }
    else if (race == "random")  { return sc2::Race::Random; }
    
    BOT_ASSERT(false, "Unknown Race: ", race.c_str());
    return sc2::Race::Random;
}


sc2::Point2DI Util::ToPoint2DI(sc2::Point2D point)
{
	return sc2::Point2DI{ static_cast<int>(point.x),
		static_cast<int>(point.y)
	};
}

sc2::Point2D Util::ToPoint2D(sc2::Point2DI point)
{
	return sc2::Point2D{ static_cast<float>(point.x),
		static_cast<float>(point.y)
	};
}

CCPositionType Util::TileToPosition(float tile)
{
    return tile;
}

UnitType Util::GetSupplyProvider(const CCRace & race, CCBot & bot)
{
#ifdef SC2API
    switch (race) 
    {
        case sc2::Race::Terran: return UnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT, bot);
        case sc2::Race::Protoss: return UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, bot);
        case sc2::Race::Zerg: return UnitType(sc2::UNIT_TYPEID::ZERG_OVERLORD, bot);
        default: return UnitType();
    }
#else
    return UnitType(race.getSupplyProvider(), bot);
#endif
}

UnitType Util::GetTownHall(const CCRace & race, CCBot & bot)
{
#ifdef SC2API
    switch (race) 
    {
        case sc2::Race::Terran: return UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, bot);
        case sc2::Race::Protoss: return UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, bot);
        case sc2::Race::Zerg: return UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, bot);
        default: return UnitType();
    }
#else
    return UnitType(race.getResourceDepot(), bot);
#endif
}

UnitType Util::GetRefinery(const CCRace & race, CCBot & bot)
{
#ifdef SC2API
    switch (race) 
    {
        case sc2::Race::Terran: return UnitType(sc2::UNIT_TYPEID::TERRAN_REFINERY, bot);
        case sc2::Race::Protoss: return UnitType(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, bot);
        case sc2::Race::Zerg: return UnitType(sc2::UNIT_TYPEID::ZERG_EXTRACTOR, bot);
        default: return UnitType();
    }
#else
    return UnitType(race.getRefinery(), bot);
#endif
}

CCPosition Util::CalcCenter(const std::vector<Unit> & units)
{
    if (units.empty())
    {
        return CCPosition(0, 0);
    }

    CCPositionType cx = 0;
    CCPositionType cy = 0;

    for (auto & unit : units)
    {
        BOT_ASSERT(unit.isValid(), "Unit pointer was null");
        cx += unit.getPosition().x;
        cy += unit.getPosition().y;
    }

    return CCPosition(cx / units.size(), cy / units.size());
}

bool Util::IsZerg(const CCRace & race)
{
#ifdef SC2API
    return race == sc2::Race::Zerg;
#else
    return race == BWAPI::Races::Zerg;
#endif
}

bool Util::IsProtoss(const CCRace & race)
{
#ifdef SC2API
    return race == sc2::Race::Protoss;
#else
    return race == BWAPI::Races::Protoss;
#endif
}

bool Util::IsTerran(const CCRace & race)
{
#ifdef SC2API
    return race == sc2::Race::Terran;
#else
    return race == BWAPI::Races::Terran;
#endif
}

bool Util::isBuilding(const sc2::UnitTypeID & type)
{
	switch (type.ToType())
	{
	case sc2::UNIT_TYPEID::TERRAN_ARMORY:           return true;
	case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR:     return true;
	case sc2::UNIT_TYPEID::ZERG_BANELINGNEST:       return true;
	case sc2::UNIT_TYPEID::TERRAN_BARRACKS:         return true;
	case sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR:  return true;
	case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:  return true;
	case sc2::UNIT_TYPEID::TERRAN_BUNKER:           return true;
	case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:    return true;
	case sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE: return true;
	case sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE:      return true;
	case sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY:   return true;
	case sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER:   return true;
	case sc2::UNIT_TYPEID::ZERG_EXTRACTOR:          return true;
	case sc2::UNIT_TYPEID::TERRAN_FACTORY:          return true;
	case sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR:   return true;
	case sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB:   return true;
	case sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON:     return true;
	case sc2::UNIT_TYPEID::PROTOSS_FORGE:           return true;
	case sc2::UNIT_TYPEID::TERRAN_FUSIONCORE:       return true;
	case sc2::UNIT_TYPEID::PROTOSS_GATEWAY:         return true;
	case sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY:     return true;
	case sc2::UNIT_TYPEID::ZERG_HATCHERY:           return true;
	case sc2::UNIT_TYPEID::ZERG_HYDRALISKDEN:       return true;
	case sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT:     return true;
	case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET:    return true;
	case sc2::UNIT_TYPEID::PROTOSS_NEXUS:           return true;
	case sc2::UNIT_TYPEID::ZERG_NYDUSCANAL:         return true;
	case sc2::UNIT_TYPEID::ZERG_NYDUSNETWORK:       return true;
	case sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON:    return true;
	case sc2::UNIT_TYPEID::PROTOSS_PYLON:           return true;
	case sc2::UNIT_TYPEID::TERRAN_REFINERY:         return true;
	case sc2::UNIT_TYPEID::ZERG_ROACHWARREN:        return true;
	case sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY:     return true;
	case sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY: return true;
	case sc2::UNIT_TYPEID::TERRAN_SENSORTOWER:      return true;
	case sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL:       return true;
	case sc2::UNIT_TYPEID::ZERG_SPINECRAWLER:       return true;
	case sc2::UNIT_TYPEID::ZERG_SPIRE:              return true;
	case sc2::UNIT_TYPEID::ZERG_SPORECRAWLER:       return true;
	case sc2::UNIT_TYPEID::PROTOSS_STARGATE:        return true;
	case sc2::UNIT_TYPEID::TERRAN_STARPORT:         return true;
	case sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR:  return true;
	case sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB:  return true;
	case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT:      return true;
	case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED:      return true;
	case sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:  return true;
	case sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL: return true;
	case sc2::UNIT_TYPEID::ZERG_ULTRALISKCAVERN:    return true;
	case sc2::UNIT_TYPEID::ZERG_HIVE:               return true;
	case sc2::UNIT_TYPEID::ZERG_LAIR:               return true;
	case sc2::UNIT_TYPEID::ZERG_GREATERSPIRE:       return true;
	case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:   return true;
	case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: return true;

	default: return false;
	}
}

CCTilePosition Util::GetTilePosition(int x, int y)
{
	return CCTilePosition(x, y);
}

CCTilePosition Util::GetTilePosition(const CCPosition & pos)
{
#ifdef SC2API
    return CCTilePosition((int)std::floor(pos.x), (int)std::floor(pos.y));
#else
    return CCTilePosition(pos);
#endif
}

CCPosition Util::GetPosition(const CCTilePosition & tile)
{
#ifdef SC2API
    return CCPosition((float)tile.x, (float)tile.y);
#else
    return CCPosition(tile);
#endif
}

float Util::Dist(const CCPosition & p1, const CCPosition & p2)
{
    return sqrtf((float)Util::DistSq(p1,p2));
}

float Util::Dist(const Unit & unit, const CCPosition & p2)
{
    return Dist(unit.getPosition(), p2);
}

float Util::Dist(const Unit & unit1, const Unit & unit2)
{
    return Dist(unit1.getPosition(), unit2.getPosition());
}

CCPositionType Util::DistSq(const CCPosition & p1, const CCPosition & p2)
{
    CCPositionType dx = p1.x - p2.x;
    CCPositionType dy = p1.y - p2.y;

    return dx*dx + dy*dy;
}

#ifdef SC2API
sc2::BuffID Util::GetBuffFromName(const std::string & name, CCBot & bot)
{
    for (const sc2::BuffData & data : bot.Observation()->GetBuffData())
    {
        if (name == data.name)
        {
            return data.buff_id;
        }
    }

    return 0;
}

sc2::AbilityID Util::GetAbilityFromName(const std::string & name, CCBot & bot)
{
    for (const sc2::AbilityData & data : bot.Observation()->GetAbilityData())
    {
        if (name == data.link_name)
        {
            return data.ability_id;
        }
    }

    return 0;
}

int Util::GetUnitTypeWidth(const sc2::UnitTypeID type, const sc2::Agent & bot)
{
	float largest_radius = 0;
	if (largest_radius < bot.Observation()->GetAbilityData()[UnitTypeIDToAbilityID(type)].footprint_radius)
		largest_radius = bot.Observation()->GetAbilityData()[UnitTypeIDToAbilityID(type)].footprint_radius;

	// OrbitalCommands have a footprint radius of 0.
	// Get the CommandCenter, and use the larger footprint radius.
	const auto tech_alias = bot.Observation()->GetUnitTypeData()[type].tech_alias;
	// If the unit has no tech alias, simply use the radius that we have already found. 
	if (tech_alias.size() == 0)
		return static_cast<int>(2 * largest_radius);

	if (largest_radius < bot.Observation()->GetAbilityData()[UnitTypeIDToAbilityID(tech_alias[0])].footprint_radius)
		largest_radius = bot.Observation()->GetAbilityData()[UnitTypeIDToAbilityID(tech_alias[0])].footprint_radius;
	return static_cast<int>(2 * largest_radius);
}

int Util::GetUnitTypeHeight(const sc2::UnitTypeID type, const sc2::Agent & bot)
{
	// Units in sc2 are square. Both the width and the height are the same. 
	return GetUnitTypeWidth(type, bot);
}
#endif

sc2::AbilityID Util::UnitTypeIDToAbilityID(const sc2::UnitTypeID & id)
{
	switch (id.ToType())
	{
	case sc2::UNIT_TYPEID::TERRAN_ARMORY: return sc2::ABILITY_ID::BUILD_ARMORY;
	case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR: return sc2::ABILITY_ID::BUILD_ASSIMILATOR;
	case sc2::UNIT_TYPEID::ZERG_BANELINGNEST: return sc2::ABILITY_ID::BUILD_BANELINGNEST;
	case sc2::UNIT_TYPEID::TERRAN_BARRACKS: return sc2::ABILITY_ID::BUILD_BARRACKS;
	case sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR: return sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS;
	case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB: return sc2::ABILITY_ID::BUILD_TECHLAB;
	case sc2::UNIT_TYPEID::TERRAN_TECHLAB: return sc2::ABILITY_ID::BUILD_TECHLAB;
	case sc2::UNIT_TYPEID::TERRAN_BUNKER: return sc2::ABILITY_ID::BUILD_BUNKER;
	case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER: return sc2::ABILITY_ID::BUILD_COMMANDCENTER;
	case sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE: return sc2::ABILITY_ID::BUILD_CYBERNETICSCORE;
	case sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE: return sc2::ABILITY_ID::BUILD_DARKSHRINE;
	case sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY: return sc2::ABILITY_ID::BUILD_ENGINEERINGBAY;
	case sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER: return sc2::ABILITY_ID::BUILD_EVOLUTIONCHAMBER;
	case sc2::UNIT_TYPEID::ZERG_EXTRACTOR: return sc2::ABILITY_ID::BUILD_EXTRACTOR;
	case sc2::UNIT_TYPEID::TERRAN_FACTORY: return sc2::ABILITY_ID::BUILD_FACTORY;
	case sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR: return sc2::ABILITY_ID::BUILD_REACTOR_FACTORY;
	case sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB: return sc2::ABILITY_ID::BUILD_TECHLAB;
	case sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON: return sc2::ABILITY_ID::BUILD_FLEETBEACON;
	case sc2::UNIT_TYPEID::PROTOSS_FORGE: return sc2::ABILITY_ID::BUILD_FORGE;
	case sc2::UNIT_TYPEID::TERRAN_FUSIONCORE: return sc2::ABILITY_ID::BUILD_FUSIONCORE;
	case sc2::UNIT_TYPEID::PROTOSS_GATEWAY: return sc2::ABILITY_ID::BUILD_GATEWAY;
	case sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY: return sc2::ABILITY_ID::BUILD_GHOSTACADEMY;
	case sc2::UNIT_TYPEID::ZERG_HATCHERY: return sc2::ABILITY_ID::BUILD_HATCHERY;
	case sc2::UNIT_TYPEID::ZERG_HYDRALISKDEN: return sc2::ABILITY_ID::BUILD_HYDRALISKDEN;
	case sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT: return sc2::ABILITY_ID::BUILD_INFESTATIONPIT;
	case sc2::UNIT_TYPEID::PROTOSS_INTERCEPTOR: return sc2::ABILITY_ID::BUILD_INTERCEPTORS;
	case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET: return sc2::ABILITY_ID::BUILD_MISSILETURRET;
	case sc2::UNIT_TYPEID::PROTOSS_NEXUS: return sc2::ABILITY_ID::BUILD_NEXUS;
	case sc2::UNIT_TYPEID::TERRAN_NUKE: return sc2::ABILITY_ID::BUILD_NUKE;
	case sc2::UNIT_TYPEID::ZERG_NYDUSCANAL: return sc2::ABILITY_ID::BUILD_NYDUSWORM;
	case sc2::UNIT_TYPEID::ZERG_NYDUSNETWORK: return sc2::ABILITY_ID::BUILD_NYDUSNETWORK;
	case sc2::UNIT_TYPEID::PROTOSS_ORACLESTASISTRAP: return sc2::ABILITY_ID::BUILD_STASISTRAP;
	case sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON: return sc2::ABILITY_ID::BUILD_PHOTONCANNON;
	case sc2::UNIT_TYPEID::PROTOSS_PYLON: return sc2::ABILITY_ID::BUILD_PYLON;
		//return sc2::ABILITY_ID::BUILD_QUEEN_CREEPTUMOR; case sc2::UNIT_TYPEID::ZERG_CREEPTUMORQUEEN: 
	case sc2::UNIT_TYPEID::TERRAN_REFINERY: return sc2::ABILITY_ID::BUILD_REFINERY;
	case sc2::UNIT_TYPEID::ZERG_ROACHWARREN: return sc2::ABILITY_ID::BUILD_ROACHWARREN;
	case sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY: return sc2::ABILITY_ID::BUILD_ROBOTICSBAY;
	case sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY: return sc2::ABILITY_ID::BUILD_ROBOTICSFACILITY;
	case sc2::UNIT_TYPEID::TERRAN_SENSORTOWER: return sc2::ABILITY_ID::BUILD_SENSORTOWER;
	case sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL: return sc2::ABILITY_ID::BUILD_SPAWNINGPOOL;
	case sc2::UNIT_TYPEID::ZERG_SPINECRAWLER: return sc2::ABILITY_ID::BUILD_SPINECRAWLER;
	case sc2::UNIT_TYPEID::ZERG_SPIRE: return sc2::ABILITY_ID::BUILD_SPIRE;
	case sc2::UNIT_TYPEID::ZERG_SPORECRAWLER: return sc2::ABILITY_ID::BUILD_SPORECRAWLER;
	case sc2::UNIT_TYPEID::PROTOSS_STARGATE: return sc2::ABILITY_ID::BUILD_STARGATE;
	case sc2::UNIT_TYPEID::TERRAN_STARPORT: return sc2::ABILITY_ID::BUILD_STARPORT;
	case sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR: return sc2::ABILITY_ID::BUILD_REACTOR_STARPORT;
	case sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB: return sc2::ABILITY_ID::BUILD_TECHLAB;
	case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT: return sc2::ABILITY_ID::BUILD_SUPPLYDEPOT;
	case sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE: return sc2::ABILITY_ID::BUILD_TEMPLARARCHIVE;
	case sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL: return sc2::ABILITY_ID::BUILD_TWILIGHTCOUNCIL;
	case sc2::UNIT_TYPEID::ZERG_ULTRALISKCAVERN: return sc2::ABILITY_ID::BUILD_ULTRALISKCAVERN;
	case sc2::UNIT_TYPEID::ZERG_HIVE: return sc2::ABILITY_ID::MORPH_HIVE;
	case sc2::UNIT_TYPEID::ZERG_LAIR: return sc2::ABILITY_ID::MORPH_LAIR;
	case sc2::UNIT_TYPEID::ZERG_GREATERSPIRE: return sc2::ABILITY_ID::MORPH_GREATERSPIRE;
	case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND: return sc2::ABILITY_ID::MORPH_ORBITALCOMMAND;
	case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: return sc2::ABILITY_ID::MORPH_PLANETARYFORTRESS;
	case sc2::UNIT_TYPEID::ZERG_OVERLORD: return sc2::ABILITY_ID::TRAIN_OVERLORD;
		/*case sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR: return sc2::ABILITY_ID::TRAINWARP_DARKTEMPLAR;
		case sc2::UNIT_TYPEID::PROTOSS_HIGHTEMPLAR: return sc2::ABILITY_ID::TRAINWARP_HIGHTEMPLAR;
		case sc2::UNIT_TYPEID::PROTOSS_SENTRY: return sc2::ABILITY_ID::TRAINWARP_SENTRY;
		case sc2::UNIT_TYPEID::PROTOSS_STALKER: return sc2::ABILITY_ID::TRAINWARP_STALKER;
		case sc2::UNIT_TYPEID::PROTOSS_ADEPT: return sc2::ABILITY_ID::TRAINWARP_WARPINADEPT;
		case sc2::UNIT_TYPEID::PROTOSS_ZEALOT: return sc2::ABILITY_ID::TRAINWARP_ZEALOT; */
	case sc2::UNIT_TYPEID::ZERG_BANELING: return sc2::ABILITY_ID::TRAIN_BANELING;
	case sc2::UNIT_TYPEID::TERRAN_BANSHEE: return sc2::ABILITY_ID::TRAIN_BANSHEE;
	case sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER: return sc2::ABILITY_ID::TRAIN_BATTLECRUISER;
	case sc2::UNIT_TYPEID::TERRAN_CYCLONE: return sc2::ABILITY_ID::TRAIN_CYCLONE;
	case sc2::UNIT_TYPEID::PROTOSS_CARRIER: return sc2::ABILITY_ID::TRAIN_CARRIER;
	case sc2::UNIT_TYPEID::PROTOSS_COLOSSUS: return sc2::ABILITY_ID::TRAIN_COLOSSUS;
	case sc2::UNIT_TYPEID::ZERG_CORRUPTOR: return sc2::ABILITY_ID::TRAIN_CORRUPTOR;
	case sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR: return sc2::ABILITY_ID::TRAIN_DARKTEMPLAR;
	case sc2::UNIT_TYPEID::ZERG_DRONE: return sc2::ABILITY_ID::TRAIN_DRONE;
	case sc2::UNIT_TYPEID::TERRAN_GHOST: return sc2::ABILITY_ID::TRAIN_GHOST;
	case sc2::UNIT_TYPEID::TERRAN_HELLION: return sc2::ABILITY_ID::TRAIN_HELLION;
		//case sc2::UNIT_TYPEID::TERRAN_HELLION: return sc2::ABILITY_ID::TRAIN_HELLIONTANK; 
	case sc2::UNIT_TYPEID::PROTOSS_HIGHTEMPLAR: return sc2::ABILITY_ID::TRAIN_HIGHTEMPLAR;
	case sc2::UNIT_TYPEID::ZERG_HYDRALISK: return sc2::ABILITY_ID::TRAIN_HYDRALISK;
	case sc2::UNIT_TYPEID::PROTOSS_IMMORTAL: return sc2::ABILITY_ID::TRAIN_IMMORTAL;
	case sc2::UNIT_TYPEID::ZERG_INFESTOR: return sc2::ABILITY_ID::TRAIN_INFESTOR;
	case sc2::UNIT_TYPEID::TERRAN_LIBERATOR: return sc2::ABILITY_ID::TRAIN_LIBERATOR;
	case sc2::UNIT_TYPEID::TERRAN_MARAUDER: return sc2::ABILITY_ID::TRAIN_MARAUDER;
	case sc2::UNIT_TYPEID::TERRAN_MARINE: return sc2::ABILITY_ID::TRAIN_MARINE;
	case sc2::UNIT_TYPEID::TERRAN_MEDIVAC: return sc2::ABILITY_ID::TRAIN_MEDIVAC;
	case sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE: return sc2::ABILITY_ID::TRAIN_MOTHERSHIPCORE;
	case sc2::UNIT_TYPEID::ZERG_MUTALISK: return sc2::ABILITY_ID::TRAIN_MUTALISK;
	case sc2::UNIT_TYPEID::PROTOSS_OBSERVER: return sc2::ABILITY_ID::TRAIN_OBSERVER;
	case sc2::UNIT_TYPEID::PROTOSS_ORACLE: return sc2::ABILITY_ID::TRAIN_ORACLE;
	case sc2::UNIT_TYPEID::PROTOSS_PHOENIX: return sc2::ABILITY_ID::TRAIN_PHOENIX;
	case sc2::UNIT_TYPEID::PROTOSS_PROBE: return sc2::ABILITY_ID::TRAIN_PROBE;
	case sc2::UNIT_TYPEID::ZERG_QUEEN: return sc2::ABILITY_ID::TRAIN_QUEEN;
	case sc2::UNIT_TYPEID::TERRAN_RAVEN: return sc2::ABILITY_ID::TRAIN_RAVEN;
	case sc2::UNIT_TYPEID::TERRAN_REAPER: return sc2::ABILITY_ID::TRAIN_REAPER;
	case sc2::UNIT_TYPEID::ZERG_ROACH: return sc2::ABILITY_ID::TRAIN_ROACH;
	case sc2::UNIT_TYPEID::TERRAN_SCV: return sc2::ABILITY_ID::TRAIN_SCV;
	case sc2::UNIT_TYPEID::PROTOSS_SENTRY: return sc2::ABILITY_ID::TRAIN_SENTRY;
	case sc2::UNIT_TYPEID::TERRAN_SIEGETANK: return sc2::ABILITY_ID::TRAIN_SIEGETANK;
	case sc2::UNIT_TYPEID::PROTOSS_STALKER: return sc2::ABILITY_ID::TRAIN_STALKER;
	case sc2::UNIT_TYPEID::ZERG_SWARMHOSTMP: return sc2::ABILITY_ID::TRAIN_SWARMHOST;
	case sc2::UNIT_TYPEID::PROTOSS_TEMPEST: return sc2::ABILITY_ID::TRAIN_TEMPEST;
	case sc2::UNIT_TYPEID::TERRAN_THOR: return sc2::ABILITY_ID::TRAIN_THOR;
	case sc2::UNIT_TYPEID::ZERG_ULTRALISK: return sc2::ABILITY_ID::TRAIN_ULTRALISK;
	case sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER: return sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER;
	case sc2::UNIT_TYPEID::ZERG_VIPER: return sc2::ABILITY_ID::TRAIN_VIPER;
	case sc2::UNIT_TYPEID::PROTOSS_VOIDRAY: return sc2::ABILITY_ID::TRAIN_VOIDRAY;
	case sc2::UNIT_TYPEID::PROTOSS_ADEPT: return sc2::ABILITY_ID::TRAIN_ADEPT;
	case sc2::UNIT_TYPEID::PROTOSS_DISRUPTOR: return sc2::ABILITY_ID::TRAIN_DISRUPTOR;
	case sc2::UNIT_TYPEID::PROTOSS_WARPPRISM: return sc2::ABILITY_ID::TRAIN_WARPPRISM;
	case sc2::UNIT_TYPEID::TERRAN_WIDOWMINE: return sc2::ABILITY_ID::TRAIN_WIDOWMINE;
	case sc2::UNIT_TYPEID::PROTOSS_ZEALOT: return sc2::ABILITY_ID::TRAIN_ZEALOT;
	case sc2::UNIT_TYPEID::ZERG_ZERGLING: return sc2::ABILITY_ID::TRAIN_ZERGLING;

	default: return 0;
	}
}

// checks where a given unit can make a given unit type now
// this is done by iterating its legal abilities for the build command to make the unit
bool Util::UnitCanMetaTypeNow(const Unit & unit, const UnitType & type, CCBot & m_bot)
{
#ifdef SC2API
    BOT_ASSERT(unit.isValid(), "Unit pointer was null");
    sc2::AvailableAbilities available_abilities = m_bot.Query()->GetAbilitiesForUnit(unit.getUnitPtr());
    
    // quick check if the unit can't do anything it certainly can't build the thing we want
    if (available_abilities.abilities.empty()) 
    {
        return false;
    }
    else 
    {
        // check to see if one of the unit's available abilities matches the build ability type
        sc2::AbilityID MetaTypeAbility = m_bot.Data(type).buildAbility;
        for (const sc2::AvailableAbility & available_ability : available_abilities.abilities) 
        {
            if (available_ability.ability_id == MetaTypeAbility)
            {
                return true;
            }
        }
    }
#else

#endif
    return false;
}
