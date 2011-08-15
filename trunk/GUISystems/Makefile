#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../PlayAreaElements \
-I../GameRunTime \
-I../GameRunTimeLib \
-I../GameEntityLib \
-I../GameGUI \
-I../GameGUILib \
-I../GameGraphics \
-I../GameManagers \
-I../VectorLib \
-I../SoundSystems \

SOURCES = GUISystems.cpp \
GameInterface.cpp \
MainWindow.cpp \
ConfirmationDialog.cpp \
GameMenu.cpp \
GameOverDialog.cpp \
HighScoresDialog.cpp \
HighScoresTable.cpp \
ControlsDialog.cpp \
KeyCaptureDialog.cpp \
MainMenu.cpp \
OptionsMenu.cpp \
LevelOptions.cpp \
AudioOptions.cpp \
CreditsDialog.cpp \
LoadDialog.cpp \
SaveDialog.cpp \
SavedGameRow.cpp \
stdafx.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -MMD -g

LIBS = ../GameGUILib/lib/GameGUILib.a  ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a
LIBS_DEBUG =  ../GameGUILib/lib/GameGUILibd.a ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: ../Demo/Bin/Release/GUISystems.so
debug: ../Demo/Bin/Debug/GUISystems.so

../Demo/Bin/Release/GUISystems.so: $(OUT_SOURCES:.cpp=.o) $(LIBS)
	$(CC) -shared -o ../Demo/Bin/Release/GUISystems.so .intermediate/*.o $(CFLAGS) $(LIBS)

../Demo/Bin/Debug/GUISystems.so: $(OUT_SOURCES:.cpp=.od) $(LIBS_DEBUG)
	$(CC) -shared -o ../Demo/Bin/Debug/GUISystems.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/GUISystems.so
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
