/************************************************************************
 *   IRC - Internet Relay Chat, newconf/maskfile_lexer.l
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
 * $Id: maskfile_lexer.l,v 1.4 2003/07/01 11:01:19 tr-ircd Exp $
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
#include "maskfile_parser.h"

#define YY_NO_UNPUT	1

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    if (!(result = conf_fbgets(buf, max_size, mask_fbfile_in))) { \
    	conf_yy_fatal_error("input in flex scanner failed"); \
    } 


int mlineno=1;
char mlinebuf[BUFSIZE];
void ccomment(void);
%}

ws        [ \t]*
digit     [0-9]
comment   #.*
qstring   \"[^\"\n]*[\"\n]

%%
"/*"	        { ccomment(); }

\n.*	        { strcpy(mlinebuf, yytext+1); mlineno++; yyless(1); }

{ws}            ;
{comment}       ;

{digit}+        { maskfilelval.number = atoi(yytext); return NUMBER; }

{qstring}	{ maskfilelval.string = yytext+1;
		  if(maskfilelval.string[yyleng-2] != '"')
		    logevent_call(LogSys.generalerror, "Unterminated character string");
		  else
		    maskfilelval.string[yyleng-2] = '\0'; /* remove close quote */
		  return QSTRING;
		}

mask			{ return MASKTYPE; }
kline			{ return KLINE; }
quarantine		{ return QUARANTINE; }
gecos_deny		{ return GECOSDENY; }
channel_jupiter		{ return JUPITER; }
zap_address		{ return ZAP; }

userhost		{ return USERHOST; }
reason			{ return RESFIELD; }
masktext		{ return NAMEFIELD; }

.		        { return yytext[0]; }
<<EOF>>     { yyterminate(); }

%%

