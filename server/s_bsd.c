/************************************************************************
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
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "fd.h"
#include "s_bsd.h"
#include "event.h"
#include "listener.h"
#include "numeric.h"
#include "packet.h"
#include "resnew.h"
#include "s_auth.h"
#include "s_conf.h"
#include "hook.h"
#include "dh.h"

#ifndef IN_LOOPBACKNET
#define IN_LOOPBACKNET        0x7f
#endif

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned int) 0xffffffff)
#endif

time_t timeofday;

char *const NONB_ERROR_MSG = "set_non_blocking failed for %s:%s";
char *const OPT_ERROR_MSG = "disable_sock_options failed for %s:%s";
char *const SETBUF_ERROR_MSG = "set_sock_buffers failed for server %s:%s";

static const char *comm_err_str[] = { 
	"Comm OK", 
	"Error during bind()",
    	"Error during DNS lookup", 	
	"connect timeout", 
	"Error during connect()",
    	"Comm Error"
};

static void comm_connect_callback(int fd, int status);
static PF comm_connect_timeout;
static void comm_connect_dns_callback(void *vptr, adns_answer * reply);
static void comm_tryall_callback(int fd, void *data);
static PF comm_connect_tryconnect;

static int hookid_read_packet = -1;
static int hookid_make_listener_client = -1;

void init_bsd(void) {
    hookid_read_packet = hook_add_event("read packet");
    hookid_make_listener_client = hook_add_event("make listener client");
}

/*
 * get_sockerr - get the error value from the socket or the current errno
 *
 * Get the *real* error from the socket (well try to anyway..).
 * This may only work when SO_DEBUG is enabled but its worth the
 * gamble anyway.
 */
int get_sockerr(int fd)
{
    int errtmp = errno;
#ifdef SO_ERROR
    int err = 0;
    socklen_t len = sizeof(err);

    if (-1 < fd && !getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *) &err, (int *) &len)) {
	if (err)
	    errtmp = err;
    }
    errno = errtmp;
#endif
    return errtmp;
}

/*
 * report_error - report an error from an errno. 
 * Record error to log and also send a copy to all *LOCAL* opers online.
 *
 *        text        is a *format* string for outputing error. It must
 *                contain only two '%s', the first will be replaced
 *                by the sockhost from the client_p, and the latter will
 *                be taken from sys_errlist[errno].
 *
 *        client_p        if not NULL, is the *LOCAL* client associated with
 *                the error.
 *
 * Cannot use perror() within daemon. stderr is closed in
 * ircd and cannot be used. And, worse yet, it might have
 * been reassigned to a normal connection...
 * 
 * Actually stderr is still there IFF ircd was run with -s --Rodder
 */

void report_error(char *text, const char *who, int error)
{
    char buf[1024];
    who = (who) ? who : "";

    ircsnprintf(buf, 1024, text, who, strerror(error));
    sendto_lev(SNOTICE_LEV, buf);
    logevent_call(LogSys.report_error, buf);
}

/*
 * set_sock_buffers - set send and receive buffers for socket
 * 
 * inputs       - fd file descriptor
 *              - size to set
 * output       - returns true (1) if successful, false (0) otherwise
 * side effects -
 */
int set_sock_buffers(int fd, int size)
{
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *) &size, sizeof(size)) ||
	setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *) &size, sizeof(size)))
	return 0;
    return 1;
}

/*
 * disable_sock_options
 * 
 * inputs       - fd
 * output       - returns true (1) if successful, false (0) otherwise
 * side effects - disable_sock_options - if remote has any socket options set,
 *                disable them 
 */
int disable_sock_options(int fd)
{
#if defined(IP_OPTIONS) && defined(IPPROTO_IP) && !defined(IPV6)
    if (setsockopt(fd, IPPROTO_IP, IP_OPTIONS, NULL, 0))
	return 0;
#endif
    return 1;
}

/*
 * set_non_blocking - Set the client connection into non-blocking mode. 
 *
 * inputs       - fd to set into non blocking mode
 * output       - 1 if successful 0 if not
 * side effects - use POSIX compliant non blocking and
 *                be done with it.
 */
