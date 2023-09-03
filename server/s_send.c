/***********************************************************************
 *   IRC - Internet Relay Chat, server/s_send.c
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

/*
 * $Id: s_send.c,v 1.10 2005/06/19 09:52:46 tr-ircd Exp $ 
 */

#include "struct.h"
#include "channel.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "numeric.h"
#include "language.h"
#ifdef STATIC_MODULES
#define TOKEN 1
#include "msg.h"
#undef TOKEN
#else
#include "msg.h"
#endif
#include "supported.h"
#include "s_conf.h"

#define IRC_SB_SIZE		1024
#define IRC_RB_SIZE		4096

static char sendbuf[IRC_SB_SIZE];
static char privbuf[IRC_SB_SIZE];
static char servbuf[IRC_RB_SIZE];
static char remotebuf[IRC_RB_SIZE];

extern struct Token tok1_msgtab[];

static inline void clear_buffer(char *b, int size)
{

    memset(b, 0, size * sizeof(char));

}

void init_send(void)
{

}

/*
 * * send message to single client
 */
void sendto_one(aClient *to, char *pattern, ...)
{
    va_list vl;
    int len;			/* used for the length of the current message */

    va_start(vl, pattern);

    clear_buffer(sendbuf, IRC_SB_SIZE);
    len = ircvsprintf(sendbuf, pattern, vl);

    if (to->from)
	to = to->from;
    send_message(to, sendbuf, len);
    va_end(vl);
}

void vsendto_one(aClient *to, char *pattern, va_list vl)
{
    int len;			/* used for the length of the current message */

    clear_buffer(sendbuf, IRC_SB_SIZE);
    len = ircvsprintf(sendbuf, pattern, vl);

    if (to->from)
	to = to->from;
    send_message(to, sendbuf, len);
}

void send_me_notice(aClient *cptr, char *pattern, ...)
{
    char nbuf[1024];
    int len;
    va_list vl;

    va_start(vl, pattern);

    len = ircsprintf(nbuf, "%c%s %s %s ",
		     (IsIDCapable(cptr->from) ? '!' : ':'),
		     ((MyClient(cptr) && UsesAlias(cptr)) ? ServerInfo.aliasname :
		      (IsIDCapable(cptr->from) ? me.id.string : me.name)),
		     /* We can safely user ServerInfo.aliasname, because it always exists,
		      * when user is PFLAGS_ALIASED -TimeMr14C 
		      * Please note that ServerInfo.aliasname is not rehashable */
		     IsToken1(cptr->from) ? TOK1_NOTICE : MSG_NOTICE, cptr->name);

    strncat(nbuf, pattern, 1024 - len);
    vsendto_one(cptr, nbuf, vl);
    va_end(vl);
    return;
}

void send_me_numeric(aClient *cptr, int numeric, ...)
{
    char nbuf[1024];
    char *pattern = NULL;
    int len;
    va_list vl;

    pattern = get_numeric_format_in_lang(&numeric, get_language(cptr->lang));

    va_start(vl, numeric);

    len = ircsprintf(nbuf, "%c%s %N %s ",
		     (IsIDCapable(cptr->from) ? '!' : ':'),
		     ((MyClient(cptr) && UsesAlias(cptr)) ? ServerInfo.aliasname :
		      (IsIDCapable(cptr->from) ? me.id.string : me.name)), numeric, cptr->name);

    strncat(nbuf, pattern, 1024 - len);
    vsendto_one(cptr, nbuf, vl);
    va_end(vl);
    return;
}

void send_me_debug(aClient *cptr, char *pattern, ...)
{
    char nbuf[1024];
    int len;
    va_list vl;

    va_start(vl, pattern);

    len = ircsprintf(nbuf, "%c%s %N %s ",
		     (IsIDCapable(cptr->from) ? '!' : ':'),
		     ((MyClient(cptr) && UsesAlias(cptr)) ? ServerInfo.aliasname :
		      (IsIDCapable(cptr->from) ? me.id.string : me.name)),
		     RPL_STATSDEBUG, cptr->name);

    strncat(nbuf, pattern, 1024 - len);
    vsendto_one(cptr, nbuf, vl);
    va_end(vl);
    return;
}

