/* Low level packing and unpacking of values for GDB.
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.

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

/* The value-history records all the values printed
   by print commands during this session.  Each chunk
   records 60 consecutive values.  The first chunk on
   the chain records the most recent values.
   The total number of values is in value_history_count.  */

#define VALUE_HISTORY_CHUNK 60

struct value_history_chunk
{
  struct value_history_chunk *next;
  value values[VALUE_HISTORY_CHUNK];
};

/* Chain of chunks now in use.  */

static struct value_history_chunk *value_history_chain;

static int value_history_count;	/* Abs number of last entry stored */

START_FILE

/* List of all value objects currently allocated
   (except for those released by calls to release_value)
   This is so they can be freed after each command.  */

static value all_values;

/* Allocate a  value  that has the correct length for type TYPE.  */

value
allocate_value (type)
     struct type *type;
{
  register value val;

  val = (value) xmalloc (sizeof (struct value) + TYPE_LENGTH (type));
  VALUE_NEXT (val) = all_values;
  all_values = val;
  VALUE_TYPE (val) = type;
  VALUE_LVAL (val) = not_lval;
  VALUE_ADDRESS (val) = 0;
  VALUE_OFFSET (val) = 0;
  VALUE_BITPOS (val) = 0;
  VALUE_BITSIZE (val) = 0;
  VALUE_REPEATED (val) = 0;
  VALUE_REPETITIONS (val) = 0;
  VALUE_REGNO (val) = -1;
  return val;
}

/* Allocate a  value  that has the correct length
   for COUNT repetitions type TYPE.  */

value
allocate_repeat_value (type, count)
     struct type *type;
     int count;
{
  register value val;

  val = (value) xmalloc (sizeof (struct value) + TYPE_LENGTH (type) * count);
  VALUE_NEXT (val) = all_values;
  all_values = val;
  VALUE_TYPE (val) = type;
  VALUE_LVAL (val) = not_lval;
  VALUE_ADDRESS (val) = 0;
  VALUE_OFFSET (val) = 0;
  VALUE_BITPOS (val) = 0;
  VALUE_BITSIZE (val) = 0;
  VALUE_REPEATED (val) = 1;
  VALUE_REPETITIONS (val) = count;
  VALUE_REGNO (val) = -1;
  return val;
}

/* Free all the values that have been allocated (except for those released).
   Called after each command, successful or not.  */

void
free_all_values ()
{
  register value val, next;

  for (val = all_values; val; val = next)
    {
      next = VALUE_NEXT (val);
      free (val);
    }

  all_values = 0;
}

/* Remove VAL from the chain all_values
   so it will not be freed automatically.  */

void
release_value (val)
     register value val;
{
  register value v;

  if (all_values == val)
    {
      all_values = val->next;
      return;
    }

  for (v = all_values; v; v = v->next)
    {
      if (v->next == val)
	{
	  v->next = val->next;
	  break;
	}
    }
}

/* Return a copy of the value ARG.
   It contains the same contents, for same memory address,
   but it's a different block of storage.  */

static value
value_copy (arg)
     value arg;
{
  register value val;
  register struct type *type = VALUE_TYPE (arg);
  if (VALUE_REPEATED (arg))
    val = allocate_repeat_value (type, VALUE_REPETITIONS (arg));
  else
    val = allocate_value (type);
  VALUE_LVAL (val) = VALUE_LVAL (arg);
  VALUE_ADDRESS (val) = VALUE_ADDRESS (arg);
  VALUE_OFFSET (val) = VALUE_OFFSET (arg);
  VALUE_BITPOS (val) = VALUE_BITPOS (arg);
  VALUE_BITSIZE (val) = VALUE_BITSIZE (arg);
  VALUE_REGNO (val) = VALUE_REGNO (arg);
  bcopy (VALUE_CONTENTS (arg), VALUE_CONTENTS (val),
	 TYPE_LENGTH (VALUE_TYPE (arg))
	 * (VALUE_REPEATED (arg) ? VALUE_REPETITIONS (arg) : 1));
  return val;
}

