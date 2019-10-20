#ifndef PLAYER_H_
#define PLAYER_H_

#include "Bob.h"

class Player
{
	public:
		Player();
		Player(string handle);

		~Player();

		string getHandle();
		
		int getID();
		
		int getAP();
		void setAP(int ap);
		
		void setBobs(Bob *bobs, int numBobs);
		Bob* getBobs();
		int getNumBobs();

		void operator =(Player const &player);

	private:

		string handle;
		Bob *bobList;
		int	ID,
			numBobs,
			AP;
};


#endif