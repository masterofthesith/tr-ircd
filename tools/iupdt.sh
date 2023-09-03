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

# This is the script run by cron to supply information for the iupdt module.
# It automates the task of cvs & make.

prefix="/home/delir/ircd"
SOURCEDIR="/home/delir/trircd-5-enhanced"
DESTFILE="${prefix}/var/cvscheck.tmp"

PATCHEDCOUNT=0
NEWFILECOUNT=0
CONFLICT=0
DO_MAKE=0

touch $DESTFILE

cd $SOURCEDIR
for i in `cvs update | awk {'print $1'}` ; do 
        if test "$i" == "P" ; then
		PATCHEDCOUNT=$((PATCHEDCOUNT+1))
        fi
        if test "$i" == "U" ; then
		NEWFILECOUNT=$((NEWFILECOUNT+1))
        fi
        if test "$i" == "C" ; then
		CONFLICT=$((CONFLICT+1))
        fi
done

if test $PATCHEDCOUNT -gt 0 ; then
	DO_MAKE=1
fi
if test $NEWFILECOUNT -gt 0 ; then
	DO_MAKE=1
fi
if test $CONFLICT -gt 0 ; then
	DO_MAKE=0
fi

while read PC NC WN ; do
	PATCHEDCOUNT=$((PATCHEDCOUNT+PC))
	NEWFILECOUNT=$((NEWFILECOUNT+NC))
	CONFLICT=$((CONFLICT+WN))
done < $DESTFILE

echo "$PATCHEDCOUNT $NEWFILECOUNT $CONFLICT" > $DESTFILE

if test $DO_MAKE -gt 0 ; then
	echo "make install"
fi
