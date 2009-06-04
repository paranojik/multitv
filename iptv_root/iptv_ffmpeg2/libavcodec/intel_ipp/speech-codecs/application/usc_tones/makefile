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
BinNameExe=usc_tones_$(CPU)$(EXESFX)
else
ifeq ($(ARCH), xscale_be)
BinNameExe=usc_tones_$(CPU)$(EXESFX)
else
BinNameExe=usc_tones$(EXESFX)
endif
endif
ObjDir=../../bin/$(ARCH)_$(COMP)/obj/usc_tones

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

CCDEFS=-D__unix__ -Dlinux -DNDEBUG 

ifeq ($(ARCH), linux32)
CCDEFS += -Dlinux32
else
ifeq ($(ARCH), linux64)
CCDEFS += -Dlinux64
else
ifeq ($(ARCH), linuxem64t)
CCDEFS += -Dlinux32e
else
ifeq ($(ARCH), iwmmxt_le)
CCDEFS += -D_USC_ARCH_XSC
else
ifeq ($(ARCH), xscale_be)
CCDEFS += -D_BIG_ENDIAN -D_USC_ARCH_XSC
endif
endif
endif
endif   
endif

CCINCLS=-I./include \
        -I"../../core/usc/include" $(IPPINCLS)


LNKOUT=-o
LNKOPTS=
LNKLIBS=-L$(LibDir) -lusc \
         $(IPPLIBS) -lpthread -lm

build: prepare_directory build_usc_lib build_exe; @echo "Done"

SRCDIRCOMMON =./src

OBJECT_USC_TONES=$(BinDir)/$(BinNameExe)

CPPSRC=$(wildcard $(SRCDIRCOMMON)/*$(CPPSFX))
OBJSRC=$(patsubst %$(CPPSFX), %$(OBJSFX), $(CPPSRC))

build_exe: $(OBJECT_USC_TONES) ; @echo "done USC_TONES"

$(OBJECT_USC_TONES): $(OBJSRC) 
	$(CC) -o $@ $(LNKOPTS) $(patsubst %.o, $(ObjDir)/%.o,$(notdir $^)) $(LNKLIBS)


OBJECTS=$(OBJSRC)
$(OBJECTS): %$(OBJSFX): %$(CPPSFX)
	$(CC) $(CCINCLS) $(CCDEFS) $(CCOPTS) -o $(ObjDir)/$(notdir $@) $<

prepare_directory: FORCE
	@echo "+++++++++++ preparing USC TONES"
	@if test ! -d $(BinDir); then mkdir -p $(BinDir); fi
	@if test ! -d $(ObjDir); then mkdir -p $(ObjDir); fi

build_usc_lib:
	cd ../../codec/speech; make ARCH=${ARCH} COMP=${COMP} CC=${CC} CXX=${CXX} IPPROOT=${IPPROOT} LIBx=${LIBx} build

clean: FORCE
	rm -f -d $(ObjDir)/*$(OBJSFX)
	rm -f -d $(BinDir)/$(BinNameExe)
FORCE:
    
