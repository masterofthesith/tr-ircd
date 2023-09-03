/************************************************************************
 *   IRC - Internet Relay Chat, src/textedit.c
 *   Copyright (C)2000-2003 TR-IRCD Development
 *   Copyright (C) 1990, 1991 Armin Gruner
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
 * $Id: textedit.c,v 1.5 2004/02/24 15:00:30 tr-ircd Exp $ 
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "tools.h"
#include "s_conf.h"
#include "chanmode.h"

/*
 * * canonize * 
 *
 * reduce a string of duplicate list entries to contain only the unique *
 * items.  Unavoidably O(n^2).
 */

char *canonize(char *buffer)
{
    static char cbuf[BUFSIZ];
    char *s, *t, *cp = cbuf;
    int l = 0;
    char *p = NULL, *p2;

    *cp = '\0';

    for (s = strtoken(&p, buffer, ","); s; s = strtoken(&p, NULL, ",")) {
	if (l) {
	    for (p2 = NULL, t = strtoken(&p2, cbuf, ","); t; t = strtoken(&p2, NULL, ","))
		if (!irc_strcmp(s, t))
		    break;
		else if (p2)
		    p2[-1] = ',';
	} else
	    t = NULL;

	if (!t) {
	    if (l)
		*(cp - 1) = ',';
	    else
		l = 1;
	    strcpy(cp, s);
	    if (p)
		cp += (p - s);
	} else if (p2)
	    p2[-1] = ',';
    }
    return cbuf;
}

/*
 * Fixes a string so that the first white space found becomes an end of
 * string marker (`\-`).  returns the 'fixed' string or "*" if the
 * string was NULL length or a NULL pointer.
 */
static char *check_string(char *s)
{
    static char star[2] = "*";
    char *str = s;

    if (BadPtr(s))
	return star;

    for (; *s; s++)
	if (IsSpace(*s)) {
	    *s = '\0';
	    break;
	}

    return (BadPtr(str)) ? star : str;
}

/*
 * create a string of form "foo!bar@fubar" given foo, bar and fubar as
 * the parameters.  If NULL, they become "*".
 */
char *make_nick_user_host(char *nick, char *name, char *host)
{
    static char namebuf[NICKLEN + USERLEN + HOSTLEN + 6];
    int n;
    char *ptr1, *ptr2;

    ptr1 = namebuf;
    for (ptr2 = check_string(nick), n = NICKLEN; *ptr2 && n--;)
	*ptr1++ = *ptr2++;
    *ptr1++ = '!';
    for (ptr2 = check_string(name), n = USERLEN; *ptr2 && n--;)
	*ptr1++ = *ptr2++;
    *ptr1++ = '@';
    for (ptr2 = check_string(host), n = HOSTLEN; *ptr2 && n--;)
	*ptr1++ = *ptr2++;
    *ptr1 = '\0';
    return (namebuf);
}

char *pretty_mask(char *mask)
{
    char *cp, *user, *host;

    if ((user = strchr((cp = mask), '!')))
	*user++ = '\0';
    if ((host = strrchr(user ? user : cp, '@'))) {
	*host++ = '\0';
	if (!user)
	    return make_nick_user_host(NULL, cp, host);
    } else if (!user && strchr(cp, '.'))
	return make_nick_user_host(NULL, NULL, cp);
    return make_nick_user_host(cp, user, host);
}

/*
 * This function checks to see if a CTCP message (other than ACTION) is
 * contained in the passed string.  This might seem easier than I am   
 * doing it, but a CTCP message can be changed together, even after a  
 * normal message.
 * 
 * If the message is found, and it's a DCC message, pass it back in
 * *dccptr.
 * 
 * Unfortunately, this makes for a bit of extra processing in the
 * server.
 * Bahamut-Coder (?)
 */

int check_for_ctcp(char *str, char **dccptr, int checkonly)
{
    char *p = str;

    if (checkonly && p && strchr(p, 1))
	return CTCP_YES;

    while ((p = strchr(p, 1)) != NULL) {
	if (irc_strncmp(++p, "DCC", 3) == 0) {
	    if (dccptr)
		*dccptr = p;
	    if (irc_strncmp(p + 3, " SEND", 5) == 0)
		return CTCP_DCCSEND;
	    else
		return CTCP_DCC;
	} else {
	    if (!(*(++p)))
	    	break;
	    return CTCP_YES;
	}
    }
    return CTCP_NONE;
}

/*
 * check to see if the message has any color chars in it.
 */
int msg_has_colors(char *msg)
{
    char *c = msg;

    while (*c) {
	if (*c == '\003' || *c == '\033')
	    break;
	else
	    c++;
    }

    if (*c)
	return 1;

    return 0;
}

int check_forbidden_words(char *text)
{

    if ((!match("*/server*", text)) || (!match("*irc.*", text)) ||
	(!match("*script*", text)) || (strchr(text, '#')) || (!match("*www.*", text))) {

	return 1;
    }
    return 0;

}

char *stealth_server(char *name, char *out)
{
    char *tmp = NULL;
    char arr[HOSTLEN];

    if (!name) {
	strcpy(out, "no-server");
    } else {
    	tmp = strdup(name);
        tmp = strpbrk(tmp, ".");
        ircsnprintf(arr, sizeof(arr), "*%s", tmp);
    	strcpy(out, arr);
    }
    return out;
}

/*
 * strip_tabs(dst, src, length)
 *
 *   Copies src to dst, while converting all \t (tabs) into spaces.
 *
 * NOTE: jdc: I have a gut feeling there's a faster way to do this.
 */
char *strip_tabs(char *dest, const unsigned char *src, size_t len)
{
    char *d = dest;
    /* Sanity check; we don't want anything nasty... */
    assert(0 != dest);
    assert(0 != src);

    while (*src && (len > 0)) {
	if (*src == '\t') {
	    *d++ = ' ';		/* Translate the tab into a space */
	} else {
	    *d++ = *src;	/* Copy src to dst */
	}
	++src;
	--len;
    }
    *d = '\0';			/* Null terminate, thanks and goodbye */
    return dest;
}

