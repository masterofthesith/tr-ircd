/************************************************************************
 *   IRC - Internet Relay Chat, src/loader.c
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
 * $Id: loader.c,v 1.6 2003/06/14 13:55:52 tr-ircd Exp $
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
#include "tools.h"
#include "modules.h"

#ifndef STATIC_MODULES

static int loader_initcheck = 0;
static int envoke_exit_loader = 0;

void do_queue(void);

extern void *dynlink_load_element(char *path, int logevent);
extern void *dynlink_extract_symbol(void *library, char *symbolname, void *symbol);

pthread_t loader_thread;
pthread_attr_t loader_attributes;
pthread_cond_t loader_condition;
pthread_condattr_t loader_condition_attributes;

/* Do not reorder the following elements, they are
 * required for the mutex to work correctly
 * -TimeMr14C
 */

int modlist_action_add = 0;
int modlist_action_del = 0;
pthread_mutex_t loader_mutex;
pthread_mutexattr_t loader_mutex_attributes;

static int hookid_load_module = 0;
static int hookid_unload_module = 0;
static int undef_symbol = -1;
static int toomany_symbols = -1;
static int link_editor = -1;
static int module_error = -1;

dlink_list loaded_module_list;
dlink_list load_queue;
dlink_list unload_queue;

void init_loader(void *unused)
{
    int loader_main_return = 0;

    hookid_load_module = hook_add_event("load module");
    hookid_unload_module = hook_add_event("unload module");
    undef_symbol =
	logevent_register("undefined symbol", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR,
			  "Undefined symbol: %s");
    toomany_symbols =
	logevent_register("too many symbols", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR,
			  "Symbol `%s' found in `%s' and `%s'");
    link_editor =
	logevent_register("link editor error", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR,
			  "Link editor error: %s for %s");
    module_error =
	logevent_register("module load error", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR,
			  "Error loading module %s: %s");

    if (pthread_attr_init(&loader_attributes)) {
	logevent_personal("Failed in initializing module loader: INIT ATTRIBUTES");
	return;
    }

    if (pthread_attr_setdetachstate(&loader_attributes, PTHREAD_CREATE_JOINABLE) != 0) {
	logevent_personal("Failed in setting detachstate for the module loader thread");
	return;
    }

    if (pthread_attr_setschedpolicy(&loader_attributes, SCHED_OTHER) != 0) {
	logevent_personal("Failed in setting scheduling policy for the module loader thread");
	return;
    }

    if (pthread_attr_setinheritsched(&loader_attributes, PTHREAD_INHERIT_SCHED) != 0) {
	logevent_personal("Failed in setting interit scheduling for the module loader thread");
	return;
    }

    if (pthread_mutexattr_init(&loader_mutex_attributes) != 0) {
	logevent_personal("Failed in initializing module loader mutex attributes");
	return;
    }

    if (pthread_mutex_init(&loader_mutex, &loader_mutex_attributes) != 0) {
	logevent_personal("Failed in initializing module loader mutex");
	return;
    }

    pthread_condattr_init(&loader_condition_attributes);

    if (pthread_cond_init(&loader_condition, &loader_condition_attributes) != 0) {
	logevent_personal("Failed in initializing module loader conditional predicate");
	return;
    }

    if (pthread_create(&loader_thread, &loader_attributes, loader_main, &loader_main_return) != 0) {
	logevent_personal("Failed in creating thread for the module loader");
	return;
    }

    memset(&loaded_module_list, 0, sizeof(loaded_module_list));
    memset(&load_queue, 0, sizeof(load_queue));
    memset(&unload_queue, 0, sizeof(unload_queue));

    loaded_module_list.head = loaded_module_list.tail = NULL;
    load_queue.head = load_queue.tail = NULL;
    unload_queue.head = unload_queue.tail = NULL;
}

static void loader_init_begin(int check)
{
    if (check)
	return;

    if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) {
	logevent_personal("Unable to set cancel state for the loader thread");
	pthread_exit(0);
    }

    if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0) {
	logevent_personal("Unable to set cancel type for loader thread");
	pthread_exit(0);
    }

    loader_initcheck = 1;
}

void *loader_main(void *unused)
{
    loader_init_begin(loader_initcheck);

    while (1) {

	pthread_mutex_lock(&loader_mutex);
	while ((modlist_action_add < 1) && (modlist_action_del < 1))
	    pthread_cond_wait(&loader_condition, &loader_mutex);

	do_queue();

	pthread_mutex_unlock(&loader_mutex);

 	if (envoke_exit_loader == 1)
	    break;
    }

    pthread_mutex_destroy(&loader_mutex);

    pthread_mutexattr_destroy(&loader_mutex_attributes);

    pthread_cond_destroy(&loader_condition);

    pthread_condattr_destroy(&loader_condition_attributes);

    pthread_attr_destroy(&loader_attributes);

    pthread_exit(0);
    return NULL;
}

