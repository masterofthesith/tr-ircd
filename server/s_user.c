/************************************************************************
 *   IRC - Internet Relay Chat, server/s_user.c
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
 *
 *  $Id: s_user.c,v 1.9 2004/07/12 09:14:59 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "tools.h"
#include "h.h"
#include "channel.h"
#include "class.h"
#include "fd.h"
#include "listener.h"
#include "msg.h"
#include "numeric.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "throttle.h"
#include "supported.h"
#include "packet.h"
#include "hook.h"
#include "language.h"

#define IRCD_BUFSIZE 512

static int hookid_introduce_client = 0;
static int hookid_proxy_add = 0;

static int user_welcome(struct Client *source_p, struct ConfItem *aconf);
static void manage_faked_oper_hostname(struct Client *sptr, struct ConfItem *pwaconf);

static int introduce_client(struct Client *client_p, struct Client *source_p,
			    struct User *user, char *nick);

void init_user(void)
{
    hookid_introduce_client = hook_add_event("introduce client");
    hookid_proxy_add = hook_add_event("add to proxy");
    return;
}

/*
 * ** register_local_user
 * **      This function is called when both NICK and USER messages
 * **      have been accepted for the client, in whatever order. Only
 * **      after this, is the USER message propagated.
 * **
 * **      NICK's must be propagated at once when received, although
 * **      it would be better to delay them too until full info is
 * **      available. Doing it is not so simple though, would have
 * **      to implement the following:
 * **
 * **      (actually it has been implemented already for a while) -orabidoo
 * **
 * **      1) user telnets in and gives only "NICK foobar" and waits
 * **      2) another user far away logs in normally with the nick
 * **         "foobar" (quite legal, as this server didn't propagate
 * **         it).
 * **      3) now this server gets nick "foobar" from outside, but
 * **         has already the same defined locally. Current server
 * **         would just issue "KILL foobar" to clean out dups. But,
 * **         this is not fair. It should actually request another
 * **         nick from local user or kill him/her...
 */

