/***********************************************************************
 *   IRC - Internet Relay Chat, src/flush.c
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

/*
 * $Id: flush.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

#include "struct.h"
#include "channel.h"
#include "common.h"
#include "sys.h"
#include "s_conf.h"
#include "msg.h"
#include "h.h"
#include "numeric.h"

/*******************************************
 * Flushing functions (empty queues)
 *******************************************/

/*
 * flush_connections
 * Unintelligently try to empty all buffers.
 */
void flush_connections(void *unused)
{
    aClient *cptr;
    dlink_node *ptr;

    for (ptr = serv_list.head; ptr; ptr = ptr->next) {
	cptr = ptr->data;
	if (cptr && (cptr->flags & (FLAGS_RC4IN|FLAGS_RC4OUT|FLAGS_ZIPPED_IN|FLAGS_ZIPPED_OUT)))
	    send_queued(cptr->fd, cptr);
    }
}

/*
 * dianora's code in the new checkpings is slightly wasteful.
 * * however, upon inspection (thanks seddy), when we close a connection,
 * * the ordering of local[i] is NOT reordered; simply local[highest_fd] becomes
 * * local[i], so we can just i--;  - lucas
 */

static void check_pings_list(dlink_list * list)
{
    aClient *cptr;
    char *ktype;
    int retval = 0, ping = 0;
    char *errtxt = ":No response from %s, closing link";
    dlink_node *ptr, *next_ptr;

    for (ptr = list->head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	cptr = ptr->data;

	if (IsDead(cptr) && !IsExited(cptr)) {
	    exit_client(cptr, &me, (cptr->flags & FLAGS_SENDQEX) ?
			"SendQ Exceeded" : "Dead Socket");
	    continue;
	}

	if (rehashed) {
	    if (IsPerson(cptr)) 
		retval = find_kill_level(cptr);
	}

	if (retval) {
            ktype = ((retval == MASKITEM_ZAPLINE) ? "Zap Line" :
                     (retval == MASKITEM_KLINE) ? "Kill Line" : 
		     (retval == MASKITEM_KLINE_REGEX) ? "Kill Line" : "Autokill");
	    sendto_lev(KLINE_LEV, "%s active for %C", ktype, cptr);
            ktype = ((retval == MASKITEM_ZAPLINE) ? "Zapped" :
                     (retval == MASKITEM_KLINE) ? "K-lined" : 
		     (retval == MASKITEM_KLINE_REGEX) ? "K-Lined" : "Autokilled");
            send_me_numeric(cptr, ERR_YOUREBANNEDCREEP, ktype);
	    exit_client(cptr, &me, ktype);
	    continue;
	}

	if (IsRegistered(cptr))
	    ping = cptr->pingval;
	else
	    ping = CONNECTTIMEOUT;
	/*
	 * If the client pingtime is fine (ie, not larger than the client ping) 
	 * skip over all the checks below. - lucas
	 */

	if (ping < (timeofday - cptr->lasttime)) {
	    /*
	     * If the server hasnt talked to us in 2*ping seconds and it has
	     * a ping time, then close its connection. If the client is a
	     * user and a KILL line was found to be active, close this
	     * connection too.
	     */
	    if (cptr->flags & FLAGS_PINGSENT) {
		if ((timeofday - cptr->lasttime) >= (2 * ping)) {
		    if (IsServer(cptr) || IsConnecting(cptr) || IsHandshake(cptr)) {
			char fbuf[512];
			ircsprintf(fbuf, "from %C: %s", &me, errtxt);
			sendto_gnotice(fbuf, get_client_name(cptr, HIDEME));
			sendto_serv_butone(cptr, &me, TOK1_GNOTICE, errtxt,
					   get_client_name(cptr, HIDEME));
		    }
		    exit_client(cptr, &me, "Ping timeout");
		}
		continue;
	    }

	    /*
	     * don't send pings during a burst, as we send them already. 
	     */
	    else if (!(cptr->flags & FLAGS_PINGSENT) || !(cptr->flags & FLAGS_BURST)) {
		/*
		 * if we havent PINGed the connection and we havent heard from
		 * it in a while, PING it to make sure it is still alive.
		 */
		cptr->flags |= FLAGS_PINGSENT;
		cptr->lasttime = timeofday - ping;
		sendto_one_person(cptr, NULL, TOK1_PING, ":%C", &me);
#if 0
		comm_setselect(cptr->fd, FDLIST_IRCD, COMM_SELECT_WRITE,
			       send_queued, cptr, 0);
#endif
	    }
	}

    }

    rehashed = 0;

    return;
}

    /*
     * check_unknowns_list
     *
     * inputs       - pointer to list of unknown clients
     * output       - NONE
     * side effects - unknown clients get marked for termination after n seconds
     */
static void check_unknowns_list(dlink_list * list)
{
dlink_node *ptr, *next_ptr;
struct Client *client_p;

    for (ptr = list->head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	client_p = ptr->data;

	/*
	 * Check UNKNOWN connections - if they have been in this state
	 * for > 30s, close them.
	 */

	if (client_p->firsttime ? ((timeofday - client_p->firsttime) > 30) : 0) 
		exit_client(client_p, &me, "Connection timed out");
    }
}

void check_pings(void *notused)
{
    check_pings_list(&lclient_list);
    check_pings_list(&serv_list);
    check_unknowns_list(&unknown_list);
}
