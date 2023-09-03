/************************************************************************
 *   IRC - Internet Relay Chat, src/proxymon/pm_main.c
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
 * $Id: pm_callbacks.c,v 1.3 2003/06/14 13:55:53 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "modules.h"
#include "msg.h"
#include "numeric.h"
#include "proxy.h"
#include "h.h"
#include "logtype.h"
#include "s_auth.h"
#include "s_conf.h"
#include "interproc.h"

/* This array will be used when referencing to the different proxy
 * counter types
 */

static unsigned long proxy_counter_array[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void ircnotice_open_proxy(OPM_T *IproxyScan, OPM_REMOTE_T *remote, int notused, void *unused)
{
    int i;
    char buf[BUFSIZE];

    ircsnprintf(buf, BUFSIZE, "Detected open proxy type %s from %s port %d. (%d bytes read)",
		protocol_types[(remote->protocol)], remote->ip, remote->port, remote->bytes_read);
    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
    logevent_call(LogSys.proxyevent, buf);
    i = proxy_counter_array[remote->protocol];
    i++;
    proxy_counter_array[remote->protocol] = i;
    pm_action(remote->ip, remote->port, protocol_types[(remote->protocol)], ScannerConf.action);
}

void ircnotice_open_proxy_oper(OPM_T *IproxyScan, OPM_REMOTE_T *remote, int notused, void *unused)
{
    int i;
    char buf[BUFSIZE];

    ircsnprintf(buf, BUFSIZE, "Detected open proxy type %s from %s port %d. (%d bytes read)",
		protocol_types[(remote->protocol)], remote->ip, remote->port, remote->bytes_read);
    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
    logevent_call(LogSys.proxyevent, buf);
    send_interproc_numeric(pfd, RPL_PROXY_DETECT, qcptr, protocol_types[(remote->protocol)], remote->ip,
		    remote->port, remote->bytes_read);
    i = proxy_counter_array[remote->protocol];
    i++;
    proxy_counter_array[remote->protocol] = i;
}

void ircnotice_negfail(OPM_T *IproxyScan, OPM_REMOTE_T *remote, int notused, void *unused)
{
    if (ScannerConf.negfail_notices > 0) {
    char buf[BUFSIZE];
	ircsnprintf(buf, BUFSIZE,
		    "Negotiation failed on %s from port %d. (%d bytes read). No %s proxy found.",
		    remote->ip, remote->port, remote->bytes_read,
		    protocol_types[(remote->protocol)]);
	logevent_call(LogSys.proxyevent, buf);
	if (ScannerConf.negfail_notices > 1)
	    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
    }
}

void ircnotice_negfail_oper(OPM_T *IproxyScan, OPM_REMOTE_T *remote, int notused, void *unused)
{
    char buf[BUFSIZE];
    ircsnprintf(buf, BUFSIZE,
		"Negotiation failed on %s from port %d. (%d bytes read). No %s proxy found.",
		remote->ip, remote->port, remote->bytes_read, protocol_types[(remote->protocol)]);
    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
    logevent_call(LogSys.proxyevent, buf);
    send_interproc_numeric(pfd, RPL_PROXY_NEGFAIL, qcptr, remote->ip, remote->port,
		    protocol_types[(remote->protocol)]);
}

void ircnotice_pm_timeout(OPM_T *IproxyScan, OPM_REMOTE_T *remote, int notused, void *unused)
{
    if (ScannerConf.negfail_notices > 0) {
    char buf[BUFSIZE];
	ircsnprintf(buf, BUFSIZE, "Negotiation failed on %s port %d. Connection timed out",
		    remote->ip, remote->port);
	logevent_call(LogSys.proxyevent, buf);
	if (ScannerConf.negfail_notices > 1)
	    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
    }
}

void ircnotice_pm_timeout_oper(OPM_T *IproxyScan, OPM_REMOTE_T *remote, int notused, void *unused)
{
    char buf[BUFSIZE];

    ircsnprintf(buf, BUFSIZE, "Negotiation failed on %s port %d. Connection timed out", remote->ip,
		remote->port);
    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
    logevent_call(LogSys.proxyevent, buf);
    send_interproc_numeric(pfd, RPL_PROXY_TIMEOUT, qcptr, remote->ip, remote->port);
}

void ircnotice_pm_endscan(OPM_T *IproxyScan, OPM_REMOTE_T *remote, int notused, void *unused)
{
    if (ScannerConf.negfail_notices > 0) {
    char buf[BUFSIZE];
	ircsnprintf(buf, BUFSIZE, "Proxy scan on %s ended.", remote->ip);
	logevent_call(LogSys.proxyevent, buf);
	if (ScannerConf.negfail_notices > 1)
	    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
    }
    pm_scan_counter--;
    opm_remote_free(remote);
}

void ircnotice_pm_endscan_oper(OPM_T *IproxyScan, OPM_REMOTE_T *remote, int notused, void *unused)
{
    char buf[BUFSIZE];
    ircsnprintf(buf, BUFSIZE, "Proxy scan on %s ended.", remote->ip);
    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
    logevent_call(LogSys.proxyevent, buf);
    send_interproc_numeric(pfd, RPL_PROXY_END, qcptr, remote->ip);
    pm_scan_counter--;
    cmd_scanner_active = 0;
    opm_remote_free(remote);
}

void ircnotice_pm_error(OPM_T *IproxyScan, OPM_REMOTE_T *remote, int err, void *unused)
{
    char buf[BUFSIZE];

    switch (err) {
	case OPM_ERR_MAX_READ:
	    ircsnprintf(buf, BUFSIZE, "Reached MAX READ while scanning %s on port %d",
			remote->ip, remote->port);
	    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
	    logevent_call(LogSys.proxyevent, buf);
	    break;
	case OPM_ERR_BIND:
	    ircsnprintf(buf, BUFSIZE, "Unable to bind to address %s to scan %s on port %d",
			(char *) &ScannerConf.bind_ip, remote->ip, remote->port);
	    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
	    logevent_call(LogSys.proxyevent, buf);
	    break;
	case OPM_ERR_NOFD:
	    ircsnprintf(buf, BUFSIZE,
			"Unable to allocate a new file descriptor to scan %s on port %d",
			remote->ip, remote->port);
	    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
	    logevent_call(LogSys.proxyevent, buf);
	    break;
	default:
	    ircsnprintf(buf, BUFSIZE, "Got unknown error while scanning %s on port %d",
			remote->ip, remote->port);
	    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
	    logevent_call(LogSys.proxyevent, buf);
    }
    pm_scan_counter--;
    opm_remote_free(remote);
}

void ircnotice_pm_error_oper(OPM_T *IproxyScan, OPM_REMOTE_T *remote, int err, void *unused)
{
    char buf[BUFSIZE];

    send_interproc_numeric(pfd, ERR_PROXY_ERROR, qcptr, "Proxy Monitor error");
    switch (err) {
	case OPM_ERR_MAX_READ:
	    ircsnprintf(buf, BUFSIZE, "Reached MAX READ while scanning %s on port %d",
			remote->ip, remote->port);
	    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
	    logevent_call(LogSys.proxyevent, buf);
	    send_interproc_numeric(pfd, ERR_PROXY_MAX_READ, qcptr, remote->ip, remote->port);
	    break;
	case OPM_ERR_BIND:
	    ircsnprintf(buf, BUFSIZE, "Unable to bind to address %s to scan %s on port %d",
			ScannerConf.bind_ip, remote->ip, remote->port);
	    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
	    logevent_call(LogSys.proxyevent, buf);
	    send_interproc_numeric(pfd, ERR_PROXY_NOT_BIND, qcptr, ScannerConf.bind_ip, remote->ip,
			    remote->port);
	    break;
	case OPM_ERR_NOFD:
	    ircsnprintf(buf, BUFSIZE,
			"Unable to allocate a new file descriptor to scan %s on port %d",
			remote->ip, remote->port);
	    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
	    logevent_call(LogSys.proxyevent, buf);
	    send_interproc_numeric(pfd, ERR_PROXY_NO_FDS, qcptr, remote->ip, remote->port);
	    break;
	default:
	    ircsnprintf(buf, BUFSIZE, "Got unknown error while scanning %s on port %d",
			remote->ip, remote->port);
	    send_interproc_message(pfd, "%s %s :%s", TOK1_NOTICE, "&PROXYMON", buf);
	    logevent_call(LogSys.proxyevent, buf);
	    send_interproc_numeric(pfd, ERR_PROXY_UNKNOWN, qcptr, remote->ip, remote->port);
    }
    pm_scan_counter--;
    cmd_scanner_active = 0;
    opm_remote_free(remote);
}

void display_proxy_scanner_statistics(aClient *cptr)
{
    send_interproc_numeric(pfd, RPL_PROXYSTATSTEXT, cptr, "Proxy Scanner Statistics:");
    send_interproc_numeric(pfd, RPL_PROXYSTATS, cptr, proxy_counter_array[OPM_TYPE_HTTP], "http");
    send_interproc_numeric(pfd, RPL_PROXYSTATS, cptr, proxy_counter_array[OPM_TYPE_WINGATE], "WinGate");
    send_interproc_numeric(pfd, RPL_PROXYSTATS, cptr, proxy_counter_array[OPM_TYPE_ROUTER], "Cisco Router");
    send_interproc_numeric(pfd, RPL_PROXYSTATS, cptr, proxy_counter_array[OPM_TYPE_SOCKS4], "Socks-V4");
    send_interproc_numeric(pfd, RPL_PROXYSTATS, cptr, proxy_counter_array[OPM_TYPE_SOCKS5], "Socks-V5");
    send_interproc_numeric(pfd, RPL_PROXYSTATS, cptr, proxy_counter_array[OPM_TYPE_HTTPPOST], "http POST");
    send_interproc_numeric(pfd, RPL_PROXYSTATSTEXT, cptr, "End of Proxy Scanner Statistics");
}
