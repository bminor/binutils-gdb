/* Parse C expressions for GDB.
   Copyright (C) 1986 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

/* Parse a C expression from text in a string,
   and return the result as a  struct expression  pointer.
   That structure contains arithmetic operations in reverse polish,
   with constants represented by operations that are followed by special data.
   See expression.h for the details of the format.
   What is important here is that it can be built up sequentially
   during the process of parsing; the lower levels of the tree always
   come first in the result.  */
   
%{
#include "defs.h"
#include "param.h"
#include "symtab.h"
#include "frame.h"
#include "expression.h"

#include <stdio.h>

static struct expression *expout;
static int expout_size;
static int expout_ptr;

static int yylex ();
static yyerror ();
static void write_exp_elt ();
static void write_exp_string ();
static void start_arglist ();
static int end_arglist ();
static void free_funcalls ();
static char *copy_name ();

/* If this is nonzero, this block is used as the lexical context
   for symbol names.  */

static struct block *expression_context_block;

/* Number of arguments seen so far in innermost function call.  */
static int arglist_len;

/* Data structure for saving values of arglist_len
   for function calls whose arguments contain other function calls.  */

struct funcall
  {
    struct funcall *next;
    int arglist_len;
  };

struct funcall *funcall_chain;

/* This kind of datum is used to represent the name
   of a symbol token.  */

struct stoken
  {
    char *ptr;
    int length;
  };
%}

/* Although the yacc "value" of an expression is not used,
   since the result is stored in the structure being created,
   other node types do have values.  */

%union
  {
    long lval;
    double dval;
    struct symbol *sym;
    struct type *tval;
    struct stoken sval;
    int voidval;
    struct block *bval;
    enum exp_opcode opcode;
    struct internalvar *ivar;
  }

%type <voidval> exp exp1 start variable
%type <tval> type typebase
%type <bval> block

%token <lval> INT CHAR
%token <dval> FLOAT

/* Both NAME and TYPENAME tokens represent symbols in the input,
   and both convey their data as strings.
   But a TYPENAME is a string that happens to be defined as a typedef
   or builtin type name (such as int or char)
   and a NAME is any other symbol.

   Contexts where this distinction is not important can use the
   nonterminal "name", which matches either NAME or TYPENAME.  */

%token <sval> NAME TYPENAME STRING
%type <sval> name

%token STRUCT UNION ENUM SIZEOF UNSIGNED COLONCOLON

%token <lval> LAST REGNAME

%token <ivar> VARIABLE

%token <opcode> ASSIGN_MODIFY

%left ','
%left ABOVE_COMMA
%right '=' ASSIGN_MODIFY
%left OR
%left AND
%left '|'
%left '^'
%left '&'
%left EQUAL NOTEQUAL
%left '<' '>' LEQ GEQ
%left LSH RSH
%left '+' '-'
%left '*' '/' '%'
%left '@'
%right UNARY INCREMENT DECREMENT
%right ARROW '.' '['
%left COLONCOLON

%%

start   :	exp1
	;

/* Expressions, including the comma operator.  */
exp1	:	exp
	|	exp1 ',' exp
			{ write_exp_elt (BINOP_COMMA); }
	;

/* Expressions, not including the comma operator.  */
exp	:	'*' exp    %prec UNARY
			{ write_exp_elt (UNOP_IND); }

exp	:	'&' exp    %prec UNARY
			{ write_exp_elt (UNOP_ADDR); }

exp	:	'-' exp    %prec UNARY
			{ write_exp_elt (UNOP_NEG); }
	;

exp	:	'!' exp    %prec UNARY
			{ write_exp_elt (UNOP_ZEROP); }
	;

exp	:	'~' exp    %prec UNARY
			{ write_exp_elt (UNOP_LOGNOT); }
	;

exp	:	INCREMENT exp    %prec UNARY
			{ write_exp_elt (UNOP_PREINCREMENT); }
	;

