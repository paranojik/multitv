# Makefile for iptv_xfiletransfer
# Date: 2008-05-08 14:36
# Author: Andre Cotrim
#

# Tools
CXX = g++
AR = ar

# External dependencies
ifndef (IPTV_SHARED)
IPTV_SHARED = ../../iptv_shared
endif

# Input directories
INCLUDES = $(IPTV_SHARED) $(IPTV_SHARED)/shared

# Flags
INCLUDEFLAGS = $(foreach dir,$(INCLUDES), -I$(dir))
CXXFLAGS = $(INCLUDEFLAGS) -D_LINUX -O3 -fno-strict-aliasing
ARFLAGS = rcs

# Files
OBJS = XFileTransfer.o XFileTransferDll.o
OUTLIB = lib/libiptv_xfiletransfer.a

# Implicit rule
.cpp.o:
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(OUTLIB)

$(OUTLIB): $(OBJS)
	@echo "Creating library $@..."
	@$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	@echo "Cleaning up..."
	@rm -f $(OBJS) $(OUTLIB)

