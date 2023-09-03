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
LD		= /mingw64/bin/ld
RANLIB          = ranlib
MD5SUMS         = md5sum
LEXLIB          = 
CFLAGS          =  -O2 -ggdb -Wall -fno-strict-aliasing -I. -D_REENTRANT -DREENTRANT -DBASENAME=\"$(BN1)$(BN2)$(BN3)\" -DIRCD_PREFIX=\"/home/delir/ircd\"
LDFLAGS         =  -Wl,-export-dynamic
MKDEP           = gcc -MM $(CFLAGS) -DBASENAME=\"$(BN1)$(BN2)$(BN3)\" -DIRCD_PREFIX=\"/home/delir/ircd\"
MV              = /usr/bin/mv
GREP		= /usr/bin/grep
RM              = /usr/bin/rm
YACC            = bison -y
PICFLAGS        = -fPIC -shared
GREP		= /usr/bin/grep
prefix          = /home/delir/ircd
exec_prefix     = ${prefix}
bindir          = ${exec_prefix}/bin
libexecdir      = ${exec_prefix}/libexec
libdir          = ${exec_prefix}/lib
sysconfdir      = ${prefix}/etc
localstatedir   = ${prefix}/var
BN1             = tr
BN2             = -ir
BN3             = cd
PATCH           = ../include/patchlevel.h

IRCD_EXE        = ircd          
MD5SUM_EXE      = md5sum

PROGS           = $(IRCD_EXE) $(MD5SUM_EXE)

STATICLIBS      = 
IRCDLIBS        = $(SUBDIRLIBS) $(STATICLIBS)  -ldl -lpthread  

INCLUDES        = -I../include  
CPPFLAGS        = ${INCLUDES}  
MODFLAGS	= 

PROTO_0 = native.c 

SOURCES = $(PROTO_0)

OBJECTS = $(SOURCES:.c=.so)
SHLIBS  = $(OBJECTS:.so=.sl)

all: dynamic

build: all

depend:
	@${MKDEP} $(INCLUDES) ${SOURCES} > .depend.tmp 
	@sed -e '/^# DO NOT DELETE THIS LINE/,$$d' <Makefile >Makefile.depend
	@echo '# DO NOT DELETE THIS LINE!!!' >>Makefile.depend
	@echo '# make depend needs it.' >>Makefile.depend
	@sed -e s/.o:/.so:/ <.depend.tmp >>Makefile.depend
	@$(MV) Makefile.depend Makefile
	@$(RM) -f .depend.tmp

clean:
	$(RM) -f *.so *~ *.a *.lo *.sl

distclean: clean
	$(RM) -f Makefile

.SUFFIXES: .so .sl

.c.so:
	$(CC) -DTOKEN $(PICFLAGS) $(CFLAGS) $(INCLUDES) $(MODFLAGS) $< -o $@

.so.sl:
	$(LD) -b $< -o $@

dynamic: $(OBJECTS)

shlibs: $(SHLIBS)

hpux_shared: $(OBJECTS)
	${MAKE} shlibs

static: $(OBJECTS)
	$(RM) -f libproto.a
	$(AR) cq libproto.a native.so
	$(RANLIB) libproto.a

install:
	-@if test -z "`grep STATIC_MODULES\ 1 ../include/setup.h`" ; then \
		echo "Installing protocol support module files" ; \
		echo "$(INSTALL) -d ${libdir}/protocol" ; \
		$(INSTALL) -d ${libdir}/protocol ; \
		echo "$(INSTALL) *.s* ${libdir}/protocol" ; \
		$(INSTALL) *.s* ${libdir}/protocol ; \
	else \
		echo "Not installing protocol support module files, since STATIC_MODULES defined." ; \
	fi

makesums:
	@echo "Creating .md5sums..."
	@$(RM) -f .md5sums
	@for i in $(SOURCES); do \
		$(MD5SUMS) $$i >> .md5sums; \
	done

.PHONY: depend clean distclean

# DO NOT DELETE THIS LINE!!!
# make depend needs it.
native.so: native.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/numeric.h \
 ../include/msg.h ../include/protodef.h ../include/protocol.h \
 ../include/h.h ../include/blalloc.h ../include/channel.h \
 ../include/class.h ../include/client.h ../include/confitem.h \
 ../include/fileio.h ../include/debug.h ../include/floodspam.h \
 ../include/identity.h ../include/init.h ../include/ircsprintf.h \
 ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/hook.h ../include/s_conf.h ../include/comply.h \
 ../include/usermode.h
