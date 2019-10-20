#ifndef PASSIVEABILITY_H_
#define PASSIVEABILITY_H_

#include "Ability.h"

class PassiveAbility : public Ability
{
public:
	PassiveAbility();
	PassiveAbility(AbilityNameType name, int buyCost, int hpMod, int apMod, int movementMod, int toHitMod, int mitigationMod, int dodgeMod);
	PassiveAbility(string file);
	
	virtual ~PassiveAbility();
	
	int getID();
	
	int getHpMod();
	void setHpMod(int hpMod);
	
	int getApMod();
	void setApMod(int apMod);
	
	int getMovementMod();
	void setMovementMod(int movementMod);
	
	int getToHitMod();
	void setToHitMod(int hpMod);
	
	int getMitigationMod();
	void setMitigationMod(int mitigationMod);
	
	int getDodgeMod();
	void setDodgeMod(int dodgeMod);
	
protected:
	int	ID,
		hpMod,
		apMod,
		movementMod,
		toHitMod,
		mitigationMod,
		dodgeMod;	
};

#endif /*PASSIVEABILITY_H_*/