int set_non_blocking(int fd)
{
    int nonb = 0;
    int res;
    nonb |= O_NONBLOCK;

#ifdef USE_SIGIO
    setup_sigio_fd(fd);
#endif

    res = fcntl(fd, F_GETFL, 0);
    if (-1 == res || fcntl(fd, F_SETFL, res | nonb) == -1)
	return 0;

    fd_table[fd].flags.nonblocking = 1;
    return 1;
}

/*
 * add_connection - creates a client which has just connected to us on 
 * the given fd. The sockhost field is initialized with the ip# of the host.
 * The client is sent to the auth module for verification, and not put in
 * any client list yet.
 */
void add_connection(struct Listener *listener, int fd)
{
    aClient *new_client;
#ifdef HAVE_ENCRYPTION_ON
    int new_ssl = 0;
#endif
    socklen_t len = sizeof(struct irc_sockaddr);
    struct irc_sockaddr irn;
    assert(0 != listener);

    /* 
     * get the client socket name from the socket
     * the client has already been checked out in accept_connection
     */
    if (getpeername(fd, (struct sockaddr *) &SOCKADDR(irn), (int *) &len)) {
	report_error("Failed in adding new connection %s :%s", get_listener_name(listener), errno);
	ircstp->is_ref++;
	fd_close(fd);
	return;
    }

    new_client = make_client(NULL);

    /* 
     * copy address to 'sockhost' as a string, copy it to host too
     * so we have something valid to put into error messages...
     */
    copy_s_addr(IN_ADDR(new_client->ip), S_ADDR(irn));
    inetntop(DEF_FAM, &IN_ADDR(new_client->ip), new_client->hostip, HOSTIPLEN);
#ifdef IPV6
    if((!IN6_IS_ADDR_V4MAPPED(&IN_ADDR2(new_client->ip))) &&
        (!IN6_IS_ADDR_V4COMPAT(&IN_ADDR2(new_client->ip))))
        new_client->aftype = AF_INET6;
    else {
        memmove(&new_client->ip.sins.sin.s_addr,&IN_ADDR(new_client->ip)[12], sizeof(struct in_addr));
        new_client->aftype = AF_INET;
    }
#else
    new_client->aftype = AF_INET;
#endif

    new_client->connectport = htons(S_PORT(irn));

    strcpy(new_client->sockhost, new_client->hostip);

    if (IsListenerHttp(listener)) {
	struct hook_data thisdata;
	thisdata.client_p = new_client;
	if (hook_call_event(hookid_make_listener_client, &thisdata)) {
	    dlink_node * ptr;
	    ptr = dlinkFind(&unknown_list, new_client);
	    if (ptr)
		dlinkDeleteNode(ptr, &unknown_list);
	    free_client(new_client);
	    new_client = thisdata.client_p;
	}
    }

    new_client->fd = fd;

    new_client->listener = listener;
    ++listener->ref_count;

#ifdef HAVE_ENCRYPTION_ON
    if (IsListenerClientSSL(listener) && GeneralOpts.doing_ssl && !(new_client->ssl)) {
	new_ssl = 1;
        /*SSL client init */

        if((new_client->ssl = SSL_new(ircdssl_ctx)) == NULL) {
            dlink_node * ptr;
            ptr = dlinkFind(&unknown_list, new_client);
            if (ptr)
                dlinkDeleteNode(ptr, &unknown_list);
            sendto_lev(DEBUG_LEV, "Could not create new ssl connection %s", new_client->hostip);
            ircstp->is_ref++;
            free_client(new_client);
            return;
	}
        SetSSL(new_client);
    }
#endif

    if (!set_non_blocking(new_client->fd))
        report_error(NONB_ERROR_MSG, get_client_name(new_client, TRUE), errno);
    if (!set_sock_buffers(new_client->fd, READBUF_SIZE))
        report_error(SETBUF_ERROR_MSG, get_client_name(new_client, SHOW_IP), errno);
    if (!disable_sock_options(new_client->fd))
        report_error(OPT_ERROR_MSG, get_client_name(new_client, TRUE), errno);

#ifdef HAVE_ENCRYPTION_ON
    if (IsSSL(new_client) && new_ssl) {
   	SSL_set_fd(new_client->ssl, new_client->fd);
        if (!safe_SSL_accept(new_client, new_client->fd)) {
            dlink_node * ptr;
            ptr = dlinkFind(&unknown_list, new_client);
            if (ptr)
                dlinkDeleteNode(ptr, &unknown_list);
            SSL_set_shutdown(new_client->ssl, SSL_RECEIVED_SHUTDOWN);
            SSL_smart_shutdown(new_client->ssl);
            SSL_free(new_client->ssl);
	    ircstp->is_ref++;
	    free_client(new_client);
	    return;
        }
    }
#endif

    if (IsListenerHttp(new_client->listener))
	comm_setselect(fd, FDLIST_HTTPD, COMM_SELECT_WRITE, comm_tryall_callback, new_client, 0);
    else
    	comm_setselect(fd, FDLIST_IRCD, COMM_SELECT_WRITE, comm_tryall_callback, new_client, 0);	
}

