#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../PortFiles -I../GameRunTime

SOURCES = 3DSFiles.cpp\
3DSTypes.cpp\
ASEFiles.cpp\
GBSFiles.cpp\
QuakeFiles.cpp\
StdAfx.cpp\
Utilities.cpp\
VectorLib.cpp


CFLAGS +=  -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG +=   -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -g

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: $(OUT_SOURCES:.cpp=.o)
	rm -f VectorLib.a
	ar q bin/VectorLib.a .intermediate/*.o
debug: $(OUT_SOURCES:.cpp=.od)
	rm -f VectorLibd.a
	ar q bin/VectorLibd.a .intermediate/*.od

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./bin/*
	rm -f ./.intermediate/*

