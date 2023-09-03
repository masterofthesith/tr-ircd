#
# Makefile.in for trircd5
#
BN1             = tr
BN2             = -ir
BN3             = cd
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
CFLAGS          =  -O2 -ggdb -Wall -fno-strict-aliasing -I. -D_REENTRANT -DREENTRANT -DBASENAME=\"$(BN1)$(BN2)$(BN3)\" -DIRCD_PREFIX=\"/home/delir/ircd\"
LDFLAGS         =  -Wl,-export-dynamic
MKDEP           = gcc -MM $(CFLAGS) -DBASENAME=\"$(BN1)$(BN2)$(BN3)\" -DIRCD_PREFIX=\"/home/delir/ircd\"
MV              = /usr/bin/mv
RM              = /usr/bin/rm
YACC            = bison -y
prefix          = /home/delir/ircd
exec_prefix     = ${prefix}
bindir          = ${exec_prefix}/bin
libexecdir      = ${exec_prefix}/libexec
sysconfdir      = ${prefix}/etc
localstatedir   = ${prefix}/var

PROGS           = ircpasswd iupdt.sh isslconfig.sh irandom.sh
IRCPASSWD_EXE	= ircpasswd

SUBDIRLIBS      = ../lib/md5hash.o ../lib/md5/md5.o
STATICLIBS      = 
IRCDLIBS        = $(SUBDIRLIBS) $(STATICLIBS) $(LDFLAGS)  -ldl -lpthread  

INCLUDES        = -I../include  
CPPFLAGS        = ${INCLUDES}  

TOOLS = ircpasswd.c

all: $(IRCPASSWD_EXE)

build: all

depend:
	@echo "Finalized"

$(IRCPASSWD_EXE): ircpasswd.o
	$(CC) ircpasswd.o $(SUBDIRLIBS) -o $(IRCPASSWD_EXE)

clean:
	$(RM) -f $(OBJECTS) $(IRCPASSWD_EXE) *.o *~ core

distclean: clean
	$(RM) -f Makefile iupdt.sh isslconfig.sh irandom.sh

.c.o:
	$(CC) -DIRCPASSWD $(CFLAGS) $(INCLUDES) -c $<

install:
	@echo "Installing tools"
	$(INSTALL) -d ${bindir}
	$(INSTALL_BIN) $(PROGS) ${bindir}

