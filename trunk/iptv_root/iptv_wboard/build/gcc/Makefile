# Makefile

CXX = g++
LD = g++
AR = ar


# Externals
ifndef IPTV_SHARED
IPTV_SHARED = ../../../iptv_shared
endif

# Paths and formation

INCLUDE_DIRS = $(IPTV_SHARED) $(IPTV_SHARED)/shared ../../include ../../include/iptv_wboard
SRC_DIR = ../../src
OBJ_DIR = ../../obj
OUT_DIR = ../../lib/gcc

OUT_FILE = libiptv_wboard.a

# Flags
CXXFLAGS = -Wall -O3 -fno-strict-aliasing `wx-config --cxxflags` -D_LINUX
ARFLAGS = rcs

# Include flags for CXX
INCLUDES = $(foreach dir,$(INCLUDE_DIRS),-I$(dir))

# Object file determination
OBJ_SRCS = $(patsubst %.cpp,%.o,$(wildcard $(SRC_DIR)/*.cpp))
OBJ_NAMES = $(notdir $(OBJ_SRCS))
OBJ_FILES = $(foreach file,$(OBJ_NAMES),$(OBJ_DIR)/$(file))

# Rules
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo "Compiling $(notdir $<)..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

all: $(OUT_DIR)/$(OUT_FILE)

$(OUT_DIR)/$(OUT_FILE) : $(OBJ_FILES)
	@echo "Archiving $(OUT_FILE)..."
	@$(AR) $(ARFLAGS) $(OUT_DIR)/$(OUT_FILE) $(OBJ_FILES)

clean:
	-rm -f $(OUT_DIR)/$(OUT_FILE) $(OBJ_FILES)

debug-makefile:
	@echo $(OBJ_SRCS)
	@echo $(OBJ_FILES)