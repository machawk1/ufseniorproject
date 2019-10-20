#ifndef BOBC_H_
#define BOBC_H_

#include <string>

#define MAX_BOB_AMOUNT 15

using std::string;

class BobC {


public:
	string playerHandle;
		
		int	ID,
			hpBase,
			hpCurrent,
			apBase,
			apCurrent,
			toHitBase,
			toHitCurrent,
			mitigationBase,
			mitigationCurrent,
			moveBase,
			moveCurrent,
			dodgeBase,
			dodgeCurrent,
			passiveSize,
			activeHPSize,
			activeStatSize;

		short x, y;
		bool active;

		string model;

		Entity* mEntity;                // The entity of this bob
        SceneNode* mNode;               // The SceneNode of this bob
        AnimationState* mAnimationState;
			
		//PassiveAbility *passiveAbilities;
		//ActiveHPMod *activeHPMods;
		//ActiveStatMod *activeStatMods;

		
};

#endif