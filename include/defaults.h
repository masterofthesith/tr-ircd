/*
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
 *
 */

/*
 * NICKNAMEHISTORYLENGTH - size of WHOWAS array this defines the length
 * of the nickname history.  each time a user changes nickname or signs
 * off, their old nickname is added to the top of the list. NOTE: this
 * is directly related to the amount of memory ircd will use whilst
 * resident and running - it hardly ever gets swapped to disk!  Memory
 * will be preallocated for the entire whowas array when ircd is
 * started.
 */
#define NICKNAMEHISTORYLENGTH 2048

/*
 * PINGFREQUENCY - ping frequency for idle connections If daemon
 * doesn't receive anything from any of its links within PINGFREQUENCY
 * seconds, then the server will attempt to check for an active link
 * with a PING message. If no reply is received within (PINGFREQUENCY *
 * 2) seconds, then the connection will be closed.
 */
#define PINGFREQUENCY    120	/* Recommended value: 120 */

/*
 * CONNECTFREQUENCY - time to wait before auto-reconencting If the
 * connection to to uphost is down, then attempt to reconnect every
 * CONNECTFREQUENCY  seconds.
 */
#define CONNECTFREQUENCY 600	/* Recommended value: 600 */

/*
 * HANGONGOODLINK and HANGONGOODLINK Often net breaks for a short time
 * and it's useful to try to establishing the same connection again
 * faster than CONNECTFREQUENCY would allow. But, to keep trying on bad
 * connection, we require that connection has been open for certain
 * minimum time (HANGONGOODLINK) and we give the net few seconds to
 * steady (HANGONRETRYDELAY). This latter has to be long enough that
 * the other end of the connection has time to notice it broke too.
 * 1997/09/18 recommended values by ThemBones for modern Efnet
 */

#define HANGONRETRYDELAY 60	/* Recommended value: 30-60 seconds */
#define HANGONGOODLINK 3600	/* Recommended value: 30-60 minutes */

/*
 * CONNECTTIMEOUT - Number of seconds to wait for a connect(2) call to
 * complete. NOTE: this must be at *LEAST* 10.  When a client connects,
 * it has CONNECTTIMEOUT - 10 seconds for its host to respond to an
 * ident lookup query and for a DNS answer to be retrieved.
 */
#define	CONNECTTIMEOUT	30	/* Recommended value: 30 */

/*
 * KILLCHASETIMELIMIT - Max time from the nickname change that still
 * causes KILL automaticly to switch for the current nick of that user.
 * (seconds)
 */
#define KILLCHASETIMELIMIT 90	/* Recommended value: 90 */
#define CLEANUP_TKLINES_TIME 60
#define MAXIMUM_LINKS 1

/*
 * If the OS has SOMAXCONN use that value, otherwise Use the value in
 * HYBRID_SOMAXCONN for the listen(); backlog try 5 or 25. 5 for AIX
 * and SUNOS, 25 should work better for other OS's
 */
#define HYBRID_SOMAXCONN 25

/*
 * this hides in here rather than a config.h because it really shouldn't
 * be tweaked unless you *REALLY REALLY* know what you're doing!   
 * Remember, messages are only anti-flooded on incoming from the client, not on
 * incoming from a server for a given client, so if you tweak this you risk
 * allowing a client to flood differently depending upon where they are on
 * the network..
 *   -- adrian
 */
#define MAX_FLOOD_PER_SEC               8
/* And the initial rate of flooding after registration... -A1kmm. */
#define MAX_FLOOD_PER_SEC_I            24
  
/*
 * MAXSENDQLENGTH - Max amount of internal send buffering Max amount of
 * internal send buffering when socket is stuck (bytes)
 */
#define MAXSENDQLENGTH 5050000  /* Recommended value: 5050000 for efnet
                                 */

/*
 * The following definitions are values for specific things in the
 * server config file. 
 * They are only used, if no value has been set via ircd.conf.
 * It is not necessary to modify these normally.
 * Use the ircd.conf to modify them instead.
 * -TimeMr14C
 */

#define DEF_QUITMSG	"IRCTR - Türkiyenin IRC Networkü"
#define DEF_KLINEADDR	"kline@irctr.gen.tr"
#define DEF_AKILLADDR	"kline@irctr.gen.tr"
#define DEF_MONHOST	"irc.ada.net.tr"
#define DEF_CHANSERV	"ChanServ"
#define DEF_NICKSERV	"NickServ"
#define DEF_MEMOSERV	"MemoServ"
#define DEF_STATSERV	"StatServ"
#define DEF_OPERSERV	"Haydar"
#define DEF_SERVICES	"services.irctr.gen.tr"
#define DEF_ADMINNAME	"IRCTR"
#define DEF_ADMINMAIL	"irctr@irctr.gen.tr"
#define DEF_ADMINDESC	"IRCTR Administration"
#define DEF_PROXYURL	"kline.dal.net/proxy/wingate.htm"
#define DEF_GCOSTEXT	"IRCTR - Türkiyenin IRC Networkü"
#define DEF_HTTPD_INDEX	"/"
