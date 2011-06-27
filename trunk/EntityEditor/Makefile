#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../AnimationSystems\
-I../ParticleSystems\
-I../GameRunTime \
-I../GameRunTimeLib \
-I../GameEntityLib \
-I../GameGUI \
-I../GameGUILib \
-I../GameGraphics \
-I../GameManagers \
-I../SoundSystems \
-I../VectorLib \

SOURCES = EntityEditor.cpp\
EntityEditorMainWindow.cpp\
EntityEditorNamedObjectSelector.cpp\
EntityEditorObjectListSelector.cpp\
EntityEditorClassSelector.cpp\
EntityEditorGeneralPropertyPanel.cpp\
EntityEditorAnimationPropertyPanel.cpp\
EntityEditorModelPropertyPanel.cpp\
EntityEditorSoundPropertyPanel.cpp\
EntityEditorEventPropertyPanel.cpp\
EntityEditorParticleSystemPropertyPanel.cpp\
EntityEditorEntityPropertyPanel.cpp\
stdafx.cpp



CFLAGS += -DNDEBUG -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -MMD -g

LIBS = ../GameEntityLib/lib/GameEntityLib.a ../GameGUILib/lib/GameGUILib.a  ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a
LIBS_DEBUG = ../GameEntityLib/lib/GameEntityLibd.a ../GameGUILib/lib/GameGUILibd.a ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: ../Demo/Bin/Release/EntityEditor.so
debug: ../Demo/Bin/Debug/EntityEditor.so

../Demo/Bin/Release/EntityEditor.so: $(OUT_SOURCES:.cpp=.o) $(LIBS)
	$(CC) -shared -o ../Demo/Bin/Release/EntityEditor.so .intermediate/*.o $(CFLAGS) $(LIBS)

../Demo/Bin/Debug/EntityEditor.so: $(OUT_SOURCES:.cpp=.od) $(LIBS_DEBUG)
	$(CC) -shared -o ../Demo/Bin/Debug/EntityEditor.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/EntityEditor.so
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
