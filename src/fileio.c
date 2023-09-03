/*
 * IRC - Internet Relay Chat, src/fileio.c
 * Copyright (C) 1998 Thomas Helvey <tomh@inxpress.net>
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
 *
 * $Id: fileio.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "fileio.h"

/* The following are to get the fd manipulation routines. eww. */
#include "fd.h"

/*
 * Wrappers around open() / close() for fileio, since a whole bunch of
 * code that should be using the fbopen() / fbclose() code isn't.
 * Grr. -- adrian
 */

int file_open(const char *filename, int mode, int fmode)
{
    int fd;
    fd = open(filename, mode, fmode);
    if (fd == MAXCONNECTIONS) {
	fd_close(fd);		/* Too many FDs! */
	errno = ENFILE;
	fd = -1;
    } else if (fd >= 0)
	fd_open(fd, FD_FILE, filename);

    return fd;
}

void file_close(int fd)
{
    /*
     * we set type to FD_FILECLOSE so we can get trapped
     * in fd_close() with type == FD_FILE. This will allow us to
     * convert all abusers of fd_close() of a FD_FILE fd over
     * to file_close() .. mwahaha!
     */
    assert(fd_table[fd].type == FD_FILE);
    fd_table[fd].type = FD_FILECLOSE;
    fd_close(fd);
}

FBFILE *fbopen(const char *filename, const char *mode)
{
    int openmode = 0;
    int pmode = 0;
    FBFILE *fb = NULL;
    int fd;
    assert(filename);
    assert(mode);

    while (*mode) {
	switch (*mode) {
	    case 'r':
		openmode = O_RDONLY;
		break;
	    case 'w':
		openmode = O_WRONLY | O_CREAT | O_TRUNC;
		pmode = 0644;
		break;
	    case 'a':
		openmode = O_WRONLY | O_CREAT | O_APPEND;
		pmode = 0644;
		break;
	    case '+':
		openmode &= ~(O_RDONLY | O_WRONLY);
		openmode |= O_RDWR;
		break;
	    default:
		break;
	}
	++mode;
    }

    if ((fd = file_open(filename, openmode, pmode)) == -1) {
	return fb;
    }

    if (NULL == (fb = fdbopen(fd, NULL)))
	file_close(fd);
    return fb;
}

FBFILE *fdbopen(int fd, const char *mode)
{
    /*
     * ignore mode, if file descriptor hasn't been opened with the
     * correct mode, the first use will fail
     */
FBFILE *fb = (FBFILE *) MyMalloc(sizeof(FBFILE));
    if (NULL != fb) {
	fb->ptr = fb->endp = fb->buf;
	fb->fd = fd;
	fb->flags = 0;
	fb->pbptr = (char *) NULL;
    }
    return fb;
}

void fbclose(FBFILE * fb)
{
    assert(fb);
    file_close(fb->fd);
    MyFree(fb);
}

static int fbfill(FBFILE * fb)
{
int n;
    assert(fb);
    if (fb->flags)
	return -1;
    n = read(fb->fd, fb->buf, BUFSIZ);
    if (0 < n) {
	fb->ptr = fb->buf;
	fb->endp = fb->buf + n;
    } else if (n < 0)
	fb->flags |= FB_FAIL;
    else
	fb->flags |= FB_EOF;
    return n;
}

int fbgetc(FBFILE * fb)
{
    assert(fb);
    if (fb->pbptr) {
	if ((fb->pbptr == (fb->pbuf + BUFSIZ)) || (!*fb->pbptr))
	    fb->pbptr = NULL;
    }

    if (fb->ptr < fb->endp || fbfill(fb) > 0)
	return *fb->ptr++;
    return EOF;
}

void fbungetc(char c, FBFILE * fb)
{
    assert(fb);

    if (!fb->pbptr) {
	fb->pbptr = fb->pbuf + BUFSIZ;
    }

    if (fb->pbptr != fb->pbuf) {
	fb->pbptr--;
	*fb->pbptr = c;
    }
}

char *fbgets(char *buf, size_t len, FBFILE * fb)
{
    char *p = buf;
    assert(buf);
    assert(fb);
    assert(0 < len);

    if (fb->pbptr) {
	strlcpy_irc(buf, fb->pbptr, len);
	fb->pbptr = NULL;
	return (buf);
    }

    if (fb->ptr == fb->endp && fbfill(fb) < 1)
	return 0;
    --len;
    while (len--) {
	*p = *fb->ptr++;
	if ('\n' == *p) {
	    ++p;
	    break;
	}
	/*
	 * deal with CR's
	 */
	else if ('\r' == *p) {
	    if (fb->ptr < fb->endp || fbfill(fb) > 0) {
		if ('\n' == *fb->ptr)
		    ++fb->ptr;
	    }
	    *p++ = '\n';
	    break;
	}
	++p;
	if (fb->ptr == fb->endp && fbfill(fb) < 1)
	    break;
    }
    *p = '\0';
    return buf;
}

int fbputs(const char *str, FBFILE * fb)
{
    int n = -1;
    assert(str);
    assert(fb);

    if (0 == fb->flags) {
	n = write(fb->fd, str, strlen(str));
	if (-1 == n)
	    fb->flags |= FB_FAIL;
    }
    return n;
}

int fbstat(struct stat *sb, FBFILE * fb)
{
    assert(sb);
    assert(fb);
    return fstat(fb->fd, sb);
}
