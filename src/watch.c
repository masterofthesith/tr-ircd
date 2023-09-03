/************************************************************************
 *   IRC - Internet Relay Chat, src/watch.c
 *   Copyright (C) 1991 Darren Reed
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
 * $Id: watch.c,v 1.4 2006/01/03 23:57:30 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "s_conf.h"
#include "h.h"

static unsigned
int hash_nick_name(const char *name)
{ 
    unsigned int h = 0;

    while (*name) {
        h = (h << 4) - (h + (unsigned char) ToLower(*name++));
    }

    return (h & (HASHSIZE - 1));
}

/*
 * Rough figure of the datastructures for notify:
 *
 * NOTIFY HASH      cptr1
 *   |                |- nick1
 * nick1-|- cptr1     |- nick2
 *   |   |- cptr2                cptr3
 *   |   |- cptr3   cptr2          |- nick1
 *   |                |- nick1
 * nick2-|- cptr2     |- nick2
 *       |- cptr1
 *
 * add-to-notify-hash-table:
 * del-from-notify-hash-table:
 * hash-del-notify-list:
 * hash-check-notify:
 * hash-get-notify:
 */

static aWatch *watchTable[WATCHHASHSIZE];

void count_watch_memory(count, memory)
     int *count;
     u_long *memory;
{
int i = WATCHHASHSIZE;
aWatch *awptr;

    while (i--) {
	awptr = watchTable[i];
	while (awptr) {
	    (*count)++;
	    (*memory) += sizeof(aWatch) + strlen(awptr->watchnick);
	    awptr = awptr->hnext;
	}
    }
}

void clear_watch_hash_table(void)
{
    memset((char *) watchTable, '\0', sizeof(watchTable));
}

int add_to_watch_hash_table(char *nick, aClient *cptr)
{
    int hashv;
    aWatch *awptr;
    dlink_node *lp = NULL;

    if (strlen(nick) > NICKLEN)
	return 0;
    hashv = hash_nick_name(nick) % WATCHHASHSIZE;
    if (hashv <= 0)
        return 0;
    if ((awptr = (aWatch *) watchTable[hashv]))
	while (awptr && irc_strcmp(awptr->watchnick, nick))
	    awptr = awptr->hnext;

    if (!awptr) {
	awptr = (aWatch *) MyMalloc(sizeof(aWatch) + strlen(nick));
	awptr->lasttime = timeofday;
	strlcpy_irc(awptr->watchnick, nick, NICKLEN);

	awptr->hnext = watchTable[hashv];
	awptr->watched_by.head = NULL;
	awptr->watched_by.tail = NULL;
	watchTable[hashv] = awptr;
    } else {
    	lp = dlinkFind(&awptr->watched_by, cptr);
    }

    if (!lp) {
	lp = make_dlink_node();
	dlinkAdd(cptr, lp, &awptr->watched_by);

	lp = make_dlink_node();
	dlinkAdd(awptr, lp, &cptr->watchlist);
	cptr->watches++;
    }

    return 0;
}

int hash_check_watch(aClient *cptr, int reply)
{
    int hashv;
    aWatch *awptr;
    aClient *acptr;
    dlink_node *lp;

    hashv = hash_nick_name(cptr->name) % WATCHHASHSIZE;
    if (hashv <= 0)
        return 0;

    if ((awptr = (aWatch *) watchTable[hashv]))
	while (awptr && irc_strcmp(awptr->watchnick, cptr->name))
	    awptr = awptr->hnext;

    if (!awptr)
	return 0;		/* This nick isn't on watch */

    awptr->lasttime = NOW;

    for (lp = awptr->watched_by.head; lp; lp = lp->next) {
	acptr = lp->data;
	send_me_numeric(acptr, reply, cptr->name, (IsPerson(cptr) ? cptr->user->username : "<N/A>"),
			(IsPerson(cptr)
			 ? (IsFake(cptr) ? cptr->user->fakehost : cptr->user->host) : "<N/A>"),
			awptr->lasttime, cptr->info);
    }
    return 0;
}

aWatch *hash_get_watch(char *name)
{
int hashv;
aWatch *awptr;

    hashv = hash_nick_name(name) % WATCHHASHSIZE;
    if (hashv <= 0)
        return NULL;
    if ((awptr = (aWatch *) watchTable[hashv]))
	while (awptr && irc_strcmp(awptr->watchnick, name))
	    awptr = awptr->hnext;

    return awptr;
}

int del_from_watch_hash_table(char *nick, aClient *cptr)
{
    int hashv;
    aWatch *awptr, *nlast = NULL;
    dlink_node *lp;

    hashv = hash_nick_name(nick) % WATCHHASHSIZE;
    if (hashv <= 0)
        return 0;
    if ((awptr = (aWatch *) watchTable[hashv]))
	while (awptr && irc_strcmp(awptr->watchnick, nick)) {
	    nlast = awptr; 
	    awptr = awptr->hnext;
    }

    if (!awptr)
	return 0;		/* No such watch */

    if ((lp = dlinkFind(&awptr->watched_by, cptr)))
    	dlinkDeleteNode(lp, &awptr->watched_by);
    else
	return 0;

    if ((lp = dlinkFind(&cptr->watchlist, awptr)))
	dlinkDeleteNode(lp, &cptr->watchlist);
    /*
     * In case this header is now empty of notices, remove it 
     */
    if (!awptr->watched_by.head) {
	if (!nlast)
	    watchTable[hashv] = awptr->hnext;
	else
	    nlast->hnext = awptr->hnext;
	MyFree(awptr);
    }

    /*
     * Update count of notifies on nick 
     */
    cptr->watches--;

    return 0;
}

int hash_del_watch_list(aClient *cptr)
{
    int hashv;
    aWatch *awptr;
    dlink_node *ptr, *next_ptr, *lp;

    if (!(cptr->watchlist.head))
	return 0;		/* Nothing to do */

    for (ptr = cptr->watchlist.head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	awptr = ptr->data;
	if (awptr) {
	    if ((lp = dlinkFind(&awptr->watched_by, cptr)))
		dlinkDeleteNode(lp, &awptr->watched_by);

	    if (!awptr->watched_by.head) {
    aWatch *awptr2, *awptr3;

		hashv = hash_nick_name(awptr->watchnick) % WATCHHASHSIZE;

		awptr3 = NULL;
		awptr2 = watchTable[hashv];
		while (awptr2 && (awptr2 != awptr)) {
		    awptr3 = awptr2;
		    awptr2 = awptr2->hnext;
		}

		if (awptr3)
		    awptr3->hnext = awptr->hnext;
		else
		    watchTable[hashv] = awptr->hnext;
		MyFree(awptr);
	    } 
	}
	dlinkDeleteNode(ptr, &cptr->watchlist);
    }

    cptr->watches = 0;

    return 0;
}

