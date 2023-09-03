/************************************************************************
 *   IRC - Internet Relay Chat, src/hash.c
 *
 *   Copyright (C)2000-2003 TR-IRCD Development
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
 * $Id: hash.c,v 1.5 2006/01/03 23:57:30 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "s_conf.h"
#include "h.h"

static aHashEntry clientTable[HASHSIZE];
static aHashEntry channelTable[HASHSIZE];
static unsigned int ircd_random_key = 0;
/*
 *  * New hash function based on the Fowler/Noll/Vo (FNV) algorithm from
 *   * http://www.isthe.com/chongo/tech/comp/fnv/
 *    *
 *     * Here, we use the FNV-1 method, which gives slightly better results
 *      * than FNV-1a.   -Michael
 *       */
unsigned int strhash(const char *name) {
	const unsigned char *p = (const unsigned char *)name;
	unsigned int hval = FNV1_32_INIT;
	if (*p == '\0')
		return 0;
	for (; *p != '\0'; ++p) {
		hval += (hval << 1) + (hval <<  4) + (hval << 7) + (hval << 8) + (hval << 24);
		hval ^= (ToLower(*p) ^ ircd_random_key);
	}
	return (hval >> FNV1_32_BITS) ^ (hval & ((1 << FNV1_32_BITS) -1));
}


/*
 * Hashing.
 * 
 * The server uses a chained hash table to provide quick and efficient
 * hash table mantainence (providing the hash function works evenly
 * over the input range).  The hash table is thus not susceptible to
 * problems of filling all the buckets or the need to rehash. It is
 * expected that the hash table would look somehting like this during
 * use: +-----+    +-----+    +-----+   +-----+ 
 *   ---| 224 |----| 225 |----| 226 |---| 227 |--- 
 *      +-----+    +-----+    +-----+   +-----+ 
 *         |          |          | 
 *      +-----+    +-----+    +-----+ 
 *      |  A  |    |  C  |    |  D  | 
 *      +-----+    +-----+    +-----+ 
 *         | 
 *      +-----+ 
 *      |  B  | 
 *      +-----+
 * 
 * A - GOPbot, B - chang, C - hanuaway, D - *.mu.OZ.AU
 * 
 * The order shown above is just one instant of the server.  Each time a
 * lookup is made on an entry in the hash table and it is found, the
 * entry is moved to the top of the chain.
 * 
 * ^^^^^^^^^^^^^^^^ **** Not anymore - Dianora
 * 
 */

static unsigned
int hash_nick_name(const char *name)
{
    return strhash(name);
}

/*
 * hash_channel_name
 *
 * calculate a hash value on at most the first 30 characters of the channel
 * name. Most names are short than this or dissimilar in this range. There
 * is little or no point hashing on a full channel name which maybe 255 chars
 * long.
 */
static unsigned
int hash_channel_name(const char *name)
{
    return strhash(name);
}

void clear_client_hash_table()
{
    memset((char *) clientTable, '\0', sizeof(aHashEntry) * HASHSIZE);
    ircd_random_key = rand() % 256;  /* better than nothing --adx */
}

void clear_channel_hash_table()
{
    memset((char *) channelTable, '\0', sizeof(aHashEntry) * HASHSIZE);
}

int add_to_client_hash_table(char *name, aClient *cptr)
{
    unsigned int hashv;

    if (name == NULL || cptr == NULL)
    	return 0;

    hashv = hash_nick_name(name);
    cptr->hnext = (aClient *) clientTable[hashv].list;
    clientTable[hashv].list = (void *) cptr;
    clientTable[hashv].links++;
    clientTable[hashv].hits++;
    return 0;
}

int add_to_channel_hash_table(char *name, aChannel *chptr)
{
    int hashv;

    if (name == NULL || chptr == NULL)
    	return 0;

    hashv = hash_channel_name(name);
    chptr->hnextch = (aChannel *) channelTable[hashv].list;
    channelTable[hashv].list = (void *) chptr;
    channelTable[hashv].links++;
    channelTable[hashv].hits++;
    return 0;
}

int del_from_client_hash_table(char *name, aClient *cptr)
{
    aClient *tmp, *prev = NULL;
    int hashv;

    if (name == NULL || cptr == NULL)
    	return 0;

    hashv = hash_nick_name(name);
    for (tmp = (aClient *) clientTable[hashv].list; tmp; tmp = tmp->hnext) {
	if (tmp == cptr) {
	    if (prev)
		prev->hnext = tmp->hnext;
	    else
		clientTable[hashv].list = (void *) tmp->hnext;
	    tmp->hnext = NULL;
	    if (clientTable[hashv].links > 0) 
		--clientTable[hashv].links;
	    return 1;
	}
	prev = tmp;
    }
    logevent_call(LogSys.hash_error, cptr, cptr->name,
   		  cptr->from ? cptr->from->sockhost : "??host", 
		  cptr->from, cptr->next, cptr->prev, 
		  cptr->fd, cptr->status, cptr->user);
    return 0;
}

int del_from_channel_hash_table(char *name, aChannel *chptr)
{
    aChannel *tmp, *prev = NULL;
    int hashv;

    hashv = hash_channel_name(name);
    for (tmp = (aChannel *) channelTable[hashv].list; tmp; tmp = tmp->hnextch) {
	if (tmp == chptr) {
	    if (prev)
		prev->hnextch = tmp->hnextch;
	    else
		channelTable[hashv].list = (void *) tmp->hnextch;
	    tmp->hnextch = NULL;
	    if (channelTable[hashv].links > 0)
		--channelTable[hashv].links;
	    return 1;
	}
	prev = tmp;
    }
    return 0;
}

aClient *find_client(char *name)
{
    aClient *tmp;
    int hashv;

    hashv = hash_nick_name(name);
    if (hashv <= 0)
	return NULL;
    tmp = (aClient *) clientTable[hashv].list;

    /*
     * Got the bucket, now search the chain.
     */
    for (; tmp; tmp = tmp->hnext) {
	if (!tmp->name)
	    continue;
	if (irc_strcmp(name, tmp->name) == 0) 
	    return (tmp);
    }
    return NULL;
}

aClient *find_server(char *server)
{
    struct Client *tmp;
    unsigned int hashv;

    hashv = hash_nick_name(server);
    if (hashv <= 0)
        return NULL;
    tmp = (struct Client *) clientTable[hashv].list;

    for (; tmp; tmp = tmp->hnext) {
	if (!IsServer(tmp) && !IsMe(tmp))
	    continue;
	if (irc_strcmp(server, tmp->name) == 0) {
	    return tmp;
	}
    }

    return tmp;
}

aChannel *find_channel(char *name)
{
    int hashv;
    aChannel *tmp;

    if (!name)
	return NULL;

    hashv = hash_channel_name(name);
    if (hashv <= 0)
        return NULL;
    tmp = (aChannel *) channelTable[hashv].list;

    for (; tmp; tmp = tmp->hnextch) {
	if (irc_strcmp(name, tmp->chname) == 0) 
	    return tmp;
    }
    return NULL;
}

aChannel *hash_get_chan_bucket(int hashv)
{
    if (hashv > HASHSIZE)
	return NULL;
    return (aChannel *) channelTable[hashv].list;
}
