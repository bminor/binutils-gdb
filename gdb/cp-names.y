/* YACC parser for C expressions, for GDB.
   Copyright 1986, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997,
   1998, 1999, 2000, 2003
   Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* Parse a C expression from text in a string,
   and return the result as a  struct expression  pointer.
   That structure contains arithmetic operations in reverse polish,
   with constants represented by operations that are followed by special data.
   See expression.h for the details of the format.
   What is important here is that it can be built up sequentially
   during the process of parsing; the lower levels of the tree always
   come first in the result.

   Note that malloc's and realloc's in this file are transformed to
   xmalloc and xrealloc respectively by the same sed command in the
   makefile that remaps any other malloc/realloc inserted by the parser
   generator.  Doing this with #defines and trying to control the interaction
   with include files (<malloc.h> and <stdlib.h> for example) just became
   too messy, particularly when such includes can be inserted at random
   times by the parser generator.  */
   
%{

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef long long LONGEST;
typedef unsigned long long ULONGEST;
typedef long double DOUBLEST;

#include "safe-ctype.h"

#include "../libiberty/cp-demangle.c"

static char *lexptr, *prev_lexptr;

static struct d_comp *d_qualify (struct d_comp *, LONGEST, int);

static struct d_comp *d_int_type (int);

static struct d_comp *d_op_from_string (const char *opname);

static struct d_comp *d_unary (const char *opname, struct d_comp *);
static struct d_comp *d_binary (const char *opname, struct d_comp *, struct d_comp *);

/* Global state, ew.  */
struct d_info *di;
static struct d_comp *result;

/* Ew ew, ew ew, ew ew ew.  */
#define error printf

#define HOST_CHAR_BIT 8
#define NORETURN

#undef TARGET_INT_BIT
#define TARGET_INT_BIT 32

#undef TARGET_LONG_BIT
#define TARGET_LONG_BIT 32

#undef TARGET_LONG_LONG_BIT
#define TARGET_LONG_LONG_BIT 64

#define QUAL_CONST 1
#define QUAL_RESTRICT 2
#define QUAL_VOLATILE 4

#define INT_CHAR	1
#define INT_SHORT	2
#define INT_LONG	3
#define INT_LLONG	4

#define INT_SIGNED	(1 << 3)
#define INT_UNSIGNED	(2 << 3)

#define BINOP_ADD 1
#define BINOP_RSH 2
#define BINOP_LSH 3
#define BINOP_SUB 4
#define BINOP_MUL 5
#define BINOP_DIV 6
#define BINOP_REM 7
#define BINOP_BITWISE_IOR 8
#define BINOP_BITWISE_AND 9
#define BINOP_BITWISE_XOR 10
#define BINOP_END 11

/* Remap normal yacc parser interface names (yyparse, yylex, yyerror, etc),
   as well as gratuitiously global symbol names, so we can have multiple
   yacc generated parsers in gdb.  Note that these are only the variables
   produced by yacc.  If other parser generators (bison, byacc, etc) produce
   additional global names that conflict at link time, then those parser
   generators need to be fixed instead of adding those names to this list. */

#define	yymaxdepth cpname_maxdepth
#define	yyparse	cpname_parse
#define	yylex	cpname_lex
#define	yyerror	cpname_error
#define	yylval	cpname_lval
#define	yychar	cpname_char
#define	yydebug	cpname_debug
#define	yypact	cpname_pact	
#define	yyr1	cpname_r1			
#define	yyr2	cpname_r2			
#define	yydef	cpname_def		
#define	yychk	cpname_chk		
#define	yypgo	cpname_pgo		
#define	yyact	cpname_act		
#define	yyexca	cpname_exca
#define yyerrflag cpname_errflag
#define yynerrs	cpname_nerrs
#define	yyps	cpname_ps
#define	yypv	cpname_pv
#define	yys	cpname_s
#define	yy_yys	cpname_yys
#define	yystate	cpname_state
#define	yytmp	cpname_tmp
#define	yyv	cpname_v
#define	yy_yyv	cpname_yyv
#define	yyval	cpname_val
#define	yylloc	cpname_lloc
#define yyreds	cpname_reds		/* With YYDEBUG defined */
#define yytoks	cpname_toks		/* With YYDEBUG defined */
#define yyname	cpname_name		/* With YYDEBUG defined */
#define yyrule	cpname_rule		/* With YYDEBUG defined */
#define yylhs	cpname_yylhs
#define yylen	cpname_yylen
#define yydefred cpname_yydefred
#define yydgoto	cpname_yydgoto
#define yysindex cpname_yysindex
#define yyrindex cpname_yyrindex
#define yygindex cpname_yygindex
#define yytable	 cpname_yytable
#define yycheck	 cpname_yycheck

#ifndef YYDEBUG
#define	YYDEBUG 1		/* Default to yydebug support */
#endif

int yyparse (void);

static int yylex (void);

void yyerror (char *);

%}

/* Although the yacc "value" of an expression is not used,
   since the result is stored in the structure being created,
   other node types do have values.  */

%union
  {
    struct d_comp *comp;
    struct {
      struct d_comp *comp;
      struct d_comp **last;
    } nested;
    struct {
      struct d_comp *comp, *last;
    } nested1;
    LONGEST lval;
    struct {
      LONGEST val;
      struct d_comp *type;
    } typed_val_int;
    struct {
      DOUBLEST dval;
    } typed_val_float;
    const char *opname;
  }

%{
/* YYSTYPE gets defined by %union */
static int parse_number (char *, int, int, YYSTYPE *);
%}

%type <comp> exp exp1 type start operator qualified
%type <comp> unqualified_name scope_id ext_name colon_ext_name
%type <comp> template template_arg basic_exp
%type <comp> /* base_function typed_function */ colon_name
%type <comp> builtin_type type2 type1a type1b function_arglist
%type <comp> decl1b

%type <nested> template_params function_args
%type <nested> ptr_operator_1 ptr_operator_2 ptr_operator ptr_operator_seq

%type <nested1> nested_name

%type <lval> qualifier qualifiers qualifiers_opt

%type <lval> sign size int_type

%token <typed_val_int> INT
%token <typed_val_float> FLOAT

%token <comp> NAME
%type <comp> name

%token STRUCT CLASS UNION ENUM SIZEOF UNSIGNED COLONCOLON
%token TEMPLATE
%token ERROR
%token NEW DELETE OPERATOR
%token STATIC_CAST REINTERPRET_CAST DYNAMIC_CAST

/* Special type cases, put in to allow the parser to distinguish different
   legal basetypes.  */
%token SIGNED_KEYWORD LONG SHORT INT_KEYWORD CONST_KEYWORD VOLATILE_KEYWORD DOUBLE_KEYWORD BOOL
%token ELLIPSIS RESTRICT VOID FLOAT_KEYWORD CHAR WCHAR_T

%token <opname> ASSIGN_MODIFY

/* C++ */
%token TRUEKEYWORD
%token FALSEKEYWORD

