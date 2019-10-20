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
Filename:    ExampleFrameListener.h
Description: Defines an Projekt frame listener which responds to frame events.
This frame listener just moves a specified camera around based on
keyboard and mouse movements.
Mouse:    Freelook
W or Up:  Forward
S or Down:Backward
A:        Step left
D:        Step right
             PgUp:     Move upwards
             PgDown:   Move downwards
             F:        Toggle frame rate stats on/off
			 R:        Render mode
             T:        Cycle texture filtering
                       Bilinear, Trilinear, Anisotropic(8)
-----------------------------------------------------------------------------
*/

#ifndef __ProjektFrameListener_H__
#define __ProjektFrameListener_H__

#include "Ogre.h"
#include "OgreKeyEvent.h"
#include "OgreEventListeners.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "TileManager.h"
#include "TileInterface.h"
#include "logger.h"


using namespace Ogre;


class ProjektFrameListener: public FrameListener, public KeyListener
{
private:
	int mSceneDetailIndex ;
	bool grid;
	bool quality;
	bool filter;
	int pixels;

protected:
	Overlay* mDebugOverlay;

    void updateStats(void)
    {
        static String currFps = "Current FPS: ";
        static String avgFps = "Average FPS: ";
        static String bestFps = "Best FPS: ";
        static String worstFps = "Worst FPS: ";
        static String tris = "Triangle Count: ";

        // update stats when necessary
        try {
            OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
            OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
            OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
            OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

            const RenderTarget::FrameStats& stats = mWindow->getStatistics();

            guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
            guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
            guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
                +" "+StringConverter::toString(stats.bestFrameTime)+" ms");
            guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
                +" "+StringConverter::toString(stats.worstFrameTime)+" ms");

            OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
            guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

			OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
			guiDbg->setCaption(mWindow->getDebugText());
			mWindow->setDebugText("Keys: 1 -> Select a Robot, 2 -> Place a new Robot, 3 -> Move a Robot\tg -> grid , f -> Filter, t -> toggle texture (tst texture needed), p -> change pixel");
		}
		catch(...)
		{
			// ignore
		}
	}

