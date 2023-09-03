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
AWK		= /usr/bin/awk
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

LEXICOS = classes_lexer.l httpd_lexer.l ircd_lexer.l maskfile_lexer.l proxymon_lexer.l
GRAMMAR = classes_parser.y httpd_parser.y ircd_parser.y maskfile_parser.y proxymon_parser.y

SSRCS_0 = classes_parser.c httpd_parser.c ircd_parser.c maskfile_parser.c proxymon_parser.c 
SSRCS_1 = classes_lexer.c httpd_lexer.c ircd_lexer.c maskfile_lexer.c proxymon_lexer.c
SSRCS_2 = config.c dumpconf.c

SOURCES = $(SSRCS_0) $(SSRCS_1) $(SSRCS_2)

SCANNER = $(LEXICOS:.l=.c)
PARSERS = $(GRAMMAR:.y=.c)
OBJECTS = $(SOURCES:.c=.o)

all: config.a

build: all

depend:

clean:
	$(RM) -f *.o *~ *.tab.* lex.* *parser.c *.h *lexer.c *.a *.lo

distclean: clean         
	$(RM) -f Makefile

.SUFFIXES: .lo

.l.c:
	${LEX} -i -o$@ -P`echo "$@" | awk -F_ {'print $$1'}` $<
.y.c:
	${YACC} -d $< -o $@ -p `echo "$@" | awk -F_ {'print $$1'}`

.c.o:
	${CC} ${CFLAGS} $(INCLUDES) -I. -c $< -o $@

config.a: $(OBJECTS)    
	$(AR) rcv $@ $(OBJECTS)
	$(RANLIB) $@

install: build

.PHONY: depend clean distclean

# DO NOT DELETE THIS LINE!!!        
# make depend needs it.    

