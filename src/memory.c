/************************************************************************
 *   IRC - Internet Relay Chat, src/memory.c
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
 * $Id: memory.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

#define MALLOC_CHECK_ 	1

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "numeric.h"
#include "blalloc.h"
#include "s_conf.h"


void *MyMalloc(size_t x)
{
void *ret;

    ret = calloc(1, x);

    if (!ret) 
	outofmemory("MyMalloc");

    return ret;
}

void *MyRealloc(void *x, size_t y)
{
void *ret;

    ret = realloc(x, y);

    if (!ret)
        outofmemory("MyRealloc");

    return ret; 
}

/*
 * outofmemory()
 * 
 * input                - NONE output           - NONE side effects     -
 * simply try to report there is a problem I free all the memory in the
 * kline lists hoping to free enough memory so that a proper report can
 * be made. If I was already here (was_here) then I got called twice,
 * and more drastic measures are in order. I'll try to just abort() at
 * least. -Dianora
 */

void outofmemory(char *msg)
{
static int was_here = 0;

    if (was_here)
	abort();

    was_here = 1;
    logevent_call(LogSys.serverkill, msg);
    logevent_call(LogSys.serverkill, "Out of memory");
    restart("Out of Memory");
}

