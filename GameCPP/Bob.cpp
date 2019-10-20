#include "Bob.h"

using std::cout;
using std::endl;

Bob::Bob()
{
	// Set defauly values for the BOB
	ID = GLOBAL_ID++;
	
	passiveAbilities = NULL;
	activeHPMods = NULL;
	activeStatMods = NULL;
	
	passiveSize = 0;
	activeHPSize = 0;
	activeStatSize = 0;
}

Bob::Bob(string playerHandle, int hpBase, int apBase, int toHitBase, int mitigationBase,
			int moveBase, int dodgeBase, int passiveSize, int activeHPSize, int activeStatSize, 
			PassiveAbility *passiveAbilities, ActiveHPMod *activeHPMods, ActiveStatMod *activeStatMods)
{
	int i;

	// Set values of the BOB
	ID = GLOBAL_ID++;
	this->playerHandle = playerHandle;
	this->x = -1;
	this->y = -1;
	this->hpBase = hpBase;
	this->hpCurrent = hpBase;
	this->apBase = apBase;
	this->apCurrent = apBase;
	this->toHitBase = toHitBase;
	this->toHitCurrent = toHitBase;
	this->mitigationBase = mitigationBase;
	this->mitigationCurrent = mitigationBase;
	this->moveBase = moveBase;
	this->moveCurrent = moveBase;
	this->dodgeBase = dodgeBase;
	this->dodgeCurrent = dodgeBase;
	this->passiveSize = passiveSize;
	this->activeHPSize = activeHPSize;
	this->activeStatSize = activeStatSize;

	// Allocate memory for abilities
	this->passiveAbilities = new PassiveAbility[passiveSize];
	this->activeHPMods = new ActiveHPMod[activeHPSize];
	this->activeStatMods = new ActiveStatMod[activeStatSize];
	
	// Assign abilities
	for (i = 0; i < passiveSize; i++)
	{
		this->passiveAbilities[i] = passiveAbilities[i];
		this->operator +(passiveAbilities[i]);	// Add passive abilities to BOB's attributes
	}

	for (i = 0; i < activeHPSize; i++)
	{
		this->activeHPMods[i] = activeHPMods[i];
	}
	
	for (i = 0; i < activeStatSize; i++)
	{
		this->activeStatMods[i] = activeStatMods[i];
	}		
}


Bob::~Bob()
{
	#ifdef DEBUG
		cout << "Bob " << this->ID << " destructed!" << endl;
	#endif
	
	// Delete dynamic memory if allocated
	if(passiveAbilities != NULL)
	{
		delete [] passiveAbilities;
	}
	if(activeHPMods != NULL)
	{
		delete [] activeHPMods;
	}
	if(activeStatMods != NULL)
	{
		delete [] activeStatMods;
	}
	
	passiveAbilities = NULL;
	activeHPMods = NULL;
	activeStatMods = NULL;
}

int Bob::getID()
{
	return ID;
}

void Bob::setID(int ID)
{
	this->ID = ID;
}


string Bob::getPlayerHandle()
{
	return playerHandle;
}

void Bob::setPlayerHandle(string playerHandle)
{
	this->playerHandle = playerHandle;
}

int Bob::getX()
{
	return x;
}

int Bob::getY()
{
	return y;
}

void Bob::setPosition(int x, int y)
{
	this->x = x;
	this->y = y;
}

int Bob::getHP()
{
	return hpCurrent;
}

void Bob::setHP(int hp)
{
	// Ensure that hit points are not out of bounds
	if (hp < 0) 
	{
		this->hpCurrent = 0;
	}
	else
	{

		this->hpCurrent = (hp > this ->hpBase) ? this ->hpBase : hp;
	}
}

int Bob::getAP()
{
	return apCurrent;
}

void Bob::setAP(int ap)
{
	this->apCurrent = ap;
}

int Bob::getToHit()
{
	return toHitCurrent;
}

