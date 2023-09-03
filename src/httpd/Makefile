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
BITFLAGS	= 
PICFLAGS        = -fPIC -shared
MODFLAGS        = 
MKDEP           = gcc -MM $(CFLAGS)
MV              = /usr/bin/mv
GREP		= /usr/bin/grep
RM              = /usr/bin/rm
YACC            = bison -y
prefix          = /home/delir/ircd
exec_prefix     = ${prefix}
bindir          = ${exec_prefix}/bin
libexecdir      = ${exec_prefix}/libexec
libdir          = ${exec_prefix}/lib
sysconfdir      = ${prefix}/etc
localstatedir   = ${prefix}/var

STATICLIBS	= 

INCLUDES        = -I../../include
CPPFLAGS        = ${INCLUDES}  

HTTPD		= httpd.so

HSRCS_0 = httpd_author.c httpd_config.c httpd_get.c httpd_headers.c
HSRCS_1 = httpd_init.c httpd_main.c httpd_reader.c httpd_post.c httpd_send.c

SOURCES = $(HSRCS_0) $(HSRCS_1)

OBJECTS = $(SOURCES:.c=.o)

all: dynamic

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
	$(RM) -f *.o *~ $(HTTPD)

distclean: clean
	$(RM) -f Makefile

.c.o:
	$(CC) -DHTTPD_MODULE $(CFLAGS) $(INCLUDES) -c $< -o $@

dynamic: $(OBJECTS)
	$(CC) $(PICFLAGS) $(MODFLAGS) -o $(HTTPD) $(OBJECTS)

static: $(OBJECTS)
	$(AR) rc $(HTTPD) $(OBJECTS)

install:
	@echo "Installing httpd"	
	$(INSTALL) -d ${libdir}/tools
	$(INSTALL) $(HTTPD) ${libdir}/tools

uninstall:
	@echo "Removing httpd"
	${RM} -f ${libdir}/tools/$(HTTPD)

makesums:
	@echo "Creating .md5sums..."
	@$(RM) -f .md5sums
	@for i in $(SOURCES); do \
		$(MD5SUMS) $$i >> .md5sums; \
	done

# DO NOT DELETE THIS LINE!!!
# make depend needs it.
httpd_author.o: httpd_author.c ../../include/struct.h \
 ../../include/config.h ../../include/setup.h ../../include/ircpath.h \
 ../../include/defaults.h ../../include/sys.h \
 ../../include/../lib/zlib/zlib.h ../../include/../lib/zlib/zconf.h \
 ../../include/../include/setup.h ../../include/linebuf.h \
 ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/numeric.h ../../include/msg.h \
 ../../include/protodef.h ../../include/protocol.h \
 ../../include/channel.h ../../include/s_conf.h ../../include/listener.h \
 ../../include/h.h ../../include/blalloc.h ../../include/channel.h \
 ../../include/class.h ../../include/client.h ../../include/confitem.h \
 ../../include/fileio.h ../../include/debug.h ../../include/floodspam.h \
 ../../include/identity.h ../../include/init.h ../../include/ircsprintf.h \
 ../../include/logtype.h ../../include/maskitem.h \
 ../../include/../lib/regex/regex.h ../../include/memory.h \
 ../../include/modules.h ../../include/motditem.h ../../include/parse.h \
 ../../include/s_bsd.h ../../include/fd.h ../../include/resnew.h \
 ../../include/../lib/adns/adns.h ../../include/../lib/adns/internal.h \
 ../../include/../lib/adns/adns.h ../../include/../lib/adns/dlist.h \
 ../../include/s_service.h ../../include/s_user.h ../../include/send.h \
 ../../include/sysvar.h ../../include/httpd.h http.h ../../include/hook.h
httpd_config.o: httpd_config.c ../../include/struct.h \
 ../../include/config.h ../../include/setup.h ../../include/ircpath.h \
 ../../include/defaults.h ../../include/sys.h \
 ../../include/../lib/zlib/zlib.h ../../include/../lib/zlib/zconf.h \
 ../../include/../include/setup.h ../../include/linebuf.h \
 ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/numeric.h ../../include/msg.h \
 ../../include/protodef.h ../../include/protocol.h \
 ../../include/confitem.h ../../include/fileio.h ../../include/channel.h \
 ../../include/s_conf.h ../../include/listener.h ../../include/h.h \
 ../../include/blalloc.h ../../include/channel.h ../../include/class.h \
 ../../include/client.h ../../include/confitem.h ../../include/debug.h \
 ../../include/floodspam.h ../../include/identity.h ../../include/init.h \
 ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/internal.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/dlist.h ../../include/s_service.h \
 ../../include/s_user.h ../../include/send.h ../../include/sysvar.h \
 ../../include/httpd.h http.h ../../include/hook.h
