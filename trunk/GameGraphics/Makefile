#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../GameRunTime -I../GameRunTimeLib -I../VectorLib -I../GameLib -I../GameGUI -I../JPEGLibrary/inc

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

SOURCES_C = GLee.c

CFLAGS += -DNDEBUG -fPIC -Wall -Werror -D_DO_NOT_USE_ASSEMBLER_ -D_REENTRANT -O3 -g
CFLAGS_DEBUG += -DDEBUG -fPIC -Wall -Werror -D_DO_NOT_USE_ASSEMBLER_ -D_REENTRANT -g

LIBS = ../GameLib/lib/GameLib.a ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a -ldl -lGLU -lGL -lX11
LIBS_DEBUG =  ../GameLib/lib/GameLibd.a ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a -ldl -lGLU -lGL -lX11

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))
OUT_SOURCES_C=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES_C))

all: release debug
cfiles_release:$(OUT_SOURCES_C:.c=.o)
cppfiles_release: $(OUT_SOURCES:.cpp=.o)  
cfiles_debug:$(OUT_SOURCES_C:.c=.od)
cppfiles_debug: $(OUT_SOURCES:.cpp=.od)  

release: cfiles_release cppfiles_release
	rm -f ../Demo/Bin/Release/GameGraphics.so
	$(CC) -shared -o ../Demo/Bin/Release/GameGraphics.so ../JPEGLibrary/.intermediate/*.o .intermediate/*.o $(CFLAGS) $(LIBS)

debug: cfiles_debug cppfiles_debug
	rm -f ../Demo/Bin/Debug/GameGraphics.so 
	$(CC) -shared -o ../Demo/Bin/Debug/GameGraphics.so  ../JPEGLibrary/.intermediate/*.od .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG)

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.o: ./%.c
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.c
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/GameGraphics.so 
	rm -f ./.intermediate/*




