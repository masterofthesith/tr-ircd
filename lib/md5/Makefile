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
BITFLAGS	= 
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

MD5SUM_EXE      = md5sum

PROGS           = $(MD5SUM_EXE)

IRCDLIBS        = $(SUBDIRLIBS)  -ldl -lpthread 

INCLUDES        = -I../../include  
CPPFLAGS        = ${INCLUDES}
MODFLAGS        = 

OBJS = main.o md5.o error.o xmalloc-kludge.o getline.o version-etc.o closeout.o getopt1.o getopt.o

build: $(MD5SUM_EXE)

$(MD5SUM_EXE): 	$(OBJS)
	$(CC) ${LDFLAGS} ${BITFLAGS} -o $(MD5SUM_EXE) $(LFLAGS) $(OBJS)

clean:
	$(RM) -f *.o core $(MD5SUM_EXE) *~

distclean:
	$(RM) -f *.o core $(MD5SUM_EXE) config.h Makefile

depend:

install:
	@echo "Installing md5 utilities"
	$(INSTALL) -d ${libexecdir}
	$(INSTALL) $(PROGS) ${libexecdir}
	@echo "Installing md5 includes"
	$(INSTALL) -d ${prefix}/include/md5 
	$(INSTALL) *.h ${prefix}/include/md5
