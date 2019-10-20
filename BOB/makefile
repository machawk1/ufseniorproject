CC = cl.exe /EHsc /MT /c
LN = link.exe
EXENAME = Game

all: $(EXENAME).exe
	@echo Build Successful!
$(EXENAME).exe: $(EXENAME).obj
	$(LN) -out:$(EXENAME).exe *.obj
	rm *.obj
$(EXENAME).obj: GameState.obj AI.obj
	$(CC) -Fo$(EXENAME).obj $(EXENAME).cpp
GameState.obj: Map.obj Player.obj Reader.obj
	$(CC) -FoGamesState.obj GameState.cpp
Ability.obj: Ability.cpp
	$(CC) -FoAbility.obj Ability.cpp
ActiveAbility.obj: Ability.obj
	$(CC) -FoActiveAbility.obj ActiveAbility.cpp
ActiveHPMod.obj: ActiveAbility.obj
	$(CC) -FoActiveHPMod.obj ActiveHPMod.cpp
ActiveStatMod.obj: ActiveAbility.obj
	$(CC) -FoActiveStatMod.obj ActiveStatMod.cpp
PassiveAbility.obj: Ability.obj
	$(CC) -FoPassiveAbility.obj PassiveAbility.cpp
Bob.obj: ActiveHPMod.obj ActiveStatMod.obj PassiveAbility.obj
	$(CC) -FoBob.obj Bob.cpp
Player.obj: Bob.obj
	$(CC) -FoPlayer.obj Player.cpp
Tile.obj: Bob.obj
	$(CC) -FoTile.obj Tile.cpp
Map.obj: Tile.obj
	$(CC) -FoMap.obj Map.cpp
Reader.obj: Reader.cpp
	$(CC) -FoReader.obj Reader.cpp
AI.obj: AI.cpp
	$(CC) -FoAI.obj AI.cpp
clean:
	rm *.exe *.obj
	