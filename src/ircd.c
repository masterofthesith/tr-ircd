
/************************************************************************
 *   IRC - Internet Relay Chat, src/ircd.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
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
 * $Id: ircd.c,v 1.10 2004/02/24 15:00:30 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "channel.h"
#include "msg.h"
#include "h.h"
#include "optparse.h"
#include "dh.h"
#include "hook.h"
#include "tools.h"
#include "s_auth.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "event.h"
#include "modules.h"
#include "language.h"
#include "throttle.h"
#include "comply.h"
#include "packet.h"
#include "usermode.h"
#include "interproc.h"

static unsigned long initialVMTop = 0;	/* top of virtual memory at init */

aClient me;			/* That's me */
aClient *GlobalClientList;	/* Pointer to beginning of Client list */

struct Counter Count;

aConfItem *GlobalConfItemList;	/* global confitem beginning */

int dorehash = 0;
int rehashed = 1;

int callbacks_called = 0;	/* A measure of server load... */

time_t NOW;

void restart(char *);
static void io_loop(void);

char **myargv;

dlink_list lclient_list;
dlink_list serv_list;
dlink_list global_serv_list;
dlink_list unknown_list;
dlink_list locoper_list;
dlink_list hclient_list;
dlink_list http_except_list;
dlink_list exit_list;

struct server_options ServerOpts;
struct server_info ServerInfo;
struct services_conf ServicesConf;
struct admin_info AdminInfo;
struct channel_conf ChannelConf;
struct server_hide ServerHide;
struct general_options GeneralOpts;
struct log_system LogSys;
struct scanner_conf ScannerConf;
struct httpd_conf HttpdConf;

