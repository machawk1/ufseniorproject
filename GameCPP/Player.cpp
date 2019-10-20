#include "Player.h"

// Constructors
Player::Player()
{
	ID = GLOBAL_ID++;
	bobList = NULL;
	numBobs = 0;
}

Player::Player(string handle)
{
	ID = GLOBAL_ID++;
	bobList = NULL;
	numBobs = 0;
	this->handle = handle;
}


// Destructor
Player::~Player()
{
	#ifdef DEBUG
		cout << this->handle << " destructed!" << endl;
	#endif
	
	if(bobList != NULL)
	{
		delete [] bobList;
	}
	
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
	AP = ap;
}

void Player::setBobs(Bob *bobs, int numBobs)
{
	if(numBobs < 1)
	{
		throw InvalidLength;
	}

	this->numBobs = numBobs;

	bobList = new Bob[numBobs];

	for(int i = 0; i < numBobs; i++)
	{
		bobList[i] = bobs[i];
	}
}

void Player::operator =(Player const &player)
{
	this->AP = player.AP;
	this->handle = player.handle;
	this->numBobs = player.numBobs;

	if(this->bobList != NULL)
	{
		delete [] bobList;
	}

	this->bobList = new Bob[this->numBobs];

	for(int i = 0; i < this->numBobs; i++)
	{
		this->bobList[i] = player.bobList[i];
	}
}