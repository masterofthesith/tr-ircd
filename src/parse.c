/************************************************************************
 *   IRC - Internet Relay Chat, src/parse.c
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
 * $Id: parse.c,v 1.5 2004/07/30 11:30:26 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#define TOKEN 1
#ifdef STATIC_MODULES
#include "msg.h"
#include "msgtok1.h"
#else
#include "msg.h"
#endif
#undef TOKEN
#include "sys.h"
#include "numeric.h"
#include "h.h"
#include "s_conf.h"

static char *para[MAXPARA + 1];
static char sender[HOSTLEN + 1];

static int cancel_clients(aClient *cptr, aClient *sptr, char *cmd)
{
    /*
     * * with TS, fake prefixes are a common thing, during the * connect
     * burst when there's a nick collision, and they * must be ignored
     * rather than killed because one of the * two is surviving.. so we
     * don't bother sending them to * all ops everytime, as this could
     * send 'private' stuff * from lagged clients. we do send the ones
     * that cause * servers to be dropped though, as well as the ones
     * from * non-TS servers -orabidoo
     */
    /*
     * Incorrect prefix for a server from some connection.  If it is a
     * client trying to be annoying, just QUIT them, if it is a server
     * then the same deal.
     */
    if (IsServer(sptr) || IsMe(sptr)) {
	/*
	 * or we could just take out the message. <EG>  -wd
	 */
	sendto_lev(DEBUG_LEV, "Message for %s[%s] from %s",
		   sptr->name, sptr->from->name,
		   get_client_name(cptr, (IsServer(cptr) ? HIDEME : TRUE)));
	if (IsServer(cptr)) {
	    sendto_lev(DEBUG_LEV,
		       "Not dropping server %s (%s) for Fake Direction", cptr->name, sptr->name);
	    return -1;
	}

	if (IsPerson(cptr))
	    sendto_lev(DEBUG_LEV,
		       "Would have dropped client %C [%s from %s]",
		       cptr, cptr->user->server, cptr->from->name);
	return -1;
    }
    /*
     * Ok, someone is trying to impose as a client and things are
     * confused.  If we got the wrong prefix from a server, send out a
     * kill, else just exit the lame client.
     */
    if (IsServer(cptr)) {
	/*
	 * * * If the fake prefix is coming from a TS server, discard it *
	 * * silently -orabidoo
	 * * also drop it if we're gonna kill services by not doing so }:/
	 */
	if (DoesTS(cptr)) {
	    if (sptr->user)
		sendto_lev(DEBUG_LEV,
			   "Message for %s[%s@%s!%s] from %s (TS, ignored)",
			   sptr->name, sptr->user->username,
			   sptr->user->host, sptr->from->name, get_client_name(cptr, TRUE));
	    return 0;
	} else {
	    if (sptr->user)
		sendto_lev(DEBUG_LEV,
			   "Message for %s[%s@%s!%s] from %s",
			   sptr->name, sptr->user->username,
			   sptr->user->host, sptr->from->name,
			   get_client_name(cptr, (IsServer(cptr) ? HIDEME : TRUE)));
	    if (IsULine(sptr)) {
		sendto_lev(DEBUG_LEV,
			   "Would have killed U:lined client %s for fake direction", sptr->name);
		return 0;
	    }
	    sendto_serv_butone(NULL, &me, TOK1_KILL,
			       "%~C :%C (%C[%C] != %C, Fake Prefix)",
			       sptr, &me, sptr, sptr->from, cptr);
	    sptr->flags |= FLAGS_KILLED;
	    return exit_client(sptr, &me, "Fake Prefix");
	}
    }
    return exit_client(cptr, &me, "Fake prefix");
}

static void remove_unknown(aClient *cptr, char *asender, char *buffer)
{
    if (!IsRegistered(cptr))
	return;

    if (IsPerson(cptr)) {
	sendto_lev(DEBUG_LEV,
		   "Weirdness: Unknown client prefix (%s) from %s, Ignoring %s",
		   buffer, get_client_name(cptr, FALSE), asender);
	return;
    }
    /*
     * Not from a server so don't need to worry about it.
     */
    if (!IsServer(cptr) || !asender)
	return;
    /*
     * Do kill if it came from a server because it means there is a
     * ghost user on the other server which needs to be removed. -avalon
     * Tell opers about this. -Taner
     */
    if (!strchr(asender, '.'))
	sendto_one_person(cptr, &me, TOK1_KILL, "%~C %s :%C (%s(?) <- %s)",
			  cptr, asender, &me, asender, get_client_name(cptr, FALSE));
    else {
	sendto_lev(DEBUG_LEV,
		   "Unknown prefix (%s) from %s, Squitting %s",
		   buffer, get_client_name(cptr, HIDEME), asender);
	sendto_one_server(cptr, &me, TOK1_SQUIT, "%s :(Unknown prefix (%s) from %s)",
			  asender, buffer, get_client_name(cptr, HIDEME));
    }
}

