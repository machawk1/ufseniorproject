#include "BobsNetwork.h"

#define CLIENT_PORT  9001
#define SERVER_PORT  9000
#define CLIENT_PORT2 9002
#define CLIENT_PORT3 9003
#define CLIENT_PORT4 9004



using std::string;
using BasicDataStructures::List;


void DrawMap(int mapHeight, int mapLength, List<char> bobplayer, List<int> bobID, List<int> bobx, List<int> boby, List<int> bobhp, List<int> bobap, List<int> avx, List<int> avy, List<int> avcost, List<int> blockedX, List<int> blockedY);
void BobsClientGame(RakClientInterface *BobsClient, StaticClientDataStruct ClientData,int mapLength, int mapHeight);
void DrawStore();
unsigned char GetPacketIdentifier(Packet *p);

void BobsClient()
{
	/************************/
	/* Variable Declaration
	/***********************/
	char temp[512];												
	char temp2[512];
	char ip[20];
	int AP=0;				
	int i, j;
	int mapHeight;
	int mapLength;
	int x,y;
	int av = 0;
	int usedx[10];
	int usedy[10];
	int usex = 0;
	int usey = 0;
	int numInput;	
	int nbid = 0;
	bool validinput = true;
	bool SetUpLooping = true;	

	/************************/
	/* Max Store Items
	/***********************/
	bool maxhp = true;
	int maxh = 3;
	bool maxlight = true;
	int maxl = 4;
	bool maxattack = true;
	int maxa = 5;

	/************************/
	/* Identify Packet 
	/***********************/
	unsigned char packetIdentifier;							
	Packet* p;		

	/************************/
	/* ArrayList Declaration
	/***********************/
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

	/*************************/
	/* Structures Declaration
	/************************/
	BobsStruct bobsStruct;										
	bobsStruct.typeID = INITIALIZED_BOB;
	bobsStruct.apBase = 1;
	bobsStruct.passiveSize = 0;
	bobsStruct.activeHPSize = 0;
	bobsStruct.activeStatSize = 0;
	bobsStruct.bobsNumber = 0;
		
	Flag flag;													

	NumberOfBobs numOfBobs;							

	BobsPosition bobsPosition;								
	bobsPosition.typeID = PLACE_BOB;

	StaticClientDataStruct ClientData;
				
	// Map map1= Map();

/****************************************************************************
						CONNECTING TO SERVER
*****************************************************************************/
	printf("-----BOBSCLIENT-----\n");	
	RakClientInterface *BobsClient=RakNetworkFactory::GetRakClientInterface();//RAKNET: CLIENTS INTERFACE

	/***************************************************************************/
	/* Get clients player handle
	/***************************************************************************/
	while(true)
	{
		printf("Players Name: ");								//CLIENTS NAME
		gets(ClientData.name);

		if(strlen(ClientData.name) <= 0)
			puts("Please enter a player name");
		else
		{
			printf("Your player name is %s\n",ClientData.name);
			break;
		}
	}
	BobsClient->SetStaticClientData(UNASSIGNED_PLAYER_ID, (char*)&ClientData, sizeof(StaticClientDataStruct));

	/***************************************************************************/
	/* Get Servers IP Address
	/***************************************************************************/
	while(true)
	{
		printf("Enter HOST IP: ");									//HOSTS IP ADDRESS
		gets(ip);

		if (ip[0]==0)
		{
			strcpy(ip, "127.0.01");
			puts("-----CONNECTING TO LOCAL COMPUTER-----");
		}	

		/***************************************************************************/
		/* Get Servers PORT Number
		/***************************************************************************/
		printf("PORT: (1)(2)(3)(4): ");
		gets(temp);
		numInput = atoi(temp);

		/***************************************************************************/
		/* Connect To Server
		/***************************************************************************/
		if(numInput == 1)
		{
			if (!(BobsClient->Connect(ip, SERVER_PORT, CLIENT_PORT, 0, 30)))
				continue;		
			break;
		}
		if(numInput == 2)
		{
			if (!(BobsClient->Connect(ip, SERVER_PORT, CLIENT_PORT2, 0, 30)))
				continue;		
			break;
		}
		if(numInput == 3)
		{
			if(!(BobsClient->Connect(ip, SERVER_PORT, CLIENT_PORT3, 0, 30)))
				continue;		
			break;
		}
		if(numInput == 4)
		{
			if(!(BobsClient->Connect(ip, SERVER_PORT, CLIENT_PORT4, 0, 30)))
				continue;		
			break;
		}
		puts("Bad connection attempt, try again");
		continue; 	  
	}		

	printf("-----CONNECTING  TO  BOBSSERVER-----\n"); 
	while (SetUpLooping)
	{
		Sleep(30);
		/****************************/
		/* Receive Packet
		/****************************/
		p = BobsClient->Receive();								
		if (p==0) continue;										
		packetIdentifier = GetPacketIdentifier(p);

		/***************************************************************************/
		/* Pointers to Received Structures
		/***************************************************************************/		
		ClientsAP* theAP = (ClientsAP *)p->data;				//POINTS TO RECIEVED STRUCTURE(AP)
		ServerMap* theMap = (ServerMap*)p->data;				//RECEIVE BOBS POSITION
		NumberOfBobs* theBobID = (NumberOfBobs *)p->data;					
		BlockedTiles *theBlocked = (BlockedTiles*)p->data;

		/****************************/
		/* Identify Packet
		/****************************/
		bool occupied = false;	
		bool upgradingBob;
		bool purchase;
		switch (packetIdentifier)
		{
			/***************************************************************************/
			/* Player Disconnected
			/***************************************************************************/
			case ID_DISCONNECTION_NOTIFICATION:					//PLAYER DISCONNECTED
				printf("(%d) DISCONNECTED : \n", p->playerId);
				break;

			/***************************************************************************/
			/* Player Connecting
			/***************************************************************************/
			case ID_NEW_INCOMING_CONNECTION:					//NEW PLAYER CONNECTED
				printf("%s HAS ENTER ROOM",((StaticClientDataStruct*)(BobsClient->GetStaticClientData(p->playerId))->GetData())->name);
				break;
			
			/************************************************************/
			/*First real packet sent to clients, gives them their AP
			/* and then has them create their team. When finished sends
			/* clients team data to server for intialization
			/************************************************************/

			case RETRIEVE_AP:									//RECIEVED ACTION POINTS
				AP=theAP->AP;
				ClientData.myplayerid = theAP->myplayerid;
				purchase = true;

			/****************************************************************/
			/* Bobs Store
			/****************************************************************/
				while(purchase)
				{
					printf("AP: %d\nBOBS:%d\n",AP, bobsStruct.bobsNumber);
					printf("BUY BOB?||Cost: 10AP (y)/(n): "); gets(temp);		
					if ( AP < 10)
					{
						printf("NOT ENOUGH MONEY TO BUY BOB\n");
						break;
					}
					if (strcmp("y",temp) == 0)
					{

						AP = AP - 10;
						upgradingBob = true;
						
						bobsStruct.activeHPMods[bobsStruct.activeHPSize] = DefaultMelee;
						bobsStruct.activeHPSize++;

						while (upgradingBob)
						{

							if (AP <= 0)
							 {
								    //
									puts("OUT OF ACTION POINTS");
									puts("CAN'T PURCHASE ANYMORE UPGRADES");
									bobsStruct.bobsNumber = bobsStruct.bobsNumber + 1;	
									BobsClient->Send((char*)&bobsStruct,sizeof(bobsStruct),HIGH_PRIORITY,RELIABLE_ORDERED,0);
									bobsStruct.apBase = 10;
									bobsStruct.passiveSize = 0;
									bobsStruct.activeHPSize = 0;
									bobsStruct.activeStatSize = 0;
									upgradingBob = false;
									purchase = false;

									break;
							 }
							printf("AP: %d\nBOBS: %d\n",AP, bobsStruct.bobsNumber);
							DrawStore();

							printf("Enter Ability #: ");	
							gets(temp);
							numInput = atoi(temp);
							switch(numInput)
							{

							case 0:							
								bobsStruct.bobsNumber = bobsStruct.bobsNumber + 1;
								BobsClient->Send((char*)&bobsStruct,sizeof(bobsStruct),HIGH_PRIORITY,RELIABLE_ORDERED,0);
								bobsStruct.apBase = 10;
								bobsStruct.passiveSize = 0;
								bobsStruct.activeHPSize = 0;
								bobsStruct.activeStatSize = 0;

								maxhp = true;
								maxh = 3;
								maxlight = true;
								maxl = 4;
								maxattack = true;
								maxa = 5;

								if (AP < 10)
								{	
									printf("NOT ENOUGH AP TO BUY BOB\n");
									upgradingBob = false;
									purchase = false;
								}
								else
									upgradingBob = false;
									
								break;
							
							case 1:
									if(maxhp == false)
									{
										printf("YOU HAVE 3 HPINCREASE, can't buy more\n");
									}
									else
									{
										bobsStruct.passiveAbilities[bobsStruct.passiveSize] = PassiveLightHPIncrease;
										bobsStruct.passiveSize++;
										AP = AP - 1;
									}
									maxh--;
									maxl--;
									if (maxh <= 0)
									{
										maxhp = false;
									}
									if(maxl <= 0)
									{
										maxlight = false;
									}

								break;

							case 2:
								if (maxattack == false)
								{
									printf("you have purchased the max!\n");
								}
								else
								{
									if ( AP > 2)
									{
										bobsStruct.activeHPMods[bobsStruct.activeHPSize] = MediumMelee;
										bobsStruct.activeHPSize++;
										AP = AP - 2;
									}
								
									else
										puts("INSUFFICIENT AP");
								}
								maxa--;
								if (maxa <= 0)
								{
									puts("YOU have reached max purchase");
									maxattack = false;
								}

								break;

							case 3:
								if (maxattack == false)
								{
									printf("you have purchased the max!\n");
								}
								else
								{	
									bobsStruct.activeHPMods[bobsStruct.activeHPSize] = MediumRanged;
									bobsStruct.activeHPSize++;
									AP = AP - 1;
								}
								maxa--;
								if (maxa <= 0)
								{
									puts("YOU have reached max purchase");
									maxattack = false;
								}
								
								break;

							case 4:
								if (maxattack == false)
								{
									printf("you have purchased the max!\n");
								}
								else
								{
									if (AP > 5)
									{
										bobsStruct.activeHPMods[bobsStruct.activeHPSize] = LightLightningAOE;
										bobsStruct.activeHPSize++;
										AP = AP - 5;
									}
									else
										puts("INSUFFICIENT AP");
								}
								maxa--;
								if (maxa <= 0)
								{
									puts("YOU have reached max purchase");
									maxattack = false;
								}
								break;

							case 5:
								if (maxattack == false)
								{
									printf("you have purchased the max!\n");
								}
								else
								{
									if (AP > 3)
									{
										bobsStruct.activeHPMods[bobsStruct.activeHPSize] = MediumHeal;
										bobsStruct.activeHPSize++;
										AP = AP - 3;
									}
									else
										puts("INSUFFICIENT AP");
								}

								maxa--;
								if (maxa <= 0)
								{
									puts("YOU have reached max purchase");
									maxattack = false;
								}


								break;

							case 6:
								if (maxlight == false)
								{
									printf("you have purchased the max!\n");
								}
								else
								{	
									if (AP > 3)
									{
										bobsStruct.passiveAbilities[bobsStruct.passiveSize] = PassiveLightDodge;
										bobsStruct.passiveSize++;
										AP = AP - 3;
									}
									else
										puts("INSUFFICIENT AP");
								}
								maxl--;
								if (maxl <= 0)
								{
									puts("YOU have reached max purchase");
									maxlight = false;
									maxhp = false;
								}
								break;

							case 7:
								if (maxlight == false)
								{
									printf("you have purchased the max!\n");
								}
								else
								{
									if (AP > 8)
									{
										bobsStruct.passiveAbilities[bobsStruct.passiveSize] = PassiveLightArmor;
										bobsStruct.passiveSize++;
										AP = AP - 8;
									}
									else
										puts("INSUFFICIENT AP");
								}
								maxl--;
								if (maxl <= 0)
								{
									puts("YOU have reached max purchase");
									maxlight = false;
									maxhp = false;
								}	
								break;

							case 8:
								if (maxlight == false)
								{
									printf("you have purchased the max!\n");
								}
								{
									if (AP > 3)
									{
										bobsStruct.passiveAbilities[bobsStruct.passiveSize] = PassiveLightMoveBoost;
										bobsStruct.passiveSize++;
										AP = AP - 3;
									}
									else
										puts("NOT YET AVAILABLE");
								}
								maxl--;
								if (maxl <= 0)
								{
									puts("YOU have reached max purchase");
									maxlight = false;
									maxhp = false;
								}	

								break;

							default:
								puts("Please enter a valid number.\n");
							}

							
						}
					}				
					else if (strcmp("n",temp) == 0)
						break;
			
				}
				printf("AP: %d\nBOBS:%d\n",AP, bobsStruct.bobsNumber);
				
				/****************************************************/
				/*Is the player ready has no real functional purpose
				/* Does send clients data
				/* Should be used in future to see if player wants to
				/* modify their bobs
				/***************************************************/
				while(true)
				{
					printf("READY (y)/(n):");
					gets(temp);
					if (strcmp("y", temp) == 0 )
					{			
						puts("----WAITING FOR OTHER PLAYERS----");	
						numOfBobs.typeID = READY;	
						numOfBobs.numOfBobs = bobsStruct.bobsNumber;
						BobsClient->Send((char*)&numOfBobs, sizeof(numOfBobs), HIGH_PRIORITY, RELIABLE_ORDERED, 0);			
						break;
					}
				}
				break;
			
			/**************************************************/
			/* Since client does not have access to the gamestate
			/* the server tells it which Bobs ids are valid for
			/* each player
			/**************************************************/
			case CLIENTS_BOBS_ID:
				ClientData.validBobIDs[nbid] = theBobID->singleBobId;
				nbid++;
				printf("BOB ID VALID: %d \n", ClientData.validBobIDs[nbid - 1]);
				break;
			
			/*************************************************/
			/* Clients recieve map.
			/*************************************************/
			case BLOCKED_TILES:
				blockedX.insert(theBlocked->blockedX);
				blockedY.insert(theBlocked->blockedY);
				break;
			case BROADCAST_MAP:
				puts("RECEIVING THE MAP......");
				printf("GOT UPDATED MAP[%d by %d]\n",theMap->length, theMap->height); 

				bobplayer1.clear();
				bobID1.clear();
				bobx1.clear();
				boby1.clear();
				bobap1.clear();
				bobhp1.clear();

				avx1.clear();
				avy1.clear();
				avcost1.clear();			
				
				mapHeight = theMap->height;
				mapLength = theMap->length;
				usex = 0;
				usey = 0;
				
				//PLACE BOBS ON CLIENTS
				puts("PLACE YOUR BOBS");	
				if (ClientData.myplayerid == 0)
				{
					for(i = 0; i<= 2; i++)
					{	
						for(j = 0; j<= 2; j++)
						{
							occupied = false;
						//	printf("blockedx[%d]: %d\nblockedy[%d]: %d\n\n",i,blockedX[i],j,blockedY[j]);
							for(int q = 0; q < blockedX.size(); q++)
							{
								//printf("blockedX[]: %d ",blockedX[q]);
								if(blockedX[q] == i && blockedY[q] == j)
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
								avx1.insert(i); 
								avy1.insert(j);
								avcost1.insert(0);
							}
						}			
					}
					
					DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);

					for(int i = 0; i < bobsStruct.bobsNumber; i++)
					{
						validinput = true;
						while(validinput)
						{
							bobplayer1.insert(ClientData.name[i]);
							bobID1.insert(10);
							printf("\nBOB[%d] X CORD: ",i);
							gets(temp);
							if( atoi(temp) != 0 && atoi(temp) != 1 && atoi(temp) != 2)
								continue;
						
							printf("BOB[%d] Y CORD: ",i);
							gets(temp2);
							if( atoi(temp2) != 0 && atoi(temp2) != 1 && atoi(temp2) != 2)
								continue;

							for (int ii = 0; ii <= usex; ii++)
							{
								if (atoi(temp2) == usedy[ii] && atoi(temp) == usedx[ii])
								{	
									printf("UNABLE TO PLACE!!!\n");
									validinput = false;
								}
							}
							if(validinput == true)					
							{
								usedx[usex] = atoi(temp);
								usex++;
								usedy[usey] = atoi(temp2);
								usey++;
								validinput = false;
								printf("Bob Placed!\n");
							}	
							else
							{
								validinput = true;
								puts("PLACE IN AVAILABLE SPOTS");
							}
						}

						bobsPosition.x = atoi(temp);
						bobx1.insert(bobsPosition.x);				
						bobsPosition.y = atoi(temp2);
						boby1.insert(bobsPosition.y);
						bobhp1.insert(0);
						bobap1.insert(0);
						bobsPosition.BobsID = i;
						BobsClient->Send((char*)&bobsPosition,sizeof(bobsPosition),HIGH_PRIORITY,RELIABLE_ORDERED,0);
						DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);
					}
				}

				if (ClientData.myplayerid == 1)
				{
					for(i = mapHeight - 1; i>= mapHeight - 3; i--)
					{	
						for(j = mapLength - 1; j>= mapLength - 3; j--)
						{
							occupied = false;
							for(int q = 0; q < blockedX.size(); q++)
							{
								if(blockedX[q] == i && blockedY[q] == j)
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
								avx1.insert(i); 
								avy1.insert(j);
								avcost1.insert(0);
							}
						}			
					}
					DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);
					for(int i = 0; i < bobsStruct.bobsNumber; i++)
					{
						validinput = true;
						while(validinput)
						{
							bobplayer1.insert(ClientData.name[i]);
							bobID1.insert(10);
							printf("\nBOB[%d] X CORD: ",i);
							gets(temp);
							if( atoi(temp) != mapLength - 1 && atoi(temp) != mapLength - 2 && atoi(temp) != mapLength - 3)
								continue;

							printf("BOB[%d] Y CORD: ",i);
							gets(temp2);
							if(atoi(temp2) != mapHeight - 1 && atoi(temp2) != mapHeight - 2 && atoi(temp2) != mapHeight - 3)
								continue;

							for (int ii = 0; ii <= usex; ii++)
							{
								if (atoi(temp2) == usedy[ii] && atoi(temp) == usedx[ii])
								{	
									printf("UNABLE TO PLACE!!!\n");
									validinput = false;
								}
							}
							if(validinput == true)					
							{
								usedx[usex] = atoi(temp);
								usex++;
								usedy[usey] = atoi(temp2);
								usey++;
								validinput = false;
								printf("Bob Placed!\n");
							}	
							else
							{
								validinput = true;
								puts("PLACE IN AVAILABLE SPOTS");
							}
						}

						bobsPosition.x = atoi(temp);
						bobx1.insert(bobsPosition.x);				
						bobsPosition.y = atoi(temp2);
						boby1.insert(bobsPosition.y);
						bobhp1.insert(0);
						bobap1.insert(0);
						bobsPosition.BobsID = i;
						BobsClient->Send((char*)&bobsPosition,sizeof(bobsPosition),HIGH_PRIORITY,RELIABLE_ORDERED,0);
						DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);
					}

				}
				if (ClientData.myplayerid == 2)
				{
					for(i = mapLength - 1; i >= mapLength - 3; i--)
					{	
						for(j = 2; j>= 0; j--)
						{
							occupied = false;
						//	printf("blockedx[%d]: %d\nblockedy[%d]: %d\n\n",i,blockedX[i],j,blockedY[j]);
							for(int q = 0; q < blockedX.size(); q++)
							{
								//printf("blockedX[]: %d ",blockedX[q]);
								if(blockedX[q] == i && blockedY[q] == j)
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
								avx1.insert(i); 
								avy1.insert(j);
								avcost1.insert(0);
							}
						}			
					}
					DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);

					for(int i = 0; i < bobsStruct.bobsNumber; i++)
					{
						validinput = true;
						while(validinput)
						{
							bobplayer1.insert(ClientData.name[i]);
							bobID1.insert(10);
							printf("\nBOB[%d] X CORD: ",i);
							gets(temp);
							if( atoi(temp) != mapLength - 1 && atoi(temp) != mapLength - 2 && atoi(temp) != mapLength - 3)
								continue;

							printf("BOB[%d] Y CORD: ",i);
							gets(temp2);
							if(atoi(temp2) != 0 && atoi(temp2) != 1 && atoi(temp2) !=2)
								continue;

							for (int ii = 0; ii <= usex; ii++)
							{
								if (atoi(temp2) == usedy[ii] && atoi(temp) == usedx[ii])
								{	
									printf("UNABLE TO PLACE!!!\n");
									validinput = false;
								}
							}
							if(validinput == true)					
							{
								usedx[usex] = atoi(temp);
								usex++;
								usedy[usey] = atoi(temp2);
								usey++;
								validinput = false;
								printf("Bob Placed!\n");
							}	
							else
							{
								validinput = true;
								puts("PLACE IN AVAILABLE SPOTS");
							}
						}

					
						bobsPosition.x = atoi(temp);
						bobx1.insert(bobsPosition.x);				
						bobsPosition.y = atoi(temp2);
						boby1.insert(bobsPosition.y);
						bobhp1.insert(0);
						bobap1.insert(0);
						bobsPosition.BobsID = i;
						BobsClient->Send((char*)&bobsPosition,sizeof(bobsPosition),HIGH_PRIORITY,RELIABLE_ORDERED,0);
						DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);
					}
				}

				if (ClientData.myplayerid == 3)
				{
					for(i = 0; i<= 2; i++)
					{	
						for(j = mapHeight - 3; j<= mapHeight - 1; j++)
						{
							occupied = false;
						//	printf("blockedx[%d]: %d\nblockedy[%d]: %d\n\n",i,blockedX[i],j,blockedY[j]);
							for(int q = 0; q < blockedX.size(); q++)
							{
								//printf("blockedX[]: %d ",blockedX[q]);
								if(blockedX[q] == i && blockedY[q] == j)
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
								avx1.insert(i); 
								avy1.insert(j);
								avcost1.insert(0);
							}
						}			
					}
					DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);
					for(i = 0; i < bobsStruct.bobsNumber; i++)
					{
						validinput = true;
						while(validinput)
						{
							bobplayer1.insert(ClientData.name[i]);
							bobID1.insert(10);
							printf("\nBOB[%d] X CORD: ",i);
							gets(temp);
							if( atoi(temp) != 0 && atoi(temp) != 1 && atoi(temp) != 2)
								continue;

							printf("BOB[%d] Y CORD: ",i);
							gets(temp2);
							if( atoi(temp2) != mapHeight - 1 && atoi(temp2) != mapHeight - 2 && atoi(temp2) != mapHeight - 3)
								continue;

							for (int ii = 0; ii <= usex; ii++)
							{
								if (atoi(temp2) == usedy[ii] && atoi(temp) == usedx[ii])
								{	
									printf("UNABLE TO PLACE!!!\n");
									validinput = false;
								}
							}
							if(validinput == true)					
							{
								usedx[usex] = atoi(temp);
								usex++;
								usedy[usey] = atoi(temp2);
								usey++;
								validinput = false;
								printf("Bob Placed!\n");
							}	
							else
							{
								validinput = true;
								puts("PLACE IN AVAILABLE SPOTS");
							}
						}
					
						bobsPosition.x = atoi(temp);
						bobx1.insert(bobsPosition.x);				
						bobsPosition.y = atoi(temp2);
						boby1.insert(bobsPosition.y);
						bobhp1.insert(0);
						bobap1.insert(0);
						bobsPosition.BobsID = i;
						BobsClient->Send((char*)&bobsPosition,sizeof(bobsPosition),HIGH_PRIORITY,RELIABLE_ORDERED,0);
						DrawMap(mapHeight, mapLength, bobplayer1, bobID1, bobx1, boby1, bobhp1, bobap1, avx1, avy1, avcost1, blockedX, blockedY);
					}

				}
															//STORING USED TILES
	

				
				puts("No more Bobs to place");
				
				bobplayer1.clear();
				bobID1.clear();
				bobx1.clear();
				boby1.clear();
				bobap1.clear();
				bobhp1.clear();

				avx1.clear();
				avy1.clear();
				avcost1.clear();		

				/**************************************************/
				/* No real functional purpose, tells server 
				/* I am done placing Bobs. Can be used in
				/* future to double check with player
				/* if they want to edit their Bobs.
				/*************************************************/
				while(true)
				{
					printf("Done Placing Bobs?(y)/(n): ");
					gets(temp);
					if(strcmp("y",temp)==0)
					{
						puts("-----STARTING GAME-----");
						flag.typeID = DONE_PLACING_BOBS;		
						BobsClient->Send((char*)&flag, sizeof(flag), HIGH_PRIORITY, RELIABLE_ORDERED, 0);
						SetUpLooping = false;
						break;
					}
				}	
				break;
			default:
				break;
		}			
		
		//Clean up
		BobsClient->DeallocatePacket(p);
	}

	//Start actual in game client
	BobsClientGame(BobsClient, ClientData, mapLength, mapHeight);
	
	BobsClient->Disconnect(300);
	RakNetworkFactory::DestroyRakClientInterface(BobsClient);
}	
	
void DrawStore()
{
	printf("WELCOME TO BOBS STORE\n");
	puts("_________________________________ ");				
	printf("Abil #	ABILITY       COST\n");
	printf("------  -------       ----\n");
	printf(" (1)   hpIncrease     || *1\n");
	printf(" (2)   mediumAttack   || *2\n");
	printf(" (3)   mediumRange    || *1\n");
	printf(" (4)   Lightning      || *5\n");
	printf(" (5)   mediumHeal     || *3\n");
	printf(" (6)   lightDodgeBase || *3\n");
	printf(" (7)   lightArmor     || *8\n");
	printf(" (8)   lightMoveBoost |Coming Soon| *3\n");
	printf(" (0)   DONE UPGRADING\n");
	puts("_________________________________ ");	
}
