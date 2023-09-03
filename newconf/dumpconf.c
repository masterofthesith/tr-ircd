/************************************************************************
 *   IRC - Internet Relay Chat, newconf/dumpconf.c
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

#define	CONF_SECTION_BEGIN		"%s {\n"
#define CONF_SECTION_END		"};\n\n"
#define CONF_SECTION_BEGIN_TABBED	"    {\n"
#define CONF_SECTION_BEGIN_TABBED_NAMED	"    %s {\n"
#define CONF_SECTION_END_TABBED		"    };\n"
#define CONF_SECTION_ELEMENT_STRING	"\t%s = \"%s\";\n"
#define CONF_SECTION_ELEMENT_RAW_STRING	"\t\"%s\";\n"
#define CONF_SECTION_ELEMENT_NUMBER	"\t%s = %d%s;\n"
#define CONF_SECTION_ELEMENT_RAW	"\t%s = %s;\n"
#define CONF_COMMENT_BEGIN		"#\n"
#define CONF_COMMENT_LINE		"# %s\n"
#define CONF_COMMENT_END		"#\n\n"

#define CONF_MASKFILE_ITEM		"mask : %s {\n\t%s = \"%s\";\n\treason = \"%s\";\n};\n"

#define CONF_LINE_LENGTH		256

extern dlink_list maskitem_list[MASKITEM_MASK];

static char lb[CONF_LINE_LENGTH];
static int lblen = 0;

static int dump_line(FBFILE * file, char *pattern, ...)
{
    va_list vl;

    if (lblen < 0)
	return 0;

    memset(lb, 0, CONF_LINE_LENGTH * sizeof(char));
    va_start(vl, pattern);
    ircvsnprintf(lb, CONF_LINE_LENGTH, pattern, vl);
    va_end(vl);
    lblen = fbputs(lb, file);
    return lblen;
}

static inline void dump_line_boolean(FBFILE * file, char *name, int val)
{
    dump_line(file, CONF_SECTION_ELEMENT_RAW, name, val ? "yes" : "no");
}

static inline void dump_line_permission(FBFILE * file, char *name, int val)
{
    dump_line(file, CONF_SECTION_ELEMENT_RAW, name, val ? "allow" : "deny");
}

static inline void dump_line_timed(FBFILE * file, char *name, int t)
{
    char *desc;

    int val = t / 3600;

    if (t <= 0)
	return;

    if (val <= 0) {
	val = t / 60;
	if (val <= 0) {
	    desc = " seconds";
	    val = t;
	} else {
	    desc = " minutes";
	}
    } else {
	desc = " hours";
    }

    dump_line(file, CONF_SECTION_ELEMENT_NUMBER, name, val, desc);
}

int generate_server_config_file(char *filename)
{
    FBFILE *f;

    struct Listener *li;
    struct Listener *next_li;
    struct Listener *prev_li;
    struct ConfItem *aconf;
    aClass *aclass;

#ifndef STATIC_MODULES
    struct module *mod;
    dlink_node *ptr;
#endif

    char strBuf1[200];

    backup_configuration_files();
    
    f = fbopen(filename, "w");

    if (f == NULL)
	return -1;

    dump_line(f, CONF_COMMENT_BEGIN);
    dump_line(f, CONF_COMMENT_LINE, "TR-IRCD Configuration file");
    dump_line(f, CONF_COMMENT_LINE, date(0));
    dump_line(f, CONF_COMMENT_END);

    dump_line(f, CONF_SECTION_BEGIN, "serverinfo");
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "name", me.name);
    if (ServerInfo.aliasname && ServerInfo.aliasname[0])
	dump_line(f, CONF_SECTION_ELEMENT_STRING, "aliasname", ServerInfo.aliasname);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "description", ServerInfo.description);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "networkname", ServerInfo.networkname);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "networkdesc", ServerInfo.networkdesc);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "restartpass", ServerInfo.restartpass);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "diepass", ServerInfo.diepass);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "displaypass", ServerInfo.displaypass);
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "identity", ServerInfo.identity, "");
    if (ServerInfo.specific_ipv4_vhost) {
    char h[HOSTIPLEN + 1];
	inetntop(DEF_FAM, &IN_ADDR(ServerInfo.address), h, HOSTIPLEN);
	dump_line(f, CONF_SECTION_ELEMENT_STRING, "address", h);
    }
    if (ServerInfo.specific_ipv6_vhost) {
    char h[HOSTIPLEN + 1];
	inetntop(DEF_FAM, &IN_ADDR(ServerInfo.address6), h, HOSTIPLEN);
	dump_line(f, CONF_SECTION_ELEMENT_STRING, "address6", h);
    }
    dump_line(f, CONF_SECTION_END);

    dump_line(f, CONF_SECTION_BEGIN, "serverhide");
    dump_line_boolean(f, "enable", ServerHide.enable);
    dump_line_boolean(f, "links_from_file", ServerHide.links_from_file);
    dump_line_boolean(f, "flatten_links", ServerHide.flatten_links);
    dump_line_boolean(f, "links_oper_only", ServerHide.links_oper_only);
    dump_line_timed(f, "links_delay", ServerHide.links_delay);
    dump_line(f, CONF_SECTION_END);

    dump_line(f, CONF_SECTION_BEGIN, "serveropts");
    dump_line_timed(f, "ts_warn_delta", ServerOpts.ts_warn_delta);
    dump_line_timed(f, "ts_max_delta", ServerOpts.ts_max_delta);
    dump_line_timed(f, "save_maxclient_stats_time", ServerOpts.save_maxclient_stats_time);
    dump_line_boolean(f, "use_registerfilter", ServerOpts.use_registerfilter);
    dump_line_boolean(f, "use_short_motd", ServerOpts.short_motd);
    if (ServerOpts.language && ServerOpts.language[0])
	dump_line(f, CONF_SECTION_ELEMENT_STRING, "language", ServerOpts.language);
    dump_line_boolean(f, "hide_gcos_with_sgline", ServerOpts.hide_gcos_with_sgline);
    if (ServerOpts.realname_replacement && ServerOpts.realname_replacement[0])
	dump_line(f, CONF_SECTION_ELEMENT_STRING, "realname_replacement",
		  ServerOpts.realname_replacement);
    dump_line_boolean(f, "default_fakehost_mode", ServerOpts.default_fakehost_mode);
    dump_line_boolean(f, "use_regex", ServerOpts.use_regex);
    dump_line_boolean(f, "no_messages_on_away", ServerOpts.no_messages_on_away);
    dump_line(f, CONF_SECTION_END);

    dump_line(f, CONF_SECTION_BEGIN, "connect_control");
    dump_line_timed(f, "default_kline_time", ServerOpts.default_kline_time);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "server_kline_address",
	      ServerOpts.server_kline_address);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "network_kline_address",
	      ServerOpts.network_kline_address);
    dump_line_boolean(f, "wingate_notice", ServerOpts.wingate_notice);
    if (ServerOpts.monitor_host && ServerOpts.monitor_host[0])
	dump_line(f, CONF_SECTION_ELEMENT_STRING, "monitor_host", ServerOpts.monitor_host);
    if (ServerOpts.proxy_url && ServerOpts.proxy_url[0])
	dump_line(f, CONF_SECTION_ELEMENT_STRING, "monitor_url", ServerOpts.proxy_url);
    dump_line_boolean(f, "identd_complain", ServerOpts.identd_complain);
    dump_line_boolean(f, "identd_use_tilde", ServerOpts.identd_use_tilde);
    dump_line_timed(f, "identd_timeout", ServerOpts.identd_timeout);
    dump_line_timed(f, "throttle_time", ServerOpts.throttle_time);
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "throttle_count", ServerOpts.throttle_count, "");
    dump_line(f, CONF_SECTION_END);

    dump_line(f, CONF_SECTION_BEGIN, "flood_control");
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "client_flood", ServerOpts.client_flood, "");
    dump_line_timed(f, "motd_wait", ServerOpts.motd_wait);
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "motd_max", ServerOpts.motd_max, "");
    dump_line_boolean(f, "anti_nick_flood", ServerOpts.anti_nick_flood);
    dump_line_timed(f, "max_nick_time", ServerOpts.max_nick_time);
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "max_nick_changes", ServerOpts.max_nick_changes, "");
    dump_line_boolean(f, "no_away_flood", ServerOpts.no_away_flood);
    dump_line_timed(f, "max_away_time", ServerOpts.max_away_time);
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "max_away_count", ServerOpts.max_away_count, "");
    dump_line_timed(f, "max_knock_time", ServerOpts.max_knock_time);
    dump_line_boolean(f, "anti_spambot", ServerOpts.anti_spambot);
    dump_line_timed(f, "min_join_leave_time", ServerOpts.min_join_leave_time);
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "max_join_leave_count",
	      ServerOpts.max_join_leave_count, "");
    dump_line_timed(f, "join_leave_count_expire_time", ServerOpts.join_leave_count_expire_time);
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "oper_spam_countdown",
	      ServerOpts.oper_spam_countdown, "");
    dump_line(f, CONF_SECTION_END);

    dump_line(f, CONF_SECTION_BEGIN, "operator_options");
    dump_line_boolean(f, "staffhide", ServerOpts.staffhide);
    dump_line_boolean(f, "no_oper_jupiter", ServerOpts.no_oper_jupiter);
    dump_line(f, CONF_SECTION_END);

    dump_line(f, CONF_SECTION_BEGIN, "servicesconf");
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "nickserv_name", ServicesConf.nickserv);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "chanserv_name", ServicesConf.chanserv);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "memoserv_name", ServicesConf.memoserv);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "operserv_name", ServicesConf.operserv);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "statserv_name", ServicesConf.statserv);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "helpserv_name", ServicesConf.helpserv);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "services_name", ServicesConf.services_name);
    dump_line_boolean(f, "hide_ulined_servers", ServicesConf.hide_ulined_servers);
    dump_line(f, CONF_SECTION_END);

    dump_line(f, CONF_SECTION_BEGIN, "admin");
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "name", AdminInfo.name);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "email", AdminInfo.email);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "description", AdminInfo.description);
    dump_line(f, CONF_SECTION_END);

    dump_line(f, CONF_SECTION_BEGIN, "channelconf");
    dump_line_boolean(f, "default_extended_topic_limitation", ChannelConf.default_extended_topic_limitation);
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "default_quit_message", ChannelConf.default_quit_msg);
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "max_channels_per_user",
	      ChannelConf.max_channels_per_user, "");
    dump_line(f, CONF_SECTION_END);

    dump_line(f, CONF_SECTION_BEGIN, "listen");
    prev_li = next_li = NULL;
    for (li = ListenerPollList; li; li = next_li) {
	next_li = li->next;
	prev_li = li;
	if (IsListenerHttp(li))
	    continue;
	while (next_li && (uuid_compare(li->uuid, next_li->uuid) == 0)) {
	    prev_li = next_li;
	    next_li = next_li->next;
	}
	dump_line(f, CONF_SECTION_BEGIN_TABBED);
	if (prev_li && (prev_li != li->next) && (prev_li != li))
	    ircsnprintf(strBuf1, 200, "%d .. %d", prev_li->port, li->port);
	else
	    ircsnprintf(strBuf1, 200, "%d", li->port);
	dump_line(f, CONF_SECTION_ELEMENT_RAW, "port", strBuf1);
	if (li->vhost && li->vhost[0])
	    dump_line(f, CONF_SECTION_ELEMENT_STRING, "ip", li->vhost);
	if (IsListenerClient(li))
	    fbputs("\tclientport;\n", f);
	if (IsListenerServer(li))
	    fbputs("\tserverport;\n", f);
	if (IsListenerClientSSL(li))
	    fbputs("\tclient_ssl;\n", f);

	dump_line(f, CONF_SECTION_END_TABBED);
    }
    dump_line(f, CONF_SECTION_END);

    for (aclass = ConnectionClasses; aclass; aclass = aclass->next) {
	if (aclass->number == 0)
	    continue;
	dump_line(f, CONF_SECTION_BEGIN, "class");
	dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "class", aclass->number, "");
	dump_line_timed(f, "pingtime", aclass->ping_frequency);
	dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "connectfreq", aclass->connect_frequency, "");
	dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "max_links", aclass->max_connections, "");
	dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "sendq", aclass->max_sendq, " bytes");
	dump_line(f, CONF_SECTION_END);
    }

    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
	if (aconf->status & CONF_SERVER) {
	    dump_line(f, CONF_SECTION_BEGIN, "connect");
	    dump_line(f, CONF_SECTION_ELEMENT_STRING, "name", aconf->name);
	    dump_line(f, CONF_SECTION_ELEMENT_STRING, "host", aconf->host);
	    dump_line(f, CONF_SECTION_ELEMENT_STRING, "send_password", aconf->spasswd);
	    dump_line(f, CONF_SECTION_ELEMENT_STRING, "accept_password", aconf->passwd);
	    if (aconf->port > 0)
		dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "port", aconf->port, "");
	    if (aconf->class && aconf->class->number > 0)
		dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "class", aconf->class->number, "");
	    dump_line_boolean(f, "encrypted", aconf->flags & CONF_FLAGS_ENCRYPTED);
	    dump_line_boolean(f, "autoconnect", aconf->flags & CONF_FLAGS_ALLOW_AUTO_CONN);
	    dump_line_boolean(f, "compressed", aconf->flags & CONF_FLAGS_COMPRESSED);
	    dump_line_boolean(f, "ishub", aconf->flags & CONF_FLAGS_HUB);
	    dump_line_boolean(f, "ultimate", aconf->flags & CONF_FLAGS_ULTIMATE);
	    dump_line(f, CONF_SECTION_ELEMENT_RAW, "aftype",
		      ((aconf->aftype == AF_INET) ? "ipv4" : "ipv6"));
	    dump_line(f, CONF_SECTION_END);
	}
    }

    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
	if (aconf->status & CONF_OPERATOR) {
	    dump_line(f, CONF_SECTION_BEGIN, "operator");
	    dump_line(f, CONF_SECTION_ELEMENT_STRING, "name", aconf->name);
	    memset(strBuf1, 0, 200 * sizeof(char));
	    ircsnprintf(strBuf1, 200, "%s@%s", aconf->user, aconf->host);
	    dump_line(f, CONF_SECTION_ELEMENT_STRING, "user", strBuf1);
	    dump_line(f, CONF_SECTION_ELEMENT_STRING, "password", aconf->passwd);
	    if (aconf->class && aconf->class->number > 0)
		dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "class", aconf->class->number, "");
	    dump_line_boolean(f, "die", aconf->port & OFLAG_DIE);
	    dump_line_boolean(f, "restart", aconf->port & OFLAG_RESTART);
	    dump_line_boolean(f, "admin", aconf->port & OFLAG_ADMIN);
	    dump_line_boolean(f, "sadmin", aconf->port & OFLAG_SADMIN);
	    dump_line_boolean(f, "kill", aconf->port & OFLAG_OPKILL);
	    dump_line_boolean(f, "operdo", aconf->port & OFLAG_OPERDO);
	    dump_line_boolean(f, "gkline", aconf->port & OFLAG_GKLINE);
	    dump_line_boolean(f, "see_hidden", aconf->port & OFLAG_SEEHIDDEN);
	    dump_line_boolean(f, "login_via_web", aconf->flags & CONF_FLAGS_WEBLOGIN);
	    dump_line(f, CONF_SECTION_END);
	}
    }

    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
        if ((aconf->status & CONF_CLIENT) && (!aconf->localhost || (aconf->localhost[0] == '\0'))) {
            dump_line(f, CONF_SECTION_BEGIN, "auth");
            memset(strBuf1, 0, 200 * sizeof(char));
            ircsnprintf(strBuf1, 200, "%s@%s", aconf->user, aconf->host);
            dump_line(f, CONF_SECTION_ELEMENT_STRING, "user", strBuf1);
            if (aconf->passwd && aconf->passwd[0])
                dump_line(f, CONF_SECTION_ELEMENT_STRING, "password", aconf->passwd);
            if (aconf->class && aconf->class->number > 0)
                dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "class", aconf->class->number, "");
            if (aconf->localhost && aconf->localhost[0])
                dump_line(f, CONF_SECTION_ELEMENT_STRING, "spoof", aconf->localhost);
            dump_line_boolean(f, "spoof_notice", aconf->flags & CONF_FLAGS_SPOOF_NOTICE);
            dump_line_boolean(f, "can_choose_fakehost", aconf->flags & CONF_FLAGS_ALLOW_FAKEHOST);
            dump_line_boolean(f, "kline_exempt", aconf->flags & CONF_FLAGS_EXEMPTKLINE);
            dump_line_boolean(f, "exceed_limit", aconf->flags & CONF_FLAGS_NOLIMIT);
            if (aconf->port > 0)
                dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "port", aconf->port, "");
            dump_line(f, CONF_SECTION_END);
        }
    }

    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
	if ((aconf->status & CONF_CLIENT) && (aconf->localhost && aconf->localhost[0])) {
	    dump_line(f, CONF_SECTION_BEGIN, "auth");
	    memset(strBuf1, 0, 200 * sizeof(char));
	    ircsnprintf(strBuf1, 200, "%s@%s", aconf->user, aconf->host);
	    dump_line(f, CONF_SECTION_ELEMENT_STRING, "user", strBuf1);
	    if (aconf->passwd && aconf->passwd[0])
		dump_line(f, CONF_SECTION_ELEMENT_STRING, "password", aconf->passwd);
	    if (aconf->class && aconf->class->number > 0)
		dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "class", aconf->class->number, "");
	    if (aconf->localhost && aconf->localhost[0])
		dump_line(f, CONF_SECTION_ELEMENT_STRING, "spoof", aconf->localhost);
	    dump_line_boolean(f, "spoof_notice", aconf->flags & CONF_FLAGS_SPOOF_NOTICE);
	    dump_line_boolean(f, "can_choose_fakehost", aconf->flags & CONF_FLAGS_ALLOW_FAKEHOST);
	    dump_line_boolean(f, "kline_exempt", aconf->flags & CONF_FLAGS_EXEMPTKLINE);
	    dump_line_boolean(f, "exceed_limit", aconf->flags & CONF_FLAGS_NOLIMIT);
	    if (aconf->port > 0)
		dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "port", aconf->port, "");
	    dump_line(f, CONF_SECTION_END);
	}
    }

#ifndef STATIC_MODULES
    dump_line(f, CONF_SECTION_BEGIN, "modules");
    for (ptr = loaded_module_list.head; ptr; ptr = ptr->next) {
	mod = ptr->data;
	if (mod->flags & MODULE_FROMCONFIG) 
	    dump_line(f, CONF_SECTION_ELEMENT_STRING, "module", mod->pathname);
    }
    dump_line(f, CONF_SECTION_END);
#endif

    dump_line(f, CONF_SECTION_BEGIN, "proxy_monitor_options");
    dump_line_boolean(f, "enable", GeneralOpts.enable_proxymonitor);

    /*for (ptr = loaded_module_list.head; ptr; ptr = ptr->next) {
     *   mod = ptr->data;
     *   if (mod->flags & MODULE_PROXYMODULE) {
     *       dump_line(f, CONF_SECTION_ELEMENT_STRING, "module", mod->pathname);
     *	    break;
     *	}
     * }
     * 
     */
	
    if (strstr((const char *)GeneralOpts.proxymodulefile, "proxymon.so"))
	dump_line(f, CONF_SECTION_ELEMENT_STRING, "module", GeneralOpts.proxymodulefile);
	
    
    if ((GeneralOpts.proxyconffile == NULL) || (GeneralOpts.proxyconffile[0] == '\0'))
	DupString(GeneralOpts.proxyconffile, "socks.conf");
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "configuration_file", GeneralOpts.proxyconffile);
    dump_line(f, CONF_SECTION_END);

    dump_line(f, CONF_SECTION_BEGIN, "httpd_options");
    dump_line_boolean(f, "enable", GeneralOpts.enable_httpd);
