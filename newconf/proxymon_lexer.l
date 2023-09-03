/************************************************************************
 *   IRC - Internet Relay Chat, newconf/proxymon_lexer.l
 *
 *   Copyright (C)2000-2003 TR-IRCD Development
 *   Copyright (C) 2000 Diane Bruce <db@db.net>
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                    University of Oulu, Co Center
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: proxymon_lexer.l,v 1.4 2003/07/01 11:01:19 tr-ircd Exp $
 */

%{

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "class.h"
#include "fileio.h"
#include "ircsprintf.h"
#include "s_conf.h"
#include "proxymon_parser.h"

#define YY_NO_UNPUT	1

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    if (!(result = conf_fbgets(buf, max_size, pmconf_fbfile_in))) { \
    	conf_yy_fatal_error("input in flex scanner failed"); \
    } 


int pmlineno=1;
char pmlinebuf[BUFSIZE];
void pccomment(void);
%}

ws        [ \t]*
digit     [0-9]
comment   #.*
qstring   \"[^\"\n]*[\"\n]

%%
"/*"	        { pccomment(); }

\n.*	        { strcpy(pmlinebuf, yytext+1); pmlineno++; yyless(1); }

{ws}            ;
{comment}       ;

{digit}+        { proxymonlval.number = atoi(yytext); return NUMBER; }

{qstring}	{ proxymonlval.string = yytext+1;
		  if(proxymonlval.string[yyleng-2] != '"')
		    logevent_call(LogSys.generalerror, "Unterminated character string");
		  else
		    proxymonlval.string[yyleng-2] = '\0'; /* remove close quote */
		  return QSTRING;
		}

proxymon_config		{ return PM_CONFIG; }
main			{ return PM_MAIN; }
fd_limit		{ return PM_FD_LIMIT; }
dnsbl_hostname		{ return PM_DNSBLHOST; }
max_read		{ return PM_MAX_READ; }
timeout			{ return PM_TIMEOUT; }
negfail_notices		{ return PM_NEGFAIL_NOTICES; }
scanner			{ return PM_SCAN; }
action			{ return PM_ACTION; }
kline			{ return PM_KLINE; }
autokill		{ return PM_AUTOKILL; }
global_notice		{ return PM_GNOTICE; }
check_ip		{ return PM_SCAN_IP; }
check_port		{ return PM_SCAN_PORT; }
bind_ip			{ return PM_BIND_IP; }

yes                     { return QYES; }                 
no                      { return QNO; } 
log			{ return QLOG; }

hours                   { return HOURS; }
hour                    { return HOURS; }
minutes                 { return MINUTES; }
minute                  { return MINUTES; }
seconds                 { return SECONDS; }
second                  { return SECONDS; }

bytes                   { return BYTES; }
byte                    { return BYTES; }
kilobytes               { return KBYTES; }
kilobyte                { return KBYTES; }
kbytes                  { return KBYTES; }
kbyte                   { return KBYTES; }
kb                      { return KBYTES; }
megabytes               { return MBYTES; }
megabyte                { return MBYTES; }
mbytes                  { return MBYTES; }
mbyte                   { return MBYTES; }
mb                      { return MBYTES; }

.		        { return yytext[0]; }
<<EOF>>     { yyterminate(); }

%%

void pccomment(void)
{
    int c;

    /* log(L_NOTICE, "got comment"); */
    while (1) {
        while ((c = input()) != '*' && c != EOF)
            if (c == '\n')
                ++pmlineno;
        if (c == '*') {
            while ((c = input()) == '*');
            if (c == '/')
                break;
        }
        if (c == EOF) {
            YY_FATAL_ERROR("EOF in comment");
            /* XXX hack alert this disables
             * the stupid unused function warning
             * gcc generates
             */
            if (1 == 0)
                yy_fatal_error("EOF in comment");
            break;
        }
    }
}

