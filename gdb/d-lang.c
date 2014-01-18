/* D language support routines for GDB, the GNU debugger.

   Copyright (C) 2005-2014 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "symtab.h"
#include "language.h"
#include "varobj.h"
#include "d-lang.h"
#include "c-lang.h"
#include "parser-defs.h"
#include "gdb_obstack.h"

#include "safe-ctype.h"

static const char *parse_function_args (struct obstack *, const char *);
static const char *parse_type (struct obstack *, const char *);

/* The name of the symbol to use to get the name of the main subprogram.  */
static const char D_MAIN[] = "D main";

/* Function returning the special symbol name used by D for the main
   procedure in the main program if it is found in minimal symbol list.
   This function tries to find minimal symbols so that it finds them even
   if the program was compiled without debugging information.  */

const char *
d_main_name (void)
{
  struct minimal_symbol *msym;

  msym = lookup_minimal_symbol (D_MAIN, NULL, NULL);
  if (msym != NULL)
    return D_MAIN;

  /* No known entry procedure found, the main program is probably not D.  */
  return NULL;
}

/* Demangle the calling convention from MANGLE and append it to TEMPBUF.
   Return the remaining string on success or NULL on failure.  */

static const char *
parse_call_convention (struct obstack *tempbuf, const char *mangle)
{
  if ((mangle == NULL) || (*mangle == '\0'))
    return mangle;

  switch (*mangle)
    {
    case 'F': /* (D) */
      mangle++;
      break;
    case 'U': /* (C) */
      mangle++;
      obstack_grow_str (tempbuf, "extern(C) ");
      break;
    case 'W': /* (Windows) */
      mangle++;
      obstack_grow_str (tempbuf, "extern(Windows) ");
      break;
    case 'V': /* (Pascal) */
      mangle++;
      obstack_grow_str (tempbuf, "extern(Pascal) ");
      break;
    case 'R': /* (C++) */
      mangle++;
      obstack_grow_str (tempbuf, "extern(C++) ");
      break;
    default:
      return NULL;
    }

  return mangle;
}

/* Demangle the D function attributes from MANGLE and append it to TEMPBUF.
   Return the remaining string on success or NULL on failure.  */

static const char *
parse_attributes (struct obstack *tempbuf, const char *mangle)
{
  if ((mangle == NULL) || (*mangle == '\0'))
    return mangle;

  while (*mangle == 'N')
    {
      mangle++;
      switch (*mangle)
	{
	case 'a': /* pure */
	  mangle++;
	  obstack_grow_str (tempbuf, "pure ");
	  continue;
	case 'b': /* nothrow */
	  mangle++;
	  obstack_grow_str (tempbuf, "nothrow ");
	  continue;
	case 'c': /* ref */
	  mangle++;
	  obstack_grow_str (tempbuf, "ref ");
	  continue;
	case 'd': /* @property */
	  mangle++;
	  obstack_grow_str (tempbuf, "@property ");
	  continue;
	case 'e': /* @trusted */
	  mangle++;
	  obstack_grow_str (tempbuf, "@trusted ");
	  continue;
	case 'f': /* @safe */
	  mangle++;
	  obstack_grow_str (tempbuf, "@safe ");
	  continue;
	case 'g':
	case 'h':
	  /* inout parameter is represented as 'Ng'.
	     vector parameter is represented as 'Nh'.
	     If we see this, then we know we're really in the
	     parameter list.  Rewind and break.  */
	  mangle--;
	}
      break;
    }
  return mangle;
}

/* Demangle the function type from MANGLE and append it to TEMPBUF.
   Return the remaining string on success or NULL on failure.  */

