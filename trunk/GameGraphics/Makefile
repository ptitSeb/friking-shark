#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../PortFiles -I../GameRunTime -I../GameRunTimeLib -I../VectorLib -I../GameLib -I../GameGUI

SOURCES = GameGraphics.cpp\
OpenGLCamera.cpp\
OpenGLFont.cpp\
OpenGLLight.cpp\
OpenGLModel.cpp\
OpenGLRender.cpp\
OpenGLShader.cpp\
OpenGLTexture.cpp\
OpenGLViewport.cpp\
stdafx.cpp


CFLAGS +=  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -g
CFLAGS_DEBUG +=   -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -g

LIBS = ../GameLib/lib/GameLib.a ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a -ldl -lGLU -lSDL -lGL -lX11
LIBS_DEBUG =  ../GameLib/lib/GameLibd.a ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a -ldl -lGLU -lSDL -lGL -lX11

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug

release: $(OUT_SOURCES:.cpp=.o)
	rm -f ../Demo/Bin/GameGraphics.so 
	$(CC) -shared -o ../Demo/Bin/GameGraphics.so .intermediate/*.o $(CFLAGS) $(LIBS)

debug: $(OUT_SOURCES:.cpp=.od)
	rm -f ../Demo/Bin/GameGraphicsd.so 
	$(CC) -shared -o ../Demo/Bin/GameGraphicsd.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/GameGraphics*.so 
	rm -f ./.intermediate/*

