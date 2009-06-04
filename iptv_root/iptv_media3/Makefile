#Project: Media library
#Author : Daniel Chaves Toscano Dantas

CC              = g++
AR              = ar
ARFLAGS	        = rcs


OBJ_DIR         = ./obj

ifndef OPENAL
OPENAL          = ../openal-soft/include/AL
endif

ifndef OPENAL_LIB
OPENAL_LIB      = ../openal-soft/CMakeConf
endif

ifndef IPTV_MEDIA_UTIL
IPTV_MEDIA_UTIL = ../iptv_media_util
endif

ifndef IPTV_SHARED
IPTV_SHARED     = ../iptv_shared
endif

ifndef IPTV_FFMPEG
IPTV_FFMPEG     = ../iptv_ffmpeg2
endif

ifndef LUA_PATH
LUA_PATH        = $(IPTV_MEDIA_UTIL)/lua
endif


TEST_IPTV_MEDIA = ./test_iptv_media


DEFS            = -D_LINUX

INCLUDES        = -I. -I$(IPTV_MEDIA_UTIL) -I$(OPENAL) -I$(IPTV_SHARED) -I$(IPTV_SHARED)/shared -I$(IPTV_FFMPEG)/libavcodec -I$(IPTV_FFMPEG)/libavformat -I$(IPTV_FFMPEG)/libavutil 
CFLAGS          = $(INCLUDES) $(DEFS)
LDFLAGS         = -L./lib -L$(LUA_PATH)/lib -L$(IPTV_MEDIA_UTIL)/lib -L$(IPTV_SHARED)/shared/lib -L$(IPTV_FFMPEG)/libavcodec -L$(IPTV_FFMPEG)/libavformat -L$(IPTV_FFMPEG)/libavutil -L$(OPENAL_LIB)
LIBS            = -liptv_media -liptv_media_util -llua -llualib -lshared -lavcodec -lspeex -lx264 -lopenal -lavformat -lavutil -lz

ifeq ($(DEBUG),1)
CFLAGS += -g
else
CFLAGS += -O2 -fno-strict-aliasing
endif

LIB_IPTV_MEDIA_OBJS    = CAudio.o\
                         IAudioApi.o\
                         CBufferId.o\
                         CBufferMedia.o\
                         CClockSync.o\
                         CInputPkt.o\
                         CMedia.o \
                         CTransmission.o\
                         COpenALApi.o\
                         COpenALObjectApi.o\
                         COutputBuffer.o\
                         CPkt.o\
                         CVideo.o\
                         Global_functions.o\
                         IMediaStreaming.o \
                         AudioDecoder.o \
                         AudioEncoder.o \
                         VideoDecoder.o \
                         VideoEncoder.o \
                         MediaNetwork.o \
                         CodecConfig.o \
                         Codec.o \
                         Demuxer.o \
                         SimposiumNetwork.o \

LIB_IPTV_MEDIA_OUTPUT = ./lib/libiptv_media.a

#TEST_MEDIA_OBJS       = $(TEST_IPTV_MEDIA)/test_iptv_media_linux.o
#TEST_MEDIA_OUTPUT     = $(TEST_IPTV_MEDIA)/test_iptv_media_linux

%.o : %.cpp
	@echo Compiling $<...
	@$(CC) $(CFLAGS) -c $< -o $@

all: $(LIB_IPTV_MEDIA_OUTPUT)

$(LIB_IPTV_MEDIA_OUTPUT) : $(LIB_IPTV_MEDIA_OBJS)
	@echo Archiving $@...
	@$(AR) $(ARFLAGS) $@ $(LIB_IPTV_MEDIA_OBJS)

#$(TEST_MEDIA_OUTPUT) : $(TEST_MEDIA_OBJS) $(LIB_IPTV_MEDIA_OUTPUT)
#	@echo Linking $@...
#	@$(CC) $(LDFLAGS) $(TEST_MEDIA_OBJS) -o $@ $(LIBS)

clean:
	@echo Cleaning up...
	@rm -f $(LIB_IPTV_MEDIA_OBJS) $(LIB_IPTV_MEDIA_OUTPUT)