static const char *
parse_function_type (struct obstack *tempbuf, const char *mangle)
{
  struct obstack obattr, obargs, obtype;
  char *attr, *args, *type;
  size_t szattr, szargs, sztype;

  if ((mangle == NULL) || (*mangle == '\0'))
    return mangle;

  /* The order of the mangled string is:
     TypeFunction ::
        CallConvention FuncAttrs Arguments ArgClose Type

     The demangled string is re-ordered as:
        CallConvention Type Arguments FuncAttrs
   */
  obstack_init (&obattr);
  obstack_init (&obargs);
  obstack_init (&obtype);

  /* Function call convention.  */
  mangle = parse_call_convention (tempbuf, mangle);

  /* Function attributes.  */
  mangle = parse_attributes (&obattr, mangle);
  szattr = obstack_object_size (&obattr);
  attr = obstack_finish (&obattr);

  /* Function arguments.  */
  mangle = parse_function_args (&obargs, mangle);
  szargs = obstack_object_size (&obargs);
  args = obstack_finish (&obargs);

  /* Function return type.  */
  mangle = parse_type (&obtype, mangle);
  sztype = obstack_object_size (&obtype);
  type = obstack_finish (&obtype);

  /* Append to buffer in order. */
  obstack_grow (tempbuf, type, sztype);
  obstack_grow_str (tempbuf, "(");
  obstack_grow (tempbuf, args, szargs);
  obstack_grow_str (tempbuf, ") ");
  obstack_grow (tempbuf, attr, szattr);

  obstack_free (&obattr, NULL);
  obstack_free (&obargs, NULL);
  obstack_free (&obtype, NULL);
  return mangle;
}

/* Demangle the argument list from MANGLE and append it to TEMPBUF.
   Return the remaining string on success or NULL on failure.  */

static const char *
parse_function_args (struct obstack *tempbuf, const char *mangle)
{
  size_t n = 0;

  while ((mangle != NULL) && (*mangle != '\0'))
    {
      switch (*mangle)
	{
	case 'X': /* (variadic T t...) style.  */
	  mangle++;
	  obstack_grow_str (tempbuf, "...");
	  return mangle;
	case 'Y': /* (variadic T t, ...) style.  */
	  mangle++;
	  obstack_grow_str (tempbuf, ", ...");
	  return mangle;
	case 'Z': /* Normal function.  */
	  mangle++;
	  return mangle;
	}

      if (n++)
	obstack_grow_str (tempbuf, ", ");

      if (*mangle == 'M') /* scope(T) */
	{
	  mangle++;
	  obstack_grow_str (tempbuf, "scope ");
	}

      switch (*mangle)
	{
	case 'J': /* out(T) */
	  mangle++;
	  obstack_grow_str (tempbuf, "out ");
	  break;
	case 'K': /* ref(T) */
	  mangle++;
	  obstack_grow_str (tempbuf, "ref ");
	  break;
	case 'L': /* lazy(T) */
	  mangle++;
	  obstack_grow_str (tempbuf, "lazy ");
	  break;
	}
      mangle = parse_type (tempbuf, mangle);
    }
  return mangle;
}

/* Demangle the type from MANGLE and append it to TEMPBUF.
   Return the remaining string on success or NULL on failure.  */