void send_me_desynch(aClient *cptr, aChannel *chptr, char *ev, int val)
{
    char nbuf[1024];
    int len;

    len = ircsnprintf(nbuf, 1024, "%c%s %N %s :Desynch by %s in channel %H (0 != %d)",
		      (IsIDCapable(cptr->from) ? '!' : ':'),
		      ((MyClient(cptr) && UsesAlias(cptr)) ? ServerInfo.aliasname :
		       (IsIDCapable(cptr->from) ? me.id.string : me.name)),
		      ERR_DESYNC, cptr->name, ev, chptr, val);

    sendto_one(cptr, nbuf);
    logevent_personal("Desynch from %C by %s in channel %H (0 != %d)", cptr, ev, chptr, val);
}

static inline void vsendto_users(long umode, char *pattern, va_list vl)
{
    aClient *cptr;
    dlink_node *ptr;
    for (ptr = lclient_list.head; ptr; ptr = ptr->next) {
	cptr = ptr->data;
	if (!cptr || (umode && !((cptr->umode & umode) == umode)))
	    continue;
	vsendto_one(cptr, pattern, vl);
    }
}

void sendto_all(char *pattern, ...)
{
    va_list vl;
    va_start(vl, pattern);
    vsendto_users(0, pattern, vl);
    va_end(vl);
    return;
}

void sendto_users(long umode, char *pattern, ...)
{
    va_list vl;
    va_start(vl, pattern);
    vsendto_users(umode, pattern, vl);
    va_end(vl);
    return;
}

static inline void vsendto_operators(long umode, char *type, char *pattern, va_list vl)
{
    aClient *cptr;
    char nbuf[1024];
    dlink_node *ptr;
    for (ptr = locoper_list.head; ptr; ptr = ptr->next) {
	cptr = ptr->data;
	if (!cptr || (umode && !((cptr->umode & umode) == umode)))
	    continue;
	ircsprintf(nbuf, ":%C %s %s :*** %s -- %s", &me, MSG_NOTICE, cptr->name, type, pattern);
	vsendto_one(cptr, nbuf, vl);
    }
}

void sendto_gnotice(char *pattern, ...)
{
    va_list vl;
    va_start(vl, pattern);
    vsendto_operators(UMODE_n, "Routing", pattern, vl);
    va_end(vl);
    return;
}

void sendto_ops(char *pattern, ...)
{
    va_list vl;
    va_start(vl, pattern);
    vsendto_operators(0, "Notice", pattern, vl);
    va_end(vl);
    return;
}

void sendto_operators(long umode, char *type, char *pattern, ...)
{
    va_list vl;
    va_start(vl, pattern);
    vsendto_operators(umode, type, pattern, vl);
    va_end(vl);
    return;
}

static inline char *prepare_server_prefixes(aClient *from, aChannel *chptr,
					    aClient *to, char *token,
					    char *const buffer, int *msglen)
{
    int prefixlen = 0;
    int lastlen = *msglen;
    char *dest = NULL;
    char *command = NULL;
    char *chan = NULL;

    clear_buffer(servbuf, IRC_RB_SIZE);

    if (IsToken1(to))
	command = token;
    else
	command = tok1_msgtab[(u_char) *token].cmd;

    if (from) {
	if (IsIDCapable(to) && HasID(from)) {
	    dest = from->id.string;
	    servbuf[prefixlen++] = '!';
	} else {
	    dest = from->name;
	    servbuf[prefixlen++] = ':';
	}
	while (*dest)
	    servbuf[prefixlen++] = *dest++;
	servbuf[prefixlen++] = ' ';
    }

    while (*command)
	servbuf[prefixlen++] = *command++;

    servbuf[prefixlen++] = ' ';

    if (chptr) {
	chan = chptr->chname;
	while (*chan)
	    servbuf[prefixlen++] = *chan++;
	servbuf[prefixlen++] = ' ';
    }

    if (lastlen) {
	memcpy(&servbuf[prefixlen], buffer, lastlen);
	servbuf[prefixlen + lastlen] = '\0';
    } else {
	if (servbuf[prefixlen - 1] == ' ')
	    prefixlen--;
	servbuf[prefixlen] = '\0';
    }

    *msglen = lastlen + prefixlen;

    return servbuf;
}

