#include "GameState.h"

// Constructors

GameState::GameState()
{
	reader = Reader();
	ID = GLOBAL_ID++;
	players = NULL;
	activePlayer = NULL;
	numPlayers = 0;
	activeBob = NULL;
	activeAbility = NULL;
	obstacleX = NULL;
	obstacleY = NULL;
	obstacleSize = 0;
	occupiedX = NULL;
	occupiedY = NULL;
	occupiedSize = 0;
}

// Deconstructor

GameState::~GameState()
{
	if(players)
		delete [] players;
	if(obstacleX)
		delete [] obstacleX;
	if(obstacleY)
		delete [] obstacleY;
	
	players = NULL;
	activePlayer = NULL;
	activeBob = NULL;
	activeAbility = NULL;
	obstacleX = NULL;
	obstacleY = NULL;
}

/************************************************************************************************
 * Initializes the random number generator, map, player array, turn clock, action state, active
 * player, active bob, active ability
 ************************************************************************************************/
void GameState::initialize(Player* players, int numPlayers, int APLimit, Map map)
{
	/************************************************************************************************
 	* Seed random number generator
 	*************************************************************************************************/
	srand((unsigned)time(0));
	
	/************************************************************************************************
 	* Initialize game map
 	*************************************************************************************************/
	this->map = map;
	
	/************************************************************************************************
 	* Find stationary map obstacles
 	*************************************************************************************************/
 	queue< Tile* > obstacles;
 	
 	for(int x=0; x < this->map.getLength(); x++)
 	{
 		for(int y=0; y < this->map.getHeight(); y++)
 		{
			if(this->map.getTile(x,y)->getLineOfSight())
 			{
 				obstacleSize++;
 				obstacles.push(this->map.getTile(x,y));
 			}
 		}
 	}
 	obstacleX = new int[obstacleSize];
 	obstacleY = new int[obstacleSize];
 	for(int k=0; k < obstacleSize; k++)
 	{
 		obstacleX[k] = obstacles.front()->getX();
 		obstacleY[k] = obstacles.front()->getY();
 		obstacles.pop();
 	}
	
	/************************************************************************************************
 	* Allocate memory for players
 	*************************************************************************************************/
	this->players = new Player[numPlayers];
	this->numPlayers = numPlayers;
	this->APLimit = APLimit;

	for(int i=0; i<numPlayers; i++)
		this->players[i] = players[i];
	
	/************************************************************************************************
 	* Initialize game clock
 	*************************************************************************************************/
	lastTick = clock();
	lastDecrement = 0;		
	startDecrementing = 20 * CLK_TCK;	// the point, in CLK_TCKs, when APs will start to degrade
										// this should probably come from a config file
	decrementRate = 1 * CLK_TCK;		// Number of ticks that should elapse before decrementing AP
										// Constant should be replaced with value from config file
										
	/************************************************************************************************
 	* Initialize turn state
 	*************************************************************************************************/									
	actionState = ActionRequest;
	activePlayer = &(this->players[0]);     // will probably change this to random selection
	activeBob = NULL;
	activeAbility = NULL;
}

/************************************************************************************************
* Advances the active player and resets their AP,Bob stats, turn clock, and decrements abilities'
* turns left
*************************************************************************************************/
void GameState::nextActivePlayer()
{
	/************************************************************************************************
 	* Switch active player
 	*************************************************************************************************/
	int i;
	bool playerHasBobs = false;
	while(!playerHasBobs)
	{	
		if(activePlayer == &players[numPlayers-1])
			activePlayer = &players[0];
		else
			activePlayer++;
		
		for(i = 0; i < activePlayer->getNumBobs() && !playerHasBobs; i++)
		{
			if(activePlayer->getBobs()[i].getHP() > 0)
				playerHasBobs = true;
		}
	}
	
	
	/************************************************************************************************
 	* Reset player AP, Bob stats, and target options
 	*************************************************************************************************/
//	activePlayer->setAP(this->APLimit-activePlayer->getAP());	// Reset Action Points
	
	
	for(i = 0; i < activePlayer->getNumBobs(); i++)
	{
		if(activePlayer->getBobs()[i].getHP() == 0)
			continue;
			
		activePlayer->getBobs()[i].resetAP();
		activePlayer->getBobs()[i].resetToHit();
		activePlayer->getBobs()[i].resetMitigation();
		activePlayer->getBobs()[i].resetMove();
		activePlayer->getBobs()[i].resetDodge();
	}
	
	clearQueues();

	/************************************************************************************************
 	* Reset clock
 	*************************************************************************************************/
	lastTick = clock();
	lastDecrement = 0;
	actionState = ActionRequest;
	
	/************************************************************************************************
 	* Decrement abilities' turns left
 	*************************************************************************************************/
	int j;
	for(i = 0; i < activePlayer->getNumBobs(); i++)
	{
		for(j = 0; j < activePlayer->getBobs()[i].getActiveHPSize(); j++)
		{
			if(activePlayer->getBobs()[i].getActiveHPMods()[j].turnsLeft != 0)
				activePlayer->getBobs()[i].getActiveHPMods()[j].turnsLeft--;
		}
	}
	for(i = 0; i < activePlayer->getNumBobs(); i++)
	{
		for(j = 0; j < activePlayer->getBobs()[i].getActiveStatSize(); j++)
		{
			if(activePlayer->getBobs()[i].getActiveStatMods()[j].turnsLeft != 0)
				activePlayer->getBobs()[i].getActiveStatMods()[j].turnsLeft--;
		}
	}
	
	#ifdef debug
		//cout << "Switching players...\n";
	#endif
}

