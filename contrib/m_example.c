/************************************************************************
 *   IRC - Internet Relay Chat, modules/m_example.c
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

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "h.h"
#include "s_conf.h"

#define MSG_EXAMPLE	"TEST"

int m_example(aClient *cptr, aClient *sptr, int parc, char *parv[]);

static struct Message _msgtab[] = {
    {MSG_TEST, 0, MAXPARA, M_SLOW | M_IDLE_RESET, 0L,
     m_unregistered, m_permission, m_test, m_test, m_ignore}
};

/* m_unregistered is a function, telling the source that they have
 * not registered via NICK/USER or SERVER or SERvICE so they are not
 * allowed to use the command.
 *
 * m_permission tells the source that they do not have the permission 
 * to issue the command.
 *
 * m_ignore does not do anything, just drop the message.
 *
 * in the struct message, the fields are the following:
 * COMMAND TEXT,
 * count
 * MAX number of parameters
 * If the message is high priority (M_SLOW) and if the message sets the idle time
 * memory usage of the message
 * function in unregistered state
 * function for a user
 * function for an oper
 * function for a server
 * function for a service
 */

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
void m_example_init(void)
{
    mod_add_cmd(_msgtab);
}
#endif

int m_example(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    send_me_notice(sptr, ":Hello World");
    return 0;
}
