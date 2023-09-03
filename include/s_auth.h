/************************************************************************
 *   IRC - Internet Relay Chat, include/s_auth.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
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
 *   $Id: s_auth.h,v 1.4 2003/07/01 11:01:18 tr-ircd Exp $
 */
#ifndef S_AUTH_H
#define S_AUTH_H 	1

#include "resnew.h"

/* 
 * How many auth allocations to allocate in a block. I'm guessing that
 * a good number here is 64, because these are temporary and don't live
 * as long as clients do.
 *     -- adrian
 */
#define	AUTH_BLOCK_SIZE		64

#define PROXYMON_TARGET_STRING	"*** Looking up your hostname..."

struct AuthRequest {
  aClient      	      *client;    /* pointer to client struct for request */
  unsigned int        flags;     /* current state of request */
  int                 fd;        /* file descriptor for auth queries */
  time_t              timeout;   /* time when query expires */
};

/*
 * flag values for AuthRequest
 * NAMESPACE: AM_xxx - Authentication Module
 */
#define AM_AUTH_CONNECTING   (1 << 0)
#define AM_AUTH_PENDING      (1 << 1)
#define AM_DNS_PENDING       (1 << 2)

#define SetDNSPending(x)     ((x)->flags |= AM_DNS_PENDING)
#define ClearDNSPending(x)   ((x)->flags &= ~AM_DNS_PENDING)
#define IsDNSPending(x)      ((x)->flags &  AM_DNS_PENDING)

#define SetAuthConnect(x)    ((x)->flags |= AM_AUTH_CONNECTING)
#define ClearAuthConnect(x)  ((x)->flags &= ~AM_AUTH_CONNECTING)
#define IsAuthConnect(x)     ((x)->flags &  AM_AUTH_CONNECTING)

#define SetAuthPending(x)    ((x)->flags |= AM_AUTH_PENDING)
#define ClearAuthPending(x)  ((x)->flags &= AM_AUTH_PENDING)
#define IsAuthPending(x)     ((x)->flags &  AM_AUTH_PENDING)

#define ClearAuth(x)         ((x)->flags &= ~(AM_AUTH_PENDING | AM_AUTH_CONNECTING))
#define IsDoingAuth(x)       ((x)->flags &  (AM_AUTH_PENDING | AM_AUTH_CONNECTING))
#define SetGotId(x)       ((x)->flags |= FLAGS_GOTID) 



extern void start_auth(struct Client *);
extern void start_lookup(struct Client *);

extern void send_auth_query(struct AuthRequest* req);
extern void remove_auth_request(struct AuthRequest *req);
extern void free_auth_request(struct AuthRequest *);
extern struct AuthRequest *FindAuthClient(long id);
extern void init_auth(void);
extern void delete_identd_queries(struct Client *);

#endif
