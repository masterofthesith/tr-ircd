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

SSL_INCLUDES    = 

IRCD_EXE        = ircd          

PROGS           = $(IRCD_EXE)

SUBDIRLIBS_1    = ../server/server.a ../newconf/config.a 
SUBDIRLIBS_2	= logsystem/liblog.a iupdt/iupdt.a ../lib/ilib.a interproc/iproc.a

SUBDIRLIBS	= $(SUBDIRLIBS_1) $(SUBDIRLIBS_2)

LIBRARYLIBS	=  -lcrypto -lssl $(LEXLIB) -L../lib/adns -ladns -L../lib/zlib -lz

STATICLIBS	= 
IRCDLIBS        = $(SUBDIRLIBS) $(STATICLIBS)  -ldl -lpthread  $(LIBRARYLIBS)

INCLUDES        = -I../include   $(SSL_INCLUDES)
CPPFLAGS        = ${INCLUDES}  

SRC_0 = adns.c boot.c blalloc.c chanmodes.c channel.c capability.c class.c client.c 
SRC_1 = commands.c confitem.c dh.c dynlink.c event.c eventmap.c fd.c fileio.c flush.c  
SRC_2 = hash.c hook.c hostmask.c identity.c ircd.c ircsprintf.c linebuf.c loader.c 
SRC_3 = maskitem.c match.c memory.c modcommands.c modules.c optparse.c parse.c 
SRC_4 = paths.c pseudo.c protocol.c rc4.c send.c ssl.c stats.c support.c 
SRC_5 = textedit.c throttle.c tools.c usermodes.c watch.c whowas.c zlink.c

SOURCES = $(SRC_0) $(SRC_1) $(SRC_2) $(SRC_3) $(SRC_4) $(SRC_5)

OBJECTS = $(SOURCES:.c=.o) version.o

all: $(IRCD_EXE)

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
	${MAKE} -C interproc depend
	${MAKE} -C iupdt depend
	${MAKE} -C httpd depend
	${MAKE} -C proxymon depend
	${MAKE} -C logsystem depend

clean:
	$(RM) -f *.o *~ $(IRCD_EXE).core core $(IRCD_EXE) version.c.last
	${MAKE} -C ../lib/zlib -f Makefile.tr-ircd clean
	${MAKE} -C interproc clean
	${MAKE} -C iupdt clean
	${MAKE} -C httpd clean
	${MAKE} -C logsystem clean
	${MAKE} -C proxymon clean

distclean: clean
	$(RM) -f Makefile version.c.last
	${MAKE} -C ../lib/zlib -f Makefile.tr-ircd distclean
	${MAKE} -C interproc distclean
	${MAKE} -C iupdt distclean
	${MAKE} -C httpd distclean
	${MAKE} -C logsystem distclean
	${MAKE} -C proxymon distclean

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

static-objects:
	if test -n "`grep "STATIC_MODULES\ 1" ../include/setup.h`" ; then \
		if test -z "`grep httpd static.c`" ; then \
			echo "extern void httpd_init(void);" >> ../include/static.h ; \
			if test -n "`grep "USE_OPM\ 1" ../include/setup.h`" ; then \
				echo "extern void proxymon_init(void);" >> ../include/static.h ; \
				echo " proxymon_init();" >> static.c ; \
			fi ; \
			echo " httpd_init();" >> static.c ; \
		fi \
	fi
	if test -z "`grep "}" static.c`" ; then \
		echo "}" >> static.c ; \
	fi
	$(CC) $(CFLAGS) $(INCLUDES) -c static.c -o static.o

