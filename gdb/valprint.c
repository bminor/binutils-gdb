/* Print values for GNU debugger gdb.
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

#include <stdio.h>
#include "defs.h"
#include "initialize.h"
#include "symtab.h"
#include "value.h"

/* Maximum number of chars to print for a string pointer value
   or vector contents.  */

static int print_max;

static void type_print_varspec_suffix ();
static void type_print_varspec_prefix ();
static void type_print_base ();
static void type_print_method_args ();

START_FILE

char **unsigned_type_table;
char **signed_type_table;
char **float_type_table;

/* Print the value VAL in C-ish syntax on stream STREAM.
   If the object printed is a string pointer, returns
   the number of string bytes printed.  */

value_print (val, stream)
     value val;
     FILE *stream;
{
  register int i, n, typelen;

  /* A "repeated" value really contains several values in a row.
     They are made by the @ operator.
     Print such values as if they were arrays.  */

  if (VALUE_REPEATED (val))
    {
      n = VALUE_REPETITIONS (val);
      typelen = TYPE_LENGTH (VALUE_TYPE (val));
      fputc ('{', stream);
      /* Print arrays of characters using string syntax.  */
      if (VALUE_TYPE (val) == builtin_type_char
	  || VALUE_TYPE (val) == builtin_type_unsigned_char)
	{
	  fputc ('"', stream);
	  for (i = 0; i < n && i < print_max; i++)
	    {
	      QUIT;
	      printchar (VALUE_CONTENTS (val)[i], stream);
	    }
	  if (i < n)
	    fprintf (stream, "...");
	  fputc ('"', stream);
	}
      else
	{
	  for (i = 0; i < n && i < print_max; i++)
	    {
	      if (i)
		fprintf (stream, ", ");
	      val_print (VALUE_TYPE (val), VALUE_CONTENTS (val) + typelen * i,
			 VALUE_ADDRESS (val) + typelen * i, stream);
	    }
	  if (i < n)
	    fprintf (stream, "...");
	}
      fputc ('}', stream);
    }
  else
    {
      /* If it is a pointer, indicate what it points to.

         C++: if it is a member pointer, we will take care
	 of that when we print it.  */
      if (TYPE_CODE (VALUE_TYPE (val)) == TYPE_CODE_PTR)
	{
	  fprintf (stream, "(");
	  type_print (VALUE_TYPE (val), "", stream, -1);
	  fprintf (stream, ") ");
	}
      return val_print (VALUE_TYPE (val), VALUE_CONTENTS (val),
			VALUE_ADDRESS (val), stream);
    }
}

/* Print on STREAM data stored in debugger at address VALADDR
   according to the format of type TYPE.
   ADDRESS is the location in the inferior that the data
   is supposed to have come from.

   If the data are a string pointer, returns the number of
   sting characters printed.  */

