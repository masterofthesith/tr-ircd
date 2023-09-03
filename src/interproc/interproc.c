/************************************************************************
 *   IRC - Internet Relay Chat, src/interproc/interproc.c
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
 *  $Id: interproc.c,v 1.5 2003/07/01 11:01:20 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "listener.h"
#include "language.h"
#include "s_bsd.h"
#include "msg.h"
#include "s_conf.h"
#ifndef INTERPROC_TOKENS
#define INTERPROC_TOKENS 1
#include "interproc.h"
#endif

static int interproc_fd;

static char readBuf[READBUF_SIZE];
static char sendbuf[IRC_SB_SIZE];
static char *parv[MAXPARA + 1];
static char sockbuf[MAXPATHLEN + 1];

buf_head_t interproc_recvQ;
buf_head_t interproc_sendQ;

static void generate_sockname(void);
static void generate_sockname(void)
{
    pid_t p = getpid();

    ircsnprintf(sockbuf, MAXPATHLEN, "%s.%d", SOCKET_NAME, p);
    return;
}

int init_interproc(void)
{
    int fd;
    int opt = 1;
    struct sockaddr_un saddr;

    fd = socket(PF_UNIX, SOCK_STREAM, 0);

    if (fd < 0) {
	report_error("opening local domain socket %s:%s", sockbuf, errno);
	return 0;
    }

    generate_sockname();

    memset((char *) &saddr, 0, sizeof(saddr));

    saddr.sun_family = PF_UNIX;
    strcpy(saddr.sun_path, sockbuf);

    unlink(sockbuf);
    chmod(sockbuf, 0600);

    fd_open(fd, FD_SOCKET, "Interproc Socket");

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt))) {
	report_error("setting SO_REUSEADDR for listener %s:%s", sockbuf, errno);
	fd_close(fd);
	return 0;
    }

    if (bind(fd, (struct sockaddr *) &saddr, sizeof(saddr.sun_family) + strlen(sockbuf)) < 0) {
	report_error("binding listener socket %s:%s", sockbuf, errno);
	fd_close(fd);
	return 0;
    }

    if (listen(fd, 2)) {
	report_error("listen failed for %s:%s", sockbuf, errno);
	fd_close(fd);
	return 0;
    }

    if (!set_non_blocking(fd))
	report_error("error setting socket non blocking %s:%s", sockbuf, errno);

    /* Listen completion events are READ events .. */

    comm_setselect(fd, FDLIST_IRCD, COMM_SELECT_READ, accept_localdomain_socket, sockbuf, 0);

    interproc_fd = fd;

    return 1;
}

void terminate_interproc(void)
{
    remove(sockbuf);
    close(interproc_fd);
}

void accept_localdomain_socket(int pfd, void *data)
{
    struct sockaddr_un caddr;
    int fd;
    socklen_t addrlen = sizeof(struct sockaddr_un);

    fd = accept(pfd, (struct sockaddr *) &caddr, (int *) &addrlen);

    if (fd < 0) {
	if (ignoreErrno(errno)) {
	    comm_setselect(pfd, FDLIST_IRCD, COMM_SELECT_READ, accept_localdomain_socket, data, 0);
	    return;
	}
    }

    /* Set the socket non-blocking, and other wonderful bits */
    if (!set_non_blocking(fd)) {
	close(fd);
	return;
    }

    /*
     * check for connection limit
     */
    if ((MAXCONNECTIONS - 10) < fd) {
	++ircstp->is_ref;
	fd_close(fd);
	/* Re-register a new IO request for the next accept .. */
	comm_setselect(pfd, FDLIST_IRCD, COMM_SELECT_READ, accept_localdomain_socket, data, 0);
	return;
    }

    ircstp->is_ac++;

    if (!set_sock_buffers(fd, READBUF_SIZE))
	report_error("error setting readbuf size %s:%s", data, errno);
    if (!disable_sock_options(fd))
	report_error("error disabling socket options %s:%s", data, errno);

    fd_open(fd, FD_SOCKET, "Interproc Socket Accepted");

    comm_setselect(fd, FDLIST_IRCD, COMM_SELECT_WRITE, read_interproc_packet, data, 0);

    /* Re-register a new IO request for the next accept .. */
    comm_setselect(pfd, FDLIST_IRCD, COMM_SELECT_READ, accept_localdomain_socket, data, 0);
}

void read_interproc_packet(int fd, void *data)
{
    int length;
    int dolen = 0;
    int lbuf_len = 0;

    memset(readBuf, 0, READBUF_SIZE);

    length = read(fd, readBuf, READBUF_SIZE);

    if (length <= 0) {
	if ((length == -1) && ignoreErrno(errno)) {
	    comm_setselect(fd, FDLIST_IRCD, COMM_SELECT_READ, read_interproc_packet, data, 0);
	    return;
	}
	linebuf_donebuf(&interproc_recvQ);
	linebuf_donebuf(&interproc_sendQ);
	fd_close(fd);
	return;
    }

    lbuf_len = linebuf_parse(&interproc_recvQ, readBuf, length, 0);

    if (lbuf_len < 0) {
	linebuf_donebuf(&interproc_recvQ);
	linebuf_donebuf(&interproc_sendQ);
	fd_close(fd);
	return;
    }

    for (;;) {
	dolen = linebuf_get(&interproc_recvQ, readBuf, READBUF_SIZE,
			    LINEBUF_COMPLETE, LINEBUF_PARSED);
	if (!dolen)
	    break;
	parse_interproc(readBuf, dolen);
    }
    comm_setselect(fd, FDLIST_IRCD, COMM_SELECT_READ, read_interproc_packet, data, 0);

}