static inline char *prepare_server_prefixes_mask(aClient *from, aClient *to, char *token,
						 char *const buffer, int *msglen)
{
    int prefixlen = 0;
    int lastlen = *msglen;
    char *dest = NULL;
    char *command = NULL;

    clear_buffer(servbuf, IRC_RB_SIZE);

    if (IsToken1(to))
	command = token;
    else
	command = tok1_msgtab[(u_char) *token].cmd;

    if (from) {
	if (IsIDCapable(to) && HasID(from)) {
	    dest = from->id.string;
	    servbuf[prefixlen++] = '!';
	} else {
	    dest = from->name;
	    servbuf[prefixlen++] = ':';
	}
	while (*dest)
	    servbuf[prefixlen++] = *dest++;
	servbuf[prefixlen++] = ' ';
    }

    while (*command)
	servbuf[prefixlen++] = *command++;

    servbuf[prefixlen++] = ' ';

    if (lastlen) {
	memcpy(&servbuf[prefixlen], buffer, lastlen);
	servbuf[prefixlen + lastlen] = '\0';
    } else {
	if (servbuf[prefixlen - 1] == ' ')
	    prefixlen--;
	servbuf[prefixlen] = '\0';
    }

    *msglen = lastlen + prefixlen;

    return servbuf;
}

static inline char *prepare_service_prefixes(aClient *from, aChannel *chptr,
					     aService * svc, char *token,
					     char *const buffer, int *msglen)
{
    int prefixlen = 0;
    int lastlen = *msglen;
    char *dest = NULL;
    char *command = tok1_msgtab[(u_char) *token].cmd;
    char *chan = NULL;

    clear_buffer(remotebuf, IRC_RB_SIZE);

    if (from) {
	dest = from->name;
	remotebuf[prefixlen++] = ':';
	while (*dest)
	    remotebuf[prefixlen++] = *dest++;
	if ((svc->enable & SERVICE_SEE_PREFIX) && from->user) {
	    dest = from->user->username;
	    remotebuf[prefixlen++] = '!';
	    while (*dest)
		remotebuf[prefixlen++] = *dest++;
	    remotebuf[prefixlen++] = '@';
	    dest = from->user->host;
	    while (*dest)
		remotebuf[prefixlen++] = *dest++;
	}
	remotebuf[prefixlen++] = ' ';
    }

    while (*command)
	remotebuf[prefixlen++] = *command++;

    remotebuf[prefixlen++] = ' ';

    if (chptr) {
	chan = chptr->chname;
	while (*chan)
	    remotebuf[prefixlen++] = *chan++;
	remotebuf[prefixlen++] = ' ';
    }

    if (lastlen) {
	memcpy(&remotebuf[prefixlen], buffer, lastlen);
	remotebuf[prefixlen + lastlen] = '\0';
    } else {
	if (remotebuf[prefixlen - 1] == ' ')
	    prefixlen--;
	remotebuf[prefixlen] = '\0';
    }

    *msglen = lastlen + prefixlen;

    return remotebuf;
}

static inline char *prepare_prefixes_localremote(aClient *from,
						 aClient *to, char *token,
						 char *const buffer, int *msglen)
{
    int prefixlen = 0;
    int lastlen = *msglen;
    int alocal = MyClient(to);
    char *dest = NULL;
    char *command = NULL;

    clear_buffer(remotebuf, IRC_RB_SIZE);

    if (!alocal && IsServer(to) && IsToken1(to))
	command = token;
    else
	command = tok1_msgtab[(u_char) *token].cmd;

    if (from) {
	if (IsIDCapable(to) && HasID(from)) {
	    dest = from->id.string;
	    remotebuf[prefixlen++] = '!';
	} else {
	    dest = from->name;
	    remotebuf[prefixlen++] = ':';
	}
	while (*dest)
	    remotebuf[prefixlen++] = *dest++;
	if (alocal && from->user) {
	    dest = from->user->username;
	    remotebuf[prefixlen++] = '!';
	    while (*dest)
		remotebuf[prefixlen++] = *dest++;
	    remotebuf[prefixlen++] = '@';
	    dest = IsFake(from) ? from->user->fakehost : from->user->host;
	    while (*dest)
		remotebuf[prefixlen++] = *dest++;
	}
	remotebuf[prefixlen++] = ' ';
    }

    while (*command)
	remotebuf[prefixlen++] = *command++;

    remotebuf[prefixlen++] = ' ';

    if (lastlen) {
	memcpy(&remotebuf[prefixlen], buffer, lastlen);
	remotebuf[prefixlen + lastlen] = '\0';
    } else {
	if (remotebuf[prefixlen - 1] == ' ')
	    prefixlen--;
	remotebuf[prefixlen] = '\0';
    }

    *msglen = lastlen + prefixlen;

    return remotebuf;
}

