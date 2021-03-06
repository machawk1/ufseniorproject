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

#ifndef TILE_ENVIRONMENT_H
#define TILE_ENVIRONMENT_H

#include "Ogre.h"

using namespace Ogre;

class TileChunk;
class TileManager;

/**
 * TileEngine class which manages environmental objects like trees and rocks.
 *****************************************************************************/
class TileEnvironment
{
private:
	TileChunk* m_ChunkPtr;
	TileManager* m_TileManagerPtr;
	
	MeshPtr m_Environment_Mesh;
	SubMesh* m_Environment_SubMesh;
	Entity* m_Environment_Entity;
	
	SceneNode* m_Environment_SceneNode;
	
	HardwareVertexBufferSharedPtr m_vbuf0;
	HardwareIndexBufferSharedPtr m_ibuf;

public:

	TileEnvironment(TileManager* TileManagerPtr, TileChunk* ChunkPointer);
	~TileEnvironment();

	void UpdateEnvironment();
};

#endif
