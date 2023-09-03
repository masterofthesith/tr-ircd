/***********************************************************************
 *   IRC - Internet Relay Chat, newconf/maskfile_parser.y
 *
 *   Copyright (C)2000-2003 TR-IRCD Development
 *   Copyright (C) 2000 Diane Bruce <db@db.net>
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Computing Center
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
 */

%{

#define YY_NO_UNPUT

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "class.h"
#include "fileio.h"
#include "ircsprintf.h"
#include "s_conf.h"
#include "s_bsd.h"
#include "listener.h"
#include "language.h"

extern int mcount;

int yyparse();

static aMaskItem *ami;
static char *host = NULL;
static char *user = NULL;
static char *reason = NULL;

%}

%union {
        int  number;
        char *string;
}

%token MASKTYPE
%token KLINE
%token QUARANTINE
%token GECOSDENY
%token JUPITER
%token ZAP
%token USERHOST
%token RESFIELD
%token NAMEFIELD
%token QSTRING
%token NUMBER

%type   <string>   QSTRING
%type 	<number>   NUMBER

%%
conf:   
        | conf conf_item
        ;

conf_item: 	all_items | error '}' | error ';' ;

all_items:	MASKTYPE ':' KLINE '{' mask_items '}' ';'
{
    ami = create_maskitem(me.name, host, user, MASKITEM_KLINE_CONFIG, 0);
    if (ami)
    	DupString(ami->reason, reason);
}
		|
		MASKTYPE ':' QUARANTINE '{' mask_items_name '}' ';'
{
    ami = create_maskitem(me.name, host, NULL, MASKITEM_QUARANTINE_CONFIG, 0);
    if (ami)
        DupString(ami->reason, reason);
}
     		|
                MASKTYPE ':' JUPITER '{' mask_items_name '}' ';'
{
    ami = create_maskitem(me.name, host, NULL, MASKITEM_JUPITER_CONFIG, 0);
    if (ami)
        DupString(ami->reason, reason);
}

		|
		MASKTYPE ':' ZAP '{' mask_items_name '}' ';'
{
    ami = create_maskitem(me.name, host, NULL, MASKITEM_ZAPLINE_CONFIG, 0);
    if (ami)
        DupString(ami->reason, reason);
}

                |
                MASKTYPE ':' GECOSDENY '{' mask_items_name '}' ';'
{
    ami = create_maskitem(me.name, host, NULL, MASKITEM_GECOS_CONFIG, 0);
    if (ami)
        DupString(ami->reason, reason);
};


mask_items:	mask_items mask_item |
		mask_item ;

mask_item:	mask_user | mask_reason | error ;


mask_items_name:	mask_items_name mask_item_name |
			mask_item_name ;

mask_item_name:	mask_name | mask_reason | error ;

mask_user:      USERHOST '='  QSTRING ';'
{
    char *p;
    char *new_user;
    char *new_host;

    MyFree(user);
    MyFree(host);
    if((p = strchr(yylval.string,'@'))) {
	*p = '\0';
	DupString(new_user,yylval.string);
	user = new_user;
	p++;
	DupString(new_host,p);
	host = new_host;
    } else {
   	DupString(new_host, yylval.string);
   	DupString(new_user,"*");
	user = new_user;
    }
};

mask_name:	NAMEFIELD '=' QSTRING ';'
{
    MyFree(host);
    DupString(host, yylval.string);
};

mask_reason:	RESFIELD '=' QSTRING ';'
{
    MyFree(reason);
    DupString(reason, yylval.string);
};

