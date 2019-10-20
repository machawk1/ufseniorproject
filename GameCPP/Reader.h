#ifndef READER_H_
#define READER_H_

#include <string>
#include <queue>
#include "Common.h"

using std::queue;

// need to add message broadcasting active player to all clients

struct ActionRequestType      // used to receive bobID and action type from client
{
	int bobID;
	AbilityNameType ability;
};

struct MoveOptionType      // used to send possible destination tiles to client
{
	int x;
	int y;
	int cost;
};

struct TargetOptionType      // used to send possible target tiles to client
{
	int x;
	int y;
};

struct ActionToPerformType  // used to recieve action commands from client
{
	int bobID;
	AbilityNameType ability;
	int x;
	int y;
};

struct ChangeStateType    // used to send game state updates to client
{
	// tons of stuff
};

struct ChatMessageType   // used to send and receive chat messages with client 
{
	string playerHandle;
	string message;
};

static queue < ActionRequestType > actionRequestQueue;
static queue < MoveOptionType > moveOptionQueue;
static queue < TargetOptionType > targetOptionQueue;
static queue < ActionToPerformType > actionToPerformQueue;
static queue < ChangeStateType > updateStateQueue;
static queue < ChatMessageType > chatMessageQueue;


void getMessages();
ActionRequestType getActionRequest();
ActionToPerformType getActionToPerform();
ChatMessageType getChatMessage();


#endif 