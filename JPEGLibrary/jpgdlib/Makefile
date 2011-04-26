#Makefile
#SHELL = /bin/sh

CC=g++

INCLUDES = -I. -I../../PortFiles -I../inc

SOURCES = H2v2.cpp\
idct.cpp\
jpegdecoder.cpp


CFLAGS += -D_DO_NOT_USE_ASSEMBLER_ -fPIC -D_REENTRANT -g
CFLAGS_DEBUG += -D_DO_NOT_USE_ASSEMBLER_ -fPIC -D_REENTRANT -g

INTERMEDIATE_DIR = ../.intermediate
OUT_SOURCES=$(addprefix $(INTERMEDIATE_DIR)/,$(SOURCES))

all: release debug
release: $(OUT_SOURCES:.cpp=.o)
	ar q ../lib/jpgdlib.a ../.intermediate/*.o
debug: $(OUT_SOURCES:.cpp=.od)
	ar q ../lib/jpgdlibd.a ../.intermediate/*.od

$(INTERMEDIATE_DIR)/%.o: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS) $(INCLUDES) $<

$(INTERMEDIATE_DIR)/%.od: ./%.cpp
	$(CXX) -o $@ -c $(DFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $<

clean:
	rm -f ../lib/*.a
	rm -f ../intermediate/*