$(IRCD_EXE): $(OBJECTS)
	${MAKE} static-objects
	${MAKE} -C ../lib/zlib -f Makefile.tr-ircd build
	${MAKE} -C interproc build
	${MAKE} -C iupdt build
	${MAKE} -C httpd build
	${MAKE} -C logsystem build
	if test -n "`grep "STATIC_MODULES\ 1" ../include/setup.h`" ; then \
		if test -n "`grep "USE_OPM\ 1" ../include/setup.h`" ; then \
			${MAKE} -C proxymon build ; \
			$(CC) ${LDFLAGS} ${BITFLAGS} -o ${IRCD_EXE} $(OBJECTS) static.o httpd/httpd.so proxymon/proxymon.so $(IRCDLIBS) ; \
		else \
			$(CC) ${LDFLAGS} ${BITFLAGS} -o ${IRCD_EXE} $(OBJECTS) static.o httpd/httpd.so $(IRCDLIBS) ; \
		fi \
	else \
		if test -n "`grep "USE_OPM\ 1" ../include/setup.h`" ; then \
			${MAKE} -C proxymon build ; \
		fi ; \
		$(CC) ${LDFLAGS} ${BITFLAGS} -o ${IRCD_EXE} $(OBJECTS) static.o $(IRCDLIBS) ; \
	fi
	$(MV) version.c version.c.last

install:
	@echo "Installing ircd"	
	$(INSTALL) -d ${bindir}
	$(INSTALL) -d ${localstatedir}/log/$(BN1)$(BN2)$(BN3)
	$(INSTALL) -d ${localstatedir}/run/$(BN1)$(BN2)$(BN3)
	$(INSTALL) -d ${localstatedir}/lib/$(BN1)$(BN2)$(BN3)
	$(INSTALL_BIN) $(PROGS) ${bindir}
	if test -z "`grep "STATIC_MODULES\ 1" ../include/setup.h`" ; then \
		if test -n "`grep "USE_OPM\ 1" ../include/setup.h`" ; then \
			${MAKE} -C proxymon install ; \
		fi ; \
		${MAKE} -C httpd install ; \
	fi

uninstall:
	@echo "Removing ircd"
	${RM} -f ${bindir}/$(PROGS)
	${MAKE} -C proxymon uninstall
	${MAKE} -C httpd uninstall

makesums:
	@echo "Creating .md5sums..."
	@$(RM) -f .md5sums
	@for i in $(SOURCES); do \
		$(MD5SUMS) $$i >> .md5sums; \
	done
	${MAKE} -C interproc makesums
	${MAKE} -C iupdt makesums
	${MAKE} -C httpd makesums
	${MAKE} -C logsystem makesums
	${MAKE} -C proxymon makesums

version.c: version.c.SH
	/bin/sh ./version.c.SH $(MD5SUMS)

# DO NOT DELETE THIS LINE!!!
# make depend needs it.
adns.o: adns.c ../include/struct.h ../include/config.h ../include/setup.h \
 ../include/ircpath.h ../include/defaults.h ../include/sys.h \
 ../include/../lib/zlib/zlib.h ../include/../lib/zlib/zconf.h \
 ../include/setup.h ../include/linebuf.h ../include/tools.h \
 ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/fileio.h \
 ../include/resnew.h ../include/fileio.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_bsd.h ../include/fd.h \
 ../include/resnew.h ../include/event.h ../include/numeric.h \
 ../include/h.h ../include/blalloc.h ../include/channel.h \
 ../include/class.h ../include/client.h ../include/confitem.h \
 ../include/debug.h ../include/floodspam.h ../include/identity.h \
 ../include/init.h ../include/ircsprintf.h ../include/logtype.h \
 ../include/maskitem.h ../include/../lib/regex/regex.h \
 ../include/memory.h ../include/modules.h ../include/motditem.h \
 ../include/parse.h ../include/s_bsd.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h
boot.o: boot.c ../include/struct.h ../include/config.h ../include/setup.h \
 ../include/ircpath.h ../include/defaults.h ../include/sys.h \
 ../include/../lib/zlib/zlib.h ../include/../lib/zlib/zconf.h \
 ../include/setup.h ../include/linebuf.h ../include/tools.h \
 ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/event.h ../include/h.h \
 ../include/blalloc.h ../include/channel.h ../include/class.h \
 ../include/client.h ../include/confitem.h ../include/fileio.h \
 ../include/debug.h ../include/floodspam.h ../include/identity.h \
 ../include/init.h ../include/ircsprintf.h ../include/logtype.h \
 ../include/maskitem.h ../include/../lib/regex/regex.h \
 ../include/memory.h ../include/modules.h ../include/motditem.h \
 ../include/parse.h ../include/s_bsd.h ../include/fd.h \
 ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/s_conf.h ../include/logtype.h ../include/usermode.h