/************************************************************************************************
* Main turn control loop
*************************************************************************************************/
void GameState::turnLoop()
{
	int i;
	/************************************************************************************************
 	* Record clock time
 	*************************************************************************************************/
	/*currentTick = clock();
	ticksSinceTurnStart = currentTick - lastTick;*/

	
	/************************************************************************************************
 	* Reduce active player's AP periodically once turn time is up
 	*************************************************************************************************/
	/*if((ticksSinceTurnStart > startDecrementing) && ((ticksSinceTurnStart - lastDecrement) > decrementRate))
	{
		lastDecrement = ticksSinceTurnStart;
		activePlayer->setAP(activePlayer->getAP()-1);	// take another AP away

		#ifdef debug
			//cout << "Player: " << activePlayer->getHandle() << " AP: " << activePlayer->getAP() <<
				" at time " << ticksSinceTurnStart / CLK_TCK << endl;
		#endif
	}*/

	/************************************************************************************************
 	* If active player is out of AP advance to the next player, else active player acts
 	*************************************************************************************************/
	bool bobsActive = false;
	
	for(i = 0; i < activePlayer->getNumBobs() && bobsActive == false; i++)
	{
		if(activePlayer->getBobs()[i].getAP() > 0)
			bobsActive = true;
	}
	
//	if(activePlayer->getAP() <= 0 || !bobsActive)
	if(!bobsActive)
		nextActivePlayer();	
	else
		actionLoop();
}

/************************************************************************************************
* Main action control loop
*************************************************************************************************/
void GameState::actionLoop()
{
	/************************************************************************************************
 	* Action State Machine:
 	* 	State 0: ActionRequest takes in BobID and AbilityNameType, sets active bob and ability
 	* 	State 1: ActionOptions computes available targets for the requested action
 	* 	State 2: PerformAction takes in BobID, AbilityNameType, and x,y coordinates of target and
 	* 			 validates the action
 	* 	State 3: ActionResults processes the results of the performed action
 	*************************************************************************************************/
	switch(actionState)
	{
		int i;
		
		/************************************************************************************************
		* State 0: ActionRequest
		*************************************************************************************************/
		case ActionRequest:
			////cout << "Processing Action Request\n";
			
			/************************************************************************************************
		 	* Clear previous active bob, ability, and target options
		 	*************************************************************************************************/		 	
			activeBob = NULL;
			activeAbility = NULL;
					
			/************************************************************************************************
		 	* Checks request queue for an action request until one is found
		 	*************************************************************************************************/
			try
			{
				actionRequest = reader.getActionRequest();
			}
			catch(ExceptionType)
			{
				//cout << "No action request received\n";
				return;	
			}
			
			/************************************************************************************************
		 	* Sets active bob
		 	*************************************************************************************************/
			for(i=0; i<activePlayer->getNumBobs(); i++)
			{
				if(activePlayer->getBobs()[i].getID() == actionRequest.bobID)
					activeBob = &(activePlayer->getBobs()[i]);
			}

			if(actionRequest.ability != PassTurn && (activeBob == NULL || activeBob->getHP() == 0))
				return;
			
			/************************************************************************************************
		 	* Sets active ability and advances the state to ActionOptions(1)
		 	*************************************************************************************************/
			switch(actionRequest.ability)
			{
				case Cancel:
					break;
					
				case Move:
					//cout << "Moving on\n";
					actionState = ActionOptions;
					break;
					
				case DefaultMelee:
					activeAbility = &DefaultMeleeAbility;
					if(verifyActionUsable())
						actionState = ActionOptions;
					break;
					
				case MediumRanged:
					activeAbility = &MediumRangedAbility;
					if(verifyActionUsable())
						actionState = ActionOptions;
					break;
					
				case LightLightningAOE:
					activeAbility = &LightLightningAOEAbility;
					if(verifyActionUsable())
						actionState = ActionOptions;
					break;
					
				case MediumHeal:
					activeAbility = &MediumHealAbility;
					if(verifyActionUsable())
						actionState = ActionOptions;
					break;

				case PassTurn:
					nextActivePlayer();
			//		cout << "Turn passed\r\n";
					break;
			}
			break;
		
		/************************************************************************************************
	 	* State 1: ActionOptions
	 	*************************************************************************************************/			
		case ActionOptions: 
			////cout << "Processing Action Options\n";
			
			/************************************************************************************************
		 	* Finds all legal targets for the requested action, if none are found return to ActionRequest(0)
		 	* else advance to PerformAction(2) 
		 	*************************************************************************************************/

			findTargets();
				
			if(reader.targetOptionQueue.empty())
			{
				//cout << "No targets found\n";
				actionState = ActionRequest;
			}
			else
				actionState = PerformAction;
			
			break;
		
		/************************************************************************************************
	 	* State 2: PerformAction
	 	*************************************************************************************************/	
		case PerformAction:
			////cout << "Processing Action To Perform\n";
			
			/************************************************************************************************
		 	* Validates the action to perform and changes the State acccordingly (See validateAction())
		 	*************************************************************************************************/	
			validateAction();
			break;
		
		/************************************************************************************************
	 	* State 3: ActionResults
	 	*************************************************************************************************/
		case ActionResults:
			////cout << "Processing Action Results\n";
			
			/************************************************************************************************
		 	* Processes the action to perform and returns to ActionRequest(0) (See processAction())
		 	*************************************************************************************************/
			processAction();
			break;
	}
}

