/* YACC grammar for Chill expressions, for GDB.
   Copyright 1992, 1993, 1994 Free Software Foundation, Inc.

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

/* Parse a Chill expression from text in a string,
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
   times by the parser generator.

   Also note that the language accepted by this parser is more liberal
   than the one accepted by an actual Chill compiler.  For example, the
   language rule that a simple name string can not be one of the reserved
   simple name strings is not enforced (e.g "case" is not treated as a
   reserved name).  Another example is that Chill is a strongly typed
   language, and certain expressions that violate the type constraints
   may still be evaluated if gdb can do so in a meaningful manner, while
   such expressions would be rejected by the compiler.  The reason for
   this more liberal behavior is the philosophy that the debugger
   is intended to be a tool that is used by the programmer when things
   go wrong, and as such, it should provide as few artificial barriers
   to it's use as possible.  If it can do something meaningful, even
   something that violates language contraints that are enforced by the
   compiler, it should do so without complaint.

 */
   
%{

#include "defs.h"
#include <string.h>
#include <ctype.h>
#include "expression.h"
#include "language.h"
#include "value.h"
#include "parser-defs.h"
#include "ch-lang.h"
#include "bfd.h" /* Required by objfiles.h.  */
#include "symfile.h" /* Required by objfiles.h.  */
#include "objfiles.h" /* For have_full_symbols and have_partial_symbols */

/* Remap normal yacc parser interface names (yyparse, yylex, yyerror, etc),
   as well as gratuitiously global symbol names, so we can have multiple
   yacc generated parsers in gdb.  Note that these are only the variables
   produced by yacc.  If other parser generators (bison, byacc, etc) produce
   additional global names that conflict at link time, then those parser
   generators need to be fixed instead of adding those names to this list. */

#define	yymaxdepth chill_maxdepth
#define	yyparse	chill_parse
#define	yylex	chill_lex
#define	yyerror	chill_error
#define	yylval	chill_lval
#define	yychar	chill_char
#define	yydebug	chill_debug
#define	yypact	chill_pact
#define	yyr1	chill_r1
#define	yyr2	chill_r2
#define	yydef	chill_def
#define	yychk	chill_chk
#define	yypgo	chill_pgo
#define	yyact	chill_act
#define	yyexca	chill_exca
#define	yyerrflag chill_errflag
#define	yynerrs	chill_nerrs
#define	yyps	chill_ps
#define	yypv	chill_pv
#define	yys	chill_s
#define	yy_yys	chill_yys
#define	yystate	chill_state
#define	yytmp	chill_tmp
#define	yyv	chill_v
#define	yy_yyv	chill_yyv
#define	yyval	chill_val
#define	yylloc	chill_lloc
#define	yyreds	chill_reds		/* With YYDEBUG defined */
#define	yytoks	chill_toks		/* With YYDEBUG defined */
#define yylhs	chill_yylhs
#define yylen	chill_yylen
#define yydefred chill_yydefred
#define yydgoto	chill_yydgoto
#define yysindex chill_yysindex
#define yyrindex chill_yyrindex
#define yygindex chill_yygindex
#define yytable	 chill_yytable
#define yycheck	 chill_yycheck

#ifndef YYDEBUG
#define	YYDEBUG	0		/* Default to no yydebug support */
#endif

static void
write_lower_upper_value PARAMS ((enum exp_opcode, struct type *type));

int
yyparse PARAMS ((void));

static int
yylex PARAMS ((void));

void
yyerror PARAMS ((char *));

%}

/* Although the yacc "value" of an expression is not used,
   since the result is stored in the structure being created,
   other node types do have values.  */

%union
  {
    LONGEST lval;
    unsigned LONGEST ulval;
    struct {
      LONGEST val;
      struct type *type;
    } typed_val;
    double dval;
    struct symbol *sym;
    struct type *tval;
    struct stoken sval;
    struct ttype tsym;
    struct symtoken ssym;
    int voidval;
    struct block *bval;
    enum exp_opcode opcode;
    struct internalvar *ivar;

    struct type **tvec;
    int *ivec;
  }

%token <typed_val>	INTEGER_LITERAL
%token <ulval>		BOOLEAN_LITERAL
%token <typed_val>	CHARACTER_LITERAL
%token <dval>		FLOAT_LITERAL
%token <ssym>		GENERAL_PROCEDURE_NAME
%token <ssym>		LOCATION_NAME
%token <voidval>	EMPTINESS_LITERAL
%token <sval>		CHARACTER_STRING_LITERAL
%token <sval>		BIT_STRING_LITERAL
%token <tsym>		TYPENAME
%token <sval>		FIELD_NAME

%token <voidval>	'.'
%token <voidval>	';'
%token <voidval>	':'
%token <voidval>	CASE
%token <voidval>	OF
%token <voidval>	ESAC
%token <voidval>	LOGIOR
%token <voidval>	ORIF
%token <voidval>	LOGXOR
%token <voidval>	LOGAND
%token <voidval>	ANDIF
%token <voidval>	'='
%token <voidval>	NOTEQUAL
%token <voidval>	'>'
%token <voidval>	GTR
%token <voidval>	'<'
%token <voidval>	LEQ
%token <voidval>	IN
%token <voidval>	'+'
%token <voidval>	'-'
%token <voidval>	'*'
%token <voidval>	'/'
%token <voidval>	SLASH_SLASH
%token <voidval>	MOD
%token <voidval>	REM
%token <voidval>	NOT
%token <voidval>	POINTER
%token <voidval>	RECEIVE
%token <voidval>	'['
%token <voidval>	']'
%token <voidval>	'('
%token <voidval>	')'
%token <voidval>	UP
%token <voidval>	IF
%token <voidval>	THEN
%token <voidval>	ELSE
%token <voidval>	FI
%token <voidval>	ELSIF
%token <voidval>	ILLEGAL_TOKEN
%token <voidval>	NUM
%token <voidval>	PRED
%token <voidval>	SUCC
%token <voidval>	ABS
%token <voidval>	CARD
%token <voidval>	MAX_TOKEN
%token <voidval>	MIN_TOKEN
%token <voidval>	ADDR_TOKEN
%token <voidval>	SIZE
%token <voidval>	UPPER
%token <voidval>	LOWER
%token <voidval>	LENGTH
%token <voidval>	ARRAY

