/************************************************************************
 *   IRC - Internet Relay Chat, include/client.h
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

extern aClient *make_client(aClient *);
extern aClient *GlobalClientList, me;
extern aClient *find_chasing(aClient *, char *, int *);
extern aClient *find_chasing_user(char *);
extern aClient *find_person(char *name);
extern aClient *next_client(aClient *, char *);
extern aClient *next_client_double(aClient *, char *);
extern aClient *find_client(char *);
extern aClient *find_server(char *);

extern void add_history(aClient *, int);
extern void off_history(aClient *);
extern aClient *get_history(char *, time_t);

extern aServer *make_server(aClient *);

extern aService *firstservice;
extern aService *make_service(aClient *cptr);

extern anUser *make_user(aClient *);
extern aClass *make_class(void);

extern int check_client(aClient *, aClient *, char *);

extern int exit_client(aClient *, aClient *, char *);

extern int get_client_ping(aClient *);
extern int get_client_class(aClient *);
extern int add_to_client_hash_table(char *, aClient *);
extern int del_from_client_hash_table(char *, aClient *);

extern int add_to_watch_hash_table(char *, aClient *);
extern int del_from_watch_hash_table(char *, aClient *);
extern int hash_check_watch(aClient *, int);
extern int hash_del_watch_list(aClient *);  

extern void add_client_to_list(aClient *);
extern void remove_client_from_list(aClient *);

extern void add_client_to_llist(struct Client **, struct Client *);
extern void del_client_from_llist(struct Client **, struct Client *);

extern int do_remote_user(char *, aClient *, aClient *, char *, char *,
                   char *, char *, unsigned long, char *);
extern int do_local_user(char *, aClient *, aClient *, char *, char *,
                   char *, char *, unsigned long, char *);

extern int register_remote_user(aClient *, aClient *, char *, char *);
extern int register_local_user(aClient *, aClient *, char *, char *);
extern int server_estab(struct Client *);
extern int continue_server_estab(struct Client *);

extern int check_server(char *, struct Client *);

extern void add_server_to_list(struct Client *);
extern void remove_server_from_list(struct Client *);

extern void free_user(anUser *);

extern aWatch *hash_get_watch(char *);
extern unsigned int hash_whowas_name(char *);

extern void free_client(aClient *);
extern void remove_exited_clients(void *);

extern int remove_dcc_references(aClient *);
extern int remove_accept_references(aClient *);
