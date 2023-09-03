/************************************************************************
 *   IRC - Internet Relay Chat, server/s_motd.c
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
 * $Id: s_motd.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "h.h"
#include "s_conf.h"
#include "language.h"

/*
 ** send_motd
 **  parv[0] = sender prefix
 **  parv[1] = servername
 **
 ** This function split off so a server notice could be generated on a
 ** user requested motd, but not on each connecting client.
 ** -Dianora
 */
void send_message_file(aMotd *mfile, aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    aMotdItem *temp;

    if (!mfile) {
	send_me_numeric(sptr, ERR_NOMOTD);
	return;
    }

    temp = mfile->content;
    send_me_numeric(sptr, RPL_MOTDSTART, me.name);
    send_me_numeric(sptr, RPL_MOTD, mfile->lastchange);

    while (temp) {
	send_me_numeric(sptr, RPL_MOTD, temp->line);
	temp = temp->next;
    }

    send_me_numeric(sptr, RPL_ENDOFMOTD);
    return;
}

void read_message_file(aMotd *item)
{
    aMotdItem *ami = NULL;
    aMotdItem *next = NULL;
    aMotdItem *new = NULL;
    aMotdItem *curr = NULL;
    struct stat sb;
    struct tm *mtm;
    char buffer[MOTDLINELEN], *tmp;
    FBFILE *mfile;

    if (stat(item->filename, &sb) < 0)
	return;

    for (ami = item->content; ami; ami = next) {
	next = ami->next;
	MyFree(ami);
    }

    item->content = NULL;
    mtm = localtime(&sb.st_mtime);

    if (mtm) {
	ircsprintf(item->lastchange, "%d/%d/%d %d:%02d",
		   mtm->tm_mday, mtm->tm_mon + 1, 1900 + mtm->tm_year, mtm->tm_hour, mtm->tm_min);
    }

    mfile = fbopen(item->filename, "r");
    if (!mfile)
	return;

    while (fbgets(buffer, MOTDLINELEN, mfile)) {
	if ((tmp = (char *) strchr(buffer, '\n')))
	    *tmp = '\0';
	if ((tmp = (char *) strchr(buffer, '\r')))
	    *tmp = '\0';
	new = (aMotdItem *) MyMalloc(sizeof(aMotdItem));
	strlcpy_irc(new->line, buffer, MOTDLINELEN);
	new->next = (aMotdItem *) NULL;
	if (item->content) {
	    if (curr)
		curr->next = new;
	    curr = new;
	} else {
	    item->content = new;
	    curr = new;
	}
    }

    fbclose(mfile);
}

void init_message_file(int type, char *filename, aMotd *mfile)
{
    strlcpy_irc(mfile->filename, filename, MAXPATHLEN);
    mfile->type = type;
    mfile->content = NULL;
    mfile->lastchange[0] = '\0';
}

void send_config_file(aMotd *mfile, aClient *cptr, aClient *sptr, char *section)
{
    aMotdItem *temp;

    temp = mfile->content;

    while (temp) {
	if (strstr(temp->line, section) && temp->line[0] == section[0]) {
	    send_me_numeric(sptr, RPL_DISPLAYSTART, section);
	    while (!strstr("};", temp->line)) {
		send_me_numeric(sptr, RPL_DISPLAY, temp->line);
		temp = temp->next;
	    }
	    send_me_numeric(sptr, RPL_DISPLAY, "};");
	}
	temp = temp->next;
    }

    send_me_numeric(sptr, RPL_ENDOFDISPLAY);
    return;
}

    /*
     * write_links_file 
     */
void write_links_file(void *notused)
{
    aMotdItem *next_mptr = 0;
    aMotdItem *mptr = 0;
    aMotdItem *currentMessageLine = 0;
    aMotdItem *newMessageLine = 0;
    aMotd *MessageFileptr;
    struct Client *target_p;
    char *p;
    FBFILE *file;
    char buff[512];
    dlink_node *ptr;

    MessageFileptr = &(GeneralOpts.linksfile);

    if ((file = fbopen(MessageFileptr->filename, "w")) == 0)
	return;

    for (mptr = MessageFileptr->content; mptr; mptr = next_mptr) {
	next_mptr = mptr->next;
	MyFree(mptr);
    }
    MessageFileptr->content = NULL;
    currentMessageLine = NULL;

    for (ptr = global_serv_list.head; ptr; ptr = ptr->next) {
	target_p = ptr->data;

	if (target_p->info[0])
	    p = target_p->info;
	else
	    p = "(Unknown Location)";

	newMessageLine = (aMotdItem *) MyMalloc(sizeof(aMotdItem));

	/* Attempt to format the file in such a way it follows the usual links output
	 * ie  "servername uplink :hops info"
	 * Mostly for aesthetic reasons - makes it look pretty in mIRC ;)
	 * - madmax
	 */

	ircsprintf(newMessageLine->line, "%s %s :1 %s", target_p->name, me.name, p);
	newMessageLine->next = (aMotdItem *) NULL;

	if (MessageFileptr->content) {
	    if (currentMessageLine)
		currentMessageLine->next = newMessageLine;
	    currentMessageLine = newMessageLine;
	} else {
	    MessageFileptr->content = newMessageLine;
	    currentMessageLine = newMessageLine;
	}

	ircsprintf(buff, "%s %s :1 %s\n", target_p->name, me.name, p);
	fbputs(buff, file);
    }

    fbclose(file);
}

void dohelp(struct Client *source_p, char *hpath, char *topic, char *nick)
{
    char path[MAXPATHLEN + 1];
    char *langpath;
    struct stat sb;
    int i;

    if (topic) {
        /* convert to lower case */
        for (i = 0; topic[i] != '\0'; i++) {
            topic[i] = ToLower(topic[i]);
        }
    } else
        topic = "index";        /* list available help topics */

    if (strchr(topic, '/')) {
        send_me_numeric(source_p, ERR_HELPNOTFOUND, topic);
        return;
    }

    langpath = get_langpath(source_p);

    if (strlen(hpath) + strlen(langpath) + strlen(topic) + 1 > MAXPATHLEN) {
        send_me_numeric(source_p, ERR_HELPNOTFOUND, topic);
        return;
    }

    ircsprintf(path, "%s/%s/%s", hpath, langpath, topic);

    if (stat(path, &sb) < 0) {
        send_me_numeric(source_p, ERR_HELPNOTFOUND, topic);
        return;
    }

    if (!S_ISREG(sb.st_mode)) {
        send_me_numeric(source_p, ERR_HELPNOTFOUND, topic);
        return;
    }

    sendhelpfile(source_p, path, topic, nick);
    return;
}

void sendhelpfile(struct Client *source_p, char *path, char *topic, char *nick)
{
    FBFILE *file;
    char line[HELPLEN];

    if ((file = fbopen(path, "r")) == NULL) {
        send_me_numeric(source_p, ERR_HELPNOTFOUND, topic);
        return;
    }

    if (fbgets(line, sizeof(line), file) == NULL) {
        send_me_numeric(source_p, ERR_HELPNOTFOUND, topic);
        return;
    }

    send_me_numeric(source_p, RPL_HELPSTART, topic, line);

    while (fbgets(line, sizeof(line), file)) {
        send_me_numeric(source_p, RPL_HELPTXT, topic, line);
    }

    send_me_numeric(source_p, RPL_ENDOFHELP, topic);
    fbclose(file);
    return;
}


