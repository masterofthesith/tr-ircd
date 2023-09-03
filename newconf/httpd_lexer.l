/************************************************************************
 *   IRC - Internet Relay Chat, newconf/httpd_lexer.l
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
 * $Id: httpd_lexer.l,v 1.4 2003/07/01 11:01:19 tr-ircd Exp $
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
#include "httpd_parser.h"

#define YY_NO_UNPUT	1

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    if (!(result = conf_fbgets(buf, max_size, httpd_fbfile_in))) { \
    	conf_yy_fatal_error("input in flex scanner failed"); \
    }


int hlineno = 1;
char hlinebuf[BUFSIZE];
void hccomment (void);

%}

ws	[ \t]* 
digit	[0-9] 
comment	#.*
qstring \"[^\"\n]*[\"\n]

%%

"/*"            { hccomment(); }

\n.*            { strcpy(hlinebuf, yytext+1); hlineno++; yyless(1); }

{ws}            ;
{comment}       ;
{digit}+        { httpdlval.number = atoi(yytext); return NUMBER; }
{qstring}   	{ 
		httpdlval.string = yytext+1; 
		if(httpdlval.string[yyleng-2] != '"') 
		    logevent_call(LogSys.generalerror, "Unterminated character string"); 
		else 
		    httpdlval.string[yyleng-2] = '\0';
		return QSTRING; 
		}

httpd_config	{ return HTTPDCONFIG; }
listen		{ return LISTEN; }
ip		{ return IP; }
port		{ return PORT; }
host_header	{ return HOST_HEADER; }
index_page	{ return INDEX_PAGE; }
require		{ return REQUIRE; }
value		{ return VALUE; }
ssl		{ return SSL_ENABLE; }
policy		{ return POLICY; }
options		{ return OPTIONS; }
except		{ return EXCEPT; }
yes		{ return QYES; }
no		{ return QNO; }
allow		{ return QALLOW; }
deny 		{ return QDENY; }
.		{ return yytext[0]; }

<<EOF>>		{ yyterminate(); }

%%

void hccomment(void)
{
    int c;

    /* log(L_NOTICE, "got comment"); */
    while (1) {
        while ((c = input()) != '*' && c != EOF)
            if (c == '\n')
                ++hlineno;
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