void dead_link_on_read(struct Client *client_p, int error)
{
    /*
     * ...hmm, with non-blocking sockets we might get
     * here from quite valid reasons, although.. why
     * would select report "data available" when there
     * wasn't... so, this must be an error anyway...  --msa
     * actually, EOF occurs when read() returns 0 and
     * in due course, select() returns that fd as ready
     * for reading even though it ends up being an EOF. -avalon
     */

    char errmsg[255];
    int current_error = get_sockerr(client_p->fd);

    logevent_call(LogSys.read_error, client_p, client_p->fd, current_error, error);
    if (IsServer(client_p) || IsHandshake(client_p)) {
    int connected = timeofday - client_p->firsttime;

	if (error == 0) {
	    sendto_gnotice("Server %s closed the connection", get_client_name(client_p, TRUE));

	} else {
	    report_error("Lost connection to %s: %d",
			 get_client_name(client_p, TRUE), current_error);

	}

	sendto_gnotice("%s had been connected for %d day%s, %2d:%02d:%02d",
		       client_p->name, connected / 86400,
		       (connected / 86400 == 1) ? "" : "s",
		       (connected % 86400) / 3600, (connected % 3600) / 60, connected % 60);
    }
    if (error == 0) {
	strcpy(errmsg, "Remote host closed the connection");
    } else {
	ircsprintf(errmsg, "Read error: %d (%s)", current_error, strerror(current_error));
    }
    exit_client(client_p, &me, errmsg);
}

/*
 * stolen from squid - its a neat (but overused! :) routine which we
 * can use to see whether we can ignore this errno or not. It is
 * generally useful for non-blocking network IO related errnos.
 *     -- adrian
 */
int ignoreErrno(int ierrno)
{
    switch (ierrno) {
	case EINPROGRESS:
	case EWOULDBLOCK:
#if EAGAIN != EWOULDBLOCK
	case EAGAIN:
#endif
	case EALREADY:
	case EINTR:
#ifdef ERESTART
	case ERESTART:
#endif
	    return 1;
	default:
	    return 0;
    }
    /* NOTREACHED */
    return 0;
}

/*
 * comm_settimeout() - set the socket timeout
 *
 * Set the timeout for the fd
 */
void comm_settimeout(int fd, time_t timeout, PF * callback, void *cbdata)
{
    assert(fd > -1);
    assert(fd_table[fd].flags.open);

    fd_table[fd].timeout = timeofday + (timeout / 1000);
    fd_table[fd].timeout_handler = callback;
    fd_table[fd].timeout_data = cbdata;
}

/*
 * comm_setflush() - set a flush function
 *
 * A flush function is simply a function called if found during
 * comm_timeouts(). Its basically a second timeout, except in this case
 * I'm too lazy to implement multiple timeout functions! :-)
 * its kinda nice to have it seperate, since this is designed for
 * flush functions, and when comm_close() is implemented correctly
 * with close functions, we _actually_ don't call comm_close() here ..
 */
void comm_setflush(int fd, time_t timeout, PF * callback, void *cbdata)
{
    if (fd < 1)
	return;

    fd_table[fd].flush_timeout = timeofday + (timeout / 1000);
    fd_table[fd].flush_handler = callback;
    fd_table[fd].flush_data = cbdata;
}

