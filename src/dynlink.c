/*
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
 * $Id: dynlink.c,v 1.5 2004/12/25 01:19:21 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "confitem.h"
#include "h.h"
#include "hook.h"
#include "s_conf.h"
#include "msg.h"
#include "numeric.h"
#include "modules.h"

#ifndef STATIC_MODULES

#ifdef HAVE_MACH_O_DYLD_H
#include "dlcompat.c"
#endif

static char unknown_ver[] = "<unknown>";

/*
 * load_queued_module
 *
 * inputs       - path name of module
 * output       - -1 if error 1 if success
 * side effects - loads a module if successful
 */
int load_queued_module(struct module *mod)
{
    DYNLINK_TYPE(tmpptr, mod->pathname);

    dlink_node *ptr;

    void (*initfunc) (void) = NULL;
    char **verp;
    char *ver;
    int *persistent = 0;
    int persist = 0;

    if (tmpptr == NULL) {
        const char *err = DYNLINK_ERROR;
        logevent_personal("Error [%s] for module %s", err, mod->name);
        MyFree(mod->name);
        MyFree(mod->pathname);
        MyFree(mod);
        return -1;
    }

    DYNLINK_EXTRACT_SYMBOL(tmpptr, "_modinit", "__modinit", (void (*)(void)), initfunc);

    if (!initfunc) {
        logevent_call(LogSys.modnoinit, mod->name);
        sendto_ops("Module %s has no _modinit() function", mod->name);
        MyFree(mod->name);
        MyFree(mod->pathname);
        MyFree(mod);
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

    mod->address = tmpptr;
    mod->version = ver;
    mod->flags |= (persist ? MODULE_PERSISTENT : 0);

    initfunc();

    sendto_ops("Module %s [version: %s] loaded at 0x%lx", mod->name, ver, (unsigned long) tmpptr);

    logevent_call(LogSys.modload, mod->name);

    ptr = make_dlink_node();
    dlinkAdd(mod, ptr, &loaded_module_list);

    return 0;
}

/*
 * unload_queued_module
 *
 * inputs       - name of module to unload
 *              - 1 to say modules unloaded, 0 to not
 * output       - 0 if successful, -1 if error
 * side effects - module is unloaded
 */

int unload_queued_module(struct module *mod)
{
    void (*deinitfunc) (void) = NULL;
    dlink_node *ptr;

    DYNLINK_EXTRACT_SYMBOL(mod->address, "_moddeinit", "__moddeinit", (void (*)(void)), deinitfunc);

    if (!deinitfunc) {
        sendto_ops("Module %s has no _moddeinit() function", mod->name);
    } else {
    	deinitfunc();
    }

    DYNLINK_CLOSE(mod->address);

    logevent_call(LogSys.modunload, mod->name);

    sendto_ops("Module %s unloaded", mod->name);

    MyFree(mod->pathname);
    MyFree(mod->name);

    if ((ptr = dlinkFind(&loaded_module_list, mod)))    
    	dlinkDeleteNode(ptr, &loaded_module_list);

    MyFree(mod);
    return 0;
}

#endif
