/* gasp.c - Gnu assembler preprocessor main program.
   Copyright (C) 1994 Free Software Foundation, Inc.

   Written by Steve and Judy Chamberlain of Cygnus Support,
      sac@cygnus.com

   This file is part of GASP, the GNU Assembler Preprocessor.

   GASP is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GASP is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GASP; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

/*

This program translates the input macros and stuff into a form
suitable for gas to consume.


  gasp [-sdhau] [-c char] [-o <outfile>] <infile>*

  -s copy source to output
  -c <char> comments are started with <char> instead of !
  -u allow unreasonable stuff
  -p print line numbers
  -d print debugging stats
  -s semi colons start comments
  -a use alternate syntax
     Pseudo ops can start with or without a .
     Labels have to be in first column.
    Macro arg parameters subsituted by name, don't need the &.
     String can start with ' too.
     Strings can be surrounded by <..>
     A %<exp> in a string evaluates the expression 
     Literal char in a string with !


*/


#include <stdio.h>
#include <getopt.h>
#include <ctype.h>
#include "config.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef NEED_MALLOC_DECLARATION
extern char *malloc ();
#endif

#include "libiberty.h"

char *program_version = "1.2";

#define MAX_INCLUDES 30		/* Maximum include depth */
#define MAX_REASONABLE 1000	/* Maximum number of expansions */

int unreasonable;		/* -u on command line */
int stats;			/* -d on command line */
int print_line_number;		/* -p flag on command line */
int copysource;			/* -c flag on command line */
int warnings;			/* Number of WARNINGs generated so far. */
int errors;			/* Number of ERRORs generated so far. */
int fatals;			/* Number of fatal ERRORs generated so far (either 0 or 1). */
int alternate = 0;              /* -a on command line */
char comment_char = '!';
int radix = 10;			/* Default radix */

int had_end; /* Seen .END */

/* The output stream */
FILE *outfile;


/* Forward declarations. */
static int condass_lookup_name();
static int condass_on();
static int get();
static int get_and_process();
static int get_token();
static int getstring();
static int include_next_index();
static int macro_op();
static int linecount();
static int process_pseudo_op();
static void include_pop();
static void include_print_where_line();
/* string blocks

   I had a couple of choices when deciding upon this data structure.
   gas uses null terminated strings for all its internal work.  This
   often means that parts of the program that want to examine
   substrings have to manipulate the data in the string to do the
   right thing (a common operation is to single out a bit of text by
   saving away the character after it, nulling it out, operating on
   the substring and then replacing the character which was under the
   null).  This is a pain and I remember a load of problems that I had with
   code in gas which almost got this right.  Also, it's harder to grow and
   allocate null terminated strings efficiently.

   Obstacks provide all the functionality needed, but are too
   complicated, hence the sb.

   An sb is allocated by the caller, and is initialzed to point to an
   sb_element.  sb_elements are kept on a free lists, and used when
   needed, replaced onto the free list when unused.
 */

#define max_power_two    30	/* don't allow strings more than
			           2^max_power_two long */
/* structure of an sb */
typedef struct sb
  {
    char *ptr;			/* points to the current block. */
    int len;			/* how much is used. */
    int pot;			/* the maximum length is 1<<pot */
    struct le *item;
  }
sb;

/* Structure of the free list object of an sb */
typedef struct le
  {
    struct le *next;
    int size;
    char data[1];
  }
sb_element;

/* The free list */
typedef struct
  {
    sb_element *size[max_power_two];
  } sb_list_vector;

sb_list_vector free_list;

int string_count[max_power_two];

/* the attributes of each character are stored as a bit pattern
   chartype, which gives us quick tests. */


#define FIRSTBIT 1
#define NEXTBIT  2
#define SEPBIT   4
#define WHITEBIT 8
#define COMMENTBIT 16
#define BASEBIT  32
#define ISCOMMENTCHAR(x) (chartype[(unsigned)(x)] & COMMENTBIT)
#define ISFIRSTCHAR(x)  (chartype[(unsigned)(x)] & FIRSTBIT)
#define ISNEXTCHAR(x)   (chartype[(unsigned)(x)] & NEXTBIT)
#define ISSEP(x)        (chartype[(unsigned)(x)] & SEPBIT)
#define ISWHITE(x)      (chartype[(unsigned)(x)] & WHITEBIT)
#define ISBASE(x)       (chartype[(unsigned)(x)] & BASEBIT)
static char chartype[256];


/* Conditional assembly uses the `ifstack'.  Each aif pushes another
   entry onto the stack, and sets the on flag if it should.  The aelse
   sets hadelse, and toggles on.  An aend pops a level.  We limit to
   100 levels of nesting, not because we're facists pigs with read
   only minds, but because more than 100 levels of nesting is probably
   a bug in the user's macro structure.  */

#define IFNESTING 100
struct
  {
    int on;			/* is the level being output */
    int hadelse;		/* has an aelse been seen */
  }
ifstack[IFNESTING];
int ifi;

/* The final and intermediate results of expression evaluation are kept in
   exp_t's.  Note that a symbol is not an sb, but a pointer into the input
   line.  It must be coped somewhere safe before the next line is read in. */

typedef struct
  {
    char *name;
    int len;
  }
symbol;

typedef struct
  {
    int value;			/* constant part */
    symbol add_symbol;		/* name part */
    symbol sub_symbol;		/* name part */
  }
exp_t;


/* Hashing is done in a pretty standard way.  A hash_table has a
   pointer to a vector of pointers to hash_entrys, and the size of the
   vector.  A hash_entry contains a union of all the info we like to
   store in hash table.  If there is a hash collision, hash_entries
   with the same hash are kept in a chain. */

/* What the data in a hash_entry means */
typedef enum
  {
    hash_integer,		/* name->integer mapping */
    hash_string,		/* name->string mapping */
    hash_macro,			/* name is a macro */
    hash_formal			/* name is a formal argument */
  } hash_type;

typedef struct hs
  {
    sb key;			/* symbol name */
    hash_type type;		/* symbol meaning */
    union
      {
	sb s;
	int i;
	struct macro_struct *m;
	struct formal_struct *f;
      } value;
    struct hs *next;		/* next hash_entry with same hash key */
  } hash_entry;

typedef struct
  {
    hash_entry **table;
    int size;
  } hash_table;


/* Structures used to store macros. 

   Each macro knows its name and included text.  It gets built with a
   list of formal arguments, and also keeps a hash table which points
   into the list to speed up formal search.  Each formal knows its
   name and its default value.  Each time the macro is expanded, the
   formals get the actual values attatched to them. */

/* describe the formal arguments to a macro */

typedef struct formal_struct
  {
    struct formal_struct *next;	/* next formal in list */
    sb name;			/* name of the formal */
    sb def;			/* the default value */
    sb actual;			/* the actual argument (changed on each expansion) */
    int index;			/* the index of the formal 0..formal_count-1 */
  }
formal_entry;

/* describe the macro. */

typedef struct macro_struct
  {
    sb sub;			/* substitution text. */
    int formal_count;		/* number of formal args. */
    formal_entry *formals;	/* pointer to list of formal_structs */
    hash_table formal_hash;	/* hash table of formals. */
  }
macro_entry;

/* how we nest files and expand macros etc.

   we keep a stack of of include_stack structs.  each include file
   pushes a new level onto the stack.  we keep an sb with a pushback
   too.  unget chars are pushed onto the pushback sb, getchars first
   checks the pushback sb before reading from the input stream.

   small things are expanded by adding the text of the item onto the
   pushback sb.  larger items are grown by pushing a new level and
   allocating the entire pushback buf for the item.  each time
   something like a macro is expanded, the stack index is changed. we
   can then perform an exitm by popping all entries off the stack with
   the same stack index.  if we're being reasonable, we can detect
   recusive expansion by checking the index is reasonably small.
 */

typedef enum
  {
    include_file, include_repeat, include_while, include_macro
  } include_type;

struct include_stack
  {
    sb pushback;		/* current pushback stream */
    int pushback_index;		/* next char to read from stream */
    FILE *handle;		/* open file */
    sb name;			/* name of file */
    int linecount;		/* number of lines read so far */
    include_type type;
    int index;			/* index of this layer */
  }
include_stack[MAX_INCLUDES];

struct include_stack *sp;
#define isp (sp - include_stack)

#define dsize 5


void include_print_where_line ();


#define FATAL(x) \
  do { include_print_where_line (stderr); fprintf x ; fatals++; quit(); } while(0) 
#define ERROR(x) \
  do { include_print_where_line (stderr); fprintf x; errors++; } while(0)
#define WARNING(x) \
  do { include_print_where_line (stderr); fprintf x; warnings++;} while(0) 



/* exit the program and return the right ERROR code. */
void
quit ()
{
  int exitcode;
  if (fatals + errors)
    exitcode = 1;
  else
    exitcode = 0;

  if (stats) 
    {
      int i;
      for (i = 0; i < max_power_two; i++) 
	{
	  fprintf (stderr, "strings size %8d : %d\n", 1<<i, string_count[i]);
	}
    }
  exit (exitcode);
}


/* this program is about manipulating strings.
   they are managed in things called `sb's which is an abbreviation
   for string buffers.  an sb has to be created, things can be glued
   on to it, and at the end of it's life it should be freed.  the
   contents should never be pointed at whilst it is still growing,
   since it could be moved at any time

   eg:
   sb_new (&foo);
   sb_grow... (&foo,...);
   use foo->ptr[*];
   sb_kill (&foo);

*/

/* initializes an sb. */

void
sb_build (ptr, size)
     sb *ptr;
     int size;
{
  /* see if we can find one to allocate */
  sb_element *e;

  if (size > max_power_two)
    {
      FATAL ((stderr, "string longer than %d bytes requested.\n",
	      1 << max_power_two));
    }
  e = free_list.size[size];
  if (!e)
    {
      /* nothing there, allocate one and stick into the free list */
      e = (sb_element *) xmalloc (sizeof (sb_element) + (1 << size));
      e->next = free_list.size[size];
      e->size = 1 << size;
      free_list.size[size] = e;
      string_count[size]++;
    }

  /* remove from free list */

  free_list.size[size] = e->next;

  /* copy into callers world */
  ptr->ptr = e->data;
  ptr->pot = size;
  ptr->len = 0;
  ptr->item = e;
}


static void
sb_new (ptr)
     sb *ptr;
{
  sb_build (ptr, dsize);
}

/* deallocate the sb at ptr */

static
void
sb_kill (ptr)
     sb *ptr;
{
  /* return item to free list */
  ptr->item->next = free_list.size[ptr->pot];
  free_list.size[ptr->pot] = ptr->item;
}

/* add the sb at s to the end of the sb at ptr */

static void sb_check ();

static
void
sb_add_sb (ptr, s)
     sb *ptr;
     sb *s;
{
  sb_check (ptr, s->len);
  memcpy (ptr->ptr + ptr->len, s->ptr, s->len);
  ptr->len += s->len;
}

/* make sure that the sb at ptr has room for another len characters,
   and grow it if it doesn't. */

static void
sb_check (ptr, len)
     sb *ptr;
     int len;
{
  if (ptr->len + len >= 1 << ptr->pot)
    {
      sb tmp;
      int pot = ptr->pot;
      while (ptr->len + len >= 1 << pot)
	pot++;
      sb_build (&tmp, pot);
      sb_add_sb (&tmp, ptr);
      sb_kill (ptr);
      *ptr = tmp;
    }
}

/* make the sb at ptr point back to the beginning.  */

static void
sb_reset (ptr)
     sb *ptr;
{
  ptr->len = 0;
}

/* add character c to the end of the sb at ptr. */

