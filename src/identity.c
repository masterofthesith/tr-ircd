/************************************************************************
 *   IRC - Internet Relay Chat, src/identity.c
 *   Copyright (C) 2000 Lucas Madar [bahamut team]
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

/* $Id: identity.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $ */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "h.h"

/* This code is mostly adapted from Bahamut Ircd */


/* 
 * 0xFFFFF is reserved as "invalid client id"
 * Don't change these without changing things in struct.h as well
 */

#define MAX_SERV_IDENTITY 0xFFF /* 4096 servers will be enough for a long, long time, can be 
                                   increased as needed */

#define MAX_USER_IDENTITY 0xFFFFE /* FFFFF is reserved for "unset id" */
#define MIN_SERV_IDENTITY 0x1     /* 0 is reserved for the server */
#define MIN_USER_IDENTITY 0x1     /* it's not fair to have a server "0" :) */

static aClient *serv_id_table[MAX_SERV_IDENTITY + 1];
static u_long last_localuser_id, userids_inuse;

/* forward declarations */
static inline char *int_to_base64(IRCU32);
static inline IRCU32 base64_to_int(char *);
static inline char *int_to_base64_r(IRCU32, char *);

static inline u_short hash_cid(u_short id)
{
    /* since our distribution is already relatively random,
     * we don't need to do anything fancy here. I hope. */

    return id % ID_MAP_SIZE;
}

static aClient *find_by_userid(aClient *server, u_short id)
{
    u_short idval = hash_cid(id);
    aClient *cptr = server->serv->userid[idval];

    while (cptr) {
	if (USERID_PART(cptr->id.id) == id)
	    return cptr;
	cptr = cptr->idhnext;
    }
    return NULL;
}

aClient *find_by_base64_id(char *b64)
{
    u_long id = base64_to_int(b64);
    u_short idval;
    aClient *cptr, *server;
    u_short serverid;
    u_short userid;

    serverid = SERVERID_PART(id);

    if (serverid > MAX_SERV_IDENTITY || !(server = serv_id_table[serverid])) {
	sendto_lev(DEBUG_LEV, "got idmsg for user on unknown serverid %u", serverid);
	return NULL;
    }

    userid = USERID_PART(id);

    idval = hash_cid(userid);
    cptr = server->serv->userid[idval];

    while (cptr) {
	if (USERID_PART(cptr->id.id) == userid)
	    return cptr;
	cptr = cptr->idhnext;
    }

    return NULL;
}

aClient *find_server_by_base64_id(char *b64, u_long *giveid)
{
    u_long id = base64_to_int(b64);
    aClient *server;
    u_short serverid;

    serverid = SERVERID_PART(id);

    if (serverid > MAX_SERV_IDENTITY || !(server = serv_id_table[serverid])) {
	sendto_lev(DEBUG_LEV, "got idmsg for user on unknown serverid %u", serverid);
	return NULL;
    }

    if (giveid)
	*giveid = id;

    return server;
}

void add_userid_to_server(aClient *server, aClient *user)
{
    u_short idval = hash_cid(USERID_PART(user->id.id));

    user->idhnext = server->serv->userid[idval];
    server->serv->userid[idval] = user;
}

/* returns nonzero on failure */
int rem_userid_from_server(aClient *server, aClient *user)
{
    u_short idval;
    aClient *cptr, *prevcptr = NULL;

    if (!server || !user)
	return 0;
    if (!server->serv)
	return 0;

    idval = hash_cid(USERID_PART(user->id.id));
    cptr = server->serv->userid[idval];

    while (cptr) {
	if (cptr == user) {
	    if (prevcptr)
		prevcptr->idhnext = cptr->idhnext;
	    else
		server->serv->userid[idval] = cptr->idhnext;
	    return 0;
	}
	prevcptr = cptr;
	cptr = cptr->idhnext;
    }

    // sendto_ops("rem_userid_from_server(%s, %s[%d]) failed!", server->name, user->name, idval);

    return -1;
}

