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

PROGS           = ircpasswd converter.sh

STATICLIBS      = 
IRCDLIBS        = $(SUBDIRLIBS) $(STATICLIBS) $(LDFLAGS)  -ldl -lpthread  

INCLUDES        = -I../include -Imd5 -Iregex
CPPFLAGS        = ${INCLUDES}  

SRC_0 = md5hash.c regmatch.c

SOURCES = $(SRC_0)
OBJECTS = $(SOURCES:.c=.o) regex.o

all: library

build: all

depend:
	@${MKDEP} $(INCLUDES) ${SOURCES} > .depend.tmp
	@sed -e '/^# DO NOT DELETE THIS LINE/,$$d' <Makefile >Makefile.depend
	@echo '# DO NOT DELETE THIS LINE!!!' >>Makefile.depend
	@echo '# make depend needs it.' >>Makefile.depend
	@cat .depend.tmp >>Makefile.depend
	@$(MV) Makefile.depend Makefile
	@$(RM) -f .depend.tmp

library: $(OBJECTS)
	${MAKE} -C uuid library
	$(RM) -f ilib.a
	$(AR) cqv ilib.a $(OBJECTS) md5/md5.o uuid/*.o
	$(RANLIB) ilib.a

clean:
	$(RM) -f $(OBJECTS) *.a *.o *~ core
	${MAKE} -C uuid clean

distclean: clean
	${MAKE} -C uuid distclean
	$(RM) -f Makefile

regex.o:
	$(CC) -Iregex -c regex/regex.c -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

install:

makesums:
	@echo "Creating .md5sums..."
	@$(RM) -f .md5sums
	@for i in $(SOURCES); do \
		$(MD5SUMS) $$i >> .md5sums; \
	done


# DO NOT DELETE THIS LINE!!!
# make depend needs it.
md5hash.o: md5hash.c ../include/struct.h ../include/config.h \
 ../include/setup.h ../include/ircpath.h ../include/defaults.h \
 ../include/sys.h ../include/../lib/zlib/zlib.h \
 ../include/../lib/zlib/zconf.h ../include/setup.h ../include/linebuf.h \
 ../include/tools.h ../include/capabflag.h ../include/ircd_defs.h \
 ../include/../lib/uuid/uuidP.h ../include/../lib/uuid/uuid.h \
 ../include/common.h ../include/sys.h ../include/s_user.h \
 ../include/s_conf.h ../lib/md5/md5.h
regmatch.o: regmatch.c ../include/struct.h ../include/config.h \
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
 ../include/s_conf.h regex/regex.h
