/************************************************************************
 *   IRC - Internet Relay Chat, newconf/ircd_lexer.l
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
 * $Id: classes_lexer.l,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
 */

/* *INDENT-OFF* */

%{

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "class.h"
#include "fileio.h"
#include "ircsprintf.h"
#include "s_conf.h"
#include "classes_parser.h"

#define YY_NO_UNPUT	1

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    if (!(result = conf_fbgets(buf, max_size, conf_fbfile_in))) { \
    	conf_yy_fatal_error("input in flex scanner failed"); \
    } 

extern int lineno;
extern char linebuf[];

void cccomment(void);

%}

ws        [ \t]*
digit     [0-9]
comment   #.*
qstring   \"[^\"\n]*[\"\n]
include   \.include{ws}(\<.*\>|\".*\")

%%
"/*"	        { cccomment(); }

\n.*	        { strcpy(linebuf, yytext+1); lineno++; yyless(1); }

{ws}            ;
{comment}       ;

{digit}+        { classeslval.number = atoi(yytext); return NUMBER; }

{qstring}	{ classeslval.string = yytext+1;
		  if(classeslval.string[yyleng-2] != '"')
		    logevent_call(LogSys.generalerror, "Unterminated character string");
		  else
		    classeslval.string[yyleng-2] = '\0'; /* remove close quote */
		  return QSTRING;
		}


class			{ return CLASS; }
pingtime		{ return PING_TIME; }
connectfreq		{ return CONNECTFREQ; }
max_links		{ return MAX_LINKS; }
sendq			{ return SENDQ; }

millennia		{ return MILLENNIA; }
millennium		{ return MILLENNIA; }
centuries		{ return CENTURIES; }
century			{ return CENTURIES; }
decades			{ return DECADES; }
decade			{ return DECADES; }
years			{ return YEARS; }
year			{ return YEARS; }
months			{ return MONTHS; }
month			{ return MONTHS; }
weeks			{ return WEEKS; }
week			{ return WEEKS; }
days			{ return DAYS; }
day			{ return DAYS; }
hours			{ return HOURS; }
hour			{ return HOURS; }
minutes			{ return MINUTES; }
minute			{ return MINUTES; }
seconds			{ return SECONDS; }
second			{ return SECONDS; }

bytes			{ return BYTES; }
byte			{ return BYTES; }
kilobytes		{ return KBYTES; }
kilobyte		{ return KBYTES; }
kbytes			{ return KBYTES; }
kbyte			{ return KBYTES; }
kb			{ return KBYTES; }
megabytes		{ return MBYTES; }
megabyte		{ return MBYTES; }
mbytes			{ return MBYTES; }
mbyte			{ return MBYTES; }
mb			{ return MBYTES; }

.		        { return yytext[0]; }
<<EOF>>     { yyterminate(); }

%%

void cccomment(void)
{
    int c;

    /* log(L_NOTICE, "got comment"); */
    while (1) {
        while ((c = input()) != '*' && c != EOF)
            if (c == '\n')
                ++lineno;
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

