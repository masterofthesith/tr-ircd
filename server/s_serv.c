/***********************************************************************
 *   IRC - Internet Relay Chat, server/s_serv.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers.
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
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "h.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "confitem.h"
#include "listener.h"
#include "ircsprintf.h"
#include "packet.h"
#include "throttle.h"
#include "zlink.h"
#include "dh.h"
#include "hook.h"

static int do_server_estab(aClient *);
static void serv_connect_callback(int fd, int status, void *data);

static int hookid_sendtnick_ts = 0;
static int hookid_inform_other_servers = 0;
static int hookid_start_server_estab = 0;
static int hookid_continue_server_estab = 0;
static int hookid_connect_to_server = 0;

void init_server(void)
{
    hookid_sendtnick_ts = hook_add_event("sendnick TS");
    hookid_inform_other_servers = hook_add_event("inform other servers");
    hookid_start_server_estab = hook_add_event("start server estab");
    hookid_continue_server_estab = hook_add_event("continue server estab");
    hookid_connect_to_server = hook_add_event("connect to server");
    return;
}

/*
 * add_server_to_list()
 * input        - pointer to client
 * output       - none
 * side effects - server is added to global_serv_list
 */
void add_server_to_list(struct Client *client_p)
{
    dlink_node *ptr;

    ptr = make_dlink_node();
    dlinkAdd(client_p, ptr, &global_serv_list);

    return;
}

/*
 * remove_server_from_list()
 *
 * input        - pointer to client
 * output       - none
 * side effects - server is removed from GlocalServerList
 */
void remove_server_from_list(struct Client *client_p)
{
    dlink_node *ptr;
    struct Client *target_p;

    for (ptr = global_serv_list.head; ptr; ptr = ptr->next) {
	target_p = ptr->data;

	if (client_p == target_p) {
	    dlinkDelete(ptr, &global_serv_list);
	    free_dlink_node(ptr);
	    break;
	}
    }
    return;
}

/*
 * try_connections - scan through configuration and try new connections.
 * Returns the calendar time when the next call to this
 * function should be made latest. (No harm done if this
 * is called earlier or later...)
 */
void try_connections(void *unused)
{
    struct ConfItem *aconf;
    struct Client *client_p;
    int connecting = FALSE;
    int confrq;
    time_t next = 0;
    struct ConnClass *cltmp;
    struct ConfItem *con_conf = NULL;

    logevent_call(LogSys.conncheck, smalldate(timeofday));

    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
	/*
	 * Also when already connecting! (update holdtimes) --SRB 
	 */
	if (!(aconf->status & CONF_SERVER) || aconf->port <= 0)
	    continue;
	cltmp = aconf->class;
	/*
	 * Skip this entry if the use of it is still on hold until
	 * future. Otherwise handle this entry (and set it on hold
	 * until next time). Will reset only hold times, if already
	 * made one successfull connection... [this algorithm is
	 * a bit fuzzy... -- msa >;) ]
	 */
	if (aconf->hold > timeofday) {
	    if (next > aconf->hold || next == 0)
		next = aconf->hold;
	    continue;
	}

	confrq = get_con_freq(cltmp);

	aconf->hold = timeofday + confrq;
	/*
	 * Found a CONNECT config with port specified, scan clients
	 * and see if this server is already connected?
	 */
	client_p = find_server(aconf->name);
	if (!client_p && (cltmp->links < cltmp->max_connections) && !connecting) {
	    con_conf = aconf;
	    /* We connect only one at time... */
	    connecting = TRUE;
	}
	if ((next > aconf->hold) || (next == 0))
	    next = aconf->hold;
    }

    if (connecting) {
	if (con_conf->next) {	/* are we already last? */
    struct ConfItem **pconf;
	    for (pconf = &GlobalConfItemList; (aconf = *pconf); pconf = &(aconf->next))
		/* 
		 * put the current one at the end and
		 * make sure we try all connections
		 */
		if (aconf == con_conf)
		    *pconf = aconf->next;
	    (*pconf = con_conf)->next = 0;
	}
	if (con_conf->flags & CONF_FLAGS_ALLOW_AUTO_CONN) {
	    /*
	     * We used to only print this if serv_connect() actually
	     * suceeded, but since comm_tcp_connect() can call the callback
	     * immediately if there is an error, we were getting error messages
	     * in the wrong order. SO, we just print out the activated line,
	     * and let serv_connect() / serv_connect_callback() print an
	     * error afterwards if it fails.
	     *   -- adrian
	     */
	    sendto_lev(SNOTICE_LEV,
		       "Connection to %s[%s] activated.", con_conf->name, con_conf->host);
	    serv_connect(con_conf, 0);
	}
	logevent_call(LogSys.conncheck, smalldate(next));
    }
}

