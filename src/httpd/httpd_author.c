/************************************************************************
 *   IRC - Internet Relay Chat, src/httpd/httpd_author.c
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
 */

/*
 * $Id: httpd_author.c,v 1.3 2003/06/14 13:55:53 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "s_conf.h"
#include "listener.h"
#include "h.h"
#include "httpd.h"
#include "http.h"

/*
 * The following element is a typical html content 
 * <XX yy=zz tt=qq>HH</XX> 
 * <a href="http://www.tr-ircd.net">TR-IRCD Homepage</a>
 */

/* FIXME: Buffer overflows */

static inline int vcreate_html_object(char *buf, size_t s, char *ot, int count, 
				      char *pattern, va_list vldef)
{
    char *b = ot;
    int t = 0;

    if (t < s)
    	buf[t++] = '<';

    while(*b && (t < s))
	buf[t++] = *b++;

    if (pattern && (t < s)) {
	buf[t++] = ' ';
	t += ircvsnprintf(buf + t, s - t - 1, pattern, vldef);
    } 

    if (t < s)
    	buf[t++] = '>';
    while (count && (t < s)) {
    	buf[t++] = '%';	/* This is an ugly hack */
    	buf[t++] = 's';
	if (--count)
	    buf[t++] = ' ';
    }
    if (t < s)
    	buf[t++] = '<';
    if (t < s)
    	buf[t++] = '/';

    b = ot;
    while(*b && (t < s))
        buf[t++] = *b++;

    if (t < s)
    	buf[t++] = '>';

    if (t < s)
    	buf[t] = '\0';

    return t;
}

char *enervate_html_object(char *buf, size_t size, char *topic, int count, char *pattern, ...)
{
    va_list vl;

    va_start(vl, pattern);
    vcreate_html_object(buf, size, topic, count, pattern, vl);
    va_end(vl);
    return buf;
}

static inline int vcreate_html_object_begin(char *buf, size_t s, char *ot, int count,
                                            char *pattern, va_list vldef)
{
    char *b = ot;
    int t = 0;

    if (t < s)
        buf[t++] = '<';

    while(*b && (t < s))
        buf[t++] = *b++;

    if (pattern && (t < s)) {
        buf[t++] = ' ';
        t += ircvsnprintf(buf + t, s - t - 1, pattern, vldef);
    } 

    if (t < s)
        buf[t++] = '>';
    while (count && (t < s)) {
        buf[t++] = '%'; /* This is an ugly hack */
        buf[t++] = 's';
        if (--count)
            buf[t++] = ' ';
    }

    if (t < s)
        buf[t++] = '\n';

    if (t < s)
        buf[t] = '\0';

    return t;
}

char *enervate_html_object_begin(char *buf, size_t size, char *topic, int count, char *pattern, ...)
{
    va_list vl;

    va_start(vl, pattern);
    vcreate_html_object_begin(buf, size, topic, count, pattern, vl);
    va_end(vl);
    return buf;
}

