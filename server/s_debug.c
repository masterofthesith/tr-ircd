/************************************************************************
 *   IRC - Internet Relay Chat, server/s_debug.c
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
 * $Id: s_debug.c,v 1.5 2006/01/03 23:57:30 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "blalloc.h"
#include "h.h"
#include "msg.h"
#include "s_conf.h"
#include "zlink.h"
#include "numeric.h"

/*
 * This is part of the STATS replies. There is no offical numeric for
 * this since this isnt an official command, in much the same way as
 * HASH isnt. It is also possible that some systems wont support this
 * call or have different field names for "struct rusage". -avalon
 */
void send_usage(aClient *cptr, char *nick)
{

#if defined( HAVE_GETRUSAGE )
    struct rusage rus;
    time_t secs, rup;

#ifdef	hz
#define hzz hz
#else
#ifdef HZ
#define hzz HZ
#else
    int hzz = 1;

#endif
#endif

    if (getrusage(RUSAGE_SELF, &rus) == -1) {
	send_me_notice(cptr, ":Getrusage error");
	return;
    }
    secs = rus.ru_utime.tv_sec + rus.ru_stime.tv_sec;
    rup = timeofday - me.since;
    if (secs == 0)
	secs = 1;

    send_me_debug(cptr, "CPU Secs %d:%d User %d:%d System %d:%d", secs / 60,
		    secs % 60, rus.ru_utime.tv_sec / 60, rus.ru_utime.tv_sec % 60,
		    rus.ru_stime.tv_sec / 60, rus.ru_stime.tv_sec % 60);
    send_me_debug(cptr, "RSS %d ShMem %d Data %d Stack %d", rus.ru_maxrss,
		    rus.ru_ixrss / (rup * hzz), rus.ru_idrss / (rup * hzz),
		    rus.ru_isrss / (rup * hzz));
    send_me_debug(cptr, "Swaps %d Reclaims %d Faults %d", rus.ru_nswap,
		    rus.ru_minflt, rus.ru_majflt);
    send_me_debug(cptr, "Block in %d out %d", rus.ru_inblock, rus.ru_oublock);
    send_me_debug(cptr, "Msg Rcv %d Send %d", rus.ru_msgrcv, rus.ru_msgsnd);
    send_me_debug(cptr, "Signals %d Context Vol. %d Invol %d", rus.ru_nsignals,
		    rus.ru_nvcsw, rus.ru_nivcsw);

#endif
    return;
}

