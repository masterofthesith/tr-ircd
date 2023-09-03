/************************************************************************
 *   IRC - Internet Relay Chat, src/interproc/iproc_gnotice.c
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
 *  $Id: iproc_gnotice.c,v 1.4 2003/07/01 11:01:20 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "listener.h"
#include "s_bsd.h"
#include "msg.h"
#include "interproc.h"

int iproc_gnotice(int parc, char **parv)
{
    sendto_serv_butone(NULL, &me, TOK1_GNOTICE, ":%s", parv[1]);
    sendto_gnotice("from Proxymon: %s", parv[1]);
    return 0;
}
