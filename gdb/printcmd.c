/* Print values for GNU debugger GDB.
   Copyright (C) 1986, 1987, 1988 Free Software Foundation, Inc.

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
#include "expression.h"

struct format_data
{
  int count;
  char format;
  char size;
};

/* Last specified output format.  */

static char last_format = 'x';

/* Last specified examination size.  'b', 'h', 'w' or `q'.  */

static char last_size = 'w';

/* Default address to examine next.  */

static CORE_ADDR next_address;

/* Last address examined.  */

static CORE_ADDR last_examine_address;

/* Contents of last address examined.
   This is not valid past the end of the `x' command!  */

static value last_examine_value;

/* Number of auto-display expression currently being displayed.
   So that we can deleted it if we get an error or a signal within it.
   -1 when not doing one.  */

int current_display_number;

static void do_one_display ();

void do_displays ();
void print_address ();
void print_scalar_formatted ();

START_FILE

/* Decode a format specification.  *STRING_PTR should point to it.
   OFORMAT and OSIZE are used as defaults for the format and size
   if none are given in the format specification.
   The structure returned describes all the data
   found in the specification.  In addition, *STRING_PTR is advanced
   past the specification and past all whitespace following it.  */

struct format_data
decode_format (string_ptr, oformat, osize)
     char **string_ptr;
     char oformat;
     char osize;
{
  struct format_data val;
  register char *p = *string_ptr;

  val.format = oformat;
  val.size = osize;
  val.count = 1;

  if (*p >= '0' && *p <= '9')
    val.count = atoi (p);
  while (*p >= '0' && *p <= '9') p++;

  /* Now process size or format letters that follow.  */

  while (1)
    {
      if (*p == 'b' || *p == 'h' || *p == 'w' || *p == 'g')
	val.size = *p++;
      else if (*p >= 'a' && *p <= 'z')
	val.format = *p++;
      else
	break;
    }

  /* Make sure 'g' size is not used on integer types.  */
  if (val.size == 'g' && val.format != 'f')
    val.size = 'w';

  while (*p == ' ' || *p == '\t') p++;
  *string_ptr = p;

  return val;
}

/* Print value VAL on stdout according to FORMAT, a letter or 0.
   Do not end with a newline.
   0 means print VAL according to its own type.
   SIZE is the letter for the size of datum being printed.
   This is used to pad hex numbers so they line up.  */

static void
print_formatted (val, format, size)
     register value val;
     register char format;
     char size;
{
  register CORE_ADDR val_long;
  int len = TYPE_LENGTH (VALUE_TYPE (val));

  if (VALUE_LVAL (val) == lval_memory)
    next_address = VALUE_ADDRESS (val) + len;

  switch (format)
    {
    case 's':
      next_address = VALUE_ADDRESS (val)
	+ value_print (value_addr (val), stdout, 0);
      break;

    case 'i':
      next_address = VALUE_ADDRESS (val)
	+ print_insn (VALUE_ADDRESS (val), stdout);
      break;

    default:
      if (format == 0
	  || TYPE_CODE (VALUE_TYPE (val)) == TYPE_CODE_ARRAY
	  || TYPE_CODE (VALUE_TYPE (val)) == TYPE_CODE_STRUCT
	  || TYPE_CODE (VALUE_TYPE (val)) == TYPE_CODE_UNION)
	value_print (val, stdout, format);
      else
	print_scalar_formatted (VALUE_CONTENTS (val), VALUE_TYPE (val),
				format, size, stdout);
    }
}

/* Print a scalar of data of type TYPE, pointed to in GDB by VALADDR,
   according to letters FORMAT and SIZE on STREAM.
   FORMAT may not be zero.  Formats s and i are not supported at this level.

   This is how the elements of an array or structure are printed
   with a format.  */

