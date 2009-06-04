## VBLib 7.0 Makefile
# Makefile for building library under Unix/GCC.
# Designed to work with GNU make, other 'make' tools may or may not work; 
# your mileage may vary.

## Tools definition ##
CXX = g++
LD = g++
AR = ar rcs
ECHO = echo
CP = cp -af
RM = rm -f
MKDIR = mkdir -p
FAKEROOT = fakeroot
DPKG_DEB = dpkg-deb

## Directories ##
# Source files
SRCDIR = ../../src
# Test program directory
TESTDIR = test
# Include files
INCLUDEDIR = ../../include
# Intermediate object files
OBJDIR = ../../obj
# Library output directory
LIBDIR = ../../lib/g++
# Binary (executable) directory
BINDIR = ../../bin
# Packages directory
PKGDIR = ../../packages
DEBDIR = debian
# Executable prefix
PREFIX = /usr/bin

## Definitions ##
# Base library name
BASELIBNAME = VBLib
# Library version
# LIBVERSION = 7_0
LIBMAJOR = 6
LIBMINOR = 2
LIBREVISION = 0
# Package version info
PKGVERSION = $(LIBMAJOR).$(LIBMINOR).$(LIBREVISION)
# Package names. Add others (RPM, etc.) as needed.
DEBPKGBASE = vblib-$(LIBMAJOR).$(LIBMINOR)
# Full library name
LIBNAME = $(BASELIBNAME)-$(LIBMAJOR).$(LIBMINOR)
# Shared object library name
LIBSONAME = lib$(LIBNAME).so

## Files ##
# Static library
STATICLIB = lib$(LIBNAME).a
# Shared (dynamic) library
SHAREDLIB = $(LIBSONAME).$(LIBREVISION)
# Object file list. NOTE: VBSerial is Windows-only at the moment
OBJS = VBBinaryData.o VBClock.o VBCrossDLL.o VBDate.o VBException.o \
VBFileDirectory.o VBLib.o VBMD5.o VBProcessArgs.o VBString.o
OBJFILES = $(foreach FILE, $(OBJS), $(OBJDIR)/$(BASELIBNAME)/$(FILE))
# Test program object files
TESTOBJS = test.o VBCrossDll_test.o
TESTOBJFILES = $(foreach FILE, $(TESTOBJS), $(OBJDIR)/$(TESTDIR)/$(FILE))
# Test program executable
TESTEXEC = test
# Libraries
LIBS = 
TESTLIBS = -l$(LIBNAME) -ldl -L$(LIBDIR)
# Debian package build structure archive
DEBARCHIVE = deb-build.tar.bz2
# Debian package file names
DEBPKG = $(DEBPKGBASE)_$(PKGVERSION)_$(DEB_ARCH).deb
# The dev package contains only the headers, so it's architecture-independent
DEBDEVPKG = $(DEBPKGBASE)-dev_$(PKGVERSION)_all.deb


## Flags ##
CXXFLAGS = -fPIC
LDFLAGS = -Wl,-soname,$(LIBSONAME)
TESTLDFLAGS = -rdynamic

# Architecture types (for building packages). Add others (RPM, etc.) as needed.
ifndef DEB_ARCH
	DEB_ARCH = i386
endif

# Debug mode. Do 'make DEBUG=1' to compile a debug build.
ifdef DEBUG
	CXXFLAGS += -g
else
	CXXFLAGS += -O3 -fno-strict-aliasing
	LDFLAGS += -Wl,--strip-all
endif

## Rules ##
# Define search paths for *.cpp files
VPATH = $(SRCDIR)/$(BASELIBNAME) $(SRCDIR)/$(TESTDIR)

# Compile
$(OBJDIR)/$(BASELIBNAME)/%.o $(OBJDIR)/$(TESTDIR)/%.o : %.cpp
	@$(ECHO) Compiling $<...
	@$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -I$(INCLUDEDIR)/$(TESTDIR) -c $< -o $@

# Build everything.
all: $(LIBDIR) $(LIBDIR)/$(SHAREDLIB) $(LIBDIR)/$(STATICLIB) $(BINDIR)/$(TESTEXEC)
	@$(ECHO) Build complete.

# Build shared library only
shared: $(LIBDIR) $(LIBDIR)/$(SHAREDLIB)
	@$(ECHO) Build complete.

# Build static library only
static: $(LIBDIR) $(LIBDIR)/$(STATICLIB)
	@$(ECHO) Build complete.

# Build test program only
test: $(BINDIR)/$(TESTEXEC)
	@$(ECHO) Build complete.

# Build Debian package
deb: $(PKGDIR)/$(DEBDIR)/$(DEBPKGBASE) $(PKGDIR)/$(DEBDIR)/$(DEBPKG) $(PKGDIR)/$(DEBDIR)/$(DEBDEVPKG)
	@$(ECHO) Packaging complete.