int
val_print (type, valaddr, address, stream)
     struct type *type;
     char *valaddr;
     CORE_ADDR address;
     FILE *stream;
{
  register int i;
  int len;
  struct type *elttype;
  int eltlen;
  int val;
  unsigned char c;

  QUIT;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_ARRAY:
      if (TYPE_LENGTH (type) >= 0)
	{
	  elttype = TYPE_TARGET_TYPE (type);
	  eltlen = TYPE_LENGTH (elttype);
	  len = TYPE_LENGTH (type) / eltlen;
	  fprintf (stream, "{");
	  /* For an array of chars, print with string syntax.  */
	  if (elttype == builtin_type_char
	      || elttype == builtin_type_unsigned_char)
	    {
	      fputc ('"', stream);
	      for (i = 0; i < len && i < print_max; i++)
		{
		  QUIT;
		  printchar (valaddr[i], stream);
		}
	      if (i < len)
		fprintf (stream, "...");
	      fputc ('"', stream);
	    }
	  else
	    {
	      for (i = 0; i < len && i < print_max; i++)
		{
		  if (i) fprintf (stream, ", ");
		  val_print (elttype, valaddr + i * eltlen,
			     0, stream);
		}
	      if (i < len)
		fprintf (stream, "...");
	    }
	  fprintf (stream, "}");
	  break;
	}
      /* Array of unspecified length: treat like pointer.  */

    case TYPE_CODE_PTR:
      fprintf (stream, "0x%x", * (int *) valaddr);
      /* For a pointer to char or unsigned char,
	 also print the string pointed to, unless pointer is null.  */
      if ((TYPE_TARGET_TYPE (type) == builtin_type_char
	   || TYPE_TARGET_TYPE (type) == builtin_type_unsigned_char)
	  && unpack_long (type, valaddr) != 0)
	{
	  fputc (' ', stream);
	  fputc ('"', stream);
	  for (i = 0; i < print_max; i++)
	    {
	      QUIT;
	      read_memory (unpack_long (type, valaddr) + i, &c, 1);
	      if (c == 0)
		break;
	      printchar (c, stream);
	    }
	  fputc ('"', stream);
	  if (i == print_max)
	    fprintf (stream, "...");
	  fflush (stream);
	  /* Return number of characters printed, plus one for the
	     terminating null if we have "reached the end".  */
	  return i + (i != print_max);
	}
      break;

    case TYPE_CODE_MPTR:
      {
	struct type *domain = TYPE_DOMAIN_TYPE (type);
	struct type *target = TYPE_TARGET_TYPE (type);
	struct fn_field *f;
	int j, len2;
	char *kind = "";

	val = unpack_long (builtin_type_int, valaddr);
	if (TYPE_CODE (target) == TYPE_CODE_FUNC)
	  {
	    if (val < 128)
	      {
		len = TYPE_NFN_FIELDS (domain);
		for (i = 0; i < len; i++)
		  {
		    f = TYPE_FN_FIELDLIST1 (domain, i);
		    len2 = TYPE_FN_FIELDLIST_LENGTH (domain, i);

		    for (j = 0; j < len2; j++)
		      {
			QUIT;
			if (TYPE_FN_FIELD_VOFFSET (f, j) == val)
			  {
			    kind = " virtual";
			    goto common;
			  }
		      }
		  }
	      }
	    else
	      {
		struct symbol *sym = find_pc_function (val);
		if (sym == 0)
		  error ("invalid pointer to member function");
		len = TYPE_NFN_FIELDS (domain);
		for (i = 0; i < len; i++)
		  {
		    f = TYPE_FN_FIELDLIST1 (domain, i);
		    len2 = TYPE_FN_FIELDLIST_LENGTH (domain, i);

		    for (j = 0; j < len2; j++)
		      {
			QUIT;
			if (!strcmp (SYMBOL_NAME (sym), TYPE_FN_FIELD_PHYSNAME (f, j)))
			  goto common;
		      }
		  }
	      }
	  common:
	    if (i < len)
	      {
		fprintf (stream, "& ");
		type_print_base (domain, stream, 0, 0);
		fprintf (stream, "::%s", kind);
		type_print_varspec_prefix (TYPE_FN_FIELD_TYPE (f, j), stream, 0, 0);
		if (TYPE_FN_FIELD_PHYSNAME (f, j)[0] == '_'
		    && TYPE_FN_FIELD_PHYSNAME (f, j)[1] == '$')
		  type_print_method_args
		    (TYPE_FN_FIELD_ARGS (f, j) + 1, "~",
		     TYPE_FN_FIELDLIST_NAME (domain, i), stream);
		else
		  type_print_method_args
		    (TYPE_FN_FIELD_ARGS (f, j), "",
		     TYPE_FN_FIELDLIST_NAME (domain, i), stream);
		break;
	      }
	  }
	else
	  {
	    /* VAL is a byte offset into the structure type DOMAIN.
	       Find the name of the field for that offset and
	       print it.  */
	    int extra = 0;
	    int bits = 0;
	    len = TYPE_NFIELDS (domain);
	    val <<= 3;		/* @@ Make VAL into bit offset */
	    for (i = 0; i < len; i++)
	      {
		int bitpos = TYPE_FIELD_BITPOS (domain, i);
		QUIT;
		if (val == bitpos)
		  break;
		if (val < bitpos && i > 0)
		  {
		    int ptrsize = (TYPE_LENGTH (builtin_type_char) * TYPE_LENGTH (target));
		    /* Somehow pointing into a field.  */
		    i -= 1;
		    extra = (val - TYPE_FIELD_BITPOS (domain, i));
		    if (extra & 0x3)
		      bits = 1;
		    else
		      extra >>= 3;
		    break;
		  }
	      }
	    if (i < len)
	      {
		fprintf (stream, "& ");
		type_print_base (domain, stream, 0, 0);
		fprintf (stream, "::");
		fprintf (stream, "%s", TYPE_FIELD_NAME (domain, i));
		if (extra)
		  fprintf (stream, " + %d bytes", extra);
		if (bits)
		  fprintf (stream, " (offset in bits)");
		break;
	      }
	  }
	fputc ('(', stream);
	type_print (type, "", stream, -1);
	fprintf (stream, ") %d", val >> 3);
	break;
      }

    case TYPE_CODE_REF:
      fprintf (stream, "(0x%x &) = ", * (int *) valaddr);
      /* De-reference the reference.  */
      if (TYPE_CODE (TYPE_TARGET_TYPE (type)) != TYPE_CODE_UNDEF)
	{
	  value val = value_at (TYPE_TARGET_TYPE (type), * (int *)valaddr);
	  val_print (VALUE_TYPE (val), VALUE_CONTENTS (val),
		     VALUE_ADDRESS (val), stream);
	}
      else
	fprintf (stream, "???");
      break;

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      fprintf (stream, "{");
      len = TYPE_NFIELDS (type);
      if (TYPE_BASECLASS (type))
	{
	  i = 1;
	  fprintf (stream, "<%s> = ", TYPE_NAME (TYPE_BASECLASS (type)));
	  val_print (TYPE_FIELD_TYPE (type, 0),
		     valaddr + TYPE_FIELD_BITPOS (type, 0) / 8,
		     0, stream);

	}
      else i = 0;
      for (; i < len; i++)
	{
	  if (i) fprintf (stream, ", ");
	  fprintf (stream, "%s = ", TYPE_FIELD_NAME (type, i));
	  /* check if static field */
	  if (TYPE_FIELD_STATIC (type, i))
	    {
	      value v;

	      v = value_static_field (type, TYPE_FIELD_NAME (type, i), i);
	      val_print (TYPE_FIELD_TYPE (type, i),
			 VALUE_CONTENTS (v), 0, stream);
	    }
	  else if (TYPE_FIELD_PACKED (type, i))
	    {
	      val = unpack_field_as_long (type, valaddr, i);
	      val_print (TYPE_FIELD_TYPE (type, i), &val, 0, stream);
	    }
	  else
	    {
	      val_print (TYPE_FIELD_TYPE (type, i), 
			 valaddr + TYPE_FIELD_BITPOS (type, i) / 8,
			 0, stream);
	    }
	}
      fprintf (stream, "}");
      break;

    case TYPE_CODE_ENUM:
      len = TYPE_NFIELDS (type);
      val = unpack_long (builtin_type_int, valaddr);
      for (i = 0; i < len; i++)
	{
	  QUIT;
	  if (val == TYPE_FIELD_VALUE (type, i))
	    break;
	}
      if (i < len)
	fprintf (stream, "%s", TYPE_FIELD_NAME (type, i));
      else
	fprintf (stream, "%d", val);
      break;

    case TYPE_CODE_FUNC:
      fprintf (stream, "{");
      type_print (type, "", stream, -1);
      fprintf (stream, "} ");
      fprintf (stream, "0x%x", address);
      break;

    case TYPE_CODE_INT:
      fprintf (stream,
	       TYPE_UNSIGNED (type) ? "%u" : "%d",
	       unpack_long (type, valaddr));
      if (type == builtin_type_char
	  || type == builtin_type_unsigned_char)
	{
	  fprintf (stream, " '");
	  printchar (unpack_long (type, valaddr), stream);
	  fputc ('\'', stream);
	}
      break;

    case TYPE_CODE_FLT:
#ifdef IEEE_FLOAT
      if (is_nan (unpack_double (type, valaddr)))
	{
	  fprintf (stream, "Nan");
	  break;
	}
#endif
      fprintf (stream, "%g", unpack_double (type, valaddr));
      break;

    case TYPE_CODE_VOID:
      fprintf (stream, "void");
      break;

    default:
      error ("Invalid type code in symbol table.");
    }
  fflush (stream);
}

