/***********************************************************************
 *   IRC - Internet Relay Chat, newconf/proxymon_parser.y
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

extern int pmcount;

int yyparse();

%}

%union {
        int  number;
        char *string;
}

%token	PM_CONFIG
%token	PM_MAIN
%token  PM_SCAN
%token  PM_FD_LIMIT
%token  PM_DNSBLHOST
%token  PM_MAX_READ
%token  PM_TIMEOUT
%token  PM_NEGFAIL_NOTICES
%token  PM_SCAN_IP
%token  PM_SCAN_PORT
%token	PM_BIND_IP
%token  PM_ACTION
%token  PM_KLINE PM_AUTOKILL PM_GNOTICE

%token  BYTES KBYTES MBYTES
%token  SECONDS MINUTES HOURS
%token  QSTRING
%token  NUMBER
%token  QYES QNO QLOG

%type   <string>   QSTRING
%type 	<number>   NUMBER
%type   <number>   sizespec
%type   <number>   sizespec_
%type   <number>   timespec
%type   <number>   timespec_


%%

conf:   
        | conf conf_item
        ;

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
                };


sizespec_:      { $$ = 0; } | sizespec;
sizespec:       NUMBER sizespec_ { $$ = $1 + $2; }
                | NUMBER BYTES sizespec_ { $$ = $1 + $3; }
                | NUMBER KBYTES sizespec_ { $$ = $1 * 1024 + $3; }
                | NUMBER MBYTES sizespec_ { $$ = $1 * 1024 * 1024 + $3; }
                ;


conf_item: 	proxymon_config | error '}' | error ';' ;

proxymon_config:	PM_CONFIG '{' pm_configuration '}' ';' ;

pm_configuration:	pm_config_main pm_config_scanner | error ;

pm_config_main:		PM_MAIN '{' pm_config_main_items '}' ';' ;
pm_config_scanner:	PM_SCAN '{' pm_config_scanner_items '}' ';' ;

pm_config_main_items:	pm_config_main_items pm_config_main_item |
			pm_config_main_item ;

pm_config_scanner_items: pm_config_scanner_items pm_config_scanner_item |
			pm_config_scanner_item ;

pm_config_main_item:	pm_cf_main_fd_limit | pm_cf_main_dnsbl |
			pm_cf_main_max_read | pm_cf_main_timeout | 
			pm_cf_main_negfail_notices | pm_cf_main_action | error ;

pm_config_scanner_item:	pm_cf_scanner_scan_ip | pm_cf_scanner_scan_port |
			pm_cf_scanner_bind_ip | error ;

pm_cf_main_fd_limit:	PM_FD_LIMIT '=' NUMBER ';' 
{
    ScannerConf.fd_limit = $3;
};

pm_cf_main_dnsbl:	PM_DNSBLHOST '=' QSTRING ';'
{
    MyFree(ScannerConf.dnsblhost);
    DupString(ScannerConf.dnsblhost, yylval.string);
};

pm_cf_main_max_read:	PM_MAX_READ '=' sizespec ';'
{
    ScannerConf.max_read = $3;
};

pm_cf_main_timeout:	PM_TIMEOUT '=' timespec ';'
{
    ScannerConf.timeout = $3;
};

pm_cf_main_negfail_notices:	PM_NEGFAIL_NOTICES '=' QYES ';'
{
    ScannerConf.negfail_notices = 2;
}
				|
				PM_NEGFAIL_NOTICES '=' QNO ';'
{
    ScannerConf.negfail_notices = 0;
}
				|
				PM_NEGFAIL_NOTICES '=' QLOG ';'
{  
    ScannerConf.negfail_notices = 1;
};

pm_cf_main_action:	PM_ACTION '=' pm_action_elems ';' ;

pm_action_elems:	pm_action_elem ',' pm_action_elems | pm_action_elem | error ;

pm_action_elem:		PM_KLINE
{
    ScannerConf.action |= PROXYMON_KLINE;
}
			|
			PM_AUTOKILL
{
    ScannerConf.action |= PROXYMON_AUTOKILL;
}
			|
			PM_GNOTICE
{
    ScannerConf.action |= PROXYMON_GNOTICE;
};

pm_cf_scanner_scan_ip:	PM_SCAN_IP '=' QSTRING ';'
{
    MyFree(ScannerConf.scan_ip);
    DupString(ScannerConf.scan_ip, yylval.string);
};

pm_cf_scanner_scan_port: PM_SCAN_PORT '=' NUMBER ';'
{
    ScannerConf.scan_port = $3;
};

pm_cf_scanner_bind_ip:	PM_BIND_IP '=' QSTRING ';'
{
    MyFree(ScannerConf.bind_ip);
    DupString(ScannerConf.bind_ip, yylval.string);
};

