#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../GameRunTime \
-I../GameRunTimeLib \
-I../GameEntityLib \
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

CFLAGS += -DNDEBUG -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -MMD -g

LIBS = ../GameGUILib/lib/GameGUILib.a  ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a
LIBS_DEBUG =  ../GameGUILib/lib/GameGUILibd.a ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: ../Demo/Bin/Release/IATestSystems.so
debug: ../Demo/Bin/Debug/IATestSystems.so

../Demo/Bin/Release/IATestSystems.so: $(OUT_SOURCES:.cpp=.o) $(LIBS)
	$(CC) -shared -o ../Demo/Bin/Release/IATestSystems.so .intermediate/*.o $(CFLAGS) $(LIBS) -lGLU -lGL

../Demo/Bin/Debug/IATestSystems.so: $(OUT_SOURCES:.cpp=.od) $(LIBS_DEBUG) 
	$(CC) -shared -o ../Demo/Bin/Debug/IATestSystems.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG) -lGLU -lGL

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/IATestSystems.so
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