void
print_scalar_formatted (valaddr, type, format, size, stream)
     char *valaddr;
     struct type *type;
     char format;
     int size;
     FILE *stream;
{
  long val_long;
  int len = TYPE_LENGTH (type);

  val_long = unpack_long (type, valaddr);

  /* If value is unsigned, truncate it in case negative.  */
  if (format != 'd')
    {
      if (len == sizeof (char))
	val_long &= (1 << 8 * sizeof(char)) - 1;
      else if (len == sizeof (short))
	val_long &= (1 << 8 * sizeof(short)) - 1;
    }

  switch (format)
    {
    case 'x':
      switch (size)
	{
	case 'b':
	  printf ("0x%02x", val_long);
	  break;
	case 'h':
	  printf ("0x%04x", val_long);
	  break;
	case 0:		/* no size specified, like in print */
	case 'w':
	  printf ("0x%08x", val_long);
	  break;
	case 'g':
	  printf ("0x%16x", val_long);
	  break;
	default:
	  error ("Undefined output size \"%c\".", size);
	}
      break;

    case 'd':
      printf ("%d", val_long);
      break;

    case 'u':
      printf ("%u", val_long);
      break;

    case 'o':
      if (val_long)
	printf ("0%o", val_long);
      else
	printf ("0");
      break;

    case 'a':
      print_address (val_long, stream);
      break;

    case 'c':
      value_print (value_from_long (builtin_type_char, val_long), stream, 0);
      break;

    case 'f':
      if (len == sizeof (float))
	type = builtin_type_float;
      if (len == sizeof (double))
	type = builtin_type_double;
#ifdef IEEE_FLOAT
      if (is_nan (unpack_double (type, valaddr)))
	{
	  printf ("Nan");
	  break;
	}
#endif
      printf ("%g", unpack_double (type, valaddr));
      break;

    case 0:
      abort ();

    default:
      error ("Undefined output format \"%c\".", format);
    }
}

/* Specify default address for `x' command.
   `info lines' uses this.  */

void
set_next_address (addr)
     CORE_ADDR addr;
{
  next_address = addr;

  /* Make address available to the user as $_.  */
  set_internalvar (lookup_internalvar ("_"),
		   value_from_long (builtin_type_int, addr));
}

/* Print address ADDR symbolically on STREAM.
   First print it as a number.  Then perhaps print
   <SYMBOL + OFFSET> after the number.  */

void
print_address (addr, stream)
     CORE_ADDR addr;
     FILE *stream;
{
  register int i;

  fprintf (stream, "0x%x", addr);

  i = find_pc_misc_function (addr);
  if (i >= 0)
    if (misc_function_vector[i].address != addr)
      fprintf (stream, " <%s+%d>",
	       misc_function_vector[i].name,
	       addr - misc_function_vector[i].address);
    else
      fprintf (stream, " <%s>", misc_function_vector[i].name);

}

/* Examine data at address ADDR in format FMT.
   Fetch it from memory and print on stdout.  */

static void
do_examine (fmt, addr)
     struct format_data fmt;
     CORE_ADDR addr;
{
  register char format = 0;
  register char size;
  register int count = 1;
  struct type *val_type;
  register int i;
  register int maxelts;

  format = fmt.format;
  size = fmt.size;
  count = fmt.count;
  next_address = addr;

  /* String or instruction format implies fetch single bytes
     regardless of the specified size.  */
  if (format == 's' || format == 'i')
    size = 'b';

  if (size == 'b')
    val_type = builtin_type_char;
  else if (size == 'h')
    val_type = builtin_type_short;
  else if (size == 'w')
    val_type = builtin_type_long;
  else if (size == 'g')
    val_type = builtin_type_double;

  maxelts = 8;
  if (size == 'w')
    maxelts = 4;
  if (size == 'g')
    maxelts = 2;
  if (format == 's' || format == 'i')
    maxelts = 1;

  /* Print as many objects as specified in COUNT, at most maxelts per line,
     with the address of the next one at the start of each line.  */

  while (count > 0)
    {
      print_address (next_address, stdout);
      fputc (':', stdout);
      for (i = maxelts;
	   i > 0 && count > 0;
	   i--, count--)
	{
	  fputc ('\t', stdout);
	  /* Note that this sets next_address for the next object.  */
	  last_examine_address = next_address;
	  last_examine_value = value_at (val_type, next_address);
	  print_formatted (last_examine_value, format, size);
	}
      fputc ('\n', stdout);
      fflush (stdout);
    }
}