void Bob::setToHit(int toHit)
{
	// Set the modified To Hit value
	this->toHitCurrent = this->toHitBase + toHit;
}

int Bob::getMitigation()
{
	return mitigationCurrent;
}

void Bob::setMitigation(int mitigation)
{
	// Set the modified Mitigation value
	this->mitigationCurrent = this->mitigationBase + mitigation;
}

int Bob::getMove()
{
	return moveCurrent;
}

void Bob::setMove(int move)
{
	// Set the modified move value
	this->moveCurrent = this->moveBase + move;
}

int Bob::getDodge()
{
	return dodgeCurrent;
}

void Bob::setDodge(int dodge)
{
	// Set the modified dodge value
	this->dodgeCurrent = this->dodgeBase + dodge;
}

int Bob::getPassiveSize()
{
	return passiveSize;
}

PassiveAbility* Bob::getPassiveAbilities()
{
	return passiveAbilities;
}

int Bob::getActiveHPSize()
{
	return activeHPSize;
}

ActiveHPMod* Bob::getActiveHPMods()
{
	return activeHPMods;
}

int Bob::getActiveStatSize()
{
	return activeStatSize;
}

ActiveStatMod* Bob::getActiveStatMods()
{
	return activeStatMods;
}


void Bob::operator =(Bob const &bob)
{
	int i;

	// Set the new BOB's value to the old BOB's value
	this->ID = bob.ID;
	this->playerHandle = bob.playerHandle;
	this->x = bob.x;
	this->y = bob.y;
	this->hpBase = bob.hpBase;
	this->hpCurrent = bob.hpCurrent;
	this->apBase = bob.apBase;
	this->apCurrent = bob.apCurrent;
	this->toHitBase = bob.toHitBase;
	this->toHitCurrent = bob.toHitCurrent;
	this->mitigationBase = bob.mitigationBase;
	this->mitigationCurrent = bob.mitigationCurrent;
	this->moveBase = bob.moveBase;
	this->moveCurrent = bob.moveCurrent;
	this->dodgeBase = bob.dodgeBase;
	this->dodgeCurrent = bob.dodgeCurrent;
	this->passiveSize = bob.passiveSize;
	this->activeHPSize = bob.activeHPSize;
	this->activeStatSize = bob.activeStatSize;
	
	// Delete old memory
	if(this->activeHPMods == NULL)
	{
		delete [] this->activeHPMods;
	}
	if(this->activeStatMods == NULL)
	{
		delete [] this->activeStatMods;
	}
	if(this->passiveAbilities == NULL)
	{
		delete [] this->passiveAbilities;
	}
	
	// Reallocate memory to handle new sized arrays
	this->activeHPMods = new ActiveHPMod[this->activeHPSize];
	this->activeStatMods = new ActiveStatMod[this->activeStatSize];
	this->passiveAbilities = new PassiveAbility[this->passiveSize];
	
	// Deep copy abilities to new BOB
	for(i = 0; i < this->activeHPSize; i++)
	{
		this->activeHPMods[i] = bob.activeHPMods[i];
	}
	for(i = 0; i < this->activeStatSize; i++)
	{
		this->activeStatMods[i] = bob.activeStatMods[i];
	}
	for(i = 0; i < this->passiveSize; i++)
	{
		this->passiveAbilities[i] = bob.passiveAbilities[i];
	}
}

// Add passive ability modifiers to BOB's base stats
void Bob::operator +(PassiveAbility &passive)
{
	this->apBase += passive.getApMod();
	this->dodgeBase += passive.getDodgeMod();
	this->hpBase += passive.getHpMod();
	this->mitigationBase += passive.getMitigationMod();
	this->moveBase += passive.getMovementMod();
	this->toHitBase += passive.getToHitMod();

	apCurrent = apBase;
	dodgeCurrent = dodgeBase;
	hpCurrent = hpBase;
	mitigationCurrent = mitigationBase;
	moveCurrent = moveBase;
	toHitCurrent = toHitBase;
}