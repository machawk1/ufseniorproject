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

#include "TileInterface.h"
#include "TileManager.h"
#include "Ogre.h"
#include "logger.h"

TileSelection::TileSelection(TileManager* TileManager)
{
	m_TileManager = TileManager;
	m_vdata = NULL;
	ActionState = SELECT;
	Selected_Bob = NULL;
	//BobC* bob = m_TileManager->Get_Bob_Array();
}
void TileSelection::set_ActionState(int value)
{
	ActionState = value;
}
void TileSelection::save_Selection()
{
	m_x_old = m_x;
	m_y_old = m_y;
}
void TileSelection::save_Tile()
{
	old_tile_x = tile_x;
	old_tile_z = tile_z;
}

void TileSelection::reset()
{
	m_distance = 1000000; // something big
	m_x = -1;
	m_y = -1;
}

void TileSelection::select()
{
	
	static int counter = 0;
	++counter;
	
	if (counter == 1) create_Entity();
	
	change_Selection();

	if (m_x != m_x_old && m_y != m_y_old)
	{
		// note: insert actions here

	}
}

void TileSelection::create_Entity()
{	
	unsigned long numVertices = 6; 

	if (m_vdata) delete m_vdata;
	m_vdata = new VertexData();
	IndexData* m_idata;
	
	char name[50];
	char name2[50];
	
	sprintf( name, "Selection");
	m_Mesh = MeshManager::getSingleton().createManual( name,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME ); 
	sprintf( name2, "SubSelection");
	m_SubMesh= m_Mesh->createSubMesh(name2); 
	m_idata = m_SubMesh->indexData;

	m_vdata->vertexCount = numVertices; // Wichtig! Anzahl der Punkte muss angegeben werden, sonst Objekt nicht existent

	VertexDeclaration* vdec = m_vdata->vertexDeclaration; 

	size_t offset = 0;
	vdec->addElement( 0, offset, VET_FLOAT3, VES_POSITION ); 
	offset += VertexElement::getTypeSize(VET_FLOAT3);
	vdec->addElement( 0, offset, VET_FLOAT3, VES_NORMAL ); 
	offset += VertexElement::getTypeSize(VET_FLOAT3);
	vdec->addElement( 0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0); 
	offset += VertexElement::getTypeSize(VET_FLOAT2);

    m_vbuf0 = HardwareBufferManager::getSingleton().createVertexBuffer( 
		offset, // size of one whole vertex 
		numVertices, // number of vertices 
        HardwareBuffer::HBU_STATIC_WRITE_ONLY, // usage 
        false); // no shadow buffer 

	VertexBufferBinding* vbbind = m_vdata->vertexBufferBinding; 
	vbbind->setBinding(0, m_vbuf0); 

	Real* pReal;
	pReal = static_cast<Real*>(m_vbuf0->lock(HardwareBuffer::HBL_NORMAL)); 
				///////////////////////////////////////////////////////////////////////// 
				// .
				///////////////////////////////////////////////////////////////////////// 
			Real height[4];
			float StretchZ = m_TileManager->Get_StretchZ();

			height[0] = m_TileManager->Get_Map_Height(m_x    , m_y    ) * StretchZ;
			height[1] = m_TileManager->Get_Map_Height(m_x + 1, m_y    ) * StretchZ;
			height[2] = m_TileManager->Get_Map_Height(m_x    , m_y + 1) * StretchZ;
			height[3] = m_TileManager->Get_Map_Height(m_x + 1, m_y + 1) * StretchZ;

			for (int a = 0; a != 4; ++a)
			{
				if (height[a] < LEVEL_WATER_CLP * StretchZ)
					height[a] = LEVEL_WATER_CLP * StretchZ;
			}

		// 1st triangle
			pReal[ 0] =   0; pReal[ 1] =   height[0] ; pReal[2] = 0;
			pReal[ 3] =   0; pReal[ 4] =   0; pReal[5] = 1;
			pReal[ 6] =   0; pReal[ 7] =   0;

			pReal[ 8] =   0; pReal[ 9] =   height[2] ; pReal[10] = TILE_SIZE;
			pReal[11] =   0; pReal[12] =   0; pReal[13] = 1;
			pReal[14] =   0; pReal[15] =   1; 

			pReal[16] =   TILE_SIZE; pReal[17] =  height[1] ; pReal[18] = 0;
			pReal[19] =   0; pReal[20] =   0; pReal[21] = 1;
			pReal[22] =   1; pReal[23] =   0; 

		// 2nd triangle
			pReal[24] =   TILE_SIZE; pReal[25] =   height[1] ; pReal[26] = 0;
			pReal[27] =   0; pReal[28] =   0; pReal[29] = 1;
			pReal[30] =   1; pReal[31] =   0;

			pReal[32] =   TILE_SIZE; pReal[33] =  height[2] ; pReal[34] = TILE_SIZE;
			pReal[35] =   0; pReal[36] =   0; pReal[37] = 1;
			pReal[38] =   0; pReal[39] =   1; 

			pReal[40] =   TILE_SIZE; pReal[41] =   height[3] ; pReal[42] = TILE_SIZE;
			pReal[43] =   0; pReal[44] =   0; pReal[45] = 1;
			pReal[46] =   1; pReal[47] =   1; 

	m_vbuf0->unlock(); 

	HardwareIndexBufferSharedPtr m_ibuf = HardwareBufferManager::getSingleton(). 
	createIndexBuffer( 
	HardwareIndexBuffer::IT_16BIT, // type of index 
	numVertices, // number of indexes 
	HardwareBuffer::HBU_STATIC_WRITE_ONLY, // usage 
	false); // no shadow buffer 

	m_idata->indexBuffer = m_ibuf; 
	m_idata->indexStart = 0; 
	m_idata->indexCount = numVertices; 

	unsigned short* pIdx = static_cast<unsigned short*>(m_ibuf->lock(HardwareBuffer::HBL_DISCARD)); 
   
	for (unsigned short a = 0; a < (unsigned short)numVertices; a++)
		pIdx[a] = a; 
   
	m_ibuf->unlock(); 

	m_SubMesh->operationType = RenderOperation::OT_TRIANGLE_LIST; 
	m_SubMesh->useSharedVertices = false; 
	m_SubMesh->vertexData = m_vdata; 

	// Setzen des Sichtbarkeits-Quaders. Fällt dieser Quader außerhalb des Sichtbereits der
	// Kamera, so wird das Kartenstück nicht gerendert.
	
	AxisAlignedBox* bounds = new AxisAlignedBox(
		0, 0 , 0,
		TILE_SIZE, 255 * StretchZ , TILE_SIZE); 
	m_Mesh->_setBounds( *bounds );
	
	delete bounds;

	m_SubMesh->setMaterialName("Selection"); 
	sprintf( name2, "Selection Entity");
	m_Entity = m_TileManager->Get_pSceneManager()->createEntity(name2, name); 

	m_SceneNode = m_TileManager->Get_pSceneManager()->getRootSceneNode()->createChildSceneNode();
	m_SceneNode->attachObject( m_Entity );
	m_SceneNode->setPosition(m_x * TILE_SIZE,0,m_y * TILE_SIZE);
	m_Entity->setRenderQueueGroup(RENDER_QUEUE_OVERLAY);
}

