#include "Player.h"
#include "Map.h"
#include "GameState.h"

int main()
{
	string handle1 = "Player1";
	string handle2 = "Player2";

	Player *players = new Player[2];
	players[0] = Player(handle1);
	players[1] = Player(handle2);
	players[0].setAP(10);
	players[1].setAP(10);

	ActiveHPMod *HPMod1 = new ActiveHPMod[1];
	HPMod1[0] = ActiveHPMod(Ability, 0, 0, 0, 0, 0, 0, 0, 0);

	ActiveStatMod *StatMod1 = new ActiveStatMod[1];
	StatMod1[0] = ActiveStatMod(Ability, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	PassiveAbility *Pass1 = new PassiveAbility[1];
	Pass1[0] = PassiveAbility(Ability, 0, 0, 0, 0, 0, 0, 0);
	
	Bob *bobs1 = new Bob[1];
	Bob *bobs2 = new Bob[1];

	bobs1[0] = Bob(players[0].getHandle(), 1, 4, 1, 1, 1, 1, 1, 1, 1, Pass1, HPMod1, StatMod1);
	bobs2[0] = Bob(players[1].getHandle(), 1, 1, 1, 1, 1, 1, 1, 1, 1, Pass1, HPMod1, StatMod1);
	
	delete [] HPMod1;
	delete [] StatMod1;
	delete [] Pass1;
	
	players[0].setBobs(bobs1, 1);
	players[1].setBobs(bobs2, 1);
	
	delete [] bobs1;
	delete [] bobs2;
	
	Map map1 = Map();
	
	GameState game = GameState();
	game.initialize(players, 2, 20, map1);
	
	delete [] players;
	
	game.placeBob(&game.getPlayers()[0].getBobs()[0], 5, 5);
	
	queue< Tile* > Q = game.getPaths(&game.getPlayers()[0].getBobs()[0]);
	
	Tile *T;
	int i = 0;
	while(Q.size() > 0)
	{
		T = Q.front();
		Q.pop();
		
		cout << "Tile " << i++ << " X: " << T->getX() << ", Y: " << T->getY() << ", " << T->getTraversalCost() << endl;
	}

//	while(1)
//	{
//		game.turnLoop();
//	}
	
	cout << "NO ERRORS!\n";
	
	
	return 0;
}