# object directories 
$(OBJDIR)/$(BASELIBNAME) $(OBJDIR)/$(TESTDIR) $(LIBDIR):
	@$(ECHO) Creating directory $@...
	@$(MKDIR) $@

# Static library
$(LIBDIR)/$(STATICLIB): $(OBJDIR)/$(BASELIBNAME) $(OBJFILES)
	@$(ECHO) Archiving $@...
	@$(AR) $@ $(OBJFILES)

# Shared (dynamic) library
$(LIBDIR)/$(SHAREDLIB): $(OBJDIR)/$(BASELIBNAME) $(OBJFILES)
	@$(ECHO) Linking $@...
	@$(LD) $(LDFLAGS) -shared -o $@ $(OBJFILES) $(LIBS) && chmod -x $@

# Test program
$(BINDIR)/$(TESTEXEC): $(OBJDIR)/$(TESTDIR) $(LIBDIR)/$(STATICLIB) $(TESTOBJFILES)
	@$(ECHO) Linking $@...
	@$(LD) $(TESTLDFLAGS) -o $@ $(TESTOBJFILES) $(TESTLIBS)

# Building Debian packages
# The build structure is saved in a tar.bz2 file, deb-build.tar.bz2.
# This is done in order to save everything into a version control repository
# (Subversion, CVS, etc) without having to worry about the version control tool's
# files going into the package.
$(PKGDIR)/$(DEBDIR)/$(DEBPKGBASE):
	@$(ECHO) Extracting package build structure...
	@tar -C $(PKGDIR)/$(DEBDIR) -xjf $(PKGDIR)/$(DEBDIR)/$(DEBARCHIVE)

# Debian binary package
$(PKGDIR)/$(DEBDIR)/$(DEBPKG): $(LIBDIR)/$(STATICLIB) $(LIBDIR)/$(SHAREDLIB)
	@$(ECHO) Building Debian binary package...
	@$(CP) $(LIBDIR)/$(STATICLIB) $(LIBDIR)/$(SHAREDLIB) $(PKGDIR)/$(DEBDIR)/$(DEBPKGBASE)/usr/lib
	@$(CP) $(PKGDIR)/$(DEBDIR)/control-files/$(DEBPKGBASE)/control.$(DEB_ARCH) $(PKGDIR)/$(DEBDIR)/$(DEBPKGBASE)/DEBIAN/control
	@if [ ! -x $(PREFIX)/$(FAKEROOT) ]; then \
		echo "ERROR: You must have '$(FAKEROOT)' installed in your system to build packages."; \
	elif [ ! -x $(PREFIX)/$(DPKG_DEB) ]; then \
		echo "ERROR: You must have '$(DPKG_DEB)' installed in your system to build packages."; \
	else \
		$(FAKEROOT) $(DPKG_DEB) --build $(PKGDIR)/$(DEBDIR)/$(DEBPKGBASE) $(PKGDIR)/$(DEBDIR); \
	fi

# Debian dev package
$(PKGDIR)/$(DEBDIR)/$(DEBDEVPKG): $(INCLUDEDIR)/$(BASELIBNAME)/$(wildcard *.h)
	@$(ECHO) Building Debian development package...
	@$(CP) $(INCLUDEDIR)/$(BASELIBNAME)/*.h $(PKGDIR)/$(DEBDIR)/$(DEBPKGBASE)-dev/usr/include/$(BASELIBNAME)
	@if [ ! -x $(PREFIX)/$(FAKEROOT) ]; then \
		echo "ERROR: You must have '$(FAKEROOT)' installed in your system to build packages."; \
	elif [ ! -x $(PREFIX)/$(DPKG_DEB) ]; then \
		echo "ERROR: You must have $(DPKG_DEB) installed in your system to build packages."; \
	else \
		$(FAKEROOT) $(DPKG_DEB) --build $(PKGDIR)/$(DEBDIR)/$(DEBPKGBASE)-dev $(PKGDIR)/$(DEBDIR); \
	fi

clean:
	@$(ECHO) Removing intermediate files...
	@$(RM) $(OBJFILES) $(TESTOBJFILES)
	@$(ECHO) Removing output files...
	@$(RM) $(LIBDIR)/$(STATICLIB) $(LIBDIR)/$(SHAREDLIB) $(BINDIR)/$(TESTEXEC)
	@$(RM) -r $(OBJDIR)
	@$(ECHO) Removing package build structure...
	@$(RM) -r $(PKGDIR)/$(DEBDIR)/control-files $(PKGDIR)/$(DEBDIR)/$(DEBPKGBASE) $(PKGDIR)/$(DEBDIR)/$(DEBPKGBASE)-dev
	@$(ECHO) Removing packages...
	@$(RM) $(PKGDIR)/$(DEBDIR)/$(DEBDEVPKG) $(PKGDIR)/$(DEBDIR)/$(DEBPKG)
	@$(ECHO) Done.
