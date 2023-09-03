/************************************************************************
 *   IRC - Internet Relay Chat, include/channel.h
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

extern aChannel *hash_get_chan_bucket(int);
extern aChannel *channel;
extern aChannel *find_channel(char *);
extern aChannel *create_channel(aClient *cptr, char *chname, int *new, int recurse);
extern aChannel *make_channel(void);

extern aClient *find_user_member(aChannel *, aClient *);

extern int can_send(aClient *, aChannel *, char *);
extern int set_mode(aClient *, aClient *, aChannel *, int, char **, char *, char *);
extern int can_join(aClient *sptr, aChannel *chptr, char *key);

extern int add_to_channel_hash_table(char *, aChannel *);
extern int del_from_channel_hash_table(char *, aChannel *);

extern void init_channel(void *);
extern void init_chanmodes(void *);

extern void create_channelmodelists(void);

extern int add_user_to_channel(aChannel *, aClient *, int);
extern void remove_user_from_channel(aClient *, aChannel *);
extern void channel_modes(aClient *, char *, char *, aChannel *, int);
extern void send_user_joins(aClient *, aClient *);
extern void del_invite(aClient *cptr, aChannel *chptr);
extern void send_channel_modes(aClient *, aChannel *);
extern void free_channel(aChannel *);
extern void add_invite(aClient *cptr, aChannel *chptr);
extern void send_names(aClient *sptr, aChannel *chptr);
extern void send_topic_burst(aClient *);
extern void privileged_join(aClient *sptr, aClient *cptr, char *name);
extern void remove_chanmember(aClient *, aChannel *);

#ifndef CHANNEL_H
#define CHANNEL_H 1

#define IsMember(b,c)           ((b && b->user && dlinkFind(&((b->user)->channel), c)) ? 1 : 0)

static inline int get_flags(aClient *acptr, aChannel *chptr) {       
    
    dlink_node *ptr;
    struct ChanMember *cm;

    for (ptr = chptr->members.head; ptr; ptr = ptr->next) {
	cm = ptr->data;
	if (acptr == cm->client_p)
	    return cm->flags;
    }
    return 0;
}

static inline void update_userflags(aClient *acptr, aChannel *chptr, int add, int flag)
{
    dlink_node *ptr;
    struct ChanMember *cm;

    for (ptr = chptr->members.head; ptr; ptr = ptr->next) {
        cm = ptr->data;
        if (acptr == cm->client_p) {
            if (add)
                cm->flags |= flag;
            else
                cm->flags &= ~flag;
        }
    }
}

static inline int IsChanUser(aClient *acptr, aChannel *chptr, int flag)
{
    dlink_node *ptr;
    struct ChanMember *cm;

    for (ptr = chptr->members.head; ptr; ptr = ptr->next) {
        cm = ptr->data;
        if (acptr == cm->client_p) {
	    if ((cm->flags & flag) == flag)
            	return 1;
	}
    }

    return 0;
}


static inline int get_bans(aClient *acptr, aChannel *chptr)
{
    dlink_node *ptr;
    struct ChanMember *cm;

    for (ptr = chptr->members.head; ptr; ptr = ptr->next) {
        cm = ptr->data;
        if (acptr == cm->client_p)  
            return cm->bans;
    }

    return 0;
}

#endif 
