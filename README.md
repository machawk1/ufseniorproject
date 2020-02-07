# ufseniorproject
Game for Dave Small's Senior Project development group at the University of Florida. This was originally developed in 2006 and stored using [CVS on Sourceforge](https://sourceforge.net/projects/ufseniorproject/).

This repo contains the entire VS7.1 project as well as compiled versions of client and server executables.

## Repo contents

* A folder of the Client and Server Visual Studio 7.1 projects.
* An executable called BobsNetworkGame.exe inside the server Debug folder which is the compiled server.
* An executable called GraphicsEngine.exe inside the client folder which is the client.

# Usage

To run the game to play

## Run BobsNetworkGame.exe
1. Type in a server name
1. Select how many players
1. Select how many AI players you want (if applicable)
1. Select the number of buy points
1. The server will then start - your firewall may activate - just let it through.

## Run GraphicsEngine.exe for each human player

* If you have the server running on the same computer, change your port number(1-4) so that each running client has a different port number.
  * Keep the default IP 
  * Change the name so that it is different from every other client. This name has to be unique.
* If you have the server on a different computer, then your port number (1-4) can be any of the given values.
  * Change the IP to the IP address of the server.
  * Again, change the name so that it is unique.
* Your firewall may activate after this, just let it through.
* If during this process, your client freezes, then you have violated one of the above. Restart your server and clients.

## When the game starts

* You will be prompted to buy Bobs.
** Click 'Buy Bob' and 'Done' after you have bought your Bob.
* Each Bob costs buy points and along with abilities for that bob.
* Click 'Ready' when you are done buying all the Bobs.

Your client will now wait for the other clients to click 'Ready'. After everyone is ready, you can use the chat screen.

Once everyone is ready, you can start placing your Bobs on the screen. Each Bob will be placed when you click on the map.

When all your Bobs have been placed, a 'DonePlaceBob' button will appear. Clicking this button will signal to the server that you are ready to play.

Every player must click 'DonePlaceBob' for the game to continue on.

# Gameplay

All the units will be placed on the screen once everyone have placed their Bobs.

When it is your turn, buttons will appear and you can select actions. When it is not your turn, you have to wait - you'll see other players move when they move their units. You can also chat with other players while you're waiting.

Game play is a simple and intuitive process:
* Click on the Bob you want to perform an action. Click one of the buttons in the top panel.
* If you can perform that action, tiles will be highlighted by fishes, click on a fish and click 'Select'.
* If you don't want to perform the action, then click 'Cancel' and buttons will appear allowing you to perform another action.

* In summary, the process is:
  * Click a Bob
  * Click a tile (fish)
  * Click 'Select'
  * The action is then performed.

* You can see what was done by the animations that appear and by how much damage you have inflicted on your target.

* Your action may have missed. In this case, no damage will have been done.

* The goal is to plan your attack so that you eliminate all of the opposing player's Bobs.

* Explore the buttons and have fun.

# KEYBOARD AND MOUSE GUIDE:

* F1 KEY: Shows the GUI
* F2 KEY: Hides the GUI so you can see better
* A,S,D,W: Allows you to move the camera around
* F: Lowers the terrain resolution - to see better

* MOUSE:
  * Left click to select a tile or a target
    * You can't click the terrain if the mouse is over a GUI object.
  * Click the right mouse button and drag to rotate your vies.

# CHATBOX:

* Click textbox, type message, and press enter to send a chat message.
* Doubleclick the top bar of the chat window to minimize it.

# Building from Source

## Client

Run the TileManager.sln file in the VC7.1 folder in the client folder
* The settings for this project should work properly.
* If things don't compile, check the project settings and ensure that $OGRE_HOME is correct along with the C++ and Linker directories.
* The client compiles to the file 'GraphicsEngine.exe'.

## Server
Run BobsNetworkGame.sln in the server folder

* This should open the Visual Studio 7.1 project.
* Simply build the solution.
* The server is easier to compile than the client.
* RakNet is already included in both the server and client files.
* The server compiles to the file 'BobsNetworkGame.exe'.
