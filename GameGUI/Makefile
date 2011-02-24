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

LIBS = ../GameLib/lib/GameLib.a ../GameGUILib/lib/GameGUILib.a ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a -ldl -lGLU -lGL
LIBS_DEBUG =  ../GameLib/lib/GameLibd.a ../GameGUILib/lib/GameGUILibd.a ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a -ldl -lGLU -lGL

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug

release: $(OUT_SOURCES:.cpp=.o)
	rm -f ../Demo/Bin/Release/GameGUI.so
	$(CC) -shared -o ../Demo/Bin/Release/GameGUI.so .intermediate/*.o $(CFLAGS) $(LIBS)

debug: $(OUT_SOURCES:.cpp=.od)
	rm -f ../Demo/Bin/Debug/GameGUI.so
	$(CC) -shared -o ../Demo/Bin/Debug/GameGUI.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/GameGUI.so
	rm -f ./.intermediate/*

