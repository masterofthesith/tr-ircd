/*
 *   IRC - Internet Relay Chat, src/channel.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *   Copyright (C) 1990-2002 Past and Present Ircd Coders
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
 */

/*
 * $Id: channel.c,v 1.10 2006/01/03 23:57:30 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "channel.h"
#include "h.h"
#include "msg.h"
#include "hook.h"
#include "event.h"
#include "blalloc.h"
#include "chanmode.h"
#include "usermode.h"
#include "s_conf.h"

aChannel *channel = NULL;

static char buf[BUFSIZE];

static int hookid_sub_from_channel = 0;
static int hookid_can_send = 0;
static int hookid_can_join = 0;
static int hookid_can_send_more = 0;

BlockHeap *free_channels;
BlockHeap *chan_members;

/* INIT function for the hooks used in channelmode modules */

void init_channel(void *unused)
{
    free_channels = BlockHeapCreate((size_t) sizeof(aChannel), CHANNELS_PREALLOCATE);
    chan_members = BlockHeapCreate((size_t) sizeof(struct ChanMember), CHANNELS_PREALLOCATE * 10);

    hookid_sub_from_channel = hook_add_event("sub1 from channel");
    hookid_can_send = hook_add_event("can send");
    hookid_can_join = hook_add_event("can join");
    hookid_can_send_more = hook_add_event("can send more");
}

/*
 * we also tell the client if the channel is invalid.
 */
int check_channelname(aClient *cptr, unsigned char *cn)
{
    aMaskItem *ami;

    if (!MyClient(cptr))
	return 1;

    /*
     * We bypass Jupiter Checks, if the client is an operator
     * * I personally accepted this idea, because, Ircops can also
     * * use quarantined nicknames, so they ought to be able to join
     * * jupitered channels
     */

    if (!IsAnOper(cptr)) {
	ami = find_maskitem((char *) cn, NULL, MASKITEM_JUPITER, 1);
	if (ami) {
	    send_me_numeric(cptr, ERR_CHANISJUPE, cn, ami->string, ami->reason);
	    return 0;
	} else if ((ami = find_maskitem((char *) cn, NULL, MASKITEM_JUPITER_CONFIG, 1))) {
            send_me_numeric(cptr, ERR_CHANISJUPE, cn, ami->string, ami->reason);
            return 0;
	} else if (ServerOpts.use_regex) {
	    ami = find_maskitem((char *) cn, NULL, MASKITEM_JUPITER_REGEX, 1);
	    if (ami) {
		send_me_numeric(cptr, ERR_CHANISJUPE, cn, ami->string, ami->reason);
		return 0;
	    }
	}
    }
    for (; *cn; cn++) {
	if (!IsChanChar(*cn))
      	    return 0; 
    }

    return 1;
}

/*
 * make_channel() free_channel()
 * functions to maintain blockheap of channels.
 */

aChannel *make_channel(void)
{
    aChannel *chan;

    chan = (aChannel *) BlockHeapAlloc(free_channels);

    if (chan == NULL)
	outofmemory("Make channel");

    memset((char *) chan, '\0', sizeof(aChannel));

    return chan;
}

void free_channel(aChannel *chan)
{
    BlockHeapFree(free_channels, chan);
}

/*
 * Look for ptr in the linked list pointed to by link.
 */
aClient *find_user_member(aChannel *chptr, aClient *acptr)
{
    dlink_node *ptr;
    struct ChanMember *cm;

    if (acptr) {
	for (ptr = chptr->members.head; ptr; ptr = ptr->next) {
	    cm = ptr->data;
	    if (cm->client_p == acptr)
		return acptr;
	}
    }
    return NULL;
}

/*
 * *  Get Channel block for chname (and allocate a new channel *
 * block, if it didn't exist before).
 */
aChannel *create_channel(aClient *cptr, char *chname, int *new, int recurse)
{
    aChannel *chptr;
    int len;
    int anew;

    if (BadPtr(chname))
	return NULL;

    len = strlen(chname);
    if (MyClient(cptr) && len > CHANNELLEN) {
	len = CHANNELLEN;
	*(chname + CHANNELLEN) = '\0';
    }
    if ((chptr = find_channel(chname))) {
	anew = 0;
	*new = anew;
	if (recurse && IsChanLinked(chptr))
	    return create_channel(cptr, chptr->mode.link, new, 0);
	return chptr;
    }
    chptr = make_channel();
    strlcpy_irc(chptr->chname, chname, len);
    if (channel)
	channel->prevch = chptr;
    chptr->prevch = NULL;
    chptr->nextch = channel;
    channel = chptr;
    chptr->tsval = timeofday;
    add_to_channel_hash_table(chname, chptr);
    chptr->mode.mode = 0;
    chptr->mode.limit = 0;
    chptr->mode.key[0] = '\0';
    Count.chan++;
    anew = 1;
    *new = anew;
    return chptr;
}

