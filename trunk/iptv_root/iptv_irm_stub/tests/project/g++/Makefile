# Project: Stub Tests

OUTDIR=../../bin
OBJDIR=../../obj
SRCDIR=../../src

OUTFILES=testIrm testConnect testInstantiate

TESTINSTOBJS = testInstantiate.o
TESTCONNECTOBJS = testIrmQuery.o testConnect.o 
TESTIRMOBJS = testIrmQuery.o testIrm.o

CXX=	g++
LD=	g++
CXXFLAGS= -Wall -I../../../../iptv_irm_stub -I../../../../iptv_irm_stub/tests/include \
-I../../../../iptv_chat -I../../../../iptv_shared
LIBS=	 -liptv_stub -liptv_chat -lshared
LDFLAGS=-pthread -L../../../../iptv_irm_stub/stub/lib -L../../../../iptv_chat/chat/lib \
-L../../../../iptv_shared/shared/lib

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

all: $(OUTDIR) $(OBJDIR) $(OUTFILES)

$(OUTDIR) $(OBJDIR):
	@echo Creating directory $@...
	@mkdir -p $@

testInstantiate: $(foreach FILE,$(TESTINSTOBJS),$(OBJDIR)/$(FILE))
	@echo Linking $@...
	$(LD) -o $(OUTDIR)/$@ $(foreach FILE,$(TESTINSTOBJS),$(OBJDIR)/$(FILE)) $(LDFLAGS) $(LIBS)

testConnect: $(foreach FILE,$(TESTCONNECTOBJS),$(OBJDIR)/$(FILE))
	@echo Linking $@...
	$(LD) -o $(OUTDIR)/$@ $(foreach FILE,$(TESTCONNECTOBJS),$(OBJDIR)/$(FILE)) $(LDFLAGS) $(LIBS)

testIrm: $(foreach FILE,$(TESTIRMOBJS),$(OBJDIR)/$(FILE))
	@echo Linking $@...
	$(LD) -o $(OUTDIR)/$@ $(foreach FILE,$(TESTIRMOBJS),$(OBJDIR)/$(FILE)) $(LDFLAGS) $(LIBS)

#$(OUTDIR)/$(OUTFILES): $(foreach FILE,$(OBJS),$(OBJDIR)/$(FILE))
#	@echo Linking $@...
#	$(LD) -o $@ $(foreach FILE,$(OBJS),$(OBJDIR)/$(FILE)) $(LDFLAGS) $(LIBS)

clean:
	@echo Cleaning up...
	-rm -f $(OBJDIR)/*.o
	-rm -f $(foreach FILE,$(OUTFILES),$(OUTDIR)/$(FILE))


# End of Makefile

