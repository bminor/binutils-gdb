/* Demangler for GNU C++ 
   Copyright 1989, 1991 Free Software Foundation, Inc.
   written by James Clark (jjc@jclark.uucp)
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* This is for g++ 1.95.03 (November 13 verison).  */

/* This file exports one function

   char *cplus_demangle (const char *name, int mode)

   If NAME is a mangled function name produced by GNU C++, then
   a pointer to a malloced string giving a C++ representation
   of the name will be returned; otherwise NULL will be returned.
   It is the caller's responsibility to free the string which
   is returned.

   If MODE > 0, then ANSI qualifiers such as `const' and `void' are output.
   Otherwise they are not.
   If MODE >= 0, parameters are emitted; otherwise not.

   For example,
   
   cplus_demangle ("foo__1Ai",  0)	=> "A::foo(int)"
   cplus_demangle ("foo__1Ai",  1)	=> "A::foo(int)"
   cplus_demangle ("foo__1Ai", -1)	=> "A::foo"

   cplus_demangle ("foo__1Afe",  0)	=> "A::foo(float,...)"
   cplus_demangle ("foo__1Afe",  1)	=> "A::foo(float,...)"
   cplus_demangle ("foo__1Afe", -1)	=> "A::foo"

   This file imports xmalloc and xrealloc, which are like malloc and
   realloc except that they generate a fatal error if there is no
   available memory. */

/* define this if names don't start with _ */
/* #define nounderscore 1 */

/* GDB-specific, FIXME.  */
#include "defs.h"

#include <ctype.h>

#ifdef USG
#include <memory.h>
#include <string.h>
#else
#include <strings.h>
#endif

/* This is '$' on systems where the assembler can deal with that.
   Where the assembler can't, it's '.' (but on many systems '.' is
   used for other things).  */
#if !defined (CPLUS_MARKER)
#define CPLUS_MARKER '$'
#endif

#ifndef __STDC__
#define const
#endif

#ifdef __STDC__
extern char *cplus_demangle (const char *type, int mode);
#else
extern char *cplus_demangle ();
#endif

static char **typevec = 0;
static int ntypes = 0;
static int typevec_size = 0;

