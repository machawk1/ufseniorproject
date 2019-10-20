#include "ExampleApplication.h"
#include "BobC.h"
#include "ProjektFrameListener.h"
#include <deque>
#include <math.h>
#include "main.h"
using namespace std;

void initBobArray();

class MoveDemoListener : public ProjektFrameListener
{
public:

    MoveDemoListener(RenderWindow* win, Camera* cam, BobC* b)
        : ProjektFrameListener(win, cam, false, false)
    {
		bob = b;
        mWalkSpeed = 10.0f;
        mDirection = Vector3::ZERO;
		bobExists = mTileManager->Get_bobExists();

		initBobArray();

    } // MoveDemoListener

    /* This function is called to start the object moving to the next position
       in mWalkList.
    */
    bool nextLocation( )
    {
		if ( mWalkList.empty() )
            return false;
		mDestination = mWalkList.front( );  // this gets the front of the deque
        mWalkList.pop_front( );             // this removes the front of the deque

        mDirection = mDestination - mNode->getPosition( );
        mDistance = mDirection.normalise( );
		Vector3 src = mNode->getOrientation( ) * Vector3::UNIT_X;
	
		mDirectionNoY = mDirection;
		mDirectionNoY.y = 0;

        if ( (1.0f + src.dotProduct( mDirectionNoY )) < 0.0001f )
            mNode->yaw( Degree(180) );
        else
        {
            Ogre::Quaternion quat = src.getRotationTo( mDirectionNoY );

            mNode->rotate( quat );
	
        } // else
        return true;
    } // nextLocation( )

    bool frameStarted(const FrameEvent &evt)
    {
		if ( mDirection != Vector3::ZERO )
        {
            Real move = mWalkSpeed * evt.timeSinceLastFrame;
            mDistance -= move;
			if (mDistance <= 0.0f)
            {
                mNode->setPosition( mDestination );
                mDirection = Vector3::ZERO;
				if (! nextLocation( ) )
                {   
                    bob[currBob].mAnimationState = bob[currBob].mEntity->getAnimationState( "Idle" );
                    bob[currBob].mAnimationState->setLoop( true );
                    bob[currBob].mAnimationState->setEnabled( true );
                } // if
            } // if
			else
            {
                mNode->translate( mDirection * move );
				
            } // else
        } // if
		
		if ( bobExists == true )
		{
			for (int i = 0; i < MAX_BOB_AMOUNT; i++) 
			{
				bob[i].mAnimationState->addTime( evt.timeSinceLastFrame );   
			}
		}

        return ProjektFrameListener::frameStarted(evt);
    }

	void addWalkList(deque<Vector3> &walk, int cB) 
	{
        mWalkList = walk;
		currBob = cB;
        mEntity = bob[currBob].mEntity;
        mNode = bob[currBob].mNode;
		if ( nextLocation() )
        {
            // Set walking animation
            bob[currBob].mAnimationState = mEntity->getAnimationState( "Walk" );
            bob[currBob].mAnimationState->setLoop( true );
            bob[currBob].mAnimationState->setEnabled( true );
        } // if
        else
        {
            // Set idle animation
            bob[currBob].mAnimationState = mEntity->getAnimationState( "Idle" );
            bob[currBob].mAnimationState->setLoop( true );
            bob[currBob].mAnimationState->setEnabled( true );
        } // else
	}

	void initBobArray()
	{
		for (int i = 0; i < MAX_BOB_AMOUNT; i++) 
		{
			if ( bobExists == true )
			{
				mEntity = bob[i].mEntity;
				bob[i].mAnimationState = mEntity->getAnimationState( "Idle" );
				bob[i].mAnimationState->setLoop( true );
				bob[i].mAnimationState->setEnabled( true );
			}
		}
		
	}

protected:
	int currBob;
	BobC *bob;
	bool bobExists;
    Real mDistance;                  // The distance the object has left to travel
    Vector3 mDirection;              // The direction the object is moving
	Vector3 mDirectionNoY;
    Vector3 mDestination;            // The destination the object is moving towards
    
    Entity *mEntity;                 // The Entity we are animating
    SceneNode *mNode;                // The SceneNode that the Entity is attached to
    std::deque<Vector3> mWalkList;   // The list of points we are walking to

    Real mWalkSpeed;                 // The speed at which the object is moving
	TileManager *mTileManager;

};