public:
	// Constructor takes a RenderWindow because it uses that to determine input context
	ProjektFrameListener(RenderWindow* win, Camera* cam, bool useBufferedInputKeys = false, bool useBufferedInputMouse = false)
	{
		mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
		mUseBufferedInputKeys = useBufferedInputKeys;
		mUseBufferedInputMouse = useBufferedInputMouse;
		mInputTypeSwitchingOn = mUseBufferedInputKeys || mUseBufferedInputMouse;

		if (mInputTypeSwitchingOn)
		{
			mEventProcessor = new EventProcessor();
			mEventProcessor->initialise(win);
			//OverlayManager::getSingleton().createCursorOverlay();
			mEventProcessor->startProcessingEvents();
			mEventProcessor->addKeyListener(this);
			mInputDevice = mEventProcessor->getInputReader();
		}
		else
		{
			mInputDevice = PlatformManager::getSingleton().createInputReader();
			mInputDevice->initialise(win,true, true);
		}

		mCamera = cam;
		mWindow = win;
		mStatsOn = true;
		mNumScreenShots = 0;
		mTimeUntilNextToggle = 0;
		mSceneDetailIndex = 0;
		mMoveScale = 0.0f;
		mRotScale = 0.0f;
		mTranslateVector = Vector3::ZERO;
		mAniso = 1;
		mfiltering = TFO_TRILINEAR;
		counter = 0;
		grid = false;
		quality = false;
		filter = false;
		pixels = 128;
		showDebugOverlay(true);
	}

	virtual ~ProjektFrameListener()
	{
		if (mInputTypeSwitchingOn)
		{
			 delete mEventProcessor;
		}
		else
		{
			PlatformManager::getSingleton().destroyInputReader( mInputDevice );
		}
	}

	virtual bool processUnbufferedKeyInput(const FrameEvent& evt)
	{
		/* ------ Camera Movement Controls ----------------------------------*/

        if (mInputDevice->isKeyDown(KC_A) || mInputDevice->isKeyDown(KC_LEFT))
        {
            // Move camera left
            mTranslateVector.x = - 2* mMoveScale;
        }

        if (mInputDevice->isKeyDown(KC_D) || mInputDevice->isKeyDown(KC_RIGHT))
        {
            // Move camera RIGHT
            mTranslateVector.x = 2*mMoveScale;
        }

		// Move camera forward by keypress.
		if (mInputDevice->isKeyDown(KC_UP) || mInputDevice->isKeyDown(KC_W) )
		{
			mTranslateVector.z = -2* mMoveScale;
		}

		/* Move camera backward by keypress. */
        if (mInputDevice->isKeyDown(KC_DOWN) || mInputDevice->isKeyDown(KC_S) )
        {
            mTranslateVector.z =  2 *mMoveScale;
        }

		if (mInputDevice->isKeyDown(KC_G) && mTimeUntilNextToggle <= 0 )
		{
			//select grid
			pTileManager->ToggleGrid();
			mTimeUntilNextToggle = .5;
		}

		if (mInputDevice->isKeyDown(KC_F) && mTimeUntilNextToggle <= 0)
		{
			//select quality
			pTileManager->ToggleMaterial();
			mTimeUntilNextToggle = .5;
		}

		// Move camera up/down by mousewheel.
		if( mInputDevice->getMouseRelativeZ() > 0 )
		{
			mTranslateVector.y = -mMoveScale * 32.0;
		}

        /* Move camera backward by mouse wheel. */
        if( mInputDevice->getMouseRelativeZ() < 0 )
        {
            mTranslateVector.y = mMoveScale * 32.0;
        }

		if (mInputDevice->isKeyDown(KC_PGUP))
        {
            // Move camera up
            mTranslateVector.y = -mMoveScale;
        }

        if (mInputDevice->isKeyDown(KC_PGDOWN))
        {
            // Move camera down
            mTranslateVector.y = +mMoveScale;
        }

		/* ------- Event/Action selection controls --------------------------------*/

		if (mInputDevice->isKeyDown(KC_1))
		{
			// Select the mouse event "Select" Unit
			pTileManager->get_TileInterface()->get_Selection()->set_ActionState(SELECT);
		}

		if (mInputDevice->isKeyDown(KC_2))
		{
			// Select the mouse event "Place" Unit
			pTileManager->get_TileInterface()->get_Selection()->set_ActionState(PLACE);
		}

		if (mInputDevice->isKeyDown(KC_3))
		{
			// Select the mouse event "Move" Unit
			pTileManager->get_TileInterface()->get_Selection()->set_ActionState(MOVE);
		}

		/* ------- General Terrain and Application controls -----------------------*/

		if (mInputDevice->isKeyDown(KC_P) && mTimeUntilNextToggle <= 0)
		{
			//change pixel size of terrain textures
			pixels /= 2; // shrink pixel value
			if (pixels < 8) pixels = 128; // if value is too low resize to maximum
			
			pTileManager->SetTextureSize(pixels);
			mTimeUntilNextToggle = .5;
		}	   

        if( mInputDevice->isKeyDown( KC_ESCAPE) )
        {            
            return false;
        }

		// see if switching is on, and you want to toggle 
        if (mInputTypeSwitchingOn && mInputDevice->isKeyDown(KC_M) && mTimeUntilNextToggle <= 0)
        {
			switchMouseMode();
            mTimeUntilNextToggle = 1;
        }

        if (mInputTypeSwitchingOn && mInputDevice->isKeyDown(KC_K) && mTimeUntilNextToggle <= 0)
        {
			// must be going from immediate keyboard to buffered keyboard
			switchKeyMode();
            mTimeUntilNextToggle = 1;
        }
        if (mInputDevice->isKeyDown(KC_F) && mTimeUntilNextToggle <= 0)
        {
            mStatsOn = !mStatsOn;
            showDebugOverlay(mStatsOn);

            mTimeUntilNextToggle = 1;
        }
        if (mInputDevice->isKeyDown(KC_T) && mTimeUntilNextToggle <= 0)
        {
						pTileManager->ChangeTexture();
/*
            switch(mfiltering)
            {
            case TFO_BILINEAR:
                mfiltering = TFO_TRILINEAR;
                mAniso = 1;
                break;
            case TFO_TRILINEAR:
                mfiltering = TFO_ANISOTROPIC;
                mAniso = 8;
                break;
            case TFO_ANISOTROPIC:
                mfiltering = TFO_BILINEAR;
                mAniso = 1;
                break;
            default:
                break;
            }
            MaterialManager::getSingleton().setDefaultTextureFiltering(mfiltering);
            MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);
            showDebugOverlay(mStatsOn);
*/
            mTimeUntilNextToggle = 1;
        }

        if (mInputDevice->isKeyDown(KC_SYSRQ) && mTimeUntilNextToggle <= 0)
        {
			char tmp[20];
			sprintf(tmp, "screenshot_%d.png", ++mNumScreenShots);
            mWindow->writeContentsToFile(tmp);
            mTimeUntilNextToggle = 0.5;
			mWindow->setDebugText(String("Wrote ") + tmp);
        }
		
		if (mInputDevice->isKeyDown(KC_R) && mTimeUntilNextToggle <=0)
		{
			mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
			switch(mSceneDetailIndex) {
				case 0 : mCamera->setDetailLevel(SDL_SOLID) ; break ;
				case 1 : mCamera->setDetailLevel(SDL_WIREFRAME) ; break ;
				case 2 : mCamera->setDetailLevel(SDL_POINTS) ; break ;
			}
			mTimeUntilNextToggle = 0.5;
		}
		// Return true to continue rendering
		return true;
	}

    bool processUnbufferedMouseInput(const FrameEvent& evt)
    {
        /* Rotation factors, may not be used if the second mouse button is pressed. */

        /* If the second mouse button is pressed, then the mouse movement results in 
           sliding the camera, otherwise we rotate. */
		if( mInputDevice->getMouseButton( 0 ) && mTimeUntilNextToggle <=0 ) // left mouse button pressed
        {
			// activate mouse picking of tiles

			this->pTileManager->get_TileInterface()->get_Mouse()->move_Relative
				(mInputDevice->getMouseRelativeX(), - mInputDevice->getMouseRelativeY());

			this->pTileManager->get_TileInterface()->pick_Tile();

			mTimeUntilNextToggle = 0.05;

        }
		else if( mInputDevice->getMouseButton( 1 ) )
        {
			mRotX = -mInputDevice->getMouseRelativeX() * 0.013;
            mRotY = -mInputDevice->getMouseRelativeY() * 0.013;

            // mTranslateVector.x += mInputDevice->getMouseRelativeX() * 0.013;
            // mTranslateVector.y -= mInputDevice->getMouseRelativeY() * 0.013;
        }
        else
        {
            // mRotX = -mInputDevice->getMouseRelativeX() * 0.013;
            // mRotY = -mInputDevice->getMouseRelativeY() * 0.013;

			this->pTileManager->get_TileInterface()->get_Mouse()->move_Relative
				(mInputDevice->getMouseRelativeX(), - mInputDevice->getMouseRelativeY());
        }
		return true;
	}

	void moveCamera()
	{
		// Make all the changes to the camera
		// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW (e.g. airplane)
		mCamera->yaw(Radian(mRotX));
		mCamera->pitch(Radian(mRotY));
		Real camPitch = mCamera->getOrientation().getPitch().valueDegrees();
		if (mCamera->getDirection().z < 0) {
            if (camPitch < -70)
				mCamera->pitch(Degree(-70 - camPitch));
			else if (camPitch > 70)
				mCamera->pitch(Degree(70 - camPitch));
		}
		else if (mCamera->getDirection().z > 0){
            if (camPitch < 110 && camPitch > 0)
				mCamera->pitch(Degree(110 - camPitch));
			else if (camPitch > -110 && camPitch < 0)
				mCamera->pitch(Degree(-110 - camPitch));
		}

		mCamera->move(mCamera->getOrientation() * mTranslateVector );	
		Vector3 pos = mCamera->getPosition();
		if (pos.y <100.0) mCamera->setPosition(pos.x, 100.0, pos.z);
	}

	void showDebugOverlay(bool show)
	{
		if (mDebugOverlay)
		{
			if (show) { mDebugOverlay->show(); }
			else			{ mDebugOverlay->hide(); }
		}
	}

	// Override frameStarted event to process that (don't care about frameEnded)
	bool frameStarted(const FrameEvent& evt)
	{
		if (++counter >= 100)
		{
			static bool once = true;
			if (once)
			{
				mCamera->setPosition(410, 450, 410);
				mCamera->yaw(Degree(270));
				once = false;
			}
			pTileManager->ControlChunks(mCamera->getPosition());
			counter = 0;
		}
		if (!mInputTypeSwitchingOn)
		{
			mInputDevice->capture();
		}

		if ( !mUseBufferedInputMouse || !mUseBufferedInputKeys)
		{
			// one of the input modes is immediate, so setup what is needed for immediate mouse/key movement
			if (mTimeUntilNextToggle >= 0) 
				mTimeUntilNextToggle -= evt.timeSinceLastFrame;

			// If this is the first frame, pick a speed
			if (evt.timeSinceLastFrame == 0)
			{
				mMoveScale = 1;
				mRotScale = 0.1;
			}
			// Otherwise scale movement units by time passed since last frame
			else
			{
				// Move about 100 units per second,
				mMoveScale = 100.0 * evt.timeSinceLastFrame;
				// Take about 10 seconds for full rotation
				mRotScale = 36 * evt.timeSinceLastFrame;
			}
			mRotX = 0;
            mRotY = 0;
	        mTranslateVector = Vector3::ZERO;
		}

        if (mUseBufferedInputKeys)
        {
            // no need to do any processing here, it is handled by event processor and 
			// you get the results as KeyEvents
        }
        else
        {
            if (processUnbufferedKeyInput(evt) == false)
			{
				return false;
			}
        }
        if (mUseBufferedInputMouse)
        {
            // no need to do any processing here, it is handled by event processor and 
			// you get the results as MouseEvents

        }
        else
        {
            if (processUnbufferedMouseInput(evt) == false)
			{
				return false;
			}
        }

		if ( !mUseBufferedInputMouse || !mUseBufferedInputKeys)
		{
			// one of the input modes is immediate, so update the movement vector

			moveCamera();

		}

		return true;
    }

    bool frameEnded(const FrameEvent& evt)
    {
        updateStats();
        return true;
    }

	void switchMouseMode() 
	{
        mUseBufferedInputMouse = !mUseBufferedInputMouse;
		mInputDevice->setBufferedInput(mUseBufferedInputKeys, mUseBufferedInputMouse);
	}
	void switchKeyMode() 
	{
        mUseBufferedInputKeys = !mUseBufferedInputKeys;
		mInputDevice->setBufferedInput(mUseBufferedInputKeys, mUseBufferedInputMouse);
	}

	void keyClicked(KeyEvent* e) 
	{
		if (e->getKeyChar() == 'm')
		{
			switchMouseMode();
		}
		else if (e->getKeyChar() == 'k')
		{

			switchKeyMode();
		}
	}
	void keyPressed(KeyEvent* e) {}
	void keyReleased(KeyEvent* e) {}

	void Set_pTileManager(TileManager* TileManager)
	{
		pTileManager = TileManager;
	}

protected:
    EventProcessor* mEventProcessor;
    InputReader* mInputDevice;
    Camera* mCamera;

    Vector3 mTranslateVector;
    RenderWindow* mWindow;
    bool mStatsOn;
    bool mUseBufferedInputKeys, mUseBufferedInputMouse, mInputTypeSwitchingOn;
	unsigned int mNumScreenShots;
    float mMoveScale;
    float mRotScale;
    // just to stop toggles flipping too fast
    Real mTimeUntilNextToggle ;
    float mRotX, mRotY;
    TextureFilterOptions mfiltering;
    int mAniso;
	long counter;
	TileManager* pTileManager;
};

#endif
