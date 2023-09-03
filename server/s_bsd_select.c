/************************************************************************
 *
 * s_bsd_select.c - code implementing a select IO loop
 *   By Adrian Chadd <adrian@creative.net.au>
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
 *  $Id: s_bsd_select.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
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

#ifdef USE_SELECT

#if MAXCONNECTIONS >= FD_SETSIZE
#error MAXCONNECTIONS must be less than FD_SETSIZE(try using poll instead of select)
#endif

/*
 * Note that this is only a single list - multiple lists is kinda pointless
 * under select because the list size is a function of the highest FD :-)
 *   -- adrian
 */

fd_set select_readfds;
fd_set select_writefds;

fd_set httpd_readfds;
fd_set httpd_writefds;

/*
 * You know, I'd rather have these local to comm_select but for some
 * reason my gcc decides that I can't modify them at all..
 *   -- adrian
 */
fd_set tmpreadfds;
fd_set tmpwritefds;

static void select_update_selectfds(int fd, fdlist_t, short event, PF * handler);

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/* Private functions */

/*
 * set and clear entries in the select array ..
 */
static void select_update_selectfds(int fd, fdlist_t list, short event, PF * handler)
{
    if (list == FDLIST_IRCD) {
	/* Update the read / write set */
	if (event & COMM_SELECT_READ) {
	    if (handler)
		FD_SET(fd, &select_readfds);
	    else
		FD_CLR(fd, &select_readfds);
	}
	if (event & COMM_SELECT_WRITE) {
	    if (handler)
		FD_SET(fd, &select_writefds);
	    else
		FD_CLR(fd, &select_writefds);
	}
    } else if (list == FDLIST_HTTPD) {
	if (event & COMM_SELECT_READ) {
	    if (handler)
		FD_SET(fd, &httpd_readfds);
	    else
		FD_CLR(fd, &httpd_readfds);
	}
	if (event & COMM_SELECT_WRITE) {
	    if (handler)
		FD_SET(fd, &httpd_writefds);
	    else
		FD_CLR(fd, &httpd_writefds);
	}
    } else if (list == FDLIST_NONE) {
	if (FD_ISSET(fd, &select_readfds))
	    FD_CLR(fd, &select_readfds);
	if (FD_ISSET(fd, &select_writefds))
	    FD_CLR(fd, &select_writefds);
	if (FD_ISSET(fd, &httpd_readfds))
	    FD_CLR(fd, &httpd_readfds);
	if (FD_ISSET(fd, &httpd_writefds))
	    FD_CLR(fd, &httpd_writefds);
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
    FD_ZERO(&select_readfds);
    FD_ZERO(&select_writefds);
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
	select_update_selectfds(fd, list, COMM_SELECT_READ, handler);
    }
    if (type & COMM_SELECT_WRITE) {
	F->write_handler = handler;
	F->write_data = client_data;
	select_update_selectfds(fd, list, COMM_SELECT_WRITE, handler);
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
 * Do IO events
 */

int comm_select(unsigned long delay, int *callbacks, fdlist_t list)
{
    int num;
    int fd;
    PF *hdl;
    fde_t *F;
    struct timeval to;

    if (list == FDLIST_IRCD) {
	/* Copy over the read/write sets so we don't have to rebuild em */
	bcopy(&select_readfds, &tmpreadfds, sizeof(fd_set));
	bcopy(&select_writefds, &tmpwritefds, sizeof(fd_set));
    } else if (list == FDLIST_HTTPD) {
	bcopy(&httpd_readfds, &tmpreadfds, sizeof(fd_set));
	bcopy(&httpd_writefds, &tmpwritefds, sizeof(fd_set));
    } else {
	return 0;
    }

    for (;;) {
	to.tv_sec = 0;
	to.tv_usec = delay * 1000;
	num = select(highest_fd + 1, &tmpreadfds, &tmpwritefds, NULL, &to);
	if (num >= 0)
	    break;
	if (ignoreErrno(errno))
	    continue;
	recheck_clock(NULL);
	/* error! */
	return -1;
	/* NOTREACHED */
    }
    *callbacks += num;
    recheck_clock(NULL);

    if (num == 0)
	return 0;

    /* XXX we *could* optimise by falling out after doing num fds ... */
    for (fd = 0; fd < highest_fd + 1; fd++) {
	F = &fd_table[fd];

	if (FD_ISSET(fd, &tmpreadfds)) {
	    hdl = F->read_handler;
	    F->read_handler = NULL;
	    select_update_selectfds(fd, F->list, COMM_SELECT_READ, NULL);
	    if (!hdl) {
		/* XXX Eek! This is another bad place! */
	    } else {
		hdl(fd, F->read_data);
	    }
	}
	if (FD_ISSET(fd, &tmpwritefds)) {
	    hdl = F->write_handler;
	    F->write_handler = NULL;
	    select_update_selectfds(fd, F->list, COMM_SELECT_WRITE, NULL);
	    if (!hdl) {
		/* XXX Eek! This is another bad place! */
	    } else {
		hdl(fd, F->write_data);
	    }
	}
    }
    return 0;
}

#endif /* USE_SELECT */
