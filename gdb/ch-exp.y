/* YACC grammar for Chill expressions, for GDB.
   Copyright (C) 1992 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

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

#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "frame.h"
#include "expression.h"
#include "language.h"
#include "value.h"
#include "parser-defs.h"
#include "bfd.h"
#include "symfile.h"
#include "objfiles.h"

/* These MUST be included in any grammar file!!!! Please choose unique names!
   Note that this are a combined list of variables that can be produced
   by any one of bison, byacc, or yacc. */
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
#define yyerrflag chill_errflag
#define yynerrs	chill_nerrs
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
#define yyss	chill_yyss		/* byacc */
#define	yyssp	chill_yysp		/* byacc */
#define	yyvs	chill_yyvs		/* byacc */
#define	yyvsp	chill_yyvsp	/* byacc */

static int
yylex PARAMS ((void));

static void
yyerror PARAMS ((char *));

int
yyparse PARAMS ((void));

/* #define	YYDEBUG	1 */

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

%{
static int parse_number PARAMS ((void));
%}

%token <voidval> FIXME

%token <typed_val>	INTEGER_LITERAL
%token <ulval>		BOOLEAN_LITERAL
%token <typed_val>	CHARACTER_LITERAL
%token <voidval>	SET_LITERAL
%token <voidval>	EMPTINESS_LITERAL
%token <voidval>	CHARACTER_STRING_LITERAL
%token <voidval>	BIT_STRING_LITERAL

%token <voidval>	STRING
%token <voidval>	CONSTANT
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
%token <voidval>	SC
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

%type <voidval>		location
%type <voidval>		primitive_value
%type <voidval>		location_contents
%type <voidval>		value_name
%type <voidval>		literal
%type <voidval>		tuple
%type <voidval>		value_string_element
%type <voidval>		value_string_slice
%type <voidval>		value_array_element
%type <voidval>		value_array_slice
%type <voidval>		value_structure_field
%type <voidval>		expression_conversion
%type <voidval>		value_procedure_call
%type <voidval>		value_built_in_routine_call
%type <voidval>		start_expression
%type <voidval>		zero_adic_operator
%type <voidval>		parenthesised_expression
%type <voidval>		value
%type <voidval>		undefined_value
%type <voidval>		expression
%type <voidval>		conditional_expression
%type <voidval>		then_alternative
%type <voidval>		else_alternative
%type <voidval>		sub_expression
%type <voidval>		value_case_alternative
%type <voidval>		operand_0
%type <voidval>		operand_1
%type <voidval>		operand_2
%type <voidval>		operand_3
%type <voidval>		operand_4
%type <voidval>		operand_5
%type <voidval>		operand_6
%type <voidval>		integer_literal_expression
%type <voidval>		synonym_name
%type <voidval>		value_enumeration_name
%type <voidval>		value_do_with_name
%type <voidval>		value_receive_name
%type <voidval>		general_procedure_name
%type <voidval>		string_primitive_value
%type <voidval>		start_element
%type <voidval>		left_element
%type <voidval>		right_element
%type <voidval>		slice_size
%type <voidval>		array_primitive_value
%type <voidval>		expression_list
%type <voidval>		lower_element
%type <voidval>		upper_element
%type <voidval>		first_element
%type <voidval>		structure_primitive_value
%type <voidval>		field_name
%type <voidval>		mode_name
%type <voidval>		boolean_expression
%type <voidval>		case_selector_list
%type <voidval>		subexpression
%type <voidval>		case_label_specification
%type <voidval>		buffer_location

%%

/* Z.200, 5.3.1 */

value		:	expression
			{
			  $$ = 0;	/* FIXME */
			}
		|	undefined_value
			{
			  $$ = 0;	/* FIXME */
			}
		;

undefined_value	:	FIXME
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 4.2.1 */

location	:	FIXME
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.1 */

