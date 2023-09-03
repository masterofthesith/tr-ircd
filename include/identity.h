/************************************************************************
 *   IRC - Internet Relay Chat, include/identity.h
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

extern int assign_localuser_identity(aClient *);
extern int free_localuser_identity(aClient *);
extern int init_identity(u_short);
extern aClient *find_by_base64_id(char *);
extern aClient *find_server_by_base64_id(char *, u_long *);
extern void add_userid_to_server(aClient *, aClient *);
extern int rem_userid_from_server(aClient *, aClient *);
extern int valid_base64_server_id(char *);
extern int add_base64_server(aClient *, char *);
extern void rem_base64_server(aClient *);

extern char *base64enc_r(IRCU32, char *);
extern IRCU32 base64dec(char *);

