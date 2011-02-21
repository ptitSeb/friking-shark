#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../PortFiles \
-I../AnimationSystems\
-I../GameRunTime \
-I../GameRunTimeLib \
-I../GameEntityLib \
-I../GameLib \
-I../GameGUI \
-I../GameGUILib \
-I../GameGraphics \
-I../GameManagers \
-I../VectorLib \

SOURCES = GroundUnitTypes.cpp\
StaticEntityType.cpp\
StaticStructureType.cpp\
TruckType.cpp\
stdafx.cpp



CFLAGS +=  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG +=   -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -g

LIBS = ../GameEntityLib/lib/GameEntityLib.a ../GameGUILib/lib/GameGUILib.a  ../GameLib/lib/GameLib.a ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a -lGL
LIBS_DEBUG = ../GameEntityLib/lib/GameEntityLibd.a  ../GameGUILib/lib/GameGUILibd.a ../GameLib/lib/GameLibd.a ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a -lGL

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug

release: $(OUT_SOURCES:.cpp=.o)
	rm -f ../Demo/Bin/GroundUnitTypes.so
	$(CC) -shared -o ../Demo/Bin/GroundUnitTypes.so .intermediate/*.o $(CFLAGS) $(LIBS)

debug: $(OUT_SOURCES:.cpp=.od)
	rm -f ../Demo/Bin/GroundUnitTypesd.so
	$(CC) -shared -o ../Demo/Bin/GroundUnitTypesd.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./Demo/Bin/GroundUnitTypes*.so
	rm -f ./.intermediate/*

