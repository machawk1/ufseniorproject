#include "Reader.h"

Reader::Reader()
{
	queue< ActionRequestType > actionRequestQueue;
	queue< TargetOptionType > targetOptionQueue;
	queue< ActionToPerformType > actionToPerformQueue;
	queue< ChangeStateType > updateStateQueue;
	queue< ChatMessageType > chatMessageQueue;
	queue< TargetOptionType > movePathQueue;
}

Reader::~Reader()
{
}

void Reader::getMessages()			// get message object from network
{
	
	// check for message availability

	// check to ensure that message is complete and valid
		// if message is ok check header of message
		// else, break

	// based on the header, load data into message struct of appropriate type

	// load struct into appropriate queue

}


ActionRequestType Reader::getActionRequest()	// extract action request object and place in queue 
{
	if(actionRequestQueue.empty())
		throw EmptyQueue;
	
	ActionRequestType temp = actionRequestQueue.front();
	actionRequestQueue.pop();

	return temp;
}

void Reader::sendTargetOption()		// send back the list of potential targets
{
}

ActionToPerformType Reader::getActionToPerform()	// extract chosen move info and place in queue
{
	if(actionToPerformQueue.empty())
		throw EmptyQueue;
	
	ActionToPerformType temp = actionToPerformQueue.front();
	actionToPerformQueue.pop();

	return temp;
}

void Reader::sendResults()		// send updated game state to client
{
}

ChatMessageType Reader::getChatMessage()
{
	if(chatMessageQueue.empty())
		throw EmptyQueue;
	
	ChatMessageType temp = chatMessageQueue.front();
	chatMessageQueue.pop();

	return temp;
}

void Reader::sendChatMessage()
{
}