/* Tokens which are not Chill tokens used in expressions, but rather GDB
   specific things that we recognize in the same context as Chill tokens
   (register names for example). */

%token <voidval>	GDB_VARIABLE	/* Convenience variable */
%token <voidval>	GDB_ASSIGNMENT	/* Assign value to somewhere */

%type <voidval>		access_name
%type <voidval>		primitive_value
%type <voidval>		value_name
%type <voidval>		literal
%type <voidval>		tuple
%type <voidval>		slice
%type <voidval>		expression_conversion
%type <voidval>		value_built_in_routine_call
%type <voidval>		parenthesised_expression
%type <voidval>		value
%type <voidval>		expression
%type <voidval>		conditional_expression
%type <voidval>		then_alternative
%type <voidval>		else_alternative
%type <voidval>		operand_0
%type <voidval>		operand_1
%type <voidval>		operand_2
%type <voidval>		operand_3
%type <voidval>		operand_4
%type <voidval>		operand_5
%type <voidval>		operand_6
%type <voidval>		expression_list
%type <tval>		mode_argument
%type <voidval>		single_assignment_action
%type <tsym>		mode_name
%type <lval>		rparen

/* Not implemented:
%type <voidval>		undefined_value
%type <voidval>		array_mode_name
%type <voidval>		string_mode_name
%type <voidval>		variant_structure_mode_name
*/

%%

/* Z.200, 5.3.1 */

start	:	value { }
	|	mode_name
			{ write_exp_elt_opcode(OP_TYPE);
			  write_exp_elt_type($1.type);
			  write_exp_elt_opcode(OP_TYPE);}
	;

value		:	expression
/*
		|	undefined_value
			{ ??? }
*/
		;

/* Z.200, 4.2.2 */

access_name	:	LOCATION_NAME
			{
			  write_exp_elt_opcode (OP_VAR_VALUE);
			  write_exp_elt_block (NULL);
			  write_exp_elt_sym ($1.sym);
			  write_exp_elt_opcode (OP_VAR_VALUE);
			}
		|	GDB_VARIABLE	/* gdb specific */
		;

/* Z.200, 4.2.8 */

expression_list	:	expression
			{
			  arglist_len = 1;
			}
		|	expression_list ',' expression
			{
			  arglist_len++;
			}
		;

maybe_expression_list:	/* EMPTY */
			{
			  arglist_len = 0;
			}
		|	expression_list
		;


/* Z.200, 5.2.1 */

primitive_value_lparen: primitive_value '('
				/* This is to save the value of arglist_len
				   being accumulated for each dimension. */
				{ start_arglist (); }
		;

rparen		:	')'
				{ $$ = end_arglist (); }
		;

primitive_value	:
			access_name
		|	primitive_value_lparen maybe_expression_list rparen
			{
			  write_exp_elt_opcode (MULTI_SUBSCRIPT);
			  write_exp_elt_longcst ($3);
			  write_exp_elt_opcode (MULTI_SUBSCRIPT);
			}
		|	primitive_value FIELD_NAME
			{ write_exp_elt_opcode (STRUCTOP_STRUCT);
			  write_exp_string ($2);
			  write_exp_elt_opcode (STRUCTOP_STRUCT);
			}
  		|	primitive_value POINTER
			{
			  write_exp_elt_opcode (UNOP_IND);
			}
  		|	primitive_value POINTER mode_name
			{
			  write_exp_elt_opcode (UNOP_CAST);
			  write_exp_elt_type (lookup_pointer_type ($3.type));
			  write_exp_elt_opcode (UNOP_CAST);
			  write_exp_elt_opcode (UNOP_IND);
			}
                |	value_name
                |	literal
                |	tuple
                |	slice
                |	expression_conversion
                |	value_built_in_routine_call
/*
                |	start_expression
			{ ??? }
                |	zero_adic_operator
			{ ??? }
*/
                |	parenthesised_expression
		;

/* Z.200, 5.2.3 */

value_name	:	GENERAL_PROCEDURE_NAME
			{
			  write_exp_elt_opcode (OP_VAR_VALUE);
			  write_exp_elt_block (NULL);
			  write_exp_elt_sym ($1.sym);
			  write_exp_elt_opcode (OP_VAR_VALUE);
			}
		;

/* Z.200, 5.2.4.1 */

literal		:	INTEGER_LITERAL
			{
			  write_exp_elt_opcode (OP_LONG);
			  write_exp_elt_type ($1.type);
			  write_exp_elt_longcst ((LONGEST) ($1.val));
			  write_exp_elt_opcode (OP_LONG);
			}
		|	BOOLEAN_LITERAL
			{
			  write_exp_elt_opcode (OP_BOOL);
			  write_exp_elt_longcst ((LONGEST) $1);
			  write_exp_elt_opcode (OP_BOOL);
			}
		|	CHARACTER_LITERAL
			{
			  write_exp_elt_opcode (OP_LONG);
			  write_exp_elt_type ($1.type);
			  write_exp_elt_longcst ((LONGEST) ($1.val));
			  write_exp_elt_opcode (OP_LONG);
			}
		|	FLOAT_LITERAL
			{
			  write_exp_elt_opcode (OP_DOUBLE);
			  write_exp_elt_type (builtin_type_double);
			  write_exp_elt_dblcst ($1);
			  write_exp_elt_opcode (OP_DOUBLE);
			}
		|	EMPTINESS_LITERAL
			{
			  struct type *void_ptr_type
			    = lookup_pointer_type (builtin_type_void);
			  write_exp_elt_opcode (OP_LONG);
			  write_exp_elt_type (void_ptr_type);
			  write_exp_elt_longcst (0);
			  write_exp_elt_opcode (OP_LONG);
			}
		|	CHARACTER_STRING_LITERAL
			{
			  write_exp_elt_opcode (OP_STRING);
			  write_exp_string ($1);
			  write_exp_elt_opcode (OP_STRING);
			}
		|	BIT_STRING_LITERAL
			{
			  write_exp_elt_opcode (OP_BITSTRING);
			  write_exp_bitstring ($1);
			  write_exp_elt_opcode (OP_BITSTRING);
			}
		;