/*
 * adds a user to a channel by adding another link to the channels
 * member chain.
 */
int add_user_to_channel(aChannel *chptr, aClient *who, int flags)
{
    dlink_node *ptr;
    struct hook_data thisdata;
    struct ChanMember *cm;

    thisdata.client_p = who;
    thisdata.channel = chptr;
    thisdata.check = flags;

    if (who->user) {
	chptr->users++;
	ptr = make_dlink_node();
	cm = (struct ChanMember *) BlockHeapAlloc(chan_members);
	if (cm == NULL) {
	    outofmemory("new chanmember");
	    return 0;
	}
	memset(cm, 0, sizeof(struct ChanMember));
	cm->client_p = who;
	cm->flags = flags;
	cm->bans = 0;
	dlinkAdd(cm, ptr, &chptr->members);

	ptr = make_dlink_node();
	dlinkAdd(chptr, ptr, &who->user->channel);
	who->user->joined++;
	return 1;
    }
    return 0;
}

void remove_chanmember(aClient *acptr, aChannel *chptr)
{
    dlink_node *ptr, *next_ptr;
    struct ChanMember *cm;

    for (ptr = chptr->members.head; ptr; ptr = next_ptr) {
        next_ptr = ptr->next;
        cm = ptr->data;
	if (!cm) 
	    dlinkDeleteNode(ptr, &chptr->members);
        else if (acptr == cm->client_p) {
            dlinkDelete(ptr, &chptr->members);
	    BlockHeapFree(chan_members, cm);
	    free_dlink_node(ptr);
            return;
        }
    }
}

/*
 * *  Subtract one user from channel i (and free channel *  block, if
 * channel became empty).
 */
static void sub1_from_channel(aChannel *chptr)
{
    dlink_node *tmp;
    struct hook_data thisdata;

    if (--chptr->users <= 0) {
	tmp = chptr->invites.head;
	while (tmp && (tmp = tmp->next))
	    del_invite(tmp->data, chptr);

	thisdata.channel = chptr;

	hook_call_event(hookid_sub_from_channel, &thisdata);

	if (chptr->prevch)
	    chptr->prevch->nextch = chptr->nextch;
	else
	    channel = chptr->nextch;
	if (chptr->nextch)
	    chptr->nextch->prevch = chptr->prevch;
	del_from_channel_hash_table(chptr->chname, chptr);
	free_channel(chptr);
	Count.chan--;
    }
}

void remove_user_from_channel(aClient *who, aChannel *chptr)
{
    dlink_node *ptr;

    remove_chanmember(who, chptr);

    if (!who->user)
	return;

    if ((ptr = dlinkFind(&who->user->channel, chptr)))
	dlinkDeleteNode(ptr, &who->user->channel);

    who->user->joined--;
    sub1_from_channel(chptr);
}