%left ','
%right '=' ASSIGN_MODIFY
%right '?'
%left OROR
%left ANDAND
%left '|'
%left '^'
%left '&'
%left EQUAL NOTEQUAL
%left '<' '>' LEQ GEQ
%left LSH RSH
%left '@'
%left '+' '-'
%left '*' '/' '%'
%right UNARY INCREMENT DECREMENT
%right ARROW '.' '[' '('
%left COLONCOLON


%%

start		:	type
			{ result = $1; }
		|	qualified
			{ result = $1; }
		;

operator	:	OPERATOR NEW
			{ $$ = d_op_from_string ("new"); }
		|	OPERATOR DELETE
			{ $$ = d_op_from_string ("delete"); }
		|	OPERATOR NEW '[' ']'
			{ $$ = d_op_from_string ("new[]"); }
		|	OPERATOR DELETE '[' ']'
			{ $$ = d_op_from_string ("delete[]"); }
		|	OPERATOR '+'
			{ $$ = d_op_from_string ("+"); }
		|	OPERATOR '-'
			{ $$ = d_op_from_string ("-"); }
		|	OPERATOR '*'
			{ $$ = d_op_from_string ("*"); }
		|	OPERATOR '/'
			{ $$ = d_op_from_string ("/"); }
		|	OPERATOR '%'
			{ $$ = d_op_from_string ("%"); }
		|	OPERATOR '^'
			{ $$ = d_op_from_string ("^"); }
		|	OPERATOR '&'
			{ $$ = d_op_from_string ("&"); }
		|	OPERATOR '|'
			{ $$ = d_op_from_string ("|"); }
		|	OPERATOR '~'
			{ $$ = d_op_from_string ("~"); }
		|	OPERATOR '!'
			{ $$ = d_op_from_string ("!"); }
		|	OPERATOR '='
			{ $$ = d_op_from_string ("="); }
		|	OPERATOR '<'
			{ $$ = d_op_from_string ("<"); }
		|	OPERATOR '>'
			{ $$ = d_op_from_string (">"); }
		|	OPERATOR ASSIGN_MODIFY
			{ $$ = d_op_from_string ($2); }
		|	OPERATOR LSH
			{ $$ = d_op_from_string ("<<"); }
		|	OPERATOR RSH
			{ $$ = d_op_from_string (">>"); }
		|	OPERATOR EQUAL
			{ $$ = d_op_from_string ("=="); }
		|	OPERATOR NOTEQUAL
			{ $$ = d_op_from_string ("!="); }
		|	OPERATOR LEQ
			{ $$ = d_op_from_string ("<="); }
		|	OPERATOR GEQ
			{ $$ = d_op_from_string (">="); }
		|	OPERATOR ANDAND
			{ $$ = d_op_from_string ("&&"); }
		|	OPERATOR OROR
			{ $$ = d_op_from_string ("||"); }
		|	OPERATOR INCREMENT
			{ $$ = d_op_from_string ("++"); }
		|	OPERATOR DECREMENT
			{ $$ = d_op_from_string ("--"); }
		|	OPERATOR ','
			{ $$ = d_op_from_string (","); }
		|	OPERATOR ARROW '*'
			{ $$ = d_op_from_string ("*"); }
		|	OPERATOR ARROW
			{ $$ = d_op_from_string ("->"); }
		|	OPERATOR '(' ')'
			{ $$ = d_op_from_string ("->*"); }
		|	OPERATOR '[' ']'
			{ $$ = d_op_from_string ("[]"); }
/*
		|	OPERATOR type
			{ $$ = d_make_node (di, D_COMP_CAST, $2, NULL); }
*/
		;

/* D_COMP_NAME */
/* This accepts certain invalid placements of '~'.  */
unqualified_name:	operator
		|	'~' NAME
			{ $$ = d_make_dtor (di, gnu_v3_complete_object_dtor, $2); }
		;

scope_id	:	NAME
		|	template
		;

colon_name	:	name
		|	COLONCOLON name
			{ $$ = $2; }
		;

/* D_COMP_QUAL_NAME */
/* D_COMP_CTOR / D_COMP_DTOR ? */
name		:	nested_name scope_id
			{ $$ = $1.comp; d_right ($1.last) = $2; }
		|	scope_id
		;

colon_ext_name	:	ext_name
		|	COLONCOLON ext_name
			{ $$ = $2; }
		;

ext_name	:	name
		|	nested_name unqualified_name
			{ $$ = $1.comp; d_right ($1.last) = $2; }
		|	unqualified_name
		;

nested_name	:	scope_id COLONCOLON
			{ $$.comp = d_make_comp (di, D_COMP_QUAL_NAME, $1, $1);
			  d_right ($$.comp) = NULL;
			  $$.last = $$.comp;
			}
		|	nested_name scope_id COLONCOLON
			{ $$.comp = $1.comp;
			  d_right ($1.last) = d_make_comp (di, D_COMP_QUAL_NAME, $2, $2);
			  $$.last = d_right ($1.last);
			  d_right ($$.last) = NULL;
			}
		;

/* D_COMP_TEMPLATE */
/* D_COMP_TEMPLATE_ARGLIST */
template	:	NAME '<' template_params '>'
			{ $$ = d_make_comp (di, D_COMP_TEMPLATE, $1, $3.comp); }
		;

template_params	:	template_arg
			{ $$.comp = d_make_comp (di, D_COMP_TEMPLATE_ARGLIST, $1, NULL);
			$$.last = &d_right ($$.comp); }
		|	template_params ',' template_arg
			{ $$.comp = $1.comp;
			  *$1.last = d_make_comp (di, D_COMP_TEMPLATE_ARGLIST, $3, NULL);
			  $$.last = &d_right (*$1.last);
			}
		;

/* Also an integral constant-expression of integral type, and a
   pointer to member (?) */
template_arg	:	type
		|	'&' colon_ext_name
			{ $$ = d_make_comp (di, D_COMP_REFERENCE, $2, NULL); }
		|	'&' '(' colon_ext_name ')'
			{ $$ = d_make_comp (di, D_COMP_REFERENCE, $3, NULL); }
		|	basic_exp
		;

function_args	:	type
			{ if ($1->type == D_COMP_BUILTIN_TYPE
			      && $1->u.s_builtin.type->print == D_PRINT_VOID)
			    {
			      $$.comp = NULL;
			      $$.last = &$$.comp;
			    }
			  else
			    {
			      $$.comp = d_make_comp (di, D_COMP_ARGLIST, $1, NULL);
			      $$.last = &d_right ($$.comp);
			    }
			}
		|	function_args ',' type
			{ *$1.last = d_make_comp (di, D_COMP_ARGLIST, $3, NULL);
			  $$.comp = $1.comp;
			  $$.last = &d_right (*$1.last);
			}
		|	function_args ',' ELLIPSIS
			{ *$1.last
			    = d_make_comp (di, D_COMP_ARGLIST,
					   d_make_builtin_type (di, &d_builtin_types['z' - 'a']),
					   NULL);
			  $$.comp = $1.comp;
			  $$.last = &d_right (*$1.last);
			}
		;

