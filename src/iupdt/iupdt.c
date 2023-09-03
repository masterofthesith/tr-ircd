/************************************************************************
 *   IRC - Internet Relay Chat, src/iupdt/iupdt.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 *
 *  $Id: iupdt.c,v 1.2 2003/06/14 13:55:53 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "s_user.h"

void check_update(aClient *cptr) {

    FILE *cvsfile;

    int pcount = 0;	/* num of patched files */
    int ucount = 0;	/* num of new files */
    int ccount = 0;	/* num of conflicts */

    cvsfile = fopen(IRCD_PREFIX_VAR "/cvscheck.tmp", "r");

    if (!cvsfile) {
	send_me_notice(cptr, "No iupdt (ircd update) configuration found. "
			     "Please check your system to ensure that the "
			     "bin/iupdt.sh script is running.");
	return;
    }

    fscanf(cvsfile, "%d %d %d", &pcount, &ucount, &ccount);

    fclose(cvsfile);

    if (pcount > 0 || ucount > 0) {
	send_me_notice(cptr, "The ircd sources have been updated. "
			     "Number of changed files: %d, number of new files: %d",
			     pcount, ucount);
    }

    if (ccount) {
	send_me_notice(cptr, "Conflicts were detected in %d files "
			     "Compilation failed, please check your installation",
			     ccount);
    } else {
	send_me_notice(cptr, "No conflicts were detected in your installation "
			     "You can now /restart your ircd if you want");
    }
 
    return;
}
