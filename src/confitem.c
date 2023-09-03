/************************************************************************
 *   IRC - Internet Relay Chat, src/confitem.c
 *   Copyright (C) 1995 Philippe Levan
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
 * $Id: confitem.c,v 1.6 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

/*
 * The dich_conf.h and dich_conf.c were written to provide a generic
 * interface for configuration lines matching. I decided to write it
 * after I read Roy's K: line tree patch. the underlying ideas are the
 * following : . get rid of the left/right branches by using a
 * dichotomy on an ordered list . arrange patterns matching one another
 * in a tree so that if we find a possible match, all other possible
 * matches are "below" These routines are meant for fast matching.
 * There is no notion of "best" of "first" (meaning the order in which
 * the lines are read) match. Therefore, the following functions are
 * fit for K: lines matching but not I: lines matching (as sad as it
 * may be). Other kinds of configuration lines aren't as time consuming
 * or just aren't use for matching so it's irrelevant to try and use
 * these functions for anything else. However, I still made them as
 * generic as possible, just in case.
 * 
 * -Soleil (Philippe Levan)
 * 
 */
/*
 * To understand this code, read Knuth, Sorting and Searching, third
 * volume of "The Art Of Computer Programming" pg. 472 dich_conf.c is
 * an implementation of multi-way trees, sometimes referred to as "B*"
 * or "B+" tree's In this implementation, the host part forms a
 * multi-way tree, with the bottom most leaf pointing to a link list of
 * aConfItem items, each corresponding to an user. Note: Soleil is
 * doing his own block allocator.
 * 
 * -Dianora
 */

#include "sys.h"
#include "common.h"
#include "struct.h"
#include "resnew.h"
#include "h.h"
#include "confitem.h"
#include "s_conf.h"
#include "listener.h"

aConfItem *make_conf()
{
aConfItem *aconf;

    aconf = (struct ConfItem *) MyMalloc(sizeof(aConfItem));
    memset((char *) aconf, '\0', sizeof(aConfItem));

    aconf->status = CONF_ILLEGAL;
    aconf->aftype = AF_INET;
    aconf->name = NULL;
    aconf->user = NULL;
    aconf->host = NULL;
    aconf->localhost = NULL;
    aconf->class = NULL;

    uuid_generate(aconf->uuid);

    return (aconf);
}

void free_conf(struct ConfItem *aconf)
{
    if (aconf == NULL)
	return;
    delete_adns_queries(aconf->dns_query);
    MyFree(aconf->host);
    if (aconf->passwd)
	memset(aconf->passwd, 0, strlen(aconf->passwd));
    // MyFree(aconf->passwd);
    if (aconf->spasswd)
	memset(aconf->spasswd, 0, strlen(aconf->spasswd));
    // MyFree(aconf->spasswd);
    MyFree(aconf->user);
    MyFree(aconf->localhost);
    MyFree(aconf->name);
    MyFree(aconf->uuid);
    MyFree((char *) aconf);
    return;
}

void delist_and_remove_conf(struct ConfItem *aconf)
{
    aConfItem *tmp;
    aConfItem *next_tmp;

    if (aconf == GlobalConfItemList) {
	GlobalConfItemList = aconf->next;
    } else {
	for (tmp = GlobalConfItemList; tmp; tmp = next_tmp) {
	    next_tmp = tmp->next;
	    if (next_tmp == aconf) {
		tmp->next = aconf->next;
		if (aconf->next)
		    next_tmp = aconf->next->next;
	    }
	}
    }
    aconf->next = NULL;

    free_conf(aconf);
}

/*
 * remove all conf entries from the client except those which match the
 * status field mask.
 */

void det_confs_butmask(aClient *cptr, int mask)
{
    dlink_node *tmp, *tmp2;
    aConfItem *aconf;

    for (tmp = cptr->confs.head; tmp; tmp = tmp2) {
	tmp2 = tmp->next;
	aconf = tmp->data;
	if ((aconf->status & mask) == 0)
	    (void) detach_conf(cptr, aconf);
    }
}

/*
 * ** detach_conf
 * **        Disassociate configuration from the client.
 * **      Also removes a class from the list if marked for deleting.
 */
int detach_conf(struct Client *client_p, struct ConfItem *aconf)
{
    dlink_node *ptr;

    if (aconf == NULL)
	return -1;

    for (ptr = client_p->confs.head; ptr; ptr = ptr->next) {
	if (ptr->data == aconf) {
	    if (aconf->class) {
		if (aconf->status & CONF_CLIENT_MASK) {
		    if (aconf->class->links > 0)
			--(aconf->class->links);
		}
		if (aconf->class->max_connections == -1 && aconf->class->links == 0) {
		    free_class(aconf->class);
		    aconf->class = NULL;
		}
	    }
	    if (!--aconf->clients && IsIllegal(aconf))
		free_conf(aconf);
	    dlinkDelete(ptr, &client_p->confs);
	    free_dlink_node(ptr);
	    return 0;
	}
    }
    return -1;
}