const static struct optable {
  const char *in;
  const char *out;
  int ansi;
} optable[] = {
  "nw", " new",	1,		/* new (1.92, ansi) */
  "dl", " delete", 1,		/* new (1.92, ansi) */
  "new", " new", 0,		/* old (1.91, and 1.x) */
  "delete", " delete", 0,	/* old (1.91, and 1.x) */
  "as", "=", 1,			/* ansi */
  "ne", "!=", 1,		/* old, ansi */
  "eq", "==", 1,		/* old, ansi */
  "ge", ">=", 1,		/* old, ansi */
  "gt", ">", 1,			/* old, ansi */
  "le", "<=", 1,		/* old, ansi */
  "lt", "<", 1,			/* old, ansi */
  "plus", "+", 0,		/* old */
  "pl", "+", 1,			/* ansi */
  "apl", "+=", 1,		/* ansi */
  "minus", "-", 0,		/* old */
  "mi", "-", 1,			/* ansi */
  "ami", "-=", 1,		/* ansi */
  "mult", "*", 0,		/* old */
  "ml", "*", 1,			/* ansi */
  "aml", "*=", 1,		/* ansi */
  "convert", "+", 0,		/* old (unary +) */
  "negate", "-", 0,		/* old (unary -) */
  "trunc_mod", "%", 0,		/* old */
  "md", "%", 1,			/* ansi */
  "amd", "%=", 1,		/* ansi */
  "trunc_div", "/", 0,		/* old */
  "dv", "/", 1,			/* ansi */
  "adv", "/=", 1,		/* ansi */
  "truth_andif", "&&", 0,	/* old */
  "aa", "&&", 1,		/* ansi */
  "truth_orif", "||", 0,	/* old */
  "oo", "||", 1,		/* ansi */
  "truth_not", "!", 0,		/* old */
  "nt", "!", 1,			/* ansi */
  "postincrement", "++", 0,	/* old */
  "pp", "++", 1,		/* ansi */
  "postdecrement", "--", 0,	/* old */
  "mm", "--", 1,		/* ansi */
  "bit_ior", "|", 0,		/* old */
  "or", "|", 1,			/* ansi */
  "aor", "|=", 1,		/* ansi */
  "bit_xor", "^", 0,		/* old */
  "er", "^", 1,			/* ansi */
  "aer", "^=", 1,		/* ansi */
  "bit_and", "&", 0,		/* old */
  "ad", "&", 1,			/* ansi */
  "aad", "&=", 1,		/* ansi */
  "bit_not", "~", 0,		/* old */
  "co", "~", 1,			/* ansi */
  "call", "()", 0,		/* old */
  "cl", "()", 1,		/* ansi */
  "alshift", "<<", 0,		/* old */
  "ls", "<<", 1,		/* ansi */
  "als", "<<=", 1,		/* ansi */
  "arshift", ">>", 0,		/* old */
  "rs", ">>", 1,		/* ansi */
  "ars", ">>=", 1,		/* ansi */
  "component", "->", 0,		/* old */
  "rf", "->", 1,		/* ansi */
  "indirect", "*", 0,		/* old */
  "method_call", "->()", 0,	/* old */
  "addr", "&", 0,		/* old (unary &) */
  "array", "[]", 0,		/* old */
  "vc", "[]", 1,		/* ansi */
  "compound", ",", 0,		/* old */
  "cm", ",", 1,			/* ansi */
  "cond", "?:", 0,		/* old */
  "cn", "?:", 1,		/* psuedo-ansi */
  "max", ">?", 0,		/* old */
  "mx", ">?", 1,		/* psuedo-ansi */
  "min", "<?", 0,		/* old */
  "mn", "<?", 1,		/* psuedo-ansi */
  "nop", "", 0,			/* old (for operator=) */
};

/* Beware: these aren't '\0' terminated. */

typedef struct string {
  char *b;			/* pointer to start of string */
  char *p;			/* pointer after last character */
  char *e;			/* pointer after end of allocated space */
} string;

static void
string_need PARAMS ((string *, int));

static void
string_delete PARAMS ((string *));

static void
string_init PARAMS ((string *));

static void
string_clear PARAMS ((string *));

static int
string_empty PARAMS ((string *));

static void
string_append PARAMS ((string *, const char *));

static void
string_appends PARAMS ((string *, string *));

static void
string_appendn PARAMS ((string *, const char *, int));

static void
string_prepend PARAMS ((string *, const char *));

static void
string_prependn PARAMS ((string *, const char *, int));

static int
get_count PARAMS ((const char **, int *));

static int
do_args PARAMS ((const char **, string *, int));

static int
do_type PARAMS ((const char **, string *, int));

static int
do_arg PARAMS ((const char **, string *, int));

static void
munge_function_name PARAMS ((string *, int));

static void
remember_type PARAMS ((const char *, int));

#if 0
static void
string_prepends PARAMS ((string *, string *));
#endif


/* Takes operator name as e.g. "++" and returns mangled
   operator name (e.g. "postincrement_expr"), or NULL if not found.

   If ARG_MODE == 1, return the ANSI name;
   if ARG_MODE == 0 return the old GNU name.  */
char *
cplus_mangle_opname (opname, arg_mode)
     char *opname;
     int arg_mode;
{
  int i, len = strlen (opname);

  if (arg_mode != 0 && arg_mode != 1)
    error ("invalid arg_mode");

  for (i = 0; i < sizeof (optable)/sizeof (optable[0]); i++)
    {
      if (strlen (optable[i].out) == len
	  && arg_mode == optable[i].ansi
	  && memcmp (optable[i].out, opname, len) == 0)
	return (char *)optable[i].in;
    }
  return 0;
}

