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
 * $Id: ircd_lexer.l,v 1.13 2004/07/12 09:14:59 tr-ircd Exp $
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
#include "ircd_parser.h"

#define YY_NO_UNPUT	1

#undef YY_INPUT
#define YY_FATAL_ERROR(msg) conf_yy_fatal_error(msg)
#define YY_INPUT(buf,result,max_size) \
    if (!(result = conf_fbgets(buf, max_size, conf_fbfile_in))) { \
    	conf_yy_fatal_error("input in flex scanner failed"); \
    } 
#define MAX_INCLUDE_DEPTH 10


int lineno=1;
char linebuf[BUFSIZE];
int include_stack_ptr=0;
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int lineno_stack[MAX_INCLUDE_DEPTH];
FBFILE* inc_fbfile_in[MAX_INCLUDE_DEPTH];
void ccomment(void);
void cinclude(void);
int ieof(void);
%}

ws        [ \t]*
digit     [0-9]
comment   #.*
qstring   \"[^\"\n]*[\"\n]
include   \.include{ws}(\<.*\>|\".*\")

%%
{include}       { cinclude(); }
"/*"	        { ccomment(); }

\n.*	        { strcpy(linebuf, yytext+1); lineno++; yyless(1); }

{ws}            ;
{comment}       ;

{digit}+        { ircdlval.number = atoi(yytext); return NUMBER; }

{qstring}	{ ircdlval.string = yytext+1;
		  if(ircdlval.string[yyleng-2] != '"')
		    logevent_call(LogSys.generalerror, "Unterminated character string");
		  else
		    ircdlval.string[yyleng-2] = '\0'; /* remove close quote */
		  return QSTRING;
		}


serverinfo		{ return SERVERINFO; }
name			{ return NAME; }
description		{ return DESCRIPTION; }
networkname		{ return NETWORKNAME; }
address			{ return ADDRESS; }
address6		{ return ADDRESS6; }
restartpass		{ return RESTARTPASS; }
diepass			{ return DIEPASS; }
displaypass		{ return DISPLAYPASS; }
identity		{ return IDENTITY; }
networkdesc		{ return NETWORKDESC; }
aliasname		{ return ALIASNAME; }

serveropts		{ return SERVEROPTS; }
flood_control		{ return FLOODOPTS; }
connect_control		{ return CONNECTOPTS; }
operator_options	{ return OPERCONF; }
ts_warn_delta		{ return TS_WARN_DELTA; }
ts_max_delta		{ return TS_MAX_DELTA; }
default_kline_time	{ return DEFAULT_KLINE_TIME; }
motd_wait		{ return MOTD_WAIT; }
motd_max		{ return MOTD_MAX; }
client_flood		{ return CLIENT_FLOOD; }
max_channels_per_user	{ return MAX_CHANNELS_PER_USER; }
server_kline_address	{ return SERVER_KLINE_ADDRESS; }
network_kline_address	{ return NETWORK_KLINE_ADDRESS; }
default_quit_message	{ return DEFAULT_QUIT_MSG; }
anti_nick_flood		{ return ANTI_NICK_FLOOD; }
max_nick_time		{ return MAX_NICK_TIME; }
max_nick_changes	{ return MAX_NICK_CHANGES; }
no_away_flood		{ return NO_AWAY_FLOOD; }
max_away_time		{ return MAX_AWAY_TIME; }
max_away_count		{ return MAX_AWAY_COUNT; }
max_knock_time		{ return MAX_KNOCK_TIME; }
save_maxclient_stats_time	{ return SAVE_MAXCLIENT_STATS_TIME; }
use_registerfilter	{ return USE_REGISTERFILTER; }
hide_gcos_with_sgline   { return HIDE_GCOS_WITH_SGLINE; }
realname_replacement	{ return REALNAME_REPLACEMENT; }
use_short_motd		{ return SHORT_MOTD; }
staffhide		{ return STAFFHIDE; }
wingate_notice		{ return WINGATE_NOTICE; }
monitor_host		{ return MONITOR_HOST; }
monitor_url		{ return MONITOR_URL; }
identd_use_tilde	{ return IDENTD_TILDE; }
identd_timeout		{ return IDENTD_TIMEOUT; }
identd_complain		{ return IDENTD_COMPLAIN; }
links_delay		{ return LINKS_DELAY; }
throttle_time		{ return THROTTLE_TIME; }
throttle_count		{ return THROTTLE_COUNT; }
anti_spambot		{ return ANTI_SPAMBOT; }
min_join_leave_time	{ return MIN_JOIN_LEAVE_TIME; }
max_join_leave_count	{ return  MAX_JOIN_LEAVE_COUNT; }
join_leave_count_expire_time 	{ return JOIN_LEAVE_COUNT_EXPIRE_TIME; }
oper_spam_countdown	{ return OPER_SPAM_COUNTDOWN; }
no_oper_jupiter		{ return NO_OPER_JUPITER; }
havent_read_conf	{ return HAVENT_READ_CONF; }
language		{ return LANGUAGE; }
aftype			{ return AFTYPE; }
channelconf		{ return CHANNELCONF; }
use_regex		{ return USEREGEX; }
files			{ return FILES; }
load			{ return LOAD; }
pidfile                 { return PIDFILE; }
default_extended_topic_limitation	{ return DEFEXTOPIC; }
visible_serverchannels	{ return VISSCHAN; }
no_messages_on_away	{ return NOMSGAWAY; }    
servicesconf		{ return SERVICESCONF; }
chanserv_name		{ return CHANSERV; }
nickserv_name		{ return NICKSERV; }
operserv_name		{ return OPERSERV; }
memoserv_name		{ return MEMOSERV; }
statserv_name		{ return STATSERV; }
helpserv_name		{ return HELPSERV; }
services_name		{ return SERVICES_NAME; }
hide_ulined_servers	{ return HIDE_ULINED_SERVERS; } 
        
admin			{ return ADMIN; }
email			{ return EMAIL; }

operator		{ return OPERATOR; }  
user			{ return USER; }
password		{ return PASSWORD; }   
class			{ return CLASS; }
die			{ return DIE; }
restart			{ return RESTART; }
sadmin			{ return SADMIN; }
gkline			{ return GKLINE; }
kill			{ return OPKILL; }
operdo			{ return OPERDO; }
see_hidden		{ return SEEHIDE; }
login_via_web		{ return WEBLOGIN; }

auth			{ return AUTH; }
pingtime		{ return PING_TIME; }
connectfreq		{ return CONNECTFREQ; }
max_links		{ return MAX_LINKS; }
sendq			{ return SENDQ; }

listen			{ return LISTEN; }
port			{ return PORT; }
ip			{ return IP; }
host			{ return HOST; }
ipv4            	{ return Q_IPV4; }
ipv6            	{ return Q_IPV6; }
serverport		{ return SERVERPORT; }
clientport		{ return CLIENTPORT; }
serviceport		{ return SERVICEPORT; }
client_ssl		{ return CLISSLPORT; }

can_choose_fakehost	{ return CAN_CHOOSE_FAKEHOST; }
spoof_notice		{ return SPOOF_NOTICE; }
spoof			{ return SPOOF; }
exceed_limit		{ return EXCEED_LIMIT; }
kline_exempt		{ return KLINE_EXEMPT; }
redirserv		{ return REDIRSERV; }
redirport		{ return REDIRPORT; }

connect			{ return CONNECT; }
send_password		{ return SEND_PASSWORD; }
accept_password		{ return ACCEPT_PASSWORD; }
encrypted		{ return ENCRYPTED; }
compressed		{ return COMPRESSED; } 
autoconnect		{ return AUTOCONN; }
ishub			{ return ISHUB; }
ultimate		{ return ULTIMATE; }

yes			{ return QYES; }
no			{ return QNO; }
modules			{ return MODULES; }
module			{ return MODULE; }
path			{ return PATH; }

service			{ return SERVICE; }
send_akill		{ return SEND_AKILL; }

serverhide		{ return SERVERHIDE; }
enable			{ return ENABLE; }
links_from_file		{ return LINKSFFILE; }
flatten_links		{ return FLATTEN; }
links_oper_only		{ return LINKS_OO; }

proxy_monitor_options	{ return PM_OPTIONS; }
configuration_file	{ return PMH_CONFFILE; }
httpd_options		{ return HTTPD_OPTIONS; }

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
default_fakehost_mode	{ return DEFAULT_FAKEHOST_MODE; }
\.\.                    { return TWODOTS; }

.		        { return yytext[0]; }
<<EOF>>     { if (ieof()) yyterminate(); }

%%

/*
 * gigabytes            { return GBYTES; }
 * gigabyte             { return GBYTES; }   
 * gbytes                       { return GBYTES; }
 * gbyte                        { return GBYTES; }
 * gb                   { return GBYTES; }
 * terabytes            { return TBYTES; }
 * terabyte             { return TBYTES; }
 * tbytes                       { return TBYTES; }
 * tbyte                        { return TBYTES; }
 * tb                   { return TBYTES; }
 */

/* *INDENT-ON* */

/* C-comment ignoring routine -kre */
void ccomment(void)
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

/* C-style .includes. This function will properly swap input conf buffers,
 * and lineno -kre */
void cinclude(void)
{
    char *c;
    if ((c = index(yytext, '<')) == NULL)
	*index(c = index(yytext, '"') + 1, '"') = 0;
    else
	*index(++c, '>') = 0;

    /* do stacking and co. */
    if (include_stack_ptr >= MAX_INCLUDE_DEPTH)
	logevent_call(LogSys.lexer_deep, c);
    else {
    FBFILE *tmp_fbfile_in;
    char filenamebuf[BUFSIZE];

	sprintf(filenamebuf, "%s", c);
	tmp_fbfile_in = fbopen(filenamebuf, "r");

	if (tmp_fbfile_in == NULL) {
	    logevent_call(LogSys.lexer_noinc, filenamebuf);
	    return;
	}
	lineno_stack[include_stack_ptr] = lineno;
	lineno = 1;
	inc_fbfile_in[include_stack_ptr] = conf_fbfile_in;
	include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
	conf_fbfile_in = tmp_fbfile_in;
	yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
    }
}

/* This is function that will be called on EOF in conf file. It will
 * apropriately close conf if it not main conf and swap input buffers -kre
 * */
int ieof(void)
{
    if (include_stack_ptr)
	fbclose(conf_fbfile_in);
    if (--include_stack_ptr < 0) {
	/* We will now exit the lexer - restore init values if we get /rehash
	 * later and reenter lexer -kre */
	include_stack_ptr = 0;
	lineno = 1;
	return 1;
    }
    /* switch buffer */
    yy_delete_buffer(YY_CURRENT_BUFFER);
    lineno = lineno_stack[include_stack_ptr];
    conf_fbfile_in = inc_fbfile_in[include_stack_ptr];
    yy_switch_to_buffer(include_stack[include_stack_ptr]);
    return 0;
}