int check_server(char *name, struct Client *client_p)
{
    struct ConfItem *aconf = NULL;
    struct ConfItem *server_aconf = NULL;
    int error = -2;
    char *q;
    char passarr[PASSWDLEN];

    assert(0 != client_p);

    if (!(client_p->passwd))
	return NOT_AUTHORIZED;

    /* loop through looking for all possible connect items that might work */
    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
	if ((aconf->status & CONF_SERVER) == 0)
	    continue;

	if (match(aconf->name, name))
	    continue;

	error = -3;

	/* XXX: Fix me for IPv6 */
	/* XXX sockhost is the IPv4 ip as a string */

        if (match(aconf->host, client_p->sockhost) && match(aconf->host, client_p->hostip)) {
            error = NOT_AUTHORIZED;
        } else {
            q = client_p->passwd;
            if (q && strcmp(aconf->passwd, calcpass(q, passarr)) == 0)   
                server_aconf = aconf;
        }
    }
    if (server_aconf == NULL)
	return error;

    if (client_p->listener && !IsListenerServer(client_p->listener))
	return INVALID_CONNECTION;

    attach_conf(client_p, server_aconf);

    if (IsConfCompressed(server_aconf))
	client_p->protoflags |= PFLAGS_DOZIP;
    if (IsConfEncrypted(server_aconf))
	client_p->protoflags |= PFLAGS_DODKEY;

    /* Now find all leaf or hub config items for this server */
    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {

	if (match(aconf->name, name))
	    continue;

	attach_conf(client_p, aconf);
    }

    if (aconf != NULL) {
#ifdef IPV6
	if (IN6_IS_ADDR_UNSPECIFIED((struct in6_addr *)
				    &IN_ADDR(aconf->ipnum)))
#else
	if (IN_ADDR(aconf->ipnum) == INADDR_NONE)
#endif
	    copy_s_addr(IN_ADDR(aconf->ipnum), IN_ADDR(client_p->ip));
    }
    return 0;
}

static void sendnick_TS(aClient *cptr, aClient *acptr)
{
    struct hook_data thisdata;
    thisdata.client_p = cptr;
    thisdata.source_p = acptr;

    hook_call_event(hookid_sendtnick_ts, &thisdata);
}

    /*
     * server_estab
     *
     * inputs       - pointer to a struct Client
     * output       -
     * side effects -
     */

