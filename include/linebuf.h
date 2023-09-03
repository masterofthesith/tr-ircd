/************************************************************************
 *   IRC - Internet Relay Chat, include/linebuf.h
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
 * include stuff for the linebuf mechanism.
 *
 * Adrian Chadd <adrian@creative.net.au>
 */
#ifndef LINEBUF_H
#define LINEBUF_H 1

/* as much as I hate includes in header files .. */
#include "tools.h"

#define LINEBUF_COMPLETE        0
#define LINEBUF_PARTIAL         1
#define LINEBUF_PARSED          0
#define LINEBUF_RAW             1

/* How big we want a buffer - 510 data bytes, plus space for a '\0' */

#define BUF_DATA_SIZE		512

struct _buf_line;
struct _buf_head;

typedef struct _buf_line buf_line_t; 
typedef struct _buf_head buf_head_t;

struct _buf_line {
    char buf[BUF_DATA_SIZE];
    unsigned int  terminated;	/* Whether we've terminated the buffer */
    unsigned int  flushing;	/* Whether we're flushing .. */
    unsigned int  raw;          /* Whether this linebuf may hold 8-bit data */
    int  len;			/* How much data we've got */
    int  refcount;              /* how many linked lists are we in? */
    struct _buf_line *next;     /* next in free list */
};

struct _buf_head {
    dlink_list list;		/* the actual dlink list */
    int  len;			/* length of all the data */
    int  alloclen;		/* Actual allocated data length */    
    int  writeofs;		/* offset in the first line for the write */
    int  numlines;		/* number of lines */
};

/* they should be functions, but .. */
#define linebuf_len(x)		((x)->len)
#define linebuf_alloclen(x)	((x)->alloclen)
#define linebuf_numlines(x)	((x)->numlines)

extern void linebuf_init(void);

extern void linebuf_newbuf(buf_head_t *);
extern void linebuf_donebuf(buf_head_t *);
extern void linebuf_put(buf_head_t *, char *, int);
extern void linebuf_attach(buf_head_t *, buf_head_t *);
extern void count_linebuf_memory(int *, u_long *);

extern buf_line_t *linebuf_allocate(void);

extern int linebuf_parse(buf_head_t *, char *, int, int);
extern int linebuf_get(buf_head_t *, char *, int, int, int);
extern int linebuf_flush(aClient *);

#endif
