/************************************************************************
 *   IRC - Internet Relay Chat, include/init.h
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

extern int irc_sleep(unsigned long useconds);
extern int setup_ping(void);
extern int make_daemon(int);

extern void init_levs(aClient *);

extern void init_sys(void);
extern void init_bsd(void);
extern void init_send(void);
extern void setup_signals(void);
extern void setup_corefile(void);
extern void write_pidfile(void);
extern void initstats(void);
extern void initlists(void); 
extern void initwhowas(void);
extern void write_pidfile(void);
extern void init_modules(void);
extern void initialize_generalopts(void);
extern void build_version(void);
extern void server_reboot(void);

extern void restart(char *);

extern void recheck_clock(void *); 
extern void save_stats(void *unused);
extern void cleanup_zombies(void *unused);
extern void create_modelists(void *unused);
extern void check_pings(void *notused);
extern void check_splitmode(void *notused);

extern unsigned long get_vm_top(void);