/************************************************************************************************
* Finds all legal targets for the requested action
*************************************************************************************************/
void GameState::findTargets()
{
	/************************************************************************************************
 	* Action is Move, process movement targets
 	*************************************************************************************************/
	if(actionRequest.ability == Move)
		getMovementTargets();
	
	/************************************************************************************************
 	* Action is an Active ability, process legal targets
 	*************************************************************************************************/
	else
		getAbilityTargets();
}

/************************************************************************************************
* Finds all tiles within the Bob's movement range if he expends all available AP
*************************************************************************************************/
void GameState::getMovementTargets()
{
	/************************************************************************************************
	* Initializes Bob's position, movement modifier, and available AP
	*************************************************************************************************/
	//cout << "Getting Move Targets\n";
	int x = activeBob->getX();
	int y = activeBob->getY();
	int move = activeBob->getMove();
	//cout << "Bob move rate = " << move << endl;
	int AP;
//	if(activePlayer->getAP() < activeBob->getAP())
//		AP = activePlayer->getAP();
//	else
		AP = activeBob->getAP();
	
	/************************************************************************************************
	* Traversal Cost Calculation Algorithm
	* 	I.Push Bob's initial location onto a queue
	* 	II.While the queue contains tiles:
	* 		a.Pop the first tile off
	* 		b.Calculate the traversal costs to all adjacent tiles and push them onto the queue if new
	* 			traversal cost is cheaper than the last
	*************************************************************************************************/
	queue< Tile* > Q;
	Q.push(map.getTile(x,y));
	//cout << "Start position (" << x << "," << y << ")\n";
	//cout << "Queue size " << Q.size() << endl;
	
	Tile *T, *tmp;
	float traversalCost, multiplier;
	int i = 0, j, k;
	int count = 1;
	while(Q.size() > 0)
	{
		////cout << "Popping tile " << count << endl;
		T = Q.front();
		Q.pop();
		x = T->getX();
		y = T->getY();

//		//cout << "Current Tile (" << x << "," << y << ")\n";
//		//cout << "Queue size " << Q.size() << endl;
		
		for(j = -1; j < 2; j++)
		{
			for(k = -1; k < 2; k++)
			{
				if(j == 0 && k == 0)
					continue;
				
				if(abs(j + k) == 1)
					multiplier = 1;
				else
					multiplier = sqrt(2.0);
				
				try
		        {
		        	tmp = map.getTile(x + j,y + k);
		        	
					if(tmp->getOccupied())
	        			continue;
	        	
		        	traversalCost = T->getTraversalCost() + (float)tmp->getMoveRate()/move*multiplier;
		        	
		        	if(traversalCost > AP)
		        		continue;
		        	
		        	if(tmp->getTraversalCost() == 0)
		        	{
		        		tmp->setTraversalCost(traversalCost);
		        		Q.push(tmp);
						////cout << "Pushing tile " << ++count << endl;
		        	}
		        	else if(traversalCost < tmp->getTraversalCost())
	        		{
	        			tmp->setTraversalCost(traversalCost);
	        			Q.push(tmp);
						////cout << "Pushing tile " << ++count << endl;
	        		}
	        	}
		        catch(ExceptionType)
		        {
		        }
			}
		}
	}
	
	/************************************************************************************************
	* Add all tiles with a traversal cost to the target option queue
	*************************************************************************************************/
	TargetOptionType targetOption;
	for(int x = 0; x < map.getLength(); x++)
	{
		for(int y = 0; y < map.getHeight(); y++)
		{
			if(map.getTile(x,y)->getTraversalCost() > 0)
			{
				targetOption.cost = ceil(map.getTile(x,y)->getTraversalCost());
				targetOption.x = x;
				targetOption.y = y;
				
				reader.targetOptionQueue.push(targetOption);
			}
		}	
	}
	//cout << "Done getting targets\n";
}

/************************************************************************************************
* Calculates the shortest path to Bob's new location
*************************************************************************************************/
void GameState::getShortestPath()
{
	/************************************************************************************************
	* Shortest Path Algorithm
	* 	I.Add the target tile to a stack
	* 	II.Until you have returned to Bob's original location
	* 		a.Pop the top tile off of the stack
	* 		b.Push the adjacent tile with the lowest traversal cost onto the stack
	*************************************************************************************************/
	int i, j, x, y;
	Tile *T, *tmp, *shortest = NULL;
	stack< Tile* > S;
	
	// Add target tile to stack
	x = performAction.x;
	y = performAction.y;
	S.push(map.getTile(x,y));
	
	bool done = false;
	while(!done)
	{

		T = S.top();
		x = T->getX();
		y = T->getY();
		
		for(i = -1; i < 2 && shortest == NULL; i++)
		{
			for(j = -1; j < 2 && shortest == NULL; j++)
			{
				if(i == 0 && j == 0)
					continue;
				
				try
				{
					shortest = 	map.getTile(x + i,y + j);
					
					if(shortest->getTraversalCost() == 0)
						shortest = NULL;
				}
				catch(ExceptionType)
				{
				}
			}	
		}
		
		for(i = -1; i < 2; i++)
		{

			for(j = -1; j < 2; j++)
			{

				if(i == 0 && j == 0)
					continue;

				try
		        {
		        	tmp = map.getTile(x + i,y + j);
		        }
		        catch(ExceptionType)
		        {
		        	continue;
		        }
		        	
				if(tmp->getX() == activeBob->getX() && tmp->getY() == activeBob->getY())
				{
					S.push(tmp);
	        		done = true;
				}
	        	
	        	if(!shortest || (tmp->getTraversalCost() < shortest->getTraversalCost() && tmp->getTraversalCost() != 0))
	        		shortest = tmp;
	        		
	        }	
		}

		if(!done)
			S.push(shortest);
	}
	
	/************************************************************************************************
	* Add the shortest path to the move path queue
	*************************************************************************************************/
	TargetOptionType temp;
	while(!S.empty())
	{
		temp.x = S.top()->getX();
		temp.y = S.top()->getY();
		reader.movePathQueue.push(temp);
		S.pop();
	}
}

