#ifndef COMMON_H_
#define COMMON_H_

//#define DEBUG

#include <iostream>

using namespace std;

static int GLOBAL_ID = 0;
const int MIN_MAP_SIZE = 10;

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
	EmptyQueue
};

enum AbilityNameType
{
	Cancel,
	Move,
	Ability
};

#endif /*COMMON_H_*/
