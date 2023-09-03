/************************************************************************
 *   IRC - Internet Relay Chat protocol/native.c
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
#include "h.h"
#include "hook.h"
#include "s_conf.h"
#include "comply.h"
#include "usermode.h"

static int log_greet = -1;

static int do_introduce_client(struct hook_data *thisdata);
static int do_sendnick_TS(struct hook_data *thisdata);
static int do_inform_remote_servers(struct hook_data *thisdata);
static int do_inform_other_servers(struct hook_data *thisdata);
static int do_continue_server_estab(struct hook_data *thisdata);
static int do_start_server_estab(struct hook_data *thisdata);
static int do_connect_to_server(struct hook_data *thisdata);

void proto_modinit()
{
    if (!GeneralOpts.protocol_in_use) {
	hook_add_hook("continue server estab", (hookfn *) do_continue_server_estab);
	hook_add_hook("start server estab", (hookfn *) do_start_server_estab);
	hook_add_hook("sendnick TS", (hookfn *) do_sendnick_TS);
	hook_add_hook("introduce client", (hookfn *) do_introduce_client);
	hook_add_hook("inform remote servers", (hookfn *) do_inform_remote_servers);
	hook_add_hook("inform other servers", (hookfn *) do_inform_other_servers);
	hook_add_hook("connect to server", (hookfn *) do_connect_to_server);
        log_greet = logevent_register("protocol init", LOG_ON_LOG, LOG_IRCLOG|LOG_STDOUT, LOG_INFO,
                        "Initialized Native TR-IRCD 5 Module");
	logevent_call(log_greet);
    }
    return;
}

int _comply_unload_modules()
{
    return 1;
}

int _comply_ignore_messages()
{
    return 1;
}

int _comply_untokenize()
{
    return 1;
}

int _comply_retokenize()
{
    return 1;
}

int _comply_unuse_messages()
{
    return 1;
}

int _comply_rehash()
{
    if (!GeneralOpts.protocol_in_use) 
        logevent_unregister(log_greet);
    return 1;
}

static int do_introduce_client(struct hook_data *thisdata)
{
    struct Client *cptr = thisdata->client_p;
    struct Client *sptr = thisdata->source_p;
    struct User *user = thisdata->user;
    char *nick = thisdata->name;
    struct Client *nsptr;
    char ubuf[32];
    unsigned long li = 0;
    char diff = '\0';
    char r_sidbuf[8], r_ipbuf[8];
    char *sidbuf, *ipbuf;

    send_umode(NULL, sptr, 0, SEND_UMODES, ubuf);

    if (!*ubuf) {
	ubuf[0] = '+';
	ubuf[1] = '\0';
    }
    hash_check_watch(sptr, RPL_LOGON);

    if (!IsClientIpv6(sptr))
        li = htonl(IN4_ADDR(sptr->ip));

    if (HasID(sptr)) {
	sidbuf = base64enc_r(sptr->user->servicestamp, r_sidbuf);
	if (IsClientIpv6(sptr)) {
	    diff = '%';
	    ipbuf = sptr->hostip;
	} else {
	    diff = ':';
	    ipbuf = base64enc_r(IN4_ADDR(sptr->ip), r_ipbuf);
	}
	sendto_flag_serv_butone(cptr, CAPAB_IDENT, CAPAB_NICKIP, NULL,
				TOK1_CLIENT,
				"%s %d %T %s %s %s %s %d !%s%c%s %s :%s", nick,
				sptr->hopcount + 1, sptr, ubuf,
				user->username, user->host, user->fakehost,
				sptr->lang, sptr->id.string, diff, ipbuf, sidbuf, 
				sptr->info);
	sendto_flag_serv_butone(cptr, CAPAB_NICKIP, CAPAB_IDENT, NULL,
				TOK1_NICK,
				"%s %d %T %s %s %s %s %s %lu %lu :%s", nick,
				sptr->hopcount + 1, sptr, ubuf,
				user->username, user->host, user->fakehost,
				user->server, sptr->user->servicestamp,
				li, sptr->info);
    } else {
	sendto_serv_butone(cptr, NULL, TOK1_NICK,
			   "%s %d %T %s %s %s %s %s %lu %lu :%s",
			   nick, sptr->hopcount + 1, sptr, ubuf,
			   user->username, user->host, user->fakehost,
			   user->server, sptr->user->servicestamp, li,
			   sptr->info);
    }
    sendto_service(SERVICE_SEE_RNICKS, 0, NULL, NULL, TOK1_NICK,
		   "%s %d %T %s %s %s %s %s %lu :%s", nick,
		   sptr->hopcount + 1, sptr, ubuf,
		   user->username, user->host, user->fakehost,
		   user->server, sptr->user->servicestamp, sptr->info);

    /* FIXME: This can make a user identify to any NickServ -TimeMr14C */

    if (MyClient(sptr)) {
        if (sptr->nspass[0]
            && (nsptr = find_person(ServicesConf.nickserv))) {
            sendto_one_server(nsptr, sptr, TOK1_PRIVMSG, "%s@%s :SIDENTIFY %s",
                              ServicesConf.nickserv,
                              ServicesConf.services_name, sptr->nspass);
        }

        memset(sptr->passwd, '\0', PASSWDLEN);

    }

    if (MyClient(cptr) && ubuf[1])
	send_umode(cptr, sptr, 0, SEND_UMODES, ubuf);
    return 0;
}

