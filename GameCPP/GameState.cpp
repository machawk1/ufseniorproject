#include "GameState.h"

// Constructors

GameState::GameState()
{
	ID = GLOBAL_ID++;
	players = NULL;
	activePlayer = NULL;
	numPlayers = 0;
	activeBob = NULL;
}

// Deconstructor

GameState::~GameState()
{
	#ifdef DEBUG
		cout << "GameState destructed!" << endl;
	#endif
	
	if(players != NULL)
	{
		delete [] players;
	}
	players = NULL;
	activePlayer = NULL;
	activeBob = NULL;
	
	#ifdef debug
	cout << "Game destructor\n";
	#endif
}


void GameState::initialize(Player* players, int numPlayers, int APLimit, Map map)
{
	this->map = map;

	this->players = new Player[numPlayers];
	this->numPlayers = numPlayers;
	this->APLimit = APLimit;

	for(int i=0; i<numPlayers; i++)
	{
		this->players[i] = players[i];
	}

	activePlayer = &(this->players[0]);     // will probably change this to random selection

	lastTick = clock();

	lastDecrement = 0;		
	startDecrementing = 20 * CLK_TCK;	// the point, in CLK_TCKs, when APs will start to degrade
										// this should probably come from a config file
	decrementRate = 1 * CLK_TCK;		// Number of ticks that should elapse before decrementing AP
										// Constant should be replaced with value from config file

	actionState = ActionRequest;
	activeBob = NULL;
}


void GameState::nextActivePlayer()		// switches the active player
{
	activePlayer->setAP(this->APLimit);	// Reset Action Points

	// Switch active player
	if(activePlayer == &players[numPlayers-1])
	{
		activePlayer = &players[0];
	}
	else
	{
		activePlayer++;
	}

	// Reset Clock
	lastTick = clock();
	lastDecrement = 0;
	actionState = ActionRequest;
#ifdef debug
	cout << "Switching players...\n";
#endif
}


void GameState::turnLoop()
{
	// getChatMessage() will go here
	// resendChatMessageToClients() will go here

	getMessages();		

	currentTick = clock();		// record the current time in ticks

	ticksSinceTurnStart = currentTick - lastTick;

	// if it is past the time to start taking away APs, and a full second has passed
	if((ticksSinceTurnStart > startDecrementing) && ((ticksSinceTurnStart - lastDecrement) > decrementRate))
	{
		lastDecrement = ticksSinceTurnStart;
		activePlayer->setAP(activePlayer->getAP()-1);	// take another AP away

#ifdef debug
		cout << "Player: " << activePlayer->getHandle() << " AP: " << activePlayer->getAP() <<
			" at time " << ticksSinceTurnStart / CLK_TCK << endl;
#endif
	}

	if(activePlayer->getAP() <= 0)	// if player is out of APs
	{
		nextActivePlayer();		// switch to next player
	}
	else
	{
		actionLoop();			// current player can choose an action
	}

}

void GameState::actionLoop()
{
	int i;

	switch(actionState)
	{
		case ActionRequest:
			// If an action request is available
			try
			{
				actionRequest = getActionRequest();	
				for(i=0; i<activePlayer->getNumBobs(); i++)
				{
					if(activePlayer->getBobs()[i].getID() == actionRequest.bobID)
					{
						activeBob = &(activePlayer->getBobs()[i]);
					}
				}
				actionState = ActionOptions;
			}
			catch(ExceptionType e)
			{
			}
			break;
			
		case ActionOptions: 
			findTargets();
			if(moveOptionQueue.empty() && targetOptionQueue.empty())
			{
				actionState = ActionRequest;
			}
			else
			{
				actionState = PerformAction;
			}
			break;
			
		case PerformAction:
			try
			{
				performAction = getActionToPerform();
				if(performAction.ability == Cancel)
				{
					actionState = ActionRequest;
				}
				else if(activeBob->getID() != performAction.bobID || actionRequest.ability != performAction.ability)
				{	// The bob has changed or the ability has changed
					actionState = ActionRequest;
				}
				else
				{
					// validateAction()
					if(map.getTile(performAction.x, performAction.y)->getTraversalCost() > 0 &&
						map.getTile(performAction.x, performAction.y)->getTraversalCost() < activePlayer->getAP())
						{
							actionState = ActionResults;
						}
				}
			}
			catch(ExceptionType e)
			{
			}
			break;
		case ActionResults:
			if(performAction.ability == Move)
			{
				getShortestPath();
			}
			break;
	}
}