/* Access to the value history.  */

/* Record a new value in the value history.
   Returns the absolute history index of the entry.  */

int
record_latest_value (val)
     value val;
{
  register int i;

  /* Get error now if about to store an invalid float.  */
  if (TYPE_CODE (VALUE_TYPE (val)) == TYPE_CODE_FLT)
    value_as_double (val);

  /* Here we treat value_history_count as origin-zero
     and applying to the value being stored now.  */

  i = value_history_count % VALUE_HISTORY_CHUNK;
  if (i == 0)
    {
      register struct value_history_chunk *new
	= (struct value_history_chunk *) xmalloc (sizeof (struct value_history_chunk));
      bzero (new->values, sizeof new->values);
      new->next = value_history_chain;
      value_history_chain = new;
    }

  value_history_chain->values[i] = val;
  release_value (val);

  /* Now we regard value_history_count as origin-one
     and applying to the value just stored.  */

  return ++value_history_count;
}

/* Return a copy of the value in the history with sequence number NUM.  */

value
access_value_history (num)
     int num;
{
  register struct value_history_chunk *chunk;
  register int i;
  register int absnum = num;

  if (absnum <= 0)
    absnum += value_history_count;

  if (absnum <= 0)
    {
      if (num == 0)
	error ("The history is empty.");
      else if (num == 1)
	error ("There is only one value in the history.");
      else
	error ("History does not go back to $$%d.", -num);
    }
  if (absnum > value_history_count)
    error ("History has not yet reached $%d.", absnum);

  absnum--;

  /* Now absnum is always absolute and origin zero.  */

  chunk = value_history_chain;
  for (i = (value_history_count - 1) / VALUE_HISTORY_CHUNK - absnum / VALUE_HISTORY_CHUNK;
       i > 0; i--)
    chunk = chunk->next;

  return value_copy (chunk->values[absnum % VALUE_HISTORY_CHUNK]);
}

/* Clear the value history entirely.
   Must be done when new symbol tables are loaded,
   because the type pointers become invalid.  */

void
clear_value_history ()
{
  register struct value_history_chunk *next;
  register int i;
  register value val;

  while (value_history_chain)
    {
      for (i = 0; i < VALUE_HISTORY_CHUNK; i++)
	if (val = value_history_chain->values[i])
	  free (val);
      next = value_history_chain->next;
      free (value_history_chain);
      value_history_chain = next;
    }
  value_history_count = 0;
}

static void
history_info (num_exp)
     char *num_exp;
{
  register int i;
  register value val;
  register int num;

  if (num_exp)
    num = parse_and_eval_address (num_exp) - 5;
  else
    num = value_history_count - 9;

  if (num <= 0)
    num = 1;

  for (i = num; i < num + 10 && i <= value_history_count; i++)
    {
      val = access_value_history (i);
      printf ("$%d = ", i);
      value_print (val, stdout, 0);
      printf ("\n");
    }
}

/* Internal variables.  These are variables within the debugger
   that hold values assigned by debugger commands.
   The user refers to them with a '$' prefix
   that does not appear in the variable names stored internally.  */

static struct internalvar *internalvars;

/* Look up an internal variable with name NAME.  NAME should not
   normally include a dollar sign.

   If the specified internal variable does not exist,
   one is created, with a void value.  */

struct internalvar *
lookup_internalvar (name)
     char *name;
{
  register struct internalvar *var;

  for (var = internalvars; var; var = var->next)
    if (!strcmp (var->name, name))
      return var;

  var = (struct internalvar *) xmalloc (sizeof (struct internalvar));
  var->name = concat (name, "", "");
  var->value = allocate_value (builtin_type_void);
  release_value (var->value);
  var->next = internalvars;
  internalvars = var;
  return var;
}

value
value_of_internalvar (var)
     struct internalvar *var;
{
  register value val = value_copy (var->value);
  VALUE_LVAL (val) = lval_internalvar;
  VALUE_INTERNALVAR (val) = var;
  return val;
}