/*
 * comm_checktimeouts() - check the socket timeouts
 *
 * All this routine does is call the given callback/cbdata, without closing
 * down the file descriptor. When close handlers have been implemented,
 * this will happen.
 */
void comm_checktimeouts(void *notused)
{
    int fd;
    PF *hdl;
    void *data;

    for (fd = 0; fd <= highest_fd; fd++) {
	if (!fd_table[fd].flags.open)
	    continue;
	if (fd_table[fd].flags.closing)
	    continue;

	/* check flush functions */
	if (fd_table[fd].flush_handler &&
	    fd_table[fd].flush_timeout > 0 && fd_table[fd].flush_timeout < timeofday) {
	    hdl = fd_table[fd].flush_handler;
	    data = fd_table[fd].flush_data;
	    comm_setflush(fd, 0, NULL, NULL);
	    hdl(fd, data);
	}

	/* check timeouts */
	if (fd_table[fd].timeout_handler &&
	    fd_table[fd].timeout > 0 && fd_table[fd].timeout < timeofday) {
	    /* Call timeout handler */
	    hdl = fd_table[fd].timeout_handler;
	    data = fd_table[fd].timeout_data;
	    comm_settimeout(fd, 0, NULL, NULL);
	    hdl(fd, fd_table[fd].timeout_data);
	}
    }
}

/*
 * void comm_connect_tcp(int fd, const char *host, u_short port,
 *                       struct sockaddr *clocal, int socklen,
 *                       CNCB *callback, void *data, int aftype)
 * Input: An fd to connect with, a host and port to connect to,
 *        a local sockaddr to connect from + length(or NULL to use the
 *        default), a callback, the data to pass into the callback, the
 *        address family.
 * Output: None.
 * Side-effects: A non-blocking connection to the host is started, and
 *               if necessary, set up for selection. The callback given
 *               may be called now, or it may be called later.
 */
void
comm_connect_tcp(int fd, const char *host, u_short port,
		 struct sockaddr *clocal, int socklen, CNCB * callback, 
		 void *data, int aftype, int timeout)
{
    fd_table[fd].flags.called_connect = 1;
    assert(callback != NULL);
    fd_table[fd].connect.callback = callback;
    fd_table[fd].connect.data = data;

    S_FAM(fd_table[fd].connect.hostaddr) = DEF_FAM;
    S_PORT(fd_table[fd].connect.hostaddr) = htons(port);

    /* Note that we're using a passed sockaddr here. This is because
     * generally you'll be bind()ing to a sockaddr grabbed from
     * getsockname(), so this makes things easier.
     * XXX If NULL is passed as local, we should later on bind() to the
     * virtual host IP, for completeness.
     *   -- adrian
     */

    if ((clocal != NULL) && (bind(fd, clocal, socklen) < 0)) {
	/* Failure, call the callback with COMM_ERR_BIND */
	comm_connect_callback(fd, COMM_ERR_BIND);
	/* ... and quit */
	return;
    }

    /* Next, if we have been given an IP, get the addr and skip the
     * DNS check (and head direct to comm_connect_tryconnect().
     */

    if (inetpton(DEF_FAM, host, S_ADDR(&fd_table[fd].connect.hostaddr)) <= 0) {
	/* Send the DNS request, for the next level */
	fd_table[fd].dns_query = (struct DNSQuery *) MyMalloc(sizeof(struct DNSQuery));
	fd_table[fd].dns_query->ptr = &fd_table[fd];
	fd_table[fd].dns_query->callback = comm_connect_dns_callback;
	adns_gethost(host, aftype, fd_table[fd].dns_query);
    } else {
	/* We have a valid IP, so we just call tryconnect */
	/* Make sure we actually set the timeout here .. */
	comm_settimeout(fd, timeout * 1000, comm_connect_timeout, NULL);
	comm_connect_tryconnect(fd, NULL);
    }
}

/*
 * comm_connect_callback() - call the callback, and continue with life
 */
