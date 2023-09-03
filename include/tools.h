/************************************************************************
 *   IRC - Internet Relay Chat, include/tools.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1992 Darren Reed
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
 * tools.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 * Definitions/prototypes for src/tools.c
 *
 * Adrian Chadd <adrian@creative.net.au>
 *
 * $Id: tools.h,v 1.4 2003/07/01 11:01:18 tr-ircd Exp $
 */
#ifndef TOOLS_H 
#define TOOLS_H 1


/*
 * double-linked-list stuff
 */
typedef struct _dlink_node dlink_node;
typedef struct _dlink_list dlink_list;

struct _dlink_node {
    void *data;
    dlink_node *prev;
    dlink_node *next;

};
  
struct _dlink_list {
    dlink_node *head;
    dlink_node *tail;
};

void
dlinkAdd(void *data, dlink_node * m, dlink_list * list);

void
dlinkAddBefore(dlink_node *b, void *data, dlink_node *m, dlink_list *list);

void
dlinkAddTail(void *data, dlink_node *m, dlink_list *list);

void
dlinkDelete(dlink_node *m, dlink_list *list);

void 
dlinkDeleteNode(dlink_node *ptr, dlink_list *list);

void
dlinkMoveList(dlink_list *from, dlink_list *to);

int
dlink_list_length(dlink_list *m);

dlink_node *dlinkFind(dlink_list *m, void *data);
dlink_node *find_str_link(dlink_list *lptr, void *charptr);

void mem_frob(void *data, int len);

extern dlink_node *find_user_link();

extern dlink_node *_make_dlink_node (void);
#define make_dlink_node() _make_dlink_node();
extern void _free_dlink_node(dlink_node *lp);
#define free_dlink_node(x) _free_dlink_node((x))

void init_dlink_nodes(void);

#endif
