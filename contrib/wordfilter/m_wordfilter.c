/************************************************************************
 *   IRC - Internet Relay Chat, m_wordfilter.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
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

#include "wordfilter.h"

/* The following command is used, when a local operator issues the
 * command. */

int o_denytext(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    BadWord *bw;

    if (parc < 3) {
	send_me_numeric(sptr, ERR_NEEDMOREPARAMS, MSG_DENYTEXT);
	return 0;
    } else if (!IsAnOper(sptr)) {
	return m_permission(cptr, sptr, parc, parv);
    }

    parv[1] = collapse(parv[1]);

    if (!(find_badword_entry(parv[1]))) {
	bw = new_badword_entry(parv[1], parv[2], BADWORD_ADD);
	sendto_ops("%^C added badword entry [%s]", sptr, parv[1]);
    }
    sendto_serv_butone(cptr, &me, TOK1_DENYTEXT, "%~C %s :%s", sptr, parv[1], parv[2]);

    return 0;
}

/* If it arrives from a remote server, the following function is used */

int m_denytext(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    aClient *acptr;

    if (parc < 4)
	return 0;

    acptr = find_person(parv[1]);
    if (!acptr)
	return 0;

    parv[1] = parv[2];
    parv[2] = parv[3];

    return o_denytext(cptr, acptr, 3, parv);
}

int o_undenytext(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    if (parc < 2) {
	send_me_numeric(sptr, ERR_NEEDMOREPARAMS, MSG_UNDENYTEXT);
	return 0;
    } else if (!IsAnOper(sptr)) {
	return m_permission(cptr, sptr, parc, parv);
    }

    parv[1] = collapse(parv[1]);

    if (remove_badword_entry(parv[1]))
	sendto_ops("%^C deleted badword entry [%s]", sptr, parv[1]);

    sendto_serv_butone(cptr, &me, TOK1_UNDENYTEXT, "%~C %s", sptr, parv[1]);

    return 0;
}

int m_undenytext(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    aClient *acptr;

    if (parc < 3)
	return 0;

    acptr = find_person(parv[1]);
    if (!acptr)
	return 0;

    parv[1] = parv[2];

    return o_undenytext(cptr, acptr, 2, parv);
}
