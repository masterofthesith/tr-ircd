/*
 *   IRC - Internet Relay Chat, server/s_kline.c
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
 * $Id: s_kline.c,v 1.8 2004/03/04 21:25:17 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "h.h"
#include "s_conf.h"

/* This function actually does look very long and overcomplicated 
 * But it is not.
 * Until now, many parts of this were called separately and 
 * successively a la find_zkill, find_kill, find_exclude,
 * find_is_zlined, find_is_klined, find_is_excluded
 * Most of the time these were called one after the other
 * leading to the same effect as here.
 * Therefore I chose to combine them all in the same function
 *
 * -TimeMr14C 16.11.2002
 */

int find_kill_level(aClient *cptr)
{
    char *host, *name;
    aConfItem *ret = NULL;
    aMaskItem *ami = NULL;
    if (!cptr->user)
	return 0;
    host = cptr->sockhost;
    name = cptr->user->username;
    if (strlen(host) > (size_t) HOSTLEN || (name ? strlen(name) : 0) > (size_t) HOSTLEN)
	return 0;

    ret = find_conf_by_host(cptr->hostip, CONF_CLIENT);
    if (ret && IsConfExemptKline(ret))
	return 0;

    ami = find_maskitem(host, name, MASKITEM_EXCLUDE, 1);
    if (ami)
	return 0;

    ami = find_maskitem(cptr->hostip, name, MASKITEM_EXCLUDE, 1);
    if (ami)
	return 0;

    ami = find_maskitem(cptr->hostip, NULL, MASKITEM_ZAPLINE, 1);
    if (ami)
	return MASKITEM_ZAPLINE;

    ami = find_maskitem(cptr->hostip, NULL, MASKITEM_ZAPLINE_CONFIG, 1);
    if (ami)
	return MASKITEM_ZAPLINE;

    ami = find_maskitem(host, name, MASKITEM_AUTOKILL, 1);
    if (ami)
	return MASKITEM_AUTOKILL;

    ami = find_maskitem(host, name, MASKITEM_KLINE, 1);
    if (ami)
	return MASKITEM_KLINE;

    ami = find_maskitem(host, name, MASKITEM_KLINE_CONFIG, 1);
    if (ami)
	return MASKITEM_KLINE;

    if (ServerOpts.use_regex) {
    char arr[USERLEN + HOSTLEN + 2];
	ircsprintf(arr, "%s@%s", cptr->user->username ? cptr->user->username : "*", host);
	ami = find_maskitem(arr, name, MASKITEM_KLINE_REGEX, 1);
	if (ami)
	    return MASKITEM_KLINE;
    }

    ami = find_maskitem(cptr->hostip, name, MASKITEM_AUTOKILL, 1);
    if (ami)
	return MASKITEM_AUTOKILL;

    ami = find_maskitem(cptr->hostip, name, MASKITEM_KLINE, 1);
    if (ami)
	return MASKITEM_KLINE;

    ami = find_maskitem(cptr->hostip, name, MASKITEM_KLINE_CONFIG, 1);
    if (ami)
	return MASKITEM_KLINE;

    if (ServerOpts.use_regex) {
    char arr[USERLEN + HOSTLEN + 2];
	ircsprintf(arr, "%s@%s", cptr->user->username ? cptr->user->username : "*", cptr->hostip);
	ami = find_maskitem(arr, name, MASKITEM_KLINE_REGEX, 1);
	if (ami)
	    return MASKITEM_KLINE;
    }

    return 0;
}

