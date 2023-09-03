/************************************************************************
 *   IRC - Internet Relay Chat, src/maskitem.c
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
 *
 * $Id: maskitem.c,v 1.8 2004/02/28 22:23:35 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "confitem.h"
#include "h.h"
#include "hostmask.h"
#include "hook.h"
#include "msg.h"
#include "numeric.h"
#include "s_conf.h"

BlockHeap *maskitem_entries;

dlink_list maskitem_list[MASKITEM_MASK];

static aHashEntry maskhash[MASKITEM_MASK][MASK_MAX];

/*
 * This is the new maskitem interface for all of the functions 
 * that depend on certain mask types.
 * klines, glines, autokills, quarantines, gecos bans, zaplines ...
 *
 */

void init_maskitem(void)
{
    int i;
    maskitem_entries = BlockHeapCreate(sizeof(aMaskItem), MASKITEM_PREALLOCATE);
    memset(maskhash, 0, sizeof(maskhash));
    memset(maskitem_list, 0, sizeof(maskitem_list));
    for (i = 0; i < MASKITEM_MASK; i++)
	maskitem_list[i].head = maskitem_list[i].tail = NULL;
}

/* unsigned long hash_ipv4(struct irc_inaddr*)
 * Input: An IP address.
 * Output: A hash value of the IP address.
 * Side effects: None
 */
static unsigned long hash_ipv4(struct irc_inaddr *addr, int bits)
{
    unsigned long av = ntohl(addr->sins.sin.s_addr) & ~((1 << (32 - bits)) - 1);
    return (av ^ (av >> 12) ^ (av >> 24)) & (MASK_MAX - 1);
}

/* unsigned long hash_ipv6(struct irc_inaddr*)
 * Input: An IP address.
 * Output: A hash value of the IP address.
 * Side effects: None
 */
#ifdef IPV6
static unsigned long hash_ipv6(struct irc_inaddr *addr, int bits)
{
    unsigned long v = 0, n;
    for (n = 0; n < 16; n++) {
	if (bits >= 8) {
	    v ^= addr->sins.sin6.s6_addr[n];
	    bits -= 8;
	} else if (bits) {
	    v ^= addr->sins.sin6.s6_addr[n] & ~((1 << (8 - bits)) - 1);
	    return v & (MASK_MAX - 1);
	} else
	    return v & (MASK_MAX - 1);
    }
    return v & (MASK_MAX - 1);
}
#endif

/* unsigned long hash_text(const char *start)
 * Input: The start of the text to hash.
 * Output: The hash of the string between 1 and (TH_MAX-1)
 * Side-effects: None.
 */
static unsigned long hash_string(char *start)
{
    const char *p = start;
    unsigned long h = 0;

    while (*p) {
	h = (h << 4) - (h + (unsigned char) ToLower(*p++));
    }
    return (h & (MASK_MAX - 1));
}

static struct MaskItem *make_maskitem(int type, unsigned long len)
{
    aMaskItem *ami;

    ami = (aMaskItem *) BlockHeapAlloc(maskitem_entries);

    if (ami == NULL)
	outofmemory("Create maskitem");

    memset(ami, 0, sizeof(aMaskItem));
    ami->type = type;
    ami->creation = time(NULL);
    ami->expiration = (len ? time(NULL) + len : 0);
    ami->username = NULL;

    return ami;
}

static void free_maskitem(struct MaskItem *ami)
{
    if (ami->string)
	MyFree(ami->string);
    if (ami->username)
	MyFree(ami->username);
    if (ami->ownername)
	MyFree(ami->ownername);
    if (ami->reason)
	MyFree(ami->reason);
    ireg_free_ami(ami);

    BlockHeapFree(maskitem_entries, ami);
}

static void remove_from_maskitem_hashtable(aMaskItem * ami)
{
aMaskItem *tmp, *prev = NULL;
int hashval = ami->hashval;
int htype = ami->type - 1;

    for (tmp = (aMaskItem *) maskhash[htype][hashval].list; tmp; tmp = tmp->hnext) {
	if (tmp == ami) {
	    if (prev)
		prev->hnext = tmp->hnext;
	    else
		maskhash[htype][hashval].list = (void *) tmp->hnext;
	    tmp->hnext = NULL;
	    if (maskhash[htype][hashval].links > 0)
		--maskhash[htype][hashval].links;
	    break;
	}
	prev = tmp;
    }
}