static void
validate_format (fmt, cmdname)
     struct format_data fmt;
     char *cmdname;
{
  if (fmt.size != 0)
    error ("Size letters are meaningless in \"%s\" command.", cmdname);
  if (fmt.count != 1)
    error ("Item count other than 1 is meaningless in \"%s\" command.",
	   cmdname);
  if (fmt.format == 'i' || fmt.format == 's')
    error ("Format letter \"%c\" is meaningless in \"%s\" command.",
	   fmt.format, cmdname);
}

static void
print_command (exp)
     char *exp;
{
  struct expression *expr;
  register struct cleanup *old_chain = 0;
  register char format = 0;
  register value val;
  struct format_data fmt;
  int histindex;
  int cleanup = 0;

  if (exp && *exp == '/')
    {
      exp++;
      fmt = decode_format (&exp, last_format, 0);
      validate_format (fmt, "print");
      last_format = format = fmt.format;
    }

  if (exp && *exp)
    {
      expr = parse_c_expression (exp);
      old_chain = make_cleanup (free_current_contents, &expr);
      cleanup = 1;
      val = evaluate_expression (expr);
    }
  else
    val = access_value_history (0);

  histindex = record_latest_value (val);
  printf ("$%d = ", histindex);

  print_formatted (val, format, fmt.size);
  printf ("\n");

  if (cleanup)
    do_cleanups (old_chain);
}

static void
output_command (exp)
     char *exp;
{
  struct expression *expr;
  register struct cleanup *old_chain;
  register char format = 0;
  register value val;
  struct format_data fmt;

  if (exp && *exp == '/')
    {
      exp++;
      fmt = decode_format (&exp, 0, 0);
      validate_format (fmt, "print");
      format = fmt.format;
    }

  expr = parse_c_expression (exp);
  old_chain = make_cleanup (free_current_contents, &expr);

  val = evaluate_expression (expr);

  print_formatted (val, format, fmt.size);

  do_cleanups (old_chain);
}

static void
set_command (exp)
     char *exp;
{
  struct expression *expr = parse_c_expression (exp);
  register struct cleanup *old_chain
    = make_cleanup (free_current_contents, &expr);
  evaluate_expression (expr);
  do_cleanups (old_chain);
}

static void
address_info (exp)
     char *exp;
{
  register struct symbol *sym;
  register CORE_ADDR val;

  if (exp == 0)
    error ("Argument required.");

  sym = lookup_symbol (exp, get_selected_block (), VAR_NAMESPACE);
  if (sym == 0)
    {
      register int i;

      for (i = 0; i < misc_function_count; i++)
	if (!strcmp (misc_function_vector[i].name, exp))
	  break;

      if (i < misc_function_count)
	printf ("Symbol \"%s\" is at 0x%x in a file compiled without -g.\n",
		exp, misc_function_vector[i].address);
      else
	error ("No symbol \"%s\" in current context.", exp);
      return;
    }

  printf ("Symbol \"%s\" is ", SYMBOL_NAME (sym));
  val = SYMBOL_VALUE (sym);

  switch (SYMBOL_CLASS (sym))
    {
    case LOC_CONST:
    case LOC_CONST_BYTES:
      printf ("constant");
      break;

    case LOC_LABEL:
      printf ("a label at address 0x%x", val);
      break;

    case LOC_REGISTER:
      printf ("a variable in register %s", reg_names[val]);
      break;

    case LOC_STATIC:
      printf ("static at address 0x%x", val);
      break;

    case LOC_ARG:
      printf ("an argument at offset %d", val);
      break;

    case LOC_LOCAL:
      printf ("a local variable at frame offset %d", val);
      break;

    case LOC_TYPEDEF:
      printf ("a typedef");
      break;

    case LOC_BLOCK:
      printf ("a function at address 0x%x",
	      BLOCK_START (SYMBOL_BLOCK_VALUE (sym)));
      break;
    }
  printf (".\n");
}