void
sb_add_char (ptr, c)
     sb *ptr;
     char c;
{
  sb_check (ptr, 1);
  ptr->ptr[ptr->len++] = c;
}

/* add null terminated string s to the end of sb at ptr. */

static void
sb_add_string (ptr, s)
     sb *ptr;
     char *s;
{
  int len = strlen (s);
  sb_check (ptr, len);
  memcpy (ptr->ptr + ptr->len, s, len);
  ptr->len += len;
}

/* add string at s of length len to sb at ptr */

static void
sb_add_buffer (ptr, s, len)
     sb *ptr;
     char *s;
     int len;
{
  sb_check (ptr, len);
  memcpy (ptr->ptr + ptr->len, s, len);
  ptr->len += len;
}


/* print the sb at ptr to the output file */

static
void
sb_print (ptr)
     sb *ptr;
{
  int i;
  int nc = 0;

  for (i = 0; i < ptr->len; i++)
    {
      if (nc)
	{
	  fprintf (outfile, ",");
	}
      fprintf (outfile, "%d", ptr->ptr[i]);
      nc = 1;
    }
}

static 
void 
sb_print_at (idx, ptr)
int idx;
sb *ptr;
{
  int i;
  for (i = idx; i < ptr->len; i++)
    putc (ptr->ptr[i], outfile);
}
/* put a null at the end of the sb at in and return the start of the
   string, so that it can be used as an arg to printf %s. */

static
char *
sb_name (in)
     sb *in;
{
  /* stick a null on the end of the string */
  sb_add_char (in, 0);
  return in->ptr;
}

/* start at the index idx into the string in sb at ptr and skip
   whitespace. return the index of the first non whitespace character */

static int
sb_skip_white (idx, ptr)
     int idx;
     sb *ptr;
{
  while (idx < ptr->len && ISWHITE (ptr->ptr[idx]))
    idx++;
  return idx;
}

/* start at the index idx into the sb at ptr. skips whitespace,
   a comma and any following whitespace. returnes the index of the
   next character. */

static int
sb_skip_comma (idx, ptr)
     int idx;
     sb *ptr;
{
  while (idx < ptr->len && ISWHITE (ptr->ptr[idx]))
    idx++;

  if (idx < ptr->len
      && ptr->ptr[idx] == ',')
    idx++;

  while (idx < ptr->len && ISWHITE (ptr->ptr[idx]))
    idx++;

  return idx;
}


/* hash table maintenance. */

/* build a new hash table with size buckets, and fill in the info at ptr. */

static void
hash_new_table (size, ptr)
     int size;
     hash_table *ptr;
{
  int i;
  ptr->size = size;
  ptr->table = (hash_entry **) xmalloc (size * (sizeof (hash_entry *)));
  /* Fill with null-pointer, not zero-bit-pattern.  */
  for (i = 0; i < size; i++)
    ptr->table[i] = 0;
}

/* calculate and return the hash value of the sb at key. */

static int
hash (key)
     sb *key;
{
  int k = 0x1234;
  int i;
  char *p = key->ptr;
  for (i = 0; i < key->len; i++)
    {
      k ^= (k << 2) ^ *p;
      p++;
    }
  return k & 0xf0fff;
}

/* lookup key in hash_table tab, if present, then return it, otherwise
   build a new one and fill it with hash_integer. */

static
hash_entry *
hash_create (tab, key)
     hash_table *tab;
     sb *key;
{
  int k = hash (key) % tab->size;
  hash_entry *p;
  hash_entry **table = tab->table;

  p = table[k];

  while (1)
    {
      if (!p)
	{
	  hash_entry *n = (hash_entry *) xmalloc (sizeof (hash_entry));
	  n->next = table[k];
	  sb_new (&n->key);
	  sb_add_sb (&n->key, key);
	  table[k] = n;
	  n->type = hash_integer;
	  return n;
	}
      if (strncmp (table[k]->key.ptr, key->ptr, key->len) == 0)
	{
	  return p;
	}
      p = p->next;
    }
}

/* add sb name with key into hash_table tab.  if replacing old value
   and again, then ERROR. */

static
void
hash_add_to_string_table (tab, key, name, again)
     hash_table *tab;
     sb *key;
     sb *name;
     int again;
{
  hash_entry *ptr = hash_create (tab, key);
  if (ptr->type == hash_integer)
    {
      sb_new (&ptr->value.s);
    }
  if (ptr->value.s.len)
    {
      if (!again)
	ERROR ((stderr, "redefintion not allowed"));
    }

  ptr->type = hash_string;
  sb_reset (&ptr->value.s);
  
  sb_add_sb (&ptr->value.s, name);
}

/* add integer name to hash_table tab with sb key. */

static
void
hash_add_to_int_table (tab, key, name)
     hash_table *tab;
     sb *key;
     int name;
{
  hash_entry *ptr = hash_create (tab, key);
  ptr->value.i = name;
}

/* lookup sb key in hash_table tab.  if found return hash_entry result,
   else 0. */
   
static
hash_entry *
hash_lookup (tab, key)
     hash_table *tab;
     sb *key;
{
  int k = hash (key) % tab->size;
  hash_entry **table = tab->table;
  hash_entry *p = table[k];
  while (p)
    {
      if (p->key.len == key->len
	  && strncmp (p->key.ptr, key->ptr, key->len) == 0)
	return p;
      p = p->next;
    }
  return 0;
}


/* expressions

   are handled in a really simple recursive decent way. each bit of
   the machine takes an index into an sb and a pointer to an exp_t,
   modifies the *exp_t and returns the index of the first character
   past the part of the expression parsed.

 expression precedence:
  ( )
 unary + - ~
  * /
  + -
  &
  | ~

*/


/* make sure that the exp_t at term is constant, if not the give the op ERROR. */

static
void
checkconst (op, term)
     char op;
     exp_t *term;
{
  if (term->add_symbol.len
      || term->sub_symbol.len)
    {
      ERROR ((stderr, "the %c operator cannot take non-absolute arguments.\n", op));
    }
}

/* turn the number in string at idx into a number of base,
   fill in ptr and return the index of the first character not in the
   number. */

static
int
sb_strtol (idx, string, base, ptr)
     int idx;
     sb *string;
     int base;
     int *ptr;
{
  int value = 0;
  idx = sb_skip_white (idx, string);

  while (idx < string->len)
    {
      int ch = string->ptr[idx];
      int dig = 0;
      if (isdigit (ch))
	dig = ch - '0';
      else if (ch >= 'a' && ch <= 'f')
	dig = ch - 'a' + 10;
      else if (ch >= 'A' && ch <= 'F')
	dig = ch - 'A' + 10;
      else
	break;

      if (dig >= base)
	break;

      value = value * base + dig;
      idx++;
    }
  *ptr = value;
  return idx;
}

static int level_5 ();

int
level_0 (idx, string, lhs)
     int idx;
     sb *string;
     exp_t *lhs;
{
  lhs->add_symbol.len = 0;
  lhs->add_symbol.name = 0;

  lhs->sub_symbol.len = 0;
  lhs->sub_symbol.name = 0;

  idx = sb_skip_white (idx, string);

  lhs->value = 0;

  if (isdigit (string->ptr[idx]))
    {
      idx = sb_strtol (idx, string, 10, &lhs->value);
    }
  else if (ISFIRSTCHAR (string->ptr[idx]))
    {
      int len = 0;
      lhs->add_symbol.name = string->ptr + idx;
      while (idx < string->len && ISNEXTCHAR (string->ptr[idx]))
	{
	  idx++;
	  len++;
	}
      lhs->add_symbol.len = len;
    }
  else if (string->ptr[idx] == '"')
    {
      sb acc;
      sb_new (&acc);
      ERROR ((stderr, "string where expression expected.\n"));
      idx = getstring (idx, string, &acc);
      sb_kill (&acc);
    }
  else
    {
      ERROR ((stderr, "can't find primary in expression.\n"));
      idx++;
    }
  return sb_skip_white (idx, string);
}



static int
level_1 (idx, string, lhs)
     int idx;
     sb *string;
     exp_t *lhs;
{
  idx = sb_skip_white (idx, string);

  switch (string->ptr[idx])
    {
    case '+':
      idx = level_1 (idx + 1, string, lhs);
      break;
    case '~':
      idx = level_1 (idx + 1, string, lhs);
      checkconst ('~', lhs);
      lhs->value = ~lhs->value;
      break;
    case '-':
      {
	symbol t;
	idx = level_1 (idx + 1, string, lhs);
	lhs->value = -lhs->value;
	t = lhs->add_symbol;
	lhs->add_symbol = lhs->sub_symbol;
	lhs->sub_symbol = t;
	break;
      }
    case '(':
      idx++;
      idx = level_5 (sb_skip_white (idx, string), string, lhs);
      if (string->ptr[idx] != ')')
	ERROR ((stderr, "misplaced closing parens.\n"));
      else
	idx++;
      break;
    default:
      idx = level_0 (idx, string, lhs);
      break;
    }
  return sb_skip_white (idx, string);
}

static int
level_2 (idx, string, lhs)
     int idx;
     sb *string;
     exp_t *lhs;
{
  exp_t rhs;

  idx = level_1 (idx, string, lhs);

  while (idx < string->len && (string->ptr[idx] == '*'
			       || string->ptr[idx] == '/'))
    {
      char op = string->ptr[idx++];
      idx = level_1 (idx, string, &rhs);
      switch (op)
	{
	case '*':
	  checkconst ('*', lhs);
	  checkconst ('*', &rhs);
	  lhs->value *= rhs.value;
	  break;
	case '/':
	  checkconst ('/', lhs);
	  checkconst ('/', &rhs);
	  if (rhs.value == 0)
	    ERROR ((stderr, "attempt to divide by zero.\n"));
	  else
	    lhs->value /= rhs.value;
	  break;
	}
    }
  return sb_skip_white (idx, string);
}


static int
level_3 (idx, string, lhs)
     int idx;
     sb *string;
     exp_t *lhs;
{
  exp_t rhs;

  idx = level_2 (idx, string, lhs);

  while (idx < string->len
	 && (string->ptr[idx] == '+'
	     || string->ptr[idx] == '-'))
    {
      char op = string->ptr[idx++];
      idx = level_2 (idx, string, &rhs);
      switch (op)
	{
	case '+':
	  lhs->value += rhs.value;
	  if (lhs->add_symbol.name && rhs.add_symbol.name)
	    {
	      ERROR ((stderr, "can't add two relocatable expressions\n"));
	    }
	  /* change nn+symbol to symbol + nn */
	  if (rhs.add_symbol.name)
	    {
	      lhs->add_symbol = rhs.add_symbol;
	    }
	  break;
	case '-':
	  lhs->value -= rhs.value;
	  lhs->sub_symbol = rhs.add_symbol;
	  break;
	}
    }
  return sb_skip_white (idx, string);
}

static int
level_4 (idx, string, lhs)
     int idx;
     sb *string;
     exp_t *lhs;
{
  exp_t rhs;

  idx = level_3 (idx, string, lhs);

  while (idx < string->len &&
	 string->ptr[idx] == '&')
    {
      char op = string->ptr[idx++];
      idx = level_3 (idx, string, &rhs);
      switch (op)
	{
	case '&':
	  checkconst ('&', lhs);
	  checkconst ('&', &rhs);
	  lhs->value &= rhs.value;
	  break;
	}
    }
  return sb_skip_white (idx, string);
}

