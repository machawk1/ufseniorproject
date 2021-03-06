#include "Tile.h"

Tile::Tile()
{
	ID = GLOBAL_ID++;
	lineOfSight = false;
	occupied    = false;
	moveRate    = 1;
	elevation   = 0;
	terrain     = Dirt;
	resident    = NULL;
	traversalCost = 0;
	x = -1;
	y = -1;
}

Tile::Tile(bool lineOfSight, bool occupied, int moveRate, int elevation, TerrainType terrain)
{
	ID = GLOBAL_ID++;
	this->lineOfSight = lineOfSight;
	this->occupied    = occupied;
	this->moveRate    = moveRate;
	this->elevation   = elevation;
	this->terrain     = terrain;
	this->resident    = NULL;
	traversalCost = 0;
	x = -1;
	y = -1;
}

Tile::~Tile()
{
	#ifdef DEBUG
		cout << "Tile destructed!" << endl;
	#endif
}

bool Tile::getLineOfSight()
{
	return lineOfSight;
}

bool Tile::getOccupied()
{
	return occupied;
}

void Tile::setOccupied(bool occupied)
{
	this->occupied = occupied;
}

int Tile::getID()
{
	return ID;
}

float Tile::getMoveRate()
{
	return moveRate;
}

int Tile::getElevation()
{
	return elevation;
}

int Tile::getX()
{
	return x;
}

int Tile::getY()
{
	return y;
}

float Tile::getTraversalCost()
{
	return traversalCost;
}

TerrainType Tile::getTerrain()
{
	return terrain;
}

Bob* Tile::getBob()
{
	return resident;
}

void Tile::setBob(Bob *resident)
{
	this->resident = resident; 
}

void Tile::setLineOfSight(bool los)
{
	lineOfSight = los;
}

void Tile::setMoveRate(int move)
{
	moveRate = move;
}

void Tile::setElevation(int elev)
{
	elevation = elev;
}

void Tile::setLocation(int x, int y)
{
	this->x = x;
	this->y = y;
}

void Tile::setTraversalCost(float cost)
{
	traversalCost = cost;
}

void Tile::setTerrain(TerrainType t)
{
	terrain = t;
}