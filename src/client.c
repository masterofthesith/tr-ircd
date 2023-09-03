/************************************************************************
 *   IRC - Internet Relay Chat, src/client.c
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
 * $Id: client.c,v 1.6 2003/08/10 17:02:16 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "numeric.h"
#include "blalloc.h"
#include "language.h"
#include "dh.h"
#include "s_conf.h"
#include "zlink.h"
#include "tools.h"

BlockHeap *free_local_aClients;
BlockHeap *free_remote_aClients;

void initlists()
{
    free_remote_aClients = BlockHeapCreate((size_t) CLIENT_REMOTE_SIZE, CLIENTS_PREALLOCATE);
    free_local_aClients = BlockHeapCreate((size_t) CLIENT_LOCAL_SIZE, MAXCONNECTIONS);
}

/*
 * * Create a new aClient structure and set it to initial state. *
 * 
 *      from == NULL,   create local client (a client connected *
 * o a socket). *
 * 
 *      from,   create remote client (behind a socket *
 * ssociated with the client defined by *
 *  ('from' is a local client!!).
 *
 */

aClient *make_client(aClient *from)
{
    aClient *cptr = NULL;
    dlink_node *m;

    if (!from) {
    cptr = (aClient *) BlockHeapAlloc(free_local_aClients);

	if (cptr == (aClient *) NULL)
	    outofmemory("Make client");

	memset((char *) cptr, '\0', CLIENT_LOCAL_SIZE);

	cptr->from = cptr;	/* 
				 * 'from' of local client is self! 
				 */
	m = make_dlink_node();

	dlinkAdd(cptr, m, &unknown_list);
	Count.unknown++;
	strlcpy_irc(cptr->username, "unknown", USERLEN);
	cptr->since = cptr->lasttime = cptr->firsttime = timeofday;
	cptr->authfd = -1;
	cptr->waitlen = 0;
	cptr->restbuf = NULL;
	cptr->sendqlen = MAXSENDQLENGTH;
	cptr->protoflags = PFLAGS_LOCALCLIENT;
#ifdef HAVE_ENCRYPTION_ON
	cptr->ssl = NULL;
#endif
	cptr->dns_query = NULL;
	cptr->listener = NULL;
	cptr->lopts = NULL;
    } else {
    cptr = (aClient *) BlockHeapAlloc(free_remote_aClients);

	if (cptr == (aClient *) NULL)
	    outofmemory("Make remote client");

	memset((char *) cptr, '\0', CLIENT_REMOTE_SIZE);

	cptr->from = from;
	cptr->protoflags = 0;
    }
    cptr->fd = -1;
    cptr->lang = 0;
    cptr->status = STAT_UNKNOWN;
    cptr->idhnext = NULL;
    cptr->id.id = 0;
    cptr->id.string[0] = '\0';
    cptr->next = NULL;
    return cptr;
}

void free_client(aClient *cptr)
{
    if (2 < cptr->fd)
	fd_close(cptr->fd);
    if (HasRestbuf(cptr))
	MyFree(cptr->restbuf);
    if (MyClient(cptr) || IsLocalClient(cptr)) {
	MyFree(cptr->lopts);
	BlockHeapFree(free_local_aClients, cptr);
    } else { 
	BlockHeapFree(free_remote_aClients, cptr);
    }
}

/*
 * * next_client *    Local function to find the next matching
 * client. The search * can be continued from the specified client
 * entry. Normal *      usage loop is: *
 *
 *      for (x = client; x = next_client(x,mask); x = x->next) *
 * andleMatchingClient; *
 *
 */
aClient *next_client(aClient *next, char *ch)
{
    aClient *tmp = next;

    next = find_client(ch);

    if (next == NULL)
    	next = tmp;

    if (tmp && tmp->prev == next)
	return ((aClient *) NULL);

    if (next != tmp)
	return next;
    for (; next; next = next->next) {
	if (!match(ch, next->name))
	    break;
    }
    return next;
}

/*
 * this slow version needs to be used for hostmasks *sigh *
 */

aClient *next_client_double(aClient *next, char *ch)
{
    aClient *tmp = next;

    next = find_client(ch);
    if (tmp && tmp->prev == next)
	return NULL;
    if (next != tmp)
	return next;
    for (; next; next = next->next) {
	if (!match(ch, next->name) || !match(next->name, ch))
	    break;
    }
    return next;
}

aServer *make_server(aClient *cptr)
{
    aServer *serv = cptr->serv;

    if (!serv) {
	serv = (aServer *) MyMalloc(sizeof(aServer));

	memset(serv, 0, sizeof(aServer));

	serv->users = NULL;
	serv->servers = NULL;
	*serv->bynick = '\0';
	cptr->serv = serv;
    }
    return cptr->serv;
}

/*
 * * 'make_user' add's an User information block to a client * if it
 * was not previously allocated.
 */
anUser *make_user(aClient *cptr)
{
    anUser *user;

    user = cptr->user;
    if (!user) {
    user = (anUser *) MyMalloc(sizeof(anUser));

	memset(user, 0, sizeof(anUser));
	cptr->user = user;
    }
    return user;
}

