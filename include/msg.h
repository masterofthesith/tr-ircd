/************************************************************************
 *   IRC - Internet Relay Chat, include/msg.h
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

/* $Id: msg.h,v 1.4 2003/07/01 11:01:18 tr-ircd Exp $ */

#ifndef	MSG_H
#define MSG_H 1

#include "protodef.h"
#include "protocol.h"

#define   M_SLOW              0x01   /* Command can be executed roughly    *
                                      * once per 2 seconds.                */
#define   M_IDLE_RESET	      0x02   /* command resets idle time */
#define   M_FLOOD_END	      0x04   /* command states the end of flood session */

/* A flood session is the time when a user reconnects after a disconnect,
 * and therefore he uses many commands one after the other (join/names/etc.)
 * -TimeMr14C
 */

struct Message {
   char *cmd;

   unsigned int count;          /* number of times command used */
   unsigned int parameters;
   unsigned int flags;
      
   unsigned long bytes;

   int (*func_unrg) ();		/* call in unregistered state */
   int (*func_user) ();		/* normal local user issuing this */
   int (*func_oper) ();		/* local operator issuing this */
   int (*func_srvr) ();		/* remote server issuing this */
   int (*func_srvc) ();		/* local service issuing this */		

};

struct Token {
   char *cmd;
   struct Message *msg;
};

#ifdef TOKEN
#ifndef STATIC_MODULES
#include "msgtok1.h"
#else
extern struct Token tok1_msgtab[];
#endif
#endif

#endif
