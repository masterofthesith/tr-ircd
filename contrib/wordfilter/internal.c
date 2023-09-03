/************************************************************************
 *   IRC - Internet Relay Chat, internal.c
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

dlink_list bwlist;

dlink_node *find_badword_entry(char *entry)
{
    BadWord *bw;
    dlink_node *ptr;

    for (ptr = bwlist.head; ptr; ptr = ptr->next) {
	bw = ptr->data;
	if (!bw)
	    continue;
	if (!irc_strcmp(bw->string, entry))
	    return ptr;
    }
    return NULL;
}

BadWord *matching_badwords(char *text)
{
BadWord *bw;
dlink_node *ptr;

    /* THIS WHOLE FUNCTION SUX -TimeMr14C */
    if (text == NULL || text[0] == '\0')
	return NULL;

    for (ptr = bwlist.head; ptr; ptr = ptr->next) {
	bw = ptr->data;
	if (!bw)
	    continue;
	if (!match(bw->string, text))
	    return bw;
    }
    return NULL;
}

BadWord *new_badword_entry(char *text, char *reason, int add)
{
BadWord *bw = (BadWord *) MyMalloc(sizeof(bw));
dlink_node *ptr;
    FILE *fp;


    if (!bw)
	outofmemory("Make new Badword entry");

    memset(bw, 0, sizeof(bw));

    DupString(bw->string, text);
    DupString(bw->reason, reason);

    ptr = make_dlink_node();
    dlinkAdd(bw, ptr, &bwlist);

    if (add) {
    	fp = fopen(IRCD_PREFIX_ETC "/badword.list", "a");   
    	if (fp != NULL) {
            fprintf(fp, "%s\n", bw->string);
            fclose(fp);
    	}
    }

    return bw;
}

int remove_badword_entry(char *entry)
{
    BadWord *bw;
    dlink_node *ptr = find_badword_entry(entry);

    if (ptr) {
	bw = ptr->data;
	dlinkDelete(ptr, &bwlist);
	MyFree(bw->string);
	MyFree(bw->reason);
	MyFree(bw);
	free_dlink_node(ptr);
	return 1;
    }

    return 0;
}

void list_badwords(aClient *cptr)
{
    dlink_node *ptr;
    BadWord *bw;

    if (!IsAnOper(cptr))
	return;

    for (ptr = bwlist.head; ptr; ptr = ptr->next) {
	bw = ptr->data;
	if (!bw)
	    continue;
	send_me_numeric(cptr, RPL_DENYLIST, bw->string, bw->reason);
    }
    return;
}