/* Z.200, 5.2.5 */

tuple_element	:	expression
		|	named_record_element
		;

named_record_element:	FIELD_NAME ',' named_record_element
			{ write_exp_elt_opcode (OP_LABELED);
			  write_exp_string ($1);
			  write_exp_elt_opcode (OP_LABELED);
			}
		|	FIELD_NAME ':' expression	
			{ write_exp_elt_opcode (OP_LABELED);
			  write_exp_string ($1);
			  write_exp_elt_opcode (OP_LABELED);
			}
		;

tuple_elements	:	tuple_element
			{
			  arglist_len = 1;
			}
		|	tuple_elements ',' tuple_element
			{
			  arglist_len++;
			}
		;

maybe_tuple_elements :	tuple_elements
		| /* EMPTY */
		;

tuple	:	'['
			{ start_arglist (); }
		maybe_tuple_elements ']'
			{
			  write_exp_elt_opcode (OP_ARRAY);
			  write_exp_elt_longcst ((LONGEST) 0);
			  write_exp_elt_longcst ((LONGEST) end_arglist () - 1);
			  write_exp_elt_opcode (OP_ARRAY);
			}
		|
		mode_name '['
			{ start_arglist (); }
		maybe_tuple_elements ']'
			{
			  write_exp_elt_opcode (OP_ARRAY);
			  write_exp_elt_longcst ((LONGEST) 0);
			  write_exp_elt_longcst ((LONGEST) end_arglist () - 1);
			  write_exp_elt_opcode (OP_ARRAY);

			  write_exp_elt_opcode (UNOP_CAST);
			  write_exp_elt_type ($1.type);
			  write_exp_elt_opcode (UNOP_CAST);
			}
		;


/* Z.200, 5.2.6 */


slice:	primitive_value_lparen expression ':' expression rparen
			{
			  write_exp_elt_opcode (TERNOP_SLICE);
			}
		|	primitive_value_lparen expression UP expression rparen
			{
			  write_exp_elt_opcode (TERNOP_SLICE_COUNT);
			}
		;

/* Z.200, 5.2.11 */

expression_conversion:	mode_name parenthesised_expression
			{
			  write_exp_elt_opcode (UNOP_CAST);
			  write_exp_elt_type ($1.type);
			  write_exp_elt_opcode (UNOP_CAST);
			}
		|	ARRAY '(' ')' mode_name parenthesised_expression
			/* This is pseudo-Chill, similar to C's '(TYPE[])EXPR'
			   which casts to an artificial array. */
			{
			  struct type *range_type
			    = create_range_type ((struct type *) NULL,
						 builtin_type_int, 0, 0);
			  struct type *array_type
			    = create_array_type ((struct type *) NULL,
						 $4.type, range_type);
			  TYPE_ARRAY_UPPER_BOUND_TYPE(array_type)
			    = BOUND_CANNOT_BE_DETERMINED;
			  write_exp_elt_opcode (UNOP_CAST);
			  write_exp_elt_type (array_type);
			  write_exp_elt_opcode (UNOP_CAST);
			}
		;

/* Z.200, 5.2.16 */

parenthesised_expression:	'(' expression ')'
		;

/* Z.200, 5.3.2 */

expression	:	operand_0
		|	single_assignment_action
		|	conditional_expression
		;

conditional_expression : IF expression then_alternative else_alternative FI
			{ write_exp_elt_opcode (TERNOP_COND); }
/*
		|	CASE case_selector_list OF value_case_alternative ELSE expression ESAC
			{ error ("not implemented:  CASE expression" }
*/
		;

then_alternative:	THEN expression
		;

else_alternative:	ELSE expression
		|	ELSIF expression then_alternative else_alternative
			{ write_exp_elt_opcode (TERNOP_COND); }
		;

/* Z.200, 5.3.3 */

operand_0	:	operand_1
		|	operand_0 LOGIOR operand_1
			{
			  write_exp_elt_opcode (BINOP_BITWISE_IOR);
			}
		|	operand_0 ORIF operand_1
			{
			  write_exp_elt_opcode (BINOP_LOGICAL_OR);
			}
		|	operand_0 LOGXOR operand_1
			{
			  write_exp_elt_opcode (BINOP_BITWISE_XOR);
			}
		;

/* Z.200, 5.3.4 */

operand_1	:	operand_2
		|	operand_1 LOGAND operand_2
			{
			  write_exp_elt_opcode (BINOP_BITWISE_AND);
			}
		|	operand_1 ANDIF operand_2
			{
			  write_exp_elt_opcode (BINOP_LOGICAL_AND);
			}
		;

/* Z.200, 5.3.5 */

