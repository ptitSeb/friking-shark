#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../GameRunTime \
-I../GameRunTimeLib \
-I../GameEntityLib \
-I../GameLib \
-I../GameGUI \
-I../GameGUILib \
-I../GameGraphics \
-I../GameManagers \
-I../VectorLib \

SOURCES = IATestSystems.cpp\
IAEntityBase.cpp\
IATestMainWindow.cpp\
IATestUtils.cpp\
SteeringBehaviours.cpp\
TestScenarios.cpp\
stdafx.cpp

CFLAGS += -DNDEBUG -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -g

LIBS = ../GameGUILib/lib/GameGUILib.a  ../GameLib/lib/GameLib.a ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a  -lGLU -lGL
LIBS_DEBUG =  ../GameGUILib/lib/GameGUILibd.a ../GameLib/lib/GameLibd.a ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a  -lGLU -lGL

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug

release: $(OUT_SOURCES:.cpp=.o)
	rm -f ../Demo/Bin/Release/IATestSystems.so
	$(CC) -shared -o ../Demo/Bin/Release/IATestSystems.so .intermediate/*.o $(CFLAGS) $(LIBS)

debug: $(OUT_SOURCES:.cpp=.od)
	rm -f ../Demo/Bin/Debug/IATestSystems.so
	$(CC) -shared -o ../Demo/Bin/Debug/IATestSystems.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/IATestSystems.so
	rm -f ./.intermediate/*

