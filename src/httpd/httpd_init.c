/************************************************************************
 *   IRC - Internet Relay Chat, src/httpd/httpd_init.c
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
 * $Id: httpd_init.c,v 1.3 2003/06/14 13:55:53 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "modules.h"
#include "msg.h"
#include "h.h"
#include "logtype.h"
#include "s_conf.h"
#include "event.h"
#include "httpd.h"
#include "http.h"

pthread_t httpd_pthread;
pthread_cond_t httpd_cond;
pthread_cond_t httpd_exit_cond;
pthread_attr_t httpd_pthread_attributes;
pthread_mutexattr_t httpd_mutex_attributes;
pthread_condattr_t httpd_cond_attributes;

int httpd_shutdown = 0;

/* The following 2 items need to be in that order for the mutex to
 * work correctly 
 * -TimeMr14C
 */

int httpd_exited = 0;
pthread_mutex_t httpd_exit_mutex;

#ifndef STATIC_MODULES

char *_version = "TR-IRCD httpd $Revision: 1.3 $";

void _modinit(void)
{
    hook_add_hook("read packet", (hookfn *) read_http_packet_hook);
    hook_add_hook("make listener client", (hookfn *) regain_httpd_listener);
    eventAddIsh("exit idle httpd clients", remove_exited_httpd_clients, NULL, HTTP_CLEANOUT);
    initialize_httpd();
}

void _moddeinit(void)
{
    deinitialize_httpd();
    hook_del_hook("read packet", (hookfn *) read_http_packet_hook);
    hook_del_hook("make listener client", (hookfn *) regain_httpd_listener);
    eventDelete(remove_exited_httpd_clients, NULL);
}
#else
void httpd_init(void)
{
    hook_add_hook("read packet", (hookfn *) read_http_packet_hook);
    hook_add_hook("make listener client", (hookfn *) regain_httpd_listener);
    eventAddIsh("exit idle httpd clients", remove_exited_httpd_clients, NULL, HTTP_CLEANOUT);
    initialize_httpd();
}
#endif

void initialize_httpd(void)
{

    if (pthread_attr_init(&httpd_pthread_attributes)) {
	logevent_call(LogSys.httpd_init, "Failed in initializing httpd: INIT ATTRIBUTES");
	return;
    }

    if (pthread_attr_setdetachstate(&httpd_pthread_attributes, PTHREAD_CREATE_DETACHED) != 0) {
	logevent_call(LogSys.httpd_init, "Failed in setting detachstate for the httpd thread");
	return;
    }

    if (pthread_attr_setschedpolicy(&httpd_pthread_attributes, SCHED_OTHER) != 0) {
	logevent_call(LogSys.httpd_init, "Failed in setting scheduling policy for the httpd thread");
	return;
    }

    if (pthread_attr_setinheritsched(&httpd_pthread_attributes, PTHREAD_INHERIT_SCHED) != 0) {
	logevent_call(LogSys.httpd_init, "Failed in setting interit scheduling for the httpd thread");
	return;
    }

    if (pthread_mutexattr_init(&httpd_mutex_attributes) != 0) {
	logevent_call(LogSys.httpd_init, "Failed in initializing mutex attributes");
	return;
    }

    if (pthread_mutex_init(&httpd_mutex, &httpd_mutex_attributes) != 0) {
	logevent_call(LogSys.httpd_init, "Failed in initializing httpd mutex");
	return;
    }

    if (pthread_mutex_init(&httpd_exit_mutex, &httpd_mutex_attributes) != 0) {
	logevent_call(LogSys.httpd_init, "Failed in initializing httpd exit mutex");
	return;
    }

    pthread_condattr_init(&httpd_cond_attributes);

    if (pthread_cond_init(&httpd_cond, &httpd_cond_attributes) != 0) {
	logevent_call(LogSys.httpd_init, "Failed in initializing conditional predicate");
	return;
    }

    if (pthread_cond_init(&httpd_exit_cond, &httpd_cond_attributes) != 0) {
	logevent_call(LogSys.httpd_init, "Failed in initializing conditional predicate for the exit routine");
	return;
    }

    if ((pthread_create(&httpd_pthread, &httpd_pthread_attributes, httpd_main, NULL)) != 0) {
	logevent_call(LogSys.httpd_init, "Failed in creating thread for the httpd");
	return;
    }

    return;
}

void deinitialize_httpd(void)
{
    httpd_shutdown = 1;

    if (httpd_exited == 0) {
	pthread_mutex_lock(&httpd_mutex);

	pthread_cond_signal(&httpd_cond);
	pthread_mutex_unlock(&httpd_mutex);

	pthread_mutex_lock(&httpd_exit_mutex);
	while (httpd_exited < 1)
	    pthread_cond_wait(&httpd_exit_cond, &httpd_exit_mutex);
	pthread_mutex_unlock(&httpd_exit_mutex);
    }

    pthread_mutex_destroy(&httpd_mutex);
    pthread_mutex_destroy(&httpd_exit_mutex);

    pthread_mutexattr_destroy(&httpd_mutex_attributes);

    pthread_cond_destroy(&httpd_cond);
    pthread_cond_destroy(&httpd_exit_cond);

    pthread_condattr_destroy(&httpd_cond_attributes);

    pthread_attr_destroy(&httpd_pthread_attributes);

    return;
}