int server_estab(struct Client *client_p)
{
    struct ConfItem *aconf;
    struct hook_data thisdata;
    const char *inpath;
    static char inpath_ip[HOSTLEN * 2 + USERLEN + 5];
    char *host;
    dlink_node *m;

    assert(0 != client_p);
    ClearAccess(client_p);

    strcpy(inpath_ip, get_client_name(client_p, SHOW_IP));
    inpath = get_client_name(client_p, MASK_IP);	/* "refresh" inpath with host */
    host = client_p->name;

    if (!(aconf = find_conf_name(&client_p->confs, host, CONF_SERVER))) {
	/* This shouldn't happen, better tell the ops... -A1kmm */
	sendto_gnotice("Warning: Lost connect{} block "
		       "for server %s(this shouldn't happen)!", host);
	return exit_client(client_p, client_p, "Lost connect{} block!");
    }
    memset((void *) client_p->passwd, 0, sizeof(client_p->passwd));

    /* Its got identd , since its a server */
    SetGotId(client_p);
    if (IsUnknown(client_p)) {
	thisdata.client_p = client_p;
	thisdata.confitem = aconf;
	/*
	 * jdc -- 1.  Use EmptyString(), not [0] index reference.
	 *        2.  Check aconf->spasswd, not aconf->passwd.
	 */
	thisdata.check = EmptyString(aconf->spasswd);

	hook_call_event(hookid_start_server_estab, &thisdata);
    }

    det_confs_butmask(client_p, CONF_SERVER);
    /*
     * WARNING
     * In the following code in place of plain server's
     * name we send what is returned by get_client_name
     * which may add the "sockhost" after the name. It's
     * *very* *important* that there is a SPACE between
     * the name and sockhost (if present). The receiving
     * server will start the information field from this
     * first blank and thus puts the sockhost into info.
     * ...a bit tricky, but you have been warned, besides
     * code is more neat this way...  --msa
     */
    SetServer(client_p);
    client_p->servptr = &me;

    /* Some day, all these lists will be consolidated *sigh* */
    add_client_to_llist(&(me.serv->servers), client_p);

    me.serv->servercnt++;

    if (IsConfUltimate(aconf)) {
	client_p->protoflags |= PFLAGS_ULINE;
	Count.myulined++;
    }

    if (IsConfHub(aconf))
	client_p->protoflags |= PFLAGS_ISHUB;

    m = dlinkFind(&unknown_list, client_p);

    if (m != NULL) {
	Count.unknown--;
	dlinkDelete(m, &unknown_list);
	dlinkAdd(client_p, m, &serv_list);
    }

    Count.server++;
    Count.myserver++;
    GeneralOpts.split = 0;

    add_server_to_list(client_p);
    add_to_client_hash_table(client_p->name, client_p);

    /* doesnt duplicate client_p->serv if allocated this struct already */
    make_server(client_p);

    client_p->firsttime = timeofday;

    throttle_remove(client_p->hostip);

    client_p->serv->nline = aconf;
    fd_note(client_p->fd, "Server: %s", client_p->name);

#ifdef HAVE_ENCRYPTION_ON
    if (!CanDoDKEY(client_p) || !WantDKEY(client_p))
	return continue_server_estab(client_p);
    else {
	SetNegoServer(client_p);	/* VERY IMPORTANT THAT THIS IS HERE */
	sendto_one(client_p, "%s START", MSG_DKEY);
    }
    return 0;
#else
    return continue_server_estab(client_p);
#endif

}

int continue_server_estab(struct Client *client_p)
{
    struct hook_data thisdata;
    thisdata.client_p = client_p;

    /*
     * Pass on my client information to the new server
     * 
     * First, pass only servers (idea is that if the link gets
     * cancelled beacause the server was already there,
     * there are no NICK's to be cancelled...). Of course,
     * if cancellation occurs, all this info is sent anyway,
     * and I guess the link dies when a read is attempted...? --msa
     * 
     * Note: Link cancellation to occur at this point means
     * that at least two servers from my fragment are building
     * up connection this other fragment at the same time, it's
     * a race condition, not the normal way of operation...
     * 
     * ALSO NOTE: using the get_client_name for server names--
     *     see previous *WARNING*!!! (Also, original inpath
     *     is destroyed...)
     */

    hook_call_event(hookid_continue_server_estab, &thisdata);

    return do_server_estab(client_p);
}

