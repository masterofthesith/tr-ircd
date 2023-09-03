/************************************************************************
 *   IRC - Internet Relay Chat, src/httpd/httpd_send.c
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
 * $Id: httpd_send.c,v 1.3 2003/06/14 13:55:53 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "logtype.h"
#include "s_conf.h"
#include "httpd.h"
#include "http.h"
#include "listener.h"
#include "dh.h"

char sendbuf[HTTPBUFSIZE];

static int dead_link(aClient *to)
{
    to->flags |= FLAGS_DEADSOCKET;
    linebuf_donebuf(&to->recvQ);
    linebuf_donebuf(&to->sendQ);
    return -1;
}

static int http_send_linebuf(struct Client *to, buf_head_t *linebuf)
{
    if (to->fd < 0)
	return 0;		/* Thou shalt not write to closed descriptors */

    if (IsDead(to))
	return 0;

    if (linebuf_len(&to->sendQ) > to->sendqlen) {
	to->flags |= FLAGS_SENDQEX;
	return dead_link(to);
    }

    linebuf_attach(&to->sendQ, linebuf);
    send_http_queued(to->fd, to);
    return 0;
}

/*
 * * send message to single client
 */
void sendto_http(aClient *to, char *pattern, ...)
{
    va_list vl;
    int len;			/* used for the length of the current message */

    va_start(vl, pattern);

    memset(sendbuf, 0, HTTPBUFSIZE);
    len = ircvsprintf(sendbuf, pattern, vl);

    send_http_message(to, sendbuf, len);
    va_end(vl);
}

void vsendto_http(aClient *to, char *pattern, va_list vl)
{
    int len;			/* used for the length of the current message */

    memset(sendbuf, 0, HTTPBUFSIZE);
    len = ircvsprintf(sendbuf, pattern, vl);

    send_http_message(to, sendbuf, len);
}

/*
 * * send_message *   Internal utility which delivers one message
 * buffer to the *      socket. Takes care of the error handling and
 * buffering, if *      needed.
 */
int send_http_message(aClient *to, char *msg, int len)
{
    buf_head_t linebuf;

    while ((msg[len] == '\0'))
	len--;

    if (len > 1023)
	len = 1023;
    msg[++len] = '\0';

    linebuf_newbuf(&linebuf);
    linebuf_put(&linebuf, msg, len);
    http_send_linebuf(to, &linebuf);
    linebuf_donebuf(&linebuf);

    return 0;
}

void send_http_queued(int fd, void *data)
{
    aClient *to = data;
    int rlen;

    if (linebuf_len(&to->sendQ)) {
	while ((rlen = linebuf_flush(to)) > 0);

	if ((rlen < 0) && (ignoreErrno(errno))) {
	    /* we have a non-fatal error, so just continue */
	} else if (rlen <= 0) {
	    /* We have a fatal error */
	    dead_link(to);
	    return;
	}
    }

    if (linebuf_len(&to->sendQ))
	comm_setselect(fd, FDLIST_HTTPD, COMM_SELECT_WRITE, send_http_queued, to, 0);

}

void send_http_status(aClient *cptr, int s)
{
    sendto_http(cptr, "HTTP/1.1 %d %s\r\n", s, herrortab[s]);
}