int can_send(aClient *cptr, aChannel *chptr, char *msg)
{
    int member;
    int h = 0;
    int flags = 0;
    aClient *acptr;
    struct hook_data thisdata;

    if (IsServer(cptr) || IsULine(cptr))
	return 0;
    if (IsChanAnon(chptr))
	return ERR_CANNOTSENDTOCHAN;
    if (IsAnOper(cptr))
	return 0;
    if (!MyClient(cptr))
	return 0;

    member = (acptr = find_user_member(chptr, cptr)) ? 1 : 0;

    if (member)
	flags = get_flags(cptr, chptr);

    thisdata.channel = chptr;
    thisdata.client_p = cptr;
    thisdata.data = msg;
    thisdata.check = member;

    if (!member) {
	h = hook_call_event(hookid_can_send, &thisdata);
	if (h)
	    return h;
	if (is_nuhed(cptr, &chptr->banlist)) {
	    if (is_nuhed(cptr, &chptr->banexlist))
		return 0;
	    else
		return ERR_BANNEDINCHAN;
	}
	if (is_nuhed(cptr, &chptr->stoplist))
	    return ERR_CANNOTSENDTOCHAN;

    } else if (!flags & CHFL_SPEAKABLE) {
	h = hook_call_event(hookid_can_send, &thisdata);
	if (h)
	    return h;
	if (get_bans(acptr, chptr) || is_nuhed(cptr, &chptr->banlist)) {
	    if (is_nuhed(cptr, &chptr->banexlist))
		return 0;
	    else
		return ERR_BANNEDINCHAN;
	}
	if (is_nuhed(cptr, &chptr->stoplist)) {
	    if (is_nuhed(cptr, &chptr->banexlist))
		return 0;
	    else
		return ERR_HOSTMODERATED;
	}
        if (!IsChanHidePartQuit(chptr) && msg) {
            struct hook_data anotherdata;
            char *parv[3];
            parv[0] = cptr->name;
            parv[1] = chptr->chname;
            parv[2] = msg;
            anotherdata.channel = chptr;  
            anotherdata.parc = 3;
            anotherdata.parv = parv;
            anotherdata.source_p = cptr;
            if (hook_call_event(hookid_can_send_more, &anotherdata) != 0)
		return ERR_CANNOTSENDTOCHAN;
        }
    }
    return 0;
}

int can_join(aClient *sptr, aChannel *chptr, char *key)
{
    dlink_node *lp;
    int invited = 0;
    aChannel *chptr2;
    aNUH *tmp;
    dlink_node *ptr;
    struct hook_data thisdata;

    if (!sptr->user)
	return -2;

    for (lp = sptr->user->invited.head; lp; lp = lp->next) {
	chptr2 = lp->data;
	if (chptr2 == chptr) {
	    invited = 1;
	    break;
	}
    }
    if (invited || IsULine(sptr) || IsAnOper(sptr) || is_nuhed(sptr, &chptr->invitelist))
	return 0;

    if (is_nuhed(sptr, &chptr->banlist)) {
	if (is_nuhed(sptr, &chptr->banexlist))
	    return 0;
	else
	    return ERR_BANNEDFROMCHAN;
    }

    chptr2 = NULL;
    for (ptr = chptr->chanbanlist.head; ptr; ptr = ptr->next) {
	tmp = ptr->data;
	if (!tmp)
	    continue;
	chptr2 = find_channel(tmp->nuhstr);

	/* We do not do link traversal here, because when
	 * can_join is called, the destination channel is
	 * already found. -TimeMr14C
	 */

	if (chptr2)
	    if (IsMember(sptr, chptr2))
		return ERR_ISINBADCHAN;
    }

    thisdata.channel = chptr;
    thisdata.source_p = sptr;
    thisdata.data = key;

    return hook_call_event(hookid_can_join, &thisdata);
}

void send_topic_burst(aClient *cptr)
{
    aChannel *chptr;
    aClient *acptr;
    for (chptr = channel; chptr; chptr = chptr->nextch) {
	if ((*chptr->chname == '#') && (chptr->topic[0] != '\0'))
	    sendto_one_server(cptr, &me, TOK1_TOPIC, "%H %s %ld :%s",
			      chptr, chptr->topic_nick, chptr->topic_time, chptr->topic);
    }
    for (acptr = GlobalClientList; acptr; acptr = acptr->next) {
	if (!IsPerson(acptr) || acptr->from == cptr)
	    continue;
	if (acptr->user->away)
	    sendto_one_server(cptr, acptr, TOK1_AWAY, ":%s", acptr->user->away);
    }
}

/* add the invite information to the users list of invited channels,
 * and also to the channels invited users list.
 * do the same removals by del_invite
 */

void add_invite(aClient *cptr, aChannel *chptr)
{
    dlink_node *inv;
    del_invite(cptr, chptr);

    inv = make_dlink_node();
    dlinkAdd(cptr, inv, &chptr->invites);

    inv = make_dlink_node();
    dlinkAdd(chptr, inv, &cptr->user->invited);
}

void del_invite(aClient *cptr, aChannel *chptr)
{
    dlink_node **inv, *tmp;
    for (inv = &(chptr->invites.head); (tmp = *inv); inv = &tmp->next)
	if (tmp->data == cptr) {
	    dlinkDeleteNode(tmp, &chptr->invites);
	    break;
	}

    for (inv = &(cptr->user->invited.head); (tmp = *inv); inv = &tmp->next)
	if (tmp->data == chptr) {
	    dlinkDeleteNode(tmp, &cptr->user->invited);
	    break;
	}
}