static const char *
parse_type (struct obstack *tempbuf, const char *mangle)
{
  if ((mangle == NULL) || (*mangle == '\0'))
    return mangle;

  switch (*mangle)
    {
    case 'O': /* shared(T) */
      mangle++;
      obstack_grow_str (tempbuf, "shared(");
      mangle = parse_type (tempbuf, mangle);
      obstack_grow_str (tempbuf, ")");
      return mangle;
    case 'x': /* const(T) */
      mangle++;
      obstack_grow_str (tempbuf, "const(");
      mangle = parse_type (tempbuf, mangle);
      obstack_grow_str (tempbuf, ")");
      return mangle;
    case 'y': /* immutable(T) */
      mangle++;
      obstack_grow_str (tempbuf, "immutable(");
      mangle = parse_type (tempbuf, mangle);
      obstack_grow_str (tempbuf, ")");
      return mangle;
    case 'N':
      mangle++;
      if (*mangle == 'g') /* wild(T) */
	{
	  mangle++;
	  obstack_grow_str (tempbuf, "inout(");
	  mangle = parse_type (tempbuf, mangle);
	  obstack_grow_str (tempbuf, ")");
	  return mangle;
	}
      else if (*mangle == 'h') /* vector(T) */
	{
	  mangle++;
	  /* The basetype for vectors are always static arrays.  */
	  if (*mangle != 'G')
	    return NULL;
	  obstack_grow_str (tempbuf, "__vector(");
	  mangle = parse_type (tempbuf, mangle);
	  obstack_grow_str (tempbuf, ")");
	  return mangle;
	}
      else
	return NULL;
    case 'A': /* dynamic array (T[]) */
      mangle++;
      mangle = parse_type (tempbuf, mangle);
      obstack_grow_str (tempbuf, "[]");
      return mangle;
    case 'G': /* static array (T[N]) */
    {
      const char *numptr;
      size_t num = 0;
      mangle++;

      numptr = mangle;
      while (ISDIGIT (*mangle))
	{
	  num++;
	  mangle++;
	}
      mangle = parse_type (tempbuf, mangle);
      obstack_grow_str (tempbuf, "[");
      obstack_grow (tempbuf, numptr, num);
      obstack_grow_str (tempbuf, "]");
      return mangle;
    }
    case 'H': /* associative array (T[T]) */
    {
      struct obstack obtype;
      char *type;
      size_t sztype;
      mangle++;

      obstack_init (&obtype);
      mangle = parse_type (&obtype, mangle);
      sztype = obstack_object_size (&obtype);
      type = obstack_finish (&obtype);

      mangle = parse_type (tempbuf, mangle);
      obstack_grow_str (tempbuf, "[");
      obstack_grow (tempbuf, type, sztype);
      obstack_grow_str (tempbuf, "]");

      obstack_free (&obtype, NULL);
      return mangle;
    }
    case 'P': /* pointer (T*) */
      mangle++;
      mangle = parse_type (tempbuf, mangle);
      obstack_grow_str (tempbuf, "*");
      return mangle;
    case 'I': /* ident T */
    case 'C': /* class T */
    case 'S': /* struct T */
    case 'E': /* enum T */
    case 'T': /* typedef T */
      mangle++;
      return d_parse_symbol (tempbuf, mangle);
    case 'D': /* delegate T */
      mangle++;
      mangle = parse_function_type (tempbuf, mangle);
      obstack_grow_str (tempbuf, "delegate");
      return mangle;
    case 'B': /* tuple T */
      /* TODO: Handle this.  */
      return NULL;

    /* Function types */
    case 'F': case 'U': case 'W':
    case 'V': case 'R':
      mangle = parse_function_type (tempbuf, mangle);
      obstack_grow_str (tempbuf, "function");
      return mangle;

    /* Basic types */
    case 'n':
      mangle++;
      obstack_grow_str (tempbuf, "none");
      return mangle;
    case 'v':
      mangle++;
      obstack_grow_str (tempbuf, "void");
      return mangle;
    case 'g':
      mangle++;
      obstack_grow_str (tempbuf, "byte");
      return mangle;
    case 'h':
      mangle++;
      obstack_grow_str (tempbuf, "ubyte");
      return mangle;
    case 's':
      mangle++;
      obstack_grow_str (tempbuf, "short");
      return mangle;
    case 't':
      mangle++;
      obstack_grow_str (tempbuf, "ushort");
      return mangle;
    case 'i':
      mangle++;
      obstack_grow_str (tempbuf, "int");
      return mangle;
    case 'k':
      mangle++;
      obstack_grow_str (tempbuf, "uint");
      return mangle;
    case 'l':
      mangle++;
      obstack_grow_str (tempbuf, "long");
      return mangle;
    case 'm':
      mangle++;
      obstack_grow_str (tempbuf, "ulong");
      return mangle;
    case 'f':
      mangle++;
      obstack_grow_str (tempbuf, "float");
      return mangle;
    case 'd':
      mangle++;
      obstack_grow_str (tempbuf, "double");
      return mangle;
    case 'e':
      mangle++;
      obstack_grow_str (tempbuf, "real");
      return mangle;

    /* Imaginary and Complex types */
    case 'o':
      mangle++;
      obstack_grow_str (tempbuf, "ifloat");
      return mangle;
    case 'p':
      mangle++;
      obstack_grow_str (tempbuf, "idouble");
      return mangle;
    case 'j':
      mangle++;
      obstack_grow_str (tempbuf, "ireal");
      return mangle;
    case 'q':
      mangle++;
      obstack_grow_str (tempbuf, "cfloat");
      return mangle;
    case 'r':
      mangle++;
      obstack_grow_str (tempbuf, "cdouble");
      return mangle;
    case 'c':
      mangle++;
      obstack_grow_str (tempbuf, "creal");
      return mangle;

    /* Other types */
    case 'b':
      mangle++;
      obstack_grow_str (tempbuf, "bool");
      return mangle;
    case 'a':
      mangle++;
      obstack_grow_str (tempbuf, "char");
      return mangle;
    case 'u':
      mangle++;
      obstack_grow_str (tempbuf, "wchar");
      return mangle;
    case 'w':
      mangle++;
      obstack_grow_str (tempbuf, "dchar");
      return mangle;

    default: /* unhandled */
      return NULL;
    }
}