exp	:	DECREMENT exp    %prec UNARY
			{ write_exp_elt (UNOP_PREDECREMENT); }
	;

exp	:	exp INCREMENT    %prec UNARY
			{ write_exp_elt (UNOP_POSTINCREMENT); }
	;

exp	:	exp DECREMENT    %prec UNARY
			{ write_exp_elt (UNOP_POSTDECREMENT); }
	;

exp	:	SIZEOF exp       %prec UNARY
			{ write_exp_elt (UNOP_SIZEOF); }
	;

exp	:	exp ARROW name
			{ write_exp_elt (STRUCTOP_PTR);
			  write_exp_string ($3);
			  write_exp_elt (STRUCTOP_PTR); }
	;

exp	:	exp '.' name
			{ write_exp_elt (STRUCTOP_STRUCT);
			  write_exp_string ($3);
			  write_exp_elt (STRUCTOP_STRUCT); }
	;

exp	:	exp '[' exp1 ']'
			{ write_exp_elt (BINOP_SUBSCRIPT); }
	;

exp	:	exp '(' 
			/* This is to save the value of arglist_len
			   being accumulated by an outer function call.  */
			{ start_arglist (); }
		arglist ')'
			{ write_exp_elt (OP_FUNCALL);
			  write_exp_elt (end_arglist ());
			  write_exp_elt (OP_FUNCALL); }
	;

arglist	:
	;

arglist	:	exp
			{ arglist_len = 1; }
	;

arglist	:	arglist ',' exp   %prec ABOVE_COMMA
			{ arglist_len++; }
	;

exp	:	'{' type '}' exp  %prec UNARY
			{ write_exp_elt (UNOP_MEMVAL);
			  write_exp_elt ($2);
			  write_exp_elt (UNOP_MEMVAL); }
	;

exp	:	'(' type ')' exp  %prec UNARY
			{ write_exp_elt (UNOP_CAST);
			  write_exp_elt ($2);
			  write_exp_elt (UNOP_CAST); }
	;

exp	:	'(' exp1 ')'
			{ }
	;

/* Binary operators in order of decreasing precedence.  */

exp	:	exp '@' exp
			{ write_exp_elt (BINOP_REPEAT); }
	;

exp	:	exp '*' exp
			{ write_exp_elt (BINOP_MUL); }
	;

exp	:	exp '/' exp
			{ write_exp_elt (BINOP_DIV); }
	;

exp	:	exp '%' exp
			{ write_exp_elt (BINOP_REM); }
	;

exp	:	exp '+' exp
			{ write_exp_elt (BINOP_ADD); }
	;

exp	:	exp '-' exp
			{ write_exp_elt (BINOP_SUB); }
	;

exp	:	exp LSH exp
			{ write_exp_elt (BINOP_LSH); }
	;

exp	:	exp RSH exp
			{ write_exp_elt (BINOP_RSH); }
	;

exp	:	exp EQUAL exp
			{ write_exp_elt (BINOP_EQUAL); }
	;

exp	:	exp NOTEQUAL exp
			{ write_exp_elt (BINOP_NOTEQUAL); }
	;

exp	:	exp LEQ exp
			{ write_exp_elt (BINOP_LEQ); }
	;

exp	:	exp GEQ exp
			{ write_exp_elt (BINOP_GEQ); }
	;

exp	:	exp '<' exp
			{ write_exp_elt (BINOP_LESS); }
	;

exp	:	exp '>' exp
			{ write_exp_elt (BINOP_GTR); }
	;

exp	:	exp '&' exp
			{ write_exp_elt (BINOP_LOGAND); }
	;

exp	:	exp '^' exp
			{ write_exp_elt (BINOP_LOGXOR); }
	;

exp	:	exp '|' exp
			{ write_exp_elt (BINOP_LOGIOR); }
	;

exp	:	exp AND exp
			{ write_exp_elt (BINOP_AND); }
	;

