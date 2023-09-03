/************************************************************************
 *   IRC - Internet Relay Chat, init.h
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

#define TOKEN 1
#include "wordfilter.h"
#undef TOKEN

static char *d_token = TOK1_DENYTEXT;
static char *u_token = TOK1_UNDENYTEXT;

static struct Message d_msgtab[] = {
    {MSG_DENYTEXT, 0, MAXPARA, M_SLOW, 0L,
     m_unregistered, m_ignore, o_denytext, m_denytext, m_ignore}
};

static struct Message u_msgtab[] = {
    {MSG_UNDENYTEXT, 0, MAXPARA, M_SLOW, 0L,
     m_unregistered, m_ignore, o_undenytext, m_undenytext, m_ignore}
};

static char std_err_denytext[] = ":%C 560 %s :Your text cannot be delivered to %s";
static char std_rpl_denyres[] = ":%C 561 %s :The text has been denied due to spam. Reason: [%s]";
static char std_rpl_denylist[] = ":%C 562 %s %s is denied due to :[%s]";

#ifndef STATIC_MODULES

void _modinit(void)
{
    mod_add_cmd(u_msgtab);
    tok1_msgtab[(u_char) *u_token].msg = u_msgtab;
    tok1_msgtab[(u_char) *u_token].cmd = MSG_DENYTEXT;
    mod_add_cmd(d_msgtab);
    tok1_msgtab[(u_char) *d_token].msg = d_msgtab;
    tok1_msgtab[(u_char) *d_token].cmd = MSG_UNDENYTEXT;
    hook_add_hook("calling m_private", (hookfn *) do_message_more);
    hook_add_hook("can send more", (hookfn *) do_message_more);
    hook_add_hook("doing stats", (hookfn *) do_stats_w);
    replies[ERR_DENYTEXT] = std_err_denytext;
    replies[RPL_DENYRES] = std_rpl_denyres;
    replies[RPL_DENYLIST] = std_rpl_denylist;

    memset(&bwlist, 0, sizeof(bwlist));
    nofilter_modinit();
    read_definitions();
}

void _moddeinit(void)
{
    mod_del_cmd(u_msgtab);
    tok1_msgtab[(u_char) *u_token].msg = NULL;
    tok1_msgtab[(u_char) *u_token].cmd = NULL;
    mod_del_cmd(d_msgtab);
    tok1_msgtab[(u_char) *d_token].msg = NULL;
    tok1_msgtab[(u_char) *d_token].cmd = NULL;
    hook_del_hook("calling m_private", (hookfn *) do_message_more);
    hook_del_hook("doing stats", (hookfn *) do_stats_w);
    hook_del_hook("can send more", (hookfn *) do_message_more);
    replies[ERR_DENYTEXT] = NULL;
    replies[RPL_DENYRES] = NULL;
    replies[RPL_DENYLIST] = NULL;
    nofilter_moddeinit();
}

char *_version = "$Revision: 1.4 $";

int _dependent = 1;		/* This means, that this module is dependent to
				 * other modules loaded with modules directory.
				 * Therefore, the ircd will have to reload this
				 * module. -TimeMr14C */

#else

void wordfilter_init(void)
{
    mod_add_cmd(u_msgtab);
    tok1_msgtab[(u_char) *u_token].msg = u_msgtab;
    tok1_msgtab[(u_char) *u_token].cmd = MSG_DENYTEXT;
    mod_add_cmd(d_msgtab);
    tok1_msgtab[(u_char) *d_token].msg = d_msgtab;
    tok1_msgtab[(u_char) *d_token].cmd = MSG_UNDENYTEXT;
    hook_add_hook("calling m_private", (hookfn *) do_message_more);
    hook_add_hook("doing stats", (hookfn *) do_stats_w);
    hook_add_hook("can send more", (hookfn *) do_message_more);
    replies[ERR_DENYTEXT] = std_err_denytext;
    replies[RPL_DENYRES] = std_rpl_denyres;
    replies[RPL_DENYLIST] = std_rpl_denylist;

    memset(&bwlist, 0, sizeof(bwlist));
    nofilter_modinit();
    read_definitions();
}

#endif

void read_definitions(void)
{
    FBFILE *file;
    char line[256];
    BadWord *bw;

    if ((file = fbopen(IRCD_PREFIX_ETC "/badword.list", "r")) == NULL) 
        return;

    while (fbgets(line, sizeof(line), file)) {
    	if (!(find_badword_entry(line))) {
	    int l = 0;
	    while (l < 256 && line[l] != '\n')
		l++;
	    if (l < 255)
		line[l] = '\0';
            bw = new_badword_entry(line, "Denied in Configuration File", BADWORD_DONTADD);
		memset(line, 0, sizeof(char) * 256);
    	}
    }

    fbclose(file);
    return;
}
