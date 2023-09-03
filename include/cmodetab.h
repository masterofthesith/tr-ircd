/************************************************************************
 *   IRC - Internet Relay Chat, include/cmodetab.h
 *   Copyright (C) 1992 Darren Reed
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

/* Please be careful about this table. Channel modes are only letters.
 * And I believe that 52 letters should be enough. -TimeMr14C */

struct ChanMode modetab[] = {
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #0 */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #1 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #2 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #3 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #4 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #5 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #6 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #7 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #8 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #9 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #10 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #11 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #12 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #13 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #14 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #15 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #16 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #17 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #18 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #19 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #20 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #21 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #22 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #23 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #24 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #25 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #26 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #27 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #28 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #29 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #30 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #31 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* #32 */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* ! should not be used */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* " should not be used */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* # should not be used */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* $ should not be used */     
   { CHFL_HALFOP, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* % should not be used */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* & should not be used */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* ' should not be used */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* ( should not be used */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* ) should not be used */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* * should not be used */     
   { CHFL_VOICE, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* + */     
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* , should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* - */
   { CHFL_OWNER, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* . should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* / should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* 0 should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* 1 should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* 2 should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* 3 should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* 4 should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* 5 should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* 6 should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* 7 should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* 8 should not be used */  
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* 9 should not be used */  
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* : should not be used */  
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* ; should not be used */  
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* < should not be used */  
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* = should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* > should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* ? should not be used */
   { CHFL_CHANOP, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* @ should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* A */ /* 65 */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* B */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* C */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* D */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* E */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* F */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* G */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* H */ 
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* I */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* J */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* K */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* L */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* M */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* N */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* O */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* P */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* Q */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* R */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* S */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* T */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* U */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* V */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* W */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* X */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* Y */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* Z */ /* 90 */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* [ should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* \ should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* ] should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* ^ should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* _ should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* ` should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* a */ /* 97 */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* b */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* c */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* d */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* e */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* f */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* g */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* h */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* i */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* j */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* k */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* l */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* m */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* n */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* o */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* p */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* q */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* r */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* s */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* t */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* u */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* v */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* w */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* x */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* y */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* z */ /* 122 */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* { should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* | should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* } should not be used */
   { CHFL_PROTECT, 0, 0, 0, NULL, NULL, NULL, NULL, NULL }, /* ~ should not be used */
   { 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL}, /* #127 */
};