void TileSelection::change_Selection()
{

	// only change if value differs from old one
	if (m_x_old == m_x && m_y_old == m_y )
	{	
		// Convert the mouse x,y into the tile index x,z
		tile_x = m_TileManager->get_TileInterface()->get_Selection()->m_x;
		tile_z = m_TileManager->get_TileInterface()->get_Selection()->m_y;
		height = m_TileManager->Get_Map_Height(tile_x, tile_z) * m_TileManager->Get_StretchZ();
		BobC* bob = m_TileManager->Get_Bob_Array();

		switch ( ActionState )
		{
			case SELECT:

				/* Select the bob on the Tile selected */
				if ( m_TileManager->Get_Occupied(tile_x, tile_z) == true )
				{
					Selected_Bob = m_TileManager->Get_Unit(tile_x, tile_z);
					save_Tile();

					// Logging the selection for testing
					Logger::log().info() << "Selected robot " << Selected_Bob->getName() << " at Tile(" << tile_x << "," << tile_z << ")";
				}

				break;

			case PLACE:
		
				/* Place a bob on the tile */
				if ( m_TileManager->Get_Occupied(tile_x, tile_z) != true )
				{
					// Code to give the Robot mesh a unique name and place it
		
					static int bobCount = 0;

					char NodeName[NAME_BUFFER_SIZE];
		
					sprintf( NodeName, "Bob[%d]", bobCount );
					
					bob[bobCount].model = "robot.mesh";
					bob[bobCount].mEntity = m_TileManager->Get_pSceneManager()->createEntity( NodeName, bob[bobCount].model );
					bob[bobCount].x = tile_x;
					bob[bobCount].y = tile_z;
					bob[bobCount].mNode = m_TileManager->Get_pSceneManager()->getRootSceneNode()->createChildSceneNode(
						NodeName , Vector3(tile_x * TILE_SIZE + 16, height , tile_z * TILE_SIZE +16));
					bob[bobCount].mNode->attachObject( bob[bobCount].mEntity );
					bob[bobCount].mNode->setScale(0.4f, 0.4f, 0.4f);

					// Setting the this tile as occupied by with this Node, and setting this bob as the selected bob
					m_TileManager->Set_Map_Occupied(tile_x, tile_z, true);
					m_TileManager->Set_Map_Unit(tile_x, tile_z, bob[bobCount].mNode);
					Selected_Bob = bob[bobCount].mNode;

					// Set flag that indicates that a Bob now exists
					m_TileManager->Set_bobExists(true);

					// Incement the bob counter
					++bobCount;

					// Logging the creation and position of each robot
					Logger::log().info() << "Creating and Placing robot " << NodeName << " at Tile(" << tile_x << "," << tile_z << ")";
					Logger::log().info() << "Occupied and Node name just placed is: " << m_TileManager->Get_Occupied(tile_x, tile_z) << " and " << bob[bobCount].mNode->getName();
					
				}
				
				break;

			case MOVE:
					
				/* Move the Selected bob to the tile selected */
				if ( m_TileManager->Get_Occupied(tile_x, tile_z) != true && Selected_Bob != NULL )
				{
					Selected_Bob->setPosition(tile_x * TILE_SIZE + 16, height, tile_z * TILE_SIZE + 16);

					// Set the original tile to unoccupied
					m_TileManager->Set_Map_Occupied(old_tile_x, old_tile_z, false);

					// Logging the selection for testing
					Logger::log().info() << "Moved robot " << Selected_Bob->getName() << " to Tile(" << tile_x << "," << tile_z << ")";
				}
				break;
		}

		return;
	}
		
	Real* pReal;
	pReal = static_cast<Real*>(m_vbuf0->lock(HardwareBuffer::HBL_NORMAL)); 
				///////////////////////////////////////////////////////////////////////// 
				// .
				///////////////////////////////////////////////////////////////////////// 
			Real height[4];
			float StretchZ = m_TileManager->Get_StretchZ();

			height[0] = m_TileManager->Get_Map_Height(m_x    , m_y    ) * StretchZ;
			height[1] = m_TileManager->Get_Map_Height(m_x + 1, m_y    ) * StretchZ;
			height[2] = m_TileManager->Get_Map_Height(m_x    , m_y + 1) * StretchZ;
			height[3] = m_TileManager->Get_Map_Height(m_x + 1, m_y + 1) * StretchZ;

			for (int a = 0; a != 4; ++a)
			{
				if (height[a] < LEVEL_WATER_CLP * StretchZ)
					height[a] = LEVEL_WATER_CLP * StretchZ;
			}

		// 1st triangle
			pReal[ 0] =   0; pReal[ 1] =   height[0] ; pReal[2] = 0;
			pReal[ 3] =   0; pReal[ 4] =   0; pReal[5] = 1;
			pReal[ 6] =   0; pReal[ 7] =   0;

			pReal[ 8] =   0; pReal[ 9] =   height[2] ; pReal[10] = TILE_SIZE;
			pReal[11] =   0; pReal[12] =   0; pReal[13] = 1;
			pReal[14] =   0; pReal[15] =   1; 

			pReal[16] =   TILE_SIZE; pReal[17] =  height[1] ; pReal[18] = 0;
			pReal[19] =   0; pReal[20] =   0; pReal[21] = 1;
			pReal[22] =   1; pReal[23] =   0; 

		// 2nd triangle
			pReal[24] =   TILE_SIZE; pReal[25] =   height[1] ; pReal[26] = 0;
			pReal[27] =   0; pReal[28] =   0; pReal[29] = 1;
			pReal[30] =   1; pReal[31] =   0;

			pReal[32] =   0; pReal[33] =  height[2] ; pReal[34] = TILE_SIZE;
			pReal[35] =   0; pReal[36] =   0; pReal[37] = 1;
			pReal[38] =   0; pReal[39] =   1; 

			pReal[40] =   TILE_SIZE; pReal[41] =   height[3] ; pReal[42] = TILE_SIZE;
			pReal[43] =   0; pReal[44] =   0; pReal[45] = 1;
			pReal[46] =   1; pReal[47] =   1; 

	m_vbuf0->unlock(); 
	
	m_SceneNode->setPosition(m_x * TILE_SIZE,0,m_y * TILE_SIZE);
}

