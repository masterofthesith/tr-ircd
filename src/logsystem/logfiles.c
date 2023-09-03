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

dlink_list logitem_list;

aLogItem *make_logitem(char *filename, int when, int where) {

    aLogItem *li;

    li = (aLogItem *) MyMalloc(sizeof(*li));
    memset(li, 0, sizeof(*li));
    DupString(li->filename, filename);
    li->when = when;
    li->where = where;
    li->file = NULL;
    li->active = 0;

    return li;

}

static void create_logitems() {

    aLogItem *li;
    dlink_node *ptr;

    li = make_logitem(LOG_FN_LOGFILE, LOG_ALWAYS | LOG_ON_LOG, LOG_LOGFILE);
    ptr = make_dlink_node();

    dlinkAdd(li, ptr, &logitem_list);

    li = make_logitem(LOG_FN_IRCLOG,  LOG_ALWAYS | LOG_ON_LOG, LOG_IRCLOG);
    ptr = make_dlink_node();
  
    dlinkAdd(li, ptr, &logitem_list);

    li = make_logitem(LOG_FN_DEBUGLOG,  LOG_ALWAYS | LOG_ON_LOG, LOG_DEBUGLOG);
    ptr = make_dlink_node();
  
    dlinkAdd(li, ptr, &logitem_list);

    li = make_logitem(LOG_FN_ERRORLOG, LOG_ALWAYS | LOG_ON_LOG, LOG_ERRORLOG);
    ptr = make_dlink_node();
  
    dlinkAdd(li, ptr, &logitem_list);

    li = make_logitem(LOG_FN_STDERR, LOG_ALWAYS | LOG_ON_LOG, LOG_STDERR);
    ptr = make_dlink_node();
  
    dlinkAdd(li, ptr, &logitem_list);

    li = make_logitem(LOG_FN_STDOUT, LOG_ALWAYS | LOG_ON_LOG, LOG_STDOUT);
    ptr = make_dlink_node();
  
    dlinkAdd(li, ptr, &logitem_list);

    li = make_logitem(LOG_FN_PROXY,  LOG_ALWAYS | LOG_ON_LOG, LOG_PROXYLOG);
    ptr = make_dlink_node();

    dlinkAdd(li, ptr, &logitem_list);

    li = make_logitem(LOG_FN_HTTPD,  LOG_ALWAYS | LOG_ON_LOG, LOG_HTTPLOG);
    ptr = make_dlink_node();

    dlinkAdd(li, ptr, &logitem_list);

}
 
static int open_one_logfile(aLogItem *li, const char *mode) {

    if ((li->when & LOG_ALWAYS) ||
	((li->when & LOG_ON_LOG) && GeneralOpts.enable_logging)) {
    	
	    li->file = fbopen(li->filename, mode);
    	    if (li->file == NULL) 
		return -2;
	    li->active = 1;
    	    return li->file->fd;
    }
    return 0;
}
 
static void open_logfiles() {

    dlink_node *ptr;
    aLogItem *li;

    for (ptr = logitem_list.head; ptr; ptr = ptr->next) {
	li = ptr->data;
	li->fd = open_one_logfile(li, "a");
	if (li->fd == -2)
	    fprintf(stderr, "ERROR: Opening file %s\n", li->filename); 
    }
}

static void close_logfiles() {

    dlink_node *ptr;
    aLogItem *li;

    for (ptr = logitem_list.head; ptr; ptr = ptr->next) {
        li = ptr->data;
        if (li) {
            fbclose(li->file);
            li->file = NULL;
            li->active = 0;
        }
    }
}

static void write_logbegin() {

    dlink_node *ptr;
    aLogItem *li;
    char beginbuf[1024];
    char *styletext;

    styletext = logevent_getstyletext();

    for (ptr = logitem_list.head; ptr; ptr = ptr->next) {
	li = ptr->data;
	if (li->file == NULL)
	    continue;
	if (li->active == 0)
	    continue;
	ircsnprintf(beginbuf, 1024, "\n\nBegin Logfile: %s\nDate: %s\n\nFormat:%s\n\n", 
				    li->filename, smalldate(time(NULL)), styletext);
	fbputs(beginbuf, li->file);
    }
}


void init_log_files(void *unused) {

    memset(&logitem_list, 0, sizeof(logitem_list));
    create_logitems();
    open_logfiles();
    write_logbegin();
}

void reinit_log_files(void) {

    close_logfiles();
    open_logfiles();
    write_logbegin();
}


void write_log(aLogItem *li, const char* message)
{
    if (li == NULL || message == NULL)
	return;
    if (li->file == NULL)
        return;
    if (li->active == 0)
	return;
    fbputs(message, li->file);
}
