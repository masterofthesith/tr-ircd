/************************************************************************
 *   IRC - Internet Relay Chat, server/s_listener.c
 *   Copyright (C) 1999 Thomas Helvey <tomh@inxpress.net>
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
 *  $Id: s_listener.c,v 1.5 2003/07/01 11:01:19 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "listener.h"
#include "numeric.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "throttle.h"

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned int) 0xffffffff)
#endif

static PF accept_connection;

struct Listener *ListenerPollList = NULL;

struct Listener *make_listener(int port, struct irc_inaddr *addr)
{
    struct Listener *listener = (struct Listener *) MyMalloc(sizeof(struct Listener));
    assert(0 != listener);

    listener->name = me.name;
    listener->fd = -1;
    copy_s_addr(IN_ADDR(listener->addr), PIN_ADDR(addr));

    listener->port = port;
    listener->ref_count = 0;
    listener->next = NULL;
    return listener;
}

void free_listener(struct Listener *listener)
{
    if (listener == NULL)
	return;

    /*
     * remove from listener list
     */
    if (listener == ListenerPollList)
	ListenerPollList = listener->next;
    else {
    struct Listener *prev = ListenerPollList;
	for (; prev; prev = prev->next) {
	    if (listener == prev->next) {
		prev->next = listener->next;
		break;
	    }
	}
    }

    /* free */
    MyFree(listener);
}

#define PORTNAMELEN 6		/* ":31337" */

/*
 * get_listener_name - return displayable listener name and port
 * returns "host.foo.org:6667" for a given listener
 */
const char *get_listener_name(const struct Listener *listener)
{
    static char buf[HOSTLEN + HOSTLEN + PORTNAMELEN + 4];
    assert(0 != listener);
    ircsprintf(buf, "%s[%s/%u]", me.name, listener->name, listener->port);
    return buf;
}

/*
 * inetport - create a listener socket in the AF_INET or AF_INET6 domain, 
 * bind it to the port given in 'port' and listen to it  
 * returns true (1) if successful false (0) on error.
 *
 * If the operating system has a define for SOMAXCONN, use it, otherwise
 * use HYBRID_SOMAXCONN 
 */
#ifdef SOMAXCONN
#undef HYBRID_SOMAXCONN
#define HYBRID_SOMAXCONN SOMAXCONN
#endif

static int inetport(struct Listener *listener)
{
    struct irc_sockaddr lsin;
    int fd;
    int opt = 1;

    /*
     * At first, open a new socket
     */
    fd = comm_open(DEF_FAM, SOCK_STREAM, 0, "Listener socket");
#ifdef IPV6
    if (!IN6_ARE_ADDR_EQUAL((struct in6_addr *) &listener->addr, &in6addr_any))
#else
    if (INADDR_ANY != listener->addr.sins.sin.s_addr)
#endif
    {
	inetntop(DEF_FAM, &IN_ADDR(listener->addr), listener->vhost, HOSTLEN);
	listener->name = listener->vhost;
    }
    if (-1 == fd) {
	report_error("opening listener socket %s:%s", get_listener_name(listener), errno);
	return 0;
    } else if (MAXCONNECTIONS < fd) {
	report_error("no more connections left for listener %s:%s",
		     get_listener_name(listener), errno);
	fd_close(fd);
	return 0;
    }
    /* 
     * XXX - we don't want to do all this crap for a listener
     * set_sock_opts(listener);
     */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt))) {
	report_error("setting SO_REUSEADDR for listener %s:%s", get_listener_name(listener), errno);
	fd_close(fd);
	return 0;
    }

    /*
     * Bind a port to listen for new connections if port is non-null,
     * else assume it is already open and try get something from it.
     */
    memset(&lsin, 0, sizeof(struct irc_sockaddr));
    S_FAM(lsin) = DEF_FAM;
    copy_s_addr(S_ADDR(lsin), IN_ADDR(listener->addr));
    S_PORT(lsin) = htons(listener->port);

    if (bind(fd, (struct sockaddr *) &SOCKADDR(lsin), sizeof(struct irc_sockaddr))) {
	report_error("binding listener socket %s:%s", get_listener_name(listener), errno);
	fd_close(fd);
	return 0;
    }

    if (listen(fd, HYBRID_SOMAXCONN)) {
	report_error("listen failed for %s:%s", get_listener_name(listener), errno);
	fd_close(fd);
	return 0;
    }

    /*
     * XXX - this should always work, performance will suck if it doesn't
     */
    if (!set_non_blocking(fd))
	report_error(NONB_ERROR_MSG, get_listener_name(listener), errno);

    listener->fd = fd;

    /* Listen completion events are READ events .. */

    comm_setselect(fd, FDLIST_IRCD, COMM_SELECT_READ, accept_connection, listener, 0);

    return 1;
}

