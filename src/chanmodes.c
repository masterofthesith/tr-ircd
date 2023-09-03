/*
 *   IRC - Internet Relay Chat, chanmodes/chanmodes.c
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
 */

/*
 * $Id: chanmodes.c,v 1.7 2004/02/24 19:03:33 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "channel.h"
#include "h.h"
#include "hook.h"
#include "msg.h"
#include "s_conf.h"
#include "chanmode.h"
#include "cmodetab.h"

static char buf[BUFSIZE];
static char modebuf[REALMODEBUFLEN], parabuf[REALMODEBUFLEN];
static char modebuf2[REALMODEBUFLEN], parabuf2[REALMODEBUFLEN];

char allchanmodes[60];		/* we do not have more than 60 now */
char paramchanmodes[30];
char chanmodelist[60];

static int hookid_send_modes = 0;

void init_chanmodes(void *unused)
{
    hookid_send_modes = hook_add_event("send mode list");
}

int add_id(aClient *cptr, aChannel *chptr, char *id, int maxnums, dlink_list * list)
{
    int cnt = 0;
    dlink_node *ptr;
    aNUH *nuhptr;

    for (ptr = list->head; ptr; ptr = ptr->next) {
	nuhptr = ptr->data;
	if (MyClient(cptr) && (++cnt >= maxnums)) {
	    send_me_numeric(cptr, ERR_BANLISTFULL, chptr, id);
	    return 0;
	} else if (nuhptr && !match(nuhptr->nuhstr, id))
	    return 0;
    }

    nuhptr = (aNUH *) MyMalloc(sizeof(aNUH));
    nuhptr->nuhstr = (char *) MyMalloc(strlen(id) + 1);
    DupString(nuhptr->nuhstr, id);

    if (IsPerson(cptr) && !IsChanHideOps(chptr)) {
	nuhptr->who = (char *) MyMalloc(strlen(cptr->name) + strlen(cptr->user->username) +
					strlen(IsFake(cptr) ? cptr->user->fakehost :
					       cptr->user->host) + 3);
	ircsprintf(nuhptr->who, "%C", cptr);
    } else if (IsChanHideOps(chptr)) {
	nuhptr->who = (char *) MyMalloc(strlen(chptr->chname) + 1);
	strcpy(nuhptr->who, chptr->chname);
    } else {
	nuhptr->who = (char *) MyMalloc(strlen(cptr->name) + 1);
	strcpy(nuhptr->who, cptr->name);
    }

    if (id[0] == '*' && id[1] == '!') {
	if (id[2] == '*' && id[3] == '@')
	    nuhptr->type = MTYP_HOST;
	else
	    nuhptr->type = MTYP_USERHOST;
    } else if (id[0] == '#')
	nuhptr->type = MTYP_CHANNEL;
    else
	nuhptr->type = MTYP_FULL;

    nuhptr->when = timeofday;

    ptr = make_dlink_node();
    dlinkAdd(nuhptr, ptr, list);

    return 1;
}

int del_id(char *id, dlink_list * list)
{
    aNUH *nuhptr;
    dlink_node *ptr, *next_ptr;

    if (!id)
	return -1;

    for (ptr = list->head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	nuhptr = ptr->data;
	if (!nuhptr)
	    continue;
	if (irc_strcmp(id, nuhptr->nuhstr) == 0) {
	    MyFree(nuhptr->nuhstr);
	    MyFree(nuhptr->who);
	    MyFree(nuhptr);
	    dlinkDelete(ptr, list);
	    free_dlink_node(ptr);
	    break;
	}
    }
    return 1;
}

