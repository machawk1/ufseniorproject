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

#ifdef LOG_TIMING
#include <ctime>
#endif
#include <cmath>
#include <iostream>
#include "OgreHardwarePixelBuffer.h"
#include "TileChunk.h"
#include "TileManager.h"
#include "TileInterface.h"
#include "logger.h"

//#define LOW_QUALITY_RENDERING

///=================================================================================================
/// Constructor
///=================================================================================================
TileManager::TileManager() 
{
	m_Interface = NULL;
}

///=================================================================================================
/// Destructor
///=================================================================================================
TileManager::~TileManager()
{
	for(int x = 0; x < TILES_SUM_X + 1; ++x) { delete[] m_Map[x]; }
	delete[] m_Map;

	delete m_Interface;
}

///=================================================================================================
/// Init the TileEngine.
///=================================================================================================
void TileManager::Init(SceneManager* SceneMgr, int tileTextureSize, int tileStretchZ)
{
	Logger::log().headline("Init TileEngine:");
	m_SceneManager = SceneMgr;
	m_StretchZ = tileStretchZ;
	m_TileTextureSize = tileTextureSize;
	mHighDetails = true;
	mGrid = false;
	bobExists = false;

	m_Interface = new TileInterface(this);
	m_Interface->Init();

	srand(1);
	Logger::log().info() << "Creating map";
	m_Map = new WorldMap*[TILES_SUM_X+1];
	for (int x = 0; x < TILES_SUM_X + 1; ++x) { m_Map[x] = new WorldMap[TILES_SUM_Z + 1]; }
	Load_Map(FILE_HEIGHT_MAP);
	/////////////////////////////////////////////////////////////////////////
	/// Create all TextureGroups.
	/////////////////////////////////////////////////////////////////////////
	std::string strTextureGroup = "terrain";
	Logger::log().info() << "Creating texture group " << strTextureGroup;
	CreateTextureGroup(strTextureGroup); // only used once, to create a new texture group (if a texture has changed)
	CreateMipMaps(strTextureGroup); // has to be called everytime
	/////////////////////////////////////////////////////////////////////////
	/// Create TileChunks.
	/////////////////////////////////////////////////////////////////////////
	Logger::log().info() << "Creating tile-chunks";
	CreateChunks();
	/////////////////////////////////////////////////////////////////////////
	/// Init is done.
	/////////////////////////////////////////////////////////////////////////
	Logger::log().info() << "Init done.";
	Logger::log().headline("Running TileEngine:");
}

///=================================================================================================
/// Create the worldmap.
///=================================================================================================
void TileManager::Load_Map(const std::string &png_filename)
{
	Image image;
	if (!LoadImage(image, png_filename))
	{
		Logger::log().error() << "Heightmap '" << png_filename << "' was not found.";
		return;
	}
	uchar* heightdata_temp = image.getData();
	int dimx = image.getWidth();
	int dimy = image.getHeight();
	int posX = 0, posY;
	short Map[TILES_SUM_X+2][TILES_SUM_Z+2];
	/////////////////////////////////////////////////////////////////////////
	/// Fill the heightdata buffer with the image-color.
	/////////////////////////////////////////////////////////////////////////
	for(int x = 0; x < TILES_SUM_X+2; ++x)
	{
		posY =0;
		for(int y = 0; y < TILES_SUM_Z+2; ++y)
		{
			if ( x && x != TILES_SUM_X && y && y != TILES_SUM_Z)
			{
				Map[x][y] = heightdata_temp[posY * dimx + posX];
			}
			else
			{
				Map[x][y] = 0;
			}
			if (++posY >= dimy) posY =0; // if necessary, repeat the image.
		}
		if (++posX >= dimx) posX =0; // if necessary, repeat the image.
	}

	for (int x = 0; x < TILES_SUM_X+1; ++x)
	{
		for (int y = 0; y < TILES_SUM_Z+1; ++y)
		{
			m_Map[x][y].height = (Map[x][y] + Map[x][y+1] + Map[x+1][y] + Map[x+1][y+1] )/ 4;
			m_Map[x][y].node = NULL;
			m_Map[x][y].Occupied = false;
		}
	}
	Set_Map_Textures();
}