static inline char *prepare_channel_prefixes_localremote(aClient *from,
							 aChannel *chptr,
							 aClient *to,
							 int hidden, int cflags,
							 char *token, int flag,
							 char *const buffer, int *msglen)
{
    int prefixlen = 0;
    int lastlen = *msglen;
    int alocal = 0;
    int use_token = 0;
    char *dest = NULL;
    char *command = NULL;
    char *chan = chptr->chname;

    clear_buffer(privbuf, IRC_SB_SIZE);

    if (!to) {
	alocal = 1;
	use_token = 0;
    } else {
	alocal = MyClient(to);
	if (!alocal)
	    use_token = IsToken1(to->from);
    }

    if (!alocal && use_token)
	command = token;
    else
	command = tok1_msgtab[(u_char) *token].cmd;

    if (IsMe(from))
	alocal = 0;

    if (from && (!hidden || cflags)) {
	if (to && IsIDCapable(to->from) && HasID(from)) {
	    dest = from->id.string;
	    privbuf[prefixlen++] = '!';
	} else {
	    dest = from->name;
	    privbuf[prefixlen++] = ':';
	}
	while (*dest)
	    privbuf[prefixlen++] = *dest++;
	if (alocal && from->user) {
	    dest = from->user->username;
	    privbuf[prefixlen++] = '!';
	    while (*dest)
		privbuf[prefixlen++] = *dest++;
	    privbuf[prefixlen++] = '@';
	    dest = IsFake(from) ? from->user->fakehost : from->user->host;
	    while (*dest)
		privbuf[prefixlen++] = *dest++;
	}
	privbuf[prefixlen++] = ' ';
    } else if (from && (hidden && !cflags)) {
	privbuf[prefixlen++] = ':';
	dest = chptr->chname;
	while (*dest)
	    privbuf[prefixlen++] = *dest++;
	privbuf[prefixlen++] = ' ';
    }

    while (*command)
	privbuf[prefixlen++] = *command++;

    privbuf[prefixlen++] = ' ';

    if (flag & CHFL_CHANOP)
	privbuf[prefixlen++] = '@';
    if (flag & CHFL_VOICE)
	privbuf[prefixlen++] = '+';
    if (flag & CHFL_HALFOP)
	privbuf[prefixlen++] = '%';

    while (*chan)
	privbuf[prefixlen++] = *chan++;
    privbuf[prefixlen++] = ' ';

    if (lastlen) {
	memcpy(&privbuf[prefixlen], buffer, lastlen);
	privbuf[prefixlen + lastlen] = '\0';
    } else {
	if (privbuf[prefixlen - 1] == ' ')
	    prefixlen--;
	privbuf[prefixlen] = '\0';
    }

    *msglen = lastlen + prefixlen;

    return privbuf;
}

static inline char *prepare_channel_prefixes_local_short(aClient *from,
							 aChannel *chptr, char *token, int *msglen)
{
    int prefixlen = 0;
    char *dest = NULL;
    char *command = NULL;
    char *chan = chptr->chname;

    clear_buffer(privbuf, IRC_SB_SIZE);

    command = tok1_msgtab[(u_char) *token].cmd;

    if (from) {
	dest = from->name;
	privbuf[prefixlen++] = ':';

	while (*dest)
	    privbuf[prefixlen++] = *dest++;
	if (from->user) {
	    dest = from->user->username;
	    privbuf[prefixlen++] = '!';
	    while (*dest)
		privbuf[prefixlen++] = *dest++;
	    privbuf[prefixlen++] = '@';
	    dest = IsFake(from) ? from->user->fakehost : from->user->host;
	    while (*dest)
		privbuf[prefixlen++] = *dest++;
	}
	privbuf[prefixlen++] = ' ';
    }

    while (*command)
	privbuf[prefixlen++] = *command++;

    privbuf[prefixlen++] = ' ';

    if (*chan)
	privbuf[prefixlen++] = ':';

    /* The above issue is only required by JOIN, since the channel name is prepended
     * with a semicolon.
     * -TimeMr14C, 6/8/2002
     */

    while (*chan)
	privbuf[prefixlen++] = *chan++;
    privbuf[prefixlen++] = ' ';

    if (privbuf[prefixlen - 1] == ' ')
	prefixlen--;
    privbuf[prefixlen] = '\0';

    *msglen = prefixlen;

    return privbuf;
}

