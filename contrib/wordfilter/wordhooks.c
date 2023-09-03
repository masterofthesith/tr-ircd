/************************************************************************
 *   IRC - Internet Relay Chat, wordhooks.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
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

#include "wordfilter.h"

int do_message_more(struct hook_data *data)
{
    aClient *sptr = data->source_p;
    aChannel *chptr = find_channel(data->parv[1]);

    /* We do not need link traversal here, because users
     * do not exist in the root channel, and it is already
     * set to +tnL, after cleaning. -TimeMr14C
     */

    aClient *acptr = find_person(data->parv[1]);
    BadWord *bw;
    int sendmsg = 0;

    int ischannel = (chptr ? 2 : (acptr ? 1 : -1));

    if (IsPrivileged(sptr))
	sendmsg = 1;

    if (!sendmsg) {
	switch (ischannel) {
	    case 2:
		if (IsChanNoFilter(chptr)) {
		    sendmsg = 1;
		} else {
		    if (IsChanUser(sptr, chptr, CHFL_SPEAKABLE))
			sendmsg = 1;
		}
		break;
	    case 1:
		if (IsPrivileged(acptr))
		    sendmsg = 1;
		break;
	    default:
	    case -1:
		/* This case should not be reached. It indicates
		 * that the recipient does not exist. -TimeMr14C */
		data->check = 0;
		return 0;
	}
    }

    if (sendmsg) {
	data->check = 0;
	return 0;
    } else {

	/* This means, that the message should be checked 
	 * against the list of the bad word entries.
	 * -TimeMr14C */

	bw = matching_badwords(data->parv[2]);

	if (bw) {
	    data->check = 1;
	    send_me_numeric(data->source_p, ERR_DENYTEXT, data->parv[1]);
	    send_me_numeric(data->source_p, RPL_DENYRES, bw->reason ? bw->reason : "");
	    return 1;
	} else
	    data->check = 0;
	return 0;
    }

    return 0;
}

int do_stats_w(struct hook_data *data)
{
    char stat = data->parv[1][0];
    char *s = DT_STATCHAR;
    char *c = DT_STATCHAR_C;

    if (stat) {
	if ((stat == (u_char) *s) || (stat == (u_char) *c))
	    list_badwords(data->client_p);
    }
    return 0;
}