void
set_internalvar_component (var, offset, bitpos, bitsize, newval)
     struct internalvar *var;
     int offset, bitpos, bitsize;
     value newval;
{
  register char *addr = VALUE_CONTENTS (var->value) + offset;
  if (bitsize)
    modify_field (addr, value_as_long (newval),
		  bitpos, bitsize);
  else
    bcopy (VALUE_CONTENTS (newval), addr,
	   TYPE_LENGTH (VALUE_TYPE (newval)));
}

void
set_internalvar (var, val)
     struct internalvar *var;
     value val;
{
  free (var->value);
  var->value = value_copy (val);
  release_value (var->value);
}

char *
internalvar_name (var)
     struct internalvar *var;
{
  return var->name;
}

/* Free all internalvars.  Done when new symtabs are loaded,
   because that makes the values invalid.  */

void
clear_internalvars ()
{
  register struct internalvar *var;

  while (internalvars)
    {
      var = internalvars;
      internalvars = var->next;
      free (var->name);
      free (var->value);
      free (var);
    }
}

static void
convenience_info ()
{
  register struct internalvar *var;

  if (internalvars)
    printf ("Debugger convenience variables:\n\n");
  else
    printf ("No debugger convenience variables now defined.\n\
Convenience variables have names starting with \"$\";\n\
use \"set\" as in \"set $foo = 5\" to define them.\n");

  for (var = internalvars; var; var = var->next)
    {
      printf ("$%s: ", var->name);
      value_print (var->value, stdout, 0);
      printf ("\n");
    }
}

/* Extract a value as a C number (either long or double).
   Knows how to convert fixed values to double, or
   floating values to long.
   Does not deallocate the value.  */

long
value_as_long (val)
     register value val;
{
  return unpack_long (VALUE_TYPE (val), VALUE_CONTENTS (val));
}

double
value_as_double (val)
     register value val;
{
  return unpack_double (VALUE_TYPE (val), VALUE_CONTENTS (val));
}

/* Unpack raw data (copied from debugee) at VALADDR
   as a long, or as a double, assuming the raw data is described
   by type TYPE.  Knows how to convert different sizes of values
   and can convert between fixed and floating point.  */

long
unpack_long (type, valaddr)
     struct type *type;
     char *valaddr;
{
  register enum type_code code = TYPE_CODE (type);
  register int len = TYPE_LENGTH (type);
  register int nosign = TYPE_UNSIGNED (type);

  if (code == TYPE_CODE_ENUM)
    code = TYPE_CODE_INT;
  if (code == TYPE_CODE_FLT)
    {
      if (len == sizeof (float))
	return * (float *) valaddr;

      if (len == sizeof (double))
	return * (double *) valaddr;
    }
  else if (code == TYPE_CODE_INT && nosign)
    {
      if (len == sizeof (char))
	return * (unsigned char *) valaddr;

      if (len == sizeof (short))
	return * (unsigned short *) valaddr;

      if (len == sizeof (int))
	return * (unsigned int *) valaddr;

      if (len == sizeof (long))
	return * (unsigned long *) valaddr;
    }
  else if (code == TYPE_CODE_INT)
    {
      if (len == sizeof (char))
	return * (char *) valaddr;

      if (len == sizeof (short))
	return * (short *) valaddr;

      if (len == sizeof (int))
	return * (int *) valaddr;

      if (len == sizeof (long))
	return * (long *) valaddr;
    }
  else if (code == TYPE_CODE_PTR)
    {
      if (len == sizeof (char *))
	return (CORE_ADDR) * (char **) valaddr;
    }

  error ("Value not integer or pointer.");
}

