#************************************************************************
#*   IRC - Internet Relay Chat, Makefile
#*   Copyright (C) 1990, Jarkko Oikarinen
#*
#*   This program is free software; you can redistribute it and/or modify
#*   it under the terms of the GNU General Public License as published by
#*   the Free Software Foundation; either version 2, or (at your option)
#*   any later version.
#*
#*   This program is distributed in the hope that it will be useful,
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#*   GNU General Public License for more details.
#*
#*   You should have received a copy of the GNU General Public License
#*   along with this program; if not, write to the Free Software
#*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#*
#*/

#
# Makefile.in for trircd5
#
CC              = gcc
INSTALL         = /home/delir/trircd-5-enhanced/autoconf/install-sh -c
INSTALL_BIN     = /home/delir/trircd-5-enhanced/autoconf/install-sh -c -m 4755
INSTALL_DATA    = /home/delir/trircd-5-enhanced/autoconf/install-sh -c -m 0644
INSTALL_SUID    = /home/delir/trircd-5-enhanced/autoconf/install-sh -c -m 4755 -o root -m 4755
RM              = /usr/bin/rm
LEX             = flex
AR              = /mingw64/bin/ar
RANLIB          = ranlib
MD5SUMS         = md5sum
LEXLIB          = 
CFLAGS          =  -O2 -ggdb -Wall -fno-strict-aliasing -I. -D_REENTRANT -DREENTRANT -DIRCD_PREFIX=\"/home/delir/ircd\"
LDFLAGS         =  -Wl,-export-dynamic
MKDEP           = gcc -MM $(CFLAGS) -DIRCD_PREFIX=\"/home/delir/ircd\"
MV              = /usr/bin/mv
RM              = /usr/bin/rm
CP		= /usr/bin/cp
YACC            = bison -y
PICFLAGS        = -fPIC -shared
prefix          = /home/delir/ircd
exec_prefix     = ${prefix}
bindir          = ${exec_prefix}/bin
sbindir		= ${exec_prefix}/sbin
libexecdir      = ${exec_prefix}/libexec
libdir		= ${exec_prefix}/lib
datadir		= ${prefix}/share
sysconfdir      = ${prefix}/etc
localstatedir   = ${prefix}/var
sharedstatedir	= ${prefix}/com
includedir	= ${prefix}/include
oldincludedir	= /usr/include
infodir		= ${prefix}/info
mandir		= ${prefix}/man

#END OF DEFINITIONS

IRCD_EXE        = ircd
MD5SUM_EXE	= md5sum
IRCPASSWD_EXE	= ircpasswd

PROGS           = src/$(IRCD_EXE) lib/md5/$(MD5SUM_EXE) tools/$(IRCPASSWD_EXE)
HASHPROG        = ${bindir}/$(IRCPASSWD_EXE)

SUBDIRLIBS      = ../lib/zlib/libz.a ../lib/md5/md5.a ../server/server.a ../lib/adns/libadns.a ../newconf/config.a
IRCDLIBS        = $(SUBDIRLIBS)  -ldl -lpthread  @GMP_LIBS@ 

CPPFLAGS        = ${INCLUDES}  
MODFLAGS        = 

# Default make flags - you may want to uncomment this on a multicpu machine
#MFLAGS = -j 4

SHELL=/bin/sh
SUBDIRS=include doc help protocol modules lang chanmodes contrib server newconf lib/md5 lib lib/adns src tools
MD5DIRS=include lib protocol modules chanmodes server src

MAKE = /usr/bin/make ${MFLAGS} 

all:	build


autoconf: autoconf/configure.in
	autoconf autoconf/configure.in >configure
	autoheader autoconf/configure.in > include/setup.h.in
	${RM} -f config.cache

build:
	$(CP) src/static.c.in src/static.c	
	$(CP) include/static.h.in include/static.h
	@for i in $(SUBDIRS); do \
		echo "build ==> $$i";\
		${MAKE} -C $$i build ;\
	done
	@cat src/mdsums.log
	$(RM) -f src/mdsums.log

clean:
	@for i in $(SUBDIRS); do \
		echo "clean ==> $$i";\
		${MAKE} -C $$i clean ;\
	done
	-@if [ -f include/setup.h ] ; then \
	echo "To really restart installation, make distclean" ;\
	fi

distclean:
	${RM} -f Makefile *~ *.rej *.orig core ircd.core
	${RM} -f config.status config.cache config.log
	@for i in $(SUBDIRS); do \
		echo "distclean ==> $$i";\
		${MAKE} -C $$i distclean ;\
	done

depend:
	@for i in $(SUBDIRS); do \
		echo "depend ==> $$i";\
		${MAKE} -C $$i depend ;\
	done

install: all
	@echo "install ==> trircd5"
	@for i in $(SUBDIRS); do \
		echo "install ==> $$i";\
		${MAKE} -C $$i install ;\
	done
	@${bindir}/irandom.sh
	@echo "***********************************************"
	@echo "Please do not forget to check your example.conf"
	@echo "When creating ircd.conf configuration file"
	@echo "***********************************************"
	@echo "If you have enabled the SSL option, you would"
	@echo "Need creating an SSL Certificate. Please check"
	@echo "out the ircdssl.conf for this."
	@echo "The certificate can be created with the"
	@echo "isslconfig.sh utility provided"
	@echo "***********************************************"
	@echo "Finished Installation"

uninstall_begin:
	@echo "Removing binaries"
	${MAKE} -C src uninstall
	@echo "Removing libraries and modules"
	${RM} -rf ${libdir}/modules 
	${RM} -rf ${libdir}/protocol 
	${RM} -rf ${libdir}/chanmodes 
	${RM} -rf ${libdir}/contrib 
	${RM} -rf ${libdir}/languages
	@echo "Removing includes"
	${MAKE} -C include uninstall
	@echo "Removing documentation and manpages"
	${MAKE} -C help uninstall
	${MAKE} -C doc uninstall

uninstall_leave_dirs:
	@echo "Uninstallation completed, some directories were not removed"
	@echo "Persisting, possibly empty directories are:"
	@echo "   ${bindir}"
	@echo "   ${libdir}"
	@echo "   ${includedir}"
	@echo "   ${datadir}"
	@echo "   ${mandir}"

uninstall_no_touch:
	@echo "Following directories were not touched"
	@echo "   ${sysconfdir}"
	@echo "   ${localstatedir}"

uninstall_remove_dirs:
	@echo "Now removing directories"
	${RM} -rf ${bindir} ${libdir} ${includedir} ${datadir} ${mandir}

uninstall_end:
	@echo "************************************"
	@echo "Finished uninstallation"

uninstall: uninstall_begin uninstall_leave_dirs uninstall_no_touch uninstall_end
uninstall_all: uninstall_begin uninstall_remove_dirs uninstall_no_touch uninstall_end

makesums:
	@for i in $(MD5DIRS); do \
		echo "makesums ==> $$i";\
		${MAKE} -C $$i makesums ;\
	done
	@echo "Finished"

ircpathgen:
	@sh autoconf/ircpathgen ${PWD}
	@echo "Path information generated in include/ircpath.h"	