char *
cplus_demangle (type, arg_mode)
     const char *type;
     int arg_mode;
{
  string decl;
  int n;
  int success = 0;
  int constructor = 0;
  int destructor = 0;
  int static_type = 0;
  int const_flag = 0;
  int i;
  const char *p;
#ifndef LONGERNAMES
  const char *premangle;
#endif

# define print_ansi_qualifiers (arg_mode >  0)
# define print_arg_types       (arg_mode >= 0)

  if (type == NULL || *type == '\0')
    return NULL;
#ifndef nounderscore
  if (*type++ != '_')
    return NULL;
#endif
  p = type;
  while (*p != '\0' && !(*p == '_' && p[1] == '_'))
    p++;
  if (*p == '\0')
    {
      /* destructor */
      if (type[0] == '_' && type[1] == CPLUS_MARKER && type[2] == '_')
	{
	  int n;
	  char *tem;

	  type += 3;		/* Get past _$_ at front.  */
	  while (isdigit (*type))
	    /* If there are digits at the front, it's because
	       of new 2.0 name mangling.  Just skip them.  */
	    type++;

	  n = strlen (type)*2 + 3 + 2 + 1;
	  tem = (char *) xmalloc (n);
	  strcpy (tem, type);
	  strcat (tem, "::~");
	  strcat (tem, type);
	  if (print_arg_types)
	    strcat (tem, "()");
	  return tem;
	}
      /* static data member */
      if (*type != '_' && (p = strchr (type, CPLUS_MARKER)) != NULL)
	{
	  int n = strlen (type) + 2;
	  char *tem = (char *) xmalloc (n);
	  memcpy (tem, type, p - type);
	  strcpy (tem + (p - type), "::");
	  strcpy (tem + (p - type) + 2, p + 1);
	  return tem;
	}
      /* virtual table "_vt$"  */
      if (type[0] == '_' && type[1] == 'v' && type[2] == 't' && type[3] == CPLUS_MARKER)
	{
	  int n = strlen (type + 4) + 14 + 1;
	  char *tem = (char *) xmalloc (n);
	  strcpy (tem, type + 4);
	  strcat (tem, " virtual table");
	  return tem;
	}
      return NULL;
    }

  string_init (&decl);

  if (static_type)
    {
      if (!isdigit (p[0]) && ('t' != p[0]))
	{
	  string_delete (&decl);
	  return NULL;
	}
    }
  else if (p == type)
    {
      if (!isdigit (p[2]) && ('t' != p[2]))
	{
	  p += 1;
	  while (*p != '\0' && !(*p == '_' && p[1] == '_'))
	    p++;
	  string_appendn (&decl, type, p - type);
	  string_need (&decl, 1);
	  *(decl.p) = '\0';
	  munge_function_name (&decl, 1);
	  if (decl.b[0] == '_')
	    {
	      string_delete (&decl);
	      return NULL;
	    }
	  else
	    p += 2;
	}
      else
	{
	  constructor = 1;
	  p += 2;
	}
    }
  else
    {
      string_appendn (&decl, type, p - type);
      string_need (&decl, 1);
      *(decl.p) = '\0';
      munge_function_name (&decl, arg_mode);
      p += 2;
    }

#ifndef LONGERNAMES
  premangle = p;
#endif
  switch (*p)
    {
    case 'C':
      /* a const member function */
      if (!isdigit (p[1]))
	{
	  string_delete (&decl);
	  return NULL;
	}
      p += 1;
      const_flag = 1;
      /* fall through */
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
      n = 0;
      do
	{
	  n *= 10;
	  n += *p - '0';
	  p += 1;
	}
      while (isdigit (*p));
      if (strlen (p) < n)
	{
	  string_delete (&decl);
	  return NULL;
	}
      if (constructor || destructor)
	{
	  string_appendn (&decl, p, n);
	  string_append (&decl, "::");
	  if (destructor)
	    string_append(&decl, "~");
	  string_appendn (&decl, p, n);
	}
      else
	{
	  string_prepend (&decl, "::");
	  string_prependn (&decl, p, n);
	}
      p += n;
#ifndef LONGERNAMES
      remember_type (premangle, p - premangle);
#endif
      if (static_type)
	{
	  string_append(&decl, p+1);
	  p += strlen(p);
	  success = 1;
	}
      else
	success = do_args (&p, &decl, arg_mode);
      if (const_flag && print_arg_types)
	string_append (&decl, " const");
      break;
    case 'F':
      p += 1;
      success = do_args (&p, &decl, arg_mode);
      break;
    /* template additions */
    case 't':
      p += 1;
      {
	int r, i;
	string tname;
	string trawname;
	
	string temp;
	int need_comma = 0;
	
	string_init(&tname);
	string_init(&trawname);
	
	/* get template name */
	if (!get_count (&p, &r))
	  return 0;
	string_appendn (&tname, p, r);
	string_appendn (&trawname, p, r);
	string_appendn (&trawname, "", 1);
	p += r;
	string_append (&tname, "<");
	/* get size of template parameter list */
	if (!get_count (&p, &r))
	  return 0;
	for (i = 0; i < r; i++)
	  {
	    if (need_comma)
	      string_append (&tname, ", ");
	    /* Z for type parameters */
	    if (*p == 'Z')
	      {
		p += 1;
		
		success = do_type (&p, &temp, arg_mode);
		string_appendn (&temp, "", 1);
		if (success)
		  string_append (&tname, temp.b);
		string_delete(&temp);
		if (!success)
		  break;
	      }
	    /* otherwise, value parameter */
	    else
	      {
		const char *old_p  = p;
		int is_pointer = 0;
		int is_real = 0;
		int is_integral = 0;
		int done = 0;

		success = do_type (&p, &temp, arg_mode);
		string_appendn (&temp, "", 1);
		if (success)
		  string_append (&tname, temp.b);
		string_delete(&temp);
		if (!success)
		  break;
		string_append (&tname, "=");
		while (*old_p && !done)
		  {	
		    switch (*old_p)
		      {
		      case 'P':
		      case 'R':
			done = is_pointer = 1;
			break;
		      case 'C':	/* const */
		      case 'S':	/* explicitly signed [char] */
		      case 'U':	/* unsigned */
		      case 'V':	/* volatile */
		      case 'F':	/* function */
		      case 'M':	/* member function */
		      case 'O':	/* ??? */
			old_p++;
			continue;
		      case 'Q':	/* repetition of following */
		      case 'T':	/* remembered type */
			abort();
			break;
		      case 'v':	/* void */
			abort();
			break;
		      case 'x':	/* long long */
		      case 'l':	/* long */
		      case 'i':	/* int */
		      case 's':	/* short */
		      case 'c':	/* char */
			done = is_integral = 1;
			break;
		      case 'r':	/* long double */
		      case 'd':	/* double */
		      case 'f':	/* float */
			done = is_real = 1;
			break;
		      default:
			abort();
		      }
		  }
		if (is_integral)
		  {
		    if (*p == 'm')
		      {
			string_appendn (&tname, "-", 1);
			p++;
		      }
		    while (isdigit (*p))	
		      {
			string_appendn (&tname, p, 1);
			p++;
		      }
		  }
		else if (is_real)
		  {
		    if (*p == 'm')
		      {
			string_appendn (&tname, "-", 1);
			p++;
		      }
		    while (isdigit (*p))	
		      {
			string_appendn (&tname, p, 1);
			p++;
		      }
		    if (*p == '.') /* fraction */
		      {
			string_appendn (&tname, ".", 1);
			p++;
			while (isdigit (*p))	
			  {
			    string_appendn (&tname, p, 1);
			    p++;
			  }
		      }
		    if (*p == 'e') /* exponent */
		      {
			string_appendn (&tname, "e", 1);
			p++;
			while (isdigit (*p))	
			  {
			    string_appendn (&tname, p, 1);
			    p++;
			  }
		      }
		  }
		else if (is_pointer)
		  {
		    int symbol_len;
		    
		    if (!get_count (&p, &symbol_len))
		      {
			success = 0;
			break;
		      }
		    string_appendn (&tname, p, symbol_len);
		    p += symbol_len;
		  }
	      }
	    need_comma = 1;
	  }
	string_append (&tname, ">::");
	if (destructor)
	  string_append(&tname, "~");
	if (constructor || destructor) {
	  string_append (&tname, trawname.b);
	}
	string_delete(&trawname);
	
	if (!success) {
	  string_delete(&tname);
	  return 0;
	}
	string_prepend (&decl, tname.b);
	string_delete(&tname);

	if (static_type)
	  {
	    string_append(&decl, p+1);
	    p += strlen(p);
	    success = 1;
	  }
	else
	  success = do_args (&p, &decl, arg_mode);
	break;
      }
    }

  for (i = 0; i < ntypes; i++)
    if (typevec[i] != NULL)
      free (typevec[i]);
  ntypes = 0;
  if (typevec != NULL)
    {
      free ((char *)typevec);
      typevec = NULL;
      typevec_size = 0;
    }

  if (success)
    {
      string_appendn (&decl, "", 1);
      return decl.b;
    }
  else
    {
      string_delete (&decl);
      return NULL;
    }
}