operand_2	:	operand_3
		|	operand_2 '=' operand_3
			{
			  write_exp_elt_opcode (BINOP_EQUAL);
			}
		|	operand_2 NOTEQUAL operand_3
			{
			  write_exp_elt_opcode (BINOP_NOTEQUAL);
			}
		|	operand_2 '>' operand_3
			{
			  write_exp_elt_opcode (BINOP_GTR);
			}
		|	operand_2 GTR operand_3
			{
			  write_exp_elt_opcode (BINOP_GEQ);
			}
		|	operand_2 '<' operand_3
			{
			  write_exp_elt_opcode (BINOP_LESS);
			}
		|	operand_2 LEQ operand_3
			{
			  write_exp_elt_opcode (BINOP_LEQ);
			}
		|	operand_2 IN operand_3
			{
			  write_exp_elt_opcode (BINOP_IN);
			}
		;


/* Z.200, 5.3.6 */

operand_3	:	operand_4
		|	operand_3 '+' operand_4
			{
			  write_exp_elt_opcode (BINOP_ADD);
			}
		|	operand_3 '-' operand_4
			{
			  write_exp_elt_opcode (BINOP_SUB);
			}
		|	operand_3 SLASH_SLASH operand_4
			{
			  write_exp_elt_opcode (BINOP_CONCAT);
			}
		;

/* Z.200, 5.3.7 */

operand_4	:	operand_5
		|	operand_4 '*' operand_5
			{
			  write_exp_elt_opcode (BINOP_MUL);
			}
		|	operand_4 '/' operand_5
			{
			  write_exp_elt_opcode (BINOP_DIV);
			}
		|	operand_4 MOD operand_5
			{
			  write_exp_elt_opcode (BINOP_MOD);
			}
		|	operand_4 REM operand_5
			{
			  write_exp_elt_opcode (BINOP_REM);
			}
		;

/* Z.200, 5.3.8 */

operand_5	:	operand_6
		|	'-' operand_6
			{
			  write_exp_elt_opcode (UNOP_NEG);
			}
		|	NOT operand_6
			{
			  write_exp_elt_opcode (UNOP_LOGICAL_NOT);
			}
		|	parenthesised_expression literal
/* We require the string operand to be a literal, to avoid some
   nasty parsing ambiguities. */
			{
			  write_exp_elt_opcode (BINOP_CONCAT);
			}
		;

/* Z.200, 5.3.9 */

operand_6	:	POINTER primitive_value
			{
			  write_exp_elt_opcode (UNOP_ADDR);
			}
		|	RECEIVE expression
			{ error ("not implemented:  RECEIVE expression"); }
		|	primitive_value
		;


/* Z.200, 6.2 */

single_assignment_action :
			primitive_value GDB_ASSIGNMENT value
			{
			  write_exp_elt_opcode (BINOP_ASSIGN);
			}
		;

/* Z.200, 6.20.3 */

value_built_in_routine_call :
			NUM '(' expression ')'
			{
			  write_exp_elt_opcode (UNOP_CAST);
			  write_exp_elt_type (builtin_type_int);
			  write_exp_elt_opcode (UNOP_CAST);
			}
		|	PRED '(' expression ')'
			{ error ("not implemented:  PRED builtin function"); }
		|	SUCC '(' expression ')'
			{ error ("not implemented:  SUCC builtin function"); }
		|	ADDR_TOKEN '(' expression ')'
			{ write_exp_elt_opcode (UNOP_ADDR); }
		|	ABS '(' expression ')'
			{ error ("not implemented:  ABS builtin function"); }
		|	CARD '(' expression ')'
			{ error ("not implemented:  CARD builtin function"); }
		|	MAX_TOKEN '(' expression ')'
			{ error ("not implemented:  MAX builtin function"); }
		|	MIN_TOKEN '(' expression ')'
			{ error ("not implemented:  MIN builtin function"); }
		|	SIZE '(' expression ')'
			{ write_exp_elt_opcode (UNOP_SIZEOF); }
		|	SIZE '(' mode_argument ')'
			{ write_exp_elt_opcode (OP_LONG);
			  write_exp_elt_type (builtin_type_int);
			  write_exp_elt_longcst ((LONGEST) TYPE_LENGTH ($3));
			  write_exp_elt_opcode (OP_LONG); }
		|	LOWER '(' mode_argument ')'
			{ write_lower_upper_value (UNOP_LOWER, $3); }
		|	UPPER '(' mode_argument ')'
			{ write_lower_upper_value (UNOP_UPPER, $3); }
		|	LOWER '(' expression ')'
			{ write_exp_elt_opcode (UNOP_LOWER); }
		|	UPPER '(' expression ')'
			{ write_exp_elt_opcode (UNOP_UPPER); }
		|	LENGTH '(' expression ')'
			{ write_exp_elt_opcode (UNOP_LENGTH); }
		;

mode_argument :		mode_name
			{
			  $$ = $1.type;
			}
/*
		|	array_mode_name '(' expression ')'
			{ ??? }
		|	string_mode_name '(' expression ')'
			{ ??? }
		|	variant_structure_mode_name '(' expression_list ')'
			{ ??? }
*/
		;

mode_name :		TYPENAME
		;

%%

/* Implementation of a dynamically expandable buffer for processing input
   characters acquired through lexptr and building a value to return in
   yylval. */

static char *tempbuf;		/* Current buffer contents */
static int tempbufsize;		/* Size of allocated buffer */
static int tempbufindex;	/* Current index into buffer */

#define GROWBY_MIN_SIZE 64	/* Minimum amount to grow buffer by */

#define CHECKBUF(size) \
  do { \
    if (tempbufindex + (size) >= tempbufsize) \
      { \
	growbuf_by_size (size); \
      } \
  } while (0);

/* Grow the static temp buffer if necessary, including allocating the first one
   on demand. */

static void
growbuf_by_size (count)
     int count;
{
  int growby;

  growby = max (count, GROWBY_MIN_SIZE);
  tempbufsize += growby;
  if (tempbuf == NULL)
    {
      tempbuf = (char *) malloc (tempbufsize);
    }
  else
    {
      tempbuf = (char *) realloc (tempbuf, tempbufsize);
    }
}

