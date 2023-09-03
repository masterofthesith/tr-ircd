/************************************************************************
 *   IRC - Internet Relay Chat, server/s_validity.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers.
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
 * $Id: s_validity.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "numeric.h"

/*
 * valid_hostname - check hostname for validity
 *
 * Inputs       - pointer to user
 * Output       - YES if valid, NO if not
 * Side effects - NONE
 *
 * NOTE: this doesn't allow a hostname to begin with a dot and
 * will not allow more dots than chars.
 */
int valid_hostname(const char *hostname)
{ 
    const char *p = hostname;

    assert(0 != p);

    if ('.' == *p)
        return 0;

    while (*p) {  
        if (!IsHostChar(*p))
            return 0;
        p++;
    }

    return 1;
}

/*
 * valid_username - check username for validity
 *
 * Inputs       - pointer to user
 * Output       - YES if valid, NO if not
 * Side effects - NONE
 *
 * Absolutely always reject any '*' '!' '?' '@' in an user name
 * reject any odd control characters names.
 * Allow '.' in username to allow for "first.last"
 * style of username
 */
int valid_username(const char *username)
{
    int dots = 0;
    const char *p = username;

    assert(0 != p);

    if ('~' == *p)
        ++p;

    /* reject usernames that don't start with an alphanum
     * i.e. reject jokers who have '-@somehost' or '.@somehost'
     * or "-hi-@somehost", "h-----@somehost" would still be accepted.
     */
    if (!IsAlNum(*p))
        return 0;

    while (*++p) {
        if ((*p == '.')) {
            dots++;
            if (dots > 1)
                return 0;
            if (!IsUserChar(p[1]))
                return 0;
        } else if (!IsUserChar(*p))
            return 0;
    }
    return 1;
}

/*
 * Returns 1 if the user matches a drone style
 * discovered by PB@DAL.net
 */
int check_drone_PB(char *username, char *gcos)
{
    unsigned char *x;

    if (*username == '~')
        username++;

    if (strlen(username) <= 2)
        return 0;

    /* verify that it's all uppercase leters */
    for (x = (unsigned char *) username; *x; x++) {
        if (*x < 'A' || *x > 'Z')
            return 0;
    }

    if (strcmp(username, gcos))
        return 0;

    return 1;
}