void parse_interproc(char *buf, int len)
{
    char *s;
    int i = 0;
    struct IToken *it;
    aClient *acptr;
    char *command, *dest;

    logevent_call(LogSys.parse_debug, "Socket", &me, buf);

    for (s = buf; *s == ' '; s++);

    while (*s == ' ')
	*s++ = '\0';

    if (!s)
	return;


    parv[0] = me.name;

    command = s++;
    while (*s == ' ')
        *s++ = '\0';

    if (IsDigit(*command) && 
	(*command + 1 && IsDigit(*command + 1)) &&
	(*command + 2 && IsDigit(*command + 2))) {
	while (*s && *s != ' ')
            s++;
        while (*s == ' ')
            *s++ = '\0';
	dest = s++;
	while (*s && *s != ' ')
            s++;
        while (*s == ' ')
            *s++ = '\0';
	acptr = find_client(dest);
	if (acptr && IsPerson(acptr))
	    sendto_one(acptr, ":%~C %s %s", &me, command, s);
   	return;
    }

    while (i <= MAXPARA) {
        while (*s == ' ')
            *s++ = '\0';
	if (*s == '\0')
	    break;
	if (*s == ':') {
	    parv[++i] = s + 1;
	    break;
	}
	parv[++i] = s;
	while (*s && *s != ' ')
	    s++;
    }

    it = &interproc_tab[((u_char) *command)];
    if (it->func)
	it->func(i, parv);
}

int configure_interproc(int *sfd)
{
    struct sockaddr_un addr;
    int fd;

    if ((fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
	report_error("opening local domain socket %s:%s", sockbuf, errno);
	return 1;
    }

    memset((char *) &addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, sockbuf);

    if (connect(fd, (struct sockaddr *) &addr, sizeof(addr.sun_family) + strlen(sockbuf)) < 0) {
	report_error("connecting to local socket %s:%s", sockbuf, errno);
	fd_close(fd);
	return 1;
    }

    fd_open(fd, FD_SOCKET, "Interproc Socket incoming");

    *sfd = fd;

    return 0;

}

int sendto_interproc(int fd, char *msg, int len)
{
    buf_head_t linebuf;

    logevent_call(LogSys.send_debug, me.name, len, msg);

    /* Chop trailing CRLF's .. */
    while ((msg[len] == '\r') || (msg[len] == '\n') || (msg[len] == '\0'))
	len--;

    if (len > 509)
	len = 509;
    msg[++len] = '\r';
    msg[++len] = '\n';
    msg[++len] = '\0';

    if (linebuf_len(&interproc_sendQ) > MAXSENDQLENGTH) {
	linebuf_donebuf(&interproc_recvQ);
	linebuf_donebuf(&interproc_sendQ);
	fd_close(fd);
    }

    linebuf_newbuf(&linebuf);
    linebuf_put(&linebuf, msg, len);
    linebuf_attach(&interproc_sendQ, &linebuf);
    linebuf_donebuf(&linebuf);
    send_interproc_queued(fd, NULL);
    return 0;
}

void send_interproc_queued(int fd, void *data)
{
    int rlen;

    if (linebuf_len(&interproc_sendQ)) {
	while ((rlen = linebuf_flush_interproc(fd, &interproc_sendQ)) > 0);
	if ((rlen < 0) && (ignoreErrno(errno))) {
	    ;
	} else if (rlen < 0) {
	    linebuf_donebuf(&interproc_recvQ);
	    linebuf_donebuf(&interproc_sendQ);
	    fd_close(fd);
	    return;
	} else if (rlen == 0) {
	    linebuf_donebuf(&interproc_recvQ);
	    linebuf_donebuf(&interproc_sendQ);
	    fd_close(fd);	/* 0 bytes is an EOF .. */
	    return;
	}
    }

    if (linebuf_len(&interproc_sendQ))
	comm_setselect(fd, FDLIST_IRCD, COMM_SELECT_WRITE, send_interproc_queued, data, 0);

    return;
}

void send_interproc_message(int fd, char *pattern, ...)
{ 
    va_list vl;
    int len;

    va_start(vl, pattern);

    memset(sendbuf, 0, sizeof(char) * IRC_SB_SIZE);
    len = ircvsprintf(sendbuf, pattern, vl);

    sendto_interproc(fd, sendbuf, len);
    va_end(vl);
}

void vsend_interproc_message(int fd, char *pattern, va_list vl)
{
    int len;

    memset(sendbuf, 0, sizeof(char) * IRC_SB_SIZE);
    len = ircvsprintf(sendbuf, pattern, vl);

    sendto_interproc(fd, sendbuf, len);
}

void send_interproc_numeric(int fd, int numeric, aClient *cptr, ...)
{
    char nbuf[1024];
    char *pattern = NULL;
    int len;
    va_list vl;

    pattern = get_numeric_format_in_lang(&numeric, get_language(cptr->lang));

    va_start(vl, cptr);

    len = ircsprintf(nbuf, ":%s %N %s ", me.name, numeric, cptr->name);

    strncat(nbuf, pattern, 1024 - len);
    vsend_interproc_message(fd, nbuf, vl);
    va_end(vl);
    return;
}