static int
level_5 (idx, string, lhs)
     int idx;
     sb *string;
     exp_t *lhs;
{
  exp_t rhs;

  idx = level_4 (idx, string, lhs);

  while (idx < string->len
	 && (string->ptr[idx] == '|' || string->ptr[idx] == '~'))
    {
      char op = string->ptr[idx++];
      idx = level_4 (idx, string, &rhs);
      switch (op)
	{
	case '|':
	  checkconst ('|', lhs);
	  checkconst ('|', &rhs);
	  lhs->value |= rhs.value;
	  break;
	case '~':
	  checkconst ('~', lhs);
	  checkconst ('~', &rhs);
	  lhs->value ^= rhs.value;
	  break;
	}
    }
  return sb_skip_white (idx, string);
}


/* parse the expression at offset idx into string, fill up res with
   the result. return the index of the first char past the expression.
   */

static int
exp_parse (idx, string, res)
     int idx;
     sb *string;
     exp_t *res;
{
  return level_5 (sb_skip_white (idx, string), string, res);
}


/* turn the expression at exp into text and glue it onto the end of
   string. */

static void
exp_string (exp, string)
     exp_t *exp;
     sb *string;
{
  int np = 0;
  int ad = 0;
  sb_reset (string);

  if (exp->add_symbol.len)
    {
      sb_add_buffer (string, exp->add_symbol.name, exp->add_symbol.len);
      np = 1;
      ad = 1;
    }
  if (exp->value)
    {
      char buf[20];
      if (np)
	sb_add_char (string, '+');
      sprintf (buf, "%d", exp->value);
      sb_add_string (string, buf);
      np = 1;
      ad = 1;
    }
  if (exp->sub_symbol.len)
    {
      sb_add_char (string, '-');
      sb_add_buffer (string, exp->add_symbol.name, exp->add_symbol.len);
      np = 0;
      ad = 1;
    }

  if (!ad)
    sb_add_char (string, '0');
}


/* parse the expression at offset idx into sb in, return the value in val.  
   if the expression is not constant, give ERROR emsg.  returns the index
   of the first character past the end of the expression. */

static int
exp_get_abs (emsg, idx, in, val)
     char *emsg;
     int idx;
     sb *in;
     int *val;
{
  exp_t res;
  idx = exp_parse (idx, in, &res);
  if (res.add_symbol.len || res.sub_symbol.len)
    ERROR ((stderr, emsg));
  *val = res.value;
  return idx;
}


sb label; /* current label parsed from line */
hash_table assign_hash_table;   /* hash table for all assigned variables */
hash_table keyword_hash_table;  /* hash table for keyword */
hash_table vars;  /* hash table for  eq variables */

#define in_comment ';'

#if 1
void
strip_comments (out)
     sb *out;
{
  char *s = out->ptr;
  int i = 0;
  for (i = 0; i < out->len; i++)
    {
      if (ISCOMMENTCHAR(s[i]))
	{
	  out->len = i;
	  return;
	}
    }
}
#endif

/* push back character ch so that it can be read again. */

void
unget (ch)
     int ch;
{
  if (ch == '\n')
    {
      sp->linecount--;
    }
  if (sp->pushback_index)
    sp->pushback_index--;
  else
    sb_add_char (&sp->pushback, ch);
}

/* push the sb ptr onto the include stack, with the given name, type and index. */

static
void
include_buf (name, ptr, type, index)
     sb *name;
     sb *ptr;
     include_type type;
     int index;
{
  sp++;
  if (sp - include_stack >= MAX_INCLUDES)
    FATAL ((stderr, "unreasonable nesting.\n"));
  sb_new (&sp->name);
  sb_add_sb (&sp->name, name);
  sp->handle = 0;
  sp->linecount = 1;
  sp->pushback_index = 0;
  sp->type = type;
  sp->index = index;
  sb_new (&sp->pushback);
  sb_add_sb (&sp->pushback, ptr);
}


/* used in ERROR messages, print info on where the include stack is onto file. */
static 
void
include_print_where_line (file)
     FILE *file;
{
  struct include_stack *p = include_stack + 1;

  while (p <= sp)
    {
      fprintf (file, "%s:%d ", sb_name (&p->name), p->linecount - ((p == sp) ? 1 : 0));
      p++;
    }
}

/* used in listings, print the line number onto file. */
static void
include_print_line (file)
     FILE *file;
{
  int n;
  struct include_stack *p = include_stack + 1;

  n = fprintf (file, "%4d", p->linecount);
  p++;
  while (p <= sp)
    {
      n += fprintf (file, ".%d", p->linecount);
      p++;
    }
  while (n < 8 * 3)
    {
      fprintf (file, " ");
      n++;
    }
}


/* read a line from the top of the include stack into sb in. */

static int
get_line (in)
     sb *in;
{
  int online = 0;
  int more = 1;

  if (copysource)
    {
      putc (comment_char, outfile);
      if (print_line_number)
	include_print_line (outfile);
    }

  while (1)
    {
      int ch = get ();

      while (ch == '\r')
	ch = get ();

      if (ch == EOF)
	{
	  if (online)
	    {
	      WARNING ((stderr, "End of file not at start of line.\n"));
	      if (copysource)
		putc ('\n', outfile);
	      ch = '\n';
	    }
	  else
	    more = 0;
	  break;
	}

      if (copysource)
	{
	  putc (ch, outfile);
	}

      if (ch == '\n')
	{
	  ch = get ();
	  online = 0;
	  if (ch == '+')
	    {
	      /* continued line */
	      if (copysource)
		{
		  putc (comment_char, outfile);
		  putc ('+', outfile);
		}
	      ch = get ();
	    }
	  else
	    {
	      if (ch != EOF)
		unget (ch);
	      break;
	    }
	}
      else
	{
	  sb_add_char (in, ch);
	}
      online++;
    }

  return more;
}

/* find a label from sb in and put it in out. */

static int
grab_label (in, out)
     sb *in;
     sb *out;
{
  int i = 0;
  sb_reset (out);
  if (ISFIRSTCHAR (in->ptr[i]))
    {
      sb_add_char (out, in->ptr[i]);
      i++;
      while ((ISNEXTCHAR (in->ptr[i]) 
	      || in->ptr[i] == '\\'
	      || in->ptr[i] == '&') 
	     && i < in->len)
	{
	  sb_add_char (out, in->ptr[i]);
	  i++;
	}
    }
  return i;
}

/* find all strange base stuff and turn into decimal. also
   find all the other numbers and convert them from the default radix */

static void
change_base (idx, in, out)
     int idx;
     sb *in;
     sb *out;
{
  char buffer[20];

  while (idx < in->len)
    {
      if (idx < in->len - 1 && in->ptr[idx + 1] == '\'')
	{
	  int base;
	  int value;
	  switch (in->ptr[idx])
	    {
	    case 'b':
	    case 'B':
	      base = 2;
	      break;
	    case 'q':
	    case 'Q':
	      base = 8;
	      break;
	    case 'h':
	    case 'H':
	      base = 16;
	      break;
	    case 'd':
	    case 'D':
	      base = 10;
	      break;
	    default:
	      ERROR ((stderr, "Illegal base character %c.\n", in->ptr[idx]));
	      base = 10;
	      break;
	    }

	  idx = sb_strtol (idx + 2, in, base, &value);
	  sprintf (buffer, "%d", value);
	  sb_add_string (out, buffer);
	}
      else if (ISFIRSTCHAR (in->ptr[idx]))
	{
	  /* copy entire names through quickly */
	  sb_add_char (out, in->ptr[idx]);
	  idx++;
	  while (idx < in->len && ISNEXTCHAR (in->ptr[idx]))
	    {
	      sb_add_char (out, in->ptr[idx]);
	      idx++;
	    }
	}
      else if (isdigit (in->ptr[idx]))
	{
	  int value;
	  /* all numbers must start with a digit, let's chew it and
	     spit out decimal */
	  idx = sb_strtol (idx, in, radix, &value);
	  sprintf (buffer, "%d", value);
	  sb_add_string (out, buffer);

	  /* skip all undigsested letters */
	  while (idx < in->len && ISNEXTCHAR (in->ptr[idx]))
	    {
	      sb_add_char (out, in->ptr[idx]);
	      idx++;
	    }
	}
      else
	{
	  /* nothing special, just pass it through */
	  sb_add_char (out, in->ptr[idx]);
	  idx++;
	}
    }

}

/* .end */
static void
do_end ()
{
  had_end = 1;
}

/* .assign */

static void
do_assign (again, idx, in)
     int again;
     int idx;
     sb *in;
{
  /* stick label in symbol table with following value */
  exp_t e;
  sb acc;

  sb_new (&acc);
  idx = exp_parse (idx, in, &e);
  exp_string (&e, &acc);
  hash_add_to_string_table (&assign_hash_table, &label, &acc, again);
  sb_kill (&acc);
}


/* .radix [b|q|d|h] */

static
void
do_radix (ptr)
     sb *ptr;
{
  int idx = sb_skip_white (0, ptr);
  switch (ptr->ptr[idx])
    {
    case 'B':
    case 'b':
      radix = 2;
      break;
    case 'q':
    case 'Q':
      radix = 8;
      break;
    case 'd':
    case 'D':
      radix = 10;
      break;
    case 'h':
    case 'H':
      radix = 16;
      break;
    default:
      ERROR ((stderr, "radix is %c must be one of b, q, d or h", radix));
    }
}


/* Parse off a .b, .w or .l */

static int
get_opsize (idx, in, size)
int idx;
sb *in;
int *size;
{
  *size = 4;
  if (in->ptr[idx] == '.')
    {
      idx++;
    }
  switch (in->ptr[idx])
    {
    case 'b':
    case 'B':
      *size = 1;
      break;
    case 'w':
    case 'W':
      *size = 2;
      break;
    case 'l':
    case 'L':
      *size = 4;
      break;
    case ' ':
    case '\t':
      break;
    default:
      ERROR ((stderr, "size must be one of b, w or l, is %c.\n", in->ptr[idx]));
      break;
    }
  idx++;

  return idx;
}

static 
int eol(idx, line)
int idx;
sb *line;
{
  idx = sb_skip_white (idx, line);
  if (idx < line->len 
      && ISCOMMENTCHAR(line->ptr[idx]))
    return 1;
  if (idx >= line->len)
    return 1;
  return 0;
}

/* .data [.b|.w|.l] <data>* 
    or d[bwl] <data>* */

static void
do_data (idx, in, size)
     int idx;
     sb *in;
     int size;
{
  int opsize = 4;
  char *opname;
  sb acc;
  sb_new (&acc);

  if (!size) 
    {
      idx = get_opsize (idx, in, &opsize);
    }
  else {
    opsize = size;
  }
  switch (opsize)
    {
    case 4:
      opname = ".long";
      break;
    case 2:
      opname = ".short";
      break;
    case 1:
      opname = ".byte";
      break;
    }


  fprintf (outfile, "%s\t", opname);

  idx =   sb_skip_white (idx, in);

  if (alternate 
      && idx < in->len 
      && in->ptr[idx] == '"')
    {
      int i;
      idx = getstring (idx, in, &acc);
      for (i = 0; i < acc.len; i++)
	{
	  if (i)
	    fprintf(outfile,",");
	  fprintf (outfile, "%d", acc.ptr[i]);
	}
    }
  else 
    {
      while (!eol (idx, in))
	{
	  exp_t e;
	  idx = exp_parse (idx, in, &e);
	  exp_string (&e, &acc);
	  sb_add_char (&acc, 0);
	  fprintf (outfile, acc.ptr);
	  if (idx < in->len && in->ptr[idx] == ',')
	    {
	      fprintf (outfile, ",");
	      idx++;
	    }
	}
    }
  sb_kill (&acc);
  sb_print_at (idx, in);
  fprintf (outfile, "\n");
}

/* .datab [.b|.w|.l] <repeat>,<fill> */