exp	:	exp OR exp
			{ write_exp_elt (BINOP_OR); }
	;

exp	:	exp '?' exp ':' exp
			{ write_exp_elt (TERNOP_COND); }
	;
			  
exp	:	exp '=' exp
			{ write_exp_elt (BINOP_ASSIGN); }
	;

exp	:	exp ASSIGN_MODIFY exp
			{ write_exp_elt (BINOP_ASSIGN_MODIFY);
			  write_exp_elt ($2);
			  write_exp_elt (BINOP_ASSIGN_MODIFY); }
	;

exp	:	INT
			{ write_exp_elt (OP_LONG);
			  write_exp_elt (builtin_type_long);
			  write_exp_elt ($1);
			  write_exp_elt (OP_LONG); }
	;

exp	:	CHAR
			{ write_exp_elt (OP_LONG);
			  write_exp_elt (builtin_type_char);
			  write_exp_elt ($1);
			  write_exp_elt (OP_LONG); }
	;

exp	:	FLOAT
			{ write_exp_elt (OP_DOUBLE);
			  write_exp_elt (builtin_type_double);
			  write_exp_elt ($1);
			  write_exp_elt (OP_DOUBLE); }
	;

exp	:	variable
	;

exp	:	LAST
			{ write_exp_elt (OP_LAST);
			  write_exp_elt ($1);
			  write_exp_elt (OP_LAST); }
	;

exp	:	REGNAME
			{ write_exp_elt (OP_REGISTER);
			  write_exp_elt ($1);
			  write_exp_elt (OP_REGISTER); }
	;

exp	:	VARIABLE
			{ write_exp_elt (OP_INTERNALVAR);
			  write_exp_elt ($1);
			  write_exp_elt (OP_INTERNALVAR); }
	;

exp	:	SIZEOF '(' type ')'
			{ write_exp_elt (OP_LONG);
			  write_exp_elt (builtin_type_int);
			  write_exp_elt ((long) TYPE_LENGTH ($3));
			  write_exp_elt (OP_LONG); }
	;

exp	:	STRING
			{ write_exp_elt (OP_STRING);
			  write_exp_string ($1);
			  write_exp_elt (OP_STRING); }
	;

block	:	name
			{ struct symtab *tem = lookup_symtab (copy_name ($1));
			  struct symbol *sym;
			  
			  if (tem)
			    $$ = BLOCKVECTOR_BLOCK (BLOCKVECTOR (tem), 1);
			  else
			    {
			      sym = lookup_symbol (copy_name ($1),
						   expression_context_block,
						   VAR_NAMESPACE);
			      if (sym && SYMBOL_CLASS (sym) == LOC_BLOCK)
				$$ = SYMBOL_BLOCK_VALUE (sym);
			      else
				error ("No file or function \"%s\".",
				       copy_name ($1));
			    }}
	;

block	:	block COLONCOLON name
			{ struct symbol *tem
			    = lookup_symbol (copy_name ($3), $1, VAR_NAMESPACE);
			  if (!tem || SYMBOL_CLASS (tem) != LOC_BLOCK)
			    error ("No function \"%s\" in specified context.",
				   copy_name ($3));
			  $$ = SYMBOL_BLOCK_VALUE (tem); }
	;

variable:	block COLONCOLON name
			{ struct symbol *sym;
			  sym = lookup_symbol (copy_name ($3), $1, VAR_NAMESPACE);
			  if (sym == 0)
			    error ("No symbol \"%s\" in specified context.",
				   copy_name ($3));
			  write_exp_elt (OP_VAR_VALUE);
			  write_exp_elt (sym);
			  write_exp_elt (OP_VAR_VALUE); }
	;

