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

extern char *ip_string;
extern int mlineno;
extern int mcount;
extern int pmlineno;
extern int pmcount;

int yyparse();
int i = -1;

static struct ConfItem *yy_achead = NULL;
static struct ConfItem *yy_aprev = NULL;
static int              yy_acount = 0;
static struct ConfItem *yy_aconf = NULL;

static char *listener_address;
int *listener_ports;
int port_count = 0;
int port_count_max = 10;
int port_type = 0;

int modflags = MODULE_FROMCONFIG;

extern int   scount;

%}

%union {
        int  number;
        char *string;
        struct ip_value ip_entry;
}

%token SERVERINFO
%token NAME
%token DESCRIPTION
%token NETWORKNAME
%token ADDRESS
%token ADDRESS6
%token RESTARTPASS
%token DIEPASS
%token DISPLAYPASS
%token IDENTITY
%token NETWORKDESC
%token ALIASNAME
%token SERVEROPTS
%token CONNECTOPTS
%token FLOODOPTS
%token OPERCONF
%token TS_WARN_DELTA
%token TS_MAX_DELTA
%token DEFAULT_KLINE_TIME
%token MOTD_WAIT
%token MOTD_MAX
%token CLIENT_FLOOD
%token MAX_CHANNELS_PER_USER
%token SERVER_KLINE_ADDRESS
%token NETWORK_KLINE_ADDRESS
%token DEFAULT_QUIT_MSG
%token ANTI_NICK_FLOOD
%token MAX_NICK_TIME
%token MAX_NICK_CHANGES
%token NO_AWAY_FLOOD
%token MAX_AWAY_TIME
%token MAX_AWAY_COUNT
%token MAX_KNOCK_TIME
%token SAVE_MAXCLIENT_STATS_TIME
%token USE_REGISTERFILTER
%token HIDE_GCOS_WITH_SGLINE
%token REALNAME_REPLACEMENT
%token SHORT_MOTD
%token STAFFHIDE
%token WINGATE_NOTICE
%token MONITOR_HOST
%token MONITOR_URL
%token IDENTD_TILDE
%token IDENTD_TIMEOUT
%token IDENTD_COMPLAIN
%token THROTTLE_TIME
%token THROTTLE_COUNT
%token ANTI_SPAMBOT
%token MIN_JOIN_LEAVE_TIME
%token MAX_JOIN_LEAVE_COUNT
%token JOIN_LEAVE_COUNT_EXPIRE_TIME
%token OPER_SPAM_COUNTDOWN
%token NO_OPER_JUPITER
%token HAVENT_READ_CONF
%token LANGUAGE
%token AFTYPE
%token CHANNELCONF
%token FILES 
%token LOAD
%token PIDFILE 
%token DEFAULT_FAKEHOST_MODE
%token USEREGEX
%token SERVICESCONF
%token CHANSERV
%token NICKSERV
%token OPERSERV
%token MEMOSERV
%token STATSERV
%token HELPSERV
%token SERVICES_NAME
%token HIDE_ULINED_SERVERS
%token DEFEXTOPIC
%token ADMIN
%token EMAIL
%token VISSCHAN
%token OPERATOR
%token USER
%token PASSWORD
%token CLASS
%token DIE
%token RESTART
%token SADMIN
%token GKLINE
%token OPKILL
%token OPERDO
%token SEEHIDE
%token WEBLOGIN
%token PING_TIME
%token CONNECTFREQ
%token MAX_LINKS
%token SENDQ
%token NOMSGAWAY
%token LISTEN
%token PORT
%token IP
%token HOST
%token Q_IPV4
%token Q_IPV6
%token SERVERPORT
%token CLIENTPORT
%token SERVICEPORT
%token CLISSLPORT

%token AUTH
%token CAN_CHOOSE_FAKEHOST
%token SPOOF_NOTICE
%token SPOOF
%token EXCEED_LIMIT
%token KLINE_EXEMPT
%token REDIRSERV
%token REDIRPORT

%token CONNECT
%token SEND_PASSWORD
%token ACCEPT_PASSWORD
%token ENCRYPTED
%token COMPRESSED
%token AUTOCONN
%token ISHUB
%token ULTIMATE

%token SERVICE
%token SEND_AKILL

%token SERVERHIDE
%token ENABLE
%token LINKSFFILE
%token FLATTEN
%token LINKS_OO
%token LINKS_DELAY

%token PM_OPTIONS 
%token HTTPD_OPTIONS
%token PMH_CONFFILE 

%token TWODOTS
%token QSTRING
%token QYES
%token QNO
%token NUMBER

%token MODULES
%token MODULE
%token PATH

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

conf_item:        admin_entry
                | oper_entry
                | class_entry 
                | listen_entry
                | auth_entry
                | serverinfo_entry
		| channelconf_entry
		| services_entry
		| service_entry
                | connect_entry
		| serveropts_entry
		| connectopts_entry
		| operconf_entry
		| floodopts_entry
		| serverhide_entry
		| files_entry
                | modules_entry
		| proxymonitor_entry
		| httpd_entry
                | error ';'
                | error '}'
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
 *  section modules
 ***************************************************************************/

modules_entry:   MODULES 	'{' modules_items '}' ';' ;

modules_items:   modules_items modules_item |
                 modules_item ;

modules_item:    modules_module | modules_path |
                 error ;

modules_module:  MODULE '=' QSTRING ';'
{
#ifndef STATIC_MODULES
    char *mod_basename = irc_basename(yylval.string);
    char modpath[MAXPATHLEN];
    dlink_node *pathst;
    struct module_path *mpath;
    struct stat statbuf;

    /* store the absolute path for the proxymon module.
     * used for the web interface.
     * -dsginosa
     */

    if(strstr((const char *)yylval.string, "proxymon.so"))
	DupString(GeneralOpts.proxymodulefile, yylval.string);	

    if (findmodule_byname(mod_basename)) {
	MyFree(mod_basename);
        break;
    }

    if (strchr(yylval.string, '/')) {
        enqueue_one_module_to_load(yylval.string, modflags);
    } else {
	for (pathst = mod_paths.head; pathst; pathst = pathst->next) {
            mpath = (struct module_path *) pathst->data;
            sprintf(modpath, "%s/%s", mpath->path, yylval.string);
            if (stat(modpath, &statbuf) == 0) {
            	enqueue_one_module_to_load(modpath, modflags);
	    	break;
	    }
	}
    }
    MyFree(mod_basename);
    modflags = MODULE_FROMCONFIG;
#endif
};

