/*
 *   IRC - Internet Relay Chat, modules/m_topic.c
 *
 *   Copyright (C) 2000-2002 TR-IRCD Development
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

/*
 * $Id: m_topic.c,v 1.2 2003/06/14 13:55:48 tr-ircd Exp $ 
 */

/* Das ist die HaDiNet version, wo für /topic keine Berechtigungen
 * erforderlich sind.
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "channel.h"
#include "msg.h"
#include "chanmode.h"
#include "h.h"

static char *token = TOK1_TOPIC;

static struct Message _msgtab[] = {
    {MSG_TOPIC, 0, MAXPARA, M_SLOW, 0L,
     m_unregistered, m_topic, m_topic, s_topic, m_topic}
};

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
void m_topic_init(void)
{
    mod_add_cmd(_msgtab);
    tok1_msgtab[(u_char) *token].msg = _msgtab;
}
#endif

/*
 * m_topic 
 * parv[0] = sender prefix 
 * parv[1] = topic text
 */
int m_topic(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    aChannel *chptr = NULL;
    char *topic = NULL, *name, *tnick = sptr->name;
    time_t ts = timeofday;
    int member;

    if (parc < 2) {
	send_me_numeric(sptr, ERR_NEEDMOREPARAMS, MSG_TOPIC);
	return 0;
    }
    name = parv[1];
    chptr = find_channel(name);
    if (!chptr) {
	send_me_numeric(sptr, ERR_NOSUCHCHANNEL, name);
	return 0;
    }
    member = IsMember(sptr, chptr);
    if (parc == 2) {		/* user is requesting a topic */
    char *namep = chptr->chname;
    char tempchname[CHANNELLEN + 2];

	if (!member && !(ShowChannel(sptr, chptr))) {
	    if (IsAdmin(sptr)) {
		tempchname[0] = '%';
		strcpy(&tempchname[1], chptr->chname);
		namep = tempchname;
	    } else {
		send_me_numeric(sptr, ERR_NOTONCHANNEL, name);
		return 0;
	    }
	}

	if (chptr->topic[0] == '\0')
	    send_me_numeric(sptr, RPL_NOTOPIC, namep);
	else {
	    send_me_numeric(sptr, RPL_TOPIC, namep, chptr->topic);
	    send_me_numeric(sptr, RPL_TOPICWHOTIME, namep,
			    IsChanHideOps(chptr) ? chptr->chname : chptr->
			    topic_nick, chptr->topic_time);
	}
	return 0;
    }

    if (!member && !IsServer(sptr) && !IsULine(sptr)) {
	send_me_numeric(sptr, ERR_NOTONCHANNEL, name);
	return 0;
    }

    topic = parv[2];

    strlcpy_irc(chptr->topic, topic, TOPICLEN);
    strcpy(chptr->topic_nick, tnick);
    chptr->topic_time = ts;

    /*
     * in this case I think it's better that we send all the info that df 
     * sends with the topic, so I changed everything to work like that. -wd 
     */

    sendto_match_servs(chptr, cptr, TOK1_TOPIC, "%s %lu :%s",
		       chptr->topic_nick, chptr->topic_time, chptr->topic);

    sendto_channel_butserv(chptr, sptr, TOK1_TOPIC, CHFL_GETSETTER, ":%s", chptr->topic);

    sendto_service(SERVICE_SEE_TOPIC, 0, sptr, chptr, TOK1_TOPIC,
		   "%s %lu :%s", chptr->topic_nick, chptr->topic_time, chptr->topic);
    return 0;
}

/*
 * s_topic 
 * parv[0] = sender prefix 
 * parv[1] = channel
 * parv[2] = topic setter
 * parv[3] = topic time
 * parv[4] = topic text
 */
int s_topic(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    aChannel *chptr = NULL;

    char *topic = NULL, *tnick = NULL;
    time_t ts = timeofday;

    if (parc < 4)
	return 0;

    chptr = find_channel(parv[1]);
    if (!chptr)
	return 0;

    tnick = parv[2];
    ts = atoi(parv[3]);
    topic = (parc > 4 ? parv[4] : "");

    /*
     * local topic is newer than remote topic and we have a topic
     * and we're in a synch (server setting topic) 
     */

    if (!IsULine(sptr) && chptr->topic_time >= ts && chptr->topic[0])
	return 0;

    strlcpy_irc(chptr->topic, topic, TOPICLEN);
    strcpy(chptr->topic_nick, tnick);
    chptr->topic_time = ts;

    sendto_match_servs(chptr, cptr, TOK1_TOPIC, "%s %lu :%s",
		       chptr->topic_nick, chptr->topic_time, chptr->topic);

    sendto_channel_butserv(chptr, sptr, TOK1_TOPIC, CHFL_GETSETTER, ":%s", chptr->topic);

    sendto_service(SERVICE_SEE_TOPIC, 0, sptr, chptr, TOK1_TOPIC,
		   "%s %lu :%s", chptr->topic_nick, chptr->topic_time, chptr->topic);

    return 0;
}