/* Extract the identifier from MANGLE and append it to TEMPBUF.
   Return the remaining string on success or NULL on failure.  */

static const char *
parse_identifier (struct obstack *tempbuf, const char *mangle)
{
  if ((mangle == NULL) || (*mangle == '\0'))
    return mangle;

  if (ISDIGIT (*mangle))
    {
      char *endptr;
      long i = strtol (mangle, &endptr, 10);

      if (i <= 0 || strlen (endptr) < i)
	return NULL;

      mangle = endptr;

      /* No support for demangling templates.  */
      if (i >= 5 && strncmp (mangle, "__T", 3) == 0)
	return NULL;

      if (strncmp (mangle, "__ctor", i) == 0)
	{
	  /* Constructor symbol for a class/struct.  */
	  obstack_grow_str (tempbuf, "this");
	  mangle += i;
	  return mangle;
	}
      else if (strncmp (mangle, "__dtor", i) == 0)
	{
	  /* Destructor symbol for a class/struct.  */
	  obstack_grow_str (tempbuf, "~this");
	  mangle += i;
	  return mangle;
	}
      else if (strncmp (mangle, "__postblit", i) == 0)
	{
	  /* Postblit symbol for a struct.  */
	  obstack_grow_str (tempbuf, "this(this)");
	  mangle += i;
	  return mangle;
	}
      else if (strncmp (mangle, "__initZ", i+1) == 0)
	{
	  /* The static initialiser for a given symbol.  */
	  obstack_grow_str (tempbuf, "init$");
	  mangle += i + 1;
	  return mangle;
	}
      else if (strncmp (mangle, "__ClassZ", i+1) == 0)
	{
	  /* The classinfo symbol for a given class.  */
	  obstack_grow_str (tempbuf, "classinfo$");
	  mangle += i + 1;
	  return mangle;
	}
      else if (strncmp (mangle, "__vtblZ", i+1) == 0)
	{
	  /* The vtable symbol for a given class.  */
	  obstack_grow_str (tempbuf, "vtbl$");
	  mangle += i + 1;
	  return mangle;
	}
      else if (strncmp (mangle, "__InterfaceZ", i+1) == 0)
	{
	  /* The interface symbol for a given class.  */
	  obstack_grow_str (tempbuf, "interface$");
	  mangle += i + 1;
	  return mangle;
	}
      else if (strncmp (mangle, "__ModuleInfoZ", i+1) == 0)
	{
	  /* The ModuleInfo symbol for a given module.  */
	  obstack_grow_str (tempbuf, "moduleinfo$");
	  mangle += i + 1;
	  return mangle;
	}
      obstack_grow (tempbuf, mangle, i);
      mangle += i;
    }
  else
    return NULL;

  return mangle;
}