primitive_value	:	location_contents
			{
			  $$ = 0;	/* FIXME */
			}
                |	value_name
			{
			  $$ = 0;	/* FIXME */
			}
                |	literal
			{
			  $$ = 0;	/* FIXME */
			}
                |	tuple
			{
			  $$ = 0;	/* FIXME */
			}
                |	value_string_element
			{
			  $$ = 0;	/* FIXME */
			}
                |	value_string_slice
			{
			  $$ = 0;	/* FIXME */
			}
                |	value_array_element
			{
			  $$ = 0;	/* FIXME */
			}
                |	value_array_slice
			{
			  $$ = 0;	/* FIXME */
			}
                |	value_structure_field
			{
			  $$ = 0;	/* FIXME */
			}
                |	expression_conversion
			{
			  $$ = 0;	/* FIXME */
			}
                |	value_procedure_call
			{
			  $$ = 0;	/* FIXME */
			}
                |	value_built_in_routine_call
			{
			  $$ = 0;	/* FIXME */
			}
                |	start_expression
			{
			  $$ = 0;	/* FIXME */
			}
                |	zero_adic_operator
			{
			  $$ = 0;	/* FIXME */
			}
                |	parenthesised_expression
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.2 */

location_contents:	location
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.3 */

value_name	:	synonym_name
			{
			  $$ = 0;	/* FIXME */
			}
		|	value_enumeration_name
			{
			  $$ = 0;	/* FIXME */
			}
		|	value_do_with_name
			{
			  $$ = 0;	/* FIXME */
			}
		|	value_receive_name
			{
			  $$ = 0;	/* FIXME */
			}
		|	general_procedure_name
			{
			  $$ = 0;	/* FIXME */
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
		|	SET_LITERAL
			{
			  $$ = 0;	/* FIXME */
			}
		|	EMPTINESS_LITERAL
			{
			  $$ = 0;	/* FIXME */
			}
		|	CHARACTER_STRING_LITERAL
			{
			  $$ = 0;	/* FIXME */
			}
		|	BIT_STRING_LITERAL
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.5 */

tuple		:	FIXME
			{
			  $$ = 0;	/* FIXME */
			}
		;


/* Z.200, 5.2.6 */

value_string_element:	string_primitive_value '(' start_element ')'
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.7 */

value_string_slice:	string_primitive_value '(' left_element ':' right_element ')'
			{
			  $$ = 0;	/* FIXME */
			}
		|	string_primitive_value '(' start_element UP slice_size ')'
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.8 */

value_array_element:	array_primitive_value '(' expression_list ')'
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.9 */

value_array_slice:	array_primitive_value '(' lower_element ':' upper_element ')'
			{
			  $$ = 0;	/* FIXME */
			}
		|	array_primitive_value '(' first_element UP slice_size '('
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.10 */

value_structure_field:	structure_primitive_value '.' field_name
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.11 */

expression_conversion:	mode_name '(' expression ')'
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.12 */

value_procedure_call:	FIXME
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.13 */

value_built_in_routine_call:	FIXME
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.14 */

start_expression:	FIXME
			{
			  $$ = 0;	/* FIXME */
			}	/* Not in GNU-Chill */
		;

/* Z.200, 5.2.15 */

zero_adic_operator:	FIXME
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.2.16 */

parenthesised_expression:	'(' expression ')'
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.3.2 */

expression	:	operand_0
			{
			  $$ = 0;	/* FIXME */
			}
		|	conditional_expression
			{
			  $$ = 0;	/* FIXME */
			}
		;

conditional_expression : IF boolean_expression then_alternative else_alternative FI
			{
			  $$ = 0;	/* FIXME */
			}
		|	CASE case_selector_list OF value_case_alternative '[' ELSE sub_expression ']' ESAC
			{
			  $$ = 0;	/* FIXME */
			}
		;

then_alternative:	THEN subexpression
			{
			  $$ = 0;	/* FIXME */
			}
		;

else_alternative:	ELSE subexpression
			{
			  $$ = 0;	/* FIXME */
			}
		|	ELSIF boolean_expression then_alternative else_alternative
			{
			  $$ = 0;	/* FIXME */
			}
		;

sub_expression	:	expression
			{
			  $$ = 0;	/* FIXME */
			}
		;

value_case_alternative:	case_label_specification ':' sub_expression ';'
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.3.3 */

operand_0	:	operand_1
			{
			  $$ = 0;	/* FIXME */
			}
		|	operand_0 LOGIOR operand_1
			{
			  write_exp_elt_opcode (BINOP_BITWISE_IOR);
			}
		|	operand_0 ORIF operand_1
			{
			  $$ = 0;	/* FIXME */
			}
		|	operand_0 LOGXOR operand_1
			{
			  write_exp_elt_opcode (BINOP_BITWISE_XOR);
			}
		;

/* Z.200, 5.3.4 */

operand_1	:	operand_2
			{
			  $$ = 0;	/* FIXME */
			}
		|	operand_1 LOGAND operand_2
			{
			  write_exp_elt_opcode (BINOP_BITWISE_AND);
			}
		|	operand_1 ANDIF operand_2
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.3.5 */

operand_2	:	operand_3
			{
			  $$ = 0;	/* FIXME */
			}
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
			  $$ = 0;	/* FIXME */
			}
		;