int main(int argc, char *argv[])
{
    build_version();

    Count.server = 1;		/* it's just me */
    Count.start = NOW;
    Count.day = NOW;
    Count.week = NOW;
    Count.month = NOW;
    Count.year = NOW;

    /*
     * this code by mika@cs.caltech.edu 
     * it is intended to keep the ircd from being swapped out. BSD
     * swapping criteria do not match the requirements of ircd
     */

    myargv = argv;
    (void) umask(077);		/* better safe than sorry --SRB */

    memset(&me, 0, sizeof(me));

    memset(&unknown_list, 0, sizeof(unknown_list));
    memset(&lclient_list, 0, sizeof(lclient_list));
    memset(&serv_list, 0, sizeof(serv_list));
    memset(&global_serv_list, 0, sizeof(global_serv_list));
    memset(&locoper_list, 0, sizeof(locoper_list));
    memset(&hclient_list, 0, sizeof(hclient_list));
    memset(&http_except_list, 0, sizeof(http_except_list));
    memset(&exit_list, 0, sizeof(exit_list));

    unknown_list.head = unknown_list.tail = NULL;
    lclient_list.head = lclient_list.tail = NULL;
    serv_list.head = serv_list.tail = NULL;
    global_serv_list.head = global_serv_list.tail = NULL;
    locoper_list.head = locoper_list.tail = NULL;
    hclient_list.head = hclient_list.tail = NULL;
    http_except_list.head = http_except_list.tail = NULL;
    exit_list.head = exit_list.tail = NULL;

    memset((void *) &ServerInfo, 0, sizeof(ServerInfo));
    memset((void *) &AdminInfo, 0, sizeof(AdminInfo));
    memset((void *) &ServerOpts, 0, sizeof(ServerOpts));
    memset((void *) &ChannelConf, 0, sizeof(ChannelConf));
    memset((void *) &ServicesConf, 0, sizeof(ServicesConf));
    memset((void *) &GeneralOpts, 0, sizeof(GeneralOpts));
    memset((void *) &LogSys, 0, sizeof(LogSys));
    memset((void *) &ScannerConf, 0, sizeof(ScannerConf));
    memset((void *) &HttpdConf, 0, sizeof(HttpdConf));

    GlobalClientList = &me;	/* Pointer to beginning of Client list */

    setup_corefile();

    /*
     * set initialVMTop before we allocate any memory
     */
    initialVMTop = get_vm_top();

    initialize_generalopts();

    init_optparse(NULL);

    process_commandline(argc, argv);

    init_sys();
    make_daemon(0);

#ifdef HAVE_ENCRYPTION_ON
    if (dh_init() == -1)
        exit(0);
    ircd_init_ssl();            /* Merged ssl initialization */
#endif

    make_daemon(1);

    setup_signals();

    clear_client_hash_table();
    clear_channel_hash_table();

    recheck_clock(NULL);

    fdlist_init();

    initBlockHeap();
    init_dlink_nodes();
    linebuf_init();

    init_logsystem(NULL);	/* Log System initialize */
    init_logevents(NULL);	/* Log Events initialize */
    init_log_elems(NULL);
    init_log_files(NULL);	/* Log Files opening */

    initlists();
    initclass();
    initwhowas();
    initstats();

    init_netio();		/* This needs to be setup early ! -- adrian */
    init_resolver();		/* Needs to be setup before the io loop */
    init_hooks();
    init_dopacket(NULL);
    init_bsd();	
    init_send();

    clear_hash_parse();
    clear_hash_lang();
    init_channel(NULL);
    init_chanmodes(NULL);

    init_maskitem();		/* initialise maskitem arrays */
    init_auth();		/* Initialise the auth code */
    eventInit();		/* Initialise event manager */

    init_modules();		/* Initialise modules */

    init_message_file(MF_MOTD, MOTD, &(GeneralOpts.motd));
    init_message_file(MF_SHORTMOTD, SHORTMOTD, &(GeneralOpts.shortmotd));
    init_message_file(MF_LINKS, LINKSFILE, &(GeneralOpts.linksfile));
    init_message_file(MF_CONF, CONFIGFILE, &(GeneralOpts.conffile));

    read_message_file(&(GeneralOpts.motd));
    read_message_file(&(GeneralOpts.linksfile));
    read_message_file(&(GeneralOpts.shortmotd));
    read_message_file(&(GeneralOpts.conffile));

    init_user();
    init_server();
    init_interproc();

    check_class();

    make_daemon(2);

    init_loader(NULL);

    NOW = time(NULL);

    if (GeneralOpts.webconfig == 0) {
	read_conf_files(1);

	load_all_modules(LM_MODULES | LM_LANG, 0);

	if (!init_protocol(0)) {
	    logevent_call(LogSys.generalerror, "Failed to initialize protocol module");
	    logevent_call(LogSys.generalerror, "Server is exiting...");
	    exit(-1);
	}

	GeneralOpts.protocol_in_use = protocol_rehash();

	load_all_modules(LM_CHANMODES | LM_CONTRIB, 0);

	make_daemon(3);

	GeneralOpts.spam_num = ServerOpts.max_join_leave_count;
	GeneralOpts.spam_time = ServerOpts.min_join_leave_time;

	throttle_init();    /* init the throttle system -wd */

	/* Config file validation is in read_conf_files
	 * that way we do not need to check items here again */

	strlcpy_irc(me.name, ServerInfo.name, HOSTLEN);

	me.fd = -1;
	me.hopcount = 0;
	me.authfd = -1;
	me.confs.head = NULL;
	me.next = NULL;
	me.user = NULL;
	me.from = &me;
	me.servptr = &me;

	SetMe(&me);
	make_server(&me);

	make_user(&me);		/* -TimeMr14C */

	me.lasttime = me.since = me.firsttime = NOW;
	DupString(me.user->server, me.name);
	strlcpy_irc(me.user->username, "local", sizeof(me.user->username));
	strlcpy_irc(me.user->host, me.name, HOSTLEN);

	add_to_client_hash_table(me.name, &me);
	add_server_to_list(&me);	/* add ourselves to global_serv_list */

	init_levs(&me);

	if (init_identity(ServerInfo.identity ? ServerInfo.identity : 0xFFFF))
	    logevent_call(LogSys.generalerror, "Invalid identity specified.");

	logevent_call(LogSys.generalerror, "Server ready...");

	NOW = time(NULL);

	eventAddIsh("Check connections", try_connections, NULL, 60);
	eventAddIsh("Check timeouts", comm_checktimeouts, NULL, 1);
	eventAddIsh("Check pings", check_pings, NULL, 10);
	eventAddIsh("Delete throttles", throttle_timer_zapped, NULL, THROTTLE_LENGTH);
	eventAddIsh("cleanup_zombies", cleanup_zombies, NULL, 30);
	eventAddIsh("Flush server connections", flush_connections, NULL, 1);
	eventAddIsh("Save stats", save_stats, NULL, ServerOpts.save_maxclient_stats_time);
	eventAddIsh("Garbage collection", block_garbage_collect, NULL, 30);
	eventAddIsh("check splitmode", check_splitmode, NULL, 60);
	eventAddIsh("create modelists", create_modelists, NULL, 1);
	eventAddIsh("remove exited clients", remove_exited_clients, NULL, 30);

	if (ServerHide.links_from_file)
	    eventAddIsh("write_links_file", write_links_file, NULL, ServerHide.links_delay);

    } else if (GeneralOpts.webconfig == 1) {
        ServerOpts.throttle_count = 100000;
        ServerOpts.throttle_time = 1;
	throttle_init();    /* init the throttle system -wd */
#ifndef STATIC_MODULES
	enqueue_one_module_to_load(TOOLMODPATH "/httpd.so", MODULE_HTTPDMODULE);
#endif
	make_daemon(3);
    }

    write_pidfile();

    io_loop();

    return 0;
}

static void io_loop(void)
{
    int delay = 0, empty_cycles = 0, st = 0;
    while (1) {

	delay = eventNextTime();
	if (delay <= timeofday)
	    eventRun();
	NOW = timeofday;

	if (callbacks_called > 0)
	    empty_cycles = 0;

	else
	    empty_cycles++;

	/* Reset the callback counter... */
	callbacks_called = 0;
	st = (empty_cycles + 1) * 15000;
	if (st > 250000)
	    st = 250000;

	irc_sleep(st);

	dns_select();

	comm_select(0, &callbacks_called, FDLIST_IRCD);

	if (dorehash) {
	    rehash(&me, &me, 1);
	    dorehash = 0;
	}

    }
}