void remove_matching_nuhs(aChannel *chptr, aClient *cptr, aClient *from,
			  dlink_list * list, char letter)
{
    aNUH *tmpA;
    dlink_node *ptr, *next_ptr;
    char targhost[NICKLEN + USERLEN + HOSTLEN + 6];
    char fakeuhost[NICKLEN + USERLEN + HOSTLEN + 6];
    char targip[NICKLEN + USERLEN + HOSTLEN + 6];
    char *m;
    int count = 0, send = 0;

    if (!IsPerson(cptr))
	return;

    strcpy(targhost, make_nick_user_host(cptr->name, cptr->user->username, cptr->user->host));
    strcpy(fakeuhost, make_nick_user_host(cptr->name, cptr->user->username, cptr->user->fakehost));
    strcpy(targip, make_nick_user_host(cptr->name, cptr->user->username, cptr->hostip));

    m = modebuf;
    *m++ = '-';
    *m = '\0';
    *parabuf = '\0';

    for (ptr = list->head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	tmpA = ptr->data;
	if (!tmpA)
	    continue;
	if (!tmpA->nuhstr)
	    continue;
	if ((match(tmpA->nuhstr, targhost) == 0)
	    || (match(tmpA->nuhstr, targip) == 0)
	    || (match(tmpA->nuhstr, fakeuhost) == 0)
	    ) {
	    if (strlen(parabuf) + strlen(tmpA->nuhstr) + 10 < (size_t) MODEBUFLEN) {
		if (*parabuf)
		    strcat(parabuf, " ");
		strcat(parabuf, tmpA->nuhstr);
		count++;
		*m++ = letter;
		*m = '\0';
	    } else if (*parabuf)
		send = 1;
	    if (count == MAXMODEPARAMS)
		send = 1;
	    if (send) {
		sendto_channel_butserv(chptr, from, TOK1_MODE, 0, "%s %s", modebuf, parabuf);
		sendto_serv_butone(from, from, TOK1_TMODE, "%H %T %s %s",
					chptr, chptr, modebuf, parabuf);
		send = 0;
		*parabuf = '\0';
		m = modebuf;
		*m++ = '-';
		if (count != MAXMODEPARAMS) {
		    strcpy(parabuf, tmpA->nuhstr);
		    *m++ = letter;
		    count = 1;
		} else
		    count = 0;
		*m = '\0';
	    }
	    MyFree(tmpA->nuhstr);
	    MyFree(tmpA->who);
	    MyFree(tmpA);
	    dlinkDelete(ptr, list);
	    free_dlink_node(ptr);
	}
    }

    if (*parabuf) {
	sendto_channel_butserv(chptr, from, TOK1_MODE, 0, "%s %s", modebuf, parabuf);
	sendto_serv_butone(from, from, TOK1_TMODE, "%H %T %s %s", chptr, chptr,
				modebuf, parabuf);
    }

    return;
}

void remove_nuh_list(aClient *cptr, aChannel *chptr, dlink_list * list, char letter)
{
    aNUH *bp = NULL;
    dlink_node *ptr, *next_ptr;
    char *cp;
    int count = 0, send = 0;
    cp = modebuf;
    *cp++ = '-';
    *cp = '\0';
    *parabuf = '\0';

    for (ptr = list->head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	bp = ptr->data;
	if (!bp)
	    continue;
	if (strlen(parabuf) + strlen(bp->nuhstr) + 10 < (size_t) MODEBUFLEN) {
	    if (*parabuf)
		strcat(parabuf, " ");
	    strcat(parabuf, bp->nuhstr);
	    count++;
	    *cp++ = letter;
	    *cp = '\0';
	} else if (*parabuf)
	    send = 1;
	if (count == MAXMODEPARAMS)
	    send = 1;
	if (send) {
	    sendto_channel_butserv(chptr, &me, TOK1_MODE, 0, "%s %s", modebuf, parabuf);
	    send = 0;
	    *parabuf = '\0';
	    cp = modebuf;
	    *cp++ = '-';
	    if (count != MAXMODEPARAMS) {
		strcpy(parabuf, bp->nuhstr);
		*cp++ = letter;
		count = 1;
	    } else
		count = 0;
	    *cp = '\0';
	}
	MyFree(bp->nuhstr);
	MyFree(bp->who);
	MyFree(bp);
	dlinkDelete(ptr, list);
	free_dlink_node(ptr);
    }

    if (*parabuf) {
	sendto_channel_butserv(chptr, &me, TOK1_MODE, 0, "%s %s", modebuf, parabuf);
    }

}

