/************************************************************************
 *   IRC - Internet Relay Chat, src/textedit.c
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
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "s_user.h"
#include "s_conf.h"
#include "../lib/md5/md5.h"

/* HOSTMASKING, the way we want it */

#ifndef IRCPASSSWD
char *calcmask(char *myip, char *yourip)
{
    int cnt;
    unsigned char arr[16];
    char res[HOSTLEN], out[HOSTLEN];
 
    md5_buffer(myip, HOSTLEN, arr);  
 
    sprintf(res, "%s.%01X", ServerInfo.networkname, arr[0]);

    for (cnt = 1; cnt < 7; ++cnt) {
        sprintf(out, "%01X", arr[cnt]);
        strcat(res, out);
    }

    strcat(res, FAKEHOST_POSTFIX);      /* defined in setup.h */
    strcpy(yourip, res);
    return yourip;
}
#endif

char *calcpass(char *mypass, char *yourpass)
{
    int cnt;
    unsigned char arr[16];
    char res[PASSWDLEN], out[PASSWDLEN];
    size_t t = strlen(mypass);

    md5_buffer(mypass, t, arr);

    sprintf(res, "%01X", arr[0]);

    for (cnt = 1; cnt < 7; ++cnt) {
        sprintf(out, "%01X", arr[cnt]);
        strcat(res, out);
    }

    strcpy(yourpass, res);
    return yourpass;
}

char *calchash(char *a, char *b)
{
    int cnt;
    unsigned char arr[16];
    char res[PASSWDLEN], out[PASSWDLEN];
    size_t t = strlen(a);
 
    md5_buffer(a, t, arr);
 
    sprintf(res, "%d", arr[0]);
 
    for (cnt = 1; cnt < 2; ++cnt) {
        sprintf(out, "%d", arr[cnt]);
        strcat(res, out);
    }
 
    strcpy(b, res);
    return b;
} 