/* Test whether the current position of MANGLE is pointing to the
   beginning of a mangled function parameter list, which starts with
   the calling convention.  Returns 1 on success or 0 on failure.  */

static int
call_convention_p (const char *mangle)
{
  size_t i;

  if ((mangle == NULL) || (*mangle == '\0'))
    return 0;

  switch (*mangle)
    {
    case 'F': case 'U': case 'V':
    case 'W': case 'R':
      return 1;

    case 'M': /* Prefix for functions needing 'this'.  */
      i = 1;
      if (mangle[i] == 'x')
	i++;

      switch (mangle[i])
	{
	case 'F': case 'U': case 'V':
	case 'W': case 'R':
	  return 1;
	}

    default:
      return 0;
    }
}

/* Extract and demangle the symbol in MANGLE and append it to TEMPBUF.
   Return the remaining signature on success or NULL on failure.  */

const char *
d_parse_symbol (struct obstack *tempbuf, const char *mangle)
{
  size_t n = 0;
  do
    {
      if (n++)
	obstack_grow_str (tempbuf, ".");

      mangle = parse_identifier (tempbuf, mangle);

      if (call_convention_p (mangle))
	{
	  char *saved;

	  /* Skip over 'this' parameter.  */
	  if (*mangle == 'M')
	    mangle += (mangle[1] == 'x') ? 2 : 1;

	  /* Skip over calling convention and attributes in qualified name.  */
	  saved = obstack_next_free (tempbuf);
	  mangle = parse_call_convention (tempbuf, mangle);
	  mangle = parse_attributes (tempbuf, mangle);
	  obstack_next_free (tempbuf) = saved;

	  obstack_grow_str (tempbuf, "(");
	  mangle = parse_function_args (tempbuf, mangle);
	  obstack_grow_str (tempbuf, ")");

	  /* Demangle the function return type as a kind of sanity test.  */
	  if ((mangle != NULL) && !ISDIGIT (*mangle))
	    {
	      saved = obstack_next_free (tempbuf);
	      mangle = parse_type (tempbuf, mangle);
	      obstack_next_free (tempbuf) = saved;
	    }
	}
    }
  while ((mangle != NULL) && ISDIGIT (*mangle));

  return mangle;
}

/* Implements the la_demangle language_defn routine for language D.  */

char *
d_demangle (const char *symbol, int options)
{
  struct obstack tempbuf;
  char *result;

  if ((symbol == NULL) || (*symbol == '\0'))
    return NULL;
  else if (strcmp (symbol, "_Dmain") == 0)
    return xstrdup ("D main");

  obstack_init (&tempbuf);

  if (strncmp (symbol, "_D", 2) == 0)
    symbol += 2;
  else
    {
      obstack_free (&tempbuf, NULL);
      return NULL;
    }

  if (d_parse_symbol (&tempbuf, symbol) != NULL)
    {
      obstack_grow_str0 (&tempbuf, "");
      result = xstrdup (obstack_finish (&tempbuf));
      obstack_free (&tempbuf, NULL);
    }
  else
    {
      obstack_free (&tempbuf, NULL);
      return NULL;
    }

  return result;
}

/* Table mapping opcodes into strings for printing operators
   and precedences of the operators.  */
