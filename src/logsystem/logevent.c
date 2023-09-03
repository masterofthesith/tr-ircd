/************************************************************************
 *   IRC - Internet Relay Chat, src/logsystem/logevent.c
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
#include "numeric.h"
#include "h.h"

dlink_list logevent_list;

static char logbuffer[1024];


void init_logevents(void *unused)
{
    memset(&logevent_list, 0, sizeof(logevent_list));
}

static aLogEvent *make_logevent(char *name, char *format) {

    aLogEvent *le;
    char txt[PASSWDLEN];
    char out[PASSWDLEN];

    le = (struct LogEvent *) MyMalloc(sizeof(*le));
    memset(le, 0, sizeof(*le));
    DupString(le->name, name);
    DupString(le->log_format, format);

    strlcpy_irc(txt, name, PASSWDLEN);
    
    le->id = atol(calchash(txt, out));

    le->log_when = 0;
    le->log_where = 0;
    le->log_level = 0;

    return le;
}

int logevent_register(char *name, int log_when, int log_where, 
		      int log_type, char *log_format) {

    dlink_node *ptr;

    aLogEvent *new_logevent;

    ptr = make_dlink_node();
    new_logevent = make_logevent(name, log_format);

    new_logevent->log_when |= log_when;
    new_logevent->log_where |= log_where;
    new_logevent->log_level = log_type;

    dlinkAdd(new_logevent, ptr, &logevent_list);

    return new_logevent->id;
}


void logevent_unregister(int id) {

    dlink_node *ptr;
    aLogEvent *le;

    for (ptr = logevent_list.head; ptr; ptr = ptr->next) {
	le = ptr->data;
	if (le->id == id) {
	    dlinkDelete(ptr, &logevent_list);
            MyFree(le->name);
            MyFree(le->log_format);
	    MyFree(le);
            return;
        }
    }
    return;
}

void dump_logevents(aClient *cptr) {
    
    dlink_node *ptr;
    aLogEvent *le;

    for (ptr = logevent_list.head; ptr; ptr = ptr->next) {
	le = ptr->data;
	send_me_numeric(cptr, RPL_LOGENTLIST, le->id, le->name);
    }
    
}
static aLogEvent *logevent_find(int id) {

    dlink_node *ptr;
    aLogEvent *le;

    for (ptr = logevent_list.head; ptr; ptr = ptr->next) {
        le = ptr->data;
        if (le->id == id) 
            return le;
    }
    return NULL;
}

void logevent_call(int id, ...) {

    aLogEvent *le;
    va_list vl;    
    char *style;

    if (id <= 0)
	return;

    le = logevent_find(id);

    if (!le)
	return;

    va_start(vl, id);
    ircvsnprintf(logbuffer, 1023, le->log_format, vl);
    va_end(vl);

    style = logevent_getstyleformat();

    logevent_writelog(style, logbuffer, le);

    return;

}

void logevent_personal(char *pattern, ...) {

    va_list vl;

    va_start(vl, pattern);
    ircvsnprintf(logbuffer, 512, pattern, vl);
    va_end(vl);

    logevent_call(LogSys.generalerror, logbuffer);

    return;
}
