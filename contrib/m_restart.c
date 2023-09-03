/*
 *   IRC - Internet Relay Chat, modules/m_restart.c
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
#include "s_conf.h"
#include "event.h"

static char buf[BUFSIZE];

static struct Message _msgtab[] = {
    {MSG_RESTART, 0, MAXPARA, M_SLOW, 0L,
     m_unregistered, m_permission, m_restart, m_ignore, m_ignore}
};

static int log_restart = -1;

#ifndef STATIC_MODULES

char *_version = "$Revision: 1.1 $";

void _modinit(void)
{
    mod_add_cmd(_msgtab);
    log_restart = logevent_register("restart", LOG_ALWAYS, LOG_LOGFILE | LOG_IRCLOG, LOG_NOTICE,
                                    "RESTART From %s\n");  
}

void _moddeinit(void)
{
    mod_del_cmd(_msgtab);
    logevent_unregister(log_restart);
}
#else
void m_restart_init(void)
{
    mod_add_cmd(_msgtab);
    log_restart = logevent_register("restart", LOG_ALWAYS, LOG_LOGFILE | LOG_IRCLOG, LOG_NOTICE,
                                    "RESTART From %s\n"); 
}
#endif

static void restart_delayed(void *buffer)
{
    restart((char *)buffer);
}

int m_restart(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    char *pass = ServerInfo.restartpass;
    char passarr[PASSWDLEN];
    int delay = 0;

    if (!OPCanRestart(sptr)) {
	send_me_numeric(sptr, ERR_NOPRIVILEGES);
	return 0;
    }
    /*
     * m_restart is now password protected as in df465 only change --
     * this one doesn't allow a reason to be specified. future changes:
     * crypt()ing of password, reason to be re-added -mjs
     */
    if (pass) {
	if (parc < 2) {
	    send_me_numeric(sptr, ERR_NEEDMOREPARAMS, MSG_RESTART);
	    return 0;
	}
	if (strcmp(pass, calcpass(parv[1], passarr))) {
	    send_me_numeric(sptr, ERR_PASSWDMISMATCH);
	    return 0;
	}
    }
    if (parc > 2)
	delay = atoi(parv[2]);
    if (delay <= 0)
	delay = 0;

    logevent_call(log_restart, get_client_name(sptr, FALSE));

    if (delay) {
	ircsprintf(buf, "Server RESTART by %~C in %d seconds", sptr, delay);
	sendto_all("%s REST :%s", MSG_NOTICE, buf);
    } else {
	ircsprintf(buf, "Immediate server RESTART by %~C", sptr);
	sendto_all("%s REST :%s", MSG_NOTICE, buf);
    }
    eventAdd("RESTART", restart_delayed, buf, delay);
    return 0;
}