void GameState::getAbilityTargets()
{
	int x = activeBob->getX();
	int y = activeBob->getY();
	int AP;
//	if(activePlayer->getAP() < activeBob->getAP())
//		AP = activePlayer->getAP();
//	else
		AP = activeBob->getAP();
	
	int minRange = activeAbility->getMinRange(),
		maxRange = activeAbility->getMaxRange(),
		useCost = activeAbility->getUseCost();
	
	if(AP < useCost)
		return;
	
	Tile *T;
	// Traverses all tiles in a square size (minRange+maxRange)^2 centered on Bob
	int i, j;
	for(i = x - maxRange; i <= x + maxRange; i++)
	{
		for(j = y - maxRange; j <= y + maxRange; j++)
		{
			if(sqrt(pow(i-x,2) + pow(j-y,2)) < minRange) // Ignore tiles in minRange
				continue;
			
			if(maxRange != 1 && sqrt(pow(i-x,2) + pow(j-y,2)) > maxRange) // Ignore tiles outside maxRange
				continue;
				
			try
			{
				T = map.getTile(i,j);
				
				switch(actionRequest.ability)
				{
					case DefaultMelee: case MediumRanged:
						if(T->getBob() != NULL)
						{
							if(T->getBob()->getPlayerHandle() != activePlayer->getHandle())
							{
								TargetOptionType targetOption;
									targetOption.x = i;
									targetOption.y = j;
									targetOption.cost = useCost;
								if(isInLOS(targetOption))
									reader.targetOptionQueue.push(targetOption);
							}							
						}
						break;
						
					case LightLightningAOE:
						TargetOptionType targetOption;
							targetOption.x = i;
							targetOption.y = j;
							targetOption.cost = useCost;
						if(isInLOS(targetOption))
							reader.targetOptionQueue.push(targetOption);
						break;
						
					case MediumHeal:
						if(T->getBob() != NULL)
						{
							if(T->getBob()->getPlayerHandle() == activePlayer->getHandle())
							{
								TargetOptionType targetOption;
									targetOption.x = i;
									targetOption.y = j;
									targetOption.cost = useCost;
								if(isInLOS(targetOption))
									reader.targetOptionQueue.push(targetOption);
							}							
						}
						break;
				}
			}
			catch(ExceptionType e)
			{
			}
		}
	}
}

void GameState::validateAction()
{
	Bob *target;

	int useCost, minRange, maxRange;
	try
	{
		performAction = reader.getActionToPerform();
	}
	catch(ExceptionType)
	{
		actionState = PerformAction;
		return;	
	}
		
	if(activeBob->getID() != performAction.bobID || actionRequest.ability != performAction.ability)
	{
		actionState = ActionRequest;
		map.resetTraversalCost();
		clearTargetOptions();
		return;
	}
	
	if(activeAbility != NULL)
	{		
		useCost = activeAbility->getUseCost();
		minRange = activeAbility->getMinRange();
		maxRange = activeAbility->getMaxRange();
		
		if(sqrt(pow(performAction.x-activeBob->getX(),2) + pow(performAction.y-activeBob->getY(),2)) < minRange)
		{			
			actionState = PerformAction;
			return;
		}	
		
		if(maxRange != 1 && sqrt(pow(performAction.x-activeBob->getX(),2) + pow(performAction.y-activeBob->getY(),2)) > maxRange)
		{			
			actionState = PerformAction;
			return;
		}
	}
		
	switch(performAction.ability)
	{		
		case Cancel:
			actionState = ActionRequest;
			return;
			break;	
		
		case Move:		
			try
			{
				useCost = map.getTile(performAction.x, performAction.y)->getTraversalCost();
				
				if(useCost == 0)
				{
					actionState = PerformAction;
					return;
				}
			}
			catch(ExceptionType)
			{
				actionState = PerformAction;
				return;	
			}
			
			break;
		
		case DefaultMelee: case MediumRanged:		
			try
			{
				target = map.getTile(performAction.x, performAction.y)->getBob();
				
				if(target == NULL)
				{
					actionState = PerformAction;
					return;
				}
					
				if(activeBob->getPlayerHandle() == target->getPlayerHandle())
				{
					actionState = PerformAction;
					return;
				}
			}
			catch(ExceptionType)
			{
				actionState = PerformAction;
				return;
			}
			break;
			
		case LightLightningAOE:	
			try
			{
				map.getTile(performAction.x, performAction.y);
			}
			catch(ExceptionType)
			{
				actionState = PerformAction;
				return;
			}
			break;
			
		case MediumHeal:	
			try
			{
				target = map.getTile(performAction.x, performAction.y)->getBob();
				
				if(target == NULL)
				{
					actionState = PerformAction;
					return;
				}
				
				if(activeBob->getPlayerHandle() != target->getPlayerHandle())
				{
					actionState = PerformAction;
					return;
				}
			}
			catch(ExceptionType)
			{
				actionState = PerformAction;
				return;
			}
			break;
	}
	
//	if(useCost <= activePlayer->getAP() && useCost <= activeBob->getAP())
	if(useCost <= activeBob->getAP())
	{		
		actionState = ActionResults;
	}
	else
	{	
		actionState = PerformAction;
	}
}