/*FIXME d_qualify */
function_arglist:	'(' function_args ')' qualifiers_opt
			{ $$ = d_make_comp (di, D_COMP_FUNCTION_TYPE, NULL, $2.comp); }
		|	'(' ')' qualifiers_opt
			{ $$ = d_make_comp (di, D_COMP_FUNCTION_TYPE, NULL, NULL); }
		;

/*
base_function	:	ext_name '(' function_args ')'
			{ $$ = d_make_comp (di, D_COMP_TYPED_NAME, $1,
					    d_make_comp (di, D_COMP_FUNCTION_TYPE, NULL, $3.comp)); }
		|	ext_name '(' ')'
			{ $$ = d_make_comp (di, D_COMP_TYPED_NAME, $1,
					    d_make_comp (di, D_COMP_FUNCTION_TYPE, NULL, NULL)); }
		;

typed_function	:	type base_function
			{ $$ = $2; d_left (d_right ($2)) = $1; }
		|	type1a COLONCOLON base_function
			{ $$ = $3; d_left (d_right ($3)) = $1; }
		;
*/

qualified	:	ext_name function_arglist
		{}
		|	type1a ext_name function_arglist
		{}
		|	type2 ext_name function_arglist
		{}
		|	type1a COLONCOLON ext_name function_arglist
		{}
		|	decl1b function_arglist
		{}
		;
/* base_function qualifiers_opt 
			{ $$ = $1;
			  d_left ($$) = d_qualify (d_left ($$), $2, 1); }
*/
/*
		|	COLONCOLON base_function qualifiers_opt
			{ $$ = $2;
			  d_left ($$) = d_qualify (d_left ($$), $3, 1); }
*/
/*
		|	typed_function qualifiers_opt
			{ $$ = $1;
			  d_left ($$) = d_qualify (d_left ($$), $2, 1); }
		;
*/

/* Should do something about D_COMP_VENDOR_TYPE_QUAL */
qualifiers_opt	:	/* epsilon */
			{ $$ = 0; }
		|	qualifiers
			{ $$ = $1; }
		;

qualifier	:	RESTRICT
			{ $$ = QUAL_RESTRICT; }
		|	VOLATILE_KEYWORD
			{ $$ = QUAL_VOLATILE; }
		|	CONST_KEYWORD
			{ $$ = QUAL_CONST; }
		;

qualifiers	:	qualifier
			{ $$ = $1; }
		|	qualifier qualifiers
			{ $$ = $1 | $2; }
		;

int_opt		:	/* */
			{ }
		|	INT_KEYWORD
			{ }
		;

sign		:	SIGNED_KEYWORD
			{ $$ = INT_SIGNED; }
		|	UNSIGNED
			{ $$ = INT_UNSIGNED; }
		;

size		:	CHAR
			{ $$ = INT_CHAR; }
		|	LONG
			{ $$ = INT_LONG; }
		|	SHORT
			{ $$ = INT_SHORT; }
		|	LONG LONG
			{ $$ = INT_LLONG; }
		;

int_type	:	sign size int_opt
			{ $$ = $1 | $2; }
		|	size sign int_opt
			{ $$ = $1 | $2; }
		|	sign INT_KEYWORD size
			{ $$ = $1 | $3; }
		|	size INT_KEYWORD sign
			{ $$ = $1 | $3; }
		|	INT_KEYWORD sign size
			{ $$ = $2 | $3; }
		|	INT_KEYWORD size sign
			{ $$ = $2 | $3; }
		|	sign int_opt
			{ $$ = $1; }
		|	INT_KEYWORD sign
			{ $$ = $2; }
		|	size int_opt
			{ $$ = $1; }
		|	INT_KEYWORD size
			{ $$ = $2; }
		|	INT_KEYWORD
			{ $$ = 0; }
		;

builtin_type	:	int_type
			{ $$ = d_int_type ($1); }
		|	FLOAT_KEYWORD
			{ $$ = d_make_builtin_type (di, &d_builtin_types['f' - 'a']); }
		|	DOUBLE_KEYWORD
			{ $$ = d_make_builtin_type (di, &d_builtin_types['d' - 'a']); }
		|	LONG DOUBLE_KEYWORD
			{ $$ = d_make_builtin_type (di, &d_builtin_types['e' - 'a']); }
		|	BOOL
			{ $$ = d_make_builtin_type (di, &d_builtin_types['b' - 'a']); }
		|	WCHAR_T
			{ $$ = d_make_builtin_type (di, &d_builtin_types['w' - 'a']); }
		|	VOID
			{ $$ = d_make_builtin_type (di, &d_builtin_types['v' - 'a']); }
		;

ptr_operator_1	:	'*' qualifiers_opt
			{ $$.comp = d_make_empty (di, D_COMP_POINTER);
			  $$.comp->u.s_binary.left = $$.comp->u.s_binary.right = NULL;
			  $$.last = &d_left ($$.comp);
			  $$.comp = d_qualify ($$.comp, $2, 0); }
		|	'&'
			{ $$.comp = d_make_empty (di, D_COMP_REFERENCE);
			  $$.comp->u.s_binary.left = $$.comp->u.s_binary.right = NULL;
			  $$.last = &d_left ($$.comp); }
		|	nested_name '*' qualifiers_opt
			{ $$.comp = d_make_empty (di, D_COMP_PTRMEM_TYPE);
			  $$.comp->u.s_binary.left = $1.comp;
			  /* Convert the innermost D_COMP_QUAL_NAME to a D_COMP_NAME.  */
			  *$1.last = *d_left ($1.last);
			  $$.comp->u.s_binary.right = NULL;
			  $$.last = &d_right ($$.comp);
			  $$.comp = d_qualify ($$.comp, $3, 0); }
		;

ptr_operator_2	:	COLONCOLON nested_name '*' qualifiers_opt
			{ $$.comp = d_make_empty (di, D_COMP_PTRMEM_TYPE);
			  $$.comp->u.s_binary.left = $2.comp;
			  /* Convert the innermost D_COMP_QUAL_NAME to a D_COMP_NAME.  */
			  *$2.last = *d_left ($2.last);
			  $$.comp->u.s_binary.right = NULL;
			  $$.last = &d_right ($$.comp);
			  $$.comp = d_qualify ($$.comp, $4, 0); }
		;

ptr_operator	:	ptr_operator_1
		|	ptr_operator_2
		;

ptr_operator_seq:	ptr_operator
		|	ptr_operator ptr_operator_seq
			{ $$.comp = $2.comp;
			  $$.last = $1.last;
			  *$2.last = $1.comp; }
		;

/* "type1" are the type productions which can legally be followed by a ::
   token.  "type2" are the type productions which can not.  For instance,
   int ::Foo::* and Bar Foo::* are both pointer-to-member types, but
   Bar ::Foo::* is a ptr_operator, because :: is consumed greedily.  */

type1a		:	builtin_type qualifiers_opt
			{ $$ = d_qualify ($1, $2, 0); }
		|	qualifiers builtin_type qualifiers_opt
			{ $$ = d_qualify ($2, $1 | $3, 0); }
		|	colon_name qualifiers
			{ $$ = d_qualify ($1, $2, 0); }
		|	qualifiers colon_name qualifiers
			{ $$ = d_qualify ($2, $1 | $3, 0); }

		|	type1a ptr_operator_1
			{ $$ = $2.comp;
			  *$2.last = $1; }
		|	type2 ptr_operator_1
			{ $$ = $2.comp;
			  *$2.last = $1; }
