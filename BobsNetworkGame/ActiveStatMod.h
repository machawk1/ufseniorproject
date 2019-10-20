#ifndef ACTIVESTATMOD_H_
#define ACTIVESTATMOD_H_

#include "ActiveAbility.h"

class ActiveStatMod : public ActiveAbility
{
	public:
		ActiveStatMod();
		ActiveStatMod(AbilityNameType name, int buyCost, int useCost, int minRange, int maxRange, int radius,
			int cooldown, int duration, int hpMod, int apMod, int movementMod, int toHitMod, int mitigationMod, int dodgeMod);
		ActiveStatMod(string file);

		virtual ~ActiveStatMod();
	
		int getID();
		
		int getHpMod();
		void setHpMod(int hpMod);
			
		int getApMod();
		void setApMod(int apMod);
			
		int getMovementMod();
		void setMovementMod(int movementMod);
	
		int getToHitMod();
		void setToHitMod(int toHitMod);
	
		int getMitigationMod();
		void setMitigationMod(int mitigationMod);
			
		int getDodgeMod();
		void setDodgeMod(int dodgeMod);
			
		int getDuration();
		void setDuration(int duration);
	
	protected:	
		int	ID,
			hpMod,
			apMod,
			movementMod,
			toHitMod,
			mitigationMod,
			dodgeMod,
			duration;
};

#endif /*ACTIVESTATMOD_H_*/
