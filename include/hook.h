/************************************************************************
 *   IRC - Internet Relay Chat, include/hook.h
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
 */

#ifndef HOOK_H
#define HOOK_H 1

#include "tools.h"

typedef struct
{
	char *name;
	int id;
	dlink_list hooks;
} hook;

/* we don't define the arguments to hookfn, because they can
   vary between different hooks */

typedef int (*hookfn)(void *data);

/* this is used when a hook is called by an m_function
   stand data you'd need in that situation */

struct hook_data 
{
	struct Client *client_p;
	struct Client *source_p;
  	struct Client *aclient_p;
	struct Channel *channel;
	struct ConfItem *confitem;
	struct User *user;
	int parc;
	char **parv;
	char statchar;
	char *name;
	char *mask;
	char *extra;
        char *data;
        unsigned int len;
	int check;
};


int hook_add_event(char *);
int hook_add_hook(char *, hookfn *);
int hook_call_event(int, void *);
int hook_del_event(char *);
int hook_del_hook(char *event, hookfn *fn);
void init_hooks(void);
void hook_dump(aClient *);

#endif