/*
		|	type qualifier
			{ $$ = d_qualify ($1, $2, 0); }
*/
		|	type '[' ']'
			{ $$ = d_make_comp (di, D_COMP_ARRAY_TYPE, $1, NULL); }
		|	type '[' INT ']'
			{ struct d_comp *i;
			  /* FIXME: Blatant memory leak.  */
			  char *buf = malloc (24);
			  sprintf (buf, "%d", (int) $3.val);
			  i = d_make_name (di, buf, strlen (buf));
			  $$ = d_make_comp (di, D_COMP_ARRAY_TYPE, $1,
					    d_make_comp (di, D_COMP_LITERAL, $3.type, i));
			}
		;

		/* FIXME this comment is completely wrong; this is not allowing, it's rejecting */
		/* This ext_name and the one below for
		   pointer-to-member-function should normally be
		   "type" instead.  That causes a reduce/reduce
		   conflict.  Allow a few invalid inputs to keep life
		   simple.  */
type1b		:	ext_name '(' ptr_operator_seq ')' '(' function_args ')'
			{ struct d_comp *funtype;
			  funtype = d_make_comp (di, D_COMP_FUNCTION_TYPE, $1, $6.comp);
			  $$ = $3.comp;
			  *$3.last = funtype; }
		;

/* FIXME ACTION */
/* FIXME when I add the name here I get a conflict on ptr-to-members that I don't see a soln for */
decl1b		:	ext_name '(' ptr_operator_seq ')' '(' function_args ')'
			{ struct d_comp *funtype;
			  funtype = d_make_comp (di, D_COMP_FUNCTION_TYPE, $1, $6.comp);
			  $$ = $3.comp;
			  *$3.last = funtype; }

/*
		|	COLONCOLON ext_name '(' ptr_operator_seq ')' '(' function_args ')'
			{ struct d_comp *funtype;
			  funtype = d_make_comp (di, D_COMP_FUNCTION_TYPE, $2, $7.comp);
			  $$ = $4.comp;
			  *$4.last = funtype; }
*/
		;

type2		:	qualifiers colon_name
			{ $$ = d_qualify ($2, $1, 0); }
		|	name
		|	type1a ptr_operator_2
			{ $$ = d_make_comp (di, D_COMP_POINTER, $1, NULL); }
		;

type		:	type1a
		|	type1b
		|	type2
		;

basic_exp	:	exp
		;

exp	:	'(' exp1 ')'
		{ $$ = $2; }
	;

/* Silly trick.  Only allow '>' when parenthesized, in order to
   handle conflict with templates.  */
exp1	:	exp
	;

exp1	:	exp '>' exp
		{ $$ = d_binary (">", $1, $3); }
	;

/* Expressions, not including the comma operator.  */
exp	:	'-' exp    %prec UNARY
		{ $$ = d_unary ("-", $2); }
	;

exp	:	'!' exp    %prec UNARY
		{ $$ = d_unary ("!", $2); }
	;

exp	:	'~' exp    %prec UNARY
		{ $$ = d_unary ("~", $2); }
	;

/* Casts.  First your normal C-style cast.  */

exp	:	'(' type ')' exp  %prec UNARY
		{ $$ = d_make_comp (di, D_COMP_UNARY,
				    d_make_comp (di, D_COMP_CAST, $2, NULL),
				    $4);
		}
	;

/* Mangling does not differentiate between these, so we don't need to
   either.  */
exp	:	STATIC_CAST '<' type '>' '(' exp1 ')' %prec UNARY
		{ $$ = d_make_comp (di, D_COMP_UNARY,
				    d_make_comp (di, D_COMP_CAST, $3, NULL),
				    $6);
		}
	;

exp	:	DYNAMIC_CAST '<' type '>' '(' exp1 ')' %prec UNARY
		{ $$ = d_make_comp (di, D_COMP_UNARY,
				    d_make_comp (di, D_COMP_CAST, $3, NULL),
				    $6);
		}
	;

exp	:	REINTERPRET_CAST '<' type '>' '(' exp1 ')' %prec UNARY
		{ $$ = d_make_comp (di, D_COMP_UNARY,
				    d_make_comp (di, D_COMP_CAST, $3, NULL),
				    $6);
		}
	;

/* Another form of C++-style cast.  "type ( exp1 )" is not allowed (it's too
   ambiguous), but "name ( exp1 )" is.  We don't support that since it looks
   too much like a function type, and doesn't appear in the output of any of
   the demanglers.  */
exp	:	builtin_type '(' exp1 ')' %prec UNARY
		{ $$ = d_make_comp (di, D_COMP_UNARY,
				    d_make_comp (di, D_COMP_CAST, $1, NULL),
				    $3);
		}
	;

/* FIXME ._0 style anonymous names; anonymous namespaces */

/* Binary operators in order of decreasing precedence.  */

exp	:	exp '*' exp
		{ $$ = d_binary ("*", $1, $3); }
	;

exp	:	exp '/' exp
		{ $$ = d_binary ("/", $1, $3); }
	;

exp	:	exp '%' exp
		{ $$ = d_binary ("%", $1, $3); }
	;

exp	:	exp '+' exp
		{ $$ = d_binary ("+", $1, $3); }
	;

exp	:	exp '-' exp
		{ $$ = d_binary ("-", $1, $3); }
	;

exp	:	exp LSH exp
		{ $$ = d_binary ("<<", $1, $3); }
	;

exp	:	exp RSH exp
		{ $$ = d_binary (">>", $1, $3); }
	;

exp	:	exp EQUAL exp
		{ $$ = d_binary ("==", $1, $3); }
	;

exp	:	exp NOTEQUAL exp
		{ $$ = d_binary ("!=", $1, $3); }
	;

exp	:	exp LEQ exp
		{ $$ = d_binary ("<=", $1, $3); }
	;

exp	:	exp GEQ exp
		{ $$ = d_binary (">=", $1, $3); }
	;

exp	:	exp '<' exp
		{ $$ = d_binary ("<", $1, $3); }
	;

exp	:	exp '&' exp
		{ $$ = d_binary ("&", $1, $3); }
	;

exp	:	exp '^' exp
		{ $$ = d_binary ("^", $1, $3); }
	;

exp	:	exp '|' exp
		{ $$ = d_binary ("|", $1, $3); }
	;

exp	:	exp ANDAND exp
		{ $$ = d_binary ("&&", $1, $3); }
	;

exp	:	exp OROR exp
		{ $$ = d_binary ("||", $1, $3); }
	;

/* Not 100% sure these are necessary, but they're harmless.  */
exp	:	exp ARROW NAME
		{ $$ = d_binary ("->", $1, $3); }
	;

exp	:	exp '.' NAME
		{ $$ = d_binary (".", $1, $3); }
	;

