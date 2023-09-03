/************************************************************************
 *   IRC - Internet Relay Chat, include/parse.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1992 Darren Reed
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
 *
 * "parse.h". - Headers file.
 *
 *
 * $Id: parse.h,v 1.4 2003/07/01 11:01:18 tr-ircd Exp $
 *
 */

#ifndef PARSE_H
#define PARSE_H 1

struct Message;
struct Client;

struct MessageHash
{
  char   *cmd;
  struct Message      *msg;
  struct MessageHash  *next;
}; 

#define MAX_MSG_HASH  387

extern int server_parse(struct Client *, char *, char *);
extern int user_parse(struct Client *, char *, char *);
extern void clear_hash_parse (void);
extern void mod_add_cmd(struct Message *msg);
extern void mod_del_cmd(struct Message *msg);
extern void report_messages(struct Client *);
extern void mod_del_msg(char *string);

extern struct Message *hash_parse(char *cmd);
extern struct MessageHash *msg_hash_table[MAX_MSG_HASH];

#endif
