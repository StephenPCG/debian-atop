# Makefile for System & Process Monitor ATOP (Linux version)
#
# Gerlof Langeveld - AT Computing - Nijmegen, The Netherlands
# (gerlof@ATComputing.nl)
#
DESTDIR =

BINPATH = /usr/bin
SCRPATH = /etc/atop
LOGPATH = /var/log/atop
MANPATH = /usr/share/man/man1
INIPATH = /etc/rc.d/init.d
CRNPATH = /etc/cron.d
ROTPATH = /etc/logrotate.d

CFLAGS  = -O -I. -Wall	 # -DHTTPSTATS
LDFLAGS = -lncurses -lm -lz
OBJMOD0 = version.o
OBJMOD1 = various.o  deviate.o   procdbase.o
OBJMOD2 = acctproc.o photoproc.o photosyst.o  rawlog.o ifprop.o parseable.o
OBJMOD3 = showgeneric.o          showlinux.o
OBJMOD4 = atopsar.o
ALLMODS = $(OBJMOD0) $(OBJMOD1) $(OBJMOD2) $(OBJMOD3) $(OBJMOD4)

all: 		atop

atop:		atop.o    $(ALLMODS) Makefile
		cc atop.o $(ALLMODS) -o atop $(LDFLAGS)

clean:
		rm -f *.o atop

install:	atop
		if [ ! -d $(DESTDIR)$(LOGPATH) ]; then mkdir -p $(DESTDIR)$(LOGPATH); fi
		if [ ! -d $(DESTDIR)$(BINPATH) ]; then mkdir -p $(DESTDIR)$(BINPATH); fi
		if [ ! -d $(DESTDIR)$(SCRPATH) ]; then mkdir -p $(DESTDIR)$(SCRPATH); fi
		if [ ! -d $(DESTDIR)$(MANPATH) ]; then mkdir -p $(DESTDIR)$(MANPATH); fi
		if [ ! -d $(DESTDIR)$(INIPATH) ]; then mkdir -p $(DESTDIR)$(INIPATH); fi
		if [ ! -d $(DESTDIR)$(CRNPATH) ]; then mkdir -p $(DESTDIR)$(CRNPATH); fi
		if [ ! -d $(DESTDIR)$(ROTPATH) ]; then mkdir -p $(DESTDIR)$(ROTPATH); fi
		cp atop   	 $(DESTDIR)$(BINPATH)/atop
		chown root	 $(DESTDIR)$(BINPATH)/atop
		chmod 04711 	 $(DESTDIR)$(BINPATH)/atop
		-rm              $(DESTDIR)$(BINPATH)/atopsar
		ln -s atop       $(DESTDIR)$(BINPATH)/atopsar
		cp atop.daily    $(DESTDIR)$(SCRPATH)
		chmod 0711 	 $(DESTDIR)$(SCRPATH)/atop.daily
		cp man/atop.1    $(DESTDIR)$(MANPATH)
		cp man/atopsar.1 $(DESTDIR)$(MANPATH)
		cp atop.init     $(DESTDIR)$(INIPATH)/atop
		cp atop.cron     $(DESTDIR)$(CRNPATH)/atop
		cp psaccs_atop   $(DESTDIR)$(ROTPATH)/psaccs_atop
		cp psaccu_atop   $(DESTDIR)$(ROTPATH)/psaccu_atop
		touch            $(DESTDIR)$(LOGPATH)/dummy_before
		touch            $(DESTDIR)$(LOGPATH)/dummy_after
		if [ -z "$(DESTDIR)" ]; then /sbin/chkconfig --add atop; fi

distr:
		rm -f *.o
		tar czvf /tmp/atop.tar.gz *
##########################################################################

atop.o:		atop.h	photoproc.h photosyst.h  acctproc.h showgeneric.h
atopsar.o:	atop.h	photoproc.h photosyst.h                           
rawlog.o:	atop.h	photoproc.h photosyst.h
various.o:	atop.h                           acctproc.h
ifprop.o:	atop.h	            photosyst.h             ifprop.h
parseable.o:	atop.h	photoproc.h photosyst.h             parseable.h
deviate.o:	atop.h	photoproc.h photosyst.h
procdbase.o:	atop.h	photoproc.h
acctproc.o:	atop.h	photoproc.h              acctproc.h
photoproc.o:	atop.h	photoproc.h
photosyst.o:	atop.h	            photosyst.h
showgeneric.o:	atop.h	photoproc.h photosyst.h  showgeneric.h
showlinux.o:	atop.h	photoproc.h photosyst.h  showgeneric.h
