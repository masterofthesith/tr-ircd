/************************************************************************
 *
 * s_bsd_poll.c - code implementing a poll IO loop
 *   By Adrian Chadd <adrian@creative.net.au>
 *
 * Based upon:
 *
 *   IRC - Internet Relay Chat, server/s_bsd.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
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
 *  $Id: s_bsd_poll.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "fd.h"
#include "s_bsd.h"
#include "listener.h"
#include "numeric.h"
#include "packet.h"
#include "resnew.h"
#include "s_auth.h"

#ifdef USE_POLL

/* I hate linux -- adrian */
#ifndef POLLRDNORM
#define POLLRDNORM POLLIN
#endif
#ifndef POLLWRNORM
#define POLLWRNORM POLLOUT
#endif

struct _pollfd_list {
    struct pollfd pollfds[MAXCONNECTIONS];
    int maxindex;		/* highest FD number */
};

typedef struct _pollfd_list pollfd_list_t;

pollfd_list_t pollfd_list;
pollfd_list_t httpfd_list;

static void poll_update_pollfds(int, fdlist_t, short, PF *);

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/* Private functions */

/*
 * find a spare slot in the fd list. We can optimise this out later!
 *   -- adrian
 */
static inline int poll_findslot(pollfd_list_t *list)
{
    int i;
    for (i = 0; i < MAXCONNECTIONS; i++) {
	if (list->pollfds[i].fd == -1) {
	    /* MATCH!!#$*&$ */
	    return i;
	}
    }
    assert(1 == 0);
    /* NOTREACHED */
    return -1;
}

/*
 * set and clear entries in the pollfds[] array.
 */
static void poll_update_pollfds(int fd, fdlist_t list, short event, PF * handler)
{
    fde_t *F = &fd_table[fd];
    int comm_index;
    pollfd_list_t *fd_list;

    if (list == FDLIST_IRCD)
	fd_list = &pollfd_list;
    else if (list == FDLIST_HTTPD) 
	fd_list = &httpfd_list;
    else
	fd_list = &pollfd_list;

    if (F->comm_index < 0)
	F->comm_index = poll_findslot(fd_list);

    comm_index = F->comm_index;

    /* Update the events */
    if (handler) {
	F->list = list;
	fd_list->pollfds[comm_index].events |= event;
	fd_list->pollfds[comm_index].fd = fd;
	/* update maxindex here */
	if (comm_index > fd_list->maxindex)
	    fd_list->maxindex = comm_index;
    } else {
	if (comm_index >= 0) {
	    fd_list->pollfds[comm_index].events &= ~event;
	    if (fd_list->pollfds[comm_index].events == 0) {
		fd_list->pollfds[comm_index].fd = -1;
		fd_list->pollfds[comm_index].revents = 0;
		F->comm_index = -1;
		F->list = FDLIST_NONE;

		/* update pollfd_list.maxindex here */
		if (comm_index == fd_list->maxindex)
		    while (fd_list->maxindex >= 0 && fd_list->pollfds[fd_list->maxindex].fd == -1)
			fd_list->maxindex--;
	    }
	}
    }
}

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/* Public functions */

/*
 * init_netio
 *
 * This is a needed exported function which will be called to initialise
 * the network loop code.
 */
void init_netio(void)
{
    int fd;

    memset(&pollfd_list, 0, sizeof(pollfd_list));
    memset(&httpfd_list, 0, sizeof(httpfd_list));

    for (fd = 0; fd < MAXCONNECTIONS; fd++) {
	pollfd_list.pollfds[fd].fd = -1;
	httpfd_list.pollfds[fd].fd = -1;
    }
    pollfd_list.maxindex = 0;
    httpfd_list.maxindex = 0;
}

/*
 * comm_setselect
 *
 * This is a needed exported function which will be called to register
 * and deregister interest in a pending IO state for a given FD.
 */
void
comm_setselect(int fd, fdlist_t list, unsigned int type, PF * handler,
	       void *client_data, time_t timeout)
{
    fde_t *F = &fd_table[fd];
    assert(fd >= 0);
    assert(F->flags.open);

    F->list = list;

    if (type & COMM_SELECT_READ) {
	F->read_handler = handler;
	F->read_data = client_data;
	poll_update_pollfds(fd, list, POLLRDNORM, handler);
    }
    if (type & COMM_SELECT_WRITE) {
	F->write_handler = handler;
	F->write_data = client_data;
	poll_update_pollfds(fd, list, POLLWRNORM, handler);
    }
    if (timeout)
	F->timeout = timeofday + (timeout / 1000);
}

/* int comm_select_fdlist(unsigned long delay)
 * Input: The maximum time to delay.
 * Output: Returns -1 on error, 0 on success.
 * Side-effects: Deregisters future interest in IO and calls the handlers
 *               if an event occurs for an FD.
 * Comments: Check all connections for new connections and input data
 * that is to be processed. Also check for connections with data queued
 * and whether we can write it out.
 * Called to do the new-style IO, courtesy of of squid (like most of this
 * new IO code). This routine handles the stuff we've hidden in
 * comm_setselect and fd_table[] and calls callbacks for IO ready
 * events.
 */
int comm_select(unsigned long delay, int *callbacks, fdlist_t list)
{
    int num;
    int fd;
    int ci;
    PF *hdl;
    pollfd_list_t *fd_list;

    if (list == FDLIST_IRCD)
	fd_list = &pollfd_list;
    else if (list == FDLIST_HTTPD)
	fd_list = &httpfd_list;
    else
	return 0;

    for (;;) {
	/* XXX kill that +1 later ! -- adrian */
	num = poll(fd_list->pollfds, fd_list->maxindex + 1, delay);
	if (num >= 0)
	    break;
	if (ignoreErrno(errno))
	    continue;
	/* error! */
	recheck_clock(NULL);
	return -1;
	/* NOTREACHED */
    }
    /* update current time again, eww.. */
    recheck_clock(NULL);
    *callbacks += num;

    if (num == 0)
	return 0;
    /* XXX we *could* optimise by falling out after doing num fds ... */
    for (ci = 0; ci < fd_list->maxindex + 1; ci++) {
    fde_t *F;
    int revents;
	if (((revents = fd_list->pollfds[ci].revents) == 0) || (fd_list->pollfds[ci].fd) == -1)
	    continue;
	fd = fd_list->pollfds[ci].fd;
	F = &fd_table[fd];
	if (revents & (POLLRDNORM | POLLIN | POLLHUP | POLLERR)) {
	    hdl = F->read_handler;
	    F->read_handler = NULL;
	    poll_update_pollfds(fd, F->list, POLLRDNORM, NULL);
	    if (hdl)
		hdl(fd, F->read_data);
	}
	if (revents & (POLLWRNORM | POLLOUT | POLLHUP | POLLERR)) {
	    hdl = F->write_handler;
	    F->write_handler = NULL;
	    poll_update_pollfds(fd, F->list, POLLWRNORM, NULL);
	    if (hdl)
		hdl(fd, F->write_data);
	}
    }
    return 0;
}

#endif
