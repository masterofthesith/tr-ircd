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

INCLUDES        = -I../../include
CPPFLAGS        = ${INCLUDES}  

SRC_0 = clear.c compare.c copy.c gen_uuid.c isnull.c pack.c parse.c unpack.c unparse.c uuid_time.c

SOURCES = $(SRC_0)
OBJECTS = $(SOURCES:.c=.o)

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
#	$(RM) -f ilibuuid.a
#	$(AR) cqv ilibuuid.a $(OBJECTS)
#	$(RANLIB) ilibuuid.a

clean:
	$(RM) -f *.a *.o *~ core

distclean: clean
	$(RM) -f Makefile

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

install:

makesums:
	@echo "Creating .md5sums..."
	@$(RM) -f .md5sums
	@for i in $(SOURCES); do \
		$(MD5SUMS) $$i >> .md5sums; \
	done


# DO NOT DELETE THIS LINE
# make depend needs it.
