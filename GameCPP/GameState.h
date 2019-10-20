#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#include <queue>
#include <stack>
#include <time.h>
#include "Player.h"
#include "Map.h"
#include "Reader.h"

enum ActionStateType
{
	ActionRequest,
	ActionOptions,
	PerformAction,
	ActionResults
};


class GameState			// Creates game object
{
	public:
		GameState();

		~GameState();

		void initialize(Player* players, int numPlayers, int APLimit, Map map);
		void turnLoop();
		
//		vector< Tile* > getPaths(Bob* bob);
		queue< Tile* > getPaths(Bob* bob);
		bool placeBob(Bob* bob, int x, int y);
		
		Player* getPlayers();
		Map* getMap();
		
		int getID();
		
	private:
		
		void nextActivePlayer();
		void actionLoop();
		void findTargets();

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
				decrementRate;

		ActionStateType actionState;
		ActionRequestType actionRequest;	// NEW!
		MoveOptionType moveTarget;
		ActionToPerformType performAction;
		

		int bobID;
		Bob* activeBob;

		queue< Tile* > tileQueue;
};


#endif 