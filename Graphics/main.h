/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright © 2000-2003 The OGRE Team
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    ExampleApplication.h
Description: Base class for all the OGRE examples
-----------------------------------------------------------------------------
*/

#ifndef __ExampleApplication_H__
#define __ExampleApplication_H__

#include <stdlib.h>
#include "Ogre.h"
#include "OgreConfigFile.h"
#include "ProjektFrameListener.h"
#include "TileManager.h"
#include "Move.cpp"


using namespace Ogre;

/** Base class which manages the standard startup of an Ogre application.
	Designed to be subclassed for specific examples if required.
*/

class ExampleApplication
{
public:

	/// Standard constructor
	ExampleApplication()
	{
		mFrameListener = 0;
		mRoot = 0;
	}
	/// Standard destructor
	virtual ~ExampleApplication()
	{
    /*    if (mFrameListener)
            delete mFrameListener;
        if (mRoot)
            delete mRoot;
	*/
	}

	/// Start the example
	virtual void go(void)
	{
		if (!setup()) { return; }
		mRoot->startRendering();
	}

protected:
	Root *mRoot;
	Camera* mCamera;
	SceneManager* mSceneMgr;
	MoveDemoListener* mFrameListener;
	RenderWindow* mWindow;
    TileManager *mTileManager;

	float stretchZ;
	BobC bob[MAX_BOB_AMOUNT];

	// These internal methods package up the stages in the startup process
	/** Sets up the application - returns false if the user chooses to abandon configuration. */
	virtual bool setup(void)
	{
		mRoot = new Root();
		setupResources();

		bool carryOn = configure();
		if (!carryOn) return false;

		chooseSceneManager();
		createCamera();
		createViewports();

		//Set default mipmap level (NB some APIs ignore this)

		TextureManager::getSingleton().setDefaultNumMipmaps(3);

		// Create any resource listeners (for loading screens)
		createResourceListener();
		// Load resources
		loadResources();

		// Create the scene
		createScene();
		createFrameListener();
		mFrameListener->Set_pTileManager(mTileManager);
		return true;
	}
	
	/** Configures the application - returns false if the user chooses to abandon configuration. */
	virtual bool configure(void)
	{
		// Show the configuration dialog and initialise the system
		// You can skip this and use root.restoreConfig() to load configuration
		// settings if you were sure there are valid ones saved in ogre.cfg
		if(mRoot->showConfigDialog())
		{
            // If returned true, user clicked OK so initialise
            // Here we choose to let the system create a default rendering window by passing 'true'
            mWindow = mRoot->initialise(true);
            return true;
        }
        else
        {
            return false;
        }
    }
    
    virtual void chooseSceneManager(void)
    {
        // Get the SceneManager, in this case a generic one
        mSceneMgr = mRoot->getSceneManager( ST_GENERIC);
    }
    virtual void createCamera(void)
    {
        // Create the camera
        mCamera = mSceneMgr->createCamera("PlayerCam");
        mCamera->setPosition(Vector3(0,0,600));
        mCamera->lookAt(Vector3(0,0,0));
        mCamera->setNearClipDistance(1);
		mCamera->setFarClipDistance(10130);

	}
	
virtual void createFrameListener(void)
{
		mFrameListener= new MoveDemoListener(mWindow, mCamera, bob);
		mFrameListener->showDebugOverlay(true);   
		mRoot->addFrameListener(mFrameListener);
}

virtual void createScene(void) = 0;    // pure virtual - this has to be overridden


virtual void createViewports(void)
{
	// Create one viewport, entire window
	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0,0,0));
}

    /// Method which will define the source of resources (other than current folder)
    virtual void setupResources(void)
    {
         // Load resource paths from config file
        ConfigFile cf;
        cf.load("resources.cfg");

        // Go through all sections & settings in the file
        ConfigFile::SectionIterator seci = cf.getSectionIterator();

        String secName, typeName, archName;
        while (seci.hasMoreElements())
        {
            secName = seci.peekNextKey();
            ConfigFile::SettingsMultiMap *settings = seci.getNext();
            ConfigFile::SettingsMultiMap::iterator i;
            for (i = settings->begin(); i != settings->end(); ++i)
            {
                typeName = i->first;
                archName = i->second;
                ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
            }
        }
    }

	/// Optional override method where you can create resource listeners (e.g. for loading screens)
	virtual void createResourceListener(void)
	{
	}

	/// Optional override method where you can perform resource group loading
	/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	virtual void loadResources(void)
	{
		// Initialise, parse scripts etc
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}
};


#endif
