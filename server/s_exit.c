/************************************************************************
 *   IRC - Internet Relay Chat, server/s_exit.c
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
 * $Id: s_exit.c,v 1.11 2004/02/24 19:03:33 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "zlink.h"
#include "dh.h"
#include "h.h"
#include "msg.h"
#include "listener.h"
#include "resnew.h"
#include "s_auth.h"
#include "s_conf.h"
#include "event.h"

static void exit_one_client(aClient *, aClient *, char *, int);

/*
 * NOQUIT
 * a method of reducing the stress on the network during server splits
 * by sending only a simple "SQUIT" message for the server that is dropping,
 * instead of thousands upon thousands of QUIT messages for each user,
 * plus an SQUIT for each server behind the dead link.
 *
 * Original idea by Cabal95, implementation by lucas
 */

/*
 * exit_recursion
 * 
 * recursive function!
 * therefore, we pass dead and reason to ourselves.
 * in the beginning, dead == cptr, so it will be the one
 * out of the loop last. therefore, dead should remain a good pointer.
 * dead: the actual server that split (if this belongs to us, we
 *       absolutely CANNOT send to it)
 * source_p: the client that caused this split
 * spinfo: split reason, as generated in exit_server
 * comment: comment provided
 */

static inline void exit_recursion(aClient *dead, aClient *source_p, char *spinfo)
{
    aClient *acptr;

    if (IsMe(dead))
	return;
    if (dead->serv == NULL)
	return;

    /*
     * okay, this is annoying.
     * first off, we need two loops.
     * one: to remove all the clients.
     * two: to remove all the servers.
     * HOWEVER! removing a server may cause removal of more servers and more clients.
     * and this may make our pointer to next bad. therefore, we have to restart
     * the server loop each time we find a server.
     * We _NEED_ two different loops: all clients must be removed before the server is
     * removed. Otherwise, bad things (tm) can happen.
     */

    logevent_call(LogSys.server_noquit, "BEGIN NOQUIT RECURSION", dead);

    while ((acptr = dead->serv->users)) {
        exit_one_client(acptr, dead, spinfo, 1);
        SetExited(acptr);
	remove_client_from_list(acptr);
    }
    while ((acptr = dead->serv->servers)) {
        exit_recursion(acptr, source_p, spinfo);
        exit_one_client(acptr, source_p, spinfo, 1);
        SetExited(acptr);
	remove_client_from_list(acptr);
    }

    logevent_call(LogSys.server_noquit, "END NOQUIT RECURSION", dead);
}

/*
 * * exit_client *    
 * 
 * This function exits a client of *any* type (user, server, etc)
 * from this server. Also, this generates all necessary prototol
 * messages that this exit may cause. 
 * 
 *   1) If the client is a local client, then this implicitly exits
 * all other clients depending on this connection (e.g. remote
 * clients having 'from'-field that points to this. 
 * 
 *   2) If the client is a remote client, then only this is exited.
 * 
 * For convenience, this function returns a suitable value for
 * m_function return value: 
 * 
 */

