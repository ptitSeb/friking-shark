#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../PortFiles -I../GameRunTime

SOURCES = ConfigFile.cpp\
Persistency.cpp\
stdafx.cpp\
SystemModuleHelpers.cpp\
SystemObjectWrapper.cpp\
EventBase.cpp\
PersistentTypes.cpp\
SystemLoaderHelper.cpp\
SystemObjectBase.cpp\
SystemUnknownBase.cpp

CFLAGS +=  -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG +=   -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -g

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: $(OUT_SOURCES:.cpp=.o)
	rm -f bin/GameRuntimeLib.a
	ar q bin/GameRuntimeLib.a .intermediate/*.o
debug: $(OUT_SOURCES:.cpp=.od)
	rm -f bin/GameRuntimeLibd.a
	ar q bin/GameRuntimeLibd.a .intermediate/*.od

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./bin/*
	rm -f ./.intermediate/*