/* Z.200, 5.3.6 */

operand_3	:	operand_4
			{
			  $$ = 0;	/* FIXME */
			}
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
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.3.7 */

operand_4	:	operand_5
			{
			  $$ = 0;	/* FIXME */
			}
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
			  $$ = 0;	/* FIXME */
			}
		|	operand_4 REM operand_5
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.3.8 */

operand_5	:	operand_6
			{
			  $$ = 0;	/* FIXME */
			}
		|	'-' operand_6
			{
			  write_exp_elt_opcode (UNOP_NEG);
			}
		|	NOT operand_6
			{
			  write_exp_elt_opcode (UNOP_LOGICAL_NOT);
			}
		|	'(' integer_literal_expression ')' operand_6
			{
			  $$ = 0;	/* FIXME */
			}
		;

/* Z.200, 5.3.9 */

operand_6	:	POINTER location
			{
			  $$ = 0;	/* FIXME */
			}
		|	RECEIVE buffer_location
			{
			  $$ = 0;	/* FIXME */
			}
		|	primitive_value
			{
			  $$ = 0;	/* FIXME */
			}
		;


/* Z.200, 12.4.3 */
/* FIXME:  For now we just accept only a single integer literal. */

integer_literal_expression:
			INTEGER_LITERAL
			{
			  $$ = 0;
			}

/* Things which still need productions... */
synonym_name	 	:	FIXME { $$ = 0; }
value_enumeration_name 	:	FIXME { $$ = 0; }
value_do_with_name 	:	FIXME { $$ = 0; }
value_receive_name 	:	FIXME { $$ = 0; }
general_procedure_name 	:	FIXME { $$ = 0; }
string_primitive_value 	:	FIXME { $$ = 0; }
start_element 		:	FIXME { $$ = 0; }
left_element 		:	FIXME { $$ = 0; }
right_element 		:	FIXME { $$ = 0; }
slice_size 		:	FIXME { $$ = 0; }
array_primitive_value 	:	FIXME { $$ = 0; }
expression_list 	:	FIXME { $$ = 0; }
lower_element 		:	FIXME { $$ = 0; }
upper_element 		:	FIXME { $$ = 0; }
first_element 		:	FIXME { $$ = 0; }
structure_primitive_value:	FIXME { $$ = 0; }
field_name 		:	FIXME { $$ = 0; }
mode_name 		:	FIXME { $$ = 0; }
boolean_expression 	:	FIXME { $$ = 0; }
case_selector_list 	:	FIXME { $$ = 0; }
subexpression 		:	FIXME { $$ = 0; }
case_label_specification:	FIXME { $$ = 0; }
buffer_location 	:	FIXME { $$ = 0; }

%%

