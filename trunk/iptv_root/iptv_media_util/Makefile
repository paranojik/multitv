#Project: Media utility library
#Author : Daniel Chaves Toscano Dantas

ifndef (IPTV_FFMPEG)
IPTV_FFMPEG  = ../iptv_ffmpeg2
endif

ifndef (IPTV_SHARED)
IPTV_SHARED  = ../iptv_shared
endif

LUA_PATH = ./lua

CC           = gcc
CXX          = g++
AR           = ar
ARFLAGS	     = rcs
DEFS         = -D_LINUX
CFLAGS       = -Wall -O2 -I. -I$(IPTV_SHARED) -I$(IPTV_SHARED)/shared -I$(LUA_PATH)/include -I$(IPTV_FFMPEG)/libavcodec -I$(IPTV_FFMPEG)/libavutil

LIB_MEDIA_UTIL_OBJS = CFrame.o \
                      CVideoFrame.o \
                      CAudioFrame.o \
                      pixel_conversion.o \
                      mjpeg_decoder.o \
                      CLinuxFileLog.o \
                      IFileLog.o \
                      media_utilities.o \
		              dscscript.o \
                      dscstring.o \
                      parseString.o \
                      streamingBuffer.o \
                      ASFDataObject.o \
                      ASFDataPacket.o \
                      ASFFileSourceStream.o \
                      ASFHeader.o \
                      ASFObject.o \
                      ASFSimpleIndexObject.o \
                      ASFStream.o \
                      BufManager.o \
                      EncodedBuffer.o \
                      FileStream.o \
                      INSSBuffer.o \
                      ASFPktSink.o
                 
LIB_MEDIA_UTIL_OUTPUT = ./lib/libiptv_media_util.a

%.o : %.cpp
	@echo Compiling $<...
	@$(CXX) $(CFLAGS) $(DEFS) -c $< -o $@

%.o : %.c
	@echo Compiling $<...
	@$(CC) -c $< -o $@


all: $(LIB_MEDIA_UTIL_OUTPUT)

$(LIB_MEDIA_UTIL_OUTPUT) : $(LIB_MEDIA_UTIL_OBJS) 
	@echo Linking $(LIB_MEDIA_UTIL_OUTPUT) 
	@$(AR) $(ARFLAGS) $(LIB_MEDIA_UTIL_OUTPUT) $(LIB_MEDIA_UTIL_OBJS)

clean:
	rm -f $(LIB_MEDIA_UTIL_OUTPUT) $(LIB_MEDIA_UTIL_OBJS)

