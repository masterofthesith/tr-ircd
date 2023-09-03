/************************************************************************
 *   IRC - Internet Relay Chat, include/eventmap.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
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

#ifndef EVENTMAP_H
#define EVENTMAP_H 1
#ifndef EVENTMAP_DEF

/* This is the module event<-->pathname map.
 * Whenever a module calls hook_add_event and 
 * initiates a new event, that event has to be declared
 * here with the pathname of the module. All this is
 * for the sake of module dependencies.
 * -dsginosa
 */

#ifdef HAVE_SHL_LOAD
/* This is a list of parent module paths.
 * Add modulepath here if it adds a new
 * hook event. (*.sl module types)
 * -dsginosa
 */
#define M_CORE          MODPATH "/m_core.sl"
#define M_MESSAGE       MODPATH "/m_message.sl"
#define M_REHASH        MODPATH "/m_rehash.sl"
#define M_SERVER        MODPATH "/m_server.sl"
#define M_SJOIN         MODPATH "/m_sjoin.sl"
#define M_STATS         MODPATH "/m_stats.sl"
#define M_SVSMODE       MODPATH "/m_svsmode.sl"
#define M_REXCOM	MODPATH "/m_rexcom.sl"
/* This is a list of sibling module paths.
 * Add modulepath here if it adds a new
 * hook to an already existing event.
 * ( *.sl module types)
 * -dsginosa
 */
#define M_OPERDO_HELP	"m_operdo_help.sl"
#define M_OPERDO_JOIN	"m_operdo_join.sl"
#define M_OPERDO_KICK	"m_operdo_kick.sl"
#define M_OPERDO_MODE	"m_operdo_mode.sl"
#define M_OPERDO_TOPIC	"m_operdo_topic.sl"
#define C_WORDFILTER	"wordfilter.sl"
#define C_REHASH_MORE	"rehash_more.sl"
#define P_PROXYMON	"proxymon.sl"
#define R_KLINE		"r_kline.sl"
#define R_JUPITER	"r_jupiter.sl"
#define R_QUARANTINE	"r_quarantine.sl"
#define R_GECOS		"r_gecos.sl"
#else
/* This is a list of parent module paths.
 * Add modulepath here if it adds a new 
 * hook event. (*.so module.types)
 * -dsginosa
 */
#define M_CORE 		MODPATH "/m_core.so"
#define M_MESSAGE 	MODPATH "/m_message.so"
#define M_REHASH	MODPATH	"/m_rehash.so"
#define M_SERVER	MODPATH	"/m_server.so"
#define M_SJOIN		MODPATH	"/m_sjoin.so"
#define M_STATS		MODPATH "/m_stats.so"
#define M_SVSMODE	MODPATH "/m_svsmode.so"
#define M_REXCOM	MODPATH "/m_rexcom.so"
/* This is a list of sibling module paths.
 * Add modulepath here if it adds a new
 * hook to an already existing event.
 * ( *.so module types)
 * -dsginosa
 */
#define M_OPERDO_HELP	"m_operdo_help.so"
#define M_OPERDO_JOIN	"m_operdo_join.so"
#define M_OPERDO_KICK	"m_operdo_kick.so"
#define M_OPERDO_MODE	"m_operdo_mode.so"
#define M_OPERDO_TOPIC	"m_operdo_topic.so"
#define C_WORDFILTER	"wordfilter.so"
#define C_REHASH_MORE	"rehash_more.so"
#define P_PROXYMON	"proxymon.so"
#define R_KLINE         "r_kline.so"
#define R_JUPITER       "r_jupiter.so"
#define R_QUARANTINE    "r_quarantine.so"
#define R_GECOS         "r_gecos.so"
#endif

modEvent eventmap[] = {
    {"load operdo core module", M_CORE, 1},
    {"calling m_private", M_MESSAGE, 1},
    {"doing rehash", M_REHASH, 1},
    {"inform remote servers", M_SERVER, 1},
    {"kill list", M_SJOIN, 1},
    {"doing stats", M_STATS, 1},
    {"channel svsmode", M_SVSMODE, 1},
    {"load rexcom core module", M_REXCOM, 1},
    {NULL, NULL, 0}
    ,
};

modEvent hookmap[] = {
    {"load operdo core module", M_OPERDO_HELP, 1},
    {"load operdo core module", M_OPERDO_JOIN, 1},
    {"load operdo core module", M_OPERDO_KICK, 1},
    {"load operdo core module", M_OPERDO_MODE, 1},
    {"load operdo core module", M_OPERDO_TOPIC, 1},
    {"calling m_private", C_WORDFILTER, 1},
    {"doing stats", C_WORDFILTER, 1},
    {"doing rehash", C_REHASH_MORE, 1},
    {"doing rehash", P_PROXYMON, 1},
    {"load rexcom core module", R_KLINE, 1},
    {"load rexcom core module", R_QUARANTINE, 1},
    {"load rexcom core module", R_GECOS, 1},
    {"load rexcom core module", R_JUPITER, 1},
    {NULL, NULL, 0}
    ,
};
#endif

extern int tryToLoadEvent(char *);
extern int unloadDependentHooks(char *);
extern int loadModule(char *);
extern int unloadModule(char *);

#endif
