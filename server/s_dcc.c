/************************************************************************
 *   IRC - Internet Relay Chat, server/s_dcc.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers.
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
 * $Id: s_dcc.c,v 1.7 2003/08/12 10:22:19 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "numeric.h"

int add_dccallow(aClient *sptr, aClient *cptr)
{
    dlink_node *ptr;
    int cnt;

    if (!IsPerson(cptr) || !IsPerson(sptr))
	return 0;
    cnt = dlink_list_length(&(sptr->user->dccallow));

    if (dlinkFind(&(sptr->user->dccallow), cptr))
	return 0;
    if (++cnt >= MAXDCCALLOW) {
	send_me_numeric(sptr, ERR_TOOMANYDCC, cptr->name, MAXDCCALLOW);
	return 0;
    }
    ptr = make_dlink_node();
    dlinkAdd(cptr, ptr, &(sptr->user->dccallow));
    ptr = make_dlink_node();
    dlinkAdd(sptr, ptr, &(cptr->user->allowed_by));
    send_me_numeric(sptr, RPL_DCCSTATUS, cptr->name, "added to");
    return 0;
}

int del_dccallow(aClient *sptr, aClient *optr)
{
    dlink_node **inv, *tmp;

    for (inv = &(sptr->user->dccallow.head); (tmp = *inv); inv = &tmp->next) {
	if (tmp->data == optr) {
	    dlinkDeleteNode(tmp, &(sptr->user->dccallow));
	    break;
	}
    }
    for (inv = &(optr->user->allowed_by.head); (tmp = *inv); inv = &tmp->next) {
	if (tmp->data == sptr) {
	    dlinkDeleteNode(tmp, (&(optr->user->allowed_by)));
	    break;
	}
    }
    send_me_numeric(sptr, RPL_DCCSTATUS, optr->name, "removed from");
    return 0;
}

int remove_dcc_references(aClient *sptr)
{
    aClient *acptr;
    dlink_node *lp, *nextlp;
    dlink_node **lpp, *tmp, *next_tmp;
    lp = sptr->user->dccallow.head;
    while (lp) {
	nextlp = lp->next;
	acptr = lp->data;
	for (lpp = &(acptr->user->allowed_by.head); (tmp = *lpp); lpp = &next_tmp) {
	    next_tmp = tmp->next;
	    if (tmp->data == sptr) 
		dlinkDeleteNode(tmp, &(acptr->user->allowed_by));
	}
	dlinkDeleteNode(lp, &(sptr->user->dccallow));
	lp = nextlp;
    }
    lp = sptr->user->allowed_by.head;
    while (lp) {
        nextlp = lp->next;
        acptr = lp->data;
        for (lpp = &(acptr->user->dccallow.head); (tmp = *lpp); lpp = &next_tmp) {
	    next_tmp = tmp->next;
            if (tmp->data == sptr) {
                sendto_one(acptr, ":%C %N %s :%s has been removed from your DCC allow list for signing off",
                           &me, RPL_DCCINFO, acptr->name, sptr->name);
                dlinkDeleteNode(tmp, &(acptr->user->dccallow));
            }
        }
        dlinkDeleteNode(lp, &(sptr->user->allowed_by));
	lp = nextlp;
    }
    return 0;
}

char *exploits_2char[] = {
    "js", "pl", NULL
};
char *exploits_3char[] = {
    "exe", "com", "bat", "dll", "ini", "vbs", "pif", "mrc",
    "scr", "doc", "xls", "lnk", "shs", "htm", NULL
};
char *exploits_4char[] = {
    "html", NULL
};
int check_dccsend(aClient *from, aClient *to, char *msg)
{
    /*
     * we already know that msg will consist of "DCC SEND" so we can skip to the end
     */
    char *filename = msg + 8;
    char *ext;
    char **farray;
    int arraysz;
    int len = 0, extlen = 0, i;
    /*
     * people can send themselves stuff all the like..
     * * opers need to be able to send cleaner files
     * * sanity checks..
     */
    if (from == to || !IsPerson(from) || IsAnOper(from) || !MyClient(to) || IsDccallowAll(to))
	return 0;
    while (*filename == ' ')
	filename++;
    if (!(*filename))
	return 0;
    while (*(filename + len) != ' ') {
	if (!(*(filename + len)))
	    break;
	len++;
    }
    for (ext = filename + len;; ext--) {
	if (ext == filename)
	    return 0;
	if (*ext == '.') {
	    ext++;
	    extlen--;
	    break;
	}
	extlen++;
    }
    switch (extlen) {
	case 2:
	    farray = exploits_2char;
	    arraysz = 2;
	    break;
	case 3:
	    farray = exploits_3char;
	    arraysz = 3;
	    break;
	case 4:
	    farray = exploits_4char;
	    arraysz = 4;
	    break;
	    /*
	     * no executable file here..
	     */
	default:
	    return 0;
    }

    for (i = 0; farray[i]; i++) {
	if (irc_strncmp(farray[i], ext, arraysz) == 0)
	    break;
    }

    if (farray[i] == NULL)
	return 0;
    if (!allow_dcc(to, from)) {
    char tmpext[8];
    char tmpfn[128];
    dlink_node *tlp, *flp;
    aChannel *chptr = NULL;
    aChannel *achptr = NULL;
    aChannel *bchptr = NULL;
	strlcpy_irc(tmpext, ext, extlen);
	tmpext[extlen] = '\0';
	if (len > 127)
	    len = 127;
	strlcpy_irc(tmpfn, filename, len);
	tmpfn[len] = '\0';
	/*
	 * use notices!
	 * *   server notices are hard to script around.
	 * *   server notices are not ignored by clients.
	 */
	send_me_notice(from,
		       ":The user %s is not accepting DCC sends of filetype *.%s from you."
		       " Your file %s was not sent.", to->name, tmpext, tmpfn);
	send_me_notice(to,
		       ":%s (%s@%s) has attempted to send you a file named %s, which was blocked.",
		       from->name, from->user->username, from->user->host, tmpfn);
	if (!SeenDCCNotice(to)) {
	    to->protoflags |= PFLAGS_DCCNOTICE;
	    send_me_notice(to,
			   ":The majority of files sent of this type are malicious virii and trojan horses."
			   " In order to prevent the spread of this problem, we are blocking DCC sends of"
			   " these types of files by default.");
	    send_me_notice(to,
			   ":If you trust %s, and want him/her to send you this file, you may obtain"
			   " more information on using the dccallow system by typing /quote help dccallow",
			   from->name, to->name);
	}
	for (tlp = to->user->channel.head; tlp && !chptr; tlp = tlp->next) {
	    achptr = tlp->data;
	    for (flp = from->user->channel.head; flp && !chptr; flp = flp->next) {
		bchptr = flp->data;
		if (achptr == bchptr)
		    chptr = achptr;
	    }
	}

	sendto_lev(DCCSEND_LEV, "%^C sending forbidden filetyped file %s to %C",
		       from, tmpfn, to);
	return 1;
    }
    return 0;
}

int allow_dcc(aClient *to, aClient *from)
{
    dlink_node *lp;
    aClient *acptr;
    for (lp = to->user->dccallow.head; lp; lp = lp->next) {
	acptr = lp->data;
	if (acptr == from)
	    return 1;
    }
    return 0;
}
