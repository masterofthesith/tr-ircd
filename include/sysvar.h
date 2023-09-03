/************************************************************************
 *   IRC - Internet Relay Chat, include/sysvar.h
 *   Copyright (C) 2000 
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

extern int callbacks_called;
extern int dorehash;   
extern int rehashed;   

extern time_t NOW;
extern time_t last_stat_save;
extern time_t timeofday;
extern time_t pending_kline_time;

extern int service_info[];
extern int service_modes[];
extern char *replies[];

/* struct */

extern struct Counter Count;
extern struct stats *ircstp;

extern struct capablist my_capabs[];
extern struct capablist my_excapabs[];

extern char version[128], *infotext[], *copyrighttext[];
extern char *generation, *creation, *genesis;
extern char *uname_result[], *ulimit_result[];
extern char *srcsums[], *serversums[];
extern char *modulessums[], *chanmodessums[];
extern char *protocolsums[], *includesums[];
extern char *logsystemsums[], *iupdtsums[], *httpdsums[];
extern char *proxymonsums[], *libsums[];

extern char *exploits_2char[];
extern char *exploits_3char[];
extern char *exploits_4char[];

extern char *readbuf;

extern aWhowas *WHOWASHASH[HASHSIZE];

extern dlink_list lclient_list;
extern dlink_list serv_list;
extern dlink_list global_serv_list;
extern dlink_list unknown_list;        /* unknown clients ON this server only */
extern dlink_list locoper_list;
extern dlink_list hclient_list;
extern dlink_list http_except_list;
extern dlink_list exit_list;

