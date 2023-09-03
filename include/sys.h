/************************************************************************
 *   IRC - Internet Relay Chat, include/sys.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 University of Oulu, Computing Center
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
 */

/*
 * $Id: sys.h,v 1.9 2005/06/19 09:52:46 tr-ircd Exp $ 
 */

#ifndef	SYS_H
#define SYS_H	1

#include "setup.h"

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <pthread.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifdef USE_DEVPOLL
#include <sys/devpoll.h>
#endif

#ifdef USE_KQUEUE
#include <sys/event.h>
#endif

#if defined(USE_POLL) || defined(USE_SIGIO)
#include <sys/poll.h>
#endif

#ifdef USE_EPOLL
#include <sys/epoll.h>
#endif

#ifdef HAVE_SYS_ERRNO_H
#include <sys/errno.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STDLIB_H 
#include <stdlib.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef HAVE_SYS_SIGNAL_H
#include <sys/signal.h>
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#else
#ifndef HAVE_UINTPTR_T
typedef unsigned long uintptr_t;
#endif
#endif

#if defined(HAVE_SHL_LOAD)   
#include <dl.h>
#endif

#include <dlfcn.h>

#ifdef HAVE_MACH_O_DYLD_H
#include <mach-o/dyld.h>
#endif

#ifdef PROXYMON_MODULE
#include <opm.h>
#include <opm_error.h>
#include <opm_types.h>
#endif

#ifdef HAVE_MMAN
#include <sys/mman.h>
#endif

#ifdef HAVE_ENCRYPTION_ON
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/rsa.h> 
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#endif

#include "../lib/zlib/zlib.h"

#endif