modules_path: 	PATH '=' QSTRING ';'
{
#ifndef STATIC_MODULES
    mod_add_path(yylval.string);
#endif 
};


/***************************************************************************
 *  section serverinfo
 ***************************************************************************/

serverinfo_entry:       SERVERINFO	'{' serverinfo_items '}' ';' ;

serverinfo_items:       serverinfo_items serverinfo_item |
                        serverinfo_item ;

serverinfo_item:        serverinfo_name | serverinfo_address |
                        serverinfo_description | serverinfo_networkname |
			serverinfo_restartpass | serverinfo_diepass | 
			serverinfo_displaypass | serverinfo_identity |
			serverinfo_networkdesc | 
			serverinfo_aliasname | serverinfo_address6 | error ;

serverinfo_name:        NAME '=' QSTRING ';' 
{
    /* this isn't rehashable */
    if(ServerInfo.name == NULL)
      	DupString(ServerInfo.name, yylval.string);
};

serverinfo_description: DESCRIPTION '=' QSTRING ';' 
{
    if (ServerInfo.description == NULL)
    	DupString(ServerInfo.description, yylval.string);
};

serverinfo_networkname: NETWORKNAME '=' QSTRING ';' 
{
    if (ServerInfo.networkname == NULL)
    	DupString(ServerInfo.networkname, yylval.string);
};

serverinfo_networkdesc:	NETWORKDESC '=' QSTRING ';'
{
    if (ServerInfo.networkdesc == NULL)
	DupString(ServerInfo.networkdesc, yylval.string);
};

serverinfo_address:  	ADDRESS '=' QSTRING ';'
{
    if(inetpton(DEF_FAM, yylval.string, &IN_ADDR(ServerInfo.address)) <= 0) {
     	logevent_call(LogSys.parser_netmask, yylval.string);
    }
    ServerInfo.specific_ipv4_vhost = 1;
};

serverinfo_address6:	ADDRESS6 '=' QSTRING ';'
{
#ifdef IPV6
    if(inetpton(DEF_FAM, yylval.string, &IN_ADDR(ServerInfo.address6)) <= 0) {
	logevent_call(LogSys.parser_netmask, yylval.string);
    }
    ServerInfo.specific_ipv6_vhost = 1;
#endif
};

serverinfo_restartpass:	RESTARTPASS '=' QSTRING ';'
{
    MyFree(ServerInfo.restartpass);
    DupString(ServerInfo.restartpass, yylval.string);
};

serverinfo_diepass:	DIEPASS '=' QSTRING ';'
{
    MyFree(ServerInfo.diepass);
    DupString(ServerInfo.diepass, yylval.string);
};

serverinfo_displaypass:	DISPLAYPASS '=' QSTRING ';'
{
    MyFree(ServerInfo.displaypass);
    DupString(ServerInfo.displaypass, yylval.string);
};

serverinfo_identity:	IDENTITY '=' NUMBER ';'
{
    ServerInfo.identity = $3;
};

serverinfo_aliasname:	ALIASNAME '=' QSTRING ';'
{
    if (ServerInfo.aliasname == NULL)
    	DupString(ServerInfo.aliasname, yylval.string);
};

/***************************************************************************
 *  section serveropts
 ***************************************************************************/

connectopts_entry:	CONNECTOPTS	'{' connectopts_items '}' ';' ;

connectopts_items:	connectopts_items connectopts_item |
			connectopts_item ;

connectopts_item:	serveropts_default_kline_time |
			serveropts_server_kline_address | serveropts_network_kline_address |
			serveropts_wingate_notice | serveropts_havent_read_conf |
			serveropts_monitor_host | serveropts_monitor_url | 
			serveropts_throttle_time | serveropts_throttle_count |    
			serveropts_identd_use_tilde | serveropts_identd_complain | 
			serveropts_identd_timeout | error;

floodopts_entry:	FLOODOPTS	'{' floodopts_items '}' ';' ;

floodopts_items:	floodopts_items floodopts_item |
			floodopts_item ;

floodopts_item:		serveropts_motd_wait | serveropts_motd_max | serveropts_havent_read_conf |
			serveropts_client_flood |
			serveropts_anti_nick_flood | serveropts_max_nick_time | serveropts_max_nick_changes |
			serveropts_no_away_flood | serveropts_max_away_count | serveropts_max_away_time | 
			serveropts_anti_spambot | serveropts_min_join_leave_time | serveropts_max_knock_time |
			serveropts_max_join_leave_count | serveropts_join_leave_count_expire_time |
			serveropts_oper_spam_countdown | error ;

operconf_entry:		OPERCONF	'{' operconf_items '}' ';' ;

operconf_items:		operconf_items operconf_item |
			operconf_item ;

operconf_item:		serveropts_no_oper_jupiter | serveropts_staffhide |
			serveropts_havent_read_conf | error ;

serveropts_entry:       SERVEROPTS 	'{' serveropts_items '}' ';' ;
  
serveropts_items:       serveropts_items serveropts_item |
                        serveropts_item ;
    
serveropts_item:	serveropts_ts_warn_delta | serveropts_ts_max_delta | serveropts_save_maxclient_stats_time |
			serveropts_use_registerfilter | serveropts_short_motd | 
			serveropts_hide_gcos_with_sgline | serveropts_realname_replacement |
			serveropts_havent_read_conf | serveropts_language | serveropts_use_regex | 
			serveropts_default_fakehost_mode | serveropts_no_messages_away | error ;

serveropts_ts_warn_delta:		TS_WARN_DELTA '=' timespec ';'
{
    ServerOpts.ts_warn_delta = $3;
};

serveropts_ts_max_delta:		TS_MAX_DELTA '=' timespec ';'
{
    ServerOpts.ts_max_delta = $3;
};

serveropts_default_kline_time:		DEFAULT_KLINE_TIME '=' timespec ';'
{
    ServerOpts.default_kline_time = $3;
};

serveropts_motd_wait:			MOTD_WAIT '=' timespec ';'
{
    ServerOpts.motd_wait = $3;
};

serveropts_motd_max:			MOTD_MAX '=' NUMBER ';'
{
    ServerOpts.motd_max = $3;
};