static void
do_datab (idx, in)
     int idx;
     sb *in;
{
  int opsize;
  int repeat;
  int fill;

  idx = get_opsize (idx, in, &opsize);

  idx = exp_get_abs ("datab repeat must be constant.\n", idx, in, &repeat);
  idx = sb_skip_comma (idx, in);
  idx = exp_get_abs ("datab data must be absolute.\n", idx, in, &fill);

  fprintf (outfile, ".fill\t%d,%d,%d\n", repeat, opsize, fill);
}

/* .align <size> */

void
do_align (idx, in)
     int idx;
     sb *in;
{
  int al;
  idx = exp_get_abs ("align needs absolute expression.\n", idx, in, &al);

  if (al != 1
      && al != 2
      && al != 4)
    WARNING ((stderr, "alignment must be one of 1, 2 or 4.\n"));

  fprintf (outfile, ".align	%d\n", al);
}

/* .res[.b|.w|.l] <size> */

void
do_res (idx, in, type)
     int idx;
     sb *in;
     char type;
{
  int size = 4;
  int count = 0;

  idx = get_opsize (idx, in, &size);
  while (!eol(idx, in))
    {
      idx = sb_skip_white (idx, in);
      if (in->ptr[idx] == ',')
	idx++;
      idx = exp_get_abs ("res needs absolute expression for fill count.\n", idx, in, &count);

      if (type == 'c' || type == 'z')
	count++;

      fprintf (outfile, ".space	%d\n", count * size);
    }
}


/* .export */

void
do_export (in)
     sb *in;
{
  fprintf (outfile, ".global	%s\n", sb_name (in));
}

/* .print [list] [nolist] */

void
do_print (idx, in)
     int idx;
     sb *in;
{
  idx = sb_skip_white (idx, in);
  while (idx < in->len)
    {
      if (strncmp (in->ptr + idx, "LIST", 4) == 0)
	{
	  fprintf (outfile, ".list\n");
	  idx += 4;
	}
      else if (strncmp (in->ptr + idx, "NOLIST", 6) == 0)
	{
	  fprintf (outfile, ".nolist\n");
	  idx += 6;
	}
      idx++;
    }
}

/* .head */
void
do_heading (idx, in)
     int idx;
     sb *in;
{
  sb head;
  sb_new (&head);
  idx = getstring (idx, in, &head);
  fprintf (outfile, ".title	\"%s\"\n", sb_name (&head));
  sb_kill (&head);
}

/* .page */

void
do_page ()
{
  fprintf (outfile, ".eject\n");
}

/* .form [lin=<value>] [col=<value>] */
void
do_form (idx, in)
     int idx;
     sb *in;
{
  int lines = 60;
  int columns = 132;
  idx = sb_skip_white (idx, in);

  while (idx < in->len)
    {

      if (strncmp (in->ptr + idx, "LIN=", 4) == 0)
	{
	  idx += 4;
	  idx = exp_get_abs ("form LIN= needs absolute expresssion.\n", idx, in, &lines);
	}

      if (strncmp (in->ptr + idx, "COL=", 4) == 0)
	{
	  idx += 4;
	  idx = exp_get_abs ("form COL= needs absolute expresssion.\n", idx, in, &columns);
	}

      idx++;
    }
  fprintf (outfile, ".psize %d,%d\n", lines, columns);

}


/* Fetch string from the input stream,
   rules:
    'Bxyx<whitespace>  	-> return 'Bxyza
    %<char>		-> return string of decimal value of x
    "<string>"		-> return string
    xyx<whitespace>     -> return xyz
*/
int
get_any_string (idx, in, out, expand, pretend_quoted)
     int idx;
     sb *in;
     sb *out;
     int expand;
     int pretend_quoted;
{
  sb_reset (out);
  idx = sb_skip_white (idx, in);

  if (idx < in->len)
    {
      if (in->len > 2 && in->ptr[idx+1] == '\'' && ISBASE (in->ptr[idx]))
	{
	  while (!ISSEP (in->ptr[idx]))
	    sb_add_char (out, in->ptr[idx++]);
	}
      else if (in->ptr[idx] == '%'
	       && alternate
	       && expand)
	{
	  int val;
	  char buf[20];
	  /* Turns the next expression into a string */
	  idx = exp_get_abs ("% operator needs absolute expression",
			     idx + 1,
			     in,
			     &val);
	  sprintf(buf, "%d", val);
	  sb_add_string (out, buf);
	}
      else if (in->ptr[idx] == '"'
	       || in->ptr[idx] == '<'
	       || (alternate && in->ptr[idx] == '\''))
	{
	  if (alternate && expand)
	    {
	      /* Keep the quotes */
	            sb_add_char (out,  '\"');
		    
	      idx =  getstring (idx, in, out);
	    	      sb_add_char (out,  '\"');

	    }
	  else {
	    idx = getstring (idx, in, out);
	  }
	}
      else 
	{
	  while (idx < in->len 
		 && (in->ptr[idx] == '"'
		     || in->ptr[idx] == '\''
		     || pretend_quoted 
		     || !ISSEP (in->ptr[idx])))
	    {
	      if (in->ptr[idx] == '"' 
		  || in->ptr[idx] == '\'')
		{
		  char tchar = in->ptr[idx];
		  sb_add_char (out, in->ptr[idx++]);
		  while (idx < in->len
			 && in->ptr[idx] != tchar)
		    sb_add_char (out, in->ptr[idx++]);		    
		  if (idx == in->len)
		    return idx;	      
		}
	      sb_add_char (out, in->ptr[idx++]);
	    }
	}
    }

  return idx;
}


/* skip along sb in starting at idx, suck off whitespace a ( and more
   whitespace.  return the idx of the next char */

int
skip_openp (idx, in)
     int idx;
     sb *in;
{
  idx = sb_skip_white (idx, in);
  if (in->ptr[idx] != '(')
    ERROR ((stderr, "misplaced ( .\n"));
  idx = sb_skip_white (idx + 1, in);
  return idx;
}

/* skip along sb in starting at idx, suck off whitespace a ) and more
   whitespace.  return the idx of the next char */

int
skip_closep (idx, in)
     int idx;
     sb *in;
{
  idx = sb_skip_white (idx, in);
  if (in->ptr[idx] != ')')
    ERROR ((stderr, "misplaced ).\n"));
  idx = sb_skip_white (idx + 1, in);
  return idx;
}

/* .len */

int
dolen (idx, in, out)
     int idx;
     sb *in;
     sb *out;
{

  sb stringout;
  char buffer[10];

  sb_new (&stringout);
  idx = skip_openp (idx, in);
  idx = get_and_process (idx, in, &stringout);
  idx = skip_closep (idx, in);
  sprintf (buffer, "%d", stringout.len);
  sb_add_string (out, buffer);

  sb_kill (&stringout);
  return idx;
}


/* .instr */

static
int
doinstr (idx, in, out)
     int idx;
     sb *in;
     sb *out;
{
  sb string;
  sb search;
  int i;
  int start;
  int res;
  char buffer[10];

  sb_new (&string);
  sb_new (&search);
  idx = skip_openp (idx, in);
  idx = get_and_process (idx, in, &string);
  idx = sb_skip_comma (idx, in);
  idx = get_and_process (idx, in, &search);
  idx = sb_skip_comma (idx, in);
  if (isdigit (in->ptr[idx]))
    {
      idx = exp_get_abs (".instr needs absolute expresson.\n", idx, in, &start);
    }
  else
    {
      start = 0;
    }
  idx = skip_closep (idx, in);
  res = -1;
  for (i = start; i < string.len; i++)
    {
      if (strncmp (string.ptr + i, search.ptr, search.len) == 0)
	{
	  res = i;
	  break;
	}
    }
  sprintf (buffer, "%d", res);
  sb_add_string (out, buffer);
  sb_kill (&string);
  sb_kill (&search);
  return idx;
}


static int
dosubstr (idx, in, out)
     int idx;
     sb *in;
     sb *out;
{
  sb string;
  int pos;
  int len;
  sb_new (&string);

  idx = skip_openp (idx, in);
  idx = get_and_process (idx, in, &string);
  idx = sb_skip_comma (idx, in);
  idx = exp_get_abs ("need absolute position.\n", idx, in, &pos);
  idx = sb_skip_comma (idx, in);
  idx = exp_get_abs ("need absolute length.\n", idx, in, &len);
  idx = skip_closep (idx, in);


  if (len < 0 || pos < 0 ||
      pos > string.len
      || pos + len > string.len)
    {
      sb_add_string (out, " ");
    }
  else 
    {
      sb_add_char (out, '"');
      while (len > 0)
	{
	  sb_add_char (out, string.ptr[pos++]);
	  len--;
	}
      sb_add_char (out, '"');
    }
  sb_kill(&string);
  return idx;
}

/* scan line, change tokens in the hash table to their replacements */
void
process_assigns (idx, in, buf)
     int idx;
     sb *in;
     sb *buf;
{
  while (idx < in->len)
    {
      hash_entry *ptr;
      if (in->ptr[idx] == '\\'
	  && in->ptr[idx + 1] == '&')
	{
	  idx = condass_lookup_name (in, idx + 2, buf, 1);
	}
      else if (in->ptr[idx] == '\\'
	       && in->ptr[idx + 1] == '$')
	{
	  idx = condass_lookup_name (in, idx + 2, buf, 0);
	}
      else if (idx + 3 < in->len
	       && in->ptr[idx] == '.'
	       && in->ptr[idx + 1] == 'L'
	       && in->ptr[idx + 2] == 'E'
	       && in->ptr[idx + 3] == 'N')
	idx = dolen (idx + 4, in, buf);
      else if (idx + 6 < in->len
	       && in->ptr[idx] == '.'
	       && in->ptr[idx + 1] == 'I'
	       && in->ptr[idx + 2] == 'N'
	       && in->ptr[idx + 3] == 'S'
	       && in->ptr[idx + 4] == 'T'
	       && in->ptr[idx + 5] == 'R')
	idx = doinstr (idx + 6, in, buf);
      else if (idx + 7 < in->len
	       && in->ptr[idx] == '.'
	       && in->ptr[idx + 1] == 'S'
	       && in->ptr[idx + 2] == 'U'
	       && in->ptr[idx + 3] == 'B'
	       && in->ptr[idx + 4] == 'S'
	       && in->ptr[idx + 5] == 'T'
	       && in->ptr[idx + 6] == 'R')
	idx = dosubstr (idx + 7, in, buf);
      else if (ISFIRSTCHAR (in->ptr[idx]))
	{
	  /* may be a simple name subsitution, see if we have a word */
	  sb acc;
	  int cur = idx + 1;
	  while (cur < in->len
		 && (ISNEXTCHAR (in->ptr[cur])))
	    cur++;

	  sb_new (&acc);
	  sb_add_buffer (&acc, in->ptr + idx, cur - idx);
	  ptr = hash_lookup (&assign_hash_table, &acc);
	  if (ptr)
	    {
	      /* Found a definition for it */
	      sb_add_sb (buf, &ptr->value.s);
	    }
	  else
	    {
	      /* No definition, just copy the word */
	      sb_add_sb (buf, &acc);
	    }
	  sb_kill (&acc);
	  idx = cur;
	}
      else
	{
	  sb_add_char (buf, in->ptr[idx++]);
	}
    }
}

static int
get_and_process (idx, in, out)
     int idx;
     sb *in;
     sb *out;
{
  sb t;
  sb_new (&t);
  idx = get_any_string (idx, in, &t, 1, 0);
  process_assigns (0, &t, out);
  sb_kill (&t);
  return idx;
}

