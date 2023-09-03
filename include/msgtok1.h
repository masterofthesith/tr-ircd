/************************************************************************
 *   IRC - Internet Relay Chat, include/msgtok1.h
 *   Copyright (C) 1992 Darren Reed
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

#ifndef STATIC_MODULES
#define PARSE_TOKEN
#endif

struct Token tok1_msgtab[] = {
   { NULL, NULL }, /* #0 */
   { NULL, NULL }, /* #1 */     
   { NULL, NULL }, /* #2 */     
   { NULL, NULL }, /* #3 */     
   { NULL, NULL }, /* #4 */     
   { NULL, NULL }, /* #5 */     
   { NULL, NULL }, /* #6 */     
   { NULL, NULL }, /* #7 */     
   { NULL, NULL }, /* #8 */     
   { NULL, NULL }, /* #9 */     
   { NULL, NULL }, /* #10 */     
   { NULL, NULL }, /* #11 */     
   { NULL, NULL }, /* #12 */     
   { NULL, NULL }, /* #13 */     
   { NULL, NULL }, /* #14 */     
   { NULL, NULL }, /* #15 */     
   { NULL, NULL }, /* #16 */     
   { NULL, NULL }, /* #17 */     
   { NULL, NULL }, /* #18 */     
   { NULL, NULL }, /* #19 */     
   { NULL, NULL }, /* #20 */     
   { NULL, NULL }, /* #21 */     
   { NULL, NULL }, /* #22 */     
   { NULL, NULL }, /* #23 */     
   { NULL, NULL }, /* #24 */     
   { NULL, NULL }, /* #25 */     
   { NULL, NULL }, /* #26 */     
   { NULL, NULL }, /* #27 */     
   { NULL, NULL }, /* #28 */     
   { NULL, NULL }, /* #29 */     
   { NULL, NULL }, /* #30 */     
   { NULL, NULL }, /* #31 */     
   { NULL, NULL }, /* #32 */     
   { NULL, NULL }, 		/* ! should not be used */
   { NULL, NULL }, 		/* " */     
   { NULL, NULL }, 		/* # */     
   { NULL, NULL }, 		/* $ */     
   { NULL, NULL }, 		/* % */     
   { NULL, NULL }, 		/* & */     
   { NULL, NULL }, 		/* ' */     
   { NULL, NULL }, 		/* ( */     
   { NULL, NULL }, 		/* ) */     
   { NULL, NULL }, 		/* * */     
   { NULL, NULL }, 		/* + */     
   {MSG_EWHOIS, NULL }, 	/* , */
   { NULL, NULL }, 		/* - */
   { NULL, NULL }, 		/* . */
   { NULL, NULL }, 		/* / */
   { NULL, NULL }, 		/* 0 */
   {MSG_SVSKILL, NULL }, 	/* 1 */
   {MSG_SVSMODE, NULL }, 	/* 2 */
   {MSG_SVSNICK, NULL }, 	/* 3 */
   {MSG_SVSNOOP, NULL }, 	/* 4 */
   {MSG_SGLINE, NULL }, 	/* 5 */
   {MSG_UNSGLINE, NULL }, 	/* 6 */
   {MSG_UNSZLINE, NULL }, 	/* 7 */
   {NULL, NULL }, 		/* 8 */  
   {NULL, NULL }, 		/* 9 */  
   { NULL, NULL }, 		/* : should not be used */  
   {MSG_HASH, NULL }, 		/* ; */  
   {MSG_ADMINS, NULL }, 	/* < */  
   {MSG_MYID, NULL }, 		/* = */
   {MSG_SADMINS, NULL }, 	/* > */    
   {MSG_STATS, NULL }, 		/* ? */
   {MSG_LUSERS, NULL }, 	/* @ */
   {MSG_AWAY, NULL }, 		/* A */
   {MSG_BURST, NULL }, 		/* B */
   {MSG_JUPITER, NULL }, 	/* C */
   {MSG_UNJUPE, NULL }, 	/* D */
   {MSG_ERROR, NULL }, 		/* E */
   {MSG_GLINE, NULL }, 		/* F */
   {MSG_GNOTICE, NULL }, 	/* G */
   {MSG_CLIENT, NULL }, 	/* H */ 
   {MSG_INVITE, NULL }, 	/* I */
   {MSG_JOIN, NULL }, 		/* J */
   {MSG_KICK, NULL }, 		/* K */
   {MSG_GLOBOPS, NULL }, 	/* L */
   {MSG_MODE, NULL }, 		/* M */
   {MSG_NICK, NULL }, 		/* N */
   {MSG_SQUERY, NULL }, 	/* O */
   {MSG_PRIVATE, NULL }, 	/* P */
   {MSG_QUIT, NULL }, 		/* Q */
   {MSG_RAKILL, NULL }, 	/* R */
   {MSG_SJOIN, NULL }, 		/* S */
   {MSG_TOPIC, NULL }, 		/* T */
   {MSG_UNSQLINE, NULL }, 	/* U */
   {MSG_NETSET, NULL }, 	/* V */
   {MSG_WHOIS, NULL }, 		/* W */
   {MSG_SERVICE, NULL }, 	/* X */
   {MSG_INFO, NULL }, 		/* Y */
   {MSG_SZLINE, NULL }, 	/* Z */
   {MSG_RPING, NULL }, 		/* [ */
   {MSG_USERS, NULL }, 		/* \ */
   {MSG_RPONG, NULL }, 		/* ] */
   {MSG_INFOCONF, NULL }, 	/* ^ */
   { NULL, NULL }, 		/* _ IN USE BY WORDFILTER */
   { NULL, NULL }, 		/* ` IN USE BY WORDFILTER */
   {MSG_AKILL, NULL }, 		/* a */
   {MSG_SVSJOIN, NULL }, 	/* b */
   {MSG_CONNECT, NULL }, 	/* c */
   {MSG_ADMIN, NULL }, 		/* d */
   {MSG_EXCLUDE, NULL }, 	/* e */
   {MSG_PING, NULL }, 		/* f */
   {MSG_GOPER, NULL }, 		/* g */
   {MSG_SILENCE, NULL }, 	/* h */
   {MSG_TIME, NULL }, 		/* i */
   {MSG_REXCLUDE, NULL }, 	/* j */
   {MSG_KILL, NULL }, 		/* k */ 
   {MSG_WALLOPS, NULL }, 	/* l */
   {MSG_MOTD, NULL }, 		/* m */
   {MSG_NOTICE, NULL }, 	/* n */
   {MSG_UPTIME, NULL }, 	/* o */
   {MSG_PART, NULL }, 		/* p */
   {MSG_SQUIT, NULL }, 		/* q */
   {MSG_RNOTICE, NULL }, 	/* r */
   {MSG_SERVER, NULL }, 	/* s */
   {MSG_TRACE, NULL }, 		/* t */
   {MSG_PONG, NULL }, 		/* u */
   {MSG_VERSION, NULL }, 	/* v */
   {MSG_WHOWAS, NULL }, 	/* w */
   {MSG_SQLINE, NULL }, 	/* x */
   {MSG_UNGLINE, NULL }, 	/* y */
   {MSG_SVINFO, NULL }, 	/* z */
   {MSG_KNOCK, NULL }, 		/* { */
   {MSG_REXCOM, NULL }, 	/* | */
   { NULL, NULL }, 		/* } */
   {MSG_TMODE, NULL }, 		/* ~ */
   { NULL, NULL }, 		/* #127 */
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
   { NULL, NULL },
};


