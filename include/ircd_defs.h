/* - Internet Relay Chat, include/ircd_defs.h
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
 * $Id: ircd_defs.h,v 1.5 2003/07/07 21:59:12 tr-ircd Exp $
 *
 * ircd_defs.h - Global size definitions for record entries used
 * througout ircd. Please think 3 times before adding anything to this
 * file.
 */
#ifndef IRCD_DEFS_H
#define IRCD_DEFS_H 1

#ifdef __CYGWIN32__
#define uint32_t u_int32_t
#endif

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#ifndef INADDRSZ
#define INADDRSZ 4
#endif

#ifdef IPV6
#ifndef IN6ADDRSZ
#define IN6ADDRSZ 16
#endif
#endif

#ifndef INT16SZ
#define INT16SZ 2
#endif


struct irc_inaddr
{
	union {
		struct in_addr sin;
#ifdef IPV6
		struct in6_addr sin6;	
#endif
	} sins;
};

struct irc_sockaddr
{
	union {
		struct sockaddr_in sin;
#ifdef IPV6
		struct sockaddr_in6 sin6;
#endif			
	} sins;
};

#define IN4_ADDR(x) x.sins.sin.s_addr

#ifdef IPV6
#define copy_s_addr(a, b)  \
do { \
((uint32_t *)a)[0] = ((uint32_t *)b)[0]; \
((uint32_t *)a)[1] = ((uint32_t *)b)[1]; \
((uint32_t *)a)[2] = ((uint32_t *)b)[2]; \
((uint32_t *)a)[3] = ((uint32_t *)b)[3]; \
} while(0)


/* irc_sockaddr macros */
#define PS_ADDR(x) x->sins.sin6.sin6_addr.s6_addr  	/* s6_addr for pointer */
#define S_ADDR(x) x.sins.sin6.sin6_addr.s6_addr 	/* s6_addr for non pointer */
#define S_PORT(x) x.sins.sin6.sin6_port			/* s6_port */
#define S_FAM(x) x.sins.sin6.sin6_family		/* sin6_family */
#define SOCKADDR(x) x.sins.sin6				/* struct sockaddr_in6 for nonpointer */
#define PSOCKADDR(x) x->sins.sin6			/* struct sockaddr_in6 for pointer */


/* irc_inaddr macros */
#define IN_ADDR(x) x.sins.sin6.s6_addr
#define IPV4_MAPPED(x) ((uint32_t *)x.sins.sin6.s6_addr)[3]
#define PIN_ADDR(x) x->sins.sin6.s6_addr /* For Pointers */
#define IN_ADDR2(x) x.sins.sin6

#define DEF_FAM AF_INET6

#else
#define copy_s_addr(a, b) a = b


#define PS_ADDR(x)	x->sins.sin.sin_addr.s_addr	/* s_addr for pointer */
#define S_ADDR(x)	x.sins.sin.sin_addr.s_addr	/* s_addr for nonpointer */
#define S_PORT(x)	x.sins.sin.sin_port		/* sin_port   */
#define S_FAM(x)	x.sins.sin.sin_family		/* sin_family */
#define SOCKADDR(x)	x.sins.sin			/* struct sockaddr_in */
#define PSOCKADDR(x)	x->sins.sin			/* struct sockaddr_in */


#define PIN_ADDR(x) x->sins.sin.s_addr 
#define IN_ADDR(x) x.sins.sin.s_addr

#ifndef AF_INET6
#define AF_INET6 10 /* Dummy AF_INET6 declaration */
#endif 
#define DEF_FAM AF_INET

#endif

#ifndef INADDR_NONE
#define INADDR_NONE (0xFFFFFFFF)
#endif

#endif