blalloc.o: blalloc.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/blalloc.h ../include/tools.h ../include/fd.h \
 ../include/s_conf.h
chanmodes.o: chanmodes.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/numeric.h \
 ../include/channel.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/hook.h ../include/msg.h ../include/protodef.h \
 ../include/protocol.h ../include/s_conf.h ../include/chanmode.h \
 ../include/cmodetab.h
channel.o: channel.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/numeric.h \
 ../include/channel.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/msg.h ../include/protodef.h ../include/protocol.h \
 ../include/hook.h ../include/event.h ../include/blalloc.h \
 ../include/chanmode.h ../include/usermode.h ../include/s_conf.h
capability.o: capability.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/tools.h \
 ../include/hook.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/msg.h ../include/protodef.h ../include/protocol.h \
 ../include/numeric.h
class.o: class.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/numeric.h ../include/sys.h ../include/h.h \
 ../include/blalloc.h ../include/channel.h ../include/class.h \
 ../include/client.h ../include/confitem.h ../include/fileio.h \
 ../include/debug.h ../include/floodspam.h ../include/identity.h \
 ../include/init.h ../include/ircsprintf.h ../include/logtype.h \
 ../include/maskitem.h ../include/../lib/regex/regex.h \
 ../include/memory.h ../include/modules.h ../include/motditem.h \
 ../include/parse.h ../include/s_bsd.h ../include/fd.h \
 ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h
client.o: client.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/numeric.h ../include/blalloc.h ../include/language.h \
 ../include/dh.h ../include/s_conf.h ../include/zlink.h \
 ../include/tools.h
commands.o: commands.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/msg.h ../include/protodef.h \
 ../include/protocol.h ../include/sys.h ../include/numeric.h \
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
 ../include/s_conf.h
confitem.o: confitem.c ../include/sys.h ../include/setup.h \
 ../include/../lib/zlib/zlib.h ../include/../lib/zlib/zconf.h \
 ../include/setup.h ../include/common.h ../include/struct.h \
 ../include/config.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/linebuf.h ../include/tools.h \
 ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/resnew.h ../include/fileio.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/debug.h ../include/floodspam.h \
 ../include/identity.h ../include/init.h ../include/ircsprintf.h \
 ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/confitem.h ../include/s_conf.h ../include/listener.h
dh.o: dh.c ../include/struct.h ../include/config.h ../include/setup.h \
 ../include/ircpath.h ../include/defaults.h ../include/sys.h \
 ../include/../lib/zlib/zlib.h ../include/../lib/zlib/zconf.h \
 ../include/setup.h ../include/linebuf.h ../include/tools.h \
 ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/memory.h ../include/dh.h
dynlink.o: dynlink.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/confitem.h \
 ../include/fileio.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/debug.h ../include/floodspam.h \
 ../include/identity.h ../include/init.h ../include/ircsprintf.h \
 ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/hook.h ../include/s_conf.h ../include/msg.h \
 ../include/protodef.h ../include/protocol.h ../include/numeric.h \
 ../include/modules.h
event.o: event.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/event.h ../include/numeric.h
eventmap.o: eventmap.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/eventmap.h ../include/common.h ../include/sys.h \
 ../include/numeric.h ../include/msg.h ../include/protodef.h \
 ../include/protocol.h ../include/tools.h ../include/confitem.h \
 ../include/fileio.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/debug.h ../include/floodspam.h \
 ../include/identity.h ../include/init.h ../include/ircsprintf.h \
 ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/s_conf.h ../include/modules.h
