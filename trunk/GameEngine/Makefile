#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../GameRunTime  -I../VectorLib -I../GameRunTimeLib -I../GameGUI -I../GameGraphics

SOURCES = GameEngine.cpp\
StdAfx.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -Wall -Werror  -D_REENTRANT -g

LIBS = ../GameRunTimeLib/lib/GameRuntimeLib.a  ./GameRuntime.so -lpthread
LIBS_DEBUG = ../GameRunTimeLib/lib/GameRuntimeLibd.a  ./GameRuntime.so -lpthread

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug

release: $(OUT_SOURCES:.cpp=.o)
	cp ../Demo/Bin/Release/GameRuntime.so .
	rm -f ../Demo/Bin/Release/GameEngine
	$(CC) -o ../Demo/Bin/Release/GameEngine $< $(CFLAGS) $(LIBS)

debug: $(OUT_SOURCES:.cpp=.od)
	cp ../Demo/Bin/Debug/GameRuntime.so .
	rm -f ../Demo/Bin/Debug/GameEngine
	$(CC) -o ../Demo/Bin/Debug/GameEngine $< $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/GameEngine
	rm -f ./.intermediate/*

