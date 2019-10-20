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
//mem probs without this next one
#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include "OgreCEGUIRenderer.h"
#include "OgreCEGUIResourceProvider.h"
//regular mem handler
#include <OgreMemoryMacros.h>

#include <CEGUI/elements/CEGUIPushButton.h>
#include <CEGUI/elements/CEGUIFrameWindow.h>
#include <CEGUI/elements/CEGUIStaticText.h>
#include <CEGUI/elements/CEGUIEditbox.h>

#include <CEGUI/elements/CEGUIListbox.h>
#include <CEGUI/elements/CEGUIListboxTextItem.h>

#include "Ogre.h"
#include "main.h"
#include "TileChunk.h"
#include "BobC.h"

#include "BobsNetwork.h"
#include "ArrayList.h"

#define CLIENT_PORT  9001
#define SERVER_PORT  9000
#define CLIENT_PORT2 9002
#define CLIENT_PORT3 9003
#define CLIENT_PORT4 9004

using BasicDataStructures::List;

unsigned char GetPacketIdentifier(Packet *p);

//#define BIND_CEGUI_EVENT(window, event, method) window->subscribeEvent(event, CEGUI::Event::Subscriber(&method, this));

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
		bool log=true;
		if(log) Logger::log().headline("Enter SampleApp createScene() method:");
		mTileManager = new TileManager();
		mTileManager->Init(mSceneMgr);
		mSceneMgr->setAmbientLight(ColourValue(1,1,1));
		mTileManager->Set_Bob_Array(bob);
		mTileManager->Set_Walklist(&Walklist);
		
		for ( int i = 0; i < MAX_BOB_AMOUNT; ++i)
		{
			bob[i].active = false;
			char EntityName[20];
		
			sprintf( EntityName, "Bob[%d]", i );
	
			bob[i].model = "robot.mesh";
			bob[i].mEntity = mTileManager->Get_pSceneManager()->createEntity( EntityName, bob[i].model );

			bob[i].mAnimationState = bob[i].mEntity->getAnimationState( "Idle" );
            bob[i].mAnimationState->setLoop( true );
            bob[i].mAnimationState->setEnabled( true );
		}

		// setup GUI system
		if(log) Logger::log().info() << "trying to set up mGUIRenderer";
		mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000);
		if(log) Logger::log().info() << "done setting up mGUIRenderer";

		mGUISystem = new CEGUI::System(mGUIRenderer);
		CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

		if(log) Logger::log().info() << "starting to load TaharezLook.scheme";
		CEGUI::SchemeManager::getSingleton().loadScheme((CEGUI::utf8*)"TaharezLook.scheme");
		if(log) Logger::log().info() << "done loading TaharezLook.scheme";

		mGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
		mGUISystem->setDefaultFont((CEGUI::utf8*)"Tahoma-12");

		mEditorGuiSheet = CEGUI::WindowManager::getSingleton().loadWindowLayout("layout1.layout");
		mGUISystem->setGUISheet(mEditorGuiSheet);

		//mEditorGuiSheet= CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"DefaultWindow", (CEGUI::utf8*)"Sheet");  
		//mGUISystem->setGUISheet(mEditorGuiSheet);
		//Logger::log().info() << "done setting mEditorGuiSheet";

		//CEGUI::Editbox* textBox = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Editbox", (CEGUI::utf8*)"TextBox");
		//mEditorGuiSheet->addChildWindow(textBox);
		//textBox->setPosition(CEGUI::Point(0.03f, 0.7f));
		//textBox->setSize(CEGUI::Size(0.3f, 0.07f));
		//textBox->setText("Type here.");

		//CEGUI::Window* mMainWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("chat.layout");
		//if (mMainWindow) mEditorGuiSheet->addChildWindow(mMainWindow);
		//BIND_CEGUI_EVENT(textBox, CEGUI::Editbox::EventTextAccepted, SampleApp::TextBoxEdit)

		initializeVariables();

		setupEventHandlers();

		//mSceneMgr->setSkyDome( true, "Examples/CloudySky", 5, 8 );
		if(log) Logger::log().info() << "Exit SampleApp createScene() method.";
	}

	void initializeVariables(void)
	{
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
		MeleeAttack = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"MeleeAttack");
		RangedAttack = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"RangedAttack");
		LightningAttack = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"LightningAttack");
		Heal = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"Heal");
		Quit = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"Quit");
		CONNECT = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"CONNECT");
		BuyBob = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"BuyBob");
		Done = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"Done");

		HPIncrease = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"HPIncrease");
		sprintf(temp, "HP Upgrade (%d)", PassiveLightHPIncreaseAbility.getBuyCost());
		HPIncrease->setText(temp);

		MediumRangedButton = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"MediumRangedButton");
		sprintf(temp, "Medium Ranged (%d)", MediumRangedAbility.getBuyCost());
		MediumRangedButton->setText(temp);
		
		Lightning = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"Lightning");
		sprintf(temp, "Lightning (%d)", LightLightningAOEAbility.getBuyCost());
		Lightning->setText(temp);
		
		MediumHealButton = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"MediumHealButton");
		sprintf(temp, "Medium Heal (%d)", MediumHealAbility.getBuyCost());
		MediumHealButton->setText(temp);
		
		LightDodge = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"LightDodge");
		sprintf(temp, "Light Dodge Bonus (%d)", PassiveLightDodgeAbility.getBuyCost());
		LightDodge->setText(temp);

		LightArmor = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"LightArmor");
		sprintf(temp, "Light Armor (%d)", PassiveLightArmorAbility.getBuyCost());
		LightArmor->setText(temp);

		LightMove = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"LightMove");
		sprintf(temp, "Light Move Boost (%d)", PassiveLightMoveBoostAbility.getBuyCost());
		LightMove->setText(temp);

		Ready = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"Ready");

		TextPanel = (CEGUI::StaticText*)wmgr.getWindow((CEGUI::utf8*)"TextPanel");

		TextBox = (CEGUI::Editbox*)wmgr.getWindow((CEGUI::utf8*)"TextBox");
		IPTextBox = (CEGUI::Editbox*)wmgr.getWindow((CEGUI::utf8*)"IPTextBox");
		HandleTextBox = (CEGUI::Editbox*)wmgr.getWindow((CEGUI::utf8*)"HandleTextBox");
		PortTextBox = (CEGUI::Editbox*)wmgr.getWindow((CEGUI::utf8*)"PortTextBox");

		FWindow = (CEGUI::FrameWindow*)wmgr.getWindow((CEGUI::utf8*)"FWindow");
		FWindow->hide();
		TextBox->hide();
        
		//mEditBox = static_cast<CEGUI::Editbox*> (CEGUI::WindowManager::getSingleton().getWindow("Chat/Wnd/Edit"));
		//mListBox = static_cast<CEGUI::Listbox*> (CEGUI::WindowManager::getSingleton().getWindow("Chat/Wnd/List"));
		

	}

	void setupEventHandlers(void)
	{
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
		wmgr.getWindow((CEGUI::utf8*)"Quit")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleQuit, this));
		wmgr.getWindow((CEGUI::utf8*)"MeleeAttack")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleMeleeAttack, this));
		wmgr.getWindow((CEGUI::utf8*)"RangedAttack")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleRangedAttack, this));
		wmgr.getWindow((CEGUI::utf8*)"LightningAttack")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleLightningAttack, this));
		wmgr.getWindow((CEGUI::utf8*)"Heal")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleHeal, this));
		//wmgr.getWindow((CEGUI::utf8*)"Chat/Wnd/Edit")->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&SampleApp::EditTextAccepted, this));
		wmgr.getWindow((CEGUI::utf8*)"TextBox")->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&SampleApp::TextBoxEdit, this));

		wmgr.getWindow((CEGUI::utf8*)"CONNECT")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleCONNECT, this));
		wmgr.getWindow((CEGUI::utf8*)"BuyBob")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleBuyBob, this));
		wmgr.getWindow((CEGUI::utf8*)"Done")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleDone, this));
		wmgr.getWindow((CEGUI::utf8*)"HPIncrease")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleHPIncrease, this));
		wmgr.getWindow((CEGUI::utf8*)"MediumRangedButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleMediumRanged, this));
		wmgr.getWindow((CEGUI::utf8*)"Lightning")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleLightning, this));
		wmgr.getWindow((CEGUI::utf8*)"MediumHealButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleMediumHeal, this));
		wmgr.getWindow((CEGUI::utf8*)"LightDodge")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleLightDodge, this));
		wmgr.getWindow((CEGUI::utf8*)"LightArmor")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleLightArmor, this));
		wmgr.getWindow((CEGUI::utf8*)"LightMove")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleLightMove, this));
		wmgr.getWindow((CEGUI::utf8*)"Ready")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SampleApp::handleReady, this));
	}

	bool TextBoxEdit(const CEGUI::EventArgs& args)
	{
		TextPanel->setText(TextBox->getText());
		TextBox->setText("");
		return true;

	}

	/*bool SampleApp::EditTextAccepted(const CEGUI::EventArgs& args)
	{
		//add text to list
		CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(mEditBox->getText());
		mListBox->addItem ( item );
		mListBox->ensureItemIsVisible(mListBox->getItemCount());
		//remove old text
		mEditBox->setText("");

		return true;
	}*/

	bool handleQuit(const CEGUI::EventArgs& e)
	{
		static_cast<MoveDemoListener*>(mFrameListener)->requestShutdown();
		return true;
	}

	bool handleMeleeAttack(const CEGUI::EventArgs& e)
	{
		TextPanel->setText("Melee attack.");
		return true;
	}

	bool handleRangedAttack(const CEGUI::EventArgs& e)
	{
		TextPanel->setText("Ranged attack.");
		return true;
	}

	bool handleLightningAttack(const CEGUI::EventArgs& e)
	{
		TextPanel->setText("Lightning attack.");
		return true;
	}

	bool handleHeal(const CEGUI::EventArgs& e)
	{
		TextPanel->setText("Heal.");
		return true;
	}

	bool handleCONNECT(const CEGUI::EventArgs& e)
	{
		CONNECT->setText("Connecting...");
		// Handles connection to SERVER
		String buffer, buffer2;

		// Set Client
		BobsClient=RakNetworkFactory::GetRakClientInterface();

		buffer = HandleTextBox->getText().c_str();
		strcpy(ClientData.name, buffer.c_str());

		// Connect to Server
		// NEED TO CHANGE UNASSIGNED_PLAYER_ID HERE FOR MULTIPLE CONNECTIONS
		BobsClient->SetStaticClientData(UNASSIGNED_PLAYER_ID, (char*)&ClientData, sizeof(StaticClientDataStruct));

		buffer = IPTextBox->getText().c_str();
		strcpy(ClientData.ip, buffer.c_str());

		buffer = PortTextBox->getText().c_str();
		strcpy(ClientData.Port, buffer.c_str());

		// Set Port Data
		if (ClientData.Port[0] == 0)
		{
			BobsClient->Connect(ClientData.ip, SERVER_PORT, CLIENT_PORT, 0, 30);
		}
		else
		{
			BobsClient->Connect(ClientData.ip, SERVER_PORT, CLIENT_PORT2, 0, 30);
		}

		// Display Client Data
		buffer = ClientData.name;
		buffer2 = ClientData.ip;
		buffer = buffer + '\n' + buffer2;

		TextPanel->setText(buffer);
		IPTextBox->hide();
		HandleTextBox->hide();
		PortTextBox->hide();

		AP=0;
		SetUpLooping = true;

		while (SetUpLooping)
		{
			Sleep(30);
			/****************************/
			/* Receive Packet
			/****************************/
			p = BobsClient->Receive();								
			if (p==0) continue;										
			packetIdentifier = GetPacketIdentifier(p);

			/***************************************************************************/
			/* Pointers to Received Structures
			/***************************************************************************/		
			ClientsAP* theAP = (ClientsAP *)p->data;				//POINTS TO RECEIVED STRUCTURE(AP)
			ServerMap* theMap = (ServerMap*)p->data;				//RECEIVE BOBS POSITION
			NumberOfBobs* theBobID = (NumberOfBobs *)p->data;					
			BlockedTiles *theBlocked = (BlockedTiles*)p->data;

			/****************************/
			/* Identify Packet
			/****************************/
			bool occupied = false;	
			bool upgradingBob;
			bool purchase;
			switch (packetIdentifier)
			{
				/***************************************************************************/
				/* Player Disconnected
				/***************************************************************************/
				case ID_DISCONNECTION_NOTIFICATION:					//PLAYER DISCONNECTED
					//printf("(%d) DISCONNECTED : \n", p->playerId);
					break;

				/***************************************************************************/
				/* Player Connecting
				/***************************************************************************/
				case ID_NEW_INCOMING_CONNECTION:					//NEW PLAYER CONNECTED
					//printf("%s HAS ENTER ROOM",((StaticClientDataStruct*)(BobsClient->GetStaticClientData(p->playerId))->GetData())->name);
					break;
				case RETRIEVE_AP:									//RECEIVED ACTION POINTS
					AP=theAP->AP;
					ClientData.myplayerid = theAP->myplayerid;
					
					sprintf(temp, "Buy Points: %d", AP);
					buffer = temp;
	 
					// Display buy points
					TextPanel->setText(buffer);

					// Display Bob Store Buttons
					BuyBob->setEnabled(true);
					BuyBob->setVisible(true);

					SetUpLooping = false;				
					break;
					//purchase = true;
				default:
					break;
			}
			BobsClient->DeallocatePacket(p);
		}

		// Hide Connect Button
		CONNECT->disable();
		CONNECT->hide();

		return true;
	}	

	bool handleBuyBob(const CEGUI::EventArgs& e)
	{
		String buffer;

		if (AP < 10)	// Not enough AP
		{
			BuyBob->setEnabled(false);
		}

		else
		{
			// Bob Cost
			AP = AP - 10;

			// Initialize Bob Structure
			bobsStruct.typeID = INITIALIZED_BOB;
			bobsStruct.apBase = 1;
			bobsStruct.passiveSize = 0;
			bobsStruct.activeHPSize = 0;
			bobsStruct.activeStatSize = 0;
			bobsStruct.bobsNumber = 0;

			// Display new AP
			sprintf(temp, "Buy Points: %d\0", AP);
			buffer = temp;
			TextPanel->setText(buffer);

			BuyBob->setEnabled(false);
			Ready->setEnabled(false);

			// Enable Store Buttons
			Done->setVisible(true);
			HPIncrease->setVisible(true);
			MediumRangedButton->setVisible(true);
			Lightning->setVisible(true);
			MediumHealButton->setVisible(true);
			LightDodge->setVisible(true);
			LightArmor->setVisible(true);
			LightMove->setVisible(true);

			Done->setEnabled(true);
			HPIncrease->setEnabled(true);
			MediumRangedButton->setEnabled(true);
			Lightning->setEnabled(true);
			MediumHealButton->setEnabled(true);
			LightDodge->setEnabled(true);
			LightArmor->setEnabled(true);
			LightMove->setEnabled(true);
		}

		return true;
	}	

	bool handleDone(const CEGUI::EventArgs& e)
	{
		// Send Bob to Server
		bobsStruct.bobsNumber = bobsStruct.bobsNumber + 1;
		BobsClient->Send((char*)&bobsStruct,sizeof(bobsStruct),HIGH_PRIORITY,RELIABLE_ORDERED,0);
		bobsStruct.apBase = 10;
		bobsStruct.passiveSize = 0;
		bobsStruct.activeHPSize = 0;
		bobsStruct.activeStatSize = 0;

		sprintf(temp, "Buy Points: %d\0", AP);
		TextPanel->setText(temp);

		// Disable Store Buttons
		Done->setEnabled(false);
		HPIncrease->setEnabled(false);
		MediumRangedButton->setEnabled(false);
		Lightning->setEnabled(false);
		MediumHealButton->setEnabled(false);
		LightDodge->setEnabled(false);
		LightArmor->setEnabled(false);
		LightMove->setEnabled(false);

		Ready->setEnabled(true);
		Ready->setVisible(true);

		if(AP < 10)
		{	
			// Not enough points left, wait for other players to finish buying bobs
			// Remove Store Buttons
			HPIncrease->hide();
			MediumRangedButton->hide();
			Lightning->hide();
			MediumHealButton->hide();
			LightDodge->hide();
			LightArmor->hide();
			LightMove->hide();

			Done->setVisible(false);
			BuyBob->setVisible(true);
		}
		else
		{	// Buy more Bobs
			BuyBob->setEnabled(true);
		}
		return true;
	}	

	bool handleHPIncrease(const CEGUI::EventArgs& e)
	{
		String buffer;
		if(AP < PassiveLightHPIncreaseAbility.getBuyCost())
		{
			HPIncrease->setEnabled(false);
			sprintf(temp, "Buy Points: %d\nInsufficient rupees", AP);
			buffer = temp;
			TextPanel->setText(buffer);
		}
		else
		{
			bobsStruct.passiveAbilities[bobsStruct.passiveSize] = PassiveLightHPIncrease;
			bobsStruct.passiveSize++;
			AP = AP - PassiveLightHPIncreaseAbility.getBuyCost();

			sprintf(temp, "Buy Points: %d\0", AP);
			buffer = temp;
			TextPanel->setText(buffer);

			HPIncrease->setEnabled(false);
		}
		return true;
	}	

	bool handleMediumRanged(const CEGUI::EventArgs& e)
	{
		String buffer;
		if(AP < MediumRangedAbility.getBuyCost())
		{
			MediumRangedButton->setEnabled(false);
			sprintf(temp, "Buy Points: %d\nInsufficient rupees", AP);
			buffer = temp;
			TextPanel->setText(buffer);
		}
		else
		{
			bobsStruct.activeHPMods[bobsStruct.activeHPSize] = MediumRanged;
			bobsStruct.activeHPSize++;
			AP = AP - MediumRangedAbility.getBuyCost();

			sprintf(temp, "Buy Points: %d\0", AP);
			buffer = temp;
			TextPanel->setText(buffer);
			MediumRangedButton->setEnabled(false);
		}
		return true;
	}	

	bool handleLightning(const CEGUI::EventArgs& e)
	{
		String buffer;
		if(AP < LightLightningAOEAbility.getBuyCost())
		{
			Lightning->setEnabled(false);
			sprintf(temp, "Buy Points: %d\nInsufficient rupees", AP);
			buffer = temp;
			TextPanel->setText(buffer);
		}
		else
		{
			bobsStruct.activeHPMods[bobsStruct.activeHPSize] = LightLightningAOE;
			bobsStruct.activeHPSize++;
			AP = AP - LightLightningAOEAbility.getBuyCost();

			sprintf(temp, "Buy Points: %d\0", AP);
			buffer = temp;
			TextPanel->setText(buffer);
			Lightning->setEnabled(false);
		}
		return true;
	}

	bool handleMediumHeal(const CEGUI::EventArgs& e)
	{
		String buffer;
		if(AP < MediumHealAbility.getBuyCost())
		{
			MediumHealButton->setEnabled(false);
			sprintf(temp, "Buy Points: %d\nInsufficient rupees", AP);
			buffer = temp;
			TextPanel->setText(buffer);
		}
		else
		{
			bobsStruct.activeHPMods[bobsStruct.activeHPSize] = MediumHeal;
			bobsStruct.activeHPSize++;
			AP = AP - MediumHealAbility.getBuyCost();

			sprintf(temp, "Buy Points: %d\0", AP);
			buffer = temp;
			TextPanel->setText(buffer);
			MediumHealButton->setEnabled(false);
		}
		return true;
	}	

	bool handleLightDodge(const CEGUI::EventArgs& e)
	{
		String buffer;
		if(AP < PassiveLightDodgeAbility.getBuyCost())
		{
			LightDodge->setEnabled(false);
			sprintf(temp, "Buy Points: %d\nInsufficient rupees", AP);
			buffer = temp;
			TextPanel->setText(buffer);
		}
		else
		{
			bobsStruct.passiveAbilities[bobsStruct.passiveSize] = PassiveLightDodge;
			bobsStruct.passiveSize++;
			AP = AP - PassiveLightDodgeAbility.getBuyCost();

			sprintf(temp, "Buy Points: %d\0", AP);
			buffer = temp;
			TextPanel->setText(buffer);
			LightDodge->setEnabled(false);
		}
		return true;
	}	

	bool handleLightArmor(const CEGUI::EventArgs& e)
	{
		String buffer;
		if(AP < PassiveLightArmorAbility.getBuyCost())
		{
			LightArmor->setEnabled(false);
			sprintf(temp, "Buy Points: %d\nInsufficient rupees", AP);
			buffer = temp;
			TextPanel->setText(buffer);
		}
		else
		{
			bobsStruct.passiveAbilities[bobsStruct.passiveSize] = PassiveLightArmor;
			bobsStruct.passiveSize++;
			AP = AP - PassiveLightArmorAbility.getBuyCost();

			sprintf(temp, "Buy Points: %d\0", AP);
			buffer = temp;
			TextPanel->setText(buffer);
			LightArmor->setEnabled(false);
		}
		return true;
	}	

	bool handleLightMove(const CEGUI::EventArgs& e)
	{
		String buffer;
		if(AP < PassiveLightMoveBoostAbility.getBuyCost())
		{
			LightMove->setEnabled(false);
			sprintf(temp, "Buy Points: %d\nInsufficient rupees", AP);
			buffer = temp;
			TextPanel->setText(buffer);
		}
		else
		{
			bobsStruct.passiveAbilities[bobsStruct.passiveSize] = PassiveLightArmor;
			bobsStruct.passiveSize++;
			AP = AP - PassiveLightMoveBoostAbility.getBuyCost();

			sprintf(temp, "Buy Points: %d\0", AP);
			buffer = temp;
			TextPanel->setText(buffer);
			LightMove->setEnabled(false);
		}
		return true;
	}	

	bool handleReady(const CEGUI::EventArgs& e)
	{
		BuyBob->setVisible(false);
		Done->setVisible(false);
		HPIncrease->setVisible(false);
		MediumRangedButton->setVisible(false);
		Lightning->setVisible(false);
		MediumHealButton->setVisible(false);
		LightDodge->setVisible(false);
		LightArmor->setVisible(false);
		LightMove->setVisible(false);
		Ready->setVisible(false);
		TextPanel->setText("READY. What's next?");
		return true;
	}	

