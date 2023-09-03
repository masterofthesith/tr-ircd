/************************************************************************
 *   IRC - Internet Relay Chat, include/confitem.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1995 Philippe Levan
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
 * The dich_conf.h and dich_conf.c were written to provide a generic
 * interface for configuration lines matching. I decided to write it
 * after I read Roy's K: line tree patch. the underlying ideas are the
 * following : . get rid of the left/right branches by using a
 * dichotomy on an ordered list . arrange patterns matching one another
 * in a tree so that if we find a possible match, all other possible
 * matches are "below" These routines are meant for fast matching.
 * There is no notion of "best" of "first" (meaning the order in which
 * the lines are read) match. Therefore, the following functions are
 * fit for K: lines matching but not I: lines matching (as sad as it
 * may be). Other kinds of configuration lines aren't as time consuming
 * or just aren't use for matching so it's irrelevant to try and use
 * these functions for anything else. However, I still made them as
 * generic as possible, just in case.
 * 
 * -Soleil (Philippe Levan)
 * 
 */

#include "fileio.h"

/*
 * $Id: confitem.h,v 1.4 2003/06/14 13:55:50 tr-ircd Exp $ 
 */

extern int rehash(struct Client *, struct Client *, int);

extern int ircdlex(void);
extern int ircdwrap(void);
extern int ircdparse(void);
extern void ircderror(char *);

extern int maskfilelex(void);
extern int maskfilewrap(void);
extern int maskfileparse(void);
extern void maskfileerror(char *);

extern int proxymonlex(void);
extern int proxymonwrap(void);
extern int proxymonparse(void);
extern void proxymonerror(char *);

extern int httpdlex(void);
extern int httpdwrap(void);
extern int httpdparse(void);
extern void httpderror(char *); 

extern int classeslex(void);
extern int classeswrap(void);
extern int classesparse(void);
extern void classeserror(char *);

extern int conf_yy_fatal_error(char *);
extern int conf_fbgets(char *, int, FBFILE *);

extern void read_conf_files(int cold);

extern FBFILE *conf_fbfile_in;
extern FBFILE *mask_fbfile_in;
extern FBFILE *pmconf_fbfile_in;
extern FBFILE *httpd_fbfile_in;
extern char conf_line_in[256];

extern int check_client(struct Client *client_p, struct Client *source_p, char *);
extern int attach_conf(struct Client *, struct ConfItem *);
extern int attach_cn_lines(struct Client *client, char *name, char *host);
extern int attach_Iline(struct Client *client, char *username);
extern int detach_conf(struct Client *, struct ConfItem *);

extern int conf_add_server(struct ConfItem *, int);

extern char *get_client_name(aClient *, int);

extern void free_conf(aConfItem *);

extern void conf_add_class_to_conf(struct ConfItem *);

extern void conf_add_conf(struct ConfItem *);
extern void det_confs_butmask(struct Client *, int);
extern void free_conf(struct ConfItem *);
extern void delist_and_remove_conf(struct ConfItem *);
extern void lookup_confhost(struct ConfItem *aconf);

extern void report_connect_blocks(aClient *sptr, int all);
extern void report_connect_blocks_flag(aClient *sptr, int confflag,
				       int reply, char letter);
extern void report_auth_blocks(aClient *sptr, int onlywhat);
extern void report_operator_blocks(aClient *sptr);
extern void report_service_blocks(aClient *sptr, int all);
extern void show_opers(aClient *cptr, char *name);
extern void show_servers(aClient *cptr, char *name);
extern void report_protocol(aClient *cptr);

extern aConfItem *GlobalConfItemList;
extern aConfItem *make_conf(void);

extern aConfItem *find_kill(aClient *);
extern aConfItem *find_zkill(aClient *);
extern aConfItem *make_conf(void);

extern struct ConfItem *det_confs_butone(struct Client *, struct ConfItem *);
extern struct ConfItem *find_conf_exact(char *name, char *user, char *host, int statmask);
extern struct ConfItem *find_conf_name(dlink_list * list, char *name, int statmask);
extern struct ConfItem *find_conf_by_name(char *name, int status);
extern struct ConfItem *find_conf_by_host(char *host, int status);
extern struct ConfItem *find_conf_by_uuid(char *uuid, int status);
extern struct ConfItem *find_conf_for_client(aClient *cptr, int status, int flags);
