#include "Ability.h"

Ability::Ability()
{
}

Ability::~Ability()
{
}

AbilityNameType Ability::getName()
{
	return name;
}

void Ability::setName(AbilityNameType name)
{
	this->name = name;
}

int Ability::getBuyCost()
{
	return buyCost;
}

void Ability::setBuyCost(int buyCost)
{
	this->buyCost = buyCost;	
}

