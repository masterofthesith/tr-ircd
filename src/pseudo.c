/************************************************************************
 *   IRC - Internet Relay Chat, src/pseudo.c
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

#include "struct.h"
#include "common.h"
#include "msg.h"
#include "sys.h"
#include "numeric.h"
#include "h.h"

int m_pseudo(aClient *cptr, aClient *sptr, char *name, char *host, int parc, char *parv[])
{
    aClient *acptr;

    if (parc < 2 || *parv[1] == '\0') {
        send_me_numeric(sptr, ERR_NOTEXTTOSEND);
        return -1;
    }
    if ((acptr = find_person(name)) && (irc_strcmp(host, acptr->user->server) == 0))
        sendto_one_person(acptr, sptr, TOK1_PRIVMSG, "%s@%s :%s", name, host, parv[1]);
    else
        send_me_numeric(sptr, ERR_SERVICESDOWN, name);
    return 0;
}

int m_pseudo_help(aClient *cptr, aClient *sptr, char *name, char *host, int parc, char *parv[])
{
    aClient *acptr;

    if (parc < 2 || *parv[1] == '\0') {
        send_me_numeric(sptr, ERR_NOTEXTTOSEND);
        return -1;
    }
    if ((acptr = find_person(name)) && (irc_strcmp(host, acptr->user->server) == 0))
        sendto_one_person(acptr, sptr, TOK1_PRIVMSG, "%s@%s :%s", name, host, parv[1]);
    else
        dohelp(sptr, HELPPATH, parv[1], parv[0]);
    return 0;
}


int m_unregistered(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    send_me_numeric(cptr, ERR_NOTREGISTERED);
    return 0;
}

int m_registered(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    send_me_numeric(cptr, ERR_ALREADYREGISTRED);
    return 0;
}

int m_ignore(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    return -1;
}

int m_permission(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    send_me_numeric(cptr, ERR_NOPRIVILEGES);
    return 0;
}


