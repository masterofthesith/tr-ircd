/*
 * IRC - Internet Relay Chat, src/class.c 
 *
 * Copyrught (C) 2003 TR-IRCD Development. Code based on code by Darren Reed
 * Copyright (C) 1990 Darren Reed
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * $Id: class.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "numeric.h"
#include "sys.h"
#include "h.h"

#define BAD_CONF_CLASS		-1
#define BAD_PING		-2
#define BAD_CLIENT_CLASS	-3

aClass *ConnectionClasses;

int get_conf_class(aConfItem *aconf)
{
    if (!aconf)
	return BAD_CONF_CLASS;
	
    assert(aconf->class != 0);
 
    return aconf->class->number;
}

static int get_conf_ping(aConfItem *aconf)
{
    if (!aconf)
        return BAD_PING;

    assert(aconf->class != 0);

    return aconf->class->ping_frequency;
}

int get_client_class(aClient *acptr)
{
    dlink_node *tmp;
    aClass *cl;
    aConfItem *aconf;

    if (!acptr)
	return BAD_CLIENT_CLASS;

    if (IsMe(acptr))
	return BAD_CLIENT_CLASS;

    for (tmp = acptr->confs.head; tmp; tmp = tmp->next) {
	aconf = tmp->data;
	if (!aconf || !(cl = aconf->class))
	    continue;
	if (cl->number > 0)
	    return cl->number;
    }

    return BAD_CLIENT_CLASS;;
}

int get_client_ping(aClient *acptr)
{
    int ping = PINGFREQUENCY;
    aConfItem *aconf;
    dlink_node *tmp;

    for (tmp = acptr->confs.head; tmp; tmp = tmp->next) {
        aconf = tmp->data;
	if (aconf->status & (CONF_CLIENT | CONF_SERVER)) {
	    ping = get_conf_ping(aconf);
	    if (ping != BAD_PING)
		return ping;
	}
    }

    return PINGFREQUENCY;
}

int get_con_freq(aClass *clptr)
{
    if (clptr)
	return clptr->connect_frequency;
    else
	return CONNECTFREQUENCY;
}

long get_sendq(aClient *cptr)
{
    dlink_node *tmp;
    aClass *cl;
    aConfItem *aconf;

    if (!cptr)
        return BAD_CLIENT_CLASS;

    if (IsMe(cptr))
        return BAD_CLIENT_CLASS;


    for (tmp = cptr->confs.head; tmp; tmp = tmp->next) {
        aconf = tmp->data;
        if (!aconf || !(cl = aconf->class))
            continue;
        if (cl->number > 0) {
	    return cl->max_sendq;
        }
    }
    return MAXSENDQLENGTH;
}


/*
 * When adding a class, check to see if it is already present first. if
 * so, then update the information for that class, rather than create a
 * new entry for it and later delete the old entry. if no present entry
 * is found, then create a new one and add it in immediately after the
 * first one (class 0).
 */
void add_class(int num, int ping, int confreq, int maxli, long sendq)
{
    aClass *new, *tmp;

    if (num == 0)
	return;

    tmp = find_class(num);
    if (tmp == ConnectionClasses) {
	new = (aClass *) make_class();
	new->next = tmp->next;
	tmp->next = new;
    } else
	new = tmp;

    new->number = num;
    new->ping_frequency = ping;
    new->connect_frequency = confreq;
    new->max_connections = maxli;
    new->max_sendq = (sendq > 0) ? sendq : MAXSENDQLENGTH;

    if (new != tmp)
	new->links = 0;
}

aClass *find_class(int cclass)
{
    aClass *cltmp;

    for (cltmp = ConnectionClasses; cltmp; cltmp = cltmp->next)
	if (cltmp->number == cclass)
	    return cltmp;
    return ConnectionClasses;
}

void check_class()
{
aClass *cltmp, *cltmp2;

    for (cltmp2 = cltmp = ConnectionClasses; cltmp; cltmp = cltmp->next) {
	if (cltmp->max_connections < 0) {
	    cltmp2->next = cltmp->next;
	    if (cltmp->links <= 0)
		free_class(cltmp);
	} else
	    cltmp2 = cltmp;
    }
}

void initclass()
{
    ConnectionClasses = (aClass *) make_class();

    ConnectionClasses->number = 0;
    ConnectionClasses->connect_frequency = CONNECTFREQUENCY;
    ConnectionClasses->ping_frequency = PINGFREQUENCY;
    ConnectionClasses->max_connections = MAXIMUM_LINKS;
    ConnectionClasses->max_sendq = MAXSENDQLENGTH;
    ConnectionClasses->links = 0;
    ConnectionClasses->next = NULL;
}

void report_classes(aClient *sptr)
{
    aClass *cltmp;

    for (cltmp = ConnectionClasses; cltmp; cltmp = cltmp->next)
	send_me_numeric(sptr, RPL_STATSYLINE, 'Y', cltmp->number,
			cltmp->ping_frequency, cltmp->connect_frequency,
			cltmp->max_connections, cltmp->max_sendq);
}

aClass *make_class(void) {
aClass *tmp;

    tmp = (aClass *) MyMalloc(sizeof(aClass));
    return tmp;
}

void free_class(aClass *tmp) {
    MyFree((char *) tmp);
}
