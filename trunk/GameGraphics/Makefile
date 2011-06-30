#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../GameRunTime \
-I../GameRunTimeLib \
-I../VectorLib \
-I../GameGUI \
-I../JPEGLibrary/inc

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

CFLAGS += -DNDEBUG -fPIC -Wall -Werror -D_DO_NOT_USE_ASSEMBLER_ -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG -fPIC -Wall -Werror -D_DO_NOT_USE_ASSEMBLER_ -D_REENTRANT -MMD -g

LIBS = ../GameRunTimeLib/lib/GameRuntimeLib.a   ../VectorLib/lib/VectorLib.a 
LIBS_DEBUG =  ../GameRunTimeLib/lib/GameRuntimeLibd.a  ../VectorLib/lib/VectorLibd.a

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))
OUT_SOURCES_C=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES_C))

all: release debug
release: ../Demo/Bin/Release/GameGraphics.so
debug: ../Demo/Bin/Debug/GameGraphics.so

../Demo/Bin/Release/GameGraphics.so: $(OUT_SOURCES_C:.c=.o)  $(OUT_SOURCES:.cpp=.o) $(LIBS)
	$(CC) -shared -o ../Demo/Bin/Release/GameGraphics.so .intermediate/*.o ../JPEGLibrary/.intermediate/*.o $(CFLAGS) $(LIBS) -ldl -lGLU -lGL -lX11 -lXrandr -lpng

../Demo/Bin/Debug/GameGraphics.so:  $(OUT_SOURCES_C:.c=.od)  $(OUT_SOURCES:.cpp=.od) $(LIBS_DEBUG)
	$(CC) -shared -o ../Demo/Bin/Debug/GameGraphics.so .intermediate/*.od ../JPEGLibrary/.intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG) -ldl -lGLU -lGL -lX11 -lXrandr -lpng

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.o: ./%.c
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.c
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/GameGraphics.so 
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES_C:c=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
-include $(OUT_SOURCES_C:c=od.dep)



