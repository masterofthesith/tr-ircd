/*
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
 *
 */

#define FEATURESET1 \
		"NOQUIT"	\
		" SAFELIST"	\
		" INVEX"	\
		" EXCEPTS"	\
		" WALLCHOPS"	\
		" MAP"		\
		" USERIP"	\
		" KNOCK"	\
		" CHARSET=rfc1459"   \
		" CASEMAPPING=rfc1459" \
		" MAXBANS=%i"	\
		" TOPICLEN=%i"  \
		" KICKLEN=%i"  

#define FEATURESET2 \
		"WATCH=%i"	\
                " MODES=%i"     \
                " SILENCE=%i"   \
		" NICKLEN=%i"	\
		" CHANTYPES=&#" \
		" PREFIX=%s"	\
		" CHANMODES=%s" \
		" NETWORK=%s"	\
		" MAXCHANNELS=%i"