#ifdef IEEE_FLOAT

union ieee {
  int i[2];
  double d;
};

/* Nonzero if ARG (a double) is a NAN.  */

int
is_nan (arg)
     union ieee arg;
{
  int lowhalf, highhalf;
  union { int i; char c; } test;

  /* Separate the high and low words of the double.
     Distinguish big and little-endian machines.  */
  test.i = 1;
  if (test.c != 1)
    /* Big-endian machine */
    lowhalf = arg.i[1], highhalf = arg.i[0];
  else
    lowhalf = arg.i[0], highhalf = arg.i[1];

  /* Nan: exponent is the maximum possible, and fraction is nonzero.  */
  return (((highhalf>>20) & 0x7ff) == 0x7ff
	  &&
	  ! ((highhalf & 0xfffff == 0) && (lowhalf == 0)));
}
#endif

/* Print a description of a type TYPE
   in the form of a declaration of a variable named VARSTRING.
   Output goes to STREAM (via stdio).
   If SHOW is positive, we show the contents of the outermost level
   of structure even if there is a type name that could be used instead.
   If SHOW is negative, we never show the details of elements' types.  */

type_print (type, varstring, stream, show)
     struct type *type;
     char *varstring;
     FILE *stream;
     int show;
{
  type_print_1 (type, varstring, stream, show, 0);
}

