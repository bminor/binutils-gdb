/* Print values for GNU debugger gdb.
   Copyright (C) 1986, 1988 Free Software Foundation, Inc.

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
#include "param.h"
#include "symtab.h"
#include "value.h"

/* Maximum number of chars to print for a string pointer value
   or vector contents.  */

static int print_max;

static void type_print_varspec_suffix ();
static void type_print_varspec_prefix ();
static void type_print_base ();

START_FILE

char **unsigned_type_table;
char **signed_type_table;
char **float_type_table;

/* Print the value VAL in C-ish syntax on stream STREAM.
   FORMAT is a format-letter, or 0 for print in natural format of data type.
   If the object printed is a string pointer, returns
   the number of string bytes printed.  */

value_print (val, stream, format)
     value val;
     FILE *stream;
     char format;
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
      if (typelen == 1 && TYPE_CODE (VALUE_TYPE (val)) == TYPE_CODE_INT
	  && format == 0)
	{
	  fputc ('"', stream);
	  for (i = 0; i < n && i < print_max; i++)
	    {
	      QUIT;
	      printchar (VALUE_CONTENTS (val)[i], stream, '"');
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
			 VALUE_ADDRESS (val) + typelen * i, stream, format);
	    }
	  if (i < n)
	    fprintf (stream, "...");
	}
      fputc ('}', stream);
    }
  else
    {
      /* A simple (nonrepeated) value */
      /* If it is a pointer, indicate what it points to.  */
      if (TYPE_CODE (VALUE_TYPE (val)) == TYPE_CODE_PTR)
	{
	  fprintf (stream, "(");
	  type_print (VALUE_TYPE (val), "", stream, -1);
	  fprintf (stream, ") ");
	}
      return val_print (VALUE_TYPE (val), VALUE_CONTENTS (val),
			VALUE_ADDRESS (val), stream, format);
    }
}

/* Print data of type TYPE located at VALADDR (within GDB),
   which came from the inferior at address ADDRESS,
   onto stdio stream STREAM according to FORMAT
   (a letter or 0 for natural format).

   If the data are a string pointer, returns the number of
   sting characters printed.  */