void send_nuh_list(aClient *cptr, aChannel *chptr, dlink_list * list,
		   char letter, char *mbuf, char *pbuf)
{
    char *cp;
    dlink_node *ptr;
    aNUH *bp;
    int count = 0, send = 0;
    cp = mbuf + strlen(mbuf);
    if (*pbuf)			/* mode +l or +k xx */
	count = 1;

    for (ptr = list->head; ptr; ptr = ptr->next) {
	bp = ptr->data;
	if (!bp)
	    continue;
	if (strlen(pbuf) + strlen(bp->nuhstr) + 10 < (size_t) MODEBUFLEN) {
	    if (*pbuf)
		strcat(pbuf, " ");
	    strcat(pbuf, bp->nuhstr);
	    count++;
	    *cp++ = letter;
	    *cp = '\0';
	} else if (*pbuf)
	    send = 1;
	if (count == MAXMODEPARAMS)
	    send = 1;
	if (send) {
	    sendto_one_server(cptr, &me, TOK1_TMODE, "%H %T %s %s", chptr, chptr, mbuf, pbuf);
	    send = 0;
	    *pbuf = '\0';
	    cp = mbuf;
	    *cp++ = '+';
	    if (count != MAXMODEPARAMS) {
		strcpy(pbuf, bp->nuhstr);
		*cp++ = letter;
		count = 1;
	    } else
		count = 0;
	    *cp = '\0';
	}
    }
    if (mbuf[1] || *pbuf) {
	sendto_one_server(cptr, &me, TOK1_TMODE, "%H %T %s %s", chptr, chptr, mbuf, pbuf);
    }

}

aNUH *is_nuhed(aClient *cptr, dlink_list * list)
{
    char s[NICKLEN + USERLEN + HOSTLEN + 6];
    char fakeuhost[NICKLEN + USERLEN + HOSTLEN + 6];
    char *s2;
    dlink_node *ptr;
    aNUH *tmp;

    if (!IsPerson(cptr))
	return NULL;

    strcpy(s, make_nick_user_host(cptr->name, cptr->user->username, cptr->user->host));
    s2 = make_nick_user_host(cptr->name, cptr->user->username, cptr->hostip);
    strcpy(fakeuhost, make_nick_user_host(cptr->name, cptr->user->username, cptr->user->fakehost));
    for (ptr = list->head; ptr; ptr = ptr->next) {
	tmp = ptr->data;
	if (!tmp)
	    continue;
	if (!tmp->nuhstr)
	    continue;
	if ((match(tmp->nuhstr, s) == 0) || (match(tmp->nuhstr, s2) == 0) ||
	    (match(tmp->nuhstr, fakeuhost) == 0))
	    return (tmp);
    }
    return NULL;
}

/* This function checks, if the attempted new /nick is denied
 * by a nuhptr
 * -TimeMr14C
 */

aNUH *nick_is_nuhed(aChannel *chptr, char *nick, aClient *cptr, dlink_list * list)
{
    dlink_node *ptr;
    aNUH *tmp = NULL;
    char *s, s2[NICKLEN + USERLEN + HOSTLEN + 6];
    char fakeuhost[NICKLEN + USERLEN + HOSTLEN + 6];

    if (!IsPerson(cptr))
	return NULL;
    strcpy(s2, make_nick_user_host(nick, cptr->user->username, cptr->user->host));
    strcpy(fakeuhost, make_nick_user_host(nick, cptr->user->username, cptr->user->fakehost));
    s = make_nick_user_host(nick, cptr->user->username, cptr->hostip);

    for (ptr = list->head; ptr; ptr = ptr->next) {
	tmp = ptr->data;
	if (!tmp)
	    continue;
	if (!tmp->nuhstr)
	    continue;
	if (tmp->type == MTYP_FULL &&	/* only check applicable bans */
	    ((match(tmp->nuhstr, s2) == 0) ||	/* check host before IP */
	     (match(tmp->nuhstr, s) == 0) || (match(tmp->nuhstr, fakeuhost) == 0)))
	    return tmp;
    }
    return NULL;
}

/*
 * write the "simple" list of channel modes for channel chptr onto
 * buffer mbuf with the parameters in pbuf.
 */
