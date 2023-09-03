/************************************************************************
 *   IRC - Internet Relay Chat, src/httpd/httpd_post.c
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
 * $Id: httpd_post.c,v 1.12 2004/07/12 09:14:59 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "dh.h"
#include "h.h"
#include "msg.h"
#include "event.h"
#include "language.h"
#include "listener.h"
#include "logtype.h"
#include "s_conf.h"
#include "httpd.h"
#include "http.h"

char httpBuf[HTTPBUFSIZE];
extern void sigterm_handler(int sig);

static inline void http_send_tr(aClient *cptr, int end)
{
    sendto_http(cptr, end ? "</tr>\r\n" : "<tr>\r\n");
}

static inline void http_send_p(aClient *cptr, int end)
{
    sendto_http(cptr, end ? "</p>\r\n" : "<p>\r\n");
}

static inline void http_send_td(aClient *cptr, int end, int width)
{
    sendto_http(cptr, end ? "</td>\r\n" : "<td width=\"%d%%\">", width);
}

static void restart_delayed(void *buffer)
{
    restart((char *) buffer);
}

static void die_delayed(void *unused)
{
    sigterm_handler(0);
}

int http_post(aClient *cptr, char *url, char *data)
{
    char *hurl = url;
    char hBuf[HTTPBUFSIZE];
    char decoded_data[HTTPBUFSIZE];
    char strBuf1[200];
    char strBuf2[200];

    if (cptr->httpflags & HFLAGS_EXPECTATION_FAILED) {
	send_http_header(cptr, hurl, HTTP_EXPECTATION_FAILED);
	exit_httpd_client(cptr);
	return 0;
    }

    if (data == NULL || data[0] == '\0') {
	cptr->httpflags |= HFLAGS_PARSE_WAIT;
	cptr->hparsefunc = http_post;
	return 0;
    }

    cptr->httpflags &= ~HFLAGS_PARSE_WAIT;

    if (!cptr->len_to_parse) {
	send_http_header(cptr, hurl, HTTP_LENGTH_REQUIRED);
	exit_httpd_client(cptr);
	return 0;
    }

    cptr->len_to_parse = 0;
    cptr->hparsefunc = http_mna;

    url_decode(data, strlen(data), decoded_data);

    if (hurl[0] == '/') {
	if (irc_strcmp(hurl + 1, "logon.hti") == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;
    char *user = NULL;
    char *pass = NULL;
    int login = 0;

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4) == 0) {
		    if (irc_strncmp(value, "Login", 5) == 0)
			login = 1;
		} else if (irc_strncmp(element, "user", 4) == 0) {
		    user = value;
		} else if (irc_strncmp(element, "pass", 4) == 0) {
		    pass = value;
		} else if (irc_strncmp(element, "reset", 5) == 0) {
		    if (irc_strncmp(value, "Reset", 5) == 0)
			login = 2;
		}
	    }

	    if (GeneralOpts.webconfig == 0) {
		if (login == 1) {
    char arr[PASSWDLEN];
    aConfItem *aconf;
		    if (!user || !pass) {
			send_http_header(cptr, hurl, HTTP_FORBIDDEN);
			exit_httpd_client(cptr);
			return 0;
		    }
		    aconf = find_conf_by_name(user, CONF_OPERATOR);
		    if (!aconf) {
			send_http_header(cptr, hurl, HTTP_FORBIDDEN);
			exit_httpd_client(cptr);
			return 0;
		    }
		    if (irc_strcmp(aconf->passwd, calcpass(pass, arr))) {
			send_http_header(cptr, hurl, HTTP_FORBIDDEN);
			exit_httpd_client(cptr);
			return 0;
		    }
		    if (!IsConfWeblogin(aconf)) {
			send_http_header(cptr, hurl, HTTP_FORBIDDEN);
			exit_httpd_client(cptr);
			return 0;
		    }
		    cptr->httpflags |= HFLAGS_AUTHENTICATED;
		    send_http_cookie(cptr, "/select.htm", HTTP_CREATED);
		    http_get(cptr, "/select.htm", "a");
		    return 1;
		} else if (login == 2) {
		    send_http_header(cptr, "/login.htm", HTTP_CREATED);
		    http_get(cptr, "/login.htm", "a");
		    return 1;
		} else {
		    send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		    exit_httpd_client(cptr);
		    return 0;
		}

	    } else {
		if (login == 1) {
		    if (!user || !pass) {
			send_http_header(cptr, hurl, HTTP_FORBIDDEN);
			exit_httpd_client(cptr);
			return 0;
		    }
		    if (irc_strcmp(user, "admin")) {
			send_http_header(cptr, hurl, HTTP_FORBIDDEN);
			exit_httpd_client(cptr);
			return 0;
		    }
		    if (irc_strcmp(pass, GeneralOpts.webconfigpass)) {
			send_http_header(cptr, hurl, HTTP_FORBIDDEN);
			exit_httpd_client(cptr);
			return 0;
		    }
		    cptr->httpflags |= HFLAGS_AUTHENTICATED;
		    send_http_cookie(cptr, "/select.htm", HTTP_CREATED);
		    http_get(cptr, "/select.htm", "a");
		    return 1;
		} else if (login == 2) {
		    send_http_header(cptr, "/login.htm", HTTP_CREATED);
		    http_get(cptr, "/login.htm", "a");
		    return 1;
		} else {
		    send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		    exit_httpd_client(cptr);
		    return 0;
		}
	    }

	}

	else if (irc_strcmp(hurl + 1, "listener.hti") == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;
    char *id = NULL;
    int commit = 0;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "listener.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4) == 0) {
		    if (irc_strncmp(value, "Sho", 3) == 0)
			commit = 1;
		    else if (irc_strncmp(value, "New", 3) == 0)
			commit = 2;
		} else if (irc_strncmp(element, "LIS", 3) == 0) {
		    id = value;
		}
	    }

	    if (commit == 1) {
    struct Listener *li = find_listener_by_uuid(id);
    struct Listener *tmp_li, *next_li, *prev_li;

		send_http_header(cptr, "listener.htm", HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Listener Setup");
		sendto_http(cptr, HTTP_LINEBREAK);

		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
					   "method=\"%s\" action=\"%s?id=%s\" enctype=\"%s\"",
					   H_POST, "listener.htc", id,
					   "application/x-www-form-urlencoded");
		sendto_http(cptr, hBuf);

		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"", "submit",
					   "Commit", "commbutton");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"", "reset",
					   "Revoke", "resetbutton");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"", "submit",
					   "Delete Block", "delete");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"", "submit",
					   "New Block", "new");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
					   "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
					   0, 0, 0, "border-collapse: collapse", "#111111", "100%",
					   "Listener");
		sendto_http(cptr, hBuf);

		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Listener Address");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "la", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		if (li->vhost[0])
		    sendto_http(cptr, HTTP_TABLE_ROW, 24, li->vhost);
		else
		    sendto_http(cptr, HTTP_TABLE_ROW, 24, "ALL-INTERFACES");
		http_send_tr(cptr, 1);

		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Listener Ports (Range)");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "range",
					   32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		next_li = prev_li = NULL;
		for (tmp_li = ListenerPollList; tmp_li; tmp_li = next_li) {
		    next_li = tmp_li->next;
		    if (uuid_compare(tmp_li->uuid, li->uuid) != 0)
			continue;
		    while (next_li && (uuid_compare(next_li->uuid, li->uuid) == 0)) {
			prev_li = next_li;
			next_li = next_li->next;
		    }
		    if (prev_li && (prev_li != li->next) && (prev_li != li))
			ircsnprintf(strBuf2, 200, "%d .. %d", prev_li->port, li->port);
		    else
			ircsnprintf(strBuf2, 200, "%d", li->port);
		    break;
		}
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf2);
		http_send_tr(cptr, 1);

		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Listener Properties");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" %svalue=\"%s\"", "checkbox",
					   "client",
					   ((li->flags & LISTENER_CLIENT) ? "checked " : ""), "1");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Is this port a client port ?\r\n");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24,
			    ((li->flags & LISTENER_CLIENT) ? "yes" : "no"));
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" %svalue=\"%s\"",
					   "checkbox", "server",
					   ((li->flags & LISTENER_SERVER) ? "checked " : ""), "1");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Is this port a server port ?\r\n");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24,
			    ((li->flags & LISTENER_SERVER) ? "yes" : "no"));
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" %svalue=\"%s\"",
					   "checkbox", "ssl",
					   ((li->
					     flags & LISTENER_CLIENT_SSL) ? "checked " : ""), "1");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Is this port a secure client port ? (SSL)\r\n");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24,
			    ((li->flags & LISTENER_CLIENT_SSL) ? "yes" : "no"));
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE,
					   "input", 0,
					   "type=\"%s\" name=\"%s\" %svalue=\"%s\"",
					   "checkbox", "web",
					   ((li->flags & LISTENER_HTTP) ? "checked " : ""), "1");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Is this port a web server port ?\r\n");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, ((li->flags & LISTENER_HTTP) ? "yes" : "no"));
		http_send_tr(cptr, 1);
		sendto_http(cptr, "</table>\r\n</form>\r\n");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "form",
					   0,
					   "method=\"%s\" action=\"%s\" enctype=\"%s\"", H_POST,
					   "listener.hti", "application/x-www-form-urlencoded");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Show", "commbutton");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, HTTP_LINEBREAK);
		sendto_http(cptr, "Listener Blocks");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "select", 0, "size=\"%d\" name=\"%s\"", 1, "LISTEN");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "\r\n");
		prev_li = next_li = NULL;
		for (li = ListenerPollList; li; li = next_li) {
		    next_li = li->next;
		    prev_li = li;
		    memset(strBuf1, 0, sizeof(strBuf1));
		    memset(strBuf2, 0, sizeof(strBuf1));
		    while (next_li && (uuid_compare(li->uuid, next_li->uuid) == 0)) {
			prev_li = next_li;
			next_li = next_li->next;
		    }
		    uuid_unparse(li->uuid, strBuf1);
		    enervate_html_object(hBuf, HTTPBUFSIZE, "option", 1, "value=\"%s\"", strBuf1);
		    if (prev_li && (prev_li != li->next)
			&& (prev_li != li))
			ircsnprintf(strBuf2, 200, "%d .. %d", prev_li->port, li->port);
		    else if (prev_li || li)
			ircsnprintf(strBuf2, 200, "%d", li->port);
		    sendto_http(cptr, hBuf, strBuf2);
		    sendto_http(cptr, "\r\n");
		}
		sendto_http(cptr, "</select>\r\n</form>\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

	    } else if (commit == 2) {
		send_http_header(cptr, "/listener.hta", HTTP_CREATED);
		http_get(cptr, "/listener.hta", "a");
		return 1;
	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }
	}

	else if (irc_strncmp(hurl + 1, "listener.htc", 12) == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;

    char *id = NULL;
    struct Listener *li = NULL, *next_li = NULL;
    uuid_t u;

    int commit = 0;

    char *address = NULL;
    char *range = NULL;
    int flags = LISTENER_CLIENT;
    int listener_ports[256];	/* Must be ok */
    int port_count = 0;
    int i;
    int t = -1;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "listener.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    id = hurl + 17;

	    uuid_parse(id, u);

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4) == 0) {
		    if (irc_strncmp(value, "Com", 3) == 0)
			commit = 1;
		} else if (irc_strncmp(element, "del", 3) == 0) {
		    if (irc_strncmp(value, "Del", 3) == 0)
			commit = 2;
		} else if (irc_strncmp(element, "new", 3) == 0) {
		    if (irc_strncmp(value, "New", 3) == 0)
			commit = 3;
		} else if (irc_strncmp(element, "la", 2) == 0) {
		    address = value;
		} else if (irc_strncmp(element, "ran", 3) == 0) {
		    range = value;
		} else if (irc_strncmp(element, "cli", 3) == 0) {
		    flags |= LISTENER_CLIENT;
		} else if (irc_strncmp(element, "ser", 3) == 0) {
		    flags |= LISTENER_SERVER;
		} else if (irc_strncmp(element, "ssl", 3) == 0) {
		    flags |= LISTENER_CLIENT_SSL;
		} else if (irc_strncmp(element, "web", 3) == 0) {
		    flags |= LISTENER_HTTP;
		    flags &= ~LISTENER_CLIENT;
		    flags &= ~LISTENER_SERVER;
		}
	    }
	    if (commit == 1) {

		if (range[0] != '\0') {
    char t[5] = { 0, 0, 0, 0, 0 };
    int r = 0;
    int s = 0;
		    memset(listener_ports, 0, sizeof(listener_ports));

		    li = find_listener_by_uuid(id);

		    while (range[r] && (port_count < 256)) {
			if (IsDigit(range[r])) {
			    t[s] = range[r];
			    r++;
			    s++;
			} else {
			    if (t[0] != '\0') {
				listener_ports[port_count++] = atoi(t);
				s = 0;
				memset(t, 0, sizeof(t));
			    }
			    if (range[r] == ' ') {
				r++;
			    } else if (range[r] == ',') {
				r++;
			    } else if (range[r] == '.') {
				r++;
				r++;
				listener_ports[port_count++] = -1;
			    }
			}
		    }
		    if (t[0] != '\0')
			listener_ports[port_count] = atoi(t);
		}

		if (address[0] == '\0') {	/* Change existing values */
		    if (port_count == 0) {	/* Only change flags */
			for (li = ListenerPollList; li; li = li->next) {
			    if (uuid_compare(li->uuid, u) == 0) {
				li->flags = flags;
			    }
			}
		    } else {	/* Add new ports */
			for (i = 0; i <= port_count; i++) {
			    if (listener_ports[i] != -1) {
				add_listener(listener_ports[i], li->vhost, flags, u, NULL);
				t = listener_ports[i];
			    } else {
				i++;
				for (; t <= listener_ports[i]; t++)
				    add_listener(t, li->vhost, flags, u, NULL);
			    }
			}
		    }
		} else {	/* Create a new listener block */
		    if (irc_strncmp(address, "ALL", 3) == 0)
			address = NULL;
		    if (address[0] == '*')
			address = NULL;
		    for (li = ListenerPollList; li; li = next_li) {
			next_li = li->next;
			if (uuid_compare(li->uuid, u) == 0)
			    close_listener(li);
		    }
		    for (i = 0; i <= port_count; i++) {
			if (listener_ports[i] != -1) {
			    add_listener(listener_ports[i], address, flags, u, NULL);
			    t = listener_ports[i];
			} else {
			    i++;
			    for (; t <= listener_ports[i]; t++)
				add_listener(t, address, flags, u, NULL);
			}
		    }
		}

		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Listener Setup");
		sendto_http(cptr, HTTP_LINEBREAK);
		if (address[0])
		    sendto_http(cptr, "New Address: %s\r\n<br>\r\n", address);
		if (range[0])
		    sendto_http(cptr, "New Range: %s\r\n<br>\r\n", range);
		if (flags & LISTENER_CLIENT)
		    sendto_http(cptr, "Client port\r\n<br>\r\n");
		if (flags & LISTENER_SERVER)
		    sendto_http(cptr, "Server port\r\n<br>\r\n");
		if (flags & LISTENER_CLIENT_SSL)
		    sendto_http(cptr, "Secure port\r\n<br>\r\n");
		if (flags & LISTENER_HTTP)
		    sendto_http(cptr, "Web port\r\n<br>\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

	    } else if (commit == 2) {

		for (li = ListenerPollList; li; li = next_li) {
		    next_li = li->next;
		    if (uuid_compare(li->uuid, u) == 0)
			close_listener(li);
		}

		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Listener Setup");
		sendto_http(cptr, "Block deleted");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

	    } else if (commit == 3) {
		send_http_header(cptr, "/listener.hta", HTTP_CREATED);
		http_get(cptr, "/listener.hta", "a");
		return 1;
	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }
	}

	else if (irc_strcmp(hurl + 1, "connects.hti") == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;
    char *srv = NULL;
    int commit = 0;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "connects.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4) == 0) {
		    if (irc_strncmp(value, "Sho", 3) == 0)
			commit = 1;
		    else if (irc_strncmp(value, "New", 3) == 0)
			commit = 2;
		} else if (irc_strncmp(element, "CONN", 4)
			   == 0) {
		    srv = value;
		}
	    }
	    if (commit == 1) {
    aConfItem *aconf = find_conf_by_uuid(srv, CONF_SERVER);
    aClass *aclass;
		send_http_header(cptr, "connects.htm", HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 4, NULL);
		sendto_http(cptr, hBuf,
			    "TR-IRCD running on",
			    me.name, "Configuration Page for the Connect Block for", aconf->name);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "form",
					   0,
					   "method=\"%s\" action=\"%s?id=%s\" enctype=\"%s\"",
					   H_POST, "connects.htc", srv,
					   "application/x-www-form-urlencoded");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Commit", "commbutton");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "reset", "Revoke", "resetbutton");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Delete Server", "delete");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "table",
					   0,
					   "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
					   0, 0, 0,
					   "border-collapse: collapse",
					   "#111111", "100%", "Server");
		sendto_http(cptr, hBuf);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Server Name");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "name", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, aconf->name);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Server Address");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "address", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, aconf->host);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Send password");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "spass", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, aconf->spasswd);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Accept password");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "apass", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, aconf->passwd);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Server port");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "port", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		ircsnprintf(strBuf1, 200, "%d", aconf->port);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection class");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0,
					   "size=\"%d\" name=\"%s\"", 1, "class");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "\r\n");
		for (aclass = ConnectionClasses; aclass; aclass = aclass->next) {
		    if (aclass->number == 0)
			continue;
		    enervate_html_object(hBuf, HTTPBUFSIZE, "option", 1, "value=\"%d\"",
					 aclass->number);
		    ircsnprintf(strBuf1, 200, "%d", aclass->number);
		    sendto_http(cptr, hBuf, strBuf1);
		    sendto_http(cptr, "\r\n");
		}
		sendto_http(cptr, "</select>\r\n");
		http_send_td(cptr, 1, 0);
		ircsnprintf(strBuf1, 200, "%d", aconf->class->number);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
		http_send_tr(cptr, 1);

		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection type");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ipv4", ((aconf->aftype == AF_INET)
							     ? "checked " : ""), "aftype");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "ipv4");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ipv6", ((aconf->aftype == AF_INET)
							     ? "" : "checked "), "aftype");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "ipv6");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->aftype == AF_INET) ? "ipv4" : "ipv6");
		http_send_tr(cptr, 1);

		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Autoconnect");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio",
					   "ON", ((aconf->flags & CONF_FLAGS_ALLOW_AUTO_CONN)
						  ? "checked " : ""), "ACONN");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio",
					   "OFF", ((aconf->flags & CONF_FLAGS_ALLOW_AUTO_CONN)
						   ? "" : "checked "), "ACONN");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->flags & CONF_FLAGS_ALLOW_AUTO_CONN)
			    ? "Yes" : "No");
		http_send_tr(cptr, 1);

		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Encrypted");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->flags & CONF_FLAGS_ENCRYPTED)
							   ? "checked " : ""), "ENCC");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->flags & CONF_FLAGS_ENCRYPTED)
							    ? "" : "checked "), "ENCC");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW,
			    24, (aconf->flags & CONF_FLAGS_ENCRYPTED) ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Compressed");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->flags & CONF_FLAGS_COMPRESSED)
							   ? "checked " : ""), "COMP");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->flags & CONF_FLAGS_COMPRESSED)
							    ? "" : "checked "), "COMP");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->flags & CONF_FLAGS_COMPRESSED)
			    ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Is Hub");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->flags & CONF_FLAGS_HUB)
							   ? "checked " : ""), "HUB");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->flags & CONF_FLAGS_HUB)
							    ? "" : "checked "), "HUB");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->flags & CONF_FLAGS_HUB)
			    ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Is Ultimate (Ulined)");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->flags & CONF_FLAGS_ULTIMATE)
							   ? "checked " : ""), "UL");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->flags & CONF_FLAGS_ULTIMATE)
							    ? "" : "checked "), "UL");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW,
			    24, (aconf->flags & CONF_FLAGS_ULTIMATE) ? "Yes" : "No");
		http_send_tr(cptr, 1);
		sendto_http(cptr, "</table>\r\n</form>\r\n");
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "form",
					   0,
					   "method=\"%s\" action=\"%s\" enctype=\"%s\"", H_POST,
					   "connects.hti", "application/x-www-form-urlencoded");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "input",
					   0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Show", "commbutton");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object_begin(hBuf,
					   HTTPBUFSIZE,
					   "select", 0, "size=\"%d\" name=\"%s\"", 1, "CONNECT");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "\r\n");
		for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
		    if (aconf->status & CONF_SERVER) {
			uuid_unparse(aconf->uuid, strBuf1);
			enervate_html_object(hBuf,
					     HTTPBUFSIZE, "option", 1, "value=\"%s\"", strBuf1);
			sendto_http(cptr, hBuf, aconf->name);
			sendto_http(cptr, "\r\n");
		    }
		}
		sendto_http(cptr, "</select>\r\n</form>\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

	    } else if (commit == 2) {
		send_http_header(cptr, "/connects.hta", HTTP_CREATED);
		http_get(cptr, "/connects.hta", "a");
		return 1;
	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }
	}

	else if (irc_strncmp(hurl + 1, "connects.htc", 12) == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;

    char *id = NULL;
    uuid_t u;

    int commit = 0;

    char *name = NULL;
    char *address = NULL;
    char *spass = NULL;
    char *apass = NULL;
    char passarr[PASSWDLEN];
    int port = 0;
    int class = 0;
    int aftype = 0;
    int flags = 0;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "connects.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    id = hurl + 17;

	    uuid_parse(id, u);

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4) == 0) {
		    if (irc_strncmp(value, "Com", 3) == 0)
			commit = 1;
		} else if (irc_strncmp(element, "del", 3) == 0) {
		    if (irc_strncmp(value, "Del", 3) == 0)
			commit = 2;
		} else if (irc_strncmp(element, "new", 3) == 0) {
		    if (irc_strncmp(value, "New", 3) == 0)
			commit = 3;
		} else if (irc_strncmp(element, "na", 2) == 0) {
		    name = value;
		} else if (irc_strncmp(element, "add", 3) == 0) {
		    address = value;
		} else if (irc_strncmp(element, "sp", 2) == 0) {
		    spass = value;
		} else if (irc_strncmp(element, "ap", 2) == 0) {
		    apass = value;
		} else if (irc_strncmp(element, "po", 2) == 0) {
		    port = atoi(value);
		} else if (irc_strncmp(element, "cl", 2) == 0) {
		    class = atoi(value);
		} else if (irc_strncmp(element, "af", 2) == 0) {
		    if (irc_strncmp(value, "ipv4", 4) == 0)
			aftype = AF_INET;
#ifdef IPV6
		    else if (irc_strncmp(value, "ipv6", 4) == 0)
			aftype = AF_INET6;
#endif
		} else if (irc_strncmp(element, "AC", 2) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= CONF_FLAGS_ALLOW_AUTO_CONN;
		} else if (irc_strncmp(element, "EN", 2) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= CONF_FLAGS_ENCRYPTED;
		} else if (irc_strncmp(element, "CO", 2) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= CONF_FLAGS_COMPRESSED;
		} else if (irc_strncmp(element, "HU", 2) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= CONF_FLAGS_HUB;
		} else if (irc_strncmp(element, "UL", 2) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= CONF_FLAGS_ULTIMATE;
		}
	    }

	    if (commit == 1) {
    aConfItem *aconf = find_conf_by_uuid(id, CONF_SERVER);

		if (!aconf) {
    uuid_t u;
		    aconf = make_conf();
		    aconf->status = CONF_SERVER;
		    uuid_parse(id, u);
		    memcpy(aconf->uuid, u, sizeof(aconf->uuid));
		    conf_add_conf(aconf);
		    conf_add_server(aconf, 1);
		}

		if (name[0]) {
		    MyFree(aconf->name);
		    DupString(aconf->name, name);
		}
		if (address[0]) {
		    MyFree(aconf->host);
		    DupString(aconf->host, address);
		}
		if (spass[0]) {
		    if (aconf->spasswd)
			memset(aconf->spasswd, 0, strlen(aconf->spasswd));
		    DupString(aconf->spasswd, spass);
		}
		if (apass[0]) {
		    if (aconf->passwd)
			memset(aconf->passwd, 0, strlen(aconf->passwd));
		    DupString(aconf->passwd, calcpass(apass, passarr));
		}
		if (port > 0)
		    aconf->port = port;
		if (class > 0)
		    aconf->class = find_class(class);
		if (aftype)
		    aconf->aftype = aftype;
		if (flags)
		    aconf->flags = flags;

		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Uplink Setup");
		sendto_http(cptr, HTTP_LINEBREAK);
		if (name[0])
		    sendto_http(cptr, "New Name: %s\r\n<br>\r\n", name);
		else
		    sendto_http(cptr, "Server: %s\r\n<br>\r\n", aconf->name);
		if (address[0])
		    sendto_http(cptr, "New Address: %s\r\n<br>\r\n", address);
		else
		    sendto_http(cptr, "Address: %s\r\n<br>\r\n", aconf->host);
		if (port)
		    sendto_http(cptr, "Connection port: %d\r\n<br>\r\n", port);
		if (class)
		    sendto_http(cptr, "Connection class: %d\r\n<br>\r\n", class);
		if (flags & CONF_FLAGS_ALLOW_AUTO_CONN)
		    sendto_http(cptr, "Autoconnect\r\n<br>\r\n");
		if (flags & CONF_FLAGS_COMPRESSED)
		    sendto_http(cptr, "Compressed\r\n<br>\r\n");
		if (flags & CONF_FLAGS_ENCRYPTED)
		    sendto_http(cptr, "Encrypted\r\n<br>\r\n");
		if (flags & CONF_FLAGS_ULTIMATE)
		    sendto_http(cptr, "Ultimate (U:lined)\r\n<br>\r\n");
		if (flags & CONF_FLAGS_HUB)
		    sendto_http(cptr, "Link is a hub\r\n<br>\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

	    } else if (commit == 2) {
    aConfItem *aconf = find_conf_by_uuid(id, CONF_SERVER);

		if (!aconf) {
		    send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		    exit_httpd_client(cptr);
		    return 0;
		}

		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Uplink Setup");
		sendto_http(cptr, "Block (%s) deleted", aconf->name);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

		delist_and_remove_conf(aconf);

	    } else if (commit == 3) {
		send_http_header(cptr, "/connects.hta", HTTP_CREATED);
		http_get(cptr, "/connects.hta", "a");
		return 1;
	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }
	}

	else if (irc_strcmp(hurl + 1, "acl.hti") == 0) {

    char *p = NULL;
    char *element = NULL;
    char *value = NULL;
    char *address = NULL, *addaddr = NULL, *deladdr = NULL;
    int commit = 0;
    dlink_node *ptr;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "acl.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4) == 0) {
		    if (irc_strncmp(value, "Delete this", 11) == 0)
			commit = 1;
		    else if (irc_strncmp(value, "Add this", 8) == 0)
			commit = 2;
		} else if (irc_strncmp(element, "ACLAdd", 6) == 0) {
		    addaddr = value;
		} else if (irc_strncmp(element, "ACLDel", 6) == 0) {
		    deladdr = value;
		}
	    }

	    if ((commit == 1) && (deladdr != NULL)) {
		for (ptr = http_except_list.head; ptr; ptr = ptr->next) {
		    address = ptr->data;
		    if (irc_strcmp(deladdr, address) == 0) {
			dlinkDelete(ptr, &http_except_list);
			free_dlink_node(ptr);
			break;
		    }
		}
		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR_IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name,
			    ": Access List Configuration");
		sendto_http(cptr, HTTP_LINEBREAK);
		sendto_http(cptr, "%s: Entry Deleted.\r\n", deladdr);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

	    } else if ((commit == 2) && (addaddr != NULL)) {
    char *tmpstr;

		ptr = make_dlink_node();
		DupString(tmpstr, addaddr);
		dlinkAdd(tmpstr, ptr, &http_except_list);
		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR_IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name,
			    ": Access List Configuration");
		sendto_http(cptr, HTTP_LINEBREAK);
		sendto_http(cptr, "%s: Entry Added.\r\n", addaddr);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }
	}

	else if (irc_strcmp(hurl + 1, "operator.hti") == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;
    char *oper = NULL;
    int commit = 0;
	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "operator.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4)
		    == 0) {
		    if (irc_strncmp(value, "Sho", 3) == 0)
			commit = 1;
		    else if (irc_strncmp(value, "New", 3) == 0)
			commit = 2;
		} else if (irc_strncmp(element, "OPER", 4)
			   == 0) {
		    oper = value;
		}
	    }
	    if (commit == 1) {
    aConfItem *aconf = find_conf_by_uuid(oper,
					 CONF_OPERATOR);
    aClass *aclass;
		send_http_header(cptr, "operator.htm", HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 4, NULL);
		sendto_http(cptr, hBuf,
			    "TR-IRCD running on",
			    me.name, "Configuration Page for the Operator Block for", aconf->name);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
					   "method=\"%s\" action=\"%s?id=%s\" enctype=\"%s\"",
					   H_POST, "operator.htc", oper,
					   "application/x-www-form-urlencoded");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Commit", "commbutton");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "reset", "Revoke", "resetbutton");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Delete Operator", "delete");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
					   "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
					   0, 0, 0, "border-collapse: collapse",
					   "#111111", "100%", "Operator");
		sendto_http(cptr, hBuf);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Operator Name");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "name", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, aconf->name);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Operator Userhost");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "address", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		ircsnprintf(strBuf2, 200, "%s@%s", aconf->user, aconf->host);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf2);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Operator password");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "pass", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, aconf->passwd);
		http_send_tr(cptr, 1);

		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection class");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0,
					   "size=\"%d\" name=\"%s\"", 1, "class");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "\r\n");
		for (aclass = ConnectionClasses; aclass; aclass = aclass->next) {
		    if (aclass->number == 0)
			continue;
		    enervate_html_object(hBuf, HTTPBUFSIZE, "option", 1, "value=\"%d\"",
					 aclass->number);
		    ircsnprintf(strBuf1, 200, "%d", aclass->number);
		    sendto_http(cptr, hBuf, strBuf1);
		    sendto_http(cptr, "\r\n");
		}
		sendto_http(cptr, "</select>\r\n");
		http_send_td(cptr, 1, 0);
		ircsnprintf(strBuf1, 200, "%d", aconf->class->number);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
		http_send_tr(cptr, 1);

		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable /die command");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->port & OFLAG_DIE)
							   ? "checked " : ""), "DIE");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->port & OFLAG_DIE)
							    ? "" : "checked "), "DIE");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->port & OFLAG_DIE) ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable /restart command");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->port & OFLAG_RESTART)
							   ? "checked " : ""), "REST");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->port & OFLAG_RESTART)
							    ? "" : "checked "), "REST");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->port & OFLAG_RESTART) ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50,
			    "See hidden things (realhost, secret channels)");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->port & OFLAG_SEEHIDDEN)
							   ? "checked " : ""), "SEEH");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->port & OFLAG_SEEHIDDEN)
							    ? "" : "checked "), "SEEH");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->port & OFLAG_SEEHIDDEN)
			    ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Is Services Administrator");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->port & OFLAG_SADMIN)
							   ? "checked " : ""), "SADM");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->port & OFLAG_SADMIN)
							    ? "" : "checked "), "SADM");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->port & OFLAG_SADMIN) ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Is Server Administrator");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->port & OFLAG_ADMIN)
							   ? "checked " : ""), "ADM");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->port & OFLAG_ADMIN)
							    ? "" : "checked "), "ADM");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->port & OFLAG_ADMIN) ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable /kline and /gline commands");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->port & OFLAG_GKLINE)
							   ? "checked " : ""), "GKL");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->port & OFLAG_GKLINE)
							    ? "" : "checked "), "GKL");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->port & OFLAG_GKLINE) ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable /operdo command");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->port & OFLAG_OPERDO)
							   ? "checked " : ""), "OPDO");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->port & OFLAG_OPERDO)
							    ? "" : "checked "), "OPDO");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->port & OFLAG_OPERDO) ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable /kill command");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->port & OFLAG_OPKILL)
							   ? "checked " : ""), "KILL");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->port & OFLAG_OPKILL)
							    ? "" : "checked "), "KILL");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->port & OFLAG_OPKILL) ? "Yes" : "No");
		http_send_tr(cptr, 1);

		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable login via the web interface");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->flags & CONF_FLAGS_WEBLOGIN)
							   ? "checked " : ""), "WEBL");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->flags & CONF_FLAGS_WEBLOGIN)
							    ? "" : "checked "), "WEBL");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24,
			    (aconf->flags & CONF_FLAGS_WEBLOGIN) ? "Yes" : "No");
		http_send_tr(cptr, 1);

		sendto_http(cptr, "</table>\r\n</form>\r\n");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
					   "method=\"%s\" action=\"%s\" enctype=\"%s\"",
					   H_POST, "operator.hti",
					   "application/x-www-form-urlencoded");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Show", "commbutton");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0,
					   "size=\"%d\" name=\"%s\"", 1, "OPER");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "\r\n");
		for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
		    if (aconf->status & CONF_OPERATOR) {
			uuid_unparse(aconf->uuid, strBuf1);
			enervate_html_object(hBuf,
					     HTTPBUFSIZE, "option", 1, "value=\"%s\"", strBuf1);
			sendto_http(cptr, hBuf, aconf->name);
			sendto_http(cptr, "\r\n");
		    }
		}
		sendto_http(cptr, "</select>\r\n</form>\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

	    } else if (commit == 2) {
		send_http_header(cptr, "/operator.hta", HTTP_CREATED);
		http_get(cptr, "/operator.hta", "a");
		return 1;
	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }
	}

	else if (irc_strncmp(hurl + 1, "operator.htc", 12) == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;

    char *id = NULL;
    uuid_t u;

    int commit = 0;

    char *name = NULL;
    char *address = NULL;
    char *pass = NULL;
    char passarr[PASSWDLEN];
    int class = 0;
    int flags = 0;
    int flags2 = 0;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "operator.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    id = hurl + 17;

	    uuid_parse(id, u);

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4) == 0) {
		    if (irc_strncmp(value, "Com", 3) == 0)
			commit = 1;
		} else if (irc_strncmp(element, "del", 3) == 0) {
		    if (irc_strncmp(value, "Del", 3) == 0)
			commit = 2;
		} else if (irc_strncmp(element, "new", 3) == 0) {
		    if (irc_strncmp(value, "New", 3) == 0)
			commit = 3;
		} else if (irc_strncmp(element, "na", 2) == 0) {
		    name = value;
		} else if (irc_strncmp(element, "add", 3) == 0) {
		    address = value;
		} else if (irc_strncmp(element, "spa", 3) == 0) {
		    pass = value;
		} else if (irc_strncmp(element, "cla", 3) == 0) {
		    class = atoi(value);
		} else if (irc_strncmp(element, "DIE", 3) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= OFLAG_DIE;
		} else if (irc_strncmp(element, "RES", 3) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= OFLAG_RESTART;
		} else if (irc_strncmp(element, "SEE", 3) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= OFLAG_SEEHIDDEN;
		} else if (irc_strncmp(element, "SADM", 4) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= OFLAG_SADMIN;
		} else if (irc_strncmp(element, "ADM", 3) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= OFLAG_ADMIN;
		} else if (irc_strncmp(element, "GKL", 3) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= OFLAG_GKLINE;
		} else if (irc_strncmp(element, "OPD", 3) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= OFLAG_OPERDO;
		} else if (irc_strncmp(element, "KILL", 4) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= OFLAG_OPKILL;
		} else if (irc_strncmp(element, "WEBL", 4) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags2 |= CONF_FLAGS_WEBLOGIN;
		}
	    }

	    if (commit == 1) {
    aConfItem *aconf = find_conf_by_uuid(id, CONF_OPERATOR);

		if (!aconf) {
    uuid_t u;
		    aconf = make_conf();
		    aconf->status = CONF_OPERATOR;
		    uuid_parse(id, u);
		    memcpy(aconf->uuid, u, sizeof(aconf->uuid));
		    conf_add_conf(aconf);
		    conf_add_class_to_conf(aconf);
		}

		if (name[0]) {
		    MyFree(aconf->name);
		    DupString(aconf->name, name);
		}
		if (address[0]) {
    char *q;
		    if ((q = strchr(address, '@'))) {
			*q = '\0';
			MyFree(aconf->user);
			DupString(aconf->user, address);
			q++;
			MyFree(aconf->host);
			DupString(aconf->host, q);
		    } else {
			MyFree(aconf->host);
			DupString(aconf->host, address);
			DupString(aconf->user, "*");
		    }
		}
		if (pass[0]) {
		    if (aconf->passwd)
			memset(aconf->passwd, 0, strlen(aconf->passwd));
		    DupString(aconf->passwd, calcpass(pass, passarr));
		}
		if (class > 0)
		    aconf->class = find_class(class);
		if (flags)
		    aconf->port = flags;	/* Be warned it IS port */
		if (flags2)
		    aconf->flags = flags2;

		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Operator Setup");
		sendto_http(cptr, HTTP_LINEBREAK);
		if (name[0])
		    sendto_http(cptr, "New Name: %s\r\n<br>\r\n", name);
		else
		    sendto_http(cptr, "Operator: %s\r\n<br>\r\n", aconf->name);
		if (address[0])
		    sendto_http(cptr, "New Address: %s\r\n<br>\r\n", address);
		else
		    sendto_http(cptr, "Address: %s\r\n<br>\r\n", aconf->host);
		if (class)
		    sendto_http(cptr, "Connection class: %d\r\n<br>\r\n", class);
		if (flags & OFLAG_ADMIN)
		    sendto_http(cptr, "Server Administrator (module interface access)\r\n<br>\r\n");
		if (flags & OFLAG_SADMIN)
		    sendto_http(cptr, "Services Administrator (/samode access)\r\n<br>\r\n");
		if (flags & OFLAG_DIE)
		    sendto_http(cptr, "Allowed to use /die\r\n<br>\r\n");
		if (flags & OFLAG_RESTART)
		    sendto_http(cptr, "Allowed to use /restart\r\n<br>\r\n");
		if (flags & OFLAG_OPERDO)
		    sendto_http(cptr, "Allowed to use /operdo\r\n<br>\r\n");
		if (flags & OFLAG_GKLINE)
		    sendto_http(cptr, "Allowed to use /kline, /unkline, and friends\r\n<br>\r\n");
		if (flags & OFLAG_OPKILL)
		    sendto_http(cptr, "Allowed to use /kill\r\n<br>\r\n");
		if (flags & OFLAG_SEEHIDDEN)
		    sendto_http(cptr,
				"Allowed to see hidden things (realhost, secret channels)\r\n<br>\r\n");
		if (flags2 & CONF_FLAGS_WEBLOGIN)
		    sendto_http(cptr, "Allowed to login via the web interface\r\n<br>\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

	    } else if (commit == 2) {
    aConfItem *aconf = find_conf_by_uuid(id, CONF_OPERATOR);

		if (!aconf) {
		    send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		    exit_httpd_client(cptr);
		    return 0;
		}

		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Operator Setup");
		sendto_http(cptr, "Block (%s) deleted", aconf->name);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

		delist_and_remove_conf(aconf);

	    } else if (commit == 3) {
		send_http_header(cptr, "/operator.hta", HTTP_CREATED);
		http_get(cptr, "/operator.hta", "a");
		return 1;
	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }
	}

	else if (irc_strcmp(hurl + 1, "class.hti") == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;
    char *cls = NULL;
    int commit = 0;
    int class = 0;
	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "class.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4)
		    == 0) {
		    if (irc_strncmp(value, "Sho", 3) == 0)
			commit = 1;
		    else if (irc_strncmp(value, "New", 3) == 0)
			commit = 2;
		} else if (irc_strncmp(element, "CLASS", 4)
			   == 0) {
		    cls = value;
		    class = atoi(cls);
		}
	    }
	    if (commit == 1) {
    aClass *aclass = find_class(class);
		send_http_header(cptr, "class.hti", HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 4, NULL);
		sendto_http(cptr, hBuf,
			    "TR-IRCD running on",
			    me.name, "Configuration Page for the class Block for", cls);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
					   "method=\"%s\" action=\"%s?class=%d\" enctype=\"%s\"",
					   H_POST, "class.htc", class,
					   "application/x-www-form-urlencoded");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Commit", "commbutton");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "reset", "Revoke", "resetbutton");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Delete Class", "delete");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
					   "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
					   0, 0, 0, "border-collapse: collapse",
					   "#111111", "100%", "Class");
		sendto_http(cptr, hBuf);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection class item");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "class", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		ircsnprintf(strBuf1, 200, "%d", aclass->number);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Ping frequency");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "ping", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		ircsnprintf(strBuf1, 200, "%d", aclass->ping_frequency);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connect frequency");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "conn", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		ircsnprintf(strBuf1, 200, "%d", aclass->connect_frequency);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Maximum number of allowed connections");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "maxlink", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		ircsnprintf(strBuf1, 200, "%d", aclass->max_connections);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Maximum size of the send Queue");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "sendq", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		ircsnprintf(strBuf1, 200, "%d", aclass->max_sendq);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
		http_send_tr(cptr, 1);
		sendto_http(cptr, "</table>\r\n</form>\r\n");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
					   "method=\"%s\" action=\"%s\" enctype=\"%s\"",
					   H_POST, "class.hti",
					   "application/x-www-form-urlencoded");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Show", "commbutton");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0,
					   "size=\"%d\" name=\"%s\"", 1, "CLASS");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "\r\n");
		for (aclass = ConnectionClasses; aclass; aclass = aclass->next) {
		    if (aclass->number == 0)
			continue;
		    enervate_html_object(hBuf,
					 HTTPBUFSIZE, "option", 1, "value=\"%d\"", aclass->number);
		    ircsnprintf(strBuf1, 200, "%d", aclass->number);
		    sendto_http(cptr, hBuf, strBuf1);
		    sendto_http(cptr, "\r\n");
		}
		sendto_http(cptr, "</select>\r\n</form>\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);
	    } else if (commit == 2) {
		send_http_header(cptr, "/class.hta", HTTP_CREATED);
		http_get(cptr, "/class.hta", "a");
		return 1;
	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }
	}

	else if (irc_strncmp(hurl + 1, "class.htc", 9) == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;

    char *id = NULL;

    int commit = 0;

    int oldnum = 0;
    int number = 0;
    int ping_v = 0;
    int conn_v = 0;
    int maxl_v = 0;
    long sendq = 0;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "class.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    id = hurl + 17;
	    oldnum = atoi(id);

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4) == 0) {
		    if (irc_strncmp(value, "Com", 3) == 0)
			commit = 1;
		} else if (irc_strncmp(element, "new", 3) == 0) {
		    if (irc_strncmp(value, "New", 3) == 0)
			commit = 3;
		} else if (irc_strncmp(element, "cl", 2) == 0) {
		    number = atoi(value);
		} else if (irc_strncmp(element, "pi", 2) == 0) {
		    ping_v = atoi(value);
		} else if (irc_strncmp(element, "co", 2) == 0) {
		    conn_v = atoi(value);
		} else if (irc_strncmp(element, "ma", 2) == 0) {
		    maxl_v = atoi(value);
		} else if (irc_strncmp(element, "se", 2) == 0) {
		    sendq = strtol(value, NULL, 10);
		}
	    }

	    if (number < 1)
		number = oldnum;

	    if (commit > 0) {
    aClass *class;
		if ((ping_v < 1) || (conn_v < 1) || (maxl_v < 1) || (sendq < 1))
		    return 0;

		add_class(number, ping_v, conn_v, maxl_v, sendq);
		class = find_class(number);

		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Class Setup");
		sendto_http(cptr, HTTP_LINEBREAK);
		sendto_http(cptr, "Class: %d\r\n<br>\r\n", number);
		sendto_http(cptr, "Ping Frequency: %d\r\n<br>\r\n", class->ping_frequency);
		sendto_http(cptr, "Connect Frequency: %d\r\n<br>\r\n", class->connect_frequency);
		sendto_http(cptr, "Maximum number of connections: %d\r\n<br>\r\n",
			    class->max_connections);
		sendto_http(cptr, "Maximum size of send queue: %d\r\n<br>\r\n", class->max_sendq);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }

	}

	else if (irc_strcmp(hurl + 1, "auth.hti")
		 == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;
    char *cli = NULL;
    int commit = 0;
	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "operator.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4)
		    == 0) {
		    if (irc_strncmp(value, "Sho", 3) == 0)
			commit = 1;
		    else if (irc_strncmp(value, "New", 3) == 0)
			commit = 2;
		} else if (irc_strncmp(element, "AUTH", 4)
			   == 0) {
		    cli = value;
		}
	    }
	    if (commit == 1) {
    aConfItem *aconf = find_conf_by_uuid(cli, CONF_CLIENT);
    aClass *aclass;
		send_http_header(cptr, "auth.htm", HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf,
			    "TR-IRCD running on",
			    me.name, "Configuration Page for the Authentication Block");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
					   "method=\"%s\" action=\"%s?id=%s\" enctype=\"%s\"",
					   H_POST, "auth.htc", cli,
					   "application/x-www-form-urlencoded");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Commit", "commbutton");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "reset", "Revoke", "resetbutton");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Delete Block", "delete");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
					   "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
					   0, 0, 0, "border-collapse: collapse",
					   "#111111", "100%", "Auth");
		sendto_http(cptr, hBuf);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Userhost");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "userhost", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		ircsnprintf(strBuf1, 200, "%s@%s", aconf->user, aconf->host);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Password");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "pass", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, aconf->passwd);
		http_send_tr(cptr, 1);

		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection class");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0,
					   "size=\"%d\" name=\"%s\"", 1, "class");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "\r\n");
		for (aclass = ConnectionClasses; aclass; aclass = aclass->next) {
		    if (aclass->number == 0)
			continue;
		    enervate_html_object(hBuf, HTTPBUFSIZE, "option", 1, "value=\"%d\"",
					 aclass->number);
		    ircsnprintf(strBuf1, 200, "%d", aclass->number);
		    sendto_http(cptr, hBuf, strBuf1);
		    sendto_http(cptr, "\r\n");
		}
		sendto_http(cptr, "</select>\r\n");
		http_send_td(cptr, 1, 0);
		ircsnprintf(strBuf1, 200, "%d", aconf->class->number);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
		http_send_tr(cptr, 1);

		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Exempt klines (override klines)");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->flags & CONF_FLAGS_EXEMPTKLINE)
							   ? "checked " : ""), "EXKL");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->flags & CONF_FLAGS_EXEMPTKLINE)
							    ? "" : "checked "), "EXKL");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->flags & CONF_FLAGS_EXEMPTKLINE)
			    ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Exceed limits (override classes)");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->flags & CONF_FLAGS_NOLIMIT)
							   ? "checked " : ""), "EXLIM");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->flags & CONF_FLAGS_NOLIMIT)
							    ? "" : "checked "), "EXLIM");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24,
			    (aconf->flags & CONF_FLAGS_NOLIMIT) ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Spoof notice");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->flags & CONF_FLAGS_SPOOF_NOTICE)
							   ? "checked " : ""), "SPN");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->flags & CONF_FLAGS_SPOOF_NOTICE)
							    ? "" : "checked "), "SPN");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->flags & CONF_FLAGS_SPOOF_NOTICE)
			    ? "Yes" : "No");
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50,
			    "Spoof host (to replace matching users hostnames)");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "spoof", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, aconf->localhost);
		http_send_tr(cptr, 1);
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50,
			    "Connection port (useful for spoof connections)");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" name=\"%s\" size=\"%d\"",
					   "text", "port", 32);
		sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
		ircsnprintf(strBuf1, 200, "%d", aconf->port);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
		http_send_tr(cptr, 1);
		
		http_send_tr(cptr, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 50, "Can choose fakehost");
		http_send_td(cptr, 0, 26);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "ON", ((aconf->flags & CONF_FLAGS_ALLOW_FAKEHOST)
							   ? "checked " : ""), "CHF");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "Yes");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" %sname=\"%s\"",
					   "radio", "OFF", ((aconf->flags & CONF_FLAGS_ALLOW_FAKEHOST)
							    ? "" : "checked "), "CHF");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "No");
		http_send_td(cptr, 1, 0);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, (aconf->flags & CONF_FLAGS_ALLOW_FAKEHOST)
			    ? "Yes" : "No");
		http_send_tr(cptr, 1);

		
		sendto_http(cptr, "</table>\r\n</form>\r\n");
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
					   "method=\"%s\" action=\"%s\" enctype=\"%s\"",
					   H_POST, "auth.hti", "application/x-www-form-urlencoded");
		sendto_http(cptr, hBuf);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
					   "type=\"%s\" value=\"%s\" name=\"%s\"",
					   "submit", "Show", "commbutton");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0,
					   "size=\"%d\" name=\"%s\"", 1, "AUTH");
		sendto_http(cptr, hBuf);
		sendto_http(cptr, "\r\n");
		for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
		    if (aconf->status & CONF_CLIENT) {
			uuid_unparse(aconf->uuid, strBuf1);
			enervate_html_object(hBuf,
					     HTTPBUFSIZE, "option", 1, "value=\"%s\"", strBuf1);
			ircsnprintf(strBuf2, 200, "%s@%s", aconf->user, aconf->host);
			sendto_http(cptr, hBuf, strBuf2);
			sendto_http(cptr, "\r\n");
		    }
		}
		sendto_http(cptr, "</select>\r\n</form>\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);
	    } else if (commit == 2) {
		send_http_header(cptr, "/auth.hta", HTTP_CREATED);
		http_get(cptr, "/auth.hta", "a");
		return 1;
	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }
	}

	else if (irc_strncmp(hurl + 1, "auth.htc", 8) == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;

    char *id = NULL;
    uuid_t u;

    int commit = 0;

    char *address = NULL;
    char *pass = NULL;
    char *fake = NULL;
    char passarr[PASSWDLEN];
    int class = 0;
    int flags = 0;
    int port = 0;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "auth.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    id = hurl + 13;

	    uuid_parse(id, u);

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "comm", 4) == 0) {
		    if (irc_strncmp(value, "Com", 3) == 0)
			commit = 1;
		} else if (irc_strncmp(element, "del", 3) == 0) {
		    if (irc_strncmp(value, "Del", 3) == 0)
			commit = 2;
		} else if (irc_strncmp(element, "new", 3) == 0) {
		    if (irc_strncmp(value, "New", 3) == 0)
			commit = 3;
		} else if (irc_strncmp(element, "use", 3) == 0) {
		    address = value;
		} else if (irc_strncmp(element, "pas", 3) == 0) {
		    pass = value;
		} else if (irc_strncmp(element, "cla", 3) == 0) {
		    class = atoi(value);
		} else if (irc_strncmp(element, "EXK", 3) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= CONF_FLAGS_EXEMPTKLINE;
		} else if (irc_strncmp(element, "EXL", 3) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= CONF_FLAGS_NOLIMIT;
		} else if (irc_strncmp(element, "SPN", 3) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
			flags |= CONF_FLAGS_SPOOF_NOTICE;
		} else if (irc_strncmp(element, "CHF", 3) == 0) {
		    if (irc_strncmp(value, "ON", 2) == 0)
		    	flags |= CONF_FLAGS_ALLOW_FAKEHOST;	    
		} else if (irc_strncmp(element, "spo", 3) == 0) {
		    fake = value;
		} else if (irc_strncmp(element, "por", 3) == 0) {
		    port = atoi(value);
		}
	    }
	    if (commit == 1) {
    aConfItem *aconf = find_conf_by_uuid(id, CONF_CLIENT);

		if (!aconf) {
    uuid_t u;
		    aconf = make_conf();
		    aconf->status = CONF_CLIENT;
		    uuid_parse(id, u);
		    memcpy(aconf->uuid, u, sizeof(aconf->uuid));
		    conf_add_conf(aconf);
		    conf_add_class_to_conf(aconf);
		}

		if (address[0]) {
    char *q;
		    if ((q = strchr(address, '@'))) {
			*q = '\0';
			MyFree(aconf->user);
			DupString(aconf->user, address);
			q++;
			MyFree(aconf->host);
			DupString(aconf->host, q);
		    } else {
			MyFree(aconf->host);
			DupString(aconf->host, address);
			DupString(aconf->user, "*");
		    }
		}
		if (pass[0]) {
		    if (aconf->passwd)
			memset(aconf->passwd, 0, strlen(aconf->passwd));
		    DupString(aconf->passwd, calcpass(pass, passarr));
		}
		if (fake[0]) {
		    MyFree(aconf->localhost);
		    DupString(aconf->localhost, fake);
		}
		if (class)
		    aconf->class = find_class(class);
		if (port > 0)
		    aconf->port = port;
		if (flags)
		    aconf->flags = flags;

		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name,
			    "Authentication Block Setup");
		sendto_http(cptr, HTTP_LINEBREAK);
		if (address[0])
		    sendto_http(cptr, "New Address: %s\r\n<br>\r\n", address);
		else
		    sendto_http(cptr, "Address: %s@%s\r\n<br>\r\n", aconf->user, aconf->host);
		if (class)
		    sendto_http(cptr, "Connection class: %d\r\n<br>\r\n", class);
		if (port)
		    sendto_http(cptr, "Connection port: %d\r\n<br>\r\n", port);
		if (flags & CONF_FLAGS_EXEMPTKLINE)
		    sendto_http(cptr, "Override klines\r\n<br>\r\n");
		if (flags & CONF_FLAGS_NOLIMIT)
		    sendto_http(cptr, "Override limits\r\n<br>\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

	    } else if (commit == 2) {
    aConfItem *aconf = find_conf_by_uuid(id, CONF_CLIENT);

		if (!aconf) {
		    send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		    exit_httpd_client(cptr);
		    return 0;
		}

		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name,
			    "Authentication Block Setup");
		sendto_http(cptr, "Block (%s@%s) deleted", aconf->user, aconf->host);
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);

		delist_and_remove_conf(aconf);

	    } else if (commit == 3) {
		send_http_header(cptr, "/auth.hta", HTTP_CREATED);
		http_get(cptr, "/auth.hta", "a");
		return 1;
	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }

	}

	else if (irc_strncmp(hurl + 1, "configure.hti", 8) == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;

    int commit = 0;
    int i = 0;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "auth.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		ScannerConf.action = 0;
		if (irc_strncmp(element, "comm", 4) == 0) {
		    if (irc_strncmp(value, "Com", 3) == 0)
			commit = 1;
		} else if (irc_strncmp(element, "admind", 6) == 0) {
		    if (value[0]) {
			MyFree(AdminInfo.description);
			DupString(AdminInfo.description, value);
		    }
		} else if (irc_strncmp(element, "adminm", 6) == 0) {
		    if (value[0]) {
			MyFree(AdminInfo.email);
			DupString(AdminInfo.email, value);
		    }
		} else if (irc_strncmp(element, "admin", 5) == 0) {
		    if (value[0]) {
			MyFree(AdminInfo.name);
			DupString(AdminInfo.name, value);
		    }
		} else if (irc_strncmp(element, "servern", 7) == 0) {
		    if (value[0]) {
			MyFree(ServerInfo.name);
			DupString(ServerInfo.name, value);
		    }
		} else if (irc_strncmp(element, "serverd", 7) == 0) {
		    if (value[0]) {
			MyFree(ServerInfo.description);
			DupString(ServerInfo.description, value);
		    }
		} else if (irc_strncmp(element, "netn", 4) == 0) {
		    if (value[0]) {
			MyFree(ServerInfo.networkname);
			DupString(ServerInfo.networkname, value);
		    }
		} else if (irc_strncmp(element, "netd", 4) == 0) {
		    if (value[0]) {
			MyFree(ServerInfo.networkdesc);
			DupString(ServerInfo.networkdesc, value);
		    }
		} else if (irc_strncmp(element, "res", 3) == 0) {
		    if (value[0]) {
    char passarr[PASSWDLEN];
			MyFree(ServerInfo.restartpass);
			DupString(ServerInfo.restartpass, calcpass(value, passarr));
		    }
		} else if (irc_strncmp(element, "die", 3) == 0) {
		    if (value[0]) {
    char passarr[PASSWDLEN];
			MyFree(ServerInfo.diepass);
			DupString(ServerInfo.diepass, calcpass(value, passarr));
		    }
		} else if (irc_strncmp(element, "dis", 3) == 0) {
		    if (value[0]) {
    char passarr[PASSWDLEN];
			MyFree(ServerInfo.displaypass);
			DupString(ServerInfo.displaypass, calcpass(value, passarr));
		    }
		} else if (irc_strncmp(element, "saddr", 5) == 0) {
		    if (value[0]) {
			ServerInfo.specific_ipv4_vhost = 1;
			inetpton(DEF_FAM, value, &IN_ADDR(ServerInfo.address));
		    }
		} else if (irc_strncmp(element, "s6addr", 6) == 0) {
		    if (value[0]) {
			ServerInfo.specific_ipv6_vhost = 1;
			inetpton(DEF_FAM, value, &IN_ADDR(ServerInfo.address6));
		    }
		} else if (irc_strncmp(element, "ide", 3) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerInfo.identity = i;
		} else if (irc_strncmp(element, "lang", 3) == 0) {
		    i = lang_parse(value);
		    if (i > 0)
			me.lang = i;
		} else if (irc_strncmp(element, "SHM", 3) == 0) {
		    if (value[1] == 'N')
			ServerOpts.short_motd = 1;
		    else if (value[1] == 'F')
			ServerOpts.short_motd = 0;
		} else if (irc_strncmp(element, "STAF", 4) == 0) {
		    if (value[1] == 'N')
			ServerOpts.staffhide = 1;
		    else if (value[1] == 'F')
			ServerOpts.staffhide = 0;
		} else if (irc_strncmp(element, "skl", 3) == 0) {
		    if (value[0]) {
			MyFree(ServerOpts.server_kline_address);
			DupString(ServerOpts.server_kline_address, value);
		    }
		} else if (irc_strncmp(element, "nkl", 3) == 0) {
		    if (value[0]) {
			MyFree(ServerOpts.network_kline_address);
			DupString(ServerOpts.network_kline_address, value);
		    }
		} else if (irc_strncmp(element, "WIN", 3) == 0) {
		    if (value[1] == 'N')
			ServerOpts.wingate_notice = 1;
		    else if (value[1] == 'F')
			ServerOpts.wingate_notice = 0;
		} else if (irc_strncmp(element, "monh", 4) == 0) {
		    if (value[0]) {
			MyFree(ServerOpts.monitor_host);
			DupString(ServerOpts.monitor_host, value);
		    }
		} else if (irc_strncmp(element, "monu", 4) == 0) {
		    if (value[0]) {
			MyFree(ServerOpts.proxy_url);
			DupString(ServerOpts.proxy_url, value);
		    }
		} else if (irc_strncmp(element, "GEC", 3) == 0) {
		    if (value[1] == 'N')
			ServerOpts.hide_gcos_with_sgline = 1;
		    else if (value[1] == 'F')
			ServerOpts.hide_gcos_with_sgline = 0;
		} else if (irc_strncmp(element, "geco", 4) == 0) {
		    if (value[0]) {
			MyFree(ServerOpts.realname_replacement);
			DupString(ServerOpts.realname_replacement, value);
		    }
		} else if (irc_strncmp(element, "tsw", 3) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.ts_warn_delta = i;
		} else if (irc_strncmp(element, "tsm", 3) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.ts_max_delta = i;
		} else if (irc_strncmp(element, "defk", 3) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.default_kline_time = i;
		} else if (irc_strncmp(element, "thrc", 3) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.throttle_count = i;
		} else if (irc_strncmp(element, "thrw", 3) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.throttle_time = i;
		} else if (irc_strncmp(element, "clif", 4) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.client_flood = i;
		} else if (irc_strncmp(element, "motdw", 5) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.motd_wait = i;
		} else if (irc_strncmp(element, "motdm", 5) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.motd_max = i;
		} else if (irc_strncmp(element, "ANF", 3) == 0) {
		    if (value[1] == 'N')
			ServerOpts.anti_nick_flood = 1;
		    else if (value[1] == 'F')
			ServerOpts.anti_nick_flood = 0;
		} else if (irc_strncmp(element, "maxnickt", 8) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.max_nick_time = i;
		} else if (irc_strncmp(element, "maxnickc", 8) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.max_nick_changes = i;
		} else if (irc_strncmp(element, "maxawayt", 8) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.max_away_time = i;
		} else if (irc_strncmp(element, "maxknock", 8) == 0) {
                    i = atoi(value);
                    if (i > 0)
                        ServerOpts.max_knock_time = i;
		} else if (irc_strncmp(element, "maxsta", 6) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.save_maxclient_stats_time = i;
		} else if (irc_strncmp(element, "SPAM", 4) == 0) {
		    if (value[1] == 'N')
			ServerOpts.anti_spambot = 1;
		    else if (value[1] == 'F')
			ServerOpts.anti_spambot = 0;
		} else if (irc_strncmp(element, "minj", 4) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.min_join_leave_time = i;
		} else if (irc_strncmp(element, "maxj", 4) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.max_join_leave_count = i;
		} else if (irc_strncmp(element, "jl", 2) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.join_leave_count_expire_time = i;
		} else if (irc_strncmp(element, "opersp", 6) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.oper_spam_countdown = i;
		} else if (irc_strncmp(element, "DEFR", 4) == 0) {
		    if (value[1] == 'N')
			ServerOpts.use_registerfilter = 1;
		    else if (value[1] == 'F')
			ServerOpts.use_registerfilter = 0;
		} else if (irc_strncmp(element, "NOOJ", 4) == 0) {
		    if (value[1] == 'N')
			ServerOpts.no_oper_jupiter = 1;
		    else if (value[1] == 'F')
			ServerOpts.no_oper_jupiter = 0;
		} else if (irc_strncmp(element, "DEFX", 4) == 0) {
		    if (value[1] == 'N')
			ServerOpts.default_fakehost_mode = 1;
		    else if (value[1] == 'F')
			ServerOpts.default_fakehost_mode = 0;
                } else if (irc_strncmp(element, "ENOMS", 5) == 0) {
                    if (value[1] == 'N')
                        ServerOpts.no_messages_on_away = 1;
                    else if (value[1] == 'F')
                        ServerOpts.no_messages_on_away = 0;
		} else if (irc_strncmp(element, "REGEX", 5) == 0) {
		    if (value[1] == 'N')
			ServerOpts.use_regex = 1;
		    else if (value[1] == 'F')
			ServerOpts.use_regex = 0;
		} else if (irc_strncmp(element, "IDCO", 4) == 0) {
		    if (value[1] == 'N')
			ServerOpts.identd_complain = 1;
		    else if (value[1] == 'F')
			ServerOpts.identd_complain = 0;
		} else if (irc_strncmp(element, "IDTIL", 5) == 0) {
		    if (value[1] == 'N')
			ServerOpts.identd_use_tilde = 1;
		    else if (value[1] == 'F')
			ServerOpts.identd_use_tilde = 0;
		} else if (irc_strncmp(element, "idti", 4) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerOpts.identd_timeout = i;
		} else if (irc_strncmp(element, "ns", 2) == 0) {
		    if (value[0]) {
			MyFree(ServicesConf.nickserv);
			DupString(ServicesConf.nickserv, value);
		    }
		} else if (irc_strncmp(element, "cs", 2) == 0) {
		    if (value[0]) {
			MyFree(ServicesConf.chanserv);
			DupString(ServicesConf.chanserv, value);
		    }
		} else if (irc_strncmp(element, "ms", 2) == 0) {
		    if (value[0]) {
			MyFree(ServicesConf.memoserv);
			DupString(ServicesConf.memoserv, value);
		    }
		} else if (irc_strncmp(element, "os", 2) == 0) {
		    if (value[0]) {
			MyFree(ServicesConf.operserv);
			DupString(ServicesConf.operserv, value);
		    }
		} else if (irc_strncmp(element, "ss", 2) == 0) {
		    if (value[0]) {
			MyFree(ServicesConf.statserv);
			DupString(ServicesConf.statserv, value);
		    }
		} else if (irc_strncmp(element, "hs", 2) == 0) {
		    if (value[0]) {
			MyFree(ServicesConf.helpserv);
			DupString(ServicesConf.helpserv, value);
		    }
		} else if (irc_strncmp(element, "servic", 6) == 0) {
		    if (value[0]) {
			MyFree(ServicesConf.services_name);
			DupString(ServicesConf.services_name, value);
		    }
		} else if (irc_strncmp(element, "HUL", 3) == 0) {
		    if (value[1] == 'N')
			ServicesConf.hide_ulined_servers = 1;
		    else if (value[1] == 'F')
			ServicesConf.hide_ulined_servers = 0;
		} else if (irc_strncmp(element, "DEXT", 4) == 0) {
		    if (value[1] == 'N')
			ChannelConf.default_extended_topic_limitation = 1;
		    else if (value[1] == 'F')
			ChannelConf.default_extended_topic_limitation = 0;
                } else if (irc_strncmp(element, "VISS", 4) == 0) {
                    if (value[1] == 'N')
                        ChannelConf.visschan = 1;
                    else if (value[1] == 'F')
                        ChannelConf.visschan = 0;
		} else if (irc_strncmp(element, "maxch", 5) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ChannelConf.max_channels_per_user = i;
		} else if (irc_strncmp(element, "qmsg", 4) == 0) {
		    if (value[0]) {
			MyFree(ChannelConf.default_quit_msg);
			DupString(ChannelConf.default_quit_msg, value);
		    }
		} else if (irc_strncmp(element, "SHID", 4) == 0) {
		    if (value[1] == 'N')
			ServerHide.enable = 1;
		    else if (value[1] == 'F')
			ServerHide.enable = 0;
		} else if (irc_strncmp(element, "LFIL", 4) == 0) {
		    if (value[1] == 'N')
			ServerHide.links_from_file = 1;
		    else if (value[1] == 'F')
			ServerHide.links_from_file = 0;
		} else if (irc_strncmp(element, "FLAT", 4) == 0) {
		    if (value[1] == 'N')
			ServerHide.flatten_links = 1;
		    else if (value[1] == 'F')
			ServerHide.flatten_links = 0;
		} else if (irc_strncmp(element, "LINK", 4) == 0) {
		    if (value[1] == 'N')
			ServerHide.links_oper_only = 1;
		    else if (value[1] == 'F')
			ServerHide.links_oper_only = 0;
		} else if (irc_strncmp(element, "linksd", 6) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ServerHide.links_delay = i;
		} else if (irc_strncmp(element, "fdlimit", 7) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ScannerConf.fd_limit = i;
		} else if (irc_strncmp(element, "dnsbl", 5) == 0) {
		    if (value[0]) {
			MyFree(ScannerConf.dnsblhost);
			DupString(ScannerConf.dnsblhost, value);
		    }
		} else if (irc_strncmp(element, "maxread", 7) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ScannerConf.max_read = i;
		} else if (irc_strncmp(element, "timeout", 7) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ScannerConf.timeout = i;
		} else if (irc_strncmp(element, "negfail", 7) == 0) {
		    if (value[1] == 'N')
			ScannerConf.negfail_notices = 1;
		    else if (value[1] == 'Y')
			ScannerConf.negfail_notices = 0;
		} else if (irc_strncmp(element, "PRAK", 4) == 0) {
			ScannerConf.action |= PROXYMON_AUTOKILL;
		} else if (irc_strncmp(element, "PRKL", 4) == 0) {
			ScannerConf.action |= PROXYMON_KLINE;
		} else if (irc_strncmp(element, "PRGN", 4) == 0) {
			ScannerConf.action |= PROXYMON_GNOTICE;
		} else if (irc_strncmp(element, "scan_ip", 7) == 0) {
		    if (value[0]) {
			MyFree(ScannerConf.scan_ip);
			DupString(ScannerConf.scan_ip, value);
		    }
		} else if (irc_strncmp(element, "scan_port", 9) == 0) {
		    i = atoi(value);
		    if (i > 0)
			ScannerConf.scan_port = i;
		} else if (irc_strncmp(element, "bind_ip", 7) == 0) {
		    if (value[0]) {
			MyFree(ScannerConf.bind_ip);
			DupString(ScannerConf.bind_ip, value);
		    }
		} else if (irc_strncmp(element, "require_index", 13) == 0) {
		    if (value[1] == 'N')
			HttpdConf.require_index = 1;
		    else if (value[1] == 'Y')
			HttpdConf.require_index = 0;

		} else if (irc_strncmp(element, "index_file", 10) == 0) {
		    if (value[0]) {
			MyFree(HttpdConf.index_file);
			DupString(HttpdConf.index_file, value);
		    }
		} else if (irc_strncmp(element, "policy", 6) == 0) {
		    if (value[1] == 'N')
			HttpdConf.policy = 1;
		    else if (value[1] == 'Y')
			HttpdConf.policy = 0;
		} else if (irc_strncmp(element, "proxy_enable", 12) == 0) {
		    if (value[1] == 'N')
			GeneralOpts.enable_proxymonitor = 1;
		    else if (value[1] == 'Y')
			GeneralOpts.enable_proxymonitor = 0;

		} else if (irc_strncmp(element, "proxy_module", 12) == 0) {
		    if (value[0]) {
			MyFree(GeneralOpts.proxymodulefile);
			DupString(GeneralOpts.proxymodulefile, value);
		    }

		} else if (irc_strncmp(element, "proxy_conffi", 12) == 0) {
		    if (value[0]) {
			MyFree(GeneralOpts.proxyconffile);
			DupString(GeneralOpts.proxyconffile, value);
		    }
		}

	    }

	    if (commit == 1) {
		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Server Options");
		sendto_http(cptr, HTTP_LINEBREAK);
		sendto_http(cptr, "Server configured.\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "config.htm");
		sendto_http(cptr, hBuf, "Back to the options");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);
		return 0;
	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }

	}

	else if (irc_strcmp(hurl + 1, "commands.hti") == 0) {
    char *p = NULL;
    char *element = NULL;
    char *value = NULL;

    int commit = 0;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "commands.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    for (element = strtoken(&p, decoded_data, "&"); element; element = strtoken(&p, NULL, "&")) {
		value = strchr(element, '=');
		value[0] = '\0';
		value++;
		if (irc_strncmp(element, "reha", 4) == 0)
		    commit = 1;
		else if (irc_strncmp(element, "shut", 4) == 0)
		    commit = 2;
		else if (irc_strncmp(element, "rest", 4) == 0)
		    commit = 3;
		else if (irc_strncmp(element, "writ", 4) == 0)
		    commit = 4;
	    }

	    if (commit == 1) {
		rehash(&me, &me, 1);
		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Commands");
		sendto_http(cptr, HTTP_LINEBREAK);
		sendto_http(cptr, "Server rehashed.\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);
	    } else if (commit == 2) {
		sendto_all("%s RDIE :Server shutdown in 15 seconds", MSG_NOTICE);
		eventAdd("DIE", die_delayed, NULL, 15);
		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Commands");
		sendto_http(cptr, HTTP_LINEBREAK);
		sendto_http(cptr, "Server shutdown in 15 seconds.\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);
	    } else if (commit == 3) {
		sendto_all("%s REST :Server restart in 15 seconds", MSG_NOTICE);
		eventAdd("RESTART", restart_delayed, "Http Restart", 15);
		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Commands");
		sendto_http(cptr, HTTP_LINEBREAK);
		sendto_http(cptr, "Server restart in 15 seconds.\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);
	    } else if (commit == 4) {
		generate_server_config_file(CONFIGFILE);
		send_http_header(cptr, hurl, HTTP_OK);
		sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
		enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
		sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Commands");
		sendto_http(cptr, HTTP_LINEBREAK);
		sendto_http(cptr, "Server configuration updated.\r\n");
		sendto_http(cptr, HTTP_LINEBREAK);
		enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
		sendto_http(cptr, hBuf, "Back to the selection");
		sendto_http(cptr, "\r\n");
		sendto_http(cptr, HTTP_PAGE_FOOT);
		exit_httpd_client(cptr);
	    } else {
		send_http_header(cptr, hurl, HTTP_INTERNAL_ERROR);
		exit_httpd_client(cptr);
		return 0;
	    }
	}

	else {
	    send_http_header(cptr, hurl, HTTP_NOT_FOUND);
	    exit_httpd_client(cptr);
	    return 0;
	}

    }

    return 0;
}
