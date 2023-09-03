/************************************************************************
 *   IRC - Internet Relay Chat, src/httpd/httpd_main.c
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
 * $Id: httpd_main.c,v 1.6 2004/02/24 15:00:30 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "logtype.h"
#include "s_conf.h"
#include "dh.h"
#include "listener.h"
#include "httpd.h"
#include "http.h"

int httpd_callbacks = 0;

pthread_mutex_t httpd_mutex;

char *herrortab[505];

void setup_http_signals(void);

static inline void free_http_listeners()
{
struct Listener *listener = NULL;
struct Listener *listener_next = NULL;

    for (listener = ListenerPollList; listener; listener = listener_next) {
	listener_next = listener->next;
	if (listener->flags & LISTENER_HTTP)
	    free_listener(listener);
    }
}

void *httpd_main(void *data)
{

    int http_empty_cycles = 0, st = 0;

    if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) {
	logevent_call(LogSys.httpd_init, "Unable to set cancel state for the httpd thread");
	pthread_exit(0);
    }

    if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0) {
	logevent_call(LogSys.httpd_init, "Unable to set cancel type for the httpd thread");
	pthread_exit(0);
    }

    configure_httpd();

    configure_httpd_errors();

    setup_http_signals();

    memset(&hclient_list, 0, sizeof(hclient_list));
    hclient_list.head = hclient_list.tail = NULL;

    while (1) {

	if (httpd_callbacks > 0)
	    http_empty_cycles = 0;

	else
	    http_empty_cycles++;

	/* Reset the callback counter... */
	httpd_callbacks = 0;
	st = (http_empty_cycles + 1) * 15000;
	if (st > 250000)
	    st = 250000;

	irc_sleep(st);		/* we can use the same sleep function here */

	comm_select(0, &httpd_callbacks, FDLIST_HTTPD);

	if (httpd_shutdown) {

	    sendto_ops("Exiting httpd");

	    pthread_mutex_lock(&httpd_exit_mutex);
	    httpd_exited = 1;
	    pthread_cond_signal(&httpd_exit_cond);
	    pthread_mutex_unlock(&httpd_exit_mutex);
	    remove_exited_httpd_clients(NULL);
  	    remove_remaining_httpd_clients(NULL);

	    pthread_exit(0);

	    return NULL;
	}

    }

    return NULL;

}

int exit_httpd_client(aClient *exit_ptr)
{
    if (exit_ptr->fd >= 0) {
	if (!IsDead(exit_ptr))
	    send_queued(exit_ptr->fd, exit_ptr);
#ifdef HAVE_ENCRYPTION_ON
	/* Close the ssl connection also */
	if (IsSSL(exit_ptr)) {
	    SSL_set_shutdown(exit_ptr->ssl, SSL_RECEIVED_SHUTDOWN);
	    SSL_smart_shutdown(exit_ptr->ssl);
	    SSL_free(exit_ptr->ssl);
	    exit_ptr->ssl = NULL;
	}
#endif
	fd_close(exit_ptr->fd);
	exit_ptr->fd = -1;
	exit_ptr->flags |= FLAGS_DEADSOCKET;
    }

    delete_adns_queries(exit_ptr->dns_query);

    exit_ptr->flags |= FLAGS_CLOSING;

    linebuf_donebuf(&exit_ptr->sendQ);
    linebuf_donebuf(&exit_ptr->recvQ);
    SetExited(exit_ptr);
    return CLIENT_EXITED;
}

void remove_exited_httpd_clients(void *notused)
{
    dlink_node *ptr, *prev_ptr;
    aClient *cptr;
    for (ptr = hclient_list.tail; ptr; ptr = prev_ptr) {
	prev_ptr = ptr->prev;
	cptr = ptr->data;
	if (!cptr)
	    continue;
	if ((time(NULL) - cptr->lasttime) > HTTP_TIMEOUT) {
            dlink_node *tptr;
            tptr = dlinkFind(&unknown_list, cptr);
            if (tptr)
                dlinkDeleteNode(tptr, &unknown_list);
	    if (!IsExited(cptr))
	        exit_httpd_client(cptr);
	    dlinkDelete(ptr, &hclient_list);
	    free_client(cptr);
	}
    }
}

void remove_remaining_httpd_clients(void *notused)
{
    dlink_node *ptr, *prev_ptr, *kptr;
    aClient *cptr;
    for (ptr = hclient_list.tail; ptr; ptr = prev_ptr) {
        prev_ptr = ptr->prev;
        cptr = ptr->data;
        if (!cptr)
            continue;
        if (!IsExited(cptr))
            exit_httpd_client(cptr);
        dlinkDelete(ptr, &hclient_list);
        kptr = dlinkFind(&unknown_list, cptr);
        if (kptr)
            dlinkDeleteNode(kptr, &unknown_list);
        free_client(cptr);
    }
}


int regain_httpd_listener(struct hook_data *thisdata)
{
    aClient *acptr;
    char *host = thisdata->client_p->hostip;
    dlink_node *ptr;

    if (!host)
	return 0;


    for (ptr = hclient_list.head; ptr; ptr = ptr->next) {
	acptr = ptr->data;
	if (!acptr)
	    continue;
	if (irc_strcmp(acptr->hostip, host) == 0) {
	    thisdata->client_p = acptr;
	    return 1;
	}
    }
    return 0;
}

static char *months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char *weekdays[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

char *httpdate(time_t ac)
{
static char buf[80];
struct tm *lt = localtime(&ac);

    ircsprintf(buf, "%s, %d %s %04d %02d:%02d:%02d GMT",
	       weekdays[lt->tm_wday], lt->tm_mday, months[lt->tm_mon],
	       lt->tm_year + 1900, lt->tm_hour, lt->tm_min, lt->tm_sec);

    return buf;
}

static void dummy_handler(int sig)
{
    /* Empty */
}

static void sighup_handler(int sig)
{
    configure_httpd();
}    

void setup_http_signals(void)
{
struct sigaction act;

    act.sa_flags = 0;
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGPIPE);
    sigaddset(&act.sa_mask, SIGALRM);

# ifdef SIGWINCH
    sigaddset(&act.sa_mask, SIGWINCH);
    sigaction(SIGWINCH, &act, 0);
# endif
    act.sa_handler = dummy_handler;
    sigaction(SIGPIPE, &act, 0);

    act.sa_handler = dummy_handler;
    sigaction(SIGALRM, &act, 0);

    act.sa_handler = sighup_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGHUP);
    sigaction(SIGHUP, &act, 0);

}
