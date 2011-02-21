#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../PortFiles -I../GameRunTime -I../GameRunTimeLib -I../VectorLib -I../JPEGLibrary/inc

SOURCES = GameLib.cpp\
OpenGLFonts.cpp\
ResourceStore.cpp\
StdAfx.cpp


CFLAGS += -D_DO_NOT_USE_ASSEMBLER_ -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG += -D_DO_NOT_USE_ASSEMBLER_  -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -g

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: $(OUT_SOURCES:.cpp=.o)
	rm -f ./bin/GameLib.a
	ar q bin/GameLib.a .intermediate/*.o ../JPEGLibrary/.intermediate/*.o
debug: $(OUT_SOURCES:.cpp=.od)
	rm -f ./bin/GameLibd.a
	ar q bin/GameLibd.a .intermediate/*.od ../JPEGLibrary/.intermediate/*.od

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./bin/*
	rm -f ./.intermediate/*

