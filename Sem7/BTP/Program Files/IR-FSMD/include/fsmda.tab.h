/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_FSMDA_TAB_H_INCLUDED
# define YY_YY_FSMDA_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IDENTIFIER = 258,
    CONSTANT = 259,
    STRING_LITERAL = 260,
    LE_OP = 261,
    GE_OP = 262,
    EQ_OP = 263,
    NE_OP = 264,
    AND = 265,
    OR = 266,
    NOT = 267,
    READ = 268,
    OUT = 269,
    EQUAL = 270,
    COMMA = 271,
    SEMIC = 272,
    SEPARATOR = 273,
    LT_OP = 274,
    GT_OP = 275,
    LEFT_BR = 276,
    RIGHT_BR = 277,
    LEFT_SQBR = 278,
    RIGHT_SQBR = 279,
    MULT = 280,
    DIV = 281,
    MOD = 282,
    PLUS = 283,
    MINUS = 284
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 14 "./src/fsmda.y" /* yacc.c:1909  */

	char* string;
	struct normalized_cell* norm;
	struct Assign* data_trans;
	struct Assign_list* dt_list;
	struct Substatement* substmt;
	struct Substatement_list* substmt_list;
	struct Statement* stmt;
	struct Statement_list* stmt_list;
	

#line 96 "fsmda.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_FSMDA_TAB_H_INCLUDED  */
