#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../PortFiles -I../GameRunTimeLib

SOURCES = GameRunTime.cpp\
InterfaceLeakAPI.cpp\
stdafx.cpp\
System.cpp\
SystemManager.cpp\
SystemModule.cpp


CFLAGS +=  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG +=   -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -g

LIBS = ../GameRunTimeLib/lib/GameRuntimeLib.a -ldl
LIBS_DEBUG = ../GameRunTimeLib/lib/GameRuntimeLibd.a -ldl

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug

release: $(OUT_SOURCES:.cpp=.o)
	rm -f ../Demo/Bin/GameRuntime.so
	$(CC) -shared -o ../Demo/Bin/GameRuntime.so .intermediate/*.o $(CFLAGS) $(LIBS)

debug: $(OUT_SOURCES:.cpp=.od)
	rm -f ../Demo/Bin/GameRuntimed.so
	$(CC) -shared -o ../Demo/Bin/GameRuntimed.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./Demo/Bin/GameRuntime*.so
	rm -f ./.intermediate/*

