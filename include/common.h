/************************************************************************
 *   IRC - Internet Relay Chat, include/common.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Armin Gruner
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
 */

/*
 * $Id: common.h,v 1.5 2003/07/07 21:59:12 tr-ircd Exp $ 
 */

#ifndef	COMMON_H
#define COMMON_H 1

# ifdef TRUE
#	undef TRUE
# endif
# ifdef FALSE
#	undef FALSE
# endif

# define FALSE (0)
# define TRUE  (!FALSE)
# define HIDEME 2

#define DupString(x,y) do{x=MyMalloc(strlen(y)+1);(void)strcpy(x,y);}while(0)
#define EmptyString(x) (!(x) || (*(x) == '\0'))

extern const unsigned char ToLowerTab[];
#define ToLower(c) (ToLowerTab[(unsigned char)(c)])

extern const unsigned char ToUpperTab[];
#define ToUpper(c) (ToUpperTab[(unsigned char)(c)])

extern const unsigned int CharAttrs[];

#define PRINT_C   0x001
#define CNTRL_C   0x002
#define ALPHA_C   0x004
#define PUNCT_C   0x008
#define DIGIT_C   0x010
#define SPACE_C   0x020
#define NICK_C    0x040
#define CHAN_C    0x080
#define KWILD_C   0x100
#define CHANPFX_C 0x200
#define USER_C    0x400
#define HOST_C    0x800
#define NONEOS_C 0x1000
#define SERV_C   0x2000
#define EOL_C    0x4000

#define IsHostChar(c)   (CharAttrs[(unsigned char)(c)] & HOST_C)
#define IsUserChar(c)   (CharAttrs[(unsigned char)(c)] & USER_C)
#define IsChanPrefix(c) (CharAttrs[(unsigned char)(c)] & CHANPFX_C)
#define IsChanChar(c)   (CharAttrs[(unsigned char)(c)] & CHAN_C)
#define IsKWildChar(c)  (CharAttrs[(unsigned char)(c)] & KWILD_C)
#define IsNickChar(c)   (CharAttrs[(unsigned char)(c)] & NICK_C)
#define IsServChar(c)   (CharAttrs[(unsigned char)(c)] & (NICK_C | SERV_C))
#define IsCntrl(c)      (CharAttrs[(unsigned char)(c)] & CNTRL_C)
#define IsAlpha(c)      (CharAttrs[(unsigned char)(c)] & ALPHA_C)
#define IsSpace(c)      (CharAttrs[(unsigned char)(c)] & SPACE_C)
#define IsLower(c)      (IsAlpha((c)) && ((unsigned char)(c) > 0x5f))
#define IsUpper(c)      (IsAlpha((c)) && ((unsigned char)(c) < 0x60))
#define IsDigit(c)      (CharAttrs[(unsigned char)(c)] & DIGIT_C)
#define IsXDigit(c) 	(IsDigit(c) || ('a' <= (c) && (c) <= 'f') || ('A' <= (c) && (c) <= 'F'))
#define IsAlNum(c) 	(CharAttrs[(unsigned char)(c)] & (DIGIT_C | ALPHA_C))
#define IsPrint(c) 	(CharAttrs[(unsigned char)(c)] & PRINT_C)
#define IsAscii(c) 	((unsigned char)(c) < 0x80)
#define IsGraph(c) 	(IsPrint((c)) && ((unsigned char)(c) != 0x32))
#define IsPunct(c) 	(!(CharAttrs[(unsigned char)(c)] & (CNTRL_C | ALPHA_C | DIGIT_C)))
#define IsNonEOS(c) 	(CharAttrs[(unsigned char)(c)] & NONEOS_C)
#define IsEol(c) 	(CharAttrs[(unsigned char)(c)] & EOL_C)

#define BadPtr(x) 	(!(x) || (*(x) == '\0'))

#define IsValid(c) 	(((c) >= 'A' && (c) < '~') || IsDigit(c) || (c) == '-')

#endif

