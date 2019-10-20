#ifndef BOB_H_
#define BOB_H_

#include "ActiveHPMod.h"
#include "ActiveStatMod.h"
#include "PassiveAbility.h"
#include <string>

using std::string;

struct BobHPMod
{
	ActiveHPMod *ability;
	int turnsLeft;
};

struct BobStatMod
{
	ActiveStatMod *ability;
	int turnsLeft;
};

struct BobPassive
{
	PassiveAbility *ability;
	int turnsLeft;
};

class Bob
{
	public:

		Bob();
		Bob(string playerHandle, int hpBase, int apBase, int toHitBase, int mitigationBase,
			int moveBase, int dodgeBase, int passiveSize, int activeHPSize, int activeStatSize, 
			BobPassive *passiveAbilities, BobHPMod *activeHPMods, BobStatMod *activeStatMods);

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
		void resetAP();
		
		int getToHit();
		void setToHit(int toHit);
		void resetToHit();
		
		int getMitigation();
		void setMitigation(int mitigaton);
		void resetMitigation();
		
		int getMove();
		void setMove(int move);
		void resetMove();
		
		int getDodge();
		void setDodge(int dodge);
		void resetDodge();
		
		int getPassiveSize();
		BobPassive* getPassiveAbilities();
		
		int getActiveHPSize();
		BobHPMod* getActiveHPMods();
		
		int getActiveStatSize();
		BobStatMod* getActiveStatMods();

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
			
		BobPassive *passiveAbilities;
		BobHPMod *activeHPMods;
		BobStatMod *activeStatMods;
};

#endif