serveropts_client_flood:		CLIENT_FLOOD '=' NUMBER ';'
{
    ServerOpts.client_flood = $3;
};

serveropts_server_kline_address:	SERVER_KLINE_ADDRESS '=' QSTRING ';'
{   
    MyFree(ServerOpts.server_kline_address);
    DupString(ServerOpts.server_kline_address, yylval.string);
};

serveropts_network_kline_address:	NETWORK_KLINE_ADDRESS '=' QSTRING ';'
{
    MyFree(ServerOpts.network_kline_address);
    DupString(ServerOpts.network_kline_address, yylval.string);
};

serveropts_use_regex:            	USEREGEX '=' QYES ';'
{
    ServerOpts.use_regex = 1;
}
                                        |
                                        USEREGEX '=' QNO ';' 
{
    ServerOpts.use_regex = 0;
};

serveropts_anti_nick_flood:		ANTI_NICK_FLOOD '=' QYES ';'
{
    ServerOpts.anti_nick_flood = 1;
}                                       
					|
					ANTI_NICK_FLOOD '=' QNO ';'
{
    ServerOpts.anti_nick_flood = 0;
};

serveropts_default_fakehost_mode:	DEFAULT_FAKEHOST_MODE '=' QYES ';'
{
    ServerOpts.default_fakehost_mode = 1;
}
					|
					DEFAULT_FAKEHOST_MODE '=' QNO ';'
{
    ServerOpts.default_fakehost_mode = 0;
};    

serveropts_hide_gcos_with_sgline:	HIDE_GCOS_WITH_SGLINE '=' QYES ';'
{
    ServerOpts.hide_gcos_with_sgline = 1;
}
                                        |
					HIDE_GCOS_WITH_SGLINE '=' QNO ';'
{
    ServerOpts.hide_gcos_with_sgline = 0;
};

serveropts_realname_replacement:	REALNAME_REPLACEMENT '=' QSTRING ';'
{
    MyFree(ServerOpts.realname_replacement);
    DupString(ServerOpts.realname_replacement, yylval.string);
};

serveropts_havent_read_conf:            HAVENT_READ_CONF '=' NUMBER ';'
{
    fprintf(stderr, "You do not seem to have checked your ircd.conf");
    exit(0);
} 
                                        |
                                        HAVENT_READ_CONF '=' QSTRING ';'
{
    fprintf(stderr, "You do not seem to have checked your ircd.conf");
    exit(0);
}
					|
					HAVENT_READ_CONF '=' QYES ';'
{
    fprintf(stderr, "You do not seem to have checked your ircd.conf");
    exit(0);
};

serveropts_language:			LANGUAGE '=' QSTRING ';'
{
    MyFree(ServerOpts.language);
    DupString(ServerOpts.language, yylval.string);
};

serveropts_max_nick_time:		MAX_NICK_TIME '=' timespec ';'
{
    ServerOpts.max_nick_time = $3;
};

serveropts_max_nick_changes:		MAX_NICK_CHANGES '=' NUMBER ';'
{
    ServerOpts.max_nick_changes = $3;
};

serveropts_no_away_flood:		NO_AWAY_FLOOD '=' QYES ';'
{
    ServerOpts.no_away_flood = 1;
}
 					|
					NO_AWAY_FLOOD '=' QNO ';'
{ 
    ServerOpts.no_away_flood = 0;
};

serveropts_max_away_time:		MAX_AWAY_TIME '=' timespec ';'
{
    ServerOpts.max_away_time = $3;
};
	
serveropts_max_knock_time:		MAX_KNOCK_TIME '=' timespec ';'
{
    ServerOpts.max_knock_time = $3;
};
				
serveropts_max_away_count:		MAX_AWAY_COUNT '=' NUMBER ';'
{
    ServerOpts.max_away_count = $3;
};

serveropts_save_maxclient_stats_time:	SAVE_MAXCLIENT_STATS_TIME '=' timespec ';'
{
    ServerOpts.save_maxclient_stats_time = $3;
};

serveropts_use_registerfilter:		USE_REGISTERFILTER '=' QYES ';'
{
    ServerOpts.use_registerfilter = 1;
}
					|
					USE_REGISTERFILTER '=' QNO ';'
{
    ServerOpts.use_registerfilter = 0;
};

serveropts_short_motd:			SHORT_MOTD '=' QYES ';'
{
    ServerOpts.short_motd = 1;
}
					|
					SHORT_MOTD '=' QNO ';'
{
    ServerOpts.short_motd = 0;
};

serveropts_staffhide:			STAFFHIDE '=' QYES ';'
{
    ServerOpts.staffhide = 1;
}
					|
					STAFFHIDE '=' QNO ';'
{
    ServerOpts.staffhide = 0;
};

serveropts_wingate_notice:		WINGATE_NOTICE '=' QYES ';'
{
    ServerOpts.wingate_notice = 1;
}
					|
					WINGATE_NOTICE '=' QNO ';'
{
    ServerOpts.wingate_notice = 0;
};

serveropts_monitor_host:		MONITOR_HOST '=' QSTRING ';'
{
    MyFree(ServerOpts.monitor_host);
    DupString(ServerOpts.monitor_host, yylval.string);
    ServerOpts.monitor_host_len = strlen(yylval.string);
};

serveropts_monitor_url:			MONITOR_URL '=' QSTRING ';'
{
    MyFree(ServerOpts.proxy_url);
    DupString(ServerOpts.proxy_url, yylval.string);
    ServerOpts.proxy_url_len = strlen(yylval.string);
};

serveropts_identd_use_tilde:			IDENTD_TILDE '=' QYES ';'
{
    ServerOpts.identd_use_tilde = 1;
}
					|
					IDENTD_TILDE '=' QNO ';'
{
    ServerOpts.identd_use_tilde = 0;
};

serveropts_identd_complain:		IDENTD_COMPLAIN '=' QYES ';'
{
    ServerOpts.identd_complain = 1;
}
					|
					IDENTD_COMPLAIN '=' QNO ';'
{
    ServerOpts.identd_complain = 0;
};   

serveropts_identd_timeout:		IDENTD_TIMEOUT '=' timespec ';'
{
    ServerOpts.identd_timeout = $3;
};

serveropts_throttle_time:		THROTTLE_TIME '=' timespec ';'
{
    ServerOpts.throttle_time = $3;
};

