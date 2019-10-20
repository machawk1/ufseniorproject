#include "BobsNetwork.h"
#define SERVER_PORT 29000

using BasicDataStructures::List;

unsigned char GetPacketIdentifier(Packet *p);		//IDENTIFY PACKET FUNCTION
void BobsServerGame(RakServerInterface *BobsServer, StaticServerDataStruct ServerData, GameState game, Map map1);
Bob* buyAIBobs(int NumOfPlayers, int numOfAIPlayers, int AP, Player *players);

void BobsServer()
{
	//variables
	char temp[512];															
	char temp2[512];
	int ready = 0;																
	int done = 0;
	int area = 4;
	int a = 0;
	int AP;

	bool AIplaced = false;

	List<int> blockedListX;
	List<int> blockedListY;



	//Structures
	StaticServerDataStruct ServerData;											
	ClientsAP clientAP;	
	int mapChoice;

	BlockedTiles blockedTiles;
	blockedTiles.typeID = BLOCKED_TILES;
/****************************************************************************
						CREATING SERVER
*****************************************************************************/
	puts("---BOBSSERVER---");
	RakServerInterface *BobsServer=RakNetworkFactory::GetRakServerInterface();	//SERVER INITIALIZED
	

	int numOfAIPlayers = -1;
	

	while(true)
	{
		printf("Enter BobServer name: ");											//SERVER NAME
		gets(ServerData.serverName);
		
		if (ServerData.serverName[0]==0)
		{
			puts("USING Default: SERVER DEFAULT");
			strcpy(ServerData.serverName, "SERVER DEFAULT");
		}
		while(true)
		{
			printf("Enter Number Of Players(2 - 4): ");										//SERVER NUMBER OF PLAYERS
			gets(ServerData.NumOfPlayers);
			if (ServerData.NumOfPlayers[0]==0)
			{
				puts("Default Number of Players: 2");
				strcpy(ServerData.NumOfPlayers,"2");
			}
			if (atoi(ServerData.NumOfPlayers) < 2 || atoi(ServerData.NumOfPlayers) > 40)
			{
				continue;
			}
			break;
		}
	
		while(true)
		{
			printf("AP per Player (10 - 90): ");
			gets(temp);
			if(temp[0]==0)
			{
				printf("default clientAP : 30\n");
				clientAP.AP = 30;
				AP = 30;
			}
			else
			{
				clientAP.AP = atoi(temp);
				AP = atoi(temp);
			}

			if(clientAP.AP >= 10 && clientAP.AP <= 90)
			{
				break;
			}
		}


		int maxAI = atoi(ServerData.NumOfPlayers)-1;

		while(numOfAIPlayers < 0 || numOfAIPlayers > maxAI)
		{
			printf("Enter Number Of AI Players(0-%d): ", maxAI);	
			cin >> numOfAIPlayers;
		}


		while(true)
		{
			printf("Map Choices\n");
			printf("0. Blank_10      1. default_5        2. default_10     3. default_50\n");
			printf("4. Melee_Map_10  5. Middle_Block_10  6. Barracks_10    7. Skull_15\n\n");
			printf("Choose a map(0 - 7): ");
			cin >> mapChoice;

			if (mapChoice < 0 || mapChoice > 7)
			{
				mapChoice = 6;
				cout << "Default Map is Barracks_10" << endl;
			}
			break;
		}


		BobsServer->SetStaticServerData((char*)&ServerData, sizeof(ServerData));	//SET SERVER STATIC DATA
		if (BobsServer->Start(atoi(ServerData.NumOfPlayers), 0, 30, SERVER_PORT))	//INITIALIZE SERVER
		{		
			printf("STARTING...  %s \n", ServerData.serverName);	
			break;
		}	
		
	}
/****************************************************************************
						INITIALIZING GAME
*****************************************************************************/

	int NumOfPlayers = atoi(ServerData.NumOfPlayers);

	//Structs to send to Client
	clientAP.typeID = RETRIEVE_AP;		

	ServerMap map;											//MAP STRUCT TO SEND TO CLIENTS
	map.typeID = BROADCAST_MAP;

	NumberOfBobs numOfBobs;
	numOfBobs.typeID = CLIENTS_BOBS_ID;

	int np = atoi(ServerData.NumOfPlayers);										//HOLD MAX ALLOWED PLAYERS
	Player *players = new Player[np];											//INITIALIZE MAX ALLOWED PLAYERS
	
	Bob *bobs = new Bob[30];													//MAX NUMBER OF BOBS PER PLAYER

	BobPassive *passiveAbilities;
	BobHPMod *activeHPMods;
	BobStatMod *activeStatMods;
	
	int p_joined = 0;															//NUMBER OF PLAYERS THAT HAS JOINED THE ROOM

	Map map1;
	
	GameState game = GameState();												//INITIALIZE GAME

	int TotalNumberOfBobs = 0;

	char AIhandle[3];


	/*
	for(int i = 0; i< numOfAIPlayers; i++)
	{
		sprintf(AIhandle, "AI%d",i);
		players[i]=Player(AIhandle, CPU);
		printf("Creating CPU Player\n");
	}
	*/

	for(int i = NumOfPlayers-numOfAIPlayers; i< NumOfPlayers; i++)
	{
		sprintf(AIhandle, "AI%d",i);
		players[i]=Player(AIhandle, CPU);
		printf("Creating CPU Player\n");
	}


	bobs = buyAIBobs(NumOfPlayers, numOfAIPlayers, AP, players);
	ready = ready += numOfAIPlayers;	


	bool equalTurnName =false;
	bool looping = true;
	while (looping)
	{	
		Sleep(30);												//KEEP RACKNET RESPONSIVE
		Packet* p;												//POINTER TO PACKET
		unsigned char packetIdentifier;							//HOLDS PACKET IDENTITY
		p = BobsServer->Receive();								//RECIEVE PACKET
		if (p==0)
			continue;											//PACKET EMPTY
		packetIdentifier = GetPacketIdentifier(p);				//GET THE PACKET IDENTIFIER
	
		BobsStruct* theBob = (BobsStruct *)p->data;				//POINTS TO BOBSTRUCT RECEIVED FROM CLIENT
		BobsPosition* thePosition = (BobsPosition *)p->data;	//POINTS TO BOBPOSTION RECEIVED FROM CLIENT
		Flag* theFlag = (Flag *)p->data;						//POINTS TO FLAG TYPE
		NumberOfBobs* theNumOfBobs = (NumberOfBobs*)p->data;	//POINTS TO BOBS NumOf Bobs

		switch(packetIdentifier)		
		{
		case ID_NEW_INCOMING_CONNECTION:						//PLAYER CONNECTED
			printf("%s HAS JOINED\n",((StaticClientDataStruct*)(BobsServer->GetStaticClientData(p->playerId))->GetData())->name);
			//sprintf(temp, "%s HAS JOINED\n", ((StaticClientDataStruct*)(BobsServer->GetStaticClientData(p->playerId))->GetData())->name);
			//BobsServer->Send(temp, strlen(temp)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->playerId, true);
			sprintf(temp, "%s",((StaticClientDataStruct*)(BobsServer->GetStaticClientData(p->playerId))->GetData())->name);		

			players[p_joined]=Player(temp, Human);						//INITIALIZE PLAYER
			printf("Creating Human Player\n");

			clientAP.myplayerid = p_joined;
			p_joined = p_joined + 1;
			BobsServer->Send((char*)&clientAP,sizeof(clientAP),HIGH_PRIORITY,RELIABLE_ORDERED,0,p->playerId,false);
			break;

		case ID_DISCONNECTION_NOTIFICATION:						//PLAYER DISCONECTED
			printf("PLAYER DISCONECTED\n");
			sprintf(temp,"%s HAS BEEN DISCONNECTED\n",((StaticClientDataStruct*)(BobsServer->GetStaticClientData(p->playerId))->GetData())->name);
			BobsServer->Send(temp, strlen(temp)+1,HIGH_PRIORITY,RELIABLE_ORDERED,0,p->playerId,true);
			break;

		case INITIALIZED_BOB:									//INITIALIZING CLIENTS BOB
			sprintf(temp, "%s",((StaticClientDataStruct*)(BobsServer->GetStaticClientData(p->playerId))->GetData())->name);
			for(int i = 0; i<=atoi(ServerData.NumOfPlayers); i++)
			{
				
				if(players[i].getHandle() == temp)				
				{
					puts("SETTING BOB...");
					passiveAbilities = new BobPassive[theBob->passiveSize];
					activeHPMods = new BobHPMod[theBob->activeHPSize];
					activeStatMods = new BobStatMod[theBob->activeStatSize];

					for(int count = 0; count < theBob->passiveSize; count++)
					{
						switch(theBob->passiveAbilities[count])
						{
							case PassiveLightDodge :
								passiveAbilities[count].ability = &PassiveLightDodgeAbility;
								passiveAbilities[count].turnsLeft = 0;
								break;
							case PassiveLightArmor :
								passiveAbilities[count].ability = &PassiveLightArmorAbility;
								passiveAbilities[count].turnsLeft = 0;
								break;
							case PassiveLightMoveBoost :
								passiveAbilities[count].ability = &PassiveLightMoveBoostAbility;
								passiveAbilities[count].turnsLeft = 0;
								printf("Move Boost\n");
								break;
							case PassiveLightHPIncrease:
								passiveAbilities[count].ability = &PassiveLightHPIncreaseAbility;
								passiveAbilities[count].turnsLeft = 0;
								break;
						}
					}

					for(int count = 0; count < theBob->activeHPSize; count++)
					{
						switch(theBob->activeHPMods[count])
						{
							case DefaultMelee :
								activeHPMods[count].ability = &DefaultMeleeAbility;
								activeHPMods[count].turnsLeft = 0;
								break;
							case MediumMelee :
								activeHPMods[count].ability = &MediumMeleeAbility;
								activeHPMods[count].turnsLeft = 0;
								break;
							case MediumRanged :
								activeHPMods[count].ability = &MediumRangedAbility;
								activeHPMods[count].turnsLeft = 0;
								break;
							case LightLightningAOE :
								activeHPMods[count].ability = &LightLightningAOEAbility;
								activeHPMods[count].turnsLeft = 0;
								break;
							case MediumHeal :
								activeHPMods[count].ability = &MediumHealAbility;
								activeHPMods[count].turnsLeft = 0;
								break;
						}
					}

					for(int count = 0; count < theBob->activeHPSize; count++)
					{
						switch(theBob->activeStatMods[count])
						{
							case ActiveLightDodge :
								activeStatMods[count].ability = &ActiveLightDodgeAbility;
								activeStatMods[count].turnsLeft = 0;
								break;
						}
					}

					bobs[theBob->bobsNumber-1] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,theBob->passiveSize, theBob->activeHPSize, theBob->activeStatSize,passiveAbilities, activeHPMods, activeStatMods);
					
				
					printf("%s HAS INITIALIZED ",temp);
					printf("BOB #%d \n",theBob->bobsNumber);
					players[i].setBobs(bobs,theBob->bobsNumber);
					printf("BOBS ID: %d\n", players[i].getBobs()[theBob->bobsNumber-1].getID());
					printf("BOBS HP: %d\n", players[i].getBobs()[theBob->bobsNumber-1].getHP());

					numOfBobs.typeID = CLIENTS_BOBS_ID;
					numOfBobs.singleBobId = players[i].getBobs()[theBob->bobsNumber-1].getID();
					BobsServer->Send((char*)&numOfBobs,sizeof(numOfBobs),HIGH_PRIORITY,RELIABLE_ORDERED,0,p->playerId,false);
					
					for(int c = 0; c < theBob->passiveSize; c++)
						printf("BOBS PASS SIZE: %d\n", players[i].getBobs()[theBob->bobsNumber-1].getPassiveAbilities()[c].ability->getName());				
					
					for(int d = 0; d < theBob->activeHPSize; d++)
						printf("BOBS ACTIVE HP: %d\n",players[i].getBobs()[theBob->bobsNumber-1].getActiveHPMods()[d].ability->getName());				

					for(int d = 0; d < theBob->activeStatSize; d++)
						printf("BOBS ACTIVE STAT: %d\n",players[i].getBobs()[theBob->bobsNumber-1].getActiveStatMods()[d].ability->getName());				

					if(theBob->passiveSize > 0)	delete [] passiveAbilities;	
					if(theBob->activeHPSize > 0) delete [] activeHPMods;
					if(theBob->activeStatSize > 0) delete [] activeStatMods;	
				}
			}


			break;

		case READY:															//CLIENT FINISHED PURCHASING BOBS
			ready = ready + 1;				
			printf("players ready: %i \n", ready);
			printf("player ready: %s \n",((StaticClientDataStruct*)(BobsServer->GetStaticClientData(p->playerId))->GetData())->name);								
	
			TotalNumberOfBobs = TotalNumberOfBobs + theNumOfBobs->numOfBobs;
			printf("Total Bobs: %i \n", TotalNumberOfBobs);

			if (ready == atoi(ServerData.NumOfPlayers))
			{
				for (int x = 0; x < np; x++)
				{
					players[x].setAP((TotalNumberOfBobs / np) * 5);				//SET PLAYERS ACTION POINTS		
				}

		//		map.length = //TotalNumberOfBobs * 2;
		//		map.height = //TotalNumberOfBobs * 2;

				if (TotalNumberOfBobs *2 < 10)
				{			
					map.length = 10;
					map.height = 10;
				}
				else 
				{
					map.length = TotalNumberOfBobs *2;
					map.height = TotalNumberOfBobs *2;
				}
/*
				if ( map.length < 8 || map.height < 8 )
				{
					map.length = 8; map.height = 8;
				}
*/
				map.terrain = Grass;

		//		map1 = Map(TotalNumberOfBobs * 2,TotalNumberOfBobs * 2, Grass);
		//		map1 = Map(50, 50, Grass);
		//		printf("making map\n");

				if(mapChoice == 1)
					map1 = Map("default_5.map");
				else if(mapChoice == 2)
					map1 = Map("default_10.map");
				else if(mapChoice == 3)
					map1 = Map("default_50map");
				else if(mapChoice == 4)
					map1 = Map("Melee_Map_10.map");
				else if(mapChoice == 5)
		  			map1 = Map("Middle_Block_10.map");
				else if(mapChoice == 6)
					map1 = Map("Barracks_10.map");
				else if(mapChoice == 7)
					map1 = Map("Skull_15.map");
				else
					map1 = Map("Blank_10.map");

		//		printf("map made\n");
				map.length = map1.getLength();
				map.height = map1.getHeight();
		//		printf("values initialized\n");


				cout << "Initializing Game!" << endl;
				game.initialize(players, np, (TotalNumberOfBobs / np) * 5, map1);
				cout << "Game Initialized" << endl;

				delete [] bobs;
				delete [] players;

				puts("EVERYBODY IS READY");
				puts("SENDING MAP");

				for (int i = 0; i < map1.getHeight(); i++)
				{
					for(int j = 0; j < map1.getLength(); j++)
					{

						if((game.getMap()->getTile(i, j)->getOccupied()) == 1)
						{
							blockedTiles.blockedX = i;
							blockedTiles.blockedY = j;

							blockedListX.insert(i);
							blockedListY.insert(j);

							BobsServer->Send((char *)&blockedTiles, sizeof(blockedTiles), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
						}
					}
				}
				map.typeID = BROADCAST_MAP;
				BobsServer->Send((char*)&map,sizeof(map),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_PLAYER_ID,true);

			}
			break;

		case PLACE_BOB:														//CLIENT PLACING THEIR BOBS

			if(AIplaced == false)
			{
				for(int x = NumOfPlayers-numOfAIPlayers; x < NumOfPlayers; x++)
				{
					int usedx[10];
					int usedy[10];

					for(i=0; i<10; i++)
					{
						usedx[i] = -1;
						usedy[i] = -1;
					}

					int usex = 0;
					int usey = 0;

					bool occupied = false;

					int area = 4;
					int a = 0;
					int i;
					int j;
					int mapHeight = game.getMap()->getHeight();
					int mapLength = game.getMap()->getLength();

				
					a = game.getPlayers()[x].getNumBobs();

					area = 4;

					if(a > 4)
						area = a;	

					if (x == 0)
					{	
						for(i = 0; i< area && a != 0; i++)
						{
							for(j = 0; j< area && a != 0; j++)
							{
								occupied = false;
								for(int q = 0; q < blockedListX.size(); q++)
								{
									if(blockedListX[q] == i && blockedListY[q] == j)	
									{
										usedx[usex] = i;
										usex++;
										usedy[usey] = j;
										usey++;
										occupied = true;
									}
								}
								if(!occupied)
								{
									printf("AI %d IS PLACING BOB[%d]  TO LOCATION X:%d, Y:%d\n",x,game.getPlayers()[x].getNumBobs()-a,i, j);

									game.placeBob(&game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a], i, j);

									printf("PLACED AT  X: %d Y: %d\n", game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a].getX(), game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a].getY());
									a--;
											//	DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);
								}
							}
						}
					}
					else if(x == 1)
					{
						for(i = mapHeight - 1; i> mapHeight - (area+1) && a != 0; i--)
						{	
							for(j = mapLength - 1; j> mapLength - (area+1) && a != 0; j--)
							{
								occupied = false;
								for(int q = 0; q < blockedListX.size(); q++)
								{
									if(blockedListX[q] == i && blockedListY[q] == j)
									{
										usedx[usex] = i;
										usex++;
										usedy[usey] = j;
										usey++;
										occupied = true;
									}
								}
								if(!occupied)
								{
									printf("AI %d IS PLACING BOB[%d]  TO LOCATION X:%d, Y:%d\n",x,game.getPlayers()[x].getNumBobs()-a,i, j);

									game.placeBob(&game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a], i, j);

									printf("PLACED AT  X: %d Y: %d\n", game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a].getX(), game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a].getY());
									a--;
											//	DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);
								}
							}
						}
					}
					else if(x == 2)
					{
						for(i = mapLength - 1; i > mapLength - (area+1) && a != 0; i--)
						{	
							for(j = area-1; j>= 0 && a != 0; j--)
							{
								occupied = false;
								for(int q = 0; q < blockedListX.size(); q++)
								{
									if(blockedListX[q] == i && blockedListY[q] == j)
									{
										usedx[usex] = i;
										usex++;
										usedy[usey] = j;
										usey++;
										occupied = true;
									}
								}
								if(!occupied)
								{
									printf("AI %d IS PLACING BOB[%d]  TO LOCATION X:%d, Y:%d\n",x,game.getPlayers()[x].getNumBobs()-a,i, j);

									game.placeBob(&game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a], i, j);

									printf("PLACED AT  X: %d Y: %d\n", game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a].getX(), game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a].getY());
									a--;
											//	DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);
								}
							}
						}
					}
					else if(x==3)
					{
						for(i = 0; i<area && a != 0; i++)
						{	
							for(j = mapHeight - area; j<= mapHeight - 1 && a != 0; j++)
							{
								occupied = false;
								for(int q = 0; q < blockedListX.size(); q++)
								{
									if(blockedListX[q] == i && blockedListY[q] == j)
									{
										usedx[usex] = i;
										usex++;
										usedy[usey] = j;
										usey++;
										occupied = true;
									}
								}
								if(!occupied)
								{
									printf("AI %d IS PLACING BOB[%d]  TO LOCATION X:%d, Y:%d\n",x,game.getPlayers()[x].getNumBobs()-a,i, j);

									game.placeBob(&game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a], i, j);

									printf("PLACED AT  X: %d Y: %d\n", game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a].getX(), game.getPlayers()[x].getBobs()[game.getPlayers()[x].getNumBobs()-a].getY());
									a--;
											//	DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);
								}
							}
						}
					}
				}

			//	placeAIBobs(numOfAIPlayers, game, blockedListX, blockedListY);
				done += numOfAIPlayers;
				AIplaced = true;

				cout << "AI Bobs placed" << endl;
			}

			sprintf(temp, "%s",((StaticClientDataStruct*)(BobsServer->GetStaticClientData(p->playerId))->GetData())->name);

			for(int i = 0; i<p_joined; i++)
			{
		//		cout << "numBobs = " << game.getPlayers()[i].getNumBobs() << endl;
				sprintf(temp2,"%s", game.getPlayers()[i].getHandle().c_str());	

				if (strcmp (temp2,temp) == 0)
				{	
					printf("%s ",temp2); //game.getPlayers()[i].getHandle().c_str());	
					printf("IS PLACING BOB[%d]  TO LOCATION X:%d, Y:%d\n",thePosition->BobsID,thePosition->x, thePosition->y);	
					game.placeBob(&game.getPlayers()[i].getBobs()[thePosition->BobsID], thePosition->x, thePosition->y);
					printf("PLACED AT  X: %d Y: %d\n", game.getPlayers()[i].getBobs()[thePosition->BobsID].getX(), game.getPlayers()[i].getBobs()[thePosition->BobsID].getY());		
					if(game.getPlayers()[i].getBobs()[thePosition->BobsID].getX() == -1 || game.getPlayers()[i].getBobs()[thePosition->BobsID].getY() == -1)
					{
						//ERROR... RESTART EVERYTHING
						puts("SENDING MAP");
						BobsServer->Send((char*)&map,sizeof(map),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_PLAYER_ID,true);
					}
				}
			}
			break;

		case DONE_PLACING_BOBS:			//CLIENT HAS PLACED ALL ITS BOB
			done = done + 1;	
			printf("players done: %i \n", done);
			sprintf(temp, "---%s IS DONE PLACING BOBS---\n", ((StaticClientDataStruct*)(BobsServer->GetStaticClientData(p->playerId))->GetData())->name);
			BobsServer->Send(temp, strlen(temp)+1, LOW_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true);
			if (done == atoi(ServerData.NumOfPlayers))
			{
				looping = false;
				puts("EVERYBODY IS DONE PLACING BOBS");
				BobsServer->Send("Starting Game ", strlen("Starting Game ") + 1,HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
				printf("stuff sent\n");

			}	
			break;
		default:
			break;
		}	
		BobsServer->DeallocatePacket(p);
	}

	//Sleep(500);
	BobsServerGame(BobsServer, ServerData, game, map1);
	BobsServer->Disconnect(300);
	RakNetworkFactory::DestroyRakServerInterface(BobsServer);
}