void send_names(aClient *sptr, aChannel *chptr)
{
    struct ChanMember *cm;
    int mlen = strlen(me.name) + NICKLEN + 7;
    int idx, flag = 1, spos;
    int f = 0;
    int uflags = 0;
    char *s;
    aClient *acptr;
    dlink_node *ptr;

    int m = IsMember(sptr, chptr);
    int o = IsSeeHidden(sptr);
    int member = (o || IsService(sptr) || m);

    /* The omembr thing does the following: 
     * it enables an operator to see ops/voices of a channel WHEN they are not
     * in that channel AND the channel is +x (hideops)
     * But, when they join the channel, the get the .@%+ things hidden in their
     * names reply.
     *
     * I have been told, that there were opers who /names a channel before they
     * join the channel. They should get informed who might be operator etc.
     * They also can see in /whois or /who if a person is an op in that channel.
     *
     * If they actually join a channel, they should be treated as normal users
     * who join a channel. Therefore, if the channel is hiding ops, opers
     * should also get the hidden names list.
     *
     * -TimeMr14C 13/02/2002
     */

    int omembr = (o && !m);

    if (PubChannel(chptr))
	buf[0] = '=';
    else if (SecretChannel(chptr))
	buf[0] = '@';
    else
	buf[0] = '*';

    idx = 1;
    buf[idx++] = ' ';
    for (s = chptr->chname; *s; s++)
	buf[idx++] = *s;
    buf[idx++] = ' ';
    buf[idx++] = ':';

    /*
     * If we go through the following loop and never add anything,
     * we need this to be empty, otherwise spurious things from the
     * LAST /names call get stuck in there.. - lucas
     */

    buf[idx] = '\0';
    spos = idx;			/* starting point in buffer for names! */
    uflags = get_flags(sptr, chptr);
    for (ptr = chptr->members.head; ptr; ptr = ptr->next) {
	cm = ptr->data;
	if (!cm)
	    continue;
	acptr = cm->client_p;
	f = cm->flags;
	if (IsAnon(acptr))
	    continue;
	if (IsInvisible(acptr) && !member)
	    continue;
	if (IsChanAnon(chptr) && (sptr != acptr))
	    continue;
	if (!IsChanHideOps(chptr) || omembr || uflags) {
	    if (f & CHFL_OWNER)
		buf[idx++] = '.';
	    if (f & CHFL_CHANOP)
		buf[idx++] = '@';
	    if (f & CHFL_HALFOP)
		buf[idx++] = '%';
	    if (f & CHFL_VOICE)
		buf[idx++] = '+';
	}
	for (s = acptr->name; *s; s++)
	    buf[idx++] = *s;
	buf[idx++] = ' ';
	buf[idx] = '\0';
	flag = 1;
	if (mlen + idx + NICKLEN > BUFSIZE - 3) {
	    send_me_numeric(sptr, RPL_NAMREPLY, buf);
	    idx = spos;
	    flag = 0;
	}
    }

    if (flag)
	send_me_numeric(sptr, RPL_NAMREPLY, buf);
    send_me_numeric(sptr, RPL_ENDOFNAMES, chptr->chname);
}