serveropts_throttle_count:		THROTTLE_COUNT '=' NUMBER ';'
{
    ServerOpts.throttle_count = $3;
};

serveropts_anti_spambot:		ANTI_SPAMBOT '=' QYES ';'
{
    ServerOpts.anti_spambot = 1;
}
					|
					ANTI_SPAMBOT '=' QNO ';'
{
    ServerOpts.anti_spambot = 0;
};

serveropts_no_oper_jupiter:		NO_OPER_JUPITER '=' QYES ';'
{
    ServerOpts.no_oper_jupiter = 1;
}
					|
					NO_OPER_JUPITER '=' QNO ';'
{
    ServerOpts.no_oper_jupiter = 0;
};

serveropts_no_messages_away:            NOMSGAWAY '=' QYES ';'
{
    ServerOpts.no_messages_on_away = 1;
}
                                        |
                                        NOMSGAWAY '=' QNO ';'
{
    ServerOpts.no_messages_on_away = 0;
};

serveropts_min_join_leave_time:		MIN_JOIN_LEAVE_TIME '=' timespec ';'
{
    ServerOpts.min_join_leave_time = $3;
};

serveropts_max_join_leave_count:	MAX_JOIN_LEAVE_COUNT '=' NUMBER ';'
{
    ServerOpts.max_join_leave_count = $3;
};

serveropts_join_leave_count_expire_time: JOIN_LEAVE_COUNT_EXPIRE_TIME '=' timespec ';'
{
    ServerOpts.join_leave_count_expire_time = $3;
};

serveropts_oper_spam_countdown:		OPER_SPAM_COUNTDOWN '=' NUMBER ';'
{
    ServerOpts.oper_spam_countdown = $3;
};

/***************************************************************************
 * services section
 ***************************************************************************/

services_entry: 	SERVICESCONF '{' services_items '}' ';' ;

services_items:		services_items services_item |
			services_item ;

services_item:  	services_chanserv | services_nickserv | services_operserv |
			services_memoserv | services_statserv | services_helpserv | 
			services_services_name | services_hide_ulined_servers | error ;

services_chanserv:		CHANSERV '=' QSTRING ';'
{
    MyFree(ServicesConf.chanserv);
    DupString(ServicesConf.chanserv, yylval.string);
};

services_nickserv:              NICKSERV '=' QSTRING ';'
{   
    MyFree(ServicesConf.nickserv);
    DupString(ServicesConf.nickserv, yylval.string);
};

services_memoserv:              MEMOSERV '=' QSTRING ';'
{   
    MyFree(ServicesConf.memoserv);
    DupString(ServicesConf.memoserv, yylval.string);
};

services_operserv:              OPERSERV '=' QSTRING ';'
{   
    MyFree(ServicesConf.operserv);
    DupString(ServicesConf.operserv, yylval.string);
};

services_statserv:              STATSERV '=' QSTRING ';'
{   
    MyFree(ServicesConf.statserv);
    DupString(ServicesConf.statserv, yylval.string);
};

services_helpserv:		HELPSERV '=' QSTRING ';'
{
    MyFree(ServicesConf.helpserv);
    DupString(ServicesConf.helpserv, yylval.string);
};

services_services_name:		SERVICES_NAME '=' QSTRING ';'
{
    MyFree(ServicesConf.services_name);
    DupString(ServicesConf.services_name, yylval.string);
};

services_hide_ulined_servers:	HIDE_ULINED_SERVERS '=' QYES ';'
{
    ServicesConf.hide_ulined_servers = 1;
}
				|
				HIDE_ULINED_SERVERS '=' QNO ';'
{
    ServicesConf.hide_ulined_servers = 0;
};

/***************************************************************************
 * channels section
 ***************************************************************************/

channelconf_entry: 	CHANNELCONF '{' channelconf_items '}' ';' ;

channelconf_items:	channelconf_items channelconf_item |
			channelconf_item ;

channelconf_item:	channelconf_max_channels_per_user | channelconf_default_quit_msg |
			channelconf_defextopic | channelconf_visschan | error ; 


channelconf_visschan:	VISSCHAN '=' QYES ';'
{
    ChannelConf.visschan = 1;
}
			|
			VISSCHAN '=' QNO ';'
{
    ChannelConf.visschan = 0;
};

channelconf_max_channels_per_user:       MAX_CHANNELS_PER_USER '=' NUMBER ';'
{
    ChannelConf.max_channels_per_user = $3;
};

channelconf_default_quit_msg:            DEFAULT_QUIT_MSG '=' QSTRING ';'
{ 
    MyFree(ChannelConf.default_quit_msg);
    DupString(ChannelConf.default_quit_msg, yylval.string);
};

channelconf_defextopic:	DEFEXTOPIC '=' QYES ';'
{
    ChannelConf.default_extended_topic_limitation = 1;
}
			|
			DEFEXTOPIC '=' QNO ';'
{
    ChannelConf.default_extended_topic_limitation = 0;
};

/***************************************************************************
 * serverhide section
 ***************************************************************************/

serverhide_entry:	SERVERHIDE '{' serverhide_items '}' ';' ;

serverhide_items:	serverhide_items serverhide_item |
			serverhide_item ;

serverhide_item:	serverhide_enable | serverhide_linksfile |
			serverhide_flatten | serverhide_links_oo | 
			serverhide_links_delay | error ;

serverhide_enable:	ENABLE '=' QYES ';'
{
    ServerHide.enable = 1;
}
			|
			ENABLE '=' QNO ';'
{
    ServerHide.enable = 0;
};

serverhide_linksfile:	LINKSFFILE '=' QYES ';'
{
    ServerHide.links_from_file = 1;
}
			|
			LINKSFFILE '=' QNO ';'
{
    ServerHide.links_from_file = 0;
};

serverhide_flatten:	FLATTEN '=' QYES ';'
{
    ServerHide.flatten_links = 1;
}
 			|
			FLATTEN '=' QNO ';'
{
    ServerHide.flatten_links = 0;
};

serverhide_links_oo:	LINKS_OO '=' QYES ';'
{
    ServerHide.links_oper_only = 1;
}
			|
			LINKS_OO '=' QNO ';'
{
    ServerHide.links_oper_only = 0;
};