static int
get_count (type, count)
     const char **type;
     int *count;
{
  if (!isdigit (**type))
    return 0;
  *count = **type - '0';
  *type += 1;
  /* see flush_repeats in cplus-method.c */
  if (isdigit (**type))
    {
      const char *p = *type;
      int n = *count;
      do 
	{
	  n *= 10;
	  n += *p - '0';
	  p += 1;
	} 
      while (isdigit (*p));
      if (*p == '_')
	{
	  *type = p + 1;
	  *count = n;
	}
    }
  return 1;
}

/* result will be initialised here; it will be freed on failure */

static int
do_type (type, result, arg_mode)
     const char **type;
     string *result;
     int arg_mode;
{
  int n;
  int done;
  int non_empty = 0;
  int success;
  string decl;
  const char *remembered_type;

  string_init (&decl);
  string_init (result);

  done = 0;
  success = 1;
  while (success && !done)
    {
      int member;
      switch (**type)
	{
	case 'Q':
	  n = (*type)[1] - '0';
	  if (n < 0 || n > 9)
	    success = 0;
	  *type += 2;
	  while (n-- > 0)
	    do_type (type, result, arg_mode);
	  break;

	case 'P':
	  *type += 1;
	  string_prepend (&decl, "*");
	  break;

	case 'R':
	  *type += 1;
	  string_prepend (&decl, "&");
	  break;

	case 'T':
	  *type += 1;
	  if (!get_count (type, &n) || n >= ntypes)
	    success = 0;
	  else
	    {
	      remembered_type = typevec[n];
	      type = &remembered_type;
	    }
	  break;

	case 'F':
	  *type += 1;
	  if (!string_empty (&decl) && decl.b[0] == '*')
	    {
	      string_prepend (&decl, "(");
	      string_append (&decl, ")");
	    }
	  if (!do_args (type, &decl, arg_mode) || **type != '_')
	    success = 0;
	  else
	    *type += 1;
	  break;

	case 'M':
	case 'O':
	  {
	    int constp = 0;
	    int volatilep = 0;

	    member = **type == 'M';
	    *type += 1;
	    if (!isdigit (**type))
	      {
		success = 0;
		break;
	      }
	    n = 0;
	    do
	      {
		n *= 10;
		n += **type - '0';
		*type += 1;
	      } 
	    while (isdigit (**type));
	    if (strlen (*type) < n)
	      {
		success = 0;
		break;
	      }
	    string_append (&decl, ")");
	    string_prepend (&decl, "::");
	    string_prependn (&decl, *type, n);
	    string_prepend (&decl, "(");
	    *type += n;
	    if (member)
	      {
		if (**type == 'C')
		  {
		    *type += 1;
		    constp = 1;
		  }
		if (**type == 'V')
		  {
		    *type += 1;
		    volatilep = 1;
		  }
		if (*(*type)++ != 'F')
		  {
		    success = 0;
		    break;
		  }
	      }
	    if ((member && !do_args (type, &decl, arg_mode)) || **type != '_')
	      {
		success = 0;
		break;
	      }
	    *type += 1;
	    if (! print_ansi_qualifiers)
	      break;
	    if (constp)
	      {
		if (non_empty)
		  string_append (&decl, " ");
		else
		  non_empty = 1;
		string_append (&decl, "const");
	      }
	    if (volatilep)
	      {
		if (non_empty)
		  string_append (&decl, " ");
		else
		  non_empty = 1;
		string_append (&decl, "volatile");
	      }
	    break;
	  }

	case 'C':
	  if ((*type)[1] == 'P')
	    {
	      *type += 1;
	      if (print_ansi_qualifiers)
		{
		  if (!string_empty (&decl))
		    string_prepend (&decl, " ");
		  string_prepend (&decl, "const");
		}
	      break;
	    }

	  /* fall through */
	default:
	  done = 1;
	  break;
	}
    }

  done = 0;
  non_empty = 0;
  while (success && !done)
    {
      switch (**type)
	{
	case 'C':
	  *type += 1;
	  if (print_ansi_qualifiers)
	    {
	      if (non_empty)
		string_append (result, " ");
	      else
		non_empty = 1;
	      string_append (result, "const");
	    }
	  break;
	case 'U':
	  *type += 1;
	  if (non_empty)
	    string_append (result, " ");
	  else
	    non_empty = 1;
	  string_append (result, "unsigned");
	  break;
	case 'S': /* signed char only */
	  *type += 1;
	  if (non_empty)
	    string_append (result, " ");
	  else
	    non_empty = 1;
	  string_append (result, "signed");
	  break;
	case 'V':
	  *type += 1;
	  if (print_ansi_qualifiers)
	    {
	      if (non_empty)
		string_append (result, " ");
	      else
		non_empty = 1;
	      string_append (result, "volatile");
	    }
	  break;
	default:
	  done = 1;
	  break;
	}
    }

  if (success)
    switch (**type)
      {
      case '\0':
      case '_':
	break;
      case 'v':
	*type += 1;
	if (non_empty)
	  string_append (result, " ");
	string_append (result, "void");
	break;
      case 'x':
	*type += 1;
	if (non_empty)
	  string_append (result, " ");
	string_append (result, "long long");
	break;
      case 'l':
	*type += 1;
	if (non_empty)
	  string_append (result, " ");
	string_append (result, "long");
	break;
      case 'i':
	*type += 1;
	if (non_empty)
	  string_append (result, " ");
	string_append (result, "int");
	break;
      case 's':
	*type += 1;
	if (non_empty)
	  string_append (result, " ");
	string_append (result, "short");
	break;
      case 'c':
	*type += 1;
	if (non_empty)
	  string_append (result, " ");
	string_append (result, "char");
	break;
      case 'r':
	*type += 1;
	if (non_empty)
	  string_append (result, " ");
	string_append (result, "long double");
	break;
      case 'd':
	*type += 1;
	if (non_empty)
	  string_append (result, " ");
	string_append (result, "double");
	break;
      case 'f':
	*type += 1;
	if (non_empty)
	  string_append (result, " ");
	string_append (result, "float");
	break;
      case 'G':
	*type += 1;
	if (!isdigit (**type))
	  {
	    success = 0;
	    break;
	  }
	/* fall through */
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
	n = 0;
	do
	  {
	    n *= 10;
	    n += **type - '0';
	    *type += 1;
	  }
	while (isdigit (**type));
	if (strlen (*type) < n)
	  {
	    success = 0;
	    break;
	  }
	if (non_empty)
	  string_append (result, " ");
	string_appendn (result, *type, n);
	*type += n;
	break;
      default:
	success = 0;
	break;
      }

  if (success)
    {
      if (!string_empty (&decl))
	{
	  string_append (result, " ");
	  string_appends (result, &decl);
	}
      string_delete (&decl);
      return 1;
    }
  else
    {
      string_delete (&decl);
      string_delete (result);
      return 0;
    }
}