static int is_attached(struct ConfItem *aconf, struct Client *client_p)
{
    dlink_node *ptr = NULL;

    for (ptr = client_p->confs.head; ptr; ptr = ptr->next)
	if (ptr->data == aconf)
	    return 1;

    return 0;
}

/*
 * attach_conf
 * 
 * inputs       - client pointer
 *              - conf pointer  
 * output       -
 * side effects - Associate a specific configuration entry to a *local*
 *                client (this is the one which used in accepting the  
 *                connection). Note, that this automatically changes the
 *                attachment if there was an old one...
 */

int attach_conf(struct Client *client_p, struct ConfItem *aconf)
{
    dlink_node *lp;

    if (is_attached(aconf, client_p)) {
	return 1;
    }
    if (IsIllegal(aconf)) {
	return (NOT_AUTHORIZED);
    }

    lp = make_dlink_node();

    dlinkAdd(aconf, lp, &client_p->confs);

    aconf->clients++;
    if (aconf->status & CONF_CLIENT_MASK)
	aconf->class->links++;

    return 0;
}

/*
 * find a conf entry which matches the hostname and has the same name.
 */
struct ConfItem *find_conf_exact(char *name, char *user, char *host, int statmask)
{
    struct ConfItem *tmp;

    for (tmp = GlobalConfItemList; tmp; tmp = tmp->next) {
	if (!(tmp->status & statmask) || !tmp->name || !tmp->host || irc_strcmp(tmp->name, name))
	    continue;
	/*
	 * ** Accept if the *real* hostname (usually sockethost)
	 * ** socket host) matches *either* host or name field
	 * ** of the configuration.
	 */
	if (match(tmp->host, host)
	    || match(tmp->user, user))
	    continue;
	if (tmp->status & CONF_OPERATOR) {
	    if (tmp->clients < tmp->class->max_connections) {
		return tmp;
	    } else
		continue;
	} else
	    return tmp;
    }
    return NULL;
}

struct ConfItem *find_conf_name(dlink_list * list, char *name, int statmask)
{
    dlink_node *ptr;
    struct ConfItem *aconf;

    for (ptr = list->head; ptr; ptr = ptr->next) {
	aconf = ptr->data;
	if ((aconf->status & statmask) && aconf->name &&
	    (!irc_strcmp(aconf->name, name) || !match(aconf->name, name)))
	    return aconf;
    }
    return NULL;
}

/*
 * find_conf_by_name - return a conf item that matches name and type
 */
struct ConfItem *find_conf_by_name(char *name, int status)
{
    struct ConfItem *aconf;

    if (!name)
	return NULL;

    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
	if ((aconf->status & status) && aconf->name && !match(aconf->name, name))
	    return aconf;
    }
    return NULL;
}

/*
 * find_conf_by_name - return a conf item that matches host and type
 */
struct ConfItem *find_conf_by_host(char *host, int status)
{
    struct ConfItem *aconf;
    if (!host)
	return NULL;

    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
	if ((aconf->status & status) && aconf->host && !match(aconf->host, host))
	    return aconf;
    }
    return NULL;
}

/*
 * find_conf_by_name - return a conf item that matches id and type
 */
struct ConfItem *find_conf_by_uuid(char *uuid, int status)
{
    struct ConfItem *aconf;
    uuid_t u;

    if (!uuid)
	return NULL;

    uuid_parse(uuid, u);

    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
	if ((aconf->status & status) && !uuid_compare(aconf->uuid, u))
	    return aconf;
    }
    return NULL;
}

struct ConfItem *find_conf_for_client(aClient *cptr, int status, int flags)
{
    struct ConfItem *aconf;
    int p;

    if (IsExited(cptr))
	return NULL;

    p = cptr->listener->port;

    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
	if ((aconf->status & status) && aconf->host && !match(aconf->host, cptr->hostip)) {
	    if (flags) {
		if ((aconf->port && (aconf->port == p)) || !(aconf->port))
		    return aconf;
	    } else
		return aconf;

	}
        else if ((aconf->status & status) && aconf->host && !match(aconf->host, cptr->sockhost)) {
            if (flags) {
                if ((aconf->port && (aconf->port == p)) || !(aconf->port))
                    return aconf;
            } else
                return aconf;

        }

    }
    return NULL;
}
