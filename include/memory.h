/************************************************************************
 *   IRC - Internet Relay Chat, include/memory.h
 *   Copyright (C) 2000 
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

#define MyFree(x)       if ((x) != NULL) free(x)

extern void *MyMalloc(size_t);
extern void *MyRealloc(void *, size_t);

extern void block_garbage_collect(void *);        /* list.c */
extern void block_destroy(void);        /* list.c */
extern void flush_cache(void);
extern void clear_channel_hash_table(void);
extern void clear_client_hash_table(void);
extern void clear_watch_hash_table(void);

extern void outofmemory(char *);




