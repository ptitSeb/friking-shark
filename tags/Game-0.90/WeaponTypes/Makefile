#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../AnimationSystems\
-I../GameRunTime \
-I../GameRunTimeLib \
-I../GameEntityLib \
-I../GameGUI \
-I../GameGUILib \
-I../GameGraphics \
-I../GameManagers \
-I../VectorLib \

SOURCES = WeaponTypes.cpp\
BombProjectileType.cpp\
BulletProjectileType.cpp\
HomingMissileProjectileType.cpp\
ProjectileLauncherType.cpp\
stdafx.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -MMD -g

LIBS = ../GameEntityLib/lib/GameEntityLib.a  ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a
LIBS_DEBUG = ../GameEntityLib/lib/GameEntityLibd.a ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: ../Demo/Bin/Release/WeaponTypes.so
debug: ../Demo/Bin/Debug/WeaponTypes.so

../Demo/Bin/Release/WeaponTypes.so: $(OUT_SOURCES:.cpp=.o) $(LIBS)
	$(CC) -shared -o ../Demo/Bin/Release/WeaponTypes.so .intermediate/*.o $(CFLAGS) $(LIBS)

../Demo/Bin/Debug/WeaponTypes.so: $(OUT_SOURCES:.cpp=.od) $(LIBS_DEBUG)
	$(CC) -shared -o ../Demo/Bin/Debug/WeaponTypes.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/WeaponTypes.so
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
