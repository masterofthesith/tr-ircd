/************************************************************************
 *   IRC - Internet Relay Chat, src/interproc/iproc_kline.c
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
 *  $Id: iproc_kline.c,v 1.5 2004/02/24 15:00:30 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "listener.h"
#include "s_bsd.h"
#include "interproc.h"

int iproc_kline(int parc, char **parv)
{
    aMaskItem *ami, *ami2;
    char *user, *host, *reason, buffer[1024], *current_date;
    time_t length = 0;

    if (parc < 4)
        return 0;

    host = parv[1];
    user = parv[2];
    length = atoi(parv[3]);
    reason = (parv[4] ? parv[4] : "<no reason>");

    current_date = smalldate(time(NULL));
    if (strlen(reason) > 250)
        reason[251] = 0;

    ami2 = find_maskitem(host, user, MASKITEM_KLINE, 1);
    if (ami2 != NULL) 
        return 0;

    ami = create_maskitem(me.name, host, user, MASKITEM_KLINE, length);

    ircsprintf(buffer, "%s (%s)", reason, current_date);

    DupString(ami->reason, buffer);

    rehashed = 1;
    return 0;
}