static void __new_vsendto_list_butone(dlink_list * listp,
				      aClient *one, aClient *from,
				      int incflags, int exflags,
				      aChannel *chptr, char *token, char *pattern, va_list vl)
{
    int len;
    int t;
    char *txt;
    dlink_node *ptr, *next_ptr;
    aClient *cptr;

    for (ptr = listp->head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	cptr = ptr->data;
	if (!cptr || (cptr == one)
	    || (incflags && !((cptr->capabilities & incflags) == incflags))
	    || (exflags && (cptr->capabilities & exflags) == exflags))
	    continue;
	clear_buffer(sendbuf, IRC_SB_SIZE);
	t = len = ircvsprintf(sendbuf, pattern, vl);
	txt = prepare_server_prefixes(from, chptr, cptr, token, sendbuf, &len);
	send_message(cptr, txt, len);
    }
}

/*
 * sendto_flag_serv_butone
 *
 * Send a message to all connected servers except the client 'one'.
 * Only if they include INCLUDE and exclude EXCLUDE
 */
void sendto_flag_serv_butone(aClient *one, int include, int exclude,
			     aClient *from, char *token, char *pattern, ...)
{
    va_list vl;

    va_start(vl, pattern);
    __new_vsendto_list_butone(&serv_list, one, from, include, exclude, NULL, token, pattern, vl);
    va_end(vl);
    return;
}

void sendto_serv_butone(aClient *one, aClient *from, char *token, char *pattern, ...)
{
    va_list vl;

    va_start(vl, pattern);
    __new_vsendto_list_butone(&serv_list, one, from, 0, 0, NULL, token, pattern, vl);
    va_end(vl);
    return;
}

void sendto_match_servs(aChannel *chptr, aClient *from, char *token, char *pattern, ...)
{
    va_list vl;

    if (chptr)
	if (*chptr->chname == '&')
	    return;

    va_start(vl, pattern);
    __new_vsendto_list_butone(&serv_list, from->from, from, 0, 0, chptr, token, pattern, vl);
    va_end(vl);
    return;
}

void sendto_one_server(aClient *to, aClient *from, char *token, char *pattern, ...)
{
    va_list vl;
    int len;
    char *txt;

    va_start(vl, pattern);
    clear_buffer(sendbuf, IRC_SB_SIZE);
    len = ircvsprintf(sendbuf, pattern, vl);

    if (to->from)
	to = to->from;
    txt = prepare_server_prefixes(from, NULL, to, token, sendbuf, &len);
    send_message(to, txt, len);
    va_end(vl);
}

void sendto_one_person(aClient *to, aClient *from, char *token, char *pattern, ...)
{
    va_list vl;
    int len;			/* used for the length of the current message */
    char *txt;

    va_start(vl, pattern);
    clear_buffer(sendbuf, IRC_SB_SIZE);
    len = ircvsprintf(sendbuf, pattern, vl);

    if (to->from)
	to = to->from;
    txt = prepare_prefixes_localremote(from, to, token, sendbuf, &len);

    send_message(to, txt, len);
    va_end(vl);
}

static int match_it(aClient *one, char *mask, int what)
{
    if (what == MATCH_HOST)
	return (match(mask, one->user->host) == 0);
    else
	return (match(mask, one->user->server) == 0);
}

