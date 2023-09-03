/************************************************************************
 *   IRC - Internet Relay Chat, include/proxy.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
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
 * $Id: proxy.h,v 1.3 2003/06/14 13:55:50 tr-ircd Exp $
 *
 */

#ifndef PROXY_H
#define PROXY_H 1

#include "hook.h"

extern int pfd;
extern struct Client *qcptr;

extern int pm_shutdown, pm_exited, cmd_scanner_active;
extern int pm_sleep(unsigned long);
extern unsigned long pm_scan_counter;

extern int do_rehash_pm(struct hook_data *data);

extern char *protocol_types[8];

extern OPM_T *IproxyScan;
extern OPM_T *CproxyScan;

extern pthread_mutex_t pm_mutex, pm_exit_mutex;
extern pthread_mutexattr_t pm_mutex_attributes;
extern pthread_cond_t pm_cond, pm_exit_cond;
extern pthread_condattr_t pm_cond_attributes;
extern pthread_t pm_pthread;
extern pthread_attr_t pm_pthread_attributes;

extern void initialize_proxy_monitor(void);
extern void initialize_proxy_monitor_interface(void);
extern void deinitialize_proxy_monitor(void);
extern void deinitialize_proxy_monitor_interface(void);

extern void reconfigure_proxy_monitor(OPM_T *);
extern void read_proxy_configuration(void);

extern void display_proxy_scanner_statistics(aClient *);
extern void pm_action(char *, int, char *, int);

extern void *pm_main(void *);
extern void pm_addto_scanner(char *);
extern void cmd_addto_scanner(char *, char *, int);

extern int do_add_to_proxy(struct hook_data *thisdata);


/* Callback function prototypes */

void ircnotice_open_proxy(OPM_T *, OPM_REMOTE_T *, int, void *);
void ircnotice_negfail(OPM_T *, OPM_REMOTE_T *, int, void *);
void ircnotice_pm_timeout(OPM_T *, OPM_REMOTE_T *, int, void *);
void ircnotice_pm_endscan(OPM_T *, OPM_REMOTE_T *, int, void *);
void ircnotice_pm_error(OPM_T *, OPM_REMOTE_T *, int, void *);

void ircnotice_open_proxy_oper(OPM_T *, OPM_REMOTE_T *, int, void *);
void ircnotice_negfail_oper(OPM_T *, OPM_REMOTE_T *, int, void *);
void ircnotice_pm_timeout_oper(OPM_T *, OPM_REMOTE_T *, int, void *);
void ircnotice_pm_endscan_oper(OPM_T *, OPM_REMOTE_T *, int, void *);
void ircnotice_pm_error_oper(OPM_T *, OPM_REMOTE_T *, int, void *);

#endif