/* LEVEL is the depth to indent lines by.  */

type_print_1 (type, varstring, stream, show, level)
     struct type *type;
     char *varstring;
     FILE *stream;
     int show;
     int level;
{
  register enum type_code code;
  type_print_base (type, stream, show, level);
  code = TYPE_CODE (type);
  if ((varstring && *varstring)
      ||
      /* Need a space if going to print stars or brackets;
	 but not if we will print just a type name.  */
      ((show > 0 || TYPE_NAME (type) == 0)
       &&
       (code == TYPE_CODE_PTR || code == TYPE_CODE_FUNC
	|| code == TYPE_CODE_ARRAY
	|| code == TYPE_CODE_MPTR
	|| code == TYPE_CODE_REF)))
    fprintf (stream, " ");
  type_print_varspec_prefix (type, stream, show, 0);
  fprintf (stream, "%s", varstring);
  type_print_varspec_suffix (type, stream, show, 0);
}

/* Print the method arguments ARGS to the file STREAM.  */
static void
type_print_method_args (args, prefix, varstring, stream)
     struct type **args;
     char *prefix, *varstring;
     FILE *stream;
{
  int i;

  fprintf (stream, " %s%s (", prefix, varstring);
  if (args[1] && args[1]->code != TYPE_CODE_VOID)
    {
      i = 1;			/* skip the class variable */
      while (1)
	{
	  type_print (args[i++], "", stream, 0);
	  if (args[i]->code != TYPE_CODE_VOID)
	    {
	      fprintf (stream, ", ");
	    }
	  else break;
	}
    }
  fprintf (stream, ")");
}
  
/* Print any asterisks or open-parentheses needed before the
   variable name (to describe its type).

   On outermost call, pass 0 for PASSED_A_PTR.
   On outermost call, SHOW > 0 means should ignore
   any typename for TYPE and show its details.
   SHOW is always zero on recursive calls.  */

static void
type_print_varspec_prefix (type, stream, show, passed_a_ptr)
     struct type *type;
     FILE *stream;
     int show;
     int passed_a_ptr;
{
  if (TYPE_NAME (type) && show <= 0)
    return;

  QUIT;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_PTR:
      type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0, 1);
      fputc ('*', stream);
      break;

    case TYPE_CODE_MPTR:
      type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0, 1);
      if (passed_a_ptr)
	fputc ('(', stream);
      type_print_base (TYPE_DOMAIN_TYPE (type), stream, 0, 1);
      fprintf (stream, "::*");
      break;

    case TYPE_CODE_REF:
      type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0, 1);
      fputc ('&', stream);
      break;

    case TYPE_CODE_FUNC:
      type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0,
				 passed_a_ptr);
      if (passed_a_ptr)
	fputc ('(', stream);
      break;

    case TYPE_CODE_ARRAY:
      type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0,
				 passed_a_ptr);
    }
}

/* Print any array sizes, function arguments or close parentheses
   needed after the variable name (to describe its type).
   Args work like type_print_varspec_prefix.  */