void channel_modes(aClient *cptr, char *mbuf, char *pbuf, aChannel *chptr, int operation)
{
    int i = 0;
    if (operation == 1) {
	*mbuf++ = '+';
	for (i = 0; i < 128; i++) {
	    if (modetab[i].in_use && !(modetab[i].flags & MFLG_IGNORE)) {
		if (chptr->mode.mode & modetab[i].type) {
		    if (modetab[i].flags & MFLG_PARAMNUM) {
    char tmp[9];
    int len;
			*mbuf++ = (char) i;
			if (IsMember(cptr, chptr) || IsPrivileged(cptr)) {
			    len = ircsprintf(tmp, " %d", (i == (int) 'l' ? chptr->mode.limit : chptr->mode.joindelay));
			    strncat(pbuf, tmp, len);
			}
		    } else if (modetab[i].flags & MFLG_PARAMSTR) {
    char k[KEYLEN + 1];
    int len;
			*mbuf++ = (char) i;
			if (IsMember(cptr, chptr) || IsPrivileged(cptr)) {
			    len = ircsprintf(k, " %s", chptr->mode.key);
			    strncat(pbuf, k, len);
			}
		    } else if (modetab[i].flags & MFLG_PARAMDBL) {
    char t[20];
    int len;
			*mbuf++ = (char) i;
			if (IsMember(cptr, chptr) || IsPrivileged(cptr)) {
			    len = ircsprintf(t, " %d:%d", chptr->mode.lines, chptr->mode.intime);
			    strncat(pbuf, t, len);
			}
		    } else if (modetab[i].flags & MFLG_PARAMCHN) {
    char L[CHANNELLEN + 1];
    int len;
  		        *mbuf++ = (char) i;
			if (IsMember(cptr, chptr) || IsPrivileged(cptr)) {
			    len = ircsprintf(L, " %s", chptr->mode.link);
			    strncat(pbuf, L, len);
			}
		    } else {
			*mbuf++ = (char) i;
		    }
		}
	    }
	}
	*mbuf++ = '\0';
    } else if (operation == 2) {
	/* for a future plan eventually */
    }
    return;
}

/*
 * send "cptr" a full list of the modes for channel chptr. 
 */
void send_channel_modes(aClient *cptr, aChannel *chptr)
{
    struct ChanMember *cm;
    dlink_node *ptr;
    aClient *acptr;
    int n = 0;
    int len;
    int f;
    char *t;
    struct hook_data thisdata;

    if (*chptr->chname != '#')
	return;

    *modebuf = *parabuf = '\0';
    channel_modes(cptr, modebuf, parabuf, chptr, 1);
    len = ircsprintf(buf, "%T %H %s%s :", chptr, chptr, modebuf, parabuf);
    t = buf + len;

    for (ptr = chptr->members.head; ptr; ptr = ptr->next) {
	cm = ptr->data;
	if (!cm)
	    continue;
	acptr = cm->client_p;
	if (acptr == NULL)
	    continue;
	if (IsAnon(acptr))
	    continue;
	f = cm->flags;
	if (f & CHFL_OWNER)
	    *t++ = '.';
	if (f & CHFL_PROTECT)
	    *t++ = '~';
	if (f & CHFL_HALFOP)
	    *t++ = '%';
	if (f & CHFL_CHANOP)
	    *t++ = '@';
	if (f & CHFL_VOICE)
	    *t++ = '+';
	f = 0;
	strcpy(t, acptr->name);
	t += strlen(t);
	*t++ = ' ';
	n++;
	if (t - buf > BUFSIZE - 80) {
	    *t++ = '\0';
	    if (t[-1] == ' ')
		t[-1] = '\0';
	    sendto_one_server(cptr, &me, TOK1_SJOIN, "%s", buf);
	    len = ircsprintf(buf, "%T %H 0 :", chptr, chptr);
	    t = buf + len;
	    n = 0;
	}
    }

    if (n) {
	*t++ = '\0';
	if (t[-1] == ' ')
	    t[-1] = '\0';
	sendto_one_server(cptr, &me, TOK1_SJOIN, "%s", buf);
    }

    thisdata.client_p = cptr;
    thisdata.channel = chptr;

    hook_call_event(hookid_send_modes, &thisdata);

}

void link_add_server_to_channel(aClient *cptr, aChannel *chptr)
{
    add_user_to_channel(chptr, cptr, CHFL_CHANOP | CHFL_OWNER);
}