static void comm_connect_callback(int fd, int status)
{
    CNCB *hdl;
    /* This check is gross..but probably necessary */
    if (fd_table[fd].connect.callback == NULL)
	return;
    /* Clear the connect flag + handler */
    hdl = fd_table[fd].connect.callback;
    fd_table[fd].connect.callback = NULL;
    fd_table[fd].flags.called_connect = 0;

    /* Clear the timeout handler */
    comm_settimeout(fd, 0, NULL, NULL);

    /* Call the handler */
    hdl(fd, status, fd_table[fd].connect.data);
}

/*
 * comm_tryall_callback - This does ensure the finishing of an ssl
 * accept routine before we continue with identd lookups.
 * -TimeMr14C
 */

static void comm_tryall_callback(int fd, void *data)
{
    int doauth = -1;
    aClient *cptr = data;
    struct hook_data thisdata;

    fdlist_t fdlist = FDLIST_NONE;

    if (IsListenerHttp(cptr->listener)) {
 	fdlist = FDLIST_HTTPD;
	doauth = 0;
    } else if (IsListenerClient(cptr->listener)) {
        fdlist = FDLIST_IRCD;
        doauth = 2;
    } else if (IsListenerServer(cptr->listener)) { 
	fdlist = FDLIST_IRCD;
	doauth = 1;
    } else if (IsListenerService(cptr->listener)) {
        fdlist = FDLIST_IRCD;
        doauth = 1;
    }

#ifdef HAVE_ENCRYPTION_ON
    if (IsSSL(cptr)) {
	if (!SSL_is_init_finished(cptr->ssl)) {
	    if (!safe_SSL_accept(cptr, cptr->fd)) {
		dead_link_on_read(cptr, errno);
		return;
	    } else {
		comm_setselect(fd, fdlist, COMM_SELECT_WRITE, comm_tryall_callback, cptr, 0);
		return;
	    }
	}
    }
#endif

    if (doauth == 2) {
    	start_auth(cptr);
    } else if (doauth == 1) {
	start_lookup(cptr);
    } else if (doauth == 0) {
	thisdata.check = fd;
	thisdata.client_p = cptr;
	hook_call_event(hookid_read_packet, &thisdata);
    }
}

/*
 * comm_connect_timeout() - this gets called when the socket connection
 * times out. This *only* can be called once connect() is initially
 * called ..
 */
static void comm_connect_timeout(int fd, void *notused)
{
    /* error! */
    comm_connect_callback(fd, COMM_ERR_TIMEOUT);
}

/*
 * comm_connect_dns_callback() - called at the completion of the DNS request
 *
 * The DNS request has completed, so if we've got an error, return it,
 * otherwise we initiate the connect()
 */
static void comm_connect_dns_callback(void *vptr, adns_answer * reply)
{
    fde_t *F = vptr;

    if (!reply) {
	comm_connect_callback(F->fd, COMM_ERR_DNS);
	return;
    }

    if (reply->status != adns_s_ok) {
	/* Yes, callback + return */
	comm_connect_callback(F->fd, COMM_ERR_DNS);
	MyFree(reply);
	MyFree(F->dns_query);
	return;
    }

    /* No error, set a 10 second timeout */
    comm_settimeout(F->fd, 30 * 1000, comm_connect_timeout, NULL);

    /* Copy over the DNS reply info so we can use it in the connect() */
    /*
     * Note we don't fudge the refcount here, because we aren't keeping
     * the DNS record around, and the DNS cache is gone anyway.. 
     *     -- adrian
     */
#ifdef IPV6
    if(reply->rrs.addr->addr.sa.sa_family == AF_INET6) {
        copy_s_addr(S_ADDR(F->connect.hostaddr), reply->rrs.addr->addr.inet6.sin6_addr.s6_addr);
    } else {
        /* IPv4 mapped address */
        /* This is lazy... */
        memset(&F->connect.hostaddr.sins.sin6.sin6_addr.s6_addr, 0x0000, 10);
        memset(&F->connect.hostaddr.sins.sin6.sin6_addr.s6_addr[10], 0xffff, 2);
        memcpy(&F->connect.hostaddr.sins.sin6.sin6_addr.s6_addr[12], &reply->rrs.addr->addr.inet.sin_addr.s_addr, 4);
    }
#else
    F->connect.hostaddr.sins.sin.sin_addr.s_addr = reply->rrs.addr->addr.inet.sin_addr.s_addr;
#endif

    /* Now, call the tryconnect() routine to try a connect() */
    MyFree(reply);
    comm_connect_tryconnect(F->fd, NULL);
}

