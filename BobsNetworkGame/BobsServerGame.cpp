#include "BobsNetwork.h"
#include "AI.h"

#define SERVER_PORT 9000

unsigned char GetPacketIdentifier(Packet *p);		//IDENTIFY PACKET FUNCTION

void BobsServerGame(RakServerInterface *BobsServer, StaticServerDataStruct ServerData, GameState game, Map map1)
{
	puts("BobsServerGame");
	char temp2[256];

	//STRUCTS SENT TO CLIENT
	Flag flag;
	WhosTurn whosTurn;
	whosTurn.typeID = WHOS_TURN;
	ShellMap shellMap;										//HOLDS BOBS POSITION IN MAP TO SEND TO CLIENT
	shellMap.typeID = SHELL_MAP;
	NetworkTargetOption networkOption;
	networkOption.typeID = TARGET_TILES;
	
	BlockedTiles blockedTiles;
	blockedTiles.typeID = BLOCKED_TILES;

	//GAMESTATE STRUCT
	ActionRequestType actionRequest;
	ActionToPerformType actionType;

	AI *ai = new AI(&game);


	for (int i = 0; i < map1.getHeight(); i++)
	{
		for(int j = 0; j < map1.getLength(); j++)
		{
			if((game.getMap()->getTile(i, j)->getOccupied()) == 1)
			{

		//		cout << "Blocked  i, j = " << i << ", " << j << endl;

				blockedTiles.blockedX = i;
				blockedTiles.blockedY = j;
				BobsServer->Send((char *)&blockedTiles, sizeof(blockedTiles), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
			}
		}
	}

	//REDISTRUBUTE BOBS LOCATIONS TO CLIENTS
	for (int i = 0; i < atoi(ServerData.NumOfPlayers); i++)
	{ 
		sprintf(temp2,"%s",game.getPlayers()[i].getHandle().c_str());
		printf("PLAYER: %s...\n", temp2);
		for (int j = 0; j < game.getPlayers()[i].getNumBobs(); j++)
		{
			printf("    Bobs ID: %d  ", game.getPlayers()[i].getBobs()[j].getID()); 																								
			printf("LOCATED AT X: %d Y:%d \n", game.getPlayers()[i].getBobs()[j].getX(), game.getPlayers()[i].getBobs()[j].getY());
			strcpy(shellMap.player,temp2);			
			shellMap.bob = game.getPlayers()[i].getBobs()[j].getID();
			shellMap.x = game.getPlayers()[i].getBobs()[j].getX();
			shellMap.y = game.getPlayers()[i].getBobs()[j].getY();
			shellMap.AP = game.getPlayers()[i].getBobs()[j].getAP();
			shellMap.HP = game.getPlayers()[i].getBobs()[j].getHP();
			printf("_______________\n");
			BobsServer->Send((char *)&shellMap, sizeof(shellMap), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
		}
	}
	puts("MapDistributed");

	//NOTIFY DONE DISTRIBUTING BOBS 
	flag.typeID = MAP_DISTRIBUTED;	
	BobsServer->Send((char*)&flag, sizeof(flag), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);

	//GET WHOS CLIENT TURN
	strcpy(whosTurn.player,game.getActivePlayer()->getHandle().c_str());
	whosTurn.AP = game.getActivePlayer()->getAP();
	whosTurn.bob = game.getActivePlayer()->getNumBobs();
	printf("IT IS %s TURN\n",whosTurn.player);	



		// if the AI has the first turn
	while(game.getActivePlayer()->getPlayerType() == CPU)
	{
		cout << game.getActivePlayer()->getHandle() << "'s turn" << endl;

		ai->determineAction();

		for (int i = 0; i < map1.getHeight(); i++)
		{
			for(int j = 0; j < map1.getLength(); j++)
			{
				if((game.getMap()->getTile(i, j)->getOccupied()) == 1)
				{
					blockedTiles.blockedX = i;
					blockedTiles.blockedY = j;
					BobsServer->Send((char *)&blockedTiles, sizeof(blockedTiles), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
				}
			}
		}

		for (int i = 0; i < atoi(ServerData.NumOfPlayers); i++)
		{ 
			sprintf(temp2,"%s",game.getPlayers()[i].getHandle().c_str());
			printf("PLAYER: %s...\n", temp2); 


			for (int j = 0; j < game.getPlayers()[i].getNumBobs(); j++)
			{
				printf("    Bobs ID: %d  ", game.getPlayers()[i].getBobs()[j].getID()); 																								
				printf("LOCATED AT X: %d Y:%d \n", game.getPlayers()[i].getBobs()[j].getX(), game.getPlayers()[i].getBobs()[j].getY());
				strcpy(shellMap.player,temp2);		
				shellMap.bob = game.getPlayers()[i].getBobs()[j].getID();
				shellMap.x = game.getPlayers()[i].getBobs()[j].getX();
				shellMap.y = game.getPlayers()[i].getBobs()[j].getY();
				shellMap.AP = game.getPlayers()[i].getBobs()[j].getAP();
				shellMap.HP = game.getPlayers()[i].getBobs()[j].getHP();
				printf("_ _ _ _ _ _ _ _ _ _ _ _\n");
				BobsServer->Send((char *)&shellMap, sizeof(shellMap), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
			}				
		}	

		flag.typeID = MAP_DISTRIBUTED;	
		BobsServer->Send((char*)&flag, sizeof(flag), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);

		strcpy(whosTurn.player,game.getActivePlayer()->getHandle().c_str());
		whosTurn.AP = game.getActivePlayer()->getAP();
		printf("IT IS %s TURN\n",whosTurn.player);	
	}
		// End of AI's Turn



	BobsServer->Send((char*)&whosTurn,sizeof(whosTurn),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_PLAYER_ID,TRUE);
	
	//RECEIVE - IDENTIFY PACKET
	Packet* p;													//POINTER TO PACKET
	unsigned char packetIdentifier;								//HOLDS PACKET IDENTITY

	//MAIN BOBSERVERGAME LOOP
	bool loop2 = true;
	while (loop2)
	{	
		Sleep(30);												//KEEP RACKNET RESPONSIVE

		p = BobsServer->Receive();								//RECIEVE PACKET
		if (p==0)
			continue;											//PACKET EMPTY
		packetIdentifier = GetPacketIdentifier(p);				//GET THE PACKET IDENTIFIER
	
		//READS DATA FROM CLIENTS
		RequestStruct* theRequest = (RequestStruct *)p->data;   
		ActualTarget* theTarget = (ActualTarget *)p->data;		


		switch(packetIdentifier)		
		{

		//GET REQUEST FROM CLIENT, RETURN AVAILABLE TILES		
		case REQUEST_STRUCT:

			printf("REQUEST BOB ID: %d\n", theRequest->bobID);
			actionRequest.bobID = theRequest->bobID;
			actionRequest.ability = theRequest->ability;
				
			game.getReader()->actionRequestQueue.push(actionRequest);
			game.turnLoop();//grap
			game.turnLoop();//request
			game.turnLoop();//response

			printf("Bob: %d - Action: %d\n", game.getReader()->actionRequestQueue.front().bobID, (int)game.getReader()->actionRequestQueue.front().ability);
				
			if(game.getReader()->targetOptionQueue.empty())
			{

				printf("IT DIDN'T LOAD UP THE TARGETS\n");
				networkOption.x = -1;
				networkOption.y = -1;
				networkOption.cost = 0;
				game.clearQueues();
				BobsServer->Send((char*)&networkOption,sizeof(networkOption),HIGH_PRIORITY,RELIABLE_ORDERED,0, p->playerId,FALSE);
				break;
			}

			while(!game.getReader()->targetOptionQueue.empty())
			{
				networkOption.x = game.getReader()->targetOptionQueue.front().x;
				networkOption.y = game.getReader()->targetOptionQueue.front().y;
				networkOption.cost = game.getReader()->targetOptionQueue.front().cost;
				game.getReader()->targetOptionQueue.pop();
				BobsServer->Send((char*)&networkOption,sizeof(networkOption),HIGH_PRIORITY,RELIABLE_ORDERED,0,p->playerId,FALSE);
			}


			for (int i = 0; i < atoi(ServerData.NumOfPlayers); i++)
			{ 
				sprintf(temp2,"%s",game.getPlayers()[i].getHandle().c_str());
				printf("PLAYER: %s...\n", temp2); 


				for (int j = 0; j < game.getPlayers()[i].getNumBobs(); j++)
				{
					printf("    Bobs ID: %d  ", game.getPlayers()[i].getBobs()[j].getID()); 																								
					printf("LOCATED AT X: %d Y:%d \n", game.getPlayers()[i].getBobs()[j].getX(), game.getPlayers()[i].getBobs()[j].getY());
					strcpy(shellMap.player,temp2);		
					shellMap.bob = game.getPlayers()[i].getBobs()[j].getID();
					shellMap.x = game.getPlayers()[i].getBobs()[j].getX();
					shellMap.y = game.getPlayers()[i].getBobs()[j].getY();
					shellMap.AP = game.getPlayers()[i].getBobs()[j].getAP();
					shellMap.HP = game.getPlayers()[i].getBobs()[j].getHP();
					printf("_ _ _ _ _ _ _ _ _ _ _ _\n");
					BobsServer->Send((char *)&shellMap, sizeof(shellMap), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
				}
						
			}	


			for (int i = 0; i < map1.getHeight(); i++)
			{
				for(int j = 0; j < map1.getLength(); j++)
				{
					if((game.getMap()->getTile(i, j)->getOccupied()) == 1)
					{
						blockedTiles.blockedX = i;
						blockedTiles.blockedY = j;
						BobsServer->Send((char *)&blockedTiles, sizeof(blockedTiles), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
					}
				}
			}




			flag.typeID = DONE_TARGET_TILES;
			BobsServer->Send((char*)&flag,sizeof(flag),HIGH_PRIORITY,RELIABLE_ORDERED,0,p->playerId,FALSE);

			break;

		//GET CLIENTS ACTION, RETURN NEW CLIENTS TURN
		case ACTION_TARGET:


				// AI's turn
			while(game.getActivePlayer()->getPlayerType() == CPU)
			{
				cout << game.getActivePlayer()->getHandle() << "'s turn" << endl;

				ai->determineAction();

				for (int i = 0; i < map1.getHeight(); i++)
				{
					for(int j = 0; j < map1.getLength(); j++)
					{
						if((game.getMap()->getTile(i, j)->getOccupied()) == 1)
						{
							blockedTiles.blockedX = i;
							blockedTiles.blockedY = j;
							BobsServer->Send((char *)&blockedTiles, sizeof(blockedTiles), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
						}
					}
				}

				for (int i = 0; i < atoi(ServerData.NumOfPlayers); i++)
				{ 
					sprintf(temp2,"%s",game.getPlayers()[i].getHandle().c_str());
					printf("PLAYER: %s...\n", temp2); 


					for (int j = 0; j < game.getPlayers()[i].getNumBobs(); j++)
					{
						printf("    Bobs ID: %d  ", game.getPlayers()[i].getBobs()[j].getID()); 																								
						printf("LOCATED AT X: %d Y:%d \n", game.getPlayers()[i].getBobs()[j].getX(), game.getPlayers()[i].getBobs()[j].getY());
						strcpy(shellMap.player,temp2);		
						shellMap.bob = game.getPlayers()[i].getBobs()[j].getID();
						shellMap.x = game.getPlayers()[i].getBobs()[j].getX();
						shellMap.y = game.getPlayers()[i].getBobs()[j].getY();
						shellMap.AP = game.getPlayers()[i].getBobs()[j].getAP();
						shellMap.HP = game.getPlayers()[i].getBobs()[j].getHP();
						printf("_ _ _ _ _ _ _ _ _ _ _ _\n");
						BobsServer->Send((char *)&shellMap, sizeof(shellMap), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
					}				
				}	

				for (int i = 0; i < map1.getHeight(); i++)
				{
					for(int j = 0; j < map1.getLength(); j++)
					{
						if((game.getMap()->getTile(i, j)->getOccupied()) == 1)
						{
							blockedTiles.blockedX = i;
							blockedTiles.blockedY = j;
							BobsServer->Send((char *)&blockedTiles, sizeof(blockedTiles), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
						}
					}
				}
						
				flag.typeID = MAP_DISTRIBUTED;	
				BobsServer->Send((char*)&flag, sizeof(flag), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
						
				strcpy(whosTurn.player,game.getActivePlayer()->getHandle().c_str());
				whosTurn.AP = game.getActivePlayer()->getAP();
				
				printf("IT IS %s'S TURN\n",whosTurn.player);	
			}

					// Human player's turn
			actionType.bobID = theTarget->bobID;
			actionType.ability = theTarget->ability;
			actionType.x = theTarget->x;
			actionType.y = theTarget->y;

			game.getReader()->actionToPerformQueue.push(actionType);
			game.turnLoop();
			game.turnLoop();
			game.turnLoop();

			for (int i = 0; i < map1.getHeight(); i++)
			{
				for(int j = 0; j < map1.getLength(); j++)
				{
					if((game.getMap()->getTile(i, j)->getOccupied()) == 1)
					{
						blockedTiles.blockedX = i;
						blockedTiles.blockedY = j;
						BobsServer->Send((char *)&blockedTiles, sizeof(blockedTiles), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
					}
				}
			}

			for (int i = 0; i < atoi(ServerData.NumOfPlayers); i++)
			{ 
				sprintf(temp2,"%s",game.getPlayers()[i].getHandle().c_str());
				printf("PLAYER: %s...\n", temp2); 


				for (int j = 0; j < game.getPlayers()[i].getNumBobs(); j++)
				{
					printf("    Bobs ID: %d  ", game.getPlayers()[i].getBobs()[j].getID()); 																								
					printf("LOCATED AT X: %d Y:%d \n", game.getPlayers()[i].getBobs()[j].getX(), game.getPlayers()[i].getBobs()[j].getY());
					strcpy(shellMap.player,temp2);		
					shellMap.bob = game.getPlayers()[i].getBobs()[j].getID();
					shellMap.x = game.getPlayers()[i].getBobs()[j].getX();
					shellMap.y = game.getPlayers()[i].getBobs()[j].getY();
					shellMap.AP = game.getPlayers()[i].getBobs()[j].getAP();
					shellMap.HP = game.getPlayers()[i].getBobs()[j].getHP();
					printf("_ _ _ _ _ _ _ _ _ _ _ _\n");
					BobsServer->Send((char *)&shellMap, sizeof(shellMap), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
				}
						
			}	

			flag.typeID = MAP_DISTRIBUTED;	
			BobsServer->Send((char*)&flag, sizeof(flag), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
						
						
			strcpy(whosTurn.player,game.getActivePlayer()->getHandle().c_str());
			whosTurn.AP = game.getActivePlayer()->getAP();
				
			printf("IT IS %s TURN\n",whosTurn.player);	

			BobsServer->Send((char*)&whosTurn,sizeof(whosTurn),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_PLAYER_ID,TRUE);
			puts("________________________________________________________________________");

			break;

		//NO TARGETS AVAILABLE FOR CLIENT, REFRESH GAME
		case REFRESH:
			puts("REFRESHING");

			cout << "game.getReader()->actionRequestQueue.size() = " << game.getReader()->actionRequestQueue.size() << endl;

			while(game.getActivePlayer()->getPlayerType() == CPU)
			{

				cout << game.getActivePlayer()->getHandle() << "'s turn" << endl;

				ai->determineAction();

				for (int i = 0; i < map1.getHeight(); i++)
				{
					for(int j = 0; j < map1.getLength(); j++)
					{
						if((game.getMap()->getTile(i, j)->getOccupied()) == 1)
						{
							blockedTiles.blockedX = i;
							blockedTiles.blockedY = j;
							BobsServer->Send((char *)&blockedTiles, sizeof(blockedTiles), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
						}
					}
				}

				for (int i = 0; i < atoi(ServerData.NumOfPlayers); i++)
				{ 
					sprintf(temp2,"%s",game.getPlayers()[i].getHandle().c_str());
					printf("PLAYER: %s...\n", temp2); 


					for (int j = 0; j < game.getPlayers()[i].getNumBobs(); j++)
					{
						printf("    Bobs ID: %d  ", game.getPlayers()[i].getBobs()[j].getID()); 																								
						printf("LOCATED AT X: %d Y:%d \n", game.getPlayers()[i].getBobs()[j].getX(), game.getPlayers()[i].getBobs()[j].getY());
						strcpy(shellMap.player,temp2);		
						shellMap.bob = game.getPlayers()[i].getBobs()[j].getID();
						shellMap.x = game.getPlayers()[i].getBobs()[j].getX();
						shellMap.y = game.getPlayers()[i].getBobs()[j].getY();
						shellMap.AP = game.getPlayers()[i].getBobs()[j].getAP();
						shellMap.HP = game.getPlayers()[i].getBobs()[j].getHP();
						printf("_ _ _ _ _ _ _ _ _ _ _ _\n");
						BobsServer->Send((char *)&shellMap, sizeof(shellMap), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
					}				
				}	
				
			flag.typeID = MAP_DISTRIBUTED;	
			BobsServer->Send((char*)&flag, sizeof(flag), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
			
			strcpy(whosTurn.player,game.getActivePlayer()->getHandle().c_str());
			whosTurn.AP = game.getActivePlayer()->getAP();
	
			printf("IT IS %s'S TURN\n",whosTurn.player);	

			}
				// End of AI's Turn

			for (int i = 0; i < atoi(ServerData.NumOfPlayers); i++)
			{ 
				sprintf(temp2,"%s",game.getPlayers()[i].getHandle().c_str());
				printf("PLAYER: %s...\n", temp2); 

				for (int j = 0; j < game.getPlayers()[i].getNumBobs(); j++)
				{
					printf("    Bobs ID: %d  ", game.getPlayers()[i].getBobs()[j].getID()); 																								
					printf("LOCATED AT X: %d Y:%d \n", game.getPlayers()[i].getBobs()[j].getX(), game.getPlayers()[i].getBobs()[j].getY());
					strcpy(shellMap.player,temp2);		
					shellMap.bob = game.getPlayers()[i].getBobs()[j].getID();
					shellMap.x = game.getPlayers()[i].getBobs()[j].getX();
					shellMap.y = game.getPlayers()[i].getBobs()[j].getY();
					shellMap.AP = game.getPlayers()[i].getBobs()[j].getAP();
					shellMap.HP = game.getPlayers()[i].getBobs()[j].getHP();
					printf("_ _ _ _ _ _ _ _ _ _ _ _\n");
					BobsServer->Send((char *)&shellMap, sizeof(shellMap), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
				}
			}

			for (int i = 0; i < map1.getHeight(); i++)
			{
				for(int j = 0; j < map1.getLength(); j++)
				{
					if((game.getMap()->getTile(i, j)->getOccupied()) == 1)
					{
						blockedTiles.blockedX = i;
						blockedTiles.blockedY = j;
						BobsServer->Send((char *)&blockedTiles, sizeof(blockedTiles), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
					}
				}
			}
				
			flag.typeID = MAP_DISTRIBUTED;	
			BobsServer->Send((char*)&flag, sizeof(flag), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
			
			strcpy(whosTurn.player,game.getActivePlayer()->getHandle().c_str());
			whosTurn.AP = game.getActivePlayer()->getAP();
	
			printf("IT IS %s'S TURN\n",whosTurn.player);	

			game.clearQueues();

			BobsServer->Send((char*)&whosTurn,sizeof(whosTurn),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_PLAYER_ID,TRUE);
			puts("________________________________________________________________________");

			break;

		default:
			break;	
		}


		BobsServer->DeallocatePacket(p);
	}

	delete ai;
}