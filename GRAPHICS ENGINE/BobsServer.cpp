#include "BobsNetwork.h"
#define SERVER_PORT 9000

unsigned char GetPacketIdentifier(Packet *p);		//IDENTIFY PACKET FUNCTION
void BobsServerGame(RakServerInterface *BobsServer, StaticServerDataStruct ServerData, GameState game, Map map1);

void BobsServer()
{
	//variables
	char temp[512];															
	char temp2[512];
	int ready = 0;																
	int done = 0;																

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
			}
			else
				clientAP.AP = atoi(temp);

			if(clientAP.AP >= 10 && clientAP.AP <= 90)
			{
				break;
			}
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
			sprintf(temp, "%s HAS JOINED\n", ((StaticClientDataStruct*)(BobsServer->GetStaticClientData(p->playerId))->GetData())->name);
			BobsServer->Send(temp, strlen(temp)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->playerId, true);
			sprintf(temp, "%s",((StaticClientDataStruct*)(BobsServer->GetStaticClientData(p->playerId))->GetData())->name);		
			players[p_joined]=Player(temp, Human);						//INITIALIZE PLAYER
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
				game.initialize(players, np, (TotalNumberOfBobs / np) * 5, map1);
				
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
							BobsServer->Send((char *)&blockedTiles, sizeof(blockedTiles), HIGH_PRIORITY,RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, TRUE);
						}
					}
				}
				map.typeID = BROADCAST_MAP;
				BobsServer->Send((char*)&map,sizeof(map),HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_PLAYER_ID,true);

			}
			break;

		case PLACE_BOB:														//CLIENT PLACING THEIR BOBS
			sprintf(temp, "%s",((StaticClientDataStruct*)(BobsServer->GetStaticClientData(p->playerId))->GetData())->name);
			for(int i = 0; i<p_joined; i++)
			{
				sprintf(temp2,"%s",game.getPlayers()[i].getHandle().c_str());	
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

		case DONE_PLACING_BOBS:												//CLIENT HAS PLACED ALL ITS BOB
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