serverhide_links_delay:	LINKS_DELAY '=' timespec ';'
{
    ServerHide.links_delay = $3;
};

/***************************************************************************
 * files section
 ***************************************************************************/

files_entry:	FILES '{' files_items '}' ';' ;

files_items:	files_items files_item | 
		files_item ;

files_item:	files_load_file | files_pidfile | error ;

files_load_file:	LOAD '=' QSTRING ';'
{
    mask_fbfile_in = NULL;
    mcount = 0;
    mlineno = 0;

    if ((mask_fbfile_in = fbopen(yylval.string, "r")) == NULL) {
        logevent_call(LogSys.conferror, yylval.string);
        break;
    }
    maskfileparse();
    fbclose(mask_fbfile_in);
};

files_pidfile:          PIDFILE '=' QSTRING ';'
{
    MyFree(GeneralOpts.PIDFile);
    DupString(GeneralOpts.PIDFile, yylval.string);
};

/***************************************************************************
 * proxymonitor section
 ***************************************************************************/

proxymonitor_entry:	PM_OPTIONS 
{
    modflags = MODULE_PROXYMODULE;
} 

'{' pm_items '}' ';' 

{
    modflags = MODULE_FROMCONFIG;
};

pm_items:	pm_items pm_item | pm_item ;
pm_item:	pm_enable | pm_conffile | modules_module | error ;

pm_enable:	ENABLE '=' QYES ';'
{
#ifdef USE_OPM
    GeneralOpts.enable_proxymonitor = 1;
#endif
}
		|
		ENABLE '=' QNO ';'
{
#ifdef USE_OPM
    GeneralOpts.enable_proxymonitor = 0;
#endif
};

pm_conffile:	PMH_CONFFILE '=' QSTRING ';'
{
#ifdef USE_OPM
    MyFree(GeneralOpts.proxyconffile);
    DupString(GeneralOpts.proxyconffile, yylval.string);
#endif
};    

/***************************************************************************
 * httpd section
 ***************************************************************************/

httpd_entry:     HTTPD_OPTIONS 
{
    modflags = MODULE_HTTPDMODULE;
}

'{' ihttpd_items '}' ';' 

{
    modflags = MODULE_FROMCONFIG;
};

ihttpd_items:       ihttpd_items ihttpd_item | ihttpd_item ;
ihttpd_item:        ihttpd_enable | ihttpd_conffile | modules_module | error ;

ihttpd_enable:      ENABLE '=' QYES ';'
{
    GeneralOpts.enable_httpd = 1;
}
                |
                ENABLE '=' QNO ';'
{
    GeneralOpts.enable_httpd = 0;
};

ihttpd_conffile:    PMH_CONFFILE '=' QSTRING ';'
{
    MyFree(GeneralOpts.httpdconffile);
    DupString(GeneralOpts.httpdconffile, yylval.string);
};


/***************************************************************************
 * admin section
 ***************************************************************************/

admin_entry: 	ADMIN  '{' admin_items '}' ';'  ;

admin_items:    admin_items admin_item |
                admin_item ;

admin_item:     admin_name | admin_description |
                admin_email | error ;

admin_name:     NAME '=' QSTRING ';' 
{
    MyFree(AdminInfo.name);
    DupString(AdminInfo.name, yylval.string);
};

admin_email:    EMAIL '=' QSTRING ';'
{
    MyFree(AdminInfo.email);
    DupString(AdminInfo.email, yylval.string);
};

admin_description:      DESCRIPTION '=' QSTRING ';'
{
    MyFree(AdminInfo.description);
    DupString(AdminInfo.description, yylval.string);
};


/***************************************************************************
 * oper section
  ***************************************************************************/

oper_entry:     OPERATOR 
{
    struct ConfItem *yy_tmp;

    yy_tmp = yy_achead;
    while(yy_tmp) {
        yy_aconf = yy_tmp;
        yy_tmp = yy_tmp->next;
        yy_aconf->next = NULL;
        free_conf(yy_aconf);
    }
    yy_acount = 0;

    yy_achead = yy_aconf = make_conf();
    yy_aconf->status = CONF_OPERATOR;
}
 
'{' oper_items '}' ';'
  
{
    struct ConfItem *yy_tmp;
    struct ConfItem *yy_next;

    /* copy over settings from first struct */
    for( yy_tmp = yy_achead->next; yy_tmp; yy_tmp = yy_tmp->next ) {
     	if (yy_achead->name)
       	    DupString(yy_tmp->name, yy_achead->name);
      	if (yy_achead->passwd)
            DupString(yy_tmp->passwd, yy_achead->passwd);
      	yy_tmp->port = yy_achead->port;
    }

    for( yy_tmp = yy_achead; yy_tmp; yy_tmp = yy_next ) {
        yy_next = yy_tmp->next;
        yy_tmp->next = NULL;

        if(yy_tmp->name && yy_tmp->passwd && yy_tmp->host) {
            conf_add_class_to_conf(yy_tmp); 
            conf_add_conf(yy_tmp);
        } else {
            free_conf(yy_tmp);
        }
    }
    yy_achead = NULL;
    yy_aconf = NULL;
    yy_aprev = NULL;
    yy_acount = 0;
}; 

oper_items:     oper_items oper_item |
                oper_item ;

oper_item:      oper_name  | oper_user | oper_password | oper_class |
                oper_die | oper_restart | oper_admin | oper_sadmin | 
		oper_gkline | oper_kill | oper_operdo | oper_see_hide | 
		oper_weblogin | error ;

oper_name:      NAME '=' QSTRING ';'
{
   MyFree(yy_achead->name);
   DupString(yy_achead->name, yylval.string);
};

oper_user:      USER '='  QSTRING ';'
{
    char *p;
    char *new_user;
    char *new_host;

    /* The first user= line doesn't allocate a new conf */
    if ( yy_acount++ ) {
      yy_aconf = (yy_aconf->next = make_conf());
      yy_aconf->status = CONF_OPERATOR;
    }

    if((p = strchr(yylval.string,'@'))) {
	*p = '\0';
	DupString(new_user,yylval.string);
	MyFree(yy_aconf->user);
	yy_aconf->user = new_user;
	p++;
	DupString(new_host,p);
	MyFree(yy_aconf->host);
	yy_aconf->host = new_host;
    } else {
	MyFree(yy_aconf->host);
   	DupString(yy_aconf->host, yylval.string);
   	DupString(yy_aconf->user,"*");
    }
};