variable:	NAME
			{ struct symbol *sym;
			  sym = lookup_symbol (copy_name ($1),
					      expression_context_block,
					      VAR_NAMESPACE);
			  if (sym)
			    {
			      write_exp_elt (OP_VAR_VALUE);
			      write_exp_elt (sym);
			      write_exp_elt (OP_VAR_VALUE);
			    }
			  else
			    {
			      register char *arg = copy_name ($1);
			      register int i;
			      for (i = 0; i < misc_function_count; i++)
				if (!strcmp (misc_function_vector[i].name, arg))
				  break;

			      if (i < misc_function_count)
				{
				  write_exp_elt (OP_LONG);
				  write_exp_elt (builtin_type_int);
				  write_exp_elt (misc_function_vector[i].address);
				  write_exp_elt (OP_LONG);
				  write_exp_elt (UNOP_MEMVAL);
				  write_exp_elt (builtin_type_char);
				  write_exp_elt (UNOP_MEMVAL);
				}
			      else
				if (symtab_list == 0)
				  error ("No symbol table is loaded.  Use the \"symbol-file\" command.");
				else
				  error ("No symbol \"%s\" in current context.",
					 copy_name ($1));
			    }
			}
	;

type	:	typebase
	|	type '*'
			{ $$ = lookup_pointer_type ($1); }
	;

typebase
	:	TYPENAME
			{ $$ = lookup_typename (copy_name ($1),
						expression_context_block, 0); }
	|	STRUCT name
			{ $$ = lookup_struct (copy_name ($2),
					      expression_context_block); }
	|	UNION name
			{ $$ = lookup_union (copy_name ($2),
					     expression_context_block); }
	|	ENUM name
			{ $$ = lookup_enum (copy_name ($2),
					    expression_context_block); }
	|	UNSIGNED name
			{ $$ = lookup_unsigned_typename (copy_name ($2)); }
	;

name	:	NAME
	|	TYPENAME
	;
%%

/* Begin counting arguments for a function call,
   saving the data about any containing call.  */

static void
start_arglist ()
{
  register struct funcall *new = (struct funcall *) xmalloc (sizeof (struct funcall));

  new->next = funcall_chain;
  new->arglist_len = arglist_len;
  arglist_len = 0;
  funcall_chain = new;
}

/* Return the number of arguments in a function call just terminated,
   and restore the data for the containing function call.  */

static int
end_arglist ()
{
  register int val = arglist_len;
  register struct funcall *call = funcall_chain;
  funcall_chain = call->next;
  arglist_len = call->arglist_len;
  free (call);
  return val;
}

/* Free everything in the funcall chain.
   Used when there is an error inside parsing.  */

static void
free_funcalls ()
{
  register struct funcall *call, *next;

  for (call = funcall_chain; call; call = next)
    {
      next = call->next;
      free (call);
    }
}

/* This page contains the functions for adding data to the  struct expression
   being constructed.  */

/* Add one element to the end of the expression.  */

static void
write_exp_elt (expelt)
     union exp_element expelt;
{
  if (expout_ptr >= expout_size)
    {
      expout_size *= 2;
      expout = (struct expression *) xrealloc (expout,
					       sizeof (struct expression)
					       + expout_size * sizeof (union exp_element));
    }
  expout->elts[expout_ptr++] = expelt;
}

/* Add a string constant to the end of the expression.
   Follow it by its length in bytes, as a separate exp_element.  */

static void
write_exp_string (str)
     struct stoken str;
{
  register int len = str.length;
  register int lenelt
    = (len + sizeof (union exp_element)) / sizeof (union exp_element);

  expout_ptr += lenelt;

  if (expout_ptr >= expout_size)
    {
      expout_size = max (expout_size * 2, expout_ptr + 10);
      expout = (struct expression *) xrealloc (expout,
					       sizeof (struct expression)
					       + expout_size * sizeof (union exp_element));
    }
  bcopy (str.ptr, (char *) &expout->elts[expout_ptr - lenelt], len);
  ((char *) &expout->elts[expout_ptr - lenelt])[len] = 0;
  write_exp_elt (len);
}

/* During parsing of a C expression, the pointer to the next character
   is in this variable.  */