static void
x_command (exp, from_tty)
     char *exp;
     int from_tty;
{
  struct expression *expr;
  struct format_data fmt;
  struct cleanup *old_chain;

  fmt.format = last_format;
  fmt.size = last_size;
  fmt.count = 1;

  if (exp && *exp == '/')
    {
      exp++;
      fmt = decode_format (&exp, last_format, last_size);
      last_size = fmt.size;
      last_format = fmt.format;
    }

  /* If we have an expression, evaluate it and use it as the address.  */

  if (exp != 0 && *exp != 0)
    {
      expr = parse_c_expression (exp);
      /* Cause expression not to be there any more
	 if this command is repeated with Newline.
	 But don't clobber a user-defined command's definition.  */
      if (from_tty)
	*exp = 0;
      old_chain = make_cleanup (free_current_contents, &expr);
      next_address = value_as_long (evaluate_expression (expr));
      do_cleanups (old_chain);
    }

  do_examine (fmt, next_address);

  /* Make last address examined available to the user as $_.  */
  set_internalvar (lookup_internalvar ("_"),
		   value_from_long (builtin_type_int, last_examine_address));

  /* Make contents of last address examined available to the user as $__.  */
  set_internalvar (lookup_internalvar ("__"), last_examine_value);
}

/* Commands for printing types of things.  */

static void
whatis_command (exp)
     char *exp;
{
  struct expression *expr;
  register value val;
  register struct cleanup *old_chain;

  if (exp)
    {
      expr = parse_c_expression (exp);
      old_chain = make_cleanup (free_current_contents, &expr);
      val = evaluate_type (expr);
    }
  else
    val = access_value_history (0);

  printf ("type = ");
  type_print (VALUE_TYPE (val), "", stdout, 1);
  printf ("\n");

  if (exp)
    do_cleanups (old_chain);
}

static void
ptype_command (typename)
     char *typename;
{
  register char *p = typename;
  register int len;
  extern struct block *get_current_block ();
  register struct block *b
    = (have_inferior_p () || have_core_file_p ()) ? get_current_block () : 0;
  register struct type *type;

  if (typename == 0)
    error_no_arg ("type name");

  while (*p && *p != ' ' && *p != '\t') p++;
  len = p - typename;
  while (*p == ' ' || *p == '\t') p++;

  if (len == 6 && !strncmp (typename, "struct", 6))
    type = lookup_struct (p, b);
  else if (len == 5 && !strncmp (typename, "union", 5))
    type = lookup_union (p, b);
  else if (len == 4 && !strncmp (typename, "enum", 4))
    type = lookup_enum (p, b);
  else
    {
      type = lookup_typename (typename, b, 1);
      if (type == 0)
	{
	  register struct symbol *sym
	    = lookup_symbol (typename, b, STRUCT_NAMESPACE);
	  if (sym == 0)
	    error ("No type named %s.", typename);
	  printf ("No type named %s, but there is a ",
		  typename);
	  switch (TYPE_CODE (SYMBOL_TYPE (sym)))
	    {
	    case TYPE_CODE_STRUCT:
	      printf ("struct");
	      break;

	    case TYPE_CODE_UNION:
	      printf ("union");
	      break;

	    case TYPE_CODE_ENUM:
	      printf ("enum");
	    }
	  printf (" %s.  Type \"help ptype\".\n", typename);
	  type = SYMBOL_TYPE (sym);
	}
    }

  type_print (type, "", stdout, 1);
  printf ("\n");
}

struct display
{
  /* Chain link to next auto-display item.  */
  struct display *next;
  /* Expression to be evaluated and displayed.  */
  struct expression *exp;
  /* Item number of this auto-display item.  */
  int number;
  /* Display format specified.  */
  struct format_data format;
  /* Block in which expression is to be evaluated.  */
  struct block *block;
};

/* Chain of expressions whose values should be displayed
   automatically each time the program stops.  */

static struct display *display_chain;

static int display_number;

/* Add an expression to the auto-display chain.
   Specify the expression.  */

