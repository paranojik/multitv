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
BinNameExe=usc_nr_$(CPU)$(EXESFX)
else
ifeq ($(ARCH), xscale_be)
BinNameExe=usc_nr_$(CPU)$(EXESFX)
else
BinNameExe=usc_nr$(EXESFX)
endif
endif
ObjDir=../../bin/$(ARCH)_$(COMP)/obj/usc_nr

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
CCDEFS += -D_USC_ARCH_XSC -D_USC_EPHM_NR \
-D_USC_G729_VAD -D_USC_G723_VAD	-D_USC_INTEL_VAD 

else
ifeq ($(ARCH), xscale_be)
CCDEFS += -D_BIG_ENDIAN -D_USC_ARCH_XSC -D_USC_EPHM_NR \
-D_USC_G729_VAD -D_USC_G723_VAD	-D_USC_INTEL_VAD 

endif
endif
endif
endif   
endif

CCINCLS=-I./include \
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

OBJECT_USC_NR=$(BinDir)/$(BinNameExe)

CPPSRC_VM=$(wildcard $(SRCDIRVM)/vm_sys_info_linux32$(CPPSFX) $(SRCDIRVM)/vm_time_linux32$(CPPSFX))
OBJSRC_VM=$(patsubst %$(CPPSFX), %$(OBJSFX), $(CPPSRC_VM))

CPPSRC=$(wildcard $(SRCDIRCOMMON)/*$(CPPSFX))
OBJSRC=$(patsubst %$(CPPSFX), %$(OBJSFX), $(CPPSRC))

build_exe: $(OBJECT_USC_NR) ; @echo "done USC_NR"

$(OBJECT_USC_NR): $(OBJSRC) $(OBJSRC_VM)
	$(CC) -o $@ $(LNKOPTS) $(patsubst %.o, $(ObjDir)/%.o,$(notdir $^)) $(LNKLIBS)


OBJECTS=$(OBJSRC) $(OBJSRC_VM)
$(OBJECTS): %$(OBJSFX): %$(CPPSFX)
	$(CC) $(CCINCLS) $(CCDEFS) $(CCOPTS) -o $(ObjDir)/$(notdir $@) $<

prepare_directory: FORCE
	@echo "+++++++++++ preparing USC NR"
	@if test ! -d $(BinDir); then mkdir -p $(BinDir); fi
	@if test ! -d $(ObjDir); then mkdir -p $(ObjDir); fi

build_usc_lib:
	cd ../../codec/speech; make ARCH=${ARCH} COMP=${COMP} CC=${CC} CXX=${CXX} LIBx=${LIBx} IPPROOT=${IPPROOT} build

clean: FORCE
	rm -f -d $(ObjDir)/*$(OBJSFX)
	rm -f -d $(BinDir)/$(BinNameExe)
FORCE:
    
