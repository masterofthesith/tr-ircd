/************************************************************************
 *   IRC - Internet Relay Chat, include/httpd.h
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
 *
 * $Id: httpd.h,v 1.3 2003/06/14 13:55:50 tr-ircd Exp $
 *
 */

#ifndef HTTPD_H
#define HTTPD_H 1

extern int httpd_shutdown, httpd_exited;

extern pthread_mutex_t httpd_mutex, httpd_exit_mutex;
extern pthread_mutexattr_t httpd_mutex_attributes;
extern pthread_cond_t httpd_cond, httpd_exit_cond;
extern pthread_condattr_t httpd_cond_attributes;
extern pthread_t httpd_pthread;
extern pthread_attr_t httpd_pthread_attributes;

extern void initialize_httpd(void);
extern void deinitialize_httpd(void);

extern void *httpd_main(void *);

#endif