struct MaskItem *create_maskitem(char *owner, char *string, char *user, int type,
				 unsigned long length)
{
    /* The value of string may happily differ from host, ip, nick, gecos entries */

    struct MaskItem *ami = NULL;
    unsigned long hashval = 0;
    int htype = type - 1;
    dlink_node *ptr;

    ami = find_maskitem(string, user, type, 0);
    if (ami)
	return ami;

    switch (type) {

	case MASKITEM_KLINE:
	case MASKITEM_KLINE_CONFIG:
	case MASKITEM_AUTOKILL:
	case MASKITEM_EXCLUDE:
	    if (user && string) {
    int bits, t;
		ami = make_maskitem(type, length);
		t = parse_netmask(string, &(ami->ipnum), &bits);
		if (t == HM_HOST)
		    hashval = hash_string(string);
#ifdef IPV6
		if (t == HM_IPV6)
		    hashval = hash_ipv6(&(ami->ipnum), bits);
		else if (t == HM_IPV4)
		    hashval = hash_ipv4(&(ami->ipnum), bits);
#else
		if (t == HM_IPV4)
		    hashval = hash_ipv4(&(ami->ipnum), bits);
#endif
		DupString(ami->username, user);
	    }
	    break;
	case MASKITEM_ZAPLINE:
	case MASKITEM_ZAPLINE_CONFIG:
	    if (string) {
    int bits, t;
		ami = make_maskitem(type, length);
		t = parse_netmask(string, &(ami->ipnum), &bits);
		if (t == HM_HOST) {
		    free_maskitem(ami);
		    ami = NULL;
		    break;
		}
#ifdef IPV6
		if (t == HM_IPV6)
		    hashval = hash_ipv6(&(ami->ipnum), bits);
		else
		    hashval = hash_ipv4(&(ami->ipnum), bits);
#else
		hashval = hash_ipv4(&(ami->ipnum), bits);
#endif
	    }
	    break;
	case MASKITEM_KLINE_REGEX:
	    if (string) {
		ami = make_maskitem(type, length);
		hashval = hash_string(string);
	    }
	    if (user) 
		DupString(ami->username, user);
	    break;
	default:
	    if (string) {
		ami = make_maskitem(type, length);
		hashval = hash_string(string);
	    }
	    break;
    }

    if (!ami)
	return NULL;

    DupString(ami->ownername, owner);
    DupString(ami->string, string);

    ami->hashval = hashval;

    /* In order to use the table at an effective level, we do -1 */

    ami->hnext = (aMaskItem *) maskhash[htype][hashval].list;

    maskhash[htype][hashval].list = (void *) ami;
    maskhash[htype][hashval].links++;
    maskhash[htype][hashval].hits++;

    ptr = make_dlink_node();

    dlinkAdd(ami, ptr, &maskitem_list[htype]);
    return ami;

}

struct MaskItem *find_maskitem(char *string, char *user, int type, int m)
{
    dlink_node *ptr;
    aMaskItem *ami;
    int htype = type - 1;
    int (*compfunc)(char *, char *);

    if (!string || string[0] == '\0')
	return NULL;

    expire_maskitems();

    compfunc = m ? match : irc_strcmp;

    if (type < 13) {
	for (ptr = maskitem_list[htype].head; ptr; ptr = ptr->next) {
	    ami = ptr->data;
	    if (!ami->string)
		continue;
	    if (compfunc(ami->string, string) == 0) {
		if (user && ami->username) {
		    if (compfunc(ami->username, user) == 0)
			return ami;
		} else {
		    return ami;
		}
	    }
	}
    } else {
        for (ptr = maskitem_list[htype].head; ptr; ptr = ptr->next) {
            ami = ptr->data;
            if (ireg_match(ami, string) > 0) 
                return ami;
        }
    }
    return NULL;
}

