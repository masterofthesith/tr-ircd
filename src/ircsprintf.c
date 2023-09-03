/************************************************************************
 *   IRC - Internet Relay Chat, src/ircsprintf.c
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
#include "ircsprintf.h"
#include "s_conf.h"
#include "s_user.h"
#include "channel.h"

#define PF_STRING	0x0001	/* %s */
#define PF_CHAR		0x0002	/* %c */
#define PF_DECIMAL	0x0004	/* %d %i %l */

#define PF_NUMERIC	0x0010	/* %N */
#define PF_CHANNEL	0x0020	/* %H */
#define PF_CONFITEM	0x0040	/* %K */
#define PF_CLIENT	0x0080	/* %C */

#define PF_OCTAL	0x0100	/* %o */
#define PF_HEX		0x0200	/* %n %x %X */
#define PF_TIMESTAMP	0x0400	/* %T */
#define PF_MASKITEM	0x0800	/* %M */
#define PF_CHARPP	0x1000	/* %S */

#define PFF_UNSIGNED	0x001	/* %u. */

#define PFF_CLI_FULL_P	0x800	/* nick (user@host) : %^C */
#define PFF_CLI_NAME	0x400	/* nick %~C */
#define PFF_CLI_ID	0x200	/* !XXXXX : %!C */
#define PFF_CLI_FAKE	0x100	/* *.sub.domain %*C */
#define PFF_NOZERO	0x080	/* no 0x in typing hex values */

