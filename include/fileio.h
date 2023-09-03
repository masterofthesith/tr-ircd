/* - Internet Relay Chat, include/fileio.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1999 Thomas Helvey <tomh@inxpress.net>
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
 * $Id: fileio.h,v 1.4 2003/07/01 11:01:18 tr-ircd Exp $
 */
#ifndef FILEIO_H
#define FILEIO_H 1

#define FB_EOF  0x01
#define FB_FAIL 0x02

struct FileBuf {
  int   fd;           /* file descriptor */
  char* endp;         /* one past the end */
  char* ptr;          /* current read pos */
  char* pbptr;        /* pointer to push back char */
  int   flags;        /* file state */
  char  buf[BUFSIZ];  /* buffer */
  char  pbuf[BUFSIZ+1]; /* push back buffer */
};

/* XXX This shouldn't be here */
struct Client;

/*
 * FileBuf is a mirror of the ANSI FILE struct, but it works for any
 * file descriptor. FileBufs are allocated when a file is opened with
 * fbopen, and they are freed when the file is closed using fbclose.
 */
typedef struct FileBuf FBFILE;

/*
 * open a file and return a FBFILE*, see fopen(3)
 */
extern FBFILE* fbopen(const char* filename, const char* mode);
/*
 * associate a file descriptor with a FBFILE*
 * if a FBFILE* is associated here it MUST be closed using fbclose
 * see fdopen(3)
 */
extern FBFILE* fdbopen(int fd, const char* mode);
/*
 * close a file opened with fbopen, see fclose(3)
 */
extern void    fbclose(FBFILE* fb);
/* 
 * return the next character from the file, EOF on end of file
 * see fgetc(3)
 */
extern int     fbgetc(FBFILE* fb);
/*
 * return next string in a file up to and including the newline character
 * see fgets(3)
 */
extern char*   fbgets(char* buf, size_t len, FBFILE* fb);
/*
 * ungets c to fb see ungetc(3)
 */
extern void    fbungetc(char c, FBFILE* fb);
/*
 * write a null terminated string to a file, see fputs(3)
 */
extern int     fbputs(const char* str, FBFILE* fb);
/*
 * return the status of the file associated with fb, see fstat(3)
 */
extern int     fbstat(struct stat* sb, FBFILE* fb);
/*
 * popen a file.
 */
extern FBFILE *fbpopen(const char *, const char *);

/* file_open / file_close -- adrian */
extern int     file_open(const char *filename, int mode, int fmode);
extern void    file_close(int fd);

#endif 
