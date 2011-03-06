#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../GameRunTime \
-I../GameRunTimeLib \
-I../GameGUI \
-I../GameGraphics \
-I../GameManagers \
-I../VectorLib \

SOURCES = SoundSystems.cpp\
SoundSystemManager.cpp\
SoundType.cpp\
stdafx.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -g

LIBS = ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a -lopenal  -lalut
LIBS_DEBUG =  ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a -lopenal -lalut

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug

release: $(OUT_SOURCES:.cpp=.o)
	rm -f ../Demo/Bin/Release/SoundSystems.so
	$(CC) -shared -o ../Demo/Bin/Release/SoundSystems.so .intermediate/*.o $(CFLAGS) $(LIBS)

debug: $(OUT_SOURCES:.cpp=.od)
	rm -f ../Demo/Bin/Debug/SoundSystems.so
	$(CC) -shared -o ../Demo/Bin/Debug/SoundSystems.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/SoundSystems.so
	rm -f ./.intermediate/*

