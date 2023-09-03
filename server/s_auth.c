/************************************************************************
 *   IRC - Internet Relay Chat, server/s_auth.c
 *   Copyright (C) 1992 Darren Reed
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
 *   $Id: s_auth.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
 *
 * Changes:
 *   July 6, 1999 - Rewrote most of the code here. When a client connects
 *     to the server and passes initial socket validation checks, it
 *     is owned by this module (auth) which returns it to the rest of the
 *     server when dns and auth queries are finished. Until the client is
 *     released, the server does not know it exists and does not process
 *     any messages from it.
 *     --Bleep  Thomas Helvey <tomh@inxpress.net>
 */

#include "struct.h"
#include "common.h"
#include "s_auth.h"
#include "event.h"
#include "fd.h"			/* fdlist_add */
#include "sys.h"
#include "h.h"
#include "numeric.h"
#include "packet.h"
#include "resnew.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "config.h"
#include "dh.h"

dlink_list auth_client_list;
dlink_list auth_poll_list;

static EVH timeout_auth_queries_event;

static PF read_auth_reply;
static CNCB auth_connect_callback;

/*
 * a bit different approach
 * this replaces the original sendheader macros
 */

struct {
    const char *message;
    size_t length;
} HeaderMessages[] = {
    /* 123456789012345678901234567890123456789012345678901234567890 */
    {"NOTICE AUTH :*** Looking up your hostname...\r\n", 46}, 
    {"NOTICE AUTH :*** Found your hostname\r\n", 38}, 
    {"NOTICE AUTH :*** Found your hostname, cached\r\n", 46}, 
    {"NOTICE AUTH :*** Couldn't look up your hostname\r\n", 49}, 
    {"NOTICE AUTH :*** Checking Ident\r\n", 33}, 
    {"NOTICE AUTH :*** Got Ident response\r\n", 37}, 
    {"NOTICE AUTH :*** No Ident response\r\n", 36}, 
    {"NOTICE AUTH :*** Your forward and reverse DNS do not match, ignoring hostname.\r\n", 80}, 
    {"NOTICE AUTH :*** Your hostname is too long, ignoring hostname\r\n", 63},
    {"NOTICE AUTH :*** This server runs an open proxy/wingate detection monitor from ", 79},  /* !!! Do not add \r\n */
    {"NOTICE AUTH :*** If you do not accept this, disconnect now. Further information is available at ", 96}, /* Do not add \r\n */ 
};

typedef enum {
    REPORT_DO_DNS,
    REPORT_FIN_DNS,
    REPORT_FIN_DNSC,
    REPORT_FAIL_DNS,
    REPORT_DO_ID,
    REPORT_FIN_ID,
    REPORT_FAIL_ID,
    REPORT_IP_MISMATCH,
    REPORT_HOST_TOOLONG,
    REPORT_RUN_PROXYSCANNER,
    REPORT_DISCONNECT,
} ReportType;

#ifdef HAVE_ENCRYPTION_ON
#define sendheader(c, r) \
    if (IsSSL(c)) \
	safe_SSL_write(c, HeaderMessages[(r)].message, HeaderMessages[(r)].length); \
    else \
   	send((c)->fd, HeaderMessages[(r)].message, HeaderMessages[(r)].length, 0)
#define sendstring(c, t, l, n) \
    if (IsSSL(c)) \
	safe_SSL_write(c, t, l); \
    else \
	send((c)->fd, t, l, n);
#else
#define sendheader(c, r) send((c)->fd, HeaderMessages[(r)].message, HeaderMessages[(r)].length, 0)
#define sendstring(c, t, l, n) send((c)->fd, t, l, n)
#endif

/*
 * init_auth()
 *
 * Initialise the auth code
 */
void init_auth(void)
{
    memset(&auth_client_list, 0, sizeof(auth_client_list));
    memset(&auth_poll_list, 0, sizeof(auth_poll_list));
    eventAdd("timeout_auth_queries_event", timeout_auth_queries_event, NULL, 1);
}