#ifndef STATIC_MODULES
    for (ptr = loaded_module_list.head; ptr; ptr = ptr->next) {
        mod = ptr->data;
        if (mod->flags & MODULE_HTTPDMODULE) {
            dump_line(f, CONF_SECTION_ELEMENT_STRING, "module", mod->pathname);
            break;
        }
    }
#endif
    if ((GeneralOpts.httpdconffile == NULL) || (GeneralOpts.httpdconffile[0] == '\0'))
        DupString(GeneralOpts.httpdconffile, "ihttpd.conf");
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "configuration_file", GeneralOpts.httpdconffile);
    dump_line(f, CONF_SECTION_END);

    dump_line(f, CONF_SECTION_BEGIN, "files");
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "load", IRCD_PREFIX_ETC "/maskfile.conf");
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "pidfile", GeneralOpts.PIDFile);
    dump_line(f, CONF_SECTION_END);

    fbclose(f);

    lblen = 0;

    generate_maskitem_config_file(IRCD_PREFIX_ETC "/maskfile.conf");
    generate_proxymon_config_file(IRCD_PREFIX_ETC "/socks.conf");
    generate_ihttpd_config_file(IRCD_PREFIX_ETC "/ihttpd.conf");

    return 0;
}

int generate_maskitem_config_file(char *filename)
{
    FBFILE *f;

    struct MaskItem *ami;
    dlink_node *ptr;
    int htype = 0;
    char strBuf1[200];

    f = fbopen(filename, "w");

    if (f == NULL)
        return -1;

    htype = MASKITEM_KLINE_CONFIG - 1;

    for (ptr = maskitem_list[htype].head; ptr; ptr = ptr->next) {
        ami = ptr->data;
        if (!ami)
            continue;
	memset(strBuf1, 0, 200 * sizeof(char));
	ircsnprintf(strBuf1, 200, "%s@%s", ami->username, ami->string);
	dump_line(f, CONF_MASKFILE_ITEM, "kline", "userhost", strBuf1, ami->reason);
    }

    htype = MASKITEM_QUARANTINE_CONFIG - 1;

    for (ptr = maskitem_list[htype].head; ptr; ptr = ptr->next) {
        ami = ptr->data;
        if (!ami)
            continue;
        dump_line(f, CONF_MASKFILE_ITEM, "quarantine", "masktext", ami->string, ami->reason);
    }

    htype = MASKITEM_JUPITER_CONFIG - 1;

    for (ptr = maskitem_list[htype].head; ptr; ptr = ptr->next) {
        ami = ptr->data;
        if (!ami)
            continue;
        dump_line(f, CONF_MASKFILE_ITEM, "channel_jupiter", "masktext", ami->string, ami->reason);
    }

    htype = MASKITEM_GECOS_CONFIG - 1;

    for (ptr = maskitem_list[htype].head; ptr; ptr = ptr->next) {
        ami = ptr->data;
        if (!ami)
            continue;
        dump_line(f, CONF_MASKFILE_ITEM, "gecos_deny", "masktext", ami->string, ami->reason);
    }

    htype = MASKITEM_ZAPLINE_CONFIG - 1;

    for (ptr = maskitem_list[htype].head; ptr; ptr = ptr->next) {
        ami = ptr->data;
        if (!ami)
            continue;
        dump_line(f, CONF_MASKFILE_ITEM, "zap_address", "masktext", ami->string, ami->reason);
    }

    fbclose(f);

    lblen = 0;
    return 0;
}

