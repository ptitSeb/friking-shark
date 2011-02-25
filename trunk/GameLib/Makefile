#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../GameRunTime -I../GameRunTimeLib -I../VectorLib

SOURCES = ResourceStore.cpp\
StdAfx.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG += -DDEBUG -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -g

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: $(OUT_SOURCES:.cpp=.o)
	rm -f ./lib/GameLib.a
	ar q ./lib/GameLib.a .intermediate/*.o
debug: $(OUT_SOURCES:.cpp=.od)
	rm -f ./lib/GameLibd.a
	ar q ./lib/GameLibd.a .intermediate/*.od

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./lib/*
	rm -f ./.intermediate/*

