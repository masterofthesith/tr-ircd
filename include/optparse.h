/************************************************************************
 *   IRC - Internet Relay Chat, include/optparse.h
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

#define OPT_NONE		0x001
#define OPT_STRING		0x002
#define OPT_DEBUG		0x004
#define OPT_USAGE		0x008
#define OPT_FOREGROUND  	0x010
#define OPT_LOG			0x020
#define OPT_WEBCONFIG		0x040
#define OPT_WEBCONFIG_BIND	0x080
#define OPT_WEBCONFIG_PORT	0x100
#define OPT_ROOT		0x200
#define OPT_MAXCLIENTS		0x400
#define OPT_SMALLNET		0x800

typedef struct {
    char *optname;
    char optchar;
    char *element;
    int opttype;
    char *optdesc;
} OptItem;

int process_commandline(int, char **);
void init_optparse(void *);