int generate_proxymon_config_file(char *filename)
{
    FBFILE *f;

    char strBuf1[200];

    f = fbopen(filename, "w");

    if (f == NULL)
        return -1;

    dump_line(f, CONF_COMMENT_BEGIN);
    dump_line(f, CONF_COMMENT_LINE, "TR-IRCD Proxy Monitor Module Configuration file");
    dump_line(f, CONF_COMMENT_LINE, date(0));
    dump_line(f, CONF_COMMENT_END);

    dump_line(f, CONF_SECTION_BEGIN, "proxymon_config");
    dump_line(f, CONF_SECTION_BEGIN_TABBED_NAMED, "main");
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "fd_limit", ScannerConf.fd_limit, "");
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "dnsbl_hostname", ScannerConf.dnsblhost);
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "max_read", ScannerConf.max_read, "");
    dump_line_timed(f, "timeout", ScannerConf.timeout);
    dump_line_boolean(f, "negfail_notices", ScannerConf.negfail_notices);
    memset(strBuf1, 0, 200 * sizeof(char));
    ircsnprintf(strBuf1, 200, "%s%s%s%s%s", 
		(ScannerConf.action & PROXYMON_GNOTICE) ? "global_notice" : "",
		(ScannerConf.action & (PROXYMON_GNOTICE|PROXYMON_AUTOKILL)) == (PROXYMON_GNOTICE|PROXYMON_AUTOKILL) ? ", " 
			: ((ScannerConf.action & (PROXYMON_GNOTICE|PROXYMON_KLINE)) == (PROXYMON_GNOTICE|PROXYMON_KLINE) ? ", " : ""),
		(ScannerConf.action & PROXYMON_AUTOKILL) ? "autokill" : "",
		(ScannerConf.action & (PROXYMON_AUTOKILL|PROXYMON_KLINE)) == (PROXYMON_AUTOKILL|PROXYMON_KLINE) ? ", " : "",
		(ScannerConf.action & PROXYMON_KLINE) ? "kline" : "");
    if (ScannerConf.action)
	dump_line(f, CONF_SECTION_ELEMENT_RAW, "action", strBuf1);
    dump_line(f, CONF_SECTION_END_TABBED);
    dump_line(f, CONF_SECTION_BEGIN_TABBED_NAMED, "scanner");
    dump_line(f, CONF_SECTION_ELEMENT_STRING, "check_ip", ScannerConf.scan_ip);
    dump_line(f, CONF_SECTION_ELEMENT_NUMBER, "check_port", ScannerConf.scan_port, "");
    if (ScannerConf.bind_ip && ScannerConf.bind_ip[0])
    	dump_line(f, CONF_SECTION_ELEMENT_STRING, "bind_ip", ScannerConf.bind_ip);
    dump_line(f, CONF_SECTION_END_TABBED);
    dump_line(f, CONF_SECTION_END);

    fbclose(f);

    lblen = 0;
    return 0;
}

