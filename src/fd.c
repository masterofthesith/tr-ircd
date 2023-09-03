/*
 *  ircd-hybrid: an advanced Internet Relay Chat Daemon(ircd).
 *  fdlist.c: Maintains a list of file descriptors.
 *
 *  Copyright (C) 2002 by the past and present ircd coders, and others.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 *
 *  $Id: fd.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
 */

/* 
 *
 * fdlist.c   maintain lists of file descriptors
 *
 *
 * $Id: fd.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "event.h"
#include "s_bsd.h"		/* highest_fd */
#include "numeric.h"

fde_t *fd_table = NULL;

static void fdlist_update_biggest(int fd, int opening);

/* Highest FD and number of open FDs .. */
int highest_fd = -1;		/* Its -1 because we haven't started yet -- adrian */
int number_fd = 0;

static void fdlist_update_biggest(int fd, int opening)
{
    if (fd < highest_fd)
	return;
    assert(fd < MAXCONNECTIONS);
    if (fd > highest_fd) {
	/*  
	 * assert that we are not closing a FD bigger than
	 * our known biggest FD
	 */
	assert(opening);
	highest_fd = fd;
	return;
    }
    /* if we are here, then fd == Biggest_FD */
    /*
     * assert that we are closing the biggest FD; we can't be
     * re-opening it
     */
    assert(!opening);
    while (highest_fd >= 0 && !fd_table[highest_fd].flags.open)
	highest_fd--;
}

void fdlist_init(void)
{
    static int initialized = 0;
    assert(0 == initialized);
    if (!initialized) {
	/* Since we're doing this once .. */
	fd_table = (fde_t *) MyMalloc((MAXCONNECTIONS + 1) * sizeof(fde_t));
	/* XXXX I HATE THIS CHECK. Can someone please fix? */
	if (!fd_table)
	    exit(69);
	initialized = 1;
    }
}

/* Called to open a given filedescriptor */
void fd_open(int fd, unsigned int type, const char *desc)
{
    fde_t *F = &fd_table[fd];
    assert(fd >= 0);
    if (F->flags.open) {
	fd_close(fd);
    }
    assert(!F->flags.open);
    F->fd = fd;
    F->type = type;
    F->flags.open = 1;
    fdlist_update_biggest(fd, 1);
    F->comm_index = -1;
    F->list = FDLIST_NONE;
    if (desc)
	strlcpy_irc(F->desc, desc, FD_DESC_SZ);
    number_fd++;
}

/* Called to close a given filedescriptor */
void fd_close(int fd)
{
    fde_t *F = &fd_table[fd];
    assert(F->flags.open);

    /* All disk fd's MUST go through file_close() ! */
    assert(F->type != FD_FILE);
    if (F->type == FD_FILE) {
      	assert(F->read_handler == NULL);
      	assert(F->write_handler == NULL);
    }

    comm_setselect(fd, FDLIST_NONE, COMM_SELECT_WRITE | COMM_SELECT_READ, NULL, NULL, 0);
    F->flags.open = 0;
    fdlist_update_biggest(fd, 0);
    number_fd--;
    memset(F, '\0', sizeof(fde_t));
    F->timeout = 0;
    /* Unlike squid, we're actually closing the FD here! -- adrian */
    close(fd);
}

/*
 * fd_dump() - dump the list of active filedescriptors
 */
void fd_dump(struct Client *source_p)
{
    int i;

    for (i = 0; i <= highest_fd; i++) {
	if (!fd_table[i].flags.open)
	    continue;
	send_me_numeric(source_p, RPL_FDLIST, i, fd_table[i].desc);
    }
}

/*
 * fd_note() - set the fd note
 *
 * Note: must be careful not to overflow fd_table[fd].desc when
 *       calling.
 */
void fd_note(int fd, const char *format, ...)
{
    va_list args;
    int len;

    if (format) {
	va_start(args, format);
	len = ircvsprintf(fd_table[fd].desc, format, args);
	assert(len < FD_DESC_SZ);	/* + '\0' */
	va_end(args);
    } else
	fd_table[fd].desc[0] = '\0';
}