static char *lexptr;

/* Tokens that refer to names do so with explicit pointer and length,
   so they can share the storage that lexptr is parsing.

   When it is necessary to pass a name to a function that expects
   a null-terminated string, the substring is copied out
   into a block of storage that namecopy points to.

   namecopy is allocated once, guaranteed big enough, for each parsing.  */

static char *namecopy;

/* Current depth in parentheses within the expression.  */

static int paren_depth;

/* Nonzero means stop parsing on first comma (if not within parentheses).  */

static int comma_terminates;

/* Take care of parsing a number (anything that starts with a digit).
   Set yylval and return the token type; update lexptr.
   LEN is the number of characters in it.  */

/*** Needs some error checking for the float case ***/

static int
parse_number (olen)
     int olen;
{
  register char *p = lexptr;
  register long n = 0;
  register int c;
  register int base = 10;
  register int len = olen;
  char *err_copy;

  extern double atof ();

  for (c = 0; c < len; c++)
    if (p[c] == '.')
      {
	/* It's a float since it contains a point.  */
	yylval.dval = atof (p);
	lexptr += len;
	return FLOAT;
      }

  if (len >= 3 && (!strncmp (p, "0x", 2) || !strncmp (p, "0X", 2)))
    {
      p += 2;
      base = 16;
      len -= 2;
    }
  else if (*p == '0')
    base = 8;

  while (len-- > 0)
    {
      c = *p++;
      n *= base;
      if (c >= '0' && c <= '9')
	n += c - '0';
      else
	{
	  if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
	  if (base == 16 && c >= 'a' && c <= 'f')
	    n += c - 'a' + 10;
	  else if (len == 0 && c == 'l')
	    ;
	  else
	    {
	      err_copy = (char *) alloca (olen + 1);
	      bcopy (lexptr, err_copy, olen);
	      err_copy[olen] = 0;
	      error ("Invalid number \"%s\".", err_copy);
	    }
	}
    }

  lexptr = p;
  yylval.lval = n;
  return INT;
}

struct token
{
  char *operator;
  int token;
  enum exp_opcode opcode;
};

static struct token tokentab3[] =
  {
    {">>=", ASSIGN_MODIFY, BINOP_RSH},
    {"<<=", ASSIGN_MODIFY, BINOP_LSH}
  };

