/*
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
 *
 *   Numerous fixes by Markku Savela
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
 * $Id: s_numeric.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "channel.h"
#include "language.h"
#include "h.h"

static char buffer[1024];

static struct LangHash *lang_hash_table[MAX_LANG_HASH];

static int standardindex = 0;

static char *en_us = "en_us";

/*
 * hash
 *
 * inputs       - char string
 * output       - hash index
 * side effects - NONE
 *
 * BUGS         - This a HORRIBLE hash function
 */
static int hash(char *p)
{
    int hash_val = 0;

    while (*p) {
	hash_val += ((int) (*p) & 0xDF);
	p++;
    }

    return (hash_val % MAX_LANG_HASH);
}

int clear_hash_lang()
{
    memset(lang_hash_table, 0, sizeof(lang_hash_table));
    lang_add_table("standard", "en_us", replies);
    lang_add_table("english", "en_us", replies);
    standardindex = hash("english");
    return 0;
}

void lang_add_table(char *langname, char *helpdir, char **fmtarray)
{
    struct LangHash *ptr;
    struct LangHash *last_ptr = NULL;
    struct LangHash *new_ptr;
    int langindex;

    langindex = hash(langname);

    for (ptr = lang_hash_table[langindex]; ptr; ptr = ptr->next) {
	if (irc_strcmp(langname, ptr->langname) == 0)
	    return;		/* Its already added */
	last_ptr = ptr;
    }

    new_ptr = (struct LangHash *) MyMalloc(sizeof(struct LangHash));

    new_ptr->next = NULL;
    DupString(new_ptr->langname, langname);
    DupString(new_ptr->helpdirname, helpdir);
    new_ptr->fmtarray = fmtarray;

    if (last_ptr == NULL)
	lang_hash_table[langindex] = new_ptr;
    else
	last_ptr->next = new_ptr;
}

void lang_del_table(char *langname)
{
    struct LangHash *ptr;
    struct LangHash *last_ptr = NULL;
    int langindex;

    langindex = hash(langname);

    for (ptr = lang_hash_table[langindex]; ptr; ptr = ptr->next) {
	if (irc_strcmp(langname, ptr->langname) == 0) {
	    MyFree(ptr->langname);
	    MyFree(ptr->helpdirname);
	    if (last_ptr != NULL)
		last_ptr->next = ptr->next;
	    else
		lang_hash_table[langindex] = ptr->next;
	    ptr->fmtarray = NULL;
	    MyFree(ptr);
	    return;
	}
    }
}

int lang_parse(char *givenname)
{
    struct LangHash *ptr;
    int langindex;

    langindex = hash(givenname);
    for (ptr = lang_hash_table[langindex]; ptr; ptr = ptr->next) {
	if (irc_strcmp(givenname, ptr->langname) == 0) {
	    return langindex;
	}
    }
    return 0;
}

char **get_language(int langindex)
{
    struct LangHash *ptr;

    if (langindex) {
	ptr = lang_hash_table[langindex];
	if (ptr)
	    return ptr->fmtarray;
    } else if (me.lang) {
	ptr = lang_hash_table[me.lang];
	if (ptr)
	    return ptr->fmtarray;
    }

    return replies;		/* standard english */
}

int set_language(int langindex)
{

    if (langindex) {
	if (lang_hash_table[langindex])
	    return langindex;
    }
    return standardindex;
}

char *get_langpath(aClient *cptr)
{
    struct LangHash *ptr;
    int langindex;

    langindex = cptr->lang;
    if (langindex) {
	ptr = lang_hash_table[langindex];
	if (ptr)
	    return ptr->helpdirname;
    } else if (me.lang) {
	ptr = lang_hash_table[me.lang];
	if (ptr)
	    return ptr->helpdirname;
    }

    return en_us;
}
    

char *rpl_str(int numeric)
{
    if ((numeric < 0 || numeric > 999) || !replies[numeric])
	return (replies[ERR_NUMERIC_ERR]);
    else
	return (replies[numeric]);
}

/* This hack here searches for the given numeric
 * in the language array. If it is not available, then the default
 * array is used. -TimeMr14C */

char *get_numeric_format_in_lang(int *numeric, char **replylist)
{
    if ((*numeric < 0 || *numeric > 999) || !replylist[*numeric]) {
	if (!replies[*numeric]) {
	    *numeric = ERR_NUMERIC_ERR;
	    return replies[ERR_NUMERIC_ERR] + 11;
	} else
	    return replies[*numeric] + 11;
    } else
	return replylist[*numeric] + 11;
}

void list_languages(aClient *cptr)
{
    int i;
    struct LangHash *ptr;

    send_me_notice(cptr, ":Available languages");
    for (i = 0; i < MAX_LANG_HASH; i++) {
	for (ptr = lang_hash_table[i]; ptr; ptr = ptr->next)
	    send_me_notice(cptr, ":%s", ptr->langname);
    }
    send_me_notice(cptr, ":End of language list");
}

/*
 * * DoNumeric (replacement for the old do_numeric) *
 * 
 *      parc    number of arguments ('sender' counted as one!) *
 * parv[0]      pointer to 'sender' (may point to empty string) (not
 * used) *      parv[1]..parv[parc-1] *         pointers to additional
 * parameters, this is a NULL *         terminated list (parv[parc] ==
 * NULL). *
 * 
 * *WARNING* *  Numerics are mostly error reports. If there is
 * something *  wrong with the message, just *DROP* it! Don't even
 * think of *   sending back a neat error message -- big danger of
 * creating *   a ping pong error message...
 */
int do_numeric(int numeric, aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    aClient *acptr;
    int i;

    if (parc < 1 || (!IsServer(sptr) && !IsService(sptr)))
	return 0;

    /*
     * Remap low number numerics. 
     * WHAT THE HELL DOES THIS ???
     * -TimeMr14C
     * if (numeric < 100)
     *  numeric += 100;
     */

    /*
     * * Prepare the parameter portion of the message into 'buffer'. *
     * (Because the buffer is twice as large as the message buffer * for
     * the socket, no overflow can occur here... ...on current *
     * assumptions--bets are off, if these are changed --msa) * Note: if
     * buffer is non-empty, it will begin with SPACE.
     */

    buffer[0] = '\0';
    if (parc > 1) {
	for (i = 2; i < (parc - 1); i++) {
	    strcat(buffer, " ");
	    strcat(buffer, parv[i]);
	}
	strcat(buffer, " :");
	strcat(buffer, parv[parc - 1]);
    }

    if ((acptr = find_client(parv[1]))) {

	if (IsMe(acptr))
	    return 0;
	else if (acptr->from == cptr)
	    return 0;
	else if (MyClient(acptr) && !IsAnOper(acptr))
	    sendto_one(acptr, ":%*C %N %~C%s", sptr, numeric, acptr, buffer);
	else
	    sendto_one(acptr, ":%C %N %~C%s", sptr, numeric, acptr, buffer);
    }

    return 0;
}
