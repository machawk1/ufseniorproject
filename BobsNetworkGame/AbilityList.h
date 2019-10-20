#ifndef ABILITY_LIST_H_
#define ABILITY_LIST_H_

/*****************************************************************************
	Include once in main() file only
*****************************************************************************/

#include "ActiveHPMod.h"
#include "ActiveStatMod.h"
#include "PassiveAbility.h"

//ActiveHPMods								AbilityNameType		buy	use	range	rad	cool	min	max
static ActiveHPMod DefaultMeleeAbility(		DefaultMelee,		0,	5,	1,1,	0,	0,		10,	20);
static ActiveHPMod MediumMeleeAbility(		MediumMelee,		2,	2,	1,1,	0,	0,		16,	28);
static ActiveHPMod MediumRangedAbility(		MediumRanged,		1,	6,	3,10,	0,	0,		5,	15);
static ActiveHPMod LightLightningAOEAbility(LightLightningAOE,	5,	7,	0,3,	1,	2,		20,	30);
static ActiveHPMod MediumHealAbility(		MediumHeal,			3,	5,	0,1,	0,	1,		10,	20);

//ActiveStatMods								AbilityNameType		buy	use	range	rad	cool	dur	H/A/Mv/H/Mi/D	
static ActiveStatMod ActiveLightDodgeAbility(	ActiveLightDodge,	0,	0,	0,0,	0,	0,		0,	0,0,0 ,0,0 ,0);

//PassiveAbilities									AbilityNameType			buy	H /A /Mv/H /Mi/D
static PassiveAbility PassiveLightDodgeAbility(		PassiveLightDodge,		3,	0 ,0 ,0 ,0 ,0 ,5);
static PassiveAbility PassiveLightArmorAbility(		PassiveLightArmor,		8,	0 ,0 ,0 ,0 ,5 ,0);
static PassiveAbility PassiveLightMoveBoostAbility(	PassiveLightMoveBoost,	3,	0 ,0 ,1 ,0 ,0 ,0);
static PassiveAbility PassiveLightHPIncreaseAbility(PassiveLightHPIncrease,	1,	25,0 ,0 ,0 ,0 ,0);

#endif