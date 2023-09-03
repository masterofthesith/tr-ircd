/************************************************************************
 *   IRC - Internet Relay Chat, server/s_packet.c
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
 *
 *   $Id: s_packet.c,v 1.6 2004/07/14 14:26:04 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "s_bsd.h"
#include "parse.h"
#include "fd.h"
#include "packet.h"
#include "zlink.h"
#include "dh.h"
#include "s_conf.h"

static char readBuf[READBUF_SIZE];

int client_dopacket(aClient *cptr, char *buffer, size_t length);
int server_dopacket(aClient *cptr, char *buffer, size_t length);
int binary_dopacket(aClient *cptr, char *buffer, size_t length);

void init_dopacket(void *unused)
{
    memset(readBuf, 0, READBUF_SIZE);
    return;
}

void parse_client_queued(aClient *client_p)
{
    int dolen = 0;
    int fd_r;

    /* 
     * Handle flood protection here - if we exceed our flood limit on
     * messages in this loop, we simply drop out of the loop prematurely.
     *   -- adrian
     */
    for (;;) {
	if (!IsAnOper(client_p) && (client_p->sent_parsed > client_p->allow_read))
	    break;
	dolen = linebuf_get(&client_p->recvQ, readBuf, READBUF_SIZE,
			    LINEBUF_COMPLETE, LINEBUF_PARSED);
	if (!dolen)
	    break;
	if (client_dopacket(client_p, readBuf, dolen) == -2)
	    return;
#if 0
	client_p->sent_parsed++;
#endif
    }

    /* server fd may have changed */
    fd_r = client_p->fd;

    if (!IsDead(client_p)) {
        /* If we get here, we need to register for another COMM_SELECT_READ */
        if (IsServer(client_p) || IsNegoServer(client_p))
            comm_setselect(fd_r, FDLIST_IRCD, COMM_SELECT_READ, read_server_packet, client_p, 0);
        else
            comm_setselect(fd_r, FDLIST_IRCD, COMM_SELECT_READ, read_client_packet, client_p, 0);
    }

}

void read_client_packet(int fd, void *data)
{
    aClient *client_p = data;
    int length = 0;
    int lbuf_len;
    int fd_r = client_p->fd;
    int binary = 0;

    /* if the client is dead, kill it off now -davidt */
    if (IsDead(client_p)) {
	exit_client(client_p, &me,
		    (client_p->flags & FLAGS_SENDQEX) ? "SendQ exceeded" : "Dead socket");
	return;
    }

    /*
     * Read some data. We *used to* do anti-flood protection here, but
     * I personally think it makes the code too hairy to make sane.
     *     -- adrian
     */

    memset(readBuf, 0, READBUF_SIZE);

#ifdef HAVE_ENCRYPTION_ON
    if (IsSSL(client_p))
	length = safe_SSL_read(client_p, readBuf, READBUF_SIZE);
    else
#endif
	length = read(fd_r, readBuf, READBUF_SIZE);

    if (length <= 0) {
	if ((length == -1) && ignoreErrno(errno)) {
	    comm_setselect(fd_r, FDLIST_IRCD, COMM_SELECT_READ, read_client_packet, client_p, 0);
	    return;
	}
	dead_link_on_read(client_p, length);
	return;
    }

    if (client_p->lasttime < timeofday)
	client_p->lasttime = timeofday;
    if (client_p->lasttime > client_p->since)
	client_p->since = timeofday;
    client_p->flags &= ~FLAGS_PINGSENT;

    /*
     * Before we even think of parsing what we just read, stick
     * it on the end of the receive queue and do it when its
     * turn comes around.
     */

    if (IsServer(client_p) || IsNegoServer(client_p) || DoZipThis(client_p) || WantDKEY(client_p))
	binary = 1;

    if (binary) {
	binary_dopacket(client_p, readBuf, length);
    } else {

	lbuf_len = linebuf_parse(&client_p->recvQ, readBuf, length, binary);

	if (lbuf_len < 0) {
	    dead_link_on_read(client_p, 0);
	    return;
	}

	/* Check to make sure we're not flooding */
	if (IsPerson(client_p) && !IsAnOper(client_p) &&
	    (linebuf_alloclen(&client_p->recvQ) > ServerOpts.client_flood)) {
	    exit_client(client_p, client_p, "Excess Flood");
	    return;
	}

	parse_client_queued(client_p);
    }

    remove_exited_clients(NULL);
}

void read_server_packet(int fd, void *data)
{
    aClient *client_p = data;
    int length = 0;
    int fd_r = client_p->fd;

    /* if the client is dead, kill it off now -davidt */
    if (IsDead(client_p)) {
	exit_client(client_p, &me,
		    (client_p->flags & FLAGS_SENDQEX) ? "SendQ exceeded" : "Dead socket");
	return;
    }

    /*
     * Read some data. We *used to* do anti-flood protection here, but
     * I personally think it makes the code too hairy to make sane.
     *     -- adrian
     */

    memset(readBuf, 0, READBUF_SIZE);

    length = read(fd_r, readBuf, READBUF_SIZE);
    if (length <= 0) {
	if ((length == -1) && ignoreErrno(errno)) {
	    comm_setselect(fd_r, FDLIST_IRCD, COMM_SELECT_READ, read_server_packet, client_p, 0);
	    return;
	}
	dead_link_on_read(client_p, length);
	return;
    }

    if (client_p->lasttime < timeofday)
	client_p->lasttime = timeofday;
    if (client_p->lasttime > client_p->since)
	client_p->since = timeofday;
    client_p->flags &= ~FLAGS_PINGSENT;

    binary_dopacket(client_p, readBuf, length);

    /* server fd may have changed */
    fd_r = client_p->fd;

    if (!IsDead(client_p))
	comm_setselect(fd_r, FDLIST_IRCD, COMM_SELECT_READ, read_server_packet, client_p, 0);

    remove_exited_clients(NULL);
}