void link_remove_users_from_channel(aChannel *chptr)
{
    struct ChanMember *cm;
    dlink_node *ptr, *next_ptr;
    aClient *acptr;

    for (ptr = chptr->members.head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	cm = ptr->data;
	if (!cm)
	    continue;
	acptr = cm->client_p;
	if (acptr) {
	    if (MyClient(acptr) && !IsMe(acptr)) {
		sendto_one_person(acptr, acptr, TOK1_PART, ":%H", chptr);
		sendto_match_servs(chptr, acptr, TOK1_PART, "");
		remove_user_from_channel(acptr, chptr);
	    }
	}
    }
}

void link_set_modes_in_channel(aChannel *chptr)
{
    chptr->mode.mode &= ~MODE_KEY;
    chptr->mode.mode &= ~MODE_FLOOD;
    chptr->mode.limit = 0;
    chptr->mode.key[0] = '\0';
    chptr->mode.lines = 0;
    chptr->mode.intime = 0;
    chptr->mode.mode |= MODE_EXTOPIC;
    chptr->mode.mode |= MODE_NOPRIVMSGS;
    chptr->mode.mode |= MODE_SECRET;
    chptr->mode.mode |= MODE_PRIVATE;
    chptr->mode.mode |= MODE_LIMIT;
    chptr->mode.limit = 2;
}

int
set_mode(aClient *cptr, aClient *sptr, aChannel *chptr,
	 int parc, char *parv[], char *mbuf, char *pbuf)
{
    struct ChanMode *modetab_item;

    int adl = CMODE_LIST;

    char *modes = parv[0];	/* user's idea of mode changes */
    int args;			/* counter for what argument we're on */
    int maxmodes = ((IsServer(sptr) || IsULine(sptr)) ? 512 : MAXMODEPARAMS);
    int nmodes = 0;		/* how many modes we've set so far */
    int pidx = 0;		/* index into pbuf */
    int mbix = 0;
    int s;

    args = 1;

    if (parc < 1)
	return 0;

    mbuf[mbix++] = '+';

    mbuf[mbix] = '\0';
    pbuf[pidx] = '\0';

    /*
     * go through once to clean the user's mode string so we can
     * * have a simple parser run through it...
     */

    while (*modes && (nmodes < maxmodes)) {
	s = (u_char) *modes;
	if (*modes == '+') {
	    if (mbuf[(mbix - 1)] == '-') {
		mbuf[(mbix - 1)] = '+';	/* change it around now */
	    } else if (adl == CMODE_DEL)
		mbuf[mbix++] = '+';
	    adl = CMODE_ADD;

	} else if (*modes == '-') {
	    if (mbuf[(mbix - 1)] == '+') {
		mbuf[(mbix - 1)] = '-';	/* change it around now */
	    } else if (adl == CMODE_ADD)
		mbuf[mbix++] = '-';
	    adl = CMODE_DEL;

	} else if ((s > 31) && (s < 128)) {
	    modetab_item = &modetab[(u_char) *modes];
	    if (!(modetab_item->in_use)) {
		if (MyClient(sptr))
		    send_me_numeric(sptr, ERR_UNKNOWNMODE, *modes);
	    } else {
		if ((parc == 1) && (modetab_item->flags & MFLG_LISTABLE))
		    adl = CMODE_LIST;
		if (IsService(sptr)) {
		    nmodes = (*modetab_item->cmode_handler_service) (adl, chptr,
								     nmodes,
								     &args, &pidx,
								     &mbix, mbuf,
								     pbuf, cptr, sptr, parc, parv);
		} else if (IsULine(sptr)) {
		    nmodes = (*modetab_item->cmode_handler_ulined) (adl, chptr,
								    nmodes,
								    &args, &pidx,
								    &mbix, mbuf,
								    pbuf, cptr, sptr, parc, parv);
		} else if (IsServer(sptr)) {
		    nmodes = (*modetab_item->cmode_handler_server) (adl, chptr,
								    nmodes,
								    &args, &pidx,
								    &mbix, mbuf,
								    pbuf, cptr, sptr, parc, parv);
		} else if (IsAnOper(sptr)) {
		    nmodes =
			(*modetab_item->cmode_handler_oper) (adl, chptr, nmodes,
							     &args, &pidx, &mbix,
							     mbuf, pbuf, cptr, sptr, parc, parv);
		} else {
		    nmodes =
			(*modetab_item->cmode_handler_user) (adl, chptr, nmodes,
							     &args, &pidx, &mbix,
							     mbuf, pbuf, cptr, sptr, parc, parv);
		}
	    }
	} else {
	    if (MyClient(sptr))
		send_me_numeric(sptr, ERR_UNKNOWNMODE, *modes);
	}
	modes++;
    }

    pbuf[pidx] = '\0';

    if (mbuf[(mbix - 1)] == '+' || mbuf[(mbix - 1)] == '-')
	mbuf[(mbix - 1)] = '\0';
    else
	mbuf[mbix] = '\0';

    ClearOperMode(sptr);
    return nmodes;
}