/*
 * make_auth_request - allocate a new auth request
 */
static struct AuthRequest *make_auth_request(struct Client *client)
{
    struct AuthRequest *request = (struct AuthRequest *) MyMalloc(sizeof(struct AuthRequest));
    request->fd = -1;
    request->client = client;
    request->timeout = timeofday + CONNECTTIMEOUT;
    return request;
}

/*
 * free_auth_request - cleanup auth request allocations
 */
void free_auth_request(struct AuthRequest *request)
{
    MyFree(request);
}

/*
 * unlink_auth_request - remove auth request from a list
 */
static void unlink_auth_request(struct AuthRequest *request, dlink_list * list)
{
    dlink_node *ptr;
    if ((ptr = dlinkFind(list, request)) != NULL) {
	dlinkDelete(ptr, list);
	free_dlink_node(ptr);
    }
}

/*
 * link_auth_request - add auth request to a list
 */
static void link_auth_request(struct AuthRequest *request, dlink_list * list)
{
    dlink_node *m;

    m = make_dlink_node();
    dlinkAdd(request, m, list);
}

/*
 * release_auth_client - release auth client from auth system
 * this adds the client into the local client lists so it can be read by
 * the main io processing loop
 */
static void release_auth_client(struct Client *client)
{
    if (client->fd > highest_fd)
	highest_fd = client->fd;

    /* This ensures that the client gets these notice about the scanner
     * before any information from them is parsed. -TimeMr14C
     */

    if (ServerOpts.wingate_notice && (ServerOpts.monitor_host_len > 0) && (ServerOpts.proxy_url_len > 0)) {
        sendheader(client, REPORT_RUN_PROXYSCANNER);
	sendstring(client, ServerOpts.monitor_host, ServerOpts.monitor_host_len, 0);
	sendstring(client, "\r\n", 2, 0);
	sendheader(client, REPORT_DISCONNECT);
	sendstring(client, ServerOpts.proxy_url, ServerOpts.proxy_url_len, 0);
	sendstring(client, "\r\n", 2, 0);
    }

    /*
     * When a client has auth'ed, we want to start reading what it sends
     * us. This is what read_packet() does.
     *     -- adrian
     */
    client->allow_read = MAX_FLOOD_PER_SEC;
    /* comm_setselect(client->fd, FDLIST_IRCD, COMM_SELECT_READ, read_client_packet, client, 0);
     * is removed, and replaced with the read_client_packet call directly.
     * -TimeMr14C: Taken from hybrid.
     */
    comm_setflush(client->fd, 1000, flood_recalc, client);
    add_client_to_list(client);
    read_client_packet(client->fd, client);
}

/*
 * auth_dns_callback - called when resolver query finishes
 * if the query resulted in a successful search, hp will contain
 * a non-null pointer, otherwise hp will be null.
 * set the client on it's way to a connection completion, regardless
 * of success of failure
 */
static void auth_dns_callback(void *vptr, adns_answer * reply)
{

    struct AuthRequest *auth = (struct AuthRequest *) vptr;
    char *str = auth->client->sockhost;
    ClearDNSPending(auth);

    if (reply && (reply->status == adns_s_ok)) {

	if (strlen(*reply->rrs.str) < HOSTLEN) {
	    strcpy(str, *reply->rrs.str);
	    sendheader(auth->client, REPORT_FIN_DNS);
	} else {
#ifdef IPV6
	    if (auth->client->aftype == AF_INET6) {
		if (*auth->client->hostip == ':') {
		    strcpy(str, "0");
		    strcat(str, auth->client->hostip);
		} else {
		    strcpy(str, auth->client->hostip);
		}
		strcat(str, ".");
	    } else
#endif
		strcpy(str, auth->client->hostip);
	    sendheader(auth->client, REPORT_HOST_TOOLONG);
	    auth->client->protoflags |= PFLAGS_NONRESOLVED;
	}
    } else {
#ifdef IPV6
	if (auth->client->aftype == AF_INET6) {
	    if (*auth->client->hostip == ':') {
		strcpy(str, "0");
		strcat(str, auth->client->hostip);
	    } else {
		strcpy(str, auth->client->hostip);
	    }
	    strcat(str, ".");
	} else
#endif
	    strcpy(str, auth->client->hostip);
	sendheader(auth->client, REPORT_FAIL_DNS);
	auth->client->protoflags |= PFLAGS_NONRESOLVED;
	/* Used for the channelmode +N preventing 
	 * non resolved addresses from joining */
    }

    MyFree(reply);
    MyFree(auth->client->dns_query);

    auth->client->dns_query = NULL;
    if (!IsDoingAuth(auth)) {
    struct Client *client_p = auth->client;
	unlink_auth_request(auth, &auth_poll_list);
	free_auth_request(auth);
	release_auth_client(client_p);
    }
}

