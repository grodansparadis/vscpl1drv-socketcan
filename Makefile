#
# Makefile : Builds vscpl1drv-socketcan for Unix.
#

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_DIR = /usr/bin/install -c -d
PROJ_SUBDIRS=linux 
VSCP_PROJ_BASE_DIR=/srv/vscp
IPADDRESS :=  $(shell hostname -I)

all:
	@for d in $(PROJ_SUBDIRS); do (echo "====================================================" &&\
	echo "Building in dir " $$d && echo "====================================================" && cd $$d && $(MAKE)); done

install-all: install install-folders install-startup-script install-config install-sample-data install-sample-certs install-manpages

install:
# Install sub components
	@for d in $(PROJ_SUBDIRS); do (echo "====================================================" &&\
	echo "Building in dir " $$d && echo "====================================================" && cd $$d && $(MAKE) install); done

install-manpages:
	@echo "- Installing man-pages."
	$(INSTALL_PROGRAM) -d $(DESTDIR)/usr/share/man/man8
	$(INSTALL_PROGRAM) -d $(DESTDIR)/usr/share/man/man7
	$(INSTALL_PROGRAM) -d $(DESTDIR)/usr/share/man/man1
	$(INSTALL_PROGRAM) -b -m644 man/vscpd.8 $(DESTDIR)/usr/share/man/man8/
	$(INSTALL_PROGRAM) -b -m644 man/uvscpd.8 $(DESTDIR)/usr/share/man/man8/
	$(INSTALL_PROGRAM) -b -m644 man/vscpworks.1 $(DESTDIR)/usr/share/man/man1/
	$(INSTALL_PROGRAM) -b -m644 man/vscpcmd.1 $(DESTDIR)/usr/share/man/man1/
	$(INSTALL_PROGRAM) -b -m644 man/vscp-mkpasswd.1 $(DESTDIR)/usr/share/man/man1/
	$(INSTALL_PROGRAM) -b -m644 man/vscphelperlib.1 $(DESTDIR)/usr/share/man/man1/
	$(INSTALL_PROGRAM) -b -m644 man/vscpdrivers.7 $(DESTDIR)/usr/share/man/man7/
	mandb

clean:
	@for d in $(PROJ_SUBDIRS); do (cd $$d && $(MAKE) clean); done
	rm -f config.log
	rm -f config.startup
	rm -f config.status

distclean: clean
	@sh clean_for_dist
	rm -f m4/Makefile

deb:
	@for d in $(PROJ_SUBDIRS); do (echo "====================================================" &&\
	echo "Building deb in dir " $$d && echo "====================================================" && cd $$d && $(MAKE) deb ); done