exp	:	exp '?' exp ':' exp	%prec '?'
		{ $$ = d_make_comp (di, D_COMP_TRINARY, d_op_from_string ("?"),
				    d_make_comp (di, D_COMP_TRINARY_ARG1, $1,
						 d_make_comp (di, D_COMP_TRINARY_ARG2, $3, $5)));
		}
	;
			  
exp	:	INT
			{ struct d_comp *i;
			  /* FIXME: Blatant memory leak.  */
			  char *buf = malloc (24);
			  sprintf (buf, "%d", (int) $1.val);
			  i = d_make_name (di, buf, strlen (buf));
			  $$ = d_make_comp (di, D_COMP_LITERAL, $1.type, i);
			}
	;

/* Not generally allowed.  */
exp	:	FLOAT
			{ struct d_comp *i;
			  /* FIXME: Blatant memory leak.  */
			  char *buf = malloc (24);
			  sprintf (buf, "%f", (double) $1.dval);
			  i = d_make_name (di, buf, strlen (buf));
			  $$ = d_make_comp (di, D_COMP_LITERAL,
					    d_make_builtin_type (di, &d_builtin_types['d' - 'a']),
					    i);
			}
	;

exp	:	SIZEOF '(' type ')'	%prec UNARY
		{ $$ = d_unary ("sizeof", $3); }
	;

/* C++.  */
exp     :       TRUEKEYWORD    
		{ struct d_comp *i;
		  i = d_make_name (di, "1", 1);
		  $$ = d_make_comp (di, D_COMP_LITERAL,
				    d_make_builtin_type (di, &d_builtin_types['b' - 'a']),
				    i);
		}
	;

exp     :       FALSEKEYWORD   
		{ struct d_comp *i;
		  i = d_make_name (di, "0", 1);
		  $$ = d_make_comp (di, D_COMP_LITERAL,
				    d_make_builtin_type (di, &d_builtin_types['b' - 'a']),
				    i);
		}
	;

/* end of C++.  */

%%

/* */
struct d_comp *
d_qualify (struct d_comp *lhs, LONGEST qualifiers, int is_method)
{
  struct d_comp **inner_p;
  enum d_comp_type type;

  /* For now the order is CONST (innermost), VOLATILE, RESTRICT.  */

#define HANDLE_QUAL(TYPE, MTYPE, QUAL)				\
  if ((qualifiers & QUAL) && (type != TYPE) && (type != MTYPE))	\
    {								\
      *inner_p = d_make_comp (di, is_method ? MTYPE : TYPE,	\
			      *inner_p, NULL);			\
      inner_p = &d_left (*inner_p);				\
      type = (*inner_p)->type;					\
    }								\
  else if (type == TYPE || type == MTYPE)			\
    {								\
      inner_p = &d_left (*inner_p);				\
      type = (*inner_p)->type;					\
    }

  inner_p = &lhs;

  type = (*inner_p)->type;

  HANDLE_QUAL (D_COMP_RESTRICT, D_COMP_RESTRICT_THIS, QUAL_RESTRICT);
  HANDLE_QUAL (D_COMP_VOLATILE, D_COMP_VOLATILE_THIS, QUAL_VOLATILE);
  HANDLE_QUAL (D_COMP_CONST, D_COMP_CONST_THIS, QUAL_CONST);

  return lhs;
}

static struct d_comp *
d_int_type (int flags)
{
  int i;

  switch (flags)
    {
    case INT_SIGNED | INT_CHAR:
      i = 0;
      break;
    case INT_CHAR:
      i = 2;
      break;
    case INT_UNSIGNED | INT_CHAR:
      i = 7;
      break;
    case 0:
    case INT_SIGNED:
      i = 8;
      break;
    case INT_UNSIGNED:
      i = 9;
      break;
    case INT_LONG:
    case INT_SIGNED | INT_LONG:
      i = 11;
      break;
    case INT_UNSIGNED | INT_LONG:
      i = 12;
      break;
    case INT_SHORT:
    case INT_SIGNED | INT_SHORT:
      i = 18;
      break;
    case INT_UNSIGNED | INT_SHORT:
      i = 19;
      break;
    case INT_LLONG:
    case INT_SIGNED | INT_LLONG:
      i = 23;
      break;
    case INT_UNSIGNED | INT_LLONG:
      i = 24;
      break;
    default:
      return NULL;
    }

  return d_make_builtin_type (di, &d_builtin_types[i]);
}

static struct d_comp *
d_op_from_string (const char *opname)
{
  const struct d_operator_info *i = d_operators;

  while (1)
    {
      if (strcmp (i->name, opname) == 0)
	break;
      i++;
    }
  return d_make_operator (di, i);
}

static struct d_comp *
d_unary (const char *name, struct d_comp *lhs)
{
  return d_make_comp (di, D_COMP_UNARY, d_op_from_string (name), lhs);
}

static struct d_comp *
d_binary (const char *name, struct d_comp *lhs, struct d_comp *rhs)
{
  return d_make_comp (di, D_COMP_BINARY, d_op_from_string (name),
		      d_make_comp (di, D_COMP_BINARY_ARGS, lhs, rhs));
}

static const char *
target_charset (void)
{
  return "foo";
}

static const char *
host_charset (void)
{
  return "bar";
}

/* Take care of parsing a number (anything that starts with a digit).
   Set yylval and return the token type; update lexptr.
   LEN is the number of characters in it.  */

/*** Needs some error checking for the float case ***/