/*
 * authsenderr - handle auth send errors
 */
static void auth_error(struct AuthRequest *auth)
{
    ++ircstp->is_abad;

    fd_close(auth->fd);
    auth->fd = -1;

    ClearAuth(auth);
    sendheader(auth->client, REPORT_FAIL_ID);

    if (!IsDNSPending(auth)) {
    struct Client *client_p = auth->client;
	unlink_auth_request(auth, &auth_poll_list);
	free_auth_request(auth);
	release_auth_client(client_p);
    }
}

/*
 * start_auth_query - Flag the client to show that an attempt to 
 * contact the ident server on
 * the client's host.  The connect and subsequently the socket are all put
 * into 'non-blocking' mode.  Should the connect or any later phase of the
 * identifing process fail, it is aborted and the user is given a username
 * of "unknown".
 */
static int start_auth_query(struct AuthRequest *auth)
{
    /*  struct sockaddr_in sock; */
    struct irc_sockaddr localaddr;
    socklen_t locallen = sizeof(struct irc_sockaddr);
    int fd;

    if ((fd = comm_open(DEF_FAM, SOCK_STREAM, 0, "ident")) == -1) {
	report_error("creating auth stream socket %s:%s",
		     get_client_name(auth->client, TRUE), errno);
	++ircstp->is_abad;
	return 0;
    }
    if ((MAXCONNECTIONS - 10) < fd) {
	sendto_lev(SNOTICE_LEV, "Can't allocate fd for auth on %s",
		   get_client_name(auth->client, TRUE));
	fd_close(fd);
	return 0;
    }

    sendheader(auth->client, REPORT_DO_ID);
    if (!set_non_blocking(fd)) {
	report_error(NONB_ERROR_MSG, get_client_name(auth->client, TRUE), errno);
	fd_close(fd);
	return 0;
    }

    /* 
     * get the local address of the client and bind to that to
     * make the auth request.  This used to be done only for
     * ifdef VIRTUAL_HOST, but needs to be done for all clients
     * since the ident request must originate from that same address--
     * and machines with multiple IP addresses are common now
     */
    memset(&localaddr, 0, locallen);
    getsockname(auth->client->fd, (struct sockaddr *) &SOCKADDR(localaddr), (int *) &locallen);
    S_PORT(localaddr) = htons(0);

    auth->fd = fd;
    SetAuthConnect(auth);

    comm_connect_tcp(fd, auth->client->hostip, 113,
		     (struct sockaddr *) &SOCKADDR(localaddr), locallen,
		     auth_connect_callback, auth, DEF_FAM, ServerOpts.identd_timeout);
    return 1;			/* We suceed here for now */
}

/*
 * GetValidIdent - parse ident query reply from identd server
 * 
 * Inputs        - pointer to ident buf
 * Output        - NULL if no valid ident found, otherwise pointer to name
 * Side effects  -
 */