httpd_get.o: httpd_get.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/dh.h ../../include/h.h \
 ../../include/blalloc.h ../../include/channel.h ../../include/class.h \
 ../../include/client.h ../../include/confitem.h ../../include/fileio.h \
 ../../include/debug.h ../../include/floodspam.h ../../include/identity.h \
 ../../include/init.h ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/internal.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/dlist.h ../../include/s_service.h \
 ../../include/s_user.h ../../include/send.h ../../include/sysvar.h \
 ../../include/listener.h ../../include/logtype.h ../../include/s_conf.h \
 ../../include/httpd.h http.h ../../include/hook.h
httpd_headers.o: httpd_headers.c ../../include/struct.h \
 ../../include/config.h ../../include/setup.h ../../include/ircpath.h \
 ../../include/defaults.h ../../include/sys.h \
 ../../include/../lib/zlib/zlib.h ../../include/../lib/zlib/zconf.h \
 ../../include/../include/setup.h ../../include/linebuf.h \
 ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/numeric.h ../../include/msg.h \
 ../../include/protodef.h ../../include/protocol.h \
 ../../include/channel.h ../../include/s_conf.h ../../include/listener.h \
 ../../include/h.h ../../include/blalloc.h ../../include/channel.h \
 ../../include/class.h ../../include/client.h ../../include/confitem.h \
 ../../include/fileio.h ../../include/debug.h ../../include/floodspam.h \
 ../../include/identity.h ../../include/init.h ../../include/ircsprintf.h \
 ../../include/logtype.h ../../include/maskitem.h \
 ../../include/../lib/regex/regex.h ../../include/memory.h \
 ../../include/modules.h ../../include/motditem.h ../../include/parse.h \
 ../../include/s_bsd.h ../../include/fd.h ../../include/resnew.h \
 ../../include/../lib/adns/adns.h ../../include/../lib/adns/internal.h \
 ../../include/../lib/adns/adns.h ../../include/../lib/adns/dlist.h \
 ../../include/s_service.h ../../include/s_user.h ../../include/send.h \
 ../../include/sysvar.h ../../include/httpd.h http.h ../../include/hook.h
httpd_init.o: httpd_init.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/modules.h ../../include/msg.h \
 ../../include/protodef.h ../../include/protocol.h ../../include/h.h \
 ../../include/blalloc.h ../../include/channel.h ../../include/class.h \
 ../../include/client.h ../../include/confitem.h ../../include/fileio.h \
 ../../include/debug.h ../../include/floodspam.h ../../include/identity.h \
 ../../include/init.h ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/internal.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/dlist.h ../../include/s_service.h \
 ../../include/s_user.h ../../include/send.h ../../include/sysvar.h \
 ../../include/logtype.h ../../include/s_conf.h ../../include/event.h \
 ../../include/httpd.h http.h ../../include/hook.h
httpd_main.o: httpd_main.c ../../include/struct.h ../../include/config.h \
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
 ../../include/logtype.h ../../include/s_conf.h ../../include/dh.h \
 ../../include/listener.h ../../include/httpd.h http.h \
 ../../include/hook.h
httpd_reader.o: httpd_reader.c ../../include/struct.h \
 ../../include/config.h ../../include/setup.h ../../include/ircpath.h \
 ../../include/defaults.h ../../include/sys.h \
 ../../include/../lib/zlib/zlib.h ../../include/../lib/zlib/zconf.h \
 ../../include/../include/setup.h ../../include/linebuf.h \
 ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/dh.h ../../include/h.h \
 ../../include/blalloc.h ../../include/channel.h ../../include/class.h \
 ../../include/client.h ../../include/confitem.h ../../include/fileio.h \
 ../../include/debug.h ../../include/floodspam.h ../../include/identity.h \
 ../../include/init.h ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/internal.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/dlist.h ../../include/s_service.h \
 ../../include/s_user.h ../../include/send.h ../../include/sysvar.h \
 ../../include/listener.h ../../include/logtype.h ../../include/s_conf.h \
 ../../include/httpd.h http.h ../../include/hook.h
httpd_post.o: httpd_post.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/common.h \
 ../../include/sys.h ../../include/dh.h ../../include/h.h \
 ../../include/blalloc.h ../../include/channel.h ../../include/class.h \
 ../../include/client.h ../../include/confitem.h ../../include/fileio.h \
 ../../include/debug.h ../../include/floodspam.h ../../include/identity.h \
 ../../include/init.h ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/internal.h ../../include/../lib/adns/adns.h \
 ../../include/../lib/adns/dlist.h ../../include/s_service.h \
 ../../include/s_user.h ../../include/send.h ../../include/sysvar.h \
 ../../include/msg.h ../../include/protodef.h ../../include/protocol.h \
 ../../include/event.h ../../include/language.h ../../include/listener.h \
 ../../include/logtype.h ../../include/s_conf.h ../../include/httpd.h \
 http.h ../../include/hook.h
httpd_send.o: httpd_send.c ../../include/struct.h ../../include/config.h \
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
 ../../include/logtype.h ../../include/s_conf.h ../../include/httpd.h \
 http.h ../../include/hook.h ../../include/listener.h ../../include/dh.h