public:
	~SampleApp()
	{
		if (mTileManager) {delete mTileManager; }
		if (mFrameListener) { delete mFrameListener; }
		if (mRoot) { delete mRoot; }
	}

	/************************/
	/* Variable Declaration
	/***********************/
	char temp[512];												
	char temp2[512];
	char ip[20];
	int AP;				
	int i, j;
	int mapHeight;
	int mapLength;
	int x,y;
	int av;
	int usedx[10];
	int usedy[10];
	int usex;
	int usey;
	int numInput;	
	int nbid;
	bool validinput;
	bool SetUpLooping;	

	/************************/
	/* Max Store Items
	/***********************/
	bool maxhp;// = true;
	int maxh;// = 3;
	bool maxlight;// = true;
	int maxl;// = 4;
	bool maxattack;// = true;
	int maxa;// = 5;

	/************************/
	/* Identify Packet 
	/***********************/
	unsigned char packetIdentifier;							
	Packet* p;		

	/************************/
	/* ArrayList Declaration
	/***********************/
	List<char> bobplayer1;
	List<int> bobID1;
	List<int> bobx1;
	List<int> boby1;
	List<int> bobhp1;
	List<int> bobap1;
	List<int> avx1;
	List<int> avy1;
	List<int> avcost1;

	List<int> blockedX;
	List<int> blockedY;

	/*************************/
	/* Structures Declaration
	/************************/
	BobsStruct bobsStruct;										
		
	Flag flag;													

	NumberOfBobs numOfBobs;							

	BobsPosition bobsPosition;								
	//bobsPosition.typeID = PLACE_BOB;
	StaticClientDataStruct ClientData;
	RakClientInterface *BobsClient;


private:
	//List of buttons for easy access. Add any new buttons you make here.
	CEGUI::PushButton *MeleeAttack,*RangedAttack,*LightningAttack, *Heal,*Quit,*CONNECT,*BuyBob,*Done,*HPIncrease,*MediumRangedButton,*Lightning,
		*MediumHealButton,*LightDodge,*LightArmor,*LightMove,*Ready, *Test;
	CEGUI::StaticText *TextPanel;
	CEGUI::Editbox *TextBox,*IPTextBox,*HandleTextBox,*PortTextBox;
	CEGUI::FrameWindow *FWindow;


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

//// GET PACKETS 
//unsigned char GetPacketIdentifier(Packet *p)
//{
//	if (p==0)
//		return 255;
//
//	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
//	{
//		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
//		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
//	}
//	else
//		return (unsigned char) p->data[0];
//}