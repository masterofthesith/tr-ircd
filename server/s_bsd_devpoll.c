/************************************************************************
 *
 * s_bsd_devpoll.c - code implementing a /dev/poll IO loop
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
 *  $Id: s_bsd_devpoll.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "fd.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "listener.h"
#include "numeric.h"
#include "packet.h"
#include "resnew.h"
#include "s_auth.h"

#ifdef USE_DEVPOLL

#define POLL_LENGTH	MAXCONNECTIONS

static int dpfd;
static int httpdpfd;
static short fdmask[POLL_LENGTH];
static void devpoll_update_events(int, fdlist_t, short, PF *);
static void devpoll_write_update(int, int, int);

/* #define NOTYET 1 */

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/* Private functions */

/*
 * Write an update to the devpoll filter.
 * See, we end up having to do a seperate (?) remove before we do an
 * add of a new polltype, so we have to have this function seperate from
 * the others.
 */
static void devpoll_write_update(int fd, int events, int dpdesc)
{
    struct pollfd pollfds[1];	/* Just to be careful */
    int retval;

    /* Build the pollfd entry */
    pollfds[0].revents = 0;
    pollfds[0].fd = fd;
    pollfds[0].events = events;

    /* Write the thing to our poll fd */
    retval = write(dpdesc, &pollfds[0], sizeof(struct pollfd));
    if (retval != sizeof(struct pollfd)) 
	logevent_call(LogSys.devpoll_write_update, errno, strerror(errno));

    /* Done! */
}

