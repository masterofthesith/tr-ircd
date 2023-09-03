/************************************************************************
 *   IRC - Internet Relay Chat, include/maskitem.h
 *   Copyright (C) 2000 TR-IRCD Development
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

#ifndef MASKITEM_H
#define MASKITEM_H 1

#include "../lib/regex/regex.h"

struct MaskItem {

    aMaskItem *hnext;           /* next item in the hash table */

    struct irc_inaddr ipnum;    /* ip address */

    int type;                   /* type */

    unsigned long hashval;      /* hash value */

    char *string;               /* a string, possibly a nick, host, gecos */
    char *username;
    char *ownername;
    char *reason;

    time_t creation;            /* creation time */
    time_t expiration;          /* expire time, 0 if no expire */

    struct re_pattern_buffer rex;       /* regex pattern */

};

extern void init_maskitem(void);
extern struct MaskItem *create_maskitem(char *, char *, char *, int, unsigned long);
extern struct MaskItem *find_maskitem(char *, char *, int, int);
extern void terminate_maskitem(char *, char *, int);
extern void terminate_maskitem_matching(char *, char *, int);
extern void report_maskitem_list_secondary(aClient *, char *, int, int, char);
extern void report_maskitem_list_primary(aClient *, char *, int, int, char);
extern void push_all_maskitems(aClient *, int);
extern void rehash_maskitems(int);
extern void expire_maskitems(void);

extern void ireg_free_ami(aMaskItem *);
extern void ireg_merge_pattern(aMaskItem *, int, char *);
extern int ireg_match(aMaskItem *, char *);

#endif


