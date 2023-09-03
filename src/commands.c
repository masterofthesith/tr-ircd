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
 * $Id: commands.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "msg.h"
#include "sys.h"
#include "numeric.h"
#include "h.h"
#include "s_conf.h"

struct MessageHash *msg_hash_table[MAX_MSG_HASH];

/*
 * hash
 *
 * inputs       - char string
 * output       - hash index
 * side effects - NONE
 *
 * BUGS         - This a HORRIBLE hash function
 */
static int hash(char *p)
{
    int hash_val = 0;

    while (*p) {
	hash_val += ((int) (*p) & 0xDF);
	p++;
    }

    return (hash_val % MAX_MSG_HASH);
}

/* hash_parse
 * 
 * inputs       - command name
 * output       - pointer to struct Message
 * side effects -
 */
struct Message *hash_parse(char *cmd)
{
    struct MessageHash *ptr;
    int msgindex;

    msgindex = hash(cmd);

    for (ptr = msg_hash_table[msgindex]; ptr; ptr = ptr->next) {
	if (irc_strcmp(cmd, ptr->cmd) == 0) {
	    return (ptr->msg);
	}
    }
    return NULL;
}

void clear_hash_parse()
{
    memset(msg_hash_table, 0, sizeof(msg_hash_table));
}

/* mod_add_cmd
 *
 * inputs       - command name
 *              - pointer to struct Message
 * output       - none
 * side effects - load this one command name
 *                msg->count msg->bytes is modified in place, in
 *                modules address space. Might not want to do that...
 */
void mod_add_cmd(struct Message *msg)
{
    struct MessageHash *ptr;
    struct MessageHash *last_ptr = NULL;
    struct MessageHash *new_ptr;
    int msgindex;

    assert(msg != NULL);

    msgindex = hash(msg->cmd);

    for (ptr = msg_hash_table[msgindex]; ptr; ptr = ptr->next) {
	if (irc_strcmp(msg->cmd, ptr->cmd) == 0)
	    return;		/* Its already added */
	last_ptr = ptr;
    }

    new_ptr = (struct MessageHash *) MyMalloc(sizeof(struct MessageHash));

    new_ptr->next = NULL;
    DupString(new_ptr->cmd, msg->cmd);
    new_ptr->msg = msg;

    msg->count = 0;
    msg->bytes = 0;

    if (last_ptr == NULL)
	msg_hash_table[msgindex] = new_ptr;
    else
	last_ptr->next = new_ptr;
}

/* mod_del_cmd
 *
 * inputs       - command name
 * output       - none
 * side effects - unload this one command name
 */
void mod_del_cmd(struct Message *msg)
{
    struct MessageHash *ptr;
    struct MessageHash *last_ptr = NULL;
    int msgindex;

    assert(msg != NULL);

    msgindex = hash(msg->cmd);

    for (ptr = msg_hash_table[msgindex]; ptr; ptr = ptr->next) {
	if (irc_strcmp(msg->cmd, ptr->cmd) == 0) {
	    MyFree(ptr->cmd);
	    if (last_ptr != NULL)
		last_ptr->next = ptr->next;
	    else
		msg_hash_table[msgindex] = ptr->next;
	    MyFree(ptr);
	    return;
	}
	last_ptr = ptr;
    }
}

/* mod_del_msg, similar to above, removes a message structure
 * by the given name
 * -TimeMr14C
 */

void mod_del_msg(char *string)
{
    struct MessageHash *ptr;
    struct MessageHash *last_ptr = NULL;
    int msgindex;

    if (string == NULL)
	return;

    msgindex = hash(string);

    for (ptr = msg_hash_table[msgindex]; ptr; ptr = ptr->next) {
	if (irc_strcmp(string, ptr->cmd) == 0) {
	    MyFree(ptr->cmd);
	    if (last_ptr != NULL)
		last_ptr->next = ptr->next;
	    else
		msg_hash_table[msgindex] = ptr->next;
	    MyFree(ptr);
	    return;
	}
	last_ptr = ptr;
    }
}

void report_messages(aClient *sptr)
{
    int i;
    struct MessageHash *ptr;

    for (i = 0; i < MAX_MSG_HASH; i++) {
	for (ptr = msg_hash_table[i]; ptr; ptr = ptr->next) {
	    assert(ptr->msg != NULL);
	    assert(ptr->cmd != NULL);

	    send_me_numeric(sptr, RPL_STATSCOMMANDS, ptr->cmd, ptr->msg->count, ptr->msg->bytes);
	}
    }
}

