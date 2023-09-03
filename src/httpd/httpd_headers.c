/************************************************************************
 *   IRC - Internet Relay Chat, src/httpd/httpd_headers.c
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
 * $Id: httpd_headers.c,v 1.5 2004/02/24 15:00:30 tr-ircd Exp $
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

/* INDENT-OFF */

struct hmethod header_messages[50] = {
    {"Accept", 0, http_none},
    {"Accept-Charset", 0, http_none},
    {"Accept-Encoding", 1, http_accept_encoding},
    {"Accept-Language", 0, http_none},
    {"Accept-Ranges", 0, http_none},
    {"Age", 2, http_age},
    {"Allow", 2, http_allow},
    {"Authorization", 0, http_none},
    {"Cache-Control", 2, http_cache_control},
    {"Connection", 2, http_connection},
    {"Content-Encoding", 0, http_none},
    {"Content-Language", 2, http_content_language},
    {"Content-Length", 1, http_content_length},
    {"Content-Location", 2, http_content_location},
    {"Content-MD5", 0, http_none},
    {"Content-Range", 0, http_none},
    {"Content-Type", 2, http_content_type},
    {"Cookie", 1, http_cookie},
    {"Date", 2, http_date},
    {"ETag", 0, http_none},
    {"Expect", 1, http_expect},
    {"Expires", 2, http_expires},
    {"From", 0, http_none},
    {"Host", 1, http_host_header},
    {"If-Match", 0, http_none},
    {"If-Modified-Since", 0, http_none},
    {"If-None-Match", 0, http_none},
    {"If-Range", 0, http_none},
    {"If-Unmodified-Since", 0, http_none},
    {"Last-Modified", 2, http_last_modified},
    {"Location", 2, http_location},
    {"Max-Forwards", 0, http_none},
    {"Pragma", 0, http_none},
    {"Proxy-Authenticate", 0, http_none},
    {"Proxy-Authorization", 0, http_none},
    {"Range", 0, http_none},
    {"Referer", 0, http_none},
    {"Retry-After", 2, http_retry_after},
    {"Server", 2, http_server},
    {"Set-Cookie", 2, http_set_cookie},
    {"TE", 0, http_none},
    {"Trailer", 0, http_none},
    {"Transfer-Encoding", 0, http_none},
    {"Upgrade", 0, http_none},
    {"User-Agent", 0, http_none},
    {"Vary", 0, http_none},
    {"Via", 0, http_none},
    {"Warning", 0, http_none},
    {"WWW-Authenticate", 0, http_none},
    {NULL, 0},
};

/* INDENT-ON */

/* The following two functions were taken and modified from uncgi package
 * -TimeMr14C
 * Available at: <http://www.midwinter.com/~koreth/uncgi.html
 */

/*
 * @(#)uncgi.c  1.36 07/18/01
 *
 * Unescape all the fields in a form and stick them in the environment
 * so they can be used without awful machinations.
 *
 * Call with an ACTION such as:
 *      http://foo.bar.com/cgi-bin/uncgi/myscript/extra/path/stuff
 *
 * Uncgi will run "myscript" from the SCRIPT_BIN directory (configured at
 * compile time) and set PATH_INFO to "/extra/path/stuff".
 *
 * Environment variable names are "WWW_" plus the field name.
 *
 * Copyright 1994, Steven Grimm <koreth@midwinter.com>.
 *
 * Permission is granted to redistribute freely and use for any purpose,
 * commercial or private, so long as this copyright notice is retained
 * and the source code is included free of charge with any binary
 * distributions.
 */

inline int hex_to_int(char c)
{
    int retval;

    if (IsUpper(c))
	c = ToLower(c);
    retval = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10);

    return retval;
}

inline char *url_decode(char *inchr, int il, char *outchr)
{
    int rchar = 0;
    int len = 0;
    int ol = 0;

    while (len <= il) {
	if (inchr[len] == '+') {
	    outchr[ol++] = ' ';
	} else if (inchr[len] == '%') {
	    rchar = 0;
	    len++;
	    if (IsXDigit(inchr[len])) {
		rchar += (hex_to_int(inchr[len]) * 16);
		len++;
		if (IsXDigit(inchr[len])) {
		    rchar += hex_to_int(inchr[len]);
		    outchr[ol++] = rchar;
		}
	    }
	} else {
	    outchr[ol++] = inchr[len];
	}
	len++;
    }
    return outchr;
}

/**********************************************************************************/

void parse_client_header(aClient *cptr, char *buffer, char *url, int length,
			 int (*func) (aClient *sptr, char *u, char *data))
{
    int llen = length;
    char line[HTTPBUFSIZE];
    char obuf[HTTPBUFSIZE];
    char *c;
    int i = 0, j = 0, l = 0;

    memset(line, 0, (sizeof(char) * HTTPBUFSIZE));
    memset(obuf, 0, (sizeof(char) * HTTPBUFSIZE));

    while (length > 0) {
	if ((buffer[i] != '\r') && (buffer[i] != '\n') && (buffer[i] != '\0') && (i < llen)) {
	    line[l] = buffer[i];
	    l++;
	    i++;
	} else if (line[0]) {
	    line[l] = '\0';
	    c = strchr(line, ':');
	    if (!c) {
		length -= l;
		func(cptr, url, url_decode(line, l, obuf));
		return;
	    }
	    c[0] = '\0';
	    c++;
	    if (c[0] == ' ') {
		c[0] = '\0';
		c++;
	    }
	    for (j = 0; j < 47; j++) {
		if (header_messages[j].parse == 1) {
		    if (irc_strcmp(line, header_messages[j].name) == 0) {
			header_messages[j].func(cptr, c, NULL);
			j = 47;
		    }
		}
	    }
	    length -= l;
	    memset(line, 0, (sizeof(char) * HTTPBUFSIZE));
	    l = 0;
	    line[0] = '\0';
	} else {
	    i++;
	    length--;
	}
    }
    if (cptr->flags & HFLAGS_EMPTY_HOST) {
	send_http_header(cptr, NULL, HTTP_BAD_REQUEST);
	exit_httpd_client(cptr);
    }
    func(cptr, url, NULL);
}

