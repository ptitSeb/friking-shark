#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../GameRunTimeLib

SOURCES = GameRunTime.cpp\
InterfaceLeakAPI.cpp\
stdafx.cpp\
System.cpp\
SystemManager.cpp\
SystemModule.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -fPIC -Wall -Werror  -D_REENTRANT -MMD -g

LIBS = ../GameRunTimeLib/lib/GameRuntimeLib.a
LIBS_DEBUG = ../GameRunTimeLib/lib/GameRuntimeLibd.a

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: ../Demo/Bin/Release/GameRuntime.so
debug: ../Demo/Bin/Debug/GameRuntime.so

../Demo/Bin/Release/GameRuntime.so: $(OUT_SOURCES:.cpp=.o) $(LIBS)
	$(CC) -shared -o ../Demo/Bin/Release/GameRuntime.so .intermediate/*.o $(CFLAGS) $(LIBS) -ldl

../Demo/Bin/Debug/GameRuntime.so: $(OUT_SOURCES:.cpp=.od) $(LIBS_DEBUG)
	$(CC) -shared -o ../Demo/Bin/Debug/GameRuntime.so .intermediate/*.od $(CFLAGS_DEBUG) $(LIBS_DEBUG) -ldl

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../Demo/Bin/*/GameRuntime.so
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
