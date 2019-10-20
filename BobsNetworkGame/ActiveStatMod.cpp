#include "ActiveStatMod.h"

ActiveStatMod::ActiveStatMod()
{
}

ActiveStatMod::ActiveStatMod(AbilityNameType name, int buyCost, int useCost, int minRange, int maxRange, int radius,
	int cooldown, int duration, int hpMod, int apMod, int movementMod, int toHitMod, int mitigationMod, int dodgeMod)
{
	ID = GLOBAL_ID++;
	this->name = name;
	this->buyCost = buyCost;
	this->useCost = useCost;
	this->minRange = minRange;
	this->maxRange = maxRange;
	this->radius = radius;
	this->cooldown = cooldown;
	this->hpMod = hpMod;
	this->apMod = apMod;
	this->movementMod = movementMod;
	this->toHitMod = toHitMod;
	this->mitigationMod = mitigationMod;
	this->dodgeMod = dodgeMod;
	this->duration = duration;
}

ActiveStatMod::ActiveStatMod(string file)
{
	ifstream infile;
	int temp;

	infile.open(file.c_str(), ios::in);

	if(!infile.is_open())
		throw FileNotFound;
	
	ID = GLOBAL_ID++;
	cin >> temp >> buyCost >> useCost >> minRange >> maxRange >> radius >> cooldown >> hpMod >> apMod >>
		movementMod >> toHitMod >> mitigationMod >> dodgeMod >> duration;
	name = (AbilityNameType)temp;
}

ActiveStatMod::~ActiveStatMod()
{
}

int ActiveStatMod::getID()
{
	return ID;
}

int ActiveStatMod::getHpMod()
{
	return hpMod;
}

void ActiveStatMod::setHpMod(int hpMod)
{
	this->hpMod = hpMod;
}

int ActiveStatMod::getApMod()
{
	return apMod;
}

void ActiveStatMod::setApMod(int apMod)
{
	this->apMod = apMod;
}
	
int ActiveStatMod::getMovementMod()
{
	return movementMod;
}

void ActiveStatMod::setMovementMod(int movementMod)
{
	this->movementMod = movementMod;
}

int ActiveStatMod::getToHitMod()
{
	return toHitMod;
}

void ActiveStatMod::setToHitMod(int toHitMod)
{
	this->toHitMod = toHitMod;
}

int ActiveStatMod::getMitigationMod()
{
	return mitigationMod;
}

void ActiveStatMod::setMitigationMod(int mitigationMod)
{
	this->mitigationMod = mitigationMod;
}
	
int ActiveStatMod::getDodgeMod()
{
	return dodgeMod;
}

void ActiveStatMod::setDodgeMod( int dodgeMod)
{
	this->dodgeMod = dodgeMod;
}
	
int ActiveStatMod::getDuration()
{
	return duration;
}

void ActiveStatMod::setDuration(int duration)
{
	this->duration = duration;
}
