#Project: Audio capture library
#Author : Daniel Chaves Toscano Dantas

CC          = g++
AR          = ar
ARFLAGS	    = rcs

DEFS        = -D_LINUX

# External dependencies
ifndef OPENAL
OPENAL      = ../openal-soft/include/AL
endif

ifndef OPENAL_LIB
OPENAL_LIB = ../openal-soft/CMakeConf
endif

ifndef IPTV_MEDIA_UTIL
IPTV_MEDIA_UTIL  = ../iptv_media_util
endif

ifndef IPTV_SHARED
IPTV_SHARED      = ../iptv_shared
endif

TEST_IPTV_AUDIO_CAPTURE = ./test_iptv_audio_capture

INCLUDES        = -I. -I$(IPTV_MEDIA_UTIL) -I$(OPENAL) -I$(IPTV_SHARED) -I$(IPTV_SHARED)/shared
CFLAGS          = $(INCLUDES) $(DEFS)

#dinamic version
# LDFLAGS         = -L./lib -L$(IPTV_MEDIA_UTIL)/lib -L$(IPTV_SHARED)/shared/lib
#static version
LDFLAGS         = -L./lib -L$(IPTV_MEDIA_UTIL)/lib -L$(IPTV_SHARED)/shared/lib

ifeq ($(DEBUG),1)
CFLAGS += -g
else
CFLAGS += -O2 -fno-strict-aliasing
endif

LIB_IPTV_AUDIO_CAPTURE_OBJS    = AudioCapture.o
LIB_IPTV_AUDIO_CAPTURE_OUTPUT  = ./lib/libiptv_audio_capture.a

TEST_IPTV_AUDIO_CAPTURE_OBJS   = $(TEST_IPTV_AUDIO_CAPTURE)/main.o
TEST_IPTV_AUDIO_CAPTURE_OUTPUT = $(TEST_IPTV_AUDIO_CAPTURE)/test_iptv_audio_capture

%.o : %.cpp
	@echo Compiling $<...
	$(CC) $(CFLAGS) -c $< -o $@

all: $(LIB_IPTV_AUDIO_CAPTURE_OUTPUT) $(TEST_IPTV_AUDIO_CAPTURE_OUTPUT)

$(LIB_IPTV_AUDIO_CAPTURE_OUTPUT) : $(LIB_IPTV_AUDIO_CAPTURE_OBJS)
	@echo Archiving $(LIB_IPTV_AUDIO_CAPTURE_OUTPUT) 
	$(AR) $(ARFLAGS) $(LIB_IPTV_AUDIO_CAPTURE_OUTPUT) $(LIB_IPTV_AUDIO_CAPTURE_OBJS)

$(TEST_IPTV_AUDIO_CAPTURE_OUTPUT) : $(TEST_IPTV_AUDIO_CAPTURE_OBJS)
	@echo Linking $(TEST_IPTV_AUDIO_CAPTURE_OUTPUT) 
#dinamic version
#	$(CC) $(LDFLAGS) $(TEST_IPTV_AUDIO_CAPTURE_OBJS) -o $(TEST_IPTV_AUDIO_CAPTURE_OUTPUT) -liptv_media_util -lshared -lopenal -liptv_audio_capture
#static version
	$(CC) $(LDFLAGS) $(TEST_IPTV_AUDIO_CAPTURE_OBJS) -o $(TEST_IPTV_AUDIO_CAPTURE_OUTPUT) $(OPENAL_LIB)/libopenal.a -liptv_media_util -liptv_audio_capture -lshared -ldl -pthread

clean:
	@echo Cleaning up...
	rm -f $(LIB_IPTV_AUDIO_CAPTURE_OBJS) $(LIB_IPTV_AUDIO_CAPTURE_OUTPUT) $(TEST_IPTV_AUDIO_CAPTURE_OBJS) $(TEST_IPTV_AUDIO_CAPTURE_OUTPUT)

