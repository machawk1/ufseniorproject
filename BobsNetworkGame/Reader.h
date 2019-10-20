#ifndef READER_H_
#define READER_H_

#include <queue>
#include <string>
#include "Bob.h"

struct ActionRequestType      // used to receive bobID and action type from client
{
	int bobID;
	AbilityNameType ability;
};

struct TargetOptionType      // used to send possible target tiles to client
{
	int x;
	int y;
	int cost;
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
	Bob *bob;
	AbilityNameType ability;
};

struct ChatMessageType   // used to send and receive chat messages with client 
{
	string playerHandle;
	string message;
};

class Reader
{
	public:
		Reader();
		
		virtual ~Reader();
		
		void getMessages();		// get message object from network
		void sendTargetOption();		// send list of possible targets
		void sendChatMessage();		
		void sendResults();					// update client's game state
		ActionRequestType getActionRequest();	// extract action request info from message
		ActionToPerformType getActionToPerform();	// extract chosen move info from message
		ChatMessageType getChatMessage();
		
		queue< ActionRequestType > actionRequestQueue;
		queue< TargetOptionType > targetOptionQueue;
		queue< ActionToPerformType > actionToPerformQueue;
		queue< ChangeStateType > updateStateQueue;
		queue< ChatMessageType > chatMessageQueue;
		queue< TargetOptionType > movePathQueue;
};

#endif /*READER_H_*/
