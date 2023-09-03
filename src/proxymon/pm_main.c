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
 * $Id: pm_main.c,v 1.3 2003/06/14 13:55:53 tr-ircd Exp $
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

int pfd = 0;

/* Do not reorder the following two declarations. It is required
 * for the mutex to work 
 * -TimeMr14C
 */

static int initcheck = 0;
unsigned long pm_scan_counter = 0;
pthread_mutex_t pm_mutex;

int pm_shutdown = 0;
int cmd_scanner_active = 0;

/* Parser/Lexer Variables */

extern int pmcount;
extern int pmlineno;
extern char pmlinebuf[];

extern aClient *qcptr;

/* Scanner definitions */

OPM_T *IproxyScan;		/* Internal proxy scanner */
OPM_T *CproxyScan;		/* Command based proxy scanner */

char *protocol_types[8] = {
    "NONE",
    "HTTP",
    "SOCKS4",
    "SOCKS5",
    "WINGATE",
    "ROUTER",
    "HTTPPOST",
    NULL,
};
/* Scanner interaction functions */

void pm_addto_scanner(char *ip)
{
    OPM_REMOTE_T *remote;

    remote = opm_remote_create(ip);

    pthread_mutex_lock(&pm_mutex);
    switch (opm_scan(IproxyScan, remote)) {
	case OPM_SUCCESS:
	    pm_scan_counter++;
	    pthread_cond_signal(&pm_cond);
	    break;
	case OPM_ERR_BADADDR:
	    sendto_lev(PROXY_LEV, "Bad address %s", ip);
	    opm_remote_free(remote);
	default:
	    sendto_lev(PROXY_LEV, "Unknown Error adding %s to the proxy scanner", ip);
	    opm_remote_free(remote);
    }
    pthread_mutex_unlock(&pm_mutex);

    return;

}

void cmd_addto_scanner(char *ip, char *type, int port)
{
    OPM_REMOTE_T *remote;
    int i;

    remote = opm_remote_create(ip);

    if (type && port) {
	for (i = 1; i < 7; i++) {
	    if (irc_strcmp(type, protocol_types[i]) == 0) {
		opm_remote_addtype(remote, i, port);
		break;
	    }
	}
    }

    pthread_mutex_lock(&pm_mutex);
    switch (opm_scan(CproxyScan, remote)) {
	case OPM_SUCCESS:
	    pm_scan_counter++;
	    pthread_cond_signal(&pm_cond);
	    break;
	case OPM_ERR_BADADDR:
	    sendto_lev(PROXY_LEV, "Bad address %s", ip);
	    opm_remote_free(remote);
	default:
	    sendto_lev(PROXY_LEV, "Unknown Error adding %s to the proxy scanner", ip);
	    opm_remote_free(remote);
    }
    pthread_mutex_unlock(&pm_mutex);

    return;

}

int do_add_to_proxy(struct hook_data *thisdata)
{
    struct Client *sptr = thisdata->source_p;

    if (sptr)
    	pm_addto_scanner(sptr->hostip);

    return 0;
}

void read_proxy_configuration(void)
{
    pmconf_fbfile_in = NULL;
    pmcount = 0;
    pmlineno = 0;

    if ((pmconf_fbfile_in = fbopen(GeneralOpts.proxyconffile, "r")) == NULL) {
	logevent_call(LogSys.conferror, GeneralOpts.proxyconffile);
	return;
    }
    proxymonparse();
    fbclose(pmconf_fbfile_in);
}

void reconfigure_proxy_monitor(OPM_T *scanner)
{

    if (ScannerConf.fd_limit)
	opm_config(scanner, OPM_CONFIG_FD_LIMIT, &ScannerConf.fd_limit);

    if (ScannerConf.scan_ip && ScannerConf.scan_port) {
	opm_config(scanner, OPM_CONFIG_SCAN_IP, ((char *) ScannerConf.scan_ip));
	opm_config(scanner, OPM_CONFIG_SCAN_PORT, &ScannerConf.scan_port);
    }

    if (ScannerConf.timeout)
	opm_config(scanner, OPM_CONFIG_TIMEOUT, &ScannerConf.timeout);

    if (ScannerConf.max_read)
	opm_config(scanner, OPM_CONFIG_MAX_READ, &ScannerConf.max_read);

    if (ScannerConf.bind_ip)
	opm_config(scanner, OPM_CONFIG_BIND_IP, ((char *) ScannerConf.bind_ip));

}

