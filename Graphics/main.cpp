/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

#include "Ogre.h"
#include "main.h"
#include "TileChunk.h"
#include "BobC.h"

// ----------------------------------------------------------------------------
// Define the application object
// This is derived from ExampleApplication which is the class OGRE provides to 
// make it easier to set up OGRE without rewriting the same code all the time.
// You can override extra methods of ExampleApplication if you want to further
// specialise the setup routine, otherwise the only mandatory override is the
// 'createScene' method which is where you set up your own personal scene.
// ----------------------------------------------------------------------------
class SampleApp : public ExampleApplication
{

public:
	
	SampleApp()
	{
		mTileManager = NULL;
	}
	
	void createScene(void)
	{
		
		mTileManager = new TileManager();
		mTileManager->Init(mSceneMgr);
		mSceneMgr->setAmbientLight(ColourValue(1,1,1));
		mTileManager->Set_Bob_Array(bob);

/*
		bob[0].model = "robot.mesh";
        bob[0].mEntity = mSceneMgr->createEntity( "Bob0", bob[0].model);
        bob[0].x = 30;
		bob[0].y = 40;
		bob[1].model = "robot.mesh";
        bob[1].mEntity = mSceneMgr->createEntity( "Bob1", bob[1].model);
		bob[1].x = 50;
		bob[1].y = 40;
		bob[2].model = "robot.mesh";
        bob[2].mEntity = mSceneMgr->createEntity( "Bob2", bob[2].model);
		bob[2].x = 20;
		bob[2].y = 30;
	    
		stretchZ = mTileManager->Get_StretchZ();
		
            bob[0].mNode = mSceneMgr->getRootSceneNode()->
                createChildSceneNode("BobNode0", Vector3( 32 * bob[0].x - 16, stretchZ*mTileManager->Get_Map_Height(bob[0].x,bob[0].y) , 32 * bob[0].y - 16 ) );
            bob[0].mNode->attachObject( bob[0].mEntity );
            bob[0].mNode->scale(.3,.3,.3);

			bob[1].mNode = mSceneMgr->getRootSceneNode()->
                createChildSceneNode("BobNode1", Vector3( 32 * bob[1].x - 16, stretchZ*mTileManager->Get_Map_Height(bob[1].x,bob[1].y) , 32 * bob[1].y - 16 ) );
            bob[1].mNode->attachObject( bob[1].mEntity );
            bob[1].mNode->scale(.3,.3,.3);

			bob[2].mNode = mSceneMgr->getRootSceneNode()->
                createChildSceneNode("BobNode2", Vector3( 32 * bob[2].x - 16, stretchZ*mTileManager->Get_Map_Height(bob[2].x,bob[2].y) , 32 * bob[2].y - 16 ) );
            bob[2].mNode->attachObject( bob[2].mEntity );
            bob[2].mNode->scale(.3,.3,.3);        
		
		//mSceneMgr->setSkyDome( true, "Examples/CloudySky", 5, 8 );
*/
		}

public:
	~SampleApp()
	{
		if (mTileManager) {delete mTileManager; }
		if (mFrameListener) { delete mFrameListener; }
		if (mRoot) { delete mRoot; }
	}
};

// ----------------------------------------------------------------------------
// Main function, just boots the application object
// ----------------------------------------------------------------------------
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char **argv)
#endif
{
	// Create application object
	SampleApp app;
	try { app.go(); }
	catch( Ogre::Exception& e )
	{
		#ifdef WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL );
		#else
		std::cerr << "An exception has occured: " << e.getFullDescription();
		#endif
	}
	return 0;
}
