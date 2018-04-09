#pragma once

#include "Common.h"
#include "BuildOrder.h"
#include "Condition.h"
#include "BuildingPlacer.h"

typedef std::pair<UnitType, size_t> UnitPair;
typedef std::vector<UnitPair>       UnitPairVector;

class CCBot;

enum BMBuildType { BUNKER, MARINE };

struct Strategy
{
    std::string m_name;
	
    CCRace      m_race;
    int         m_wins;
    int         m_losses;
    BuildOrder  m_buildOrder;
    Condition   m_scoutCondition;
    Condition   m_attackCondition;
	int			m_expandNum;

    Strategy();
    Strategy(const std::string & name
		, const CCRace & race
		, const BuildOrder & buildOrder
		, const Condition & scoutCondition
		, const Condition & attackCondition
		, const int expandNum);
};

class StrategyManager
{
    CCBot & m_bot;

    CCRace									m_selfRace;
    CCRace									m_enemyRace;
    std::map<std::string, Strategy>			m_strategies;
	typedef std::pair<sc2::Point2DI, std::pair<bool, int>> BM;
	std::vector<BM>							m_bunkerMarines;
    int										m_totalGamesPlayed;
    const BuildOrder						m_emptyBuildOrder;
	bool									m_bases_safe;
	bool									m_exbase_onbuild;

	void HandleUnitAssignments();

	void checkExpand() const;
	void checkExpandBunker();

	bool  shouldExpandNow() const;
    const UnitPairVector getProtossBuildOrderGoal() const;
    const UnitPairVector getTerranBuildOrderGoal() const;
    const UnitPairVector getZergBuildOrderGoal() const;

public:

    StrategyManager(CCBot & bot);

    const Strategy & getCurrentStrategy() const;
    bool scoutConditionIsMet() const;
    bool attackConditionIsMet() const;
    void onStart();
    void onFrame();
	bool areBasesSafe();
    void onEnd(const bool isWinner);
    void addStrategy(const std::string & name, const Strategy & strategy);
    const UnitPairVector getBuildOrderGoal() const;
    const BuildOrder & getOpeningBookBuildOrder() const;
    void readStrategyFile(const std::string & str);
	void activateExpandState(sc2::Point2DI exbase_position);
	void resetExpandState();
	void validateBM(sc2::Point2DI exbase_pos, BMBuildType type);
};