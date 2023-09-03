/************************************************************************
 *   IRC - Internet Relay Chat, include/s_bsd.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
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
 *   $Id: s_bsd.h,v 1.5 2003/07/01 11:01:18 tr-ircd Exp $
 *
 */
#ifndef S_BSD_H
#define S_BSD_H 1

#include "fd.h"

#ifndef HAVE_SOCKLEN_T
typedef int socklen_t;
#endif

#define READBUF_SIZE    16384

/* Type of IO */
#define	COMM_SELECT_READ		1
#define	COMM_SELECT_WRITE		2
#define COMM_SELECT_NONE		3

struct Client;
struct ConfItem;
struct hostent;
struct DNSReply;
struct Listener;

#endif

extern char* const NONB_ERROR_MSG; 
extern char* const SETBUF_ERROR_MSG;

extern void do_sigio(int s);
extern void setup_sigio_fd(int);

extern void  add_connection(struct Listener*, int);
extern int   connect_server(struct ConfItem*, struct Client*, 
                            struct DNSReply*);
extern void  report_error(char*, const char*, int);
extern int   set_non_blocking(int);
extern int   set_sock_buffers(int, int);

extern void  dead_link_on_read(struct Client*, int);
extern int   get_sockerr(int);
extern int   ignoreErrno(int ierrno);

extern void  comm_settimeout(int, time_t, PF *, void *);
extern void  comm_setflush(int, time_t, PF *, void *);
extern void  comm_checktimeouts(void *);
extern void  comm_connect_tcp(int, const char *, u_short,
                 struct sockaddr *, int, CNCB *, void *, int, int);
extern const char * comm_errstr(int status);
extern int   comm_open(int family, int sock_type, int proto,
                 const char *note);
extern int   comm_accept(int fd, struct irc_sockaddr *pn);

/* These must be defined in the network IO loop code of your choice */
extern void  comm_setselect(int fd, fdlist_t list, unsigned int type,
                 PF *handler, void *client_data, time_t timeout);
extern void  init_netio(void);
extern int   comm_select(unsigned long, int *, fdlist_t);
extern int   disable_sock_options(int);

extern int serv_connect(aConfItem *aconf, struct Client *by);

extern struct sockaddr *connect_inet(aConfItem *, aClient *, int *);

extern void clear_ip_hash_table(void);

extern void flush_deleted_I_P(void);

extern void remove_one_ip(struct irc_inaddr *ip);

extern void try_connections(void *unused);


