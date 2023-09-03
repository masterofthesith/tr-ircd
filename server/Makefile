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
libdir          = ${exec_prefix}/lib
sysconfdir      = ${prefix}/etc
localstatedir   = ${prefix}/var

IRCD_EXE        = ircd          
MD5SUM_EXE      = md5sum

PROGS           = $(IRCD_EXE) $(MD5SUM_EXE)

STATICLIBS      = 
IRCDLIBS        = $(SUBDIRLIBS) $(STATICLIBS)  -ldl -lpthread  

INCLUDES        = -I../include  
CPPFLAGS        = ${INCLUDES}  

SRV_0 = s_accept.c s_auth.c s_boot.c s_bsd.c s_bsd_devpoll.c s_bsd_epoll.c s_bsd_kqueue.c
SRV_1 = s_bsd_poll.c s_bsd_select.c s_bsd_sigio.c s_conf.c s_dcc.c s_debug.c
SRV_2 = s_err.c s_exit.c s_flood.c s_kline.c s_lev.c s_listener.c
SRV_3 = s_misc.c s_motd.c s_numeric.c s_packet.c s_query.c s_send.c s_serv.c
SRV_4 = s_servauth.c s_service.c s_signal.c s_silence.c s_user.c s_validity.c

SOURCES = $(SRV_0) $(SRV_1) $(SRV_2) $(SRV_3) $(SRV_4)

OBJECTS = $(SOURCES:.c=.o)

all: server.a

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
	$(RM) -f *.o server.a *~ core

distclean: clean
	$(RM) -f Makefile

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

server.a: $(OBJECTS)
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
