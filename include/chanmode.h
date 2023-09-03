/************************************************************************
 *   IRC - Internet Relay Chat, include/chanmode.h
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
 *
 */

#ifndef	CHANMODE_H
#define CHANMODE_H 1

#include "tools.h"

struct ChanMode {

   /* char letter; - Is not used, will be taken from the index of the table */

   int type;
   int in_use;
   int flags;
   int cmtype;

   int (*cmode_handler_user) ();
   int (*cmode_handler_oper) ();
   int (*cmode_handler_ulined) ();
   int (*cmode_handler_server) ();
   int (*cmode_handler_service) ();
};

extern char *make_nick_user_host(char *, char *, char *);

extern void send_mode_burst(aClient *, aChannel *, char);
extern void remove_matching_nuhs(aChannel *, aClient *, aClient *, dlink_list *, char);
extern void remove_nuh_list(aClient *, aChannel *, dlink_list *, char);
extern void send_nuh_list(aClient *, aChannel *, dlink_list *, char, char *, char *);

extern aNUH *nick_is_nuhed(aChannel *, char *, aClient *, dlink_list *);
extern aNUH *is_nuhed(aClient *, dlink_list *);

extern int add_id(aClient *, aChannel *, char *, int, dlink_list *);
extern int del_id(char *, dlink_list *);

extern void link_add_server_to_channel(aClient *cptr, aChannel *chptr);
extern void link_remove_users_from_channel(aChannel *chptr);
extern void link_set_modes_in_channel(aChannel *chptr);

extern int find_cmode(int , unsigned char *, int);

#ifdef CMODE_MODULAR
#ifndef STATIC_MODULES
#include "cmodetab.h"
#else
extern struct ChanMode modetab[];
#endif                 
#endif 

#endif