/* `result' will be initialised in do_type; it will be freed on failure */

static int
do_arg (type, result, arg_mode)
     const char **type;
     string *result;
     int arg_mode;
{
  const char *start = *type;

  if (!do_type (type, result, arg_mode))
    return 0;
  remember_type (start, *type - start);
  return 1;
}

static void
remember_type (start, len)
     const char *start;
     int len;
{
  char *tem;

  if (ntypes >= typevec_size)
    {
      if (typevec_size == 0)
	{
	  typevec_size = 3;
	  typevec = (char **) xmalloc (sizeof (char*)*typevec_size);
	}
      else
	{
	  typevec_size *= 2;
	  typevec = (char **) xrealloc ((char *)typevec, sizeof (char*)*typevec_size);
	}
    }
  tem = (char *) xmalloc (len + 1);
  memcpy (tem, start, len);
  tem[len] = '\0';
  typevec[ntypes++] = tem;
}

/* `decl' must be already initialised, usually non-empty;
   it won't be freed on failure */

static int
do_args (type, decl, arg_mode)
     const char **type;
     string *decl;
     int arg_mode;
{
  string arg;
  int need_comma = 0;

  if (print_arg_types)
    string_append (decl, "(");

  while (**type != '_' && **type != '\0' && **type != 'e' && **type != 'v')
    {
      if (**type == 'N')
	{
	  int r;
	  int t;
	  *type += 1;
	  if (!get_count (type, &r) || !get_count (type, &t) || t >= ntypes)
	    return 0;
	  while (--r >= 0)
	    {
	      const char *tem = typevec[t];
	      if (need_comma && print_arg_types)
		string_append (decl, ", ");
	      if (!do_arg (&tem, &arg, arg_mode))
		return 0;
	      if (print_arg_types)
		string_appends (decl, &arg);
	      string_delete (&arg);
	      need_comma = 1;
	    }
	}
      else
	{
	  if (need_comma & print_arg_types)
	    string_append (decl, ", ");
	  if (!do_arg (type, &arg, arg_mode))
	    return 0;
	  if (print_arg_types)
	    string_appends (decl, &arg);
	  string_delete (&arg);
	  need_comma = 1;
	}
    }

  if (**type == 'v')
    *type += 1;
  else if (**type == 'e')
    {
      *type += 1;
      if (print_arg_types)
	{
	  if (need_comma)
	    string_append (decl, ",");
	  string_append (decl, "...");
	}
    }

  if (print_arg_types)
    string_append (decl, ")");
  return 1;
}

