#include "GameState.h"

int main()
{
	bool placed;
	int i, j, x, y, bNum, act, numPlayers = 0, numBobs = 0, startSize;
	
	cout << endl;
	cout << "  Battle Bob Beta v0.10" << endl << endl;
	
	while(true)
	{
		cout << "  How many Players? ";
		cin >> numPlayers;
		
		if(numPlayers < 2 || numPlayers > 4)
			cout << "  You must set between 2 and 4 Players!" << endl << endl;
		else
			break;
	}
	
	cout << endl;
	
	while(true)
	{
		cout << "  How many Bobs per Player? ";
		cin >> numBobs;
		
		if(numBobs < 1)
			cout << "You must set at least 1 Bob per Player!" << endl << endl;
		else
			break;
	}
	
	string *handle = new string[numPlayers];
	Player *players = new Player[numPlayers];
		
	for(i = 0; i < numPlayers; i++)
	{
		cout << endl;
		cout << "  Player " << i << " enter your handle: ";
		
		cin >> handle[i];
		
		for(j = 0; j < i; j++)
		{
			if(handle[i] == handle[j])
			{
				cout << "  Duplicate handles not allowed!" << endl << endl;
				i--;
				break;
			}
		}
		
	}
	
	for(i = 0; i < numPlayers; i++)
	{
		players[i] = Player(handle[i], Human);
		players[i].setAP(numBobs*5);
	}
	
	BobHPMod *HPMod = new BobHPMod[4];
		struct BobHPMod hpMod;
		hpMod.ability = &DefaultMeleeAbility;
		hpMod.turnsLeft = 0;
	HPMod[0] = hpMod;
		hpMod.ability = &MediumRangedAbility;
		hpMod.turnsLeft = 0;
	HPMod[1] = hpMod;
		hpMod.ability = &LightLightningAOEAbility;
		hpMod.turnsLeft = 0;
	HPMod[2] = hpMod;
		hpMod.ability = &MediumHealAbility;
		hpMod.turnsLeft = 0;
	HPMod[3] = hpMod;

	BobStatMod *StatMod = new BobStatMod[0];
	BobPassive *Pass = new BobPassive[0];
	
	Bob *bobs;
	for(i = 0; i < numPlayers; i++)
	{
		bobs = new Bob[numBobs];
		for(j = 0; j < numBobs; j++)
		{
			bobs[j] = Bob(players[i].getHandle(), 50, 10, 80, 0, 1, 0, 0, 4, 0, Pass, HPMod, StatMod);
		}
		players[i].setBobs(bobs, numBobs);
	}
	
	Map map1 = Map(numPlayers*numBobs, numPlayers*numBobs, Grass);
	
	GameState game = GameState();
	game.initialize(players, numPlayers, numBobs*5, map1);
	
	delete [] HPMod;
	delete [] StatMod;
	delete [] Pass;
	delete [] bobs;
	delete [] players;
	delete [] handle;
	
	if(numBobs < MIN_START_SIZE)
		startSize = MIN_START_SIZE;
	else
		startSize = numBobs;
	
	for(i = 0; i < game.getNumPlayers(); i++)
	{
		game.clearTargetOptions();
				
		for(j = 0; j < game.getPlayers()[i].getNumBobs(); j++)
		{
			placed = false;
			while(!placed)
			{
				
				TargetOptionType target;
				if(i == 0)
				{
					for(x = 0; x < startSize; x++)
					{
						for(y = 0; y < startSize; y++)
						{
							target.x = x;
							target.y = y;
							target.cost = -1;
							
							game.getReader()->targetOptionQueue.push(target);
						}
					}
				}
				else if(i == 1)
				{
					for(x = game.getMap()->getLength() - startSize; x < game.getMap()->getLength(); x++)
					{
						for(y = game.getMap()->getHeight() - startSize; y < game.getMap()->getHeight(); y++)
						{
							target.x = x;
							target.y = y;
							target.cost = -1;
							
							game.getReader()->targetOptionQueue.push(target);
						}
					}
				}
				else if(i == 2)
				{
					for(x = game.getMap()->getLength() - startSize; x < game.getMap()->getLength(); x++)
					{
						for(y = 0; y < startSize; y++)
						{
							target.x = x;
							target.y = y;
							target.cost = -1;
							
							game.getReader()->targetOptionQueue.push(target);
						}
					}
				}
				else if(i == 3)
				{
					for(x = 0; x < startSize; x++)
					{
						for(y = game.getMap()->getHeight() - startSize; y < game.getMap()->getHeight(); y++)
						{
							target.x = x;
							target.y = y;
							target.cost = -1;
							
							game.getReader()->targetOptionQueue.push(target);
						}
					}
				}
				
				game.draw();
				
				cout << "   " << game.getPlayers()[i].getHandle() << ", place Bob " << j << ":" << endl;
				cout << "   X: ";
				cin >> x;
				cout << "   Y: ";
				cin >> y;
				
				if(i == 0 && (x > startSize-1 || y > startSize-1))
					continue;
				
				if(i == 1 && (x < game.getMap()->getLength() - startSize || y < game.getMap()->getHeight() - startSize))
					continue;
				
				if(i == 2 && (x < game.getMap()->getLength() - startSize || y > startSize-1))
					continue;
					
				if(i == 3 && (x > startSize-1 || y < game.getMap()->getHeight() - startSize))
					continue;
				
				placed = game.placeBob(&game.getPlayers()[i].getBobs()[j], x, y);
			}
		}
	}
	game.clearTargetOptions();
	
	while(!game.gameOver())
	{
		while(game.getActionState() == 0)
		{
			while(true)
			{
				game.draw();
				
				cout << "   AP Remaining: " << game.getActivePlayer()->getAP() << endl << endl;
				cout << "   " << game.getActivePlayer()->getHandle() << ", select a Bob to act:" << endl;
				cout << "   Bob #: ";
				cin >> bNum;
				
				// Pass the Turn
				if(bNum < 0)
				{
					struct ActionRequestType request;
						request.bobID = -1;
						request.ability = PassTurn;
					game.getReader()->actionRequestQueue.push(request);
					
					game.turnLoop();
					break;
				}
				else if(bNum < numBobs && game.getActivePlayer()->getBobs()[bNum].getHP() > 0)
					break;
			}
			
			while(bNum > -1)
			{
				game.draw();
				
				cout << "   AP Remaining: " << game.getActivePlayer()->getAP() << endl << endl;
				cout << "   " << game.getActivePlayer()->getHandle() << ", select an action: 0-Cancel 1-Move 2-Melee 3-Ranged 4-AOE 5-Heal" << endl;
				cout << "   Action #: ";
				cin >> act;
				
				if(act >= 0 && act <=5)
					break;
			}
			
			if(act == 0)
				continue;
			
			struct ActionRequestType request;
				request.bobID = game.getActivePlayer()->getBobs()[bNum].getID();
				request.ability = (AbilityNameType)act;
			game.getReader()->actionRequestQueue.push(request);
			
			game.turnLoop();
			game.turnLoop();
			game.turnLoop();
		}
		
		while(game.getActionState() == 2)
		{
			game.draw();
				
			cout << "   AP Remaining: " << game.getActivePlayer()->getAP() << endl << endl;
			cout << "   " << game.getActivePlayer()->getHandle() << ", select a target:" << endl;
			cout << "   X: ";
			cin >> x;
			cout << "   Y: ";
			cin >> y;
			
			struct ActionToPerformType action;
				action.bobID = game.getActivePlayer()->getBobs()[bNum].getID();
				
				if(x == -1 || y == -1)
					action.ability = Cancel;
				else
					action.ability = (AbilityNameType)act;
				action.x = x;
				action.y = y;
			game.getReader()->actionToPerformQueue.push(action);
			
			game.turnLoop();
			game.turnLoop();
			game.turnLoop();
		}
	}
	
	cout << "Thanks for Playing! (Q to Quit)\n";
	cin >> i;
	return 0;
}