#include "Map.h"

Map::Map()
{
	ID = GLOBAL_ID++;
	
	int i,j;

	x = MIN_MAP_SIZE;
	y = MIN_MAP_SIZE;

	/* Begin memory allocation */
	TileArray = new Tile*[x];

	for(i = 0; i < x; i++)
	{
		TileArray[i] = new Tile[y];
		for(j = 0; j < y; j++)
		{
			TileArray[i][j] = Tile(false, false, 1, 0, Grass);
			TileArray[i][j].setLocation(i,j);
		}
	}
	/* End memory allocation */
}

Map::Map(string mapFileName)
{
	ID = GLOBAL_ID++;
	
	/*bool tempLOS, tempOCC;
	int tempMove, tempElev, i, j;
	TerrainType tempTerrain;*/
	int i, j;
	//char temp[80];
	Tile temp;

	/* Open File */
	ifstream infile;
	infile.open(mapFileName.c_str(), ios::in);

	if(!infile.is_open())
	{
		throw FileNotFound;
	}

	/* Read map dimensions */
	infile >> x >> y;

	/* Allocate memory */
	TileArray = new Tile*[x];
	for(i = 0; i < x; i++)
	{
		TileArray[i] = new Tile[y];
		for(j = 0; j < y; j++)
		{
			TileArray[i][j] = Tile();
			infile >> TileArray[i][j];
		}
	}
}

Map::Map(int length, int height, TerrainType terrain)
{
	ID = GLOBAL_ID++;
	
	int i, j;
	if(length < MIN_MAP_SIZE)
	{
		length = MIN_MAP_SIZE;
	}
	if(height < MIN_MAP_SIZE)
	{
		height = MIN_MAP_SIZE;
	}

	/* Begin memory allocation */
	TileArray = new Tile*[length];

	for(i = 0; i < length; i++)
	{
		TileArray[i] = new Tile[height];
	}
	/* End memory allocation */

	x = length;
	y = height;

	for(i = 0; i < x; i++)
	{
		for(j = 0; j < y; j++)
		{
			TileArray[i][j] = Tile(false, false, 1, 0, terrain);
			TileArray[i][j].setLocation(i,j);
		}
	}
}

Map::~Map()
{
	#ifdef DEBUG
		cout << "Map destructed!" << endl;
	#endif
	
	int i;
	for(i = 0; i < x; i++)
	{
		if(y > 0)
		{
			delete [] TileArray[i];
		}
		TileArray[i] = NULL;
	}

	if(x > 0)
	{
		delete [] TileArray;
	}
	TileArray = NULL;
}

void Map::resetTraversalCost()
{
	for(int i=0; i<x; i++)
	{
		for(int j=0; j<y; j++)
		{
			TileArray[i][j].setTraversalCost(0);
		}
	}
}

Tile* Map::getTile(int posX, int posY)
{
	/* Check index */
	if(posX < 0 || posX >= x || posY < 0 || posY >= y)
	{
		throw IndexOutOfBounds;
	}

	return &TileArray[posX][posY];
}

int Map::getLength()
{
	return x;
}

int Map::getHeight()
{
	return y;
}

int Map::getID()
{
	return ID;	
}

void Map::saveMap(string mapFileName)
{
	int i, j;

	/* Open file */
	ofstream outfile;
	outfile.open(mapFileName.c_str(), ios::out);

	/* Check file */
	if(!outfile.is_open())
	{
		throw BadFile;
	}

	/* Write out file */
	outfile << x << " " << y << "\n";

	for(i = 0; i < x; i++)
	{
		for(j = 0; j < y; j++)
		{
			outfile << TileArray[i][j];
		}
	}
}

void Map::operator =(Map &map)
{
	int i,j;
	for(i = 0; i < this->x; i++)
	{
		if(this->y > 0)
		{
			delete [] TileArray[i];
		}
		TileArray[i] = NULL;
	}
	if(this->x > 0)
	{
		delete [] TileArray;
	}

	this->x = map.x;
	this->y = map.y;

	TileArray = new Tile*[this->x];

	for(i = 0; i < this->x; i++)
	{
		TileArray[i] = new Tile[y];
		for(j = 0; j < this->y; j++)
		{
			TileArray[i][j] = *map.getTile(i,j);
		}
	}
}