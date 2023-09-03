/************************************************************************
 *   IRC - Internet Relay Chat, src/proxymon/pm_init.c
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
 */

/*
 * $Id: pm_init.c,v 1.3 2003/06/14 13:55:53 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "modules.h"
#include "msg.h"
#include "proxy.h"
#include "h.h"
#include "logtype.h"
#include "s_conf.h"

/* We use the POSIX Thread library to create a second thread to do the
 * proxy scan task. For this, we also need an attribute structure, which
 * has to be initialized correctly.
 * -TimeMr14C
 */

pthread_t pm_pthread;
pthread_cond_t pm_cond;
pthread_cond_t pm_exit_cond;
pthread_attr_t pm_pthread_attributes;
pthread_mutexattr_t pm_mutex_attributes;
pthread_condattr_t pm_cond_attributes;

int proxymon_pthread_succeed = 0;

/* The following 2 items need to be in that order for the mutex to
 * work correctly 
 * -TimeMr14C
 */

int pm_exited = 0;
pthread_mutex_t pm_exit_mutex;

static struct Message _msgtab[] = {
    {MSG_PROXYCHK, 0, MAXPARA, M_SLOW | M_IDLE_RESET, 0L,
     m_unregistered, m_permission, m_proxychk, m_ignore, m_ignore}
};

#ifndef STATIC_MODULES

char *_version = "Proxy Monitor $Revision: 1.3 $";

void _modinit(void)
{
    initialize_proxy_monitor();
    initialize_proxy_monitor_interface();
}

void _moddeinit(void)
{
    deinitialize_proxy_monitor();
    deinitialize_proxy_monitor_interface();
}
#else
void proxymon_init(void)
{
    initialize_proxy_monitor();
    initialize_proxy_monitor_interface();
}
#endif

void initialize_proxy_monitor(void)
{
    int pm_pthread_main_return = 0;

    if (pthread_attr_init(&pm_pthread_attributes)) {
	logevent_call(LogSys.proxyevent, "Failed in initializing proxy monitor: INIT ATTRIBUTES");
	return;
    }

    if (pthread_attr_setdetachstate(&pm_pthread_attributes, PTHREAD_CREATE_DETACHED) != 0) {
	logevent_call(LogSys.proxyevent, "Failed in setting detachstate for the proxy monitor thread");
	return;
    }

    if (pthread_attr_setschedpolicy(&pm_pthread_attributes, SCHED_OTHER) != 0) {
	logevent_call(LogSys.proxyevent, "Failed in setting scheduling policy for the proxy monitor thread");
	return;
    }

    if (pthread_attr_setinheritsched(&pm_pthread_attributes, PTHREAD_INHERIT_SCHED) != 0) {
	logevent_call(LogSys.proxyevent, "Failed in setting interit scheduling for the proxy monitor thread");
	return;
    }

    if (pthread_mutexattr_init(&pm_mutex_attributes) != 0) {
	logevent_call(LogSys.proxyevent, "Failed in initializing mutex attributes");
	return;
    }

    if (pthread_mutex_init(&pm_mutex, &pm_mutex_attributes) != 0) {
	logevent_call(LogSys.proxyevent, "Failed in initializing proxy monitor mutex");
	return;
    }

    if (pthread_mutex_init(&pm_exit_mutex, &pm_mutex_attributes) != 0) {
	logevent_call(LogSys.proxyevent, "Failed in initializing proxy monitor exit mutex");
	return;
    }

    pthread_condattr_init(&pm_cond_attributes);

    if (pthread_cond_init(&pm_cond, &pm_cond_attributes) != 0) {
	logevent_call(LogSys.proxyevent, "Failed in initializing conditional predicate");
	return;
    }

    if (pthread_cond_init(&pm_exit_cond, &pm_cond_attributes) != 0) {
	logevent_call(LogSys.proxyevent, "Failed in initializing conditional predicate for the exit routine");
	return;
    }

    /* And now... The end is near... So I face the final curtain.
     * -TimeMr14C
     */

    if ((proxymon_pthread_succeed =
	 pthread_create(&pm_pthread, &pm_pthread_attributes, pm_main,
			&pm_pthread_main_return)) != 0) {
	logevent_call(LogSys.proxyevent, "Failed in creating thread for the proxy monitor");
	return;
    }

    return;
}

void initialize_proxy_monitor_interface(void)
{
    hook_add_hook("doing rehash", (hookfn *) do_rehash_pm);
    hook_add_hook("add to proxy", (hookfn *) do_add_to_proxy);
    mod_add_cmd(_msgtab);
    return;
}

void deinitialize_proxy_monitor(void)
{
    pm_shutdown = 1;

    if (pm_exited == 0) {
	pthread_mutex_lock(&pm_mutex);
	pm_scan_counter++;
	pthread_cond_signal(&pm_cond);
	pthread_mutex_unlock(&pm_mutex);

	pthread_mutex_lock(&pm_exit_mutex);
	while (pm_exited < 1)
	    pthread_cond_wait(&pm_exit_cond, &pm_exit_mutex);
	pthread_mutex_unlock(&pm_exit_mutex);
    }

    pthread_mutex_destroy(&pm_mutex);
    pthread_mutex_destroy(&pm_exit_mutex);

    pthread_mutexattr_destroy(&pm_mutex_attributes);

    pthread_cond_destroy(&pm_cond);
    pthread_cond_destroy(&pm_exit_cond);

    pthread_condattr_destroy(&pm_cond_attributes);

    pthread_attr_destroy(&pm_pthread_attributes);

    return;
}

void deinitialize_proxy_monitor_interface(void)
{
    GeneralOpts.enable_proxymonitor = 0;
    /* This will set the old scanner function, probably the dummy one */

    hook_del_hook("doing rehash", (hookfn *) do_rehash_pm);
    hook_del_hook("add to proxy", (hookfn *) do_add_to_proxy);
    mod_del_cmd(_msgtab);
    return;
}