fd.o: fd.c ../include/struct.h ../include/config.h ../include/setup.h \
 ../include/ircpath.h ../include/defaults.h ../include/sys.h \
 ../include/../lib/zlib/zlib.h ../include/../lib/zlib/zconf.h \
 ../include/setup.h ../include/linebuf.h ../include/tools.h \
 ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/event.h ../include/s_bsd.h ../include/numeric.h
fileio.o: fileio.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/fileio.h ../include/fd.h
flush.o: flush.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/channel.h ../include/common.h ../include/sys.h \
 ../include/s_conf.h ../include/msg.h ../include/protodef.h \
 ../include/protocol.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/numeric.h
hash.o: hash.c ../include/struct.h ../include/config.h ../include/setup.h \
 ../include/ircpath.h ../include/defaults.h ../include/sys.h \
 ../include/../lib/zlib/zlib.h ../include/../lib/zlib/zconf.h \
 ../include/setup.h ../include/linebuf.h ../include/tools.h \
 ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/s_conf.h ../include/h.h \
 ../include/blalloc.h ../include/channel.h ../include/class.h \
 ../include/client.h ../include/confitem.h ../include/fileio.h \
 ../include/debug.h ../include/floodspam.h ../include/identity.h \
 ../include/init.h ../include/ircsprintf.h ../include/logtype.h \
 ../include/maskitem.h ../include/../lib/regex/regex.h \
 ../include/memory.h ../include/modules.h ../include/motditem.h \
 ../include/parse.h ../include/s_bsd.h ../include/fd.h \
 ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h
hook.o: hook.c ../include/struct.h ../include/config.h ../include/setup.h \
 ../include/ircpath.h ../include/defaults.h ../include/sys.h \
 ../include/../lib/zlib/zlib.h ../include/../lib/zlib/zconf.h \
 ../include/setup.h ../include/linebuf.h ../include/tools.h \
 ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/tools.h \
 ../include/hook.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/numeric.h ../include/eventmap.h
hostmask.o: hostmask.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/s_conf.h ../include/hostmask.h ../include/numeric.h
identity.o: identity.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/numeric.h \
 ../include/msg.h ../include/protodef.h ../include/protocol.h \
 ../include/channel.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h
ircd.o: ircd.c ../include/struct.h ../include/config.h ../include/setup.h \
 ../include/ircpath.h ../include/defaults.h ../include/sys.h \
 ../include/../lib/zlib/zlib.h ../include/../lib/zlib/zconf.h \
 ../include/setup.h ../include/linebuf.h ../include/tools.h \
 ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/numeric.h \
 ../include/channel.h ../include/msg.h ../include/protodef.h \
 ../include/protocol.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/optparse.h ../include/dh.h ../include/hook.h \
 ../include/tools.h ../include/s_auth.h ../include/s_bsd.h \
 ../include/s_conf.h ../include/event.h ../include/modules.h \
 ../include/language.h ../include/throttle.h ../include/comply.h \
 ../include/packet.h ../include/usermode.h ../include/interproc.h
ircsprintf.o: ircsprintf.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/ircsprintf.h ../include/s_conf.h ../include/s_user.h \
 ../include/channel.h
linebuf.o: linebuf.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/linebuf.h ../include/h.h \
 ../include/blalloc.h ../include/channel.h ../include/class.h \
 ../include/client.h ../include/confitem.h ../include/fileio.h \
 ../include/debug.h ../include/floodspam.h ../include/identity.h \
 ../include/init.h ../include/ircsprintf.h ../include/logtype.h \
 ../include/maskitem.h ../include/../lib/regex/regex.h \
 ../include/memory.h ../include/modules.h ../include/motditem.h \
 ../include/parse.h ../include/s_bsd.h ../include/fd.h \
 ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/event.h ../include/blalloc.h ../include/tools.h \
 ../include/interproc.h ../include/dh.h
loader.o: loader.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/confitem.h \
 ../include/fileio.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/debug.h ../include/floodspam.h \
 ../include/identity.h ../include/init.h ../include/ircsprintf.h \
 ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/hook.h ../include/s_conf.h ../include/msg.h \
 ../include/protodef.h ../include/protocol.h ../include/numeric.h \
 ../include/tools.h ../include/modules.h
