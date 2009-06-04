###############################
# DO NOT EDIT BELOW THIS LINE #
###############################

# System Specific
INCLSFX=.h
CPPSFX=.c
OBJSFX=.o

LIBSFX=.a

LibDir=../../bin/$(ARCH)_$(COMP)/lib
LibNameUSC=usc
ObjDir=../../bin/$(ARCH)_$(COMP)/obj/usc

IPPROOT = /opt/intel/ipp/5.3.3.075/ia32
LIBx    = ar

# IPP Dependencies
IPPINCLS=-I$(IPPROOT)/include

# Compiler Specifics
CCOPTS = -c -O2

ifeq ($(ARCH), iwmmxt_le)
CCOPTS += -mcpu=strongarm -Wno-multichar -msoft-float
else
ifeq ($(ARCH), xscale_be)
CCOPTS += -mcpu=strongarm -Wno-multichar -msoft-float
endif
endif

CCDEFS = -D__unix__ -DNDEBUG
#for scratchmem.h#
ifeq ($(ARCH), linux32)
CCDEFS +=-Dlinux32
else
ifeq ($(ARCH), linux64)
CCDEFS +=-Dlinux64
else
ifeq ($(ARCH), linuxem64t)
CCDEFS +=-Dlinux32e
else    	
ifeq ($(ARCH), iwmmxt_le)
CCDEFS +=-Dlinux32 -D_USC_ARCH_XSC
else
ifeq ($(ARCH), xscale_be)
CCDEFS +=-Dlinux32 -D_USC_ARCH_XSC
endif
endif
endif
endif
endif
ifeq ($(ARCH), xscale_be)
CCDEFS +=-D_IPP_BE
endif

CCINCLS=-I./amrwb/include \
        -I./amrwbe/include \
        -I./g711/include \
        -I./g722/include \
        -I./g722.1/include \
        -I./g723.1/include \
        -I./g728/include \
        -I./g729fp/include \
        -I./g729i/include \
        -I./gsmamr/include \
        -I./gsmfr/include \
        -I./ec_int/include \
        -I./nr_int/include \
        -I./common/include \
        -I./vad/include \
        -I./vad_sr/include \
        -I../../core/usc/include $(IPPINCLS)


LNKOUT=-o
LNKOPTS=
LNKLIBS=


build: prepare_directory build_libusc ; @echo "Done"

SRCDIRLIBamrwb =./amrwb/src
SRCDIRLIBamrwbe=./amrwbe/src
SRCDIRLIBg711  =./g711/src
SRCDIRLIBg722  =./g722/src
SRCDIRLIBg7221 =./g722.1/src
SRCDIRLIBg7231 =./g723.1/src
SRCDIRLIBg726  =./g726/src
SRCDIRLIBg728  =./g728/src
SRCDIRLIBg729fp  =./g729fp/src
SRCDIRLIBg729i =./g729i/src
SRCDIRLIBgsmamr=./gsmamr/src
SRCDIRLIBgsmfr =./gsmfr/src
SRCDIRLIBec_int =./ec_int/src
SRCDIRLIBnr_int =./nr_int/src
SRCDIRLIBtd =./td/src
SRCDIRLIBvad =./vad/src
SRCDIRLIBvad_sr =./vad_sr/src
SRCDIRLIButil =./common/src
SRCUSCFCN = .

