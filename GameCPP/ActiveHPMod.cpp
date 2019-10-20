#include "ActiveHPMod.h"

ActiveHPMod::ActiveHPMod()
{
}

ActiveHPMod::ActiveHPMod(AbilityNameType name, int buyCost, int useCost, int minRange, int maxRange, int radius, int cooldown, int min, int max)
{
	ID = GLOBAL_ID++;
	this->name = name;
	this->buyCost = buyCost;
	this->useCost = useCost;
	this->minRange = minRange;
	this->maxRange = maxRange;
	this->radius = radius;
	this->cooldown = cooldown;
	this->min = min;	
	this->max = max;
	this->turnsLeft = 0;	
}

ActiveHPMod::~ActiveHPMod()
{
	#ifdef DEBUG
		cout << this->name << " destructed!" << endl;
	#endif
}

int ActiveHPMod::getID()
{
	return ID;	
}

int ActiveHPMod::getMin()
{
	return min;
}

void ActiveHPMod::setMin(int min)
{
	this->min = min;
}

int ActiveHPMod::getMax()
{
	return max;
}
void ActiveHPMod::setMax(int max)
{
	this->max = max;
}