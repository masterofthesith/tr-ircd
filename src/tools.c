/************************************************************************
 *   IRC - Internet Relay Chat, src/ircd.c
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
 */

/*
 * tools.c
 * 
 * Useful stuff, ripped from places ..
 *
 * adrian chadd <adrian@creative.net.au>
 *
 * $Id: tools.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "tools.h"
#include "blalloc.h"

#ifndef NDEBUG
/*
 * frob some memory. debugging time.
 * -- adrian
 */
void mem_frob(void *data, int len)
{
    /* correct for Intel only! little endian */
    unsigned char b[4] = { 0xef, 0xbe, 0xad, 0xde };
    int i;
    char *cdata = data;
    for (i = 0; i < len; i++) {
	*cdata = b[i % 4];
	cdata++;
    }
}
#endif

/* 
 * dlink_ routines are stolen from squid, except for dlinkAddBefore,
 * which is mine.
 *   -- adrian
 */
void dlinkAdd(void *data, dlink_node * m, dlink_list * list)
{
    m->data = data;
    m->prev = NULL;
    m->next = list->head;
    if (list->head)
	list->head->prev = m;
    else if (list->tail == NULL)
	list->tail = m;
    list->head = m;
}

void dlinkAddBefore(dlink_node * b, void *data, dlink_node * m, dlink_list * list)
{
    /* Shortcut - if its the first one, call dlinkAdd only */
    if (b == list->head)
	dlinkAdd(data, m, list);
    else {
	m->data = data;
	b->prev->next = m;
	m->prev = b->prev;
	b->prev = m;
	m->next = b;
    }
}

void dlinkAddTail(void *data, dlink_node * m, dlink_list * list)
{
    m->data = data;
    m->next = NULL;
    m->prev = list->tail;
    if (list->tail)
	list->tail->next = m;
    else if (list->head == NULL)
	list->head = m;
    list->tail = m;
}

void dlinkDelete(dlink_node * m, dlink_list * list)
{
    if (m->next)
	m->next->prev = m->prev;
    else
	list->tail = m->prev;

    if (m->prev)
	m->prev->next = m->next;
    else
	list->head = m->next;

    m->next = m->prev = NULL;
}

void dlinkDeleteNode(dlink_node *ptr, dlink_list * list)
{
    dlinkDelete(ptr, list);
    free_dlink_node(ptr);
}

/* 
 * dlink_list_length
 * inputs       - pointer to a dlink_list
 * output       - return the length (>=0) of a chain of links.
 * side effects -
 */
extern int dlink_list_length(dlink_list * list)
{
dlink_node *ptr;
int count = 0;

    for (ptr = list->head; ptr; ptr = ptr->next)
	count++;
    return count;
}

/*
 * dlinkFind
 * inputs       - list to search 
 *              - data
 * output       - pointer to link or NULL if not found
 * side effects - Look for ptr in the linked listed pointed to by link.
 */
dlink_node *dlinkFind(dlink_list * list, void *data)
{
dlink_node *ptr;

    if (!list->head)
	return NULL;
    for (ptr = list->head; ptr; ptr = ptr->next) {
	if (ptr->data == data)
	    return (ptr);
    }
    return (NULL);
}

void dlinkMoveList(dlink_list * from, dlink_list * to)
{
    /* There are three cases */
    /* case one, nothing in from list */

    if (from->head == NULL)
	return;

    /* case two, nothing in to list */
    /* actually if to->head is NULL and to->tail isn't, thats a bug */

    if (to->head == NULL) {
	to->head = from->head;
	to->tail = from->tail;
	from->head = from->tail = NULL;
	return;
    }

    /* third case play with the links */

    from->tail->next = to->head;
    from->head->prev = to->head->prev;
    to->head->prev = from->tail;
    to->head = from->head;
    from->head = from->tail = NULL;

    /* I think I got that right */
}

/*
 * init_dlink_nodes
 *      
 */
BlockHeap *dnode_heap;

void init_dlink_nodes(void)
{
    dnode_heap = BlockHeapCreate(sizeof(dlink_node), DLINK_PREALLOCATE);
    if (dnode_heap == NULL)
	outofmemory("Init dlink nodes");
}

/*
 * make_dlink_node
 *
 * inputs       - NONE
 * output       - pointer to new dlink_node
 * side effects - NONE
 */
dlink_node *_make_dlink_node(void)
{
    dlink_node *lp;

    lp = (dlink_node *) BlockHeapAlloc(dnode_heap);

    if (lp == NULL)
	outofmemory("Create dlink node");

    lp->next = NULL;
    lp->prev = NULL;

    return lp;
}

/*  
 * _free_dlink_node
 *
 * inputs       - pointer to dlink_node
 * output       - NONE
 * side effects - free given dlink_node
 */
void _free_dlink_node(dlink_node * ptr)
{
    BlockHeapFree(dnode_heap, ptr);
}

/*
 * Look for a match in a list of strings. Go through the list, and run
 * match() on it. Side effect: if found, this link is moved to the top of
 * the list.
 */
dlink_node *find_str_link(dlink_list * lptr, void *charptr)
{
    dlink_node *ptr;
    if (!charptr)
	return ((dlink_node *) NULL);
    ptr = lptr->head;
    while (ptr != NULL) {
	if (!match((char *) ptr->data, (char *) charptr))
	    return ptr;
	ptr = ptr->next;
    }
    return ((dlink_node *) NULL);
}