void sendto_match_butone(aClient *one, char *mask, int what, aClient *from, char *pattern, ...)
{
    aClient *cptr;
    dlink_node *ptr, *next_ptr;
    va_list vl;
    int len, t;
    char *txt;

    va_start(vl, pattern);

    for (ptr = lclient_list.head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	cptr = ptr->data;
	if (!cptr)
	    continue;
	if (cptr == one)	/* must skip the origin !! */
	    continue;
	if (!(IsPerson(cptr) && match_it(cptr, mask, what)))
	    continue;
	clear_buffer(sendbuf, IRC_SB_SIZE);
	len = ircvsprintf(sendbuf, pattern, vl);
	t = len;
	txt = prepare_prefixes_localremote(from, cptr, TOK1_NOTICE, sendbuf, &len);
	send_message(cptr, txt, len);
    }
    for (ptr = serv_list.head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	cptr = ptr->data;
	if (cptr == NULL)
	    continue;
	if (cptr == one)
	    continue;
	clear_buffer(sendbuf, IRC_SB_SIZE);
	len = ircvsprintf(sendbuf, pattern, vl);
	t = len;
	txt = prepare_server_prefixes_mask(from, cptr, TOK1_NOTICE, sendbuf, &len);
	send_message(cptr, txt, len);
    }
    va_end(vl);
    return;
}

void sendto_service(int include, int exclude, aClient *from,
		    aChannel *chptr, char *token, char *pattern, ...)
{
    aService *svc, *next_svc;
    va_list vl;
    int msglen = 0;
    int t;
    char *txt;

    va_start(vl, pattern);

    for (svc = firstservice; svc; svc = next_svc) {
	next_svc = svc->next;
	if ((svc->scptr == NULL) || (!MyConnect(svc->scptr)) ||
	    (include && !((svc->enable & include) == include)) ||
	    (exclude && (svc->enable & exclude) == exclude))
	    continue;
	clear_buffer(sendbuf, IRC_SB_SIZE);
	t = msglen = ircvsprintf(sendbuf, pattern, vl);
	txt = prepare_service_prefixes(from, chptr, svc, token, sendbuf, &msglen);
	send_message(svc->scptr, txt, msglen);
    }
    va_end(vl);
    return;
}

static inline int check_fake_direction(aClient *from, aClient *to)
{
    if (!MyClient(from) && IsPerson(to) && (to->from == from->from)) {
	if (IsServer(from)) {
	    sendto_lev(SNOTICE_LEV, "Message to %s[%s] dropped from %s (Fake Direction)",
		       to->name, to->from->name, from->name);
	    return -1;
	}

	sendto_ops("Ghosted: %C from %C (%C)", to, from, to->from);
	sendto_serv_butone(NULL, &me, TOK1_KILL, "%~C :Ghosted %s", to, to->from->name);

	to->flags |= FLAGS_KILLED;
	exit_client(to, &me, "Ghosted client");

	return -1;
    }

    return 0;
}

void sendto_channel_butone(aClient *one, int incflags, int exflags, aClient *from,
			   aChannel *chptr, char *token, char *pattern, ...)
{
    struct ChanMember *cm;
    aClient *acptr;
    int msglen = 0;
    int t;
    va_list vl;
    char *txt;
    dlink_node *ptr, *next_ptr;
    int i;

    unsigned long ssentalong[MAXCONNECTIONS];
    memset(ssentalong, 0, MAXCONNECTIONS * sizeof(unsigned long));

    va_start(vl, pattern);

    for (ptr = chptr->members.head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	cm = ptr->data;
	if (!cm)
	    continue;
	acptr = cm->client_p;
	if ((acptr->from == one) || IsAnon(acptr)
	    || ((cm->flags & incflags) != incflags))
	    continue;		/* skip the ONE and those who do not have these flags */

	i = acptr->fd;

	if (check_fake_direction(from, acptr))
	    continue;
	clear_buffer(sendbuf, IRC_SB_SIZE);
	t = msglen = ircvsprintf(sendbuf, pattern, vl);
	if (MyClient(acptr)) {
	    txt = prepare_channel_prefixes_localremote(from, chptr, acptr, 0, 0,
						       token, incflags, sendbuf, &msglen);
	    send_message(acptr, txt, msglen);
	    ssentalong[i] = 1;
	} else {
	    if (!acptr->from)
		continue;
	    i = acptr->from->fd;
	    if (ssentalong[i] != 1) {
		txt = prepare_channel_prefixes_localremote(from, chptr, acptr, 0, 0,
							   token, incflags, sendbuf, &msglen);
		send_message(acptr->from, txt, msglen);
		ssentalong[i] = 1;
	    }
	}

    }

    va_end(vl);
    return;
}

