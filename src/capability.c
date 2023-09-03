/************************************************************************
 *   IRC - Internet Relay Chat, src/capability.c
 *
 *   Copyright (C)2000-2003 TR-IRCD Development
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

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "tools.h"
#include "hook.h"
#include "h.h"
#include "msg.h"
#include "numeric.h"

/* *INDENT-OFF* */
struct capablist my_capabs[] = {
    { "ZIP",       CAPAB_ZIP },
#ifdef HAVE_ENCRYPTION_ON
    { "DKEY",      CAPAB_DKEY },
#endif
    { "SERVICE",   CAPAB_SERVICES },
    { "TOKEN1",    CAPAB_TOKEN1 },
    { "IDENTITY",  CAPAB_IDENT },
    { "HIDENAME",  CAPAB_HIDENAME },
    { "EXCAP", 	   CAPAB_EXCAP },
    { NULL, 0},
};

struct capablist my_excapabs[] = {
    {"JOINDELAY", CAPAB_JOINDLY },
    { NULL, 0},
};

/* *INDENT-ON* */

static void make_capab_string(char *buf, unsigned int noflags)
{
    int i;
    int c;
    *buf = '\0';
    for (i = c = 0; my_capabs[i].name; i++) {
        if (my_capabs[i].flag & noflags)  
            continue;
        if (c)
            strcat(buf, " ");
        strcat(buf, my_capabs[i].name);
        c++;
    }
    /* A Small kludge */
    strcat(buf, " TRIRCD5");
}

static void make_excap_string(char *buf, unsigned int noflags)
{ 
    int i;
    int c;
    *buf = '\0';
    for (i = c = 0; my_excapabs[i].name; i++) {
        if ((my_excapabs[i].flag & noflags))
            continue;
        if (c)
            strcat(buf, " ");
        strcat(buf, my_excapabs[i].name);
        c++;
    }
}  

void send_capab_to(aClient *cptr, int noflag)
{
    char capabbuf[500];
    char excapbuf[500];
    make_capab_string(capabbuf, noflag);
    sendto_one(cptr, "%s %s", MSG_CAPAB, capabbuf);
    make_excap_string(excapbuf, noflag);
    sendto_one(cptr, "%s :%s", MSG_EXCAP, excapbuf);
    return;
}