static void pm_init_begin(int check)
{
    int fd_limit_back = 256;
    int port_back = 6667;
    int timeout_back = 10;
    int max_read_back = 4096;

    if (check)
	return;

    if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) {
	logevent_call(LogSys.proxyevent, "Unable to set cancel state for proxy monitor thread");
	pthread_exit(0);
    }

    if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0) {
	logevent_call(LogSys.proxyevent, "Unable to set cancel type for proxy monitor thread");
	pthread_exit(0);
    }

    IproxyScan = opm_create();
    CproxyScan = opm_create();

    /* Setup callbacks */
    opm_callback(IproxyScan, OPM_CALLBACK_OPENPROXY, &ircnotice_open_proxy, 0);
    opm_callback(IproxyScan, OPM_CALLBACK_NEGFAIL, &ircnotice_negfail, 0);
    opm_callback(IproxyScan, OPM_CALLBACK_TIMEOUT, &ircnotice_pm_timeout, 0);
    opm_callback(IproxyScan, OPM_CALLBACK_END, &ircnotice_pm_endscan, 0);
    opm_callback(IproxyScan, OPM_CALLBACK_ERROR, &ircnotice_pm_error, 0);

    opm_callback(CproxyScan, OPM_CALLBACK_OPENPROXY, &ircnotice_open_proxy_oper, 0);
    opm_callback(CproxyScan, OPM_CALLBACK_NEGFAIL, &ircnotice_negfail_oper, 0);
    opm_callback(CproxyScan, OPM_CALLBACK_TIMEOUT, &ircnotice_pm_timeout_oper, 0);
    opm_callback(CproxyScan, OPM_CALLBACK_END, &ircnotice_pm_endscan_oper, 0);
    opm_callback(CproxyScan, OPM_CALLBACK_ERROR, &ircnotice_pm_error_oper, 0);

    read_proxy_configuration();

    if (ScannerConf.fd_limit)
	opm_config(IproxyScan, OPM_CONFIG_FD_LIMIT, &ScannerConf.fd_limit);
    else
	opm_config(IproxyScan, OPM_CONFIG_FD_LIMIT, &fd_limit_back);

    if (ScannerConf.scan_ip && ScannerConf.scan_port) {
	opm_config(IproxyScan, OPM_CONFIG_SCAN_IP, ((char *) ScannerConf.scan_ip));
	opm_config(IproxyScan, OPM_CONFIG_SCAN_PORT, &ScannerConf.scan_port);
    } else {
	opm_config(IproxyScan, OPM_CONFIG_SCAN_IP, "127.0.0.1");
	opm_config(IproxyScan, OPM_CONFIG_SCAN_PORT, &port_back);
    }

    opm_config(IproxyScan, OPM_CONFIG_TARGET_STRING, PROXYMON_TARGET_STRING);

    if (ScannerConf.timeout)
	opm_config(IproxyScan, OPM_CONFIG_TIMEOUT, &ScannerConf.timeout);
    else
	opm_config(IproxyScan, OPM_CONFIG_TIMEOUT, &timeout_back);

    if (ScannerConf.max_read)
	opm_config(IproxyScan, OPM_CONFIG_MAX_READ, &ScannerConf.max_read);
    else
	opm_config(IproxyScan, OPM_CONFIG_MAX_READ, &max_read_back);

    if (ScannerConf.bind_ip)
	opm_config(IproxyScan, OPM_CONFIG_BIND_IP, ((char *) ScannerConf.bind_ip));

    if (ScannerConf.fd_limit)
	opm_config(CproxyScan, OPM_CONFIG_FD_LIMIT, &ScannerConf.fd_limit);
    else
	opm_config(CproxyScan, OPM_CONFIG_FD_LIMIT, &fd_limit_back);

    if (ScannerConf.scan_ip && ScannerConf.scan_port) {
	opm_config(CproxyScan, OPM_CONFIG_SCAN_IP, ((char *) ScannerConf.scan_ip));
	opm_config(CproxyScan, OPM_CONFIG_SCAN_PORT, &ScannerConf.scan_port);
    } else {
	opm_config(CproxyScan, OPM_CONFIG_SCAN_IP, "127.0.0.1");
	opm_config(CproxyScan, OPM_CONFIG_SCAN_PORT, &port_back);
    }

    opm_config(CproxyScan, OPM_CONFIG_TARGET_STRING, PROXYMON_TARGET_STRING);

    if (ScannerConf.timeout)
	opm_config(CproxyScan, OPM_CONFIG_TIMEOUT, &ScannerConf.timeout);
    else
	opm_config(CproxyScan, OPM_CONFIG_TIMEOUT, &timeout_back);

    if (ScannerConf.max_read)
	opm_config(CproxyScan, OPM_CONFIG_MAX_READ, &ScannerConf.max_read);
    else
	opm_config(CproxyScan, OPM_CONFIG_MAX_READ, &max_read_back);

    if (ScannerConf.bind_ip)
	opm_config(CproxyScan, OPM_CONFIG_BIND_IP, ((char *) ScannerConf.bind_ip));

    opm_addtype(IproxyScan, OPM_TYPE_HTTP, 8080);
    opm_addtype(IproxyScan, OPM_TYPE_HTTP, 80);
    opm_addtype(IproxyScan, OPM_TYPE_HTTP, 3128);
    opm_addtype(IproxyScan, OPM_TYPE_WINGATE, 23);
    opm_addtype(IproxyScan, OPM_TYPE_ROUTER, 23);
    opm_addtype(IproxyScan, OPM_TYPE_SOCKS4, 1080);
    opm_addtype(IproxyScan, OPM_TYPE_SOCKS5, 1080);

    opm_addtype(CproxyScan, OPM_TYPE_HTTP, 8080);
    opm_addtype(CproxyScan, OPM_TYPE_HTTP, 80);
    opm_addtype(CproxyScan, OPM_TYPE_HTTP, 3128);
    opm_addtype(CproxyScan, OPM_TYPE_WINGATE, 23);
    opm_addtype(CproxyScan, OPM_TYPE_ROUTER, 23);
    opm_addtype(CproxyScan, OPM_TYPE_SOCKS4, 1080);
    opm_addtype(CproxyScan, OPM_TYPE_SOCKS5, 1080);
    opm_addtype(CproxyScan, OPM_TYPE_HTTPPOST, 80);
    opm_addtype(CproxyScan, OPM_TYPE_HTTPPOST, 8090);
    opm_addtype(CproxyScan, OPM_TYPE_HTTPPOST, 3128);

    configure_interproc(&pfd);

    initcheck = 1;
}

void *pm_main(void *value)
{
    pm_init_begin(initcheck);

    while (1) {
	pthread_mutex_lock(&pm_mutex);
	while (pm_scan_counter < 1)
	    pthread_cond_wait(&pm_cond, &pm_mutex);
	opm_cycle(IproxyScan);
	opm_cycle(CproxyScan);

	if (pm_shutdown) {
	    opm_free(IproxyScan);
	    opm_free(CproxyScan);

	    send_interproc_message(pfd, "%s :Exiting proxy scanner thread", TOK1_GLOBOPS);

	    pthread_mutex_lock(&pm_exit_mutex);
	    pm_exited = 1;
	    pthread_cond_signal(&pm_exit_cond);
	    pthread_mutex_unlock(&pm_exit_mutex);
	    pthread_mutex_unlock(&pm_mutex);

	    pthread_exit(0);

	    return NULL;
	}

	pthread_mutex_unlock(&pm_mutex);

    }

    return NULL;

}

