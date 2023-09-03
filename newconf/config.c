/************************************************************************
 *   IRC - Internet Relay Chat, newconf/config.c
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
 *  (C) 1988 University of Oulu,Computing Center and Jarkko Oikarinen"
 *
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "tools.h"
#include "channel.h"
#include "confitem.h"
#include "class.h"
#include "event.h"
#include "listener.h"
#include "modules.h"
#include "language.h"
#include "numeric.h"
#include "fd.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "fileio.h"
#include "ircd_parser.h"

FBFILE *conf_fbfile_in;
FBFILE *mask_fbfile_in;
FBFILE *pmconf_fbfile_in;
FBFILE *httpd_fbfile_in;

int scount = 0;
extern int lineno;
int mcount = 0;
extern int mlineno;
int pmcount = 0;
extern int pmlineno;
int hcount = 0;
extern int hlineno;
extern char linebuf[];
extern char mlinebuf[];
extern char pmlinebuf[];
extern char hlinebuf[];

extern int class_ping_time_var;

static void set_default_conf(void)
{
    ServerOpts.ts_warn_delta = 60;
    ServerOpts.ts_max_delta = 300;
    ServerOpts.default_kline_time = 180;
    ServerOpts.motd_wait = 10;
    ServerOpts.motd_max = 3;
    ServerOpts.client_flood = 2560;
    ServerOpts.anti_nick_flood = 1;
    ServerOpts.max_nick_time = 20;
    ServerOpts.max_nick_changes = 5;
    ServerOpts.no_away_flood = 1;
    ServerOpts.max_away_time = 180;
    ServerOpts.max_away_count = 5;
    ServerOpts.max_knock_time = 3600;
    ServerOpts.save_maxclient_stats_time = 3600;
    ServerOpts.use_registerfilter = 0;
    ServerOpts.hide_gcos_with_sgline = 0;
    ServerOpts.short_motd = 1;
    ServerOpts.staffhide = 1;
    ServerOpts.wingate_notice = 1;
    ServerOpts.identd_use_tilde = 1;
    ServerOpts.identd_timeout = 30;
    ServerOpts.identd_complain = 0;
    ServerOpts.throttle_time = 30;
    ServerOpts.throttle_count = 5;
    ServerOpts.anti_spambot = 0;
    ServerOpts.min_join_leave_time = 60;
    ServerOpts.max_join_leave_count = 25;
    ServerOpts.join_leave_count_expire_time = 120;
    ServerOpts.oper_spam_countdown = 0;
    ServerOpts.no_oper_jupiter = 1;
    ServerOpts.default_fakehost_mode = 1;
    ServerOpts.use_regex = 0;
    ServerOpts.no_messages_on_away = 0;

    ServicesConf.hide_ulined_servers = 1;

    ChannelConf.max_channels_per_user = 25;
    ChannelConf.default_extended_topic_limitation = 1;
    ChannelConf.visschan = 1;

    ServerHide.enable = 0;
    ServerHide.links_from_file = 0;
    ServerHide.flatten_links = 0;
    ServerHide.links_oper_only = 0;
    ServerHide.links_delay = 600;

    DupString(ChannelConf.default_quit_msg, DEF_QUITMSG);

    DupString(ServerOpts.server_kline_address, DEF_KLINEADDR);
    DupString(ServerOpts.network_kline_address, DEF_AKILLADDR);
    DupString(ServerOpts.monitor_host, DEF_MONHOST);
    DupString(ServerOpts.proxy_url, DEF_PROXYURL);
    DupString(ServerOpts.realname_replacement, DEF_GCOSTEXT);

    ServerOpts.language = NULL;

    DupString(ServicesConf.chanserv, DEF_CHANSERV);
    DupString(ServicesConf.nickserv, DEF_NICKSERV);
    DupString(ServicesConf.memoserv, DEF_MEMOSERV);
    DupString(ServicesConf.operserv, DEF_OPERSERV);
    DupString(ServicesConf.statserv, DEF_STATSERV);
    DupString(ServicesConf.services_name, DEF_SERVICES);

    DupString(AdminInfo.name, DEF_ADMINNAME);
    DupString(AdminInfo.email, DEF_ADMINMAIL);
    DupString(AdminInfo.description, DEF_ADMINDESC);
    DupString(GeneralOpts.PIDFile, IRCD_PIDFILE);
    DupString(GeneralOpts.proxymodulefile, "< Not set in the conf file >");

    memset(&ServerInfo.address, 0, sizeof(ServerInfo.address));
    ServerInfo.specific_ipv4_vhost = 0;
    memset(&ServerInfo.address6, 0, sizeof(ServerInfo.address6));
    ServerInfo.specific_ipv6_vhost = 0;

    memset(&ServerInfo.aliasname, 0, sizeof(ServerInfo.aliasname));

    return;
}

static void validate_conf(void)
{
    if (ServerInfo.name == NULL) {
	fprintf(stderr, "ERROR: No server name specified in serverinfo block.\n");
	exit(-1);
    }

    if (ServerInfo.description == NULL) {
	fprintf(stderr, "ERROR: No server description specified in serverinfo block.\n");
	exit(-1);
    }

    strlcpy_irc(me.info, ServerInfo.description, REALLEN);

    if (ServerInfo.networkname == NULL) {
	fprintf(stderr, "ERROR: No network name specified in serverinfo block.\n");
	exit(-1);
    }

    if (ServerInfo.networkdesc == NULL) {
	fprintf(stderr, "ERROR: No network description specified in serverinfo block.\n");
	exit(-1);
    }

    if (ServerInfo.aliasname == NULL)
	DupString(ServerInfo.aliasname, "localhost");

    if (ServerOpts.language == NULL)
	me.lang = set_language(0);
    else
	me.lang = lang_parse(ServerOpts.language);

    return;
}

int conf_fbgets(char *lbuf, int max_size, FBFILE * fb)
{
    char *buff;

    buff = fbgets(lbuf, max_size, fb);

    if (!buff)
	return 0;

    return (strlen(lbuf));
}

/*
 * read_conf_new() 
 *
 *
 * inputs       - file descriptor pointing to config file to use
 * output       - None
 * side effects - Read configuration file.
 */
