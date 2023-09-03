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
 * $Id: modules.h,v 1.5 2003/07/01 11:01:18 tr-ircd Exp $
 */

#ifndef MODULES_H
#define MODULES_H 1

#define LM_MODULES 	0x001
#define LM_LANG		0x002
#define LM_CHANMODES	0x004
#define LM_CONTRIB	0x008

#define MODULE_PERSISTENT	0x001
#define MODULE_FROMCONFIG	0x002
#define MODULE_AUTOLOAD		0x004
#define MODULE_PROXYMODULE	0x008
#define MODULE_HTTPDMODULE	0x010

#ifdef HAVE_SHL_LOAD
#define DYNLINK_TYPE(x, path)           shl_t x = shl_load(path, BIND_IMMEDIATE, NULL);
#define DYNLINK_EXTRACT_SYMBOL(library, symbolname, symbolnametd, symboltype, symbol)   \
        shl_findsym(& library, symbolname, TYPE_UNDEFINED, & symbol);
#define DYNLINK_ERROR                   strerror(errno);
#define DYNLINK_CLOSE(addr)		shl_unload((shl_t) & (addr));
#define DYNLINK_ADDRESS			shl_t address
#else
#define DYNLINK_TYPE(x, path)           void * x = dlopen(path, RTLD_NOW)
#define DYNLINK_EXTRACT_SYMBOL(library, symbolname, symbolnametd, symboltype, symbol) \
        symbol = symboltype (uintptr_t) dlsym(library, symbolname); \
        if (symbol == NULL) \
            symbol = symboltype (uintptr_t) dlsym(library, symbolnametd);
#define DYNLINK_ERROR                   dlerror()
#define DYNLINK_CLOSE(addr)		dlclose(addr);
#define DYNLINK_ADDRESS			void *address
#endif

struct module {
  char *name;
  char *pathname;
  char *version;
  DYNLINK_ADDRESS;
  int flags;
};

struct module_path
{
        char path[MAXPATHLEN];
};

extern void init_loader(void *);
extern void *loader_main(void *unused);
extern void terminate_loader(void);

extern int modlist_action_add;
extern int modlist_action_del;

extern dlink_list loaded_module_list;
extern dlink_list load_queue;
extern dlink_list unload_queue;
extern dlink_list mod_paths;

extern struct module *enqueue_one_module_to_load(char *path, int flags);
extern struct module *enqueue_one_module_to_unload(char *name);
extern struct module *findmodule_byname (char *);

extern void enqueue_all_modules_to_unload(void);

extern int load_queued_module(struct module *mod);
extern int unload_queued_module(struct module *mod);

void mod_add_path(char *path);
void mod_clear_paths(void);

extern void load_module(char *path);
extern void load_all_modules(int which, int check);
extern void load_modules_from(int check, char *frompath);

#ifndef STATIC_MODULES
extern void _modinit(void);
extern void _moddeinit(void);
#else
extern void static_modules_init(void);
#include "static.h"
#endif

extern void init_modules(void);

extern char *irc_basename(char *path);   

#ifndef RTLD_NOW
#define RTLD_NOW RTLD_LAZY      /* openbsd deficiency */
#endif

#endif