void send_mode_burst(aClient *cptr, aChannel *chptr, char change)
{
    struct ChanMember *cm;
    char *c;
    int count = 0, send = 0;
    aClient *acptr;
    dlink_node *ptr;
    int f;

    modebuf2[0] = '\0';
    parabuf2[0] = '\0';
    c = modebuf2;

    for (ptr = chptr->members.head; ptr; ptr = ptr->next) {
	cm = ptr->data;
	if (!cm)
	    continue;
	acptr = cm->client_p;
	f = cm->flags;
	if (strlen(parabuf2) + strlen(acptr->name) + 10 < (size_t) MODEBUFLEN) {
	    if (f & CHFL_OWNER) {
		*c++ = 'u';
		if (*parabuf2)
		    strcat(parabuf2, " ");
		strcat(parabuf2, acptr->name);
		count++;
	    }
	}
	if (strlen(parabuf2) + strlen(acptr->name) + 10 < (size_t) MODEBUFLEN) {
	    if (f & CHFL_PROTECT) {
		*c++ = 'a';
		if (*parabuf2)
		    strcat(parabuf2, " ");
		strcat(parabuf2, acptr->name);
		count++;
	    }
	}
	if (strlen(parabuf2) + strlen(acptr->name) + 10 < (size_t) MODEBUFLEN) {
	    if (f & CHFL_CHANOP) {
		*c++ = 'o';
		if (*parabuf2)
		    strcat(parabuf2, " ");
		strcat(parabuf2, acptr->name);
		count++;
	    }
	}
	if (strlen(parabuf2) + strlen(acptr->name) + 10 < (size_t) MODEBUFLEN) {
	    if (f & CHFL_HALFOP) {
		*c++ = 'h';
		if (*parabuf2)
		    strcat(parabuf2, " ");
		strcat(parabuf2, acptr->name);
		count++;
	    }
	}
	if (strlen(parabuf2) + strlen(acptr->name) + 10 < (size_t) MODEBUFLEN) {
	    if (f & CHFL_VOICE) {
		*c++ = 'v';
		if (*parabuf2)
		    strcat(parabuf2, " ");
		strcat(parabuf2, acptr->name);
		count++;
	    }
	}
	if (strlen(parabuf2) + strlen(acptr->name) + 10 < (size_t) MODEBUFLEN) {
	    *c = '\0';
	} else if (*parabuf2)
	    send = 1;
	if (count == MAXMODEPARAMS)
	    send = 1;
	if (send) {
	    sendto_one(cptr, ":%H %s %H %c%s %s", chptr, MSG_MODE,
		       chptr, change, modebuf2, parabuf2);
	    send = 0;
	    *parabuf2 = '\0';
	    c = modebuf2;
	    if (count != MAXMODEPARAMS) {
		if (f & CHFL_OWNER) {
		    *c++ = 'u';
		    strcat(parabuf2, acptr->name);
		    strcat(parabuf2, " ");
		    count++;
		}
		if (f & CHFL_PROTECT) {
		    *c++ = 'a';
		    strcat(parabuf2, acptr->name);
		    strcat(parabuf2, " ");
		    count++;
		}
		if (f & CHFL_CHANOP) {
		    *c++ = 'o';
		    strcat(parabuf2, acptr->name);
		    strcat(parabuf2, " ");
		    count++;
		}
		if (f & CHFL_HALFOP) {
		    *c++ = 'h';
		    strcat(parabuf2, acptr->name);
		    strcat(parabuf2, " ");
		    count++;
		}
		if (f & CHFL_VOICE) {
		    *c++ = 'v';
		    strcat(parabuf2, acptr->name);
		    strcat(parabuf2, " ");
		    count++;
		}
	    } else
		count = 0;
	    *c = '\0';
	}
    }
    if (*parabuf2) {
	sendto_one(cptr, ":%H %s %H %c%s %s", chptr, MSG_MODE, chptr, change, modebuf2, parabuf2);
    }
    *parabuf2 = '\0';
    *modebuf2 = '\0';
}

