# Makefile

# Tools
CXX = g++
LD = g++
STRIP = strip

# Library paths (may be overridden by make command line)
ifndef VBLIB
VBLIB = ./VBLib
endif

CXXFLAGS = -O3 -fno-strict-aliasing
LDFLAGS =

INCLUDES = -I$(VBLIB)/include
LIBS = -L$(VBLIB)/lib/g++ -lVBLib-6.2

OBJS = main.o BuildIDParser.o
EXEC = buildid-parser

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(EXEC): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	$(STRIP) $@

clean:
	rm -f $(EXEC) $(OBJS)