static void
type_print_varspec_suffix (type, stream, show, passed_a_ptr)
     struct type *type;
     FILE *stream;
     int show;
     int passed_a_ptr;
{
  if (TYPE_NAME (type) && show <= 0)
    return;

  QUIT;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_ARRAY:
      type_print_varspec_suffix (TYPE_TARGET_TYPE (type), stream, 0,
				 passed_a_ptr);
      fprintf (stream, "[");
      if (TYPE_LENGTH (type) >= 0)
	fprintf (stream, "%d",
		 TYPE_LENGTH (type) / TYPE_LENGTH (TYPE_TARGET_TYPE (type)));
      fprintf (stream, "]");
      break;

    case TYPE_CODE_MPTR:
      if (passed_a_ptr)
	fputc (')', stream);
      /* Fall through.  */
    case TYPE_CODE_PTR:
    case TYPE_CODE_REF:
      type_print_varspec_suffix (TYPE_TARGET_TYPE (type), stream, 0, 1);
      break;

    case TYPE_CODE_FUNC:
      type_print_varspec_suffix (TYPE_TARGET_TYPE (type), stream, 0,
				 passed_a_ptr);
      if (passed_a_ptr)
	fprintf (stream, ")");
      fprintf (stream, "()");
      break;
    }
}

/* Print the name of the type (or the ultimate pointer target,
   function value or array element), or the description of a
   structure or union.

   SHOW nonzero means don't print this type as just its name;
   show its real definition even if it has a name.
   SHOW zero means print just typename or struct tag if there is one
   SHOW negative means abbreviate structure elements.
   SHOW is decremented for printing of structure elements.

   LEVEL is the depth to indent by.
   We increase it for some recursive calls.  */

static void
type_print_base (type, stream, show, level)
     struct type *type;
     FILE *stream;
     int show;
     int level;
{
  char *name;
  register int i;
  register int len;
  register int lastval;

  QUIT;

  if (TYPE_NAME (type) && show <= 0)
    {
      fprintf (stream, TYPE_NAME (type));
      return;
    }

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_ARRAY:
    case TYPE_CODE_PTR:
    case TYPE_CODE_MPTR:
    case TYPE_CODE_REF:
    case TYPE_CODE_FUNC:
      type_print_base (TYPE_TARGET_TYPE (type), stream, show, level);
      break;

    case TYPE_CODE_STRUCT:
      fprintf (stream, "struct ");
      goto struct_union;

    case TYPE_CODE_UNION:
      fprintf (stream, "union ");
    struct_union:
      if (TYPE_NAME (type) && (name = TYPE_NAME (type)))
	{
	  while (*name != ' ') name++;
	  fprintf (stream, "%s ", name + 1);
	}
      if (show < 0)
	fprintf (stream, "{...}");
      else
	{
	  struct type *basetype, *dtype;

	  dtype = type;
	  basetype = TYPE_BASECLASS (type);
	  while (basetype)
	    {
	      if (TYPE_NAME (basetype) && (name = TYPE_NAME (basetype)))
		{
		  while (*name != ' ') name++;
		  fprintf (stream, ": %s %s ",
			   TYPE_VIA_PUBLIC (dtype) ? "public" : "private",
			   name + 1);
		}
	      dtype = basetype;
	      basetype = TYPE_BASECLASS (basetype);
	    }
	  fprintf (stream, "{");
	  len = TYPE_NFIELDS (type);
	  if (len) fprintf (stream, "\n");
	  else fprintf (stream, "<no data fields>\n");

	  /* If there is a base class for this type,
	     do not print the field that it occupies.  */
	  for (i = !! TYPE_BASECLASS (type); i < len; i++)
	    {
	      QUIT;
	      /* Don't print out virtual function table.  */
	      if (! strncmp (TYPE_FIELD_NAME (type, i),
			   "_vptr$", 6))
		continue;

	      print_spaces (level + 4, stream);
	      if (TYPE_FIELD_STATIC (type, i))
		{
		  fprintf (stream, "static ");
		}
	      type_print_1 (TYPE_FIELD_TYPE (type, i),
			    TYPE_FIELD_NAME (type, i),
			    stream, show - 1, level + 4);
	      if (!TYPE_FIELD_STATIC (type, i)
		  && TYPE_FIELD_PACKED (type, i))
		{
		  /* ??? don't know what to put here ??? */;
		}
	      fprintf (stream, ";\n");
	    }

	  /* C++: print out the methods */
	  len = TYPE_NFN_FIELDS (type);
	  if (len) fprintf (stream, "\n");
	  for (i = 0; i < len; i++)
	    {
	      struct fn_field *f = TYPE_FN_FIELDLIST1 (type, i);
	      int j, len2 = TYPE_FN_FIELDLIST_LENGTH (type, i);

	      for (j = 0; j < len2; j++)
		{
		  QUIT;
		  print_spaces (level + 4, stream);
		  if (TYPE_FN_FIELD_VIRTUAL_P (f, j))
		    fprintf (stream, "virtual ");
		  type_print_base (TYPE_FN_FIELD_TYPE (f, j), stream, 0, level);
		  type_print_varspec_prefix (TYPE_FN_FIELD_TYPE (f, j), stream, 0, 0);
		  if (TYPE_FN_FIELD_PHYSNAME (f, j)[0] == '_'
		      && TYPE_FN_FIELD_PHYSNAME (f, j)[1] == '$')
		    type_print_method_args
		      (TYPE_FN_FIELD_ARGS (f, j) + 1, "~",
		       TYPE_FN_FIELDLIST_NAME (type, i), stream);
		  else
		    type_print_method_args
		      (TYPE_FN_FIELD_ARGS (f, j), "",
		       TYPE_FN_FIELDLIST_NAME (type, i), stream);

		  fprintf (stream, ";\n");
		}
	      if (len2) fprintf (stream, "\n");
	    }

	  print_spaces (level, stream);
	  fputc ('}', stream);
	}
      break;

    case TYPE_CODE_ENUM:
      fprintf (stream, "enum ");
      if (TYPE_NAME (type))
	{
	  name = TYPE_NAME (type);
	  while (*name != ' ') name++;
	  fprintf (stream, "%s ", name + 1);
	}
      if (show < 0)
	fprintf (stream, "{...}");
      else
	{
	  fprintf (stream, "{");
	  len = TYPE_NFIELDS (type);
	  lastval = 0;
	  for (i = 0; i < len; i++)
	    {
	      QUIT;
	      if (i) fprintf (stream, ", ");
	      fprintf (stream, "%s", TYPE_FIELD_NAME (type, i));
	      if (lastval != TYPE_FIELD_VALUE (type, i))
		{
		  fprintf (stream, " : %d", TYPE_FIELD_VALUE (type, i));
		  lastval = TYPE_FIELD_VALUE (type, i);
		}
	      lastval++;
	    }
	  fprintf (stream, "}");
	}
      break;

    case TYPE_CODE_INT:
      if (TYPE_UNSIGNED (type))
	name = unsigned_type_table[TYPE_LENGTH (type)];
      else
	name = signed_type_table[TYPE_LENGTH (type)];
      fprintf (stream, "%s", name);
      break;

    case TYPE_CODE_FLT:
      name = float_type_table[TYPE_LENGTH (type)];
      fprintf (stream, "%s", name);
      break;

    case TYPE_CODE_VOID:
      fprintf (stream, "void");
      break;

    case 0:
      fprintf (stream, "struct unknown");
      break;

    default:
      error ("Invalid type code in symbol table.");
    }
}