static void
munge_function_name (name, arg_mode)
     string *name;
     int arg_mode;
{
  if (string_empty (name))
    return;

  if (name->p - name->b >= 3 
      && name->b[0] == 'o' && name->b[1] == 'p' && name->b[2] == CPLUS_MARKER)
    {
      int i;
      /* see if it's an assignment expression */
      if (name->p - name->b >= 10 /* op$assign_ */
	  && memcmp (name->b + 3, "assign_", 7) == 0)
	{
	  for (i = 0; i < sizeof (optable)/sizeof (optable[0]); i++)
	    {
	      int len = name->p - name->b - 10;
	      if (strlen (optable[i].in) == len
		  && memcmp (optable[i].in, name->b + 10, len) == 0)
		{
		  string_clear (name);
		  string_append (name, "operator");
		  string_append (name, optable[i].out);
		  string_append (name, "=");
		  return;
		}
	    }
	}
      else
	{
	  for (i = 0; i < sizeof (optable)/sizeof (optable[0]); i++)
	    {
	      int len = name->p - name->b - 3;
	      if (strlen (optable[i].in) == len 
		  && memcmp (optable[i].in, name->b + 3, len) == 0)
		{
		  string_clear (name);
		  string_append (name, "operator");
		  string_append (name, optable[i].out);
		  return;
		}
	    }
	}
      return;
    }
  else if (name->p - name->b >= 5 && memcmp (name->b, "type$", 5) == 0)
    {
      /* type conversion operator */
      string type;
      const char *tem = name->b + 5;
      if (do_type (&tem, &type, arg_mode))
	{
	  string_clear (name);
	  string_append (name, "operator ");
	  string_appends (name, &type);
	  string_delete (&type);
	  return;
	}
    }
  /* ANSI.  */
  else if (name->b[2] == 'o' && name->b[3] == 'p')
    {
      /* type conversion operator.  */
      string type;
      const char *tem = name->b + 4;
      if (do_type (&tem, &type, arg_mode))
	{
	  string_clear (name);
	  string_append (name, "operator ");
	  string_appends (name, &type);
	  string_delete (&type);
	  return;
	}
    }
  else if (name->b[0] == '_' && name->b[1] == '_'
	   && name->b[2] >= 'a' && name->b[2] <= 'z'
	   && name->b[3] >= 'a' && name->b[3] <= 'z')
    {
      int i;

      if (name->b[4] == '\0')
	{
	  /* Operator.  */
	  for (i = 0; i < sizeof (optable)/sizeof (optable[0]); i++)
	    {
	      if (strlen (optable[i].in) == 2
		  && memcmp (optable[i].in, name->b + 2, 2) == 0)
		{
		  string_clear (name);
		  string_append (name, "operator");
		  string_append (name, optable[i].out);
		  return;
		}
	    }
	}
      else
	{
	  if (name->b[2] != 'a' || name->b[5] != '\0')
	    return;
	  /* Assignment.  */
	  for (i = 0; i < sizeof (optable)/sizeof (optable[0]); i++)
	    {
	      if (strlen (optable[i].in) == 3
		  && memcmp (optable[i].in, name->b + 2, 3) == 0)
		{
		  string_clear (name);
		  string_append (name, "operator");
		  string_append (name, optable[i].out);
		  return;
		}
	    }
	}
    }
}