int find_connection_kill_level(struct Client *client_p, char *username)
{
    struct MaskItem *kconf;

    if ((kconf = find_maskitem(client_p->hostip, username, MASKITEM_KLINE, 1)) != NULL) {
	send_me_numeric(client_p, ERR_YOUREBANNEDCREEP, "klined");
	send_me_notice(client_p, ":*** You are not welcome on this server");
	send_me_notice(client_p, ":*** Klined for %s", kconf->reason);
	send_me_notice(client_p, ":*** Your hostmask is %M", kconf);
	send_me_notice(client_p,
		       ":*** For more information, please mail %s and include everything shown here.",
		       ServerOpts.server_kline_address);
	return (BANNED_CLIENT);
    }
    if ((kconf = find_maskitem(client_p->hostip, username, MASKITEM_KLINE_CONFIG, 1)) != NULL) {
	send_me_numeric(client_p, ERR_YOUREBANNEDCREEP, "klined");
	send_me_notice(client_p, ":*** You are not welcome on this server");
	send_me_notice(client_p, ":*** Klined for %s", kconf->reason);
	send_me_notice(client_p, ":*** Your hostmask is %M", kconf);
	send_me_notice(client_p,
		       ":*** For more information, please mail %s and include everything shown here.",
		       ServerOpts.server_kline_address);
	return (BANNED_CLIENT);
    }
    if ((kconf = find_maskitem(client_p->hostip, username, MASKITEM_AUTOKILL, 1)) != NULL) {
	send_me_numeric(client_p, ERR_YOUREBANNEDCREEP, "autokill");
	send_me_notice(client_p, ":*** You are not welcome on this network");
	send_me_notice(client_p, ":*** Autokill for %s", kconf->reason);
	send_me_notice(client_p, ":*** Your hostmask is %M", kconf);
	send_me_notice(client_p,
		       ":*** For more information, please mail %s and include everything shown here.",
		       ServerOpts.network_kline_address);
	return (BANNED_CLIENT);
    }
    if (ServerOpts.use_regex &&
	((kconf = find_maskitem(client_p->hostip, username, MASKITEM_KLINE_REGEX, 1)) != NULL)) {
	send_me_numeric(client_p, ERR_YOUREBANNEDCREEP, "klined");
	send_me_notice(client_p, ":*** You are not welcome on this server");
	send_me_notice(client_p, ":*** Klined for %s", kconf->reason);
	send_me_notice(client_p, ":*** Your hostmask is %M", kconf);
	send_me_notice(client_p,
		       ":*** For more information, please mail %s and include everything shown here.",
		       ServerOpts.server_kline_address);
	return (BANNED_CLIENT);
    }
    if ((kconf = find_maskitem(client_p->sockhost, username, MASKITEM_KLINE, 1)) != NULL) {
	send_me_numeric(client_p, ERR_YOUREBANNEDCREEP, "klined");
	send_me_notice(client_p, ":*** You are not welcome on this server");
	send_me_notice(client_p, ":*** Klined for %s", kconf->reason);
	send_me_notice(client_p, ":*** Your hostmask is %M", kconf);
	send_me_notice(client_p,
		       ":*** For more information, please mail %s and include everything shown here.",
		       ServerOpts.server_kline_address);
	return (BANNED_CLIENT);
    }
    if ((kconf = find_maskitem(client_p->sockhost, username, MASKITEM_KLINE_CONFIG, 1)) != NULL) {
	send_me_numeric(client_p, ERR_YOUREBANNEDCREEP, "klined");
	send_me_notice(client_p, ":*** You are not welcome on this server");
	send_me_notice(client_p, ":*** Klined for %s", kconf->reason);
	send_me_notice(client_p, ":*** Your hostmask is %M", kconf);
	send_me_notice(client_p,
		       ":*** For more information, please mail %s and include everything shown here.",
		       ServerOpts.server_kline_address);
	return (BANNED_CLIENT);
    }
    if ((kconf = find_maskitem(client_p->sockhost, username, MASKITEM_AUTOKILL, 1)) != NULL) {
	send_me_numeric(client_p, ERR_YOUREBANNEDCREEP, "autokill");
	send_me_notice(client_p, ":*** You are not welcome on this network");
	send_me_notice(client_p, ":*** Autokill for %s", kconf->reason);
	send_me_notice(client_p, ":*** Your hostmask is %M", kconf);
	send_me_notice(client_p,
		       ":*** For more information, please mail %s and include everything shown here.",
		       ServerOpts.network_kline_address);
	return (BANNED_CLIENT);
    }
    if (ServerOpts.use_regex &&
	((kconf = find_maskitem(client_p->sockhost, username, MASKITEM_KLINE_REGEX, 1)) != NULL)) {
	send_me_numeric(client_p, ERR_YOUREBANNEDCREEP, "klined");
	send_me_notice(client_p, ":*** You are not welcome on this server");
	send_me_notice(client_p, ":*** Klined for %s", kconf->reason);
	send_me_notice(client_p, ":*** Your hostmask is %M", kconf);
	send_me_notice(client_p,
		       ":*** For more information, please mail %s and include everything shown here.",
		       ServerOpts.server_kline_address);
	return (BANNED_CLIENT);
    }
    return 0;
}
