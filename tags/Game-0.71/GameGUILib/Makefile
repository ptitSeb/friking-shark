#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../GameRunTime \
-I../GameRunTimeLib \
-I../VectorLib \
-I../GameGraphics  \
-I../GameGUI

SOURCES = GameGUIUtil.cpp\
GameWindowBase.cpp\
GameDialogBase.cpp\
stdafx.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -MMD -g

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: ./lib/GameGUILib.a
debug: ./lib/GameGUILibd.a

./lib/GameGUILib.a: $(OUT_SOURCES:.cpp=.o)
	rm -f ./lib/GameGUILib.a
	ar q ./lib/GameGUILib.a .intermediate/*.o

./lib/GameGUILibd.a: $(OUT_SOURCES:.cpp=.od)
	rm -f ./lib/GameGUILibd.a
	ar q ./lib/GameGUILibd.a .intermediate/*.od

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./lib/*
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