static void read_conf_new(FBFILE * file, int do_classes)
{
    scount = lineno = 0;

    if (do_classes) {
	classesparse();
	check_class();		/* Make sure classes are valid */
	return;
    }

    set_default_conf();		/* Set default values prior to conf parsing */

    scount = lineno = 0;
    ircdparse();		/* Load the values from the conf */
    validate_conf();
}

/*
 * yyerror
 *
 * inputs       - message from parser
 * output       - none
 * side effects - message to opers and log file entry is made
 */

void ircderror(char *msg)
{
    char newlinebuf[BUFSIZE];

    strip_tabs(newlinebuf, (const unsigned char *) linebuf, strlen(linebuf));

    sendto_lev(SNOTICE_LEV, "%d: %s on line: %s", lineno + 1, msg, newlinebuf);
    logevent_call(LogSys.yyerror, lineno + 1, msg, newlinebuf);
}

void classeserror(char *msg)
{
    if (class_ping_time_var != 0) {
    char newlinebuf[BUFSIZE];

	strip_tabs(newlinebuf, (const unsigned char *) linebuf, strlen(linebuf));
	sendto_lev(SNOTICE_LEV, "%d: %s on line: %s", lineno + 1, msg, newlinebuf);
	logevent_call(LogSys.cyyerror, lineno + 1, msg, newlinebuf);
    }
}