static const struct op_print d_op_print_tab[] =
{
  {",", BINOP_COMMA, PREC_COMMA, 0},
  {"=", BINOP_ASSIGN, PREC_ASSIGN, 1},
  {"||", BINOP_LOGICAL_OR, PREC_LOGICAL_OR, 0},
  {"&&", BINOP_LOGICAL_AND, PREC_LOGICAL_AND, 0},
  {"|", BINOP_BITWISE_IOR, PREC_BITWISE_IOR, 0},
  {"^", BINOP_BITWISE_XOR, PREC_BITWISE_XOR, 0},
  {"&", BINOP_BITWISE_AND, PREC_BITWISE_AND, 0},
  {"==", BINOP_EQUAL, PREC_EQUAL, 0},
  {"!=", BINOP_NOTEQUAL, PREC_EQUAL, 0},
  {"<=", BINOP_LEQ, PREC_ORDER, 0},
  {">=", BINOP_GEQ, PREC_ORDER, 0},
  {">", BINOP_GTR, PREC_ORDER, 0},
  {"<", BINOP_LESS, PREC_ORDER, 0},
  {">>", BINOP_RSH, PREC_SHIFT, 0},
  {"<<", BINOP_LSH, PREC_SHIFT, 0},
  {"+", BINOP_ADD, PREC_ADD, 0},
  {"-", BINOP_SUB, PREC_ADD, 0},
  {"*", BINOP_MUL, PREC_MUL, 0},
  {"/", BINOP_DIV, PREC_MUL, 0},
  {"%", BINOP_REM, PREC_MUL, 0},
  {"@", BINOP_REPEAT, PREC_REPEAT, 0},
  {"-", UNOP_NEG, PREC_PREFIX, 0},
  {"!", UNOP_LOGICAL_NOT, PREC_PREFIX, 0},
  {"~", UNOP_COMPLEMENT, PREC_PREFIX, 0},
  {"*", UNOP_IND, PREC_PREFIX, 0},
  {"&", UNOP_ADDR, PREC_PREFIX, 0},
  {"sizeof ", UNOP_SIZEOF, PREC_PREFIX, 0},
  {"++", UNOP_PREINCREMENT, PREC_PREFIX, 0},
  {"--", UNOP_PREDECREMENT, PREC_PREFIX, 0},
  {NULL, 0, 0, 0}
};

/* Mapping of all D basic data types into the language vector.  */

enum d_primitive_types {
  d_primitive_type_void,
  d_primitive_type_bool,
  d_primitive_type_byte,
  d_primitive_type_ubyte,
  d_primitive_type_short,
  d_primitive_type_ushort,
  d_primitive_type_int,
  d_primitive_type_uint,
  d_primitive_type_long,
  d_primitive_type_ulong,
  d_primitive_type_cent,    /* Signed 128 bit integer.  */
  d_primitive_type_ucent,   /* Unsigned 128 bit integer.  */
  d_primitive_type_float,
  d_primitive_type_double,
  d_primitive_type_real,
  d_primitive_type_ifloat,  /* Imaginary float types.  */
  d_primitive_type_idouble,
  d_primitive_type_ireal,
  d_primitive_type_cfloat,  /* Complex number of two float values.  */
  d_primitive_type_cdouble,
  d_primitive_type_creal,
  d_primitive_type_char,    /* Unsigned character types.  */
  d_primitive_type_wchar,
  d_primitive_type_dchar,
  nr_d_primitive_types
};

/* Implements the la_language_arch_info language_defn routine
   for language D.  */

static void
d_language_arch_info (struct gdbarch *gdbarch,
		      struct language_arch_info *lai)
{
  const struct builtin_d_type *builtin = builtin_d_type (gdbarch);

  lai->string_char_type = builtin->builtin_char;
  lai->primitive_type_vector
    = GDBARCH_OBSTACK_CALLOC (gdbarch, nr_d_primitive_types + 1,
			      struct type *);

