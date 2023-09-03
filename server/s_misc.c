/************************************************************************
 *   IRC - Internet Relay Chat, server/s_misc.c
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
 * $Id: s_misc.c,v 1.5 2004/12/25 01:19:20 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "numeric.h"

static char *months[] = {
    "January", "February", "March", "April",
    "May", "June", "July", "August",
    "September", "October", "November", "December"
};

static char *weekdays[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

/*
 * stats stuff
 */
char *date(time_t aClock)
{
static char buf[80], plus;
struct tm *lt, *gm;
struct tm gmbuf;
int minswest;

    if (!aClock)
	time(&aClock);
    gm = gmtime(&aClock);
    memcpy((char *) &gmbuf, (char *) gm, sizeof(gmbuf));
    gm = &gmbuf;
    lt = localtime(&aClock);

    if (lt->tm_yday == gm->tm_yday)
	minswest = (gm->tm_hour - lt->tm_hour) * 60 + (gm->tm_min - lt->tm_min);
    else if (lt->tm_yday > gm->tm_yday)
	minswest = (gm->tm_hour - (lt->tm_hour + 24)) * 60;
    else
	minswest = ((gm->tm_hour + 24) - lt->tm_hour) * 60;

    plus = (minswest > 0) ? '-' : '+';
    if (minswest < 0)
	minswest = -minswest;

    (void) ircsprintf(buf, "%s %s %d %04d -- %02d:%02d %c%02d:%02d",
		      weekdays[lt->tm_wday], months[lt->tm_mon],
		      lt->tm_mday, lt->tm_year + 1900, lt->tm_hour,
		      lt->tm_min, plus, minswest / 60, minswest % 60);

    return buf;
}

/*
 */
char *smalldate(time_t aClock)
{
static char buf[MAX_DATE_STRING + 1];
struct tm *lt, *gm;
struct tm gmbuf;

    if (!aClock)
        time(&aClock);
    gm = gmtime(&aClock);
    memcpy((char *) &gmbuf, (char *) gm, sizeof(gmbuf));
    gm = &gmbuf;

    lt = localtime(&aClock);
    strftime(buf, MAX_DATE_STRING, "%F %T", lt);

    return buf;
}