void count_memory(aClient *cptr, char *nick)
{
    aClient *acptr;
    dlink_node *links;
    aNUH *ap;
    aChannel *chptr;
    aConfItem *aconf;
    aClass *cltmp;
    aMotdItem *amo;

    int lc = 0;			/* local clients */
    int ch = 0;			/* channels */
    int lcc = 0;		/* local client conf links */
    int rc = 0;			/* remote clients */
    int us = 0;			/* user structs */
    int chi = 0;		/* channel invites */
    int chb = 0;		/* channel bans */
    int ce = 0;			/* channel ban exceptions */
    int chci = 0;		/* channel chaninvites */
    int chsm = 0;		/* channel moderated hosts */
    int chcb = 0;		/* channel channel bans */
    int chu = 0;		/* channel member */
    int wwu = 0;		/* whowas users */
    int cl = 0;			/* classes */
    int co = 0;			/* conf lines */
    int usi = 0;		/* users invited */
    int usc = 0;		/* users in channels */
    int usdm = 0;		/* dccallow local */
    int usdr = 0;		/* dccallow remote */
    int uss = 0;		/* silenced users */
    int aw = 0;			/* aways set */

    int linebuf_count = 0;
    u_long linebuf_memory_used = 0;

    u_long chbm = 0;		/* memory used by channel bans */
    u_long chep = 0;		/* memory used by ban exceptions */
    u_long chim = 0;		/* memory used by channel invites */
    u_long chmh = 0;		/* memory used by channel mhosts  */
    u_long chcm = 0;		/* memory used by channel channel bans */
    u_long lcm = 0;		/* memory used by local clients */
    u_long rcm = 0;		/* memory used by remote clients */
    u_long awm = 0;		/* memory used by aways */
    u_long wwm = 0;		/* whowas array memory used */
    u_long com = 0;		/* memory used by conf lines */

    u_long totcl = 0;
    u_long totch = 0;
    u_long totww = 0;
    u_long totmisc = 0;
    u_long tothash = 0;
    u_long tot = 0;

    int wlh = 0, wle = 0;	/* watch headers/entries */
    u_long wlhm = 0;		/* memory used by watch */

    int lcalloc = 0;		/* local clients allocated */
    int rcalloc = 0;		/* remote clients allocated */
    int useralloc = 0;		/* allocated users */
    int linkalloc = 0;		/* allocated links */
    int totallinks = 0;		/* total links used */
    int chanalloc = 0;		/* total channels alloc'd */

    u_long lcallocsz = 0, rcallocsz = 0;	/* size for stuff above */
    u_long userallocsz = 0, linkallocsz = 0, chanallocsz = 0, cmemballocsz = 0;

    int motdlen = 0;

    int servn = 0;

    count_whowas_memory(&wwu, &wwm);
    count_watch_memory(&wlh, &wlhm);
    count_linebuf_memory(&linebuf_count, &linebuf_memory_used);

    lcm = lc * CLIENT_LOCAL_SIZE;
    rcm = rc * CLIENT_REMOTE_SIZE;

    for (acptr = GlobalClientList; acptr; acptr = acptr->next) {

	if (MyConnect(acptr)) {
	    lc++;
	    wle += acptr->watches;
	} else
	    rc++;

	if (acptr->serv) {
	    servn++;
	}

	if (acptr->user) {
	    us++;
	    for (links = acptr->user->invited.head; links; links = links->next)
		usi++;
	    for (links = acptr->user->channel.head; links; links = links->next)
		usc++;
	    for (links = acptr->user->dccallow.head; links; links = links->next) {
		usdm++;
		usdr++;
	    }
	    for (links = acptr->user->silence.head; links; links = links->next)
		uss++;
	    if (acptr->user->away) {
		aw++;
		awm += (strlen(acptr->user->away) + 1);
	    }
	}
    }

    for (chptr = channel; chptr; chptr = chptr->nextch) {
	ch++;

	chu = chptr->users;
	for (links = chptr->invites.head; links; links = links->next)
	    chi++;
	for (links = chptr->banlist.head; links; links = links->next) {
	    ap = links->data;
	    chb++;
	    if (ap)
	    	chbm += (strlen(ap->who) + strlen(ap->nuhstr) + 2 + sizeof(aNUH));
	}
        for (links = chptr->invitelist.head; links; links = links->next) {
            ap = links->data;
            chci++;
            if (ap)
                chim += (strlen(ap->who) + strlen(ap->nuhstr) + 2 + sizeof(aNUH));  
        }
        for (links = chptr->banexlist.head; links; links = links->next) {
            ap = links->data;
            ce++;
            if (ap)
                chep += (strlen(ap->who) + strlen(ap->nuhstr) + 2 + sizeof(aNUH));  
        }
        for (links = chptr->stoplist.head; links; links = links->next) {
            ap = links->data;
            chsm++;
            if (ap)
                chmh += (strlen(ap->who) + strlen(ap->nuhstr) + 2 + sizeof(aNUH));  
        }
        for (links = chptr->chanbanlist.head; links; links = links->next) {
            ap = links->data;
            chcb++;
            if (ap)
                chcm += (strlen(ap->who) + strlen(ap->nuhstr) + 2 + sizeof(aNUH));  
        }
    }

    for (aconf = GlobalConfItemList; aconf; aconf = aconf->next) {
	co++;
	com += aconf->host ? strlen(aconf->host) + 1 : 0;
	com += aconf->passwd ? strlen(aconf->passwd) + 1 : 0;
	com += aconf->name ? strlen(aconf->name) + 1 : 0;
	com += sizeof(aConfItem);
    }

    for (cltmp = ConnectionClasses; cltmp; cltmp = cltmp->next)
	cl++;

    for (amo = (&(GeneralOpts.motd))->content; amo; amo = amo->next)
	motdlen++;
    for (amo = (&(GeneralOpts.shortmotd))->content; amo; amo = amo->next)
	motdlen++;
    for (amo = (&(GeneralOpts.linksfile))->content; amo; amo = amo->next)
	motdlen++;
    for (amo = (&(GeneralOpts.conffile))->content; amo; amo = amo->next)
	motdlen++;

#ifndef NOBALLOC
    lcalloc = free_local_aClients->blocksAllocated * free_local_aClients->elemsPerBlock;
    lcallocsz = lcalloc * free_local_aClients->elemSize;

    rcalloc = free_remote_aClients->blocksAllocated * free_remote_aClients->elemsPerBlock;
    rcallocsz = rcalloc * free_remote_aClients->elemSize;

    chanalloc = free_channels->blocksAllocated * free_channels->elemsPerBlock;
    chanallocsz = chanalloc * free_channels->elemSize;

#endif

    totallinks = lcc + usi + uss + usc + chi + wle + usdm + usdr;

    send_me_debug(cptr, "Memory Use Summary");
    send_me_debug(cptr, "Client usage %d(%d) ALLOC %d(%d)", lc + rc, lcm + rcm,
		    lcalloc + rcalloc, lcallocsz + rcallocsz);
    send_me_debug(cptr, "   Local %d(%d) ALLOC %d(%d)", lc, lcm, lcalloc,
		    lcallocsz);
    send_me_debug(cptr, "   Remote %d(%d) ALLOC %d(%d)", rc, rcm, rcalloc,
		    rcallocsz);
    send_me_debug(cptr, "Users %d(%d) ALLOC %d(%d)", us, us * sizeof(anUser),
		    useralloc, userallocsz);

    totcl = lcallocsz + rcallocsz + userallocsz;

    send_me_debug(cptr, "Links %d(%d) ALLOC %d(%d)", totallinks,
		    totallinks * sizeof(dlink_list), linkalloc, linkallocsz);


    send_me_debug(cptr, "   UserInvites %d(%d) ChanInvites %d(%d)",
		    usi, usi * sizeof(dlink_list), chi, chi * sizeof(dlink_list));
    send_me_debug(cptr, "   UserChannels %d(%d)", usc, usc * sizeof(dlink_list));
    send_me_debug(cptr, "   DCCAllow Local %d(%d) Remote %d(%d)",
		    usdm, usdm * sizeof(dlink_list), usdr, usdr * sizeof(dlink_list));
    send_me_debug(cptr, "   WATCH entries %d(%d)", wle, wle * sizeof(dlink_list));
    send_me_debug(cptr, "   Attached confs %d(%d)", lcc,
		    lcc * sizeof(dlink_list));
    send_me_debug(cptr, "WATCH headers %d(%d)", wlh, wlhm);
    send_me_debug(cptr, "Conflines %d(%d)", co, com);
    send_me_debug(cptr, "Classes %d(%d)", cl, cl * sizeof(aClass));
    send_me_debug(cptr, "Away Messages %d(%d)", aw, awm);
    send_me_debug(cptr, "MOTD structs %d(%d)", motdlen, motdlen * sizeof(aMotd));
    send_me_debug(cptr, "Servers %d(%d)", servn, servn * sizeof(aServer));

    send_me_debug(cptr,
		    "Channels %d(%d) ALLOC %d(%d) Bans %d(%d)", ch,
		    ch * sizeof(aChannel), chanalloc, chanallocsz, chb, chbm);

    totch = chanallocsz + cmemballocsz + chbm;

    send_me_debug(cptr, "Whowas users %d(%d)", wwu, wwu * sizeof(anUser));
    send_me_debug(cptr, "Whowas array %d(%d)", NICKNAMEHISTORYLENGTH, wwm);

    totww = wwu * sizeof(anUser) + wwm;

    send_me_debug(cptr,
		    "Hash: client %d(%d) chan %d(%d) whowas %d(%d) watch %d(%d)", HASHSIZE,
		    sizeof(aHashEntry) * HASHSIZE, HASHSIZE, sizeof(aHashEntry) * HASHSIZE, HASHSIZE,
		    sizeof(aWhowas *) * HASHSIZE, WATCHHASHSIZE, sizeof(aWatch *) * WATCHHASHSIZE);
    send_me_debug(cptr, "Linebuf %d(%d)", linebuf_count,
		    (int) linebuf_memory_used);
    tothash =
	(sizeof(aHashEntry) * HASHSIZE) + (sizeof(aHashEntry) * HASHSIZE) +
	(sizeof(aWatch *) * WATCHHASHSIZE) + (sizeof(aWhowas *) * HASHSIZE);

    tot = totww + totch + totcl + totmisc + tothash + linkallocsz;

    send_me_debug(cptr,
		    "whowas %d chan %d client/user %d misc %d hash %d link %d", totww, totch, totcl,
		    totmisc, tothash, linkallocsz);

    return;
}
