/************************************************************************
 *   IRC - Internet Relay Chat, src/logsystem/logsystem.c
 *   Copyright (C) 2000 TR-IRCD Development Team
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

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "tools.h"
#include "fileio.h"
#include "s_conf.h"
#include "h.h"

static char logbuf[LOG_BUFSIZE];

void init_logsystem(void *unused) {

	return;

}

char *logevent_getstyleformat(void) {
	return logstyleformat;
}

char *logevent_getstyletext(void) {
	return logstyletext;
}

int logevent_writelog(char *style, char *buf, aLogEvent *le) {

    dlink_node *ptr;
    aLogItem *li;

    ircsnprintf(logbuf, LOG_BUFSIZE-1, style, 
			smalldate(time(NULL)),
			le->name, buf);

    if (le->log_where & LOG_STDERR)
	fprintf(stderr, logbuf);

    if (GeneralOpts.foreground && (le->log_where & LOG_STDOUT))
	fprintf(stdout, logbuf);

    for (ptr = logitem_list.head; ptr; ptr = ptr->next) {
	li = ptr->data;
	if (li == NULL)
	    continue;
	if (GeneralOpts.debuglevel < le->log_level)
	    continue;
	if (le->log_where & li->where) {
	    if (le->log_when & li->when) 
		write_log(li, logbuf);
	}
    }	

    memset(logbuf, 0, LOG_BUFSIZE * sizeof(char));

    return 0;
}