int find_cmode(int mode, unsigned char *modelist, int size)
{
    int i = 0;
    for (i = 0; i < size; i++) {
	if (modelist[i] == mode)
	    return i;
    }
    return -1;
}

void create_channelmodelists(void)
{
    int i = 0;
    int j = 0;
    int allmode_res = 0, chanmode_res = 0, parammode_res = 0;
    int chan_counter = 0, all_counter = 0, param_counter = 0;

    unsigned char *tmpchanmodelist;
    unsigned char *tmpallchanmodes;
    unsigned char *tmpparamchanmodes;
    tmpchanmodelist = (unsigned char *) malloc(60);
    tmpallchanmodes = (unsigned char *) malloc(60);
    tmpparamchanmodes = (unsigned char *) malloc(30);
    memset(tmpchanmodelist, 0, 60);
    memset(tmpallchanmodes, 0, 60);
    memset(tmpparamchanmodes, 0, 30);

    memset(allchanmodes, 0, sizeof(allchanmodes));

    for (i = 32; i < 127; i++) {
	if (modetab[i].in_use) {
	    tmpallchanmodes[j] = i;
	    j++;
	}
    }
    tmpallchanmodes[j] = '\0';

    memset(chanmodelist, 0, sizeof(chanmodelist));
    j = 0;
    for (i = 32; i < 127; i++) {
	if (modetab[i].in_use) {
	    if (modetab[i].cmtype & CMTYPE_LIST) {
		tmpchanmodelist[j] = i;
		j++;
	    }
	}
    }

    for (i = 65; i <= 90; i++) {
	chanmode_res = find_cmode(i + 32, tmpchanmodelist, 60);
	if (chanmode_res != -1) {
	    chanmodelist[chan_counter] = tmpchanmodelist[chanmode_res];
	    chan_counter++;
	    tmpchanmodelist[chanmode_res] = 255;
	}
	chanmode_res = find_cmode(i, tmpchanmodelist, 60);
	if (chanmode_res != -1) {
	    chanmodelist[chan_counter] = tmpchanmodelist[chanmode_res];
	    chan_counter++;
	    tmpchanmodelist[chanmode_res] = 255;
	}
    }

    chanmodelist[chan_counter] = ',';
    chan_counter++;

    memset(tmpchanmodelist, 0, 60);
    j = 0;
    for (i = 32; i < 127; i++) {
	if (modetab[i].in_use) {
	    if (modetab[i].cmtype & CMTYPE_REMWPARA) {
		tmpchanmodelist[j] = i;
		j++;
	    }
	}
    }

    for (i = 65; i <= 90; i++) {
	chanmode_res = find_cmode(i + 32, tmpchanmodelist, 60);
	if (chanmode_res != -1) {
	    chanmodelist[chan_counter] = tmpchanmodelist[chanmode_res];
	    chan_counter++;
	    tmpchanmodelist[chanmode_res] = 255;
	}
	chanmode_res = find_cmode(i, tmpchanmodelist, 60);
	if (chanmode_res != -1) {
	    chanmodelist[chan_counter] = tmpchanmodelist[chanmode_res];
	    chan_counter++;
	    tmpchanmodelist[chanmode_res] = 255;
	}
    }
    chanmodelist[chan_counter] = ',';
    chan_counter++;
    memset(tmpchanmodelist, 0, 60);
    j = 0;

    for (i = 32; i < 127; i++) {
	if (modetab[i].in_use) {
	    if (modetab[i].cmtype & CMTYPE_REMWOPARA) {
		tmpchanmodelist[j] = i;
		j++;
	    }
	}
    }

    for (i = 65; i <= 90; i++) {
	chanmode_res = find_cmode(i + 32, tmpchanmodelist, 60);
	if (chanmode_res != -1) {
	    chanmodelist[chan_counter] = tmpchanmodelist[chanmode_res];
	    chan_counter++;
	    tmpchanmodelist[chanmode_res] = 255;
	}
	chanmode_res = find_cmode(i, tmpchanmodelist, 60);
	if (chanmode_res != -1) {
	    chanmodelist[chan_counter] = tmpchanmodelist[chanmode_res];
	    chan_counter++;
	    tmpchanmodelist[chanmode_res] = 255;
	}
    }
    chanmodelist[chan_counter] = ',';
    chan_counter++;
    memset(tmpchanmodelist, 0, 60);
    j = 0;

    for (i = 32; i < 127; i++) {
	if (modetab[i].in_use) {
	    if (modetab[i].cmtype & CMTYPE_SINGLE) {
		tmpchanmodelist[j] = i;
		j++;
	    }
	}
    }

    for (i = 65; i <= 90; i++) {
	chanmode_res = find_cmode(i + 32, tmpchanmodelist, 60);
	if (chanmode_res != -1) {
	    chanmodelist[chan_counter] = tmpchanmodelist[chanmode_res];
	    chan_counter++;
	    tmpchanmodelist[chanmode_res] = 255;
	}
	chanmode_res = find_cmode(i, tmpchanmodelist, 60);
	if (chanmode_res != -1) {
	    chanmodelist[chan_counter] = tmpchanmodelist[chanmode_res];
	    chan_counter++;
	    tmpchanmodelist[chanmode_res] = 255;
	}
    }
    chanmodelist[chan_counter] = '\0';

    memset(paramchanmodes, 0, sizeof(paramchanmodes));
    j = 0;
    for (i = 32; i < 127; i++) {
	if (modetab[i].in_use) {
	    if (modetab[i].cmtype & CMTYPE_PARAMETRIC) {
		tmpparamchanmodes[j] = i;
		j++;
	    }
	}
    }
    tmpparamchanmodes[j] = '\0';

    for (i = 65; i <= 90; i++) {
	allmode_res = find_cmode(i + 32, tmpallchanmodes, 60);
	parammode_res = find_cmode(i + 32, tmpparamchanmodes, 30);
	if (allmode_res != -1) {
	    allchanmodes[all_counter] = tmpallchanmodes[allmode_res];
	    all_counter++;
	    tmpallchanmodes[allmode_res] = 255;
	}
	if (parammode_res != -1) {
	    paramchanmodes[param_counter] = tmpparamchanmodes[parammode_res];
	    param_counter++;
	    tmpparamchanmodes[parammode_res] = 255;
	}

	allmode_res = find_cmode(i, tmpallchanmodes, 60);
	parammode_res = find_cmode(i, tmpparamchanmodes, 30);
	if (allmode_res != -1) {
	    allchanmodes[all_counter] = tmpallchanmodes[allmode_res];
	    all_counter++;
	    tmpallchanmodes[allmode_res] = 255;
	}
	if (parammode_res != -1) {
	    paramchanmodes[param_counter] = tmpparamchanmodes[parammode_res];
	    param_counter++;
	    tmpparamchanmodes[parammode_res] = 255;
	}

    }
    for (i = 0; i < strlen((char *) tmpallchanmodes); i++) {
	if (tmpallchanmodes[i] == 255) {
	    continue;
	} else {
	    allchanmodes[all_counter] = tmpallchanmodes[i];
	    all_counter++;
	}
    }
    MyFree(tmpallchanmodes);

    for (i = 0; i < strlen((char *) tmpparamchanmodes); i++) {
	if (tmpparamchanmodes[i] == 255) {
	    continue;
	} else {
	    paramchanmodes[param_counter] = tmpparamchanmodes[i];
	    param_counter++;
	}
    }
    MyFree(tmpparamchanmodes);

    GeneralOpts.allchanmodes = (char *) allchanmodes;
    GeneralOpts.paramchanmodes = (char *) paramchanmodes;
    GeneralOpts.chanmodelist = (char *) chanmodelist;

}
