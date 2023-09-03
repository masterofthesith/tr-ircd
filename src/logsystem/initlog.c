/************************************************************************
 *   IRC - Internet Relay Chat, src/logsystem/initlog.c
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
#include "s_conf.h"
#include "fileio.h"
#include "logtype.h"

void init_log_elems(void *unused)
{
    LogSys.generalerror = 
	logevent_register("general notice", LOG_ALWAYS, LOG_IRCLOG | LOG_ERRORLOG,
			  LOG_ERROR, "%s");
    LogSys.restartmsg =
	logevent_register("restart mesg", LOG_ALWAYS, LOG_IRCLOG | LOG_LOGFILE,
			  LOG_ERROR, "Restarting Server: %s");
    LogSys.restart =
	logevent_register("restart server", LOG_ALWAYS, LOG_IRCLOG | LOG_LOGFILE,
			  LOG_ERROR, "Restarting server...");
    LogSys.execv =
	logevent_register("execv failed", LOG_ALWAYS, LOG_IRCLOG | LOG_ERRORLOG | LOG_STDOUT,
			  LOG_FATAL, "execv(ircd,%s) failed: %m");
    LogSys.report_error =
	logevent_register("report error", LOG_ON_LOG, LOG_IRCLOG, LOG_NOTICE, "%s");
    LogSys.read_error =
	logevent_register("read error", LOG_ON_LOG, LOG_ERRORLOG, LOG_DEBUG,
			  "READ ERROR from %C: fd = %d %d %d");
    LogSys.add_confline = 
	logevent_register("add confline", LOG_ON_LOG, LOG_LOGFILE, LOG_NOTICE,
			  "Read Init: (%d) (%s) (%s) (%s) (%d)");
    LogSys.bad_connect = 
	logevent_register("bad connect block", LOG_ON_LOG, LOG_LOGFILE, LOG_NOTICE,
			  "Bad connect block, name %s");
    LogSys.server_noquit = 
	logevent_register("server noquit", LOG_ON_LOG, LOG_DEBUGLOG, LOG_DEBUG,
			  "%s for %C");
    LogSys.exit_server = 
	logevent_register("exit server", LOG_ON_LOG, LOG_DEBUGLOG, LOG_DEBUG,
			  "%C %C for %s");
    LogSys.exit_client = 
	logevent_register("enter exit client", LOG_ON_LOG, LOG_DEBUGLOG, LOG_DEBUG,
			  "Entering exit_client for %C from %C [%s]");
    LogSys.hash_error = 
	logevent_register("hash error", LOG_ON_LOG, LOG_DEBUGLOG, LOG_DEBUG,
			  "%#x !in tab %s[%s] %#x %#x %#x %d %d %#x");
    LogSys.conncheck = 
	logevent_register("connection check", LOG_ON_LOG, LOG_DEBUGLOG, LOG_DEBUG,
			  "Connection check at: %s");
    LogSys.serverkill = 
	logevent_register("server kill", LOG_ALWAYS, LOG_ERRORLOG,
			  LOG_FATAL, "Server killed: %s");
    LogSys.modnoinit = 
	logevent_register("module has no init", LOG_ON_LOG, LOG_ERRORLOG,
			  LOG_ERROR, "Module %s has no modinit() function");
    LogSys.modload = 
	logevent_register("load module", LOG_ON_LOG, LOG_IRCLOG,
			  LOG_NOTICE, "Module %s has been loaded successfully");
    LogSys.modunload = 
	logevent_register("unload module", LOG_ON_LOG, LOG_IRCLOG,
                          LOG_NOTICE, "Module %s has been unloaded successfully");
    LogSys.noprotocol = 
	logevent_register("no protocol module", LOG_ALWAYS, LOG_IRCLOG | LOG_ERRORLOG | LOG_STDERR,
			  LOG_FATAL, "Error loading protocol module %s");
    LogSys.parse_unknown_prefix =
        logevent_register("unknown prefix", LOG_ON_LOG, LOG_LOGFILE, LOG_INFO,
                          "Unknown prefix (%s)(%s) from (%s)");   
    LogSys.parse_unknown_message =               
        logevent_register("unknown message", LOG_ON_LOG, LOG_LOGFILE, LOG_INFO,
                          "Message (%s) coming from (%s)");           
    LogSys.parse_empty =
        logevent_register("empty message", LOG_ON_LOG, LOG_LOGFILE, LOG_NOTICE,
                          "Empty message from host %s:%s");
    LogSys.parse_unknown =
        logevent_register("unknown message parse", LOG_ON_LOG, LOG_LOGFILE, LOG_INFO,
                          "Unknown (%s) from %s");
    LogSys.parse_debug =
        logevent_register("parse debug", LOG_ON_LOG, LOG_DEBUGLOG, LOG_DEBUG,
                          "%s parse from %^C: %s");
    LogSys.timeout = 
	logevent_register("AUTH/DNS timeout", LOG_ON_LOG, LOG_LOGFILE, LOG_INFO, 
			  "DNS/AUTH timeout %s");
    LogSys.ping = 
	logevent_register("next ping", LOG_ON_LOG, LOG_LOGFILE, LOG_INFO, 
			  "Next check_ping() call at: %s, %d %d %d");
    LogSys.pid = 
	logevent_register("pidfile events", LOG_ALWAYS, LOG_ERRORLOG, LOG_ERROR,
			  "Error in %s to pid file %s");
    LogSys.send_debug = 
	logevent_register("send debug", LOG_ON_LOG, LOG_DEBUGLOG, LOG_DEBUG,
			  "Sending to %^C len %d: %s");
    LogSys.badconfigline =
	logevent_register("bad config file", LOG_ALWAYS, LOG_ERRORLOG, LOG_ERROR,
			  "Bad config line: %s");
    LogSys.yyerror = 
	logevent_register("parser error", LOG_ALWAYS, LOG_ERRORLOG | LOG_STDERR, LOG_ERROR,
			  "%d: %s on line: %s");
    LogSys.cyyerror = 
	logevent_register("classes parser error", LOG_ALWAYS, LOG_ERRORLOG | LOG_STDERR, LOG_ERROR,
			  "%d: %s on line: %s");
    LogSys.conferror = 
	logevent_register("config failed", LOG_ALWAYS, LOG_ERRORLOG, 
			  LOG_ERROR, "Failed in reading configuration file %s");
    LogSys.lexer_deep =
	logevent_register("too deep includes", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR,
			  "Includes nested too deep in %s");
    LogSys.lexer_noinc = 
	logevent_register("cannot include", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR,
			  "Cannot include %s");
    LogSys.parser_netmask = 
	logevent_register("invalid netmask", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR,
			  "Invalid netmask for server vhost(%s)");
    LogSys.parser_spoof = 
	logevent_register("invalid spoof length", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR,
			  "Spoofs must be less than %d..ignoring it");
    LogSys.ssl_error =
	logevent_register("ssl error", LOG_ON_LOG, LOG_ERRORLOG, LOG_ERROR, 
			  "SSL error in %s: %s [%s]");
    LogSys.proxyevent =
        logevent_register("proxy event", LOG_ON_LOG, LOG_PROXYLOG, LOG_NOTICE, "%s");

    LogSys.httpd_init =
        logevent_register("http event", LOG_ON_LOG, LOG_PROXYLOG, LOG_NOTICE, "%s");

    LogSys.httpd =
        logevent_register("http event", LOG_ON_LOG, LOG_HTTPLOG, LOG_NOTICE, "%s : %s");

    LogSys.httpd_request =
	logevent_register("http event" , LOG_ON_LOG, LOG_HTTPLOG, LOG_NOTICE, "Request From %s: %s %s");    

    LogSys.operevent =
	logevent_register("operator command", LOG_ON_LOG, LOG_LOGFILE, LOG_INFO,
                          "OPER COMMAND (%s) by %^C : parameters: %S");
    LogSys.epoll_netio =
	logevent_register("Eventpoll Netio", LOG_ALWAYS, LOG_ERRORLOG | LOG_STDERR, LOG_FATAL, "Unable to start eventpoll");
  
    LogSys.epoll_ctl = 
	logevent_register("Eventpoll Ctl", LOG_ON_LOG, LOG_LOGFILE, LOG_FATAL, "Eventpoll ctl error: %s");
}  
