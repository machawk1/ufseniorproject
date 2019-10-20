#include "RakNetworkFactory.h"
#include "RakClientInterface.h"
#include "RakServerInterface.h"
#include "PacketEnumerations.h" //Raknet TimeStamp
#include "ArrayList.h"
#include <assert.h> //assert ...
#include <stdio.h> //printf ...
#include <string.h> //strcpy ...
#include <stdlib.h> //atoi ...
#include <conio.h> // kbhit() ...
#include <windows.h> // sleep() ..

#include "Player.h"
#include "Map.h"
#include "GameState.h"
#include "AbilityList.h"
#include <string>

struct StaticClientDataStruct										//CLIENT DATA
{
	unsigned char typeID; 
	char name[40];
	char ip[20];
	char Port[8];
	int myplayerid;
	int validBobIDs[40];
};

struct StaticServerDataStruct										//SERVER DATA
{
	unsigned char typeID;
	char serverName[40];
	char NumOfPlayers[30];
	
};

struct ClientsAP													//SENDS AP TO CLIENTS
{
	unsigned char typeID;
	int myplayerid;
	int AP;
};

struct BobsStruct													//SEND BOBS PURCHASE INFORMATION TO SERVER
{	
	unsigned char typeID;
	int apBase;
	int passiveSize;
	int activeHPSize;
	int activeStatSize;
	int bobsNumber;
	AbilityNameType passiveAbilities[4]; 
	AbilityNameType activeHPMods[5]; 
	AbilityNameType activeStatMods[1];
};

struct Flag														//NOTIFY READY
{
	unsigned char typeID;
};

struct NumberOfBobs
{
	unsigned char typeID;
	int numOfBobs;
	int singleBobId;
	int BobsID[30];
};


struct ServerMap													//MAP
{
	unsigned char typeID;
	int length;
	int height;
	TerrainType terrain;
	int blockedX;
	int blockedY;
};

struct BobsPosition
{
	unsigned char typeID;
	int BobsID;
	int x;
	int y;
};

struct BlockedTiles
{
	unsigned char typeID;
	int blockedX;
	int blockedY;
};
struct ShellMap
{
	unsigned char typeID;
	char player[20];
	int bob;
	int x, y;
	int AP;
	int HP;
};

struct WhosTurn
{
	unsigned char typeID;
	char player[20];
	int bob;
	int bobsNumber;
	int AP;
};

struct RequestStruct
{
	unsigned char typeID;
	int bobID;
	AbilityNameType ability;
	int noBobs;
};

struct NetworkTargetOption      // used to send possible target tiles to client
{
	unsigned char typeID;
	int x;
	int y;
	int cost;
	bool done;
};

struct ActualTarget  // used to recieve action commands from client
{
	unsigned char typeID;
	int bobID;
	AbilityNameType ability;
	int x;
	int y;
};