/* Try to consume a simple name string token.  If successful, returns
   a pointer to a nullbyte terminated copy of the name that can be used
   in symbol table lookups.  If not successful, returns NULL. */

static char *
match_simple_name_string ()
{
  char *tokptr = lexptr;

  if (isalpha (*tokptr) || *tokptr == '_')
    {
      char *result;
      do {
	tokptr++;
      } while (isalnum (*tokptr) || (*tokptr == '_'));
      yylval.sval.ptr = lexptr;
      yylval.sval.length = tokptr - lexptr;
      lexptr = tokptr;
      result = copy_name (yylval.sval);
      return result;
    }
  return (NULL);
}

/* Start looking for a value composed of valid digits as set by the base
   in use.  Note that '_' characters are valid anywhere, in any quantity,
   and are simply ignored.  Since we must find at least one valid digit,
   or reject this token as an integer literal, we keep track of how many
   digits we have encountered. */
  
static int
decode_integer_value (base, tokptrptr, ivalptr)
  int base;
  char **tokptrptr;
  int *ivalptr;
{
  char *tokptr = *tokptrptr;
  int temp;
  int digits = 0;

  while (*tokptr != '\0')
    {
      temp = *tokptr;
      if (isupper (temp))
        temp = tolower (temp);
      tokptr++;
      switch (temp)
	{
	case '_':
	  continue;
	case '0':  case '1':  case '2':  case '3':  case '4':
	case '5':  case '6':  case '7':  case '8':  case '9':
	  temp -= '0';
	  break;
	case 'a':  case 'b':  case 'c':  case 'd':  case 'e': case 'f':
	  temp -= 'a';
	  temp += 10;
	  break;
	default:
	  temp = base;
	  break;
	}
      if (temp < base)
	{
	  digits++;
	  *ivalptr *= base;
	  *ivalptr += temp;
	}
      else
	{
	  /* Found something not in domain for current base. */
	  tokptr--;	/* Unconsume what gave us indigestion. */
	  break;
	}
    }
  
  /* If we didn't find any digits, then we don't have a valid integer
     value, so reject the entire token.  Otherwise, update the lexical
     scan pointer, and return non-zero for success. */
  
  if (digits == 0)
    {
      return (0);
    }
  else
    {
      *tokptrptr = tokptr;
      return (1);
    }
}

static int
decode_integer_literal (valptr, tokptrptr)
  int *valptr;
  char **tokptrptr;
{
  char *tokptr = *tokptrptr;
  int base = 0;
  int ival = 0;
  int explicit_base = 0;
  
  /* Look for an explicit base specifier, which is optional. */
  
  switch (*tokptr)
    {
    case 'd':
    case 'D':
      explicit_base++;
      base = 10;
      tokptr++;
      break;
    case 'b':
    case 'B':
      explicit_base++;
      base = 2;
      tokptr++;
      break;
    case 'h':
    case 'H':
      explicit_base++;
      base = 16;
      tokptr++;
      break;
    case 'o':
    case 'O':
      explicit_base++;
      base = 8;
      tokptr++;
      break;
    default:
      base = 10;
      break;
    }
  
  /* If we found an explicit base ensure that the character after the
     explicit base is a single quote. */
  
  if (explicit_base && (*tokptr++ != '\''))
    {
      return (0);
    }
  
  /* Attempt to decode whatever follows as an integer value in the
     indicated base, updating the token pointer in the process and
     computing the value into ival.  Also, if we have an explicit
     base, then the next character must not be a single quote, or we
     have a bitstring literal, so reject the entire token in this case.
     Otherwise, update the lexical scan pointer, and return non-zero
     for success. */

  if (!decode_integer_value (base, &tokptr, &ival))
    {
      return (0);
    }
  else if (explicit_base && (*tokptr == '\''))
    {
      return (0);
    }
  else
    {
      *valptr = ival;
      *tokptrptr = tokptr;
      return (1);
    }
}

/*  If it wasn't for the fact that floating point values can contain '_'
    characters, we could just let strtod do all the hard work by letting it
    try to consume as much of the current token buffer as possible and
    find a legal conversion.  Unfortunately we need to filter out the '_'
    characters before calling strtod, which we do by copying the other
    legal chars to a local buffer to be converted.  However since we also
    need to keep track of where the last unconsumed character in the input
    buffer is, we have transfer only as many characters as may compose a
    legal floating point value. */
    
