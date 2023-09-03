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
MKDEP           = gcc -MM $(CFLAGS)
MV              = /usr/bin/mv
RM              = /usr/bin/rm
YACC            = bison -y
prefix          = /home/delir/ircd
exec_prefix     = ${prefix}
bindir          = ${exec_prefix}/bin
libexecdir      = ${exec_prefix}/libexec
libdir          = ${exec_prefix}/lib
sysconfdir      = ${prefix}/etc
localstatedir   = ${prefix}/var

INCLUDES        = -I../../include
CPPFLAGS        = ${INCLUDES}  

SOURCES = interproc.c iproc_akill.c iproc_globops.c iproc_gnotice.c iproc_kline.c iproc_notice.c

OBJECTS = $(SOURCES:.c=.o)

all: iproc.a

build: all

# Yuck - don't use this.
depend:
	@${MKDEP} $(INCLUDES) ${SOURCES} > .depend.tmp 
	@sed -e '/^# DO NOT DELETE THIS LINE/,$$d' <Makefile >Makefile.depend
	@echo '# DO NOT DELETE THIS LINE!!!' >>Makefile.depend
	@echo '# make depend needs it.' >>Makefile.depend
	@cat .depend.tmp >>Makefile.depend
	@$(MV) Makefile.depend Makefile
	@$(RM) -f .depend.tmp

clean:
	$(RM) -f *.o iproc.a *~ core

distclean: clean
	$(RM) -f Makefile

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

iproc.a: $(OBJECTS)
	$(RM) -f $@
	$(AR) rc $@ $(OBJECTS)
	$(RANLIB) $@

install: build

makesums:
	@echo "Creating .md5sums..."
	@$(RM) -f .md5sums
	@for i in $(SOURCES); do \
		$(MD5SUMS) $$i >> .md5sums; \
	done

# DO NOT DELETE THIS LINE!!!
# make depend needs it.
interproc.o: interproc.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/h.h ../../include/blalloc.h \
 ../../include/channel.h ../../include/class.h ../../include/client.h \
 ../../include/confitem.h ../../include/fileio.h ../../include/debug.h \
 ../../include/floodspam.h ../../include/identity.h ../../include/init.h \
 ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/internal.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/dlist.h ../../include/s_service.h \
 ../../include/s_user.h ../../include/send.h ../../include/sysvar.h \
 ../../include/listener.h ../../include/language.h ../../include/s_bsd.h \
 ../../include/msg.h ../../include/protodef.h ../../include/protocol.h \
 ../../include/s_conf.h ../../include/interproc.h
iproc_akill.o: iproc_akill.c ../../include/struct.h \
 ../../include/config.h ../../include/setup.h ../../include/ircpath.h \
 ../../include/defaults.h ../../include/sys.h \
 ../../include/../lib/zlib/zlib.h ../../include/../lib/zlib/zconf.h \
 ../../include/../include/setup.h ../../include/linebuf.h \
 ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/h.h ../../include/blalloc.h \
 ../../include/channel.h ../../include/class.h ../../include/client.h \
 ../../include/confitem.h ../../include/fileio.h ../../include/debug.h \
 ../../include/floodspam.h ../../include/identity.h ../../include/init.h \
 ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/internal.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/dlist.h ../../include/s_service.h \
 ../../include/s_user.h ../../include/send.h ../../include/sysvar.h \
 ../../include/listener.h ../../include/s_bsd.h ../../include/msg.h \
 ../../include/protodef.h ../../include/protocol.h \
 ../../include/interproc.h
iproc_globops.o: iproc_globops.c ../../include/struct.h \
 ../../include/config.h ../../include/setup.h ../../include/ircpath.h \
 ../../include/defaults.h ../../include/sys.h \
 ../../include/../lib/zlib/zlib.h ../../include/../lib/zlib/zconf.h \
 ../../include/../include/setup.h ../../include/linebuf.h \
 ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/h.h ../../include/blalloc.h \
 ../../include/channel.h ../../include/class.h ../../include/client.h \
 ../../include/confitem.h ../../include/fileio.h ../../include/debug.h \
 ../../include/floodspam.h ../../include/identity.h ../../include/init.h \
 ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/internal.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/dlist.h ../../include/s_service.h \
 ../../include/s_user.h ../../include/send.h ../../include/sysvar.h \
 ../../include/listener.h ../../include/s_bsd.h ../../include/interproc.h
iproc_gnotice.o: iproc_gnotice.c ../../include/struct.h \
 ../../include/config.h ../../include/setup.h ../../include/ircpath.h \
 ../../include/defaults.h ../../include/sys.h \
 ../../include/../lib/zlib/zlib.h ../../include/../lib/zlib/zconf.h \
 ../../include/../include/setup.h ../../include/linebuf.h \
 ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/h.h ../../include/blalloc.h \
 ../../include/channel.h ../../include/class.h ../../include/client.h \
 ../../include/confitem.h ../../include/fileio.h ../../include/debug.h \
 ../../include/floodspam.h ../../include/identity.h ../../include/init.h \
 ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/internal.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/dlist.h ../../include/s_service.h \
 ../../include/s_user.h ../../include/send.h ../../include/sysvar.h \
 ../../include/listener.h ../../include/s_bsd.h ../../include/msg.h \
 ../../include/protodef.h ../../include/protocol.h \
 ../../include/interproc.h
iproc_kline.o: iproc_kline.c ../../include/struct.h \
 ../../include/config.h ../../include/setup.h ../../include/ircpath.h \
 ../../include/defaults.h ../../include/sys.h \
 ../../include/../lib/zlib/zlib.h ../../include/../lib/zlib/zconf.h \
 ../../include/../include/setup.h ../../include/linebuf.h \
 ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/h.h ../../include/blalloc.h \
 ../../include/channel.h ../../include/class.h ../../include/client.h \
 ../../include/confitem.h ../../include/fileio.h ../../include/debug.h \
 ../../include/floodspam.h ../../include/identity.h ../../include/init.h \
 ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/internal.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/dlist.h ../../include/s_service.h \
 ../../include/s_user.h ../../include/send.h ../../include/sysvar.h \
 ../../include/listener.h ../../include/s_bsd.h ../../include/interproc.h
iproc_notice.o: iproc_notice.c ../../include/struct.h \
 ../../include/config.h ../../include/setup.h ../../include/ircpath.h \
 ../../include/defaults.h ../../include/sys.h \
 ../../include/../lib/zlib/zlib.h ../../include/../lib/zlib/zconf.h \
 ../../include/../include/setup.h ../../include/linebuf.h \
 ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/h.h ../../include/blalloc.h \
 ../../include/channel.h ../../include/class.h ../../include/client.h \
 ../../include/confitem.h ../../include/fileio.h ../../include/debug.h \
 ../../include/floodspam.h ../../include/identity.h ../../include/init.h \
 ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/internal.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/dlist.h ../../include/s_service.h \
 ../../include/s_user.h ../../include/send.h ../../include/sysvar.h \
 ../../include/listener.h ../../include/s_bsd.h ../../include/msg.h \
 ../../include/protodef.h ../../include/protocol.h \
 ../../include/interproc.h
