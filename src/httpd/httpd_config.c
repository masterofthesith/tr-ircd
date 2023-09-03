/************************************************************************
 *   IRC - Internet Relay Chat, src/httpd/httpd_config.c
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
 * $Id: httpd_config.c,v 1.3 2003/06/14 13:55:53 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "confitem.h"
#include "channel.h"
#include "s_conf.h"
#include "listener.h"
#include "h.h"
#include "httpd.h"
#include "http.h"

extern int hcount;
extern int hlineno;

void configure_httpd(void)
{
    /* set default values */
    uuid_t u;
    char ca[64], out[PASSWDLEN];

    if (GeneralOpts.webconfig == 0) {
	HttpdConf.require_index = 1;
	HttpdConf.index_file_len = 9;
	HttpdConf.policy = -1;

	DupString(HttpdConf.index_file, DEF_HTTPD_INDEX);

	/* Parse */

	httpd_fbfile_in = NULL;
	hcount = 0;
	hlineno = 0;

	if ((httpd_fbfile_in = fbopen(GeneralOpts.httpdconffile, "r")) == NULL) {
	    logevent_call(LogSys.conferror, GeneralOpts.httpdconffile);
	    return;
	}
	httpdparse();
	fbclose(httpd_fbfile_in);

    } else {
	uuid_generate(u);
	memset(ca, 0, 64 * sizeof(char));
	uuid_unparse(u, ca);
	HttpdConf.index_file_len = 9;
	HttpdConf.policy = -1;
	DupString(HttpdConf.index_file, DEF_HTTPD_INDEX);
	add_listener(GeneralOpts.webconfigport, GeneralOpts.webconfigbind, LISTENER_HTTP, u, NULL);
	calcpass(ca, out);
	DupString(GeneralOpts.webconfigpass, out);
	fprintf(stderr, "Web Configuration Only mode:\nusername: admin\npassword: %s\n", out);	
    }
}

void configure_httpd_errors(void)
{
    herrortab[100] = "Continue";
    herrortab[101] = "Switchting Protocols";
    herrortab[200] = "OK";
    herrortab[201] = "Created";
    herrortab[202] = "Accepted";
    herrortab[203] = "Non-Authoritative Information";
    herrortab[204] = "No Content";
    herrortab[205] = "Reset Content";
    herrortab[206] = "Partial Content";
    herrortab[300] = "Multiple Choices";
    herrortab[301] = "Moved Permanently";
    herrortab[302] = "Found";
    herrortab[303] = "See Other";
    herrortab[304] = "Not Modified";
    herrortab[305] = "Use Proxy";
    herrortab[307] = "Temporary Redirect";
    herrortab[400] = "Bad Request";
    herrortab[401] = "Unauthorized";
    herrortab[402] = "Payment Required";
    herrortab[403] = "Forbidden";
    herrortab[404] = "Not found";
    herrortab[405] = "Method Not Allowed";
    herrortab[406] = "Not Acceptable";
    herrortab[407] = "Proxy Authentication Required";
    herrortab[408] = "Request Time-out";
    herrortab[409] = "Conflict";
    herrortab[410] = "Gone";
    herrortab[411] = "Length Required";
    herrortab[412] = "Precondition Failed";
    herrortab[413] = "Request Entity Too Large";
    herrortab[414] = "Request-URI Too Large";
    herrortab[415] = "Unsupported Media Type";
    herrortab[416] = "Requested range not satisfiable";
    herrortab[417] = "Expectation Failed";
    herrortab[500] = "Internal Server Error";
    herrortab[501] = "Not Implemented";
    herrortab[502] = "Bad Gateway";
    herrortab[503] = "Service Unavailable";
    herrortab[504] = "Gateway Time-out";
    herrortab[505] = "HTTP Version not supported";

}