void maskfileerror(char *msg)
{
    char newlinebuf[BUFSIZE];

    strip_tabs(newlinebuf, (const unsigned char *) mlinebuf, strlen(mlinebuf));

    sendto_lev(SNOTICE_LEV, "MASKFILE: %d: %s on line: %s", mlineno + 1, msg, newlinebuf);
    logevent_call(LogSys.yyerror, mlineno + 1, msg, newlinebuf);
}

void proxymonerror(char *msg)
{
    char newlinebuf[BUFSIZE];

    strip_tabs(newlinebuf, (const unsigned char *) pmlinebuf, strlen(pmlinebuf));

    sendto_lev(SNOTICE_LEV, "PM: %d: %s on line: %s", pmlineno + 1, msg, newlinebuf);
    logevent_call(LogSys.yyerror, pmlineno + 1, msg, newlinebuf);
}

void httpderror(char *msg)
{
    char newlinebuf[BUFSIZE];
    strip_tabs(newlinebuf, (const unsigned char *) hlinebuf, strlen(hlinebuf));

    sendto_lev(SNOTICE_LEV, "HTTPD: %d: %s on line: %s", hlineno + 1, msg, newlinebuf);
    logevent_call(LogSys.yyerror, hlineno + 1, msg, newlinebuf);
}

int ircdwrap(void)
{
    return 1;
}

int classeswrap(void)
{
    return ircdwrap();
}

int maskfilewrap(void)
{
    return ircdwrap();
}

int proxymonwrap(void)
{
    return ircdwrap();
}

int httpdwrap(void)
{
    return ircdwrap();
}

void read_conf_files(int cold)
{
    const char *filename;
    conf_fbfile_in = NULL;
    filename = GeneralOpts.configfile;
    conf_fbfile_in = fbopen(filename, "r");
    if (conf_fbfile_in == NULL) {
	if (cold) {
	    logevent_call(LogSys.conferror, filename);
	    fprintf(stderr, "Unable to open server config file: %s\n", filename);
	    exit(-1);
	} else {
	    sendto_ops("Can't open file '%s' - aborting rehash!", filename);
	    return;
	}
    }
    if (!cold) {
    struct ConfItem **tmp = &GlobalConfItemList;
    struct ConfItem *tmp2;
    struct ConnClass *cltmp;
	/*
	 * We only need to free anything allocated by yyparse() here.
	 * Reseting structs, etc, is taken care of by set_default_conf().
	 */
	while ((tmp2 = *tmp)) {
	    if (tmp2->clients) {
		/*
		 ** Configuration entry is still in use by some
		 ** local clients, cannot delete it--mark it so
		 ** that it will be deleted when the last client
		 ** exits...
		 */
		if (!(tmp2->status & CONF_CLIENT)) {
		    *tmp = tmp2->next;
		    tmp2->next = NULL;
		} else
		    tmp = &tmp2->next;
		tmp2->status |= CONF_ILLEGAL;
	    } else {
		*tmp = tmp2->next;
		free_conf(tmp2);
	    }
	}

	/*
	 * don't delete the class table, rather mark all entries
	 * for deletion. The table is cleaned up by check_class. - avalon
	 */
	for (cltmp = ConnectionClasses->next; cltmp; cltmp = cltmp->next)
	    cltmp->max_connections = -1;

	/* operator{} and class{} blocks are freed above */
	/* clean out listeners */

	close_listeners();
    }
    read_conf_new(conf_fbfile_in, 1);
    fbclose(conf_fbfile_in);
    conf_fbfile_in = fbopen(filename, "r");
    if (conf_fbfile_in == NULL) {
	if (cold) {
	    logevent_call(LogSys.conferror, filename);
	    fprintf(stderr, "Unable to open server config file: %s\n", filename);
	    exit(-1);
	} else {
	    sendto_ops("Can't open file '%s' - aborting rehash!", filename);
	    return;
	}
    }

    read_conf_new(conf_fbfile_in, 0);
    fbclose(conf_fbfile_in);
}

int conf_yy_fatal_error(char *msg)
{
    return 0;
}
