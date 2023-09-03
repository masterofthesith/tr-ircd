/************************************************************************
 *   IRC - Internet Relay Chat, src/usermode.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers.
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
 *
 *  $Id: usermodes.c,v 1.8 2004/02/24 19:03:33 tr-ircd Exp $
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "msg.h"
#include "s_conf.h"
#include "usermode.h"

static char buf[BUFSIZE];

char usermodelist[16];		/* currently we do not have more than 15 */

struct UserMode umodetab[] = {
    {0, 0, 0, 0},		/* #0 */
    {0, 0, 0, 0},		/* #1 */
    {0, 0, 0, 0},		/* #2 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #10 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #20 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #30 */
    {0, 0, 0, 0},		/* #31 */
    {0, 0, 0, 0},		/* #32 */
    {0, 0, 0, 0},		/* ! */
    {0, 0, 0, 0},		/* " */
    {0, 0, 0, 0},		/* # */
    {0, 0, 0, 0},		/* $ */
    {0, 0, 0, 0},		/* % */
    {0, 0, 0, 0},		/* & */
    {0, 0, 0, 0},		/* ' */
    {0, 0, 0, 0},		/* ( */
    {0, 0, 0, 0},		/* ) */
    {0, 0, 0, 0},		/* * */
    {0, 0, 0, 0},		/* + */
    {0, 0, 0, 0},		/* , */
    {0, 0, 0, 0},		/* - */
    {0, 0, 0, 0},		/* . */
    {0, 0, 0, 0},		/* / */
    {0, 0, 0, 0},		/* 0 */
    {0, 0, 0, 0},		/* 1 */
    {0, 0, 0, 0},		/* 2 */
    {0, 0, 0, 0},		/* 3 */
    {0, 0, 0, 0},		/* 4 */
    {0, 0, 0, 0},		/* 5 */
    {0, 0, 0, 0},		/* 6 */
    {0, 0, 0, 0},		/* 7 */
    {0, 0, 0, 0},		/* 8 */
    {0, 0, 0, 0},		/* 9 */
    {0, 0, 0, 0},		/* : */
    {0, 0, 0, 0},		/* ; */
    {0, 0, 0, 0},		/* < */
    {0, 0, 0, 0},		/* = */
    {0, 0, 0, 0},		/* > */
    {0, 0, 0, 0},		/* ? */
    {0, 0, 0, 0},		/* @ */
    {UMODE_A, 1, UMODE_o, 0},	/* A *//* 65 */
    {0, 0, 0, 0},		/* B */
    {UMODE_C, 1, 0, 0},		/* C */
    {0, 0, 0, 0},		/* D */
    {0, 0, 0, 0},		/* E */
    {0, 0, 0, 0},		/* F */
    {0, 0, 0, 0},		/* G */
    {UMODE_H, 1, UMODE_o, 0},	/* H */
    {0, 0, 0, 0},		/* I */
    {0, 0, 0, 0},		/* J */
    {0, 0, 0, 0},		/* K */
    {0, 0, 0, 0},		/* L */
    {0, 0, 0, 0},		/* M */
    {0, 0, 0, 0},		/* N */
    {0, 0, 0, 0},		/* O */
    {UMODE_P, 1, 0, 0},		/* P */
    {0, 0, 0, 0},		/* Q */
    {UMODE_R, 1, 0, 0},		/* R */
    {0, 0, 0, 0},		/* S */
    {0, 0, 0, 0},		/* T */
    {0, 0, 0, 0},		/* U */
    {0, 0, 0, 0},		/* V */
    {0, 0, 0, 0},		/* W */
    {0, 0, 0, 0},		/* X */
    {0, 0, 0, 0},		/* Y */
    {0, 0, 0, 0},		/* Z *//* 90 */
    {0, 0, 0, 0},		/* [ */
    {0, 0, 0, 0},		/* \ */
    {0, 0, 0, 0},		/* ] */
    {0, 0, 0, 0},		/* ^ */
    {0, 0, 0, 0},		/* _ */
    {0, 0, 0, 0},		/* ` */
    {UMODE_a, 1, UMODE_o, UMODE_A},	/* a *//* 97 */
    {0, 0, 0, 0},		/* b */
    {UMODE_c, 1, 0, 0},		/* c */
    {0, 0, 0, 0},		/* d */
    {0, 0, 0, 0},		/* e */
    {0, 0, 0, 0},		/* f */
    {0, 0, 0, 0},		/* g */
    {UMODE_h, 1, 0, 0},		/* h */
    {UMODE_i, 1, 0, 0},		/* i */
    {0, 0, 0, 0},		/* j */
    {0, 0, 0, 0},		/* k */
    {0, 0, 0, 0},		/* l */
    {0, 0, 0, 0},		/* m */
    {UMODE_n, 1, UMODE_o, 0},	/* n */
    {UMODE_o, 1, UMODE_o, UMODE_a | UMODE_A | UMODE_n | UMODE_H},	/* o */
    {UMODE_p, 1, 0, 0},		/* p */
    {0, 0, 0, 0},		/* q */
    {UMODE_r, 1, 0, 0},		/* r */
    {0, 0, 0, 0},		/* s */
    {UMODE_t, 0, 0, 0},		/* t */	
    {0, 0, 0, 0},		/* u */
    {0, 0, 0, 0},		/* v */
    {UMODE_w, 1, 0, 0},		/* w */
    {UMODE_x, 1, 0, 0},		/* x */
    {0, 0, 0, 0},		/* y */
    {UMODE_z, 1, 0, 0},		/* z *//* 122 */
    {0, 0, 0, 0},		/* { */
    {0, 0, 0, 0},		/* | */
    {0, 0, 0, 0},		/* } */
    {0, 0, 0, 0},		/* ~ */
    {0, 0, 0, 0},		/* #127 */
    {0, 0, 0, 0},		/* #128 */
    {0, 0, 0, 0},		/* #129 */
    {0, 0, 0, 0},		/* #130 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #140 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #150 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #160 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #170 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #180 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #190 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #200 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #210 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #220 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #230 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #240 */
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},		/* #250 */
    {0, 0, 0, 0},		/* #251 */
    {0, 0, 0, 0},		/* #252 */
    {0, 0, 0, 0},		/* #253 */
    {0, 0, 0, 0},		/* #254 */
    {0, 0, 0, 0},		/* #255 */
};