static int
decode_integer_literal (valptr, tokptrptr)
int *valptr;
char **tokptrptr;
{
  char *tokptr = *tokptrptr;
  int base = 0;
  int ival = 0;
  int digits = 0;
  int temp;
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
  
  /* Start looking for a value composed of valid digits as set by the base
     in use.  Note that '_' characters are valid anywhere, in any quantity,
     and are simply ignored.  Since we must find at least one valid digit,
     or reject this token as an integer literal, we keep track of how many
     digits we have encountered. */
  
  while (*tokptr != '\0')
    {
      temp = tolower (*tokptr);
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
	  ival *= base;
	  ival += temp;
	}
      else
	{
	  /* Found something not in domain for current base. */
	  tokptr--;	/* Unconsume what gave us indigestion. */
	  break;
	}
    }
  
  /* If we didn't find any digits, then we don't have a valid integer
     literal, so reject the entire token.  Also, if we have an explicit
     base, then the next character must not be a single quote, or we
     have a bitstring literal, so reject the entire token in this case
     as well.  Otherwise, update the lexical scan pointer, and return
     non-zero for success. */
  
  if (digits == 0)
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

/* Recognize a character literal.  A character literal is single character
   or a control sequence, enclosed in single quotes.  A control sequence
   is a comma separated list of one or more integer literals, enclosed
   in parenthesis and introduced with a circumflex character.

   EX:  'a'  '^(7)'  '^(7,8)'

   Returns CHARACTER_LITERAL if a match is found.
   */

