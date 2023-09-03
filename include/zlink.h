/************************************************************************
 *   IRC - Internet Relay Chat, include/zlink.h
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
 *
 *
 */

#include "setup.h"

#define COMPRESSION_LEVEL	3
#define ZIP_MIN_BLOCK           1024    /* smallest block to compress */
#define ZIP_MAX_BLOCK           8192    /* largest block to compress */

/*
 * This shouldn't be necessary.
 * The outbuf should never be larger than
 * the maximum block.. should it?
 * I'll account for any weirdness in zlib.
 */

#define ZIPBUFSIZE (ZIP_MAX_BLOCK * 2)
#define UNZIPBUFSIZE (131072)

z_stream *input_unzipstream(void);
z_stream *output_zipstream(void);

int unzip_buffer(aClient *, char *, int, int (*) (aClient *, char *, size_t));

char *zip_buffer(aClient *, char *, int *);

void zip_get_stats(z_stream *z, unsigned long *insiz, unsigned long *outsiz, double *ratio);

void destroy_unzipstream(z_stream *z);
void destroy_zipstream(z_stream *z);
