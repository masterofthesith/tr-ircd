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
sysconfdir      = ${prefix}/etc
localstatedir   = ${prefix}/var

IRCD_EXE        = ircd          
MD5SUM_EXE      = md5sum

PROGS           = $(IRCD_EXE) $(MD5SUM_EXE)

IRCDLIBS        = $(SUBDIRLIBS)  -ldl -lpthread   

INCLUDES        = -I../../include  
CPPFLAGS        = ${INCLUDES}  
 
default:	all

BASE_SRCS = check.c event.c general.c parse.c query.c reply.c setup.c \
		transmit.c types.c


SRCS = ${BASE_SRCS} ${SELECT_SRC}

OBJS = ${SRCS:.c=.o}

all: libadns.a

build: all

libadns.a: ${OBJS}
		$(RM) -f $@
		${AR} cq $@ ${OBJS}
		${RANLIB} $@

# this is really the default rule for c files
.c.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

.PHONY: depend clean distclean

install: 

depend:
	@${MKDEP} $(INCLUDES) ${SRCS} > .depend.tmp 
	@sed -e '/^# DO NOT DELETE THIS LINE/,$$d' <Makefile >Makefile.depend
	@echo '# DO NOT DELETE THIS LINE!!!' >>Makefile.depend
	@echo '# make depend needs it.' >>Makefile.depend
	@cat .depend.tmp >>Makefile.depend
	@mv Makefile.depend Makefile
	@rm -f .depend.tmp

clean:
	${RM} -f *.o *.exe *~ libadns.a

lint:

distclean: clean
	${RM} -f Makefile version.c.last

# DO NOT DELETE THIS LINE!!!
# make depend needs it.
check.o: check.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/fileio.h internal.h \
 adns.h ../../include/common.h ../../include/sys.h dlist.h
event.o: event.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/fileio.h internal.h \
 adns.h ../../include/common.h ../../include/sys.h dlist.h \
 ../../include/fd.h ../../include/resnew.h ../../include/fileio.h \
 ../../include/../lib/adns/adns.h ../../include/../lib/adns/internal.h \
 ../../include/h.h ../../include/blalloc.h ../../include/channel.h \
 ../../include/class.h ../../include/client.h ../../include/confitem.h \
 ../../include/debug.h ../../include/floodspam.h ../../include/identity.h \
 ../../include/init.h ../../include/ircsprintf.h ../../include/logtype.h \
 ../../include/maskitem.h ../../include/../lib/regex/regex.h \
 ../../include/memory.h ../../include/modules.h ../../include/motditem.h \
 ../../include/parse.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/s_service.h ../../include/s_user.h ../../include/send.h \
 ../../include/sysvar.h ../../include/s_bsd.h tvarith.h
general.o: general.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/fileio.h internal.h \
 adns.h ../../include/common.h ../../include/sys.h dlist.h \
 ../../include/memory.h
parse.o: parse.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/fileio.h \
 ../../include/ircsprintf.h internal.h adns.h ../../include/common.h \
 ../../include/sys.h dlist.h
query.o: query.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/fileio.h internal.h \
 adns.h ../../include/common.h ../../include/sys.h dlist.h \
 ../../include/ircsprintf.h ../../include/memory.h ../../include/s_user.h
reply.o: reply.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/memory.h \
 ../../include/fileio.h internal.h adns.h ../../include/common.h \
 ../../include/sys.h dlist.h
setup.o: setup.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/memory.h \
 ../../include/fileio.h internal.h adns.h ../../include/common.h \
 ../../include/sys.h dlist.h ../../include/s_bsd.h ../../include/fd.h \
 ../../include/resnew.h ../../include/fileio.h \
 ../../include/../lib/adns/adns.h ../../include/../lib/adns/internal.h \
 ../../include/s_user.h ../../include/ircsprintf.h
transmit.o: transmit.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/fileio.h internal.h \
 adns.h ../../include/common.h ../../include/sys.h dlist.h tvarith.h
types.o: types.c ../../include/struct.h ../../include/config.h \
 ../../include/setup.h ../../include/ircpath.h ../../include/defaults.h \
 ../../include/sys.h ../../include/../lib/zlib/zlib.h \
 ../../include/../lib/zlib/zconf.h ../../include/../include/setup.h \
 ../../include/linebuf.h ../../include/tools.h ../../include/capabflag.h \
 ../../include/ircd_defs.h ../../include/../lib/uuid/uuidP.h \
 ../../include/../lib/uuid/uuid.h ../../include/fileio.h internal.h \
 adns.h ../../include/common.h ../../include/sys.h dlist.h \
 ../../include/ircsprintf.h ../../include/s_user.h
