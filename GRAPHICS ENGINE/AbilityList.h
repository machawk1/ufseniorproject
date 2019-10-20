#ifndef ABILITY_LIST_H_
#define ABILITY_LIST_H_

/*****************************************************************************
	Include once in main() file only
*****************************************************************************/

#include "ActiveHPMod.h"
#include "ActiveStatMod.h"
#include "PassiveAbility.h"

//ActiveHPMods								AbilityNameType		buy	use	range	rad	cool	min	max
static ActiveHPMod DefaultMeleeAbility(		DefaultMelee,		0,	1,	1,1,	0,	0,		5,	15);
static ActiveHPMod MediumMeleeAbility(		MediumMelee,		2,	2,	1,1,	0,	0,		16,	28);
static ActiveHPMod MediumRangedAbility(		MediumRanged,		1,	3,	3,12,	0,	0,		10,	20);
static ActiveHPMod LightLightningAOEAbility(LightLightningAOE,	5,	5,	0,7,	1,	2,		15,	35);
static ActiveHPMod MediumHealAbility(		MediumHeal,			3,	2,	0,3,	0,	1,		14,	26);

//ActiveStatMods								AbilityNameType		buy	use	range	rad	cool	dur	H/A/Mv/H/Mi/D	
static ActiveStatMod ActiveLightDodgeAbility(	ActiveLightDodge,	0,	0,	0,0,	0,	0,		0,	0,0,0 ,0,0 ,0);

//PassiveAbilities									AbilityNameType			buy	H /A /Mv/H /Mi/D
static PassiveAbility PassiveLightDodgeAbility(		PassiveLightDodge,		3,	0 ,0 ,0 ,0 ,0 ,5);
static PassiveAbility PassiveLightArmorAbility(		PassiveLightArmor,		8,	0 ,0 ,0 ,0 ,5 ,0);
static PassiveAbility PassiveLightMoveBoostAbility(	PassiveLightMoveBoost,	3,	0 ,0 ,1 ,0 ,0 ,0);
static PassiveAbility PassiveLightHPIncreaseAbility(PassiveLightHPIncrease,	1,	25,0 ,0 ,0 ,0 ,0);

#endif