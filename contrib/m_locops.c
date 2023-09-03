/*************************************************************************
 *   IRC - Internet Relay Chat, modules/m_locops.c
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
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "h.h"

static struct Message _msgtab[] = {
    {MSG_LOCOPS, 0, 1, M_SLOW | M_IDLE_RESET, 0L,
     m_unregistered, m_permission, m_locops, m_ignore, m_ignore}
};

#ifndef STATIC_MODULES

char *_version = "$Revision: 1.2 $";

void _modinit(void)
{
    mod_add_cmd(_msgtab);
}

void _moddeinit(void)
{
    mod_del_cmd(_msgtab);
}
#else
void m_locops_init(void)
{
    mod_add_cmd(_msgtab);
}
#endif

/* send a notice message to all local operators */

int m_locops(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    char *message = parc > 1 ? parv[1] : NULL;

    if (BadPtr(message)) {
	send_me_numeric(sptr, ERR_NEEDMOREPARAMS, MSG_LOCOPS);
	return 0;
    }

    sendto_operators(0, "LocOps", "from %C: %s", sptr, message);
    return 0;
}
