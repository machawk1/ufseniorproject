#include "Bob.h"

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
			BobPassive *passiveAbilities, BobHPMod *activeHPMods, BobStatMod *activeStatMods)
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
	this->passiveAbilities = new BobPassive[passiveSize];
	this->activeHPMods = new BobHPMod[activeHPSize];
	this->activeStatMods = new BobStatMod[activeStatSize];
	
	// Assign abilities
	for (i = 0; i < passiveSize; i++)
	{
		this->passiveAbilities[i] = passiveAbilities[i];
		this->operator +(*passiveAbilities[i].ability);	// Add passive abilities to BOB's attributes
	}

	for (i = 0; i < activeHPSize; i++)
		this->activeHPMods[i] = activeHPMods[i];
	
	for (i = 0; i < activeStatSize; i++)
		this->activeStatMods[i] = activeStatMods[i];
}


Bob::~Bob()
{
	// Delete dynamic memory if allocated
	if(passiveAbilities != NULL)
		delete [] passiveAbilities;
	
	if(activeHPMods != NULL)
		delete [] activeHPMods;
	
	if(activeStatMods != NULL)
		delete [] activeStatMods;
	
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

int Bob::getBaseHP()
{
	return hpBase;	
}


int Bob::getHP()
{
	return hpCurrent;
}

void Bob::setHP(int hp)
{
	hpCurrent = hpCurrent + hp;
	
	// Ensure that hit points are not out of bounds
	if (hpCurrent < 0) 
		hpCurrent = 0;
	else if(hpCurrent > hpBase)
		hpCurrent = hpBase;
}

int Bob::getAP()
{
	return apCurrent;
}

void Bob::setAP(int ap)
{
	apCurrent = apCurrent + ap;
	
	// Ensure that ap are not out of bounds
	if (apCurrent < 0) 
		apCurrent = 0;
	else if(apCurrent > apBase)
		apCurrent = apBase;
}

void Bob::resetAP()
{
	apCurrent = apBase;
}

int Bob::getToHit()
{
	return toHitCurrent;
}

void Bob::setToHit(int toHit)
{
	toHitCurrent = toHitBase + toHit;
}

void Bob::resetToHit()
{
	toHitCurrent = toHitBase;
}

int Bob::getMitigation()
{
	return mitigationCurrent;
}

void Bob::setMitigation(int mitigation)
{
	mitigationCurrent = mitigationBase + mitigation;
}

void Bob::resetMitigation()
{
	mitigationCurrent = mitigationBase;
}

int Bob::getMove()
{
	return moveCurrent;
}

void Bob::setMove(int move)
{
	moveCurrent = moveBase + move;
}

void Bob::resetMove()
{
	moveCurrent = moveBase;
}

int Bob::getDodge()
{
	return dodgeCurrent;
}

void Bob::setDodge(int dodge)
{
	dodgeCurrent = dodgeBase + dodge;
}

void Bob::resetDodge()
{
	dodgeCurrent = dodgeBase;
}

int Bob::getPassiveSize()
{
	return passiveSize;
}

BobPassive* Bob::getPassiveAbilities()
{
	return passiveAbilities;
}

int Bob::getActiveHPSize()
{
	return activeHPSize;
}

BobHPMod* Bob::getActiveHPMods()
{
	return activeHPMods;
}

int Bob::getActiveStatSize()
{
	return activeStatSize;
}

BobStatMod* Bob::getActiveStatMods()
{
	return activeStatMods;
}


void Bob::operator =(Bob const &bob)
{
	int i;

	ID = bob.ID;
	playerHandle = bob.playerHandle;
	x = bob.x;
	y = bob.y;
	hpBase = bob.hpBase;
	hpCurrent = bob.hpCurrent;
	apBase = bob.apBase;
	apCurrent = bob.apCurrent;
	toHitBase = bob.toHitBase;
	toHitCurrent = bob.toHitCurrent;
	mitigationBase = bob.mitigationBase;
	mitigationCurrent = bob.mitigationCurrent;
	moveBase = bob.moveBase;
	moveCurrent = bob.moveCurrent;
	dodgeBase = bob.dodgeBase;
	dodgeCurrent = bob.dodgeCurrent;
	passiveSize = bob.passiveSize;
	activeHPSize = bob.activeHPSize;
	activeStatSize = bob.activeStatSize;
	
	// Delete old memory
	if(activeHPMods == NULL)
		delete [] activeHPMods;
	
	if(activeStatMods == NULL)
		delete [] activeStatMods;
	
	if(passiveAbilities == NULL)
		delete [] passiveAbilities;
	
	// Reallocate memory to handle new sized arrays
	activeHPMods = new BobHPMod[activeHPSize];
	activeStatMods = new BobStatMod[activeStatSize];
	passiveAbilities = new BobPassive[passiveSize];
	
	// Deep copy abilities to new BOB
	for(i = 0; i < activeHPSize; i++)
		activeHPMods[i] = bob.activeHPMods[i];
	
	for(i = 0; i < activeStatSize; i++)
		activeStatMods[i] = bob.activeStatMods[i];
	
	for(i = 0; i < passiveSize; i++)
		passiveAbilities[i] = bob.passiveAbilities[i];
}

// Add passive ability modifiers to BOB's base stats
void Bob::operator +(PassiveAbility &passive)
{
	apBase += passive.getApMod();
	dodgeBase += passive.getDodgeMod();
	hpBase += passive.getHpMod();
	mitigationBase += passive.getMitigationMod();
	moveBase += passive.getMovementMod();
	toHitBase += passive.getToHitMod();

	apCurrent = apBase;
	dodgeCurrent = dodgeBase;
	hpCurrent = hpBase;
	mitigationCurrent = mitigationBase;
	moveCurrent = moveBase;
	toHitCurrent = toHitBase;
}