static inline void send_http_header_ok(aClient *cptr, char *document)
{
    send_http_status(cptr, HTTP_OK);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_age(cptr, NULL, NULL);
    http_cache_control(cptr, NULL, NULL);
    http_expires(cptr, NULL, NULL);
    http_last_modified(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_content_language(cptr, NULL, NULL);
    http_content_location(cptr, document, NULL);
    http_content_type(cptr, NULL, NULL);
    http_location(cptr, document, NULL);
}

static inline void send_http_header_created(aClient *cptr, char *document)
{
    send_http_status(cptr, HTTP_CREATED);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_age(cptr, NULL, NULL);
    http_cache_control(cptr, NULL, NULL);
    http_expires(cptr, NULL, NULL);
    http_last_modified(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_content_language(cptr, NULL, NULL);
    http_content_location(cptr, document, NULL);
    http_content_type(cptr, NULL, NULL);
    http_location(cptr, document, NULL);
}

static inline void send_http_header_mna(aClient *cptr)
{
    char hBuf[HTTPBUFSIZE];
    int len = ircsnprintf(hBuf, HTTPBUFSIZE,
                "<html><head>Method not available</head><body>Server cannot fulfill your request</body></html>\r\n\r\n");
    send_http_status(cptr, HTTP_METHOD_NOT_ALLOW);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_allow(cptr, NULL, NULL);
    http_content_type(cptr, NULL, NULL);
    sendto_http(cptr, "Content-Length: %d", len);
    sendto_http(cptr, "\r\n\r\n%s", hBuf);
}

static inline void send_http_header_sun(aClient *cptr)
{
    char hBuf[HTTPBUFSIZE];
    int len = ircsnprintf(hBuf, HTTPBUFSIZE, 
		"<html><head>Service unavailable</head><body>Server cannot fulfill your request</body></html>\r\n\r\n");
    send_http_status(cptr, HTTP_SERVICE_UNAVAIL);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_retry_after(cptr, "86400", NULL);
    http_content_type(cptr, NULL, NULL);
    sendto_http(cptr, "Content-Length: %d", len);
    sendto_http(cptr, "\r\n\r\n%s", hBuf);
}

static inline void send_http_header_nf(aClient *cptr, char *document)
{
    char hBuf[HTTPBUFSIZE];
    int len = ircsnprintf(hBuf, HTTPBUFSIZE, 
		"<html><head>Page %s not found</head><body>Page does not exist anymore</body></html>\r\n\r\n", document);
    send_http_status(cptr, HTTP_NOT_FOUND);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_content_type(cptr, NULL, NULL);
    sendto_http(cptr, "Content-Length: %d", len);
    sendto_http(cptr, "\r\n\r\n%s", hBuf);
}

static inline void send_http_header_f(aClient *cptr)
{
    char hBuf[HTTPBUFSIZE];
    int len = ircsnprintf(hBuf, HTTPBUFSIZE,
		"<html><head>Permission denied</head><body><br><a href=\"login.htm\">Back to the login page</a></body></html>\r\n\r\n");
    send_http_status(cptr, HTTP_FORBIDDEN);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_content_type(cptr, NULL, NULL);
    sendto_http(cptr, "Content-Length: %d", len);
    sendto_http(cptr, "\r\n\r\n%s", hBuf);
}

static inline void send_http_header_ef(aClient *cptr)
{
    char hBuf[HTTPBUFSIZE];
    int len = ircsnprintf(hBuf, HTTPBUFSIZE,
                "<html><head>Expectation failed</head><body>Server cannot fulfill your request</body></html>\r\n\r\n");
    send_http_status(cptr, HTTP_EXPECTATION_FAILED);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_content_type(cptr, NULL, NULL);
    sendto_http(cptr, "Content-Length: %d", len);
    sendto_http(cptr, "\r\n\r\n%s", hBuf);
}

static inline void send_http_header_vns(aClient *cptr)
{
    char hBuf[HTTPBUFSIZE];
    int len = ircsnprintf(hBuf, HTTPBUFSIZE,
                "<html><head>Version not supported</head><body>Server cannot fulfill your request</body></html>\r\n\r\n");
    send_http_status(cptr, HTTP_VERSION_NOT_SUPP);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_content_type(cptr, NULL, NULL);
    sendto_http(cptr, "Content-Length: %d", len);
    sendto_http(cptr, "\r\n\r\n%s", hBuf);
}

static inline void send_http_header_lr(aClient *cptr, char *document)
{
    char hBuf[HTTPBUFSIZE];
    int len = ircsnprintf(hBuf, HTTPBUFSIZE,
                "<html><head>Length required</head><body>Unable to understand your request for %s. Length required.</body></html>\r\n\r\n");
    send_http_status(cptr, HTTP_LENGTH_REQUIRED);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_content_type(cptr, NULL, NULL);
    sendto_http(cptr, "Content-Length: %d", len);
    sendto_http(cptr, "\r\n\r\n%s", hBuf);
}

static inline void send_http_header_ni(aClient *cptr)
{
    char hBuf[HTTPBUFSIZE];
    int len = ircsnprintf(hBuf, HTTPBUFSIZE,
                "<html><head>Method not implemented</head><body>Server cannot fulfill your request</body></html>\r\n\r\n");
    send_http_status(cptr, HTTP_NOT_IMPLEMENTED);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_content_type(cptr, NULL, NULL);
    sendto_http(cptr, "Content-Length: %d", len);
    sendto_http(cptr, "\r\n\r\n%s", hBuf);
}

static inline void send_http_header_nc(aClient *cptr)
{
    send_http_status(cptr, HTTP_NO_CONTENT);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
}

static inline void send_http_header_br(aClient *cptr)
{
    char hBuf[HTTPBUFSIZE];
    int len = ircsnprintf(hBuf, HTTPBUFSIZE,
                "<html><head>Bad Request</head><body>Bad request</body></html>\r\n\r\n");
    send_http_status(cptr, HTTP_BAD_REQUEST);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_content_type(cptr, NULL, NULL);
    sendto_http(cptr, "Content-Length: %d", len); 
    sendto_http(cptr, "\r\n\r\n%s", hBuf);
}

static inline void send_http_header_ie(aClient *cptr)
{
    char hBuf[HTTPBUFSIZE];
    int len = ircsnprintf(hBuf, HTTPBUFSIZE,
                "<html><head>Internal Error</head><body>Unable to process request</body></html>\r\n\r\n");
    send_http_status(cptr, HTTP_INTERNAL_ERROR);
    http_date(cptr, NULL, NULL);
    http_server(cptr, NULL, NULL);
    http_connection(cptr, NULL, NULL);
    http_content_type(cptr, NULL, NULL);
    sendto_http(cptr, "Content-Length: %d", len);
    sendto_http(cptr, "\r\n\r\n%s", hBuf);
}


void send_http_header(aClient *cptr, char *document, int status)
{
    switch (status) {
	case HTTP_OK:
	    send_http_header_ok(cptr, document);
    	    sendto_http(cptr, "\r\n\r\n");
	    break;
	case HTTP_CREATED:
	    send_http_header_created(cptr, document);
    	    sendto_http(cptr, "\r\n\r\n");
	    break;
	case HTTP_METHOD_NOT_ALLOW:
	    send_http_header_mna(cptr);
	    break;
	case HTTP_SERVICE_UNAVAIL:
	    send_http_header_sun(cptr);
	    break;
	case HTTP_NOT_FOUND:
	    send_http_header_nf(cptr, document);
	    break;
	case HTTP_FORBIDDEN:
	    send_http_header_f(cptr);
	    break;
	case HTTP_EXPECTATION_FAILED:
	    send_http_header_ef(cptr);
	    break;
	case HTTP_NO_CONTENT:
	    send_http_header_nc(cptr);
	    break;
	case HTTP_VERSION_NOT_SUPP:
	    send_http_header_vns(cptr);
	    break;
	case HTTP_LENGTH_REQUIRED:
	    send_http_header_lr(cptr, document);
	    break;
	case HTTP_NOT_IMPLEMENTED:
	    send_http_header_ni(cptr);
	    break;
	case HTTP_BAD_REQUEST:
	    send_http_header_br(cptr);
	    break;
	case HTTP_INTERNAL_ERROR:
	    send_http_header_ie(cptr);
	    break;
	default:
	    break;
    }
    return;
}

void send_http_cookie(aClient *cptr, char *document, int flags)
{
    char hBuf[256];
    int len = 0;
    char uus[64];

    if (flags != HTTP_CREATED)
	return;

    if (cptr->httpflags & HFLAGS_COOKIE_NONE)
	uuid_generate(cptr->uuid);

    uuid_unparse(cptr->uuid, uus);

    len = ircsnprintf(hBuf, 255, 
		 "ID=\"%s\"; Version=\"1\"", uus);

    send_http_header_created(cptr, document);
    http_set_cookie(cptr, hBuf, NULL);
    sendto_http(cptr, "\r\n\r\n");
}
