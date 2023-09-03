/************************************************************************
 *   IRC - Internet Relay Chat, server/s_boot.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
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

/*
 * $Id: s_boot.c,v 1.5 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "s_conf.h"
#include "comply.h"
#include "throttle.h"
#include "usermode.h"
#include "interproc.h"

extern char **myargv;

/*    
 * rehash
 *
 * Actual REHASH service routine. Called with sig == 0 if it has been
 * called as a result of an operator issuing this command, else assume
 * it has been called as a result of the server receiving a HUP signal.  
 */
int rehash(aClient *cptr, aClient *sptr, int sig)
{
    if (sig == SIGHUP) 
        sendto_ops("Got signal SIGHUP, reloading ircd config file");

    rehash_maskitems(MASKITEM_AUTOKILL);
    rehash_maskitems(MASKITEM_EXCLUDE);
    rehash_maskitems(MASKITEM_QUARANTINE);
    rehash_maskitems(MASKITEM_GECOS);
    rehash_maskitems(MASKITEM_ZAPLINE);
    rehash_maskitems(MASKITEM_JUPITER);
    rehash_maskitems(MASKITEM_KLINE);

    throttle_rehash();

    read_message_file(&(GeneralOpts.motd));
    read_message_file(&(GeneralOpts.shortmotd));
    read_message_file(&(GeneralOpts.linksfile));

    restart_resolver();

    read_conf_files(0);

    protocol_rehash();

    flush_deleted_I_P(); 
    GeneralOpts.split = 0;
    create_usermodelist();	

    reinit_log_files();
    
    rehashed = 1;         
    return 0;          
}

void restart(char *mesg)
{
    logevent_call(LogSys.restartmsg, mesg);
    server_reboot();        
}

void server_reboot()
{                  
int i;

    sendto_ops("Aieeeee!!!  Restarting server...");
    logevent_call(LogSys.restart);
    flush_connections(NULL);
    terminate_interproc();

    terminate_loader();

    /*
     * XXX we used to call flush_connections() here. But since this routine
     * doesn't exist anymore, we won't be flushing. This is ok, since
     * when close handlers come into existance, comm_close() will be called
     * below, and the data flushing will be implicit.
     *    -- adrian
     *
     * bah, for now, the program ain't coming back to here, so forcibly
     * close everything the "wrong" way for now, and just LEAVE...
     */
    for (i = 0; i < MAXCONNECTIONS; ++i)
        close(i);

    execv(IRCD_PREFIX "/bin/ircd", myargv);
    
    logevent_call(LogSys.execv, myargv[0]);
    exit(-1);
}