TileMouse::TileMouse(TileInterface* TileInterface)
{
	m_Interface = TileInterface;
	m_Rect = NULL;
}

TileMouse::~TileMouse()
{
}

void TileMouse::move_Relative(Real x, Real y)
{
	x *= MOUSE_SENSITY;
	y *= MOUSE_SENSITY;

	set_Position(m_x + x,m_y + y);
}

void TileMouse::set_Position(Real x, Real y)
{
	m_x = x;
	m_y = y;

	if (m_x < -1) m_x = -1;
	else if (m_x > 1) m_x = 1;
	if (m_y < -1) m_y = -1;
	else if (m_y > 1) m_y = 1;

	m_Rect->setCorners(m_x,m_y,m_x + 0.025,m_y - 0.05);
}

void TileMouse::Init()
{
	m_Rect = new Rectangle2D(true);
	
	m_Rect->setBoundingBox(AxisAlignedBox(-100000.0*Vector3::UNIT_SCALE, 100000.0*Vector3::UNIT_SCALE));
	m_Rect->setMaterial("Cursor/Normal");
	m_Rect->setRenderQueueGroup(RENDER_QUEUE_OVERLAY);

	m_SceneNode = m_Interface->get_SceneNode()->createChildSceneNode("Mouse");

	set_Position(0,0);

	m_SceneNode->attachObject(m_Rect);
}


