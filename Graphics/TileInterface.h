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

#ifndef TILE_INTERFACE_H
#define TILE_INTERFACE_H

#include "Ogre.h"
//#include "BobC.h"

class TileManager;
class TileInterface;

using namespace Ogre;

const Real MOUSE_SENSITY = 0.002;

class TileSelection
{
private:
	MeshPtr m_Mesh;
	SubMesh* m_SubMesh;
	VertexData* m_vdata;
	IndexData* m_idata;
	Entity* m_Entity;
	SceneNode* m_SceneNode;
	SceneNode* Selected_Bob;
	HardwareVertexBufferSharedPtr m_vbuf0;

	static const int NAME_BUFFER_SIZE = 50;

	int m_x_old;
	int m_y_old;
	int ActionState;
	unsigned char height;
	short tile_x;
	short tile_z;
	short old_tile_x;
	short old_tile_z;

	TileManager* m_TileManager;
	TileInterface* m_Interface;

	
public:
	int m_x;
	int m_y;
	Real m_distance;
	//BobC* bob;
	

	TileSelection( TileManager* TileManager );
	void set_ActionState(int value);

	void create_Entity();
	void change_Selection();
	void save_Selection();
	void save_Tile();
	void reset();
	void select();
};


class TileMouse
{
private:

	Real m_x;
	Real m_y;

	TileInterface* m_Interface;
	Rectangle2D* m_Rect;
	SceneNode* m_SceneNode;
	
public:

	TileMouse(TileInterface* TileInterface);
	~TileMouse();

	Real get_x(){ return m_x;}
	Real get_y(){ return m_y;}
	void Init();
	void set_Position(Real x, Real y);
	void move_Relative(Real x, Real y);
};

class TileInterface
{
private:
	TileManager* m_TileManager;
	SceneNode* m_SceneNode;
	TileMouse* m_Mouse;
	TileSelection* m_Selection;

public:
	
	TileInterface(TileManager* TileManager);
	~TileInterface();

	SceneNode* get_SceneNode(){return m_SceneNode;}
	TileMouse* get_Mouse(){return m_Mouse;}
	TileSelection* get_Selection(){return m_Selection;}
	
	void Init();
	void pick_Tile();
	void pick_Tile(Ray* mouseRay, int a, int b);
};

#endif