static int
match_float_literal ()
{
  char *tokptr = lexptr;
  char *buf;
  char *copy;
  double dval;
  extern double strtod ();
  
  /* Make local buffer in which to build the string to convert.  This is
     required because underscores are valid in chill floating point numbers
     but not in the string passed to strtod to convert.  The string will be
     no longer than our input string. */
     
  copy = buf = (char *) alloca (strlen (tokptr) + 1);

  /* Transfer all leading digits to the conversion buffer, discarding any
     underscores. */

  while (isdigit (*tokptr) || *tokptr == '_')
    {
      if (*tokptr != '_')
	{
	  *copy++ = *tokptr;
	}
      tokptr++;
    }

  /* Now accept either a '.', or one of [eEdD].  Dot is legal regardless
     of whether we found any leading digits, and we simply accept it and
     continue on to look for the fractional part and/or exponent.  One of
     [eEdD] is legal only if we have seen digits, and means that there
     is no fractional part.  If we find neither of these, then this is
     not a floating point number, so return failure. */

  switch (*tokptr++)
    {
      case '.':
        /* Accept and then look for fractional part and/or exponent. */
	*copy++ = '.';
	break;

      case 'e':
      case 'E':
      case 'd':
      case 'D':
	if (copy == buf)
	  {
	    return (0);
	  }
	*copy++ = 'e';
	goto collect_exponent;
	break;

      default:
	return (0);
        break;
    }

  /* We found a '.', copy any fractional digits to the conversion buffer, up
     to the first nondigit, non-underscore character. */

  while (isdigit (*tokptr) || *tokptr == '_')
    {
      if (*tokptr != '_')
	{
	  *copy++ = *tokptr;
	}
      tokptr++;
    }

  /* Look for an exponent, which must start with one of [eEdD].  If none
     is found, jump directly to trying to convert what we have collected
     so far. */

  switch (*tokptr)
    {
      case 'e':
      case 'E':
      case 'd':
      case 'D':
	*copy++ = 'e';
	tokptr++;
	break;
      default:
	goto convert_float;
	break;
    }

  /* Accept an optional '-' or '+' following one of [eEdD]. */

  collect_exponent:
  if (*tokptr == '+' || *tokptr == '-')
    {
      *copy++ = *tokptr++;
    }

  /* Now copy an exponent into the conversion buffer.  Note that at the 
     moment underscores are *not* allowed in exponents. */

  while (isdigit (*tokptr))
    {
      *copy++ = *tokptr++;
    }

  /* If we transfered any chars to the conversion buffer, try to interpret its
     contents as a floating point value.  If any characters remain, then we
     must not have a valid floating point string. */

  convert_float:
  *copy = '\0';
  if (copy != buf)
      {
        dval = strtod (buf, &copy);
        if (*copy == '\0')
	  {
	    yylval.dval = dval;
	    lexptr = tokptr;
	    return (FLOAT_LITERAL);
	  }
      }
  return (0);
}

/* Recognize a string literal.  A string literal is a sequence
   of characters enclosed in matching single or double quotes, except that
   a single character inside single quotes is a character literal, which
   we reject as a string literal.  To embed the terminator character inside
   a string, it is simply doubled (I.E. "this""is""one""string") */

static int
match_string_literal ()
{
  char *tokptr = lexptr;

  for (tempbufindex = 0, tokptr++; *tokptr != '\0'; tokptr++)
    {
      CHECKBUF (1);
      if (*tokptr == *lexptr)
	{
	  if (*(tokptr + 1) == *lexptr)
	    {
	      tokptr++;
	    }
	  else
	    {
	      break;
	    }
	}
      tempbuf[tempbufindex++] = *tokptr;
    }
  if (*tokptr == '\0'					/* no terminator */
      || (tempbufindex == 1 && *tokptr == '\''))	/* char literal */
    {
      return (0);
    }
  else
    {
      tempbuf[tempbufindex] = '\0';
      yylval.sval.ptr = tempbuf;
      yylval.sval.length = tempbufindex;
      lexptr = ++tokptr;
      return (CHARACTER_STRING_LITERAL);
    }
}

/* Recognize a character literal.  A character literal is single character
   or a control sequence, enclosed in single quotes.  A control sequence
   is a comma separated list of one or more integer literals, enclosed
   in parenthesis and introduced with a circumflex character.

   EX:  'a'  '^(7)'  '^(7,8)'

   As a GNU chill extension, the syntax C'xx' is also recognized as a 
   character literal, where xx is a hex value for the character.

   Note that more than a single character, enclosed in single quotes, is
   a string literal.

   Also note that the control sequence form is not in GNU Chill since it
   is ambiguous with the string literal form using single quotes.  I.E.
   is '^(7)' a character literal or a string literal.  In theory it it
   possible to tell by context, but GNU Chill doesn't accept the control
   sequence form, so neither do we (for now the code is disabled).

   Returns CHARACTER_LITERAL if a match is found.
   */

static int
match_character_literal ()
{
  char *tokptr = lexptr;
  int ival = 0;
  
  if ((*tokptr == 'c' || *tokptr == 'C') && (*(tokptr + 1) == '\''))
    {
      /* We have a GNU chill extension form, so skip the leading "C'",
	 decode the hex value, and then ensure that we have a trailing
	 single quote character. */
      tokptr += 2;
      if (!decode_integer_value (16, &tokptr, &ival) || (*tokptr != '\''))
	{
	  return (0);
	}
      tokptr++;
    }
  else if (*tokptr == '\'')
    {
      tokptr++;

      /* Determine which form we have, either a control sequence or the
	 single character form. */
      
      if ((*tokptr == '^') && (*(tokptr + 1) == '('))
	{
#if 0     /* Disable, see note above. -fnf */
	  /* Match and decode a control sequence.  Return zero if we don't
	     find a valid integer literal, or if the next unconsumed character
	     after the integer literal is not the trailing ')'.
	     FIXME:  We currently don't handle the multiple integer literal
	     form. */
	  tokptr += 2;
	  if (!decode_integer_literal (&ival, &tokptr) || (*tokptr++ != ')'))
	    {
	      return (0);
	    }
#else
	  return (0);
#endif
	}
      else
	{
	  ival = *tokptr++;
	}
      
      /* The trailing quote has not yet been consumed.  If we don't find
	 it, then we have no match. */
      
      if (*tokptr++ != '\'')
	{
	  return (0);
	}
    }
  else
    {
      /* Not a character literal. */
      return (0);
    }
  yylval.typed_val.val = ival;
  yylval.typed_val.type = builtin_type_chill_char;
  lexptr = tokptr;
  return (CHARACTER_LITERAL);
}

/* Recognize an integer literal, as specified in Z.200 sec 5.2.4.2.
   Note that according to 5.2.4.2, a single "_" is also a valid integer
   literal, however GNU-chill requires there to be at least one "digit"
   in any integer literal. */

static int
match_integer_literal ()
{
  char *tokptr = lexptr;
  int ival;
  
  if (!decode_integer_literal (&ival, &tokptr))
    {
      return (0);
    }
  else 
    {
      yylval.typed_val.val = ival;
      yylval.typed_val.type = builtin_type_int;
      lexptr = tokptr;
      return (INTEGER_LITERAL);
    }
}

