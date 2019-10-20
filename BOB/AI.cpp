#include "AI.h"

AI::AI(GameState *game)
{
	this->game = game;
}

void AI::determineAction()
{
	player = game->getActivePlayer();
	
	while(game->getActivePlayer()->getHandle() == player->getHandle())
	{
		game->clearQueues();
		
		if(game->gameOver())
			return;
		
		determineMode();
		determineContested();
		
		if(mode == Offensive)
		{	
			if(!contested)
			{
				// List all ranged attacks in order of power
				if(getBestTargetInRange(DefaultMelee));
				else if(getBestTargetInMoveRange(LightLightningAOE));
				else if(getBestTargetInMoveRange(MediumRanged));
				else if(getBestTargetInMoveRange(DefaultMelee));
				else if(getBestTargetInMoveRange(MediumHeal));
				else
				{
					moveTowardsTarget(getNearestEnemy(getCenterTile()));
						
					if(game->getActivePlayer()->getHandle() == player->getHandle())
					{
						ActionRequestType request;
							request.bobID = -1;
							request.ability = PassTurn;
						game->getReader()->actionRequestQueue.push(request);
		
						while(true)
						{
							game->turnLoop();
							
							if(game->getActionState() == 0)
								break;
						}
					}
				}
			}
			else
			{
				if(getBestTargetInRange(DefaultMelee));
				else if(getBestTargetInMoveRange(LightLightningAOE));
				else if(getBestTargetInMoveRange(MediumRanged));
				else if(getBestTargetInMoveRange(DefaultMelee));
				else if(getBestTargetInMoveRange(MediumHeal));
				else
				{
					moveTowardsTarget(getNearestEnemy(getCenterTile()));
						
					if(game->getActivePlayer()->getHandle() == player->getHandle())
					{
						ActionRequestType request;
							request.bobID = -1;
							request.ability = PassTurn;
						game->getReader()->actionRequestQueue.push(request);
		
						while(true)
						{
							game->turnLoop();
							
							if(game->getActionState() == 0)
								break;
						}
					}
				}
				// Try to own contested clusters! =(
			}
		}	
		else
		{	
			if(!contested)
			{
				if(getBestTargetInMoveRange(MediumHeal));
				else if(getBestTargetInRange(DefaultMelee));
				else if(getBestTargetInMoveRange(LightLightningAOE));
				else if(getBestTargetInMoveRange(MediumRanged));
				else if(getBestTargetInMoveRange(DefaultMelee));
				else
				{
					moveTowardsTarget(getNearestEnemy(getCenterTile()));
						
					if(game->getActivePlayer()->getHandle() == player->getHandle())
					{
						ActionRequestType request;
							request.bobID = -1;
							request.ability = PassTurn;
						game->getReader()->actionRequestQueue.push(request);
		
						while(true)
						{
							game->turnLoop();
							
							if(game->getActionState() == 0)
								break;
						}
					}
				}
				// Get to Offensive! =)
			}
			else
			{
				if(getBestTargetInMoveRange(MediumHeal));
				else if(getBestTargetInRange(DefaultMelee));
				else if(getBestTargetInMoveRange(LightLightningAOE));
				else if(getBestTargetInMoveRange(MediumRanged));
				else if(getBestTargetInMoveRange(DefaultMelee));
				else
				{
					moveTowardsTarget(getNearestEnemy(getCenterTile()));
						
					if(game->getActivePlayer()->getHandle() == player->getHandle())
					{
						ActionRequestType request;
							request.bobID = -1;
							request.ability = PassTurn;
						game->getReader()->actionRequestQueue.push(request);
		
						while(true)
						{
							game->turnLoop();
							
							if(game->getActionState() == 0)
								break;
						}
					}
				}
				// Retreat! =(
			}		
		}
	}	
}

void AI::determineMode()
{
	Bob *bob;
	int numBobs = 0;
	double avgHP = 0;
	
	for(int i = 0; i < player->getNumBobs(); i++)
	{
		bob = &player->getBobs()[i];
		
		if(bob->getHP() == 0)
			continue;
		
		numBobs++;
		avgHP += (double)bob->getHP()/(double)bob->getBaseHP();
	}
	
	avgHP = avgHP/numBobs;
	
	if(avgHP > .50)
		mode = Offensive;
	else
		mode = Defensive;	
}

