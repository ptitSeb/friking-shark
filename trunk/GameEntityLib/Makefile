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
Route.cpp\
SteeringBehaviours.cpp\
stdafx.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -fPIC -Wno-unused-function -Wall -Werror  -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -fPIC -Wno-unused-function -Wall -Werror  -D_REENTRANT -MMD -g

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: ./lib/GameEntityLib.a
debug: ./lib/GameEntityLibd.a

./lib/GameEntityLib.a: $(OUT_SOURCES:.cpp=.o)
	rm -f ./lib/GameEntityLib.a
	ar q ./lib/GameEntityLib.a .intermediate/*.o

./lib/GameEntityLibd.a: $(OUT_SOURCES:.cpp=.od)
	rm -f ./lib/GameEntityLibd.a
	ar q ./lib/GameEntityLibd.a .intermediate/*.od

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./lib/*
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
