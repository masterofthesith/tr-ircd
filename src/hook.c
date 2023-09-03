/************************************************************************
 *   IRC - Internet Relay Chat, src/hook.c
 *
 *   Copyright (C)2000-2003 TR-IRCD Development
 *   Copyright (C) 2000 Edward Brocklesby, Hybrid Development Team
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

/* hooks are used by modules to hook into events called by other parts of
 * the code.  modules can also register their own events, and call them
 * as appropriate. */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "tools.h"
#include "hook.h"
#include "h.h"
#include "numeric.h"

#define EVENTMAP_DEF 1
#include "eventmap.h"
#undef EVENTMAP_DEF

dlink_list hooks;

void init_hooks(void)
{
    memset(&hooks, 0, sizeof(hooks));
}

static hook *new_hook(char *name)
{
    hook *h;
    char txt[32];
    char out[32];

    h = (hook *) MyMalloc(sizeof(*h));
    memset(h, 0, sizeof(*h));
    DupString(h->name, name);

    strlcpy_irc(txt, name, 32);

    h->id = atol(calchash(txt, out));

    return h;
}

int hook_add_event(char *name)
{
    dlink_node *node;
    dlink_node *eventNode;
    hook *newhook;

    node = make_dlink_node();
    newhook = new_hook(name);

    eventNode = make_dlink_node();

    dlinkAdd(newhook, node, &hooks);

    return newhook->id;
}

int hook_del_event(char *name)
{
    dlink_node *node;
    hook *h;

    for (node = hooks.head; node; node = node->next) {
	h = node->data;
	if (!h)
	    continue;
	if (!strcmp(h->name, name)) {
	    unloadDependentHooks(name);
	    dlinkDeleteNode(node, &hooks);
	    MyFree(h->name);
	    MyFree(h);
	    return 0;
	}
    }
    return 0;
}

static hook *find_hook(char *name)
{
    dlink_node *node;
    hook *h;

    for (node = hooks.head; node; node = node->next) {
	h = node->data;
	if (!h)
	    continue;
	if (!strcmp(h->name, name))
	    return h;
    }
    return NULL;
}

static hook *find_hook_id(int id)
{
    dlink_node *node;
    hook *h;

    for (node = hooks.head; node; node = node->next) {
	h = node->data;
	if (!h)
	    continue;
	if (h->id == id)
	    return h;
    }
    return NULL;
}

int hook_del_hook(char *event, hookfn * fn)
{
    hook *h;
    dlink_node *node, *nnode;
    h = find_hook(event);
    if (!h)
	return -1;

    for (node = h->hooks.head; node; node = node->next) {
	nnode = node->next;
	if (fn == node->data) {
	    dlinkDelete(node, &h->hooks);
	    free_dlink_node(node);
	}
    }
    return 0;
}

int hook_add_hook(char *event, hookfn * fn)
{
    hook *h;
    dlink_node *node;

    h = find_hook(event);

    if (!h) {
	tryToLoadEvent(event);
	h = find_hook(event);	/* Was Missing */
	if (!h)
	    return -1;
    }

    node = make_dlink_node();

    dlinkAdd(fn, node, &h->hooks);
    return 0;
}

int hook_call_event(int id, void *data)
{
    int r = 0;
    hook *h;
    dlink_node *node;
    hookfn fn;

    h = find_hook_id(id);

    if (!h)
	return 0;

    for (node = h->hooks.head; node; node = node->next) {
	fn = (hookfn) (uintptr_t) node->data;
	r = fn(data);
	if (r != 0)
	    return r;
    }
    return 0;
}

void hook_dump(aClient *cptr)
{
    dlink_node *node;
    hook *h;

    for (node = hooks.head; node; node = node->next) {
	h = node->data;
	if (h)
	    send_me_numeric(cptr, RPL_HOOKLIST, h->id, h->name);
    }
    return;
}
