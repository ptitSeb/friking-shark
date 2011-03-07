#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../GameRunTime

SOURCES = PlatformDependent.cpp\
ConfigFile.cpp\
Persistency.cpp\
stdafx.cpp\
SystemModuleHelpers.cpp\
SystemObjectWrapper.cpp\
EventBase.cpp\
PersistentTypes.cpp\
SystemLoaderHelper.cpp\
SystemObjectBase.cpp\
SystemUnknownBase.cpp

CFLAGS += -DNDEBUG -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -Wno-unused-function -Wall -Werror  -D_REENTRANT -MMD -g

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: ./lib/GameRuntimeLib.a
debug: ./lib/GameRuntimeLibd.a

./lib/GameRuntimeLib.a: $(OUT_SOURCES:.cpp=.o)
	rm -f ./lib/GameRuntimeLib.a
	ar q ./lib/GameRuntimeLib.a .intermediate/*.o

./lib/GameRuntimeLibd.a: $(OUT_SOURCES:.cpp=.od)
	rm -f ./lib/GameRuntimeLibd.a
	ar q ./lib/GameRuntimeLibd.a .intermediate/*.od


$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./lib/*
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
