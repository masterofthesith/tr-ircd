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
sysconfdir      = ${prefix}/etc
localstatedir   = ${prefix}/var
includeinstall  = ${prefix}/include

IRCD_EXE        = ircd          
MD5SUM_EXE      = md5sum

PROGS           = $(IRCD_EXE) $(MD5SUM_EXE)

STATICLIBS      = 
IRCDLIBS        = $(SUBDIRLIBS) $(STATICLIBS)  -ldl -lpthread  

INCLUDES        = -I../include  
CPPFLAGS        = ${INCLUDES}  
MODFLAGS	= 

INC_0 = blalloc.h capabflag.h chanmode.h channel.h class.h client.h cmodetab.h
INC_1 = common.h comply.h config.h confitem.h debug.h defaults.h dh.h event.h
INC_2 = fd.h fileio.h floodspam.h h.h hook.h hostmask.h identity.h init.h
INC_3 = ircd_defs.h ircsprintf.h language.h linebuf.h listener.h logtype.h
INC_4 = maskitem.h memory.h modules.h motditem.h msg.h msgtok1.h numeric.h
INC_5 = optparse.h packet.h parse.h patchlevel.h protocol.h protodef.h proxy.h
INC_6 = queue.h resnew.h s_auth.h s_bsd.h s_conf.h s_service.h s_user.h send.h
INC_7 = struct.h supported.h sys.h sysvar.h throttle.h tools.h usermode.h zlink.h

SOURCES = $(INC_0) $(INC_1) $(INC_2) $(INC_3) $(INC_4) $(INC_5) $(INC_6) $(INC_7)

all:

build:

depend:

distclean: 
	$(RM) -f Makefile ircpath.h *.rej *.orig setup.h

clean:

install:
	@echo "Installing includes"
	$(INSTALL) -d ${includeinstall}
	$(INSTALL) $(SOURCES) ${includeinstall}

uninstall:
	@echo "Removing includes"
	@for i in $(SOURCES); do \
		${RM} -f ${includeinstall}/$$i; \
	done

makesums:
	@echo "Creating .md5sums..."
	@$(RM) -f .md5sums
	@for i in $(SOURCES); do \
		$(MD5SUMS) $$i >> .md5sums; \
	done

.PHONY: depend clean distclean

# DO NOT DELETE THIS LINE!!!
# make depend needs it.
