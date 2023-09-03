/************************************************************************
 * IRC - Internet Relay Chat, contrib/m_getlang.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "struct.h"
#include "common.h"
#include "numeric.h"
#include "sys.h"
#include "msg.h"
#include "h.h"
#include <fcntl.h>
#include "confitem.h"
#include "language.h"

#define MSG_GETLANG	"GETLANG"

int m_getlang(aClient *, aClient *, int, char **);

static struct Message _msgtab[] = {
    {MSG_GETLANG, 0, MAXPARA, M_SLOW, 0L,
     m_unregistered, m_getlang, m_getlang, m_ignore, m_ignore}
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
void m_setlang_init(void)
{
    mod_add_cmd(_msgtab);
}
#endif

int m_getlang(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    int l = 0;

    if (parc < 2) {
	send_me_numeric(sptr, ERR_NEEDMOREPARAMS, MSG_GETLANG);
	return 0;
    }

    l = lang_parse(parv[1]);
    if (l) 
	send_me_notice(sptr, ":Language ID for %s is %d", parv[1], l);

    return 0;
}

