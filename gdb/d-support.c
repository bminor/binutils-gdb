/* Helper routines for D support in GDB.

   Copyright (C) 2014 Free Software Foundation, Inc.

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
#include "d-lang.h"
#include "gdb_obstack.h"

#include "safe-ctype.h"

static const char *parse_function_args (struct obstack *, const char *);
static const char *parse_type (struct obstack *, const char *);


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

