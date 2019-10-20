#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#include <queue>
#include <stack>
#include <time.h>
#include <math.h>
#include <cstdlib>
using namespace std;

#include "Player.h"
#include "Map.h"
#include "Reader.h"
#include "AbilityList.h"

class GameState			// Creates game object
{
	public:
		GameState();

		~GameState();

		void initialize(Player* players, int numPlayers, int APLimit, Map map);
		void turnLoop();
		
		void getMovementTargets();
		void getAbilityTargets();
		void getShortestPath();
		void clearTargetOptions();
		bool placeBob(Bob* bob, int x, int y);
		bool killBob(Bob* bob);
		bool gameOver();
		
		Player* getPlayers();
		Map* getMap();
		Reader* getReader();
		ActionStateType getActionState();
		
		Player* getActivePlayer();
		Bob* getActiveBob();
		ActiveAbility* getActiveAbility();
		
		void validateAction();
		void processAction();
		
		int getID();
		int getNumPlayers();
		int getAPLimit();
		
		void draw();
		
	private:
		
		void nextActivePlayer();
		void actionLoop();
		void findTargets();
		void setTurnsLeft();
		bool verifyActionUsable();
		bool isInLOS(TargetOptionType targetOption);

		Player 	*players,
				*activePlayer;

		Map map;

		int ID,
			numPlayers,
			APLimit;

		clock_t currentTick,
				lastTick,
				ticksSinceTurnStart,
				lastDecrement,
				startDecrementing,
				decrementRate,
				animStart;

		ActionStateType actionState;
		ActionRequestType actionRequest;	// NEW!
		TargetOptionType moveTarget;
		ActionToPerformType performAction;
		
		Reader reader;

		int bobID;
		Bob *activeBob;
		ActiveAbility  *activeAbility;
		
		int* obstacleX;
		int* obstacleY;
		int obstacleSize;
		int* occupiedX;
		int* occupiedY;
		int occupiedSize;

		queue< Tile* > tileQueue;
};


#endif 