int exit_client(aClient *exit_ptr,	/* Client exiting */
		aClient *source_p,	/* Client generating this Exit, !NULL */
		char *comment	/* Reason for the exit */
    )
{
    dlink_node *m;
    aConfItem *aconf;
    // logevent_call(LogSys.exit_client, exit_ptr, source_p, comment);

    if (IsMe(exit_ptr))
	return 0;

    if (IsExited(exit_ptr) || IsClosing(exit_ptr) || IsHttpClient(exit_ptr))
	return CLIENT_EXITED;

    SetClosing(exit_ptr);

    if (exit_ptr->fd >= 0) {
	if (!IsDead(exit_ptr))
	    send_queued(exit_ptr->fd, exit_ptr);
	sendto_one_server(exit_ptr, NULL, TOK1_ERROR,
			  ":Closing Link: %s (%s)",
			  IsPerson(exit_ptr) ? exit_ptr->sockhost : "0.0.0.0", comment);
    }

    if (IsLocalClient(exit_ptr)) {
    	if (IsListenerHttp(exit_ptr->listener))
	    return CLIENT_EXITED;
	delete_adns_queries(exit_ptr->dns_query);
	delete_identd_queries(exit_ptr);
        m = dlinkFind(&unknown_list, exit_ptr);
        if (m != NULL) {
            Count.unknown--;
            dlinkDelete(m, &unknown_list);
            free_dlink_node(m);
        }

	if (IsClient(exit_ptr)) {
	    Count.local--;
	    if (IsAnOper(exit_ptr)) {
		m = dlinkFind(&locoper_list, exit_ptr);
		if (m != NULL) {
		    dlinkDelete(m, &locoper_list);
		    free_dlink_node(m);
		}
	    }
	    if (IsPerson(exit_ptr)) {	/* a little extra paranoia */
		m = dlinkFind(&lclient_list, exit_ptr);
		if (m != NULL) {
		    dlinkDelete(m, &lclient_list);
		    free_dlink_node(m);
		}
	    }
	    ircstp->is_cl++;
	    ircstp->is_cbs += exit_ptr->sendB;
	    ircstp->is_cbr += exit_ptr->receiveB;
	    ircstp->is_cks += exit_ptr->sendK;
	    ircstp->is_ckr += exit_ptr->receiveK;
	    ircstp->is_cti += timeofday - exit_ptr->firsttime;
	    if (ircstp->is_cbs > 2047) {
		ircstp->is_cks += (ircstp->is_cbs >> 10);
		ircstp->is_cbs &= 0x3ff;
	    }
	    if (ircstp->is_cbr > 2047) {
		ircstp->is_ckr += (ircstp->is_cbr >> 10);
		ircstp->is_cbr &= 0x3ff;
	    }
	}

	if (IsNegoServer(exit_ptr))
	    sendto_lev(SNOTICE_LEV, "Lost server %s during negotiation: %s",
		       exit_ptr->name, comment);
	if (IsServer(exit_ptr)) {
	    Count.myserver--;
	    if (IsULine(exit_ptr))
		Count.myulined--;
	    m = dlinkFind(&serv_list, exit_ptr);
	    if (m != NULL) {
		dlinkDelete(m, &serv_list);
		free_dlink_node(m);
	    }
	    sendto_gnotice("%C was connected for %lu seconds.  %lu/%lu sendK/recvK.",
			   exit_ptr, timeofday - exit_ptr->firsttime, exit_ptr->sendK,
			   exit_ptr->receiveK);
	    ircstp->is_sv++;
	    ircstp->is_sbs += exit_ptr->sendB;
	    ircstp->is_sbr += exit_ptr->receiveB;
	    ircstp->is_sks += exit_ptr->sendK;
	    ircstp->is_skr += exit_ptr->receiveK;
	    ircstp->is_sti += timeofday - exit_ptr->firsttime;
	    if (ircstp->is_sbs > 2047) {
		ircstp->is_sks += (ircstp->is_sbs >> 10);
		ircstp->is_sbs &= 0x3ff;
	    }
	    if (ircstp->is_sbr > 2047) {
		ircstp->is_skr += (ircstp->is_sbr >> 10);
		ircstp->is_sbr &= 0x3ff;
	    }
	    /*
	     * If the connection has been up for a long amount of time, schedule
	     * a 'quick' reconnect, else reset the next-connect cycle.
	     */
	    if ((aconf = find_conf_exact(exit_ptr->name, exit_ptr->username,
					 exit_ptr->sockhost, CONF_SERVER))) {
		/*
		 * Reschedule a faster reconnect, if this was a automatically
		 * connected configuration entry. (Note that if we have had
		 * a rehash in between, the status has been changed to
		 * CONF_ILLEGAL). But only do this if it was a "good" link.
		 */
		aconf->hold = time(NULL);
		aconf->hold += (aconf->hold - exit_ptr->since > HANGONGOODLINK) ?
		    HANGONRETRYDELAY : aconf->class->connect_frequency;
	    }
            if (GeneralOpts.split == 0)
                eventAddIsh("check_splitmode", check_splitmode, NULL, 1800);
            GeneralOpts.split = 1; 
	
	}
	if (!IsClient(exit_ptr) && !IsServer(exit_ptr))
	    ircstp->is_ni++;

	if (IsPerson(exit_ptr)) {
	    hash_del_watch_list(exit_ptr);
	    sendto_lev(CCONN_LEV,
		       "Client exiting: %^C [%s] [%s]",
		       exit_ptr,
		       (exit_ptr->flags & FLAGS_NORMALEX) ? "Client Quit" : comment,
		       exit_ptr->hostip);
	}

	if (IsService(exit_ptr)) {
	    sendto_lev(SERVICE_LEV, "Exiting Service %s (%s@%s)",
		       exit_ptr->name, exit_ptr->username, exit_ptr->sockhost);
	    Count.myservice--;
	}
	det_confs_butmask(exit_ptr, 0);
	exit_one_client(exit_ptr, source_p, comment, 0);
	if (exit_ptr->fd >= 0) {
#ifdef HAVE_ENCRYPTION_ON
	    /* Close the ssl connection also */
	    if (IsSSL(exit_ptr)) {
		SSL_set_shutdown(exit_ptr->ssl, SSL_RECEIVED_SHUTDOWN);
		SSL_smart_shutdown(exit_ptr->ssl);
		SSL_free(exit_ptr->ssl);
		exit_ptr->ssl = NULL;
	    }
#endif
	    fd_close(exit_ptr->fd);
	    exit_ptr->fd = -1;
	    exit_ptr->flags |= FLAGS_DEADSOCKET;
	}
	linebuf_donebuf(&exit_ptr->sendQ);
	linebuf_donebuf(&exit_ptr->recvQ);
	memset(exit_ptr->passwd, 0, sizeof(exit_ptr->passwd));
	/* clean up extra sockets from P-lines which have been discarded. */
	if (exit_ptr->listener) {
	    assert(0 < exit_ptr->listener->ref_count);
	    if (0 == --exit_ptr->listener->ref_count && !exit_ptr->listener->active)
		free_listener(exit_ptr->listener);
	    exit_ptr->listener = 0;
	}
	m = make_dlink_node();
	dlinkAdd(exit_ptr, m, &exit_list);
    } else if (!IsLocalClient(exit_ptr)) {
	// exit_ptr->from = NULL;	/* ...this should catch them! >:) --msa */
	exit_one_client(exit_ptr, source_p, comment, 0);
	remove_client_from_list(exit_ptr);
    }
    SetExited(exit_ptr);
    return CLIENT_EXITED;
}