  lai->primitive_type_vector [d_primitive_type_void]
    = builtin->builtin_void;
  lai->primitive_type_vector [d_primitive_type_bool]
    = builtin->builtin_bool;
  lai->primitive_type_vector [d_primitive_type_byte]
    = builtin->builtin_byte;
  lai->primitive_type_vector [d_primitive_type_ubyte]
    = builtin->builtin_ubyte;
  lai->primitive_type_vector [d_primitive_type_short]
    = builtin->builtin_short;
  lai->primitive_type_vector [d_primitive_type_ushort]
    = builtin->builtin_ushort;
  lai->primitive_type_vector [d_primitive_type_int]
    = builtin->builtin_int;
  lai->primitive_type_vector [d_primitive_type_uint]
    = builtin->builtin_uint;
  lai->primitive_type_vector [d_primitive_type_long]
    = builtin->builtin_long;
  lai->primitive_type_vector [d_primitive_type_ulong]
    = builtin->builtin_ulong;
  lai->primitive_type_vector [d_primitive_type_cent]
    = builtin->builtin_cent;
  lai->primitive_type_vector [d_primitive_type_ucent]
    = builtin->builtin_ucent;
  lai->primitive_type_vector [d_primitive_type_float]
    = builtin->builtin_float;
  lai->primitive_type_vector [d_primitive_type_double]
    = builtin->builtin_double;
  lai->primitive_type_vector [d_primitive_type_real]
    = builtin->builtin_real;
  lai->primitive_type_vector [d_primitive_type_ifloat]
    = builtin->builtin_ifloat;
  lai->primitive_type_vector [d_primitive_type_idouble]
    = builtin->builtin_idouble;
  lai->primitive_type_vector [d_primitive_type_ireal]
    = builtin->builtin_ireal;
  lai->primitive_type_vector [d_primitive_type_cfloat]
    = builtin->builtin_cfloat;
  lai->primitive_type_vector [d_primitive_type_cdouble]
    = builtin->builtin_cdouble;
  lai->primitive_type_vector [d_primitive_type_creal]
    = builtin->builtin_creal;
  lai->primitive_type_vector [d_primitive_type_char]
    = builtin->builtin_char;
  lai->primitive_type_vector [d_primitive_type_wchar]
    = builtin->builtin_wchar;
  lai->primitive_type_vector [d_primitive_type_dchar]
    = builtin->builtin_dchar;

  lai->bool_type_symbol = "bool";
  lai->bool_type_default = builtin->builtin_bool;
}

static const struct language_defn d_language_defn =
{
  "d",
  "D",
  language_d,
  range_check_off,
  case_sensitive_on,
  array_row_major,
  macro_expansion_no,
  &exp_descriptor_c,
  c_parse,
  c_error,
  null_post_parser,
  c_printchar,			/* Print a character constant.  */
  c_printstr,			/* Function to print string constant.  */
  c_emit_char,			/* Print a single char.  */
  c_print_type,			/* Print a type using appropriate syntax.  */
  c_print_typedef,		/* Print a typedef using appropriate
				   syntax.  */
  d_val_print,			/* Print a value using appropriate syntax.  */
  c_value_print,		/* Print a top-level value.  */
  default_read_var_value,	/* la_read_var_value */
  NULL,				/* Language specific skip_trampoline.  */
  "this",
  basic_lookup_symbol_nonlocal, 
  basic_lookup_transparent_type,
  d_demangle,			/* Language specific symbol demangler.  */
  NULL,				/* Language specific
				   class_name_from_physname.  */
  d_op_print_tab,		/* Expression operators for printing.  */
  1,				/* C-style arrays.  */
  0,				/* String lower bound.  */
  default_word_break_characters,
  default_make_symbol_completion_list,
  d_language_arch_info,
  default_print_array_index,
  default_pass_by_reference,
  c_get_string,
  NULL,				/* la_get_symbol_name_cmp */
  iterate_over_symbols,
  &default_varobj_ops,
  LANG_MAGIC
};

/* Build all D language types for the specified architecture.  */

