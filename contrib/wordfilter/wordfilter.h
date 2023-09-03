/************************************************************************
 *   IRC - Internet Relay Chat, wordfilter.h
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

/* Ircd Includes */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "h.h"
#include "tools.h"
#include "hook.h"
#include "s_conf.h"


/* Defines */

#define MSG_DENYTEXT    "DENYTEXT"
#define MSG_UNDENYTEXT  "UNDENYTEXT"
#define TOK1_DENYTEXT   "_"
#define TOK1_UNDENYTEXT "`"
#define ERR_DENYTEXT    560
#define RPL_DENYRES     561
#define RPL_DENYLIST    562
#define DT_STATCHAR     "w"
#define DT_STATCHAR_C   "W"

#define BADWORD_DONTADD	0
#define BADWORD_ADD	1

/* Internal structures */

typedef struct badword_ BadWord;

struct badword_ {
    char *string;       /* The string to deny */
    char *reason;       /* The reason for this deny */
};

extern dlink_list bwlist;

/* External functions */

extern dlink_node *find_badword_entry(char *entry);
extern BadWord *new_badword_entry(char *text, char *reason, int add);
extern BadWord *matching_badwords(char *text);

extern int remove_badword_entry(char *entry);

extern void list_badwords(aClient *cptr);
extern void read_definitions(void);

/* Irc Message functions */

extern int o_denytext(aClient *cptr, aClient *sptr, int parc, char **parv);       
extern int m_denytext(aClient *cptr, aClient *sptr, int parc, char **parv);       

extern int o_undenytext(aClient *cptr, aClient *sptr, int parc, char **parv);       
extern int m_undenytext(aClient *cptr, aClient *sptr, int parc, char **parv);      

/* Hook functions */

extern int do_message_more(struct hook_data *data);       
extern int do_stats_w(struct hook_data *data);

/* Functions of the channelmode +w; no-filter */

extern void nofilter_modinit(void);
extern void nofilter_moddeinit(void);
