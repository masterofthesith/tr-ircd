/************************************************************************
 *   IRC - Internet Relay Chat, server/s_silence.c
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
 * $Id: s_silence.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "numeric.h"
#include "msg.h"

/*
 * is_silenced - Returns 1 if a sptr is silenced by acptr
 */
int is_silenced(aClient *sptr, aClient *acptr)
{
    dlink_node *lp;
    anUser *user;
    char *cp;
    char sender[HOSTLEN + NICKLEN + USERLEN + 5];

    if (!(acptr->user) || !(acptr->user->silence.head) || !(sptr->user))
        return 0;
    user = sptr->user;
    ircsprintf(sender, "%s!%s@%s", sptr->name, user->username, user->host);
    for (lp = acptr->user->silence.head; lp; lp = lp->next) {
        cp = lp->data;
        if (!match(cp, sender)) {
            if (!MyConnect(sptr))
                sendto_one_server(sptr->from, acptr, TOK1_SILENCE, "%~C :%s", sptr, cp);
            return 1;
        }
    }
    return 0;
}

int del_silence(aClient *sptr, char *mask)
{
    dlink_node **lp;
    char *cp = NULL;

    for (lp = &(sptr->user->silence.head); *lp; lp = &((*lp)->next)) {
        cp = (*lp)->data;
        if ((cp != NULL) && (irc_strcmp(mask, cp) == 0)) {
            dlinkDeleteNode(*lp, &sptr->user->silence);
            return 0;
        }
    }
    return 1;
}

int add_silence(aClient *sptr, char *mask)
{
    dlink_node *lp;
    char *cp;
    int cnt = 0, len = 0;

    for (lp = sptr->user->silence.head; lp; lp = lp->next) {
        cp = lp->data;
        len += strlen(cp);
        if (MyClient(sptr)) {
            if ((len > MAXSILELENGTH) || (++cnt >= MAXSILES)) {
                send_me_numeric(sptr, ERR_SILELISTFULL, mask);
                return -1;
            } else {
                if (!match(cp, mask))
                    return -1;
            }
        } else if (!irc_strcmp(cp, mask))
            return -1;
    }
    lp = make_dlink_node();
    DupString(cp, mask);
    lp->data = cp;
    dlinkAdd(cp, lp, &sptr->user->silence);
    return 0;
}


