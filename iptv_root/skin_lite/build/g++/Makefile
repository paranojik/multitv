# Makefile for iptv_client
# Author: Andre Cotrim
# Date: 2008-04-24

# Build control
ifndef (DEBUG)
DEBUG = 0
endif

# Dependencies

ifndef (SKIN_SDK)
SKIN_SDK = ../../../iptv_client
endif

ifndef (VBLIB)
VBLIB = ../../../sbVB/VBLib_6_2
endif

ifndef (CHARTDIRECTOR)
CHARTDIRECTOR = ../../../ChartDirector
endif

ifndef (WBOARD)
WBOARD = ../../../iptv_wboard
endif

# Paths
OUTDIR = ../../lib/g++
LIBDIRS = $(SKIN_SDK)/lib/g++ $(VBLIB)/lib/g++ $(CHARTDIRECTOR)/lib $(WBOARD)/lib/gcc
SRCDIR = ../../src/SkinLite
OBJDIR = ../../obj/g++
INCDIRS = ../../include/SkinLite $(SKIN_SDK)/include/Skin $(VBLIB)/include $(CHARTDIRECTOR)/include $(WBOARD)/include

# Intermediate files
OBJFILES = $(patsubst $(SRCDIR)/%.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp))
OBJS = $(foreach FILE, $(OBJFILES), $(OBJDIR)/$(FILE))

# Output files
OUTFILE = SkinLite.so

# Tools
CXX = g++
LD = g++
AR = ar rcs
RM = rm -f
MKDIR = mkdir -p
STRIP = strip

# Flags
CFLAGS = `wx-config --cxxflags` -fPIC -D_LINUX $(foreach INCLUDE,$(INCDIRS),-I$(INCLUDE)) -Wno-deprecated-declarations
LDFLAGS = $(foreach DIR,$(LIBDIRS),-L$(DIR)) -shared
# NOTE: Order is important when linking statically!!
#LIBS = `wx-config --libs` -lwx_gtk2u_gl-2.8 -lGL -lopenal -liptv_media -lshared -lspeex -lx264 -lavcodec -lavutil -lcustomcontrol
LIBS = `wx-config --libs` -lVBLib-6.2 -lskin -lchartdir -liptv_wboard

ifeq ($(DEBUG),1)
CFLAGS += -g -DIPTV_DEBUG -D_DEVELOPER
else
CFLAGS += -O3 -fno-strict-aliasing
endif

# Rules
all: $(OUTFILE)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@echo Compiling $<...
	@$(CXX) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

$(OUTFILE): $(OBJDIR) $(OBJS)
	@echo Linking $@...
	@$(LD) $(LDFLAGS) -o $(OUTDIR)/$(OUTFILE) $(OBJS) $(LIBS)
ifneq ($(DEBUG),1)
	@$(STRIP) $(OUTDIR)/$(OUTFILE)
endif

# Hackish way of emulating Visual Studio's post-build event
	@echo Performing post-build event...
	@$(MKDIR) $(SKIN_SDK)/bin/Graphics/SkinLite
	@cp $(OUTDIR)/$(OUTFILE) $(SKIN_SDK)/bin/Skin
	@cp ../../Graphics/* $(SKIN_SDK)/bin/Graphics/SkinLite

$(OBJDIR):
	@echo Creating directory $@...
	@$(MKDIR) $@

clean:
	@echo Removing output files...
	@$(RM) $(OUTFILE) $(OBJS)