static int
match_character_literal ()
{
  char *tokptr = lexptr;
  int ival = 0;
  
  /* All character literals must start with a single quote.  If we don't
     find it, don't bother looking any further. */

  if (*tokptr++ != '\'')
    {
      return (0);
    }

  /* Determine which form we have, either a control sequence or the
     single character form. */

  if ((*tokptr == '^') && (*(tokptr + 1) == '('))
    {
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
  int ival = 0;
  int base = 0;
  int digits = 0;
  int temp;
  
  /* Look for an explicit base specifier, which is optional. */
  
  switch (*tokptr)
    {
    case 'd':
    case 'D':
      base = 10;
      tokptr++;
      break;
    case 'b':
    case 'B':
      base = 2;
      tokptr++;
      break;
    case 'h':
    case 'H':
      base = 16;
      tokptr++;
      break;
    case 'o':
    case 'O':
      base = 8;
      tokptr++;
      break;
    }
  
  /* If we found no explicit base then default to 10, otherwise ensure
     that the character after the explicit base is a single quote. */
  
  if (base == 0)
    {
      base = 10;
    }
  else
    {
      if (*tokptr++ != '\'')
	{
	  return (0);
	}
    }
  
  /* Start looking for a value composed of valid digits as set by the base
     in use.  Note that '_' characters are valid anywhere, in any quantity,
     and are simply ignored.  Since we must find at least one valid digit,
     or reject this token as an integer literal, we keep track of how many
     digits we have encountered. */
  
  while (*tokptr != '\0')
    {
      temp = tolower (*tokptr);
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
	  ival *= base;
	  ival += temp;
	}
      else
	{
	  /* Found something not in domain for current base. */
	  tokptr--;	/* Unconsume what gave us indigestion. */
	  break;
	}
    }
  
  /* If we didn't find any digits, then we don't have a valid integer
     literal, so reject the entire token.  Otherwise, set up the parser
     variables, advance the current lexical scan pointer, and return the
     INTEGER_LITERAL token. */
  
  if (digits == 0)
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

static void convert_float ()
{
#if 0
    extern double strtod ();
    double d;
    char	tmp[256];
    char	*p = yytext, *p1 = tmp;
    char	c;
    
    while (c = *p++)
    {
	switch (c)
	{
	case '_':
	    break;
	case 'E':
	case 'd':
	case 'D':
	    *p1++ = 'e';
	    break;
	default:
	    *p1++ = c;
	    break;
	}
    }
    *p1 = '\0';
    d = strtod (tmp, &p1);
    if (*p1)
    {
	/* add error handling here */
	;
    }
    yylval.dval = d;
#endif
}

/* Take care of parsing a number (anything that starts with a digit).
   Set yylval and return the token type; update lexptr.
   LEN is the number of characters in it.  */

/*** Needs some error checking for the float case ***/

static int
parse_number ()
{
}

struct token
{
  char *operator;
  int token;
};

const static struct token tokentab5[] =
{
    { "ANDIF", ANDIF }
};

const static struct token tokentab4[] =
{
    { "ORIF", ORIF }
};

const static struct token tokentab3[] =
{
    { "NOT", NOT },
    { "XOR", LOGXOR },
    { "AND", LOGAND }
};

const static struct token tokentab2[] =
{
    { "//", SLASH_SLASH },
    { "/=", NOTEQUAL },
    { "<=", LEQ },
    { ">=", GTR },
    { "IN", IN },
    { "OR", LOGIOR }
};

/* Read one token, getting characters through lexptr.  */
/* This is where we will check to make sure that the language and the
   operators used are compatible.  */

static int
yylex ()
{
    unsigned int i;
    int token;

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
	    case '.':
	    case '=':
	    case ':':
	    case ';':
	    case '!':
	    case '+':
	    case '-':
	    case '*':
	    case '/':
	    case '(':
	    case ')':
	    case '[':
	    case ']':
		return (*lexptr++);
	}
    /* Look for characters which start a particular kind of multicharacter
       token, such as a character literal. */
    switch (*lexptr)
      {
	case '^':
	case '\'':
	  token = match_character_literal ();
	  if (token != 0)
	    {
	      return (token);
	    }
	  break;
      }
    /* See if it is a special token of length 5.  */
    for (i = 0; i < sizeof (tokentab5) / sizeof (tokentab5[0]); i++)
	{
	    if (strncmp (lexptr, tokentab5[i].operator, 5) == 0)
		{
		    lexptr += 5;
		    return (tokentab5[i].token);
		}
	}
    /* See if it is a special token of length 4.  */
    for (i = 0; i < sizeof (tokentab4) / sizeof (tokentab4[0]); i++)
	{
	    if (strncmp (lexptr, tokentab4[i].operator, 4) == 0)
		{
		    lexptr += 4;
		    return (tokentab4[i].token);
		}
	}
    /* See if it is a special token of length 3.  */
    for (i = 0; i < sizeof (tokentab3) / sizeof (tokentab3[0]); i++)
	{
	    if (strncmp (lexptr, tokentab3[i].operator, 3) == 0)
		{
		    lexptr += 3;
		    return (tokentab3[i].token);
		}
	}
    /* See if it is a special token of length 2.  */
    for (i = 0; i < sizeof (tokentab2) / sizeof (tokentab2[0]); i++)
	{
	    if (strncmp (lexptr, tokentab2[i].operator, 2) == 0)
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
	    case '/':
	    case '<':
	    case '>':
		return (*lexptr++);
	}
    /* Look for other special tokens. */
    if (strncmp (lexptr, "TRUE", 4) == 0) /* FIXME:  What about lowercase? */
	{
	    yylval.ulval = 1;
	    lexptr += 4;
	    return (BOOLEAN_LITERAL);
	}
    if (strncmp (lexptr, "FALSE", 5) == 0) /* FIXME:  What about lowercase? */
	{
	    yylval.ulval = 0;
	    lexptr += 5;
	    return (BOOLEAN_LITERAL);
	}
    token = match_integer_literal ();
    if (token != 0);
	{
	    return (token);
	}
    return (ILLEGAL_TOKEN);
}

static void
yyerror (msg)
     char *msg;	/* unused */
{
  printf ("Parsing:  %s\n", lexptr);
  if (yychar < 256)
    {
      error ("Invalid syntax in expression near character '%c'.", yychar);
    }
  else
    {
      error ("Invalid syntax in expression");
    }
}


/* Table of operators and their precedences for printing expressions.  */