void privileged_join(aClient *sptr, aClient *cptr, char *name)
{
    aChannel *chptr;
    int i = 0, flags = 0;
    int n = 0;
    if ((sptr->user->joined >= ChannelConf.max_channels_per_user)
	&& (!IsAnOper(sptr) || (sptr->user->joined >= ChannelConf.max_channels_per_user * 3))) {
	send_me_numeric(sptr, ERR_TOOMANYCHANNELS, name);
	return;
    }
    chptr = create_channel(sptr, name, &n, 1);
    flags = n ? CHFL_CHANOP : 0;
    if (MyConnect(sptr) && (i = can_join(sptr, chptr, NULL))) {
	send_me_numeric(sptr, i, name);
	return;
    }
    if (IsMember(sptr, chptr))
	return;
    add_user_to_channel(chptr, sptr, flags);
    if (MyClient(sptr) && (flags == CHFL_CHANOP)) {
	chptr->tsval = timeofday;
	sendto_serv_butone(NULL, &me, TOK1_SJOIN, "%T %H +%s :@%~C", chptr, chptr,
			   ((ChannelConf.default_extended_topic_limitation) ? "Tn" : "tn"), sptr);
    } else {
	sendto_serv_butone(NULL, sptr, TOK1_SJOIN, "%T %H", chptr, chptr);
    }
    sendto_service(SERVICE_SEE_JOINS, 0, sptr, chptr, TOK1_JOIN, "");
    sendto_channel_butserv_short(chptr, sptr, TOK1_JOIN);
    if (flags == CHFL_CHANOP) {
        char *cmodes_sm = NULL;
	if (ChannelConf.default_extended_topic_limitation)
            cmodes_sm = "+Tn";
        else
            cmodes_sm = "+tn";
	sendto_channel_butserv(chptr, sptr, TOK1_MODE, 0, "%s", cmodes_sm);
	chptr->mode.mode |= (ChannelConf.default_extended_topic_limitation ? MODE_EXTOPIC : MODE_TOPICLIMIT);
	chptr->mode.mode |= MODE_NOPRIVMSGS;
    }
    if (MyClient(sptr)) {
	del_invite(sptr, chptr);
	if (chptr->topic[0] != '\0') {
	    send_me_numeric(sptr, RPL_TOPIC, name, chptr->topic);
	    send_me_numeric(sptr, RPL_TOPICWHOTIME, name,
			    IsChanHideOps(chptr) ? chptr->
			    chname : chptr->topic_nick, chptr->topic_time);
	}
	send_names(sptr, chptr);
    }
}

/* check_splitmode()
 *
 * input        -
 * output       -
 * side effects - compares usercount and servercount against their split
 *                values and adjusts splitmode accordingly
 */
void check_splitmode(void *unused)
{
    GeneralOpts.split = 0;
    eventDelete(check_splitmode, NULL);
}

/*
 * The function which sends the actual channel list back to the user.
 * Operates by stepping through the hashtable, sending the entries back if
 * they match the criteria.
 * cptr = Local client to send the output back to.
 * numsend = Number (roughly) of lines to send back. Once this number has
 * been exceeded, send_list will finish with the current hash bucket,
 * and record that number as the number to start next time send_list
 * is called for this user. So, this function will almost always send
 * back more lines than specified by numsend (though not by much,
 * assuming HASHSIZE is was well picked). So be conservative in your choice
 * of numsend. -Rak
 */

/*
 * This function is now changed, there is no limiting via the numsend.
 * The flood case is prevented due to the new linebuf architecture.
 * We will push the whole list to the client.
 * -TimeMr14C
 */

void send_list(aClient *cptr, LOpts *lopt)
{
    aChannel *chptr;
    dlink_node *lp, *next_lp;
    int hashnum;

    for (hashnum = lopt->starthash; hashnum < HASHSIZE; hashnum++) {
        for (chptr = (aChannel *) hash_get_chan_bucket(hashnum); chptr; chptr = chptr->hnextch) {
	    lopt->starthash = hashnum;
            if (SecretChannel(chptr) && !IsMember(cptr, chptr) && !IsSeeHidden(cptr))
                continue;
            if ((!lopt->showall) &&
                ((chptr->users < lopt->usermin) ||
                 ((lopt->usermax >= 0) && (chptr->users > lopt->usermax)) || ((chptr->
                                                                               tsval ||
                                                                               1) <
                                                                              lopt->chantimemin)
                 || (chptr->topic_time < lopt->topictimemin) ||
                 (chptr->tsval > lopt->chantimemax) ||
                 (chptr->topic_time > lopt->topictimemax) || (lopt->nolist.head &&
                                                              find_str_link(&lopt->nolist,
                                                                            chptr->chname))
                 || (lopt->yeslist.head && !find_str_link(&lopt->yeslist, chptr->chname))))
                continue;
	    if (will_exceed_sendq(cptr))
		return;
            send_me_numeric(cptr, RPL_LIST,
                            ShowChannel(cptr, chptr) ? chptr->chname : "*",
                            chptr->users, ShowChannel(cptr, chptr) ? chptr->topic : "");
        }
    }
    send_me_numeric(cptr, RPL_LISTEND);
    for (lp = lopt->yeslist.head; lp; lp = next_lp) {
        next_lp = lp->next;
        dlinkDeleteNode(lp, &lopt->yeslist);
    }
    for (lp = lopt->nolist.head; lp; lp = next_lp) {
        next_lp = lp->next;
        dlinkDeleteNode(lp, &lopt->nolist);
    }
    lopt->starthash = 0;
    return;
}

