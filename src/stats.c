/*
 *   IRC - Internet Relay Chat, src/stats.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
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
#include "tools.h"
#include "channel.h"
#include "class.h"
#include "event.h"
#include "listener.h"
#include "modules.h"
#include "numeric.h"
#include "fd.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "fileio.h"
#include "zlink.h"

void dummy_call(aClient *cptr, char *text);

struct stats ircst, *ircstp = &ircst;

void initstats()
{
    memset((char *) &ircst, '\0', sizeof(ircst));
}

void report_connect_blocks(aClient *sptr, int all)
{
    static char null[] = "<NULL>";
    aConfItem *tmp;
    int port;
    char *host, *pass, *name;

    for (tmp = GlobalConfItemList; tmp; tmp = tmp->next)
	if (tmp->status & CONF_SERVER) {
	    host = "*";
	    pass = BadPtr(tmp->passwd) ? null : tmp->passwd;
	    name = BadPtr(tmp->name) ? null : tmp->name;
	    port = (int) tmp->port;

	    if (all || find_server(name))
		send_me_numeric(sptr, RPL_STATSCLINE, 'C', host, name, port, get_conf_class(tmp));
	}
    return;
}

void report_connect_blocks_flag(aClient *sptr, int confflag, int reply, char letter)
{
    static char null[] = "<NULL>";
    aConfItem *tmp;
    int port;
    char *host, *pass, *name;

    for (tmp = GlobalConfItemList; tmp; tmp = tmp->next)
	if (tmp->status & CONF_SERVER) {
	    host = "*";
	    pass = BadPtr(tmp->passwd) ? null : tmp->passwd;
	    name = BadPtr(tmp->name) ? null : tmp->name;
	    port = (int) tmp->port;

	    if (tmp->flags & confflag)
		send_me_numeric(sptr, reply, letter, host, name, port, get_conf_class(tmp));
	}
    return;
}

void report_auth_blocks(aClient *sptr, int onlywhat)
{
    static char null[] = "<NULL>";
    aConfItem *tmp;
    int port;
    char *user, *host, *pass, *name, uhost[USERLEN + HOSTLEN + 2];

    for (tmp = GlobalConfItemList; tmp; tmp = tmp->next)
	if (tmp->status & CONF_CLIENT) {
	    user = BadPtr(tmp->user) ? null : tmp->user;
	    host = BadPtr(tmp->host) ? null : tmp->host;
	    pass = BadPtr(tmp->passwd) ? null : tmp->passwd;
	    name = BadPtr(tmp->name) ? null : tmp->name;
	    port = (int) tmp->port;
	    strlcpy_irc(uhost, user, sizeof(user));
	    strncat(uhost, "@", 1);
	    strncat(uhost, host, sizeof(host));

	    if (onlywhat) {
		if (tmp->flags & onlywhat)
		    send_me_numeric(sptr, RPL_STATSILINE, 'I', uhost, name,
				    port, get_conf_class(tmp));
	    } else {
		send_me_numeric(sptr, RPL_STATSILINE, 'I', uhost, name, port, get_conf_class(tmp));
	    }
	}
    return;
}

void report_operator_blocks(aClient *sptr)
{
    static char null[] = "<NULL>";
    aConfItem *tmp;
    int port;
    char *user, *host, *pass, *name, uhost[USERLEN + HOSTLEN + 2];

    for (tmp = GlobalConfItemList; tmp; tmp = tmp->next)
	if (tmp->status & CONF_OPERATOR) {
	    user = BadPtr(tmp->user) ? null : tmp->user;
	    host = BadPtr(tmp->host) ? null : tmp->host;
	    pass = BadPtr(tmp->passwd) ? null : tmp->passwd;
	    name = BadPtr(tmp->name) ? null : tmp->name;
	    port = (int) tmp->port;
	    strlcpy_irc(uhost, user, sizeof(user));
	    strncat(uhost, "@", 1);
	    strncat(uhost, host, sizeof(host));

	    send_me_numeric(sptr, RPL_STATSOLINE, 'O', uhost, name, port, get_conf_class(tmp));
	}
    return;
}

void report_service_blocks(aClient *sptr, int all)
{
    static char null[] = "<NULL>";
    aConfItem *tmp;
    int port;
    char *host, *pass, *name;

    for (tmp = GlobalConfItemList; tmp; tmp = tmp->next)
	if (tmp->status & CONF_SERVICE) {
	    host = BadPtr(tmp->host) ? null : tmp->host;
	    pass = BadPtr(tmp->passwd) ? null : tmp->passwd;
	    name = BadPtr(tmp->name) ? null : tmp->name;
	    port = (int) tmp->port;

	    if (all || find_client(name))
		send_me_numeric(sptr, RPL_STATSSLINE, 'S', host, name, port, get_conf_class(tmp));
	}
    return;
}

void show_opers(aClient *cptr, char *name)
{
    aClient *acptr;
    dlink_node *ptr;
    int j = 0;

    for (ptr = locoper_list.head; ptr; ptr = ptr->next) {
	acptr = ptr->data;
	if (!acptr)
	    continue;
	j++;
	send_me_debug(cptr, ":%~C Idle: %d",
		   acptr, timeofday - acptr->user->last);
    }
    send_me_debug(cptr, ":%d Operator%s", j, (j == 1) ? "" : "s");
}

void show_servers(aClient *cptr, char *name)
{
    aClient *acptr;
    dlink_node *ptr;
    int j = 0;

    for (ptr = serv_list.head; ptr; ptr = ptr->next) {
	acptr = ptr->data;
	if (!acptr)
	    continue;
	if (ServicesConf.hide_ulined_servers && IsULine(acptr) && !IsSeeHidden(cptr))
	    continue;
	j++;
	send_me_debug(cptr, ":%s (%s) Idle: %d",
		   acptr->name, (acptr->serv->bynick[0] ? acptr->serv->bynick : "Remote."), 
		   timeofday - acptr->lasttime);
    }
    send_me_debug(cptr, ":%d Server%s", j, (j == 1) ? "" : "s");
}

#define _1MEG   (1024.0)
#define _1GIG   (1024.0*1024.0)
#define _1TER   (1024.0*1024.0*1024.0)
#define _GMKs(x)        ( (x > _1TER) ? "Terabytes" : ((x > _1GIG) ? "Gigabytes" : \
                        ((x > _1MEG) ? "Megabytes" : "Kilobytes")))
#define _GMKv(x)        ( (x > _1TER) ? (float)(x/_1TER) : ((x > _1GIG) ? \
                        (float)(x/_1GIG) : ((x > _1MEG) ? (float)(x/_1MEG) : (float)x)))

void serv_info(aClient *cptr, char *name)
{
    static char Lformat[] = ":%C %N %s %s %u %u %u %u %u :%u %u TS";
    int i = 0;
    long sendK, receiveK, uptime;
    dlink_node *ptr;
    aClient *acptr;

    sendK = receiveK = 0;

    for (ptr = serv_list.head; ptr; ptr = ptr->next) {
        acptr = ptr->data;
        if (!acptr)
            continue;
        if (ServicesConf.hide_ulined_servers && IsULine(acptr) && !IsSeeHidden(cptr))
            continue;
        sendK += acptr->sendK;
        receiveK += acptr->receiveK;
        sendto_one(cptr, Lformat, &me, RPL_STATSLINKINFO,
                   name, get_client_name(acptr, HIDEME),
                   (int) linebuf_len(&acptr->sendQ),
                   (int) acptr->sendM, (int) acptr->sendK,
                   (int) acptr->receiveM, (int) acptr->receiveK,
                   timeofday - acptr->firsttime,
                   timeofday - acptr->since);
        if (ZipOut(acptr)) {
    unsigned long ib, ob;
    double rat;

            zip_get_stats(acptr->serv->zout, &ib, &ob, &rat);
            if (ib) {
                send_me_debug(cptr, 
                           ":Zip inbytes %d, outbytes %d (%3.2f%%)", ib, ob, rat);
            }
        }
        if (ZipIn(acptr)) {
    unsigned long ib, ob;
    double rat;   

            zip_get_stats(acptr->serv->zin, &ib, &ob, &rat);
            if (ib) {
                send_me_debug(cptr,
                           ":Unzip inbytes %d, outbytes %d (%3.2f%%)", ib, ob, rat);
            }
        }
        i++;
    }
    send_me_debug(cptr, "%u total server%s", i, (i == 1) ? "" : "s");
    send_me_debug(cptr, "Sent total : %7.2f %s", _GMKv(sendK), _GMKs(sendK));
    send_me_debug(cptr, "Recv total : %7.2f %s", _GMKv(receiveK), _GMKs(receiveK));
    uptime = (timeofday - me.since);
    send_me_debug(cptr, "Server send: %7.2f %s (%4.1f K/s)", _GMKv(me.sendK),
               _GMKs(me.sendK), (float) ((float) me.sendK / (float) uptime));
    send_me_debug(cptr, "Server recv: %7.2f %s (%4.1f K/s)", _GMKv(me.receiveK),
               _GMKs(me.receiveK), (float) ((float) me.receiveK / (float) uptime));
}

void tstats(aClient *cptr, char *name)
{
    aClient *acptr;
    dlink_node *ptr;
    struct stats *sp;
    struct stats tmp;

    sp = &tmp;
    memcpy((char *) sp, (char *) ircstp, sizeof(*sp));
    for (ptr = lclient_list.head; ptr; ptr = ptr->next) {
        if (!(acptr = ptr->data))
            continue;
        if (IsServer(acptr)) {
            sp->is_sbs += acptr->sendB;
            sp->is_sbr += acptr->receiveB;
            sp->is_sks += acptr->sendK;
            sp->is_skr += acptr->receiveK;
            sp->is_sti += timeofday - acptr->firsttime;
            sp->is_sv++;
            if (sp->is_sbs > 1023) {
                sp->is_sks += (sp->is_sbs >> 10);
                sp->is_sbs &= 0x3ff;
            }
            if (sp->is_sbr > 1023) {
                sp->is_skr += (sp->is_sbr >> 10);
                sp->is_sbr &= 0x3ff;
            }
        } else if (IsClient(acptr)) {
            sp->is_cbs += acptr->sendB;
            sp->is_cbr += acptr->receiveB;
            sp->is_cks += acptr->sendK;
            sp->is_ckr += acptr->receiveK;
            sp->is_cti += timeofday - acptr->firsttime;
            sp->is_cl++;
            if (sp->is_cbs > 1023) {
                sp->is_cks += (sp->is_cbs >> 10);
                sp->is_cbs &= 0x3ff;
            }
            if (sp->is_cbr > 1023) {
                sp->is_ckr += (sp->is_cbr >> 10);
                sp->is_cbr &= 0x3ff;
            }

        } else if (IsUnknown(acptr))
            sp->is_ni++;
    }
    send_me_debug(cptr, "accepts %u refused %u", sp->is_ac, sp->is_ref);
    send_me_debug(cptr, "unknown commands %u prefixes %u", sp->is_unco, sp->is_unpf);
    send_me_debug(cptr, "nick collisions %u unknown closes %u", sp->is_kill, sp->is_ni);
    send_me_debug(cptr, "wrong direction %u empty %u", sp->is_wrdi, sp->is_empt);
    send_me_debug(cptr, "numerics seen %u mode fakes %u", sp->is_num, sp->is_fake);
    send_me_debug(cptr, "auth successes %u fails %u", sp->is_asuc, sp->is_abad);
    send_me_debug(cptr, "local connections %u udp packets %u", sp->is_loc, sp->is_udp);
    send_me_debug(cptr, "Client Server");
    send_me_debug(cptr, "connected %u %u", sp->is_cl, sp->is_sv);
    send_me_debug(cptr, "bytes sent %u.%uK %u.%uK", sp->is_cks, sp->is_cbs, sp->is_sks, sp->is_sbs);
    send_me_debug(cptr, "bytes recv %u.%uK %u.%uK", sp->is_ckr, sp->is_cbr, sp->is_skr, sp->is_sbr);
    send_me_debug(cptr, "time connected -u %u", sp->is_cti, sp->is_sti);
}

void report_protocol(aClient *cptr)
{
    send_me_numeric(cptr, RPL_SERVERPROTO, "Native");
}

/* This dummy function here can be used to "call" all the functions
 * bound to the ircd via a library and therefore could be hidden when
 * linked 
 * -TimeMr14C
 */

void dummy_call(aClient *cptr, char *text) {
	
    uuid_t u;
    char str[36];

    send_usage(cptr, text);
    count_memory(cptr, text); 
    check_update(cptr);

    uuid_generate(u);   
    uuid_unparse(u, str); 
    generate_server_config_file(NULL);
}
