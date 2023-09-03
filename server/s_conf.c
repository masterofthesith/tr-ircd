/************************************************************************
 *   IRC - Internet Relay Chat, server/s_conf.c
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
#include "class.h"
#include "event.h"
#include "listener.h"
#include "modules.h"
#include "numeric.h"
#include "fd.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "throttle.h"
#include "fileio.h"

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned int) 0xffffffff)
#endif

/* internally defined functions */

char conf_line_in[256];

static int attach_iline(struct Client *, struct ConfItem *);
int attach_Iline(struct Client *client_p, char *username);

/*
 * conf_dns_callback - called when resolver query finishes
 * if the query resulted in a successful search, hp will contain
 * a non-null pointer, otherwise hp will be null.
 * if successful save hp in the conf item it was called with
 */
static void conf_dns_callback(void *vptr, adns_answer * reply)
{
    struct ConfItem *aconf = (struct ConfItem *) vptr;
    if (reply && reply->status == adns_s_ok) {
#ifdef IPV6
	copy_s_addr(IN_ADDR(aconf->ipnum), reply->rrs.addr->addr.inet6.sin6_addr.s6_addr);
#else
	copy_s_addr(IN_ADDR(aconf->ipnum), reply->rrs.addr->addr.inet.sin_addr.s_addr);
#endif
	MyFree(reply);
    }

    MyFree(aconf->dns_query);
    aconf->dns_query = NULL;
}

/*
 * conf_dns_lookup - do a nameserver lookup of the conf host
 * if the conf entry is currently doing a ns lookup do nothing, otherwise
 * set the conf_dns_pending flag
 */
static void conf_dns_lookup(struct ConfItem *aconf)
{
    if (aconf->dns_query == NULL) {
	aconf->dns_query = MyMalloc(sizeof(struct DNSQuery));
	aconf->dns_query->ptr = aconf;
	aconf->dns_query->callback = conf_dns_callback;
	adns_gethost(aconf->host, aconf->aftype, aconf->dns_query);
    }
}

/*
 * check_client
 *
 * inputs       - pointer to client
 * output       - 0 = Success
 *                NOT_AUTHORIZED (-1) = Access denied (no I line match)
 *                SOCKET_ERROR   (-2) = Bad socket.
 *                I_LINE_FULL    (-3) = I-line is full
 *                TOO_MANY       (-4) = Too many connections from hostname
 *                BANNED_CLIENT  (-5) = K-lined
 * side effects - Ordinary client access check.
 *                Look for conf lines which have the same
 *                status as the flags passed.
 */
int check_client(struct Client *client_p, struct Client *source_p, char *username)
{
    int i;

    ClearAccess(source_p);

    i = attach_Iline(source_p, username);

    switch (i) {
	case SOCKET_ERROR:
	    exit_client(source_p, &me, "Socket Error");
	    break;

	case TOO_MANY:
	    sendto_lev(SNOTICE_LEV, "Too many connections on IP from %s (%s).",
		       get_client_name(source_p, FALSE), source_p->sockhost);

	    ircstp->is_ref++;
	    exit_client(source_p, &me, "No more connections allowed on that IP");
	    break;

	case I_LINE_FULL:
	    sendto_lev(SNOTICE_LEV,
		       "I-line is full for %s (%s).",
		       get_client_name(source_p, FALSE), source_p->sockhost);

	    ircstp->is_ref++;
	    exit_client(source_p, &me,
			       "No more connections allowed in your connection class");
	    break;
	
	case INVALID_CONNECTION:
	    sendto_lev(REJ_LEV, "Unauthorized client connection from %~C on non-client port", source_p);
	    exit_client(source_p, &me, "You are not authorized to use this port");
	    break;

	case NOT_AUTHORIZED:
	    {
    static char ipaddr[HOSTIPLEN];
		ircstp->is_ref++;
		/* jdc - lists server name & port connections are on */
		/*       a purely cosmetical change */
		inetntop(source_p->aftype, &IN_ADDR(source_p->ip), ipaddr, HOSTIPLEN);
		sendto_lev(REJ_LEV,
			   "Unauthorised client connection from %s [%s] on [%s/%u].",
			   get_client_name(source_p, FALSE), ipaddr,
			   source_p->listener->name, source_p->listener->port);

		exit_client(source_p, &me,
				   "You are not authorized to use this server");
		break;
	    }
	case BANNED_CLIENT:
	    exit_client(client_p, &me, "*** Banned ");
	    ircstp->is_ref++;
	    break;

	case 0:
	default:
	    break;
    }
    return (i);
}

/*
 * attach_Iline
 *
 * inputs       -
 * output       -
 * side effect  - find the first (best) I line to attach.
 */
int attach_Iline(struct Client *client_p, char *username)
{
    struct ConfItem *aconf;

    aconf = find_conf_for_client(client_p, CONF_CLIENT, 1);

    if (aconf == NULL)
	aconf = find_conf_for_client(client_p, CONF_CLIENT, 0);

    if (aconf != NULL) {
	if (aconf->status & CONF_CLIENT) {
	    if (aconf->flags & CONF_FLAGS_REDIR) {
		send_me_numeric(client_p, RPL_REDIR, aconf->name ? aconf->name : "", aconf->port);
		return NOT_AUTHORIZED;
	    }
	    if (find_connection_kill_level(client_p, username) != 0)
		return BANNED_CLIENT;
	    return (attach_iline(client_p, aconf));
	}
    }
    return NOT_AUTHORIZED;
}

/*
 * attach_iline
 *
 * inputs       - client pointer
 *              - conf pointer
 * output       -
 * side effects -
 */
