#ifndef ABILITY_H_
#define ABILITY_H_

#include "Common.h"

class Ability
{
public:
	Ability();
	
	virtual ~Ability();
	
	AbilityNameType getName();
	void setName(AbilityNameType name);
	
	int getBuyCost();
	void setBuyCost(int buyCost);
	
protected:
	AbilityNameType name;
	int buyCost;
};

#endif /*ABILITY_H_*/
