#ifndef MAP_H_
#define MAP_H_

#include <string>
#include <fstream>
#include "Tile.h"

class Map
{
	public:
		Map();
		Map(string mapFileName);
		Map(int length, int height, TerrainType terrain);

		~Map();

		Tile* getTile(int posX, int posY);
		int getLength();
		int getHeight();
		int getID();		

		void saveMap(string mapFileName);

		void operator =(Map &map);

		void resetTraversalCost();
		
	private:
		Tile **TileArray;
		int ID,
			x,
			y;
};

#endif