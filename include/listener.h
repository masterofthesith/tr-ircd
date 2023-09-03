/* - Internet Relay Chat, include/listener.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
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
 *
 * $Id: listener.h,v 1.4 2003/07/01 11:01:18 tr-ircd Exp $
 */
#ifndef LISTENER_H
#define LISTENER_H 1

struct Listener {
  struct Listener* next;               	/* list node pointer */
  const char*      name;               	/* listener name */
  int              fd;                 	/* file descriptor */
  int              port;               	/* listener IP port */
  int              ref_count;          	/* number of connection references */
  int              active;             	/* current state of listener */
  int              index;              	/* index into poll array */
  int 		   flags;	       	/* flags of the listener: client/server/service */
  time_t           last_accept;        	/* last time listener accepted */
  struct irc_inaddr addr;              	/* virtual address or INADDR_ANY */
  struct DNSQuery  *dns_query;
  char             vhost[HOSTLEN + 1]; 	/* virtual name of listener */
  char		   *host_header;        		
  
  uuid_t	   uuid;		/* the unique id of the listener group */
};

extern struct Listener *ListenerPollList;

extern void        add_listener(int port, const char* vaddr_ip, int flags, uuid_t u, const char* host_header);
extern void        close_listener(struct Listener* listener);
extern void        close_listeners(void);
extern const char* get_listener_name(const struct Listener* listener);
extern void        show_ports(struct Client* client);
extern struct Listener *make_listener(int port, struct irc_inaddr *addr);
extern struct Listener *find_listener_by_uuid(char *);
extern void free_listener(struct Listener * );

#define LISTENER_CLIENT		0x001
#define LISTENER_SERVER		0x002
#define	LISTENER_SERVICE	0x004
#define LISTENER_GROUPED	0x008
#define LISTENER_HTTP		0x010
#define LISTENER_CLIENT_SSL	0x020

#define IsListenerClient(x)	(x && ((x)->flags & LISTENER_CLIENT))
#define IsListenerServer(x)     (x && ((x)->flags & LISTENER_SERVER))
#define IsListenerService(x)    (x && ((x)->flags & LISTENER_SERVICE))
#define IsListenerClientSSL(x)	(x && ((x)->flags & LISTENER_CLIENT_SSL))
#define IsListenerHttp(x)	(x && ((x)->flags & LISTENER_HTTP))
#define IsListenerGrouped(x)	(x && ((x)->flags & LISTENER_GROUPED))

#endif
