/************************************************************************
 *   IRC - Internet Relay Chat, server/s_lev.c
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
#include "s_conf.h"
#include "h.h"

typedef struct {
    char *lev_chname;
    struct Channel *levptr;
    char *topic;
} LChan;

static LChan lchans[NUM_LEV] = {
    {N_NOTICE, 	NULL, T_NOTICE_LEV},
    {N_CONNECT, NULL, T_CCONN_LEV},
    {N_REJECT, 	NULL, T_REJ_LEV},
    {N_KILL, 	NULL, T_SKILL_LEV},
    {N_STAT, 	NULL, T_SPY_LEV},
    {N_DEBUG, 	NULL, T_DEBUG_LEV},
    {N_FLOOD, 	NULL, T_FLOOD_LEV},
    {N_SPAMBOT, NULL, T_SPAM_LEV},
    {N_DCCSEND, NULL, T_DCCSEND_LEV},
    {N_SERVICE, NULL, T_SERVICE_LEV},
    {N_SNOTICE, NULL, T_SNOTICE_LEV},
    {N_PROXY, 	NULL, T_PROXY_LEV},
    {N_KLINE,	NULL, T_KLINE_LEV},
    {N_QLINE,   NULL, T_QLINE_LEV},
    {N_ULINE, 	NULL, NULL},
    {NULL, 	NULL, NULL}
};

#define MODE_SERVERCHANNEL \
	((!ChannelConf.visschan) \
	? \
	(MODE_TOPICLIMIT|MODE_NOPRIVMSGS|MODE_ANONYMOUS|MODE_OPERONLY|MODE_SECRET|MODE_PRIVATE) \
	: \
	(MODE_TOPICLIMIT|MODE_NOPRIVMSGS|MODE_ANONYMOUS|MODE_OPERONLY) \
	)

void init_levs(aClient *ghost)
{
    int n;
    int i;
    LChan *alptr;

    SetOper(ghost);

    ghost->protoflags |= PFLAGS_ANONYMOUS;

    i = 0;

	/* NOTICES */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    alptr->levptr->mode.mode &= ~MODE_OPERONLY;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

	/* CONNECTS */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

	/* REJECTS */ 

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

	/* KILLS */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;
	
	/* STATS */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

	/* DEBUG */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

	/* FLOOD */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

	/* SPAMBOT */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;
	
	/* DCCSEND */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

	/* SERVICE */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

	/* SNOTICE */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

	/* PROXY */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

        /* KLINES */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

        /* QLINES */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
    strcpy(alptr->levptr->topic, alptr->topic);
    i++;

	/* ULINE */

    alptr = lchans + i;
    alptr->levptr = create_channel(ghost, alptr->lev_chname, &n, 0);
    add_user_to_channel(alptr->levptr, ghost, CHFL_CHANOP);
    alptr->levptr->mode.mode |= MODE_SERVERCHANNEL;
}

void sendto_lev(int lev, char *pattern, ...)
{

    aChannel *chptr = NULL;
    LChan *alptr;
    char nbuf[512];
    va_list vl;

    alptr = lchans + (lev);

    if ((chptr = alptr->levptr)) {
	va_start(vl, pattern);
	ircvsnprintf(nbuf, 511, pattern, vl);
	va_end(vl);
	sendto_channel_butserv(chptr, &me, TOK1_NOTICE, 0, ":%s", nbuf);
    }
    return;
}
