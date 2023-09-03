/************************************************************************
 *   IRC - Internet Relay Chat, src/proxymon/pm_main.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
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
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "msg.h"
#include "numeric.h"
#include "s_conf.h"
#include "proxy.h"
#include "interproc.h"

aClient *qcptr = NULL;
int do_rehash_pm(struct hook_data *data);

int check_proxychk(aClient *cptr, aClient *sptr, int parc, char *parv[]);
int stats_proxychk(aClient *cptr, aClient *sptr, int parc, char *parv[]);
int help_proxychk(aClient *cptr, aClient *sptr, int parc, char *parv[]);

/* command structure */

typedef struct {
    char *name;
    char *params;
    int (*function) ();
} ProxyChkItem;

static ProxyChkItem pclist[4] = {
    {"CHECK", "/proxychk check \02ip\02 [type] [port] : Scans for an IP", check_proxychk},
    {"STATS", "/proxychk stats : Displays statistics", stats_proxychk},
    {"HELP", "/proxychk help : Displays this help", help_proxychk},
    {NULL, 0},
};

int do_rehash_pm(struct hook_data *data)
{
    if (!IsAnOper(data->source_p))
	return 0;

    if (irc_strncmp(data->data, "PROXYMON", 8) == 0) {
	sendto_ops("%C is rehashing proxy monitor configuration", data->source_p);
	read_proxy_configuration();
	reconfigure_proxy_monitor(IproxyScan);
	reconfigure_proxy_monitor(CproxyScan);
    }

    return 0;
}

int m_proxychk(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    int i;

    if (parc < 2)
	return help_proxychk(cptr, sptr, parc, parv);

    for (i = 0; pclist[i].name; i++) {
	if (irc_strcmp(pclist[i].name, parv[1]) == 0)
	    return pclist[i].function(cptr, sptr, parc, parv);
    }

    return help_proxychk(cptr, sptr, parc, parv);
}

int help_proxychk(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    int i;

    send_me_numeric(cptr, RPL_PROXYHELP, "Available Proxychk parametres");

    for (i = 0; pclist[i].name; i++)
	send_me_numeric(cptr, RPL_PROXYHELP, pclist[i].params);

    send_me_numeric(cptr, RPL_PROXYHELP, "Available Proxy Types:");

    for (i = 1; i < 7; i++)
	send_me_numeric(cptr, RPL_PROXYHELP, protocol_types[i]);

    send_me_numeric(cptr, RPL_PROXYHELP, "End of Proxychk help");

    return 0;
}

int stats_proxychk(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    display_proxy_scanner_statistics(cptr);

    return 0;
}

int check_proxychk(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    /* Someone typed /proxychk check
     * the command is incomplete.
     * -TimeMr14C
     */

    if (parc < 3)
	return help_proxychk(cptr, sptr, parc, parv);

    /* Someone typed /proxychk check <ip>
     * that command is ok.
     */

    if (parc < 4) {
	if (cmd_scanner_active != 1) {
	    qcptr = cptr;
	    cmd_addto_scanner(parv[2], NULL, 0);
	    cmd_scanner_active = 1;
	    send_me_notice(cptr, ":The IP [%s] has been added to the scanner queue", parv[2]);
	} else {
	    send_me_notice(cptr, ":Please wait for the previous scan to end");
	}
	return 0;
    }

    /*
     * Someone typed /proxychk check <ip> <something>
     * The command is again incomplete
     */

    if (parc < 5)
	return help_proxychk(cptr, sptr, parc, parv);

    if (parc < 6) {
    int port = 0;
	port = atoi(parv[4]);
	if (cmd_scanner_active != 1) {
	    qcptr = cptr;
	    cmd_addto_scanner(parv[2], parv[3], port);
	    cmd_scanner_active = 1;
	    send_me_notice(cptr,
			   ":The IP [%s] has been added to the scanner queue to check also %s on port %d",
			   parv[2], parv[3], port);
	} else {
	    send_me_notice(cptr, ":Please wait for the previous scan to end");
	}
	return 0;
    }

    return help_proxychk(cptr, sptr, parc, parv);

}

void pm_action(char *ip, int port, char *reason, int type)
{
    char buffer[1024];
    char *user = "*";

    if (type & PROXYMON_KLINE) {

	ircsnprintf(buffer, 1023, "Detected %s proxy on your host %s (%s)", reason, ip,
		    smalldate((time_t) time(NULL)));

	send_interproc_message(pfd, "%s %s %s %d :%s", TOK1_GLINE, ip, user,
			      (ServerOpts.default_kline_time ? ServerOpts.default_kline_time : 30), 
			      buffer);

	send_interproc_message(pfd, "%s %s :Added temporary %d min. K-Line for [%s@%s] [%s proxy]",
		   	      TOK1_NOTICE, "&PROXYMON", 
			      (ServerOpts.default_kline_time ? ServerOpts.default_kline_time : 30),
			      user, ip, reason);
    }

    if (type & PROXYMON_AUTOKILL) {

        ircsnprintf(buffer, 1023, "Detected %s proxy on your host %s (%s)", reason, ip, 
		    smalldate((time_t) time(NULL)));

        send_interproc_message(pfd, "%s %s %s %d Proxymon %lu :%s", TOK1_AKILL, ip, user, 
                              (ServerOpts.default_kline_time ? ServerOpts.default_kline_time : 30),
                              time(NULL), buffer);

        send_interproc_message(pfd, "%s %s :Added temporary %d min. K-Line for [%s@%s] [%s proxy]",
                              TOK1_NOTICE, "&PROXYMON", 
                              (ServerOpts.default_kline_time ? ServerOpts.default_kline_time : 30),
                              user, ip, reason);

    }

    if (type & PROXYMON_GNOTICE) {

        send_interproc_message(pfd, "%s :Detected %s proxy on ip %s port %d", TOK1_GLOBOPS, reason, ip, port);

    }
}