int
val_print (type, valaddr, address, stream, format)
     struct type *type;
     char *valaddr;
     CORE_ADDR address;
     FILE *stream;
     char format;
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
	  if (eltlen == 1 && TYPE_CODE (elttype) == TYPE_CODE_INT
	      && format == 0)
	    {
	      fputc ('"', stream);
	      for (i = 0; i < len && i < print_max; i++)
		{
		  QUIT;
		  printchar (valaddr[i], stream, '"');
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
			     0, stream, format);
		}
	      if (i < len)
		fprintf (stream, "...");
	    }
	  fprintf (stream, "}");
	  break;
	}
      /* Array of unspecified length: treat like pointer.  */

    case TYPE_CODE_PTR:
      if (format)
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	  break;
	}
      fprintf (stream, "0x%x", * (int *) valaddr);
      /* For a pointer to char or unsigned char,
	 also print the string pointed to, unless pointer is null.  */

      /* For an array of chars, print with string syntax.  */
      elttype = TYPE_TARGET_TYPE (type);
      if (TYPE_LENGTH (elttype) == 1 && TYPE_CODE (elttype) == TYPE_CODE_INT
	  && format == 0
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
	      printchar (c, stream, '"');
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

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      fprintf (stream, "{");
      len = TYPE_NFIELDS (type);
      for (i = 0; i < len; i++)
	{
	  if (i) fprintf (stream, ", ");
	  fprintf (stream, "%s = ", TYPE_FIELD_NAME (type, i));
	  if (TYPE_FIELD_PACKED (type, i))
	    {
	      val = unpack_field_as_long (type, valaddr, i);
	      val_print (TYPE_FIELD_TYPE (type, i), &val, 0, stream, format);
	    }
	  else
	    val_print (TYPE_FIELD_TYPE (type, i), 
		       valaddr + TYPE_FIELD_BITPOS (type, i) / 8,
		       0, stream, format);
	}
      fprintf (stream, "}");
      break;

    case TYPE_CODE_ENUM:
      if (format)
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	  break;
	}
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
      if (format)
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	  break;
	}
      fprintf (stream, "{");
      type_print (type, "", stream, -1);
      fprintf (stream, "} ");
      fprintf (stream, "0x%x", address);
      break;

    case TYPE_CODE_INT:
      if (format)
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	  break;
	}
      fprintf (stream,
	       TYPE_UNSIGNED (type) ? "%u" : "%d",
	       unpack_long (type, valaddr));
      if (TYPE_LENGTH (type) == 1)
	{
	  fprintf (stream, " '");
	  printchar (unpack_long (type, valaddr), stream, '\'');
	  fputc ('\'', stream);
	}
      break;

    case TYPE_CODE_FLT:
      if (format)
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	  break;
	}
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
	|| code == TYPE_CODE_ARRAY)))
    fprintf (stream, " ");
  type_print_varspec_prefix (type, stream, show, 0);
  fprintf (stream, "%s", varstring);
  type_print_varspec_suffix (type, stream, show, 0);
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
  if (type == 0)
    return;

  if (TYPE_NAME (type) && show <= 0)
    return;

  QUIT;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_PTR:
      type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0, 1);
      fputc ('*', stream);
      break;

    case TYPE_CODE_FUNC:
    case TYPE_CODE_ARRAY:
      type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0, 0);
      if (passed_a_ptr)
	fputc ('(', stream);
      break;
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
  if (type == 0)
    return;

  if (TYPE_NAME (type) && show <= 0)
    return;

  QUIT;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_ARRAY:
      type_print_varspec_suffix (TYPE_TARGET_TYPE (type), stream, 0, 0);
      if (passed_a_ptr)
	fprintf (stream, ")");
      fprintf (stream, "[");
      if (TYPE_LENGTH (type) >= 0)
	fprintf (stream, "%d",
		 TYPE_LENGTH (type) / TYPE_LENGTH (TYPE_TARGET_TYPE (type)));
      fprintf (stream, "]");
      break;

    case TYPE_CODE_PTR:
      type_print_varspec_suffix (TYPE_TARGET_TYPE (type), stream, 0, 1);
      break;

    case TYPE_CODE_FUNC:
      type_print_varspec_suffix (TYPE_TARGET_TYPE (type), stream, 0, 0);
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

  if (type == 0)
    {
      fprintf (stream, "type unknown");
      return;
    }

  if (TYPE_NAME (type) && show <= 0)
    {
      fprintf (stream, TYPE_NAME (type));
      return;
    }

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_ARRAY:
    case TYPE_CODE_PTR:
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
	  fprintf (stream, "{");
	  len = TYPE_NFIELDS (type);
	  fprintf (stream, "\n");
	  for (i = 0; i < len; i++)
	    {
	      QUIT;
	      print_spaces (level + 4, stream);

	      /* If this is a bit-field and there is a gap before it,
		 print a nameless field to account for the gap.  */

	      if (TYPE_FIELD_PACKED (type, i))
		{
		  int gap = (TYPE_FIELD_BITPOS (type, i)
			     - (i > 0
				? (TYPE_FIELD_BITPOS (type, i - 1)
				   + (TYPE_FIELD_PACKED (type, i - 1)
				      ? TYPE_FIELD_BITSIZE (type, i - 1)
				      : TYPE_LENGTH (TYPE_FIELD_TYPE (type, i - 1)) * 8))
				: 0));
		  if (gap != 0)
		    {
		      fprintf (stream, "int : %d;\n", gap);
		      print_spaces (level + 4, stream);
		    }
		}

	      /* Print the declaration of this field.  */

	      type_print_1 (TYPE_FIELD_TYPE (type, i),
			    TYPE_FIELD_NAME (type, i),
			    stream, show - 1, level + 4);

	      /* Print the field width.  */

	      if (TYPE_FIELD_PACKED (type, i))
		fprintf (stream, " : %d", TYPE_FIELD_BITSIZE (type, i));

	      fprintf (stream, ";\n");
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