void do_queue(void)
{
    dlink_node *ptr, *prev_ptr;
    struct module *mod;
    int val;

    if (modlist_action_del > 0) {
	for (ptr = unload_queue.tail; ptr; ptr = prev_ptr) {
	    prev_ptr = ptr->prev;
	    mod = ptr->data;

	    val = unload_queued_module(mod);
	    dlinkDeleteNode(ptr, &unload_queue);
	    modlist_action_del--;
	}
    }

    if (modlist_action_add > 0) {

	for (ptr = load_queue.tail; ptr; ptr = prev_ptr) {
	    prev_ptr = ptr->prev;
	    mod = ptr->data;
	    val = load_queued_module(mod);
	    dlinkDeleteNode(ptr, &load_queue);
	    modlist_action_add--;
	}
    }

    return;
}

struct module *enqueue_one_module_to_load(char *path, int flags)
{
    dlink_node *ptr;
    struct module *new_module, *mod;
    struct hook_data thisdata;
    char *mod_basename;

    mod_basename = irc_basename(path);

    for (ptr = loaded_module_list.head; ptr; ptr = ptr->next) {
	mod = ptr->data;
	if (irc_strcmp(mod->name, mod_basename) == 0)
	    return mod;
    }
    for (ptr = load_queue.head; ptr; ptr = ptr->next) {
	mod = ptr->data;
	if (irc_strcmp(mod->name, mod_basename) == 0)
	    return mod;
    }

    thisdata.name = mod_basename;

    if (hook_call_event(hookid_load_module, &thisdata))
	return NULL;

    new_module = (struct module *) MyMalloc(sizeof(struct module));
    if (!new_module)
	return NULL;

    memset(new_module, 0, sizeof(new_module));

    new_module->name = mod_basename;
    DupString(new_module->pathname, path);
    new_module->flags = flags;

    ptr = make_dlink_node();

    pthread_mutex_lock(&loader_mutex);
    dlinkAdd(new_module, ptr, &load_queue);
    modlist_action_add++;
    pthread_cond_signal(&loader_condition);
    pthread_mutex_unlock(&loader_mutex);
    return new_module;
}

struct module *enqueue_one_module_to_unload(char *name)
{
    struct module *mod;
    struct hook_data thisdata;
    dlink_node *ptr, *next_ptr, *m;

    for (ptr = loaded_module_list.head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	mod = ptr->data;
	if (irc_strcmp(mod->name, name) == 0) {

	    thisdata.name = mod->name;
	    if (hook_call_event(hookid_unload_module, &thisdata)) {
		sendto_ops("Module %s has been denied usage.", mod->name);
		return NULL;
	    }

	    if (mod->flags & MODULE_PERSISTENT) {
		sendto_ops("Module %s cannot be unloaded.", mod->name);
		return NULL;
	    }

	    m = make_dlink_node();

	    pthread_mutex_lock(&loader_mutex);
	    dlinkAdd(mod, m, &unload_queue);
	    modlist_action_del++;
	    pthread_cond_signal(&loader_condition);
	    pthread_mutex_unlock(&loader_mutex);
	    return mod;
	}
    }
    return NULL;
}

void enqueue_all_modules_to_unload(void)
{
    struct module *mod;
    struct hook_data thisdata;
    dlink_node *ptr, *next_ptr, *m;

    pthread_mutex_lock(&loader_mutex);

    for (ptr = loaded_module_list.head; ptr; ptr = next_ptr) {
	next_ptr = ptr->next;
	mod = ptr->data;

	thisdata.name = mod->name;
	if (mod->flags & MODULE_PERSISTENT)
	    continue;

	if (hook_call_event(hookid_unload_module, &thisdata))
	    continue;

	m = make_dlink_node();

	dlinkAdd(mod, m, &unload_queue);
	modlist_action_del++;
    }
    if (modlist_action_del)
	pthread_cond_signal(&loader_condition);
    pthread_mutex_unlock(&loader_mutex);
}

void terminate_loader(void)
{
    struct module *mod;
    dlink_node *ptr, *next_ptr, *m;

    pthread_mutex_lock(&loader_mutex);

    for (ptr = loaded_module_list.head; ptr; ptr = next_ptr) {
        next_ptr = ptr->next;
        mod = ptr->data;

        m = make_dlink_node();

        dlinkAdd(mod, m, &unload_queue);
        modlist_action_del++;
    }
    if (modlist_action_del)
        pthread_cond_signal(&loader_condition);
    pthread_mutex_unlock(&loader_mutex);
    envoke_exit_loader = 1;
}

#else

void init_loader(void *unused)
{
    return;
}

void terminate_loader(void)
{
    return;
}

#endif