static void
display_command (exp, from_tty)
     char *exp;
     int from_tty;
{
  struct format_data fmt;
  register struct expression *expr;
  register struct display *new;

  if (exp == 0)
    {
      do_displays ();
      return;
    }

  if (*exp == '/')
    {
      exp++;
      fmt = decode_format (&exp, 0, 0);
      if (fmt.size && fmt.format == 0)
	fmt.format = 'x';
      if (fmt.format == 'i' || fmt.format == 's')
	fmt.size = 'b';
    }
  else
    {
      fmt.format = 0;
      fmt.size = 0;
      fmt.count = 0;
    }

  expr = parse_c_expression (exp);

  new = (struct display *) xmalloc (sizeof (struct display));

  new->exp = expr;
  new->next = display_chain;
  new->number = ++display_number;
  new->format = fmt;
  display_chain = new;

  if (from_tty)
    do_one_display (new);

  dont_repeat ();
}

static void
free_display (d)
     struct display *d;
{
  free (d->exp);
  free (d);
}

/* Clear out the display_chain.
   Done when new symtabs are loaded, since this invalidates
   the types stored in many expressions.  */

void
clear_displays ()
{
  register struct display *d;

  while (d = display_chain)
    {
      free (d->exp);
      display_chain = d->next;
      free (d);
    }
}

/* Delete the auto-display number NUM.  */

void
delete_display (num)
     int num;
{
  register struct display *d1, *d;

  if (!display_chain)
    error ("No display number %d.", num);

  if (display_chain->number == num)
    {
      d1 = display_chain;
      display_chain = d1->next;
      free_display (d1);
    }
  else
    for (d = display_chain; ; d = d->next)
      {
	if (d->next == 0)
	  error ("No display number %d.", num);
	if (d->next->number == num)
	  {
	    d1 = d->next;
	    d->next = d1->next;
	    free_display (d1);
	    break;
	  }
      }
}

/* Delete some values from the auto-display chain.
   Specify the element numbers.  */

static void
undisplay_command (args)
     char *args;
{
  register char *p = args;
  register char *p1;
  register int num;
  register struct display *d, *d1;

  if (args == 0)
    {
      if (query ("Delete all auto-display expressions? "))
	clear_displays ();
      dont_repeat ();
      return;
    }

  while (*p)
    {
      p1 = p;
      while (*p1 >= '0' && *p1 <= '9') p1++;
      if (*p1 && *p1 != ' ' && *p1 != '\t')
	error ("Arguments must be display numbers.");

      num = atoi (p);

      delete_display (num);

      p = p1;
      while (*p == ' ' || *p == '\t') p++;
    }
  dont_repeat ();
}

/* Display a single auto-display.  */

static void
do_one_display (d)
     struct display *d;
{
  current_display_number = d->number;

  printf ("%d: ", d->number);
  if (d->format.size)
    {
      printf ("x/");
      if (d->format.count != 1)
	printf ("%d", d->format.count);
      printf ("%c", d->format.format);
      if (d->format.format != 'i' && d->format.format != 's')
	printf ("%c", d->format.size);
      printf (" ");
      print_expression (d->exp, stdout);
      if (d->format.count != 1)
	printf ("\n");
      else
	printf ("  ");
      do_examine (d->format,
		  value_as_long (evaluate_expression (d->exp)));
    }
  else
    {
      if (d->format.format)
	printf ("/%c ", d->format.format);
      print_expression (d->exp, stdout);
      printf (" = ");
      print_formatted (evaluate_expression (d->exp),
		       d->format.format, d->format.size);
      printf ("\n");
    }

  fflush (stdout);
  current_display_number = -1;
}

/* Display all of the values on the auto-display chain.  */

void
do_displays ()
{
  register struct display *d;

  for (d = display_chain; d; d = d->next)
    do_one_display (d);
}

/* Delete the auto-display which we were in the process of displaying.
   This is done when there is an error or a signal.  */

void
delete_current_display ()
{
  if (current_display_number >= 0)
    {
      delete_display (current_display_number);
      fprintf (stderr, "Deleting display %d to avoid infinite recursion.\n",
	       current_display_number);
    }
  current_display_number = -1;
}

