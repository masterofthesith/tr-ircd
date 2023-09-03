/************************************************************************
 *   IRC - Internet Relay Chat, include/s_users.h
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

extern char *date(time_t);
extern char *smalldate(time_t);

extern int msg_has_colors(char *);

extern int check_for_ctcp(char *, char **, int);
extern int check_forbidden_words(char *);
extern int check_dccsend(aClient *, aClient *, char *);
extern int add_dccallow(aClient *, aClient *);
extern int del_dccallow(aClient *, aClient *);
extern int check_accept(aClient *, aClient *);
extern int accept_client(aClient *, aClient *);
extern int deny_client(aClient *, aClient *);

extern int valid_hostname(const char *hostname);
extern int valid_username(const char *username);
extern int check_drone_PB(char *username, char *gcos);

extern int is_silenced(aClient *, aClient *);
extern int allow_dcc(aClient *to, aClient *from);

extern int check_channelname(aClient *, unsigned char *);

extern void proxy_scanner_add(char *addr);
extern void send_list(aClient *cptr, LOpts *lopt);
extern int will_exceed_sendq(aClient *);

extern void init_user(void);
extern void init_server(void);

extern int match_ipmask(char *, aClient *);

extern int send_lusers(aClient *, aClient *, int, char **);
extern int add_silence(aClient *sptr, char *mask);
extern int del_silence(aClient *, char *);
extern int send_supported(aClient *);
extern void check_update(aClient *);

extern char *canonize(char *);
extern char *pretty_mask(char *);
extern char *calcmask(char *, char *);
extern char *calcpass(char *, char *);
extern char *calchash(char *, char *);
extern char *inetntoa(char *);       
extern const char *inetntop(int, const void *, char *, unsigned int);              
extern char *stealth_server(char *, char *);
extern char *collapse(char *);

extern void serv_info(aClient *, char *);
extern void send_capab_to(aClient *, int);

extern int do_numeric(int, aClient *, aClient *, int, char **);
extern int hunt_server(aClient *, aClient *, char *, char *, int, int, char **);
extern int inetpton(int, const char *, void *);	

extern void write_links_file(void*);   

extern int find_kill_level(aClient *);
extern int find_connection_kill_level(aClient *, char *);

extern int generate_server_config_file(char *);
extern int generate_maskitem_config_file(char *);
extern int generate_proxymon_config_file(char *);
extern int generate_ihttpd_config_file(char *);
extern void backup_configuration_files(void);

extern int match(char *, char *);
extern int irc_strcmp(char *, char *);
extern int irc_strncmp(char *, char *, int);
extern int irc_equal(int, int);
extern char *strtoken(char **, char *, char *);

size_t strlcpy_irc(char *dst, const char *src, size_t siz);

extern char *strip_tabs(char *dest, const unsigned char *src, size_t len);      

extern unsigned int strhash(const char *name);
