/************************************************************************
 *   IRC - Internet Relay Chat, include/service.h
 *   Copyright (C) 2000 
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

extern char *service_modes_to_char(aService * svc);
extern char *service_info_to_char(aService * svc);

extern void sendto_service(int, int, aClient *, aChannel *, char *, char *, ...);
extern void free_service(aClient *);
extern void send_services(aClient *);