int register_local_user(struct Client *client_p, struct Client *source_p,
			char *nick, char *username)
{
    struct ConfItem *aconf;
    struct User *user = source_p->user;
    char *p;
    char passarr[PASSWDLEN];
    char tmpstr2[IRCD_BUFSIZE];
    char ipaddr[HOSTIPLEN];
    int status;
    int r;
    dlink_node *ptr;
    dlink_node *m;
    char newhostarr[HOSTLEN];
    char *q;

    assert(0 != source_p);
    assert(0 != source_p);
    assert(source_p->username != username);

    user->last = timeofday;

    /* pointed out by Mortiis, never be too careful */
    if (strlen(username) > USERLEN)
	username[USERLEN] = '\0';

    if (!valid_hostname(source_p->sockhost)) {
	send_me_notice(source_p, ":*** Notice -- You have an illegal character in your hostname");
	if (IsClosing(source_p))
	    return CLIENT_EXITED;
	strlcpy_irc(source_p->sockhost, source_p->hostip, HOSTIPLEN);
    }

    if (strchr(source_p->hostip, ':'))
	source_p->protoflags |= PFLAGS_IPV6HOST;

    strlcpy_irc(user->host, source_p->sockhost, HOSTLEN);

    q = user->host;
    strlcpy_irc(user->fakehost, calcmask(q, newhostarr), HOSTLEN);

    status = check_client(client_p, source_p, username);
    if (status < 0)
	return CLIENT_EXITED;

    ptr = source_p->confs.head;
    aconf = ptr->data;

    source_p->allow_read = MAX_FLOOD_PER_SEC_I;

    if (aconf == NULL) 
	return CLIENT_EXITED;

    if (!IsGotId(source_p)) {
	if (!ServerOpts.identd_use_tilde && ServerOpts.identd_complain) {
	    ircstp->is_ref++;
	    send_me_notice(source_p,
			   ":*** Notice -- You need to install identd to use this server");
	    exit_client(source_p, &me, "Install identd");
	    return CLIENT_EXITED;
	} else if (!ServerOpts.identd_use_tilde && !ServerOpts.identd_complain) {
	    strlcpy_irc(source_p->username, username, USERLEN);
	} else {
	    *(source_p->username) = '~';
	    strlcpy_irc(&source_p->username[1], username, USERLEN - 1);
	}
	source_p->username[USERLEN] = '\0';
    }

    /* password check */
    if (!BadPtr(aconf->passwd) && (p = source_p->passwd) &&
	strcmp(calcpass(p, passarr), aconf->passwd)) {
	ircstp->is_ref++;
	send_me_numeric(source_p, ERR_PASSWDMISMATCH);
	exit_client(source_p, &me, "Bad Password");
	return CLIENT_EXITED;
    }
    memset(source_p->passwd, 0, sizeof(source_p->passwd));

    /* Limit clients */
    /*
     * We want to be able to have servers and F-line clients
     * connect, so save room for "buffer" connections.
     * Smaller servers may want to decrease this, and it should
     * probably be just a percentage of the MAXCLIENTS...
     *   -Taner
     */
    /* Except "F:" clients */
    if ((((Count.local + 1) >= GeneralOpts.maxclients)
	 || ((Count.local + 1) >= (GeneralOpts.maxclients - 5))) && !IsConfExemptLimits(aconf)) {
	sendto_lev(REJ_LEV, "Too many clients, rejecting %s[%s].", nick, source_p->sockhost);

	ircstp->is_ref++;
	exit_client(source_p, &me, "Sorry, server is full - try later");
	return CLIENT_EXITED;
    }

    /* valid user name check */

    if (!valid_username(source_p->username)) {
	sendto_lev(REJ_LEV, "Invalid username: %s (%s@%s)",
		   nick, source_p->username, source_p->sockhost);
	ircstp->is_ref++;
	ircsprintf(tmpstr2, "Invalid username [%s]", source_p->username);
	exit_client(source_p, &me, tmpstr2);
	return CLIENT_EXITED;
    }

    if (check_drone_PB(source_p->username, source_p->info)) {
	sendto_lev(REJ_LEV, "Rejecting acebot-style drone: %^C [%s]", source_p, source_p->info);
	exit_client(source_p, &me, "You match the pattern of a known trojan, please check your system.");
	return CLIENT_EXITED;
    }

    SetClient(source_p);

    strlcpy_irc(user->username, source_p->username, USERLEN);

    /* end of valid user name check */

    m = dlinkFind(&unknown_list, source_p);
    if (m != NULL) {
        Count.unknown--;
        dlinkDelete(m, &unknown_list);
        dlinkAdd(source_p, m, &lclient_list);
    }

    inetntop(source_p->aftype, &IN_ADDR(source_p->ip), ipaddr, HOSTIPLEN);
    sendto_lev(CCONN_LEV, "Client connecting: %^C [%s] {%d} [%s]",
	       source_p, ipaddr, get_client_class(source_p), source_p->info);

    if ((++Count.local) > Count.max_loc) {
	Count.max_loc = Count.local;
	if (!(Count.max_loc % 10))
	    sendto_lev(DEBUG_LEV, "New Max Local Clients: %d", Count.max_loc);
    }

    source_p->pingval = get_client_ping(source_p);
    source_p->sendqlen = get_sendq(source_p);

    source_p->servptr = &me;
    add_client_to_llist(&(source_p->servptr->serv->users), source_p);

    source_p->servptr->serv->usercnt++;

    /* Increment our total user count here */
    if (++Count.total > Count.max_tot)
	Count.max_tot = Count.total;

    assign_localuser_identity(source_p);
    SetHasID(source_p);

    if (source_p->langstring[0]) {
	source_p->lang = lang_parse(source_p->langstring);
	if (source_p->lang)
	    sendto_one_person(source_p, NULL, TOK1_NOTICE, "AUTH :*** Setting language to %s",
				  source_p->langstring);
    }

    r = user_welcome(source_p, aconf);

    if (r & FLAGS_EXITED)
	return CLIENT_EXITED;

    return (introduce_client(client_p, source_p, user, nick));
}

/*
 * register_remote_user
 *
 * inputs
 * output
 * side effects - This function is called when a remote client
 *                is introduced by a server.
 */
int register_remote_user(struct Client *client_p, struct Client *source_p,
			 char *nick, char *username)
{
    struct User *user = source_p->user;
    struct Client *target_p;

    assert(0 != source_p);
    assert(source_p->username != username);

    user->last = timeofday;

    /* pointed out by Mortiis, never be too careful */
    if (strlen(username) > USERLEN)
	username[USERLEN] = '\0';

    strlcpy_irc(source_p->username, username, USERLEN);
    strlcpy_irc(user->username, username, USERLEN);

    SetClient(source_p);

    /* Increment our total user count here */
    if (++Count.total > Count.max_tot)
	Count.max_tot = Count.total;

    source_p->servptr = find_server(user->server); 

    if (source_p->servptr == NULL) {
	sendto_ops("Ghost killed: %s on invalid server %s", source_p->name, source_p->user->server);

	kill_client(client_p, source_p, "%s (Server doesn't exist)", me.name);

	source_p->flags |= FLAGS_KILLED;
	return exit_client(source_p, &me, "Ghost");
    }

    if ((target_p = source_p->servptr) && target_p->from != source_p->from) {
	sendto_lev(DEBUG_LEV, "Bad User [%s] :%s USER %s@%s %s, != %s[%s]",
		   client_p->name, nick, source_p->username,
		   source_p->sockhost, user->server, target_p->name, target_p->from->name);
	kill_client(client_p, source_p,
		    ":%s (%s != %s[%s] USER from wrong direction)", me.name,
		    user->server, target_p->from->name, target_p->from->sockhost);

	source_p->flags |= FLAGS_KILLED;
	return exit_client(source_p, &me, "USER server wrong direction");

    }
    /*
     * Super GhostDetect:
     * If we can't find the server the user is supposed to be on,
     * then simply blow the user away.        -Taner
     */
    if (!target_p) {
	kill_client(client_p, source_p, "%s GHOST (no server found)", me.name, user->server);
	sendto_ops("No server %s for user %s[%s@%s] from %s",
		   user->server, source_p->name, source_p->username,
		   source_p->sockhost, source_p->from->name);
	source_p->flags |= FLAGS_KILLED;
	return exit_client(source_p, &me, "Ghosted Client");
    }

    add_client_to_llist(&(source_p->servptr->serv->users), source_p);

    source_p->servptr->serv->usercnt++;

    return (introduce_client(client_p, source_p, user, nick));
}

