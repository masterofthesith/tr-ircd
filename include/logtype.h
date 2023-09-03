/************************************************************************
 *   IRC - Internet Relay Chat, include/logtype.h
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

#ifndef LOGTYPE_H
#define LOGTYPE_H 1

/* log types */

#define LOG_ALWAYS	0x01	/* Element is logged always */
#define LOG_ON_LOG	0x02	/* Element is logged only if log directive exists */

#define LOG_LOGFILE	0x01	/* Element is logged only to its logfile */
#define LOG_IRCLOG	0x02	/* Element is logged only to the server logfile */
#define LOG_DEBUGLOG	0x04	/* Element is logged only to the debugging logfile */
#define LOG_ERRORLOG	0x08	/* Element is logged only to the error logfile */
#define LOG_STDERR	0x10	/* Element is logged to the standard error */
#define LOG_STDOUT	0x20	/* Element is logged to the standard out */
#define LOG_PROXYLOG	0x40	/* Element is logged to the proxy logfile */
#define LOG_HTTPLOG	0x80	/* Element is logged to the http logfile */

#define LOG_FN_LOGFILE	IRCD_PREFIX_VAR "/log/" BASENAME "/events.log"
#define LOG_FN_IRCLOG	IRCD_PREFIX_VAR "/log/" BASENAME "/ircd.log"
#define LOG_FN_DEBUGLOG	IRCD_PREFIX_VAR "/log/" BASENAME "/debug.log"
#define LOG_FN_ERRORLOG	IRCD_PREFIX_VAR "/log/" BASENAME "/error.log"
#define LOG_FN_STDERR	IRCD_PREFIX_VAR "/log/" BASENAME "/stderr.log"
#define LOG_FN_STDOUT	IRCD_PREFIX_VAR "/log/" BASENAME "/stdout.log"
#define LOG_FN_PROXY	IRCD_PREFIX_VAR "/log/" BASENAME "/proxy.log"
#define LOG_FN_HTTPD	IRCD_PREFIX_VAR "/log/" BASENAME "/httpd.log"

#define LOG_FATAL	-2	/* Element is to be logged always */
#define LOG_ERROR	-1	/* Element has error level logging */
#define LOG_NOTICE	0	/* Element has notice level logging */
#define LOG_INFO	1	/* Element has info level logging */
#define LOG_DEBUG	2	/* Element has debug level logging */

typedef struct LogEvent aLogEvent;
typedef struct LogItem aLogItem;

#define LOG_BUFSIZE     4000

struct LogEvent {

    char *name;		/* unique eventlog name */
    long id;		/* eventlog id */
    
    int log_when;	/* when to write the log */
    int log_where;	/* where to write the log */
    int log_level;	/* level of the log */

    char *log_format;	/* pattern of the log element */

};

struct LogItem {

    char *filename;	/* Log Filename */
    FBFILE *file;

    int fd;		/* File descriptor of the logfile */
    int when;
    int where;
    int active;	
};

extern dlink_list logitem_list;

extern aLogItem *make_logitem(char *filename, int when, int where);

extern void dump_logevents(aClient *);

extern int logevent_register(char *, int, int, int, char *);
extern void logevent_unregister(int); 

extern void logevent_call(int, ...);
extern void logevent_personal(char *, ...);

extern void init_logevents(void *);
extern void init_logsystem(void *);
extern void init_log_elems(void *);
extern void init_log_files(void *);
extern void reinit_log_files(void);

extern int logevent_writelog(char *, char *, aLogEvent *);

extern char *logevent_getstyleformat(void);
extern char *logevent_getstyletext(void);

extern char *logstyleformat;
extern char *logstyletext;

extern void write_log(aLogItem *, const char *);

#endif