static int do_server_estab(aClient *cptr)
{
    aClient *acptr;
    aChannel *chptr;
    dlink_node *ptr;
    struct hook_data thisdata;
    struct ChanMember *cm;
    char *inpath = get_client_name(cptr, HIDEME);

    if (IsZipCapable(cptr) && DoZipThis(cptr)) {
	sendto_one_server(cptr, NULL, TOK1_SVINFO, "ZIP");
	SetZipOut(cptr);
	cptr->serv->zout = output_zipstream();
    }

    cptr->pingval = get_client_ping(cptr);
    cptr->sendqlen = get_sendq(cptr);
    if (cptr->id.string[0]) {
    char b64id[8];
	/*
	 * we need to do this or we strcpy on top of ourselves.
	 * somewhat kludgy, but this is the best way to do things without
	 * writing another function to handle "local" servers - lucas
	 */
	strcpy(b64id, cptr->id.string);
	if (add_base64_server(cptr, b64id)) {
    char *sname;
	    acptr = find_server_by_base64_id(b64id, NULL);
	    sname = acptr ? acptr->name : "*UNKNOWN*";
	    sendto_gnotice
		("Link %s overridden, identity %s already held by %s",
		 get_client_name(cptr, HIDEME), b64id, sname);
	    return exit_client(cptr, &me, "Duplicate identity overridden");
	}
    }

    thisdata.client_p = cptr;
    hook_call_event(hookid_inform_other_servers, &thisdata);

    sendto_gnotice("Link with %s established, states:%s%s%s%s%s",
		   inpath,
		   ZipOut(cptr) ? " Output-compressed" : "",
		   RC4EncLink(cptr) ? " encrypted" : "",
		   IsULine(cptr) ? " ULined" : "",
		   HasID(cptr) ? " Identity-capable" : "", IsTS7(cptr) ? " TS7" : " Non-TS7");
    /*
     * Notify everyone of the fact that this has just linked: the entire network should get two
     * of these, one explaining the link between me->serv and the other between serv->me 
     */
    sendto_serv_butone(NULL, &me, TOK1_GNOTICE,
		       ":Link with %s established: %s",
		       inpath, IsTS7(cptr) ? "TS7 link" : "Non-TS7 link!");
    sendto_service(SERVICE_SEE_SERVERS, 0, &me, NULL, TOK1_SERVER, "%s 2 :%s", cptr->name,
		   cptr->info);

    push_all_maskitems(cptr, MASKITEM_QUARANTINE);
    push_all_maskitems(cptr, MASKITEM_GECOS);
    push_all_maskitems(cptr, MASKITEM_JUPITER);
    push_all_maskitems(cptr, MASKITEM_ZAPLINE);

    send_services(cptr);

    /*
     * Bursts are about to start.. send a BURST 
     */
    if (!IsULine(cptr))
        sendto_one_server(cptr, NULL, TOK1_BURST, "");

    /*
     * * Send it in the shortened format with the TS, if it's a TS
     * server; walk the list of channels, sending all the nicks that
     * haven't been sent yet for each channel, then send the channel
     * itself -- it's less obvious than sending all nicks first, but
     * on the receiving side memory will be allocated more nicely 
     * saving a few seconds in the handling of a split -orabidoo
     */
    for (chptr = channel; chptr; chptr = chptr->nextch) {
	for (ptr = chptr->members.head; ptr; ptr = ptr->next) {
	    cm = ptr->data;
	    acptr = cm->client_p;
	    if (!NickIsSent(acptr)) {
		if (acptr->from != cptr) {
		    sendnick_TS(cptr, acptr);
		    acptr->protoflags |= PFLAGS_NICKISSENT;
		}
	    }
	}
	send_channel_modes(cptr, chptr);
    }
    /*
     * also send out those that are not on any channel 
     */
    for (acptr = &me; acptr; acptr = acptr->prev) {
	if (!NickIsSent(acptr)) {
	    if (acptr->from != cptr) {
		sendnick_TS(cptr, acptr);
		acptr->protoflags |= PFLAGS_NICKISSENT;
	    }
	}
    }
    for (acptr = &me; acptr; acptr = acptr->prev)
	acptr->protoflags &= ~PFLAGS_NICKISSENT;


    if (ZipOut(cptr)) {
    unsigned long inb, outb;
    double rat;
	zip_get_stats(cptr->serv->zout, &inb, &outb, &rat);
	if (inb) {
	    sendto_gnotice
		("Connect burst to %s: %lu bytes normal, %lu compressed (%3.2f%%)",
		 get_client_name(cptr, HIDEME), inb, outb, rat);
	    sendto_serv_butone(cptr, &me, TOK1_GNOTICE,
			       ":Connect burst to %s: %lu bytes normal, %lu compressed (%3.2f%%)",
			       get_client_name(cptr, HIDEME), inb, outb, rat);
	}
    }
    /*
     * stuff a PING at the end of this burst so we can figure out when
     * the other side has finished processing it. 
     */
    cptr->flags |= FLAGS_BURST | FLAGS_PINGSENT;
    if (!IsULine(cptr))
	cptr->flags |= FLAGS_SOBSENT;
    sendto_one_server(cptr, NULL, TOK1_PING, ":%C", &me);
    return 0;
}

