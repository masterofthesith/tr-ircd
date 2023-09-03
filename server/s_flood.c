/************************************************************************
 *   IRC - Internet Relay Chat, server/s_flood.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers.
 *
 *   This program is free softwmare; you can redistribute it and/or modify
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
#include "s_bsd.h"
#include "parse.h"
#include "fd.h"
#include "packet.h"
#include "zlink.h"
#include "dh.h"
#include "s_conf.h"

/*
 * flood_recalc
 *
 * recalculate the number of allowed flood lines. this should be called
 * once a second on any given client. We then attempt to flush some data.
 */

void flood_recalc(int fd, void *data)
{
    aClient *client_p = data;
    int max_flood_per_sec = MAX_FLOOD_PER_SEC;

    /* This can happen in the event that the client detached. */
    if (!MyClient(client_p))
	return;
    /* If we're a server, skip to the end. Realising here that this call is
     * cheap and it means that if a op is downgraded they still get considered
     * for anti-flood protection ..
     */
    if (!IsPrivileged(client_p)) {

	/* Is the grace period still active? */
	if (client_p->user && !IsFloodDone(client_p))
	    max_flood_per_sec = MAX_FLOOD_PER_SEC_I;

	/* ok, we have to recalculate the number of messages we can receive
	 * in this second, based upon what happened in the last second.
	 * If we still exceed the flood limit, don't move the parsed limit.
	 * If we are below the flood limit, increase the flood limit.
	 *   -- adrian
	 */

	/* Set to 1 to start with, let it rise/fall after that... */
	if (client_p->allow_read == 0)
	    client_p->allow_read = 1;
	else if (client_p->actually_read < client_p->allow_read)
	    /* Raise the allowed messages if we flooded under the limit */
	    client_p->allow_read++;
	else
	    /* Drop the limit to avoid flooding .. */
	    client_p->allow_read--;
	/* Enforce floor/ceiling restrictions */
	if (client_p->allow_read < 1)
	    client_p->allow_read = 1;
	else if (client_p->allow_read > max_flood_per_sec)
	    client_p->allow_read = max_flood_per_sec;
	/* Reset the sent-per-second count */
	client_p->sent_parsed = 0;
	client_p->actually_read = 0;
	parse_client_queued(client_p);
	if (!IsDead(client_p)) {
	    /* and finally, reset the flood check */
	    comm_setflush(fd, 1000, flood_recalc, client_p);
	}
    }
}

/* flood_endgrace()
 *
 * marks the end of the clients grace period
 */
void flood_endgrace(struct Client *client_p)
{
    client_p->protoflags |= PFLAGS_FLOODDONE;
    /* Drop their flood limit back down */
    client_p->allow_read = MAX_FLOOD_PER_SEC;

    /* sent_parsed could be way over MAX_FLOOD but under MAX_FLOOD_BURST,
     * so reset it.
     */
    client_p->sent_parsed = 0;
}
