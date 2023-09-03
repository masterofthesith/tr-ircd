/************************************************************************
 *   IRC - Internet Relay Chat, server/s_signal.c
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
 * $Id: s_signal.c,v 1.4 2004/02/24 19:03:33 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "s_conf.h"
#include "interproc.h"

void sigterm_handler(int sig);


/*   
 * dummy_handler - don't know if this is really needed but if alarm is still
 * being used we probably will
 */
static void dummy_handler(int sig)
{
    /* Empty */
}

void sigterm_handler(int sig)
{
    FILE *fp;
    flush_connections(NULL);
    logevent_call(LogSys.serverkill, "SIGTERM");
    fp = fopen(IRCD_PREFIX_VAR "/maxclients", "w");
    if (fp != NULL) {
        fprintf(fp, "%d %d %li %li %li %ld %ld %ld %ld", Count.max_loc,
 	    Count.max_tot, Count.weekly, Count.monthly,
	    Count.yearly, Count.start, Count.week, Count.month, Count.year);
        fclose(fp);
    }
    terminate_interproc();
    exit(0);
}

static void sighup_handler(int sig)
{
    rehash_maskitems(MASKITEM_KLINE_CONFIG);
    rehash_maskitems(MASKITEM_QUARANTINE_CONFIG);
    rehash_maskitems(MASKITEM_GECOS_CONFIG);
    rehash_maskitems(MASKITEM_ZAPLINE_CONFIG);
    rehash_maskitems(MASKITEM_JUPITER_CONFIG);
    dorehash = 1;
}

static void sigint_handler(int sig)
{
    static int restarting = 0;

    logevent_call(LogSys.serverkill, "SIGINT");
    if (restarting == 0) {
	restarting = 1;
	server_reboot();
    }
}

/*
 * setup_signals - initialize signal handlers for server
 */

void setup_signals()
{
struct sigaction act;

    act.sa_flags = 0;
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGPIPE);
    sigaddset(&act.sa_mask, SIGALRM);

# ifdef SIGWINCH
    sigaddset(&act.sa_mask, SIGWINCH);
    sigaction(SIGWINCH, &act, 0);
# endif
    act.sa_handler = dummy_handler;
    sigaction(SIGPIPE, &act, 0);

    act.sa_handler = dummy_handler;
    sigaction(SIGALRM, &act, 0);

    act.sa_handler = sighup_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGHUP);
    sigaction(SIGHUP, &act, 0);

    act.sa_handler = sigint_handler;
    sigaddset(&act.sa_mask, SIGINT);
    sigaction(SIGINT, &act, 0);

    act.sa_handler = sigterm_handler;
    sigaddset(&act.sa_mask, SIGTERM);
    sigaction(SIGTERM, &act, 0);
#ifdef USE_SIGIO
    act.sa_handler = do_sigio;
    sigaddset(&act.sa_mask, SIGIO);
    sigaction(SIGIO, &act, 0);
#endif

}