static int do_sendnick_TS(struct hook_data *thisdata)
{
    struct Client *cptr = thisdata->client_p;
    struct Client *acptr = thisdata->source_p;
    char ubuf[32];
    unsigned long li = 0;
    char diff = '\0';
    char r_sidbuf[8], r_ipbuf[8];
    char *sidbuf, *ipbuf;

    if (IsPerson(acptr)) {
	send_umode(NULL, acptr, 0, SEND_UMODES, ubuf);
	if (!*ubuf) {		/* trivial optimization - Dianora */
	    ubuf[0] = '+';
	    ubuf[1] = '\0';
	}
        if (!IsClientIpv6(acptr))
            li = htonl(IN4_ADDR(acptr->ip));

	if (IsIDCapable(cptr) && HasID(acptr)) {
	    sidbuf = base64enc_r(acptr->user->servicestamp, r_sidbuf);
            if (IsClientIpv6(acptr)) {
            	diff = '%';
            	ipbuf = acptr->hostip;
            } else {
            	diff = ':';
            	ipbuf = base64enc_r(IN4_ADDR(acptr->ip), r_ipbuf);
            }
	    sendto_one_server(cptr, NULL, TOK1_CLIENT,
			      "%s %d %T %s %s %s %s %d !%s%c%s %s :%s",
			      acptr->name, acptr->hopcount + 1,
			      acptr, ubuf, acptr->user->username,
			      acptr->user->host, acptr->user->fakehost, 
			      acptr->lang, acptr->id.string, diff, ipbuf, sidbuf, acptr->info);
	} else {
	    sendto_one_server(cptr, NULL, TOK1_NICK,
			      "%s %d %T %s %s %s %s %s %lu %lu :%s",
			      acptr->name, acptr->hopcount + 1, acptr, ubuf,
			      acptr->user->username,
			      acptr->user->host,
			      acptr->user->fakehost,
			      acptr->user->server,
			      acptr->user->servicestamp, li, 
			      acptr->info);
	}
    }
    return 0;
}

static int do_inform_remote_servers(struct hook_data *thisdata)
{
    struct Client *bclient_p;
    dlink_node *ptr;
    struct Client *client_p = thisdata->client_p;
    struct Client *target_p = thisdata->aclient_p;
    char *name = thisdata->name;
    struct ConfItem *aconf;

    for (ptr = serv_list.head; ptr; ptr = ptr->next) {
	bclient_p = ptr->data;

	if (bclient_p == client_p)
	    continue;
	if (!(aconf = bclient_p->serv->nline)) {
	    sendto_gnotice("Lost N-line for %s on %s. Closing",
			   get_client_name(client_p, HIDE_IP), name);
	    return exit_client(client_p, client_p, "Lost N line");
	}
	if (IsIDCapable(bclient_p)) {
	    if (HasID(target_p)) {
		sendto_one_server(bclient_p, target_p->servptr,
				  TOK1_SERVER, "%C %d %c%c%c !%s :%s",
				  target_p, target_p->hopcount + 1,
				  (WillHideName(target_p) ? 'H' : 'N'),
				  (IsULine(target_p) ? 'U' : 'N'),
				  (IsHub(target_p) ? 'R' : 'N'),
				  target_p->id.string, target_p->info);
	    } else {
		sendto_one_server(bclient_p, target_p->servptr,
				  TOK1_SERVER, "%C %d %c%c%c :%s",
				  target_p, target_p->hopcount + 1,
				  (WillHideName(target_p) ? 'H' : 'N'),
				  (IsULine(target_p) ? 'U' : 'N'),
				  (IsHub(target_p) ? 'R' : 'N'), target_p->info);
	    }
	} else {
	    sendto_one_server(bclient_p, target_p->servptr,
			      TOK1_SERVER, "%C %d :%s",
			      target_p, target_p->hopcount + 1, target_p->info);
	}

    }
    return 0;
}

