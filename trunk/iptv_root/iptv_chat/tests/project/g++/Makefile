# Project: Chat Tests

OUTDIR=../../bin
OBJDIR=../../obj
SRCDIR=../../src

OUTFILE=testInstantiate

OBJS = testInstantiate.o

CXX=	g++
LD=	g++
CXXFLAGS= -Wall -I../../../../iptv_chat -I../../../../iptv_shared
LIBS=	 -liptv_chat -lshared
LDFLAGS=-pthread -L../../../../iptv_chat/chat/lib -L../../../../iptv_shared/shared/lib

DEFS =	-D_LINUX

#VPATH = ./socket:./irm

ifeq ($(DEBUG),1)
#DEFS += -D_DEBUG -D_MM_
DEFS += -D_DEBUG
CXXFLAGS += -g
else
CXXFLAGS += -O2 -fno-strict-aliasing -Wno-unused
endif

#gcc -Wall -g -O2 -D_LINUX -D_DEBUG -D_MM_ -c clbuffer.cpp -o debug/clbuffer.o
#.cpp.o:
$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) $(DEFS) -c $< -o $@

all: $(OUTDIR) $(OBJDIR) $(OUTDIR)/$(OUTFILE)

$(OUTDIR) $(OBJDIR):
	@echo Creating directory $@...
	@mkdir -p $@

$(OUTDIR)/$(OUTFILE): $(foreach FILE,$(OBJS),$(OBJDIR)/$(FILE))
	@echo Linking $@...
	$(LD) -o $@ $(foreach FILE,$(OBJS),$(OBJDIR)/$(FILE)) $(LDFLAGS) $(LIBS)

clean:
	@echo Cleaning up...
	-rm -rf $(OBJDIR)
	-rm $(OUTDIR)/$(OUTFILE)


# End of Makefile