double
unpack_double (type, valaddr)
     struct type *type;
     char *valaddr;
{
  register enum type_code code = TYPE_CODE (type);
  register int len = TYPE_LENGTH (type);
  register int nosign = TYPE_UNSIGNED (type);

  if (code == TYPE_CODE_FLT)
    {
      if (INVALID_FLOAT (valaddr, len))
	error ("Invalid floating value found in program.");

      if (len == sizeof (float))
	return * (float *) valaddr;

      if (len == sizeof (double))
	{
	  /* Some machines require doubleword alignment for doubles.
	     This code works on them, and on other machines.  */
	  double temp;
	  bcopy ((char *) valaddr, (char *) &temp, sizeof (double));
	  return temp;
	}
    }
  else if (code == TYPE_CODE_INT && nosign)
    {
      if (len == sizeof (char))
	return * (unsigned char *) valaddr;

      if (len == sizeof (short))
	return * (unsigned short *) valaddr;

      if (len == sizeof (int))
	return * (unsigned int *) valaddr;

      if (len == sizeof (long))
	return * (unsigned long *) valaddr;
    }
  else if (code == TYPE_CODE_INT)
    {
      if (len == sizeof (char))
	return * (char *) valaddr;

      if (len == sizeof (short))
	return * (short *) valaddr;

      if (len == sizeof (int))
	return * (int *) valaddr;

      if (len == sizeof (long))
	return * (long *) valaddr;
    }

  error ("Value not floating number.");
}

/* Given a value ARG1 of a struct or union type,
   extract and return the value of one of its fields.
   FIELDNO says which field.  */

value
value_field (arg1, fieldno)
     register value arg1;
     register int fieldno;
{
  register value v;
  register struct type *type = TYPE_FIELD_TYPE (VALUE_TYPE (arg1), fieldno);
  register int offset;

  /* Handle packed fields */

  offset = TYPE_FIELD_BITPOS (VALUE_TYPE (arg1), fieldno) / 8;
  if (TYPE_FIELD_BITSIZE (VALUE_TYPE (arg1), fieldno))
    {
      v = value_from_long (type,
			   unpack_field_as_long (VALUE_TYPE (arg1),
						 VALUE_CONTENTS (arg1),
						 fieldno));
      VALUE_BITPOS (v) = TYPE_FIELD_BITPOS (VALUE_TYPE (arg1), fieldno) % 8;
      VALUE_BITSIZE (v) = TYPE_FIELD_BITSIZE (VALUE_TYPE (arg1), fieldno);
    }
  else
    {
      v = allocate_value (type);
      bcopy (VALUE_CONTENTS (arg1) + offset,
	     VALUE_CONTENTS (v),
	     TYPE_LENGTH (type));
    }
  VALUE_LVAL (v) = VALUE_LVAL (arg1);
  if (VALUE_LVAL (arg1) == lval_internalvar)
    VALUE_LVAL (v) = lval_internalvar_component;
  VALUE_ADDRESS (v) = VALUE_ADDRESS (arg1);
  VALUE_OFFSET (v) = offset + VALUE_OFFSET (arg1);
  return v;
}

long
unpack_field_as_long (type, valaddr, fieldno)
     struct type *type;
     char *valaddr;
     int fieldno;
{
  long val;
  int bitpos = TYPE_FIELD_BITPOS (type, fieldno);
  int bitsize = TYPE_FIELD_BITSIZE (type, fieldno);
  union { int i; char c; } test;

  bcopy (valaddr + bitpos / 8, &val, sizeof val);

  /* Extracting bits depends on endianness of the machine.  */
  test.i = 1;
  if (test.c == 1)
    /* Little-endian.  */
    val = val >> (bitpos % 8);
  else
    val = val >> (sizeof val * 8 - bitpos % 8 - bitsize);

  val &= (1 << bitsize) - 1;
  return val;
}

modify_field (addr, fieldval, bitpos, bitsize)
     char *addr;
     int fieldval;
     int bitpos, bitsize;
{
  long oword;
  union { int i; char c; } test;

  bcopy (addr, &oword, sizeof oword);

  /* Shifting for bit field depends on endianness of the machine.  */
  test.c = 1;
  if (test.i != 1)
    /* not little-endian: assume big-endian.  */
    bitpos = sizeof oword * 8 - bitpos - bitsize;

  oword &= ~(((1 << bitsize) - 1) << bitpos);
  oword |= fieldval << bitpos;
  bcopy (&oword, addr, sizeof oword);
}

