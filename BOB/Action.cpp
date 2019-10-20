
Action::Action()
{
}

		
Action
	datamembers
		target		(Tile obj)
		originTile	(Tile obj)
		AP cost		(int)
		damage		(double []) ~ [enemy1Damage][enemy2damage][...][enemyNdamage]
		actiontype	(ActionType obj)