void AI::determineContested()
{
	Bob *bob;
	
	for(int i = 0; i < player->getNumBobs(); i++)
	{
		bob = &player->getBobs()[i];
		
		if(bob->getHP() == 0)
			continue;
		
		for(int j = -5; j < 6; j++)
		for(int k = -5; k < 6; k++)
		{
			if(bob->getX() + j < 0 || bob->getX() + j >= game->getMap()->getLength())
				continue;
			
			if(bob->getY() + k < 0 || bob->getY() + k >= game->getMap()->getLength())
				continue;
			
			if(!game->getMap()->getTile(bob->getX()+j, bob->getY()+k)->getBob())
				continue;
			
			if(game->getMap()->getTile(bob->getX()+j, bob->getY()+k)->getBob()->getPlayerHandle() != player->getHandle())
			{
				contested = true;
				return;
			}
		}
	}
	contested = false;
}

bool AI::getBestTargetInRange(AbilityNameType abilityName)
{
	ActionRequestType request;
	ActionToPerformType action;
	
	Bob *bob = NULL,
		*bestBob = NULL,
		*targetBob = NULL;
	
	Ability *ability = NULL;
	
	Tile 	*target = NULL,
			*bestTarget = NULL;
			
	queue< Tile* > targetOptionQueue;
	
	double	targetDistance,
			shortestTargetDistance = 9999;
	
	int targetHP,
		lowestTargetHP = 9999,
		mostEnemies = 1;
	
	for(int i = 0; i < player->getNumBobs(); i++)
	{
		bob = &player->getBobs()[i];
		ability = NULL;
		
		if(bob->getHP() == 0)
			continue;
		
		for(int j = 0; j < bob->getActiveHPSize(); j++)
		{
			if(bob->getActiveHPMods()[j].ability->getName() != abilityName)
				continue;
			
			if(bob->getAP() < bob->getActiveHPMods()[j].ability->getUseCost())
				break;
			
			if(bob->getActiveHPMods()[j].turnsLeft)
				break;
			
			ability = bob->getActiveHPMods()[j].ability;
			break;
		}
		
		if(!ability)
			continue;
		
		request.bobID = bob->getID();
		request.ability = abilityName;
		game->getReader()->actionRequestQueue.push(request);
		
		while(true)
		{
			game->turnLoop();
			
			if(game->getActionState() == 0 || game->getActionState() == 2)
				break;
		}
		
		while(!game->getReader()->targetOptionQueue.empty())
		{
			targetOptionQueue.push(game->getMap()->getTile(game->getReader()->targetOptionQueue.front().x, game->getReader()->targetOptionQueue.front().y));
			game->getReader()->targetOptionQueue.pop();
		}
		
		if(game->getActionState() == 2)
		{
			action.bobID = bob->getID();
			action.ability = Cancel;
			action.x = -1;
			action.y = -1;
			game->getReader()->actionToPerformQueue.push(action);
			
			while(true)
			{
				game->turnLoop();
				
				if(game->getActionState() == 0)
					break;
			}
		}
		
		while(!targetOptionQueue.empty())
		{
			target = targetOptionQueue.front();
			targetOptionQueue.pop();
			
			targetDistance = sqrt(pow(bob->getX() - target->getX(),2) + pow(bob->getY() - target->getY(),2));
			
			if(targetDistance > 1 && targetDistance < 2)
				targetDistance = 1;
			
			switch(abilityName)
			{
				// Single target attacks
				case DefaultMelee: case MediumRanged:
					targetHP = target->getBob()->getHP();
					
					if(targetHP < lowestTargetHP)
					{
						lowestTargetHP = targetHP;
						shortestTargetDistance = targetDistance;
						bestTarget = target;
						bestBob = bob;
					}
					else if(targetHP == lowestTargetHP)
					{
						if(targetDistance < shortestTargetDistance)
						{
							shortestTargetDistance = targetDistance;
							bestTarget = target;
							bestBob = bob;
						}
					}
				break;
				
				// Single target heals
				case MediumHeal:
					targetHP = target->getBob()->getHP();
					
					if(mode == Offensive  && (double)targetHP/(double)target->getBob()->getBaseHP() > .5)
						break;
						
					if(targetHP < lowestTargetHP)
					{
						lowestTargetHP = targetHP;
						shortestTargetDistance = targetDistance;
						bestTarget = target;
						bestBob = bob;
					}
					else if(targetHP == lowestTargetHP)
					{
						if(targetDistance < shortestTargetDistance)
						{
							shortestTargetDistance = targetDistance;
							bestTarget = target;
							bestBob = bob;
						}
					}
				break;
				
				// AoE attacks
				case LightLightningAOE:
					int targetX = target->getX(),
						targetY = target->getY(),
						radius = LightLightningAOEAbility.getRadius(),
						myBobs = 0,
						enemyBobs = 0,
						myBobsHP = 0,
						enemyBobsHP = 0;					
					
					for(int n = -radius; n <= radius; n++)
					for(int m = -radius; m <= radius; m++)
					{
						if(radius != 1 && sqrt(pow(targetX-n,2) + pow(targetY-m,2)) > radius)
							continue;
						
						if(targetX+n < 0 || targetX+n >= game->getMap()->getLength() || targetY+m < 0 || targetY+m >= game->getMap()->getHeight())
							continue;
						
						targetBob = game->getMap()->getTile(targetX+n, targetY+m)->getBob();
						
						if(!targetBob)
							continue;
						
						if(targetBob->getPlayerHandle() == player->getHandle())
						{
							myBobs++;
							myBobsHP += targetBob->getHP();
						}
						else
						{
							enemyBobs++;
							enemyBobsHP += targetBob->getHP();
						}
					}
				
					if(myBobs > 0)
						break;
					
					if(enemyBobs - myBobs > mostEnemies)
					{
						bestBob = bob;
						bestTarget = target;
						mostEnemies = enemyBobs - myBobs;
						lowestTargetHP = enemyBobsHP;
						shortestTargetDistance = targetDistance;
					}
					else if(enemyBobs - myBobs == mostEnemies)
					{
						if(enemyBobsHP < lowestTargetHP)
						{
							bestBob = bob;
							bestTarget = target;
							lowestTargetHP = enemyBobsHP;
							shortestTargetDistance = targetDistance;
						}
					}
					else if(enemyBobs - myBobs == mostEnemies && enemyBobsHP == lowestTargetHP)
					{
						if(targetDistance < shortestTargetDistance)
						{
							bestBob = bob;
							bestTarget= target;
							shortestTargetDistance = targetDistance;
						}
					}
				break;
			}
		}	
	}
	
	if(bestBob && bestTarget)
	{
		request.bobID = bestBob->getID();
		request.ability = abilityName;
		game->getReader()->actionRequestQueue.push(request);
		
		while(true)
		{
			game->turnLoop();
			
			if(game->getActionState() == 2)
				break;
		}
		
		action.bobID = bestBob->getID();
		action.ability = abilityName;
		action.x = bestTarget->getX();
		action.y = bestTarget->getY();
		game->getReader()->actionToPerformQueue.push(action);
		
		while(true)
		{
			game->turnLoop();
			
			if(game->getActionState() == 0)
				break;
		}
		
		game->draw();
		return true;
	}
	return false;
}

