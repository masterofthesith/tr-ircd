/************************************************************************
*   IRC - Internet Relay Chat, src/whowas.c
*   Copyright (C) 1990 Markku Savela
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
 * $Id: whowas.c,v 1.5 2006/01/03 23:57:30 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "h.h"

static void add_whowas_to_clist(aWhowas **, aWhowas *);
static void del_whowas_from_clist(aWhowas **, aWhowas *);
static void add_whowas_to_list(aWhowas **, aWhowas *);
static void del_whowas_from_list(aWhowas **, aWhowas *);

aWhowas WHOWAS[NICKNAMEHISTORYLENGTH];
aWhowas *WHOWASHASH[HASHSIZE];

int whowas_next = 0;

unsigned int hash_whowas_name(char *name)
{
  return strhash(name);
}

void add_history(aClient *cptr, int online)
{
    aWhowas *new;

    new = &WHOWAS[whowas_next];

    if (new->hashv != -1) {
	if (new->online)
	    del_whowas_from_clist(&(new->online->whowas), new);
	del_whowas_from_list(&WHOWASHASH[new->hashv], new);
    }
    new->hashv = hash_whowas_name(cptr->name);
    new->logoff = NOW;
    strlcpy_irc(new->name, cptr->name, NICKLEN);
    strlcpy_irc(new->username, cptr->user->username, USERLEN);
    strlcpy_irc(new->hostname, cptr->user->fakehost, HOSTLEN);
    strlcpy_irc(new->morehost, cptr->user->host, HOSTLEN);
    strlcpy_irc(new->realname, cptr->info, REALLEN);
    DupString(new->servername, cptr->user->server);

    if (online) {
	new->online = cptr;
	add_whowas_to_clist(&(cptr->whowas), new);
    } else
	new->online = NULL;
    add_whowas_to_list(&WHOWASHASH[new->hashv], new);
    whowas_next++;
    if (whowas_next == NICKNAMEHISTORYLENGTH)
	whowas_next = 0;
}

void off_history(aClient *cptr)
{
    aWhowas *temp, *next;

    for (temp = cptr->whowas; temp; temp = next) {
	next = temp->cnext;
	temp->online = NULL;
	del_whowas_from_clist(&(cptr->whowas), temp);
    }
}

aClient *get_history(char *nick, time_t timelimit)
{
    aWhowas *temp;
    int blah;

    timelimit = NOW - timelimit;
    blah = hash_whowas_name(nick);
    temp = WHOWASHASH[blah];
    for (; temp; temp = temp->next) {
	if (irc_strcmp(nick, temp->name))
	    continue;
	if (temp->logoff < timelimit)
	    continue;
	return temp->online;
    }
    return NULL;
}

void count_whowas_memory(int *wwu, u_long *wwum)
{
    aWhowas *tmp;
    int i;
    int u = 0;
    u_long um = 0;

    /*
     * count the number of used whowas structs in 'u' 
     * count up the memory used of whowas structs in um 
     */

    for (i = 0, tmp = &WHOWAS[0]; i < NICKNAMEHISTORYLENGTH; i++, tmp++)
	if (tmp->hashv != -1) {
	    u++;
	    um += sizeof(aWhowas);
	}
    *wwu = u;
    *wwum = um;
    return;
}

void initwhowas()
{
int i;

    for (i = 0; i < NICKNAMEHISTORYLENGTH; i++) {
	memset((char *) &WHOWAS[i], '\0', sizeof(aWhowas));
	WHOWAS[i].hashv = -1;
    }
    for (i = 0; i < HASHSIZE; i++)
	WHOWASHASH[i] = NULL;
}

static void add_whowas_to_clist(aWhowas ** bucket, aWhowas * whowas)
{
    whowas->cprev = NULL;
    if ((whowas->cnext = *bucket) != NULL)
	whowas->cnext->cprev = whowas;
    *bucket = whowas;
}

static void del_whowas_from_clist(aWhowas ** bucket, aWhowas * whowas)
{
    if (whowas->cprev)
	whowas->cprev->cnext = whowas->cnext;
    else
	*bucket = whowas->cnext;
    if (whowas->cnext)
	whowas->cnext->cprev = whowas->cprev;
}

static void add_whowas_to_list(aWhowas ** bucket, aWhowas * whowas)
{
    whowas->prev = NULL;
    if ((whowas->next = *bucket) != NULL)
	whowas->next->prev = whowas;
    *bucket = whowas;
}

static void del_whowas_from_list(aWhowas ** bucket, aWhowas * whowas)
{
    if (whowas->prev)
	whowas->prev->next = whowas->next;
    else
	*bucket = whowas->next;
    if (whowas->next)
	whowas->next->prev = whowas->prev;
}
