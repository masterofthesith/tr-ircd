/************************************************************************
 *   IRC - Internet Relay Chat, include/debug.h
 *   Copyright (C) 2000 
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

extern char *strerror(int);

void report_classes(aClient *);
void report_messages(aClient *);

void send_usage(aClient *, char *);
void count_memory(aClient *, char *); 
void tstats(aClient *, char *);
void count_scache(int *, u_long *);
void count_watch_memory(int *, u_long *);
void count_whowas_memory(int *, u_long *);
void count_ip_hash(int *, u_long *);

u_long cres_mem(aClient *);


