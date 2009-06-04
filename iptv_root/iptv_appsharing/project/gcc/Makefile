# Project: Application Sharing Library
# Version 1.0.0

ifndef IPTV_SHARED
IPTV_SHARED = ../../../iptv_shared/
endif

ifndef ZLIB
ZLIB = ../../../zlib/
endif

ifndef IPTV_ROOT
IPTV_ROOT = ../../../
endif

CC = g++
LD = g++
AR = ar
CFLAGS   = -c -Wall -g -O2
LDFLAGS  = -Wall
ARFLAGS  = rcs
DEFS     = -D_LINUX

SRCDIR   = ../../src/
INCDIR   = ../../include/
OBJDIR   = ../../obj/gcc/
OUTDIR   = ../../lib/gcc/

INCLUDES = -I. -I$(INCDIR) -I$(IPTV_SHARED) -I$(IPTV_SHARED)/shared -I$(ZLIB) -I$(IPTV_ROOT)

OUTFILE  = libappsharing.a

VPATH    = $(SRCDIR)

SOURCES  = ASFrame.cpp ASModuleCross.cpp CxAppSharing.cpp CxEventCross.cpp CxReceiveBase.cpp CxReceiveCross.cpp CxReceiveThread.cpp CxThreadCross.cpp CxZLib.cpp
OBJECTS  = $(SOURCES:.cpp=.o)
OBJFILES = $(foreach FILE,$(OBJECTS),$(OBJDIR)/$(FILE))

$(OBJDIR)/%.o: %.cpp
	@echo --------------------------------------------------------------------------------
	@echo Building $<
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) $< -o $@

all: libAppSharing

libAppSharing: init $(OBJFILES)
	@echo ================================================================================
	@echo Assembling AppSharing lib ...
	@echo --------------------------------------------------------------------------------
	$(AR) $(ARFLAGS) $(OUTDIR)/$(OUTFILE) $(OBJFILES)

init: $(OUTDIR) $(OBJDIR)
#	@echo ================================================================================
#	@echo Preparing to build ...
	@echo ================================================================================
	@echo Starting build process ...

$(OUTDIR) $(OBJDIR):
	@echo Creating directory $@...
	mkdir -p $@

clean:
	@echo ================================================================================
	@echo Cleaning ...
	rm -f $(OBJFILES) $(OUTDIR)/$(OUTFILE)

# End of Makefile
