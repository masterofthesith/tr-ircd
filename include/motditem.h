/************************************************************************
 *   IRC - Internet Relay Chat, include/motditem.h
 *   Copyright (C) 2000 TR-IRCD Development
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

extern void send_message_file(aMotd *, aClient *, aClient *, int, char **);
extern void send_config_file(aMotd *, aClient *, aClient *, char *);
extern void read_message_file(aMotd *);
extern void init_message_file(int, char *, aMotd *);

#define MF_MOTD  	0x001	/* of type motd */
#define MF_SHORTMOTD	0x002	/* of type shortmotd */
#define MF_HELP		0x004	/* of type helpfile */
#define MF_LINKS	0x008	/* of type linksfile */
#define MF_CONF		0x020	/* of type linksfile */

void sendhelpfile(struct Client *, char *, char *, char *);
void dohelp(struct Client *, char *, char *, char *);
