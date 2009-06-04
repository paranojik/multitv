# Makefile for iptv_kernel
# Author: Andre Cotrim & Daniel Gouvêa
# Date: 2008-02-17

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

ifndef (IPTV_APPSHARING)
IPTV_APPSHARING = ../../../iptv_appsharing
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

ifndef (IPTV_IRM_STUB)
IPTV_IRM_STUB = ../../../iptv_irm_stub
endif

ifndef (IPTV_CHAT)
IPTV_CHAT = ../../../iptv_chat
endif

ifndef (IPTV_XFILETRANSFER)
IPTV_XFILETRANSFER = ../../../iptv_xfiletransfer
endif

ifndef (VBLIB)
VBLIB = ../../../sbVB/VBLib_6_2
endif

# Set this to zero (either here or via command line) to disable iptv_media dependency
ifndef (IPTV_USING_MEDIA)
IPTV_USING_MEDIA = 1
endif

# Paths
OUTDIR = ../../lib/g++
SRCDIR = ../../src
TESTSRCDIR = ../../src/test
BINDIR = ../../bin

LIBDIRS = ../../lib/g++ $(IPTV_SHARED)/shared/lib $(VBLIB)/lib/g++ $(IPTV_IRM_STUB)/stub/lib $(IPTV_CHAT)/chat/lib $(IPTV_XFILETRANSFER)/xfiletransfer/lib $(VIDEO_CAPTURE)/lib/gcc $(IPTV_APPSHARING)/lib/gcc
INCDIRS = ../../include $(IPTV_SHARED) $(IPTV_SHARED)/shared $(VBLIB)/include $(IPTV_IRM_STUB) $(IPTV_CHAT) $(IPTV_XFILETRANSFER) $(IPTV_ROOT)


OBJDIR = ../../obj/g++

# Intermediate files
OBJFILES = $(patsubst $(SRCDIR)/%.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp)) $(patsubst $(SRCDIR)/linux/%.cpp,%.o,$(wildcard $(SRCDIR)/linux/*.cpp))
OBJS = $(foreach FILE, $(OBJFILES), $(OBJDIR)/$(FILE))

INSTANTIATEOBJFILES = KernelInstantiation.o
SHELLCLIENTOBJFILES = ShellClient.o
INSTANTIATEOBJS = $(foreach FILE, $(INSTANTIATEOBJFILES), $(OBJDIR)/$(FILE))
SHELLCLIENTOBJS = $(foreach FILE, $(SHELLCLIENTOBJFILES), $(OBJDIR)/$(FILE))

# Output files
ifeq ($(MAKE_KERNEL_SO),1)
SHAREDLIB = libiptv_kernel.so
endif
STATICLIB = libiptv_kernel.a
INSTANTIATE = test_instantiate
SHELLCLIENT = shell_client

# Tools
CXX = g++
LD = g++
AR = ar rcs
RM = rm -f
MKDIR = mkdir -p


# Flags
CFLAGS = -D_LINUX $(foreach INCLUDE,$(INCDIRS),-I$(INCLUDE)) -fPIC -Wno-deprecated-declarations
LDFLAGS = $(foreach DIR,$(LIBDIRS),-L$(DIR))

ifeq ($(DEBUG),1)
CFLAGS += -g
else
CFLAGS += -O3 -fno-strict-aliasing
endif

# Initially empty LIBS variable. See NOTE below.
LIBS = 
# Enable iptv_media dependency if IPTV_USING_MEDIA equals 1
#
ifeq ($(IPTV_USING_MEDIA),1)
LIBDIRS += $(IPTV_MEDIA)/lib $(IPTV_MEDIA_UTIL)/lib $(IPTV_AUDIO_CAPTURE)/lib $(LUA_PATH)/lib $(FFMPEG)/linux-prebuilt $(INTEL_IPP)/ipp_static_libs $(INTEL_IPP)/speech-codecs/bin/_/lib $(OPENAL_LIB)
INCDIRS += $(IPTV_MEDIA) $(IPTV_MEDIA_UTIL) $(IPTV_AUDIO_CAPTURE) $(LUA_PATH)/include $(FFMPEG)/libavcodec $(FFMPEG)/libavutil $(OPENAL)
CFLAGS += -DIPTV_USING_MEDIA
LIBS += -liptv_video_capture -liptv_media -liptv_media_util -liptv_audio_capture $(FFMPEG)/linux-prebuilt/libavformat.a $(FFMPEG)/linux-prebuilt/libavcodec.a $(FFMPEG)/linux-prebuilt/libavutil.a -lspeex -lx264 -Wl,-Bstatic -lfaac -lfaad -lusc -lippscemerged -lippscmerged -lippsemerged -lippsmerged -lippcore -lopenal -llua -llualib -Wl,-Bdynamic -ldl
endif

# NOTE: Order is important when linking statically!!
LIBS +=-pthread -lVBLib-6.2 -liptv_stub -liptv_chat -liptv_xfiletransfer -lappsharing -lshared -lz -lX11

# Rules
all: lib test

lib:$(OUTDIR)/$(SHAREDLIB) $(OUTDIR)/$(STATICLIB)

test: $(BINDIR)/$(INSTANTIATE) $(BINDIR)/$(SHELLCLIENT)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@echo Compiling $<...
	@$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o : $(SRCDIR)/linux/%.cpp
	@echo Compiling $<...
	@$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o : $(TESTSRCDIR)/%.cpp
	@echo Compiling test program file $<...
	@$(CXX) $(CFLAGS) -c $< -o $@

$(OUTDIR)/$(STATICLIB): $(OBJDIR) $(OBJS)
	@echo Archiving $(STATICLIB)...
	@$(AR) $(OUTDIR)/$(STATICLIB) $(OBJS)

ifeq ($(MAKE_KERNEL_SO),1)
$(OUTDIR)/$(SHAREDLIB): $(OBJDIR) $(OBJS)
	@echo Linking $(SHAREDLIB)...
	@$(LD) -shared $(LDFLAGS) -o $(OUTDIR)/$(SHAREDLIB) $(OBJS) $(LIBS)
endif

$(OBJDIR):
	@echo "Creating directory $@..."
	@$(MKDIR) $@

$(BINDIR)/$(INSTANTIATE): $(OUTDIR)/$(STATICLIB) $(INSTANTIATEOBJS)
	@echo Linking test program $@...
	@$(LD) $(LDFLAGS) -L$(OUTDIR) -o $@ $(INSTANTIATEOBJS) -Wl,-Bstatic -liptv_kernel -Wl,-Bdynamic $(LIBS)

$(BINDIR)/$(SHELLCLIENT): $(OUTDIR)/$(STATICLIB) $(SHELLCLIENTOBJS)
	@echo Linking test program $@...
	@$(LD) $(LDFLAGS) -L$(OUTDIR) -o $@ $(SHELLCLIENTOBJS) -Wl,-Bstatic -liptv_kernel -Wl,-Bdynamic $(LIBS)

clean:
	@echo Removing output files...
ifeq ($(MAKE_KERNEL_SO),1)
	@$(RM) $(OUTDIR)/$(SHAREDLIB)
endif
	@$(RM) $(OUTDIR)/$(STATICLIB) $(BINDIR)/$(INSTANTIATE) $(BINDIR)/$(SHELLCLIENT) $(OBJS) $(INSTANTIATEOBJS) $(SHELLCLIENTOBJS)
