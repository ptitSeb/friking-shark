#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../PortFiles -I../GameRunTime -I../GameRunTimeLib -I../VectorLib -I../GameLib -I../GameGUI -I../GameGUILib -I../GameGraphics

SOURCES = GameGUIButton.cpp\
GameGUILabel.cpp\
GameGUIEdit.cpp\
GameGUIGroup.cpp\
GameMainWindow.cpp\
GameGUIMessageDialog.cpp\
GameGUIConfirmDialog.cpp\
GameGUIColorDialog.cpp\
stdafx.cpp\
GameGUIManager.cpp\
GameGUI.cpp


CFLAGS +=  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG +=   -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -g

LIBS = ../GameLib/bin/GameLib.a ../GameGUILib/bin/GameGUILib.a ../GameRunTimeLib/bin/GameRuntimeLib.a   ../VectorLib/bin/VectorLib.a -ldl -lGLU -lGL
LIBS_DEBUG =  ../GameLib/bin/GameLibd.a ../GameGUILib/bin/GameGUILibd.a ../GameRunTimeLib/bin/GameRuntimeLibd.a  ../VectorLib/bin/VectorLibd.a -ldl -lGLU -lGL

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug

release: $(OUT_SOURCES:.cpp=.o)
	rm -f bin/GameGUI.so
	$(CC) -shared -o bin/GameGUI.so .intermediate/*.o $(CFLAGS) $(LIBS)

debug: $(OUT_SOURCES:.cpp=.od)
	rm -f bin/GameGUId.so
	$(CC) -shared -o bin/GameGUId.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./bin/*
	rm -f ./.intermediate/*