static void *
build_d_types (struct gdbarch *gdbarch)
{
  struct builtin_d_type *builtin_d_type
    = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct builtin_d_type);

  /* Basic types.  */
  builtin_d_type->builtin_void
    = arch_type (gdbarch, TYPE_CODE_VOID, 1, "void");
  builtin_d_type->builtin_bool
    = arch_boolean_type (gdbarch, 8, 1, "bool");
  builtin_d_type->builtin_byte
    = arch_integer_type (gdbarch, 8, 0, "byte");
  builtin_d_type->builtin_ubyte
    = arch_integer_type (gdbarch, 8, 1, "ubyte");
  builtin_d_type->builtin_short
    = arch_integer_type (gdbarch, 16, 0, "short");
  builtin_d_type->builtin_ushort
    = arch_integer_type (gdbarch, 16, 1, "ushort");
  builtin_d_type->builtin_int
    = arch_integer_type (gdbarch, 32, 0, "int");
  builtin_d_type->builtin_uint
    = arch_integer_type (gdbarch, 32, 1, "uint");
  builtin_d_type->builtin_long
    = arch_integer_type (gdbarch, 64, 0, "long");
  builtin_d_type->builtin_ulong
    = arch_integer_type (gdbarch, 64, 1, "ulong");
  builtin_d_type->builtin_cent
    = arch_integer_type (gdbarch, 128, 0, "cent");
  builtin_d_type->builtin_ucent
    = arch_integer_type (gdbarch, 128, 1, "ucent");
  builtin_d_type->builtin_float
    = arch_float_type (gdbarch, gdbarch_float_bit (gdbarch),
		       "float", NULL);
  builtin_d_type->builtin_double
    = arch_float_type (gdbarch, gdbarch_double_bit (gdbarch),
		       "double", NULL);
  builtin_d_type->builtin_real
    = arch_float_type (gdbarch, gdbarch_long_double_bit (gdbarch),
		       "real", NULL);

  TYPE_INSTANCE_FLAGS (builtin_d_type->builtin_byte)
    |= TYPE_INSTANCE_FLAG_NOTTEXT;
  TYPE_INSTANCE_FLAGS (builtin_d_type->builtin_ubyte)
    |= TYPE_INSTANCE_FLAG_NOTTEXT;

  /* Imaginary and complex types.  */
  builtin_d_type->builtin_ifloat
    = arch_float_type (gdbarch, gdbarch_float_bit (gdbarch),
		       "ifloat", NULL);
  builtin_d_type->builtin_idouble
    = arch_float_type (gdbarch, gdbarch_double_bit (gdbarch),
		       "idouble", NULL);
  builtin_d_type->builtin_ireal
    = arch_float_type (gdbarch, gdbarch_long_double_bit (gdbarch),
		       "ireal", NULL);
  builtin_d_type->builtin_cfloat
    = arch_complex_type (gdbarch, "cfloat",
			 builtin_d_type->builtin_float);
  builtin_d_type->builtin_cdouble
    = arch_complex_type (gdbarch, "cdouble",
			 builtin_d_type->builtin_double);
  builtin_d_type->builtin_creal
    = arch_complex_type (gdbarch, "creal",
			 builtin_d_type->builtin_real);

  /* Character types.  */
  builtin_d_type->builtin_char
    = arch_character_type (gdbarch, 8, 1, "char");
  builtin_d_type->builtin_wchar
    = arch_character_type (gdbarch, 16, 1, "wchar");
  builtin_d_type->builtin_dchar
    = arch_character_type (gdbarch, 32, 1, "dchar");

  return builtin_d_type;
}

static struct gdbarch_data *d_type_data;

/* Return the D type table for the specified architecture.  */

const struct builtin_d_type *
builtin_d_type (struct gdbarch *gdbarch)
{
  return gdbarch_data (gdbarch, d_type_data);
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern initialize_file_ftype _initialize_d_language;

void
_initialize_d_language (void)
{
  d_type_data = gdbarch_data_register_post_init (build_d_types);

  add_language (&d_language_defn);
}