static
void
process_file ()
{
  sb line;
  sb t1, t2;
  sb acc;
  sb label_in;
  int more;

  sb_new (&line);
  sb_new (&t1);
  sb_new (&t2);
  sb_new(&acc);
  sb_new (&label_in);
  sb_reset (&line);
  more = get_line (&line);
  while (more)
    {
      /* Find any label and pseudo op that we're intested in */
      int l;
      if (line.len == 0)
	{
	  if (condass_on ())
	    fprintf (outfile, "\n");
	}
      else
	{
	  l = grab_label (&line, &label_in);
	  sb_reset (&label);	  	  
	  if (label_in.len)
	    {
	      /* Munge any label */

	      
	      process_assigns (0, &label_in, &label);
	    }

	  if (line.ptr[l] == ':')
	    l++;
	  while (ISWHITE (line.ptr[l]) && l < line.len)
	    l++;

	  if (l < line.len)
	    {
	      if (process_pseudo_op (l, &line, &acc))
		{



		}
	      else if (condass_on ())
		{
		  if (macro_op (l, &line))
		    {


		    }
		  else
		    {
		      {
			if (label.len)
			  {
			    fprintf (outfile, "%s:\t", sb_name (&label));
			  }
			else
			  fprintf (outfile, "\t");
			sb_reset(&t1);
			process_assigns (l, &line, &t1);
			sb_reset (&t2);
			change_base (0, &t1, &t2);
			fprintf (outfile, "%s\n", sb_name (&t2));
		      }
		    }
		}
	    }
	  else {
	    /* Only a label on this line */
	    if (label.len && condass_on())
	      {
		fprintf (outfile, "%s:\n", sb_name (&label));
	      }
	  }
	}

      if (had_end)
	break;
      sb_reset (&line);
      more = get_line (&line);
    }

  if (!had_end)
    WARNING ((stderr, "END missing from end of file.\n"));
}





static void
free_old_entry (ptr)
     hash_entry *ptr;
{
  if (ptr)
    {
      if (ptr->type == hash_string)
	sb_kill(&ptr->value.s);
    }
}

/* name: .ASSIGNA <value> */

void
do_assigna (idx, in)
     int idx;
     sb *in;
{
  sb tmp;
  int val;
  sb_new (&tmp);

  process_assigns (idx, in, &tmp);
  idx = exp_get_abs (".ASSIGNA needs constant expression argument.\n", 0, &tmp, &val);

  if (!label.len)
    {
      ERROR ((stderr, ".ASSIGNA without label.\n"));
    }
  else
    {
      hash_entry *ptr = hash_create (&vars, &label);
      free_old_entry (ptr);
      ptr->type = hash_integer;
      ptr->value.i = val;
    }
  sb_kill (&tmp);
}

/* name: .ASSIGNC <string> */

void
do_assignc (idx, in)
     int idx;
     sb *in;
{
  sb acc;
  sb_new (&acc);
  idx = getstring (idx, in, &acc);

  if (!label.len)
    {
      ERROR ((stderr, ".ASSIGNS without label.\n"));
    }
  else
    {
      hash_entry *ptr = hash_create (&vars, &label);
      free_old_entry (ptr);
      ptr->type = hash_string;
      sb_new (&ptr->value.s);
      sb_add_sb (&ptr->value.s, &acc);
    }
  sb_kill (&acc);
}


/* name: .REG (reg) */

static void
do_reg (idx, in)
     int idx;
     sb *in;
{
  /* remove reg stuff from inside parens */
  sb what;
  idx = skip_openp (idx, in);
  sb_new (&what);
  while (idx < in->len && in->ptr[idx] != ')')
    {
      sb_add_char (&what, in->ptr[idx]);
      idx++;
    }
  hash_add_to_string_table (&assign_hash_table, &label, &what, 1);
  sb_kill (&what);
}


static int
condass_lookup_name (inbuf, idx, out, warn)
     sb *inbuf;
     int idx;
     sb *out;
     int warn;
{
  hash_entry *ptr;
  sb condass_acc;
  sb_new (&condass_acc);

  while (idx < inbuf->len
	 && ISNEXTCHAR (inbuf->ptr[idx]))
    {
      sb_add_char (&condass_acc, inbuf->ptr[idx++]);
    }

  if (inbuf->ptr[idx] == '\'')
    idx++;
  ptr = hash_lookup (&vars, &condass_acc);


  if (!ptr)
    {
      if (warn) 
	{
	  WARNING ((stderr, "Can't find preprocessor variable %s.\n", sb_name (&condass_acc)));
	}
      else 
	{
	  sb_add_string (out, "0");
	}
    }
  else
    {
      if (ptr->type == hash_integer)
	{
	  char buffer[30];
	  sprintf (buffer, "%d", ptr->value.i);
	  sb_add_string (out, buffer);
	}
      else
	{
	  sb_add_sb (out, &ptr->value.s);
	}
    }
  sb_kill (&condass_acc);
  return idx;
}

#define EQ 1
#define NE 2
#define GE 3
#define LT 4
#define LE 5
#define GT 6
#define NEVER 7

int
whatcond (idx, in, val)
     int idx;
     sb *in;
     int *val;
{
  int cond;
  char *p;
  idx = sb_skip_white (idx, in);
  p = in->ptr + idx;
  if (p[0] == 'E' && p[1] == 'Q')
    cond = EQ;
  else if (p[0] == 'N' && p[1] == 'E')
    cond = NE;
  else if (p[0] == 'L' && p[1] == 'T')
    cond = LT;
  else if (p[0] == 'L' && p[1] == 'E')
    cond = LE;
  else if (p[0] == 'G' && p[1] == 'T')
    cond = GT;
  else if (p[0] == 'G' && p[1] == 'E')
    cond = GE;
  else
    {
      ERROR ((stderr, "Comparison operator must be one of EQ, NE, LT, LE, GT or GE.\n"));
      cond = NEVER;
    }
  idx = sb_skip_white (idx + 2, in);
  *val = cond;
  return idx;
}

int
istrue (idx, in)
     int idx;
     sb *in;
{
  int res;
  sb acc_a;
  sb cond;
  sb acc_b;
  sb_new (&acc_a);
  sb_new (&cond);
  sb_new (&acc_b);
  idx = sb_skip_white (idx, in);

  if (in->ptr[idx] == '"')
    {
      int cond;
      int same;
      /* This is a string comparision */
      idx = getstring (idx, in, &acc_a);
      idx = whatcond (idx, in, &cond);
      idx = getstring (idx, in, &acc_b);
      same = acc_a.len == acc_b.len && (strncmp (acc_a.ptr, acc_b.ptr, acc_a.len) == 0);

      if (cond != EQ && cond != NE)
	{
	  ERROR ((stderr, "Comparison operator for strings must be EQ or NE\n"));
	  res = 0;
	}
      else
	res = cond == EQ && same;
    }
  else
    /* This is a numeric expression */
    {
      int vala;
      int valb;
      int cond;
      idx = exp_get_abs ("Conditional operator must have absolute operands.\n", idx, in, &vala);
      idx = whatcond (idx, in, &cond);
      idx = sb_skip_white (idx, in);
      if (in->ptr[idx] == '"')
	{
	  WARNING ((stderr, "String compared against expression.\n"));
	  res = 0;
	}
      else
	{
	  idx = exp_get_abs ("Conditional operator must have absolute operands.\n", idx, in, &valb);
	  switch (cond)
	    {
	    case EQ:
	      res = vala == valb;
	      break;
	    case NE:
	      res = vala != valb;
	      break;
	    case LT:
	      res = vala < valb;
	      break;
	    case LE:
	      res = vala <= valb;
	      break;
	    case GT:
	      res = vala > valb;
	      break;
	    case GE:
	      res = vala >= valb;
	      break;
	    case NEVER:
	      res = 0;
	      break;
	    }
	}
    }

  sb_kill (&acc_a);
  sb_kill (&cond);
  sb_kill (&acc_b);
  return res;
}

/* .AIF */
static void
do_aif (idx, in)
     int idx;
     sb *in;
{
  if (ifi >= IFNESTING)
    {
      FATAL ((stderr, "AIF nesting unreasonable.\n"));
    }
  ifi++;
  ifstack[ifi].on = ifstack[ifi-1].on ? istrue (idx, in) : 0;
  ifstack[ifi].hadelse = 0;
}


/* .AELSE */
static void
do_aelse ()
{
  ifstack[ifi].on = ifstack[ifi-1].on ? !ifstack[ifi].on : 0;
  if (ifstack[ifi].hadelse)
    {
      ERROR ((stderr, "Multiple AELSEs in AIF.\n"));
    }
  ifstack[ifi].hadelse = 1;
}


/* .AENDI */
static void
do_aendi ()
{
  if (ifi != 0)
    {
      ifi--;
    }
  else
    {
      ERROR ((stderr, "AENDI without AIF.\n"));
    }
}

static int
condass_on ()
{
  return ifstack[ifi].on;
}


/* Read input lines till we get to a TO string.
   Increase nesting depth if we geta FROM string.
   Put the results into sb at PTR. */

static void
buffer_and_nest (from, to, ptr)
     char *from;
     char *to;
     sb *ptr;
{
  int from_len = strlen (from);
  int to_len = strlen (to);
  int depth = 1;
  int line_start = ptr->len;
  int line = linecount ();

  int more = get_line (ptr);

  while (more)
    {
      /* Try and find the first pseudo op on the line */
      int i = line_start;

      if (!alternate) 
	{
	  /* With normal syntax we can suck what we want till we get to the dot.
	     With the alternate, labels have to start in the first column, since
	     we cant tell what's a label and whats a pseudoop */

      /* Skip leading whitespace */
      while (i < ptr->len
	     && ISWHITE (ptr->ptr[i]))
	i++;

      /* Skip over a label */
      while (i < ptr->len
	     && ISNEXTCHAR (ptr->ptr[i]))
	i++;

      /* And a colon */
      if (i < ptr->len
	  && ptr->ptr[i] == ':')
	i++;

    }
      /* Skip trailing whitespace */
      while (i < ptr->len
	     && ISWHITE (ptr->ptr[i]))
	i++;

      if (i < ptr->len && (ptr->ptr[i] == '.' 
			   || alternate))
	{
	  if (ptr->ptr[i] == '.')
	      i++;
	  if (strncmp (ptr->ptr + i, from, from_len) == 0)
	    depth++;
	  if (strncmp (ptr->ptr + i, to, to_len) == 0)
	    {
	      depth--;
	      if (depth == 0)
		{
		  /* Reset the string to not include the ending rune */
		  ptr->len = line_start;
		  break;
		}
	    }
	}

      /* Add a CR to the end and keep running */
      sb_add_char (ptr, '\n');
      line_start = ptr->len;
      more = get_line (ptr);
    }


  if (depth)
    FATAL ((stderr, "End of file whilst inside %s, started on line %d.\n", from, line));
}


/* .ENDR */
void
do_aendr ()
{
  ERROR ((stderr, "AENDR without a AREPEAT.\n"));
}

/* .AWHILE */

static
void
do_awhile (idx, in)
     int idx;
     sb *in;
{
  sb exp;

  sb sub;

  int doit;
  sb_new (&sub);
  sb_new (&exp);

  process_assigns (idx, in, &exp);
  doit = istrue (0, &exp);

  buffer_and_nest ("AWHILE", "AENDW", &sub);

  /* Turn
     	.AWHILE exp
	     foo
	.AENDW
     into
        foo
	.AWHILE exp
	foo
	.ENDW
   */

  if (doit)
    {
      int index = include_next_index ();

      sb copy;
      sb_new (&copy);
      sb_add_sb (&copy, &sub);
      sb_add_sb (&copy, in);
      sb_add_string (&copy, "\n");
      sb_add_sb (&copy, &sub);
      sb_add_string (&copy, "\t.AENDW\n");
      /* Push another WHILE */
      include_buf (&exp, &copy, include_while, index);
      sb_kill (&copy);
    }
  sb_kill (&exp);
  sb_kill (&sub);
}


