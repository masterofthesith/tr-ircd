/***********************************************************************
 *   IRC - Internet Relay Chat, src/send.c
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
 */

/*
 * $Id: send.c,v 1.9 2005/01/18 17:00:15 tr-ircd Exp $ 
 */

#include "struct.h"
#include "channel.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "numeric.h"
#include "dh.h"
#include "zlink.h"
#include "msg.h"
#include "s_bsd.h"
#include "s_conf.h"

#ifdef HAVE_ENCRYPTION_ON
static char rc4buf[16384];
#endif

static unsigned char transtab[] = "..A.....................EHI.O.YW.ABGDEZH0IKLMN3OPR STYFXCWIYaehi.abgdezh8iklmn3oprsstufxywiuouw.";

/*
 * dead_link_on_write
 *
 * somewhere along the lines of sending out, there was an error.
 * we can't close it from the send loop, so mark it as dead
 * and close it from the main loop.
 *
 * if this link is a server, tell routing people.
 */

static int dead_link_on_write(aClient *to, char *notice, int sockerr)
{

    int errtmp = errno;		/* so we don't munge this later */

    to->flags |= FLAGS_DEADSOCKET;
    /*
     * If because of BUFFERPOOL problem then clean linebuf's now so that
     * notices don't hurt operators below.
     */
    linebuf_donebuf(&to->recvQ);
    linebuf_donebuf(&to->sendQ);
    /*
     * Ok, if the link we're dropping is a server, send a routing
     * * notice..
     */
    if (IsServer(to)) {
    char fbuf[512];

	ircsprintf(fbuf, "from %C: %s", &me, notice);
	sendto_gnotice(fbuf, get_client_name(to, HIDEME), strerror(errtmp));
	sendto_serv_butone(to, &me, TOK1_GNOTICE, notice,
			   get_client_name(to, HIDEME), strerror(errtmp));
    }

    // exit_client(to, to, "Dead Socket");
    return -1;
}

/*
 * * send_message *   Internal utility which delivers one message
 * buffer to the *      socket. Takes care of the error handling and
 * buffering, if *      needed.
 */
int send_message(aClient *cptr, char *msg, int len)
{
    buf_head_t linebuf;
    u_char c;
    short i;
    aClient *to = cptr;

    if (cptr->from)
	to = cptr->from;

    if (IsAnon(to))
	return 0;	
    if (to->fd < 0)
        return 0;               /* Thou shalt not write to closed descriptors */

    if (IsDead(to))
        return 0;

    c = 0;
    i = 0;
    if (IsForeigner(to))
	for (i = 0; (c = msg[i]); i++)
	    if (c > (u_char) 159)
		msg[i] = transtab[c - 160];

    logevent_call(LogSys.send_debug, to, len, msg);

    /* Chop trailing CRLF's .. */
    while((msg[len] == '\r') || (msg[len] == '\n') || (msg[len] == '\0'))
    	len--;

    if (IsServer(to) || IsNegoServer(to)) {
	if (len > 510) 
	    len = 510;
	msg[++len] = '\n';
	msg[++len] = '\0';
    } else {
	if (len > 509) 
	    len = 509;
	msg[++len] = '\r';
	msg[++len] = '\n';
	msg[++len] = '\0';
    }
    if (ZipOut(to)) {
	msg = zip_buffer(to, msg, &len);
	if (len == -1) {
	    sendto_ops("Zipout error for %C: %s", to, msg);
	    return dead_link_on_write(to, ":Zip output error for %s", IRCERR_ZIP);
	}

	if (len == 0)
	    return 0;
    }
#ifdef HAVE_ENCRYPTION_ON
    if (IsRC4OUT(to)) {
	rc4_process_stream_to_buf(to->serv->rc4_out, msg, rc4buf, len);
	msg = rc4buf;
    }
#endif

    if (linebuf_len(&to->sendQ) > to->sendqlen) {
        /*
         * this would be a duplicate notice, but it contains some useful information that
         * would be spamming the rest of the network. Kept in. - lucas 
         */
        if (IsServer(to))
            sendto_ops("Max SendQ limit exceeded for %s: %d > %d",
                       get_client_name(to, HIDEME), linebuf_len(&to->sendQ), get_sendq(to));
        to->flags |= FLAGS_SENDQEX;
        return dead_link_on_write(to, ":Max Sendq exceeded for %s, closing link", 0);
    }

    /*
     * * Update statistics. The following is slightly incorrect *
     * because it counts messages even if queued, but bytes * only
     * really sent. Queued bytes get updated in SendQueued.
     */
    to->sendM += 1;
    me.sendM += 1;

    linebuf_newbuf(&linebuf);
    linebuf_put(&linebuf, msg, len);
    linebuf_attach(&to->sendQ, &linebuf);
    linebuf_donebuf(&linebuf);
    send_queued(to->fd, to);
    return 0;
}

int will_exceed_sendq(aClient *cptr)
{
    if (linebuf_len(&cptr->sendQ) > cptr->sendqlen / 2)
	return 1;
    return 0;
} 

/*
 * * send_queued *    This function is called from the main
 * select-loop (or whatever) *  when there is a chance the some output
 * would be possible. This *    attempts to empty the send queue as far
 * as possible...
 */
void send_queued(int fd, void *data)
{
    int rlen;
    aClient *to = data;

    if (IsDead(to))
	return;

    if (linebuf_len(&to->sendQ) > 0) {
	while ((rlen = linebuf_flush(to)) > 0) {
	    to->sendB += rlen;
	    me.sendB += rlen;
	    if (to->sendB > 1023) {
		to->sendK += (to->sendB >> 10);
		to->sendB &= 0x03ff;	/* 2^10 = 1024, 3ff = 1023 */
	    } else if (me.sendB > 1023) {
		me.sendK += (me.sendB >> 10);
		me.sendB &= 0x03ff;
	    }
	}
	if ((rlen < 0) && (ignoreErrno(errno))) {
	    /* we have a non-fatal error, so just continue */
	} else if (rlen < 0) {
	    /* We have a fatal error */
	    dead_link_on_write(to, ":Write error to %s, closing link", errno);
	    return;
	} else if (rlen == 0) {
	    /* 0 bytes is an EOF .. */
	    dead_link_on_write(to, ":EOF during write to %s, closing link", errno);
	    return;
	}
    }

    if (linebuf_len(&to->sendQ) > 0)
	comm_setselect(fd, FDLIST_IRCD, COMM_SELECT_WRITE, send_queued, to, 0);

    if ((to->flags & FLAGS_SOBSENT) && !IsULine(to) && linebuf_len(&to->sendQ) < 20480) {	/* 20k */
	if (!(to->flags & FLAGS_BURST)) {
	    to->flags &= (~FLAGS_SOBSENT);
	    sendto_one_server(to, NULL, TOK1_BURST, "%d", linebuf_len(&to->sendQ));
	}
    }

    if (IsLocalClient(to) && to->lopts) {
	if (to->lopts->starthash) {
	    send_list(to, to->lopts);
	}
    }


    return;

}
