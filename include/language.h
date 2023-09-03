/************************************************************************
 *   IRC - Internet Relay Chat, include/language.h
 *   Copyright (C) 2001 Yusuf Iskenderoglu
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

#ifndef LANGUAGE_H
#define LANGUAGE_H 1

struct Client;

struct LangHash
{
  char   *langname;
  char   *helpdirname;
  char   **fmtarray;
  struct LangHash *next;
}; 

#define MAX_LANG_HASH  387

extern int clear_hash_lang(void);
extern void lang_add_table(char *, char *, char **);
extern void lang_del_table(char *);
extern int lang_parse(char *givenname);

extern int set_language(int langindex);

extern char **get_language(int langindex);

extern char *rpl_str(int numeric);

extern char *get_numeric_format_in_lang(int *numeric, char **replylist);

extern char *get_langpath(aClient *);

extern void list_languages(struct Client *cptr);

#endif