void send_umode(aClient *cptr, aClient *sptr, int old, int sendmask, char *umode_buf)
{

    int i;
    long flag;
    char *m;
    int what = 0;

    /*
     * build a string in umode_buf to represent the change in the user's
     * mode between the new (sptr->flag) and 'old'.
     */

    m = umode_buf;
    *m = '\0';

    for (i = 64; i < 127; i++) {
	if (!umodetab[i].in_use)
	    continue;
	flag = umodetab[i].type;
	if (MyClient(sptr) && !(flag & sendmask))
	    continue;
	if ((flag & old) && !(sptr->umode & flag)) {
	    if (what == CMODE_DEL)
		*m++ = i;
	    else {
		what = CMODE_DEL;
		*m++ = '-';
		*m++ = i;
	    }
	} else if (!(flag & old) && (sptr->umode & flag)) {
	    if (what == CMODE_ADD)
		*m++ = i;
	    else {
		what = CMODE_ADD;
		*m++ = '+';
		*m++ = i;
	    }
	}
    }
    *m = '\0';
    if (*umode_buf && cptr)
	sendto_one_person(cptr, sptr, TOK1_MODE, "%~C :%s", sptr, umode_buf);
}

    /*
     * added Sat Jul 25 07:30:42 EST 1992
     * extra argument evenTS added to send to TS servers or not -orabidoo
     * extra argument evenTS no longer needed with TS only th+hybrid server
     * -Dianora
     */

void send_umode_out(aClient *cptr, aClient *sptr, int old)
{
    aClient *acptr;
    dlink_node *ptr;
    send_umode(NULL, sptr, old, SEND_UMODES, buf);
    for (ptr = serv_list.head; ptr; ptr = ptr->next) {
	acptr = ptr->data;
	if (!acptr)
	    continue;
	if ((acptr != cptr) && (acptr != sptr) && (*buf))
	    sendto_one_server(acptr, sptr, TOK1_MODE, "%~C :%s", sptr, buf);
    }
    sendto_service(SERVICE_SEE_UMODES, 0, sptr, NULL, TOK1_MODE, "%s :%s", sptr->name, buf);
    if (cptr && MyClient(cptr))
	send_umode(cptr, sptr, old, USER_UMODES, buf);
}

int find_umode(int mode, unsigned char *modelist)
{
    int i = 0;
    for (i = 0; i < 16; i++) {
        if (modelist[i] == mode)
            return i;
    }
    return -1;
}

void create_usermodelist(void)
{
    int j = 0;
    int i = 0;
    int mode_res = 0;
    int uml_counter = 0;

    unsigned char *tmpusermodelist;
    tmpusermodelist = (unsigned char *) malloc(16);
    memset(tmpusermodelist, 0, 16);

    /* Trying to sort the user mode list in the form aAbBcCdD...etc. 
     * -dsginosa
     */

    for (i = 32; i < 127; i++) {
	if (umodetab[i].in_use) {

	    tmpusermodelist[j] = i;
	    j++;
	}

    }

    tmpusermodelist[j] = '\0';

    for (i = 65; i <= 90; i++) {
	mode_res = find_umode(i + 32, tmpusermodelist);
	if (mode_res != -1) {
	    usermodelist[uml_counter] = tmpusermodelist[mode_res];
	    uml_counter++;
	    tmpusermodelist[mode_res] = 255;
	}
	mode_res = find_umode(i, tmpusermodelist);
	if (mode_res != -1) {
	    usermodelist[uml_counter] = tmpusermodelist[mode_res];
	    uml_counter++;
	    tmpusermodelist[mode_res] = 255;
	}

    }

    for (i = 0; i < strlen((char *) tmpusermodelist); i++) {
	if (tmpusermodelist[i] == 255) {
	    continue;
	} else {
	    usermodelist[uml_counter] = tmpusermodelist[i];
	    uml_counter++;
	}

    }
    MyFree(tmpusermodelist);

    GeneralOpts.umodelist = (char *) usermodelist;
}