static struct Listener *find_listener(int port, struct irc_inaddr *addr)
{
    struct Listener *listener = NULL;
    struct Listener *last_closed = NULL;

    for (listener = ListenerPollList; listener; listener = listener->next) {

	if ((port == listener->port) &&
	    (!memcmp(&PIN_ADDR(addr), &IN_ADDR(listener->addr), sizeof(struct irc_inaddr)))) {
	    /* Try to return an open listener, otherwise reuse a closed one */
	    if (listener->fd == -1)
		last_closed = listener;
	    else
		return listener;
	}
    }
    return last_closed;
}

struct Listener *find_listener_by_uuid(char *id)
{
    uuid_t u;
    struct Listener *listener = NULL;

    uuid_parse(id, u);

    for (listener = ListenerPollList; listener; listener = listener->next) {
	if ((uuid_compare(listener->uuid, u) == 0))
	    break;
    }

    return listener;
}

/*
 * add_listener- create a new listener 
 * port - the port number to listen on
 * vhost_ip - if non-null must contain a valid IP address string in
 * the format "255.255.255.255"
 */
void add_listener(int port, const char *vhost_ip, int flags, uuid_t u, const char *host_header)
{
    struct Listener *listener;
    struct irc_inaddr vaddr;

    /*
     * if no port in conf line, don't bother
     */
    if (0 == port)
	return;

    if ((listener = find_listener(port, &vaddr))) {
	if (listener->fd > -1)
	    return;
    } else {

#ifdef IPV6
	copy_s_addr(IN_ADDR(vaddr), &in6addr_any);
#else
	copy_s_addr(IN_ADDR(vaddr), INADDR_ANY);
#endif

	if (vhost_ip) {
	    if (inetpton(DEF_FAM, vhost_ip, &IN_ADDR(vaddr)) <= 0)
		return;
	}

	listener = make_listener(port, &vaddr);
	if (host_header)
	    DupString(listener->host_header, host_header);
	listener->next = ListenerPollList;
	listener->flags = flags;
	ListenerPollList = listener;
    }

    listener->fd = -1;
    memcpy(listener->uuid, u, 16);

    if (inetport(listener)) {
	listener->active = 1;
    } else
	close_listener(listener);
}

/*
 * close_listener - close a single listener
 */
void close_listener(struct Listener *listener)
{
    if (listener == NULL)
	return;

    if (listener->fd >= 0) {
	fd_close(listener->fd);
	listener->fd = -1;
    }

    listener->active = 0;

    if (listener->ref_count)
	return;

    free_listener(listener);
}

/*
 * close_listeners - close and free all listeners that are not being used
 */
void close_listeners()
{
struct Listener *listener;
struct Listener *listener_next = 0;
    /*
     * close all 'extra' listening ports we have
     */
    for (listener = ListenerPollList; listener; listener = listener_next) {
	listener_next = listener->next;
	if (!IsListenerHttp(listener))
	    close_listener(listener);
    }
}

static int conf_connect_allowed(struct irc_inaddr *myaddr, int aftype, int fd)
{
    aMaskItem *ami;

    ami = find_maskitem(inetntoa((char *) myaddr), NULL, MASKITEM_ZAPLINE, 1);

    if (!ami)
	ami = find_maskitem(inetntoa((char *) myaddr), NULL, MASKITEM_ZAPLINE_CONFIG, 1);

    if (ami) {
	send(fd, "ERROR :You have been zapped.\r\n", 30, 0);
	return BANNED_CLIENT;
    }

    /* if they are throttled, drop them silently. */
    if (throttle_check(inetntoa((char *) myaddr), fd, NOW) == 0)
	return TOO_FAST;

    return 0;
}