static int attach_iline(struct Client *client_p, struct ConfItem *aconf)
{
    int f = aconf->class->links;

    f++;

    /* only check it if its non zero */
    if (f > aconf->class->max_connections) {
	if (!IsConfExemptLimits(aconf))
	    return TOO_MANY;	/* Already at maximum allowed ip#'s */
	else {
	    send_me_notice(client_p, ":*** :I: line is full, but you have an >I: line!");
	}
    }
    if (!IsListenerClient(client_p->listener))
	return INVALID_CONNECTION;
    return (attach_conf(client_p, aconf));
}

/*
 * attach_cn_lines
 *
 * inputs       - pointer to server to attach c/ns to
 *              - name of server
 *              - hostname of server
 * output       - true (1) if both are found, otherwise return false (0)
 * side effects -
 * attach_cn_lines - find C/N lines and attach them to connecting client
 * NOTE: this requires an exact match between the name on the C:line and
 * the name on the N:line
 * C/N lines are completely gone now, the name exists only for historical
 * reasons - A1kmm.
 */
int attach_cn_lines(struct Client *client_p, char *name, char *host)
{
    struct ConfItem *ptr;
    int tmp = 0;

    assert(0 != client_p);
    assert(0 != host);

    for (ptr = GlobalConfItemList; ptr; ptr = ptr->next) {
	if (IsIllegal(ptr))
	    continue;
	if (!(ptr->status & CONF_SERVER))
	    continue;
	if (match(ptr->name, name) || match(ptr->host, host))
	    continue;
	attach_conf(client_p, ptr);
	tmp = 1;
    }
    return tmp;
}

/*
 * SplitUserHost
 *
 * inputs       - struct ConfItem pointer
 * output       - return 1/0 true false or -1 for error
 * side effects - splits user@host found in a name field of conf given
 *                stuff the user into ->user and the host into ->host
 */
static int SplitUserHost(struct ConfItem *aconf)
{
    char *p;
    char *new_user;
    char *new_host;

    if ((p = strchr(aconf->host, '@'))) {
	*p = '\0';
	DupString(new_user, aconf->host);
	MyFree(aconf->user);
	aconf->user = new_user;
	p++;
	DupString(new_host, p);
	MyFree(aconf->host);
	aconf->host = new_host;
    } else {
	DupString(aconf->user, "*");
    }
    return (1);
}

/*
 * lookup_confhost - start DNS lookups of all hostnames in the conf
 * line and convert an IP addresses in a.b.c.d number for to IP#s.
 *
 */
void lookup_confhost(struct ConfItem *aconf)
{
    if (BadPtr(aconf->host) || BadPtr(aconf->name)) {
	sendto_lev(DEBUG_LEV, "Host/server name error: (%s) (%s)", aconf->host, aconf->name);
	return;
    }

    if (strchr(aconf->host, '*') || strchr(aconf->host, '?'))
	return;
    /*
     * ** Do name lookup now on hostnames given and store the
     * ** ip numbers in conf structure.
     */
    if (inetpton(DEF_FAM, aconf->host, &IN_ADDR(aconf->ipnum)) <= 0) 
	conf_dns_lookup(aconf);
}

/*
 * flush_deleted_I_P
 *
 * inputs       - none
 * output       - none
 * side effects - This function removes I/P conf items
 */

void flush_deleted_I_P(void)
{
    struct ConfItem **tmp = &GlobalConfItemList;
    struct ConfItem *tmp2;

    /*
     * flush out deleted I and P lines although still in use.
     */
    for (tmp = &GlobalConfItemList; (tmp2 = *tmp);) {
	if (!(tmp2->status & CONF_ILLEGAL))
	    tmp = &tmp2->next;
	else {
	    *tmp = tmp2->next;
	    tmp2->next = NULL;
	    if (!tmp2->clients)
		free_conf(tmp2);
	}
    }
}

#define MAXCONFLINKS 150

/*
 * conf_add_conf
 * Inputs       - ConfItem
 * Output       - none
 * Side effects - add given conf to link list
 */

void conf_add_conf(struct ConfItem *aconf)
{
    (void) collapse(aconf->host);
    (void) collapse(aconf->user);
    logevent_call(LogSys.add_confline, 
	   aconf->status,
	   aconf->host ? aconf->host : "<NULL>",
	   aconf->passwd ? aconf->passwd : "<NULL>",
	   aconf->user ? aconf->user : "<NULL>", aconf->port);

    aconf->next = GlobalConfItemList;
    GlobalConfItemList = aconf;
}

/*
 * conf_add_class_to_conf
 * inputs       - pointer to config item
 * output       - NONE
 * side effects - Add a class pointer to a conf 
 */

void conf_add_class_to_conf(struct ConfItem *aconf)
{
    if (aconf->class == 0) {
	aconf->class = find_class(0);
	return;
    }

    if (aconf->class->max_connections < 0) {
	aconf->class = find_class(0);
	return;
    }
}

/*
 * conf_add_server
 *
 * inputs       - pointer to config item
 *              - pointer to link count already on this conf
 * output       - NONE
 * side effects - Add a connect block
 */
int conf_add_server(struct ConfItem *aconf, int lcount)
{
    conf_add_class_to_conf(aconf);

    if (lcount > MAXCONFLINKS || !aconf->host || !aconf->name) {
	logevent_call(LogSys.bad_connect, "No-NAME");
	return -1;
    }

    if (SplitUserHost(aconf) < 0) {
	logevent_call(LogSys.bad_connect, aconf->name);
	return -1;
    }
    lookup_confhost(aconf);
    return 0;
}

