/************************************************************************
 *   IRC - Internet Relay Chat, include/s_conf.h
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
 * $Id: s_conf.h,v 1.14 2004/07/12 09:14:58 tr-ircd Exp $
 */

#ifndef S_CONF_H
#define S_CONF_H 1

struct server_options {
   int ts_warn_delta;
   int ts_max_delta;
   int default_kline_time;
   int motd_wait;
   int motd_max;
   int client_flood;
   int anti_nick_flood;
   int max_nick_time;
   int max_nick_changes;
   int no_away_flood;
   int max_away_time;
   int max_away_count;
   int max_knock_time;
   int save_maxclient_stats_time;
   int use_registerfilter;
   int hide_gcos_with_sgline;
   int short_motd;
   int staffhide;
   int wingate_notice;
   int identd_complain;
   int identd_use_tilde;
   int identd_timeout;
   int throttle_time;
   int throttle_count;
   int anti_spambot;
   int min_join_leave_time;
   int max_join_leave_count;
   int join_leave_count_expire_time;
   int oper_spam_countdown;
   int no_oper_jupiter;
   int proxy_url_len;
   int monitor_host_len;
   int default_fakehost_mode;
   int use_regex;
   int no_messages_on_away;
   char *language;
   char *server_kline_address;
   char *network_kline_address;
   char *monitor_host;
   char *proxy_url;
   char *realname_replacement;
};

struct server_info {
   char *name;
   char *description;
   char *networkname;
   struct irc_inaddr address;
   struct irc_inaddr address6;
   int specific_ipv4_vhost;
   int specific_ipv6_vhost;
   char *restartpass;
   char *diepass;
   char *displaypass;
   int identity;
   char *networkdesc;
   char *aliasname;
};

struct services_conf {
   char *chanserv;
   char *nickserv;
   char *memoserv;
   char *operserv;
   char *statserv;
   char *helpserv;
   char *services_name;
   int hide_ulined_servers;
};

struct admin_info {
   char *name;
   char *email;
   char *description;
};

struct channel_conf {
   int max_channels_per_user;
   int visschan;
   int default_extended_topic_limitation;
   char *default_quit_msg;
};

struct server_hide {
   int enable;
   int links_from_file;
   int flatten_links;
   int links_oper_only;
   int links_delay;
};

struct general_options {
   int debuglevel;
   int foreground;
   int smallnet;
   int maxclients;
   int protocol_in_use;
   int spam_num;
   int spam_time;
   int enable_logging;
   int split;
   int lists_created;
   int doing_ssl;
   int enable_proxymonitor;
   int enable_httpd;
   int webconfig;
   int webconfigport;
   
   aMotd motd;
   aMotd linksfile;
   aMotd shortmotd;
   aMotd conffile;

   char *dpath;
   char *configfile;
   char *PIDFile;
   char *proxyconffile;
   char *proxymodulefile;
   char *httpdconffile;
   char *chanmodelist;
   char *umodelist;
   char *allchanmodes;
   char *paramchanmodes;   
   char *webconfigbind;
   char *webconfigpass;
};

struct log_system {
    int generalerror;              
    int restartmsg;              
    int restart;              
    int execv;              
    int report_error;              
    int read_error;              
    int add_confline;              
    int bad_connect;              
    int server_noquit;              
    int exit_server;              
    int exit_client;
    int hash_error;              
    int conncheck;              
    int serverkill;              
    int modnoinit;              
    int modload;
    int modunload;
    int noprotocol;              
    int parse_unknown_prefix;              
    int parse_unknown_message;              
    int parse_empty;              
    int parse_unknown;              
    int parse_debug;              
    int timeout;              
    int ping;              
    int pid;              
    int send_debug;              
    int badconfigline;              
    int yyerror;              
    int cyyerror;
    int conferror;              
    int lexer_deep;              
    int lexer_noinc;              
    int parser_netmask;              
    int parser_spoof;              
    int kqueue_netio;
    int devpoll_write_update;
    int devpoll_netio;
    int devpoll_comm_select;
    int devpoll_revents;
    int proxyevent;
    int ssl_error;
    int httpd;
    int httpd_init;
    int httpd_request;
    int operevent;
    int epoll_netio;
    int epoll_ctl;
};

struct scanner_conf {
    int fd_limit;
    int max_read;
    int timeout;
    int negfail_notices;
    int scan_port;
    int action;
    char *dnsblhost;
    char *scan_ip;
    char *bind_ip;
};

struct httpd_conf {
    int require_index;
    int index_file_len;
    int http_except_any;
    int policy;
    
    char *index_file;
};

extern struct server_options ServerOpts;
extern struct server_info ServerInfo;
extern struct services_conf ServicesConf;
extern struct admin_info AdminInfo;
extern struct channel_conf ChannelConf;
extern struct server_hide ServerHide;
extern struct general_options GeneralOpts;
extern struct log_system LogSys;
extern struct scanner_conf ScannerConf;
extern struct httpd_conf HttpdConf;

#endif
