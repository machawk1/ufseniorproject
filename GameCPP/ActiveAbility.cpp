#include "ActiveAbility.h"

ActiveAbility::ActiveAbility()
{
}

ActiveAbility::~ActiveAbility()
{
}
int ActiveAbility::getUseCost()
{
	return useCost;
}

void ActiveAbility::setUseCost(int useCost)
{
	this->useCost = useCost;
}

int ActiveAbility::getMinRange()
{
	return minRange;
}

void ActiveAbility::setMinRange(int useCost)
{
	this->minRange = minRange;
}

int ActiveAbility::getMaxRange()
{
	return maxRange;
}

void ActiveAbility::setMaxRange(int useCost)
{
	this->maxRange = maxRange;
}

int ActiveAbility::getRadius()
{
	return radius;
}

void ActiveAbility::setRadius(int useCost)
{
	this->radius = radius;
}

int ActiveAbility::getCooldown()
{
	return cooldown;
}

void ActiveAbility::setCooldown(int useCost)
{
	this->cooldown = cooldown;
}

int ActiveAbility::getTurnsLeft()
{
	return turnsLeft;
}

void ActiveAbility::setTurnsLeft(int useCost)
{
	this->turnsLeft = turnsLeft;
}