int assign_localuser_identity(aClient *cptr)
{
    if (userids_inuse == 0xFFFFF)
	return -1;		/* all userids in use! */

    if (USERID_PART(cptr->id.id) != 0)
	return -1;

    do {
	if (++last_localuser_id > MAX_USER_IDENTITY)
	    last_localuser_id = MIN_USER_IDENTITY;
    } while (find_by_userid(&me, last_localuser_id));

    userids_inuse++;

    cptr->id.id = MAKE_ID(SERVERID_PART(me.id.id), last_localuser_id);
    add_userid_to_server(&me, cptr);
    strcpy(cptr->id.string, int_to_base64(cptr->id.id));
    return 0;
}

int free_localuser_identity(aClient *cptr)
{
    if (USERID_PART(cptr->id.id) == 0xFFFFF)
	return 0;		/* never assigned anyway.. */

    if (rem_userid_from_server(&me, cptr))
	return 0;

    userids_inuse--;

    cptr->id.id = 0xFFFFFFFF;
    cptr->id.string[0] = '\0';
    UnsetHasID(cptr);

    return 0;
}

int valid_base64_server_id(char *b64)
{
    IRCU32 val = base64_to_int(b64);

    if (USERID_PART(val) != 0)
	return 0;

    return 1;
}

void rem_base64_server(aClient *server)
{
    rem_userid_from_server(server, server);
    serv_id_table[SERVERID_PART(server->id.id)] = NULL;
    UnsetHasID(server);
}

int add_base64_server(aClient *server, char *b64)
{
    IRCU32 val = base64_to_int(b64);

    if (USERID_PART(val) != 0 || serv_id_table[SERVERID_PART(val)])
	return -1;

    strlcpy_irc(server->id.string, b64, 8);
    server->id.id = val;
    SetHasID(server);
    serv_id_table[SERVERID_PART(val)] = server;
    add_userid_to_server(server, server);
    return 0;
}

int init_identity(u_short serverid)
{
    memset(serv_id_table, 0, (MAX_SERV_IDENTITY + 1) * sizeof(aClient *));

    last_localuser_id = 0;
    userids_inuse = 2;		/* one for 0 (server message) and one for 0xffff (invalid) */

    if (serverid > MAX_SERV_IDENTITY)
	return -1;

    me.id.id = MAKE_ID(serverid, 0);
    serv_id_table[serverid] = &me;
    add_userid_to_server(&me, &me);
    strcpy(me.id.string, int_to_base64(me.id.id));
    SetHasID(&me);
    return 0;
}

char *base64enc_r(IRCU32 i, char *b)
{
    return int_to_base64_r(i, b);
}

IRCU32 base64dec(char *b64)
{
    return base64_to_int(b64);
}

/********** DO NOT CHANGE ANYTHING BELOW THIS LINE ********************/

/* ':' and '#' and '&' and '+' and '@' must never be in this table. */
/* these tables must NEVER CHANGE! >) */
char int6_to_base64_map[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
    'F',
    'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
    'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
    'l',
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{',
    '}'
};

char base64_to_int6_map[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
    -1, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, -1, 63, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

static inline char *int_to_base64(IRCU32 val)
{
    /* 32/6 == max 6 bytes for representation, 
     * +1 for the null, +1 for byte boundaries 
     */
static char base64buf[8];
IRCU32 i = 7;

    base64buf[i] = '\0';

    do {
	base64buf[--i] = int6_to_base64_map[val & 63];
    } while (val >>= 6);

    return base64buf + i;
}

static inline char *int_to_base64_r(IRCU32 val, char *base64buf)
{
    /* 32/6 == max 6 bytes for representation, 
     * +1 for the null, +1 for byte boundaries 
     */
    IRCU32 i = 7;

    base64buf[i] = '\0';

    do {
	base64buf[--i] = int6_to_base64_map[val & 63];
    } while (val >>= 6);

    return base64buf + i;
}

static inline IRCU32 base64_to_int(char *b64)
{
    unsigned int v = base64_to_int6_map[(u_char) *b64++];

    while (*b64) {
	v <<= 6;
	v += base64_to_int6_map[(u_char) *b64++];
    }

    return v;
}
