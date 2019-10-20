#include "Player.h"

// Constructors
Player::Player()
{
	ID = GLOBAL_ID++;
	bobList = NULL;
	numBobs = 0;
	AP = 0;
	this->playerType = Human;
}

Player::Player(string handle, PlayerType playerType)
{
	ID = GLOBAL_ID++;
	bobList = NULL;
	numBobs = 0;
	this->handle = handle;
	this->playerType = playerType;
	AP = 0;
}


// Destructor
Player::~Player()
{
	if(bobList != NULL)
		delete [] bobList;
	
	bobList = NULL;
}


// Get Methods
string Player::getHandle()
{
	return handle;
}

int Player::getID()
{
	return ID;
}

int Player::getAP()
{
	return AP;
}

PlayerType Player::getPlayerType()
{
	return playerType;	
}

int Player::getNumBobs()
{
	return numBobs;
}

Bob* Player::getBobs()
{
	return bobList;
}


// Set Methods
void Player::setAP(int ap)
{
	AP = AP + ap;
	
	// Ensure that ap are not out of bounds
	if (AP < 0) 
		AP = 0;
}

void Player::setBobs(Bob *bobs, int numBobs)
{
	if(numBobs < 1)
		throw InvalidLength;
	
	this->numBobs = numBobs;

	bobList = new Bob[numBobs];

	for(int i = 0; i < numBobs; i++)
		bobList[i] = bobs[i];
}

void Player::operator =(Player const &player)
{
	AP = player.AP;
	handle = player.handle;
	numBobs = player.numBobs;
	playerType = player.playerType;

	if(bobList != NULL)
		delete [] bobList;
	
	bobList = new Bob[numBobs];

	for(int i = 0; i < numBobs; i++)
		bobList[i] = player.bobList[i];
}