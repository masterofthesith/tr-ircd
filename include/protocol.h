/************************************************************************
 *   IRC - Internet Relay Chat, include/protocol.h
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

extern int m_kline(aClient *, aClient *, int, char **);
extern int m_unkline(aClient *, aClient *, int, char **);
extern int m_akill(aClient *, aClient *, int, char **);
extern int m_rakill(aClient *, aClient *, int, char **);
extern int m_private(aClient *, aClient *, int, char **);
extern int m_topic(aClient *, aClient *, int, char **);
extern int s_topic(aClient *, aClient *, int, char **);
extern int m_join(aClient *, aClient *, int, char **);
extern int m_part(aClient *, aClient *, int, char **);
extern int m_mode(aClient *, aClient *, int, char **);
extern int m_ping(aClient *, aClient *, int, char **);
extern int m_pong(aClient *, aClient *, int, char **);
extern int m_wallops(aClient *, aClient *, int, char **);
extern int m_kick(aClient *, aClient *, int, char **);
extern int s_nick(aClient *, aClient *, int, char **);
extern int u_nick(aClient *, aClient *, int, char **);
extern int m_error(aClient *, aClient *, int, char **);
extern int m_notice(aClient *, aClient *, int, char **);
extern int m_invite(aClient *, aClient *, int, char **);
extern int m_quit(aClient *, aClient *, int, char **);
extern int m_kill(aClient *, aClient *, int, char **);
extern int s_kill(aClient *, aClient *, int, char **);
extern int u_motd(aClient *, aClient *, int, char **);
extern int o_motd(aClient *, aClient *, int, char **);
extern int m_who(aClient *, aClient *, int, char **);
extern int m_whois(aClient *, aClient *, int, char **);
extern int m_user(aClient *, aClient *, int, char **);
extern int m_list(aClient *, aClient *, int, char **);
extern int m_server(aClient *, aClient *, int, char **);
extern int s_server(aClient *, aClient *, int, char **);
extern int m_info(aClient *, aClient *, int, char **);
extern int m_links(aClient *, aClient *, int, char **);
extern int m_summon(aClient *, aClient *, int, char **);
extern int m_stats(aClient *, aClient *, int, char **);
extern int m_users(aClient *, aClient *, int, char **);
extern int m_chanserv(aClient *, aClient *, int, char **);
extern int m_nickserv(aClient *, aClient *, int, char **);
extern int m_operserv(aClient *, aClient *, int, char **);
extern int m_statserv(aClient *, aClient *, int, char **);
extern int m_memoserv(aClient *, aClient *, int, char **);
extern int m_services(aClient *, aClient *, int, char **);
extern int m_identify(aClient *, aClient *, int, char **);
extern int m_svsnick(aClient *, aClient *, int, char **);
extern int m_svsnoop(aClient *, aClient *, int, char **);
extern int m_svskill(aClient *, aClient *, int, char **);
extern int m_svsmode(aClient *, aClient *, int, char **);
extern int m_version(aClient *, aClient *, int, char **);
extern int u_help(aClient *, aClient *, int, char **);
extern int o_help(aClient *, aClient *, int, char **);
extern int m_squit(aClient *, aClient *, int, char **);
extern int m_away(aClient *, aClient *, int, char **);
extern int m_connect(aClient *, aClient *, int, char **);
extern int m_oper(aClient *, aClient *, int, char **);
extern int m_pass(aClient *, aClient *, int, char **);
extern int m_trace(aClient *, aClient *, int, char **);
extern int m_time(aClient *, aClient *, int, char **);
extern int m_names(aClient *, aClient *, int, char **);
extern int m_admin(aClient *, aClient *, int, char **);
extern int m_lusers(aClient *, aClient *, int, char **);
extern int m_umode(aClient *, aClient *, int, char **);
extern int m_close(aClient *, aClient *, int, char **);
extern int m_whowas(aClient *, aClient *, int, char **);
extern int m_userhost(aClient *, aClient *, int, char **);
extern int m_ison(aClient *, aClient *, int, char **);
extern int m_svinfo(aClient *, aClient *, int, char **);
extern int m_sjoin(aClient *, aClient *, int, char **);
extern int m_samode(aClient *, aClient *, int, char **);
extern int m_globops(aClient *, aClient *, int, char **);
extern int m_goper(aClient *, aClient *, int, char **);
extern int m_gnotice(aClient *, aClient *, int, char **);
extern int m_rehash(aClient *, aClient *, int, char **);
extern int m_restart(aClient *, aClient *, int, char **);
extern int m_die(aClient *, aClient *, int, char **);
extern int m_htm(aClient *, aClient *, int, char **);
extern int m_set(aClient *, aClient *, int, char **);
extern int m_capab(aClient *, aClient *, int, char **);
extern int m_silence(aClient *, aClient *, int, char **);
extern int m_watch(aClient *, aClient *, int, char **);
extern int m_sqline(aClient *, aClient *, int, char **);
extern int m_unsqline(aClient *, aClient *, int, char **);
extern int m_burst(aClient *, aClient *, int, char **);
extern int m_dccallow(aClient *, aClient *, int, char **);
extern int m_szline(aClient *, aClient *, int, char **);
extern int m_unszline(aClient *, aClient *, int, char **);
extern int m_sgline(aClient *, aClient *, int, char **);
extern int m_unsgline(aClient *, aClient *, int, char **);
extern int m_dkey(aClient *, aClient *, int, char **);

/* TRIRCD */
extern int m_display(aClient *, aClient *, int, char **);
extern int m_mkpasswd(aClient *, aClient *, int, char **);
extern int m_ircops(aClient *, aClient *, int, char **);
extern int m_svsjoin(aClient *, aClient *, int, char **);
extern int m_operdo(aClient *, aClient *, int, char **);
extern int m_nethtm(aClient *, aClient *, int, char **);
extern int m_gline(aClient *, aClient *, int, char **);
extern int m_ungline(aClient *, aClient *, int, char **);
extern int m_jupiter(aClient *, aClient *, int, char **);
extern int o_jupiter(aClient *, aClient *, int, char **);
extern int m_unjupiter(aClient *, aClient *, int, char **);
extern int o_unjupiter(aClient *, aClient *, int, char **);
extern int m_service(aClient *, aClient *, int, char **);
extern int s_service(aClient *, aClient *, int, char **); 
extern int m_squery(aClient *, aClient *, int, char **);
extern int m_servset(aClient *, aClient *, int, char **);
extern int m_servlist(aClient *, aClient *, int, char **);
extern int m_exclude(aClient *, aClient *, int, char **);
extern int m_rexclude(aClient *, aClient *, int, char **);
extern int m_userip(aClient *, aClient *, int, char **);
extern int m_umode(aClient *, aClient *, int, char **);
extern int m_registered(aClient *, aClient *, int, char **);
extern int m_unregistered(aClient *, aClient *, int, char **);
extern int m_ignore(aClient *, aClient *, int, char **);
extern int m_permission(aClient *, aClient *, int, char **);
extern int m_undefined(aClient *, aClient *, int, char **);
extern int m_modload(aClient *, aClient *, int, char**);   
extern int m_modlist(aClient *, aClient *, int, char**);   
extern int m_modunload(aClient *, aClient *, int, char**);   
extern int m_modrestart(aClient *, aClient *, int, char**);   
extern int m_setlang(aClient *, aClient *, int, char **);
extern int m_myid(aClient *, aClient *, int, char **);
extern int m_netset(aClient *, aClient *, int, char **);
extern int u_map(aClient *, aClient *, int, char **);
extern int o_map(aClient *, aClient *, int, char **);
extern int m_post(aClient *, aClient *, int, char **);
extern int m_client(aClient *, aClient *, int, char **);
extern int m_locops(aClient *, aClient *, int, char **);
extern int m_rnotice(aClient *, aClient *, int, char **);
extern int m_admins(aClient *, aClient *, int, char **);
extern int m_sadmins(aClient *, aClient *, int, char **);
extern int m_helpserv(aClient *, aClient *, int, char **);
extern int m_hash(aClient *, aClient *, int, char **);
extern int u_rping(aClient *, aClient *, int, char **);
extern int o_rping(aClient *, aClient *, int, char **);
extern int s_rping(aClient *, aClient *, int, char **);
extern int m_rpong(aClient *, aClient *, int, char **);
extern int m_tmode(aClient *, aClient *, int, char **);
extern int m_uptime(aClient *, aClient *, int, char **); 
extern int m_excap(aClient *, aClient *, int, char **);
extern int m_infoconf(aClient *, aClient *, int, char **);
extern int m_ucheck(aClient *, aClient *, int, char **);
extern int m_proxychk(aClient *, aClient *, int, char **);
extern int m_copyright(aClient *, aClient *, int, char **);
extern int o_rexcom(aClient *, aClient *, int, char **);
extern int s_rexcom(aClient *, aClient *, int, char **);
extern int m_ewhois(aClient *, aClient *, int, char **);
extern int m_accept(aClient *, aClient *, int, char **);
extern int m_knock(aClient *, aClient *, int, char **);

extern int m_umode(aClient *, aClient *, int, char **);
extern int m_registered(aClient *, aClient *, int, char **);  
extern int m_unregistered(aClient *, aClient *, int, char **);
extern int m_ignore(aClient *, aClient *, int, char **);
extern int m_permission(aClient *, aClient *, int, char **);
extern int m_pseudo(aClient *, aClient *, char *, char *, int, char **);
extern int m_pseudo_help(aClient *, aClient *, char *, char *, int, char **);
