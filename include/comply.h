/************************************************************************
 *   IRC - extern internet Relay Chat, include/comply.h
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

#ifndef COMPLY_H
#define COMPLY_H

extern int init_protocol(int);
extern int protocol_comply(void);
extern int protocol_rehash(void);

extern void proto_modinit(void);
extern int _comply_unload_modules(void);
extern int _comply_ignore_messages(void);
extern int _comply_untokenize(void);
extern int _comply_retokenize(void);
extern int _comply_unuse_messages(void);
extern int _comply_rehash(void);

#endif

