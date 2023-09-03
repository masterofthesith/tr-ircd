/***********************************************************************
 *   IRC - Internet Relay Chat, newconf/httpd_parser.y
 *
 *   Copyright (C)2000-2003 TR-IRCD Development
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

extern int hcount;

int yyparse ();

static char *listener_address;
static char *host_header;
int *listener_ports;
int hport_count = 0;
int hport_count_max = 0;
int hport_type = 0;

%}

%union
{
  int number;
  char *string;
}

%token HTTPDCONFIG
%token LISTEN
%token IP
%token PORT
%token HOST_HEADER
%token INDEX_PAGE
%token REQUIRE
%token VALUE
%token SSL_ENABLE
%token OPTIONS
%token POLICY
%token EXCEPT
%token QYES
%token QNO
%token QALLOW
%token QDENY
%token QSTRING
%token NUMBER

%type <string> QSTRING 
%type <number> NUMBER 

%% 

conf:	|
	conf_item;


conf_item:	httpd_config | 
		error '}' | 
		error ';';

httpd_config:	HTTPDCONFIG '{' httpd_configuration '}' ';';

httpd_configuration:	httpd_configuration hconfig_elem | hconfig_elem;

hconfig_elem:	httpd_listen | httpd_index_page | httpd_options | error;

httpd_listen:	LISTEN
{
  listener_address = NULL;
  host_header = NULL;
}

'{' listen_items '}' ';'

{
  MyFree (listener_address);
  listener_address = NULL;
  MyFree (host_header);
  host_header = NULL;
};

listen_items:	listen_items listen_item | 
		listen_item;

listen_item:
{
  listener_ports = (int *) MyMalloc (10 * sizeof (int));
  memset (listener_ports, 0, 10 * sizeof (int));
  hport_count = 0;
  hport_count_max = 10;
  hport_type = 0;
}

'{' listen_elems '}' ';'

{
  int i;
  uuid_t u;
  hport_type |= LISTENER_HTTP;
  uuid_generate(u);
  for (i = 0; i < hport_count; i++)
    {
      add_listener (listener_ports[i], listener_address, hport_type, u, 
      		    host_header);
      listener_ports[i] = 0;
    }
  MyFree (listener_ports);
  listener_ports = NULL;
};

listen_elems:	listen_elems listen_elem | 
		listen_elem;

listen_elem:	listen_port | listen_address | listen_host | option_ssl | 
		error;

listen_port:	PORT '=' hport_items ';';

hport_items:	hport_items ',' hport_item | 
		hport_item;

hport_item:	NUMBER
{
  if (hport_count >= hport_count_max)
    {
      hport_count_max += 5;
      listener_ports = (int *) MyRealloc (listener_ports, hport_count_max * sizeof (int));
      memset (listener_ports + hport_count, 0, 5 * sizeof (int));
    }
  listener_ports[hport_count] = $1;
  hport_count++;
};

listen_address:	IP '=' QSTRING ';'
{
  MyFree (listener_address);
  DupString (listener_address, yylval.string);
};

listen_host:	HOST_HEADER '=' QSTRING ';'
{
  MyFree (host_header);
  DupString (host_header, yylval.string);
};

option_ssl:     SSL_ENABLE ';'
{
  hport_type |= LISTENER_CLIENT_SSL;
};

httpd_index_page:       INDEX_PAGE '{' index_items '}' ';';

index_items:    index_items index_item |
                index_item;

index_item:     index_require | index_value |
                error;

index_require:  REQUIRE '=' QYES ';'
{ 
  HttpdConf.require_index = 1;
}
		|
                REQUIRE '=' QNO ';'
{ 
  HttpdConf.require_index = 0;
};

index_value:    VALUE '=' QSTRING ';'
{
  MyFree (HttpdConf.index_file);
  DupString (HttpdConf.index_file, yylval.string);
  HttpdConf.index_file_len = strlen (yylval.string);
};

httpd_options:  OPTIONS '{' option_items '}' ';';

option_items:   option_items option_item |
                option_item;

option_item:    option_policy | option_except |
                error;

option_policy:  POLICY '=' QALLOW ';'
{
        HttpdConf.policy = 1;   /* HTTP_ALLOW */
}
                |
                POLICY '=' QDENY ';'
{
        HttpdConf.policy = 0;   /* HTTP_DENY */
};

option_except:          EXCEPT '{' option_except_items '}' ';';

option_except_items:    option_except_items option_except_item |
                        option_except_item;

option_except_item:     except_host_ip |
                        error;

except_host_ip: QSTRING ';'
{
        if(strcmp(yylval.string, "*") == 0)
                HttpdConf.http_except_any = 1;
        else {
                dlink_node *ptr;
                char *address;
                ptr = make_dlink_node();
                DupString(address, yylval.string);
                dlinkAdd(address, ptr, &http_except_list);
        }

};