##### lib usc object #####
ifeq ($(ARCH), iwmmxt_le)
CPPSRC_LIBUSC=$(wildcard $(SRCDIRLIBamrwb)/*$(CPPSFX) \
                         $(SRCDIRLIBamrwbe)/*$(CPPSFX) \
                         $(SRCDIRLIBg711)/*$(CPPSFX) \
                         $(SRCDIRLIBg722)/*$(CPPSFX) \
                         $(SRCDIRLIBg7221)/*$(CPPSFX) \
                         $(SRCDIRLIBg7231)/*$(CPPSFX) \
                         $(SRCDIRLIBg726)/*$(CPPSFX) \
                         $(SRCDIRLIBg728)/*$(CPPSFX) \
                         $(SRCDIRLIBg729i)/*$(CPPSFX) \
                         $(SRCDIRLIBgsmamr)/*$(CPPSFX) \
                         $(SRCDIRLIBgsmfr)/*$(CPPSFX) \
                         $(SRCDIRLIBnr_int)/*$(CPPSFX) \
                         $(SRCDIRLIBtd)/*$(CPPSFX) \
                         $(SRCDIRLIBvad)/uscvadg723$(CPPSFX) $(SRCDIRLIBvad)/uscvadg729i$(CPPSFX) $(SRCDIRLIBvad)/vadg723$(CPPSFX) $(SRCDIRLIBvad)/vadg729$(CPPSFX)\
                         $(SRCDIRLIBvad_sr)/*$(CPPSFX) \
                         $(SRCDIRLIButil)/*$(CPPSFX) $(SRCUSCFCN)/*$(CPPSFX))
else
ifeq ($(ARCH), xscale_be)
CPPSRC_LIBUSC=$(wildcard $(SRCDIRLIBamrwb)/*$(CPPSFX) \
                         $(SRCDIRLIBamrwbe)/*$(CPPSFX) \
                         $(SRCDIRLIBg711)/*$(CPPSFX) \
                         $(SRCDIRLIBg722)/*$(CPPSFX) \
                         $(SRCDIRLIBg7221)/*$(CPPSFX) \
                         $(SRCDIRLIBg7231)/*$(CPPSFX) \
                         $(SRCDIRLIBg726)/*$(CPPSFX) \
                         $(SRCDIRLIBg728)/*$(CPPSFX) \
                         $(SRCDIRLIBg729i)/*$(CPPSFX) \
                         $(SRCDIRLIBgsmamr)/*$(CPPSFX) \
                         $(SRCDIRLIBgsmfr)/*$(CPPSFX) \
                         $(SRCDIRLIBnr_int)/*$(CPPSFX) \
                         $(SRCDIRLIBtd)/*$(CPPSFX) \
                         $(SRCDIRLIBvad)/uscvadg723$(CPPSFX) $(SRCDIRLIBvad)/uscvadg729i$(CPPSFX) $(SRCDIRLIBvad)/vadg723$(CPPSFX) $(SRCDIRLIBvad)/vadg729$(CPPSFX) \
                         $(SRCDIRLIBvad_sr)/*$(CPPSFX) \
                         $(SRCDIRLIButil)/*$(CPPSFX) $(SRCUSCFCN)/*$(CPPSFX))

else
CPPSRC_LIBUSC=$(wildcard $(SRCDIRLIBamrwb)/*$(CPPSFX) \
                         $(SRCDIRLIBamrwbe)/*$(CPPSFX) \
                         $(SRCDIRLIBg711)/*$(CPPSFX) \
                         $(SRCDIRLIBg722)/*$(CPPSFX) \
                         $(SRCDIRLIBg7221)/*$(CPPSFX) \
                         $(SRCDIRLIBg7231)/*$(CPPSFX) \
                         $(SRCDIRLIBg726)/*$(CPPSFX) \
                         $(SRCDIRLIBg728)/*$(CPPSFX) \
                         $(SRCDIRLIBg729fp)/*$(CPPSFX) \
                         $(SRCDIRLIBg729i)/*$(CPPSFX) \
                         $(SRCDIRLIBgsmamr)/*$(CPPSFX) \
                         $(SRCDIRLIBgsmfr)/*$(CPPSFX) \
                         $(SRCDIRLIBec_int)/*$(CPPSFX) \
                         $(SRCDIRLIBnr_int)/*$(CPPSFX) \
                         $(SRCDIRLIBtd)/*$(CPPSFX) \
                         $(SRCDIRLIBvad)/*$(CPPSFX) \
                         $(SRCDIRLIBvad_sr)/*$(CPPSFX) \
                         $(SRCDIRLIButil)/*$(CPPSFX) $(SRCUSCFCN)/*$(CPPSFX))

endif
endif



OBJECT_LIBUSC=$(patsubst %$(CPPSFX), %$(OBJSFX), $(CPPSRC_LIBUSC))


##### build lib usc #####
LIB_USC=$(LibDir)/lib$(LibNameUSC)$(LIBSFX) 
build_libusc: $(LIB_USC) ; @echo "done lib usc"
$(LIB_USC): $(OBJECT_LIBUSC)
	$(LIBx) -rs $@ $(patsubst %.o, $(ObjDir)/%.o,$(notdir $^))
#	$(LIBx) -rs $@ $^


OBJECTS=$(OBJECT_LIBUSC)
$(OBJECTS): %$(OBJSFX): %$(CPPSFX)
	$(CC) $(CCINCLS) $(CCDEFS) $(CCOPTS) -o $(ObjDir)/$(notdir $@) $<
#	$(CC) $(CCINCLS) $(CCDEFS) $(CCOPTS) -o$@ $<

prepare_directory: FORCE
	@if test ! -d $(LibDir); then mkdir -p $(LibDir); fi
	@if test ! -d $(ObjDir); then mkdir -p $(ObjDir); fi

clean: FORCE
	rm -f -d $(ObjDir)/*$(OBJSFX)
	rm -f -d $(LibDir)/lib$(LibNameUSC)$(LIBSFX)
	@echo "removing rm -f -d $(LibDir)/lib$(LibNameUSC)$(LIBSFX) "
FORCE:
    