oper_password:  PASSWORD '=' QSTRING ';'
{
    if (yy_achead->passwd)
	memset(yy_achead->passwd, 0, strlen(yy_achead->passwd));
    DupString(yy_achead->passwd, yylval.string);
};

oper_class:     CLASS '=' NUMBER ';'
{
    yy_achead->class = find_class($3);
};

oper_die: 	DIE '=' QYES ';' { yy_achead->port |= OFLAG_DIE; }
          	|
          	DIE '=' QNO ';' { yy_achead->port &= ~OFLAG_DIE; };

oper_admin: 	ADMIN '=' QYES ';' { yy_achead->port |= OFLAG_ADMIN; }
            	|
            	ADMIN '=' QNO ';' { yy_achead->port &= ~OFLAG_ADMIN; };

oper_restart: 	RESTART '=' QYES ';' { yy_achead->port |= OFLAG_RESTART; } 
		|
		RESTART '=' QNO ';' { yy_achead->port &= ~OFLAG_RESTART; };

oper_sadmin:	SADMIN '=' QYES ';' { yy_achead->port |= OFLAG_SADMIN; }
		|
		SADMIN '=' QNO ';' { yy_achead->port &= ~OFLAG_SADMIN; };

oper_gkline:	GKLINE '=' QYES ';' { yy_achead->port |= OFLAG_GKLINE; }
		|
		GKLINE '=' QNO ';' { yy_achead->port &= ~OFLAG_GKLINE; };

oper_operdo:	OPERDO '=' QYES ';' { yy_achead->port |= OFLAG_OPERDO; }
		|
		OPERDO '=' QNO ';' { yy_achead->port &= ~OFLAG_OPERDO; };

oper_kill:	OPKILL '=' QYES ';' { yy_achead->port |= OFLAG_OPKILL; }
		|
		OPKILL '=' QNO ';' { yy_achead->port &= ~OFLAG_OPKILL; };

oper_see_hide:	SEEHIDE '=' QYES ';' { yy_achead->port |= OFLAG_SEEHIDDEN; }
		|
		SEEHIDE '=' QNO ';' { yy_achead->port &= ~OFLAG_SEEHIDDEN; };

oper_weblogin:	WEBLOGIN '=' QYES ';' { yy_achead->flags |= CONF_FLAGS_WEBLOGIN; }
		|
		WEBLOGIN '=' QNO ';' { yy_achead->flags &= ~CONF_FLAGS_WEBLOGIN; };

/***************************************************************************
 *  section class
 ***************************************************************************/

class_entry:    CLASS '{' class_items '}' ';'
{
    i = 0;
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
    i = 1;
};

class_ping_time:        PING_TIME '=' timespec ';'
{
    i = 2;
};

class_conn_freq:     	CONNECTFREQ '=' timespec ';'
{
    i = 3;
};

class_max_links:       	MAX_LINKS '=' NUMBER ';'
{
    i = 4;
};

class_sendq:    	SENDQ '=' sizespec ';'
{
    i = 5;
};


/***************************************************************************
 *  section listen
 ***************************************************************************/

listen_entry: 	LISTEN
{
    listener_address = NULL;
}
   '{' listen_items '}' ';'
{
    MyFree(listener_address);
    listener_address = NULL;
};

listen_items:   listen_items listen_item |
                listen_item ;

listen_item:    
{
    listener_ports = (int *) MyMalloc(10 * sizeof(int));
    memset(listener_ports, 0, 10 * sizeof(int));
    port_count = 0;
    port_count_max = 10;
    port_type = 0;
}
   '{' listen_elems '}' ';'
{
    int i;
    uuid_t u;
    if (port_type == 0) {
	port_type |= LISTENER_SERVER;
	port_type |= LISTENER_CLIENT;
	port_type |= LISTENER_SERVICE;
    }
    if (port_type & LISTENER_CLIENT_SSL) {
	port_type &= ~LISTENER_SERVER;
	port_type &= ~LISTENER_SERVICE;
	port_type |= LISTENER_CLIENT;
    }
    uuid_generate(u);
    for (i = 0; i < port_count; i++) {
	if (i > 0 && (listener_ports[i] - listener_ports[i - 1]) > 1) 
	    uuid_generate(u);
	add_listener(listener_ports[i], listener_address, port_type, u, NULL);
    }
    memset(listener_ports, 0, port_count * sizeof(int));
    MyFree(listener_ports);
    listener_ports = NULL;
};

listen_elems:	listen_elems listen_elem |
		listen_elem ;

listen_elem:	listen_port | listen_address | listen_host | 
		listen_type_serverport | listen_type_clientport | 
		listen_type_serviceport |
		listen_type_clientssl_port | error ;

listen_port: 	PORT '=' port_items ';' ;

port_items: 	port_items ',' port_item | port_item ;

port_item: 	NUMBER	
{ 
    if (port_count >= port_count_max) {
	port_count_max += 5;
	listener_ports = (int *) MyRealloc(listener_ports, port_count_max * sizeof(int));
	memset(listener_ports + port_count, 0, 5 * sizeof(int));
    }
    listener_ports[port_count] = $1;
    port_count++;
} 
		| 
		NUMBER TWODOTS NUMBER 
{
    int i;
    for (i = $1; i <= $3; i++) {
	if (port_count >= port_count_max) {
	    port_count_max += 5;
	    listener_ports = (int *) MyRealloc(listener_ports, port_count_max * sizeof(int));
	}
	listener_ports[port_count] = i;
	port_count++;
    }
    port_type |= LISTENER_GROUPED;
};

listen_address: IP '=' QSTRING ';'
{
    MyFree(listener_address);
    DupString(listener_address, yylval.string);
};

listen_host:	HOST '=' QSTRING ';'
{
    MyFree(listener_address);
    DupString(listener_address, yylval.string);
};

listen_type_serverport:		SERVERPORT ';'
{
    port_type |=	LISTENER_SERVER;
};

listen_type_serviceport:        SERVICEPORT ';'
{
    port_type |=        LISTENER_SERVICE;   
};

listen_type_clientport:         CLIENTPORT ';'
{
    port_type |=        LISTENER_CLIENT;   
};

listen_type_clientssl_port:	CLISSLPORT ';'
{
    port_type |= 	LISTENER_CLIENT_SSL;
};