static int do_inform_other_servers(struct hook_data *thisdata)
{
    struct Client *cptr = thisdata->client_p;

    if (HasID(cptr)) {
	sendto_flag_serv_butone(cptr, CAPAB_IDENT|CAPAB_HIDENAME, 0, &me, TOK1_SERVER,
				"%C 2 %c%c%c !%s :%s", cptr,
				(WillHideName(cptr) ? 'H' : 'N'),
				(IsULine(cptr) ? 'U' : 'N'),
				(IsHub(cptr) ? 'R' : 'N'), cptr->id.string, cptr->info);
	sendto_flag_serv_butone(cptr, 0, CAPAB_IDENT|CAPAB_HIDENAME, &me, TOK1_SERVER,
				"%C 2 :%s", cptr, cptr->info);
    } else {
	sendto_flag_serv_butone(cptr, CAPAB_IDENT|CAPAB_HIDENAME, 0, &me, TOK1_SERVER,
				"%C 2 %c%c%c :%s", cptr,
				(WillHideName(cptr) ? 'H' : 'N'),
				(IsULine(cptr) ? 'U' : 'N'), (IsHub(cptr) ? 'R' : 'N'), cptr->info);
	sendto_flag_serv_butone(cptr, 0, CAPAB_IDENT|CAPAB_HIDENAME, &me, TOK1_SERVER,
				"%C 2 :%s", cptr, cptr->info);
    }
    return 0;
}

static int do_continue_server_estab(struct hook_data *thisdata)
{
    struct Client *client_p = thisdata->client_p;
    struct Client *target_p;

    for (target_p = &me; target_p; target_p = target_p->prev) {
	/* target_p->from == target_p for target_p == client_p */
	if (target_p->from == client_p)
	    continue;
	if (IsServer(target_p)) {
	    if (IsIDCapable(client_p)) {
		if (HasID(target_p)) {
		    sendto_one_server(client_p, target_p->servptr,
				      TOK1_SERVER, "%C %d %c%c%c !%s :%s",
				      target_p, target_p->hopcount + 1,
				      (WillHideName(target_p) ? 'H' : 'N'),
				      (IsULine(target_p) ? 'U' : 'N'),
				      (IsHub(target_p) ? 'R' : 'N'),
				      target_p->id.string, target_p->info);
		} else {
		    sendto_one_server(client_p, target_p->servptr,
				      TOK1_SERVER, "%C %d %c%c%c :%s",
				      target_p, target_p->hopcount + 1,
				      (WillHideName(target_p) ? 'H' : 'N'),
				      (IsULine(target_p) ? 'U' : 'N'),
				      (IsHub(target_p) ? 'R' : 'N'), target_p->info);
		}
	    } else {
		sendto_one_server(client_p, target_p->servptr,
				  TOK1_SERVER, "%C %d :%s",
				  target_p, target_p->hopcount + 1, target_p->info);
	    }
	}
    }
    return 0;
}

static int do_start_server_estab(struct hook_data *thisdata)
{
    struct Client *client_p = thisdata->client_p;
    struct ConfItem *aconf = thisdata->confitem;
    int emptypass = thisdata->check;
    int dontwantflags = 0;

    if (!emptypass)
	sendto_one(client_p, "%s %s :TS7", MSG_PASS, aconf->spasswd);

#ifdef HAVE_ENCRYPTION_ON
    dontwantflags |= (IsConfEncrypted(aconf) ? 0 : CAPAB_DKEY);
#endif

    send_capab_to(client_p, dontwantflags);
    sendto_one_server(client_p, NULL, TOK1_MYID, "!%s", me.id.string);
    sendto_one_server(client_p, NULL, TOK1_SERVER, "%C 1 :%s", &me, me.info);
    sendto_one_server(client_p, NULL, TOK1_SVINFO, "%d %d 0 :%lu",
		      TS_CURRENT, TS_MIN, (unsigned long) timeofday);
    return 0;
}

static int do_connect_to_server(struct hook_data *thisdata)
{
#ifdef HAVE_ENCRYPTION_ON
    struct Client *client_p = thisdata->client_p;
#endif

    do_start_server_estab(thisdata);

#ifdef HAVE_ENCRYPTION_ON
    if (!CanDoDKEY(client_p) || !WantDKEY(client_p));
    else {
	SetNegoServer(client_p);	/* VERY IMPORTANT THAT THIS IS HERE */
	sendto_one(client_p, "DKEY START");
    }
#endif
    return 0;
}
