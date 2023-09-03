/************************************************************************
 *   IRC - Internet Relay Chat, src/zlink.c
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
 * $Id: zlink.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

/*
 * This streaming ircd zlib implementation was
 * inspired mostly by dianora's example in hybrid-6
 * - lucas
 * This implementation is now rewritten by TimeMr14C
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "zlink.h"

static char zipbuf[ZIPBUFSIZE];
static char unzipbuf[UNZIPBUFSIZE];

z_stream *input_unzipstream(void)
{
    z_stream *zip = (z_stream *) MyMalloc(sizeof(z_stream));

    if (inflateInit(zip) != Z_OK)
	return NULL;

    return zip;
}

z_stream *output_zipstream(void)
{
    z_stream *zip = (z_stream *) MyMalloc(sizeof(z_stream));

    if (deflateInit(zip, COMPRESSION_LEVEL) != Z_OK)
        return NULL;

    return zip;
}


int unzip_buffer(aClient *sptr, char *buffer, int len, int (*parserfunc) (aClient *sptr, char *buf, size_t len))
{
    int ret;
    z_stream *zin = sptr->serv->zin;

    memset(unzipbuf, 0, sizeof(char) * UNZIPBUFSIZE),
    zin->next_in = (Bytef *) buffer;
    zin->avail_in = len;
    zin->next_out = (Bytef *) unzipbuf;
    zin->avail_out = UNZIPBUFSIZE;

    while (zin->avail_in) {
    	ret = inflate(zin, Z_NO_FLUSH);
	if (ret == Z_OK) 
	    parserfunc(sptr, unzipbuf, UNZIPBUFSIZE - zin->avail_out);
	else 
	    return -1;
    }
    return 0;
}

char *zip_buffer(aClient *sptr, char *buffer, int *len)
{
    z_stream *zout = sptr->serv->zout;
    int ret;

    zout->next_in = (Bytef *) buffer;
    zout->avail_in = *len;
    zout->next_out = (Bytef *) zipbuf;
    zout->avail_out = ZIPBUFSIZE;

    ret = deflate(zout, Z_PARTIAL_FLUSH);

    if (ret == Z_OK) {
	*len = ZIPBUFSIZE - zout->avail_out;
	return zipbuf;
    }

    *len = -1;
    return NULL;
}

void zip_get_stats(z_stream *z, unsigned long *insiz, unsigned long *outsiz, double *ratio)
{
    *insiz = z->total_in;
    *outsiz = z->total_out;

    if (*insiz)
	*ratio = ((100.0 * (double) z->total_out) / (double) z->total_in);
}

void destroy_unzipstream(z_stream *z)
{
    deflateEnd(z);
}

void destroy_zipstream(z_stream *z)
{
    inflateEnd(z);
}

