#include "Reader.h"

void getMessages()
{
	
	// check for message availability

	// check to ensure that message is complete and valid
		// if message is ok check header of message
		// else, break

	// based on the header, load data into message struct of appropriate type

	// load struct into appropriate queue

}

/*
static queue < ActionRequestType > actionRequestQueue;
static queue < MoveOptionType > moveOptionQueue;
static queue < TargetOptionType > targetOptionQueue;
static queue < ActionToPerformType > actionToPerformQueue;
static queue < ChangeStateType > updateStateQueue;
static queue < ChatMessageType > chatMessageQueue;   
*/

ActionRequestType getActionRequest()
{
	if(actionRequestQueue.empty())
	{
		throw EmptyQueue;
	}
	ActionRequestType temp = actionRequestQueue.front();
	actionRequestQueue.pop();

	return temp;
}

ActionToPerformType getActionToPerform()
{
	if(actionToPerformQueue.empty())
	{
		throw EmptyQueue;
	}
	ActionToPerformType temp = actionToPerformQueue.front();
	actionToPerformQueue.pop();

	return temp;
}

ChatMessageType getChatMessage()
{
	if(chatMessageQueue.empty())
	{
		throw EmptyQueue;
	}
	ChatMessageType temp = chatMessageQueue.front();
	chatMessageQueue.pop();

	return temp;
}

