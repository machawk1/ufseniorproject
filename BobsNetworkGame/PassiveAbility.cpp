#include "PassiveAbility.h"

PassiveAbility::PassiveAbility()
{
}

PassiveAbility::PassiveAbility(AbilityNameType name, int buyCost, int hpMod, int apMod, int movementMod, int toHitMod, int mitigationMod, int dodgeMod)
{
	ID = GLOBAL_ID++;
	this->name = name;
	this->buyCost = buyCost;
	this->hpMod = hpMod;
	this->apMod = apMod;
	this->movementMod = movementMod;
	this->toHitMod = toHitMod;
	this->mitigationMod = mitigationMod;
	this->dodgeMod = dodgeMod;
}

PassiveAbility::PassiveAbility(string file)
{
	ifstream infile;
	int temp;

	infile.open(file.c_str(), ios::in);

	if(!infile.is_open())
		throw FileNotFound;
	
	ID = GLOBAL_ID++;
	cin >> temp >> buyCost >> hpMod >> apMod >> movementMod >> toHitMod >> mitigationMod >> dodgeMod;
	name = (AbilityNameType)temp;
}

PassiveAbility::~PassiveAbility()
{
}

int PassiveAbility::getID()
{
	return ID;	
}

int PassiveAbility::getHpMod()
{
	return hpMod;
}

void PassiveAbility::setHpMod(int hpMod)
{
	this->hpMod = hpMod;
}

int PassiveAbility::getApMod()
{
	return apMod;
}

void PassiveAbility::setApMod(int apMod)
{
	this->apMod = apMod;
}

int PassiveAbility::getMovementMod()
{
	return movementMod;
}

void PassiveAbility::setMovementMod(int movementMod)
{
	this->movementMod = movementMod;
}

int PassiveAbility::getToHitMod()
{
	return toHitMod;
}

void PassiveAbility::setToHitMod(int toHitMod)
{
	this->toHitMod = toHitMod;
}

int PassiveAbility::getMitigationMod()
{
	return mitigationMod;
}

void PassiveAbility::setMitigationMod(int mitigationMod)
{
	this->mitigationMod = mitigationMod;
}

int PassiveAbility::getDodgeMod()
{
	return dodgeMod;
}

void PassiveAbility::setDodgeMod(int dodgeMod)
{
	this->dodgeMod = dodgeMod;
}