void GameState::processAction()
{
	Bob* target;

	int i, j, x, y, targetNum, roll, useCost, min, max, hpMod, radius, cooldown;
	
	clearTargetOptions();
		
	switch(performAction.ability)
	{
		// Move abilities
		case Move:
			// Charge player and bob for the move
			activeBob->setAP(-ceil((map.getTile(performAction.x, performAction.y)->getTraversalCost()))); // subtract action cost from Bob's current AP
			activePlayer->setAP(-ceil((map.getTile(performAction.x, performAction.y)->getTraversalCost())));// subtract action cost from Player's current AP
			
			getShortestPath();
			map.resetTraversalCost();
			
			TargetOptionType movePath;
			while(!reader.movePathQueue.empty())
			{
				movePath = reader.movePathQueue.front();
				reader.movePathQueue.pop();
				
				placeBob(activeBob, movePath.x, movePath.y);
				
				draw();
				
				animStart = clock();
				while(clock() - animStart < FRAME_RATE);
			}
			
			//placeBob(activeBob, performAction.x, performAction.y);
						
			actionState = ActionRequest;
			break;
			
		// Single Target Attack Abilities
		case DefaultMelee: case MediumRanged:
			target = map.getTile(performAction.x,performAction.y)->getBob();
			useCost = activeAbility->getUseCost();
			
			// Charge player and bob for the action
			activeBob->setAP(-useCost); // subtract action cost from Bob's current AP
			activePlayer->setAP(-useCost);// subtract action cost from Player's current AP
			////Add cooldown adjustment here	
			
			// Roll to hit
			targetNum = (activeBob->getToHit() * (100 - target->getDodge()))/100;
			roll = (rand()%100)+1;

			if(roll > targetNum)
			{
				setTurnsLeft();
				actionState = ActionRequest;
				break;
			}
			
			// Hit! Roll damage			
			min = ((ActiveHPMod*)activeAbility)->getMin();
			max = ((ActiveHPMod*)activeAbility)->getMax();
			
			hpMod = (rand()%(max - min + 1)) + min;
			
			if(target->getMitigation() < hpMod)
				target->setHP(- hpMod + target->getMitigation());
			
			if(target->getHP() == 0)
				killBob(target);

			setTurnsLeft();
			
			actionState = ActionRequest;
			break;
			
		// Non-Bob targeted AOE attacks
		case LightLightningAOE:
			useCost = activeAbility->getUseCost();
			
			// Charge player and bob for the action
			activeBob->setAP(-useCost); // subtract action cost from Bob's current AP
			activePlayer->setAP(-useCost);// subtract action cost from Player's current AP
			////Add cooldown adjustment here	
			
			radius = ((ActiveHPMod*)activeAbility)->getRadius();
			x = performAction.x;
			y = performAction.y;
			
			// Roll damage			
			min = ((ActiveHPMod*)activeAbility)->getMin();
			max = ((ActiveHPMod*)activeAbility)->getMax();
			
			hpMod = (rand()%(max - min + 1)) + min;
			
			Tile *T;
			// Traverses all tiles in a square size (minRange+maxRange)^2 centered on target
			for(i = x - radius; i <= x + radius; i++)
			{
				for(j = y - radius; j <= y + radius; j++)
				{
					if(radius != 1 && sqrt(pow(i-x,2) + pow(j-y,2)) > radius) // Ignore tiles outside maxRange
						continue;
						
					try
					{
						T = map.getTile(i,j);
						
						target = T->getBob();
						
						if(target == NULL)
							continue;
						
						// Roll to dodge
						targetNum = target->getDodge();
						roll = (rand()%100)+1;
						
						if(roll > targetNum) // No dodge, full damage
						{
							if(target->getMitigation() < hpMod)
							{
								target->setHP(-hpMod + target->getMitigation());
							}
						}
						else // Dodge, half damage
						{
							if(target->getMitigation() < hpMod)
								target->setHP(- hpMod/2 + target->getMitigation());
						}
							
						if(target->getHP() == 0)
						{
							killBob(target);
						}
					}
					catch(ExceptionType e)
					{
					}
				}
			}
			
			setTurnsLeft();

			actionState = ActionRequest;
			break;
			
		// Buffs
		case MediumHeal:
			target = map.getTile(performAction.x,performAction.y)->getBob();
			useCost = activeAbility->getUseCost();
			
			// Charge player and bob for the action
			activeBob->setAP(-useCost); // subtract action cost from Bob's current AP
			activePlayer->setAP(-useCost);// subtract action cost from Player's current AP
			////Add cooldown adjustment here	
			
			min = ((ActiveHPMod*)activeAbility)->getMin();
			max = ((ActiveHPMod*)activeAbility)->getMax();
			
			hpMod = (rand()%(max - min + 1)) + min;
			
			target->setHP(hpMod);
			
			setTurnsLeft();

			actionState = ActionRequest;
			break;
	}
}