/*
 * parse a buffer.
 * 
 * NOTE: parse() should not be called recusively by any other functions!
 */
int server_parse(aClient *cptr, char *buffer, char *bufend)
{
    aClient *from = cptr;
    aClient *to;
    char *ch, *s;
    int i, numeric = 0, paramcount;
    struct Message *mptr;
    struct Token *tokptr;

    logevent_call(LogSys.parse_debug, "Server", cptr, buffer);    

    if (IsDead(cptr) || IsClosing(cptr))
	return -1;

    s = sender;
    *s = '\0';

    for (ch = buffer; *ch == ' '; ch++);	/* skip spaces */

    para[0] = from->name;
    if (*ch == ':') {

	/*
	 * Copy the prefix to 'sender' assuming it terminates with
	 * SPACE (or NULL, which is an error, though).
	 */

	for (++ch; *ch && *ch != ' '; ++ch)
	    if (s < (sender + HOSTLEN))
		*s++ = *ch;
	*s = '\0';

	/*
	 * Actually, only messages coming from servers can have the
	 * prefix--prefix silently ignored, if coming from a user
	 * client...
	 * 
	 */

	if (*sender) {
	    from = find_client(sender);

	    para[0] = sender;
	    /*
	     * Hmm! If the client corresponding to the prefix is not
	     * found--what is the correct action??? Now, I will ignore the
	     * message (old IRC just let it through as if the prefix just
	     * wasn't there...) --msa
	     */
	    if (!from) {
		logevent_call(LogSys.parse_unknown_prefix, sender, buffer, cptr->name);
		ircstp->is_unpf++;
		remove_unknown(cptr, sender, buffer);
		return 0;
	    }

	    if (from->from != cptr) {
		ircstp->is_wrdi++;
		logevent_call(LogSys.parse_unknown_message, buffer, cptr->name);
		cancel_clients(cptr, from, buffer);
		return 0;
	    }
	}
	while (*ch == ' ')
	    ch++;
    }

    else if (*ch == '!') {
	for (; *ch && *ch != ' '; ++ch)
	    if (s < (sender + HOSTLEN))
		*s++ = *ch;
	*s = '\0';

	if (*sender) {
	    from = find_by_base64_id(sender + 1);

	    if (!from) {
		sendto_lev(DEBUG_LEV, "idmsg for unknown %s from %s", sender, cptr->name);
		logevent_call(LogSys.parse_unknown_prefix, sender, buffer, cptr->name);
		ircstp->is_unpf++;
		remove_unknown(cptr, sender, buffer);
		return 0;
	    }
	    para[0] = from->name;

	    if (from->from != cptr) {
		ircstp->is_wrdi++;
		logevent_call(LogSys.parse_unknown_message, buffer, cptr->name);
		cancel_clients(cptr, from, buffer);
		return 0;
	    }
	}
	while (*ch == ' ')
	    ch++;
    }

    if (*ch == '\0') {
	ircstp->is_empt++;
	logevent_call(LogSys.parse_empty, cptr->name, from->name);
	return 0;
    }

    /*
     * Extract the command code from the packet.  Point s to the end
     * of the command code and calculate the length using pointer
     * arithmetic.  Note: only need length for numerics and *all*
     * numerics must have parameters and thus a space after the command
     * code. -avalon
     * 
     * ummm???? - Dianora
     */

    /*
     * check for token
     */

    if (*(ch + 1) == ' ' || *(ch + 1) == '\0') {
	tokptr = &tok1_msgtab[(u_char) *ch];

	if (!tokptr->cmd) {
	    sendto_lev(DEBUG_LEV, "FIRST: Unknown token %d from server %s",
		       *ch, get_client_name(cptr, HIDEME));

	    if (buffer[0] != '\0') 
		logevent_call(LogSys.parse_unknown, ch, get_client_name(cptr, TRUE));
	    ircstp->is_unco++;
	    return 0;
	}

	mptr = tokptr->msg;

	if (!mptr) {
	    sendto_lev(DEBUG_LEV, "SECOND: Unknown token %d from server %s",
		       *ch, get_client_name(cptr, HIDEME));

	    if (buffer[0] != '\0') 
		logevent_call(LogSys.parse_unknown, ch, get_client_name(cptr, TRUE));
	    ircstp->is_unco++;
	    return 0;
	}

	paramcount = mptr->parameters;
	i = bufend - ((s) ? s : ch);
	mptr->bytes += i;
	s = ch + 1;
	if (*s)
	    *s++ = '\0';
	else
	    s = NULL;

	/*
	 * check for numeric
	 */

    } else if (*(ch + 3) == ' ' && IsDigit(*ch) && IsDigit(*(ch + 1))
	       && IsDigit(*(ch + 2))) {
	mptr = (struct Message *) NULL;
	numeric = (*ch - '0') * 100 + (*(ch + 1) - '0') * 10 + (*(ch + 2) - '0');
	paramcount = MAXPARA;
	ircstp->is_num++;
	s = ch + 3;
	*s++ = '\0';

    } else {
	s = strchr(ch, ' ');

	if (s)
	    *s++ = '\0';

	mptr = hash_parse(ch);

	if (!mptr || !mptr->cmd) {
	    /*
	     * only send error messages to things that actually sent buffers to us
	     * and only people, too.
	     */
	    if (buffer[0] != '\0')
		logevent_call(LogSys.parse_unknown, ch, get_client_name(cptr, TRUE));
	    ircstp->is_unco++;
	    return 0;
	}

	paramcount = mptr->parameters;
	i = bufend - ((s) ? s : ch);
	mptr->bytes += i;

    }
    /*
     * Must the following loop really be so devious? On surface it
     * splits the message to parameters from blank spaces. But, if
     * paramcount has been reached, the rest of the message goes into
     * this last parameter (about same effect as ":" has...) --msa
     */

    /*
     * Note initially true: s==NULL || *(s-1) == '\0' !! 
     */

    i = 0;
    if (s) {
	if (paramcount > MAXPARA)
	    paramcount = MAXPARA;
	for (;;) {
	    while (*s == ' ')
		*s++ = '\0';

	    if (*s == '\0')
		break;
	    if (*s == ':') {
		/*
		 * The rest is a single parameter 
		 */
		para[++i] = s + 1;
		break;
	    }
	    para[++i] = s;
	    if (i >= paramcount)
		break;

	    while (*s && *s != ' ')
		s++;
	}
    }

    para[++i] = NULL;

    /* This kludge hopefully solves the case for halcyon */

    if (para[1] && para[1][0] == '!') {
	to = find_by_base64_id(para[1] + 1);
	if (to)
	    para[1] = to->name;
    }

    if (mptr == (struct Message *) NULL)
	return (do_numeric(numeric, cptr, from, i, para));

    mptr->count++;

    if (!IsRegistered(cptr)) {

        return (*mptr->func_unrg) (cptr, from, i, para);

    } else {

	if (IsServer(cptr))
	    return (*mptr->func_srvr) (cptr, from, i, para);
        else if (IsAnOper(cptr) && IsPerson(cptr))
            return (*mptr->func_oper) (cptr, from, i, para);
        else if (IsPerson(cptr))        /* Can we use IsPerson here ? -TimeMr14C */
            return (*mptr->func_user) (cptr, from, i, para);
        else if (IsService(cptr))
            return (*mptr->func_srvc) (cptr, from, i, para);
    } 

    return 0;
    
}