///=================================================================================================
/// Set the textures for the given height.
///=================================================================================================
void TileManager::Set_Map_Textures()
{
	short height;
	for (int x = 0; x < TILES_SUM_X; ++x)
	{
		for (int y = 0; y < TILES_SUM_Z; ++y)
		{
			height = m_Map[x][y].height;
			/////////////////////////////////////////////////////////////////////////
			// Highland.
			/////////////////////////////////////////////////////////////////////////
			if (height > LEVEL_MOUNTAIN_TOP)
			{
				m_Map[x][y].terrain_col = 0;
				m_Map[x][y].terrain_row = 1;
			}
			else if (height > LEVEL_MOUNTAIN_MID)
			{
				if (rand() % 2)
				{
					m_Map[x][y].terrain_col =6;
					m_Map[x][y].terrain_row =0;
				}
				else
				{
					m_Map[x][y].terrain_col = 0;
					m_Map[x][y].terrain_row = 0;
				}
			}
			else if (height > LEVEL_MOUNTAIN_DWN)
			{
				m_Map[x][y].terrain_col = rand() % 2 + 4;
				m_Map[x][y].terrain_row = 2;
			}
			/////////////////////////////////////////////////////////////////////////
			/// Plain.
			/////////////////////////////////////////////////////////////////////////
			else if (height > LEVEL_PLAINS_TOP)
			{ // Plain
				m_Map[x][y].terrain_col = rand() % 2;
				m_Map[x][y].terrain_row = 2;
			}
			else if (height > LEVEL_PLAINS_MID)
			{
				m_Map[x][y].terrain_col = 6;
				m_Map[x][y].terrain_row = 3;
			}
			else if (height > LEVEL_PLAINS_DWN)
			{
				m_Map[x][y].terrain_col = 0;
				m_Map[x][y].terrain_row = 4;
			}
			else if (height > LEVEL_PLAINS_SUB)
			{
				m_Map[x][y].terrain_col = 3;
				m_Map[x][y].terrain_row = 3;
			}
			/////////////////////////////////////////////////////////////////////////
			/// Sea-Ground.
			/////////////////////////////////////////////////////////////////////////
			else
			{
				m_Map[x][y].terrain_col = 3;
				m_Map[x][y].terrain_row = 3;
			}
		}
	}
}

///=================================================================================================
/// Create all chunks.
///=================================================================================================
void TileManager::CreateChunks()
{
	#ifdef LOG_TIMING
	long time = clock();
	#endif
	TileChunk::m_TileManagerPtr = this;
	TileChunk::m_bounds = new AxisAlignedBox(
		 0, 0 ,0,
			TILE_SIZE * CHUNK_SIZE_X, 255 * m_StretchZ,  TILE_SIZE * CHUNK_SIZE_Z);

	for (short x = 0; x < CHUNK_SUM_X; ++x)
	{
		for (short y = 0; y < CHUNK_SUM_Z; ++y)
		{
			m_mapchunk[x][y].Create(x, y);
		}
	}
	delete TileChunk::m_bounds;
	#ifdef LOG_TIMING
	Logger::log().info() << "Time to create Chunks: " << clock()-time << " ms";
	#endif
}

///=================================================================================================
/// Change all Chunks.
///=================================================================================================
void TileManager::ChangeChunks()
	{
	#ifdef LOG_TIMING
	long time = clock();
	#endif
	TileChunk::m_TileManagerPtr = this;
	TileChunk::m_bounds = new AxisAlignedBox(
		 -TILE_SIZE * CHUNK_SIZE_X, 0               , -TILE_SIZE * CHUNK_SIZE_Z,
			TILE_SIZE * CHUNK_SIZE_X, 100 * m_StretchZ,  TILE_SIZE * CHUNK_SIZE_Z);

	unsigned char value;

	for (int a = 0 ; a < TILES_SUM_X; ++a)
	{
		for (int b = 0; b < TILES_SUM_Z; ++b)
		{
			value = Get_Map_Height(a, b)+ 1;
			if (value > 220) value = 0;
			Set_Map_Height(a, b, value);
		}
	}

	Set_Map_Textures();
	for (short x = 0; x < CHUNK_SUM_X; ++x)
	{
		for (short y = 0; y < CHUNK_SUM_Z; ++y)
		{
			m_mapchunk[x][y].Change();
		}
	}
	delete TileChunk::m_bounds;
	#ifdef LOG_TIMING
	Logger::log().info() << "Time to change Chunks: " << clock()-time << " ms";
	#endif
}