/* .AENDW */

static void
do_aendw ()
{
  ERROR ((stderr, "AENDW without a AENDW.\n"));
}


/* .EXITM
   
   Pop things off the include stack until the type and index changes */

static void
do_exitm ()
{
  include_type type = sp->type;
  if (type == include_repeat
      || type == include_while
      || type == include_macro)
    {
      int index = sp->index;
      include_pop ();
      while (sp->index == index
	     && sp->type == type)
	{
	  include_pop ();
	}
    }
}

/* .AREPEAT */

static void
do_arepeat (idx, in)
     int idx;
     sb *in;
{
  sb exp;			/* buffer with expression in it */
  sb copy;			/* expanded repeat block */
  sb sub;			/* contents of AREPEAT */
  int rc;
  char buffer[30];
  sb_new (&exp);
  sb_new (&copy);
  sb_new (&sub);
  process_assigns (idx, in, &exp);
  idx = exp_get_abs ("AREPEAT must have absolute operand.\n", 0, &exp, &rc);
  buffer_and_nest ("AREPEAT", "AENDR", &sub);
  if (rc > 0)
    {
      /* Push back the text following the repeat, and another repeat block
	 so
	 .AREPEAT 20
	 foo
	 .AENDR
	 gets turned into
	 foo
	 .AREPEAT 19
	 foo
	 .AENDR
	 */
      int index = include_next_index ();
      sb_add_sb (&copy, &sub);
      if (rc > 1)
	{
	  sprintf (buffer, "\t.AREPEAT	%d\n", rc - 1);
	  sb_add_string (&copy, buffer);
	  sb_add_sb (&copy, &sub);
	  sb_add_string (&copy, "	.AENDR\n");
	}

      include_buf (&exp, &copy, include_repeat, index);
    }
  sb_kill (&exp);
  sb_kill (&sub);
  sb_kill (&copy);
}

/* .ENDM */

static void
do_endm ()
{
  ERROR ((stderr, ".ENDM without a matching .MACRO.\n"));
}


/* MARRO PROCESSING */

static int number;
hash_table macro_table;

/* Understand

   .MACRO <name>
   stuff
   .ENDM
*/

static int
do_formals (macro, idx, in)
     macro_entry *macro;
     int idx;
     sb *in;
{
  formal_entry **p = &macro->formals;
  macro->formal_count = 0;
  hash_new_table (5, &macro->formal_hash);
  while (idx < in->len)
    {
      formal_entry *formal;

      formal = (formal_entry *) xmalloc (sizeof (formal_entry));

      sb_new (&formal->name);
      sb_new (&formal->def);
      sb_new (&formal->actual);

      idx = sb_skip_white (idx, in);
      idx = get_token (idx, in, &formal->name);
      if (formal->name.len == 0)
	break;
      idx = sb_skip_white (idx, in);
      if (formal->name.len)
	{
	  /* This is a formal */
	  if (idx < in->len && in->ptr[idx] == '=')
	    {
	      /* Got a default */
	      idx = get_any_string (idx + 1, in, &formal->def, 1, 0);
	    }
	}

      {
	/* Add to macro's hash table */

	hash_entry *p = hash_create (&macro->formal_hash, &formal->name);
	p->type = hash_formal;
	p->value.f = formal;
      }

      formal->index = macro->formal_count;
      idx = sb_skip_comma (idx, in);
      macro->formal_count++;
      *p = formal;
      p = &formal->next;
    }
  return idx;
}

/* Parse off LOCAL n1, n2,... Invent a label name for it */
static
void 
do_local (idx, line)
     int idx;
     sb *line;
{
  static int ln;
  sb acc;
  sb sub;
  char subs[10];
  sb_new (&acc);
  sb_new (&sub);
  idx = sb_skip_white (idx, line);
  while (!eol(idx, line))
    {
      sb_reset (&acc);
      sb_reset (&sub);
      ln++;
      sprintf(subs, "LL%04x", ln);
      idx =  get_token(idx, line, &acc);
      sb_add_string (&sub, subs);
      hash_add_to_string_table (&assign_hash_table, &acc, &sub, 1);
      idx = sb_skip_comma (idx, line);
    }
  sb_kill (&sub);
  sb_kill (&acc);
}

static
void
do_macro (idx, in)
     int idx;
     sb *in;
{
  macro_entry *macro;
  sb name;

  macro = (macro_entry *) xmalloc (sizeof (macro_entry));
  sb_new (&macro->sub);
  sb_new (&name);

  macro->formal_count = 0;
  macro->formals = 0;

  idx = sb_skip_white (idx, in);
  buffer_and_nest ("MACRO", "ENDM", &macro->sub);
  if (label.len)
    {

      sb_add_sb (&name, &label);
      if (in->ptr[idx] == '(')
	{
	  /* It's the label: MACRO (formals,...)  sort */
	  idx = do_formals (macro, idx + 1, in);
	  if (in->ptr[idx] != ')')
	    ERROR ((stderr, "Missing ) after formals.\n"));
	}
      else {
	/* It's the label: MACRO formals,...  sort */
	idx = do_formals (macro, idx, in);
      }
    }
  else
    {
      idx = get_token (idx, in, &name);
      idx = sb_skip_white (idx, in);
      idx = do_formals (macro, idx, in);
    }

  /* and stick it in the macro hash table */
  hash_create (&macro_table, &name)->value.m = macro;
}

static
int
get_token (idx, in, name)
     int idx;
     sb *in;
     sb *name;
{
  if (idx < in->len
      && ISFIRSTCHAR (in->ptr[idx]))
    {
      sb_add_char (name, in->ptr[idx++]);
      while (idx < in->len
	     && ISNEXTCHAR (in->ptr[idx]))
	{
	  sb_add_char (name, in->ptr[idx++]);
	}
    }
  /* Ignore trailing & */
  if (alternate && idx < in->len && in->ptr[idx] == '&')
    idx++;
  return idx;
}

/* Scan a token, but stop if a ' is seen */
static int
get_apost_token (idx, in, name, kind)
     int idx;
     sb *in;
     sb *name;
     int kind;
{
  idx = get_token (idx, in, name);
  if (idx < in->len && in->ptr[idx] == kind)
    idx++;
  return idx;
}

static int
sub_actual (src, in, t, m, kind, out, copyifnotthere)
     int src;
     sb *in;
     sb *t;
     macro_entry *m;
     int kind;
     sb *out;
     int copyifnotthere;
{
  /* This is something to take care of */
  hash_entry *ptr;
  src = get_apost_token (src, in, t, kind);
  /* See if it's in the macro's hash table */
  ptr = hash_lookup (&m->formal_hash, t);
  if (ptr)
    {
      if (ptr->value.f->actual.len)
	{
	  sb_add_sb (out, &ptr->value.f->actual);
	}
      else
	{
	  sb_add_sb (out, &ptr->value.f->def);
	}
    }
  else if (copyifnotthere)
    {
      sb_add_sb (out, t);
    }
  else 
    {
      sb_add_char (out, '\\');
      sb_add_sb (out, t);
    }
  return src;
}

static
void
macro_expand (name, idx, in, m)
     sb *name;
     int idx;
     sb *in;
     macro_entry *m;
{
  sb t;
  sb out;
  hash_entry *ptr;
  formal_entry *f;
  int is_positional = 0;
  int is_keyword = 0;

  sb_new (&t);
  sb_new (&out);
  
  /* Reset any old value the actuals may have */
  for (f = m->formals; f; f = f->next)
      sb_reset (&f->actual);
  f = m->formals;
  /* Peel off the actuals and store them away in the hash tables' actuals */
  while (!eol(idx, in))
    {
      int scan;
      idx = sb_skip_white (idx, in);
      /* Look and see if it's a positional or keyword arg */
      scan = idx;
      while (scan < in->len
	     && !ISSEP (in->ptr[scan])
	     && (!alternate && in->ptr[scan] != '='))
	scan++;
      if (scan < in->len && (!alternate) && in->ptr[scan] == '=')
	{
	  is_keyword = 1;
	  if (is_positional)
	    {
	      ERROR ((stderr, "Can't mix positional and keyword arguments.\n"));
	      return;
	    }
	  /* This is a keyword arg, fetch the formal name and
	     then the actual stuff */
	  sb_reset (&t);
	  idx = get_token (idx, in, &t);
	  if (in->ptr[idx] != '=')
	    ERROR ((stderr, "confused about formal params.\n"));

	  /* Lookup the formal in the macro's list */
	  ptr = hash_lookup (&m->formal_hash, &t);
	  if (!ptr)
	    {
	      ERROR ((stderr, "MACRO formal argument %s does not exist.\n", sb_name (&t)));
	      return;
	    }
	  else
	    {
	      /* Insert this value into the right place */
	      sb_reset (&ptr->value.f->actual);
	      idx = get_any_string (idx + 1, in, &ptr->value.f->actual, 0, 0);
	    }
	}
      else
	{
	  /* This is a positional arg */
	  is_positional = 1;
	  if (is_keyword)
	    {
	      ERROR ((stderr, "Can't mix positional and keyword arguments.\n"));
	      return;
	    }
	  if (!f)
	    {
	      ERROR ((stderr, "Too many positional arguments.\n"));
	      return;
	    }

	  sb_reset (&f->actual);
	  idx = get_any_string (idx, in, &f->actual, 1, 0);
	  f = f->next;
	}
      idx = sb_skip_comma (idx, in);
    }

  /* Copy the stuff from the macro buffer into a safe place and substitute any args */

  {
    int src = 0;
    int inquote = 0;
    sb *in = &m->sub;
    sb_reset (&out);

    while (src < in->len)
      {
	if (in->ptr[src] == '&')
	  {
	    sb_reset (&t);
	    src = sub_actual (src + 1, in, &t, m, '&', &out, 0);
	  }
	else if (in->ptr[src] == '\\')
	  {
	    src++;
	    if (in->ptr[src] == comment_char)
	      {
		/* This is a comment, just drop the rest of the line */
		while (src < in->len
		       && in->ptr[src] != '\n')
		  src++;

	      }
	    else if (in->ptr[src] == '(')
	      {
		/* Sub in till the next ')' literally */
		src++;
		while (src < in->len && in->ptr[src] != ')')
		  {
		    sb_add_char (&out, in->ptr[src++]);
		  }
		if (in->ptr[src] == ')')
		  src++;
		else
		  ERROR ((stderr, "Missplaced ).\n"));
	      }
	    else if (in->ptr[src] == '@')
	      {
		/* Sub in the macro invocation number */

		char buffer[6];
		src++;
		sprintf (buffer, "%05d", number);
		sb_add_string (&out, buffer);
	      }
	    else if (in->ptr[src] == '&')
	      {
		/* This is a preprocessor variable name, we don't do them
		   here */
		sb_add_char (&out, '\\');
		sb_add_char (&out, '&');
		src++;
	      }
	    else
	      {
		sb_reset (&t);
		src = sub_actual (src, in, &t, m, '\'', &out, 0);
	      }
	  }
	else if (ISFIRSTCHAR (in->ptr[src]) && alternate)
	  {
		sb_reset (&t);
		src = sub_actual (src, in, &t, m, '\'', &out, 1);
	  }
	else if (ISCOMMENTCHAR (in->ptr[src])
		 && src + 1 <  in->len
		 && ISCOMMENTCHAR (in->ptr[src+1])
		 && !inquote)
	  {
	    /* Two comment chars in a row cause the rest of the line to be dropped */
	    while (src < in->len && in->ptr[src] != '\n')
	      src++;
	  }
	else if (in->ptr[src] == '"') 
	  {
	    inquote = !inquote;
	    sb_add_char (&out, in->ptr[src++]);
	  }
	else
	  {
	    sb_add_char (&out, in->ptr[src++]);
	  }
      }
    include_buf (name, &out, include_macro, include_next_index ());
  }
  sb_kill (&t);
  sb_kill (&out);
  number++;
}

