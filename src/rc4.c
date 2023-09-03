/************************************************************************
 *   IRC - Internet Relay Chat, src/rc4.c
 *   Copyright (C) 2000 Lucas Madar
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
 * $Id: rc4.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

#include "setup.h"
#ifdef HAVE_ENCRYPTION_ON

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "dh.h"

/*
 * Transparent rc4 implementation
 * Based upon sample in crypto++ library,
 * which was based upon an anonymous usenet posting.
 * Implemented by Lucas Madar <lucas@dal.net>
 *
 * Remember that it is IMPERITAVE to generate a new key
 * for each state. DO NOT USE THE SAME KEY FOR ANY TWO STATES.
 */

struct rc4_state *rc4_initstate(unsigned char *key, int keylen)
{
    RC4DWORD i;
    RC4BYTE tmp, idx1, idx2;
    struct rc4_state *rc4;

    if (sizeof(RC4BYTE) != 1)
	exit(0);		/* MUST BE 1 BYTE! */
    if (sizeof(RC4DWORD) != 4)
	exit(0);		/* MUST BE 4 BYTES! */

    rc4 = (struct rc4_state *) MyMalloc(sizeof(struct rc4_state));
    memset(rc4, 0, sizeof(struct rc4_state));

    for (i = 0; i < 256; i++)	/* initialize our state array */
	rc4->mstate[i] = (RC4BYTE) i;

    for (i = 0, idx1 = idx2 = 0; i < 256; i++) {
	idx2 = (key[idx1++] + rc4->mstate[i] + idx2);
	tmp = rc4->mstate[i];
	rc4->mstate[i] = rc4->mstate[idx2];
	rc4->mstate[idx2] = tmp;

	if (idx1 >= keylen)
	    idx1 = 0;
    }

    return rc4;
}

void rc4_process_stream(struct rc4_state *rc4, unsigned char *istring, unsigned int stringlen)
{
    RC4BYTE *s = rc4->mstate;
    RC4DWORD x = rc4->x, y = rc4->y;

    while (stringlen--) {
    RC4DWORD a, b;

	x = (x + 1) & 0xFF;
	a = s[x];
	y = (y + a) & 0xFF;
	b = s[y];
	s[x] = b;
	s[y] = a;
	*istring++ ^= s[(a + b) & 0xFF];
    }

    rc4->x = (RC4BYTE) x;
    rc4->y = (RC4BYTE) y;
}

void
rc4_process_stream_to_buf(struct rc4_state *rc4,
			  const unsigned char *istring,
			  unsigned char *ostring, unsigned int stringlen)
{
    RC4BYTE *s = rc4->mstate;
    RC4DWORD x = rc4->x, y = rc4->y;

    while (stringlen--) {
    RC4DWORD a, b;

	x = (x + 1) & 0xFF;
	a = s[x];
	y = (y + a) & 0xFF;
	b = s[y];
	s[x] = b;
	s[y] = a;
	*ostring++ = *istring++ ^ s[(a + b) & 0xFF];
    }

    rc4->x = (RC4BYTE) x;
    rc4->y = (RC4BYTE) y;
}

void rc4_destroystate(struct rc4_state *rc4)
{
    memset(rc4, 0, sizeof(struct rc4_state));
    MyFree(rc4);
}

#else

void nothing(void);

void nothing(void)
{
    return;
}

#endif

