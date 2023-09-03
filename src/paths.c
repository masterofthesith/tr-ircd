/************************************************************************
 *   IRC - Internet Relay Chat, src/paths.c
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
 *
 * $Id: paths.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "confitem.h"
#include "h.h"
#include "hook.h"
#include "msg.h"
#include "numeric.h"
#include "modules.h"
#include "s_conf.h"

char *irc_basename(char *path)
{
    char *mod_basename;
    char *s;

    if (!path)
        return NULL;

    mod_basename = MyMalloc(strlen(path) + 1);

    if (!(s = strrchr(path, '/')))
	s = path;
    else
	s++;

    strcpy(mod_basename, s);
    return mod_basename;
}

static struct module_path *mod_find_path(char *path)
{
    dlink_node *pathst = mod_paths.head;
    struct module_path *mpath;

    if (!pathst)
	return NULL;

    for (; pathst; pathst = pathst->next) {
	mpath = (struct module_path *) pathst->data;

	if (!strcmp(path, mpath->path))
	    return mpath;
    }

    return NULL;
}

void mod_add_path(char *path)
{
    struct module_path *pathst;
    dlink_node *node;

    if (mod_find_path(path))
	return;

    pathst = (struct module_path *) MyMalloc(sizeof(struct module_path));
    node = make_dlink_node();

    strcpy(pathst->path, path);
    dlinkAdd(pathst, node, &mod_paths);
}

void mod_clear_paths(void)
{
    struct module_path *pathst;
    dlink_node *node, *next;

    for (node = mod_paths.head; node; node = next) {
	next = node->next;
	pathst = (struct module_path *) node->data;
	dlinkDelete(node, &mod_paths);
	free_dlink_node(node);
	MyFree(pathst);
    }
}