static int
parse_number (p, len, parsed_float, putithere)
     char *p;
     int len;
     int parsed_float;
     YYSTYPE *putithere;
{
  /* FIXME: Shouldn't these be unsigned?  We don't deal with negative values
     here, and we do kind of silly things like cast to unsigned.  */
  LONGEST n = 0;
  LONGEST prevn = 0;
  ULONGEST un;

  int i = 0;
  int c;
  int base = 10;
  int unsigned_p = 0;

  /* Number of "L" suffixes encountered.  */
  int long_p = 0;

  /* We have found a "L" or "U" suffix.  */
  int found_suffix = 0;

  ULONGEST high_bit;
  struct d_comp *signed_type;
  struct d_comp *unsigned_type;

  if (parsed_float)
    {
      /* It's a float since it contains a point or an exponent.  */
      char c;
      int num = 0;	/* number of tokens scanned by scanf */
      char saved_char = p[len];

      p[len] = 0;	/* null-terminate the token */
      if (sizeof (putithere->typed_val_float.dval) <= sizeof (float))
	num = sscanf (p, "%g%c", (float *) &putithere->typed_val_float.dval,&c);
      else if (sizeof (putithere->typed_val_float.dval) <= sizeof (double))
	num = sscanf (p, "%lg%c", (double *) &putithere->typed_val_float.dval,&c);
      else
	{
#ifdef SCANF_HAS_LONG_DOUBLE
	  num = sscanf (p, "%Lg%c", &putithere->typed_val_float.dval,&c);
#else
	  /* Scan it into a double, then assign it to the long double.
	     This at least wins with values representable in the range
	     of doubles. */
	  double temp;
	  num = sscanf (p, "%lg%c", &temp,&c);
	  putithere->typed_val_float.dval = temp;
#endif
	}
      p[len] = saved_char;	/* restore the input stream */
      if (num != 1) 		/* check scanf found ONLY a float ... */
	return ERROR;
      /* See if it has `f' or `l' suffix (float or long double).  */

      c = TOLOWER (p[len - 1]);

#if 0
      if (c == 'f')
	putithere->typed_val_float.type = builtin_type_float;
      else if (c == 'l')
	putithere->typed_val_float.type = builtin_type_long_double;
      else if (isdigit (c) || c == '.')
	putithere->typed_val_float.type = builtin_type_double;
      else
	return ERROR;
#endif

      return FLOAT;
    }

  /* Handle base-switching prefixes 0x, 0t, 0d, 0 */
  if (p[0] == '0')
    switch (p[1])
      {
      case 'x':
      case 'X':
	if (len >= 3)
	  {
	    p += 2;
	    base = 16;
	    len -= 2;
	  }
	break;

      case 't':
      case 'T':
      case 'd':
      case 'D':
	if (len >= 3)
	  {
	    p += 2;
	    base = 10;
	    len -= 2;
	  }
	break;

      default:
	base = 8;
	break;
      }

  while (len-- > 0)
    {
      c = *p++;
      if (c >= 'A' && c <= 'Z')
	c += 'a' - 'A';
      if (c != 'l' && c != 'u')
	n *= base;
      if (c >= '0' && c <= '9')
	{
	  if (found_suffix)
	    return ERROR;
	  n += i = c - '0';
	}
      else
	{
	  if (base > 10 && c >= 'a' && c <= 'f')
	    {
	      if (found_suffix)
		return ERROR;
	      n += i = c - 'a' + 10;
	    }
	  else if (c == 'l')
	    {
	      ++long_p;
	      found_suffix = 1;
	    }
	  else if (c == 'u')
	    {
	      unsigned_p = 1;
	      found_suffix = 1;
	    }
	  else
	    return ERROR;	/* Char not a digit */
	}
      if (i >= base)
	return ERROR;		/* Invalid digit in this base */

      /* Portably test for overflow (only works for nonzero values, so make
	 a second check for zero).  FIXME: Can't we just make n and prevn
	 unsigned and avoid this?  */
      if (c != 'l' && c != 'u' && (prevn >= n) && n != 0)
	unsigned_p = 1;		/* Try something unsigned */

      /* Portably test for unsigned overflow.
	 FIXME: This check is wrong; for example it doesn't find overflow
	 on 0x123456789 when LONGEST is 32 bits.  */
      if (c != 'l' && c != 'u' && n != 0)
	{	
	  if ((unsigned_p && (ULONGEST) prevn >= (ULONGEST) n))
	    error ("Numeric constant too large.");
	}
      prevn = n;
    }

  /* An integer constant is an int, a long, or a long long.  An L
     suffix forces it to be long; an LL suffix forces it to be long
     long.  If not forced to a larger size, it gets the first type of
     the above that it fits in.  To figure out whether it fits, we
     shift it right and see whether anything remains.  Note that we
     can't shift sizeof (LONGEST) * HOST_CHAR_BIT bits or more in one
     operation, because many compilers will warn about such a shift
     (which always produces a zero result).  Sometimes TARGET_INT_BIT
     or TARGET_LONG_BIT will be that big, sometimes not.  To deal with
     the case where it is we just always shift the value more than
     once, with fewer bits each time.  */

  un = (ULONGEST)n >> 2;
  if (long_p == 0
      && (un >> (TARGET_INT_BIT - 2)) == 0)
    {
      high_bit = ((ULONGEST)1) << (TARGET_INT_BIT-1);

      /* A large decimal (not hex or octal) constant (between INT_MAX
	 and UINT_MAX) is a long or unsigned long, according to ANSI,
	 never an unsigned int, but this code treats it as unsigned
	 int.  This probably should be fixed.  GCC gives a warning on
	 such constants.  */

      unsigned_type = d_int_type ('j' - 'a');
      signed_type = d_int_type ('i' - 'a');
    }
  else if (long_p <= 1
	   && (un >> (TARGET_LONG_BIT - 2)) == 0)
    {
      high_bit = ((ULONGEST)1) << (TARGET_LONG_BIT-1);
      unsigned_type = d_int_type ('m' - 'a');
      signed_type = d_int_type ('l' - 'a');
    }
  else
    {
      int shift;
      if (sizeof (ULONGEST) * HOST_CHAR_BIT < TARGET_LONG_LONG_BIT)
	/* A long long does not fit in a LONGEST.  */
	shift = (sizeof (ULONGEST) * HOST_CHAR_BIT - 1);
      else
	shift = (TARGET_LONG_LONG_BIT - 1);
      high_bit = (ULONGEST) 1 << shift;
      unsigned_type = d_int_type ('x' - 'a');
      signed_type = d_int_type ('y' - 'a');
    }

   putithere->typed_val_int.val = n;

   /* If the high bit of the worked out type is set then this number
      has to be unsigned. */

   if (unsigned_p || (n & high_bit)) 
     putithere->typed_val_int.type = unsigned_type;
   else
     putithere->typed_val_int.type = signed_type;

   return INT;
}

/* Print an error message saying that we couldn't make sense of a
   \^mumble sequence in a string or character constant.  START and END
   indicate a substring of some larger string that contains the
   erroneous backslash sequence, missing the initial backslash.  */
static NORETURN int
no_control_char_error (const char *start, const char *end)
{
  int len = end - start;
  char *copy = alloca (end - start + 1);

  memcpy (copy, start, len);
  copy[len] = '\0';

  error ("There is no control character `\\%s' in the `%s' character set.",
	 copy, target_charset ());
  return 0;
}

static int
target_char_to_control_char (int c, int *ctrl_char)
{
  *ctrl_char = (c & 037);
  return 1;
}

static int
host_char_to_target (int c, int *ctrl_char)
{
  *ctrl_char = c;
  return 1;
}

static char backslashable[] = "abefnrtv";
static char represented[] = "\a\b\e\f\n\r\t\v";

/* Translate the backslash the way we would in the host character set.  */
static int
c_parse_backslash (int host_char, int *target_char)
{
  const char *ix;
  ix = strchr (backslashable, host_char);
  if (! ix)
    return 0;
  else
    *target_char = represented[ix - backslashable];
  return 1;
}

/* Parse a C escape sequence.  STRING_PTR points to a variable
   containing a pointer to the string to parse.  That pointer
   should point to the character after the \.  That pointer
   is updated past the characters we use.  The value of the
   escape sequence is returned.

   A negative value means the sequence \ newline was seen,
   which is supposed to be equivalent to nothing at all.

   If \ is followed by a null character, we return a negative
   value and leave the string pointer pointing at the null character.

   If \ is followed by 000, we return 0 and leave the string pointer
   after the zeros.  A value of 0 does not mean end of string.  */