bool AI::getBestTargetInMoveRange(AbilityNameType abilityName)
{
	time_t start, finish;
	
	ActionRequestType request;
	ActionToPerformType action;
	
	Bob *bob = NULL,
		*bestBob = NULL,
		*targetBob = NULL;
	
	Ability *ability = NULL;
	
	Tile 	*target = NULL,
			*bestTarget = NULL,
			*bestMoveTarget = NULL;
			
	TargetOptionType moveOption;
			
	queue< Tile* > targetOptionQueue;
	queue< TargetOptionType > moveOptionQueue;
	
	double	targetDistance,
			moveDistance,
			shortestTargetDistance = 9999,
			shortestMoveDistance = 9999;
	
	int bobsX,
		bobsY,
		targetHP,
		lowestTargetHP = 9999,
		mostEnemies = 1,
		friendlyBobs,
		leastFriendlyBobs = 9999;
	
	for(int i = 0; i < player->getNumBobs(); i++)
	{
		bob = &player->getBobs()[i];
		bobsX = bob->getX();
		bobsY = bob->getY();
		ability = NULL;
		bestTarget = NULL;
		bestMoveTarget = NULL;
		shortestTargetDistance = shortestMoveDistance = lowestTargetHP = 9999;
		mostEnemies = 1;
		leastFriendlyBobs = 9999;
		
		if(bob->getHP() == 0)
			continue;
		
		for(int j = 0; j < bob->getActiveHPSize(); j++)
		{
			if(bob->getActiveHPMods()[j].ability->getName() != abilityName)
				continue;
			
			if(bob->getAP() < bob->getActiveHPMods()[j].ability->getUseCost())
				break;
			
			if(bob->getActiveHPMods()[j].turnsLeft)
				break;
			
			ability = bob->getActiveHPMods()[j].ability;
			break;
		}
		
		if(!ability)
			continue;
		
		request.bobID = bob->getID();
		request.ability = Move;
		game->getReader()->actionRequestQueue.push(request);
		
		while(true)
		{
			game->turnLoop();
			
			if(game->getActionState() == 0 || game->getActionState() == 2)
				break;
		}
		
		while(!game->getReader()->targetOptionQueue.empty())
		{
			moveOptionQueue.push(game->getReader()->targetOptionQueue.front());
			game->getReader()->targetOptionQueue.pop();
		}
		
		if(game->getActionState() == 2)
		{
			action.bobID = bob->getID();
			action.ability = Cancel;
			action.x = -1;
			action.y = -1;
			game->getReader()->actionToPerformQueue.push(action);
			
			while(true)
			{
				game->turnLoop();
				
				if(game->getActionState() == 0)
					break;
			}
		}
		
		time( &start );
		
		while(!moveOptionQueue.empty())
		{
			moveOption = moveOptionQueue.front();
			moveOptionQueue.pop();
			
			if(abilityName == DefaultMelee && bob->getAP() - moveOption.cost < DefaultMeleeAbility.getUseCost())
				continue;
			else if(abilityName == MediumRanged && bob->getAP() - moveOption.cost < MediumRangedAbility.getUseCost())
				continue;
			else if(abilityName == LightLightningAOE && bob->getAP() - moveOption.cost < LightLightningAOEAbility.getUseCost())
				continue;
			else if(abilityName == MediumHeal && bob->getAP() - moveOption.cost < MediumHealAbility.getUseCost())
				continue;
			
			moveDistance = sqrt(pow(bob->getX() - moveOption.x,2) + pow(bob->getY() - moveOption.y,2));
			
			game->placeBob(bob, moveOption.x, moveOption.y);
			
			friendlyBobs = 0;
			
			for(int i = -2; i < 3; i++)
			for(int j = -2; j < 3; j++)
			{
				if(moveOption.x+i < 0 || moveOption.x+i >= game->getMap()->getLength() || moveOption.y+j < 0 || moveOption.y+j >= game->getMap()->getLength())
					continue;
				
				targetBob = game->getMap()->getTile(moveOption.x+i, moveOption.y+j)->getBob();
				
				if(targetBob)
				{
					if(targetBob->getPlayerHandle() == player->getHandle())
						friendlyBobs++;
						
					targetBob = NULL;
				}	
			}			
			
			request.bobID = bob->getID();
			request.ability = abilityName;
			game->getReader()->actionRequestQueue.push(request);
			
			while(true)
			{
				game->turnLoop();
				
				if(game->getActionState() == 0 || game->getActionState() == 2)
					break;
			}
			
			while(!game->getReader()->targetOptionQueue.empty())
			{
				targetOptionQueue.push(game->getMap()->getTile(game->getReader()->targetOptionQueue.front().x, game->getReader()->targetOptionQueue.front().y));
				game->getReader()->targetOptionQueue.pop();
			}
			
			if(game->getActionState() == 2)
			{
				action.bobID = bob->getID();
				action.ability = Cancel;
				action.x = -1;
				action.y = -1;
				game->getReader()->actionToPerformQueue.push(action);
				
				while(true)
				{
					game->turnLoop();
					
					if(game->getActionState() == 0)
						break;
				}
			}
			
			while(!targetOptionQueue.empty())
			{
				target = targetOptionQueue.front();
				targetOptionQueue.pop();
				
				targetDistance = sqrt(pow(bob->getX() - target->getX(),2) + pow(bob->getY() - target->getY(),2));
				
				if(targetDistance > 1 && targetDistance < 2)
					targetDistance = 1;
				
				switch(abilityName)
				{
					// Single target attacks
					case DefaultMelee: case MediumRanged:
						targetHP = target->getBob()->getHP();
						
						if(targetHP < lowestTargetHP)
						{
							lowestTargetHP = targetHP;
							leastFriendlyBobs = friendlyBobs;
							shortestMoveDistance = moveDistance;
							shortestTargetDistance = targetDistance;
							bestTarget = target;
							bestBob = bob;
							bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
						}
						else if(targetHP == lowestTargetHP)
						{
							if(friendlyBobs < leastFriendlyBobs)
							{
								leastFriendlyBobs = friendlyBobs;
								shortestMoveDistance = moveDistance;
								shortestTargetDistance = targetDistance;
								bestTarget = target;
								bestBob = bob;
								bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
							}
						}
						else if(targetHP == lowestTargetHP && friendlyBobs == leastFriendlyBobs)
						{
							if(moveDistance < shortestMoveDistance)
							{
								shortestMoveDistance = moveDistance;
								shortestTargetDistance = targetDistance;
								bestTarget = target;
								bestBob = bob;
								bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
							}
						}
						else if(targetHP == lowestTargetHP && friendlyBobs == leastFriendlyBobs && moveDistance == shortestMoveDistance)
						{
							if(targetDistance < shortestTargetDistance)
							{
								shortestTargetDistance = targetDistance;
								bestTarget = target;
								bestBob = bob;
								bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
							}	
						}
					break;
					
					// Single target heals
					case MediumHeal:
						targetHP = target->getBob()->getHP();
						
						if(mode == Offensive  && (double)targetHP/(double)target->getBob()->getBaseHP() > .5)
							break;
							
						if(targetHP < lowestTargetHP)
						{
							lowestTargetHP = targetHP;
							leastFriendlyBobs = friendlyBobs;
							shortestMoveDistance = moveDistance;
							shortestTargetDistance = targetDistance;
							bestTarget = target;
							bestBob = bob;
							bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
						}
						else if(targetHP == lowestTargetHP)
						{
							if(friendlyBobs < leastFriendlyBobs)
							{
								leastFriendlyBobs = friendlyBobs;
								shortestMoveDistance = moveDistance;
								shortestTargetDistance = targetDistance;
								bestTarget = target;
								bestBob = bob;
								bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
							}
						}
						else if(targetHP == lowestTargetHP && friendlyBobs == leastFriendlyBobs)
						{
							if(moveDistance < shortestMoveDistance)
							{
								shortestMoveDistance = moveDistance;
								shortestTargetDistance = targetDistance;
								bestTarget = target;
								bestBob = bob;
								bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
							}
						}
						else if(targetHP == lowestTargetHP && friendlyBobs == leastFriendlyBobs && moveDistance == shortestMoveDistance)
						{
							if(targetDistance < shortestTargetDistance)
							{
								shortestTargetDistance = targetDistance;
								bestTarget = target;
								bestBob = bob;
								bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
							}	
						}
					break;
					
					// AoE attacks
					case LightLightningAOE:
						int targetX = target->getX(),
							targetY = target->getY(),
							radius = LightLightningAOEAbility.getRadius(),
							myBobs = 0,
							enemyBobs = 0,
							myBobsHP = 0,
							enemyBobsHP = 0;					
						
						for(int n = -radius; n <= radius; n++)
						for(int m = -radius; m <= radius; m++)
						{
							if(radius != 1 && sqrt(pow(targetX-n,2) + pow(targetY-m,2)) > radius)
								continue;
							
							if(targetX+n < 0 || targetX+n >= game->getMap()->getLength() || targetY+m < 0 || targetY+m >= game->getMap()->getHeight())
								continue;
							
							targetBob = game->getMap()->getTile(targetX+n, targetY+m)->getBob();
							
							if(!targetBob)
								continue;
							
							if(targetBob->getPlayerHandle() == player->getHandle())
							{
								myBobs++;
								myBobsHP += targetBob->getHP();
							}
							else
							{
								enemyBobs++;
								enemyBobsHP += targetBob->getHP();
							}
						}
					
						if(myBobs > 0)
							break;
						
						if(enemyBobs - myBobs > mostEnemies)
						{
							mostEnemies = enemyBobs - myBobs;
							lowestTargetHP = enemyBobsHP;
							leastFriendlyBobs = friendlyBobs;
							shortestMoveDistance = moveDistance;
							shortestTargetDistance = targetDistance;
							bestBob = bob;
							bestTarget = target;
							bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
						}
						else if(enemyBobs - myBobs == mostEnemies)
						{
							if(enemyBobsHP < lowestTargetHP)
							{
								lowestTargetHP = enemyBobsHP;
								leastFriendlyBobs = friendlyBobs;
								shortestMoveDistance = moveDistance;
								shortestTargetDistance = targetDistance;
								bestBob = bob;
								bestTarget = target;
								bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
							}
						}
						else if(enemyBobs - myBobs == mostEnemies && enemyBobsHP == lowestTargetHP)
						{
							if(friendlyBobs < leastFriendlyBobs)
							{
								bestBob = bob;
								bestTarget= target;
								leastFriendlyBobs = friendlyBobs;
								shortestMoveDistance = moveDistance;
								shortestTargetDistance = targetDistance;
								bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
							}
						}
						else if(enemyBobs - myBobs == mostEnemies && enemyBobsHP == lowestTargetHP && friendlyBobs == leastFriendlyBobs)
						{
							if(moveDistance < shortestMoveDistance)
							{
								shortestMoveDistance = moveDistance;
								shortestTargetDistance = targetDistance;
								bestBob = bob;
								bestTarget= target;
								bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
							}
						}
						else if(enemyBobs - myBobs == mostEnemies && enemyBobsHP == lowestTargetHP && friendlyBobs == leastFriendlyBobs && moveDistance == shortestMoveDistance)
						{
							if(targetDistance < shortestTargetDistance)
							{
								shortestTargetDistance = targetDistance;
								bestBob = bob;
								bestTarget= target;
								bestMoveTarget = game->getMap()->getTile(moveOption.x, moveOption.y);
							}	
						}
					break;
				}
			}
			
			time(&finish);
			if(difftime( finish, start) > 1) break;
		}
		
		game->placeBob(bob, bobsX, bobsY);
		
		if(bestBob && bestTarget && bestMoveTarget)
		{
			request.bobID = bestBob->getID();
			request.ability = Move;
			game->getReader()->actionRequestQueue.push(request);
			
			while(true)
			{
				game->turnLoop();
				
				if(game->getActionState() == 2)
					break;
			}
			
			action.bobID = bestBob->getID();
			action.ability = Move;
			action.x = bestMoveTarget->getX();
			action.y = bestMoveTarget->getY();
			game->getReader()->actionToPerformQueue.push(action);
			
			while(true)
			{
				game->turnLoop();
				if(game->getActionState() == 0)
					break;
			}
			
			/*request.bobID = bestBob->getID();
			request.ability = abilityName;
			game->getReader()->actionRequestQueue.push(request);
			
			while(true)
			{
				game->turnLoop();
				
				if(game->getActionState() == 2)
					break;
			}
			
			action.bobID = bestBob->getID();
			action.ability = abilityName;
			action.x = bestTarget->getX();
			action.y = bestTarget->getY();
			game->getReader()->actionToPerformQueue.push(action);
			
			while(true)
			{
				game->turnLoop();
				
				if(game->getActionState() == 0)
					break;
			}*/
			
			game->draw();
			return true;
		}
	}
	return false;
}