/*
 * serv_connect() - initiate a server connection
 *
 * inputs       - pointer to conf 
 *              - pointer to client doing the connet
 * output       -
 * side effects -
 *
 * This code initiates a connection to a server. It first checks to make
 * sure the given server exists. If this is the case, it creates a socket,
 * creates a client, saves the socket information in the client, and
 * initiates a connection to the server through comm_connect_tcp(). The
 * completion of this goes through serv_completed_connection().
 *
 * We return 1 if the connection is attempted, since we don't know whether
 * it suceeded or not, and 0 if it fails in here somewhere.
 */

int serv_connect(aConfItem *aconf, struct Client *by)
{
    aClient *client_p;
    int fd;
    char buf[HOSTIPLEN];
    /* Make sure aconf is useful */
    assert(aconf != NULL);
    /* log */
    inetntop(DEF_FAM, &IN_ADDR(aconf->ipnum), buf, HOSTIPLEN);
    /*
     * Make sure this server isn't already connected
     * Note: aconf should ALWAYS be a valid C: line
     */
    if ((client_p = find_server(aconf->name))) {
	sendto_gnotice("Server %s already present from %s",
		       aconf->name, get_client_name(client_p, SHOW_IP));
	if (by && IsPerson(by) && !MyClient(by))
	    send_me_notice(by,
			   ":Server %s already present from %s",
			   aconf->name, get_client_name(client_p, HIDEME));
	return 0;
    }

    /* create a socket for the server connection */
    if ((fd = comm_open(DEF_FAM, SOCK_STREAM, 0, NULL)) < 0) {
	/* Eek, failure to create the socket */
	report_error("opening stream socket to %s: %s", aconf->name, errno);
	return 0;
    }

    /* servernames are always guaranteed under HOSTLEN chars */
    fd_note(fd, "Server: %s", aconf->name);

    /* Create a local client */
    client_p = make_client(NULL);

    /* Copy in the server, hostname, fd */
    strlcpy_irc(client_p->name, aconf->name, HOSTLEN);
    strlcpy_irc(client_p->hostip, aconf->host, HOSTLEN);

    inetntop(DEF_FAM, &IN_ADDR(aconf->ipnum), client_p->hostip, HOSTIPLEN);
    client_p->fd = fd;
    /*
     * Set up the initial server evilness, ripped straight from
     * connect_server(), so don't blame me for it being evil.
     *   -- adrian
     */
    if (!set_non_blocking(client_p->fd)) 
	report_error(NONB_ERROR_MSG, get_client_name(client_p, TRUE), errno);

    if (!set_sock_buffers(client_p->fd, READBUF_SIZE)) 
	report_error(SETBUF_ERROR_MSG, get_client_name(client_p, TRUE), errno);

    /*
     * NOTE: if we're here we have a valid C:Line and the client should
     * have started the connection and stored the remote address/port and
     * ip address name in itself
     *
     * Attach config entries to client here rather than in
     * serv_connect_callback(). This to avoid null pointer references.
     */
    if (!attach_cn_lines(client_p, aconf->name, aconf->host)) {
        dlink_node * ptr;
        ptr = dlinkFind(&unknown_list, client_p);
        if (ptr)
            dlinkDeleteNode(ptr, &unknown_list);
	sendto_gnotice("Host %s is not enabled for connecting:no C/N-line", aconf->name);
	if (by && IsPerson(by) && !MyClient(by))
	    send_me_notice(by, ":Connect to host %s failed.", client_p->name);
	det_confs_butmask(client_p, 0);
	free_client(client_p);
	return 0;
    }
    /*
     * at this point we have a connection in progress and C/N lines
     * attached to the client, the socket info should be saved in the
     * client and it should either be resolved or have a valid address.
     *
     * The socket has been connected or connect is in progress.
     */
    make_server(client_p);
    if (by && IsPerson(by)) {
	strcpy(client_p->serv->bynick, by->name);
	if (client_p->serv->user)
	    free_user(client_p->serv->user);
	client_p->serv->user = by->user;
    } else {
	strcpy(client_p->serv->bynick, "AutoConn.");
	if (client_p->serv->user)
	    free_user(client_p->serv->user);
	client_p->serv->user = NULL;
    }

    SetConnecting(client_p);

    add_client_to_list(client_p);

    /* from def_fam */
    client_p->aftype = aconf->aftype;

    /* Now, initiate the connection */
    if ((aconf->aftype == AF_INET) && ServerInfo.specific_ipv4_vhost) {
    struct irc_sockaddr ipn;
	memset(&ipn, 0, sizeof(struct irc_sockaddr));
	S_FAM(ipn) = DEF_FAM;
	S_PORT(ipn) = 0;
	copy_s_addr(S_ADDR(ipn), IN_ADDR(ServerInfo.address));
	comm_connect_tcp(client_p->fd, aconf->host,
			 aconf->port,
			 (struct sockaddr *) &SOCKADDR(ipn),
			 sizeof(struct irc_sockaddr),
			 serv_connect_callback, client_p, aconf->aftype, 30);
#ifdef IPV6
    } else if ((aconf->aftype == AF_INET6) && ServerInfo.specific_ipv6_vhost) {
    struct irc_sockaddr ipn;
	memset(&ipn, 0, sizeof(struct irc_sockaddr));
	S_FAM(ipn) = AF_INET6;
	S_PORT(ipn) = 0;

	copy_s_addr(S_ADDR(ipn), IN_ADDR(ServerInfo.address6));

	comm_connect_tcp(client_p->fd, aconf->host, aconf->port,
			 (struct sockaddr *) &SOCKADDR(ipn), sizeof(struct irc_sockaddr),
			 serv_connect_callback, client_p, aconf->aftype, 30);
#endif
    } else {
	comm_connect_tcp(client_p->fd, aconf->host,
			 aconf->port, NULL, 0, serv_connect_callback, client_p, aconf->aftype, 30);
    }
    return 1;
}