/*
 * * Exit one client, local or remote. Assuming all dependants have *
 * been already removed, and socket closed for local client.
 */
static void exit_one_client(aClient *exit_ptr, aClient *source_p, char *comment, int split)
{
    dlink_node *lp, *ptr;
    dlink_node *next_lp;
    aClient *acptr;
    if (IsMe(exit_ptr))
	return;
    if (IsClient(exit_ptr)) 
        Count.total--;
    if (IsAnOper(exit_ptr))
        Count.oper--;
    if (IsInvisible(exit_ptr))
        Count.invisi--;
    if (IsServer(exit_ptr)) {

    char splitname[HOSTLEN + HOSTLEN + 2];
	if (ServerHide.enable) {
	    if (ServerHide.flatten_links)
		ircsprintf(splitname, "%C %*C", &me, exit_ptr);
	    else
		ircsprintf(splitname, "a.server.on.%s %C", ServerInfo.networkname, &me);
	} else {
	    ircsprintf(splitname, "%*C %*C", exit_ptr->servptr, exit_ptr);
	}
	logevent_call(LogSys.exit_server, exit_ptr, source_p, comment);
	exit_recursion(exit_ptr, source_p, splitname);
	for (ptr = serv_list.head; ptr; ptr = next_lp) {
	    next_lp = ptr->next;
	    acptr = ptr->data;
	    if (!acptr || IsMe(acptr) || acptr == exit_ptr || acptr == source_p)
		continue;
	    if (exit_ptr->from == acptr)	/* "upstream" squit */
		sendto_one_server(acptr, source_p, TOK1_SQUIT, "%~C :%s", exit_ptr, comment);
	    else
		sendto_one_server(acptr, NULL, TOK1_SQUIT, "%~C :%s", exit_ptr, comment);
	}

	if (HasID(exit_ptr))
	    rem_base64_server(exit_ptr);
	remove_server_from_list(exit_ptr);
	if (exit_ptr->servptr && exit_ptr->servptr->serv) {
	    del_client_from_llist(&(exit_ptr->servptr->serv->servers), exit_ptr);
	    exit_ptr->servptr->serv->servercnt--;
	}
	Count.server--;
	if (exit_ptr->serv) {
#ifdef HAVE_ENCRYPTION_ON
	    if (exit_ptr->serv->sin)
		dh_end_session(exit_ptr->serv->sin);
	    if (exit_ptr->serv->sout)
		dh_end_session(exit_ptr->serv->sout);
	    if (exit_ptr->serv->rc4_in)
		rc4_destroystate(exit_ptr->serv->rc4_in);
	    if (exit_ptr->serv->rc4_out)
		rc4_destroystate(exit_ptr->serv->rc4_out);
#endif
	    if (exit_ptr->serv->zin)
		destroy_unzipstream(exit_ptr->serv->zin);
	    if (exit_ptr->serv->zout)
		destroy_zipstream(exit_ptr->serv->zout);
	    exit_ptr->serv->nline = NULL;
	    MyFree((char *) exit_ptr->serv);
	}
    }

    if (IsPerson(exit_ptr)) {
	/*
	 * * If a person is on a channel, send a QUIT notice * to every
	 * client (person) on the same channel (so * that the client can
	 * show the "**signoff" message). * (Note: The notice is to the
	 * local clients *only*)
	 */
	/*
	 * If this exit is generated from "m_kill", then there is no
	 * sense in sending the QUIT--KILL's have been sent instead.
	 */
	if (!split) {
	    if ((exit_ptr->flags & FLAGS_KILLED) == 0) {
		sendto_serv_butone(exit_ptr->from, exit_ptr, TOK1_QUIT, ":%s", comment);
	    }
	}
	send_quit_to_common_channels(exit_ptr, comment);
	if (MyClient(exit_ptr))
	    free_localuser_identity(exit_ptr);
	sendto_service(SERVICE_SEE_QUITS, 0, exit_ptr, NULL, TOK1_QUIT, ":%s", comment);
	for (lp = exit_ptr->user->channel.head; lp; lp = next_lp) {
	    next_lp = lp->next;
	    remove_user_from_channel(exit_ptr, lp->data);
	}
	for (lp = exit_ptr->user->invited.head; lp; lp = next_lp) {
	    next_lp = lp->next;
	    del_invite(exit_ptr, lp->data);
	}
	for (lp = exit_ptr->user->silence.head; lp; lp = next_lp) {
	    next_lp = lp->next;
	    del_silence(exit_ptr, lp->data);
	}
	remove_dcc_references(exit_ptr);
	remove_accept_references(exit_ptr);
	if (HasID(exit_ptr))
	    rem_userid_from_server(exit_ptr->servptr, exit_ptr);
	if (IsRegisteredUser(exit_ptr))
	    hash_check_watch(exit_ptr, RPL_LOGOFF);
	if (exit_ptr->servptr && exit_ptr->servptr->serv) {
	    del_client_from_llist(&(exit_ptr->servptr->serv->users), exit_ptr);
	    exit_ptr->servptr->serv->usercnt--;
	}
	add_history(exit_ptr, 0);
	off_history(exit_ptr);
	free_user(exit_ptr->user);
	exit_ptr->user = NULL;
    }
    if (IsService(exit_ptr)) {
	if (exit_ptr->service) {
	    sendto_lev(SERVICE_LEV,
		       "Received QUIT from service %s (%s@%s)",
		       exit_ptr->name, exit_ptr->username, exit_ptr->sockhost);
	    if (exit_ptr->from == exit_ptr)
		sendto_lev(SERVICE_LEV, "Service %s disconnected", exit_ptr->name);
	}
	Count.service--;
	if (exit_ptr->service)
	    free_service(exit_ptr);
    }

    del_from_client_hash_table(exit_ptr->name, exit_ptr);

    return;
}

void remove_exited_clients(void *notused)
{
    dlink_node *ptr, *prev_ptr;
    aClient *cptr;
    for (ptr = exit_list.tail; ptr; ptr = prev_ptr) {
	prev_ptr = ptr->prev;
	cptr = ptr->data;
	if (!cptr)
	    continue;
	if (IsExited(cptr)) {
	    remove_client_from_list(cptr);
	    dlinkDeleteNode(ptr, &exit_list);
	}
    }
}