static int
parse_escape (char **string_ptr)
{
  int target_char;
  int c = *(*string_ptr)++;
  if (c_parse_backslash (c, &target_char))
    return target_char;
  else
    switch (c)
      {
      case '\n':
	return -2;
      case 0:
	(*string_ptr)--;
	return 0;
      case '^':
	{
	  /* Remember where this escape sequence started, for reporting
	     errors.  */
	  char *sequence_start_pos = *string_ptr - 1;

	  c = *(*string_ptr)++;

	  if (c == '?')
	    {
	      /* XXXCHARSET: What is `delete' in the host character set?  */
	      c = 0177;

	      if (!host_char_to_target (c, &target_char))
		error ("There is no character corresponding to `Delete' "
		       "in the target character set `%s'.", host_charset ());

	      return target_char;
	    }
	  else if (c == '\\')
	    target_char = parse_escape (string_ptr);
	  else
	    {
	      if (!host_char_to_target (c, &target_char))
		no_control_char_error (sequence_start_pos, *string_ptr);
	    }

	  /* Now target_char is something like `c', and we want to find
	     its control-character equivalent.  */
	  if (!target_char_to_control_char (target_char, &target_char))
	    no_control_char_error (sequence_start_pos, *string_ptr);

	  return target_char;
	}

	/* XXXCHARSET: we need to use isdigit and value-of-digit
	   methods of the host character set here.  */

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
	{
	  int i = c - '0';
	  int count = 0;
	  while (++count < 3)
	    {
	      c = (**string_ptr);
	      if (c >= '0' && c <= '7')
		{
		  (*string_ptr)++;
		  i *= 8;
		  i += c - '0';
		}
	      else
		{
		  break;
		}
	    }
	  return i;
	}
      default:
	if (!host_char_to_target (c, &target_char))
	  error
	    ("The escape sequence `\%c' is equivalent to plain `%c', which"
	     " has no equivalent\n" "in the `%s' character set.", c, c,
	     target_charset ());
	return target_char;
      }
}

struct token
{
  char *operator;
  int token;
  int opcode;
};

static const struct token tokentab3[] =
  {
    {">>=", ASSIGN_MODIFY, BINOP_RSH},
    {"<<=", ASSIGN_MODIFY, BINOP_LSH},
  };

static const struct token tokentab2[] =
  {
    {"+=", ASSIGN_MODIFY, BINOP_ADD},
    {"-=", ASSIGN_MODIFY, BINOP_SUB},
    {"*=", ASSIGN_MODIFY, BINOP_MUL},
    {"/=", ASSIGN_MODIFY, BINOP_DIV},
    {"%=", ASSIGN_MODIFY, BINOP_REM},
    {"|=", ASSIGN_MODIFY, BINOP_BITWISE_IOR},
    {"&=", ASSIGN_MODIFY, BINOP_BITWISE_AND},
    {"^=", ASSIGN_MODIFY, BINOP_BITWISE_XOR},
    {"++", INCREMENT, BINOP_END},
    {"--", DECREMENT, BINOP_END},
    {"->", ARROW, BINOP_END},
    {"&&", ANDAND, BINOP_END},
    {"||", OROR, BINOP_END},
    {"::", COLONCOLON, BINOP_END},
    {"<<", LSH, BINOP_END},
    {">>", RSH, BINOP_END},
    {"==", EQUAL, BINOP_END},
    {"!=", NOTEQUAL, BINOP_END},
    {"<=", LEQ, BINOP_END},
    {">=", GEQ, BINOP_END}
  };

/* Read one token, getting characters through lexptr.  */