/***************************************************************************
 *  section auth
 ***************************************************************************/

auth_entry:   AUTH
{
    struct ConfItem *yy_tmp;

    yy_tmp = yy_achead;
    while(yy_tmp) {
        yy_aconf = yy_tmp;
        yy_tmp = yy_tmp->next;
        yy_aconf->next = NULL;
        free_conf(yy_aconf);
    }
    yy_achead = NULL;
    yy_aconf = NULL;
    yy_aprev = NULL;

    yy_achead = yy_aprev = yy_aconf = make_conf();
    yy_aconf->status = CONF_CLIENT;
}

 '{' auth_items '}' ';' 

{
    struct ConfItem *yy_tmp;
    struct ConfItem *yy_next;

    /* copy over settings from first struct */
    for( yy_tmp = yy_achead->next; yy_tmp; yy_tmp = yy_tmp->next ) {
      	if(yy_achead->passwd)
            DupString(yy_tmp->passwd, yy_achead->passwd);
      	if(yy_achead->name)
            DupString(yy_tmp->name, yy_achead->name);
	if (yy_achead->class)
	    memcpy(yy_tmp->class, yy_achead->class, sizeof(aClass));
	
        yy_tmp->flags = yy_achead->flags;
      	yy_tmp->port  = yy_achead->port;
    }

    for( yy_tmp = yy_achead; yy_tmp; yy_tmp = yy_next ) {
      	yy_next = yy_tmp->next; /* yy_tmp->next is used by conf_add_conf */
      	yy_tmp->next = NULL;

      	if (yy_tmp->name == NULL)
            DupString(yy_tmp->name,"AUTH");

        conf_add_class_to_conf(yy_tmp); 

      	if (yy_tmp->user == NULL)
            DupString(yy_tmp->user,"*");
        else
            collapse(yy_tmp->user);

        if (yy_tmp->host == NULL)
       	    continue;
        else
            collapse(yy_tmp->host);
 
	conf_add_conf(yy_tmp);
    }
    yy_achead = NULL;
    yy_aconf = NULL;
    yy_aprev = NULL;
    yy_acount = 0;
}; 

auth_items:     auth_items auth_item |
                auth_item ;

auth_item:      auth_user | auth_passwd | auth_class | auth_kline_exempt | 
                auth_exceed_limit | auth_spoof | auth_spoof_notice |
                auth_allow_fakehost | auth_redir_serv | auth_redir_port | auth_port |
                error ;

auth_user:   USER '=' QSTRING ';'
{
    char *p;
    char *new_user;
    char *new_host;

    /* The first user= line doesn't allocate a new conf */
    if ( yy_acount++ ) {
      yy_aprev = yy_aconf;
      yy_aconf = (yy_aconf->next = make_conf());
      yy_aconf->status = CONF_CLIENT;
    }

    if((p = strchr(yylval.string,'@'))) {
	*p = '\0';
	DupString(new_user, yylval.string);
	MyFree(yy_aconf->user);
	yy_aconf->user = new_user;
	p++;
	MyFree(yy_aconf->host);
	DupString(new_host,p);
	yy_aconf->host = new_host;
    } else {
	MyFree(yy_aconf->host);
	DupString(yy_aconf->host, yylval.string);
	DupString(yy_aconf->user,"*");
    }
};

auth_passwd:  PASSWORD '=' QSTRING ';' 
{
    if (yy_achead->passwd)
        memset(yy_achead->passwd, 0, strlen(yy_achead->passwd));
    DupString(yy_achead->passwd, yylval.string);
};

auth_allow_fakehost:	CAN_CHOOSE_FAKEHOST '=' QYES ';'
{
    yy_achead->flags |= CONF_FLAGS_ALLOW_FAKEHOST;
}
			|
			CAN_CHOOSE_FAKEHOST '=' QNO ';'
{
    yy_achead->flags &= ~CONF_FLAGS_ALLOW_FAKEHOST;
};

auth_spoof_notice:   	SPOOF_NOTICE '=' QYES ';'
{
    yy_achead->flags |= CONF_FLAGS_SPOOF_NOTICE;
}
    			|
    			SPOOF_NOTICE '=' QNO ';'
{
    yy_achead->flags &= ~CONF_FLAGS_SPOOF_NOTICE;
};

auth_spoof:   		SPOOF '=' QSTRING ';' 
{
    MyFree(yy_achead->localhost);
    if(strlen(yylval.string) < HOSTLEN) {    
	DupString(yy_achead->localhost, yylval.string);
    	yy_achead->flags |= CONF_FLAGS_SPOOF_IP;
    } else
	logevent_call(LogSys.parser_spoof, HOSTLEN);
};

auth_exceed_limit:    	EXCEED_LIMIT '=' QYES ';'
{
    yy_achead->flags |= CONF_FLAGS_NOLIMIT;
}
                      	|
                      	EXCEED_LIMIT '=' QNO ';'
{
    yy_achead->flags &= ~CONF_FLAGS_NOLIMIT;
};

auth_kline_exempt:    	KLINE_EXEMPT '=' QYES ';'
{
    yy_achead->flags |= CONF_FLAGS_EXEMPTKLINE;
}
                      	|
                      	KLINE_EXEMPT '=' QNO ';'
{
    yy_achead->flags &= ~CONF_FLAGS_EXEMPTKLINE;
};

auth_redir_serv:    REDIRSERV '=' QSTRING ';'
{
    yy_achead->flags |= CONF_FLAGS_REDIR;
    MyFree(yy_achead->name);
    DupString(yy_achead->name, yylval.string);
};

auth_port:	    PORT '=' NUMBER ';'
{
    yy_achead->port = $3;
};

auth_redir_port:    REDIRPORT '=' NUMBER ';'
{
    yy_achead->flags |= CONF_FLAGS_REDIR;
    yy_achead->port = $3;
};

auth_class:   CLASS '=' NUMBER ';'
{
    yy_achead->class = find_class($3);
};


/***************************************************************************
 *  section connect
 ***************************************************************************/

connect_entry:  CONNECT   
{
    if(yy_aconf) {
        free_conf(yy_aconf);
        yy_aconf = (struct ConfItem *)NULL;
    }

    yy_aconf=make_conf();
    yy_aconf->passwd = NULL;
    /* Finally we can do this -A1kmm. */
    yy_aconf->status = CONF_SERVER;

    /* defaults */
    yy_aconf->port = 7000;
}
  '{' connect_items '}' ';'
  
