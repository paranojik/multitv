#Project: Linux Webcam Capture Framework
#Author : Daniel Chaves Toscano Dantas

FFMPEG             = ../../../iptv_ffmpeg2
IPTV_SHARED        = ../../../iptv_shared
MEDIA_UTIL         = ../../../iptv_media_util
TEST_IPTV_CAPTURE  = ./test

CC          = g++
LD          = g++
AR          = ar
ARFLAGS	   = cr
DEFS        = -D_LINUX
#GTK_CFLAGS  = `pkg-config --cflags gtk+-2.0`
GTK_CFLAGS  = 
GTK_LDFLAGS = `pkg-config --libs   gtk+-2.0`
CFLAGS      = -Wall -O2 $(GTK_CFLAGS) -I. -I../../../ -I../../include/ -I$(FFMPEG)/libavcodec -I$(FFMPEG)/libavutil
LDFLAGS     =  -L../../lib/g++ -L$(MEDIA_UTIL)/lib -L$(IPTV_SHARED)/shared/lib -L$(FFMPEG)/linux-prebuilt

LIB_CAPTURE_OBJS_FILES = VideoCapture.o CamCaptureLinux.o ScreenCaptureLinux.o
LIB_CAPTURE_OBJS_DIR   = ../../obj/gcc/
LIB_CAPTURE_OBJS       = $(foreach FILE, $(LIB_CAPTURE_OBJS_FILES), $(LIB_CAPTURE_OBJS_DIR)$(FILE))
LIB_CAPTURE_OUTPUT     = libiptv_video_capture.a
LIB_CAPTURE_OUTPUT_DIR = ../../lib/gcc/
LIB_CAPTURE_SRC_DIR    = ../../src/
TEST_CAPTURE_OBJS      = $(TEST_IPTV_CAPTURE)/main.o $(TEST_IPTV_CAPTURE)/gui.o
TEST_CAPTURE_OUTPUT    = $(TEST_IPTV_CAPTURE)/testCapture

%.o: $(LIB_CAPTURE_SRC_DIR)%.cpp
	@echo Compiling $<...
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $(LIB_CAPTURE_OBJS_DIR)$@

all: init video_capture_lib

init:
	mkdir -p $(LIB_CAPTURE_OBJS_DIR)
	mkdir -p $(LIB_CAPTURE_OUTPUT_DIR)

video_capture_lib: init $(LIB_CAPTURE_OBJS)
	@echo Linking $(LIB_CAPTURE_OUTPUT)
	$(AR) $(ARFLAGS) $(LIB_CAPTURE_OUTPUT_DIR)$(LIB_CAPTURE_OUTPUT) $(LIB_CAPTURE_OBJS)

video_capture_test: video_capture_lib $(TEST_CAPTURE_OBJS)
	@echo Linking $(TEST_CAPTURE_OUTPUT) 
	$(LD) $(LDFLAGS) $(TEST_CAPTURE_OBJS) -o $(TEST_CAPTURE_OUTPUT) -liptv_video_capture -lshared -liptv_media_util -lavcodec -lavutil -lX11 $(GTK_LDFLAGS)

clean:
	rm -f $(LIB_CAPTURE_OUTPUT) $(TEST_CAPTURE_OUTPUT) $(TEST_CAPTURE_OBJS)
	rm -f $(LIB_CAPTURE_OBJS_DIR)*
