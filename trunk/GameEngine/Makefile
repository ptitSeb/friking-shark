#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../GameRunTime \
-I../GameRunTimeLib \
-I../VectorLib \
-I../GameGUI \
-I../GameGraphics

SOURCES = GameEngine.cpp\
StdAfx.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -Wall -Werror  -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -Wall -Werror  -D_REENTRANT -MMD -g

LIBS = ../GameRunTimeLib/lib/GameRuntimeLib.a
LIBS_DEBUG = ../GameRunTimeLib/lib/GameRuntimeLibd.a

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: ../Demo/Bin/Release/GameEngine
debug: ../Demo/Bin/Debug/GameEngine

../Demo/Bin/Release/GameEngine: $(OUT_SOURCES:.cpp=.o) $(LIBS) 
	cp ../Demo/Bin/Release/GameRuntime.so .
	$(CC) -o ../Demo/Bin/Release/GameEngine  -lstdc++ .intermediate/*.o $(CFLAGS) $(LIBS) ./GameRuntime.so -lpthread

../Demo/Bin/Debug/GameEngine: $(OUT_SOURCES:.cpp=.od) $(LIBS_DEBUG) 
	cp ../Demo/Bin/Debug/GameRuntime.so .
	$(CC) -o ../Demo/Bin/Debug/GameEngine  -lstdc++ .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG) ./GameRuntime.so -lpthread

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/GameEngine
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
