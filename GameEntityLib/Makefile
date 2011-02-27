#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../AnimationSystems  \
-I../GameManagers \
-I../GameRunTime \
-I../GameRunTimeLib \
-I../VectorLib

SOURCES = GameEntityLib.cpp\
EntityBase.cpp\
EntityTypeBase.cpp\
EntityUtilities.cpp\
SteeringBehaviours.cpp\
stdafx.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -g

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: $(OUT_SOURCES:.cpp=.o)
	rm -f ./lib/GameEntityLib.a
	ar q ./lib/GameEntityLib.a .intermediate/*.o
debug: $(OUT_SOURCES:.cpp=.od)
	rm -f ./lib/GameEntityLibd.a
	ar q ./lib/GameEntityLibd.a .intermediate/*.od

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./lib/*
	rm -f ./.intermediate/*

