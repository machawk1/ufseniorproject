#include "ExampleApplication.h"
#include "BobC.h"
#include "ProjektFrameListener.h"
#include <deque>
#include <math.h>
#include "main.h"
using namespace std;


class MoveDemoListener : public ProjektFrameListener
{
public:

    MoveDemoListener(RenderWindow* win, Camera* cam, BobC* b, std::deque<Vector3>* list, CEGUI::Renderer* renderer,int *bIndex)
        : ProjektFrameListener(win, cam, true, true, renderer) 
		// boolean inputs control whether to filter key and mouse inputs respectively, 
		// not filtering means using frameStarted() to check for what key was pressed and checking for match
    {
		//bobIndex = bIndex;
		//bIndex = ProjektFrameListener::pTileManager->get_TileInterface()->get_Selection()->returnBobIndex();
		bobIndex = bIndex;
		bob = b;
		mWalkList = list;
        mWalkSpeed = 20.0f;
        mDirection = Vector3::ZERO;

    } // MoveDemoListener

    /* This function is called to start the object moving to the next position
       in mWalkList.
    */
    bool nextLocation( )
    {
		if ( mWalkList->empty() )
            return false;
		mNode = ProjektFrameListener::pTileManager->get_TileInterface()->get_Selection()->getSelected_Bob();

		if (mDirection == Vector3::ZERO)
		{
			mDestination = mWalkList->front( );  // this gets the front of the deque
			mWalkList->pop_front( );             // this removes the front of the deque
			mDirection = mDestination - mNode->getPosition( );
			mDistance = mDirection.normalise( );
			Vector3 src = mNode->getOrientation( ) * Vector3::UNIT_X;
	
			mDirectionNoY = mDirection;
			mDirectionNoY.y = 0;

			if ( 1.0f + ( src.dotProduct( mDirectionNoY )) < 0.2f && 1.0f + ( src.dotProduct( mDirectionNoY )) > -0.2f)
				mNode->yaw( Degree(180) );
			else 
			{
				Ogre::Quaternion quat = src.getRotationTo( mDirectionNoY );

				mNode->rotate( quat );
			} // else
		}
        return true;
    } // nextLocation( )

    bool frameStarted(const FrameEvent &evt)
    {
		if ( nextLocation() || mDirection != Vector3::ZERO )
        {
            Real move = mWalkSpeed * evt.timeSinceLastFrame;
            mDistance -= move;
			if (mDistance <= 0.0f)
            {
                mNode->setPosition( mDestination );
                mDirection = Vector3::ZERO;
				if (! nextLocation( ) )
                {   
                    setIdle();
                } // if
            } // if
			else
            {
                mNode->translate( mDirection * move );
				
            } // else
        } // if
		
		for (int i = 0; i < MAX_BOB_AMOUNT; i++) 
		{
			if ( bob[i].active == true )
			{
			    //Logger::log().info() << "Adding Time here for each bob";
			    bob[i].mAnimationState->addTime( evt.timeSinceLastFrame );   
			}
		}

        return ProjektFrameListener::frameStarted(evt);
    }

	void setIdle()
	{
		for (int i = 0; i < MAX_BOB_AMOUNT; i++) 
		{
				mEntity = bob[i].mEntity;
				bob[i].mAnimationState = mEntity->getAnimationState( "Idle" );
				bob[i].mAnimationState->setLoop( true );
				bob[i].mAnimationState->setEnabled( true );
		}
		
	}

	int * bobIndex;

protected:
	//int * bobIndex;
	BobC *bob;
	bool bobExists;
    Real mDistance;                  // The distance the object has left to travel
    Vector3 mDirection;              // The direction the object is moving
	Vector3 mDirectionNoY;
    Vector3 mDestination;            // The destination the object is moving towards
    
    Entity *mEntity;                 // The Entity we are animating
    SceneNode *mNode;                // The SceneNode that the Entity is attached to
    std::deque<Vector3>* mWalkList;   // The list of points we are walking to

    Real mWalkSpeed;                 // The speed at which the object is moving

};