static struct token tokentab2[] =
  {
    {"+=", ASSIGN_MODIFY, BINOP_ADD},
    {"-=", ASSIGN_MODIFY, BINOP_SUB},
    {"*=", ASSIGN_MODIFY, BINOP_MUL},
    {"/=", ASSIGN_MODIFY, BINOP_DIV},
    {"%=", ASSIGN_MODIFY, BINOP_REM},
    {"|=", ASSIGN_MODIFY, BINOP_LOGIOR},
    {"&=", ASSIGN_MODIFY, BINOP_LOGAND},
    {"^=", ASSIGN_MODIFY, BINOP_LOGXOR},
    {"++", INCREMENT, BINOP_END},
    {"--", DECREMENT, BINOP_END},
    {"->", ARROW, BINOP_END},
    {"&&", AND, BINOP_END},
    {"||", OR, BINOP_END},
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
  register int c;
  register int namelen;
  register int i;
  register char *tokstart;

 retry:

  tokstart = lexptr;
  /* See if it is a special token of length 3.  */
  for (i = 0; i < sizeof tokentab3 / sizeof tokentab3[0]; i++)
    if (!strncmp (tokstart, tokentab3[i].operator, 3))
      {
	lexptr += 3;
	yylval.opcode = tokentab3[i].opcode;
	return tokentab3[i].token;
      }

  /* See if it is a special token of length 2.  */
  for (i = 0; i < sizeof tokentab2 / sizeof tokentab2[0]; i++)
    if (!strncmp (tokstart, tokentab2[i].operator, 2))
      {
	lexptr += 2;
	yylval.opcode = tokentab2[i].opcode;
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
      lexptr++;
      c = *lexptr++;
      if (c == '\\')
	c = parse_escape (&lexptr);
      yylval.lval = c;
      c = *lexptr++;
      if (c != '\'')
	error ("Invalid character constant.");
      return CHAR;

    case '(':
      paren_depth++;
      lexptr++;
      return c;

    case ')':
      if (paren_depth == 0)
	return 0;
      paren_depth--;
      lexptr++;
      return c;

    case ',':
      if (comma_terminates && paren_depth == 0)
	return 0;
      lexptr++;
      return c;

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
    case '.':
    case '?':
    case ':':
    case '=':
    case '{':
    case '}':
      lexptr++;
      return c;

    case '"':
      for (namelen = 1; (c = tokstart[namelen]) != '"'; namelen++)
	if (c == '\\')
	  {
	    c = tokstart[++namelen];
	    if (c >= '0' && c <= '9')
	      {
		c = tokstart[++namelen];
		if (c >= '0' && c <= '9')
		  c = tokstart[++namelen];
	      }
	  }
      yylval.sval.ptr = tokstart + 1;
      yylval.sval.length = namelen - 1;
      lexptr += namelen + 1;
      return STRING;
    }
  if (c >= '0' && c <= '9')
    {
      /* It's a number */
      for (namelen = 0;
	   c = tokstart[namelen],
	   (c == '_' || c == '$' || c == '.' || (c >= '0' && c <= '9')
	    || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
	   namelen++)
	;
      return parse_number (namelen);
    }

  if (!(c == '_' || c == '$'
	|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
    error ("Invalid token in expression.");

  /* It is a name.  See how long it is.  */

  for (namelen = 0;
       c = tokstart[namelen],
       (c == '_' || c == '$' || (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
       namelen++)
    ;

  /* The token "if" terminates the expression and is NOT 
     removed from the input stream.  */
  if (namelen == 2 && tokstart[0] == 'i' && tokstart[1] == 'f')
    {
      return 0;
    }

  lexptr += namelen;

  /* Handle the tokens $digits; also $ (short for $0) and $$ (short for $$1)
     and $$digits (equivalent to $<-digits> if you could type that).
     Make token type LAST, and put the number (the digits) in yylval.  */

  if (*tokstart == '$')
    {
      register int negate = 0;
      c = 1;
      /* Double dollar means negate the number and add -1 as well.
	 Thus $$ alone means -1.  */
      if (namelen >= 2 && tokstart[1] == '$')
	{
	  negate = 1;
	  c = 2;
	}
      if (c == namelen)
	{
	  /* Just dollars (one or two) */
	  yylval.lval = - negate;
	  return LAST;
	}
      /* Is the rest of the token digits?  */
      for (; c < namelen; c++)
	if (!(tokstart[c] >= '0' && tokstart[c] <= '9'))
	  break;
      if (c == namelen)
	{
	  yylval.lval = atoi (tokstart + 1 + negate);
	  if (negate)
	    yylval.lval = - yylval.lval;
	  return LAST;
	}
    }

  /* Handle tokens that refer to machine registers:
     $ followed by a register name.  */

  if (*tokstart == '$')
    for (c = 0; c < NUM_REGS; c++)
      if (namelen - 1 == strlen (reg_names[c])
	  && !strncmp (tokstart + 1, reg_names[c], namelen - 1))
	{
	  yylval.lval = c;
	  return REGNAME;
	}

  if (namelen == 6 && !strncmp (tokstart, "struct", 6))
    {
      return STRUCT;
    }
  if (namelen == 5 && !strncmp (tokstart, "union", 5))
    {
      return UNION;
    }
  if (namelen == 4 && !strncmp (tokstart, "enum", 4))
    {
      return ENUM;
    }
  if (namelen == 6 && !strncmp (tokstart, "sizeof", 6))
    {
      return SIZEOF;
    }
  if (namelen == 8 && !strncmp (tokstart, "unsigned", 6))
    {
      return UNSIGNED;
    }
  yylval.sval.ptr = tokstart;
  yylval.sval.length = namelen;

  /* Any other names starting in $ are debugger internal variables.  */

  if (*tokstart == '$')
    {
      yylval.ivar = (struct internalvar *) lookup_internalvar (copy_name (yylval.sval) + 1);
      return VARIABLE;
    }

  /* Use token-type TYPENAME for symbols that happen to be defined
     currently as names of types; NAME for other symbols.
     The caller is not constrained to care about the distinction.  */
  if (lookup_typename (copy_name (yylval.sval), expression_context_block, 1))
    return TYPENAME;
  return NAME;
}

static
yyerror ()
{
  error ("Invalid syntax in expression.");
}

/* Return a null-terminated temporary copy of the name
   of a string token.  */

static char *
copy_name (token)
     struct stoken token;
{
  bcopy (token.ptr, namecopy, token.length);
  namecopy[token.length] = 0;
  return namecopy;
}

/* Reverse an expression from suffix form (in which it is constructed)
   to prefix form (in which we can conveniently print or execute it).  */

static void prefixify_subexp ();

static void
prefixify_expression (expr)
     register struct expression *expr;
{
  register int len = sizeof (struct expression) +
				    expr->nelts * sizeof (union exp_element);
  register struct expression *temp;
  register int inpos = expr->nelts, outpos = 0;

  temp = (struct expression *) alloca (len);

  /* Copy the original expression into temp.  */
  bcopy (expr, temp, len);

  prefixify_subexp (temp, expr, inpos, outpos);
}

/* Return the number of exp_elements in the subexpression of EXPR
   whose last exp_element is at index ENDPOS - 1 in EXPR.  */

static int
length_of_subexp (expr, endpos)
     register struct expression *expr;
     register int endpos;
{
  register int oplen = 1;
  register int args = 0;
  register int i;

  i = (int) expr->elts[endpos - 1].opcode;

  switch (i)
    {
    case OP_LONG:
    case OP_DOUBLE:
      oplen = 4;
      break;

    case OP_VAR_VALUE:
    case OP_LAST:
    case OP_REGISTER:
    case OP_INTERNALVAR:
      oplen = 3;
      break;

    case OP_FUNCALL:
      oplen = 3;
      args = 1 + expr->elts[endpos - 2].longconst;
      break;

    case UNOP_CAST:
    case UNOP_MEMVAL:
      oplen = 3;
      args = 1;
      break;

    case STRUCTOP_STRUCT:
    case STRUCTOP_PTR:
      args = 1;
    case OP_STRING:
      oplen = 3 + ((expr->elts[endpos - 2].longconst
		    + sizeof (union exp_element))
		   / sizeof (union exp_element));
		   
      break;

    case TERNOP_COND:
      args = 3;
      break;

    case BINOP_ASSIGN_MODIFY:
      oplen = 3;
      args = 2;
      break;

    default:
      args = 1 + (i < (int) BINOP_END);
    }

  while (args > 0)
    {
      oplen += length_of_subexp (expr, endpos - oplen);
      args--;
    }

  return oplen;
}

/* Copy the subexpression ending just before index INEND in INEXPR
   into OUTEXPR, starting at index OUTBEG.
   In the process, convert it from suffix to prefix form.  */

static void
prefixify_subexp (inexpr, outexpr, inend, outbeg)
     register struct expression *inexpr;
     struct expression *outexpr;
     register int inend;
     int outbeg;
{
  register int oplen = 1;
  register int args = 0;
  register int i;
  int *arglens;
  enum exp_opcode opcode;

  /* Compute how long the last operation is (in OPLEN),
     and also how many preceding subexpressions serve as
     arguments for it (in ARGS).  */

  opcode = inexpr->elts[inend - 1].opcode;
  switch (opcode)
    {
    case OP_LONG:
    case OP_DOUBLE:
      oplen = 4;
      break;

    case OP_VAR_VALUE:
    case OP_LAST:
    case OP_REGISTER:
    case OP_INTERNALVAR:
      oplen = 3;
      break;

    case OP_FUNCALL:
      oplen = 3;
      args = 1 + inexpr->elts[inend - 2].longconst;
      break;

    case UNOP_CAST:
    case UNOP_MEMVAL:
      oplen = 3;
      args = 1;
      break;

    case STRUCTOP_STRUCT:
    case STRUCTOP_PTR:
      args = 1;
    case OP_STRING:
      oplen = 3 + ((inexpr->elts[inend - 2].longconst
		    + sizeof (union exp_element))
		   / sizeof (union exp_element));
		   
      break;

    case TERNOP_COND:
      args = 3;
      break;

    case BINOP_ASSIGN_MODIFY:
      oplen = 3;
      args = 2;
      break;

    default:
      args = 1 + ((int) opcode < (int) BINOP_END);
    }

  /* Copy the final operator itself, from the end of the input
     to the beginning of the output.  */
  inend -= oplen;
  bcopy (&inexpr->elts[inend], &outexpr->elts[outbeg],
	 oplen * sizeof (union exp_element));
  outbeg += oplen;

  /* Find the lengths of the arg subexpressions.  */
  arglens = (int *) alloca (args * sizeof (int));
  for (i = args - 1; i >= 0; i--)
    {
      oplen = length_of_subexp (inexpr, inend);
      arglens[i] = oplen;
      inend -= oplen;
    }

  /* Now copy each subexpression, preserving the order of
     the subexpressions, but prefixifying each one.
     In this loop, inend starts at the beginning of
     the expression this level is working on
     and marches forward over the arguments.
     outbeg does similarly in the output.  */
  for (i = 0; i < args; i++)
    {
      oplen = arglens[i];
      inend += oplen;
      prefixify_subexp (inexpr, outexpr, inend, outbeg);
      outbeg += oplen;
    }
}

/* This page contains the two entry points to this file.  */

/* Read a C expression from the string *STRINGPTR points to,
   parse it, and return a pointer to a  struct expression  that we malloc.
   Use block BLOCK as the lexical context for variable names;
   if BLOCK is zero, use the block of the selected stack frame.
   Meanwhile, advance *STRINGPTR to point after the expression,
   at the first nonwhite character that is not part of the expression
   (possibly a null character).

   If COMMA is nonzero, stop if a comma is reached.  */

struct expression *
parse_c_1 (stringptr, block, comma)
     char **stringptr;
     struct block *block;
{
  struct cleanup *old_chain;

  lexptr = *stringptr;

  comma_terminates = comma;

  if (lexptr == 0 || *lexptr == 0)
    error_no_arg ("expression to compute");

  old_chain = make_cleanup (free_funcalls, 0);
  funcall_chain = 0;

  expression_context_block = block ? block : get_selected_block ();

  namecopy = (char *) alloca (strlen (lexptr) + 1);
  expout_size = 10;
  expout_ptr = 0;
  expout = (struct expression *) xmalloc (sizeof (struct expression)
					  + expout_size * sizeof (union exp_element));
  make_cleanup (free_current_contents, &expout);
  if (yyparse ())
    yyerror ();
  discard_cleanups (old_chain);
  expout->nelts = expout_ptr;
  expout = (struct expression *)
    xrealloc (expout,
	      sizeof (struct expression)
	      + expout_ptr * sizeof (union exp_element));
  prefixify_expression (expout);
  *stringptr = lexptr;
  return expout;
}

/* Parse STRING as an expression, and complain if this fails
   to use up all of the contents of STRING.  */

struct expression *
parse_c_expression (string)
     char *string;
{
  register struct expression *exp;
  exp = parse_c_1 (&string, 0, 0);
  if (*string)
    error ("Junk after end of expression.");
  return exp;
}