static void process_bytecount(aClient *cptr, int length)
{
    ++me.receiveM;
    ++cptr->receiveM;
    cptr->receiveB += length;

    if (cptr->receiveB > 1023) {
	cptr->receiveK += (cptr->receiveB >> 10);
	cptr->receiveB &= 0x03ff;	/* 2^10 = 1024, 3ff = 1023 */
    }

    me.receiveB += length;

    if (me.receiveB > 1023) {
	me.receiveK += (me.receiveB >> 10);
	me.receiveB &= 0x03ff;
    }
}

/*
 * client_dopacket - copy packet to client buf and parse it
 *      client_p - pointer to client structure for which the buffer data
 *             applies.
 *      buffer - pointr to the buffer containing the newly read data
 *      length - number of valid bytes of data in the buffer
 *
 */

static inline void restbuf_create(aClient *cptr)
{

    if (!HasRestbuf(cptr)) {
	cptr->restbuf = MyMalloc(sizeof(char *) * READBUF_SIZE * 4);
	cptr->protoflags |= PFLAGS_HASRESTBUF;
    }
}

int binary_dopacket(aClient *cptr, char *buffer, size_t length)
{
#ifdef HAVE_ENCRYPTION_ON
    if (IsRC4IN(cptr))
	rc4_process_stream(cptr->serv->rc4_in, buffer, length);
#endif

    if (ZipIn(cptr)) {
	if (unzip_buffer(cptr, buffer, length, server_dopacket) < 0) {
	    sendto_gnotice("Unzip error for %s: (%d) %s", cptr->name, errno, buffer);
	    return exit_client(cptr, &me, "fatal error in unzip!");
	}
	return 0;
    }
    
    return server_dopacket(cptr, buffer, length);
}

int server_dopacket(aClient *cptr, char *buffer, size_t length)
{
    int broken = 1;
    int tmplen = 1;
    int parseval = 0;
    int fd_r = 0;
    char t;

    if (cptr->waitlen > 0) {
	memcpy(cptr->restbuf + cptr->waitlen, buffer, length);
	buffer = cptr->restbuf;
	length += cptr->waitlen;
	memset(cptr->restbuf + length, 0, sizeof(char) * ((READBUF_SIZE * 4) - length));
    }

    process_bytecount(cptr, length);

    /* Now we have to play the following game:
     * It is obvious, that the buffer can contain cr/lf characters. 
     * we have to parse it, as soon as we receive a \r or a \n.
     * and then we have to start parse again, for the next part of 
     * the buffer. this is done continously, until the buffer is
     * is finished. -TimeMr14C
     */

    while (length) {
	tmplen = 0;
	broken = 1;
	cptr->waitlen = 0;
	while (length) {
	    t = buffer[tmplen];
	    length--;
	    if ((t == '\n') || (t == '\r')) {
		broken = 0;
		break;
	    }
	    tmplen++;

	}
	if ((tmplen > 0) && !broken) {

	    buffer[tmplen] = '\0';

	    parseval = server_parse(cptr, buffer, buffer + tmplen);
	    buffer = buffer + tmplen + 1;

	    switch (parseval) {
		case CLIENT_EXITED:
		    /* Exit_client is finished, fd is removed, there is
		     * nothing to send to or read from. ignore. -TimeMr14C
		     */
		    return 0;
		case FLUSH_BUFFER:
		    comm_setselect(cptr->fd, FDLIST_IRCD, COMM_SELECT_WRITE, send_queued, cptr, 0);
		    break;
		case ZIP_NEXT_BUFFER:
		    if (ZipIn(cptr)) {
			if (unzip_buffer(cptr, buffer, length, server_dopacket) < 0) {
			    sendto_gnotice("Another Unzip error for %s: (%d) %s", cptr->name, errno,
					   buffer);
			    exit_client(cptr, &me, "fatal error in unzip!");
			    return parseval;
			}
			return 0;
		    }
		    break;
#ifdef HAVE_ENCRYPTION_ON
		case RC4_NEXT_BUFFER:
		    if (length)
			rc4_process_stream(cptr->serv->rc4_in, buffer, length);
		    break;
#endif
		default:
		    break;
	    }
	} else if ((tmplen <= 0) && !broken) {
	    buffer = buffer + tmplen + 1;
	} else if (broken) {
	    restbuf_create(cptr);
	    cptr->waitlen = tmplen;
	    memcpy(cptr->restbuf, buffer, cptr->waitlen);
	}
    }

    /* server fd may have changed */
    fd_r = cptr->fd;

    if (!IsDead(cptr))
        comm_setselect(fd_r, FDLIST_IRCD, COMM_SELECT_READ, read_server_packet, cptr, 0);

    return 0;
}

int client_dopacket(aClient *cptr, char *buffer, size_t length)
{
    process_bytecount(cptr, length);

    if (user_parse(cptr, buffer, buffer + length) < 0)
	return -2;
    return 0;
}