/*
 * introduce_client
 *
 * inputs       -
 * output       -
 * side effects - This common function introduces a client to the rest
 *                of the net, either from a local client connect or
 *                from a remote connect.
 */
static int introduce_client(struct Client *cptr, struct Client *sptr, struct User *user, char *nick)
{
    struct hook_data thisdata;
    thisdata.client_p = cptr;
    thisdata.source_p = sptr;
    thisdata.user = user;
    thisdata.name = nick;

    if ((NOW - Count.day) > 86400) {
	Count.today = 0;
	Count.day = NOW;
    }
    if ((NOW - Count.week) > 604800) {
	Count.weekly = 0;
	Count.week = NOW;
    }
    if ((NOW - Count.month) > 2592000) {
	Count.monthly = 0;
	Count.month = NOW;
    }
    if ((NOW - Count.year) > 31536000) {
	Count.yearly = 0;
	Count.year = NOW;
    }
    Count.today++;
    Count.weekly++;
    Count.monthly++;
    Count.yearly++;
    
    return hook_call_event(hookid_introduce_client, &thisdata);
}

/*
 * do_local_user
 *
 * inputs       -
 * output       -
 * side effects -
 */
int do_local_user(char *nick, aClient *cptr, aClient *sptr, char *username, char *host,
		  char *mask, char *server, unsigned long serviceid, char *realname)
{
    struct User *user;

    user = make_user(sptr);

    if (!IsUnknown(sptr)) {
	send_me_numeric(sptr, ERR_ALREADYREGISTRED);
	return 0;
    }
    sptr->umode |= UMODE_i;	/* invisible IS default. always */
    if(ServerOpts.default_fakehost_mode)	
    	sptr->umode |= UMODE_x;
    if (ServerOpts.use_registerfilter)
	sptr->umode |= UMODE_R;

    strlcpy_irc(user->host, host, sizeof(user->host));
    DupString(user->server, me.name);

    server[strlen(server) - 1] = '\0';

    if ((ServerInfo.aliasname[0] != '\0') && (server[0] != '\0')) {
	if (!irc_strcmp(server + 1, ServerInfo.aliasname))
	    sptr->protoflags |= PFLAGS_ALIASED;
    }

    Count.invisi++;

    if (mask) 
	strlcpy_irc(user->fakehost, mask, sizeof(user->fakehost));

    strlcpy_irc(sptr->info, realname, REALLEN);

    sptr->user->servicestamp = serviceid;
    sptr->oflag = 0;

    if (sptr->name[0]) {
	strlcpy_irc(user->username, username, USERLEN);
	/* NICK already received, now I have USER... */
	return register_local_user(cptr, sptr, sptr->name, username);
    } else if (!IsGotId(sptr)) {
	strlcpy_irc(user->username, username, USERLEN);
	strlcpy_irc(sptr->username, username, USERLEN);
    }
    return 0;
}

/*
 * do_remote_user
 *
 * inputs       -
 * output       -
 * side effects -
 */

int do_remote_user(char *nick, aClient *client_p, aClient *source_p, char *username, char *host,
		   char *mask, char *server, unsigned long serviceid, char *realname)
{
    struct User *user;

    assert(0 != source_p);
    assert(source_p->username != username);

    user = make_user(source_p);

    strlcpy_irc(source_p->sockhost, host, HOSTLEN);
    strlcpy_irc(user->host, host, HOSTLEN);
    strlcpy_irc(user->fakehost, mask, HOSTLEN);
    strlcpy_irc(source_p->info, realname, REALLEN);
    DupString(user->server, server);

    source_p->user->servicestamp = serviceid;

    return register_remote_user(client_p, source_p, source_p->name, username);
}

