/************************************************************************
 *   IRC - Internet Relay Chat, include/h.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1992 Darren Reed
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

#ifndef IRCSPRINTF_H
#define IRCSPRINTF_H

int ircsprintf(char *str, const char *format, ...);
int ircvsprintf(char *str, const char *format, va_list ap);
int ircvsnprintf(char *str, size_t size, const char *format,
		 va_list ap);
int ircsnprintf(char *str, size_t size, const char *format, ...);

extern void iprintf_init(void *);

#endif
