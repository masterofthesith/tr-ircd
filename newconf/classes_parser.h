/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_CLASSES_CLASSES_PARSER_H_INCLUDED
# define YY_CLASSES_CLASSES_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int classesdebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    CLASS = 258,                   /* CLASS  */
    PING_TIME = 259,               /* PING_TIME  */
    CONNECTFREQ = 260,             /* CONNECTFREQ  */
    MAX_LINKS = 261,               /* MAX_LINKS  */
    SENDQ = 262,                   /* SENDQ  */
    QSTRING = 263,                 /* QSTRING  */
    NUMBER = 264,                  /* NUMBER  */
    SECONDS = 265,                 /* SECONDS  */
    MINUTES = 266,                 /* MINUTES  */
    HOURS = 267,                   /* HOURS  */
    DAYS = 268,                    /* DAYS  */
    WEEKS = 269,                   /* WEEKS  */
    MONTHS = 270,                  /* MONTHS  */
    YEARS = 271,                   /* YEARS  */
    DECADES = 272,                 /* DECADES  */
    CENTURIES = 273,               /* CENTURIES  */
    MILLENNIA = 274,               /* MILLENNIA  */
    BYTES = 275,                   /* BYTES  */
    KBYTES = 276,                  /* KBYTES  */
    MBYTES = 277                   /* MBYTES  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define CLASS 258
#define PING_TIME 259
#define CONNECTFREQ 260
#define MAX_LINKS 261
#define SENDQ 262
#define QSTRING 263
#define NUMBER 264
#define SECONDS 265
#define MINUTES 266
#define HOURS 267
#define DAYS 268
#define WEEKS 269
#define MONTHS 270
#define YEARS 271
#define DECADES 272
#define CENTURIES 273
#define MILLENNIA 274
#define BYTES 275
#define KBYTES 276
#define MBYTES 277

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 51 "classes_parser.y"

  int number;
  char *string;

#line 116 "classes_parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE classeslval;


int classesparse (void);


#endif /* !YY_CLASSES_CLASSES_PARSER_H_INCLUDED  */