static void
display_info ()
{
  register struct display *d;

  if (!display_chain)
    printf ("There are no auto-display expressions now.\n");
  else
    printf ("Auto-display expressions now in effect:\n");
  for (d = display_chain; d; d = d->next)
    {
      printf ("%d: ", d->number);
      if (d->format.size)
	printf ("/%d%c%c ", d->format.count, d->format.size,
		d->format.format);
      else if (d->format.format)
	printf ("/%c ", d->format.format);
      print_expression (d->exp, stdout);
      printf ("\n");
      fflush (stdout);
    }
}

/* Print the value in stack frame FRAME of a variable
   specified by a struct symbol.  */

void
print_variable_value (var, frame, stream)
     struct symbol *var;
     CORE_ADDR frame;
     FILE *stream;
{
  value val = read_var_value (var, frame);
  value_print (val, stream, 0);
}

/* Print the arguments of a stack frame, given the function FUNC
   running in that frame (as a symbol), the address of the arglist,
   and the number of args according to the stack frame (or -1 if unknown).  */

static void print_frame_nameless_args ();

print_frame_args (func, addr, num, stream)
     struct symbol *func;
     register CORE_ADDR addr;
     int num;
     FILE *stream;
{
  struct block *b;
  int nsyms = 0;
  int first = 1;
  register int i;
  register int last_offset = FRAME_ARGS_SKIP;
  register struct symbol *sym, *nextsym;
  register value val;

  if (func)
    {
      b = SYMBOL_BLOCK_VALUE (func);
      nsyms = BLOCK_NSYMS (b);
    }

  while (1)
    {
      /* Find first arg that is not before LAST_OFFSET.  */
      nextsym = 0;
      for (i = 0; i < nsyms; i++)
	{
	  QUIT;
	  sym = BLOCK_SYM (b, i);
	  if (SYMBOL_CLASS (sym) == LOC_ARG
	      && SYMBOL_VALUE (sym) >= last_offset
	      && (nextsym == 0
		  || SYMBOL_VALUE (sym) < SYMBOL_VALUE (nextsym)))
	    nextsym = sym;
	}
      if (nextsym == 0)
	break;
      sym = nextsym;
      /* Print any nameless args between the last arg printed
	 and the next arg.  */
      if (last_offset != (SYMBOL_VALUE (sym) / sizeof (int)) * sizeof (int))
	{
	  print_frame_nameless_args (addr, last_offset, SYMBOL_VALUE (sym),
				     stream);
	  first = 0;
	}
      /* Print the next arg.  */
      val = value_at (SYMBOL_TYPE (sym), addr + SYMBOL_VALUE (sym));
      if (! first)
	fprintf (stream, ", ");
      fprintf (stream, "%s=", SYMBOL_NAME (sym));
      value_print (val, stream, 0);
      first = 0;
      last_offset = SYMBOL_VALUE (sym) + TYPE_LENGTH (SYMBOL_TYPE (sym));
      /* Round up address of next arg to multiple of size of int.  */
      last_offset
	= ((last_offset + sizeof (int) - 1) / sizeof (int)) * sizeof (int);
    }
  if (num >= 0 && num * sizeof (int) + FRAME_ARGS_SKIP > last_offset)
    print_frame_nameless_args (addr, last_offset,
			       num * sizeof (int) + FRAME_ARGS_SKIP, stream);
}

static void
print_frame_nameless_args (argsaddr, start, end, stream)
     CORE_ADDR argsaddr;
     int start;
     int end;
     FILE *stream;
{
  while (start < end)
    {
      QUIT;
      if (start != FRAME_ARGS_SKIP)
	fprintf (stream, ", ");
      fprintf (stream, "%d",
	       read_memory_integer (argsaddr + start, sizeof (int)));
      start += sizeof (int);
    }
}

