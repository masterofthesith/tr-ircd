/************************************************************************
 *   IRC - Internet Relay Chat, src/interproc/iproc_akill.c
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
 *  $Id: iproc_akill.c,v 1.4 2003/07/01 11:01:20 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "listener.h"
#include "s_bsd.h"
#include "msg.h"
#include "interproc.h"

int iproc_akill(int parc, char **parv)
{
    aMaskItem *ami, *ami2;
    char *user, *host, *reason, *akiller, buffer[1024], *current_date;
    time_t length = 0, timeset = 0;

    if (parc < 6)
        return 0;

    host = parv[1];
    user = parv[2];
    length = atoi(parv[3]);
    akiller = parv[4];
    timeset = atoi(parv[5]);
    reason = (parv[6] ? parv[6] : "<no reason>");

    current_date = smalldate((time_t) timeset);
    if (strlen(reason) > 250)
        reason[251] = 0;

    ami2 = find_maskitem(host, user, MASKITEM_AUTOKILL, 1);
    if (ami2 != NULL) {
        sendto_serv_butone(NULL, &me, TOK1_AKILL, "%s %s %d %s %d :%s",
                           host, user, length, akiller, timeset, reason);
        return 0;
    }

    ami = create_maskitem(akiller, host, user, MASKITEM_AUTOKILL, length);

    ircsprintf(buffer, "%s (%s)", reason, current_date);

    DupString(ami->reason, buffer);

    rehashed = 1;

    sendto_serv_butone(NULL, &me, TOK1_AKILL, "%s %s %d %s %d :%s",
                       host, user, length, akiller, timeset, reason);
    return 0;
}