/*
 * sendto_common_channels()
 * 
 * Sends a message to all people (inclusing user) on local server who are
 * in same channel with user.
 */
void sendto_common_channels(aClient *from, char *pattern, ...)
{
    struct ChanMember *cm;
    aChannel *chptr;
    aClient *cptr;
    va_list vl;
    int msglen = 0;
    int i;
    dlink_node *chn, *next_chn;
    dlink_node *usr, *next_usr;

    unsigned long ssentalong[MAXCONNECTIONS];
    memset(ssentalong, 0, MAXCONNECTIONS * sizeof(unsigned long));

    if (from->fd > 0)
	ssentalong[from->fd] = 1;

    va_start(vl, pattern);

    if (from->user) {
	for (chn = from->user->channel.head; chn; chn = next_chn) {
	    next_chn = chn->next;
	    chptr = chn->data;

	    if (IsChanAnon(chptr))
		continue;

	    for (usr = chptr->members.head; usr; usr = next_usr) {
		next_usr = usr->next;
		cm = usr->data;
		if (!cm)
		    continue;
		cptr = cm->client_p;
		i = cptr->fd;

		if (!MyConnect(cptr) || IsAnon(cptr) || (from == cptr) ||
		    ssentalong[i] == 1)
		    continue;
		if (check_fake_direction(from, cptr))
		    continue;
		clear_buffer(sendbuf, IRC_SB_SIZE);
		msglen = ircvsprintf(sendbuf, pattern, vl);
		send_message(cptr, sendbuf, msglen);
		ssentalong[i] = 1;
	    }
	}
    }

    if (MyConnect(from)) {
	clear_buffer(sendbuf, IRC_SB_SIZE);
	msglen = ircvsprintf(sendbuf, pattern, vl);
	send_message(from, sendbuf, msglen);
    }
    va_end(vl);
    return;
}

/*
 * send_quit_to_common_channels()
 * 
 * Sends a message to all people (inclusing user) on local server who are
 * in same channel with user if the user can send to this channel.
 */

void send_quit_to_common_channels(aClient *from, char *reason)
{
    struct ChanMember *cm;
    dlink_node *channels, *next_chan;
    dlink_node *users, *next_user;
    aChannel *chptr;
    aClient *cptr;
    int msglen;
    int i, j;
    int cs = 0;

    int sentalong[MAXCONNECTIONS];

    for (j = 0; j < MAXCONNECTIONS; j++)
    	sentalong[j] = 0;
    // memset(sentalong, 0, MAXCONNECTIONS * sizeof(int));

    if (from->fd > 0)
	sentalong[from->fd] = 1;

    /* This loop is required, since buffers get replaced everytime when
     * we enter this function within this function, because exit_client is
     * called. After exit_client normally a client would be marked as closing,
     * that flag signals us that we have to restart the whole function, since
     * all of the used pointers are now dumb.
     * -TimeMr14C 09.04.2003
     */

  closeloop:
    for (channels = from->user->channel.head; channels; channels = next_chan) {
	next_chan = channels->next;
	chptr = channels->data;
	cs = can_send(from, chptr, reason);
	if (IsChanAnon(chptr)) {
		;
	} else {
	    for (users = chptr->members.head; users; users = next_user) {
		next_user = users->next;
		cm = users->data;
		if (!cm)
		    continue;
		cptr = cm->client_p;
		i = cptr->fd;
		if (!MyConnect(cptr) || IsAnon(cptr) || (cptr == from) ||
		    sentalong[i] == 1)
		    continue;
		if (check_fake_direction(from, cptr))
		    continue;
		clear_buffer(sendbuf, IRC_SB_SIZE);
		if (cs == 0)
			msglen = ircsprintf(sendbuf, ":%C %s :%s", from, MSG_QUIT, reason);
		else
			msglen = ircsprintf(sendbuf, ":%C %s %H", from, MSG_PART, chptr);
		send_message(cptr, sendbuf, msglen);
		sentalong[i] = 1;
		if (IsClosing(cptr))
		    goto closeloop;
	    }
	}
	remove_user_from_channel(from, chptr);
    }
    return;
}

