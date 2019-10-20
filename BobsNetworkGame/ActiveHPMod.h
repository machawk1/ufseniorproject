#ifndef ACTIVEHPMOD_H_
#define ACTIVEHPMOD_H_

#include "ActiveAbility.h"

class ActiveHPMod : public ActiveAbility
{
	public:
		ActiveHPMod();
		ActiveHPMod(AbilityNameType name, int buyCost, int useCost, int minRange, int maxRange, int radius, int cooldown, int min, int max);
		ActiveHPMod(string file);

		virtual ~ActiveHPMod();
		
		int getID();
		
		int getMin();
		void setMin(int min);
		
		int getMax();
		void setMax(int max);
		
	protected:
		int	ID,
			min,
			max;
};

#endif /*ACTIVEHPMOD_H_*/