bool GameState::verifyActionUsable()
{
	int i;
	for(i = 0; i < activeBob->getActiveHPSize(); i++)
	{
		if(activeBob->getActiveHPMods()[i].ability->getName() == activeAbility->getName())
		{
			if(activeBob->getActiveHPMods()[i].turnsLeft == 0)
				return true;
		}
	}
	for(i = 0; i < activeBob->getActiveStatSize(); i++)
	{
		if(activeBob->getActiveStatMods()[i].ability->getName() == activeAbility->getName())
		{
			if(activeBob->getActiveStatMods()[i].turnsLeft == 0)
				return true;
		}
	}
	return false;
}

// Calculate if a target option is in line of sight
bool GameState::isInLOS(TargetOptionType targetOption)
{
	double a, b, c;

	queue< Tile* > occupied;
 	
 	for(int x=0; x < this->map.getLength(); x++)
 	{
 		for(int y=0; y < this->map.getHeight(); y++)
 		{
			if(this->map.getTile(x,y)->getBob())
 			{
 				occupiedSize++;
 				occupied.push(this->map.getTile(x,y));
 			}
 		}
 	}
 	occupiedX = new int[occupiedSize];
 	occupiedY = new int[occupiedSize];
 	for(int k=0; k < occupiedSize; k++)
 	{
 		occupiedX[k] = occupied.front()->getX();
 		occupiedY[k] = occupied.front()->getY();
 		occupied.pop();
 	}
	
	/************************************************************************************************
 	* Calculate line of sight based on objects on the map
 	*************************************************************************************************/
	

	// THESE FUNCTIONS DO NOT WORK FOR STRAIGHT VERTICAL OR HORIZONTAL LoS


		
	a = (targetOption.x - activeBob->getX())* (targetOption.x - activeBob->getX()) +
		(targetOption.y - activeBob->getY())* (targetOption.y - activeBob->getY());

		// checking for non-moving obstacles
	for(int i = 0; i < obstacleSize; i++)
	{
			// if an obstacle is between the originating tile and the target tile
		if(((targetOption.x > activeBob->getX() && obstacleX[i] > activeBob->getX()) ||
			(targetOption.x < activeBob->getX() && obstacleX[i] < activeBob->getX())) ||
			((targetOption.y > activeBob->getY() && obstacleY[i] > activeBob->getY()) ||
			(targetOption.y < activeBob->getY() && obstacleY[i] < activeBob->getY())))
		{
			if(abs(targetOption.x - activeBob->getX()) > abs(obstacleX[i] - activeBob->getX()) ||
				abs(targetOption.y - activeBob->getY()) > abs(obstacleY[i] - activeBob->getY()))
			{
				b = 2 * ((targetOption.x - activeBob->getX()) * (activeBob->getX() - obstacleX[i]) +
					(targetOption.y - activeBob->getY()) * (activeBob->getY() - obstacleY[i]));

				c = (obstacleX[i] * obstacleX[i]) + (obstacleY[i] * obstacleY[i]) + (activeBob->getX() * 
					activeBob->getX()) + (activeBob->getY() * activeBob->getY()) - (2 * ((obstacleX[i] *
					activeBob->getX()) + (obstacleY[i] * activeBob->getY()))) - 0.25;
				// Final value determines if sight ray intersects an object
				if((b*b - 4*a*c) > 0)
				{
					delete [] occupiedX;
 					delete [] occupiedY;
					return false;
				}
			}
		}
	}
		// checking for Bobs obscuring the line of sight
	for(i = 0; i < occupiedSize; i++)
	{
		if(((targetOption.x > activeBob->getX() && occupiedX[i] > activeBob->getX()) ||
			(targetOption.x < activeBob->getX() && occupiedX[i] < activeBob->getX())) ||
			((targetOption.y > activeBob->getY() && occupiedY[i] > activeBob->getY()) ||
			(targetOption.y < activeBob->getY() && occupiedY[i] < activeBob->getY())))
		{
			if(abs(targetOption.x - activeBob->getX()) > abs(occupiedX[i] - activeBob->getX()) ||
				abs(targetOption.y - activeBob->getY()) > abs(occupiedY[i] - activeBob->getY()))
			{
				b = 2 * ((targetOption.x - activeBob->getX()) * (activeBob->getX() - occupiedX[i]) +
					(targetOption.y - activeBob->getY()) * (activeBob->getY() - occupiedY[i]));

				c = (occupiedX[i] * occupiedX[i]) + (occupiedY[i] * occupiedY[i]) + (activeBob->getX() * 
					activeBob->getX()) + (activeBob->getY() * activeBob->getY()) - (2 * ((occupiedX[i] *
					activeBob->getX()) + (occupiedY[i] * activeBob->getY()))) - 0.25;
				// Final value determines if sight ray intersects an object
				if((b*b - 4*a*c) > 0)
				{
					delete [] occupiedX;
 					delete [] occupiedY;
					return false;
				}
			}
		}
	}
	
	/************************************************************************************************
 	* Calculate line of sight based on map elevations
 	* Uses Bresenham's line drawing algorithm
 	*************************************************************************************************/
 	/*int x0, y0, x1, y1, deltaX, deltaY, deltaZ, error, deltaErr, x, y, stepX, stepY, z0, z1, z;
 	x0 = activeBob->getX();
 	y0 = activeBob->getY();
 	x1 = targetOption.x;
 	y1 = targetOption.y;
 	z0 = map.getTile(x0,y0)->getElevation() + 5;
 	z1 = map.getTile(x1,y1)->getElevation() + 5;
 	deltaX = abs(x1 - x0);
 	deltaY = abs(y1 - y0);
 	deltaZ = z1 - z0;
 	error = 0;
 	deltaErr = deltaY;
 	x = x0;
 	y = y0;
 	// Adjust for positive or negative slopes
 	if(x0 < x1)
 		stepX = 1;
 	else
 		stepX = -1;
 	// Adjust for positive or negative slopes
 	if(y0 < y1)
 		stepY = 1;
 	else
 		stepY = -1;
 	bool steep;
 	// Determine if slope is between -1 and 1
 	if(abs(y1 - y0) > abs(x1 - x0))
 		steep = true;
 	else
 		steep = false;
 	
 	double dist, slope, slope1;
 	// Calculate slope of height
 	dist = sqrt((double)(deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ));
 	slope = deltaZ / dist;
 	
 	if(steep)
 	{
 		// swap x and y
 		x0 ^= y0 ^= x0 ^= y0;
 		x1 ^= y1 ^= x1 ^= x1;
 	}
 	
 	while(x != x1)
 	{
 		x = x + stepX;
 		error = error + deltaErr;
 		if(2*error > deltaX)
 		{
 			y = y + stepY;
 			error = error - deltaErr;
 		}
 		if(steep)
 		{
 			// X and Y are swapped
 			z = map.getTile(y,x)->getElevation();
 		}
 		else
 		{
 			z = map.getTile(x,y)->getElevation();
 		}
 		dist = sqrt((double)((x-x0)*(x-x0) + (y-y0)*(y-y0) + (z-z0)*(z-z0)));
		if(slope < (z-z0)/dist)
			return false;
 	}*/
	
 	// function line(x0, x1, y0, y1)
    // boolean steep := abs(y1 - y0) > abs(x1 - x0)
    // if steep then
    //     swap(x0, y0)
    //     swap(x1, y1)
    // int deltax := abs(x1 - x0)
    // int deltay := abs(y1 - y0)
    // int error := 0
    // int deltaerr := deltay
    // int x := x0
    // int y := y0
    // if x0 < x1 then xstep := 1 else xstep := -1
    // if y0 < y1 then ystep := 1 else ystep := -1
    /* if steep then plot(y,x) else plot(x,y) */
    // while x != x1
    //     x := x + xstep
    //     error := error + deltaerr
    //     if 2×error > deltax
    //         y := y + ystep
    //         error := error - deltax
    //     if steep then plot(y,x) else plot(x,y)
    delete [] occupiedX;
 	delete [] occupiedY;
	return true;
}