/*
 * sendto_channel_butserv
 * 
 * Send a message to all members of a channel that are connected to this
 * server.
 */
void sendto_channel_butserv(aChannel *chptr, aClient *from, char *token,
			    int flags, char *pattern, ...)
{
    struct ChanMember *cm;
    char *txt;
    aClient *acptr;
    va_list vl;
    int msglen = 0;
    int t = 0;
    int noanon = 0;
    dlink_node *ptr, *next_ptr;

    va_start(vl, pattern);
    clear_buffer(sendbuf, IRC_SB_SIZE);
    msglen = ircvsprintf(sendbuf, pattern, vl);

    txt = prepare_channel_prefixes_localremote(from, chptr, NULL, 0, 0, token, 0, sendbuf, &msglen);

    t = msglen;
    if (MyClient(from)) {
	send_message(from, txt, msglen);
	if (IsChanAnon(chptr) && IsLocalChan(chptr->chname))
	    return;
    }

    if (IsChanAnon(chptr) && IsGlobalChan(chptr->chname))
	return;

    noanon = (flags & CHFL_RESANON);

    for (ptr = chptr->members.head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	cm = ptr->data;
	msglen = t;
	if (!cm)
	    continue;
	acptr = cm->client_p;
	if (!MyConnect(acptr) || (acptr == from) || IsAnon(acptr)) {
	    continue;
	}

	if (check_fake_direction(from, acptr))
	    continue;
	clear_buffer(sendbuf, IRC_SB_SIZE);
	msglen = ircvsprintf(sendbuf, pattern, vl);
	txt = prepare_channel_prefixes_localremote(from, chptr, NULL,
						   (noanon ? 0 : IsChanHideOps(chptr)),
						   (noanon ? 0 : (flags & cm->flags)),
						   token, 0, sendbuf, &msglen);
	send_message(acptr, txt, msglen);

    }
    va_end(vl);
    return;
}

void sendto_channel_butserv_short(aChannel *chptr, aClient *from, char *token)
{
    struct ChanMember *cm;
    char *txt;
    aClient *acptr;
    int msglen = 0;
    dlink_node *ptr, *next_ptr;

    txt = prepare_channel_prefixes_local_short(from, chptr, token, &msglen);

    if (MyClient(from)) {
	send_message(from, txt, msglen);
	if (IsChanAnon(chptr) && IsLocalChan(chptr->chname))
	    return;
    }

    if (IsChanAnon(chptr) && IsGlobalChan(chptr->chname))
	return;

    for (ptr = chptr->members.head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	cm = ptr->data;
	if (!cm)
	    continue;
	acptr = cm->client_p;
	if (!MyConnect(acptr) || (acptr == from) || IsAnon(acptr))
	    continue;
	if (check_fake_direction(from, acptr))
	    continue;
	send_message(acptr, txt, msglen);
    }
    return;
}

int send_supported(aClient *cptr)
{
    char featurebuf[1024];

    ircsnprintf(featurebuf, sizeof(featurebuf), FEATURESET1, MAXBANS, TOPICLEN, TOPICLEN);
    send_me_numeric(cptr, RPL_ISUPPORT, featurebuf);

    ircsnprintf(featurebuf, sizeof(featurebuf), FEATURESET2, MAXWATCH, MAXMODEPARAMS,
		MAXSILES, NICKLEN, "(uohv).@%+", GeneralOpts.chanmodelist,
		ServerInfo.networkname, ChannelConf.max_channels_per_user);
    send_me_numeric(cptr, RPL_ISUPPORT, featurebuf);
    return 0;
}

/*
 * kill_client
 *
 * inputs       - client to send kill towards
 *              - pointer to client to kill
 *              - reason for kill
 * output       - NONE
 * side effects - NONE
 */

void kill_client(struct Client *client_p, struct Client *diedie, char *pattern, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, pattern);

    ircvsprintf(buf, pattern, args);

    va_end(args);

    sendto_one_server(client_p, &me, TOK1_KILL, "%~C :%s", diedie, buf);

}
