/************************************************************************
 *   IRC - Internet Relay Chat, include/packet.h
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
 *
 *
 * "packet.h". - Headers file.
 *
 * $Id: packet.h,v 1.4 2003/07/01 11:01:18 tr-ircd Exp $
 *
 */

#ifndef PACKET_H
#define PACKET_H 1

extern void init_dopacket(void *);
extern void parse_client_queued(aClient *);

extern PF  flood_recalc;

extern PF read_server_packet;
extern PF read_client_packet;

#endif