char num[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char otoa_tab[8] = { '0', '1', '2', '3', '4', '5', '6', '7' };
char itoa_tab[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
char xtoa_tab[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f'
};

void iprintf_init(void *unused)
{
    return;
}

static inline int Iprintf_string(char *str, int pos, size_t size, char *s)
{
    char *buf = str;
    int len = pos;

    if (!s)
	s = "<NULL>";		/* Is this clever -TimeMr14C */

    while (*s && (size ? len < size : 1))
	buf[len++] = *s++;

    return len;
}

static inline int Iprintf_string_array(char *str, int pos, size_t size, char ***s_tmp, int count)
{
    char *buf = str;
    int i, j;
    int len = pos;
    char **s = *s_tmp;

    if (count > 0) {
        for (i = 1; i < count; i++) {
            j = 0; 
            while (s[i][j] && (size ? len < size : 1))
                buf[len++] = s[i][j++];
            buf[len++] = ' ';
        }
    } else {
        buf[len++] = ' ';
    }

    return len;
}

static inline int Iprintf_decimal(char *str, int pos, size_t size, unsigned long ul, int flag)
{
    char *buf = str;
    int len = pos;
    char *s;

    if (!(flag & PFF_UNSIGNED)) {
	if (ul & 0x80000000) {
	    buf[len++] = '-';
	    ul = 0x80000000 - (ul & ~0x80000000);
	}
    }

    s = &num[11];
    do {
	*--s = itoa_tab[ul % 10];
	ul /= 10;
    } while (ul != 0);

    while (*s && (size ? len < size : 1))
	buf[len++] = *s++;

    return len;
}

static inline int Iprintf_timestamp(char *str, int pos, size_t size, void *avoid)
{
    /*
     * This function relies on the fact that the first element of the given structure
     * is always an (unsigned) long value, like the tsval from Client/Channel
     * structures. If you do change those, %T will not work. 
     * -TimeMr14C 20.05.2002
     */
    return Iprintf_decimal(str, pos, size, *(unsigned long *) avoid, PFF_UNSIGNED);
}

static inline int Iprintf_numeric(char *str, int pos, size_t size, unsigned long N)
{
    char *buf = str;
    int len = pos;
    char *s;

    s = &num[11];
    *--s = itoa_tab[N % 10];
    N /= 10;
    *--s = itoa_tab[N % 10];
    N /= 10;
    *--s = itoa_tab[N % 10];
    N /= 10;

    while (*s && (size ? len < size : 1))
	buf[len++] = *s++;

    return len;
}

static inline int Iprintf_hex(char *str, int pos, size_t size, long l, int flag)
{
    char *buf = str;
    int len = pos;
    char *s;

    if (!(flag & PFF_NOZERO)) {
	buf[len++] = '0';
	buf[len++] = 'x';
    }
    s = &num[11];
    do {
	*--s = xtoa_tab[l % 16];
	l /= 16;
    } while (l != 0);

    while (*s && (size ? len < size : 1))
	buf[len++] = *s++;

    return len;
}

static inline int Iprintf_octal(char *str, int pos, size_t size, long l)
{
    char *buf = str;
    int len = pos;
    char *s;

    buf[len++] = '0';
    buf[len++] = 'x';
    s = &num[11];
    do {
	*--s = otoa_tab[l % 8];
	l /= 8;
    } while (l != 0);

    while (*s && (size ? len < size : 1))
	buf[len++] = *s++;

    return len;
}

static inline int Iprintf_char(char *str, int pos, size_t size, char c)
{
    char *buf = str;
    int len = pos;

    buf[len++] = c;

    return len;
}

static inline int Iprintf_client(char *str, int pos, size_t size, aClient *acptr, int flag)
{
    char *buf = str;
    int len = pos;
    char *s;
    char out[HOSTLEN];

    if (IsServer(acptr) || IsMe(acptr)) {
	if (!flag || (flag & PFF_CLI_FULL_P) || (flag & PFF_CLI_NAME)) {
	    s = acptr->name;
	    while (*s && (size ? len < size : 1))
		buf[len++] = *s++;
	} else if (flag & PFF_CLI_ID) {
	    if (MyClient(acptr)) {
		s = acptr->name;
	    } else if (!HasID(acptr)) {
		s = acptr->name;
	    } else if (HasID(acptr)) {
		s = acptr->id.string;
		buf[len++] = '!';
	    }
	    while (*s && (size ? len < size : 1))
		buf[len++] = *s++;
	} else if (flag & PFF_CLI_FAKE) {
	    s = acptr->name;
	    if (ServerHide.enable) {
		if (WillHideName(acptr))
		    s = stealth_server(acptr->name, out);
	    }
	    while (*s && (size ? len < size : 1))
		buf[len++] = *s++;
	}
    } else if (acptr->user) {
	if (!flag || (flag & PFF_CLI_FAKE)) {
	    s = acptr->name;
	    while (*s && (size ? len < size : 1))
		buf[len++] = *s++;
	    buf[len++] = '!';
	    s = acptr->user->username;
	    while (*s && (size ? len < size : 1))
		buf[len++] = *s++;
	    buf[len++] = '@';
	    s = (IsFake(acptr) ? acptr->user->fakehost : acptr->user->host);
	    while (*s && (size ? len < size : 1))
		buf[len++] = *s++;
	} else if (flag & PFF_CLI_FULL_P) {
	    s = acptr->name;
	    while (*s && (size ? len < size : 1))
		buf[len++] = *s++;
	    buf[len++] = ' ';
	    buf[len++] = '(';
	    s = acptr->user->username;
	    while (*s && (size ? len < size : 1))
		buf[len++] = *s++;
	    buf[len++] = '@';
	    s = acptr->user->host;
	    while (*s && (size ? len < size : 1))
		buf[len++] = *s++;
	    buf[len++] = ')';
	} else if (flag & PFF_CLI_NAME) {
	    s = acptr->name;
	    while (*s && (size ? len < size : 1))
		buf[len++] = *s++;
	} else if (flag & PFF_CLI_ID) {
	    if (MyClient(acptr)) {
		s = acptr->name;
	    } else if (!HasID(acptr)) {
		s = acptr->name;
	    } else if (HasID(acptr)) {
		s = acptr->id.string;
		buf[len++] = '!';
	    }
	    while (*s && (size ? len < size : 1))
		buf[len++] = *s++;
	}
    } else {
	s = acptr->name;
	while (*s && (size ? len < size : 1))
	    buf[len++] = *s++;
    }
    return len;
}

static inline int Iprintf_channel(char *str, int pos, size_t size, aChannel *chptr)
{
    char *buf = str;
    int len = pos;
    char *s;
    s = chptr->chname;
    while (*s && (size ? len < size : 1))
	buf[len++] = *s++;
    return len;
}

static inline int Iprintf_confitem(char *str, int pos, size_t size, aConfItem *aconf)
{
    char *buf = str;
    int len = pos;
    char *s;
    s = aconf->name ? aconf->name : "(NULL)";
    while (*s && (size ? len < size : 1))
	buf[len++] = *s++;
    return len;
}

static inline int Iprintf_maskitem(char *str, int pos, size_t size, aMaskItem * ami)
{
    char *buf = str;
    int len = pos;
    char *s;
    if (ami->username && ami->username[0]) {
	s = ami->username;
    	while (*s && (size ? len < size : 1))
            buf[len++] = *s++;
    	buf[len++] = '@';
    }
    if (ami->string) {
	s = ami->string;
    	while (*s && (size ? len < size : 1))
	    buf[len++] = *s++;
    }
    return len;
}

static inline int Iprintf(char *str, size_t size, const char *pattern, va_list vl)
{
    int printf_type = 0;
    int printf_flag = 0;
    const char *format = pattern;
    char *buf = str;
    int len = 0;
    va_list ap = vl;

    while (*format) {
	switch (*format) {
	    case '%':
		format++;
		while (printf_type == 0) {
		    switch (*format) {
			case 's':
			    printf_type = PF_STRING;
			    format++;
			    break;
			case 'u':
			    printf_flag = PFF_UNSIGNED;
			    printf_type = PF_DECIMAL;
			    format++;
			    break;
			case 'l':
			case 'd':
			case 'i':
			    printf_type = PF_DECIMAL;
			    printf_flag = 0;
			    format++;
			    if (*format == 'u') {
				printf_flag = PFF_UNSIGNED;
				format++;
			    } else if (*format == 'd') {
				format++;
			    }
			    break;
			case 'N':
			    printf_type = PF_NUMERIC;
			    format++;
			    break;
			case 'n':
			    printf_flag = PFF_NOZERO;
			case 'x':
			case 'X':
			    printf_type = PF_HEX;
			    format++;
			    break;
			case 'o':
			    printf_type = PF_OCTAL;
			    format++;
			    break;
			case 'c':
			    printf_type = PF_CHAR;
			    format++;
			    break;
			case '^':
			    printf_flag = PFF_CLI_FULL_P;
			    format++;
			    break;
			case '~':
			    printf_flag = PFF_CLI_NAME;
			    format++;
			    break;
			case '!':
			    printf_flag = PFF_CLI_ID;
			    format++;
			    break;
			case '*':
			    printf_flag = (ServerHide.enable ? PFF_CLI_FAKE : PFF_CLI_NAME);
			    format++;
			    break;
			case 'C':
			    printf_type = PF_CLIENT;
			    format++;
			    break;
			case 'H':
			    printf_type = PF_CHANNEL;
			    format++;
			    break;
			case 'K':
			    printf_type = PF_CONFITEM;
			    format++;
			    break;
			case 'M':
			    printf_type = PF_MASKITEM;
			    format++;
			    break;
			case 'T':
			    printf_type = PF_TIMESTAMP;
			    format++;
			    break;
			case 'S':
			    printf_type = PF_CHARPP;
			    format++;
			    break;
			default:
#ifdef HAVE_VSNPRINTF
			    if (size)
				return vsnprintf(str, size, pattern, vl);
			    else
				return vsprintf(str, pattern, vl);
#else
			    return vsprintf(str, pattern, vl);
#endif
			    break;
		    }
		}

		switch (printf_type) {
		    case PF_STRING:
    			len = Iprintf_string(buf, len, size, va_arg(ap, char *));
			break;
		    case PF_DECIMAL:
    			len = Iprintf_decimal(buf, len, size, va_arg(ap, unsigned long), printf_flag);
			break;
		    case PF_CLIENT:
    			len = Iprintf_client(buf, len, size, va_arg(ap, aClient *), printf_flag);
			break;
		    case PF_NUMERIC:
    			len = Iprintf_numeric(buf, len, size, va_arg(ap, unsigned long));
			break;
		    case PF_CHAR:
    			len = Iprintf_char(buf, len, size, va_arg(ap, int));
			break;
		    case PF_CHANNEL:
    			len = Iprintf_channel(buf, len, size, va_arg(ap, aChannel *));
			break;
		    case PF_TIMESTAMP:
    			len = Iprintf_timestamp(buf, len, size, va_arg(ap, void *));
			break;
		    case PF_CONFITEM:
    			len = Iprintf_confitem(buf, len, size, va_arg(ap, aConfItem *));
			break;
		    case PF_MASKITEM:
			len = Iprintf_maskitem(buf, len, size, va_arg(ap, aMaskItem *));
			break;
		    case PF_HEX:
    			len = Iprintf_hex(buf, len, size, va_arg(ap, long), printf_flag);
			break;
		    case PF_OCTAL:
    			len = Iprintf_octal(buf, len, size, va_arg(ap, long));
			break;
		    case PF_CHARPP:
			do {
			    char ***t = va_arg(ap, char ***);
			    int c = va_arg(ap, int);
			    len = Iprintf_string_array(buf, len, size, t, c);
			} while(0);
			break;
		    default:
			break;
		}
		break;
	    default:
		printf_flag = printf_type = 0;
		buf[len++] = *format++;
		break;
	}
	printf_flag = printf_type = 0;
    }
    buf[len] = 0;
    return len;
}

int ircsprintf(char *str, const char *format, ...)
{
    int ret;
    va_list vl;
    va_start(vl, format);
    ret = Iprintf(str, 0, format, vl);
    va_end(vl);
    return ret;
}

int ircsnprintf(char *str, size_t size, const char *format, ...)
{
    int ret;
    va_list vl;
    va_start(vl, format);
    ret = Iprintf(str, size, format, vl);
    va_end(vl);
    return ret;
}

int ircvsprintf(char *str, const char *format, va_list ap)
{
    int ret;
    ret = Iprintf(str, 0, format, ap);
    return ret;
}

int ircvsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    int ret;
    ret = Iprintf(str, size, format, ap);
    return ret;
}