static void
set_maximum_command (arg)
     char *arg;
{
  if (!arg) error_no_arg ("value for maximum elements to print");
  print_max = atoi (arg);
}

static
initialize ()
{
  add_com ("set-maximum", class_vars, set_maximum_command,
	   "Set NUMBER as limit on string chars or array elements to print.");

  print_max = 200;

  unsigned_type_table
    = (char **) xmalloc ((1 + sizeof (unsigned long)) * sizeof (char *));
  bzero (unsigned_type_table, (1 + sizeof (unsigned long)));
  unsigned_type_table[sizeof (unsigned char)] = "unsigned char";
  unsigned_type_table[sizeof (unsigned short)] = "unsigned short";
  unsigned_type_table[sizeof (unsigned long)] = "unsigned long";
  unsigned_type_table[sizeof (unsigned int)] = "unsigned int";

  signed_type_table
    = (char **) xmalloc ((1 + sizeof (long)) * sizeof (char *));
  bzero (signed_type_table, (1 + sizeof (long)));
  signed_type_table[sizeof (char)] = "char";
  signed_type_table[sizeof (short)] = "short";
  signed_type_table[sizeof (long)] = "long";
  signed_type_table[sizeof (int)] = "int";

  float_type_table
    = (char **) xmalloc ((1 + sizeof (double)) * sizeof (char *));
  bzero (float_type_table, (1 + sizeof (double)));
  float_type_table[sizeof (float)] = "float";
  float_type_table[sizeof (double)] = "double";
}

END_FILE