maskitem.o: maskitem.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/confitem.h \
 ../include/fileio.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/debug.h ../include/floodspam.h \
 ../include/identity.h ../include/init.h ../include/ircsprintf.h \
 ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/hostmask.h ../include/hook.h ../include/msg.h \
 ../include/protodef.h ../include/protocol.h ../include/numeric.h \
 ../include/s_conf.h
match.o: match.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/s_user.h ../include/h.h \
 ../include/blalloc.h ../include/channel.h ../include/class.h \
 ../include/client.h ../include/confitem.h ../include/fileio.h \
 ../include/debug.h ../include/floodspam.h ../include/identity.h \
 ../include/init.h ../include/ircsprintf.h ../include/logtype.h \
 ../include/maskitem.h ../include/../lib/regex/regex.h \
 ../include/memory.h ../include/modules.h ../include/motditem.h \
 ../include/parse.h ../include/s_bsd.h ../include/fd.h \
 ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h
memory.o: memory.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/numeric.h ../include/blalloc.h ../include/s_conf.h
modcommands.o: modcommands.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/confitem.h \
 ../include/fileio.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/debug.h ../include/floodspam.h \
 ../include/identity.h ../include/init.h ../include/ircsprintf.h \
 ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/hook.h ../include/msg.h ../include/protodef.h \
 ../include/protocol.h ../include/numeric.h ../include/modules.h \
 ../include/s_conf.h
modules.o: modules.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/confitem.h \
 ../include/fileio.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/debug.h ../include/floodspam.h \
 ../include/identity.h ../include/init.h ../include/ircsprintf.h \
 ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/hook.h ../include/msg.h ../include/protodef.h \
 ../include/protocol.h ../include/numeric.h ../include/modules.h \
 ../include/s_conf.h
optparse.o: optparse.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/s_conf.h ../include/optparse.h
parse.o: parse.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/msg.h ../include/protodef.h \
 ../include/protocol.h ../include/msgtok1.h ../include/sys.h \
 ../include/numeric.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/s_conf.h
paths.o: paths.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/confitem.h \
 ../include/fileio.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/debug.h ../include/floodspam.h \
 ../include/identity.h ../include/init.h ../include/ircsprintf.h \
 ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/hook.h ../include/msg.h ../include/protodef.h \
 ../include/protocol.h ../include/numeric.h ../include/modules.h \
 ../include/s_conf.h
pseudo.o: pseudo.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/msg.h ../include/protodef.h \
 ../include/protocol.h ../include/sys.h ../include/numeric.h \
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
 ../include/s_user.h ../include/send.h ../include/sysvar.h
protocol.o: protocol.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/comply.h ../include/s_conf.h
rc4.o: rc4.c ../include/setup.h ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/dh.h
send.o: send.c ../include/struct.h ../include/config.h ../include/setup.h \
 ../include/ircpath.h ../include/defaults.h ../include/sys.h \
 ../include/../lib/zlib/zlib.h ../include/../lib/zlib/zconf.h \
 ../include/setup.h ../include/linebuf.h ../include/tools.h \
 ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/channel.h ../include/common.h ../include/sys.h ../include/h.h \
 ../include/blalloc.h ../include/channel.h ../include/class.h \
 ../include/client.h ../include/confitem.h ../include/fileio.h \
 ../include/debug.h ../include/floodspam.h ../include/identity.h \
 ../include/init.h ../include/ircsprintf.h ../include/logtype.h \
 ../include/maskitem.h ../include/../lib/regex/regex.h \
 ../include/memory.h ../include/modules.h ../include/motditem.h \
 ../include/parse.h ../include/s_bsd.h ../include/fd.h \
 ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/numeric.h ../include/dh.h ../include/zlink.h ../include/msg.h \
 ../include/protodef.h ../include/protocol.h ../include/s_bsd.h \
 ../include/s_conf.h