/* static void comm_connect_tryconnect(int fd, void *notused)
 * Input: The fd, the handler data(unused).
 * Output: None.
 * Side-effects: Try and connect with pending connect data for the FD. If
 *               we succeed or get a fatal error, call the callback.
 *               Otherwise, it is still blocking or something, so register
 *               to select for a write event on this FD.
 */
static void comm_connect_tryconnect(int fd, void *notused)
{
    int retval;
    assert(fd_table[fd].connect.callback != NULL);
    /* Try the connect() */
    retval =
	connect(fd,
		(struct sockaddr *) &SOCKADDR(fd_table[fd].connect.hostaddr),
		sizeof(struct irc_sockaddr));
    /* Error? */
    if (retval < 0) {
	/*
	 * If we get EISCONN, then we've already connect()ed the socket,
	 * which is a good thing.
	 *   -- adrian
	 */
	if (errno == EISCONN)
	    comm_connect_callback(fd, COMM_OK);
	else if (ignoreErrno(errno))
	    /* Ignore error? Reschedule */
	    comm_setselect(fd, FDLIST_IRCD, COMM_SELECT_WRITE, comm_connect_tryconnect, NULL, 0);
	else
	    /* Error? Fail with COMM_ERR_CONNECT */
	    comm_connect_callback(fd, COMM_ERR_CONNECT);
	return;
    }
    /* If we get here, we've suceeded, so call with COMM_OK */
    comm_connect_callback(fd, COMM_OK);
}

/*
 * comm_error_str() - return an error string for the given error condition
 */
const char *comm_errstr(int error)
{
    if (error < 0 || error >= COMM_ERR_MAX)
	return "Invalid error number!";
    return comm_err_str[error];
}

/*
 * comm_open() - open a socket
 *
 * This is a highly highly cut down version of squid's comm_open() which
 * for the most part emulates socket(), *EXCEPT* it fails if we're about
 * to run out of file descriptors.
 */
int comm_open(int family, int sock_type, int proto, const char *note)
{
    int fd;
    /* First, make sure we aren't going to run out of file descriptors */
    if (number_fd >= MAXCONNECTIONS) {
	errno = ENFILE;
	return -1;
    }

    /*
     * Next, we try to open the socket. We *should* drop the reserved FD
     * limit if/when we get an error, but we can deal with that later.
     * XXX !!! -- adrian
     */

    fd = socket(family, sock_type, proto);
    if (fd < 0)
	return -1;		/* errno will be passed through, yay.. */

    /* Set the socket non-blocking, and other wonderful bits */
    if (!set_non_blocking(fd)) {
	close(fd);
	return -1;
    }

    /* Next, update things in our fd tracking */
    fd_open(fd, FD_SOCKET, note);
    return fd;
}

/*
 * comm_accept() - accept an incoming connection
 *
 * This is a simple wrapper for accept() which enforces FD limits like
 * comm_open() does.
 */
int comm_accept(int fd, struct irc_sockaddr *pn)
{
    int newfd;
    socklen_t addrlen = sizeof(struct irc_sockaddr);
    if (number_fd >= MAXCONNECTIONS) {
	errno = ENFILE;
	return -1;
    }

    /*
     * Next, do the accept(). if we get an error, we should drop the
     * reserved fd limit, but we can deal with that when comm_open()
     * also does it. XXX -- adrian
     */

    newfd = accept(fd, (struct sockaddr *) &PSOCKADDR(pn), (int *) &addrlen);

    if (newfd < 0)
	return -1;

    /* Set the socket non-blocking, and other wonderful bits */
    if (!set_non_blocking(newfd)) {
	close(newfd);
	return -1;
    }
    /* Next, tag the FD as an incoming connection */
    fd_open(newfd, FD_SOCKET, "Incoming connection");

    /* .. and return */
    return newfd;
}
