#pragma once

#include "Common.h"
#include "UnitData.h"
#include "BaseLocation.h"
#include "Unit.h"

class CCBot;
class UnitInfoManager 
{
    CCBot &           m_bot;

	typedef std::vector<Unit> UnitV;

    std::map<CCPlayer, UnitData> m_unitData; 
    std::map<CCPlayer, UnitV> m_units;

    void                    updateUnit(const Unit & unit);
    void                    updateUnitInfo();
    bool                    isValidUnit(const Unit & unit);
    
    const UnitData &        getUnitData(CCPlayer player) const;

    void drawSelectedUnitDebugInfo();

public:

    UnitInfoManager(CCBot & bot);

    void                    onFrame();
    void                    onStart();

    const std::vector<Unit> & getUnits(CCPlayer player) const;

    size_t                  getUnitTypeCount(CCPlayer player, UnitType type, bool completed = true) const;

    void                    getNearbyForce(std::vector<UnitInfo> & unitInfo, CCPosition p, int player, float radius) const;

    const std::map<Unit, UnitInfo> & getUnitInfoMap(CCPlayer player) const;

	int getNumDepots(sc2::Unit::Alliance self) const;

	//bool                  enemyHasCloakedUnits() const;
    void                    drawUnitInformation(float x, float y) const;

};