/* a mini string-handling package */

static void
string_need (s, n)
     string *s;
     int n;
{
  if (s->b == NULL)
    {
      if (n < 32)
	n = 32;
      s->p = s->b = (char *) xmalloc (n);
      s->e = s->b + n;
    }
  else if (s->e - s->p < n)
    {
      int tem = s->p - s->b;
      n += tem;
      n *= 2;
      s->b = (char *) xrealloc (s->b, n);
      s->p = s->b + tem;
      s->e = s->b + n;
    }
}

static void
string_delete (s)
     string *s;
{
  if (s->b != NULL)
    {
      free (s->b);
      s->b = s->e = s->p = NULL;
    }
}

static void
string_init (s)
     string *s;
{
  s->b = s->p = s->e = NULL;
}

static void 
string_clear (s)
     string *s;
{
  s->p = s->b;
}

static int
string_empty (s)
     string *s;
{
  return s->b == s->p;
}

static void
string_append (p, s)
     string *p;
     const char *s;
{
  int n;
  if (s == NULL || *s == '\0')
    return;
  n = strlen (s);
  string_need (p, n);
  memcpy (p->p, s, n);
  p->p += n;
}

static void
string_appends (p, s)
     string *p, *s;
{
  int n;
  if (s->b == s->p)
    return;
  n = s->p - s->b;
  string_need (p, n);
  memcpy (p->p, s->b, n);
  p->p += n;
}

static void
string_appendn (p, s, n)
     string *p;
     const char *s;
     int n;
{
  if (n == 0)
    return;
  string_need (p, n);
  memcpy (p->p, s, n);
  p->p += n;
}

static void
string_prepend (p, s)
     string *p;
     const char *s;
{
  if (s == NULL || *s == '\0')
    return;
  string_prependn (p, s, strlen (s));
}

#if 0
static void
string_prepends (p, s)
     string *p, *s;
{
  if (s->b == s->p)
    return;
  string_prependn (p, s->b, s->p - s->b);
}
#endif

static void
string_prependn (p, s, n)
     string *p;
     const char *s;
     int n;
{
  char *q;

  if (n == 0)
    return;
  string_need (p, n);
  for (q = p->p - 1; q >= p->b; q--)
    q[n] = q[0];
  memcpy (p->b, s, n);
  p->p += n;
}
