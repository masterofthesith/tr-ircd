/*
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
 */

/*
 * $Id: config.h,v 1.6 2003/07/01 11:01:18 tr-ircd Exp $ 
 */

#ifndef	CONFIG_H
#define	CONFIG_H 1

#include "setup.h"
#include "ircpath.h"

#define	CONFIGFILE	IRCD_PREFIX_ETC "/ircd.conf"
#define	MOTD		IRCD_PREFIX_ETC "/ircd.motd"
#define	SHORTMOTD	IRCD_PREFIX_ETC "/ircd.smotd"
#define SSL_CERTIFICATE	IRCD_PREFIX_ETC	"/ircd.cert"
#define SSL_KEY		IRCD_PREFIX_ETC	"/ircd.key"

#define RANDFILE	IRCD_PREFIX_VAR "/lib/" BASENAME "/ircd.rand"	/* used to make openssl keygen faster */
#define ENTROPYFILE	IRCD_PREFIX_VAR "/lib/" BASENAME "/ircd.entropy"
#define LINKSFILE       IRCD_PREFIX_VAR "/lib/" BASENAME "/ircd.links"
#define IRCD_PIDFILE    IRCD_PREFIX_VAR "/run/" BASENAME "/ircd.pid"

#define MODPATH 	IRCD_PREFIX_LIB "/modules"
#define LANGPATH 	IRCD_PREFIX_LIB "/languages"
#define CHANMODEPATH 	IRCD_PREFIX_LIB "/chanmodes"
#define CONTRIBPATH	IRCD_PREFIC_LIB "/contrib"
#define TOOLMODPATH	IRCD_PREFIX_LIB "/tools"

#define HELPPATH	IRCD_PREFIX_DOC "/help"

#define CLIENTS_PREALLOCATE     2048
#define CHANNELS_PREALLOCATE    2048
#define LINEBUF_PREALLOCATE     4096
#define THROTTLE_PREALLOCATE    1024
#define DLINK_PREALLOCATE       8192
#define MASKITEM_PREALLOCATE	4096

#include "defaults.h"

#endif 
