/************************************************************************
 *   IRC - Internet Relay Chat, src/modcommands.c
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
 * $Id: modcommands.c,v 1.4 2003/06/14 13:55:52 tr-ircd Exp $
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

dlink_list mod_paths;

#ifndef STATIC_MODULES

struct Message modload_t[] = {
    {MSG_MODLOAD, 0, 2, M_SLOW, 0L,
     m_unregistered, m_permission, m_modload, m_ignore, m_ignore}
};
struct Message modunload_t[] = {
    {MSG_MODUNLOAD, 0, 2, M_SLOW, 0L,
     m_unregistered, m_permission, m_modunload, m_ignore, m_ignore}
};
struct Message modres_t[] = {
    {MSG_MODRESTART, 0, 1, M_SLOW, 0L,
     m_unregistered, m_permission, m_modrestart, m_ignore, m_ignore}
};
struct Message modlist_t[] = {
    {MSG_MODLIST, 0, 1, M_SLOW, 0L,
     m_unregistered, m_permission, m_modlist, m_ignore, m_ignore}
};

struct Message modload_t_n[] = {
    {MSG_INSMOD, 0, 2, M_SLOW, 0L,
     m_unregistered, m_permission, m_modload, m_ignore, m_ignore}
};
struct Message modunload_t_n[] = {
    {MSG_RMMOD, 0, 2, M_SLOW, 0L,
     m_unregistered, m_permission, m_modunload, m_ignore, m_ignore}
};
struct Message modlist_t_n[] = {
    {MSG_LSMOD, 0, 1, M_SLOW, 0L,
     m_unregistered, m_permission, m_modlist, m_ignore, m_ignore}
};

void init_modules(void)
{
    mod_add_cmd(modload_t);
    mod_add_cmd(modunload_t);
    mod_add_cmd(modres_t);
    mod_add_cmd(modlist_t);
    mod_add_cmd(modload_t_n);
    mod_add_cmd(modunload_t_n);
    mod_add_cmd(modlist_t_n);
    memset(&mod_paths, 0, sizeof(mod_paths));
    mod_paths.tail = mod_paths.head = NULL;
}

int m_modload(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    char *m_bn;
    char modpath[MAXPATHLEN];
    dlink_node *pathst;
    struct module_path *mpath;
    struct stat statbuf;
    if (!IsAdmin(sptr))
	return m_permission(cptr, sptr, parc, parv);
    if (parc < 2 || *parv[1] == '\0') {
        send_me_numeric(sptr, ERR_NEEDMOREPARAMS, MSG_MODLOAD);
        return 0;
    }
    m_bn = irc_basename(parv[1]);
    if (findmodule_byname(m_bn)) {
	send_me_notice(sptr, ":Module %s is already loaded", m_bn);
	MyFree(m_bn);
	return 0;
    }
    MyFree(m_bn);

    if (strchr(parv[1], '/'))	/* absolute path, try it */
	return enqueue_one_module_to_load(parv[1], 0) ? 1 : -1;
    for (pathst = mod_paths.head; pathst; pathst = pathst->next) {
	mpath = (struct module_path *) pathst->data;
	sprintf(modpath, "%s/%s", mpath->path, parv[1]);
	if (stat(modpath, &statbuf) == 0)
	    return enqueue_one_module_to_load(modpath, 0) ? 1 : -1;
    }

    sendto_ops("Cannot locate module %s", parv[1]);
    return 0;
}

int m_modunload(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    char *m_bn;
    if (!IsAdmin(sptr))
	return m_permission(cptr, sptr, parc, parv);
    if (parc < 2 || *parv[1] == '\0') {
        send_me_numeric(sptr, ERR_NEEDMOREPARAMS, MSG_MODUNLOAD);
        return 0;
    }
    m_bn = irc_basename(parv[1]);
    if (enqueue_one_module_to_unload(m_bn) == NULL)
	send_me_notice(sptr, ":Module %s is not loaded", m_bn);
    MyFree(m_bn);
    return 0;
}

int m_modlist(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    dlink_node *ptr;
    struct module *mod;
    if (!IsAdmin(sptr))
	return m_permission(cptr, sptr, parc, parv);
    if (parc > 1)
	send_me_notice(sptr, ":Listing modules matching string '%s'...", parv[1]);
    else
	send_me_notice(sptr, ":Listing all modules...");
    for (ptr = loaded_module_list.head; ptr; ptr = ptr->next) {
	mod = ptr->data;
	if (!mod)
	    continue;
	if (parc > 1) {
	    if (strstr(mod->name, parv[1])) {
		send_me_numeric(sptr, RPL_MODLIST,
				mod->name, mod->address, mod->version);
	    }
	} else {
	    send_me_numeric(sptr, RPL_MODLIST, mod->name, mod->address, mod->version);
	}
    }
    send_me_notice(sptr, ":Done.");
    return 0;
}

int m_modrestart(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    if (!IsAdmin(sptr))
	return m_permission(cptr, sptr, parc, parv);
    send_me_notice(sptr, ":Reloading all modules (%d available)", dlink_list_length(&loaded_module_list));
    enqueue_all_modules_to_unload();
    load_all_modules(LM_MODULES | LM_LANG, 0);
    rehash(&me, &me, 1);
    sendto_ops("Module Restart: %d modules available", dlink_list_length(&loaded_module_list));
    return 0;
}

#else

void init_modules(void)
{
    return;
}
#endif /* STATIC_MODULES */
