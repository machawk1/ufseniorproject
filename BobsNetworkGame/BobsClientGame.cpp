#include "BobsNetwork.h"
//#define CLIENT_PORT  8001
//#define SERVER_PORT  10000
//#define CLIENT_PORT2 8002
//#define CLIENT_PORT3 8003
//#define CLIENT_PORT4 8004

unsigned char GetPacketIdentifier(Packet *p);



using std::string;
using BasicDataStructures::List;

void DrawMap(int mapHeight, int mapLength, List<char> bobplayer, List<int> bobID, List<int> bobx, List<int> boby, List<int> bobhp, List<int> bobap, List<int> avx, List<int> avy, List<int> avcost, List<int> blockedX, List<int> blockedY);


void BobsClientGame(RakClientInterface *BobsClient, StaticClientDataStruct ClientData, int mapLength, int mapHeight)
{		
	int i;
	int j = 0;

	//CHECK USERS INPUT
	bool validx = false;
	bool validy = false;
	bool cancel = false;

	//INDEX OF BOBS, AVAILABLE TILES
	int bobstat1 = 0;
	int avtemp1 = 0;

	//USERS INPUT
	int actx;
	int acty;
	int Act;
	int pickbobID;


	List<char> bobplayer1;
	List<int> bobID1;
	List<int> bobx1;
	List<int> boby1;
	List<int> bobhp1;
	List<int> bobap1;
	List<int> avx1;
	List<int> avy1;
	List<int> avcost1;

	List<int> blockedX;
	List<int> blockedY;

	bool loop;
	char temp[50];
	
	//STRUCT TO SEND TO BOBSSERVERGAME
	Flag flag;		
	RequestStruct requestStruct;
	requestStruct.typeID = REQUEST_STRUCT;
	ActualTarget actualTarget;
	actualTarget.typeID = ACTION_TARGET;


	puts("FIGHT!!!");
	bool loop2 = true;
	while (loop2)
	{	
		Sleep(30);												//KEEP RACKNET RESPONSIVE

		Packet* p;
		unsigned char packetIdentifier;		

		p = BobsClient->Receive();								//RECIEVE PACKET
		if (p==0) 
			continue;										//PACKET EMPTY

		packetIdentifier = GetPacketIdentifier(p);				//GET THE PACKET IDENTIFIER



		WhosTurn *theTurn = (WhosTurn*)p->data;
		NetworkTargetOption *theOption = (NetworkTargetOption*)p->data;
		ShellMap *theShell = (ShellMap*)p->data;				//RECEIVE EVERYBODYS POSITION
		Flag *theFlag = (Flag*)p->data;							//RECEIVE BOBS POSITION
		BlockedTiles *theBlocked = (BlockedTiles*)p->data;
		switch (packetIdentifier)
		{
			case ID_DISCONNECTION_NOTIFICATION:					//PLAYER DISCONNECTED
				printf("(%d) DISCONNECTED : \n", p->playerId);
				break;

			case BLOCKED_TILES:
				blockedX.insert(theBlocked->blockedX);
				blockedY.insert(theBlocked->blockedY);
				break;

			//RECEIVE WHOS BOBS ARE LOCATED WHERE
			case SHELL_MAP:		

				bobplayer1.insert(theShell->player[0]);
				bobID1.insert(theShell->bob);
				bobx1.insert(theShell->x);
				boby1.insert(theShell->y);
				bobap1.insert(theShell->AP);
				bobhp1.insert(theShell->HP);
				break;

			//UPDATE MAP
			case MAP_DISTRIBUTED:
				printf("Map Distributed");
				avx1.clear();
				avy1.clear();
				avcost1.clear();		
				DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);

				bobplayer1.clear();
				bobID1.clear();
				bobx1.clear();
				boby1.clear();
				bobap1.clear();
				bobhp1.clear();
		
				blockedX.clear();
				blockedY.clear();

				break;

			//PLAY OR WAIT FOR TURN
			case WHOS_TURN:
				printf("IT IS %s'S TURN!\n",theTurn->player);

				if(strcmp(ClientData.name,theTurn->player) == 0)
				{
						
						//	printf("FIRST BOBID: %d\n", ClientData.validBobIDs[0]);
						//	printf("AP: %d\n",theTurn->AP);
					printf("BOBS: %d\n",theTurn->bob);
					loop = true;

					while(loop)
					{
						printf("SELECT BOB ID TO ACT (OR TYPE -1 TO PASS THE TURN): ");
						gets(temp);
						pickbobID = atoi(temp);
						for (i = 0; i <= 10; i++)
						{					
							if (ClientData.validBobIDs[i] == pickbobID || pickbobID == -1)
							{
								loop = false;
								break;
							}
						}		
					}

							//IMPLEMENT CHECK FOR CORRECT BOB ID
						if(pickbobID == -1)
						{
							Act = 12;
							requestStruct.bobID = pickbobID;
							requestStruct.ability = (AbilityNameType)Act;
							BobsClient->Send((char*)&requestStruct,sizeof(requestStruct),HIGH_PRIORITY,RELIABLE_ORDERED,0);
						}
						else
						{
							requestStruct.bobID = pickbobID; 
							printf("(c)-Cancel (1)Move (2)Melee (3)Ranged (4)AOE (5)Heal");
							printf("Action #: ");
							gets(temp);
							Act = atoi(temp);
									
							requestStruct.ability = (AbilityNameType)Act;
							BobsClient->Send((char*)&requestStruct,sizeof(requestStruct),HIGH_PRIORITY,RELIABLE_ORDERED,0);
						}

				}	
				else
				{
					puts("OTHER PLAYER IS MOVING...\n");

					bobplayer1.clear();
					bobID1.clear();
					bobx1.clear();
					boby1.clear();
					bobap1.clear();
					bobhp1.clear();

					avx1.clear();
					avy1.clear();
					avcost1.clear();

					blockedX.clear();
					blockedY.clear();
					Sleep(50);
				}

				break;

			//CURRENT PLAYER GETS AVAILABLE TILES TO ACT
			case TARGET_TILES:
	
				actualTarget.bobID = requestStruct.bobID;
				actualTarget.ability = requestStruct.ability;
				if(theOption->x == -1 || theOption->y == -1)
				{
					printf("NO TARGETS AVAILABLE\n");

						bobplayer1.clear();
						bobID1.clear();
						bobx1.clear();
						boby1.clear();
						bobap1.clear();
						bobhp1.clear();

						avx1.clear();
						avy1.clear();
						avcost1.clear();		

						blockedX.clear();
						blockedY.clear();

					flag.typeID = REFRESH;
					BobsClient->Send((char*)&flag,sizeof(flag),HIGH_PRIORITY,RELIABLE_ORDERED,0);
					break;
				}
				else
				{
					avx1.insert(theOption->x);
					avy1.insert(theOption->y);
					avcost1.insert(theOption->cost);
				}
				break;
			
			//DONE RECIEVING AVAILABLE TARGET TILES
			case DONE_TARGET_TILES:
				j = 0;
				printf("RECEIVING AVAILABLE TARGETS!");


				DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);
				cancel = false;
				validx = false;
				validy = false;
				while(!validx && !validy)
				{
					printf("SELECT TARGET X or cancel(-1): ");
					gets(temp);
					actx = atoi(temp);

					printf("SELECT TARGET Y or cancel(-1): ");
					gets(temp);
					acty = atoi(temp);

					if (acty >= mapLength || actx >= mapHeight)
					{
						continue;
					}

					validx = false;
					validy = false;
					cancel = false;

					if(actx == -1 || acty == -1)
					{
						actualTarget.ability = Cancel;
						BobsClient->Send((char*)&actualTarget,sizeof(actualTarget),HIGH_PRIORITY,RELIABLE_ORDERED,0);
						cancel = true;
						validx = true;
						break; 
					}
					else
					{
						for(int u = 0; u < avx1.size(); u++)
						{
							if (actx == avx1[u] && acty == avy1[u])
							{
								validx = true;
								validy = true;
							}

						//	if (acty == avy1[u])
						//		validy = true;
						}
					}
				}
				
				if (acty == -1 || actx == -1 || actx > mapLength || acty > mapHeight)
				{

					bobplayer1.clear();
					bobID1.clear();
					bobx1.clear();
					boby1.clear();
					bobap1.clear();
					bobhp1.clear();

					avx1.clear();
					avy1.clear();
					avcost1.clear();	

					blockedX.clear();
					blockedY.clear();
					break;
				}
				if (cancel == false)
				{
					actualTarget.y = acty;
					actualTarget.x = actx;				
					BobsClient->Send((char*)&actualTarget,sizeof(actualTarget),HIGH_PRIORITY,RELIABLE_ORDERED,0);
					printf("Bob: %d - Ability: %d\n", actualTarget.bobID, (int)actualTarget.ability);
				}

				bobplayer1.clear();
				bobID1.clear();
				bobx1.clear();
				boby1.clear();
				bobap1.clear();
				bobhp1.clear();

				avx1.clear();
				avy1.clear();
				avcost1.clear();	

				blockedX.clear();
				blockedY.clear();
				break;

			default:
				break;
		}	
		BobsClient->DeallocatePacket(p); 
		
	}
}