Tile* AI::getCenterTile()
{
	float avgX, avgY;
	int x, y, liveBobs;
	x = y = liveBobs = 0;
	Bob *bob;
	
	for(int a = 0; a < player->getNumBobs(); a++)
	{
		bob = &player->getBobs()[a];
		
		if(bob->getHP() == 0)
			continue;
			
		liveBobs++;
		x += bob->getX();
		y += bob->getY();
					
	}
	avgX = (float)x/(float)liveBobs;
	avgY = (float)y/(float)liveBobs;
	
	x = (int)avgX;
	y = (int)avgY;
	
	return game->getMap()->getTile(x,y);	
}

Tile* AI::getNearestEnemy(Tile* tile)
{
	Tile * target;
	
	for(int i = 1; ; i++)
	{
		for(int j = -i; j <= i; j++)
		for(int k = -i; k <= i; k++)
		{
			if(sqrt((double)(j*j + k*k)) > i)
				continue;
			
			if(tile->getX() + j < 0 || tile->getX() + j >= game->getMap()->getLength())
				continue;
			
			if(tile->getY() + k < 0 || tile->getY() + k >= game->getMap()->getLength())
				continue;
			
			if(!game->getMap()->getTile(tile->getX()+j, tile->getY()+k)->getBob())
				continue;
			
			if(game->getMap()->getTile(tile->getX()+j, tile->getY()+k)->getBob()->getPlayerHandle() != player->getHandle())
			{
				return game->getMap()->getTile(tile->getX()+j, tile->getY()+k);
			}
		}
	}
}