static void
printf_command (arg)
     char *arg;
{
  register char *f;
  register char *s = arg;
  char *string;
  value *val_args;
  int nargs = 0;
  int allocated_args = 20;
  char *arg_bytes;
  char *argclass;
  int i;
  int argindex;
  int nargs_wanted;

  val_args = (value *) xmalloc (allocated_args * sizeof (value));

  if (s == 0)
    error_no_arg ("format-control string and values to print");

  /* Skip white space before format string */
  while (*s == ' ' || *s == '\t') s++;

  /* A format string should follow, enveloped in double quotes */
  if (*s++ != '"')
    error ("Bad format string, missing '\"'.");

  /* Parse the format-control string and copy it into the string STRING,
     processing some kinds of escape sequence.  */

  f = string = (char *) alloca (strlen (s) + 1);
  while (*s != '"')
    {
      int c = *s++;
      switch (c)
	{
	case '\0':
	  error ("Bad format string, non-terminated '\"'.");
	  /* doesn't return */

	case '\\':
	  switch (c = *s++)
	    {
	    case '\\':
	      *f++ = '\\';
	      break;
	    case 'n':
	      *f++ = '\n';
	      break;
	    case 't':
	      *f++ = '\t';
	      break;
	    case 'r':
	      *f++ = '\r';
	      break;
	    case '"':
	      *f++ = '"';
	      break;
	    default:
	      /* ??? TODO: handle other escape sequences */
	      error ("Unrecognized \\ escape character in format string.");
	    }
	  break;

	default:
	  *f++ = c;
	}
    }

  /* Skip over " and following space and comma.  */
  s++;
  *f++ = '\0';
  while (*s == ' ' || *s == '\t') s++;

  if (*s != ',' && *s != 0)
    error ("Invalid argument syntax");

  if (*s == ',') s++;
  while (*s == ' ' || *s == '\t') s++;

  /* Now scan the string for %-specs and see what kinds of args they want.
     argclass[I] is set to 1 if the Ith arg should be a string.  */

  argclass = (char *) alloca (strlen (s));
  nargs_wanted = 0;
  f = string;
  while (*f)
    if (*f++ == '%')
      {
	while (index ("0123456789.hlL-+ #", *f)) f++;
	if (*f == 's')
	  argclass[nargs_wanted++] = 1;
	else if (*f != '%')
	  argclass[nargs_wanted++] = 0;
	f++;
      }

  /* Now, parse all arguments and evaluate them.
     Store the VALUEs in VAL_ARGS.  */

  while (*s != '\0')
    {
      char *s1;
      if (nargs == allocated_args)
	val_args = (value *) xrealloc (val_args,
				       (allocated_args *= 2)
				       * sizeof (value));
      s1 = s;
      val_args[nargs++] = parse_to_comma_and_eval (&s1);
      s = s1;
      if (*s == ',')
	s++;
    }

  if (nargs != nargs_wanted)
    error ("Wrong number of arguments for specified format-string");

  /* Now lay out an argument-list containing the arguments
     as doubles, integers and C pointers.  */

  arg_bytes = (char *) alloca (sizeof (double) * nargs);
  argindex = 0;
  for (i = 0; i < nargs; i++)
    {
      if (argclass[i])
	{
	  char *str;
	  int tem, j;
	  tem = value_as_long (val_args[i]);

	  /* This is a %s argument.  Find the length of the string.  */
	  for (j = 0; ; j++)
	    {
	      char c;
	      QUIT;
	      read_memory (tem + j, &c, 1);
	      if (c == 0)
		break;
	    }

	  /* Copy the string contents into a string inside GDB.  */
	  str = (char *) alloca (j + 1);
	  read_memory (tem, str, j);
	  str[j] = 0;

	  /* Pass address of internal copy as the arg to vprintf.  */
	  *((int *) &arg_bytes[argindex]) = (int) str;
	  argindex += sizeof (int);
	}
      else if (VALUE_TYPE (val_args[i])->code == TYPE_CODE_FLT)
	{
	  *((double *) &arg_bytes[argindex]) = value_as_double (val_args[i]);
	  argindex += sizeof (double);
	}
      else
	{
	  *((int *) &arg_bytes[argindex]) = value_as_long (val_args[i]);
	  argindex += sizeof (int);
	}
    }

  vprintf (string, arg_bytes);
}