static void manage_faked_oper_hostname(aClient *sptr, aConfItem *pwaconf)
{
    if (!ServerOpts.staffhide)
	return;

    if (!IsConfDoSpoofIp(pwaconf))
	return;

    if (IsDead(sptr))	
	return;

	/* It is possible to enter this function, even though the
	 * client has been marked as dead. -TimeMr14C 17.05.2003
	 */

    if (IsConfSpoofNotice(pwaconf))
	sendto_ops("%^C has masked their hostname.", sptr);

    send_me_notice(sptr, ":*** Your hostname has been masked.");

    if (IsDead(sptr))
	return;

	/* The above line ensures us that we dont touch the
	 * user structrue (sptr->user) if a write error has happened
	 * whilst the send_me_notice -TimeMr14C 17.05.2003
	 */

    sptr->protoflags |= PFLAGS_OPERFAKEHOST;

    sptr->user->real_oper_host = MyMalloc(strlen(sptr->user->host) + 1);
    sptr->user->real_oper_ip = MyMalloc(strlen(sptr->hostip) + 1);

    strcpy(sptr->user->real_oper_host, sptr->user->host);
    strcpy(sptr->user->real_oper_ip, sptr->hostip);

    if (IsConfCanChooseFakehost(pwaconf)) {
	if (sptr->fakehost_override[0]) {
	    strlcpy_irc(sptr->sockhost, sptr->fakehost_override, HOSTLEN);
	    strlcpy_irc(sptr->user->host, sptr->fakehost_override, HOSTLEN);
	} else if (pwaconf->localhost[0]) {
	    strlcpy_irc(sptr->sockhost, pwaconf->localhost, HOSTLEN);
	    strlcpy_irc(sptr->user->host, pwaconf->localhost, HOSTLEN);
	}
    } else if (pwaconf != NULL) {
	if (pwaconf->localhost[0]) {
	    strlcpy_irc(sptr->sockhost, pwaconf->localhost, HOSTLEN);
	    strlcpy_irc(sptr->user->host, pwaconf->localhost, HOSTLEN);
	}
    }

    throttle_remove(sptr->hostip);

    sptr->flags |= FLAGS_GOTID;
    IN4_ADDR(sptr->ip) = 0;
    strcpy(sptr->hostip, "0.0.0.0");

    return;
}

/* 
 * user_welcome
 *
 * inputs       - client pointer to client to welcome
 * output       - NONE
 * side effects -
 */
static int user_welcome(struct Client *sptr, struct ConfItem *aconf)
{
    aMotd *smotd;
    aMotdItem *smotdline;

    send_me_numeric(sptr, RPL_WELCOME, ServerInfo.networkname, sptr->name,
		    sptr->user->username, sptr->user->host);
    /* We require the above line for DCC to work, otherwise,   
     * we could use %C instead -TimeMr14C */
    send_me_numeric(sptr, RPL_YOURHOST, get_client_name(&me, TRUE), version);
    send_me_numeric(sptr, RPL_CREATED, creation);
    send_me_numeric(sptr, RPL_MYINFO, me.name, version, 
		    GeneralOpts.umodelist, GeneralOpts.allchanmodes, GeneralOpts.paramchanmodes);
    send_supported(sptr);

    manage_faked_oper_hostname(sptr, aconf);

    if (IsDead(sptr))
	return sptr->flags;

	/* The above call helps us in not being required to deal with additional things
	 * about the structure itself (reading host, while there isn't one, since freed.
	 * -TimeMr14C 17.05.2003
	 */

    /* This call here may call the dummy function with no content, or the one
     * after the replacement by the proxymon module.
     * -TimeMr14C */

    if (GeneralOpts.enable_proxymonitor && !IsConfExemptKline(aconf) && !IsOperFakehost(sptr)) {
	struct hook_data thisdata;
	thisdata.source_p = sptr;
	hook_call_event(hookid_proxy_add, &thisdata);
    }

    send_lusers(sptr, sptr, 1, NULL);

    send_me_notice(sptr, ":*** Notice -- motd was last changed at %s", (&(GeneralOpts.motd))->lastchange);
    if (ServerOpts.short_motd) {
	send_me_notice(sptr, ":*** Notice -- Please read the motd if you haven't read it");
	send_me_numeric(sptr, RPL_MOTDSTART, me.name);
	if ((smotd = &(GeneralOpts.shortmotd)) == NULL) {
	    send_me_numeric(sptr, RPL_MOTD, "*** This is the short motd ***");
	} else {
	    smotdline = smotd->content;
	    while (smotdline) {
		send_me_numeric(sptr, RPL_MOTD, smotdline->line);
		smotdline = smotdline->next;
	    }
	}
	send_me_numeric(sptr, RPL_ENDOFMOTD);
    } else {
	send_message_file(&(GeneralOpts.motd), sptr, sptr, 1, NULL);
    }
    return sptr->flags;
}