int http_none(aClient *cptr, char *url, char *s)
{
    return 0;
}

int http_accept_encoding(aClient *cptr, char *string, char *s)
{
    if (match("*gzip*", string) == 0)
	cptr->httpflags |= HFLAGS_GZIP;
    return 0;
}

int http_age(aClient *cptr, char *url, char *s)
{
    sendto_http(cptr, "Age: %lu\r\n", HTTP_PAGE_AGE);
    return 0;
}

int http_allow(aClient *cptr, char *url, char *s)
{
    sendto_http(cptr, "Allow: GET, POST\r\n");
    return 0;
}

int http_cache_control(aClient *cptr, char *url, char *s)
{
    sendto_http(cptr, "Cache-Control: must-revalidate\r\n");
    return 0;
}

int http_connection(aClient *cptr, char *string, char *s)
{
    sendto_http(cptr, "Connection: close\r\n");
    return 0;
}

int http_content_language(aClient *cptr, char *string, char *s)
{
    sendto_http(cptr, "Content-Language: en\r\n");
    return 0;
}

int http_content_length(aClient *cptr, char *string, char *s)
{
    long l = strtol(string, NULL, 10);
    if (l > 0)
	cptr->len_to_parse = l;
    else
	cptr->len_to_parse = -1;
    return 0;
}

int http_content_location(aClient *cptr, char *string, char *s)
{
    sendto_http(cptr, "Content-Location: %s\r\n", string);
    return 0;
}

int http_content_type(aClient *cptr, char *string, char *s)
{
    sendto_http(cptr, "Content-Type: text/html\r\n");
    return 0;
}

int http_cookie(aClient *cptr, char *string, char *s)
{
    char *id;
    char u[37];
    uuid_t uuid;

    /* Cookie: $Version:"1"; ID="uuid"; $Domain=".irc.time.net" */
    /* string: $Version:"1"; ID="uuid"; $Domain=".irc.time.net" */

    id = strstr(string, "ID=");
    if (id == NULL) {
	cptr->httpflags |= HFLAGS_COOKIE_NONE;
	cptr->httpflags &= ~HFLAGS_COOKIE_READ;
	return 1;
    }

    memcpy(u, id + 4, 36);
    u[37] = '\0';
    
    uuid_parse(u, uuid);
    
    if (uuid_compare(uuid, cptr->uuid) == 0) {
	cptr->httpflags |= HFLAGS_AUTHENTICATED;
    	cptr->httpflags |= HFLAGS_COOKIE_READ;
    	cptr->httpflags &= ~HFLAGS_COOKIE_NONE;
    }

    return 0;
}


int http_date(aClient *cptr, char *string, char *s)
{
    sendto_http(cptr, "Date: %s\r\n", httpdate(time(NULL)));
    return 0;
}

int http_expect(aClient *cptr, char *string, char *s)
{
    cptr->httpflags |= HFLAGS_EXPECTATION_FAILED;
    return 0;
}

int http_expires(aClient *cptr, char *string, char *s)
{
    sendto_http(cptr, "Expires: %s\r\n", httpdate(time(NULL) + HTTP_PAGE_AGE));
    return 0;
}

int http_host_header(aClient *cptr, char *hostbuf, char *other)
{
    if (cptr->listener->host_header == NULL) {
	cptr->flags &= ~HFLAGS_EMPTY_HOST;
	return 0;
    }

    if (irc_strncmp(cptr->listener->host_header, hostbuf, 
	strlen(cptr->listener->host_header) - 1) == 0)
	cptr->flags &= ~HFLAGS_EMPTY_HOST;

    return 0;
}

int http_last_modified(aClient *cptr, char *string, char *s)
{
    sendto_http(cptr, "Last-Modified: %s\r\n", httpdate(time(NULL) - 1));
    return 0;
}

int http_location(aClient *cptr, char *string, char *s)
{
    sendto_http(cptr, "Location: %s\r\n", string);
    return 0;
}

int http_retry_after(aClient *cptr, char *string, char *s)
{
    sendto_http(cptr, "Retry-After: %s\r\n", string);
    return 0;
}

int http_server(aClient *cptr, char *string, char *s)
{
    sendto_http(cptr, "Server: TR-IRCD/httpd-%s (httpd.so)\r\n", version);
    return 0;
}

int http_set_cookie(aClient *cptr, char *string, char *s)
{
    sendto_http(cptr, "Set-Cookie: %s\r\n", string);
    cptr->httpflags &= ~HFLAGS_COOKIE_NONE;
    return 0;
}

int http_mna(aClient *cptr, char *url, char *s)
{
    send_http_header(cptr, url, HTTP_METHOD_NOT_ALLOW);
    return 0;
}
