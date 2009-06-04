# Top-level makefile
# Recursively builds targets in other directories

# Directories
LIBDIR = ../../lib/g++
BINDIR = ../../bin
SRCDIR = ../../src

TARGETS = $(LIBDIR)/libskin.a  $(BINDIR)/iptv_client

all: $(TARGETS)

$(LIBDIR)/libskin.a: $(wildcard $(SRCDIR)/Skin/*.cpp)
	$(MAKE) -C $(SRCDIR)/Skin

$(BINDIR)/iptv_client: $(LIBDIR)/libskin.a $(wildcard $(SRCDIR)/iptv_client/*.cpp)
	$(MAKE) -C $(SRCDIR)/iptv_client

clean:
	$(MAKE) -C $(SRCDIR)/iptv_client clean
	$(MAKE) -C $(SRCDIR)/Skin clean
