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
BinNameExe=usc_ec_$(CPU)$(EXESFX)
else
ifeq ($(ARCH), xscale_be)
BinNameExe=usc_ec_$(CPU)$(EXESFX)
else
BinNameExe=usc_ec$(EXESFX)
endif
endif
ObjDir=../../bin/$(ARCH)_$(COMP)/obj/usc_ec

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
CCDEFS += -Dlinux32  -D_USC_EC_ALL
else
ifeq ($(ARCH), linux64)
CCDEFS += -Dlinux64 -D_USC_EC_ALL
else
ifeq ($(ARCH), linuxem64t)
CCDEFS += -Dlinux32e -D_USC_EC_ALL
else
ifeq ($(ARCH), iwmmxt_le)
CCDEFS += -D_USC_ARCH_XSC -D_USC_EC_INT
else
ifeq ($(ARCH), xscale_be)
CCDEFS += -D_BIG_ENDIAN -D_USC_ARCH_XSC -D_USC_EC_INT
endif
endif
endif
endif   
endif

CCINCLS=-I./include \
        -I"../../core/usc/include" \
        -I"../../core/usc/ec_int/include" \
        -I"../../core/vm/include/sys" \
        -I"../../core/vm/include" $(IPPINCLS)


LNKOUT=-o
LNKOPTS=
LNKLIBS=-L$(LibDir) -lusc \
         $(IPPLIBS) -lpthread -lm

build: prepare_directory build_usc_lib build_exe; @echo "Done"

SRCDIRCOMMON =./src
SRCDIRVM =../../core/vm/src

OBJECT_USC_EC=$(BinDir)/$(BinNameExe)

CPPSRC_VM=$(wildcard $(SRCDIRVM)/vm_sys_info_linux32$(CPPSFX) $(SRCDIRVM)/vm_file$(CPPSFX) $(SRCDIRVM)/vm_file_unix$(CPPSFX) $(SRCDIRVM)/vm_mutex_linux32$(CPPSFX))
OBJSRC_VM=$(patsubst %$(CPPSFX), %$(OBJSFX), $(CPPSRC_VM))

CPPSRC=$(wildcard $(SRCDIRCOMMON)/*$(CPPSFX))
OBJSRC=$(patsubst %$(CPPSFX), %$(OBJSFX), $(CPPSRC))

build_exe: $(OBJECT_USC_EC) ; @echo "done USC_EC"

$(OBJECT_USC_EC): $(OBJSRC) $(OBJSRC_VM)
	$(CC) -o $@ $(LNKOPTS) $(patsubst %.o, $(ObjDir)/%.o,$(notdir $^)) $(LNKLIBS)


OBJECTS=$(OBJSRC) $(OBJSRC_VM)
$(OBJECTS): %$(OBJSFX): %$(CPPSFX)
	$(CC) $(CCINCLS) $(CCDEFS) $(CCOPTS) -o $(ObjDir)/$(notdir $@) $<

prepare_directory: FORCE
	@echo "+++++++++++ preparing USC EC"
	@if test ! -d $(BinDir); then mkdir -p $(BinDir); fi
	@if test ! -d $(ObjDir); then mkdir -p $(ObjDir); fi

build_usc_lib:
	cd ../../codec/speech; make ARCH=${ARCH} COMP=${COMP} CC=${CC} CXX=${CXX} IPPROOT=${IPPROOT} LIBx=$(LIBx) build

clean: FORCE
	rm -f -d $(ObjDir)/*$(OBJSFX)
	rm -f -d $(BinDir)/$(BinNameExe)
FORCE:
    
