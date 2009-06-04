###############################
# DO NOT EDIT BELOW THIS LINE #
###############################

# System Specific
INCLSFX=.h
CPPSFX=.c
OBJSFX=.o
EXESFX=

LIBSFX=.a

-include $(ARCH).ini

LibDir=../../bin/$(ARCH)_$(COMP)/lib
BinDir=../../bin/$(ARCH)_$(COMP)
ifeq ($(ARCH), iwmmxt_le)
BinNameExe=usc_speech_codec_$(CPU)$(EXESFX)
else
ifeq ($(ARCH), xscale_be)
BinNameExe=usc_speech_codec_$(CPU)$(EXESFX)
else
BinNameExe=usc_speech_codec$(EXESFX)
endif
endif
ObjDir=../../bin/$(ARCH)_$(COMP)/obj/usc_speech_codec

# IPP Dependencies
IPPINCLS=-I$(IPPROOT)/include

# Compiler Specifics
CCOPTS=-c -O2
ifeq ($(ARCH), iwmmxt_le)
CCOPTS += -mcpu=strongarm -Wno-multichar -msoft-float
else
ifeq ($(ARCH), xscale_be)
CCOPTS += -mcpu=strongarm -Wno-multichar -msoft-float
endif
endif

CCDEFS=-D__unix__ -Dlinux -DNDEBUG \
       -DLINUX32 #define for VM

ifeq ($(ARCH), linux32)
CCDEFS += -Dlinux32 -D_USC_ALL
else
ifeq ($(ARCH), linux64)
CCDEFS += -Dlinux64 -D_USC_ALL
else
ifeq ($(ARCH), linuxem64t)
CCDEFS += -Dlinux32e -D_USC_ALL
else
ifeq ($(ARCH), iwmmxt_le)
CCDEFS += -D_USC_AMRWB -D_USC_AMRWBE -D_USC_G711 -D_USC_G722 -D_USC_G722SB -D_USC_G723 \
          -D_USC_G726 -D_USC_G728 -D_USC_G729 -D_USC_GSMFR -D_USC_GSMAMR -D_USC_TIME_DRV
else
ifeq ($(ARCH), xscale_be)
CCDEFS += -D_USC_AMRWB -D_USC_AMRWBE -D_USC_G711 -D_USC_G722 -D_USC_G722SB -D_USC_G723 \
          -D_USC_G726 -D_USC_G728 -D_USC_G729 -D_USC_GSMFR -D_USC_GSMAMR -D_BIG_ENDIAN -D_USC_TIME_DRV
endif
endif
endif
endif
endif

CCINCLS=-I./include \
        -I"../../core/usc/amrwbe/include" \
        -I"../../core/usc/include" \
        -I"../../core/vm/include/sys" \
        -I"../../core/vm/include" $(IPPINCLS)

LNKOUT=-o
LNKOPTS=
LNKLIBS=-L$(LibDir) -lusc \
         $(IPPLIBS) -lpthread -lm

build: prepare_directory build_usc_lib build_exe; @echo "Done"

SRCDIRCOMMON =./src
SRCDIRVM =../../core/vm/src

OBJECT_USC_SPEECH_CODEC=$(BinDir)/$(BinNameExe)

CPPSRC_VM=$(wildcard $(SRCDIRVM)/vm_sys_info_linux32$(CPPSFX) $(SRCDIRVM)/vm_time_linux32$(CPPSFX))
OBJSRC_VM=$(patsubst %$(CPPSFX), %$(OBJSFX), $(CPPSRC_VM))

CPPSRC=$(wildcard $(SRCDIRCOMMON)/*$(CPPSFX))
OBJSRC=$(patsubst %$(CPPSFX), %$(OBJSFX), $(CPPSRC))

build_exe: $(OBJECT_USC_SPEECH_CODEC) ; @echo "done USC_SPEECH_CODEC"

$(OBJECT_USC_SPEECH_CODEC): $(OBJSRC) $(OBJSRC_VM)
	$(CC) -o $@ $(LNKOPTS) $(patsubst %.o, $(ObjDir)/%.o,$(notdir $^)) $(LNKLIBS)

OBJECTS=$(OBJSRC) $(OBJSRC_VM)
$(OBJECTS): %$(OBJSFX): %$(CPPSFX)
	$(CC) $(CCINCLS) $(CCDEFS) $(CCOPTS) -o $(ObjDir)/$(notdir $@) $<

prepare_directory: FORCE
	@echo "+++++++++++ preparing USC Speech Codec"
	@if test ! -d $(BinDir); then mkdir -p $(BinDir); fi
	@if test ! -d $(ObjDir); then mkdir -p $(ObjDir); fi

build_usc_lib:
	cd ../../codec/speech; make ARCH=${ARCH} COMP=${COMP} CC=${CC} CXX=${CXX} LIBx=${LIBx} IPPROOT=${IPPROOT} build

clean: FORCE
	rm -f -d $(ObjDir)/*$(OBJSFX)
	rm -f -d $(BinDir)/$(BinNameExe)
FORCE:
    
