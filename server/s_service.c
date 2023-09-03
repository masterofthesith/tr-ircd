/************************************************************************
 *   IRC - Internet Relay Chat, server/s_service.c
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

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "h.h"

static char buf[BUFSIZE];
static char buf2[BUFSIZE];

int service_info[] = {
    SMODE_U, 'U',
    SMODE_A, 'A',
    0, 0
};

int service_modes[] = {
    SERVICE_SEE_PREFIX, 'P',
    SERVICE_SEE_OPERS, 'O',
    SERVICE_SEE_NICKS, 'N',
    SERVICE_SEE_QUITS, 'Q',
    SERVICE_SEE_KILLS, 'K',
    SERVICE_SEE_SERVERS, 'S',
    SERVICE_SEE_SERVICES, 'Z',
    SERVICE_SEE_SQUITS, 'L',
    SERVICE_SEE_RNICKS, 'R',
    SERVICE_SEE_UMODES, 'U',
    SERVICE_SEE_JOINS, 'j',
    SERVICE_SEE_KICKS, 'k',
    SERVICE_SEE_PARTS, 'p',
    SERVICE_SEE_MODES, 'm',
    SERVICE_SEE_TOPIC, 't',
    0, 0
};

aService *firstservice = NULL;

aService *make_service(aClient *cptr)
{

    aService *svc = cptr->service;

    if (!svc) {
	cptr->service = svc = (aService *) MyMalloc(sizeof(aService));
	memset(svc, 0, sizeof(aService));

	svc->scptr = cptr;
	if (firstservice)
	    firstservice->prev = svc;
	svc->next = firstservice;
	svc->prev = NULL;
	firstservice = svc;
    }
    return svc;
}

void free_service(aClient *cptr)
{

    aService *svc = cptr->service;

    if (svc) {

	if (svc->next)
	    svc->next->prev = svc->prev;
	if (svc->prev)
	    svc->prev->next = svc->next;
	if (firstservice == svc)
	    firstservice = svc->next;
	if (svc->server)
	    MyFree(svc->server);

	MyFree((char *) svc);
	cptr->service = NULL;

    }

}

char *service_modes_to_char(aService * svc)
{

char *m;
int e;
int *s;

    m = buf2;
    *m = '\0';
    for (s = service_modes; (e = *s); s += 2)
	if (svc->enable & e)
	    *m++ = *(s + 1);
    *m = '\0';

    return buf2;
}

char *service_info_to_char(aService * svc)
{

char *m;
int f;
int *s;

    m = buf;
    *m = '\0';
    for (s = service_info; (f = *s); s += 2)
	if (svc->sflags & f)
	    *m++ = *(s + 1);
    *m = '\0';

    return buf;

}

void send_services(aClient *to)
{

    aService *svc;
    aClient *acptr;

    if (!HasServices(to))
	return;

    for (svc = firstservice; svc; svc = svc->next) {
	if (((acptr = svc->scptr) != NULL) && (svc->scptr != to)) {

	    sendto_one_server(to, &me, TOK1_SERVICE,
			      "%s 1 %T %s %s %s %s :%s", acptr->name,
			      acptr, service_info_to_char(svc),
			      acptr->username, acptr->sockhost, svc->server, acptr->info);
	}
    }
    return;

}