/* Recognize a bit-string literal, as specified in Z.200 sec 5.2.4.8
   Note that according to 5.2.4.8, a single "_" is also a valid bit-string
   literal, however GNU-chill requires there to be at least one "digit"
   in any bit-string literal. */

static int
match_bitstring_literal ()
{
  register char *tokptr = lexptr;
  int bitoffset = 0;
  int bitcount = 0;
  int bits_per_char;
  int digit;
  
  tempbufindex = 0;
  CHECKBUF (1);
  tempbuf[0] = 0;

  /* Look for the required explicit base specifier. */
  
  switch (*tokptr++)
    {
    case 'b':
    case 'B':
      bits_per_char = 1;
      break;
    case 'o':
    case 'O':
      bits_per_char = 3;
      break;
    case 'h':
    case 'H':
      bits_per_char = 4;
      break;
    default:
      return (0);
      break;
    }

  /* Ensure that the character after the explicit base is a single quote. */
  
  if (*tokptr++ != '\'')
    {
      return (0);
    }
  
  while (*tokptr != '\0' && *tokptr != '\'')
    {
      digit = *tokptr;
      if (isupper (digit))
        digit = tolower (digit);
      tokptr++;
      switch (digit)
	{
	  case '_':
	    continue;
	  case '0':  case '1':  case '2':  case '3':  case '4':
	  case '5':  case '6':  case '7':  case '8':  case '9':
	    digit -= '0';
	    break;
	  case 'a':  case 'b':  case 'c':  case 'd':  case 'e': case 'f':
	    digit -= 'a';
	    digit += 10;
	    break;
	  default:
	    return (0);
	    break;
	}
      if (digit >= 1 << bits_per_char)
	{
	  /* Found something not in domain for current base. */
	  return (0);
	}
      else
	{
	  /* Extract bits from digit, packing them into the bitstring byte. */
	  int k = TARGET_BYTE_ORDER == BIG_ENDIAN ? bits_per_char - 1 : 0;
	  for (; TARGET_BYTE_ORDER == BIG_ENDIAN ? k >= 0 : k < bits_per_char;
	       TARGET_BYTE_ORDER == BIG_ENDIAN ? k-- : k++)
	    {
	      bitcount++;
	      if (digit & (1 << k))
		{
		  tempbuf[tempbufindex] |=
		    (TARGET_BYTE_ORDER == BIG_ENDIAN)
		      ? (1 << (HOST_CHAR_BIT - 1 - bitoffset))
			: (1 << bitoffset);
		}
	      bitoffset++;
	      if (bitoffset == HOST_CHAR_BIT)
		{
		  bitoffset = 0;
		  tempbufindex++;
		  CHECKBUF(1);
		  tempbuf[tempbufindex] = 0;
		}
	    }
	}
    }
  
  /* Verify that we consumed everything up to the trailing single quote,
     and that we found some bits (IE not just underbars). */

  if (*tokptr++ != '\'')
    {
      return (0);
    }
  else 
    {
      yylval.sval.ptr = tempbuf;
      yylval.sval.length = bitcount;
      lexptr = tokptr;
      return (BIT_STRING_LITERAL);
    }
}

struct token
{
  char *operator;
  int token;
};

static const struct token idtokentab[] =
{
    { "array", ARRAY },
    { "length", LENGTH },
    { "lower", LOWER },
    { "upper", UPPER },
    { "andif", ANDIF },
    { "pred", PRED },
    { "succ", SUCC },
    { "card", CARD },
    { "size", SIZE },
    { "orif", ORIF },
    { "num", NUM },
    { "abs", ABS },
    { "max", MAX_TOKEN },
    { "min", MIN_TOKEN },
    { "mod", MOD },
    { "rem", REM },
    { "not", NOT },
    { "xor", LOGXOR },
    { "and", LOGAND },
    { "in", IN },
    { "or", LOGIOR },
    { "up", UP },
    { "addr", ADDR_TOKEN },
    { "null", EMPTINESS_LITERAL }
};

static const struct token tokentab2[] =
{
    { ":=", GDB_ASSIGNMENT },
    { "//", SLASH_SLASH },
    { "->", POINTER },
    { "/=", NOTEQUAL },
    { "<=", LEQ },
    { ">=", GTR }
};

/* Read one token, getting characters through lexptr.  */
/* This is where we will check to make sure that the language and the
   operators used are compatible.  */