ssl.o: ssl.c ../include/struct.h ../include/config.h ../include/setup.h \
 ../include/ircpath.h ../include/defaults.h ../include/sys.h \
 ../include/../lib/zlib/zlib.h ../include/../lib/zlib/zconf.h \
 ../include/setup.h ../include/linebuf.h ../include/tools.h \
 ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/s_conf.h ../include/h.h \
 ../include/blalloc.h ../include/channel.h ../include/class.h \
 ../include/client.h ../include/confitem.h ../include/fileio.h \
 ../include/debug.h ../include/floodspam.h ../include/identity.h \
 ../include/init.h ../include/ircsprintf.h ../include/logtype.h \
 ../include/maskitem.h ../include/../lib/regex/regex.h \
 ../include/memory.h ../include/modules.h ../include/motditem.h \
 ../include/parse.h ../include/s_bsd.h ../include/fd.h \
 ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/dh.h
stats.o: stats.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/tools.h ../include/channel.h ../include/class.h \
 ../include/event.h ../include/listener.h ../include/modules.h \
 ../include/numeric.h ../include/fd.h ../include/s_bsd.h \
 ../include/s_conf.h ../include/fileio.h ../include/zlink.h
support.o: support.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/tools.h ../include/s_conf.h
textedit.o: textedit.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/tools.h ../include/s_conf.h ../include/chanmode.h
throttle.o: throttle.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/numeric.h ../include/h.h \
 ../include/blalloc.h ../include/channel.h ../include/class.h \
 ../include/client.h ../include/confitem.h ../include/fileio.h \
 ../include/debug.h ../include/floodspam.h ../include/identity.h \
 ../include/init.h ../include/ircsprintf.h ../include/logtype.h \
 ../include/maskitem.h ../include/../lib/regex/regex.h \
 ../include/memory.h ../include/modules.h ../include/motditem.h \
 ../include/parse.h ../include/s_bsd.h ../include/fd.h \
 ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/s_conf.h ../include/queue.h ../include/blalloc.h \
 ../include/throttle.h
tools.o: tools.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/tools.h ../include/blalloc.h
usermodes.o: usermodes.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/msg.h ../include/protodef.h ../include/protocol.h \
 ../include/s_conf.h ../include/usermode.h
watch.o: watch.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/s_conf.h ../include/h.h \
 ../include/blalloc.h ../include/channel.h ../include/class.h \
 ../include/client.h ../include/confitem.h ../include/fileio.h \
 ../include/debug.h ../include/floodspam.h ../include/identity.h \
 ../include/init.h ../include/ircsprintf.h ../include/logtype.h \
 ../include/maskitem.h ../include/../lib/regex/regex.h \
 ../include/memory.h ../include/modules.h ../include/motditem.h \
 ../include/parse.h ../include/s_bsd.h ../include/fd.h \
 ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h
whowas.o: whowas.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/numeric.h ../include/h.h \
 ../include/blalloc.h ../include/channel.h ../include/class.h \
 ../include/client.h ../include/confitem.h ../include/fileio.h \
 ../include/debug.h ../include/floodspam.h ../include/identity.h \
 ../include/init.h ../include/ircsprintf.h ../include/logtype.h \
 ../include/maskitem.h ../include/../lib/regex/regex.h \
 ../include/memory.h ../include/modules.h ../include/motditem.h \
 ../include/parse.h ../include/s_bsd.h ../include/fd.h \
 ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h
zlink.o: zlink.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/h.h ../include/blalloc.h \
 ../include/channel.h ../include/class.h ../include/client.h \
 ../include/confitem.h ../include/fileio.h ../include/debug.h \
 ../include/floodspam.h ../include/identity.h ../include/init.h \
 ../include/ircsprintf.h ../include/logtype.h ../include/maskitem.h \
 ../include/../lib/regex/regex.h ../include/memory.h ../include/modules.h \
 ../include/motditem.h ../include/parse.h ../include/s_bsd.h \
 ../include/fd.h ../include/resnew.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/internal.h ../include/../lib/adns/adns.h \
 ../include/../lib/adns/dlist.h ../include/s_service.h \
 ../include/s_user.h ../include/send.h ../include/sysvar.h \
 ../include/zlink.h