/*
 * serv_connect_callback() - complete a server connection.
 * 
 * This routine is called after the server connection attempt has
 * completed. If unsucessful, an error is sent to ops and the client
 * is closed. If sucessful, it goes through the initialisation/check
 * procedures, the capabilities are sent, and the socket is then
 * marked for reading.
 */

static void serv_connect_callback(int fd, int status, void *data)
{
    struct hook_data thisdata;
    aClient *client_p = data;
    aConfItem *aconf;

    /* First, make sure its a real client! */
    assert(client_p != NULL);
    assert(client_p->fd == fd);
    /* Next, for backward purposes, record the ip of the server */
    copy_s_addr(IN_ADDR(client_p->ip), S_ADDR(fd_table[fd].connect.hostaddr));
    /* Check the status */
    if (status != COMM_OK) {
	/* We have an error, so report it and quit */
	sendto_gnotice("Error connecting to %s: %s", client_p->name, comm_errstr(status));
	client_p->flags |= FLAGS_DEADSOCKET;
	exit_client(client_p, &me, "Connection error");
	return;
    }

    /* COMM_OK, so continue the connection procedure */
    aconf = find_conf_name(&client_p->confs, client_p->name, CONF_SERVER);
    if (!aconf) {
	sendto_gnotice("Lost C-Line for %s", get_client_name(client_p, HIDEME));
	exit_client(client_p, &me, "Lost C-line");
	return;
    }

    /* Next, send the initial handshake */
    SetHandshake(client_p);

    if (IsConfUltimate(aconf))
	client_p->protoflags |= PFLAGS_ULINE;

    if (IsConfHub(aconf))
	client_p->protoflags |= PFLAGS_ISHUB;

    if (IsConfCompressed(aconf))
	client_p->protoflags |= PFLAGS_DOZIP;

    if (IsConfEncrypted(aconf))
	client_p->protoflags |= PFLAGS_DODKEY;

    /*
     * jdc -- Check and send spasswd, not passwd.
     */

    thisdata.check = EmptyString(aconf->passwd);
    thisdata.client_p = client_p;
    thisdata.confitem = aconf;

    hook_call_event(hookid_connect_to_server, &thisdata);

    /* 
     * If we've been marked dead because a send failed, just exit
     * here now and save everyone the trouble of us ever existing.
     */
    if (IsDead(client_p)) {
	sendto_gnotice("%s went dead during handshake", client_p->name);
	exit_client(client_p, &me, "Went dead during handshake");
	return;
    }

    /* If we get here, we're ok, so lets start reading some data */
    comm_setselect(fd, FDLIST_IRCD, COMM_SELECT_READ, read_server_packet, client_p, 0);
}
