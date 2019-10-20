#ifndef AI_H_
#define AI_H_

#include "GameState.h"
#include <queue>
#include <stack>

using namespace std;

enum AIMode
{
	Offensive,
	Defensive
};

struct Cluster
{
	int myBobs;
	int enemyBobs;		
};

class AI
{
public:
	AI(GameState *game);
	virtual ~AI();
	
	void determineAction();

private:
	GameState *game;
	Player *player;
	AIMode mode;
	
	Cluster** cluster;
	int clusterLength;
	int clusterHeight;
	bool contested;
	
	queue< int > contestedX;
	queue< int > contestedY;
	
	queue< int > ownedX;
	queue< int > ownedY;

	void determineMode();
	void determineContested();
	void determineClusterStates();
	void moveTowardsTarget(Tile *tile);
	bool getBestTargetInRange(AbilityNameType abilityName);
	bool getBestTargetInMoveRange(AbilityNameType abilityName);
	Tile* getNearestEnemy(Tile *tile);
	Tile* getTargetEnemyCluster();
	Tile* getCenterTile();
};

#endif /*AI3_H_*/
