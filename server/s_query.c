/************************************************************************
 *   IRC - Internet Relay Chat, server/s_query.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers.
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
 * $Id: s_query.c,v 1.4 2007/12/29 17:50:49 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "h.h"
#include "zlink.h"
#include "s_conf.h"

extern struct Token tok1_msgtab[];

/* 
 * * hunt_server *
 * 
 *      Do the basic thing in delivering the message (command) *
 * across the relays to the specific server (server) for *
 * actions. *
 * 
 *      Note:   The command is a format string and *MUST* be *  
 * f prefixed style (e.g. ":%s COMMAND %s ..."). *              Command
 * can have only max 8 parameters. *
 * 
 *      server  parv[server] is the parameter identifying the *
 * arget server. *
 *
 *      *WARNING* *             parv[server] is replaced with the
 * pointer to the *             real servername from the matched client
 * (I'm lazy *          now --msa). *
 *
 */

int hunt_server(aClient *cptr, aClient *sptr, char *command, char *token,
		int server, int parc, char *parv[])
{
    aClient *acptr;
    int wilds;
    if (parc <= server || BadPtr(parv[server]) ||
	match(me.name, parv[server]) == 0 || match(parv[server], me.name) == 0)
	return (HUNTED_ISME);
    /*
     * * These are to pickup matches that would cause the following *
     * message to go in the wrong direction while doing quick fast * 
     * non-matching lookups.
     */

    if ((acptr = find_client(parv[server])))
	if (acptr->from == sptr->from && !MyConnect(acptr))
	    acptr = NULL;
    if (!acptr && (acptr = find_server(parv[server])))
	if (acptr->from == sptr->from && !MyConnect(acptr))
	    acptr = NULL;

    (void) collapse(parv[server]);
    wilds = (strchr(parv[server], '?') || strchr(parv[server], '*'));
    /*
     * Again, if there are no wild cards involved in the server name,
     * use the hash lookup - Dianora
     */

    if (!acptr) {
	if (!wilds) {
	    acptr = find_server(parv[server]);
	    if (!acptr || !IsRegistered(acptr) || !IsServer(acptr)) {
		send_me_numeric(sptr, ERR_NOSUCHSERVER, parv[server]);
		return (HUNTED_NOSUCH);
	    }
	} else {
	    for (acptr = GlobalClientList;
		 (acptr = next_client(acptr, parv[server])); acptr = acptr->next) {
		if (acptr->from == sptr->from && !MyConnect(acptr))
		    continue;
		/*
		 * Fix to prevent looping in case the parameter for some
		 * reason happens to match someone from the from link --jto
		 */
		if (IsRegistered(acptr) && (acptr != cptr))
		    break;
	    }
	}
    }

    if (acptr) {
	if (IsMe(acptr) || MyClient(acptr))
	    return HUNTED_ISME;
	if (match(acptr->name, parv[server]))
	    parv[server] = acptr->name;

	/* Please note that the following line REQUIRES the usage of
	 * hunt_server with token elements.
	 * -TimeMr14C
	 */

	sendto_one(acptr, command, parv[0],
		   (IsToken1(acptr->from) ? token :
		    tok1_msgtab[(u_char) *token].cmd), parv[1], parv[2],
		   parv[3], parv[4], parv[5], parv[6], parv[7], parv[8]);
	return (HUNTED_PASS);
    }
    send_me_numeric(sptr, ERR_NOSUCHSERVER, parv[server]);
    return (HUNTED_NOSUCH);
}

#define LUSERS_CACHE_TIME 180

    /*
     * send_lusers
     *     parv[0] = sender
     *     parv[1] = host/server mask.
     *     parv[2] = server to query
     */
int send_lusers(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    send_me_numeric(sptr, RPL_LUSERCLIENT, Count.total - Count.invisi, Count.invisi, Count.server);
    if (Count.service && Count.myservice)
	sendto_one(sptr,
		   ":%C %N %s :There are %d services online, I have %d services.",
		   &me, RPL_SERVICE, sptr->name, Count.service, Count.myservice);
    else if (Count.service)
	sendto_one(sptr, ":%C %N %s :There are %d services online.",
		   &me, RPL_SERVICE, sptr->name, Count.service);
    if (Count.oper)
	send_me_numeric(sptr, RPL_LUSEROP, Count.oper);
    if (Count.unknown)
	send_me_numeric(sptr, RPL_LUSERUNKNOWN, Count.unknown);
    if (Count.chan)
	send_me_numeric(sptr, RPL_LUSERCHANNELS, Count.chan);
    send_me_numeric(sptr, RPL_LUSERME, Count.local,
		    (ServicesConf.hide_ulined_servers && IsSeeHidden(sptr)) ?
		    Count.myserver : Count.myserver - Count.myulined);
    send_me_numeric(sptr, RPL_LOCALUSERS, Count.local, Count.max_loc);
    send_me_numeric(sptr, RPL_GLOBALUSERS, Count.total, Count.max_tot);
    return 0;
}

