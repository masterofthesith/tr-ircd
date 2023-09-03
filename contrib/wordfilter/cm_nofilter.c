/************************************************************************                  
 *   IRC - Internet Relay Chat, contrib/wordfilter/cm_nofilter.c
 *
 *   Copyright (C) 2002-2003 TR-IRCD Development
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

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "channel.h"
#include "h.h"
#include "hook.h"
#include "s_conf.h"
#define CMODE_MODULAR 1
#include "chanmode.h"
#undef CMODE_MODULAR
#include "wordfilter.h"

static int set_single_mode(int adl, aChannel *chptr, int nmodes,
			   int *mbix, char *mbuf, int flag, char letter)
{
    int fmbix = *mbix;

    if (adl == CMODE_ADD)
	chptr->mode.mode |= flag;
    if (adl == CMODE_DEL)
	chptr->mode.mode &= ~flag;

    mbuf[fmbix++] = letter;

    *mbix = fmbix;

    return nmodes;
}

static int ssm_service(int adl, aChannel *chptr, int nmodes, int *mbix,
		       int flag, char letter, aClient *sptr, char *mbuf)
{
    nmodes++;
    if (MyClient(sptr))
	send_me_numeric(sptr, ERR_CHANOPRIVSNEEDED, chptr);
    else
	ircstp->is_fake++;
    return nmodes;
}

static int ssm_server(int adl, aChannel *chptr, int nmodes, int *mbix, int flag,
		      char letter, aClient *sptr, char *mbuf)
{
    nmodes++;
    return set_single_mode(adl, chptr, nmodes, mbix, mbuf, flag, letter);
}

static int ssm_uline(int adl, aChannel *chptr, int nmodes, int *mbix, int flag,
		     char letter, aClient *sptr, char *mbuf)
{
    nmodes++;
    return set_single_mode(adl, chptr, nmodes, mbix, mbuf, flag, letter);
}

static int ssm_oper(int adl, aChannel *chptr, int nmodes, int *mbix, int flag,
		    char letter, aClient *sptr, char *mbuf)
{
    nmodes++;
    if (IsChanUser(sptr, chptr, CHFL_CHANOP) || IsOperMode(sptr) || IsServer(sptr->from)) {

	return set_single_mode(adl, chptr, nmodes, mbix, mbuf, flag, letter);
    } else
	send_me_numeric(sptr, ERR_CHANOPRIVSNEEDED, chptr);
    return nmodes;
}

static int ssm_user(int adl, aChannel *chptr, int nmodes, int *mbix, int flag,
		    char letter, aClient *sptr, char *mbuf)
{
    nmodes++;
    if (IsChanUser(sptr, chptr, CHFL_CHANOP))
	return set_single_mode(adl, chptr, nmodes, mbix, mbuf, flag, letter);
    else
	send_me_numeric(sptr, ERR_CHANOPRIVSNEEDED, chptr);
    return nmodes;
}

static int ssm_service_nofilter(int adl, aChannel *chptr, int nmodes,
				int *argnum, int *pidx, int *mbix, char *mbuf,
				char *pbuf, aClient *cptr, aClient *sptr, int parc, char **parv)
{
    return ssm_service(adl, chptr, nmodes, mbix, MODE_NOFILTER, 'w', sptr, mbuf);
}

static int ssm_server_nofilter(int adl, aChannel *chptr, int nmodes,
			       int *argnum, int *pidx, int *mbix, char *mbuf,
			       char *pbuf, aClient *cptr, aClient *sptr, int parc, char **parv)
{
    return ssm_server(adl, chptr, nmodes, mbix, MODE_NOFILTER, 'w', sptr, mbuf);
}

static int ssm_uline_nofilter(int adl, aChannel *chptr, int nmodes,
			      int *argnum, int *pidx, int *mbix, char *mbuf,
			      char *pbuf, aClient *cptr, aClient *sptr, int parc, char **parv)
{
    return ssm_uline(adl, chptr, nmodes, mbix, MODE_NOFILTER, 'w', sptr, mbuf);
}

static int ssm_oper_nofilter(int adl, aChannel *chptr, int nmodes,
			     int *argnum, int *pidx, int *mbix, char *mbuf,
			     char *pbuf, aClient *cptr, aClient *sptr, int parc, char **parv)
{
    return ssm_oper(adl, chptr, nmodes, mbix, MODE_NOFILTER, 'w', sptr, mbuf);
}

static int ssm_nofilter(int adl, aChannel *chptr, int nmodes,
			int *argnum, int *pidx, int *mbix, char *mbuf,
			char *pbuf, aClient *cptr, aClient *sptr, int parc, char **parv)
{
    return ssm_user(adl, chptr, nmodes, mbix, MODE_NOFILTER, 'w', sptr, mbuf);
}

static struct ChanMode mode_nofilter[] = {
    {MODE_NOFILTER, 1, 0, CMTYPE_SINGLE, 
     ssm_nofilter, ssm_oper_nofilter, ssm_uline_nofilter,
     ssm_server_nofilter, ssm_service_nofilter}
};

void nofilter_modinit(void)
{
    modetab[(int) 'w'] = mode_nofilter[0];
    GeneralOpts.lists_created = 0;
}

void nofilter_moddeinit(void)
{
    modetab[(int) 'w'].type = 0;
    modetab[(int) 'w'].in_use = 0;
    modetab[(int) 'w'].flags = 0;
    modetab[(int) 'w'].cmtype = 0;
    modetab[(int) 'w'].cmode_handler_user = NULL;
    modetab[(int) 'w'].cmode_handler_oper = NULL;
    modetab[(int) 'w'].cmode_handler_ulined = NULL;
    modetab[(int) 'w'].cmode_handler_server = NULL;
    modetab[(int) 'w'].cmode_handler_service = NULL;
    GeneralOpts.lists_created = 0;
}

/* Please be warned that this setting will not remove the +w mode from the channels
 * where it is already set, but make it invisible next time /mode is used and visible
 * again, if the module is reloaded -TimeMr14C */