static int
macro_op (idx, in)
     int idx;
     sb *in;
{
  int res = 0;
  /* The macro name must be the first thing on the line */
  if (idx < in->len)
    {
      sb name;
      hash_entry *ptr;
      sb_new (&name);
      idx = get_token (idx, in, &name);

      if (name.len)
	{
	  /* Got a name, look it up */

	  ptr = hash_lookup (&macro_table, &name);

	  if (ptr)
	    {
	      /* It's in the table, copy out the stuff and convert any macro args */
	      macro_expand (&name, idx, in, ptr->value.m);
	      res = 1;
	    }
	}
      sb_kill (&name);
    }


  return res;
}


/* STRING HANDLING */

static int
getstring (idx, in, acc)
     int idx;
     sb *in;
     sb *acc;
{
  idx = sb_skip_white (idx, in);

  while (idx < in->len
	 && (in->ptr[idx] == '"' 
	     || in->ptr[idx] == '<' 
	     || (in->ptr[idx] == '\'' && alternate)))
    {
      if (in->ptr[idx] == '<')
	{
	  if (alternate)
	    {
	      int nest = 0;
	      idx++;
	      while ((in->ptr[idx] != '>' || nest)
		     && idx < in->len)
		{
		  if (in->ptr[idx] == '!')
		    {
		      idx++  ;
		      sb_add_char (acc, in->ptr[idx++]);
		    }
		  else {
		    if (in->ptr[idx] == '>')
		      nest--;
		    if (in->ptr[idx] == '<')
		      nest++;
		    sb_add_char (acc, in->ptr[idx++]);
		  }
		}
	      idx++;
	    }
	  else {
	    int code;
	    idx++;
	    idx = exp_get_abs ("Character code in string must be absolute expression.\n",
			       idx, in, &code);
	    sb_add_char (acc, code);

	    if (in->ptr[idx] != '>')
	      ERROR ((stderr, "Missing > for character code.\n"));
	    idx++;
	  }
	}
      else if (in->ptr[idx] == '"' || in->ptr[idx] == '\'')
	{
	  char tchar = in->ptr[idx];
	  idx++;
	  while (idx < in->len)
	    {
	      if (alternate && in->ptr[idx] == '!')
		{
		  idx++  ;
		  sb_add_char (acc, in->ptr[idx++]);
		}
	      else {
		if (in->ptr[idx] == tchar)
		  {
		    idx++;
		    if (idx >= in->len || in->ptr[idx] != tchar)
		      break;
		  }
		sb_add_char (acc, in->ptr[idx]);
		idx++;
	      }
	    }
	}
    }
  
  return idx;
}

/* .SDATA[C|Z] <string> */

static
void
do_sdata (idx, in, type)
     int idx;
     sb *in;
     char type;
{
  int nc = 0;
  int pidx = -1;
  sb acc;
  sb_new (&acc);
  fprintf (outfile, ".byte\t");

  while (!eol (idx, in))
    {
      int i;
      sb_reset (&acc);
      idx = sb_skip_white (idx, in);
      while (!eol (idx, in))
	{
	  pidx = idx = get_any_string (idx, in, &acc, 0, 1);
	  if (type == 'c')
	    {
	      if (acc.len > 255)
		{
		  ERROR ((stderr, "string for SDATAC longer than 255 characters (%d).\n", acc.len));
		}
	      fprintf (outfile, "%d", acc.len);
	      nc = 1;
	    }

	  for (i = 0; i < acc.len; i++)
	    {
	      if (nc)
		{
		  fprintf (outfile, ",");
		}
	      fprintf (outfile, "%d", acc.ptr[i]);
	      nc = 1;
	    }

	  if (type == 'z')
	    {
	      if (nc)
		fprintf (outfile, ",");
	      fprintf (outfile, "0");
	    }
	  idx = sb_skip_comma (idx, in);
	  if (idx == pidx) break;
	}
      if (!alternate && in->ptr[idx] != ',' && idx != in->len)
	{
	  fprintf (outfile, "\n");
	  ERROR ((stderr, "illegal character in SDATA line (0x%x).\n", in->ptr[idx]));
	  break;
	}
      idx++;
    }
  sb_kill (&acc);
  fprintf (outfile, "\n");
}

/* .SDATAB <count> <string> */

static void
do_sdatab (idx, in)
     int idx;
     sb *in;
{
  int repeat;
  int i;
  sb acc;
  sb_new (&acc);

  idx = exp_get_abs ("Must have absolute SDATAB repeat count.\n", idx, in, &repeat);
  if (repeat <= 0)
    {
      ERROR ((stderr, "Must have positive SDATAB repeat count (%d).\n", repeat));
      repeat = 1;
    }

  idx = sb_skip_comma (idx, in);
  idx = getstring (idx, in, &acc);

  for (i = 0; i < repeat; i++)
    {
      if (i)
	fprintf (outfile, "\t");
      fprintf (outfile, ".byte\t");
      sb_print (&acc);
      fprintf (outfile, "\n");
    }
  sb_kill (&acc);

}

int
new_file (name)
     char *name;
{
  FILE *newone = fopen (name, "r");
  if (!newone)
    return 0;

  if (isp == MAX_INCLUDES)
    FATAL ((stderr, "Unreasonable include depth (%ld).\n", (long) isp));

  sp++;
  sp->handle = newone;

  sb_new (&sp->name);
  sb_add_string (&sp->name, name);

  sp->linecount = 1;
  sp->pushback_index = 0;
  sp->type = include_file;
  sp->index = 0;
  sb_new (&sp->pushback);
  return 1;
}

static void
do_include (idx, in)
     int idx;
     sb *in;
{
  sb t;
  char *text;
  sb_new (&t);
  idx = getstring (idx, in, &t);
  text = sb_name (&t);
  if (!new_file (text))
    {
      FATAL ((stderr, "Can't open include file `%s'.\n", text));
    }
  sb_kill (&t);
}

static void
include_pop ()
{
  if (sp != include_stack)
    {
      if (sp->handle)
	fclose (sp->handle);
      sp--;
    }
}

/* Get the next character from the include stack.  If there's anything
   in the pushback buffer, take that first.  If we're at eof, pop from
   the stack and try again.  Keep the linecount up to date. */

static int
get ()
{
  int r;

  if (sp->pushback.len != sp->pushback_index)
    {
      r = (char) (sp->pushback.ptr[sp->pushback_index++]);
      /* When they've all gone, reset the pointer */
      if (sp->pushback_index == sp->pushback.len)
	{
	  sp->pushback.len = 0;
	  sp->pushback_index = 0;
	}
    }
  else if (sp->handle)
    {
      r = getc (sp->handle);
    }
  else
    r = EOF;

  if (r == EOF && isp)
    {
      include_pop ();
      r = get ();
      while (r == EOF && isp)
	{
	  include_pop ();
	  r = get ();
	}
      return r;
    }
  if (r == '\n')
    {
      sp->linecount++;
    }

  return r;
}

static int
linecount ()
{
  return sp->linecount;
}

static int
include_next_index ()
{
  static int index;
  if (!unreasonable
      && index > MAX_REASONABLE)
    FATAL ((stderr, "Unreasonable expansion (-u turns off check).\n"));
  return ++index;
}


/* Initialize the chartype vector. */

static void
chartype_init ()
{
  int x;
  for (x = 0; x < 256; x++)
    {
      if (isalpha (x) || x == '_' || x == '$')
	chartype[x] |= FIRSTBIT;

      if (isdigit (x) || isalpha (x) || x == '_' || x == '$')
	chartype[x] |= NEXTBIT;

      if (x == ' ' || x == '\t' || x == ',' || x == '"' || x == ';'
	  || x == '"' || x == '<' || x == '>' || x == ')' || x == '(')
	chartype[x] |= SEPBIT;

      if (x == 'b' || x == 'B'
	  || x == 'q' || x == 'Q'
	  || x == 'h' || x == 'H'
	  || x == 'd' || x == 'D')
	chartype [x] |= BASEBIT;
	  
      if (x == ' ' || x == '\t')
	chartype[x] |= WHITEBIT;

      if (x == comment_char)
	chartype[x] |= COMMENTBIT;
    }
}



/* What to do with all the keywords */
#define PROCESS 	0x1000  /* Run substitution over the line */
#define LAB		0x2000  /* Spit out the label */

#define K_EQU 		PROCESS|1
#define K_ASSIGN 	PROCESS|2
#define K_REG 		PROCESS|3
#define K_ORG 		PROCESS|4
#define K_RADIX 	PROCESS|5
#define K_DATA 		LAB|PROCESS|6
#define K_DATAB 	LAB|PROCESS|7
#define K_SDATA 	LAB|PROCESS|8
#define K_SDATAB 	LAB|PROCESS|9
#define K_SDATAC 	LAB|PROCESS|10
#define K_SDATAZ	LAB|PROCESS|11
#define K_RES 		LAB|PROCESS|12
#define K_SRES 		LAB|PROCESS|13
#define K_SRESC 	LAB|PROCESS|14
#define K_SRESZ 	LAB|PROCESS|15
#define K_EXPORT 	LAB|PROCESS|16
#define K_GLOBAL 	LAB|PROCESS|17
#define K_PRINT 	LAB|PROCESS|19
#define K_FORM 		LAB|PROCESS|20
#define K_HEADING	LAB|PROCESS|21
#define K_PAGE		LAB|PROCESS|22
#define K_IMPORT	LAB|PROCESS|23
#define K_PROGRAM	LAB|PROCESS|24
#define K_END		PROCESS|25
#define K_INCLUDE	PROCESS|26
#define K_IGNORED	PROCESS|27
#define K_ASSIGNA	PROCESS|28
#define K_ASSIGNC	29
#define K_AIF		PROCESS|30
#define K_AELSE		PROCESS|31
#define K_AENDI		PROCESS|32
#define K_AREPEAT	PROCESS|33
#define K_AENDR		PROCESS|34
#define K_AWHILE	35
#define K_AENDW		PROCESS|36
#define K_EXITM		37
#define K_MACRO		PROCESS|38
#define K_ENDM		39
#define K_ALIGN		PROCESS|LAB|40
#define K_ALTERNATE     41
#define K_DB		LAB|PROCESS|42
#define K_DW		LAB|PROCESS|43
#define K_DL		LAB|PROCESS|44
#define K_LOCAL		45


