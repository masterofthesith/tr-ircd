/************************************************************************
 *   IRC - Internet Relay Chat, include/protocol_define.h
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
 *
 */

#define MSG_PRIVATE  "PRIVMSG"	/* PRIV */
#define MSG_WHO      "WHO"	/* WHO -> WHOC */
#define MSG_WHOIS    "WHOIS"	/* WHOI */
#define MSG_WHOWAS   "WHOWAS"	/* WHOW */
#define MSG_USER     "USER"	/* USER */
#define MSG_NICK     "NICK"	/* NICK */
#define MSG_SERVER   "SERVER"	/* SERV */
#define MSG_LIST     "LIST"	/* LIST */
#define MSG_TOPIC    "TOPIC"	/* TOPI */
#define MSG_INVITE   "INVITE"	/* INVI */
#define MSG_VERSION  "VERSION"	/* VERS */
#define MSG_QUIT     "QUIT"	/* QUIT */
#define MSG_SQUIT    "SQUIT"	/* SQUI */
#define MSG_KILL     "KILL"	/* KILL */
#define MSG_INFO     "INFO"	/* INFO */
#define MSG_LINKS    "LINKS"	/* LINK */
#define MSG_STATS    "STATS"	/* STAT */
#define MSG_USERS    "USERS"	/* USER -> USRS */
#define MSG_HELP     "HELP"	/* HELP */
#define MSG_ERROR    "ERROR"	/* ERRO */
#define MSG_AWAY     "AWAY"	/* AWAY */
#define MSG_CONNECT  "CONNECT"	/* CONN */
#define MSG_PING     "PING"	/* PING */
#define MSG_PONG     "PONG"	/* PONG */
#define MSG_OPER     "OPER"	/* OPER */
#define MSG_PASS     "PASS"	/* PASS */
#define MSG_WALLOPS  "WALLOPS"	/* WALL */
#define MSG_TIME     "TIME"	/* TIME */
#define MSG_NAMES    "NAMES"	/* NAME */
#define MSG_ADMIN    "ADMIN"	/* ADMI */
#define MSG_TRACE    "TRACE"	/* TRAC */
#define MSG_NOTICE   "NOTICE"	/* NOTI */
#define MSG_JOIN     "JOIN"	/* JOIN */
#define MSG_PART     "PART"	/* PART */
#define MSG_LUSERS   "LUSERS"	/* LUSE */
#define MSG_MOTD     "MOTD"	/* MOTD */
#define MSG_MODE     "MODE"	/* MODE */
#define MSG_KICK     "KICK"	/* KICK */
#define MSG_USERHOST "USERHOST"	/* USER -> USRH */
#define MSG_ISON     "ISON"	/* ISON */
#define MSG_REHASH   "REHASH"	/* REHA */
#define MSG_RESTART  "RESTART"	/* REST */
#define MSG_CLOSE    "CLOSE"	/* CLOS */
#define MSG_SVINFO   "SVINFO"	/* SVINFO */
#define MSG_SJOIN    "SJOIN"	/* SJOIN */
#define MSG_DIE	     "DIE"	/* DIE */
#define MSG_DNS      "DNS"	/* DNS -> DNSS */
#define MSG_GLOBOPS  "GLOBOPS"	/* GLOBOPS */
#define MSG_GOPER    "GOPER"	/* GOPER */
#define MSG_GNOTICE  "GNOTICE"	/* GNOTICE */
#define MSG_KLINE    "KLINE"	/* KLINE */
#define MSG_UNKLINE  "UNKLINE"	/* UNKLINE */
#define MSG_SET      "SET"	/* SET */
#define MSG_SAMODE   "SAMODE"	/* SAMODE */
#define MSG_CHANSERV "CHANSERV"	/* CHANSERV */
#define MSG_NICKSERV "NICKSERV"	/* NICKSERV */
#define MSG_MEMOSERV "MEMOSERV"	/* MEMOSERV */
#define MSG_OPERSERV "OPERSERV"	/* OPERSERV */
#define MSG_STATSERV "STATSERV"	/* STATSERV */
#define MSG_HELPSERV "HELPSERV" /* HELPSERV */
#define MSG_SERVICES "SERVICES"	/* SERVICES */
#define MSG_IDENTIFY "IDENTIFY"	/* IDENTIFY */
#define MSG_CAPAB    "CAPAB"	/* CAPAB */
#define MSG_SVSNICK  "SVSNICK"	/* SVSNICK */
#define MSG_SVSNOOP  "SVSNOOP"	/* SVSNOOP */
#define MSG_SVSKILL  "SVSKILL"	/* SVSKILL */
#define MSG_SVSMODE  "SVSMODE"	/* SVSMODE */
#define MSG_AKILL    "AKILL"	/* AKILL */
#define MSG_RAKILL   "RAKILL"	/* RAKILL */
#define MSG_SILENCE  "SILENCE"	/* SILENCE */
#define MSG_WATCH    "WATCH"	/* WATCH */
#define MSG_SQLINE   "SQLINE"	/* SQLINE */
#define MSG_UNSQLINE "UNSQLINE"	/* UNSQLINE */
#define MSG_BURST    "BURST"	/* BURST */
#define MSG_DCCALLOW "DCCALLOW"	/* dccallow */
#define MSG_SZLINE   "SZLINE"	/* szline */
#define MSG_UNSZLINE "UNSZLINE"	/* unszline */
#define MSG_SGLINE   "SGLINE"	/* sgline */
#define MSG_UNSGLINE "UNSGLINE"	/* unsgline */
#define MSG_DKEY     "DKEY"	/* diffie-hellman negotiation */

