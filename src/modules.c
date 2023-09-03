/************************************************************************
 *   IRC - Internet Relay Chat, src/modules.c
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
 * $Id: modules.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
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

#ifndef STATIC_MODULES

struct module *findmodule_byname(char *name)
{
    dlink_node *ptr;
    struct module *mod;

    if (!name)
	return NULL;

    for (ptr = loaded_module_list.head; ptr; ptr = ptr->next) {
	mod = ptr->data;
	if (!mod)
	    continue;
	if (!irc_strcmp(mod->name, name))
	    return mod;
    }
    return NULL;
}

/* load all modules from MODPATH & LANGPATH */
void load_all_modules(int which, int check)
{
    if (which & LM_MODULES)
    	load_modules_from(check, MODPATH);
    if (which & LM_LANG)
        load_modules_from(check, LANGPATH);
    if (which & LM_CHANMODES)
    	load_modules_from(check, CHANMODEPATH);
    mod_add_path(TOOLMODPATH);
}

void load_modules_from(int check, char *frompath)
{
    DIR *system_module_dir = NULL;
    struct dirent *ldirent = NULL;
    char module_fq_name[PATH_MAX + 1];

    mod_add_path(frompath);

    system_module_dir = opendir(frompath);

    if (system_module_dir == NULL) {
	printf("Could not load modules from %s: %s", frompath, strerror(errno));
	return;
    }
    while ((ldirent = readdir(system_module_dir)) != NULL) {
	if (ldirent->d_name[strlen(ldirent->d_name) - 3] == '.' &&
	    ldirent->d_name[strlen(ldirent->d_name) - 2] == 's' &&
	    ((ldirent->d_name[strlen(ldirent->d_name) - 1] == 'o') || 
	     (ldirent->d_name[strlen(ldirent->d_name) - 1] == 'l'))) {
	    sprintf(module_fq_name, "%s/%s", frompath, ldirent->d_name);
	    enqueue_one_module_to_load(module_fq_name, MODULE_AUTOLOAD);
	}
    }
    closedir(system_module_dir);

}

#else

static int did_once = 0;

void load_all_modules(int which, int check)
{
    if (did_once == 0) {
	did_once = 1;
    	static_modules_init();
    }
}

#endif /* STATIC_MODULES */
