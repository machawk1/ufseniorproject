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

void ActiveAbility::setMinRange(int minRange)
{
	this->minRange = minRange;
}

int ActiveAbility::getMaxRange()
{
	return maxRange;
}

void ActiveAbility::setMaxRange(int maxRange)
{
	this->maxRange = maxRange;
}

int ActiveAbility::getRadius()
{
	return radius;
}

void ActiveAbility::setRadius(int radius)
{
	this->radius = radius;
}

int ActiveAbility::getCooldown()
{
	return cooldown;
}

void ActiveAbility::setCooldown(int cooldown)
{
	this->cooldown = cooldown;
}