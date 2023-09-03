/************************************************************************
 *   IRC - Internet Relay Chat, server/s_accept.c
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
 * $Id: s_accept.c,v 1.6 2004/07/12 09:14:59 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "numeric.h"

int accept_client(aClient *sptr, aClient *cptr)
{
    dlink_node *ptr;
    int cnt;

    if (!IsPerson(cptr) || !IsPerson(sptr))
	return 0;
    cnt = dlink_list_length(&(sptr->user->accepted));

    if (dlinkFind(&(sptr->user->accepted), cptr))
	return 0;
    if (++cnt >= MAXACCEPTED) {
	send_me_numeric(sptr, ERR_TOOMANYACCEPT, cptr->name, MAXACCEPTED);
	return 0;
    }
    ptr = make_dlink_node();
    dlinkAdd(cptr, ptr, &(sptr->user->accepted));
    ptr = make_dlink_node();
    dlinkAdd(sptr, ptr, &(cptr->user->accepted_by));
    send_me_numeric(sptr, RPL_ACCEPTSTATUS, cptr->name, "added to");
    return 0;
}

int deny_client(aClient *sptr, aClient *optr)
{
    dlink_node **inv, *tmp;

    for (inv = &(sptr->user->accepted.head); (tmp = *inv); inv = &tmp->next) {
	if (tmp->data == optr) {
	    dlinkDeleteNode(tmp, &(sptr->user->accepted));
	    break;
	}
    }
    for (inv = &(optr->user->accepted_by.head); (tmp = *inv); inv = &tmp->next) {
	if (tmp->data == sptr) {
	    dlinkDeleteNode(tmp, (&(optr->user->accepted_by)));
	    break;
	}
    }
    send_me_numeric(sptr, RPL_ACCEPTSTATUS, optr->name, "removed from");
    return 0;
}

int remove_accept_references(aClient *sptr)
{
    aClient *acptr;
    dlink_node *lp, *nextlp;
    dlink_node **lpp, *tmp, *next_tmp;
    lp = sptr->user->accepted.head;
    while (lp) {
	nextlp = lp->next;
	acptr = lp->data;
	for (lpp = &(acptr->user->accepted_by.head); (tmp = *lpp); lpp = &next_tmp) {
	    next_tmp = tmp->next;
	    if (tmp->data == sptr) 
		dlinkDeleteNode(tmp, &(acptr->user->accepted_by));
	}
	dlinkDeleteNode(lp, &(sptr->user->accepted));
	lp = nextlp;
    }
    lp = sptr->user->accepted_by.head;
    while (lp) {
        nextlp = lp->next;
        acptr = lp->data;
        for (lpp = &(acptr->user->accepted.head); (tmp = *lpp); lpp = &next_tmp) {
	    next_tmp = tmp->next;
            if (tmp->data == sptr) {
                sendto_one(acptr, ":%C %N %s :%s has been removed from your ACCEPT list for signing off",
                           &me, RPL_ACCEPTINFO, acptr->name, sptr->name);
                dlinkDeleteNode(tmp, &(acptr->user->accepted));
            }
        }
        dlinkDeleteNode(lp, &(sptr->user->accepted_by));
	lp = nextlp;
    }
    lp = sptr->user->accept_informed.head;
    while (lp) {
        nextlp = lp->next;
        acptr = lp->data;
        for (lpp = &(acptr->user->accept_informed_by.head); (tmp = *lpp); lpp = &next_tmp) {
            next_tmp = tmp->next;
            if (tmp->data == sptr)
                dlinkDeleteNode(tmp, &(acptr->user->accept_informed_by));
        }
        dlinkDeleteNode(lp, &(sptr->user->accept_informed));
        lp = nextlp;
    }
    lp = sptr->user->accept_informed_by.head;
    while (lp) {
        nextlp = lp->next;
        acptr = lp->data;
        for (lpp = &(acptr->user->accept_informed.head); (tmp = *lpp); lpp = &next_tmp) {
            next_tmp = tmp->next;
            if (tmp->data == sptr) 
                dlinkDeleteNode(tmp, &(acptr->user->accept_informed));
        }
        dlinkDeleteNode(lp, &(sptr->user->accept_informed_by));
        lp = nextlp;
    }

    return 0;
}

int check_accept(aClient *from, aClient *to)
{
    dlink_node *lp;
    aClient *acptr;

    if (!MyClient(to))
	return 1;

    if (!IsDenyPrivmsg(to))
	return 1;

    if (IsPrivileged(from))
	return 1;

    for (lp = to->user->accepted.head; lp; lp = lp->next) {
        acptr = lp->data;
        if (acptr == from)
            return 1;
    }

    for (lp = to->user->accept_informed.head; lp; lp = lp->next) {
        acptr = lp->data;
        if (acptr == from)
            return 0;
    }

    send_me_notice(from, ":The user is not accepting private messages.");
    send_me_notice(to,  ":%s (%s@%s) has attempted to private message you. If you want talking to him/her "
		 	"add him to you accept list. For more information /quote help accept",
		   from->name, from->user->username, IsFake(from) ? from->user->fakehost : from->user->host);

    lp = make_dlink_node();
    dlinkAdd(from, lp, &(to->user->accept_informed));
    lp = make_dlink_node();
    dlinkAdd(to, lp, &(from->user->accept_informed_by));

    return 0;
}
