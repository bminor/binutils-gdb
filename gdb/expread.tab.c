
# line 31 "expread.y"
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
static void write_exp_elt2 ();
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

# line 86 "expread.y"
typedef union 
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

    struct type **tvec;
    int *ivec;
  } YYSTYPE;
# define INT 257
# define CHAR 258
# define FLOAT 259
# define NAME 260
# define TYPENAME 261
# define STRING 262
# define STRUCT 263
# define UNION 264
# define ENUM 265
# define SIZEOF 266
# define UNSIGNED 267
# define COLONCOLON 268
# define LAST 269
# define REGNAME 270
# define VARIABLE 271
# define ASSIGN_MODIFY 272
# define THIS 273
# define ABOVE_COMMA 274
# define OR 275
# define AND 276
# define EQUAL 277
# define NOTEQUAL 278
# define LEQ 279
# define GEQ 280
# define LSH 281
# define RSH 282
# define UNARY 283
# define INCREMENT 284
# define DECREMENT 285
# define ARROW 286
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 625 "expread.y"


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

/* To avoid a bug in the Sun 4 compiler, we pass only things that
   can fit into a single register through here.  */
static void
write_exp_elt (expelt)
     /* union exp_element expelt; */
     long expelt;
{
  union exp_element temp;
  temp.longconst = expelt;

  if (expout_ptr >= expout_size)
    {
      expout_size *= 2;
      expout = (struct expression *) xrealloc (expout,
					       sizeof (struct expression)
					       + expout_size * sizeof (union exp_element));
    }
  expout->elts[expout_ptr++] = /* expelt */ temp;
}

