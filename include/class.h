/************************************************************************
 *   IRC - Internet Relay Chat, include/class.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Darren Reed
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

/* $Id: class.h,v 1.4 2003/07/01 11:01:18 tr-ircd Exp $ */

#ifndef	CLASS_H
#define CLASS_H 1

extern aClass *ConnectionClasses;

extern aClass *find_class(int);
extern int get_conf_class(aConfItem *);
extern int get_client_class(aClient *);
extern int get_client_ping(aClient *);
extern int get_con_freq(aClass *);
extern void add_class(int, int, int, int, long);
extern void check_class(void);
extern void initclass(void);
extern long get_sendq(aClient *);

extern void free_class(aClass *);

#endif
