#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../PortFiles -I../GameRunTime  -I../VectorLib -I../GameRunTimeLib -I../GameGUI -I../GameGraphics

SOURCES = GameEngine.cpp\
StdAfx.cpp


CFLAGS +=  -pedantic -pedantic-errors -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG +=   -pedantic -pedantic-errors -Wall -Werror  -D_REENTRANT -g

LIBS = ../GameRunTimeLib/bin/GameRuntimeLib.a  ../GameRunTime/bin/GameRuntime.so -lpthread
LIBS_DEBUG = ../GameRunTimeLib/bin/GameRuntimeLibd.a  ../GameRunTime/bin/GameRuntimed.so -lpthread

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug

release: $(OUT_SOURCES:.cpp=.o)
	rm -f bin/GameEngine
	$(CC) -o bin/GameEngine $< $(CFLAGS) $(LIBS)

debug: $(OUT_SOURCES:.cpp=.od)
	rm -f bin/GameEngined
	$(CC) -o bin/GameEngined $< $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./bin/*
	rm -f ./.intermediate/*