void devpoll_update_events(int fd, fdlist_t list, short filter, PF * handler)
{
    int update_required = 0;
    int cur_mask = fdmask[fd];
    PF *cur_handler;
    fdmask[fd] = 0;
    switch (filter) {
	case COMM_SELECT_READ:
	    cur_handler = fd_table[fd].read_handler;
	    if (handler)
		fdmask[fd] |= POLLRDNORM;
	    else
		fdmask[fd] &= ~POLLRDNORM;
	    if (fd_table[fd].write_handler)
		fdmask[fd] |= POLLWRNORM;
	    break;
	case COMM_SELECT_WRITE:
	    cur_handler = fd_table[fd].write_handler;
	    if (handler)
		fdmask[fd] |= POLLWRNORM;
	    else
		fdmask[fd] &= ~POLLWRNORM;
	    if (fd_table[fd].read_handler)
		fdmask[fd] |= POLLRDNORM;
	    break;
	default:
	    return;
	    break;
    }
    if (cur_handler == NULL && handler != NULL)
	update_required++;
    else if (cur_handler != NULL && handler == NULL)
	update_required++;
    if (cur_mask != fdmask[fd])
	update_required++;
    if (update_required) {
	int where;
	/*
	 * Ok, we can call devpoll_write_update() here now to re-build the
	 * fd struct. If we end up with nothing on this fd, it won't write
	 * anything.
	 */
	if (list == FDLIST_IRCD)
	    where = dpfd;
	else if (list == FDLIST_HTTPD)
	    where = httpdpfd;
	else
	    where = dpfd;

	if (fdmask[fd]) {
	    devpoll_write_update(fd, POLLREMOVE, where);
	    devpoll_write_update(fd, fdmask[fd], where);
	} else
	    devpoll_write_update(fd, POLLREMOVE, where);
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
    LogSys.devpoll_write_update =
        logevent_register("devpoll write update", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR,
                          "dpfd write failed %d: %s");
    LogSys.devpoll_netio =
        logevent_register("devpoll netio", LOG_ALWAYS, LOG_ERRORLOG | LOG_STDERR,
                          LOG_FATAL, "Could not open /dev/poll");
    LogSys.devpoll_comm_select =
        logevent_register("devpoll comm select", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR,
                          "Unhandled write event: fdmask: %x");
    LogSys.devpoll_revents =
        logevent_register("devpoll revents", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR,
                          "revents was Invalid for %d");
    memset(&fdmask, 0, sizeof(fdmask));
    dpfd = open("/dev/poll", O_RDWR);
    if (dpfd < 0) {
	logevent_call(LogSys.devpoll_netio);
	exit(115);		/* Whee! */
    }
    httpdpfd = open("/dev/poll", O_RDWR);
    if (httpdpfd < 0) {
        logevent_call(LogSys.devpoll_netio);
        exit(115);
    }
}

/*
 * comm_setselect
 *
 * This is a needed exported function which will be called to register
 * and deregister interest in a pending IO state for a given FD.
 */ void comm_setselect(int fd, fdlist_t list, unsigned int type, PF * handler,
			void *client_data, time_t timeout)
{
    fde_t *F = &fd_table[fd];
    assert(fd >= 0);
    assert(F->flags.open);

    /* Update the list, even though we're not using it .. */
    F->list = list;

    if (type & COMM_SELECT_READ) {
	devpoll_update_events(fd, list, COMM_SELECT_READ, handler);
	F->read_handler = handler;
	F->read_data = client_data;
    }
    if (type & COMM_SELECT_WRITE) {
	devpoll_update_events(fd, list, COMM_SELECT_WRITE, handler);
	F->write_handler = handler;
	F->write_data = client_data;
    }
    if (timeout)
	F->timeout = timeofday + (timeout / 1000);
}

/*
 * Check all connections for new connections and input data that is to be
 * processed. Also check for connections with data queued and whether we can
 * write it out.
 */

/*
 * comm_select
 *
 * Called to do the new-style IO, courtesy of of squid (like most of this
 * new IO code). This routine handles the stuff we've hidden in
 * comm_setselect and fd_table[] and calls callbacks for IO ready
 * events.
 */

int comm_select(unsigned long delay, int *callbacks, fdlist_t list)
{
    int num, i, where;
    struct pollfd pollfds[POLL_LENGTH];
    struct dvpoll dopoll;

    if (list == FDLIST_IRCD)
	where = dpfd;
    else if (list == FDLIST_HTTPD)
	where = httpdpfd;
    else
	where = dpfd;

    do {
	for (;;) {
	    dopoll.dp_timeout = delay;
	    dopoll.dp_nfds = POLL_LENGTH;
	    dopoll.dp_fds = &pollfds[0];
	    num = ioctl(where, DP_POLL, &dopoll);
	    if (num >= 0)
		break;
	    if (ignoreErrno(errno))
		break;
	    recheck_clock(NULL);
	    return COMM_ERROR;
	}
	recheck_clock(NULL);
	if (num == 0)
	    continue;
	*callbacks += num;

	for (i = 0; i < num; i++) {
    int fd = dopoll.dp_fds[i].fd;
    PF *hdl = NULL;
    fde_t *F = &fd_table[fd];
	    if ((dopoll.dp_fds[i].
		 revents & (POLLRDNORM | POLLIN | POLLHUP | POLLERR)) &&
		(dopoll.dp_fds[i].events & (POLLRDNORM | POLLIN))) {
		if ((hdl = F->read_handler) != NULL) {
		    F->read_handler = NULL;
		    hdl(fd, F->read_data);
		    /*
		     * this call used to be with a NULL pointer, BUT
		     * in the devpoll case we only want to update the
		     * poll set *if* the handler changes state (active ->
		     * NULL or vice versa.)
		     */
		    devpoll_update_events(fd, F->list, COMM_SELECT_READ, F->read_handler);
		} else
		    logevent_call(LogSys.devpoll_comm_select, fdmask[fd]);
	    }
	    if ((dopoll.dp_fds[i].
		 revents & (POLLWRNORM | POLLOUT | POLLHUP | POLLERR)) &&
		(dopoll.dp_fds[i].events & (POLLWRNORM | POLLOUT))) {
		if ((hdl = F->write_handler) != NULL) {
		    F->write_handler = NULL;
		    hdl(fd, F->write_data);
		    /* See above similar code in the read case */
		    devpoll_update_events(fd, F->list, COMM_SELECT_WRITE, F->write_handler);
		} else
		    logevent_call(LogSys.devpoll_comm_select, fdmask[fd]);

	    }
	    if (dopoll.dp_fds[i].revents & POLLNVAL) 
		logevent_call(LogSys.devpoll_revents, fd);
	}
	return COMM_OK;
    } while (0);
    /* XXX Get here, we broke! */
    return 0;
}

#endif /* USE_DEVPOLL */
