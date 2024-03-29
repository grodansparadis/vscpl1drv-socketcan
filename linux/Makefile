# =========================================================================
#                      Autogenerated from Makefile.in
# =========================================================================

# Package version
MAJOR_VERSION=1
MINOR_VERSION=1
RELEASE_VERSION=2
BUILD_VERSION=1
PACKAGE_VERSION=1.1.2

# STATIC = yes if a static build is requested.
STATIC=no
VSCP_PROJ_BASE_DIR=/var/lib/vscp
DESTDIR=/var/lib/vscp

prefix = /usr/local
exec_prefix = ${prefix}
INSTALL = /usr/bin/install -c
top_srcdir = ..
top_builddir = ..

CC = gcc
CXX = g++

WARNINGS_HARD = -Werror -Wall -Wextra -pedantic -Wcast-align -Wcast-qual \
  -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self \
  -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast \
  -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo \
  -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Wno-unused \
  -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option
WARNINGS_MEDIUM = -Wformat-security -Wformat=2 -Wmissing-include-dirs \
  -Wno-parentheses -Wformat=2 -Wlogical-op
WARNINGS = -Wall

VERSION_DEFS = -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) \
	-DRELEASE_VERSION=$(RELEASE_VERSION) -DBUILD_VERSION=$(BUILD_VERSION)

CFLAGS = -std=c99  -g -O0 ${WARNINGS} ${VERSION_DEFS} -fPIC -DCBC \
	-I../vscp/src/vscp/common \
	-I../vscp/src/common
CXXFLAGS = -D__LINUX__ ${VERSION_DEFS} -fPIC ${WARNINGS} -D_REENTRANT  -g -O0
CPPFLAGS = -D__LINUX__ ${VERSION_DEFS} -fPIC ${WARNINGS}  \
	-fno-var-tracking-assignments \
	-Iinclude \
	-I../vscp/src/vscp/common \
	-I../vscp/src/common

LDFLAGS = 
LIBS = -lm -lpthread -ldl 
EXTRALIBS = 

DLFLAGS = -g -shared -fPIC -Wl,--no-undefined
STRIP = strip
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_DIR = /usr/bin/install -c -d
srcdir = .
top_srcdir = .
top_builddir =
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
datadir = ${prefix}/share
includedir = ${prefix}/include
DLLPREFIX = lib

//LIB_PLUS_VER = "vscpl1drv-socketcan.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(RELEASE_VERSION)"
LIB_PLUS_VER = "vscpl1drv-socketcan.so"
LIB_SONAME = "vscpl1drv-socketcan.so.$(MAJOR_VERSION)"

### Variables: ###


CANALSOCKETCAN_OBJECTS = vscpl1drv-socketcan.o\
	socketcanobj.o\
	dllist.o

### Targets: ###

all: $(LIB_PLUS_VER) ../man/vscpl1drv-socketcan.1

$(LIB_PLUS_VER): $(CANALSOCKETCAN_OBJECTS)
	$(CXX) -Wl,-soname,$(LIB_SONAME) -o $@ $(CANALSOCKETCAN_OBJECTS) $(DLFLAGS) -lm -lpthread -ldl  $(EXTRALIBS)
	ar rcs libvscpl1drv-socketcan.a $(CANALSOCKETCAN_OBJECTS)

vscpl1drv-socketcan.o: vscpl1drv-socketcan.cpp vscpl1drv-socketcan.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c vscpl1drv-socketcan.cpp -o $@

socketcanobj.o: socketcanobj.cpp socketcanobj.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c socketcanobj.cpp -o $@

guid.o: ../vscp/src/vscp/common/guid.cpp ../vscp/src/vscp/common/guid.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c ../vscp/src/vscp/common/guid.cpp -o $@

dllist.o: ../vscp/src/common/dllist.c ../vscp/src/common/dllist.h
	$(CC) $(CFLAGS) -c ../vscp/src/common/dllist.c -o $@

../man/vscpl1drv-socketcan.1: ../man/vscpl1drv-socketcan.1.md
	pandoc -s -t man ../man/vscpl1drv-socketcan.1.md -o ../man/vscpl1drv-can4vscp.1

#test:  test.o $(CANALSOCKETCAN_OBJECTS)
#	$(CXX) -o $@ test.o $(CANALSOCKETCAN_OBJECTS) $(LDFLAGS) -lm -lpthread -ldl  $(EXTRALIBS)

install: all
	$(INSTALL_DIR) $(DESTDIR)/drivers/level1
	$(STRIP) $(LIB_PLUS_VER)
	$(INSTALL_PROGRAM) $(LIB_PLUS_VER) $(DESTDIR)/drivers/level1
#	ln -fs /drivers/level1/$(LIB_PLUS_VER) $(DESTDIR)/drivers/level1/vscpl1drv-socketcan.so
#	ln -fs /drivers/level1/$(LIB_PLUS_VER) $(DESTDIR)/drivers/level1/vscpl1drv-socketcan.so.$(MAJOR_VERSION)
#	ln -fs /drivers/level1/$(LIB_PLUS_VER) $(DESTDIR)/drivers/level1/vscpl1drv-socketcan.so.$(MAJOR_VERSION).$(MINOR_VERSION)
#	$(INSTALL_PROGRAM) libvscpl1drv-socketcan.a $(DESTDIR)/drivers/level1

uninstall: all
	echo $(LIB_PLUS_VER)
	rm -rf $(DESTDIR)/drivers/level1/$(LIB_PLUS_VER)
#	rm -rf $(DESTDIR)/drivers/level1/vscpl1drv-socketcan.so
#	rm -rf $(DESTDIR)/drivers/level1/vscpl1drv-socketcan.so.$(MAJOR_VERSION)
#	rm -rf $(DESTDIR)/drivers/level1/vscpl1drv-socketcan.so.$(MAJOR_VERSION).$(MINOR_VERSION)
#	rm -rf $(DESTDIR)/drivers/level1/libvscpl1drv-socketcan.a

install-strip: install

man: ../man/vscpl1drv-socketcan.1.md
	pandoc -s -t man ../man/vscpl1drv-socketcan.1.md -o ../man/vscpl1drv-socketcan.1


deb:    all
	echo "\n" | ./builddist.sh
	@mkdir -p ../dist
	@mv *.deb ../dist

clean:
	rm -rf ./.deps ./.pch
	rm -f ./*.o
	rm -f ./*.so
	rm -f ./*.a
	rm -f $(LIB_PLUS_VER)
	rm -f test
	rm -f ../vscp/src/vscp/common/*.o
	rm -f ../vscp/src/common/*.o
	rm -rf *~

distclean: clean
	rm -f config.cache config.log config.status bk-deps bk-make-pch shared-ld-sh Makefile

$(ALWAYS_BUILD):  .FORCE

.FORCE:

# Include dependency info, if present:
-include .deps/*.d

.PHONY: all install uninstall clean distclean data .FORCE
