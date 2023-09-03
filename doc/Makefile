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
YACC            = bison -y
PICFLAGS        = -fPIC -shared
prefix          = /home/delir/ircd
exec_prefix     = ${prefix}
bindir          = ${exec_prefix}/bin
libexecdir      = ${exec_prefix}/libexec
libdir          = ${exec_prefix}/lib
datadir		= ${prefix}/share
sysconfdir      = ${prefix}/etc
localstatedir   = ${prefix}/var

# Local to the etc Makefile
mandir          = ${prefix}/man/man8
md5mandir	= ${prefix}/man/man1
MANPAGE         = ircd.8
MD5MANPAGE	= md5sum.1

CONFS           = configuration/example.conf configuration/example-maskfile.conf configuration/example-socks.conf configuration/example-ihttpd.conf
DEFAULTCONFS    = configuration/ircd.motd configuration/ircdssl.conf
SYSCONFELEM     = trircd
BOOTSCRIPT      = ircd

DOCUMENTATION_0 = ../AUTHORS ../ChangeLog ../INSTALL ../LICENSE ../README.* 
DOCUMENTATION_1 = ../RELNOTES ../TODO

DOCUMENTATION	= $(DOCUMENTATION_0) $(DOCUMENTATION_1)

all:

build:

depend:

distclean: 
	$(RM) Makefile

clean:

install: build
	@echo "Installing config files"
	$(INSTALL) -d ${sysconfdir}
	${INSTALL} $(DEFAULTCONFS) ${sysconfdir}
	@touch ${sysconfdir}/kline.conf
	@echo "Installing manpages"
	$(INSTALL) -d ${mandir}        
	$(INSTALL) -d ${md5mandir}
	$(INSTALL_DATA) man/$(MANPAGE) ${mandir}
	$(INSTALL_DATA) man/$(MD5MANPAGE) ${md5mandir}
	@echo "Installing documentation"
	$(INSTALL) -d ${datadir}/doc
	$(INSTALL_DATA) $(DOCUMENTATION) ${datadir}/doc/
	$(INSTALL_DATA) $(CONFS) ${datadir}	

install_linux:
	$(INSTALL_DATA} linux-fsh/$(SYSCONFELEM) /etc/sysconfig/
	$(INSTALL} linux-fsh/$(BOOTSCRIPT) /etc/init.d/

uninstall:
	@echo "Removing manpages"
	${RM} -f ${md5mandir}/$(MD5MANPAGE)
	${RM} -f ${mandir}/$(MANPAGE)
	@echo "Removing documentation"
	@for i in ${datadir}/*; do \
		${RM} -f ${datadir}/$$i; \
	done

makesums:
	@echo "Creating .md5sums..."
	@$(RM) -f .md5sums
	@for i in $(SOURCES) version.c.SH; do \
		$(MD5SUM) $$i >> .md5sums; \
	done

.PHONY: depend clean distclean

# DO NOT DELETE THIS LINE!!!
# make depend needs it.
