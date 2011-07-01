
LIBS = GameRunTimeLib\
VectorLib\
GameEntityLib\
GameGUILib

MODULES = GameRunTime\
AirUnitTypes\
AnimationSystems\
BonusTypes\
GameEngine\
GameGraphics\
GameGUI\
GameManagers\
GroundUnitTypes\
GUISystems\
IATestSystems\
ParticleSystems\
PlayAreaElements\
SoundSystems\
WeaponTypes\
ScenarioEditor\
FormationEditor\
EntityEditor\
BSPDebugger

all:
	for dir in $(LIBS) $(MODULES); do \
	(cd $$dir; ${MAKE} all); \
	done

debug:
	for dir in $(LIBS) $(MODULES); do \
	(cd $$dir; ${MAKE} debug); \
	done

release:
	for dir in $(LIBS) $(MODULES); do \
	(cd $$dir; ${MAKE} release); \
	done

clean:
	for dir in $(LIBS) $(MODULES); do \
	(cd $$dir; ${MAKE} clean); \
	done