/*
 * * free_user *      Decrease user reference count by one and
 * release block, *     if count reaches 0
 */
void free_user(anUser *user)
{
    MyFree(user->away);
    MyFree(user->real_oper_host);
    MyFree(user->real_oper_ip);
    MyFree(user->server);
    MyFree(user);
}

/*
 * taken the code from ExitOneClient() for this and placed it here. -
 * avalon
 */
void remove_client_from_list(aClient *cptr)
{
    if (cptr == NULL)
	return;

    if (cptr->prev || cptr->next) {
    
    	if (cptr->prev) {
	    cptr->prev->next = cptr->next;
    	} else if (cptr->next) {
	    GlobalClientList = cptr->next;
	    GlobalClientList->prev = NULL;
	}
	if (cptr->next)
	    cptr->next->prev = cptr->prev;
    }
    cptr->prev = cptr->next = NULL;

    free_client(cptr);
    return;
}

/*
 * although only a small routine, it appears in a number of places as a
 * collection of a few lines...functions like this *should* be in this
 * file, shouldnt they ?  after all, this is list.c, isnt it ? -avalon
 */
void add_client_to_list(aClient *cptr)
{
    /*
     * since we always insert new clients to the top of the list, this
     * should mean the "me" is the bottom most item in the list.
     */
    cptr->next = GlobalClientList;
    GlobalClientList = cptr;
    if (cptr->next)
	cptr->next->prev = cptr;
    cptr->prev = NULL;
    return;
}

/*
 * find_chasing
 *   Find the client structure for a nick name (user) using history
 *   mechanism if necessary. If the client is not found, an error message
 *   (NO SUCH NICK) is generated. If the client was found through the
 *   history, chasing will be 1 and otherwise 0.
 */
aClient *find_chasing(aClient *sptr, char *user, int *chasing)
{
    aClient *who = find_client(user);

    if (chasing)
	*chasing = 0;
    if (who)
	return who;
    if (!(who = get_history(user, (long) KILLCHASETIMELIMIT))) {
	send_me_numeric(sptr, ERR_NOSUCHNICK, user);
	return ((aClient *) NULL);
    }
    if (chasing)
	*chasing = 1;
    return who;
}

aClient *find_chasing_user(char *user)
{
    aClient *who = find_client(user);

    if (who)
        return who;

    if (!(who = get_history(user, (long) KILLCHASETIMELIMIT)))  
        return ((aClient *) NULL);

    return who;
}

aClient *find_person(char *name)
{
   aClient *acptr;

   if (name == NULL)
	return NULL;

   acptr = find_client(name);

   return acptr ? (IsClient(acptr) ? acptr : NULL) : NULL;
}

/* Functions taken from +CSr31, paranoified to check that the client
 * ** isn't on a llist already when adding, and is there when removing -orabidoo
 */
void add_client_to_llist(struct Client **bucket, struct Client *aclient)
{
    if (!aclient->lprev && !aclient->lnext) {
	aclient->lprev = NULL;
	if ((aclient->lnext = *bucket) != NULL) {
	    aclient->lnext->lprev = aclient;
	}
	*bucket = aclient;
    }
}

void del_client_from_llist(struct Client **bucket, struct Client *aclient)
{
    if (aclient->lprev) {
	aclient->lprev->lnext = aclient->lnext;
    } else if (*bucket == aclient) {
	*bucket = aclient->lnext;
    }
    if (aclient->lnext) {
	aclient->lnext->lprev = aclient->lprev;
    }
    aclient->lnext = aclient->lprev = NULL;
}

/*
 * * get_client_name *      Return the name of the client for various
 * tracking and *      admin purposes. The main purpose of this
 * function is to *      return the "socket host" name of the client,
 * if that *    differs from the advertised name (other than case). *
 * But, this can be used to any client structure. *
 *
 *      Returns: *        "name[user@ip#.port]" if 'showip' is true; *
 * "name[sockethost]", if name and sockhost are different and *
 * showip is false; else *        "name". *
 *
 * NOTE 1: *    Watch out the allocation of "nbuf", if either   
 * sptr->name * or sptr->sockhost gets changed into pointers instead of *
 * directly allocated within the structure... *
 *
 * NOTE 2: *    Function return either a pointer to the structure
 * (sptr) or *  to internal buffer (nbuf). *NEVER* use the returned
 * pointer *    to modify what it points!!!
 */
char *get_client_name(aClient *sptr, int showip)
{
    static char nbuf[HOSTLEN * 2 + USERLEN + 5];

    if (MyConnect(sptr)) {
        switch (showip) {
            case TRUE:
                ircsprintf(nbuf, "%^C [%s]", sptr, sptr->hostip);
                break;
            case HIDEME: 
                ircsprintf(nbuf, "%^C", sptr);
                break;
            default:
                if (irc_strcmp(sptr->name, sptr->sockhost))
                    ircsprintf(nbuf, "%s[%s]", sptr->name, sptr->sockhost);
                else
                    return sptr->name;
        }
        return nbuf;
    }
    return sptr->name;
}

