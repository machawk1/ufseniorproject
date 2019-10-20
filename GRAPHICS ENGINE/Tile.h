#ifndef TILE_H_
#define TILE_H_

#include "Common.h"
#include "Bob.h"

class Tile
{
	public:
		Tile();
		Tile(bool lineOfSight, bool occupied, int moveRate, int elevation, TerrainType terrain);

		~Tile();
	
		bool        getLineOfSight();
		bool        getOccupied();
		int			getID();
		int         getElevation();
		int			getX();
		int			getY();
		float		getTraversalCost();
		int       	getMoveRate();
		TerrainType getTerrain();
		Bob*        getBob();

		void setLineOfSight(bool los);
		void setOccupied(bool occupied);
		void setMoveRate(int move);
		void setElevation(int elev);
		void setLocation(int x, int y);
		void setTerrain(TerrainType t);
		void setBob(Bob *bob);
		void setTraversalCost(float cost);

		friend ostream& operator <<(ostream &out, Tile &tile)
		{
			// Line of Sight data
			if(tile.getLineOfSight())
				out << "1 ";
			else
				out << "0 ";
			
			// Occupied data
			if(tile.getOccupied())
				out << "1 ";
			else
				out << "0 ";
			
			// Movement Rate, Elevation, and Terrain data
			out << tile.getMoveRate() << " " << tile.getElevation() << " "
				<< tile.getTerrain() << "\n";

			return out;
		}

		friend istream& operator >>(istream &in, Tile &tile)
		{
			int LOS, occ, move, elev, terrain;
			
			in >> LOS >> occ >> move >> elev >> terrain;
			
			if(0 == LOS)
				tile.setLineOfSight(false);
			else
				tile.setLineOfSight(true);
			
			if(0 == occ)
				tile.setOccupied(false);
			else
				tile.setOccupied(true);
			
			tile.setMoveRate(move);
			tile.setElevation(elev);
			tile.setTerrain((TerrainType)terrain);

			return in;
		}

	private:
		bool        lineOfSight,
					occupied;
		int         ID,
					moveRate,
					elevation,
					x,
					y;
		float		traversalCost;
		TerrainType terrain;
		Bob         *bob;
};

//	Stream output
/*	Use ' ' as delimeter
	Example: 1 0 2 2 4
*/



#endif