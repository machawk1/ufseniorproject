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
}

ActiveHPMod::ActiveHPMod(string file)
{
	ifstream infile;
	int temp;

	infile.open(file.c_str(), ios::in);

	if(!infile.is_open())
		throw FileNotFound;
	
	ID = GLOBAL_ID++;
	cin >> temp >> buyCost >> useCost >> minRange >> maxRange >> radius >> cooldown >> min >> max;
	name = (AbilityNameType)temp;
}

ActiveHPMod::~ActiveHPMod()
{
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