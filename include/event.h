/************************************************************************
 *   IRC - Internet Relay Chat, include/event.h
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
 * event.h - defines for event.c, the event system. This has been ported
 * from squid by adrian to simplify scheduling events.
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 * $Id: event.h,v 1.3 2003/06/14 13:55:50 tr-ircd Exp $
 */
#ifndef __EVENT_H__
#define __EVENT_H__

/*
 * How many event entries we need to allocate at a time in the block
 * allocator. 16 should be plenty at a time.
 */
#define	MAX_EVENTS	50


typedef void EVH(void *);

/* The list of event processes */
struct ev_entry
{
  EVH *func;
  void *arg;
  const char *name;
  time_t frequency;
  time_t when;
  struct ev_entry *next;
  int active;
};

extern void eventAdd(const char *name, EVH *func, void *arg, time_t when);
extern void eventAddIsh(const char *name, EVH *func, void *arg, time_t delta_ish);
extern void eventDelete(EVH *func, void *arg);
extern void eventRun(void);
extern time_t eventNextTime(void);
extern void eventInit(void);
extern int eventFind(EVH *func, void *);
extern void set_back_events(time_t);
extern void show_events(struct Client *source_p);

#endif
