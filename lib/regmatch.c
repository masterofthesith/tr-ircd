/************************************************************************
 *   IRC - Internet Relay Chat, lib/regmatch.c
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
 * $Id: regmatch.c,v 1.2 2003/06/14 13:55:50 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "confitem.h"
#include "h.h"
#include "hostmask.h"
#include "hook.h"
#include "msg.h"
#include "numeric.h"
#include "s_conf.h"  
#include "regex.h"

void ireg_free_ami(aMaskItem *ami)
{
    regfree(&(ami->rex));
}

void ireg_merge_pattern(aMaskItem *ami, int len, char *str)
{
    if (!str)
	return; 
    re_compile_pattern(str, len, &(ami->rex));
}

int ireg_match(aMaskItem *ami, char *str)
{
    int len;
    int ret;
    if (!str)
	return 0;
    len = strlen(str);
    ret = re_match(&(ami->rex), str, len, 0, NULL);

    if (ret > 0)
	return 1;
    else
	return 0;
}