///=================================================================================================
/// Change Tile and Environmet textures.
///=================================================================================================
void TileManager::ChangeTexture()
{
	static bool once = false;
	const std::string strFilename = "terrain_texture_016.png";
	if (once) return; else once=true;
	#ifdef LOG_TIMING
	long time = clock();
	#endif
	Image tMap;
	if (!LoadImage(tMap, strFilename))
	{
		Logger::log().error() << "Group texture '" << strFilename << "' was not found.";
		return;
	}
	MaterialPtr mMaterial = MaterialManager::getSingleton().getByName("Land_HighDetails128");
	std::string texName = "testMat";
	TexturePtr mTexture = TextureManager::getSingleton().loadImage(texName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, tMap, TEX_TYPE_2D, 3,1.0f);
	mMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(texName);
//	mMaterial->unload();
	mMaterial->load();
	#ifdef LOG_TIMING
	Logger::log().info() << "Time to change Texture: " << clock()-time << " ms";
	#endif
}

///=================================================================================================
/// +/- 5 Chunks around the camera are drawn in high quality.
///=================================================================================================
void TileManager::ControlChunks(Vector3 vector)
{
	/////////////////////////////////////////////////////////////////////////
	/// Just for testing...
	/////////////////////////////////////////////////////////////////////////
	//	ChangeChunks();

	int x = (int)vector.x / (TILE_SIZE * CHUNK_SIZE_X)+1;
	int y = (int)vector.z / (TILE_SIZE * CHUNK_SIZE_Z)+1;
	if ( x > CHUNK_SUM_X || y > CHUNK_SUM_Z) { return; }

	for(int cx = 0; cx < CHUNK_SUM_X; ++cx)
	{
		for (int cy = 0; cy < CHUNK_SUM_Z; ++cy)
		{
			#ifndef LOW_QUALITY_RENDERING
			if (cx >= x - HIGH_QUALITY_RANGE && cx <= x + HIGH_QUALITY_RANGE
			&& cy >= y - HIGH_QUALITY_RANGE && cy <= y + HIGH_QUALITY_RANGE)
			{
				m_mapchunk[cx][cy].Attach(QUALITY_HIGH);
			}
			else
			#endif
			{
				m_mapchunk[cx][cy].Attach(QUALITY_LOW);
			}
		}
	}
}