/* TRIRCD */
#define MSG_SVSJOIN     "SVSJOIN"
#define MSG_OPERDO      "OPERDO"
#define MSG_GLINE       "GLINE"
#define MSG_UNGLINE     "UNGLINE"
#define MSG_NS          "NS"
#define MSG_CS          "CS"
#define MSG_MS          "MS"
#define MSG_OS          "OS"
#define MSG_SS		"SS"
#define MSG_HS		"HS"
#define MSG_UMODE       "UMODE"
#define MSG_JUPITER     "JUPITER"
#define MSG_UNJUPE      "UNJUPITER"
#define MSG_SERVICE     "SERVICE"
#define MSG_SERVLIST    "SERVLIST"
#define MSG_SQUERY      "SQUERY"
#define MSG_SERVSET     "SERVSET"
#define MSG_EXCLUDE     "EXCLUDE"
#define MSG_REXCLUDE    "REXCLUDE"
#define MSG_DISPLAY     "DISPLAY"
#define MSG_MAP         "MAP"
#define MSG_MKPASSWD    "MKPASSWD"
#define MSG_IRCOPS      "IRCOPS"
#define MSG_USERIP	"USERIP"
#define MSG_MODLOAD	"MODLOAD"
#define MSG_MODUNLOAD	"MODUNLOAD"
#define MSG_MODRESTART	"MODRESTART"
#define MSG_MODLIST	"MODLIST"
#define MSG_SETLANG	"SETLANG"
#define MSG_MYID	"MYID"
#define MSG_NETSET	"NETSET"
#define MSG_INSMOD	"INSMOD"
#define MSG_DEPMOD	"DEPMOD"
#define MSG_RMMOD	"RMMOD"
#define MSG_LSMOD	"LSMOD"
#define MSG_HELPME	"HELPME"
#define MSG_RPING	"RPING"
#define MSG_RPONG	"RPONG"
#define MSG_CLIENT	"CLIENT"
#define MSG_LOCOPS	"LOCOPS"
#define MSG_RNOTICE	"RNOTICE"
#define MSG_ADMINS	"ADMINS"
#define MSG_SADMINS	"SADMINS"
#define MSG_HASH	"HASH"
#define MSG_TMODE	"TMODE"
#define MSG_UPTIME	"UPTIME"
#define MSG_EXCAP	"EXCAP"
#define MSG_INFOCONF	"INFOCONF"
#define MSG_UCHECK	"UCHECK"
#define MSG_PROXYCHK	"PROXYCHK"
#define MSG_HTTPPOST	"POST"
#define MSG_HTTPGET	"GET"
#define MSG_HTTPPUT	"PUT"
#define MSG_COPYRIGHT	"COPYRIGHT"
#define MSG_REXCOM	"REXCOM"
#define MSG_EWHOIS	"EWHOIS"
#define MSG_ACCEPT	"ACCEPT"
#define MSG_KNOCK	"KNOCK"

