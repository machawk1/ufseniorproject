#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//#define DEBUG
#define DEBUG_TURN

const int MIN_MAP_SIZE = 10;
const int MIN_START_SIZE = 3;
const int AP_LIMIT = 20;
const int FRAME_RATE = 500;

static int GLOBAL_ID = 0;

enum PlayerType
{
	Human,
	CPU
};

enum ActionStateType
{
	ActionRequest,
	ActionOptions,
	PerformAction,
	ActionResults
};

enum TerrainType
{
	Grass,
	Dirt,
	Sand,
	Pavement,
	Swamp
};

enum ExceptionType
{
	FileNotFound,
	BadFile,
	IndexOutOfBounds,
	InvalidLength,
	EmptyQueue,
	BreakLoop
};

enum AbilityNameType
{
	Cancel,					// 0
	Move,					// 1
	DefaultMelee,			// 2
	MediumRanged,			// 3
	LightLightningAOE,		// 4
	MediumHeal,				// 5
	MediumMelee,			// 6
	ActiveLightDodge,		// 7
	PassiveLightDodge,		// 8
	PassiveLightArmor,		// 9
	PassiveLightMoveBoost,	// 10
	PassiveLightHPIncrease,	// 11
	PassTurn				// 12
};

#endif /*COMMON_H_*/