/* Things that take more space must come through here.  */
static void
write_exp_elt2 (expelt)
     double expelt;
{
  union exp_element temp;
  temp.doubleconst = expelt;

  if (expout_ptr >= expout_size)
    {
      expout_size *= 2;
      expout = (struct expression *) xrealloc (expout,
					       sizeof (struct expression)
					       + expout_size * sizeof (union exp_element));
    }
  expout->elts[expout_ptr++] = temp;
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
  if (namelen == 5)
    {
      if (!strncmp (tokstart, "union", 5))
	{
	  return UNION;
	}
    }
  if (namelen == 4)
    {
      if (!strncmp (tokstart, "enum", 4))
	{
	  return ENUM;
	}
      if (!strncmp (tokstart, "this", 4))
	{
	  return THIS;
	}
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

  if (endpos < 0)
    error ("?error in length_of_subexp");

  i = (int) expr->elts[endpos - 1].opcode;

  switch (i)
    {
      /* C++  */
    case OP_SCOPE:
      oplen = 4 + ((expr->elts[endpos - 2].longconst
		    + sizeof (union exp_element))
		   / sizeof (union exp_element));
      break;

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

      /* C++ */
    case OP_THIS:
      oplen = 2;
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
      /* C++  */
    case OP_SCOPE:
      oplen = 4 + ((inexpr->elts[inend - 2].longconst
		    + sizeof (union exp_element))
		   / sizeof (union exp_element));
      break;

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

      /* C++ */
    case OP_THIS:
      oplen = 2;
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
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 26,
	268, 79,
	-2, 64,
-1, 124,
	268, 60,
	-2, 61,
	};
# define YYNPROD 81
# define YYLAST 696
short yyact[]={

  43,  54, 140,  39, 123,  41,  44, 120,  45,  37,
  42,  78,  77,  73,  39,  29,  30,  31, 142,  32,
  37, 131,  33,  52,  60,  53,  59,  40,  80,  81,
 143, 148,  33, 135, 149,  43,  54,  59,  39, 135,
  41,  44,  71,  45,  37,  42, 137, 129, 122, 138,
  90,  33, 144,  92,  38,  23,  74,  55,  52,  60,
  53,  59,  40, 146, 118,  38, 119,  88, 117,  17,
  43,  54,   1,  39,   0,  41,  44,   0,  45,  37,
  42, 128, 118,   0, 119,   0, 117,  56,   0,  38,
   0,   0,  55,  52,   0,  53,  59,  40,   7,   0,
   0,  75,   2,   5,   0,  13,   0,   4,   0,   0,
   6,  43,  54, 115,  39,   0,  41,  44,   0,  45,
  37,  42,  56,   0,  38,   0, 118,  55, 119, 132,
 117,   0,   0,   0,  52,   0,  53,  59,  40,   7,
  91,   0,   0,   0,   5,   0,  70,   0,   4,   0,
   0,   6,   0,  43,  54,   0,  39,  56,  41,  44,
   0,  45,  37,  42,   0,  38,   0,   0,  55, 116,
 118,   0, 119, 121, 117,   0,  52,   0,  53,  59,
  40,   0,   0,   0,   0,   0,   0, 147,  12,   0,
   0,   8, 150,   0, 145,   0,  43,  54,  56,  39,
   0,  41,  44,   0,  45,  37,  42,  38,   0,   0,
  55,   0,   0,   0,   0,   0,   0,   0,   0,  52,
   0,  53,  59,  40,   0,   0,   0,   0,   0,  12,
  39,   0,   8,   0,   0,  61,  37,   0,  58,  57,
  48,  49,  50,  51,  46,  47,   0,  34,  35,  36,
  38,  80,  81,  59,  40,   0,   0,   0,  34,  35,
  36,   0,  43,   0,   0,  39,   0,  41,  80,  81,
  61,  37,  42,  58,  57,  48,  49,  50,  51,  46,
  47,  38,  34,  35,  36,  80,  81,   0,  59,  40,
   0,   0,   0,   0,  43,   0,   0,  39,   0,  41,
  44,   0,  45,  37,  42,   0,   0,   0,   0,  57,
  48,  49,  50,  51,  46,  47,  38,  34,  35,  36,
  59,  40,  14,  15,  16,  26,  28,  21,  29,  30,
  31,  11,  32,  25,  18,  19,  20,   0,  22,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  38,   9,
  10,  48,  49,  50,  51,  46,  47,   0,  34,  35,
  36,   0,   0,  14,  15,  16,  26,  28,  21,  29,
  30,  31,  11,  32,  25,  18,  19,  20,  43,  22,
   0,  39,   0,  41,  44,   0,  45,  37,  42,   0,
   9,  10,   0,  48,  49,  50,  51,  46,  47,   0,
  34,  35,  36,  43,  59,  40,  39,   0,  41,  44,
  24,  45,  37,  42,  73,   0,  29,  30,  31,   0,
  32,   0,   0,  72,  76,   0,  52,   0,  53,  59,
  40,   0,  38,   0,   0,   0,  48,  49,  50,  51,
  46,  47,   0,  34,  35,  36,  43,   0,   0,  39,
   0,  41,  44,  27,  45,  37,  42,  38,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  52,
   0,  53,  59,  40,  34,  35,  36,   0,   0,  79,
   0,  76,   0,  82,  83,  84,  85,   0,   0,   0,
  87,  89,   0,   0,   0,   0,   0,   0,   0,   0,
  38,   0,   0,   0,   0,   0,   0,   0,   0,  34,
  35,  36,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 134, 124, 125,   0,   0,   0,   0,   0,  46,  47,
   3,  34,  35,  36,   0,  62,  63,  64,  65,  66,
  67,  68,  69,   0,   0,  72,   0,   0,   0,   0,
  72,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,  86,   0,   0, 125,   0,   0,
   0,  93,  94,  95,  96,  97,  98,  99, 100, 101,
 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
 112, 113, 114,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,  34,  35,  36,   0, 126,
   0, 127,   0, 130,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  48,  49,  50,  51,  46,  47,   0,
  34,  35,  36,   0,   0,   0,   0, 133,   0,   0,
   0,   0, 136,   0,   0,   0,   0,   0,   0,   0,
   0,   0, 139, 136,   0,   0,   0,   0,   0, 141,
   0,   0,   0,   0,   0,   0,   0,   0,  50,  51,
  46,  47,   0,  34,  35,  36 };
short yypact[]={

  65,-1000, -22,  -2,  65,  65,  65,  65,  65,  65,
  65, 106,-248,  65,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-256,-257,-232,-1000,-1000,-1000,-232,
-232,-232,-232,  65,-1000,-1000,  25,   8,  65,-1000,
  65,  65,  65,  65,  65,  65,  65,  65,  65,  65,
  65,  65,  65,  65,  65,  65,  65,  65,  65,  65,
  65,  65, -26, -26, -26, -26, -26, -26, -26, -26,
  65,  44,-261,-1000, 132,   7,-264,-232,-232,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,  -2,-1000,  65,-1000,
  65, -12,  65, -26, 190, 190, 190, 225, 225, 341,
 341, 409, 409, 257, 257, 257, 257, 366, 159, 116,
  74,  33, -37,  -2,  -2,  88,  65,-1000,-1000,-248,
  -3,  65,-1000,  -9,-1000,-1000, 190, 190,-1000,   5,
  -2,  65,  65, -26,-266,-1000, -26,-1000,  65,  -2,
 -24,  -2, -11,  12, 153,-1000, -10,  26,-1000,-248,
  26 };
short yypgo[]={

   0, 540, 101,  72,  69,  42, 410,  63,  55, 453,
  53,  47 };
short yyr1[]={

   0,   3,   2,   2,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,  10,
   1,  11,  11,  11,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   8,
   8,   4,   4,   4,   4,   5,   5,   5,   5,   5,
   5,   5,   6,   6,   6,   6,   6,   7,   7,   9,
   9 };
short yyr2[]={

   0,   1,   1,   3,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   3,   4,   3,   4,   4,   0,
   5,   0,   1,   3,   4,   4,   3,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   5,   3,   3,   1,
   1,   1,   1,   1,   1,   1,   4,   1,   1,   1,
   3,   3,   3,   2,   1,   1,   2,   2,   3,   6,
   8,   9,   1,   2,   2,   2,   2,   1,   3,   1,
   1 };
short yychk[]={

-1000,  -3,  -2,  -1,  42,  38,  45,  33, 126, 284,
 285, 266, 123,  40, 257, 258, 259,  -4, 269, 270,
 271, 262, 273,  -8,  -6, 268, 260,  -9, 261, 263,
 264, 265, 267,  44, 284, 285, 286,  46,  91,  40,
  64,  42,  47,  37,  43,  45, 281, 282, 277, 278,
 279, 280,  60,  62,  38,  94, 124, 276, 275,  63,
  61, 272,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  40,  -5,  -6, 261,  -5,  -2,  -6, 268, 268,  -9,
 260, 261,  -9,  -9,  -9,  -9,  -1,  -9,  42,  -9,
  42,  -2, -10,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  -1,  -1,  -1,  -1,  -1,  -5, 125,  42,  38,  40,
 268,  41,  41, 268,  -9,  -9,  -1,  -1,  93, -11,
  -1,  58,  41,  -1,  -6,  42,  -1,  41,  44,  -1,
 268,  -1,  42,  41,  40,  41,  -7,  -5,  41,  44,
  -5 };
short yydef[]={

   0,  -2,   1,   2,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,  49,  50,  51,  52,  53,  54,
  55,  57,  58,   0,   0,   0,  -2,  59,  72,   0,
   0,   0,   0,   0,  11,  12,   0,   0,   0,  19,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   4,   5,   6,   7,   8,   9,  10,  13,
   0,   0,  65,  72,   0,   0,  65,   0,   0,  63,
  79,  80,  73,  74,  75,  76,   3,  14,   0,  16,
   0,   0,  21,  27,  28,  29,  30,  31,  32,  33,
  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,
  44,  45,   0,  47,  48,   0,   0,  66,  67,   0,
   0,   0,  26,   0,  -2,  62,  15,  17,  18,   0,
  22,   0,  56,  24,   0,  68,  25,  20,   0,  46,
   0,  23,   0,  69,   0,  70,   0,  77,  71,   0,
  78 };
#ifndef lint
static	char yaccpar_sccsid[] = "@(#)yaccpar 1.6 88/02/08 SMI"; /* from UCB 4.1 83/02/11 */
#endif

#
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps>= &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 3:
# line 159 "expread.y"
{ write_exp_elt (BINOP_COMMA); } break;
case 4:
# line 164 "expread.y"
{ write_exp_elt (UNOP_IND); } break;
case 5:
# line 167 "expread.y"
{ write_exp_elt (UNOP_ADDR); } break;
case 6:
# line 170 "expread.y"
{ write_exp_elt (UNOP_NEG); } break;
case 7:
# line 174 "expread.y"
{ write_exp_elt (UNOP_ZEROP); } break;
case 8:
# line 178 "expread.y"
{ write_exp_elt (UNOP_LOGNOT); } break;
case 9:
# line 182 "expread.y"
{ write_exp_elt (UNOP_PREINCREMENT); } break;
case 10:
# line 186 "expread.y"
{ write_exp_elt (UNOP_PREDECREMENT); } break;
case 11:
# line 190 "expread.y"
{ write_exp_elt (UNOP_POSTINCREMENT); } break;
case 12:
# line 194 "expread.y"
{ write_exp_elt (UNOP_POSTDECREMENT); } break;
case 13:
# line 198 "expread.y"
{ write_exp_elt (UNOP_SIZEOF); } break;
case 14:
# line 202 "expread.y"
{ write_exp_elt (STRUCTOP_PTR);
			  write_exp_string (yypvt[-0].sval);
			  write_exp_elt (STRUCTOP_PTR); } break;
case 15:
# line 208 "expread.y"
{ write_exp_elt (STRUCTOP_MPTR); } break;
case 16:
# line 212 "expread.y"
{ write_exp_elt (STRUCTOP_STRUCT);
			  write_exp_string (yypvt[-0].sval);
			  write_exp_elt (STRUCTOP_STRUCT); } break;
case 17:
# line 218 "expread.y"
{ write_exp_elt (STRUCTOP_MEMBER); } break;
case 18:
# line 222 "expread.y"
{ write_exp_elt (BINOP_SUBSCRIPT); } break;
case 19:
# line 228 "expread.y"
{ start_arglist (); } break;
case 20:
# line 230 "expread.y"
{ write_exp_elt (OP_FUNCALL);
			  write_exp_elt (end_arglist ());
			  write_exp_elt (OP_FUNCALL); } break;
case 22:
# line 239 "expread.y"
{ arglist_len = 1; } break;
case 23:
# line 243 "expread.y"
{ arglist_len++; } break;
case 24:
# line 247 "expread.y"
{ write_exp_elt (UNOP_MEMVAL);
			  write_exp_elt (yypvt[-2].tval);
			  write_exp_elt (UNOP_MEMVAL); } break;
case 25:
# line 253 "expread.y"
{ write_exp_elt (UNOP_CAST);
			  write_exp_elt (yypvt[-2].tval);
			  write_exp_elt (UNOP_CAST); } break;
case 26:
# line 259 "expread.y"
{ } break;
case 27:
# line 265 "expread.y"
{ write_exp_elt (BINOP_REPEAT); } break;
case 28:
# line 269 "expread.y"
{ write_exp_elt (BINOP_MUL); } break;
case 29:
# line 273 "expread.y"
{ write_exp_elt (BINOP_DIV); } break;
case 30:
# line 277 "expread.y"
{ write_exp_elt (BINOP_REM); } break;
case 31:
# line 281 "expread.y"
{ write_exp_elt (BINOP_ADD); } break;
case 32:
# line 285 "expread.y"
{ write_exp_elt (BINOP_SUB); } break;
case 33:
# line 289 "expread.y"
{ write_exp_elt (BINOP_LSH); } break;
case 34:
# line 293 "expread.y"
{ write_exp_elt (BINOP_RSH); } break;
case 35:
# line 297 "expread.y"
{ write_exp_elt (BINOP_EQUAL); } break;
case 36:
# line 301 "expread.y"
{ write_exp_elt (BINOP_NOTEQUAL); } break;
case 37:
# line 305 "expread.y"
{ write_exp_elt (BINOP_LEQ); } break;
case 38:
# line 309 "expread.y"
{ write_exp_elt (BINOP_GEQ); } break;
case 39:
# line 313 "expread.y"
{ write_exp_elt (BINOP_LESS); } break;
case 40:
# line 317 "expread.y"
{ write_exp_elt (BINOP_GTR); } break;
case 41:
# line 321 "expread.y"
{ write_exp_elt (BINOP_LOGAND); } break;
case 42:
# line 325 "expread.y"
{ write_exp_elt (BINOP_LOGXOR); } break;
case 43:
# line 329 "expread.y"
{ write_exp_elt (BINOP_LOGIOR); } break;
case 44:
# line 333 "expread.y"
{ write_exp_elt (BINOP_AND); } break;
case 45:
# line 337 "expread.y"
{ write_exp_elt (BINOP_OR); } break;
case 46:
# line 341 "expread.y"
{ write_exp_elt (TERNOP_COND); } break;
case 47:
# line 345 "expread.y"
{ write_exp_elt (BINOP_ASSIGN); } break;
case 48:
# line 349 "expread.y"
{ write_exp_elt (BINOP_ASSIGN_MODIFY);
			  write_exp_elt (yypvt[-1].opcode);
			  write_exp_elt (BINOP_ASSIGN_MODIFY); } break;
case 49:
# line 355 "expread.y"
{ write_exp_elt (OP_LONG);
			  write_exp_elt (builtin_type_long);
			  write_exp_elt (yypvt[-0].lval);
			  write_exp_elt (OP_LONG); } break;
case 50:
# line 362 "expread.y"
{ write_exp_elt (OP_LONG);
			  write_exp_elt (builtin_type_char);
			  write_exp_elt (yypvt[-0].lval);
			  write_exp_elt (OP_LONG); } break;
case 51:
# line 369 "expread.y"
{ write_exp_elt (OP_DOUBLE);
			  write_exp_elt (builtin_type_double);
			  write_exp_elt2 (yypvt[-0].dval);
			  write_exp_elt (OP_DOUBLE); } break;
case 53:
# line 379 "expread.y"
{ write_exp_elt (OP_LAST);
			  write_exp_elt (yypvt[-0].lval);
			  write_exp_elt (OP_LAST); } break;
case 54:
# line 385 "expread.y"
{ write_exp_elt (OP_REGISTER);
			  write_exp_elt (yypvt[-0].lval);
			  write_exp_elt (OP_REGISTER); } break;
case 55:
# line 391 "expread.y"
{ write_exp_elt (OP_INTERNALVAR);
			  write_exp_elt (yypvt[-0].ivar);
			  write_exp_elt (OP_INTERNALVAR); } break;
case 56:
# line 397 "expread.y"
{ write_exp_elt (OP_LONG);
			  write_exp_elt (builtin_type_int);
			  write_exp_elt ((long) TYPE_LENGTH (yypvt[-1].tval));
			  write_exp_elt (OP_LONG); } break;
case 57:
# line 404 "expread.y"
{ write_exp_elt (OP_STRING);
			  write_exp_string (yypvt[-0].sval);
			  write_exp_elt (OP_STRING); } break;
case 58:
# line 411 "expread.y"
{ write_exp_elt (OP_THIS);
			  write_exp_elt (OP_THIS); } break;
case 59:
# line 418 "expread.y"
{
			  struct symtab *tem = lookup_symtab (copy_name (yypvt[-0].sval));
			  struct symbol *sym;
			  
			  if (tem)
			    yyval.bval = BLOCKVECTOR_BLOCK (BLOCKVECTOR (tem), 1);
			  else
			    {
			      sym = lookup_symbol (copy_name (yypvt[-0].sval),
						   expression_context_block,
						   VAR_NAMESPACE);
			      if (sym && SYMBOL_CLASS (sym) == LOC_BLOCK)
				yyval.bval = SYMBOL_BLOCK_VALUE (sym);
			      else
				error ("No file or function \"%s\".",
				       copy_name (yypvt[-0].sval));
			    }
			} break;
case 60:
# line 439 "expread.y"
{
			  struct symbol *tem
			    = lookup_symbol (copy_name (yypvt[-0].sval), yypvt[-2].bval, VAR_NAMESPACE);
			  if (!tem || SYMBOL_CLASS (tem) != LOC_BLOCK)
			    error ("No function \"%s\" in specified context.",
				   copy_name (yypvt[-0].sval));
			  yyval.bval = SYMBOL_BLOCK_VALUE (tem);
			} break;
case 61:
# line 450 "expread.y"
{
			  struct symbol *sym;
			  sym = lookup_symbol (copy_name (yypvt[-0].sval), yypvt[-2].bval, VAR_NAMESPACE);
			  if (sym == 0)
			    error ("No symbol \"%s\" in specified context.",
				   copy_name (yypvt[-0].sval));
			  write_exp_elt (OP_VAR_VALUE);
			  write_exp_elt (sym);
			  write_exp_elt (OP_VAR_VALUE);
			} break;
case 62:
# line 463 "expread.y"
{
			  struct type *type = yypvt[-2].tval;
			  if (TYPE_CODE (type) != TYPE_CODE_STRUCT
			      && TYPE_CODE (type) != TYPE_CODE_UNION)
			    error ("`%s' is not defined as an aggregate type.",
				   TYPE_NAME (type));

			  write_exp_elt (OP_SCOPE);
			  write_exp_elt (type);
			  write_exp_string (yypvt[-0].sval);
			  write_exp_elt (OP_SCOPE);
			} break;
case 63:
# line 476 "expread.y"
{
			  char *name = copy_name (yypvt[-0].sval);
			  struct symbol *sym;
			  int i;

			  sym = lookup_symbol_2 (name, 0, VAR_NAMESPACE);
			  if (sym)
			    {
			      write_exp_elt (OP_VAR_VALUE);
			      write_exp_elt (sym);
			      write_exp_elt (OP_VAR_VALUE);
			      break;
			    }
			  for (i = 0; i < misc_function_count; i++)
			    if (!strcmp (misc_function_vector[i].name, name))
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
			      error ("No symbol \"%s\" in current context.", name);
			} break;
case 64:
# line 512 "expread.y"
{ struct symbol *sym;
			  sym = lookup_symbol_1 (copy_name (yypvt[-0].sval),
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
			      register char *arg = copy_name (yypvt[-0].sval);
			      register int i;
			      int v, val;
			      /* C++: see if it hangs off of `this'.  Must
			         not inadvertently convert from a method call
				 to data ref.  */
			      v = (int)value_of_this (0);
			      if (v)
				{
				  val = check_field (v, arg);
				  if (val)
				    {
				      write_exp_elt (OP_THIS);
				      write_exp_elt (OP_THIS);
				      write_exp_elt (STRUCTOP_PTR);
				      write_exp_string (yypvt[-0].sval);
				      write_exp_elt (STRUCTOP_PTR);
				      break;
				    }
				}
			      sym = lookup_symbol_2 (arg, 0, VAR_NAMESPACE);
			      if (sym)
				{
				  write_exp_elt (OP_VAR_VALUE);
				  write_exp_elt (sym);
				  write_exp_elt (OP_VAR_VALUE);
				  break; /* YACC-dependent */
				}
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
					 copy_name (yypvt[-0].sval));
			    }
			} break;
case 66:
# line 578 "expread.y"
{ yyval.tval = lookup_pointer_type (yypvt[-1].tval); } break;
case 67:
# line 580 "expread.y"
{ yyval.tval = lookup_reference_type (yypvt[-1].tval); } break;
case 68:
# line 582 "expread.y"
{ yyval.tval = lookup_member_type (builtin_type_int, yypvt[-2].tval); } break;
case 69:
# line 584 "expread.y"
{ yyval.tval = lookup_member_type (yypvt[-5].tval, yypvt[-3].tval); } break;
case 70:
# line 586 "expread.y"
{ yyval.tval = lookup_member_type (lookup_function_type (yypvt[-7].tval, 0), yypvt[-5].tval); } break;
case 71:
# line 588 "expread.y"
{ yyval.tval = lookup_member_type (lookup_function_type (yypvt[-8].tval, yypvt[-1].tvec), yypvt[-6].tval);
			  free (yypvt[-1].tvec); } break;
case 72:
# line 594 "expread.y"
{ yyval.tval = lookup_typename (copy_name (yypvt[-0].sval),
						expression_context_block, 0); } break;
case 73:
# line 597 "expread.y"
{ yyval.tval = lookup_struct (copy_name (yypvt[-0].sval),
					      expression_context_block); } break;
case 74:
# line 600 "expread.y"
{ yyval.tval = lookup_union (copy_name (yypvt[-0].sval),
					     expression_context_block); } break;
case 75:
# line 603 "expread.y"
{ yyval.tval = lookup_enum (copy_name (yypvt[-0].sval),
					    expression_context_block); } break;
case 76:
# line 606 "expread.y"
{ yyval.tval = lookup_unsigned_typename (copy_name (yypvt[-0].sval)); } break;
case 77:
# line 611 "expread.y"
{ yyval.tvec = (struct type **)xmalloc (sizeof (struct type *) * 2);
		  yyval.tvec[0] = (struct type *)0;
		  yyval.tvec[1] = yypvt[-0].tval;
		} break;
case 78:
# line 616 "expread.y"
{ int len = sizeof (struct type *) * ++(yypvt[-2].ivec[0]);
		  yyval.tvec = (struct type **)xrealloc (yypvt[-2].tvec, len);
		  yyval.tvec[yyval.ivec[0]] = yypvt[-0].tval;
		} break;
		}
		goto yystack;  /* stack new state and value */

	}
