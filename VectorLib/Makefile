#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. \
-I../GameRunTime \
-I../GameRunTimeLib

SOURCES = 3DSFiles.cpp\
3DSTypes.cpp\
ASEFiles.cpp\
GBSFiles.cpp\
GCMFiles.cpp\
StdAfx.cpp\
VectorLib.cpp


CFLAGS += -DNDEBUG -pedantic -pedantic-errors -fPIC -Wno-unused-function -Wall -Werror  -D_REENTRANT -O3 -MMD -g
CFLAGS_DEBUG += -DDEBUG  -pedantic -pedantic-errors -fPIC -Wno-unused-function -Wall -Werror  -D_REENTRANT -MMD -g

INTERMEDIATE_DIR = ./.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: ./lib/VectorLib.a
debug: ./lib/VectorLibd.a

./lib/VectorLib.a: $(OUT_SOURCES:.cpp=.o)
	rm -f ./lib/VectorLib.a
	ar q ./lib/VectorLib.a .intermediate/*.o

./lib/VectorLibd.a: $(OUT_SOURCES:.cpp=.od)
	rm -f ./lib/VectorLibd.a
	ar q ./lib/VectorLibd.a .intermediate/*.od

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -MF $@.dep -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ./lib/*
	rm -f ./.intermediate/*

-include $(OUT_SOURCES:cpp=o.dep)
-include $(OUT_SOURCES:cpp=od.dep)
