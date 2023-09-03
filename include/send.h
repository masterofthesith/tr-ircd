/************************************************************************
 *   IRC - Internet Relay Chat, include/h.h
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
/*
 * "send.h". - Headers file.
 * 
 * all the send* functions are declared here.
 */

/* $Id: send.h,v 1.3 2003/06/14 13:55:50 tr-ircd Exp $ */

#ifndef SEND_H
#define SEND_H

/* send all queued crap to aClient */
extern void send_queued(int fd, void *data);

extern void sendto_channel_butone(aClient *one, int incflags, int exflags, aClient *from,
                      aChannel *chptr, char *token, char *pattern, ...);

extern void sendto_channel_butserv(aChannel *chptr, aClient *from, char *token,
				   int flags, char *pattern, ...);
extern void sendto_channel_butserv_short(aChannel *chptr, aClient *from, char *token);
extern void sendto_common_channels(aClient *user, char *pattern, ...);
extern void send_quit_to_common_channels(aClient *from, char *reason);

extern int send_message(aClient *cptr, char *msg, int len);
extern void flush_connections(void *);

extern void sendto_one(aClient *to, char *pattern, ...);
extern void vsendto_one(aClient *to, char *pattern, va_list vl);

extern void send_me_notice(aClient *cptr, char *pattern, ...);
extern void send_me_numeric(aClient *cptr, int numeric, ...);
extern void send_me_debug(aClient *cptr, char *pattern, ...);
extern void send_me_desynch(aClient *cptr, aChannel *chptr, char *ev, int val);
extern void sendto_one_server(aClient *to, aClient *from, char *token, char *pattern, ...);
extern void sendto_one_person(aClient *to, aClient *from, char *token, char *pattern, ...);

extern void sendto_lev(int lev, char *pattern, ...);
extern void sendto_all(char *pattern, ...);

extern void sendto_ops(char *pattern, ...);
extern void sendto_gnotice(char *pattern, ...);
extern void sendto_users(long umode, char *pattern, ...);
extern void sendto_operators(long umode, char *type, char *pattern, ...);

extern void sendto_serv_butone(aClient *one, aClient *from, char *token, char *pattern, ...);
extern void sendto_match_servs(aChannel *chptr, aClient *from, char *token, char *format, ...);
extern void sendto_match_butone(aClient *one, char *mask, int what, aClient *from, char *pattern, ...);
extern void sendto_flag_serv_butone(aClient *one, int include, int exclude, aClient *from, 
				    char *token, char *pattern, ...);

extern void kill_client(aClient *, aClient *, char *, ...);

#endif