void AI::moveTowardsTarget(Tile *tile)
{
	ActionRequestType request;
	ActionToPerformType action;
	
	Bob *bob, *targetBob = NULL, *slowestBob = NULL;
	Tile *target, *bestTarget;
	
	stack< Bob * > bobStack;
	queue< Tile * > targetOptionQueue;
	
	double targetDistance, closestTargetDistance, safeDistance = NULL, formationDistance = NULL;
	
	int friendlyBobs, leastFriendlyBobs = 9999;
	
	for(int i = 0; i < player->getNumBobs(); i++)
	{
		bob = &player->getBobs()[i];
		
		if(bob->getHP() == 0)
			continue;
		
		if(!slowestBob)
			slowestBob = bob;
		else if(bob->getAP() < slowestBob->getAP())
		{
			bobStack.push(slowestBob);
			slowestBob = bob;
		}
		else
			bobStack.push(bob);
	}
	bobStack.push(slowestBob);
	
	while(!bobStack.empty())
	{
		bestTarget = NULL;
		closestTargetDistance = 9999;
		
		bob = bobStack.top();
		bobStack.pop();
		
		if(!safeDistance)
		{
			targetDistance = sqrt(pow(bob->getX() - tile->getX(),2) + pow(bob->getY() - tile->getY(),2));
			safeDistance = targetDistance/2;
		}
		
		request.bobID = bob->getID();
		request.ability = Move;
		game->getReader()->actionRequestQueue.push(request);
		
		while(true)
		{
			game->turnLoop();
			
			if(game->getActionState() == 0 || game->getActionState() == 2)
				break;	
		}
		
		while(!game->getReader()->targetOptionQueue.empty())
		{
			targetOptionQueue.push(game->getMap()->getTile(game->getReader()->targetOptionQueue.front().x, game->getReader()->targetOptionQueue.front().y));
			game->getReader()->targetOptionQueue.pop();
		}
		
		if(game->getActionState() == 2)
		{
			action.bobID = bob->getID();
			action.ability = Cancel;
			action.x = -1;
			action.y = -1;
			game->getReader()->actionToPerformQueue.push(action);
			
			while(true)
			{
				game->turnLoop();
				
				if(game->getActionState() == 0)
					break;
			}
		}
		
		while(!targetOptionQueue.empty())
		{
			target = targetOptionQueue.front();
			targetOptionQueue.pop();
			
			targetDistance = sqrt(pow(target->getX() - tile->getX(),2) + pow(target->getY() - tile->getY(),2));
			
			friendlyBobs = 0;
			
			for(int i = -2; i < 3; i++)
			for(int j = -2; j < 3; j++)
			{
				if(target->getX()+i < 0 || target->getX()+i >= game->getMap()->getLength() || target->getY()+j < 0 || target->getY()+j >= game->getMap()->getLength())
					continue;
				
				targetBob = game->getMap()->getTile(target->getX()+i, target->getY()+j)->getBob();
				
				if(targetBob)
				{
					if(targetBob->getPlayerHandle() == player->getHandle())
						friendlyBobs++;
					
					targetBob = NULL;
				}	
			}
			
			if(targetDistance < safeDistance || targetDistance < formationDistance)
				continue;
			
			if(friendlyBobs < leastFriendlyBobs)
			{
				closestTargetDistance = targetDistance;
				bestTarget = target;
			}			
			else if(friendlyBobs == leastFriendlyBobs)
			{
				if(targetDistance < closestTargetDistance)
				{
					closestTargetDistance = targetDistance;
					bestTarget = target;
				}
			}
		}
		
		if(bestTarget)
		{
			request.bobID = bob->getID();
			request.ability = Move;
			game->getReader()->actionRequestQueue.push(request);
			
			while(true)
			{
				game->turnLoop();
				
				if(game->getActionState() == 2)
					break;
			}
			
			action.bobID = bob->getID();
			action.ability = Move;
			action.x = bestTarget->getX();
			action.y = bestTarget->getY();
			game->getReader()->actionToPerformQueue.push(action);
			
			while(true)
			{
				game->turnLoop();
				
				if(game->getActionState() == 0)
					break;
			}
		}
		
		if(!formationDistance)
		{
			formationDistance = closestTargetDistance - 3;
			
			if(formationDistance < 0)
				formationDistance = 0;
		}
	}
}

AI::~AI()
{
}