#define TOK1_LUSERS	"@" /* LUSERS */
#define TOK1_USERS	"\\" /* USERS*/
#define TOK1_TMODE	"~" /* TMODE */
#define TOK1_AKILL 	"a" /* AKILL */
#define TOK1_AWAY 	"A" /* AWAY */
#define TOK1_SVSJOIN	"b" /* SVSJOIN */
#define TOK1_BURST 	"B" /* BURST */
#define TOK1_JUPITER	"C" /* JUPITER */
#define TOK1_CONNECT 	"c" /* CONNECT */
#define TOK1_ADMIN      "d" /* ADMIN */
#define TOK1_UNJUPE	"D" /* UNJUPITER */
#define TOK1_EXCLUDE	"e" /* EXCLUDE */
#define TOK1_ERROR 	"E" /* ERROR */
#define TOK1_GLINE	"F" /* GLINE */
#define TOK1_PING       "f" /* PING */
#define TOK1_GNOTICE 	"G" /* GNOTICE */
#define TOK1_GOPER 	"g" /* GOPER */
#define TOK1_SILENCE	"h" /* SILENCE */
#define TOK1_CLIENT	"H" /* CLIENT */
#define TOK1_TIME       "i" /* TIME */ 
#define TOK1_INVITE 	"I" /* INVITE */
#define TOK1_REXCLUDE	"j" /* REXCLUDE */
#define TOK1_JOIN 	"J" /* JOIN */
#define TOK1_KICK 	"K" /* KICK */
#define TOK1_KILL 	"k" /* KILL */
#define TOK1_GLOBOPS    "L" /* GLOBOPS */
#define TOK1_WALLOPS    "l" /* WALLOPS */ 
#define TOK1_MODE 	"M" /* MODE */
#define TOK1_MOTD 	"m" /* MOTD */
#define TOK1_NICK 	"N" /* NICK */
#define TOK1_NOTICE 	"n" /* NOTICE */
#define TOK1_SQUERY	"O" /* SQUERY */ 
#define TOK1_UPTIME	"o" /* UPTIME */
#define TOK1_PART 	"p" /* PART */
#define TOK1_PRIVMSG 	"P" /* PRIVMSG */
#define TOK1_SQUIT      "q" /* SQUIT */
#define TOK1_QUIT 	"Q" /* QUIT */
#define TOK1_RAKILL 	"R" /* RAKILL */
#define TOK1_RNOTICE    "r" /* RNOTICE */
#define TOK1_VERSION 	"v" /* VERSION */
#define TOK1_NETSET	"V" /* NETSET */
#define TOK1_SERVER 	"s" /* SERVER */
#define TOK1_SJOIN 	"S" /* SJOIN */
#define TOK1_TOPIC      "T" /* TOPIC */  
#define TOK1_TRACE      "t" /* TRACE */
#define TOK1_PONG       "u" /* PONG */ 
#define TOK1_UNSQLINE   "U" /* UNSQLINE */
#define TOK1_WHOIS      "W" /* WHOIS */   
#define TOK1_WHOWAS     "w" /* WHOWAS */ 
#define TOK1_UNGLINE	"y" /* UNGLINE */
#define TOK1_SERVICE	"X" /* SERVICE */
#define TOK1_SQLINE 	"x" /* SQLINE */
#define TOK1_INFO	"Y" /* INFO */
#define TOK1_SZLINE 	"Z" /* SZLINE */
#define TOK1_SVINFO     "z" /* SVINFO */
#define TOK1_STATS 	"?" /* STATS */
#define TOK1_SVSKILL 	"1" /* SVSKILL */
#define TOK1_SVSMODE 	"2" /* SVSMODE */
#define TOK1_SVSNICK 	"3" /* SVSNICK */
#define TOK1_SVSNOOP 	"4" /* SVSNOOP */
#define TOK1_SGLINE     "5" /* SGLINE */
#define TOK1_UNSGLINE 	"6" /* UNSGLINE */
#define TOK1_UNSZLINE 	"7" /* UNSQLINE */
#define TOK1_RPING	"[" /* RPING */
#define TOK1_RPONG	"]" /* RPONG */
#define TOK1_MYID	"=" /* MYID */
#define TOK1_ADMINS	"<" /* ADMINS */
#define TOK1_SADMINS	">" /* SADMINS */
#define TOK1_HASH	";" /* HASH */
#define TOK1_INFOCONF	"^" /* INFOCONF */
#define TOK1_REXCOM	"|" /* REXCOM */
#define TOK1_EWHOIS	"," /* EWHOIS */
#define TOK1_KNOCK	"{" /* KNOCK */