static
initialize ()
{
  current_display_number = -1;

  add_info ("address", address_info,
	   "Describe where variable VAR is stored.");

  add_com ("x", class_vars, x_command,
	   "Examine memory: x/FMT ADDRESS.\n\
ADDRESS is an expression for the memory address to examine.\n\
FMT is a repeat count followed by a format letter and a size letter.\n\
Format letters are o(octal), x(hex), d(decimal), u(unsigned decimal),\n\
 f(float), a(address), i(instruction), c(char) and s(string).\n\
Size letters are b(byte), h(halfword), w(word), g(giant, 8 bytes).\n\
  g is meaningful only with f, for type double.\n\
The specified number of objects of the specified size are printed\n\
according to the format.\n\n\
Defaults for format and size letters are those previously used.\n\
Default count is 1.  Default address is following last thing printed\n\
with this command or \"print\".");

  add_com ("ptype", class_vars, ptype_command,
	   "Print definition of type TYPE.\n\
Argument may be a type name defined by typedef, or \"struct STRUCTNAME\"\n\
or \"union UNIONNAME\" or \"enum ENUMNAME\".\n\
The selected stack frame's lexical context is used to look up the name.");

  add_com ("whatis", class_vars, whatis_command,
	   "Print data type of expression EXP.");

  add_info ("display", display_info,
	    "Expressions to display when program stops, with code numbers.");
  add_com ("undisplay", class_vars, undisplay_command,
	   "Cancel some expressions to be displayed whenever program stops.\n\
Arguments are the code numbers of the expressions to stop displaying.\n\
No argument means cancel all automatic-display expressions.\n\
Do \"info display\" to see current list of code numbers.");
  add_com ("display", class_vars, display_command,
	   "Print value of expression EXP each time the program stops.\n\
/FMT may be used before EXP as in the \"print\" command.\n\
/FMT \"i\" or \"s\" or including a size-letter is allowed,\n\
as in the \"x\" command, and then EXP is used to get the address to examine\n\
and examining is done as in the \"x\" command.\n\n\
With no argument, display all currently requested auto-display expressions.\n\
Use \"undisplay\" to cancel display requests previously made.");

  add_com ("printf", class_vars, printf_command,
	"printf \"printf format string\", arg1, arg2, arg3, ..., argn\n\
This is useful for formatted output in user-defined commands.");
  add_com ("output", class_vars, output_command,
	   "Like \"print\" but don't put in value history and don't print newline.\n\
This is useful in user-defined commands.");

  add_com ("set", class_vars, set_command,
	   "Perform an assignment VAR = EXP.  You must type the \"=\".\n\
VAR may be a debugger \"convenience\" variables (names starting with $),\n\
a register (a few standard names starting with $), or an actual variable\n\
in the program being debugger.  EXP is any expression.");

  add_com ("print", class_vars, print_command,
	   concat ("Print value of expression EXP.\n\
Variables accessible are those of the lexical environment of the selected\n\
stack frame, plus all those whose scope is global or an entire file.\n\
\n\
$NUM gets previous value number NUM.  $ and $$ are the last two values.\n\
$$NUM refers to NUM'th value back from the last one.\n\
Names starting with $ refer to registers (with the values they would have\n\
if the program were to return to the stack frame now selected, restoring\n\
all registers saved by frames farther in) or else to debugger\n\
\"convenience\" variables (any such name not a known register).\n\
Use assignment expressions to give values to convenience variables.\n",
		   "\n\
\{TYPE}ADREXP refers to a datum of data type TYPE, located at address ADREXP.\n\
@ is a binary operator for treating consecutive data objects\n\
anywhere in memory as an array.  FOO@NUM gives an array whose first\n\
element is FOO, whose second element is stored in the space following\n\
where FOO is stored, etc.  FOO must be an expression whose value\n\
resides in memory.\n",
		   "\n\
EXP may be preceded with /FMT, where FMT is a format letter\n\
but no count or size letter (see \"x\" command)."));
  add_com_alias ("p", "print", class_vars, 1);
}

END_FILE