static void accept_connection(int pfd, void *data)
{
    static time_t last_oper_notice = 0;

    struct irc_sockaddr sai;
    struct irc_inaddr addr;
    int fd;
    int pe;
    struct Listener *listener = data;
    fdlist_t fdlist;

    assert(0 != listener);

    listener->last_accept = timeofday;

    if (IsListenerHttp(listener))
	fdlist = FDLIST_HTTPD;
    else
	fdlist = FDLIST_IRCD;

    /*
     * There may be many reasons for error return, but
     * in otherwise correctly working environment the
     * probable cause is running out of file descriptors
     * (EMFILE, ENFILE or others?). The man pages for
     * accept don't seem to list these as possible,
     * although it's obvious that it may happen here.
     * Thus no specific errors are tested at this
     * point, just assume that connections cannot
     * be accepted until some old is closed first.
     */

    fd = comm_accept(listener->fd, &sai);

    copy_s_addr(IN_ADDR(addr), S_ADDR(sai));

#ifdef IPV6
    if ((IN6_IS_ADDR_V4MAPPED(&IN_ADDR2(addr))) || (IN6_IS_ADDR_V4COMPAT(&IN_ADDR2(addr)))) {
	memmove(&addr.sins.sin.s_addr, addr.sins.sin6.s6_addr + 12, sizeof(struct in_addr));
	sai.sins.sin.sin_family = AF_INET;
    }
#endif

    if (fd < 0) {
	if (!ignoreErrno(errno)) {
	    /*
	     * slow down the whining to opers bit
	     */
	    if ((last_oper_notice + 20) <= timeofday) {
		report_error("Error accepting connection %s:%s", listener->name, errno);
		last_oper_notice = timeofday;
	    }
	}
	/* Re-register a new IO request for the next accept .. */
	comm_setselect(listener->fd, fdlist, COMM_SELECT_READ, accept_connection, listener, 0);
	return;
    }
    /*
     * check for connection limit
     */
    if ((MAXCONNECTIONS - 10) < fd) {
	++ircstp->is_ref;
	/* 
	 * slow down the whining to opers bit 
	 */
	if ((last_oper_notice + 20) <= timeofday) {
	    sendto_gnotice("All connections in use. (%s)", get_listener_name(listener));
	    last_oper_notice = timeofday;
	}
	send(fd, "ERROR :All connections in use\r\n", 32, 0);
	fd_close(fd);
	/* Re-register a new IO request for the next accept .. */
	comm_setselect(listener->fd, fdlist, COMM_SELECT_READ, accept_connection, listener, 0);
	return;
    }

    /* Do an initial check we aren't connecting too fast or with too many
     * from this IP... */
    if ((pe = conf_connect_allowed(&addr, sai.sins.sin.sin_family, fd)) != 0) {
	ircstp->is_ref++;
	fd_close(fd);
	/* Re-register a new IO request for the next accept .. */
	comm_setselect(listener->fd, fdlist, COMM_SELECT_READ, accept_connection, listener, 0);
	return;
    }
    ircstp->is_ac++;

    add_connection(listener, fd);

    /* Re-register a new IO request for the next accept .. */
    comm_setselect(listener->fd, fdlist, COMM_SELECT_READ, accept_connection, listener, 0);
}

/*
 * show_ports - send port listing to a client
 * inputs       - pointer to client to show ports to
 * output       - none
 * side effects - show ports
 */
void show_ports(struct Client *source_p)
{
    struct Listener *listener = 0;
    char str[64];

    for (listener = ListenerPollList; listener; listener = listener->next) {
	uuid_unparse(listener->uuid, str);
	send_me_numeric(source_p, RPL_STATSPLINE, 'P',
			listener->port,
			IsAdmin(source_p) ? listener->name : me.name,
			listener->ref_count, (listener->active) ? "active" : "disabled",
			listener->flags, str);
    }
}
