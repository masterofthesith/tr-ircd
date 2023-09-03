/*
 *   IRC - Internet Relay Chat, contrib/m_rnotice.c
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
    {MSG_RNOTICE, 0, 1, M_SLOW | M_IDLE_RESET, 0L,
     m_unregistered, m_permission, m_rnotice, m_rnotice, m_rnotice}
};

static char *token = TOK1_RNOTICE;

#ifndef STATIC_MODULES

char *_version = "$Revision: 1.2 $";

void _modinit(void)
{
    mod_add_cmd(_msgtab);
    tok1_msgtab[(u_char) *token].msg = _msgtab;
}

void _moddeinit(void)
{
    mod_del_cmd(_msgtab);
    tok1_msgtab[(u_char) *token].msg = NULL;
}
#else
void m_rnotice_init(void)
{
    mod_add_cmd(_msgtab);
    tok1_msgtab[(u_char) *token].msg = _msgtab;
}
#endif

int m_rnotice(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    char *message = parc > 1 ? parv[1] : NULL;

    if (BadPtr(message)) {
	if (MyClient(sptr))
	    send_me_numeric(sptr, ERR_NEEDMOREPARAMS, MSG_RNOTICE);
	return 0;
    }

    if (strlen(message) > TOPICLEN)
	message[TOPICLEN] = '\0';

    sendto_serv_butone(IsServer(cptr) ? cptr : NULL, sptr, TOK1_RNOTICE, ":%s", message);
    sendto_users(UMODE_r, ":%C NOTICE RNCK :*** Rnotice -- from %C: %s", &me, sptr, message);
    return 0;
}
