/************************************************************************
 *   IRC - Internet Relay Chat, src/httpd/httpd_get.c
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
 * $Id: httpd_get.c,v 1.11 2004/07/12 09:14:59 tr-ircd Exp $
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

char httpBuf[HTTPBUFSIZE];

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

int http_get(aClient *cptr, char *url, char *state)
{
    char *hurl = url;
    char hBuf[HTTPBUFSIZE];
    char strBuf1[200];
    char strBuf2[200];
    char strBuf3[200];

    if (cptr->httpflags & HFLAGS_EXPECTATION_FAILED) {
	send_http_header(cptr, hurl, HTTP_EXPECTATION_FAILED);
	exit_httpd_client(cptr);
	return 0;
    }

    if (hurl[0] == '/') {
	if ((irc_strncmp(hurl, HttpdConf.index_file, HttpdConf.index_file_len) == 0)
	    || ((HttpdConf.require_index == 0) && (irc_strcmp(hurl, "/") == 0))) {
	    if (!state)
		send_http_header(cptr, hurl, HTTP_OK);
	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 1, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD Statistics Page");
	    sendto_http(cptr, "%12s: %s\r\n", "Servername", me.name);
	    sendto_http(cptr, "%12s: %s\r\n", "Network", ServerInfo.networkname);
	    sendto_http(cptr, "\r\n");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h3", 1, NULL);
	    sendto_http(cptr, hBuf, "Network Statistics");
	    sendto_http(cptr, "\r\n");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "ul", 3, NULL);
	    ircsnprintf(strBuf1, 200,
			"<li>There are %d users and %d invisible on %d servers</li>\r\n",
			Count.total, Count.invisi, Count.server);
	    ircsnprintf(strBuf2, 200,
			"<li>There are %d operators and %d unknown connections</li>\r\n",
			Count.oper, Count.unknown);
	    ircsnprintf(strBuf3, 200,
			"<li>%d channels are created, I have %d clients, and %d servers</li>\r\n",
			Count.chan, Count.local, Count.myserver - Count.myulined);
	    sendto_http(cptr, hBuf, strBuf1, strBuf2, strBuf3);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "ul", 2, NULL);
	    ircsnprintf(strBuf1, 200, "<li>Current local users: %d Max: %d</li>\r\n",
			Count.local, Count.max_loc);
	    ircsnprintf(strBuf2, 200, "<li>Current global users: %d Max: %d</li>\r\n",
			Count.total, Count.max_tot);
	    sendto_http(cptr, hBuf, strBuf1, strBuf2);
	    sendto_http(cptr, "\r\n");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "login.htm");
	    sendto_http(cptr, hBuf, "Login");
	    sendto_http(cptr, "\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);
	    exit_httpd_client(cptr);
	}

	else if (irc_strcmp(hurl + 1, "login.htm") == 0) {
	    if (!state)
		send_http_header(cptr, "login.htm", HTTP_OK);
	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 1, NULL);
	    sendto_http(cptr, hBuf, "Login");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s\" enctype=\"%s\"", H_POST,
				       "logon.hti", "application/x-www-form-urlencoded");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit", "Login",
				       "commbutton");
	    sendto_http(cptr, hBuf);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "reset", "Reset",
				       "resetbutton");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "50%",
				       "Login");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 25, "Loginname");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "user", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 25, "Password");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "password", "pass",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</form>\r\n");
	    sendto_http(cptr, "Please provide your Operator username/password to continue\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);
	    exit_httpd_client(cptr);

	}

	else if (irc_strcmp(hurl + 1, "select.htm") == 0) {

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "select.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    if (!state)
		send_http_header(cptr, "select.htm", HTTP_OK);
	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name,
			": Select the Element you want to configure");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "config.htm");
	    sendto_http(cptr, hBuf, "Ircd Setup (Options)");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "operator.htm");
	    sendto_http(cptr, hBuf, "Operator Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "connects.htm");
	    sendto_http(cptr, hBuf, "Uplink/Downlink Connections Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "class.htm");
	    sendto_http(cptr, hBuf, "Connection Classes Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "auth.htm");
	    sendto_http(cptr, hBuf, "Authentication Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "listener.htm");
	    sendto_http(cptr, hBuf, "Port configuration");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "commands.htm");
	    sendto_http(cptr, hBuf, "Ircd Control Functions");
	    sendto_http(cptr, HTTP_LINEBREAK);
    	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "acl.htm");
	    sendto_http(cptr, hBuf, "Access List Configuration");
	    sendto_http(cptr, HTTP_LINEBREAK);	    
	    sendto_http(cptr, "\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);
	    exit_httpd_client(cptr);
	}

	else if (irc_strcmp(hurl + 1, "listener.htm") == 0) {

    struct Listener *li;
    struct Listener *next_li;
    struct Listener *prev_li;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "listener.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    if (!state)
		send_http_header(cptr, "listener.htm", HTTP_OK);
	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Listener Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s\" enctype=\"%s\"", H_POST,
				       "listener.hti", "application/x-www-form-urlencoded");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit", "Show",
				       "commbutton");
	    sendto_http(cptr, hBuf);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit",
				       "New Listener Block", "commbutton");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, HTTP_LINEBREAK);

	    sendto_http(cptr, "Listener Blocks");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0, "size=\"%d\" name=\"%s\"", 1,
				       "LISTEN");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "\r\n");

	    prev_li = next_li = NULL;
	    for (li = ListenerPollList; li; li = next_li) {
		next_li = li->next;
		prev_li = li;
		memset(strBuf1, 0, sizeof(char) * 200);
		memset(strBuf2, 0, sizeof(char) * 200);
		while (next_li && (uuid_compare(li->uuid, next_li->uuid) == 0)) {
		    prev_li = next_li;
		    next_li = next_li->next;
		}
		uuid_unparse(li->uuid, strBuf1);
		enervate_html_object(hBuf, HTTPBUFSIZE, "option", 1, "value=\"%s\"", strBuf1);
		if (prev_li && (prev_li != li->next) && (prev_li != li))
		    ircsnprintf(strBuf2, 200, "%d .. %d", prev_li->port, li->port);
		else
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

	}

	else if (irc_strcmp(hurl + 1, "listener.hta") == 0) {

    uuid_t u;
    char id[64];

	    uuid_generate(u);
	    uuid_unparse(u, id);
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
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "50%",
				       "Listener");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Listener Address");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "la", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Listener Ports (Range)");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "range", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Listener Properties");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" checked value=\"%s\"", "checkbox",
				       "client", "1");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Is this port a client port ?\r\n");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" value=\"%s\"", "checkbox",
				       "server", "1");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Is this port a server port ?\r\n");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" value=\"%s\"", "checkbox",
				       "ssl", "1");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Is this port a secure connection port ?\r\n");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" value=\"%s\"", "checkbox",
				       "web", "1");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Is this port a web server port ?\r\n");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</form>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
	    sendto_http(cptr, hBuf, "Back to the selection");
	    sendto_http(cptr, "\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);
	    exit_httpd_client(cptr);

	}

	else if (irc_strcmp(hurl + 1, "connects.htm") == 0) {

    aConfItem *aconf;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "connects.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    if (!state)
		send_http_header(cptr, "connects.htm", HTTP_OK);
	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name,
			"Uplink/Downlink Connection Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s\" enctype=\"%s\"", H_POST,
				       "connects.hti", "application/x-www-form-urlencoded");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit", "Show",
				       "commbutton");
	    sendto_http(cptr, hBuf);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit",
				       "New Connect Block", "commbutton");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, HTTP_LINEBREAK);

	    sendto_http(cptr, "Connect Blocks (Linked Servers)");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0, "size=\"%d\" name=\"%s\"", 1,
				       "CONNECT");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "\r\n");
	    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
		if (aconf->status & CONF_SERVER) {
		    uuid_unparse(aconf->uuid, strBuf1);
		    enervate_html_object(hBuf, HTTPBUFSIZE, "option", 1, "value=\"%s\"", strBuf1);
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

	}

	else if (irc_strcmp(hurl + 1, "connects.hta") == 0) {

    uuid_t u;
    char id[64];
    aClass *aclass;

	    uuid_generate(u);
	    uuid_unparse(u, id);

	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name,
			"Uplink/Downlink Connection Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s?id=%s\" enctype=\"%s\"",
				       H_POST, "connects.htc", id,
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
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "50%",
				       "Server");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Server Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "name", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Server Address");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "address",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Send password");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "spass", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Accept password (automatically hashed)");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "apass", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection port");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "port", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection class");
	    http_send_td(cptr, 0, 32);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0, "size=\"%d\" name=\"%s\"", 1,
				       "class");
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
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection type");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "ipv4", "aftype");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "ipv4");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ipv6",
				       "aftype");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "ipv6");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Autoconnect");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON", "AC");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "AC");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Compressed");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "COMP");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "COMP");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Encrypted");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "ENCR");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "ENCR");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Ultimate (U:lined");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON", "UL");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "UL");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Hub ?");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "HUB");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "HUB");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</form>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
	    sendto_http(cptr, hBuf, "Back to the selection");
	    sendto_http(cptr, "\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);
	    exit_httpd_client(cptr);
	}

	else if (irc_strcmp(hurl + 1, "operator.htm") == 0) {

    aConfItem *aconf;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "operator.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    if (!state)
		send_http_header(cptr, "operator.htm", HTTP_OK);
	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Operator Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s\" enctype=\"%s\"", H_POST,
				       "operator.hti", "application/x-www-form-urlencoded");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit", "Show",
				       "commbutton");
	    sendto_http(cptr, hBuf);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit",
				       "New Operator Block", "commbutton");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, HTTP_LINEBREAK);

	    sendto_http(cptr, "Operator Blocks");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0, "size=\"%d\" name=\"%s\"", 1,
				       "OPER");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "\r\n");
	    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
		if (aconf->status & CONF_OPERATOR) {
		    uuid_unparse(aconf->uuid, strBuf1);
		    enervate_html_object(hBuf, HTTPBUFSIZE, "option", 1, "value=\"%s\"", strBuf1);
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

	}

	else if (irc_strcmp(hurl + 1, "operator.hta") == 0) {

    uuid_t u;
    char id[64];
    aClass *aclass;

	    uuid_generate(u);
	    uuid_unparse(u, id);

	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Operator Connection Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s?id=%s\" enctype=\"%s\"",
				       H_POST, "operator.htc", id,
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
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "50%",
				       "Operator");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Operator Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "name", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection Address");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "address",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Password (automatically hashed)");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "spass", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection class");
	    http_send_td(cptr, 0, 32);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0, "size=\"%d\" name=\"%s\"", 1,
				       "class");
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
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Is Server Administrator ?");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "ADM");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "ADM");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Is Services Administrator ?");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "SADM");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "SADM");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Allowed to use /die");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "DIE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "DIE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Allowed to use /restart");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "REST");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "REST");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Allowed to use /kline, /unkline and friends");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "GKL");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "GKL");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Allowed to use /operdo");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "OPDO");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "OPDO");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Allowed to use /kill");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "KILL");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "KILL");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50,
			"Allowed to see hidden things (realhost, secret channels)");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "SEEH");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "SEEH");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Allowed to login via the web interface");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "WEBL");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "WEBL");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</form>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
	    sendto_http(cptr, hBuf, "Back to the selection");
	    sendto_http(cptr, "\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);
	    exit_httpd_client(cptr);
	}

	else if (irc_strcmp(hurl + 1, "class.htm") == 0) {

    aClass *aclass;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "class.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    if (!state)
		send_http_header(cptr, "class.htm", HTTP_OK);
	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Connection Classes Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s\" enctype=\"%s\"", H_POST,
				       "class.hti", "application/x-www-form-urlencoded");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input ", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit", "Show",
				       "commbutton");
	    sendto_http(cptr, hBuf);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit",
				       "New Class Block", "commbutton");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, HTTP_LINEBREAK);

	    sendto_http(cptr, "Connection Classes");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0, "size=\"%d\" name=\"%s\"", 1,
				       "CLASS");
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
	    sendto_http(cptr, "</select>\r\n</form>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
	    sendto_http(cptr, hBuf, "Back to the selection");
	    sendto_http(cptr, "\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);
	    exit_httpd_client(cptr);

	}

	else if (irc_strcmp(hurl + 1, "class.hta") == 0) {

	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Connection Class Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s?class=%d\" enctype=\"%s\"",
				       H_POST, "class.htc", ConnectionClasses->number + 1,
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
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "50%",
				       "Class");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Class Number");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "class", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);
	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Ping Frequency");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "ping", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);
	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connect Frequency");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "conn", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);
	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Maximum Number of Allowed Connections");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "maxlink",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);
	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Maximum size of the send queue");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "sendq", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</form>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
	    sendto_http(cptr, hBuf, "Back to the selection");
	    sendto_http(cptr, "\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);
	    exit_httpd_client(cptr);
	}

	
	else if (irc_strcmp(hurl + 1, "acl.htm") == 0) {
	    dlink_node *ptr;
	    char *address;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
	    	send_http_header(cptr, "acl.htm", HTTP_FORBIDDEN);
    		exit_httpd_client(cptr);
		return 0;
	    }

	    if (!state)
		send_http_header(cptr, "acl.htm", HTTP_OK);

	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR_IRCD Configuration");
  	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR_IRCD running on", me.name, ": Access List Configuration");
	    sendto_http(cptr, HTTP_LINEBREAK);	    

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s\" enctype=\"%s\"", H_POST,
				       "acl.hti", "application/x-www-form-urlencoded");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0, "type=\"%s\" value=\"%s\" name=\"%s\"", "submit", "Delete this Entry", "commbutton");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, HTTP_LINEBREAK);
	    sendto_http(cptr, "Access List Entries");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0, "size=\"%d\" name=\"%s\"", 1, "ACLDelEntry");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "\r\n");

	    for (ptr = http_except_list.head; ptr; ptr = ptr->next) {
		address = ptr->data;
		enervate_html_object(hBuf, HTTPBUFSIZE, "option", 1, "value=\"%s\"", address);
		sendto_http(cptr, hBuf, address);
		sendto_http(cptr, "\r\n");
	    }
	    sendto_http(cptr, "</select>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    
	    sendto_http(cptr, HTTP_TABLE_ROW, 25, "Address to add into the ACL:  ");
    	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0, 
				"type=\"%s\" name=\"%s\" size=\"%d\"",
		    		"text", "ACLAddEntry", 32);
	    
	    sendto_http(cptr, HTTP_TABLE_ROW, 20, hBuf);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0, 
			"type=\"%s\" value=\"%s\" name=\"%s\"",
	    		"submit", "Add this Entry", "commbutton");
	    sendto_http(cptr, HTTP_TABLE_ROW, 20, hBuf);
	    sendto_http(cptr, "</form>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);	    
	    
 	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
	    sendto_http(cptr, hBuf, "Back to the selection");
	    sendto_http(cptr, "\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);
	    exit_httpd_client(cptr);


	}

	
	else if (irc_strcmp(hurl + 1, "commands.htm") == 0) {
	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "commands.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    if (!state)
		send_http_header(cptr, "commands.htm", HTTP_OK);
	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, ": Control Center");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s\" enctype=\"%s\"", H_POST,
				       "commands.hti", "application/x-www-form-urlencoded");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "30%",
				       "ControlCmd");

	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Rehash the IRC server");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit", "Rehash",
				       "rehashbutton");
	    sendto_http(cptr, hBuf);
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Shutdown the IRC server");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit", "Shutdown",
				       "shutdownbutton");
	    sendto_http(cptr, hBuf);
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Restart the IRC server");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit", "Restart",
				       "restartbutton");
	    sendto_http(cptr, hBuf);
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Write server configuration down");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit",
				       "Write Config", "writeconfbutton");
	    sendto_http(cptr, hBuf);
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</form>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
	    sendto_http(cptr, hBuf, "Back to the selection");
	    sendto_http(cptr, "\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);

	    exit_httpd_client(cptr);

	}

	else if (irc_strcmp(hurl + 1, "auth.htm") == 0) {

    aConfItem *aconf;

	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "auth.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    if (!state)
		send_http_header(cptr, "auth.htm", HTTP_OK);
	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Authentication Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s\" enctype=\"%s\"", H_POST,
				       "auth.hti", "application/x-www-form-urlencoded");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit", "Show",
				       "commbutton");
	    sendto_http(cptr, hBuf);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit",
				       "New Authentication Block", "commbutton");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, HTTP_LINEBREAK);

	    sendto_http(cptr, "Authentication Blocks");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0, "size=\"%d\" name=\"%s\"", 1,
				       "AUTH");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "\r\n");
	    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
		if (aconf->status & CONF_CLIENT) {
		    uuid_unparse(aconf->uuid, strBuf1);
		    enervate_html_object(hBuf, HTTPBUFSIZE, "option", 3, "value=\"%s\"", strBuf1);
		    sendto_http(cptr, hBuf, aconf->user, "@", aconf->host);
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

	}

	else if (irc_strcmp(hurl + 1, "auth.hta") == 0) {

    uuid_t u;
    char id[64];
    aClass *aclass;

	    uuid_generate(u);
	    uuid_unparse(u, id);

	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Operator Connection Setup");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s?id=%s\" enctype=\"%s\"",
				       H_POST, "auth.htc", id, "application/x-www-form-urlencoded");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit",
				       "Commit", "commbutton");
	    sendto_http(cptr, hBuf);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "reset",
				       "Revoke", "resetbutton");
	    sendto_http(cptr, hBuf);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "50%",
				       "Auth");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Userhost for the authentication");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "userhost",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);
	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Password");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "pass", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);
	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection class");
	    http_send_td(cptr, 0, 32);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "select", 0, "size=\"%d\" name=\"%s\"", 1,
				       "class");
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
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Override klines ?");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "EXKL");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "EXKL");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Override limits ?");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "EXLIM");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "EXLIM");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Activate spoof notices ?");
	    http_send_td(cptr, 0, 50);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "radio", "ON",
				       "SPN");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" checked name=\"%s\"", "radio",
				       "OFF", "SPN");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "no");
	    http_send_td(cptr, 1, 0);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Spoof hostname");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "spoof", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);
	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection Port");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "port", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, hBuf);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</form>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
	    sendto_http(cptr, hBuf, "Back to the selection");
	    sendto_http(cptr, "\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);
	    exit_httpd_client(cptr);
	}

	else if (irc_strcmp(hurl + 1, "config.htm") == 0) {


	    if (!(cptr->httpflags & HFLAGS_AUTHENTICATED)) {
		send_http_header(cptr, "config.htm", HTTP_FORBIDDEN);
		exit_httpd_client(cptr);
		return 0;
	    }

	    if (!state)
		send_http_header(cptr, "config.htm", HTTP_OK);
	    sendto_http(cptr, HTTP_PAGE_HEAD, "TR-IRCD Configuration");
	    enervate_html_object(hBuf, HTTPBUFSIZE, "h2", 3, NULL);
	    sendto_http(cptr, hBuf, "TR-IRCD running on", me.name, "Configuration Page");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "form", 0,
				       "method=\"%s\" action=\"%s\" enctype=\"%s\"", H_POST,
				       "configure.hti", "application/x-www-form-urlencoded");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "submit", "Commit",
				       "commbutton");
	    sendto_http(cptr, hBuf);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" name=\"%s\"", "reset", "Revoke",
				       "resetbutton");
	    sendto_http(cptr, hBuf);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "fieldset", 0, "style=\"%s\"",
				       "padding :2");
	    sendto_http(cptr, hBuf);

	    enervate_html_object(hBuf, HTTPBUFSIZE, "legend", 1, NULL);
	    sendto_http(cptr, hBuf, "Server Administration Options");

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "100%",
				       "Admin");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Administrator Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "admin", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, AdminInfo.name);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Administrtion More Text");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "admindesc",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, AdminInfo.description);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Administration Email");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "adminmail",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, AdminInfo.email);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</fieldset>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "fieldset", 0, "style=\"%s\"",
				       "padding :2");
	    sendto_http(cptr, hBuf);

	    enervate_html_object(hBuf, HTTPBUFSIZE, "legend", 1, NULL);
	    sendto_http(cptr, hBuf, "Server Settings");

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "100%",
				       "ServerInfo");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Server Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "servername",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerInfo.name);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Server Description");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "serverdesc",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerInfo.description);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Network Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "netname",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerInfo.networkname);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Network Description");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "netdesc",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerInfo.networkdesc);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Restart Password (Autohashed)");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "restartpass",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24,
			ServerInfo.restartpass ? ServerInfo.restartpass : "NONE");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Die Password (Autohashed)");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "diepass",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerInfo.diepass ? ServerInfo.diepass : "NONE");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Display Password (Autohashed)");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "displaypass",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24,
			ServerInfo.displaypass ? ServerInfo.displaypass : "NONE");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Server identity, must be unique");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "identity",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d", ServerInfo.identity);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50,
			"Server address (defaults to all available, if not specified)");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "saddr", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    if (ServerInfo.specific_ipv4_vhost) {
    char h[HOSTIPLEN + 1];
		inetntop(DEF_FAM, &IN_ADDR(ServerInfo.address), h, HOSTIPLEN);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, h);
	    } else {
		sendto_http(cptr, HTTP_TABLE_ROW, 24, "NONE");
	    }
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50,
			"Server ipv6 address (defaults to all available, if not specified)");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "s6addr", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    if (ServerInfo.specific_ipv6_vhost) {
    char h[HOSTIPLEN + 1];
		inetntop(DEF_FAM, &IN_ADDR(ServerInfo.address6), h, HOSTIPLEN);
		sendto_http(cptr, HTTP_TABLE_ROW, 24, h);
	    } else {
		sendto_http(cptr, HTTP_TABLE_ROW, 24, "NONE");
	    }
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</fieldset>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "fieldset", 0, "style=\"%s\"",
				       "padding :2");
	    sendto_http(cptr, hBuf);

	    enervate_html_object(hBuf, HTTPBUFSIZE, "legend", 1, NULL);
	    sendto_http(cptr, hBuf, "Server Configuration Options");

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "100%",
				       "ServerOpts");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Server Language");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "lang", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24,
			ServerOpts.language ? ServerOpts.language : "default");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Use short motd");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.short_motd ? "checked " : ""), "SHMOTD");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.short_motd ? "" : "checked "), "SHMOTD");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.short_motd ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable operator hostname hide");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.staffhide ? "checked " : ""), "STAFFH");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.staffhide ? "" : "checked "), "STAFFH");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.staffhide ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Server Kline Address");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "skline", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.server_kline_address);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Network Kline Address");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "nkline", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.network_kline_address);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable proxy monitor notices");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.wingate_notice ? "checked " : ""), "WINGATE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.wingate_notice ? "" : "checked "), "WINGATE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.wingate_notice ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Proxy Monitor Host");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "monhost",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.monitor_host);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Proxy Monitor Information URL");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "monurl", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.proxy_url);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Hide realnames with matching sglines");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.hide_gcos_with_sgline ? "checked " : ""),
				       "GECOS");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.hide_gcos_with_sgline ? "" : "checked "),
				       "GECOS");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.hide_gcos_with_sgline ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Realname replacement for matching sglines");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "gecosrep",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.realname_replacement);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Timestamp Warn Delta");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "tswarn", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.ts_warn_delta);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Timestamp Maximum Delta");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "tsmax", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.ts_max_delta);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Default kline time");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "defkline",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.default_kline_time);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Number of connections to be throttled");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "thrcount",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d", ServerOpts.throttle_count);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50,
			"Time to wait before more connections are throttled");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "thrwait",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.throttle_time);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50,
			"Maximum number of bytes allowed to be sent per second");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "clifl", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d bytes", ServerOpts.client_flood);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Delay between two /motd commands");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "motdwait",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.motd_wait);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Number of allowed /motd requests");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "motdmax",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d", ServerOpts.motd_max);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable Anti Nick Flood");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.anti_nick_flood ? "checked " : ""), "ANF");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.anti_nick_flood ? "" : "checked "), "ANF");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.anti_nick_flood ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Delay between two /nick requests");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "maxnicktime",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.max_nick_time);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50,
			"Number of allowed /nick requests in %d seconds: ",
			ServerOpts.max_nick_time);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "maxnickchg",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d", ServerOpts.max_nick_changes);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Disable Away Flood");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.no_away_flood ? "checked " : ""), "NOAWF");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.no_away_flood ? "" : "checked "), "NOAWF");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.no_away_flood ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Delay between two /away requests");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "maxawaytime",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.max_away_time);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Number of allowed /away requests");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "maxawaychg",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d", ServerOpts.max_away_count);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

            http_send_tr(cptr, 0);
            sendto_http(cptr, HTTP_TABLE_ROW, 50, "Delay between two /knock requests");
            enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
                                       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "maxknocktime",
                                       32);
            sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
            ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.max_knock_time);
            sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
            http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50,
			"Time to wait to save max. number of client statistics again");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text",
				       "maxstatstime", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.save_maxclient_stats_time);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable spambot preventions");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.anti_spambot ? "checked " : ""), "SPAMBOT");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.anti_spambot ? "" : "checked "), "SPAMBOT");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.anti_spambot ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Minimum join/part time");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "minjltime",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.min_join_leave_time);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Allowed number of join/parts");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "maxjlcount",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d", ServerOpts.max_join_leave_count);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Join/Part Counter expire time");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text",
				       "jlcountexptime", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.join_leave_count_expire_time);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Oper SpamNotice Count");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text",
				       "operspamcount", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d", ServerOpts.oper_spam_countdown);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable default +R usermode");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.use_registerfilter ? "checked " : ""),
				       "DEFREGF");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.use_registerfilter ? "" : "checked "),
				       "DEFREGF");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.use_registerfilter ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Disable operator Jupiters");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.no_oper_jupiter ? "checked " : ""), "NOOJ");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.no_oper_jupiter ? "" : "checked "), "NOOJ");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.no_oper_jupiter ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable default +x usermode on connect");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.default_fakehost_mode ? "checked " : ""),
				       "DEFX");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.default_fakehost_mode ? "" : "checked "),
				       "DEFX");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.default_fakehost_mode ? "Yes" : "No");
	    http_send_tr(cptr, 1);

            http_send_tr(cptr, 0);
            sendto_http(cptr, HTTP_TABLE_ROW, 50, "Disallow messages when away");
            http_send_td(cptr, 0, 26);
            enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
                                       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
                                       (ServerOpts.no_messages_on_away ? "checked " : ""), "ENOMSGAWAY");
            sendto_http(cptr, hBuf);
            sendto_http(cptr, "Yes");
            enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
                                       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
                                       (ServerOpts.no_messages_on_away ? "" : "checked "), "ENOMSGAWAY");
            sendto_http(cptr, hBuf);
            sendto_http(cptr, "No");
            http_send_td(cptr, 1, 0);
            sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.no_messages_on_away ? "Yes" : "No");
            http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Use the regular expression based ban system");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.use_regex ? "checked " : ""), "REGEX");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.use_regex ? "" : "checked "), "REGEX");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.use_regex ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Complain when identd connection failed");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.identd_complain ? "checked " : ""), "IDCOMP");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.identd_complain ? "" : "checked "), "IDCOMP");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.identd_complain ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Use tilde (~) when identd fails");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerOpts.identd_use_tilde ? "checked " : ""), "IDTILDE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerOpts.identd_use_tilde ? "" : "checked "), "IDTILDE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerOpts.identd_use_tilde ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Identd timeout");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "idtimeizt",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerOpts.identd_timeout);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</fieldset>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "fieldset", 0, "style=\"%s\"",
				       "padding :2");
	    sendto_http(cptr, hBuf);

	    enervate_html_object(hBuf, HTTPBUFSIZE, "legend", 1, NULL);
	    sendto_http(cptr, hBuf, "Services Related Configuration Options");

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "100%",
				       "ServicesConf");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "NickServ Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "ns", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServicesConf.nickserv);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "ChanServ Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "cs", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServicesConf.chanserv);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "MemoServ Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "ms", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServicesConf.memoserv);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "OperServ Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "os", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServicesConf.operserv);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "StatServ Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "ss", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServicesConf.statserv);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "HelpServ Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "hs", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServicesConf.helpserv);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Services Server Name");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text",
				       "servicesname", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServicesConf.services_name);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Hide Ultimate (U:Lined) clients");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServicesConf.hide_ulined_servers ? "checked " : ""),
				       "HULINE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServicesConf.hide_ulined_servers ? "" : "checked "),
				       "HULINE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServicesConf.hide_ulined_servers ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</fieldset>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "fieldset", 0, "style=\"%s\"",
				       "padding :2");
	    sendto_http(cptr, hBuf);

	    enervate_html_object(hBuf, HTTPBUFSIZE, "legend", 1, NULL);
	    sendto_http(cptr, hBuf, "Channel Related Configuration Options");

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "100%",
				       "ChannelConf");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Default +T mode instead of +t in new channels?");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ChannelConf.default_extended_topic_limitation ? "checked " : ""), "DEXT");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ChannelConf.default_extended_topic_limitation ? "" : "checked "), "DEXT");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ChannelConf.default_extended_topic_limitation ? "Yes" : "No");
	    http_send_tr(cptr, 1);

            http_send_tr(cptr, 0);
            sendto_http(cptr, HTTP_TABLE_ROW, 50, "Default +ps mode in server owned local channels?");
            http_send_td(cptr, 0, 26);
            enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
                                       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
                                       (ChannelConf.visschan ? "checked " : ""), "VISS");
            sendto_http(cptr, hBuf);
            sendto_http(cptr, "Yes");
            enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
                                       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
                                       (ChannelConf.visschan ? "" : "checked "), "VISS");
            sendto_http(cptr, hBuf);
            sendto_http(cptr, "No");
            http_send_td(cptr, 1, 0);
            sendto_http(cptr, HTTP_TABLE_ROW, 24, ChannelConf.visschan ? "Yes" : "No");
            http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Max number of channels per user");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "maxchan",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d", ChannelConf.max_channels_per_user);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Default Quit Message Replacement");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "qmsg", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ChannelConf.default_quit_msg);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</fieldset>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "fieldset", 0, "style=\"%s\"",
				       "padding :2");
	    sendto_http(cptr, hBuf);

	    enervate_html_object(hBuf, HTTPBUFSIZE, "legend", 1, NULL);
	    sendto_http(cptr, hBuf, "Server Hiding Configuration Options");

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "100%",
				       "ServerHide");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable Server hiding");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerHide.enable ? "checked " : ""), "SHIDE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerHide.enable ? "" : "checked "), "SHIDE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerHide.enable ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Read /links from file ? (created by the ircd)");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerHide.links_from_file ? "checked " : ""), "LFILE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerHide.links_from_file ? "" : "checked "), "LFILE");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerHide.links_from_file ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Flatten /links output");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerHide.flatten_links ? "checked " : ""), "FLATLINK");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerHide.flatten_links ? "" : "checked "), "FLATLINK");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerHide.flatten_links ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Make /links oper only");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ServerHide.links_oper_only ? "checked " : ""), "LINKSOPER");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ServerHide.links_oper_only ? "" : "checked "), "LINKSOPER");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ServerHide.links_oper_only ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Time to wait to write down the links file");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "linksdelay",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ServerHide.links_delay);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</fieldset>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "fieldset", 0, "style=\"%s\"",
				       "padding :2");
	    sendto_http(cptr, hBuf);

	    enervate_html_object(hBuf, HTTPBUFSIZE, "legend", 1, NULL);
	    sendto_http(cptr, hBuf, "Socks Configuration Options");

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "100%",
				       "Socks");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "File descriptor limit");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "fdlimit",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d", ScannerConf.fd_limit);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "DNS Blacklist server");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "dnsblhost",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%s", ScannerConf.dnsblhost);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Max number of bytes to read");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "maxread",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d", ScannerConf.max_read);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Connection timeout (in seconds)");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "timeout",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d seconds", ScannerConf.timeout);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Log scanner notice output");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (ScannerConf.negfail_notices ? "checked " : ""),
				       "negfail_notices");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (ScannerConf.negfail_notices ? "" : "checked "),
				       "negfail_notices");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, ScannerConf.negfail_notices ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Action to be taken when successful detection");
	    sendto_http(cptr, "<td width=\"50%%\">");
            enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
                                       "type=\"%s\" name=\"%s\" %svalue=\"%s\"", "checkbox",
                                       "PRAK", (ScannerConf.action & PROXYMON_AUTOKILL ? "checked " : ""), "1");
            sendto_http(cptr, hBuf);
            sendto_http(cptr, "Autokill client");
            enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
                                       "type=\"%s\" name=\"%s\" %svalue=\"%s\"", "checkbox",
                                       "PRKL", (ScannerConf.action & PROXYMON_KLINE ? "checked " : ""), "1");
            sendto_http(cptr, hBuf);
            sendto_http(cptr, "Kline");
            enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
                                       "type=\"%s\" name=\"%s\" %svalue=\"%s\"", "checkbox",
                                       "PRGN", (ScannerConf.action & PROXYMON_GNOTICE ? "checked " : ""), "1");
            sendto_http(cptr, hBuf);
            sendto_http(cptr, "Globops to operators");
	    sendto_http(cptr, "</td>");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Address (IP) to connect to through open proxy");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "scan_ip",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%s", ScannerConf.scan_ip);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Port to connect to through open proxy");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "scan_port",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%d", ScannerConf.scan_port);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50,
			"Address (IP) to bind before connecting to open proxy");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "bind_ip",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%s", ScannerConf.bind_ip);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</fieldset>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "fieldset", 0, "style=\"%s\"",
				       "padding :2");
	    sendto_http(cptr, hBuf);

	    enervate_html_object(hBuf, HTTPBUFSIZE, "legend", 1, NULL);
	    sendto_http(cptr, hBuf, "Httpd Configuration Options");

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "100%",
				       "Httpd");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Require index(.html) page");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (HttpdConf.require_index ? "checked " : ""),
				       "require_index");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (HttpdConf.require_index ? "" : "checked "),
				       "require_index");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, HttpdConf.require_index ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Index(.html) page value (if require = yes)");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "index_file",
				       32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%s", HttpdConf.index_file);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Access list global policy");
	    http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (HttpdConf.policy ? "" : "checked "), "policy");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Deny");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (HttpdConf.policy ? "checked " : ""), "policy");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Allow");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, HttpdConf.policy ? "Allow" : "Deny");
	    http_send_tr(cptr, 1);
	    sendto_http(cptr, "</table>\r\n</fieldset>\r\n");
	    sendto_http(cptr, HTTP_LINEBREAK);

            enervate_html_object_begin(hBuf, HTTPBUFSIZE, "fieldset", 0, "style=\"%s\"",
                                       "padding :2");
            sendto_http(cptr, hBuf);

	    enervate_html_object(hBuf, HTTPBUFSIZE, "legend", 1, NULL);
	    sendto_http(cptr, hBuf, "Proxy Configuration Options");

	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "table", 0,
				       "border=\"%d\" cellpadding=\"%d\" cellspacing=\"%d\" style=\"%s\" bordercolor=\"%s\" width=\"%s\" id=\"%s\"",
				       0, 0, 0, "border-collapse: collapse", "#111111", "100%",
				       "Proxy");
	    sendto_http(cptr, hBuf);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Enable Proxy");
	    
            http_send_td(cptr, 0, 26);
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "ON",
				       (GeneralOpts.enable_proxymonitor ? "checked " : ""), "proxy_enable");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "Yes");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" value=\"%s\" %sname=\"%s\"", "radio", "OFF",
				       (GeneralOpts.enable_proxymonitor ? "" : "checked "), "proxy_enable");
	    sendto_http(cptr, hBuf);
	    sendto_http(cptr, "No");
	    http_send_td(cptr, 1, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, GeneralOpts.enable_proxymonitor ? "Yes" : "No");
	    http_send_tr(cptr, 1);

	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Proxy Monitor Module Pathname");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "proxy_modulefile", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%s", GeneralOpts.proxymodulefile);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    
	    http_send_tr(cptr, 0);
	    sendto_http(cptr, HTTP_TABLE_ROW, 50, "Proxy Monitor Configuration File");
	    enervate_html_object_begin(hBuf, HTTPBUFSIZE, "input", 0,
				       "type=\"%s\" name=\"%s\" size=\"%d\"", "text", "proxy_conffile", 32);
	    sendto_http(cptr, HTTP_TABLE_ROW, 26, hBuf);
	    ircsnprintf(strBuf1, 200, "%s", GeneralOpts.proxyconffile);
	    sendto_http(cptr, HTTP_TABLE_ROW, 24, strBuf1);
	    http_send_tr(cptr, 1);

	    sendto_http(cptr, "</table>\r\n</fieldset>\r\n</form>\r\n");

	    sendto_http(cptr, HTTP_LINEBREAK);
	    enervate_html_object(hBuf, HTTPBUFSIZE, "a", 1, "href=\"%s\"", "select.htm");
	    sendto_http(cptr, hBuf, "Back to the selection");
	    sendto_http(cptr, "\r\n");
	    sendto_http(cptr, HTTP_PAGE_FOOT);

	    exit_httpd_client(cptr);
	}

	else {
	    send_http_header(cptr, hurl, HTTP_NOT_FOUND);
	    exit_httpd_client(cptr);
	}

    }
    return 0;
}
