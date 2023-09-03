/************************************************************************
 *   IRC - Internet Relay Chat, server/s_servauth.c
 *   Copyright (C) 1992 Darren Reed
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
 *   $Id: s_servauth.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
 *
 * Changes:
 *   July 6, 1999 - Rewrote most of the code here. When a client connects
 *     to the server and passes initial socket validation checks, it
 *     is owned by this module (auth) which returns it to the rest of the
 *     server when dns and auth queries are finished. Until the client is
 *     released, the server does not know it exists and does not process
 *     any messages from it.
 *     --Bleep  Thomas Helvey <tomh@inxpress.net>
 */

#include "struct.h"
#include "common.h"
#include "s_auth.h"
#include "event.h"
#include "fd.h"			/* fdlist_add */
#include "sys.h"
#include "h.h"
#include "numeric.h"
#include "packet.h"
#include "resnew.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "config.h"
#include "dh.h"

struct {
    const char *message;
    size_t length;
} ServerHeaderMessages[] = {
    /* 123456789012345678901234567890123456789012345678901234567890 */
    {"NOTICE AUTH :*** Looking up your hostname...\r\n", 46}, 
    {"NOTICE AUTH :*** Found your hostname\r\n", 38}, 
    {"NOTICE AUTH :*** Found your hostname, cached\r\n", 46}, 
    {"NOTICE AUTH :*** Couldn't look up your hostname\r\n", 49}, 
    {"NOTICE AUTH :*** Your forward and reverse DNS do not match, ignoring hostname.\r\n", 80}, 
    {"NOTICE AUTH :*** Your hostname is too long, ignoring hostname\r\n", 63},
};

typedef enum {
    REPORT_DO_DNS,
    REPORT_FIN_DNS,
    REPORT_FIN_DNSC,
    REPORT_FAIL_DNS,
    REPORT_IP_MISMATCH,
    REPORT_HOST_TOOLONG,
} ReportType;

#ifdef HAVE_ENCRYPTION_ON
#define sendheader(c, r) \
    if (IsSSL(c)) \
	safe_SSL_write(c, ServerHeaderMessages[(r)].message, ServerHeaderMessages[(r)].length); \
    else \
   	send((c)->fd, ServerHeaderMessages[(r)].message, ServerHeaderMessages[(r)].length, 0)
#define sendstring(c, t, l, n) \
    if (IsSSL(c)) \
	safe_SSL_write(c, t, l); \
    else \
	send((c)->fd, t, l, n);
#else
#define sendheader(c, r) send((c)->fd, ServerHeaderMessages[(r)].message, ServerHeaderMessages[(r)].length, 0)
#define sendstring(c, t, l, n) send((c)->fd, t, l, n)
#endif

/*
 * lookup_dns_callback - called when resolver query finishes
 * if the query resulted in a successful search, hp will contain
 * a non-null pointer, otherwise hp will be null.
 * set the client on it's way to a connection completion, regardless
 * of success of failure
 */
static void lookup_dns_callback(void *vptr, adns_answer * reply)
{
    struct Client *client_p = (struct Client *) vptr;
    char *str = client_p->sockhost;

    if (reply && (reply->status == adns_s_ok)) {

	if (strlen(*reply->rrs.str) < HOSTLEN) {
	    strcpy(str, *reply->rrs.str);
	    sendheader(client_p, REPORT_FIN_DNS);
	} else {
#ifdef IPV6
	    if (client_p->aftype == AF_INET6) {
		if (*client_p->hostip == ':') {
		    strcpy(str, "0");
		    strcat(str, client_p->hostip);
		} else {
		    strcpy(str, client_p->hostip);
		}
		strcat(str, ".");
	    } else
#endif
		strcpy(str, client_p->hostip);
	    sendheader(client_p, REPORT_HOST_TOOLONG);
	    client_p->protoflags |= PFLAGS_NONRESOLVED;
	}
    } else {
#ifdef IPV6
	if (client_p->aftype == AF_INET6) {
	    if (*client_p->hostip == ':') {
		strcpy(str, "0");
		strcat(str, client_p->hostip);
	    } else {
		strcpy(str, client_p->hostip);
	    }
	    strcat(str, ".");
	} else
#endif
	    strcpy(str, client_p->hostip);
	sendheader(client_p, REPORT_FAIL_DNS);
	client_p->protoflags |= PFLAGS_NONRESOLVED;
	/* Used for the channelmode +N preventing 
	 * non resolved addresses from joining */
    }

    MyFree(reply);
    MyFree(client_p->dns_query);

    client_p->dns_query = NULL;

    client_p->allow_read = MAX_FLOOD_PER_SEC;
    comm_setflush(client_p->fd, 1000, flood_recalc, client_p);
    add_client_to_list(client_p);
    read_client_packet(client_p->fd, client_p);
}

/*
 * start_auth - starts auth (identd) and dns queries for a client
 */
void start_lookup(struct Client *client)
{
    assert(0 != client);
    if (client == NULL)
	return;

    client->dns_query = MyMalloc(sizeof(struct DNSQuery));
    client->dns_query->ptr = client;
    client->dns_query->callback = lookup_dns_callback;

    sendheader(client, REPORT_DO_DNS);

    /* No DNS cache now, remember? -- adrian */
    adns_getaddr(&(client->ip), client->aftype, client->dns_query);
}

