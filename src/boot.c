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
 * $Id: boot.c,v 1.9 2005/12/25 15:18:59 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "event.h"
#include "h.h"
#include "s_conf.h"
#include "logtype.h"
#include "usermode.h"

/*
 * get_vm_top - get the operating systems notion of the resident set size
 */
unsigned long get_vm_top(void)
{
    /*
     * NOTE: sbrk is not part of the ANSI C library or the POSIX.1 standard
     * however it seems that everyone defines it. Calling sbrk with a 0
     * argument will return a pointer to the top of the process virtual
     * memory without changing the process size, so this call should be
     * reasonably safe (sbrk returns the new value for the top of memory).
     * This code relies on the notion that the address returned will be an 
     * offset from 0 (NULL), so the result of sbrk is cast to a size_t and 
     * returned. We really shouldn't be using it here but...
     */
    void *vptr = sbrk(0);
    return (unsigned long) vptr;
}

void recheck_clock(void *unused)
{
    char to_send[200];
    struct timeval newtime;
    newtime.tv_sec = 0;
    newtime.tv_usec = 0;

    if (gettimeofday(&newtime, NULL) == -1) {
	sendto_ops("Clock Failure (%d), TS can be corrupted", errno);
	restart("Clock Failure");
    }

    if (newtime.tv_sec < timeofday) {
	ircsprintf(to_send,
		   "System clock is running backwards - (%lu < %lu)", newtime.tv_sec, timeofday);
	set_back_events(timeofday - newtime.tv_sec);
    }
    timeofday = newtime.tv_sec;
}

int irc_sleep(unsigned long useconds)
{
#ifdef HAVE_NANOSLEEP
    struct timespec t;
    t.tv_sec = useconds / (unsigned long) 1000000;
    t.tv_nsec = (useconds % (unsigned long) 1000000) * 1000;
    return nanosleep(&t, (struct timespec *) NULL);
#else
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = useconds;
    return select(0, NULL, NULL, NULL, &t);
#endif
}

void save_stats(void *unused)
{
    FILE *fp;

    fp = fopen(IRCD_PREFIX_VAR "/maxclients", "w");
    if (fp != NULL) {
	fprintf(fp, "%d %d %li %li %li %ld %ld %ld %ld", Count.max_loc,
		Count.max_tot, Count.weekly, Count.monthly,
		Count.yearly, Count.start, Count.week, Count.month, Count.year);
	fclose(fp);
	sendto_lev(DEBUG_LEV, "Saved maxclient statistics");
    }
}
void write_pidfile(void)
{
    int fd;
    char buff[20];

    if ((fd = open(GeneralOpts.PIDFile, O_CREAT | O_WRONLY, 0600)) >= 0) {
	ircsprintf(buff, "%5d\n", (int) getpid());
	if (write(fd, buff, strlen(buff)) == -1)
	    logevent_call(LogSys.pid, "writing", GeneralOpts.PIDFile);
	close(fd);
	return;
    } else
	logevent_call(LogSys.pid, "opening", GeneralOpts.PIDFile);
}

void init_sys(void)
{
    FILE *mcsfp;

#ifdef RLIMIT_FD_MAX
    struct rlimit limit;

    if (!getrlimit(RLIMIT_FD_MAX, &limit)) {

	if (limit.rlim_max < MAXCONNECTIONS) {
	    fprintf(stderr, "ircd fd table too big\n");
	    fprintf(stderr, "Hard Limit: %ld IRC max: %d\n", (long) limit.rlim_max, MAXCONNECTIONS);
	    fprintf(stderr, "Fix MAXCONNECTIONS\n");
	    exit(-1);
	}

	limit.rlim_cur = limit.rlim_max;	/* make soft limit the max */
	if (setrlimit(RLIMIT_FD_MAX, &limit) == -1) {
	    fprintf(stderr, "error setting max fd's to %ld\n", (long) limit.rlim_cur);
	    exit(-1);
	}
    }
#endif /* RLIMIT_FD_MAX */

    mcsfp = fopen(IRCD_PREFIX_VAR "/maxclients", "r");
    if (mcsfp != NULL) {
	fscanf(mcsfp, "%d %d %li %li %li %ld %ld %ld %ld", &Count.max_loc,
	       &Count.max_tot, &Count.weekly, &Count.monthly,
	       &Count.yearly, &Count.start, &Count.week, &Count.month, &Count.year);
	fclose(mcsfp);
    }

}

/*
 * setup_corefile
 *
 * inputs       - nothing
 * output       - nothing
 * side effects - setups corefile to system limits.
 * -kre
 */
void setup_corefile(void)
{
#if defined(HAVE_SYS_RESOURCE_H)
    struct rlimit rlim;		/* resource limits */

    /* Set corefilesize to maximum */
    if (!getrlimit(RLIMIT_CORE, &rlim)) {
	rlim.rlim_cur = rlim.rlim_max;
	setrlimit(RLIMIT_CORE, &rlim);
    }
#endif
}

void initialize_generalopts(void)
{
    GeneralOpts.debuglevel = 0;
    GeneralOpts.foreground = 0;
    GeneralOpts.maxclients = MAXCONNECTIONS;
    GeneralOpts.protocol_in_use = 0;
    GeneralOpts.smallnet = 0;
    GeneralOpts.spam_num = 0;
    GeneralOpts.spam_time = 0;
    GeneralOpts.split = 0;

    GeneralOpts.dpath = IRCD_PREFIX;
    GeneralOpts.configfile = CONFIGFILE;
    GeneralOpts.proxyconffile = NULL;	/* :P */
}

/*
 * cleanup_zombies
 * inputs       - nothing
 * output       - nothing
 * side effects - Reaps zombies periodically
 * -AndroSyn
 */
void cleanup_zombies(void *unused)
{
    int status;
    waitpid(-1, &status, WNOHANG);
}

int make_daemon(int state)
{
    int pid;
    if (state == 0) {
	printf("Initializing TR-IRCD5...\n");
    }
    if (state == 1) {
	printf("TR-IRCD5 is now becoming a daemon\n");
	if (GeneralOpts.foreground == 0 && GeneralOpts.webconfig == 0) {
	    if ((pid = fork()) < 0) {
		perror("fork");
		exit(-1);
	    } else if (pid > 0) {
		exit(EXIT_SUCCESS);
	    }
	}
	setsid();
        if (GeneralOpts.foreground == 0 && GeneralOpts.webconfig == 0) {
            if ((pid = fork()) < 0) {
                perror("fork");
                exit(-1);
            } else if (pid > 0) {
                exit(EXIT_SUCCESS);
            }
        }
    }
    if (state == 2) {
        if (chdir(GeneralOpts.dpath)) {
            perror("chdir");
            fprintf(stderr,  
                    "Please ensure that your configuration directory exists and is accessable.\n");
            exit(-1);
        }
	umask(0);
    }
    if (state == 8) {
	close(0);
	close(1);
	close(2);
	open("/dev/null", O_NONBLOCK);
	open("/dev/null", O_NONBLOCK);
	open("/dev/null", O_NONBLOCK);
    }
    return 0;
}

void create_modelists(void *unused)
{
    if (GeneralOpts.lists_created)
	return;
    create_channelmodelists();
    create_usermodelist();
    GeneralOpts.lists_created = 1;
}
