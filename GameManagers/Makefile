#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
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



CFLAGS += -DNDEBUG -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -g

LIBS = ../GameEntityLib/lib/GameEntityLib.a  ../GameLib/lib/GameLib.a ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a -ldl -lGLU -lGL
LIBS_DEBUG =  ../GameEntityLib/lib/GameEntityLibd.a ../GameLib/lib/GameLibd.a ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a -ldl -lGLU -lGL

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug

release: $(OUT_SOURCES:.cpp=.o)
	rm -f ../Demo/Bin/Release/GameManagers.so
	$(CC) -shared -o ../Demo/Bin/Release/GameManagers.so .intermediate/*.o $(CFLAGS) $(LIBS)

debug: $(OUT_SOURCES:.cpp=.od)
	rm -f ../Demo/Bin/Debug/GameManagers.so
	$(CC) -shared -o ../Demo/Bin/Debug/GameManagers.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/GameManagers.so
	rm -f ./.intermediate/*

