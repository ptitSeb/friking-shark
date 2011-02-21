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
-I../GameGraphics \
-I../VectorLib \

SOURCES = EntityManager.cpp\
GameController.cpp\
FrameManager.cpp\
PhysicsManager.cpp\
GameManagers.cpp\
PlayAreaManager.cpp\
WorldManager.cpp\
WorldEntity.cpp\
WorldSector.cpp\
stdafx.cpp



CFLAGS +=  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG +=   -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -g

LIBS = ../GameEntityLib/bin/GameEntityLib.a  ../GameLib/bin/GameLib.a ../GameRunTimeLib/bin/GameRuntimeLib.a   ../VectorLib/bin/VectorLib.a -ldl -lGLU -lGL
LIBS_DEBUG =  ../GameEntityLib/bin/GameEntityLibd.a ../GameLib/bin/GameLibd.a ../GameRunTimeLib/bin/GameRuntimeLibd.a  ../VectorLib/bin/VectorLibd.a -ldl -lGLU -lGL

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug

release: $(OUT_SOURCES:.cpp=.o)
	rm -f bin/GameManagers.so
	$(CC) -shared -o bin/GameManagers.so .intermediate/*.o $(CFLAGS) $(LIBS)

debug: $(OUT_SOURCES:.cpp=.od)
	rm -f bin/GameManagersd.so
	$(CC) -shared -o bin/GameManagersd.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./bin/*
	rm -f ./.intermediate/*