void terminate_maskitem(char *string, char *user, int type)
{
    struct MaskItem *ami;
    int htype = type - 1;
    dlink_node *ptr;

    /* first: we locate the maskitem */

    ami = find_maskitem(string, user, type, 0);

    if (!ami)
	return;

    /* second: we remove this from the maskitem hashtable */

    remove_from_maskitem_hashtable(ami);

    /* third: we remove this maskitem from the linked list */

    if ((ptr = dlinkFind(&maskitem_list[htype], ami)))
    	dlinkDeleteNode(ptr, &maskitem_list[htype]);

    /* fourth: we remove this maskitem from the memory */

    free_maskitem(ami);
}

void expire_maskitems(void) 
{
    dlink_node *ptr, *next_ptr;
    aMaskItem *ami;

    int i = 0;

    for (i = 0; i < MASKITEM_MASK; i++) {
	for (ptr = maskitem_list[i].head; ptr; ptr = next_ptr) {
	    next_ptr = ptr->next;
	    ami = ptr->data;
	    if (!ami)
		continue;
	    if (ami->expiration > 0 && ami->expiration <= time(NULL)) {
		remove_from_maskitem_hashtable(ami);
		dlinkDeleteNode(ptr, &maskitem_list[i]);
		free_maskitem(ami);
	    }
	}
    }
}

void report_maskitem_list_secondary(aClient *cptr, char *matchto, int type, int reply, char letter)
{
    int htype = type - 1;
    dlink_node *ptr;
    aMaskItem *ami;

    expire_maskitems();

    for (ptr = maskitem_list[htype].head; ptr; ptr = ptr->next) {
	ami = ptr->data;
	if (!ami)
	    continue;
	if (matchto) {
	    if (match(ami->string, matchto) == 0)
		send_me_numeric(cptr, reply, letter, ami->reason, ami->string, 0, 0);
	} else {
	    send_me_numeric(cptr, reply, letter, ami->reason, ami->string, 0, 0);
	}
    }
    return;
}

void report_maskitem_list_primary(aClient *cptr, char *matchto, int type, int reply, char letter)
{
    int htype = type - 1;
    dlink_node *ptr;
    aMaskItem *ami;

    expire_maskitems();

    for (ptr = maskitem_list[htype].head; ptr; ptr = ptr->next) {
	ami = ptr->data;
	if (!ami)
	    continue;
	if (matchto) {
	    if (match(ami->string, matchto) == 0)
		send_me_numeric(cptr, reply, letter, ami->string, ami->username,
				(ami->expiration ? ami->expiration - ami->creation : 0),
				ami->reason);
	} else {
	    send_me_numeric(cptr, reply, letter, ami->string, ami->username,
			    (ami->expiration ? ami->expiration - ami->creation : 0), ami->reason);
	}
    }
    return;
}

void push_all_maskitems(aClient *cptr, int type)
{
    int htype = type - 1;
    dlink_node *ptr;
    aMaskItem *ami;

    expire_maskitems();

    for (ptr = maskitem_list[htype].head; ptr; ptr = ptr->next) {
	ami = ptr->data;
	if (!ami)
	    continue;
	switch (type) {
	    case MASKITEM_QUARANTINE:
		sendto_one_server(cptr, &me, TOK1_SQLINE, "%M :%s", ami, ami->reason);
		break;
	    case MASKITEM_GECOS:
		sendto_one_server(cptr, &me, TOK1_SGLINE, "%d :%M:%s", strlen(ami->string),
				  ami, ami->reason);
		break;
	    case MASKITEM_JUPITER:
		sendto_one_server(cptr, &me, TOK1_JUPITER, "%M :%s", ami, ami->reason);
		break;
	    case MASKITEM_ZAPLINE:
		sendto_one_server(cptr, &me, TOK1_SZLINE, "%M :%s", ami, ami->reason);
		break;
	    default:
		break;
	}
    }
}

void rehash_maskitems(int type)
{
    int htype = type - 1;
    dlink_node *ptr, *next_ptr;
    aMaskItem *ami;

    for (ptr = maskitem_list[htype].head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	ami = ptr->data;
	if (!ami)
	    continue;

	remove_from_maskitem_hashtable(ami);
	dlinkDeleteNode(ptr, &maskitem_list[htype]);
	free_maskitem(ami);
    }
}