void GameState::findTargets()
{
	queue < Tile* > tileList;
	int i, size;
	if(actionRequest.ability == Move) // action is move
	{
		tileList = getPaths(activeBob);
		size = tileList.size();
		for(i = 0; i < size; i++)
		{	// Convert float traversal cost from float to ceiling integer
			moveTarget.cost = tileList.front()->getTraversalCost() + 1;
			moveTarget.x = tileList.front()->getX();
			moveTarget.y = tileList.front()->getY();
			tileList.pop();

			moveOptionQueue.push(moveTarget);
		}
		
		// sendMoveOption();
	}
	else // action is activeAbility
	{
		// find active targets
		
		// sendTargetOption();
	}
}

Map* GameState::getMap()
{
	return &map;	
}

Player* GameState::getPlayers()
{
	return players;
}

int GameState::getID()
{
	return ID;	
}

bool GameState::placeBob(Bob* bob, int x, int y)
{
	Tile* T;
	
	try
	{
		T = map.getTile(x, y);
		
		T->setOccupied(true);
		T->setBob(bob);
		bob->setPosition(x, y);
		
		return true;
	}
	catch(ExceptionType)
	{
	}
	
	return false;
}

queue< Tile* > GameState::getPaths(Bob* bob)
{
	int x = bob->getX();
	int y = bob->getY();
	int move = bob->getMove();
	int AP = bob->getAP();
	if(activePlayer->getAP() < AP)
		AP = activePlayer->getAP();
	
	queue< Tile* > Q;
	Q.push(map.getTile(x,y));
	
	Tile *T, *tmp;
	float traversalCost;
	int i = 0, j, k;
	while(Q.size() > 0)
	{
		cout << i++ << " " << Q.size() << endl;
		T = Q.front();
		Q.pop();
		x = T->getX();
		y = T->getY();
		
		for(j = -1; j < 2; j++)
		{
			for(k = -1; k < 2; k++)
			{
				if(j == k)
					continue;
				
				try
		        {
		        	tmp = map.getTile(x + j,y + k);
		        	
					if(tmp->getOccupied())
	        			continue;
	        	
		        	traversalCost = T->getTraversalCost() + tmp->getMoveRate()/move;
		        	
		        	if(traversalCost > AP)
		        		continue;
		        	
		        	if(tmp->getTraversalCost() == 0)
		        	{
		        		tmp->setTraversalCost(traversalCost);
		        		Q.push(tmp);
		        	}
		        	else if(traversalCost < tmp->getTraversalCost())
	        		{
	        			tmp->setTraversalCost(traversalCost);
	        			Q.push(tmp);
	        		}
		        }
		        catch(ExceptionType)
		        {
		        	cout << "EXCEPTION!" << endl;
		        }
			}
		}
	}
	
	for(int x = 0; x < map.getLength(); x++)
	{
		for(int y = 0; y < map.getHeight(); y++)
		{
			if(map.getTile(x,y)->getTraversalCost() > 0)
				Q.push(map.getTile(x,y));
		}	
	}
	
	return Q;
}

GameState::getShortestPath()
{
	int i, j, x, y;
	Tile *T, *tmp;
	stack< Tile* > S;
	
	// Add target tile to stack
	x = performAction.x;
	y = performAction.y;
	S.push(map.getTile(x,y));
	
	bool done = false;
	while(!done)
	{
		T = S.top(); // Select top of stack
		try
		{
			for(i = -1; i < 2; i++)
			{
				for(j = -1; j < 2; j++)
				{
					if(i == j)
						continue;
					
					try
			        {
			        	tmp = map.getTile(x + j,y + k);
			        	
						if(tmp->getTraversalCost() < T->getTraversalCost() && tmp->getTraversalCost() != 0)
			        	{
			        		S.push(tmp);
			        	}
			        }
			        catch(ExceptionType)
			        {
			        	cout << "EXCEPTION!" << endl;
			        }
				}	
			}
		}
		catch(ExceptionType)
		{
		}
	}
}