const static struct op_print chill_op_print_tab[] = {
    {"AND", BINOP_LOGICAL_AND, PREC_LOGICAL_AND, 0},
    {"OR",  BINOP_LOGICAL_OR, PREC_LOGICAL_OR, 0},
    {"NOT", UNOP_LOGICAL_NOT, PREC_PREFIX, 0},
    {"MOD", BINOP_REM, PREC_MUL, 0},
    {":=",  BINOP_ASSIGN, PREC_ASSIGN, 1},
    {"=",   BINOP_EQUAL, PREC_EQUAL, 0},
    {"/=",  BINOP_NOTEQUAL, PREC_EQUAL, 0},
    {"<=",  BINOP_LEQ, PREC_ORDER, 0},
    {">=",  BINOP_GEQ, PREC_ORDER, 0},
    {">",   BINOP_GTR, PREC_ORDER, 0},
    {"<",   BINOP_LESS, PREC_ORDER, 0},
    {"+",   BINOP_ADD, PREC_ADD, 0},
    {"-",   BINOP_SUB, PREC_ADD, 0},
    {"*",   BINOP_MUL, PREC_MUL, 0},
    {"/",   BINOP_DIV, PREC_MUL, 0},
    {"-",   UNOP_NEG, PREC_PREFIX, 0},
    {NULL,  0, 0, 0}
};


/* The built-in types of Chill.  */

struct type *builtin_type_chill_bool;
struct type *builtin_type_chill_char;
struct type *builtin_type_chill_long;
struct type *builtin_type_chill_ulong;
struct type *builtin_type_chill_real;

struct type ** const (chill_builtin_types[]) = 
{
  &builtin_type_chill_bool,
  &builtin_type_chill_char,
  &builtin_type_chill_long,
  &builtin_type_chill_ulong,
  &builtin_type_chill_real,
  0
};

const struct language_defn chill_language_defn = {
  "chill",
  language_chill,
  chill_builtin_types,
  range_check_on,
  type_check_on,
  chill_parse,			/* parser */
  chill_error,			/* parser error function */
  &BUILTIN_TYPE_LONGEST,	/* longest signed   integral type */
  &BUILTIN_TYPE_UNSIGNED_LONGEST,/* longest unsigned integral type */
  &builtin_type_chill_real,	/* longest floating point type */
  {"",      "B'",  "",   ""},	/* Binary format info */
  {"O'%o",  "O'",  "o",  ""},	/* Octal format info */
  {"D'%d",  "D'",  "d",  ""},	/* Decimal format info */
  {"H'%x",  "H'",  "x",  ""},	/* Hex format info */
  chill_op_print_tab,		/* expression operators for printing */
  LANG_MAGIC
};

/* Initialization for Chill */

void
_initialize_chill_exp ()
{
  builtin_type_chill_bool =
    init_type (TYPE_CODE_BOOL, TARGET_INT_BIT / TARGET_CHAR_BIT,
	       TYPE_FLAG_UNSIGNED,
	       "BOOL", (struct objfile *) NULL);
  builtin_type_chill_char =
    init_type (TYPE_CODE_CHAR, TARGET_CHAR_BIT / TARGET_CHAR_BIT,
	       TYPE_FLAG_UNSIGNED,
	       "CHAR", (struct objfile *) NULL);
  builtin_type_chill_long =
    init_type (TYPE_CODE_INT, TARGET_LONG_BIT / TARGET_CHAR_BIT,
	       0,
	       "LONG", (struct objfile *) NULL);
  builtin_type_chill_ulong =
    init_type (TYPE_CODE_INT, TARGET_LONG_BIT / TARGET_CHAR_BIT,
	       TYPE_FLAG_UNSIGNED,
	       "ULONG", (struct objfile *) NULL);
  builtin_type_chill_real =
    init_type (TYPE_CODE_FLT, TARGET_DOUBLE_BIT / TARGET_CHAR_BIT,
	       0,
	       "LONG_REAL", (struct objfile *) NULL);

  add_language (&chill_language_defn);
}
