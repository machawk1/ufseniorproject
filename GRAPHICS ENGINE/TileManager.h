/*-----------------------------------------------------------------------------
This source file is part of Code-Black (http://www.code-black.org)
Copyright (c) 2005 by the Code-Black Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/licenses/licenses.html
-----------------------------------------------------------------------------*/

#ifndef TILE_MANAGER_H
#define TILE_MANAGER_H

#include "Ogre.h"
#include "define.h"
#include "TileChunk.h"
#include "TileInterface.h"
#include "BobC.h"

using namespace Ogre;

/** Pixel per tile in the terrain-texture. */
const int PIXEL_PER_TILE =  128;
/** Pixel per col/row in the terrain-texture. */
const int PIXEL_PER_ROW  = 1024;
/** Numbers of tile textures in one row/col of the terrain texture. */
const int TEXTURES_PER_ROW = 7;
/** Size of a tile. */
const int TILE_SIZE = 32;
/** Number of chunks in worldmap (on x-axis). */
const int CHUNK_SUM_X  = 16;
/** Number of chunks in worldmap (on z-axis). */
const int CHUNK_SUM_Z  = 16;
/** Number of tiles in a chunk (on x-axis), Must be even. */
const int CHUNK_SIZE_X = 16;
/** Number of tiles in a chunk (on z-axis), Must be even */
const int CHUNK_SIZE_Z = 16;
/** Number of tiles in the worldmap (on x-axis). */
const int TILES_SUM_X  = CHUNK_SUM_X * CHUNK_SIZE_X;
/** Number of tiles in the worldmap (on z-axis). */
const int TILES_SUM_Z  = CHUNK_SUM_Z * CHUNK_SIZE_Z;
/** Radius of the area where the tiles are drawn in high quality. */
const int HIGH_QUALITY_RANGE = 3;
/** Minimal size of tile in the shrinked terrain texture. */
const int MIN_TEXTURE_PIXEL = 8;
/** LOD for the chunks. */
enum {QUALITY_LOW, QUALITY_HIGH};
enum { SELECT, PLACE, MOVE };


/**
 * TileEngine class which manages the chunks in the worldmap.
 * Because of speed reasons, the TileEngine divides the worldmap into chunks.
 * Each chunk controls a number of tiles.
 *****************************************************************************/
class TileManager
{
private:
	/**  TileEngine struct which holds the worldmap. **/
	struct WorldMap
	{
		/** Average height. **/
		float height;
		/** Column of the texture in the terrain-texture. **/
		unsigned char terrain_col;
		/** Row of the texture in the terrain-texture. **/
		unsigned char terrain_row;
		/** Toggle for where or not tile has a unit on it **/
		bool Occupied;
		/** SceneNode pointer to the unit that is on this tile **/
		SceneNode* node;

	}** m_Map;

	TileChunk m_mapchunk[CHUNK_SUM_X][CHUNK_SUM_Z];
	SceneManager* m_SceneManager;
	TileInterface* m_Interface;
	BobC* m_bob;
	std::deque<Vector3>* Walklist;
	bool bobExists;
	float Tile_Height;

	AxisAlignedBox* bounds;
	MaterialPtr m_Kartentextur;

	/** The z-stretching of the tiles. **/
	float m_StretchZ;
	int m_TileTextureSize;
	bool mHighDetails;
	bool mGrid;

public:
	TileManager();
	~TileManager();
	SceneManager* Get_pSceneManager(){ return m_SceneManager; }
	float Get_StretchZ() { return m_StretchZ;}
	float Get_Map_Height(short x, short y) { return m_Map[x][y].height; }
	float Get_Tile_Height(short x, short y);
	unsigned char Get_Map_Texture_Row(short x, short y) { return m_Map[x][y].terrain_row; }
	unsigned char Get_Map_Texture_Col(short x, short y) { return m_Map[x][y].terrain_col; }
	bool Get_Occupied(short x, short y) { return m_Map[x][y].Occupied; }
	bool Get_bobExists() { return bobExists; }
	BobC* Get_Bob_Array(){ return m_bob; }
	std::deque<Vector3>* Get_Walklist() { return Walklist; }
	SceneNode* Get_Unit(short x, short y) { return m_Map[x][y].node; }
	TileInterface* get_TileInterface(){return m_Interface;}
	TileChunk* get_TileChunk(int x, int y){ return &m_mapchunk[x][y];}

	void Set_Map_Height(short x, short y, short value) { m_Map[x][y].height = value; }
	void Set_Map_Texture_Row(short x, short y, unsigned char value) { m_Map[x][y].terrain_row = value; }
	void Set_Map_Texture_Col(short x, short y, unsigned char value) { m_Map[x][y].terrain_col = value; }
	void Set_Map_Occupied(short x, short y, bool value) { m_Map[x][y].Occupied = value; }
	void Set_Map_Unit(short x, short y, SceneNode* node) { m_Map[x][y].node = node; }
	void Set_Map_Textures();
	void Set_Bob_Array(BobC* b) { m_bob = b; }
	void Set_Walklist(std::deque<Vector3>* list) {Walklist = list; }
	void Set_bobExists(bool value) { bobExists = value; }
	bool LoadImage(Image &image, const std::string &filename);

	AxisAlignedBox *GetBounds();
	void Init(SceneManager* SceneManager, int tileTextureSize = 128, int tileStretchZ = 2);

	void CreateChunks();
	void ChangeChunks();
	void ControlChunks(Vector3 vector);

	void CreateTexture();
	void ChangeTexture();
	/** Create a terrain-texture out of tile textures. **/
	bool CreateTextureGroup(const std::string &terrain_type);
	void SetTextureSize(int pixels);
	bool quarterImageSize(const std::string strFile, const int pix, bool reportFileNotFound = false);
	/** Import a 8bit png file as heightmap **/
	void Load_Map(const std::string &png_filename);
	/** Import a heightmap. **/
	void Load_Map(char *mapData);
	void ToggleMaterial();
	void ToggleGrid();
	void addToGroupTexture(uchar* TextureGroup_data, uchar *Filter_data, Image* Texture, short pixel, short x, short y);
	void CreateMipMaps(const std::string &terrain_type);
};

#endif