TileInterface::TileInterface(TileManager* TileManager)
{
	m_TileManager = TileManager;
	m_SceneNode = m_TileManager->Get_pSceneManager()->getRootSceneNode()->createChildSceneNode("Interface");
	m_Mouse = new TileMouse(this);
	m_Selection = new TileSelection(m_TileManager);
}

TileInterface::~TileInterface()
{
	delete m_Mouse;
	delete m_Selection;
}

void TileInterface::Init()
{
	// Initialize mouse cursor
	m_Mouse->Init();
}

void TileInterface::pick_Tile()
{
	// mouse picking

	// save old selection to compare to new selection later
	m_Selection->save_Selection();

	//reset Selection_Tile
	m_Selection->reset();

	RaySceneQuery* mRaySceneQuery = this->m_TileManager->Get_pSceneManager()->createRayQuery( Ray() );

	Ray mouseRay = this->m_TileManager->Get_pSceneManager()->getCamera("PlayerCam")
		->getCameraToViewportRay( (m_Mouse->get_x()+1) /2 , (1-(m_Mouse->get_y()+1) /2));
	mRaySceneQuery->setRay( mouseRay );

	// Perform the scene query
    RaySceneQueryResult &result = mRaySceneQuery->execute();
    RaySceneQueryResult::iterator itr = result.begin( );

	 // Get the results
	while ( itr != result.end() && itr->movable) // we only collect movable objects (our terrain is movable!)
	{
		// make sure the query doesn't contain any overlay elements like the mouse cursor (which is always hit!)
		if (itr->movable->getRenderQueueGroup() != RENDER_QUEUE_OVERLAY)
		{
			// now test if a terrain chunk is hit

			for (int a = 0; a != CHUNK_SUM_X; ++a)
			{
				for (int b = 0; b != CHUNK_SUM_Z; ++b)
				{
					if (itr->movable == this->m_TileManager->get_TileChunk(a,b)->Get_Land_entity_high())
					{
						// we found our chunk, now search for the correct tile
						pick_Tile(&mouseRay,a,b);
					}
				}
			}
		}

		itr++;
    }

	// now m_Selection contains the wanted tile

	if (m_Selection->m_x != -1 && m_Selection->m_y != -1)
	{
		m_Selection->select();
	}
}