/* Convert C numbers into newly allocated values */

value
value_from_long (type, num)
     struct type *type;
     register long num;
{
  register value val = allocate_value (type);
  register enum type_code code = TYPE_CODE (type);
  register int len = TYPE_LENGTH (type);

  if (code == TYPE_CODE_INT || code == TYPE_CODE_ENUM)
    {
      if (len == sizeof (char))
	* (char *) VALUE_CONTENTS (val) = num;
      else if (len == sizeof (short))
	* (short *) VALUE_CONTENTS (val) = num;
      else if (len == sizeof (int))
	* (int *) VALUE_CONTENTS (val) = num;
      else if (len == sizeof (long))
	* (long *) VALUE_CONTENTS (val) = num;
      else
	error ("Integer type encountered with unexpected data length.");
    }
  else
    error ("Unexpected type encountered for integer constant.");

  return val;
}

value
value_from_double (type, num)
     struct type *type;
     double num;
{
  register value val = allocate_value (type);
  register enum type_code code = TYPE_CODE (type);
  register int len = TYPE_LENGTH (type);

  if (code == TYPE_CODE_FLT)
    {
      if (len == sizeof (float))
	* (float *) VALUE_CONTENTS (val) = num;
      else if (len == sizeof (double))
	* (double *) VALUE_CONTENTS (val) = num;
      else
	error ("Floating type encountered with unexpected data length.");
    }
  else
    error ("Unexpected type encountered for floating constant.");

  return val;
}

/* Deal with the value that is "about to be returned".  */

/* Return the value that a function returning now
   would be returning to its caller, assuming its type is VALTYPE.
   RETBUF is where we look for what ought to be the contents
   of the registers (in raw form).  This is because it is often
   desirable to restore old values to those registers
   after saving the contents of interest, and then call
   this function using the saved values.  */

value
value_being_returned (valtype, retbuf)
     register struct type *valtype;
     char retbuf[REGISTER_BYTES];
{
  register value val;

  if (TYPE_CODE (valtype) == TYPE_CODE_STRUCT
      || TYPE_CODE (valtype) == TYPE_CODE_UNION)
    return value_at (valtype, EXTRACT_STRUCT_VALUE_ADDRESS (retbuf));

  val = allocate_value (valtype);
  EXTRACT_RETURN_VALUE (valtype, retbuf, VALUE_CONTENTS (val));

  return val;
}

/* Store VAL so it will be returned if a function returns now.
   Does not verify that VAL's type matches what the current
   function wants to return.  */

void
set_return_value (val)
     value val;
{
  register enum type_code code = TYPE_CODE (VALUE_TYPE (val));
  char regbuf[REGISTER_BYTES];
  double dbuf;
  long lbuf;

  if (code == TYPE_CODE_STRUCT
      || code == TYPE_CODE_UNION)
    error ("Specifying a struct or union return value is not supported.");

  if (code == TYPE_CODE_FLT)
    {
      dbuf = value_as_double (val);

      STORE_RETURN_VALUE (VALUE_TYPE (val), &dbuf);
    }
  else
    {
      lbuf = value_as_long (val);
      STORE_RETURN_VALUE (VALUE_TYPE (val), &lbuf);
    }
}

static
initialize ()
{
  add_info ("convenience", convenience_info,
	    "Debugger convenience (\"$foo\") variables.\n\
These variables are created when you assign them values;\n\
thus, \"print $foo=1\" gives \"$foo\" the value 1.  Values may be any type.\n\n\
A few convenience variables are given values automatically GDB:\n\
\"$_\"holds the last address examined with \"x\" or \"info lines\",\n\
\"$__\" holds the contents of the last address examined with \"x\".");

  add_info ("history", history_info,
	    "Elements of value history (around item number IDX, or last ten).");
}

END_FILE