static int
yylex ()
{
    unsigned int i;
    int token;
    char *inputname;
    struct symbol *sym;

    /* Skip over any leading whitespace. */
    while (isspace (*lexptr))
	{
	    lexptr++;
	}
    /* Look for special single character cases which can't be the first
       character of some other multicharacter token. */
    switch (*lexptr)
	{
	    case '\0':
	        return (0);
	    case ',':
	    case '=':
	    case ';':
	    case '!':
	    case '+':
	    case '*':
	    case '(':
	    case ')':
	    case '[':
	    case ']':
		return (*lexptr++);
	}
    /* Look for characters which start a particular kind of multicharacter
       token, such as a character literal, register name, convenience
       variable name, string literal, etc. */
    switch (*lexptr)
      {
	case '\'':
	case '\"':
	  /* First try to match a string literal, which is any
	     sequence of characters enclosed in matching single or double
	     quotes, except that a single character inside single quotes
	     is a character literal, so we have to catch that case also. */
	  token = match_string_literal ();
	  if (token != 0)
	    {
	      return (token);
	    }
	  if (*lexptr == '\'')
	    {
	      token = match_character_literal ();
	      if (token != 0)
		{
		  return (token);
		}
	    }
	  break;
        case 'C':
        case 'c':
	  token = match_character_literal ();
	  if (token != 0)
	    {
	      return (token);
	    }
	  break;
	case '$':
	  yylval.sval.ptr = lexptr;
	  do {
	    lexptr++;
	  } while (isalnum (*lexptr) || *lexptr == '_' || *lexptr == '$');
	  yylval.sval.length = lexptr - yylval.sval.ptr;
	  write_dollar_variable (yylval.sval);
	  return GDB_VARIABLE;
	  break;
      }
    /* See if it is a special token of length 2.  */
    for (i = 0; i < sizeof (tokentab2) / sizeof (tokentab2[0]); i++)
	{
	    if (STREQN (lexptr, tokentab2[i].operator, 2))
		{
		    lexptr += 2;
		    return (tokentab2[i].token);
		}
	}
    /* Look for single character cases which which could be the first
       character of some other multicharacter token, but aren't, or we
       would already have found it. */
    switch (*lexptr)
	{
	    case '-':
	    case ':':
	    case '/':
	    case '<':
	    case '>':
		return (*lexptr++);
	}
    /* Look for a float literal before looking for an integer literal, so
       we match as much of the input stream as possible. */
    token = match_float_literal ();
    if (token != 0)
	{
	    return (token);
	}
    token = match_bitstring_literal ();
    if (token != 0)
	{
	    return (token);
	}
    token = match_integer_literal ();
    if (token != 0)
	{
	    return (token);
	}

    /* Try to match a simple name string, and if a match is found, then
       further classify what sort of name it is and return an appropriate
       token.  Note that attempting to match a simple name string consumes
       the token from lexptr, so we can't back out if we later find that
       we can't classify what sort of name it is. */

    inputname = match_simple_name_string ();

    if (inputname != NULL)
      {
	char *simplename = (char*) alloca (strlen (inputname) + 1);

	char *dptr = simplename, *sptr = inputname;
	for (; *sptr; sptr++)
	  *dptr++ = isupper (*sptr) ? tolower(*sptr) : *sptr;
	*dptr = '\0';

	/* See if it is a reserved identifier. */
	for (i = 0; i < sizeof (idtokentab) / sizeof (idtokentab[0]); i++)
	    {
		if (STREQ (simplename, idtokentab[i].operator))
		    {
			return (idtokentab[i].token);
		    }
	    }

	/* Look for other special tokens. */
	if (STREQ (simplename, "true"))
	    {
		yylval.ulval = 1;
		return (BOOLEAN_LITERAL);
	    }
	if (STREQ (simplename, "false"))
	    {
		yylval.ulval = 0;
		return (BOOLEAN_LITERAL);
	    }

	sym = lookup_symbol (inputname, expression_context_block,
			     VAR_NAMESPACE, (int *) NULL,
			     (struct symtab **) NULL);
	if (sym == NULL && strcmp (inputname, simplename) != 0)
	  {
	    sym = lookup_symbol (simplename, expression_context_block,
				 VAR_NAMESPACE, (int *) NULL,
				 (struct symtab **) NULL);
	  }
	if (sym != NULL)
	  {
	    yylval.ssym.stoken.ptr = NULL;
	    yylval.ssym.stoken.length = 0;
	    yylval.ssym.sym = sym;
	    yylval.ssym.is_a_field_of_this = 0;	/* FIXME, C++'ism */
	    switch (SYMBOL_CLASS (sym))
	      {
	      case LOC_BLOCK:
		/* Found a procedure name. */
		return (GENERAL_PROCEDURE_NAME);
	      case LOC_STATIC:
		/* Found a global or local static variable. */
		return (LOCATION_NAME);
	      case LOC_REGISTER:
	      case LOC_ARG:
	      case LOC_REF_ARG:
	      case LOC_REGPARM:
	      case LOC_REGPARM_ADDR:
	      case LOC_LOCAL:
	      case LOC_LOCAL_ARG:
	      case LOC_BASEREG:
	      case LOC_BASEREG_ARG:
		if (innermost_block == NULL
		    || contained_in (block_found, innermost_block))
		  {
		    innermost_block = block_found;
		  }
		return (LOCATION_NAME);
		break;
	      case LOC_CONST:
	      case LOC_LABEL:
		return (LOCATION_NAME);
		break;
	      case LOC_TYPEDEF:
		yylval.tsym.type = SYMBOL_TYPE (sym);
		return TYPENAME;
	      case LOC_UNDEF:
	      case LOC_CONST_BYTES:
	      case LOC_OPTIMIZED_OUT:
		error ("Symbol \"%s\" names no location.", inputname);
		break;
	      }
	  }
	else if (!have_full_symbols () && !have_partial_symbols ())
	  {
	    error ("No symbol table is loaded.  Use the \"file\" command.");
	  }
	else
	  {
	    error ("No symbol \"%s\" in current context.", inputname);
	  }
      }

    /* Catch single character tokens which are not part of some
       longer token. */

    switch (*lexptr)
      {
	case '.':			/* Not float for example. */
	  lexptr++;
	  while (isspace (*lexptr)) lexptr++;
	  inputname = match_simple_name_string ();
	  if (!inputname)
	    return '.';
	  return FIELD_NAME;
      }

    return (ILLEGAL_TOKEN);
}

static void
write_lower_upper_value (opcode, type)
     enum exp_opcode opcode;  /* Either UNOP_LOWER or UNOP_UPPER */
     struct type *type;
{
  extern LONGEST type_lower_upper ();
  struct type *result_type;
  LONGEST val = type_lower_upper (opcode, type, &result_type);
  write_exp_elt_opcode (OP_LONG);
  write_exp_elt_type (result_type);
  write_exp_elt_longcst (val);
  write_exp_elt_opcode (OP_LONG);
}

void
yyerror (msg)
     char *msg;
{
  error ("A %s in expression, near `%s'.", (msg ? msg : "error"), lexptr);
}