static int
yylex ()
{
  int c;
  int namelen;
  unsigned int i;
  char *tokstart;
  char *tokptr;
  int tempbufindex;
  static char *tempbuf;
  static int tempbufsize;
  struct symbol * sym_class = NULL;
  char * token_string = NULL;
  int class_prefix = 0;
  int unquoted_expr;
   
 retry:

  prev_lexptr = lexptr;
  unquoted_expr = 1;

  tokstart = lexptr;
  /* See if it is a special token of length 3.  */
  for (i = 0; i < sizeof tokentab3 / sizeof tokentab3[0]; i++)
    if (strncmp (tokstart, tokentab3[i].operator, 3) == 0)
      {
	lexptr += 3;
	yylval.opname = tokentab3[i].operator;
	return tokentab3[i].token;
      }

  /* See if it is a special token of length 2.  */
  for (i = 0; i < sizeof tokentab2 / sizeof tokentab2[0]; i++)
    if (strncmp (tokstart, tokentab2[i].operator, 2) == 0)
      {
	lexptr += 2;
	yylval.opname = tokentab2[i].operator;
	return tokentab2[i].token;
      }

  switch (c = *tokstart)
    {
    case 0:
      return 0;

    case ' ':
    case '\t':
    case '\n':
      lexptr++;
      goto retry;

    case '\'':
      /* We either have a character constant ('0' or '\177' for example)
	 or we have a quoted symbol reference ('foo(int,int)' in C++
	 for example). */
      lexptr++;
      c = *lexptr++;
      if (c == '\\')
	c = parse_escape (&lexptr);
      else if (c == '\'')
	error ("Empty character constant.");
      else if (! host_char_to_target (c, &c))
        {
          int toklen = lexptr - tokstart + 1;
          char *tok = alloca (toklen + 1);
          memcpy (tok, tokstart, toklen);
          tok[toklen] = '\0';
          error ("There is no character corresponding to %s in the target "
                 "character set `%s'.", tok, target_charset ());
        }

      yylval.typed_val_int.val = c;
      yylval.typed_val_int.type = d_int_type ('c' - 'a');

      c = *lexptr++;
      if (c != '\'')
	error ("Invalid character constant.");

      return INT;

    case '(':
    case ')':
    case ',':
      lexptr++;
      return c;

    case '.':
      if (lexptr[1] == '.' && lexptr[2] == '.')
	{
	  lexptr += 3;
	  return ELLIPSIS;
	}

      /* Might be a floating point number.  */
      if (lexptr[1] < '0' || lexptr[1] > '9')
	goto symbol;		/* Nope, must be a symbol. */
      /* FALL THRU into number case.  */

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      {
	/* It's a number.  */
	int got_dot = 0, got_e = 0, toktype;
	char *p = tokstart;
	int hex = 0;

	if (c == '0' && (p[1] == 'x' || p[1] == 'X'))
	  {
	    p += 2;
	    hex = 1;
	  }
	else if (c == '0' && (p[1]=='t' || p[1]=='T' || p[1]=='d' || p[1]=='D'))
	  {
	    p += 2;
	    hex = 0;
	  }

	for (;; ++p)
	  {
	    /* This test includes !hex because 'e' is a valid hex digit
	       and thus does not indicate a floating point number when
	       the radix is hex.  */
	    if (!hex && !got_e && (*p == 'e' || *p == 'E'))
	      got_dot = got_e = 1;
	    /* This test does not include !hex, because a '.' always indicates
	       a decimal floating point number regardless of the radix.  */
	    /* drow: Is that true in C99? */
	    else if (!got_dot && *p == '.')
	      got_dot = 1;
	    else if (got_e && (p[-1] == 'e' || p[-1] == 'E')
		     && (*p == '-' || *p == '+'))
	      /* This is the sign of the exponent, not the end of the
		 number.  */
	      continue;
	    /* We will take any letters or digits.  parse_number will
	       complain if past the radix, or if L or U are not final.  */
	    else if ((*p < '0' || *p > '9')
		     && ((*p < 'a' || *p > 'z')
				  && (*p < 'A' || *p > 'Z')))
	      break;
	  }
	toktype = parse_number (tokstart, p - tokstart, got_dot|got_e, &yylval);
        if (toktype == ERROR)
	  {
	    char *err_copy = (char *) alloca (p - tokstart + 1);

	    memcpy (err_copy, tokstart, p - tokstart);
	    err_copy[p - tokstart] = 0;
	    error ("Invalid number \"%s\".", err_copy);
	  }
	lexptr = p;
	return toktype;
      }

    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '|':
    case '&':
    case '^':
    case '~':
    case '!':
    case '@':
    case '<':
    case '>':
    case '[':
    case ']':
    case '?':
    case ':':
    case '=':
    case '{':
    case '}':
    symbol:
      lexptr++;
      return c;

    case '"':

      /* Build the gdb internal form of the input string in tempbuf,
	 translating any standard C escape forms seen.  Note that the
	 buffer is null byte terminated *only* for the convenience of
	 debugging gdb itself and printing the buffer contents when
	 the buffer contains no embedded nulls.  Gdb does not depend
	 upon the buffer being null byte terminated, it uses the length
	 string instead.  This allows gdb to handle C strings (as well
	 as strings in other languages) with embedded null bytes */

      tokptr = ++tokstart;
      tempbufindex = 0;

      do {
        char *char_start_pos = tokptr;

	/* Grow the static temp buffer if necessary, including allocating
	   the first one on demand. */
	if (tempbufindex + 1 >= tempbufsize)
	  {
	    tempbuf = (char *) realloc (tempbuf, tempbufsize += 64);
	  }
	switch (*tokptr)
	  {
	  case '\0':
	  case '"':
	    /* Do nothing, loop will terminate. */
	    break;
	  case '\\':
	    tokptr++;
	    c = parse_escape (&tokptr);
	    if (c == -1)
	      {
		continue;
	      }
	    tempbuf[tempbufindex++] = c;
	    break;
	  default:
	    c = *tokptr++;
            if (! host_char_to_target (c, &c))
              {
                int len = tokptr - char_start_pos;
                char *copy = alloca (len + 1);
                memcpy (copy, char_start_pos, len);
                copy[len] = '\0';

                error ("There is no character corresponding to `%s' "
                       "in the target character set `%s'.",
                       copy, target_charset ());
              }
            tempbuf[tempbufindex++] = c;
	    break;
	  }
      } while ((*tokptr != '"') && (*tokptr != '\0'));
      if (*tokptr++ != '"')
	{
	  error ("Unterminated string in expression.");
	}
      tempbuf[tempbufindex] = '\0';	/* See note above */
#if 1
      free (tempbuf);
      error ("Unexpected string literal.");
#else
      yylval.sval.ptr = tempbuf;
      yylval.sval.length = tempbufindex;
      lexptr = tokptr;
      return (STRING);
#endif
    }

  if (!(c == '_' || c == '$'
	|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
    /* We must have come across a bad character (e.g. ';').  */
    error ("Invalid character '%c' in expression.", c);

  /* It's a name.  See how long it is.  */
  namelen = 0;
  for (c = tokstart[namelen];
       (c == '_' || c == '$' || (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));)
    c = tokstart[++namelen];

  lexptr += namelen;

  tryname:

  /* Catch specific keywords.  Should be done with a data structure.  */
  switch (namelen)
    {
    case 16:
      if (strncmp (tokstart, "reinterpret_cast", 16) == 0)
        return REINTERPRET_CAST;
      break;
    case 12:
      if (strncmp (tokstart, "dynamic_cast", 12) == 0)
        return DYNAMIC_CAST;
      break;
    case 11:
      if (strncmp (tokstart, "static_cast", 11) == 0)
        return STATIC_CAST;
      break;
    case 8:
      if (strncmp (tokstart, "operator", 8) == 0)
	return OPERATOR;
      if (strncmp (tokstart, "restrict", 8) == 0)
	return RESTRICT;
      if (strncmp (tokstart, "unsigned", 8) == 0)
	return UNSIGNED;
      if (strncmp (tokstart, "template", 8) == 0)
	return TEMPLATE;
      if (strncmp (tokstart, "volatile", 8) == 0)
	return VOLATILE_KEYWORD;
      break;
    case 7:
      if (strncmp (tokstart, "wchar_t", 7) == 0)
	return WCHAR_T;
      break;
    case 6:
      if (strncmp (tokstart, "delete", 6) == 0)
	return DELETE;
      if (strncmp (tokstart, "struct", 6) == 0)
	return STRUCT;
      if (strncmp (tokstart, "signed", 6) == 0)
	return SIGNED_KEYWORD;
      if (strncmp (tokstart, "sizeof", 6) == 0)
	return SIZEOF;
      if (strncmp (tokstart, "double", 6) == 0)
	return DOUBLE_KEYWORD;
      break;
    case 5:
      if (strncmp (tokstart, "false", 5) == 0)
	return FALSEKEYWORD;
      if (strncmp (tokstart, "class", 5) == 0)
	return CLASS;
      if (strncmp (tokstart, "union", 5) == 0)
	return UNION;
      if (strncmp (tokstart, "float", 5) == 0)
	return FLOAT_KEYWORD;
      if (strncmp (tokstart, "short", 5) == 0)
	return SHORT;
      if (strncmp (tokstart, "const", 5) == 0)
	return CONST_KEYWORD;
      break;
    case 4:
      if (strncmp (tokstart, "void", 4) == 0)
	return VOID;
      if (strncmp (tokstart, "bool", 4) == 0)
	return BOOL;
      if (strncmp (tokstart, "char", 4) == 0)
	return BOOL;
      if (strncmp (tokstart, "enum", 4) == 0)
	return ENUM;
      if (strncmp (tokstart, "long", 4) == 0)
	return LONG;
      if (strncmp (tokstart, "true", 4) == 0)
	return TRUEKEYWORD;
      break;
    case 3:
      if (strncmp (tokstart, "new", 3) == 0)
	return NEW;
      if (strncmp (tokstart, "int", 3) == 0)
	return INT_KEYWORD;
      break;
    default:
      break;
    }

  yylval.comp = d_make_name (di, tokstart, namelen);
  return NAME;
}

void
yyerror (msg)
     char *msg;
{
  if (prev_lexptr)
    lexptr = prev_lexptr;

  error ("A %s in expression, near `%s'.", (msg ? msg : "error"), lexptr);
}

#ifdef TEST_CPNAMES

int main(int argc, char **argv)
{
  struct d_info myinfo;
  int err = 0;
  char *str;

  lexptr = argv[1];
  d_init_info (NULL, DMGL_PARAMS | DMGL_ANSI, 2 * strlen (lexptr), &myinfo);
  di = &myinfo;
  yyparse ();
  str = d_print (DMGL_PARAMS | DMGL_ANSI, result, &err);
  printf ("Result is %s\n", str);
  return 0;
}

#endif