void TileInterface::pick_Tile(Ray* mouseRay, int a, int b)
{
	// we start with a given tile chunk (coordinates a and b) and a ray and try to find the tile that was selected
	// we have to check every tile if it was hit and return the nearest one

	Real height[5], lower, higher;

	int vertex_x = m_TileManager->get_TileChunk(a,b)->get_posX() * CHUNK_SIZE_X;
	int vertex_y = m_TileManager->get_TileChunk(a,b)->get_posZ() * CHUNK_SIZE_Z;
	
	float StretchZ = m_TileManager->Get_StretchZ();

	for (int x = 0; x != CHUNK_SIZE_X; ++x)
	{
		for (int y = 0; y!= CHUNK_SIZE_Z; ++y)
		{
			// we have to build a bounding box for each tile and check if the ray intersects this box
			// to do this we need the height of the tile corner vertices

			height[0] = m_TileManager->Get_Map_Height(vertex_x + x    , vertex_y + y    ) * StretchZ;
			height[1] = m_TileManager->Get_Map_Height(vertex_x + x + 1, vertex_y + y    ) * StretchZ;
			height[2] = m_TileManager->Get_Map_Height(vertex_x + x    , vertex_y + y + 1) * StretchZ;
			height[3] = m_TileManager->Get_Map_Height(vertex_x + x + 1, vertex_y + y + 1) * StretchZ;
			height[4] = (height[0]+height[1]+height[2]+height[3]) /4.0;
			
			// now we build four bounding boxes per tile to increase picking accuracy
			// note: Ogre only allows bounding boxes with the first vector having got the lower value in every(!) component
			// so we have to check which height value is greater
			if (height[0] > height[4])
			{
				lower = height[4];
				higher = height[0];
			}
			else
			{
				lower = height[0];
				higher = height[4];
			}

			AxisAlignedBox Box1((vertex_x + x)* TILE_SIZE, lower , (vertex_y + y)* TILE_SIZE,
			(vertex_x + x + .5)* TILE_SIZE, higher, (vertex_y + y + .5)* TILE_SIZE);

			if (height[1] > height[4])
			{
				lower = height[4];
				higher = height[1];
			}
			else
			{
				lower = height[1];
				higher = height[4];
			}

			AxisAlignedBox Box2((vertex_x + x + .5)* TILE_SIZE, lower, (vertex_y + y)* TILE_SIZE,
			(vertex_x + x + 1)* TILE_SIZE, higher, (vertex_y + y + .5)* TILE_SIZE);

			if (height[2] > height[4])
			{
				lower = height[4];
				higher = height[2];
			}
			else
			{
				lower = height[2];
				higher = height[4];
			}

			AxisAlignedBox Box3((vertex_x + x)* TILE_SIZE, lower , (vertex_y + y + .5)* TILE_SIZE,
			(vertex_x + x + .5)* TILE_SIZE, higher, (vertex_y + y + 1)* TILE_SIZE);

			if (height[3] > height[4])
			{
				lower = height[4];
				higher = height[3];
			}
			else
			{
				lower = height[3];
				higher = height[4];
			}

			AxisAlignedBox Box4((vertex_x + x + .5)* TILE_SIZE, lower , (vertex_y + y + .5)* TILE_SIZE,
			(vertex_x + x + 1)* TILE_SIZE, higher, (vertex_y + y + 1)* TILE_SIZE);

			// Test box
			std::pair< bool, Real > Test1 = mouseRay->intersects(Box1);
			std::pair< bool, Real > Test2 = mouseRay->intersects(Box2);
			std::pair< bool, Real > Test3 = mouseRay->intersects(Box3);
			std::pair< bool, Real > Test4 = mouseRay->intersects(Box4);

			if (Test1.first == true)
			{
				// intersection!
				// find the closest intersection to the camera
				if (Test1.second < m_Selection->m_distance)
				{
					m_Selection->m_distance = Test1.second;
					m_Selection->m_x  = vertex_x + x;
					m_Selection->m_y  = vertex_y + y;
				}
			}

			if (Test2.first == true)
			{
				// intersection!
				// find the closest intersection to the camera
				if (Test2.second < m_Selection->m_distance)
				{
					m_Selection->m_distance = Test2.second;
					m_Selection->m_x  = vertex_x + x;
					m_Selection->m_y  = vertex_y + y;
				}
			}

			if (Test3.first == true)
			{
				// intersection!
				// find the closest intersection to the camera
				if (Test3.second < m_Selection->m_distance)
				{
					m_Selection->m_distance = Test3.second;
					m_Selection->m_x  = vertex_x + x;
					m_Selection->m_y  = vertex_y + y;
				}
			}

			if (Test4.first == true)
			{
				// intersection!
				// find the closest intersection to the camera
				if (Test4.second < m_Selection->m_distance)
				{
					m_Selection->m_distance = Test4.second;
					m_Selection->m_x  = vertex_x + x;
					m_Selection->m_y  = vertex_y + y;
				}
			}

		} // end for y
	} // end for x
}