int user_parse(aClient *cptr, char *buffer, char *bufend)
{
    aClient *from = cptr;
    aClient *to;
    char *ch, *s;
    int i, paramcount;
    struct Message *mptr;

    logevent_call(LogSys.parse_debug, "User", cptr, buffer);

    if (IsDead(cptr) || IsClosing(cptr))
	return -1;

    s = sender;
    *s = '\0';

    for (ch = buffer; *ch == ' '; ch++);	/* skip spaces */

    para[0] = from->name;
    if (*ch == ':') {

	/*
	 * Copy the prefix to 'sender' assuming it terminates with
	 * SPACE (or NULL, which is an error, though).
	 */

	for (++ch; *ch && *ch != ' '; ++ch)
	    if (s < (sender + HOSTLEN))
		*s++ = *ch;
	*s = '\0';

	/*
	 * Actually, only messages coming from servers can have the
	 * prefix--prefix silently ignored, if coming from a user
	 * client...
	 * 
	 */

	if (*sender) {
	    from = find_client(sender);

	    para[0] = sender;
	    /*
	     * Hmm! If the client corresponding to the prefix is not
	     * found--what is the correct action??? Now, I will ignore the
	     * message (old IRC just let it through as if the prefix just
	     * wasn't there...) --msa
	     */
	    if (!from) {
		logevent_call(LogSys.parse_unknown_prefix, sender, buffer, cptr->name);
		ircstp->is_unpf++;
		remove_unknown(cptr, sender, buffer);
		return 0;
	    }
	    if (from->from != cptr) {
		ircstp->is_wrdi++;
		logevent_call(LogSys.parse_unknown_message, buffer, cptr->name);
		cancel_clients(cptr, from, buffer);
		return 0;
	    }
	}
	while (*ch == ' ')
	    ch++;
    }

    if (*ch == '\0') {
	ircstp->is_empt++;
	logevent_call(LogSys.parse_empty, cptr->name, from->name);
	return 0;
    }
    /*
     * Extract the command code from the packet.  Point s to the end
     * of the command code and calculate the length using pointer
     * arithmetic.  Note: only need length for numerics and *all*
     * numerics must have parameters and thus a space after the command
     * code. -avalon
     * 
     * ummm???? - Dianora
     */

    s = strchr(ch, ' ');

    if (s)
	*s++ = '\0';

    mptr = hash_parse(ch);

    if (!mptr || !mptr->cmd) {
	/*
	 * only send error messages to things that actually sent buffers to us
	 * and only people, too.
	 */
	if (buffer[0] != '\0') {
	    if (IsPerson(from))
		send_me_numeric(from, ERR_UNKNOWNCOMMAND, ch);
	    logevent_call(LogSys.parse_unknown, ch, get_client_name(cptr, TRUE));
	}
	ircstp->is_unco++;
	return 0;
    }

    paramcount = mptr->parameters;
    i = bufend - ((s) ? s : ch);
    mptr->bytes += i;

    /*
     * Allow only 1 msg per 2 seconds (on average) to prevent
     * dumping. to keep the response rate up, bursts of up to 5 msgs
     * are allowed -SRB Opers can send 1 msg per second, burst of ~20
     * -Taner
     */
    if (mptr->flags & M_SLOW) {
	if (IsAnOper(cptr))
	    cptr->since += (cptr->receiveM % 10) ? 1 : 0;
	else
	    cptr->since += (2 + i / 120);
    }

    /*
     * Must the following loop really be so devious? On surface it
     * splits the message to parameters from blank spaces. But, if
     * paramcount has been reached, the rest of the message goes into
     * this last parameter (about same effect as ":" has...) --msa
     */

    /*
     * Note initially true: s==NULL || *(s-1) == '\0' !! 
     */

    i = 0;
    if (s) {
	if (paramcount > MAXPARA)
	    paramcount = MAXPARA;
	for (;;) {
	    while (*s == ' ')
		*s++ = '\0';

	    if (*s == '\0')
		break;
	    if (*s == ':') {
		/*
		 * The rest is a single parameter 
		 */
		para[++i] = s + 1;
		break;
	    }
	    para[++i] = s;
	    if (i >= paramcount)
		break;

	    while (*s && *s != ' ')
		s++;
	}
    }

    para[++i] = NULL;

    /* This kludge hopefully solves the case for halcyon */

    if (para[1] && para[1][0] == '!') {
	to = find_by_base64_id(para[1] + 1);
	if (to)
	    para[1] = to->name;
    }

    mptr->count++;

    if (!IsRegistered(cptr)) {

	return (*mptr->func_unrg) (cptr, from, i, para);

    } else {
	if (IsRegisteredUser(from) && (mptr->flags & M_IDLE_RESET))
	    from->user->last = timeofday;

	if (mptr->flags & M_FLOOD_END)
	    flood_endgrace(cptr);

        if (IsServer(cptr))
            return (*mptr->func_srvr) (cptr, from, i, para);
        else if (IsAnOper(cptr) && IsPerson(cptr))
	    return (*mptr->func_oper) (cptr, from, i, para);
	else if (IsPerson(cptr))	/* Can we use IsPerson here ? -TimeMr14C */
	    return (*mptr->func_user) (cptr, from, i, para);
	else if (IsService(cptr))
	    return (*mptr->func_srvc) (cptr, from, i, para);
    }
    return 0;
}
