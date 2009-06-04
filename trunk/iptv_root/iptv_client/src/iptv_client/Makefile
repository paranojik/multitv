# Makefile for iptv_client
# Author: Andre Cotrim
# Date: 2008-06-23

# Dependencies
ifndef (VIDEO_CAPTURE)
VIDEO_CAPTURE = ../../../iptv_video_capture
endif

ifndef (OPENAL)
OPENAL = ../../../openal-soft/include/AL
endif

ifndef (OPENAL_LIB)
OPENAL_LIB = ../../../openal-soft/CMakeConf
endif

ifndef (FFMPEG)
FFMPEG = ../../../iptv_ffmpeg2
endif

ifndef (INTEL_IPP)
INTEL_IPP = $(FFMPEG)/libavcodec/intel_ipp
endif

ifndef (IPTV_ROOT)
IPTV_ROOT = ../../../
endif

ifndef (IPTV_MEDIA)
IPTV_MEDIA = ../../../iptv_media3
endif

ifndef (IPTV_MEDIA_UTIL)
IPTV_MEDIA_UTIL = ../../../iptv_media_util
endif

ifndef (IPTV_AUDIO_CAPTURE)
IPTV_AUDIO_CAPTURE = ../../../iptv_audio_capture
endif

ifndef (LUA_PATH)
LUA_PATH = $(IPTV_MEDIA_UTIL)/lua
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

ifndef (IPTV_XFILETRANSFER)
IPTV_XFILETRANSFER = ../../../iptv_xfiletransfer
endif

ifndef (IPTV_KERNEL)
IPTV_KERNEL = ../../../iptv_kernel
endif

ifndef (IPTV_APPSHARING)
IPTV_APPSHARING = ../../../iptv_appsharing
endif

ifndef (VBLIB)
VBLIB = ../../../sbVB/VBLib_6_2
endif

# Paths
OUTDIR = ../../bin

# NOTE: Order is important when linking statically!!
LIBDIRS = . ../../lib/g++ $(OPENAL_LIB) $(IPTV_MEDIA)/lib $(IPTV_MEDIA_UTIL)/lib $(IPTV_AUDIO_CAPTURE)/lib $(LUA_PATH)/lib $(IPTV_CHAT)/chat/lib $(IPTV_IRM_STUB)/stub/lib $(IPTV_SHARED)/shared/lib $(IPTV_XFILETRANSFER)/xfiletransfer/lib $(IPTV_APPSHARING)/lib/gcc $(IPTV_KERNEL)/lib/g++ $(VBLIB)/lib/g++ $(FFMPEG)/linux-prebuilt $(INTEL_IPP)/speech-codecs/bin/_/lib $(INTEL_IPP)/ipp_static_libs $(VIDEO_CAPTURE)/lib/gcc
OBJDIR = ../../obj/g++
INCDIRS = ../../include/Skin ../../include/iptv_client $(IPTV_MEDIA) $(IPTV_MEDIA_UTIL) $(IPTV_AUDIO_CAPTURE) $(LUA_PATH)/include $(IPTV_KERNEL)/include $(IPTV_SHARED) $(IPTV_SHARED)/shared $(IPTV_CHAT) $(IPTV_IRM_STUB) $(IPTV_XFILETRANSFER) $(IPTV_KERNEL)/include $(VBLIB)/include $(OPENAL) $(FFMPEG)/libavcodec $(FFMPEG)/libavutil $(IPTV_ROOT)

# Intermediate files
OBJFILES = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OBJS = $(foreach FILE, $(OBJFILES), $(OBJDIR)/$(FILE))

# Output files
OUTFILE = iptv_client

# Tools
CXX = g++
LD = g++
AR = ar rcs
RM = rm -f
MKDIR = mkdir -p
STRIP = strip

# Flags
CFLAGS = `wx-config --cxxflags` -D_LINUX -DIPTV_USING_MEDIA $(foreach INCLUDE,$(INCDIRS),-I$(INCLUDE)) -Wno-deprecated-declarations
LDFLAGS = -rdynamic $(foreach DIR,$(LIBDIRS),-L$(DIR))
# NOTE: Order is important when linking statically!!
# NOTE: iptv_kernel is forced to static link for now 
# -Wl,-Bstatic means all libs after it will be linked statically - INCLUDING THE IMPLICIT STANDARD LIBRARY!!
# -Wl,-Bdynamic means try linking all libs after it with shared object, if unable to link, try static. This is the
# default behavior. Same notice about implicit standard library goes for that too.
LIBS = -Wl,-Bstatic -liptv_kernel -liptv_video_capture -liptv_media -liptv_media_util -liptv_audio_capture -lavformat -lavcodec -lavutil -lusc -lfaac -lfaad -lippscemerged -lippscmerged -lippsemerged -lippsmerged -lippcore -Wl,-Bdynamic `wx-config --libs` -lwx_gtk2u_gl-2.8 -lGL -Wl,-Bstatic -lskin -lopenal -llua -llualib -Wl,-Bdynamic -liptv_xfiletransfer -liptv_stub -liptv_chat -lappsharing -lshared -lspeex -lx264 -lVBLib-6.2


ifeq ($(DEBUG),1)
CFLAGS += -g -DIPTV_DEBUG
else
CFLAGS += -O3 -fno-strict-aliasing
endif

# Rules
all: $(OBJDIR) $(OBJS) $(OUTDIR)/$(OUTFILE)

link: del_target $(OUTDIR)/$(OUTFILE)

del_target:
	@rm -f $(OUTDIR)/$(OUTFILE)

$(OBJDIR)/%.o : %.cpp
	@echo Compiling $<...
	@$(CXX) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

$(OUTDIR)/$(OUTFILE): $(OBJS)
	@echo Linking $(OUTFILE)...
	@$(LD) $(LDFLAGS) -o $(OUTDIR)/$(OUTFILE)  $(OBJS) $(LIBS)
ifneq ($(DEBUG),1)
	@$(STRIP) $(OUTDIR)/$(OUTFILE)
endif

$(OBJDIR):
	@echo Creating directory $@...
	@$(MKDIR) $@

clean:
	@echo Removing output files...
	@$(RM) $(OUTDIR)/$(OUTFILE) $(OBJS)
