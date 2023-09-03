/************************************************************************
 *   IRC - Internet Relay Chat, src/httpd/httpd_reader.c
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
 * $Id: httpd_reader.c,v 1.4 2004/02/24 15:00:30 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "dh.h"
#include "h.h"
#include "listener.h"
#include "logtype.h"
#include "s_conf.h"
#include "httpd.h"
#include "http.h"

char httpBuf[HTTPREADBUFSIZE];
char hostHeaderBuf[HTTPREADBUFSIZE];


/* INDENT-OFF */

struct hmethod httpd_methods[MAX_METHODS] = {
    {"GET", 1, http_get},
    {"POST", 1, http_post},
    {"HEAD", 1, http_mna},
    {"CONNECT", 0, http_mna},
    {"OPTIONS", 0, http_mna},
    {"PUT", 0, http_mna},
    {"DELETE", 0, http_mna},
    {"TRACE", 0, http_mna},
    {NULL, 0, 0},
};

/* INDENT-ON */

/* More methods could follow */

static inline void finalize_http_client(int fd, aClient *cptr)
{
    dlink_node *m;

    SetHttpClient(cptr); 
    cptr->httpflags |= HFLAGS_EMPTY_HOST;
    cptr->httpflags |= HFLAGS_COOKIE_NONE;
    cptr->flags &= ~FLAGS_DEADSOCKET;
    cptr->flags &= ~FLAGS_CLOSING;
    cptr->flags &= ~FLAGS_EXITED;

    if (!dlinkFind(&hclient_list, cptr)) {
    	m = make_dlink_node();
    	dlinkAdd(cptr, m, &hclient_list);
    }

    read_http_packet(fd, cptr);
}

int read_http_packet_hook(struct hook_data *thisdata)
{
    aClient *cptr = thisdata->client_p;
    int fd = thisdata->check;
    char *address;
    dlink_node *ptr;

    ptr = dlinkFind(&unknown_list, cptr);

    if (ptr != NULL) {
        Count.unknown--;               
        dlinkDelete(ptr, &unknown_list);
        free_dlink_node(ptr);
    }

    if (HttpdConf.policy == HTTP_DENY) {
	if (!HttpdConf.http_except_any) {	/* Clients to allow */
	    for (ptr = http_except_list.head; ptr; ptr = ptr->next) {
		address = ptr->data;
		if (irc_strcmp(cptr->hostip, address) == 0) {
		    finalize_http_client(fd, cptr);
		    return 1;
		}
	    }
	    exit_httpd_client(cptr);
	    return 1;
	}
    } else if (HttpdConf.policy == HTTP_ALLOW) {
	if (!HttpdConf.http_except_any) {	/* Clients to deny */
	    for (ptr = http_except_list.head; ptr; ptr = ptr->next) {
		address = ptr->data;
		if (irc_strcmp(cptr->hostip, address) == 0) {
		    exit_httpd_client(cptr);
		    return 1;
		}
	    }
	} 
    } 
    finalize_http_client(fd, cptr);;
    return 1;
}

void read_http_packet(int fd, void *data)
{
    aClient *client_p = data;
    int length = 0;
    int fd_r = client_p->fd;

    if (IsDead(client_p)) {
	exit_httpd_client(client_p);
	return;
    }

    memset(httpBuf, 0, HTTPREADBUFSIZE);

#ifdef HAVE_ENCRYPTION_ON
    if (IsSSL(client_p))
	length = safe_SSL_read(client_p, httpBuf, HTTPREADBUFSIZE);
    else
#endif
	length = read(fd_r, httpBuf, HTTPREADBUFSIZE);

    if (length <= 0) {
	if ((length == -1) && ignoreErrno(errno)) {
	    comm_setselect(fd_r, FDLIST_HTTPD, COMM_SELECT_READ, read_http_packet, client_p, 0);
	    return;
	}
	exit_httpd_client(client_p);
	return;
    }

    client_p->lasttime = time(NULL);

    http_dopacket(client_p, httpBuf, length);

    /* server fd may have changed */
    fd_r = client_p->fd;

    if (!IsDead(client_p))
	comm_setselect(fd_r, FDLIST_HTTPD, COMM_SELECT_READ, read_http_packet, client_p, 0);

}

static int check_version_string(char *str)
{
    char *revision;
    char *t;
    if (!str)
	return 0;
    if (irc_strncmp(str, "HTTP/", 5) == 0) {
	revision = str + 5;
	if ((t = strchr(revision, '.')) != NULL) {
	    t = '\0';
	    return atoi(revision);
	} else {
	    return -1;
	}
    }
    return -1;
}

static int lookup_method(char *met)
{
    int i;
    for (i = 0; i < MAX_METHODS; i++) {
	if (httpd_methods[i].parse) {
	    if (irc_strcmp(httpd_methods[i].name, met) == 0)
		return i;
	}
    }
    return -1;
}

void http_dopacket(aClient *cptr, char *buffer, size_t length)
{
    int i = 0;
    int metnum = -1;
    char *method = NULL;
    char *url = NULL;
    char *version = NULL;
    char *p = NULL;

    /* We only try to extract the first line of the http input we receive */

    char line[HTTPBUFSIZE];

    if (!(cptr->httpflags & HFLAGS_PARSE_WAIT)) {

	memset(line, 0, (sizeof(char) * HTTPBUFSIZE));

	while (((buffer[i] != '\r') && (buffer[i] != '\n')) && (i < length)) {
	    line[i] = buffer[i];
	    i++;
	}
	line[i] = '\0';
	method = strtoken(&p, line, " ");	/* GET|HEAD|POST */
	
	metnum = lookup_method(method);

	if (metnum < 0) {
	    send_http_header(cptr, NULL, HTTP_NOT_IMPLEMENTED);
	    exit_httpd_client(cptr);
	    return;
	}

	if((url = strtoken(&p, NULL, " ")) != NULL)	/* blabla/blups.htm */		
	    logevent_call(LogSys.httpd_request, cptr->hostip, method, url);
	
	if (!url) {
	    send_http_header(cptr, NULL, HTTP_BAD_REQUEST);
	    exit_httpd_client(cptr);
	    return;
	} else if (strstr(url, "http://")) {
	    url += 7;
	    if (cptr->listener->host_header) {
		if (irc_strncmp(url, cptr->listener->host_header, 
		    strlen(cptr->listener->host_header) - 1) == 0) {
		    url = strchr(url, '/');
		} else {
            	    send_http_header(cptr, NULL, HTTP_BAD_REQUEST);
		    exit_httpd_client(cptr);
		    return;
		}
	    } else {
		url = strchr(url, '/');
	    }
	}	    

	version = strtoken(&p, NULL, " ");	/* HTTP/1.1 */

	if (check_version_string(version) != 1) {
	    send_http_header(cptr, NULL, HTTP_VERSION_NOT_SUPP);
	    exit_httpd_client(cptr);
	    return;
	}

	strlcpy_irc(cptr->hurl, url, MAXPATHLEN);

	parse_client_header(cptr, buffer + i, url, length - i, httpd_methods[metnum].func);

    } else {

	parse_client_header(cptr, buffer, cptr->hurl, length, cptr->hparsefunc);

    }
}