///=================================================================================================
/// If the file exists load it into an image.
///=================================================================================================
bool TileManager::LoadImage(Image &image, const std::string &strFilename)
{
	std::string strTemp = PATH_TILE_TEXTURES + strFilename;
	std::ifstream chkFile;
	chkFile.open(strTemp.data());
	if (!chkFile){ chkFile.close(); return false; }
	chkFile.close();
	image.load(strFilename, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	return true;
}

///=================================================================================================
/// Create the texture-file out of the single textures + filter texture.
///=================================================================================================
bool TileManager::CreateTextureGroup(const std::string &terrain_type)
{
	std::string strFilename;
	
	/////////////////////////////////////////////////////////////////////////
	/// Shrink all filter-textures.
	/////////////////////////////////////////////////////////////////////////
	for (int pix = PIXEL_PER_TILE / 2;  pix >= MIN_TEXTURE_PIXEL; pix/=2)
	{
		quarterImageSize("filter", pix, true);
	}
	/////////////////////////////////////////////////////////////////////////
	/// Shrink all tile-textures.
	/////////////////////////////////////////////////////////////////////////
	for (int pix = PIXEL_PER_TILE / 2, sum;  pix >= MIN_TEXTURE_PIXEL; pix/=2)
	{
		sum= -1;
		while(quarterImageSize(terrain_type + "_" + StringConverter::toString(++sum, 2, '0'), pix))		;
	}
	/////////////////////////////////////////////////////////////////////////
	/// Create group-texture in various sizes.
	/////////////////////////////////////////////////////////////////////////
	#ifdef LOG_TIMING
	long time = clock();
	#endif
	int i=-1, x=0, y = 0;
	int pix = PIXEL_PER_TILE;
	Image Filter, Texture, TextureGroup;
	while (pix >= MIN_TEXTURE_PIXEL)
	{
		uchar* TextureGroup_data = new uchar[PIXEL_PER_ROW * PIXEL_PER_ROW *4];
		TextureGroup.loadDynamicImage(TextureGroup_data, pix * 8, pix * 8,PF_A8B8G8R8);
		strFilename = "filter_" + StringConverter::toString(pix, 3, '0') + ".png";
		if (!LoadImage(Filter, strFilename))
		{
			Logger::log().error() << "Filter texture '" << strFilename << "' was not found.";
			return false;
		}
		uchar* Filter_data = Filter.getData();
		i = -1; x=0, y = 0;
		while(1)
		{
			strFilename = terrain_type;
			strFilename+= "_"+ StringConverter::toString(++i, 2, '0');
			strFilename+= "_"+ StringConverter::toString(pix, 3, '0') + ".png";
			if (!LoadImage(Texture, strFilename)) { break; }
			addToGroupTexture(TextureGroup_data, Filter_data, &Texture, pix, x, y);
			if (++x == TEXTURES_PER_ROW)
			{
				if (++y == TEXTURES_PER_ROW) { break; }
				x = 0;
			}
		}
		strFilename = PATH_TILE_TEXTURES + terrain_type + "_texture";
		strFilename+= "_"+ StringConverter::toString(pix, 3, '0')+".png";
		TextureGroup.save(strFilename);
		
		delete []TextureGroup_data;
		pix /= 2;
	}
	#ifdef LOG_TIMING
	Logger::log().info() << "Time to Create Texture-Groups: " << clock()-time << " ms";
	#endif
	return true;
}

///=================================================================================================
/// Quarter the image size.
///=================================================================================================
bool TileManager::quarterImageSize(std::string strFileName, const int pix, bool reportFileNotFound)
{
	Image sizeHalf, sizeFull;
	std::string strTemp = strFileName + "_"+ StringConverter::toString(pix+pix, 3, '0') + ".png";
	if (!LoadImage(sizeFull, strTemp))
	{
		if (reportFileNotFound)
			Logger::log().error() << "Texture to scale '" << strTemp << "' was not found.";
		else
			Logger::log().info() << "Texture scaling stopped at '" << strTemp << "'";
		return false;
	}
	const uchar* sizeFull_data = sizeFull.getData();
	uchar* sizeHalf_data = new uchar[pix * pix *3];
	/////////////////////////////////////////////////////////////////////////
	/// calculate arithmetic mean for new image.
	/// (2x2 pixels fullsize image => 1 pixel shrinked image).
	/////////////////////////////////////////////////////////////////////////
	for (int x = 0; x < pix; ++x)
	{
		for (int y = 0; y < pix; ++y)
		{
			for (int z = 0; z < 3; ++z)
			{
				sizeHalf_data[3* (pix * y+ x) + z] = (
				sizeFull_data[3* (2*pix * 2*y + 2* x) + z] +
				sizeFull_data[3* (2*pix * 2*y + 2* x+1) + z] +
				sizeFull_data[3* (2*pix * 2*y + 2*pix + 2* x) + z] +
				sizeFull_data[3* (2*pix * 2*y + 2*pix + 2* x+1) + z]) / 4;
			}
		}
	}
	sizeHalf.loadDynamicImage(sizeHalf_data, pix, pix,PF_B8G8R8);
	strTemp = PATH_TILE_TEXTURES+ strFileName+ "_"+ StringConverter::toString(pix, 3, '0')+ ".png";
	sizeHalf.save(strTemp);
	delete[] sizeHalf_data;
	return true;
}

///=================================================================================================
/// Create MipMaps for tile textures
///=================================================================================================
void TileManager::CreateMipMaps(const std::string &terrain_type)
{
	// Load tile texture images in all resolutions
	enum { Pix008, Pix016, Pix032, Pix064, Pix128 };
	const int SUM_RESOLUTIONS = 5;
	Image TileImage[SUM_RESOLUTIONS];
	Image GridImage[SUM_RESOLUTIONS];
	std::string strFilename;
	TexturePtr TileTexture;

	for (int i = 0; i < SUM_RESOLUTIONS; ++i)
	{
		strFilename = terrain_type;
		strFilename+= "_texture_"+ StringConverter::toString((int)pow(2, i+3), 3, '0') + ".png";
		if (!LoadImage(TileImage[i], strFilename))
		{
			Logger::log().error() << "Ground texture '" << strFilename << "' was not found.";
			return;
		}
	}

	for (int i = 0; i < SUM_RESOLUTIONS; ++i)
	{
		strFilename = "grid_";
		strFilename+= StringConverter::toString((int)pow(2, i+3), 3, '0') + ".png";
		if (!LoadImage(GridImage[i], strFilename))
		{
			Logger::log().error() << "Grid texture '" << strFilename << "' was not found.";
			return;
		}
	}

	TileTexture = TextureManager::getSingleton().createManual("grid_128.png",
			 "General",TEX_TYPE_2D, 128,128, 4, PF_R8G8B8A8, TU_STATIC_WRITE_ONLY);
	TileTexture->getBuffer(0,0)->blitFromMemory(GridImage[Pix128].getPixelBox());
	TileTexture->getBuffer(0,1)->blitFromMemory(GridImage[Pix064].getPixelBox());
	TileTexture->getBuffer(0,2)->blitFromMemory(GridImage[Pix032].getPixelBox());
	TileTexture->getBuffer(0,3)->blitFromMemory(GridImage[Pix016].getPixelBox());
	TileTexture->getBuffer(0,4)->blitFromMemory(GridImage[Pix008].getPixelBox());

// TODO: put it into the for() loop.
	// create mipmaps for 128x128 pixel textures
	TileTexture = TextureManager::getSingleton().createManual(terrain_type+"_texture_128.png",
			 "General",TEX_TYPE_2D, 1024,1024, 4, PF_R8G8B8A8, TU_STATIC_WRITE_ONLY);
	TileTexture->getBuffer(0,0)->blitFromMemory(TileImage[Pix128].getPixelBox());
	TileTexture->getBuffer(0,1)->blitFromMemory(TileImage[Pix064].getPixelBox());
	TileTexture->getBuffer(0,2)->blitFromMemory(TileImage[Pix032].getPixelBox());
	TileTexture->getBuffer(0,3)->blitFromMemory(TileImage[Pix016].getPixelBox());
	TileTexture->getBuffer(0,4)->blitFromMemory(TileImage[Pix008].getPixelBox());

	// create mipmaps for 64x64 pixel textures
	TileTexture = TextureManager::getSingleton().createManual(terrain_type+"_texture_064.png",
			 "General",TEX_TYPE_2D, 512, 512, 3, PF_R8G8B8A8, TU_STATIC_WRITE_ONLY);
	TileTexture->getBuffer(0,0)->blitFromMemory(TileImage[Pix064].getPixelBox());
	TileTexture->getBuffer(0,1)->blitFromMemory(TileImage[Pix032].getPixelBox());
	TileTexture->getBuffer(0,2)->blitFromMemory(TileImage[Pix016].getPixelBox());
	TileTexture->getBuffer(0,3)->blitFromMemory(TileImage[Pix008].getPixelBox());

	// create mipmaps for 32x32 pixel textures
	TileTexture = TextureManager::getSingleton().createManual(terrain_type+"_texture_032.png",
			 "General",TEX_TYPE_2D, 256, 256, 2, PF_R8G8B8A8, TU_STATIC_WRITE_ONLY);
	TileTexture->getBuffer(0,0)->blitFromMemory(TileImage[Pix032].getPixelBox());
	TileTexture->getBuffer(0,1)->blitFromMemory(TileImage[Pix016].getPixelBox());
	TileTexture->getBuffer(0,2)->blitFromMemory(TileImage[Pix008].getPixelBox());

	// create mipmaps for 16x16 pixel textures
	TileTexture = TextureManager::getSingleton().createManual(terrain_type+"_texture_016.png",
			 "General",TEX_TYPE_2D, 128, 128, 1, PF_R8G8B8A8, TU_STATIC_WRITE_ONLY);
	TileTexture->getBuffer(0,0)->blitFromMemory(TileImage[Pix016].getPixelBox());
	TileTexture->getBuffer(0,1)->blitFromMemory(TileImage[Pix008].getPixelBox());

	// create mipmaps for 8x8 pixel textures
	TileTexture = TextureManager::getSingleton().createManual(terrain_type+"_texture_008.png",
			 "General",TEX_TYPE_2D, 64, 64, 0, PF_R8G8B8A8, TU_STATIC_WRITE_ONLY);
	TileTexture->getBuffer(0,0)->blitFromMemory(TileImage[Pix008].getPixelBox());

	// create mipmaps for grid texture
}

///=================================================================================================
/// Add a texture to the terrain-texture.
///=================================================================================================
inline void TileManager::addToGroupTexture(uchar* TextureGroup_data, uchar *Filter_data, Image* Texture, short pix, short x, short y)
{
	unsigned long index1, index2;
	uchar* Texture_data = Texture->getData();
	int space = pix / 16;

	/////////////////////////////////////////////////////////////////////////
	/// Texture size 8 pix.
	/////////////////////////////////////////////////////////////////////////
	if (pix <= 8)
	{
		for (int i = 0; i < pix; ++i)
		{
			for (int j = 0; j < pix; ++j)
			{
				index1 = 4*(pix * 8)* (pix + 1) * y
						 + 4* (pix * 8) * (i + space)
						 + 4* x * (pix + 1)
						 + 4* (j + space);
				index2 = 3* pix * i + 3 * j;
				TextureGroup_data[  index1] = Texture_data[index2];
				TextureGroup_data[++index1] = Texture_data[index2 + 1];
				TextureGroup_data[++index1] = Texture_data[index2 + 2];
				TextureGroup_data[++index1] = Filter_data [index2];
			}
		}

		for (int j = 0; j < pix; ++j)
		{
			int i = pix;
			index1 = 4*(pix * 8)* (pix+1) * y
						 + 4* (pix * 8) * (i + space)
						 + 4* x * (pix + 1)
						 + 4* (j + space);
			index2 = 3* pix * (i-1) + 3 * j;
			TextureGroup_data[  index1] = Texture_data[index2];
			TextureGroup_data[++index1] = Texture_data[index2 + 1];
			TextureGroup_data[++index1] = Texture_data[index2 + 2];
			TextureGroup_data[++index1] = Filter_data [index2];
		}

		for (int i = 0; i < pix; ++i)
		{
			int j = pix;
			index1 = 4*(pix * 8)* (pix + 1) * y
						 + 4* (pix * 8) * (i + space)
						 + 4* x * (pix + 1)
						 + 4* (j + space);
			index2 = 3* pix * i + 3 * (j-1);
			TextureGroup_data[  index1] = Texture_data[index2];
			TextureGroup_data[++index1] = Texture_data[index2 + 1];
			TextureGroup_data[++index1] = Texture_data[index2 + 2];
			TextureGroup_data[++index1] = Filter_data [index2];
		}

		int i = pix;
		int j = pix;
		index1 = 4*(pix * 8)* (pix + 1) * y
						 + 4* (pix * 8) * (i + space)
						 + 4* x * (pix + 1)
						 + 4* (j + space);
		index2 = 3* pix * (i-1) + 3 * (j-1);
		TextureGroup_data[  index1] = Texture_data[index2];
		TextureGroup_data[++index1] = Texture_data[index2 + 1];
		TextureGroup_data[++index1] = Texture_data[index2 + 2];
		TextureGroup_data[++index1] = Filter_data [index2];
	}

	/////////////////////////////////////////////////////////////////////////
	/// Texture size > 8 pix.
	/////////////////////////////////////////////////////////////////////////
	else
	{
			for (int i = 0; i < pix; ++i)
		{
			for (int j = 0; j < pix; ++j)
			{
				index1 = 4*(pix * 8)* (pix + 2 * space) * y
							+ 4* (pix * 8) * (i + space)
							+ 4* x * (pix + 2* space)
							+ 4* (j + space);
				index2 = 3* pix * i + 3 * j;
				TextureGroup_data[  index1] = Texture_data[index2];
				TextureGroup_data[++index1] = Texture_data[index2 + 1];
				TextureGroup_data[++index1] = Texture_data[index2 + 2];
				TextureGroup_data[++index1] = Filter_data [index2];
			}
		}
		/////////////////////////////////////////////////////////////////////////
		/// left border creation
		/////////////////////////////////////////////////////////////////////////
		for (int i = 0; i != pix; ++i)
		{
			for (int j = 0; j!= space ; ++j)
			{
				index1 = 4* (pix * 8) * (pix + 2 * space) * y
							+ 4* (pix * 8) * (i + space)
							+ 4* x * (pix + 2* space) +
								4* j;
				index2 = 3* pix * i + 3 * (pix - space + j);
				TextureGroup_data[  index1] = Texture_data[index2];
				TextureGroup_data[++index1] = Texture_data[index2 + 1];
				TextureGroup_data[++index1] = Texture_data[index2 + 2];
				TextureGroup_data[++index1] = 255 - Filter_data[index2];
			}
		}
		/////////////////////////////////////////////////////////////////////////
		/// right border creation
		/////////////////////////////////////////////////////////////////////////
		for (int i = 0; i != pix; ++i)
		{
			for (int j = 0; j!= space ; ++j)
			{
				index1 = 4* (pix * 8) * (pix + 2 * space) * y
							+ 4* (pix * 8) * (i + space)
							+ 4* x * (pix + 2* space) +
								4* (pix + space + j);
				index2 = 3* pix * i + 3 * j;
				TextureGroup_data[  index1] = Texture_data[index2];
				TextureGroup_data[++index1] = Texture_data[index2 + 1];
				TextureGroup_data[++index1] = Texture_data[index2 + 2];
				TextureGroup_data[++index1] = 255 - Filter_data[index2];
			}
		}
		/////////////////////////////////////////////////////////////////////////
		/// upper border creation
		/////////////////////////////////////////////////////////////////////////
		for (int i = 0; i != space; ++i)
		{
			for (int j = 0; j!= pix; ++j)
			{
				index1 = 4* (pix * 8) * (pix + 2 * space) * y
							+ 4* (pix * 8) * i
							+ 4* x * (pix + 2* space) +
								4* (space + j);
				index2 = 3* pix * (pix - space + i) + 3 * j;
				TextureGroup_data[  index1] = Texture_data[index2];
				TextureGroup_data[++index1] = Texture_data[index2 + 1];
				TextureGroup_data[++index1] = Texture_data[index2 + 2];
				TextureGroup_data[++index1] = 255 - Filter_data[index2];
			}
		}
		/////////////////////////////////////////////////////////////////////////
		/// lower border creation
		/////////////////////////////////////////////////////////////////////////
		for (int i = 0; i != space; ++i)
		{
			for (int j = 0; j!= pix; ++j)
			{
				index1 = 4* (pix * 8) * (pix + 2 * space) * y
							+ 4* (pix * 8) * (pix + space + i)
							+ 4* x * (pix + 2* space) +
								4* (space + j);
				index2 = 3* pix * i + 3 * j;
				TextureGroup_data[  index1] = Texture_data[index2];
				TextureGroup_data[++index1] = Texture_data[index2 + 1];
				TextureGroup_data[++index1] = Texture_data[index2 + 2];
				TextureGroup_data[++index1] = 255 - Filter_data[index2];
			}
		}
		/////////////////////////////////////////////////////////////////////////
		/// remaining 4 edges
		/////////////////////////////////////////////////////////////////////////
		// upper left
		for (int i = 0; i != space; ++i)
		{
			for (int j = 0; j!= space; ++j)
			{
				index1 = 4*(pix * 8)* (pix + 2 * space) * y
							+ 4* (pix * 8) * i
							+ 4* x * (pix + 2* space)
							+ 4* j;
				index2 = 0;
				TextureGroup_data[  index1] = Texture_data[index2];
				TextureGroup_data[++index1] = Texture_data[index2 + 1];
				TextureGroup_data[++index1] = Texture_data[index2 + 2];
				TextureGroup_data[++index1] = 255;
			}
		}
		// upper right
		for (int i = pix + space; i != pix + 2*space; ++i)
		{
			for (int j = 0; j!= space; ++j)
			{
				index1 = 4*(pix * 8)* (pix + 2 * space) * y
							+ 4* (pix * 8) * i
							+ 4* x * (pix + 2* space)
							+ 4* j;
				index2 = 0;
				TextureGroup_data[  index1] = Texture_data[index2];
				TextureGroup_data[++index1] = Texture_data[index2 + 1];
				TextureGroup_data[++index1] = Texture_data[index2 + 2];
				TextureGroup_data[++index1] = 255;
			}
		}
		// lower left
		for (int i = 0; i != space; ++i)
		{
			for (int j = pix + space; j!= pix + 2* space; ++j)
			{
				index1 = 4*(pix * 8)* (pix + 2 * space) * y
							+ 4* (pix * 8) * i
							+ 4* x * (pix + 2* space)
							+ 4* j;
				index2 = 0;
				TextureGroup_data[  index1] = Texture_data[index2];
				TextureGroup_data[++index1] = Texture_data[index2 + 1];
				TextureGroup_data[++index1] = Texture_data[index2 + 2];
				TextureGroup_data[++index1] = 255;
			}
		}
		// lower right
		for (int i = pix + space; i != pix + 2* space; ++i)
		{
			for (int j = pix + space; j!= pix + 2* space; ++j)
			{
				index1 = 4*(pix * 8)* (pix + 2 * space) * y
							+ 4* (pix * 8) * i
							+ 4* x * (pix + 2* space)
							+ 4* j;
				index2 = 0;
				TextureGroup_data[  index1] = Texture_data[index2];
				TextureGroup_data[++index1] = Texture_data[index2 + 1];
				TextureGroup_data[++index1] = Texture_data[index2 + 2];
				TextureGroup_data[++index1] = 255;
			}
		}
	}
}

///=================================================================================================
/// Toggle Material.
///=================================================================================================
void TileManager::ToggleMaterial()
{
	std::string matWater, matLand;
	mHighDetails = !mHighDetails;
	if (mHighDetails)
	{
		matLand = "Land_HighDetails" + StringConverter::toString(m_TileTextureSize, 3, '0');
		matWater= "Water_HighDetails";
	}
	else
	{
		matLand = "Land_LowDetails";
		matWater= "Water_LowDetails";
	}
	if (mGrid)
	{
		matLand +="_Grid";
		matWater+="_Grid";
	}
	for (int x = 0; x < CHUNK_SUM_X; ++x)
	{
		for (int y = 0; y < CHUNK_SUM_Z; ++y)
		{
			m_mapchunk[x][y].Get_Land_entity_high() ->setMaterialName(matLand);
			m_mapchunk[x][y].Get_Water_entity()->setMaterialName(matWater);
		}
	}
}

///=================================================================================================
/// Switch Grid.
///=================================================================================================
void TileManager::ToggleGrid()
{
	mGrid = !mGrid;
	mHighDetails = !mHighDetails;
	ToggleMaterial();
}


///=================================================================================================
/// Change resolution of terrain texture
///=================================================================================================
void TileManager::SetTextureSize(int pixels)
{
	if (pixels != 128 && pixels !=64 && pixels != 32 && pixels != 16 && pixels != 8) return;
	m_TileTextureSize = pixels;
	std::string matLand = "Land_HighDetails" + StringConverter::toString(m_TileTextureSize, 3, '0');
	{
		for (int x = 0; x < CHUNK_SUM_X; ++x)
			{
				for (int y = 0; y < CHUNK_SUM_Z; ++y)
				{
					m_mapchunk[x][y].Get_Land_entity_high()->setMaterialName(matLand);
				}
			}
	}
	mHighDetails = !mHighDetails;
	ToggleMaterial();
}

float TileManager::Get_Tile_Height(short x, short y)
{
	Real height[4];

	height[0] = m_Map[x][y].height * m_StretchZ;
	height[1] = m_Map[x+1][y].height * m_StretchZ; 
	height[2] = m_Map[x][y+1].height * m_StretchZ; 
	height[3] = m_Map[x+1][y+1].height * m_StretchZ; 

	Real min = height[0];
	Real max = height[0];

	for ( int a = 0; a < 4; ++a )
	{
		if( height[a] < min )
			min = height[a];
		if( height[a] > max )
			max = height[a];
	}

	Tile_Height = (min + max) / 2;

	return Tile_Height;
}