Bob* buyAIBobs(int NumOfPlayers, int numOfAIPlayers, int AP, Player *players)
{
	Bob *bobs = new Bob[30];
	BobPassive *passiveAbilities;
	BobHPMod *activeHPMods;
	BobStatMod *activeStatMods;

	for(int i = NumOfPlayers-numOfAIPlayers; i<NumOfPlayers; i++)
	{

	//	if(players[i].getHandle() == temp)				
	//	{
			if(AP > 80)
			{

				passiveAbilities = new BobPassive[1];
				activeHPMods = new BobHPMod[3];
				activeStatMods = new BobStatMod[0];

				activeHPMods[0].ability = &DefaultMeleeAbility;
				activeHPMods[0].turnsLeft = 0;

				activeHPMods[1].ability = &MediumRangedAbility;
				activeHPMods[1].turnsLeft = 0;

				activeHPMods[2].ability = &LightLightningAOEAbility;
				activeHPMods[2].turnsLeft = 0;

				passiveAbilities[0].ability = &PassiveLightHPIncreaseAbility;
				passiveAbilities[0].turnsLeft = 0;

				bobs[0] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 3, 0,passiveAbilities, activeHPMods, activeStatMods);



				passiveAbilities = new BobPassive[1];
				activeHPMods = new BobHPMod[3];
				activeStatMods = new BobStatMod[0];

				activeHPMods[0].ability = &DefaultMeleeAbility;
				activeHPMods[0].turnsLeft = 0;

				activeHPMods[1].ability = &MediumRangedAbility;
				activeHPMods[1].turnsLeft = 0;

				activeHPMods[2].ability = &MediumHealAbility;
				activeHPMods[2].turnsLeft = 0;

				passiveAbilities[0].ability = &PassiveLightHPIncreaseAbility;
				passiveAbilities[0].turnsLeft = 0;

				bobs[1] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 3, 0,passiveAbilities, activeHPMods, activeStatMods);



				passiveAbilities = new BobPassive[1];
				activeHPMods = new BobHPMod[4];
				activeStatMods = new BobStatMod[0];

				activeHPMods[0].ability = &DefaultMeleeAbility;
				activeHPMods[0].turnsLeft = 0;

				activeHPMods[1].ability = &MediumRangedAbility;
				activeHPMods[1].turnsLeft = 0;

				activeHPMods[2].ability = &LightLightningAOEAbility;
				activeHPMods[2].turnsLeft = 0;

				activeHPMods[3].ability = &MediumHealAbility;
				activeHPMods[3].turnsLeft = 0;

				passiveAbilities[0].ability = &PassiveLightDodgeAbility;
				passiveAbilities[0].turnsLeft = 0;

				bobs[2] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 4, 0,passiveAbilities, activeHPMods, activeStatMods);



				passiveAbilities = new BobPassive[1];
				activeHPMods = new BobHPMod[4];
				activeStatMods = new BobStatMod[0];

				activeHPMods[0].ability = &DefaultMeleeAbility;
				activeHPMods[0].turnsLeft = 0;

				activeHPMods[1].ability = &MediumRangedAbility;
				activeHPMods[1].turnsLeft = 0;

				activeHPMods[2].ability = &LightLightningAOEAbility;
				activeHPMods[2].turnsLeft = 0;

				passiveAbilities[0].ability = &PassiveLightArmorAbility;
				passiveAbilities[0].turnsLeft = 0;

				passiveAbilities[1].ability = &PassiveLightDodgeAbility;
				passiveAbilities[1].turnsLeft = 0;

				bobs[3] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,2, 3, 0,passiveAbilities, activeHPMods, activeStatMods);


				players[i].setBobs(bobs,4);

			}
			else if(AP > 70)
			{
				passiveAbilities = new BobPassive[1];
				activeHPMods = new BobHPMod[3];
				activeStatMods = new BobStatMod[0];

				activeHPMods[0].ability = &DefaultMeleeAbility;
				activeHPMods[0].turnsLeft = 0;

				activeHPMods[1].ability = &MediumRangedAbility;
				activeHPMods[1].turnsLeft = 0;

				activeHPMods[2].ability = &LightLightningAOEAbility;
				activeHPMods[2].turnsLeft = 0;

				passiveAbilities[0].ability = &PassiveLightHPIncreaseAbility;
				passiveAbilities[0].turnsLeft = 0;

				bobs[0] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 3, 0,passiveAbilities, activeHPMods, activeStatMods);


						
				passiveAbilities = new BobPassive[1];
				activeHPMods = new BobHPMod[3];
				activeStatMods = new BobStatMod[0];

				activeHPMods[0].ability = &DefaultMeleeAbility;
				activeHPMods[0].turnsLeft = 0;

				activeHPMods[1].ability = &MediumRangedAbility;
				activeHPMods[1].turnsLeft = 0;

				activeHPMods[2].ability = &MediumHealAbility;
				activeHPMods[2].turnsLeft = 0;

				passiveAbilities[0].ability = &PassiveLightHPIncreaseAbility;
				passiveAbilities[0].turnsLeft = 0;

				bobs[1] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 3, 0,passiveAbilities, activeHPMods, activeStatMods);


							
				passiveAbilities = new BobPassive[1];
				activeHPMods = new BobHPMod[4];
				activeStatMods = new BobStatMod[0];

				activeHPMods[0].ability = &DefaultMeleeAbility;
				activeHPMods[0].turnsLeft = 0;

				activeHPMods[1].ability = &MediumRangedAbility;
				activeHPMods[1].turnsLeft = 0;

				activeHPMods[2].ability = &LightLightningAOEAbility;
				activeHPMods[2].turnsLeft = 0;

				activeHPMods[3].ability = &MediumHealAbility;
				activeHPMods[3].turnsLeft = 0;

				passiveAbilities[0].ability = &PassiveLightDodgeAbility;
				passiveAbilities[0].turnsLeft = 0;

				bobs[2] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 4, 0,passiveAbilities, activeHPMods, activeStatMods);


						
				passiveAbilities = new BobPassive[1];
				activeHPMods = new BobHPMod[4];
				activeStatMods = new BobStatMod[0];

				activeHPMods[0].ability = &DefaultMeleeAbility;
				activeHPMods[0].turnsLeft = 0;

				activeHPMods[1].ability = &MediumRangedAbility;
				activeHPMods[1].turnsLeft = 0;

				activeHPMods[2].ability = &LightLightningAOEAbility;
				activeHPMods[2].turnsLeft = 0;

				passiveAbilities[0].ability = &PassiveLightDodgeAbility;
				passiveAbilities[0].turnsLeft = 0;

				bobs[3] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 3, 0,passiveAbilities, activeHPMods, activeStatMods);


				players[i].setBobs(bobs,4);

			}
			else if(AP > 60)
			{
						passiveAbilities = new BobPassive[1];
						activeHPMods = new BobHPMod[3];
						activeStatMods = new BobStatMod[0];

						activeHPMods[0].ability = &DefaultMeleeAbility;
						activeHPMods[0].turnsLeft = 0;

						activeHPMods[1].ability = &MediumRangedAbility;
						activeHPMods[1].turnsLeft = 0;

						activeHPMods[2].ability = &LightLightningAOEAbility;
						activeHPMods[2].turnsLeft = 0;

						passiveAbilities[0].ability = &PassiveLightHPIncreaseAbility;
						passiveAbilities[0].turnsLeft = 0;

						bobs[0] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 3, 0,passiveAbilities, activeHPMods, activeStatMods);




						passiveAbilities = new BobPassive[1];
						activeHPMods = new BobHPMod[4];
						activeStatMods = new BobStatMod[0];

						activeHPMods[0].ability = &DefaultMeleeAbility;
						activeHPMods[0].turnsLeft = 0;

						activeHPMods[1].ability = &MediumRangedAbility;
						activeHPMods[1].turnsLeft = 0;

						activeHPMods[2].ability = &LightLightningAOEAbility;
						activeHPMods[2].turnsLeft = 0;

						activeHPMods[3].ability = &MediumHealAbility;
						activeHPMods[3].turnsLeft = 0;

						passiveAbilities[0].ability = &PassiveLightDodgeAbility;
						passiveAbilities[0].turnsLeft = 0;

						bobs[1] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 4, 0,passiveAbilities, activeHPMods, activeStatMods);



						passiveAbilities = new BobPassive[1];
						activeHPMods = new BobHPMod[4];
						activeStatMods = new BobStatMod[0];

						activeHPMods[0].ability = &DefaultMeleeAbility;
						activeHPMods[0].turnsLeft = 0;

						activeHPMods[1].ability = &MediumRangedAbility;
						activeHPMods[1].turnsLeft = 0;

						activeHPMods[2].ability = &LightLightningAOEAbility;
						activeHPMods[2].turnsLeft = 0;

						passiveAbilities[0].ability = &PassiveLightArmorAbility;
						passiveAbilities[0].turnsLeft = 0;

						passiveAbilities[1].ability = &PassiveLightDodgeAbility;
						passiveAbilities[1].turnsLeft = 0;

						bobs[2] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,2, 3, 0,passiveAbilities, activeHPMods, activeStatMods);


						players[i].setBobs(bobs,3);

			}
					else if(AP > 50)
					{
						passiveAbilities = new BobPassive[1];
						activeHPMods = new BobHPMod[3];
						activeStatMods = new BobStatMod[0];

						activeHPMods[0].ability = &DefaultMeleeAbility;
						activeHPMods[0].turnsLeft = 0;

						activeHPMods[1].ability = &MediumRangedAbility;
						activeHPMods[1].turnsLeft = 0;

						activeHPMods[2].ability = &LightLightningAOEAbility;
						activeHPMods[2].turnsLeft = 0;

						passiveAbilities[0].ability = &PassiveLightHPIncreaseAbility;
						passiveAbilities[0].turnsLeft = 0;

						bobs[0] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 3, 0,passiveAbilities, activeHPMods, activeStatMods);




						passiveAbilities = new BobPassive[1];
						activeHPMods = new BobHPMod[3];
						activeStatMods = new BobStatMod[0];

						activeHPMods[0].ability = &DefaultMeleeAbility;
						activeHPMods[0].turnsLeft = 0;

						activeHPMods[1].ability = &MediumRangedAbility;
						activeHPMods[1].turnsLeft = 0;

						activeHPMods[2].ability = &MediumHealAbility;
						activeHPMods[2].turnsLeft = 0;

						passiveAbilities[0].ability = &PassiveLightHPIncreaseAbility;
						passiveAbilities[0].turnsLeft = 0;

						bobs[1] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 3, 0,passiveAbilities, activeHPMods, activeStatMods);



						passiveAbilities = new BobPassive[1];
						activeHPMods = new BobHPMod[4];
						activeStatMods = new BobStatMod[0];

						activeHPMods[0].ability = &DefaultMeleeAbility;
						activeHPMods[0].turnsLeft = 0;

						activeHPMods[1].ability = &MediumRangedAbility;
						activeHPMods[1].turnsLeft = 0;

						activeHPMods[2].ability = &LightLightningAOEAbility;
						activeHPMods[2].turnsLeft = 0;

						passiveAbilities[0].ability = &PassiveLightArmorAbility;
						passiveAbilities[0].turnsLeft = 0;

						passiveAbilities[1].ability = &PassiveLightDodgeAbility;
						passiveAbilities[1].turnsLeft = 0;

						bobs[2] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,2, 3, 0,passiveAbilities, activeHPMods, activeStatMods);


						players[i].setBobs(bobs,3);
					}
					else if(AP > 40)
					{
						passiveAbilities = new BobPassive[1];
						activeHPMods = new BobHPMod[4];
						activeStatMods = new BobStatMod[0];

						activeHPMods[0].ability = &DefaultMeleeAbility;
						activeHPMods[0].turnsLeft = 0;

						activeHPMods[1].ability = &MediumRangedAbility;
						activeHPMods[1].turnsLeft = 0;

						activeHPMods[2].ability = &LightLightningAOEAbility;
						activeHPMods[2].turnsLeft = 0;

						activeHPMods[3].ability = &MediumHealAbility;
						activeHPMods[3].turnsLeft = 0;

						passiveAbilities[0].ability = &PassiveLightDodgeAbility;
						passiveAbilities[0].turnsLeft = 0;

						bobs[0] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 4, 0,passiveAbilities, activeHPMods, activeStatMods);



						passiveAbilities = new BobPassive[1];
						activeHPMods = new BobHPMod[4];
						activeStatMods = new BobStatMod[0];

						activeHPMods[0].ability = &DefaultMeleeAbility;
						activeHPMods[0].turnsLeft = 0;

						activeHPMods[1].ability = &MediumRangedAbility;
						activeHPMods[1].turnsLeft = 0;

						activeHPMods[2].ability = &LightLightningAOEAbility;
						activeHPMods[2].turnsLeft = 0;

						passiveAbilities[0].ability = &PassiveLightArmorAbility;
						passiveAbilities[0].turnsLeft = 0;

						passiveAbilities[1].ability = &PassiveLightDodgeAbility;
						passiveAbilities[1].turnsLeft = 0;

						bobs[1] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,2, 3, 0,passiveAbilities, activeHPMods, activeStatMods);

						players[i].setBobs(bobs,2);
					}
					else if(AP > 30)
					{
						passiveAbilities = new BobPassive[1];
						activeHPMods = new BobHPMod[3];
						activeStatMods = new BobStatMod[0];

						activeHPMods[0].ability = &DefaultMeleeAbility;
						activeHPMods[0].turnsLeft = 0;

						activeHPMods[1].ability = &MediumRangedAbility;
						activeHPMods[1].turnsLeft = 0;

						activeHPMods[2].ability = &LightLightningAOEAbility;
						activeHPMods[2].turnsLeft = 0;

						passiveAbilities[0].ability = &PassiveLightHPIncreaseAbility;
						passiveAbilities[0].turnsLeft = 0;

						bobs[0] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 3, 0,passiveAbilities, activeHPMods, activeStatMods);




						passiveAbilities = new BobPassive[1];
						activeHPMods = new BobHPMod[4];
						activeStatMods = new BobStatMod[0];

						activeHPMods[0].ability = &DefaultMeleeAbility;
						activeHPMods[0].turnsLeft = 0;

						activeHPMods[1].ability = &MediumRangedAbility;
						activeHPMods[1].turnsLeft = 0;

						activeHPMods[2].ability = &LightLightningAOEAbility;
						activeHPMods[2].turnsLeft = 0;

						activeHPMods[3].ability = &MediumHealAbility;
						activeHPMods[3].turnsLeft = 0;

						passiveAbilities[0].ability = &PassiveLightDodgeAbility;
						passiveAbilities[0].turnsLeft = 0;

						bobs[1] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,1, 4, 0,passiveAbilities, activeHPMods, activeStatMods);

						players[i].setBobs(bobs,2);
					}
			else
			{
				passiveAbilities = new BobPassive[0];
				activeHPMods = new BobHPMod[3];
				activeStatMods = new BobStatMod[0];

				activeHPMods[0].ability = &DefaultMeleeAbility;
				activeHPMods[0].turnsLeft = 0;

				activeHPMods[1].ability = &MediumRangedAbility;
				activeHPMods[1].turnsLeft = 0;

				activeHPMods[2].ability = &LightLightningAOEAbility;
				activeHPMods[2].turnsLeft = 0;

				bobs[0] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,0, 3, 0,passiveAbilities, activeHPMods, activeStatMods);


				passiveAbilities = new BobPassive[0];
				activeHPMods = new BobHPMod[3];
				activeStatMods = new BobStatMod[0];

				activeHPMods[0].ability = &DefaultMeleeAbility;
				activeHPMods[0].turnsLeft = 0;

				activeHPMods[1].ability = &MediumRangedAbility;
				activeHPMods[1].turnsLeft = 0;

				activeHPMods[2].ability = &MediumHealAbility;
				activeHPMods[2].turnsLeft = 0;

				bobs[1] = Bob(players[i].getHandle(), 100, 10, 80, 0, 1, 0 ,0, 3, 0,passiveAbilities, activeHPMods, activeStatMods);

				players[i].setBobs(bobs,2);

			}
					
			/*	
			printf("%s HAS INITIALIZED ",temp);
			printf("BOB #%d \n",theBob->bobsNumber);
			players[i].setBobs(bobs,theBob->bobsNumber);
			printf("BOBS ID: %d\n", players[i].getBobs()[theBob->bobsNumber-1].getID());
			printf("BOBS HP: %d\n", players[i].getBobs()[theBob->bobsNumber-1].getHP());

			numOfBobs.typeID = CLIENTS_BOBS_ID;
			numOfBobs.singleBobId = players[i].getBobs()[theBob->bobsNumber-1].getID();
			BobsServer->Send((char*)&numOfBobs,sizeof(numOfBobs),HIGH_PRIORITY,RELIABLE_ORDERED,0,p->playerId,false);
					
			for(int c = 0; c < theBob->passiveSize; c++)
				printf("BOBS PASS SIZE: %d\n", players[i].getBobs()[theBob->bobsNumber-1].getPassiveAbilities()[c].ability->getName());				
					
			for(int d = 0; d < theBob->activeHPSize; d++)
				printf("BOBS ACTIVE HP: %d\n",players[i].getBobs()[theBob->bobsNumber-1].getActiveHPMods()[d].ability->getName());				

			for(int d = 0; d < theBob->activeStatSize; d++)
				printf("BOBS ACTIVE STAT: %d\n",players[i].getBobs()[theBob->bobsNumber-1].getActiveStatMods()[d].ability->getName());				
*/
				delete [] passiveAbilities;	

				delete [] activeHPMods;

				delete [] activeStatMods;	
		//}
	}

	return bobs;
}