void GameState::clearTargetOptions()
{
	while(!reader.targetOptionQueue.empty())
		reader.targetOptionQueue.pop();
}

void GameState::clearQueues()
{
	while(!reader.actionRequestQueue.empty())
		reader.actionRequestQueue.pop();
		
	while(!reader.actionToPerformQueue.empty())
		reader.actionToPerformQueue.pop();
	
	while(!reader.targetOptionQueue.empty())
		reader.targetOptionQueue.pop();
}

/************************************************************************************************
* Moves Bob to a new tile location
*************************************************************************************************/
bool GameState::placeBob(Bob* bob, int x, int y)
{
	Tile* T;
	
	try
	{

		/************************************************************************************************
		* Checks if desired tile exists
		*************************************************************************************************/
		T = map.getTile(x, y);
		
		/************************************************************************************************
	 	* Checks if desired tile is occupied
	 	*************************************************************************************************/
		if(T->getOccupied())
			return false;
		
		/************************************************************************************************
	 	* Checks if Bob is already on a tile
	 	*************************************************************************************************/
		T = map.getTile(bob->getX(), bob->getY());

		/************************************************************************************************
	 	* Removes Bob from his old tile
	 	*************************************************************************************************/
		T->setOccupied(false);
		T->setBob(NULL);
		bob->setPosition(-1,-1);

	}
	catch(ExceptionType)
	{
	}
	
	try
	{
		/************************************************************************************************
	 	* Checks if desired tile exists
	 	*************************************************************************************************/
		T = map.getTile(x, y);
		
		/************************************************************************************************
	 	* Moves Bob to the desired tile
	 	*************************************************************************************************/
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

/************************************************************************************************
* Removes a dead Bob from the map
*************************************************************************************************/
bool GameState::killBob(Bob* bob)
{
	/************************************************************************************************
 	* Cannot remove a living Bob
 	*************************************************************************************************/
	if(bob->getHP() > 0)
		return false;
	
	Tile* T;
	
	try
	{
		/************************************************************************************************
	 	* Checks if Bob is actually on the map
	 	*************************************************************************************************/
		T = map.getTile(bob->getX(),bob->getY());
		
		/************************************************************************************************
	 	* Removes Bob from the map
	 	*************************************************************************************************/
		T->setOccupied(false);
		T->setBob(NULL);
		bob->setPosition(-1,-1);
		bob->setAP(-bob->getAP());
	}
	catch(ExceptionType)
	{
	}
	
	return false;
}

bool GameState::gameOver()
{
	bool finished;
	int i, j, playersLeft = 0;
	
	for(i = 0; i < numPlayers; i++) // scroll through player list
	{
		for(j = 0; j < players[i].getNumBobs(); j++) // scroll through player's bobs
		{
			if(players[i].getBobs()[j].getHP() > 0) // live bob found on team
			{
				playersLeft++;
				break;
			}
		}
	}
	
	if(playersLeft < 2)
		return true;
		
	return false;	
}

ActionStateType GameState::getActionState()
{
	return actionState;	
}
ActiveAbility* GameState::getActiveAbility()
{
	return activeAbility;	
}

Bob* GameState::getActiveBob()
{
	return activeBob;
}

Player* GameState::getActivePlayer()
{
	return activePlayer;	
}

int GameState::getID()
{
	return ID;	
}

Map* GameState::getMap()
{
	return &map;	
}

int GameState::getAPLimit()
{
	return APLimit;	
}

int GameState::getNumPlayers()
{
	return numPlayers;	
}

Player* GameState::getPlayers()
{
	return players;
}

Reader* GameState::getReader()
{
	return &reader;	
}

void GameState::setTurnsLeft()
{
	int i;
	for(i = 0; i < activeBob->getActiveHPSize(); i++)
	{
		if(activeBob->getActiveHPMods()[i].ability->getName() == activeAbility->getName())
		{
			activeBob->getActiveHPMods()[i].turnsLeft = activeAbility->getCooldown();

			return;
		}
	}
	for(i = 0; i < activeBob->getActiveStatSize(); i++)
	{
		if(activeBob->getActiveStatMods()[i].ability->getName() == activeAbility->getName())
		{
			activeBob->getActiveStatMods()[i].turnsLeft = activeAbility->getCooldown();

			return;
		}
	}
}

void GameState::draw()
{
	system("cls");
	
	Bob *B;
	int x, y, i, j, pnum, bnum;
	
	x = map.getLength();
	y = map.getHeight();
	int** targetMap = new int*[y];
	for(i = 0; i < y; i++)
	{
		targetMap[i] = new int[x];
		for(j = 0; j < x; j++)
			targetMap[i][j] = 0;
	}
	
	j = reader.targetOptionQueue.size();
	TargetOptionType target;
	for(i = 0; i < j; i++)
	{
		target = reader.targetOptionQueue.front();
		reader.targetOptionQueue.push(target);
		reader.targetOptionQueue.pop();
		
		targetMap[target.x][target.y] = target.cost;
	}
	
	cout << endl;
	cout << "    ";
	for(x = 0; x < map.getLength(); x++)
	{
		cout << "   " << x << "    ";
		if(x < 10)
		{
			cout << " ";
		}
	}
	cout << endl;
	
	for(y = map.getHeight()-1; y > -1; y--)
	{
		cout << "   ";
		for(x = 0; x < map.getLength(); x++)
			cout << "+--------";	
		cout << "+\n";
		
		cout << "   ";
		for(x = 0; x < map.getLength(); x++)
		{
			if(targetMap[x][y] != 0)
			{
				cout << "|";
				
				if(x < 10)
					cout << " ";
				
				cout << x << "," << y;
				
				if(y < 10)
					cout << " ";
				
				cout << " ";
				
				if(targetMap[x][y] < 0)
				{
					cout << " *";
				}
				else if(targetMap[x][y] > 0)
				{
					if(targetMap[x][y] < 10)
					{
						cout << " ";
					}
					cout << targetMap[x][y];
				}
			}
			else
			{
				cout << "|        ";
			}
		}
		cout << "|\n";
		
		cout << "   ";
		for(x = 0; x < map.getLength(); x++)
			cout << "|        "; 
		cout << "|\n";
		
		cout << " ";
		
		if(y < 10)
			cout << " ";
			
		cout << y;
		for(x = 0; x < map.getLength(); x++)
		{
			B = map.getTile(x,y)->getBob();
			
			if(B == NULL)
			{
				cout << "|        ";
			}
			else
			{
				for(i = 0; i < numPlayers; i++)
				{
					if(B->getPlayerHandle() == players[i].getHandle())
						pnum = i;
				}
				
				for(i = 0; i < players[pnum].getNumBobs(); i++)
				{
					if(B->getID() == players[pnum].getBobs()[i].getID())
						bnum = i;
				}
				
				cout << "| P" << pnum << "  B" << bnum << " ";
			}
		}
		cout << "| " << y << endl;
		
		cout << "   ";
		for(x = 0; x < map.getLength(); x++)
		{
			B = map.getTile(x,y)->getBob();
			
			if(B == NULL)
			{
				cout << "|        ";
			}
			else
			{
				cout << "| HP: ";
				
				if(B->getHP() < 10)
				{
					cout << "0";
				}
					
				cout << B->getHP() << " ";
			}
		}
		cout << "|\n";
		
		cout << "   ";
		for(x = 0; x < map.getLength(); x++)
		{
			B = map.getTile(x,y)->getBob();
			
			if(B == NULL)
			{
				cout << "|        ";
			}
			else
			{
				cout << "| AP: ";
				
				if(B->getAP() < 10)
				{
					cout << "0";
				}
					
				cout << B->getAP() << " ";
			}
		}
		cout << "|\n";
	}
	
	cout << "   ";
	for(x = 0; x < map.getLength(); x++)
		cout << "+--------";	
	cout << "+\n";
	
	cout << "    ";
	for(x = 0; x < map.getLength(); x++)
	{
		cout << "   " << x << "    ";
		if(x < 10)
		{
			cout << " ";
		}
	}
	cout << endl << endl;
	
	for(i = 0; i < map.getHeight(); i++)
		delete [] targetMap[i];
	delete [] targetMap;
}

	