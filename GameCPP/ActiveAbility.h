#ifndef ACTIVEABILITY_H_
#define ACTIVEABILITY_H_

#include "Ability.h"

class ActiveAbility : public Ability
{
	public:
		ActiveAbility();
		
		virtual ~ActiveAbility();
		
		int getUseCost();
		void setUseCost(int useCost);
		
		int getMinRange();
		void setMinRange(int minRange);
		
		int getMaxRange();
		void setMaxRange(int maxRange);
		
		int getRadius();
		void setRadius(int radius);
		
		int getCooldown();
		void setCooldown(int cooldown);
		
		int getTurnsLeft();
		void setTurnsLeft(int turnsLeft);	
		
	protected:
		int	useCost,
			minRange,
			maxRange,
			radius,
			cooldown,
			turnsLeft;
};

#endif /*ACTIVEABILITY_H_*/