int generate_ihttpd_config_file(char *filename)  
{
    FBFILE *f;
    struct Listener *li;
    struct Listener *next_li;
    struct Listener *prev_li;
    dlink_node *ptr;	
    
    char strBuf1[200];

    f = fbopen(filename, "w");

    if (f == NULL)
        return -1;

    dump_line(f, CONF_COMMENT_BEGIN);
    dump_line(f, CONF_COMMENT_LINE, "TR-IRCD Httpd Module Configuration file");
    dump_line(f, CONF_COMMENT_LINE, date(0));
    dump_line(f, CONF_COMMENT_END);

    dump_line(f, CONF_SECTION_BEGIN, "httpd_config");
    dump_line(f, CONF_SECTION_BEGIN_TABBED_NAMED, "listen");
    prev_li = next_li = NULL;
    for (li = ListenerPollList; li; li = next_li) {
        next_li = li->next;
        prev_li = li;
        if (!IsListenerHttp(li))
            continue;
        while (next_li && (uuid_compare(li->uuid, next_li->uuid) == 0)) {
            prev_li = next_li;
            next_li = next_li->next;
        }
        dump_line(f, CONF_SECTION_BEGIN_TABBED);
        if (prev_li && (prev_li != li->next) && (prev_li != li))
            ircsnprintf(strBuf1, 200, "%d .. %d", prev_li->port, li->port);
        else
            ircsnprintf(strBuf1, 200, "%d", li->port);
        dump_line(f, CONF_SECTION_ELEMENT_RAW, "port", strBuf1);
        if (li->vhost && li->vhost[0])
            dump_line(f, CONF_SECTION_ELEMENT_STRING, "ip", li->vhost);
        if (IsListenerClientSSL(li))
            fbputs("\tssl;\n", f);
	if (li->host_header && li->host_header[0])
	    dump_line(f, CONF_SECTION_ELEMENT_STRING, "host_header", li->host_header);
        dump_line(f, CONF_SECTION_END_TABBED);
    }
    dump_line(f, CONF_SECTION_END_TABBED);

    dump_line(f, CONF_SECTION_BEGIN_TABBED_NAMED, "index_page");
    dump_line_boolean(f, "require", HttpdConf.require_index);
    if (HttpdConf.index_file && HttpdConf.index_file[0])
	dump_line(f, CONF_SECTION_ELEMENT_STRING, "value", HttpdConf.index_file);
    dump_line(f, CONF_SECTION_END_TABBED);


    dump_line(f, CONF_SECTION_BEGIN_TABBED_NAMED, "options");
    dump_line_permission(f, "policy", HttpdConf.policy);
    dump_line(f, CONF_SECTION_BEGIN_TABBED_NAMED, "except");

    for (ptr = http_except_list.head; ptr; ptr = ptr->next) 
	dump_line(f, CONF_SECTION_ELEMENT_RAW_STRING, (char *)(ptr->data));    
    
    dump_line(f, CONF_SECTION_END_TABBED);    
    dump_line(f, CONF_SECTION_END_TABBED);    
    
    dump_line(f, CONF_SECTION_END);

    fbclose(f);

    lblen = 0;
    return 0;
}
  

void backup_configuration_files(void) {
    char newname1[MAXPATHLEN];
    char newname2[MAXPATHLEN];
    char newname3[MAXPATHLEN];
    char newname4[MAXPATHLEN];

    ircsnprintf(newname1, MAXPATHLEN, "%s.bak-%lu", CONFIGFILE, time(NULL));  
    ircsnprintf(newname2, MAXPATHLEN, "%s.bak-%lu", IRCD_PREFIX_ETC "/maskfile.conf", time(NULL));
    ircsnprintf(newname3, MAXPATHLEN, "%s.bak-%lu", IRCD_PREFIX_ETC "/socks.conf", time(NULL));
    ircsnprintf(newname4, MAXPATHLEN, "%s.bak-%lu", IRCD_PREFIX_ETC "/ihttpd.conf", time(NULL));

    rename(CONFIGFILE, newname1);
    rename(IRCD_PREFIX_ETC "/maskfile.conf", newname2);
    rename(IRCD_PREFIX_ETC "/socks.conf", newname3);
    rename(IRCD_PREFIX_ETC "/ihttpd.conf", newname4);

}

