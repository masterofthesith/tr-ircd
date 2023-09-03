/*
 *   IRC - Internet Relay Chat, src/optparse.c
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

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "s_conf.h"
#include "optparse.h"

static OptItem parseopts[15] = {
    {"dirpath", 	'd', NULL, OPT_STRING, "Path where the configuration files are"},
    {"configfile", 	'f', NULL, OPT_STRING, "Server configuration file"},
    {"root", 		'r', NULL, OPT_ROOT, "Run server as root"},
    {"foreground", 	'z', NULL, OPT_FOREGROUND, "Run in foreground"},
    {"logging", 	'l', NULL, OPT_LOG, "Enable logging"},
    {"debuglevel", 	'x', NULL, OPT_DEBUG, "Debug level"},
    {"maxclients", 	'm', NULL, OPT_MAXCLIENTS, "Set the max number of clients"},
    {"smallnet", 	's', NULL, OPT_SMALLNET, "Run the ircd for a small network (memory savvy)"},
    {"webconfig", 	'w', NULL, OPT_WEBCONFIG, "Run as web-configuration mode"},
    {"webconfig-bind", 	'b', NULL, OPT_WEBCONFIG_BIND, "Web-Configuration Bind-Ip (Required)"},
    {"webconfig-port", 	'p', NULL, OPT_WEBCONFIG_PORT, "Web-Configuration Port (Required)"},
    {"version", 	'v', NULL, OPT_NONE, "Server Version Output"},
    {"help", 		'h', NULL, OPT_USAGE, "Usage information"},
    {NULL, 0, NULL, OPT_USAGE},
};

static int opt_root = 0;
static int wport = 0;
static char wbind[HOSTIPLEN];

void init_optparse(void *unused)
{
    parseopts[0].element = (char *) &(GeneralOpts.dpath);
    parseopts[1].element = (char *) &(GeneralOpts.configfile);
    memset(wbind, 0, sizeof(char) * HOSTIPLEN);
    return;
}

static void usage()
{
int i;

    printf("Usage: ircd [options]\n");
    printf("\n");

    for (i = 0; parseopts[i].optname; i++) {
	printf("    -%c | --%-16s %12s %s\n",
	       parseopts[i].optchar, parseopts[i].optname,
	       ((parseopts[i].opttype & OPT_STRING) ? "parameter" :
		((parseopts[i].opttype & OPT_DEBUG) ? "number" : 
		 ((parseopts[i].opttype & OPT_MAXCLIENTS) ? "number" : 
		  ((parseopts[i].opttype & OPT_WEBCONFIG_BIND) ? "address" :
		   ((parseopts[i].opttype & OPT_WEBCONFIG_PORT) ? "portnumber" : ""))))), parseopts[i].optdesc);
    }
    printf("\n");
    printf("Note that if running in foreground mode, and debug is activated,\n");
    printf("the debug output will be sent to stdout\n");
    printf(" TR-IRCD Development 2000-2003 (c)\n");
    exit(-1);
}

int process_commandline(int argcount, char **argv)
{
    int i;
    int argc = 1;
    char *arg;

    uid_t euid = geteuid();

    while (argc < argcount) {

	for (i = 0; parseopts[i].optname; i++) {
	    if (argc >= argcount)
		return 0;
	    if (argv[argc][0] == '-') {
		if (argv[argc][1] == '-') {
		    arg = argv[argc];
		    arg += 2;

		    if (!irc_strcmp(arg, parseopts[i].optname)) {
			switch (parseopts[i].opttype) {
			    case OPT_DEBUG:
				argc++;
				arg = argv[argc];
				if (!(argc < argcount))
				    usage();
				GeneralOpts.debuglevel = atoi(arg);
				break;
			    case OPT_MAXCLIENTS:
				argc++;
				arg = argv[argc];
				if (!(argc < argcount))
				    usage();
				GeneralOpts.maxclients = atoi(arg);
				break;
			    case OPT_STRING:
				argc++;
				arg = argv[argc];
				if (!(argc < argcount))
				    usage();
				*((char **) parseopts[i].element) = MyMalloc(strlen(arg));
				strcpy(*((char **) parseopts[i].element), arg);
				break;
			    case OPT_FOREGROUND:
				GeneralOpts.foreground = 1;
				break;
			    case OPT_SMALLNET:
				GeneralOpts.smallnet = 1;
				break;
			    case OPT_ROOT:
				opt_root = 1;
				break;
			    case OPT_WEBCONFIG:
				GeneralOpts.webconfig = 1;
				break;
			    case OPT_WEBCONFIG_PORT:
				argc++;
				arg = argv[argc];
				if (!(argc < argcount))
				    usage();
				wport = atoi(arg);
				break;
			    case OPT_WEBCONFIG_BIND:
				argc++;
				arg = argv[argc];
				if (!(argc < argcount))
				    usage();
				strcpy(wbind, arg);
				break;
			    case OPT_LOG:
				GeneralOpts.enable_logging = 1;
				break;
			    case OPT_NONE:
				printf("%s\n", version);
				exit(-1);
				break;
			    case OPT_USAGE:
			    default:
				usage();
				break;
			}
		    }
		} else if (argv[argc][1] == parseopts[i].optchar) {
		    switch (parseopts[i].opttype) {
			case OPT_DEBUG:
			    argc++;
			    arg = argv[argc];
			    if (!(argc < argcount))
				usage();
			    GeneralOpts.debuglevel = atoi(arg);
			    break;
			case OPT_MAXCLIENTS:
			    argc++;
			    arg = argv[argc];
			    if (!(argc < argcount))
				usage();
			    GeneralOpts.maxclients = atoi(arg);
			    break;
			case OPT_STRING:
			    argc++;
			    arg = argv[argc];
			    if (!(argc < argcount))
				usage();
			    *((char **) parseopts[i].element) = MyMalloc(strlen(arg));
			    strcpy(*((char **) parseopts[i].element), arg);
			    break;
			case OPT_FOREGROUND:
			    GeneralOpts.foreground = 1;
			    break;
			case OPT_SMALLNET:
			    GeneralOpts.smallnet = 1;
			    break;
			case OPT_ROOT:
			    opt_root = 1;
			    break;
			case OPT_WEBCONFIG:
			    GeneralOpts.webconfig = 1;
			    break;
			case OPT_WEBCONFIG_PORT:
			    argc++;
			    arg = argv[argc];
			    if (!(argc < argcount))
				usage();
			    wport = atoi(arg);
			    break;
			case OPT_WEBCONFIG_BIND:
			    argc++;
			    arg = argv[argc];
			    if (!(argc < argcount))
				usage();
			    strcpy(wbind, arg);
			    break;
			case OPT_LOG:
			    GeneralOpts.enable_logging = 1;
			    break;
			case OPT_NONE:
			    printf("%s\n", version);
			    exit(-1);
			    break;
			case OPT_USAGE:
			default:
			    usage();
			    break;
		    }
		}
	    }
	}
	argc++;
    }

    if (GeneralOpts.webconfig) {
	if (wport == 0) {
	    fprintf(stderr,
		    "ERROR: You did not specify a port for the http configuration interface.\n");
	    exit(45);
	}
	if (wbind[0] == '\0') {
	    fprintf(stderr,
		    "ERROR: You did not specify the ip for the http configuration interface.\n");
	    exit(46);
	}
	GeneralOpts.webconfigport = wport;
	GeneralOpts.webconfigbind = wbind;
    }
    if ((opt_root == 0) && (euid == 0)) {
	fprintf(stderr, "ERROR: do not run ircd setuid root. Run it as a normal user.\n");
	exit(-1);
    }

    return 0;
}
