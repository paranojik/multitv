# Makefile for libskin
# Author: Andre Cotrim
# Date: 2008-02-17

# Dependencies

ifndef (OPENAL)
OPENAL = /usr/include/AL
endif

ifndef (FFMPEG)
FFMPEG = ../../../iptv_ffmpeg
endif

ifndef (IPTV_MEDIA)
IPTV_MEDIA = ../../../iptv_media2
endif

ifndef (IPTV_SHARED)
IPTV_SHARED = ../../../iptv_shared
endif

ifndef (IPTV_CHAT)
IPTV_CHAT = ../../../iptv_chat
endif

ifndef (IPTV_IRM_STUB)
IPTV_IRM_STUB = ../../../iptv_irm_stub
endif

ifndef (IPTV_AS_MODULE)
IPTV_AS_MODULE = ../../../ASModule_1_0
endif

ifndef (IPTV_KERNEL)
IPTV_KERNEL = ../../../iptv_kernel
endif

ifndef (VBLIB)
VBLIB = ../../../sbVB/VBLib_6_2
endif

# Paths
OUTDIR = ../../lib/g++
LIBDIRS =
OBJDIR = ../../obj/g++
INCDIRS = ../../include/Skin ../../include/iptv_client $(IPTV_MEDIA) $(IPTV_KERNEL)/include $(IPTV_SHARED) $(IPTV_SHARED)/shared $(IPTV_CHAT) $(IPTV_IRM_STUB) $(IPTV_KERNEL)/include $(IPTV_AS_MODULE)/include $(VBLIB)/include

# Intermediate files
OBJFILES = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OBJS = $(foreach FILE, $(OBJFILES), $(OBJDIR)/$(FILE))

# Output files
OUTFILE = libskin.a

# Tools
CXX = g++
LD = g++
AR = ar rcs
RM = rm -f
MKDIR = mkdir -p


# Flags
CFLAGS = `wx-config --cxxflags` -fPIC -D_LINUX $(foreach INCLUDE,$(INCDIRS),-I$(INCLUDE)) -Wno-deprecated-declarations
LDFLAGS = $(foreach DIR,$(LIBDIRS),-L$(DIR))
# NOTE: Order is important when linking statically!!
# LIBS = `wx-config --libs` -lwx_gtk2u_gl-2.8 -lGL -lopenal -liptv_media -lshared -lspeex -lx264 -lavcodec -lavutil -lcustomcontrol
LIBS = `wx-config --libs`

ifeq ($(DEBUG),1)
CFLAGS += -DIPTV_DEBUG -g
else
CFLAGS += -O3 -fno-strict-aliasing
endif

# Rules
all: $(OUTDIR)/$(OUTFILE)

$(OBJDIR)/%.o : %.cpp
	@echo Compiling $<...
	@$(CXX) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

$(OUTDIR)/$(OUTFILE): $(OBJDIR) $(OBJS)
	@echo Archiving $(OUTFILE)...
	@$(AR) $(OUTDIR)/$(OUTFILE) $(OBJS)

$(OBJDIR):
	@echo Creating directory $@...
	@$(MKDIR) $@

clean:
	@echo Removing output files...
	@$(RM) $(OUTDIR)/$(OUTFILE) $(OBJS)