static char *GetValidIdent(char *buf)
{
    int remp = 0;
    int locp = 0;
    char *colon1Ptr;
    char *colon2Ptr;
    char *colon3Ptr;
    char *commaPtr;
    char *remotePortString;

    /* All this to get rid of a sscanf() fun. */
    remotePortString = buf;

    colon1Ptr = strchr(remotePortString, ':');
    if (!colon1Ptr)
	return 0;

    *colon1Ptr = '\0';
    colon1Ptr++;
    colon2Ptr = strchr(colon1Ptr, ':');
    if (!colon2Ptr)
	return 0;

    *colon2Ptr = '\0';
    colon2Ptr++;
    commaPtr = strchr(remotePortString, ',');

    if (!commaPtr)
	return 0;

    *commaPtr = '\0';
    commaPtr++;

    remp = atoi(remotePortString);
    if (!remp)
	return 0;

    locp = atoi(commaPtr);
    if (!locp)
	return 0;

    /* look for USERID bordered by first pair of colons */
    if (!strstr(colon1Ptr, "USERID"))
	return 0;

    colon3Ptr = strchr(colon2Ptr, ':');
    if (!colon3Ptr)
	return 0;

    *colon3Ptr = '\0';
    colon3Ptr++;
    return (colon3Ptr);
}

/*
 * start_auth - starts auth (identd) and dns queries for a client
 */
void start_auth(struct Client *client)
{
    struct AuthRequest *auth = 0;
    assert(0 != client);
    if (client == NULL)
	return;
    auth = make_auth_request(client);

    client->dns_query = MyMalloc(sizeof(struct DNSQuery));
    client->dns_query->ptr = auth;
    client->dns_query->callback = auth_dns_callback;

    sendheader(client, REPORT_DO_DNS);

    /* No DNS cache now, remember? -- adrian */
    adns_getaddr(&(client->ip), client->aftype, client->dns_query);
    SetDNSPending(auth);

    start_auth_query(auth);
    link_auth_request(auth, &auth_poll_list);
}

/*
 * timeout_auth_queries - timeout resolver and identd requests
 * allow clients through if requests failed
 */
static void timeout_auth_queries_event(void *notused)
{
    dlink_node *ptr;
    dlink_node *next_ptr;
    struct AuthRequest *auth;
    struct Client *client_p;

    for (ptr = auth_poll_list.head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	auth = ptr->data;

	if (auth->timeout < timeofday) {
	    if (auth->fd >= 0)
		fd_close(auth->fd);
	    if (IsDoingAuth(auth)) {
		sendheader(auth->client, REPORT_FAIL_ID);
	    }
	    if (IsDNSPending(auth)) {
		delete_adns_queries(auth->client->dns_query);
		auth->client->dns_query->query = NULL;
		sendheader(auth->client, REPORT_FAIL_DNS);
	    }

	    auth->client->since = timeofday;
	    dlinkDelete(ptr, &auth_poll_list);
	    free_dlink_node(ptr);
	    client_p = auth->client;
	    free_auth_request(auth);
	    release_auth_client(client_p);
	}
    }
}

/*
 * auth_connect_callback() - deal with the result of comm_connect_tcp()
 *
 * If the connection failed, we simply close the auth fd and report
 * a failure. If the connection suceeded send the ident server a query
 * giving "theirport , ourport". The write is only attempted *once* so
 * it is deemed to be a fail if the entire write doesn't write all the
 * data given.  This shouldnt be a problem since the socket should have
 * a write buffer far greater than this message to store it in should
 * problems arise. -avalon
 */
static
void auth_connect_callback(int fd, int error, void *data)
{
    struct AuthRequest *auth = data;
    struct sockaddr_in us;
    struct sockaddr_in them;
    char authbuf[32];
    socklen_t ulen = sizeof(struct sockaddr_in);
    socklen_t tlen = sizeof(struct sockaddr_in);

    /* Check the error */
    if (error != COMM_OK) {
	/* We had an error during connection :( */
	auth_error(auth);
	return;
    }

    if (getsockname
	(auth->client->fd, (struct sockaddr *) &us, (int *) &ulen) ||
	getpeername(auth->client->fd, (struct sockaddr *) &them, (int *) &tlen)) {
	auth_error(auth);
	return;
    }
    ircsprintf(authbuf, "%u , %u\r\n",
	       (unsigned int) ntohs(them.sin_port), (unsigned int) ntohs(us.sin_port));

    if (send(auth->fd, authbuf, strlen(authbuf), 0) == -1) {
	auth_error(auth);
	return;
    }
    ClearAuthConnect(auth);
    SetAuthPending(auth);
    read_auth_reply(auth->fd, auth);
}

