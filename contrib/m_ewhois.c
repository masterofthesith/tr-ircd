/************************************************************************
 *   IRC - Internet Relay Chat, modules/m_ewhois.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers.
 *
 *   This program is free softwmare; you can redistribute it and/or modify
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
#include "listener.h"
#include "s_conf.h"
#include "usermode.h"
#include "chanmode.h"

static char buf[BUFSIZE];

static char *token = TOK1_EWHOIS;

static struct Message _msgtab[] = {
    {MSG_EWHOIS, 0, MAXPARA, M_SLOW, 0L,
     m_unregistered, m_permission, m_ewhois, m_ewhois, m_ignore}
};

#ifndef STATIC_MODULES

char *_version = "$Revision: 1.3 $";

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
void m_whois_init(void)
{
    mod_add_cmd(_msgtab);
    tok1_msgtab[(u_char) *token].msg = _msgtab;
}
#endif

int m_ewhois(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    char *name, *cp;
    dlink_node *lp;
    aClient *acptr;
    aChannel *chptr;
    aWatch *awptr;
    char ubuf[32];
    int len, mlen;

    if (parc < 2) {
	send_me_numeric(sptr, ERR_NONICKNAMEGIVEN);
	return 0;
    }

    if (!IsAdmin(sptr))
	return m_permission(sptr, cptr, parc, parv);

    if (parc > 2) {
	if (hunt_server(cptr, sptr, ":%s %s %s :%s", TOK1_EWHOIS, 1, parc, parv)
	    != HUNTED_ISME)
	    return 0;
	parv[1] = parv[2];
    }

    acptr = find_client(parv[1]);

    if (!acptr || !IsPerson(acptr)) {
	send_me_numeric(sptr, ERR_NOSUCHNICK, parv[1]);
	return 0;
    }

    name = acptr->name;

    if (IsAnOper(acptr))
	send_me_notice(acptr, ":%^C did a /ewhois on you ", sptr);

    send_me_numeric(sptr, RPL_EWHOISUSER, acptr->name, acptr->user->username, acptr->user->host);

    send_me_numeric(sptr, RPL_EWHOISFHOST, acptr->user->fakehost);

    send_me_numeric(sptr, RPL_EWHOISIP,
		    (IsClientIpv6(acptr) ? acptr->hostip : inetntoa((char *) &IN_ADDR(acptr->ip))));

    send_me_numeric(sptr, RPL_EWHOISGCOS, acptr->info);

    send_umode(NULL, acptr, 0, USER_UMODES, ubuf);
    send_me_numeric(sptr, RPL_EWHOISMODES, ubuf);

    send_me_numeric(sptr, RPL_EWHOISSERVER, acptr->user->server);

    if (IsOperFakehost(acptr))
	send_me_numeric(sptr, RPL_EWHOISACTUALLY, 
			acptr->user->real_oper_host, acptr->user->real_oper_ip);

    mlen = strlen(me.name) + strlen(parv[0]) + 6 + strlen(name);
    for (len = 0, *buf = '\0', lp = acptr->user->channel.head; lp; lp = lp->next) {
	chptr = lp->data;
	if (len + strlen(chptr->chname) > (size_t) BUFSIZE - 4 - mlen) {
	    send_me_numeric(sptr, RPL_EWHOISCHANNELS, buf);
	    *buf = '\0';
	    len = 0;
	}
	if (len)
	    *(buf + len) = '\0';
	strcpy(buf + len, chptr->chname);
	len += strlen(chptr->chname);
	strcat(buf + len, " ");
	len++;
    }
    if (buf[0] != '\0')
	send_me_numeric(sptr, RPL_EWHOISCHANNELS, buf);

    if (IsRegNick(acptr))
	send_me_numeric(sptr, RPL_EWHOISREGNICK);

    if (IsZombie(acptr))
	send_me_numeric(sptr, RPL_EWHOISZOMBIE);

    if (acptr->user->away)
	send_me_numeric(sptr, RPL_EWHOISAWAY, acptr->user->away);

#ifdef HAVE_ENCRYPTION_ON
    if (IsSSL(acptr))
	send_me_numeric(sptr, RPL_EWHOISSSL);
#endif

    if (IsAnOper(acptr))
	send_me_numeric(sptr, RPL_EWHOISOPERATOR, "an IRC Operator");
    if (IsHelpop(acptr))
	send_me_numeric(sptr, RPL_EWHOISOPERATOR, "an #help Operator");
    if (IsAdmin(acptr))
	send_me_numeric(sptr, RPL_EWHOISOPERATOR, "a Server Administrator");
    if (IsSAdmin(acptr))
	send_me_numeric(sptr, RPL_EWHOISOPERATOR, "a Services Administrator");

    if (MyConnect(acptr))
	send_me_numeric(sptr, RPL_EWHOISIDLE, timeofday - acptr->user->last, smalldate(acptr->firsttime));

    send_me_numeric(sptr, RPL_EWHOISMEMORY, acptr);
    if (acptr->prev)
	send_me_numeric(sptr, RPL_EWHOISPREV, acptr->prev,
			acptr->prev->name ? acptr->prev->name : "NO-NAME");
    if (acptr->next)
	send_me_numeric(sptr, RPL_EWHOISNEXT, acptr->next,
			acptr->next->name ? acptr->next->name : "NO-NAME");

    if (MyConnect(acptr)) {
	send_me_numeric(sptr, RPL_EWHOISCPORT, acptr->connectport);
	send_me_numeric(sptr, RPL_EWHOISLPORT, acptr->listener->port);
	awptr = hash_get_watch(acptr->name);
	if (awptr) {
	    len = 0;
	    buf[0] = '\0';
	    mlen = strlen(me.name) + 5 + strlen(sptr->name);
	    for (lp = awptr->watched_by.head; lp; lp = lp->next) {
		acptr = lp->data;
		if (len + strlen(acptr->name) > (size_t) BUFSIZE - 4 - mlen) {
		    send_me_numeric(sptr, RPL_EWHOISWATCHED, buf);
		    *buf = '\0';
		    len = 0;
		}
		if (len)
		    *(buf + len) = '\0';
		strcpy(buf + len, acptr->name);
		len += strlen(acptr->name);
		strcat(buf + len, " ");
		len++;
	    }

	    if (buf[0] != '\0')
		send_me_numeric(sptr, RPL_EWHOISWATCHED, buf);
	}
	if (acptr->watchlist.head) {
            len = 0;
            buf[0] = '\0';
            mlen = strlen(me.name) + 5 + strlen(sptr->name);
            for (lp = acptr->watchlist.head; lp; lp = lp->next) {
                awptr = lp->data;
                if (len + strlen(awptr->watchnick) > (size_t) BUFSIZE - 4 - mlen) {
                    send_me_numeric(sptr, RPL_EWHOISWATCHES, buf);
                    *buf = '\0';
                    len = 0;
                }
                if (len)
                    *(buf + len) = '\0';
                strcpy(buf + len, awptr->watchnick);
                len += strlen(awptr->watchnick);
                strcat(buf + len, " ");
                len++;
            }

            if (buf[0] != '\0')
                send_me_numeric(sptr, RPL_EWHOISWATCHES, buf);
	}
	if (acptr->user->silence.head) {
            len = 0;
            buf[0] = '\0';
            mlen = strlen(me.name) + 5 + strlen(sptr->name);
            for (lp = acptr->user->silence.head; lp; lp = lp->next) { 
                cp = lp->data;
                if (len + strlen(cp) > (size_t) BUFSIZE - 4 - mlen) {
                    send_me_numeric(sptr, RPL_EWHOISSILENCES, buf);
                    *buf = '\0';
                    len = 0;
                }
                if (len)
                    *(buf + len) = '\0';
                strcpy(buf + len, cp);
                len += strlen(cp);
                strcat(buf + len, " ");
                len++;
            }

            if (buf[0] != '\0')
                send_me_numeric(sptr, RPL_EWHOISSILENCES, buf);
	}
	send_me_numeric(sptr, RPL_EWHOISCLASS, get_client_class(acptr));
    }

    send_me_numeric(sptr, RPL_ENDOFWHOIS, parv[1]);
    return 0;
}
