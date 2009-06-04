# Project: iptv_irm_stub Library

OUTDIR=lib
OBJDIR=obj
OUTFILE=libiptv_stub.a


OBJS = IrmStub.o IrmStubDataSink.o IrmStubDll.o 


CXX=	g++
AR=	ar
CXXFLAGS=	-Wall -fPIC -I../../iptv_shared -I../../iptv_chat
ARFLAGS=rcs

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
$(OBJDIR)/%.o : %.cpp
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) $(DEFS) -c $< -o $@

all: $(OUTDIR) $(OBJDIR) $(OUTDIR)/$(OUTFILE)

$(OUTDIR) $(OBJDIR):
	@echo Creating directory $@...
	@mkdir -p $@

$(OUTDIR)/$(OUTFILE): $(foreach FILE,$(OBJS),$(OBJDIR)/$(FILE))
	@echo Archiving $@...
	$(AR) $(ARFLAGS) $(OUTDIR)/$(OUTFILE) $(foreach FILE,$(OBJS),$(OBJDIR)/$(FILE))

clean:
	@echo Cleaning up...
	-rm -rf $(OBJDIR)
	-rm $(OUTDIR)/$(OUTFILE)


# End of Makefile