/*
 * read_auth_reply - read the reply (if any) from the ident server 
 * we connected to.
 * We only give it one shot, if the reply isn't good the first time
 * fail the authentication entirely. --Bleep
 */
#define AUTH_BUFSIZ 128

static void read_auth_reply(int fd, void *data)
{
    struct AuthRequest *auth = data;
    char *s = (char *) NULL;
    char *t = (char *) NULL;
    int len;
    int count;
    char buf[AUTH_BUFSIZ + 1];	/* buffer to read auth reply into */

    len = recv(auth->fd, buf, AUTH_BUFSIZ, 0);

    if (len < 0 && ignoreErrno(errno)) {
    	comm_setselect(fd, FDLIST_IRCD, COMM_SELECT_READ,
                   read_auth_reply, auth, 0);
    	return;
    }

    if (len > 0) {
	buf[len] = '\0';

	if ((s = GetValidIdent(buf))) {
	    t = auth->client->username;

	    while (*s == '~' || *s == '^')
		s++;

	    for (count = USERLEN; *s && count; s++) {
		if (*s == '@') {
		    break;
		}
		if (!IsSpace(*s) && *s != ':' && *s != '[') {
		    *t++ = *s;
		    count--;
		}
	    }
	    *t = '\0';
	}
    }

    fd_close(auth->fd);
    auth->fd = -1;
    ClearAuth(auth);

    if (!s) {
	++ircstp->is_abad;
	strcpy(auth->client->username, "unknown");
    } else {
	sendheader(auth->client, REPORT_FIN_ID);
	++ircstp->is_asuc;
	SetGotId(auth->client);
    }

    if (!IsDNSPending(auth)) {
    struct Client *client_p = auth->client;
	unlink_auth_request(auth, &auth_poll_list);
	free_auth_request(auth);
	release_auth_client(client_p);
    }
}

/*
 * remove_auth_request()
 * Remove request 'auth' from AuthClientList, and free it.
 * There is no need to release auth->client since it has
 * already been done
 */

void remove_auth_request(struct AuthRequest *auth)
{
    unlink_auth_request(auth, &auth_client_list);
    free_auth_request(auth);
}				/* remove_auth_request() */

/*
 * FindAuthClient()
 * Find the client matching 'id' in the AuthClientList. The
 * id will match the memory address of the client structure.
 */

struct AuthRequest *
/* XXX This is just wrong, should be a *void at least */
FindAuthClient(long id)
{
    dlink_node *ptr;
    struct AuthRequest *auth;

    for (ptr = auth_client_list.head; ptr; ptr = ptr->next) {
	auth = ptr->data;
	if (auth->client == (struct Client *) id)
	    return auth;
    }
    return (NULL);
}				/* FindAuthClient() */

/*
 * delete_identd_queries()
 *
 */
void delete_identd_queries(struct Client *target_p)
{
    dlink_node *ptr;
    dlink_node *next_ptr;
    struct AuthRequest *auth;

    for (ptr = auth_poll_list.head; ptr; ptr = next_ptr) {
	auth = ptr->data;
	next_ptr = ptr->next;

	if (auth->client == target_p) {
	    if (auth->fd >= 0)
		fd_close(auth->fd);
	    dlinkDelete(ptr, &auth_poll_list);

	    free_auth_request(auth);
	    free_dlink_node(ptr);
	}
    }

    for (ptr = auth_client_list.head; ptr; ptr = next_ptr) {
	auth = ptr->data;
	next_ptr = ptr->next;

	if (auth->client == target_p) {
	    if (auth->fd >= 0)
		fd_close(auth->fd);
	    dlinkDelete(ptr, &auth_client_list);

	    free_auth_request(auth);
	    free_dlink_node(ptr);
	}
    }
}