static struct
{
  char *name;
  int code;
  int extra;
}
kinfo[] =
{
  { "EQU", K_EQU, 0 },
  { "ALTERNATE", K_ALTERNATE, 0 },
  { "ASSIGN", K_ASSIGN, 0 },
  { "REG", K_REG, 0 },
  { "ORG", K_ORG, 0 },
  { "RADIX", K_RADIX, 0 },
  { "DATA", K_DATA, 0 },
  { "DB", K_DB, 0 },
  { "DW", K_DW, 0 },
  { "DL", K_DL, 0 },
  { "DATAB", K_DATAB, 0 },
  { "SDATA", K_SDATA, 0 },
  { "SDATAB", K_SDATAB, 0 },
  { "SDATAZ", K_SDATAZ, 0 },
  { "SDATAC", K_SDATAC, 0 },
  { "RES", K_RES, 0 },
  { "SRES", K_SRES, 0 },
  { "SRESC", K_SRESC, 0 },
  { "SRESZ", K_SRESZ, 0 },
  { "EXPORT", K_EXPORT, 0 },
  { "GLOBAL", K_GLOBAL, 0 },
  { "PRINT", K_PRINT, 0 },
  { "FORM", K_FORM, 0 },
  { "HEADING", K_HEADING, 0 },
  { "PAGE", K_PAGE, 0 },
  { "PROGRAM", K_IGNORED, 0 },
  { "END", K_END, 0 },
  { "INCLUDE", K_INCLUDE, 0 },
  { "ASSIGNA", K_ASSIGNA, 0 },
  { "ASSIGNC", K_ASSIGNC, 0 },
  { "AIF", K_AIF, 0 },
  { "AELSE", K_AELSE, 0 },
  { "AENDI", K_AENDI, 0 },
  { "AREPEAT", K_AREPEAT, 0 },
  { "AENDR", K_AENDR, 0 },
  { "EXITM", K_EXITM, 0 },
  { "MACRO", K_MACRO, 0 },
  { "ENDM", K_ENDM, 0 },
  { "AWHILE", K_AWHILE, 0 },
  { "ALIGN", K_ALIGN, 0 },
  { "AENDW", K_AENDW, 0 },
  { "ALTERNATE", K_ALTERNATE, 0 },
  { "LOCAL", K_LOCAL, 0 },
  { NULL, 0, 0 }
};

/* Look for a pseudo op on the line. If one's there then call
   its handler. */

static int
process_pseudo_op (idx, line, acc)
     int idx;
     sb *line;
     sb *acc;
{


  if (line->ptr[idx] == '.' || alternate)
    {
      /* Scan forward and find pseudo name */
      char *in;
      hash_entry *ptr;

      char *s;
      char *e;
      if (line->ptr[idx] == '.')
	idx++;
      in = line->ptr + idx;
      s = in;
      e = s;
      sb_reset (acc);

      while (idx < line->len && *e && ISFIRSTCHAR (*e))
	{
	  sb_add_char (acc, *e);
	  e++;
	  idx++;
	}

      ptr = hash_lookup (&keyword_hash_table, acc);

      if (!ptr)
	{
#if 0
	  /* This one causes lots of pain when trying to preprocess
	     ordinary code */
	  WARNING ((stderr, "Unrecognised pseudo op `%s'.\n", sb_name (acc)));
#endif
	  return 0;
	}
      if (ptr->value.i & LAB)
	{			/* output the label */
	  if (label.len)
	    {
	      fprintf (outfile, "%s:\t", sb_name (&label));
	    }
	  else
	    fprintf (outfile, "\t");
	}

      if (ptr->value.i & PROCESS)
	{
	  /* Polish the rest of the line before handling the pseudo op */
#if 0
	  strip_comments(line);
#endif
	  sb_reset (acc);
	  process_assigns (idx, line, acc);
	  sb_reset(line);
	  change_base (0, acc, line);
	  idx = 0;
	}
      if (!condass_on ())
	{
	  switch (ptr->value.i)
	    {
	    case K_AIF:
	      do_aif (idx, line);
	      break;
	    case K_AELSE:
	      do_aelse ();
	      break;
	    case K_AENDI:
	      do_aendi ();
	      break;
	    }
	  return 1;
	}
      else
	{
	  switch (ptr->value.i)
	    {
	    case K_ALTERNATE:
	      alternate = 1;
	      return 1;
	    case K_AELSE:
	      do_aelse ();
	      return 1;
	    case K_AENDI:
	      do_aendi ();
	      return 1;
	    case K_ORG:
	      ERROR ((stderr, "ORG command not allowed.\n"));
	      break;
	    case K_RADIX:
	      do_radix (line);
	      return 1;
	    case K_DB:
	      do_data (idx, line, 1);
	      return 1;
	    case K_DW:
	      do_data (idx, line, 2);
	      return 1;
	    case K_DL:
	      do_data (idx, line, 4);
	      return 1;
	    case K_DATA:
	      do_data (idx, line, 0);
	      return 1;
	    case K_DATAB:
	      do_datab (idx, line);
	      return 1;
	    case K_SDATA:
	      do_sdata (idx, line, 0);
	      return 1;
	    case K_SDATAB:
	      do_sdatab (idx, line);
	      return 1;
	    case K_SDATAC:
	      do_sdata (idx, line, 'c');
	      return 1;
	    case K_SDATAZ:
	      do_sdata (idx, line, 'z');
	      return 1;
	    case K_ASSIGN:
	      do_assign (1, 0, line);
	      return 1;
	    case K_AIF:
	      do_aif (idx, line);
	      return 1;
	    case K_AREPEAT:
	      do_arepeat (idx, line);
	      return 1;
	    case K_AENDW:
	      do_aendw ();
	      return 1;
	    case K_AWHILE:
	      do_awhile (idx, line);
	      return 1;
	    case K_AENDR:
	      do_aendr ();
	      return 1;
	    case K_EQU:
	      do_assign (0, idx, line);
	      return 1;
	    case K_ALIGN:
	      do_align (idx, line);
	      return 1;
	    case K_RES:
	      do_res (idx, line, 0);
	      return 1;
	    case K_SRES:
	      do_res (idx, line, 's');
	      return 1;
	    case K_INCLUDE:
	      do_include (idx, line);
	      return 1;
	    case K_LOCAL:
	      do_local (idx, line);
	      return 1;
	    case K_MACRO:
	      do_macro (idx, line);
	      return 1;
	    case K_ENDM:
	      do_endm ();
	      return 1;
	    case K_SRESC:
	      do_res (idx, line, 'c');
	      return 1;
	    case K_PRINT:
	      do_print (idx, line);
	      return 1;
	    case K_FORM:
	      do_form (idx, line);
	      return 1;
	    case K_HEADING:
	      do_heading (idx, line);
	      return 1;
	    case K_PAGE:
	      do_page ();
	      return 1;
	    case K_GLOBAL:
	    case K_EXPORT:
	      do_export (line);
	      return 1;
	    case K_IMPORT:
	      return 1;
	    case K_SRESZ:
	      do_res (idx, line, 'z');
	      return 1;
	    case K_IGNORED:
	      return 1;
	    case K_END:
	      do_end ();
	      return 1;
	    case K_ASSIGNA:
	      do_assigna (idx, line);
	      return 1;
	    case K_ASSIGNC:
	      do_assignc (idx, line);
	      return 1;
	    case K_EXITM:
	      do_exitm ();
	      return 1;
	    case K_REG:
	      do_reg (idx, line);
	      return 1;
	    }
	}
    }
  return 0;
}



/* Build the keyword hash table - put each keyword in the table twice,
   once upper and once lower case.*/

static void
process_init ()
{
  int i;

  for (i = 0; kinfo[i].name; i++)
    {
      sb label;
      int j;
      sb_new (&label);
      sb_add_string (&label, kinfo[i].name);

      hash_add_to_int_table (&keyword_hash_table, &label, kinfo[i].code);

      sb_reset (&label);
      for (j = 0; kinfo[i].name[j]; j++)
	sb_add_char (&label, kinfo[i].name[j] - 'A' + 'a');
      hash_add_to_int_table (&keyword_hash_table, &label, kinfo[i].code);

      sb_kill (&label);
    }
}


static void
do_define (string)
char *string;
{
  sb label;
  int res = 1;
  hash_entry *ptr;
  sb_new (&label);


  while (*string)
    {
      if (*string == '=') 
	{
	  sb value;
	  sb_new (&value);
	  string++;
	  while (*string)
	    {
	      sb_add_char (&value, *string);
	      string++;
	    }
	  exp_get_abs ("Invalid expression on command line.\n", 0, &value, &res);
	  sb_kill (&value);
	  break;
	}
      sb_add_char (&label, *string);

      string ++;
    }

  ptr = hash_create (&vars, &label);
  free_old_entry (ptr);
  ptr->type = hash_integer;
  ptr->value.i = res;
  sb_kill (&label);
}
char *program_name;

/* The list of long options.  */
static struct option long_options[] =
{
  { "alternate", no_argument, 0, 'a' },
  { "commentchar", required_argument, 0, 'c' },
  { "copysource", no_argument, 0, 's' },
  { "debug", no_argument, 0, 'd' },
  { "help", no_argument, 0, 'h' },
  { "output", required_argument, 0, 'o' },
  { "print", no_argument, 0, 'p' },
  { "unreasonable", no_argument, 0, 'u' },
  { "version", no_argument, 0, 'v' },
  { "define", required_argument, 0, 'd' },
  { NULL, no_argument, 0, 0 }
};

/* Show a usage message and exit.  */
static void
show_usage (file, status)
     FILE *file;
     int status;
{
  fprintf (file, "\
Usage: %s \n\
  [-a]      [--alternate]         enter alternate macro mode\n\
  [-c char] [--commentchar char]  change the comment character from !\n\
  [-d]      [--debug]             print some debugging info\n\
  [-h]      [--help]              print this message\n\
  [-o out]  [--output out]        set the output file\n\
  [-p]      [--print]             print line numbers\n\
  [-s]      [--copysource]        copy source through as comments \n\
  [-u]      [--unreasonable]      allow unreasonable nesting\n\
  [-v]      [--version]           print the program version\n\
  [-Dname=value]                  create preprocessor variable called name, with value\n\
  [in-file]\n",   program_name);
  exit (status);
}

/* Display a help message and exit.  */
static void
show_help ()
{
  printf ("%s: Gnu Assembler Macro Preprocessor\n",
	  program_name);
  show_usage (stdout, 0);
}

int
main (argc, argv)
     int argc;
     char **argv;
{
  int opt;
  char *out_name = 0;
  sp = include_stack;

  ifstack[0].on = 1;
  ifi = 0;



  program_name = argv[0];
  xmalloc_set_program_name (program_name);

  hash_new_table (101, &macro_table);
  hash_new_table (101, &keyword_hash_table);
  hash_new_table (101, &assign_hash_table);
  hash_new_table (101, &vars);

  sb_new (&label);
  process_init ();

  while ((opt = getopt_long (argc, argv, "sdhavc:upo:D:", long_options,
			     (int *) NULL))
	 != EOF)
    {
      switch (opt)
	{
	case 'o':
	  out_name = optarg;
	  break;
	case 'u':
	  unreasonable = 1;
	  break;
	case 'p':
	  print_line_number = 1;
	  break;
	case 'c':
	  comment_char = optarg[0];
	  break;
	case 'a':
	  alternate = 1;
	  break;
	case 's':
	  copysource = 1;
	  break;
	case 'd':
	  stats = 1;
	  break;
	case 'D':
	  do_define (optarg);
	  break;
	case 'h':
	  show_help ();
	  /*NOTREACHED*/
	case 'v':
	  printf ("GNU %s version %s\n", program_name, program_version);
	  exit (0);
	  /*NOTREACHED*/
	case 0:
	  break;
	default:
	  show_usage (stderr, 1);
	  /*NOTREACHED*/
	}
    }


  if (out_name) {
    outfile = fopen (out_name, "w");
    if (!outfile)
      {
	fprintf (stderr, "%s: Can't open output file `%s'.\n",
		 program_name, out_name);
	exit (1);
      }
  }
  else  {
    outfile = stdout;
  }

  chartype_init ();
  if (!outfile)
    outfile = stdout;

  /* Process all the input files */

  while (optind < argc)
    {
      if (new_file (argv[optind]))
	{
	  process_file ();
	}
      else
	{
	  fprintf (stderr, "%s: Can't open input file `%s'.\n",
		   program_name, argv[optind]);
	  exit (1);
	}
      optind++;
    }

  quit ();
  return 0;
}
