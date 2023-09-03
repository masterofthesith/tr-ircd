#!/bin/sh
#   Copyright (C)2000-2003 TR-IRCD Development
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2, or (at your option)
#   any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

prefix=/home/delir/ircd

IRANDDIR=${exec_prefix}/bin
OUTPUTDIR=${prefix}/etc
RANDOMDIR=${prefix}/var/lib/tr-ircd
NUMOFDAYS=365

if test -r "$OUTPUTDIR/ircd.cert" ; then
	echo "You already have an SSL Certificate"
	echo "Please remove it before running this script"
	exit 255
fi

$IRANDDIR/irandom.sh

RANDFILE="$RANDOMDIR/ircd.rand"

echo "Generating SSL Certificate for your server"

openssl req -new -x509 -days $NUMOFDAYS -nodes -config "$OUTPUTDIR/ircdssl.conf" -out "$OUTPUTDIR/ircd.cert" -keyout "$OUTPUTDIR/ircd.key" -rand "$OUTPUTDIR/rnd"
openssl x509 -subject -dates -fingerprint -noout -in $OUTPUTDIR/ircd.cert
