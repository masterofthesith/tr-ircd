/************************************************************************
 *   IRC - Internet Relay Chat, src/eventmap.c
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
 *
 */

/* $Id: eventmap.c,v 1.4 2003/06/14 13:55:52 tr-ircd Exp $ */

#include "struct.h"
#include "eventmap.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "tools.h"
#include "confitem.h"
#include "h.h"
#include "s_conf.h"
#include "modules.h"


#ifdef STATIC_MODULES
int tryToLoadEvent(char *event)
{
    return -1;
}

int unloadDependentHooks(char *name)
{
    return 0;
}

#endif

#ifndef STATIC_MODULES

static char unknown_ver[] = "<unknown>";

int tryToLoadEvent(char *event)
{
    int i = 0;
    while (eventmap[i].exists == 1) {
	if (strcmp(event, eventmap[i].event) == 0) {
	    //load the module on the fly
	    loadModule((char *) eventmap[i].modpath);
	    return 0;
	}
	i++;
    }
    return -1;
}

int unloadDependentHooks(char *eventname)
{
    int i = 0;
    while (hookmap[i].exists == 1) {
	if (strcmp(eventname, hookmap[i].event) == 0)
	    unloadModule((char *) hookmap[i].modpath);
	i++;
    }
    return 0;

}

int loadModule(char *path)
{
    DYNLINK_TYPE(tmpptr, path);
    struct module *new_module;
    char *mod_basename;
    void (*initfunc) (void) = NULL;
    char **verp;
    char *ver;
    int *persistent = 0;
    int persist = 0;
    dlink_node *ptr;

    new_module = (struct module *) MyMalloc(sizeof(struct module));
    if (!new_module)
	return -1;		// !!!!!!!!!!!!!!!!

    memset(new_module, 0, sizeof(new_module));

    mod_basename = irc_basename(path);

    new_module->name = mod_basename;
    DupString(new_module->pathname, path);

    if (tmpptr == NULL) {
    const char *err = DYNLINK_ERROR;
	logevent_personal("Error [%s] for module %s", err, new_module->name);
	MyFree(new_module->name);
	MyFree(new_module->pathname);
	MyFree(new_module);
	return -1;
    }

    DYNLINK_EXTRACT_SYMBOL(tmpptr, "_modinit", "__modinit", (void (*)(void)), initfunc);

    if (!initfunc) {
	logevent_call(LogSys.modnoinit, new_module->name);
	sendto_ops("Parent module %s has no _modinit() function", new_module->name);
	MyFree(new_module->name);
	MyFree(new_module->pathname);
	MyFree(new_module);
	return -1;
    }

    DYNLINK_EXTRACT_SYMBOL(tmpptr, "_version", "__version", (char **), verp);

    if (!verp)
	ver = unknown_ver;
    else
	ver = *verp;

    DYNLINK_EXTRACT_SYMBOL(tmpptr, "_persistent", "__persistent", (int *), persistent);

    if (persistent)
	persist = *persistent;

    new_module->address = tmpptr;
    new_module->version = ver;
    new_module->flags |= (persist ? MODULE_PERSISTENT : 0);

    initfunc();

    sendto_ops("Parent module %s [version: %s] loaded at 0x%lx", new_module->name, ver,
	       (unsigned long) tmpptr);

    logevent_call(LogSys.modload, new_module->name);

    ptr = make_dlink_node();
    dlinkAdd(new_module, ptr, &loaded_module_list);

    return 0;

}

int unloadModule(char *name)
{
    struct module *mod;
    dlink_node *ptr, *next_ptr;

    for (ptr = loaded_module_list.head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	mod = ptr->data;
	if (irc_strcmp(mod->name, name) == 0) {
    void (*deinitfunc) (void) = NULL;

	    DYNLINK_EXTRACT_SYMBOL(mod->address,
				   "_moddeinit", "__moddeinit", (void (*)(void)), deinitfunc);
	    if (!deinitfunc) {
		sendto_ops("Child module %s has no _moddeinit() function", mod->name);
		MyFree(mod->name);
		MyFree(mod->pathname);
		MyFree(mod);
		return -1;
	    }
	    deinitfunc();

	    DYNLINK_CLOSE(mod->address);
	    logevent_call(LogSys.modunload, mod->name);
	    sendto_ops("Child module %s unloaded", mod->name);

	    MyFree(mod->pathname);
	    MyFree(mod->name);
	    MyFree(mod);

	    if ((ptr = dlinkFind(&loaded_module_list, mod)))
	    	dlinkDeleteNode(ptr, &loaded_module_list);
	}
    }
    return 0;
}
#endif
