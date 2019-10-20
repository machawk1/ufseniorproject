#ifndef BOB_H_
#define BOB_H_

#include "ActiveHPMod.h"
#include "ActiveStatMod.h"
#include "PassiveAbility.h"
#include <string>

using std::string;

class Bob
{
	public:

		Bob();
		Bob(string playerHandle, int hpBase, int apBase, int toHitBase, int mitigationBase,
			int moveBase, int dodgeBase, int passiveSize, int activeHPSize, int activeStatSize, 
			PassiveAbility *passiveAbilities, ActiveHPMod *activeHPMods, ActiveStatMod *activeStatMods);

		~Bob();

		int getID();
		void setID(int ID);
		
		string getPlayerHandle();
		void setPlayerHandle(string playerHandle);
		
		int getX();
		int getY();
		void setPosition(int x, int y);
		
		int getHP();
		void setHP(int hp);
		
		int getAP();
		void setAP(int ap);
		
		int getToHit();
		void setToHit(int toHit);
		
		int getMitigation();
		void setMitigation(int mitigaton);
		
		int getMove();
		void setMove(int move);
		
		int getDodge();
		void setDodge(int dodge);
		
		int getPassiveSize();
		PassiveAbility* getPassiveAbilities();
		
		int getActiveHPSize();
		ActiveHPMod* getActiveHPMods();
		
		int getActiveStatSize();
		ActiveStatMod* getActiveStatMods();

		void operator =(Bob const &bob);
		void operator +(PassiveAbility &passive);
		
	private:
		string playerHandle;
		
		int	ID,
			x,
			y,
			hpBase,
			hpCurrent,
			apBase,
			apCurrent,
			toHitBase,
			toHitCurrent,
			mitigationBase,
			mitigationCurrent,
			moveBase,
			moveCurrent,
			dodgeBase,
			dodgeCurrent,
			passiveSize,
			activeHPSize,
			activeStatSize;
			
		PassiveAbility *passiveAbilities;
		ActiveHPMod *activeHPMods;
		ActiveStatMod *activeStatMods;
};

#endif