{
    if(yy_aconf->host && yy_aconf->passwd && yy_aconf->spasswd) {
        if(conf_add_server(yy_aconf, scount) >= 0) {
	    conf_add_conf(yy_aconf);
	    ++scount;
	} else {
	    free_conf(yy_aconf);
	    yy_aconf = NULL;
	}
    } else {
        free_conf(yy_aconf);
        yy_aconf = NULL;
    }

    yy_aconf = NULL;
};

connect_items:  connect_items connect_item |
                connect_item ;

connect_item:   connect_name | connect_host | connect_send_password |
                connect_accept_password | connect_port | connect_aftype |
		connect_class | connect_auto | connect_encrypted | 
		connect_compressed | connect_hub | connect_ultimate | error ; 

connect_name:  	 	NAME '=' QSTRING ';'
{
    if(yy_aconf->name != NULL) 
	sendto_ops("Multiple connect name entry");

    MyFree(yy_aconf->name);
    DupString(yy_aconf->name, yylval.string);
};

connect_host:   	HOST '=' QSTRING ';' 
{
    MyFree(yy_aconf->host);
    DupString(yy_aconf->host, yylval.string);
};
 
connect_send_password:  SEND_PASSWORD '=' QSTRING ';'
{
    if (yy_aconf->spasswd)
        memset(yy_aconf->spasswd, 0, strlen(yy_aconf->spasswd));
    DupString(yy_aconf->spasswd, yylval.string);
};

connect_accept_password: ACCEPT_PASSWORD '=' QSTRING ';'
{
    if (yy_aconf->passwd)
        memset(yy_aconf->passwd, 0, strlen(yy_aconf->passwd));
    DupString(yy_aconf->passwd, yylval.string);
};

connect_port:   	PORT '=' NUMBER ';' { yy_aconf->port = $3; };

connect_aftype:         AFTYPE '=' Q_IPV4 ';'
{
    yy_aconf->aftype = AF_INET;
}
                        |
                        AFTYPE '=' Q_IPV6 ';'
{
#ifdef IPV6
    yy_aconf->aftype = AF_INET6;
#endif
};

connect_encrypted:      ENCRYPTED '=' QYES ';'
{
    yy_aconf->flags |= CONF_FLAGS_ENCRYPTED;
    yy_aconf->flags |= CONF_FLAGS_COMPRESSED;
}
                        |
                        ENCRYPTED '=' QNO ';'
{
    yy_aconf->flags &= ~CONF_FLAGS_ENCRYPTED;
};


connect_compressed:     COMPRESSED '=' QYES ';'
{
    yy_aconf->flags |= CONF_FLAGS_COMPRESSED;
}
                        |
                        COMPRESSED '=' QNO ';'
{
    yy_aconf->flags &= ~CONF_FLAGS_COMPRESSED;
    yy_aconf->flags &= ~CONF_FLAGS_ENCRYPTED;
};

connect_auto:           AUTOCONN '=' QYES ';'
{
    yy_aconf->flags |= CONF_FLAGS_ALLOW_AUTO_CONN;
}
                        |
                        AUTOCONN '=' QNO ';'
{
    yy_aconf->flags &= ~CONF_FLAGS_ALLOW_AUTO_CONN;
};

connect_class:  	CLASS '=' NUMBER ';'
{
    yy_aconf->class = find_class($3);
};

connect_hub:		ISHUB '=' QYES ';'
{
    yy_aconf->flags |= CONF_FLAGS_HUB;
}
			|
			ISHUB '=' QNO ';'
{
    yy_aconf->flags &= ~CONF_FLAGS_HUB;
};

connect_ultimate:	ULTIMATE '=' QYES ';'
{
    yy_aconf->flags |= CONF_FLAGS_ULTIMATE;
}
			|
			ULTIMATE '=' QNO ';'
{
    yy_aconf->flags &= ~CONF_FLAGS_ULTIMATE;
};

/***************************************************************************
 *  section service
 ***************************************************************************/

service_entry:  SERVICE 
{
    if(yy_aconf) {
        free_conf(yy_aconf);
        yy_aconf = (struct ConfItem *)NULL;
    }

    yy_aconf=make_conf();
    yy_aconf->passwd = NULL;
    yy_aconf->status = CONF_SERVICE;

}
  '{' service_items '}' ';'
  
{
    if(yy_aconf->host && yy_aconf->passwd) {
	conf_add_class_to_conf(yy_aconf);
	lookup_confhost(yy_aconf);
        conf_add_conf(yy_aconf);
    } else {
        free_conf(yy_aconf);
        yy_aconf = NULL;
    }
    yy_aconf = NULL;
};

service_items:	service_items service_item |
		service_item ;

service_item:	service_name | service_address | service_ip
		service_password | service_class |
		service_ultimate | service_send_akill | error ;

service_name:		NAME '=' QSTRING ';'
{
    if(yy_aconf->name != NULL) 
        sendto_ops("Multiple service name entry");

    MyFree(yy_aconf->name);
    DupString(yy_aconf->name, yylval.string);
};

service_address: 	HOST '=' QSTRING ';' 
{
    MyFree(yy_aconf->host);
    DupString(yy_aconf->host, yylval.string);
};

service_ip:		IP '=' QSTRING ';'    
{
    MyFree(yy_aconf->host);
    DupString(yy_aconf->host, yylval.string);
};

service_password:	PASSWORD '=' QSTRING ';'
{
    if (yy_aconf->passwd)
        memset(yy_aconf->passwd, 0, strlen(yy_aconf->passwd));
    DupString(yy_aconf->passwd, yylval.string);
};

service_class:          CLASS '=' NUMBER ';'
{
    yy_aconf->class = find_class($3);
};

service_ultimate:	ULTIMATE '=' QYES ';'
{
    yy_aconf->port |= SMODE_U;
}
                        |
                        ULTIMATE '=' QNO ';'
{
    yy_aconf->port &= ~SMODE_U;
};

service_send_akill:     SEND_AKILL '=' QYES ';'
{                        
    yy_aconf->port |= SMODE_A;
}
                        |
                        SEND_AKILL '=' QNO ';'
{
    yy_aconf->port &= ~SMODE_A;
};
