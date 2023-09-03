/***********************************************************************
 *   IRC - Internet Relay Chat, newconf/ircd_parser.y
 *
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

int yyparse();

int   class_class_var;
int   class_ping_time_var;
int   class_conn_freq_var;
int   class_max_links_var;
int   class_sendq_var;

%}

%union
{
  int number;
  char *string;
}


%token CLASS
%token PING_TIME
%token CONNECTFREQ
%token MAX_LINKS
%token SENDQ
%token QSTRING 
%token NUMBER

%token  SECONDS MINUTES HOURS DAYS WEEKS MONTHS YEARS DECADES CENTURIES MILLENNIA
%token  BYTES KBYTES MBYTES 

%type   <string>   QSTRING
%type   <number>   NUMBER
%type   <number>   timespec 
%type   <number>   timespec_
%type   <number>   sizespec 
%type   <number>   sizespec_

%%
conf:   
        | conf conf_item
        ;

conf_item: class_entry  | error ';' | error '}' ;

timespec_: { $$ = 0; } | timespec;
timespec:       NUMBER timespec_
                {
                        $$ = $1 + $2;
                }
                | NUMBER SECONDS timespec_
                {
                        $$ = $1 + $3;
                }
                | NUMBER MINUTES timespec_
                {
                        $$ = $1 * 60 + $3;
                }
                | NUMBER HOURS timespec_
                {
                        $$ = $1 * 60 * 60 + $3;
                }
                | NUMBER DAYS timespec_
                {
                        $$ = $1 * 60 * 60 * 24 + $3;
                }
                | NUMBER WEEKS timespec_
                {
                        $$ = $1 * 60 * 60 * 24 * 7 + $3;
                }
                ;

sizespec_:      { $$ = 0; } | sizespec;
sizespec:       NUMBER sizespec_ { $$ = $1 + $2; }
                | NUMBER BYTES sizespec_ { $$ = $1 + $3; }
                | NUMBER KBYTES sizespec_ { $$ = $1 * 1024 + $3; }
                | NUMBER MBYTES sizespec_ { $$ = $1 * 1024 * 1024 + $3; }
                ;


/***************************************************************************
 *  section class
 ***************************************************************************/

class_entry:    CLASS 
{
    class_class_var = 0;
    class_ping_time_var = 0;
    class_conn_freq_var = 0;
    class_max_links_var = 0;
    class_sendq_var = 0;
}
  '{' class_items '}' ';'
{
    add_class(class_class_var, class_ping_time_var, class_conn_freq_var,
              class_max_links_var, class_sendq_var);
    class_ping_time_var = 0;
};

class_items:    class_items class_item |
                class_item ;

class_item:     class_class |
                class_ping_time |
                class_conn_freq |
                class_max_links |
                class_sendq |
		error ;

class_class:   		CLASS '=' NUMBER ';' 
{
    class_class_var = $3;
};

class_ping_time:        PING_TIME '=' timespec ';'
{
    class_ping_time_var = $3;
};

class_conn_freq:     	CONNECTFREQ '=' timespec ';'
{
    class_conn_freq_var = $3;
};

class_max_links:       	MAX_LINKS '=' NUMBER ';'
{
    class_max_links_var = $3;
};

class_sendq:    	SENDQ '=' sizespec ';'
{
    class_sendq_var = $3;
};


