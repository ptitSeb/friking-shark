#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../GameRunTime \
-I../GameRunTimeLib \
-I../VectorLib \
-I../GameGUI \
-I../GameGUILib \
-I../GameGraphics \
-I../SoundSystems

SOURCES = GameGUISlider.cpp\
GameGUIButton.cpp\
GameGUILabel.cpp\
GameGUIEdit.cpp\
GameGUIGroup.cpp\
GameGUIList.cpp\
GameMainWindow.cpp\
GameGUIMessageDialog.cpp\
GameGUIConfirmDialog.cpp\
GameGUIColorDialog.cpp\
GameGUIFileDialog.cpp\
GameGUIInputDialog.cpp\
stdafx.cpp\
GameGUIManager.cpp\
GameGUI.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -MMD -g

LIBS = ../GameGUILib/lib/GameGUILib.a ../GameRunTimeLib/lib/GameRuntimeLib.a  ../VectorLib/lib/VectorLib.a 
LIBS_DEBUG =  ../GameGUILib/lib/GameGUILibd.a ../GameRunTimeLib/lib/GameRuntimeLibd.a ../VectorLib/lib/VectorLibd.a 

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: ../Demo/Bin/Release/GameGUI.so
debug: ../Demo/Bin/Debug/GameGUI.so

../Demo/Bin/Release/GameGUI.so: $(OUT_SOURCES:.cpp=.o) $(LIBS)
	$(CC) -shared -o ../Demo/Bin/Release/GameGUI.so .intermediate/*.o $(CFLAGS) $(LIBS) -ldl -lGLU -lGL

../Demo/Bin/Debug/GameGUI.so: $(OUT_SOURCES:.cpp=.od) $(LIBS_DEBUG)
	$(CC) -shared -o ../Demo/Bin/Debug/GameGUI.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG) -ldl -lGLU -lGL

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/GameGUI.so
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
