/* Low level packing and unpacking of values for GDB.
   Copyright (C) 1986, 1987, 1989 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "param.h"
#include "symtab.h"
#include "value.h"
#include "gdbcore.h"
#include "frame.h"
#include "command.h"

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

  check_stub_type (type);

  val = (value) xmalloc (sizeof (struct value) + TYPE_LENGTH (type));
  VALUE_NEXT (val) = all_values;
  all_values = val;
  VALUE_TYPE (val) = type;
  VALUE_LVAL (val) = not_lval;
  VALUE_ADDRESS (val) = 0;
  VALUE_FRAME (val) = 0;
  VALUE_OFFSET (val) = 0;
  VALUE_BITPOS (val) = 0;
  VALUE_BITSIZE (val) = 0;
  VALUE_REPEATED (val) = 0;
  VALUE_REPETITIONS (val) = 0;
  VALUE_REGNO (val) = -1;
  VALUE_LAZY (val) = 0;
  VALUE_OPTIMIZED_OUT (val) = 0;
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
  VALUE_FRAME (val) = 0;
  VALUE_OFFSET (val) = 0;
  VALUE_BITPOS (val) = 0;
  VALUE_BITSIZE (val) = 0;
  VALUE_REPEATED (val) = 1;
  VALUE_REPETITIONS (val) = count;
  VALUE_REGNO (val) = -1;
  VALUE_LAZY (val) = 0;
  VALUE_OPTIMIZED_OUT (val) = 0;
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
      value_free (val);
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
  VALUE_LAZY (val) = VALUE_LAZY (arg);
  if (!VALUE_LAZY (val))
    {
      bcopy (VALUE_CONTENTS_RAW (arg), VALUE_CONTENTS_RAW (val),
	     TYPE_LENGTH (VALUE_TYPE (arg))
	     * (VALUE_REPEATED (arg) ? VALUE_REPETITIONS (arg) : 1));
    }
  return val;
}

/* Access to the value history.  */

/* Record a new value in the value history.
   Returns the absolute history index of the entry.
   Result of -1 indicates the value was not saved; otherwise it is the
   value history index of this new item.  */

int
record_latest_value (val)
     value val;
{
  int i;

  /* Check error now if about to store an invalid float.  We return -1
     to the caller, but allow them to continue, e.g. to print it as "Nan". */
  if (TYPE_CODE (VALUE_TYPE (val)) == TYPE_CODE_FLT) {
    (void) unpack_double (VALUE_TYPE (val), VALUE_CONTENTS (val), &i);
    if (i) return -1;		/* Indicate value not saved in history */
  }

  /* Here we treat value_history_count as origin-zero
     and applying to the value being stored now.  */

  i = value_history_count % VALUE_HISTORY_CHUNK;
  if (i == 0)
    {
      register struct value_history_chunk *new
	= (struct value_history_chunk *)
	  xmalloc (sizeof (struct value_history_chunk));
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
value_history_info (num_exp, from_tty)
     char *num_exp;
     int from_tty;
{
  register int i;
  register value val;
  static int num = 1;

  if (num_exp)
    {
      if (num_exp[0] == '+' && num_exp[1] == '\0')
	/* "info history +" should print from the stored position.  */
	;
      else
	/* "info history <exp>" should print around value number <exp>.  */
	num = parse_and_eval_address (num_exp) - 5;
    }
  else
    {
      /* "info history" means print the last 10 values.  */
      num = value_history_count - 9;
    }

  if (num <= 0)
    num = 1;

  for (i = num; i < num + 10 && i <= value_history_count; i++)
    {
      val = access_value_history (i);
      printf_filtered ("$%d = ", i);
      value_print (val, stdout, 0, Val_pretty_default);
      printf_filtered ("\n");
    }

  /* The next "info history +" should start after what we just printed.  */
  num += 10;

  /* Hitting just return after this command should do the same thing as
     "info history +".  If num_exp is null, this is unnecessary, since
     "info history +" is not useful after "info history".  */
  if (from_tty && num_exp)
    {
      num_exp[0] = '+';
      num_exp[1] = '\0';
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
  register value val;

#ifdef IS_TRAPPED_INTERNALVAR
  if (IS_TRAPPED_INTERNALVAR (var->name))
    return VALUE_OF_TRAPPED_INTERNALVAR (var);
#endif 

  val = value_copy (var->value);
  if (VALUE_LAZY (val))
    value_fetch_lazy (val);
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

#ifdef IS_TRAPPED_INTERNALVAR
  if (IS_TRAPPED_INTERNALVAR (var->name))
    SET_TRAPPED_INTERNALVAR (var, newval, bitpos, bitsize, offset);
#endif

  if (bitsize)
    modify_field (addr, (int) value_as_long (newval),
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
#ifdef IS_TRAPPED_INTERNALVAR
  if (IS_TRAPPED_INTERNALVAR (var->name))
    SET_TRAPPED_INTERNALVAR (var, val, 0, 0, 0);
#endif

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
  int varseen = 0;

  for (var = internalvars; var; var = var->next)
    {
#ifdef IS_TRAPPED_INTERNALVAR
      if (IS_TRAPPED_INTERNALVAR (var->name))
	continue;
#endif
      if (!varseen)
	{
#if 0
	  /* Useless noise.  */
	  printf ("Debugger convenience variables:\n\n");
#endif
	  varseen = 1;
	}
      printf ("$%s = ", var->name);
      value_print (var->value, stdout, 0, Val_pretty_default);
      printf ("\n");
    }
  if (!varseen)
    printf ("No debugger convenience variables now defined.\n\
Convenience variables have names starting with \"$\";\n\
use \"set\" as in \"set $foo = 5\" to define them.\n");
}

/* Extract a value as a C number (either long or double).
   Knows how to convert fixed values to double, or
   floating values to long.
   Does not deallocate the value.  */

LONGEST
value_as_long (val)
     register value val;
{
  /* This coerces arrays and functions, which is necessary (e.g.
     in disassemble_command).  It also dereferences references, which
     I suspect is the most logical thing to do.  */
  if (TYPE_CODE (VALUE_TYPE (val)) != TYPE_CODE_ENUM)
    COERCE_ARRAY (val);
  return unpack_long (VALUE_TYPE (val), VALUE_CONTENTS (val));
}

double
value_as_double (val)
     register value val;
{
  double foo;
  int inv;
  
  foo = unpack_double (VALUE_TYPE (val), VALUE_CONTENTS (val), &inv);
  if (inv)
    error ("Invalid floating value found in program.");
  return foo;
}

/* Unpack raw data (copied from debugee, target byte order) at VALADDR
   as a long, or as a double, assuming the raw data is described
   by type TYPE.  Knows how to convert different sizes of values
   and can convert between fixed and floating point.  We don't assume
   any alignment for the raw data.  Return value is in host byte order.

   If you want functions and arrays to be coerced to pointers, and
   references to be dereferenced, call value_as_long() instead.

   C++: It is assumed that the front-end has taken care of
   all matters concerning pointers to members.  A pointer
   to member which reaches here is considered to be equivalent
   to an INT (or some size).  After all, it is only an offset.  */

LONGEST
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
	{
	  float retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}

      if (len == sizeof (double))
	{
	  double retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}
      else
	{
	  error ("Unexpected type of floating point number.");
	}
    }
  else if (code == TYPE_CODE_INT && nosign)
    {
      if (len == sizeof (char))
	{
	  unsigned char retval = * (unsigned char *) valaddr;
	  /* SWAP_TARGET_AND_HOST (&retval, sizeof (unsigned char)); */
	  return retval;
	}

      if (len == sizeof (short))
	{
	  unsigned short retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}

      if (len == sizeof (int))
	{
	  unsigned int retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}

      if (len == sizeof (long))
	{
	  unsigned long retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}
#ifdef LONG_LONG
      if (len == sizeof (long long))
	{
	  unsigned long long retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}
#endif
      else
	{
	  error ("That operation is not possible on an integer of that size.");
	}
    }
  else if (code == TYPE_CODE_INT)
    {
      if (len == sizeof (char))
	{
	  char retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}

      if (len == sizeof (short))
	{
	  short retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}

      if (len == sizeof (int))
	{
	  int retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}

      if (len == sizeof (long))
	{
	  long retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}

#ifdef LONG_LONG
      if (len == sizeof (long long))
	{
	  long long retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}
#endif
      else
	{
	  error ("That operation is not possible on an integer of that size.");
	}
    }
  else if (code == TYPE_CODE_PTR
	   || code == TYPE_CODE_REF)
    {
      if (len == sizeof (char *))
	{
	  CORE_ADDR retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}
    }
  else if (code == TYPE_CODE_MEMBER)
    error ("not implemented: member types in unpack_long");

  error ("Value not integer or pointer.");
  return 0; 	/* For lint -- never reached */
}

/* Return a double value from the specified type and address.
   INVP points to an int which is set to 0 for valid value,
   1 for invalid value (bad float format).  In either case,
   the returned double is OK to use.  Argument is in target
   format, result is in host format.  */

double
unpack_double (type, valaddr, invp)
     struct type *type;
     char *valaddr;
     int *invp;
{
  register enum type_code code = TYPE_CODE (type);
  register int len = TYPE_LENGTH (type);
  register int nosign = TYPE_UNSIGNED (type);

  *invp = 0;			/* Assume valid.   */
  if (code == TYPE_CODE_FLT)
    {
      if (INVALID_FLOAT (valaddr, len))
	{
	  *invp = 1;
	  return 1.234567891011121314;
	}

      if (len == sizeof (float))
	{
	  float retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}

      if (len == sizeof (double))
	{
	  double retval;
	  bcopy (valaddr, &retval, sizeof (retval));
	  SWAP_TARGET_AND_HOST (&retval, sizeof (retval));
	  return retval;
	}
      else
	{
	  error ("Unexpected type of floating point number.");
	}
    }
  else if (nosign) {
   /* Unsigned -- be sure we compensate for signed LONGEST.  */
#ifdef LONG_LONG
   return (unsigned long long) unpack_long (type, valaddr);
#else
   return (unsigned long     ) unpack_long (type, valaddr);
#endif
  } else {
    /* Signed -- we are OK with unpack_long.  */
    return unpack_long (type, valaddr);
  }
}

/* Given a value ARG1 (offset by OFFSET bytes)
   of a struct or union type ARG_TYPE,
   extract and return the value of one of its fields.
   FIELDNO says which field.

   For C++, must also be able to return values from static fields */

value
value_primitive_field (arg1, offset, fieldno, arg_type)
     register value arg1;
     int offset;
     register int fieldno;
     register struct type *arg_type;
{
  register value v;
  register struct type *type;

  check_stub_type (arg_type);
  type = TYPE_FIELD_TYPE (arg_type, fieldno);

  /* Handle packed fields */

  offset += TYPE_FIELD_BITPOS (arg_type, fieldno) / 8;
  if (TYPE_FIELD_BITSIZE (arg_type, fieldno))
    {
      v = value_from_long (type,
			   unpack_field_as_long (arg_type,
						 VALUE_CONTENTS (arg1),
						 fieldno));
      VALUE_BITPOS (v) = TYPE_FIELD_BITPOS (arg_type, fieldno) % 8;
      VALUE_BITSIZE (v) = TYPE_FIELD_BITSIZE (arg_type, fieldno);
    }
  else
    {
      v = allocate_value (type);
      if (VALUE_LAZY (arg1))
	VALUE_LAZY (v) = 1;
      else
	bcopy (VALUE_CONTENTS_RAW (arg1) + offset,
	       VALUE_CONTENTS_RAW (v),
	       TYPE_LENGTH (type));
    }
  VALUE_LVAL (v) = VALUE_LVAL (arg1);
  if (VALUE_LVAL (arg1) == lval_internalvar)
    VALUE_LVAL (v) = lval_internalvar_component;
  VALUE_ADDRESS (v) = VALUE_ADDRESS (arg1);
  VALUE_OFFSET (v) = offset + VALUE_OFFSET (arg1);
  return v;
}

/* Given a value ARG1 of a struct or union type,
   extract and return the value of one of its fields.
   FIELDNO says which field.

   For C++, must also be able to return values from static fields */

value
value_field (arg1, fieldno)
     register value arg1;
     register int fieldno;
{
  return value_primitive_field (arg1, 0, fieldno, VALUE_TYPE (arg1));
}

value
value_fn_field (arg1, fieldno, subfieldno)
     register value arg1;
     register int fieldno;
     int subfieldno;
{
  register value v;
  struct fn_field *f = TYPE_FN_FIELDLIST1 (VALUE_TYPE (arg1), fieldno);
  register struct type *type = TYPE_FN_FIELD_TYPE (f, subfieldno);
  struct symbol *sym;

  sym = lookup_symbol (TYPE_FN_FIELD_PHYSNAME (f, subfieldno),
		       0, VAR_NAMESPACE, 0, NULL);
  if (! sym) error ("Internal error: could not find physical method named %s",
		    TYPE_FN_FIELD_PHYSNAME (f, subfieldno));
  
  v = allocate_value (type);
  VALUE_ADDRESS (v) = BLOCK_START (SYMBOL_BLOCK_VALUE (sym));
  VALUE_TYPE (v) = type;
  return v;
}

/* Return a virtual function as a value.
   ARG1 is the object which provides the virtual function
   table pointer.  ARG1 is side-effected in calling this function.
   F is the list of member functions which contains the desired virtual
   function.
   J is an index into F which provides the desired virtual function.  */
value
value_virtual_fn_field (arg1, f, j)
     value arg1;
     struct fn_field *f;
     int j;
{
  /* First, get the virtual function table pointer.  That comes
     with a strange type, so cast it to type `pointer to long' (which
     should serve just fine as a function type).  Then, index into
     the table, and convert final value to appropriate function type.  */
  value entry, vfn, vtbl;
  value vi = value_from_long (builtin_type_int, 
			      (LONGEST) TYPE_FN_FIELD_VOFFSET (f, j));
  struct type *context = lookup_pointer_type (TYPE_FN_FIELD_FCONTEXT (f, j));
  if (TYPE_TARGET_TYPE (context) != VALUE_TYPE (arg1))
    arg1 = value_ind (value_cast (context, value_addr (arg1)));

  context = VALUE_TYPE (arg1);

  /* This type may have been defined before its virtual function table
     was.  If so, fill in the virtual function table entry for the
     type now.  */
  if (TYPE_VPTR_FIELDNO (context) < 0)
    TYPE_VPTR_FIELDNO (context)
      = fill_in_vptr_fieldno (context);

  /* The virtual function table is now an array of structures
     which have the form { int16 offset, delta; void *pfn; }.  */
  vtbl = value_ind (value_field (arg1, TYPE_VPTR_FIELDNO (context)));

  /* Index into the virtual function table.  This is hard-coded because
     looking up a field is not cheap, and it may be important to save
     time, e.g. if the user has set a conditional breakpoint calling
     a virtual function.  */
  entry = value_subscript (vtbl, vi);

  /* Move the `this' pointer according to the virtual function table.  */
  VALUE_OFFSET (arg1) += value_as_long (value_field (entry, 0));
  if (! VALUE_LAZY (arg1))
    {
      VALUE_LAZY (arg1) = 1;
      value_fetch_lazy (arg1);
    }

  vfn = value_field (entry, 2);
  /* Reinstantiate the function pointer with the correct type.  */
  VALUE_TYPE (vfn) = lookup_pointer_type (TYPE_FN_FIELD_TYPE (f, j));

  return vfn;
}

/* The value of a static class member does not depend
   on its instance, only on its type.  If FIELDNO >= 0,
   then fieldno is a valid field number and is used directly.
   Otherwise, FIELDNAME is the name of the field we are
   searching for.  If it is not a static field name, an
   error is signaled.  TYPE is the type in which we look for the
   static field member.  */
value
value_static_field (type, fieldname, fieldno)
     register struct type *type;
     char *fieldname;
     register int fieldno;
{
  register value v;
  struct symbol *sym;
  char *phys_name;

  if (fieldno < 0)
    {
      register struct type *t = type;
      /* Look for static field.  */
      while (t)
	{
	  int i;
	  for (i = TYPE_NFIELDS (t) - 1; i >= TYPE_N_BASECLASSES (t); i--)
	    if (! strcmp (TYPE_FIELD_NAME (t, i), fieldname))
	      {
		if (TYPE_FIELD_STATIC (t, i))
		  {
		    fieldno = i;
		    goto found;
		  }
		else
		  error ("field `%s' is not static");
	      }
	  /* FIXME: this does not recursively check multiple baseclasses.  */
	  t = TYPE_N_BASECLASSES (t) ? TYPE_BASECLASS (t, 0) : 0;
	}

      t = type;

      if (destructor_name_p (fieldname, t))
	error ("Cannot get value of destructor");

      while (t)
	{
	  int i;

	  for (i = TYPE_NFN_FIELDS (t) - 1; i >= 0; i--)
	    {
	      if (! strcmp (TYPE_FN_FIELDLIST_NAME (t, i), fieldname))
		{
		  error ("Cannot get value of method \"%s\"", fieldname);
		}
	    }
	  t = TYPE_N_BASECLASSES (t) ? TYPE_BASECLASS (t, 0) : 0;
	}
      error("there is no field named %s", fieldname);
    }

 found:
  phys_name = TYPE_FIELD_STATIC_PHYSNAME (type, fieldno);
  sym = lookup_symbol (phys_name, 0, VAR_NAMESPACE, 0, NULL);
  if (! sym) error ("Internal error: could not find physical static variable named %s", phys_name);

  type = TYPE_FIELD_TYPE (type, fieldno);
  v = value_at (type, (CORE_ADDR)SYMBOL_BLOCK_VALUE (sym));
  return v;
}

/* Compute the address of the baseclass which is
   the INDEXth baseclass of TYPE.  The TYPE base
   of the object is at VALADDR.  */

char *
baseclass_addr (type, index, valaddr, valuep)
     struct type *type;
     int index;
     char *valaddr;
     value *valuep;
{
  struct type *basetype = TYPE_BASECLASS (type, index);

  if (BASETYPE_VIA_VIRTUAL (type, index))
    {
      /* Must hunt for the pointer to this virtual baseclass.  */
      register int i, len = TYPE_NFIELDS (type);
      register int n_baseclasses = TYPE_N_BASECLASSES (type);
      char *vbase_name, *type_name = type_name_no_tag (basetype);

      if (TYPE_MAIN_VARIANT (basetype))
	basetype = TYPE_MAIN_VARIANT (basetype);

      vbase_name = (char *)alloca (strlen (type_name) + 8);
      sprintf (vbase_name, "_vb$%s", type_name);
      /* First look for the virtual baseclass pointer
	 in the fields.  */
      for (i = n_baseclasses; i < len; i++)
	{
	  if (! strcmp (vbase_name, TYPE_FIELD_NAME (type, i)))
	    {
	      value v = value_at (basetype,
				  unpack_long (TYPE_FIELD_TYPE (type, i),
					       valaddr + (TYPE_FIELD_BITPOS (type, i) / 8)));
	      if (valuep)
		*valuep = v;
	      return (char *) VALUE_CONTENTS (v);
	    }
	}
      /* Not in the fields, so try looking through the baseclasses.  */
      for (i = index+1; i < n_baseclasses; i++)
	{
	  char *baddr;

	  baddr = baseclass_addr (type, i, valaddr, valuep);
	  if (baddr)
	    return baddr;
	}
      /* Not found.  */
      if (valuep)
	*valuep = 0;
      return 0;
    }

  /* Baseclass is easily computed.  */
  if (valuep)
    *valuep = 0;
  return valaddr + TYPE_BASECLASS_BITPOS (type, index) / 8;
}

/* Ugly hack to convert method stubs into method types.

   He ain't kiddin'.  This demangles the name of the method into a string
   including argument types, parses out each argument type, generates
   a string casting a zero to that type, evaluates the string, and stuffs
   the resulting type into an argtype vector!!!  Then it knows the type
   of the whole function (including argument types for overloading),
   which info used to be in the stab's but was removed to hack back
   the space required for them.  */
void
check_stub_method (type, i, j)
     struct type *type;
     int i, j;
{
  extern char *gdb_mangle_typename (), *strchr ();
  struct fn_field *f = TYPE_FN_FIELDLIST1 (type, i);
  char *inner_name = gdb_mangle_typename (type);
  char *mangled_name
    = (char *)xmalloc (strlen (TYPE_FN_FIELDLIST_NAME (type, i))
		       + strlen (inner_name)
		       + strlen (TYPE_FN_FIELD_PHYSNAME (f, j))
		       + 1);
  char *demangled_name, *cplus_demangle ();
  char *argtypetext, *p;
  int depth = 0, argcount = 1;
  struct type **argtypes;

  strcpy (mangled_name, TYPE_FN_FIELDLIST_NAME (type, i));
  strcat (mangled_name, inner_name);
  strcat (mangled_name, TYPE_FN_FIELD_PHYSNAME (f, j));
  demangled_name = cplus_demangle (mangled_name, 0);

  /* Now, read in the parameters that define this type.  */
  argtypetext = strchr (demangled_name, '(') + 1;
  p = argtypetext;
  while (*p)
    {
      if (*p == '(')
	depth += 1;
      else if (*p == ')')
	depth -= 1;
      else if (*p == ',' && depth == 0)
	argcount += 1;

      p += 1;
    }
  /* We need one more slot for the void [...] or NULL [end of arglist] */
  argtypes = (struct type **)xmalloc ((argcount+1) * sizeof (struct type *));
  p = argtypetext;
  argtypes[0] = lookup_pointer_type (type);
  argcount = 1;

  if (*p != ')')			/* () means no args, skip while */
    {
      while (*p)
	{
	  if (*p == '(')
	    depth += 1;
	  else if (*p == ')')
	    depth -= 1;

	  if (depth <= 0 && (*p == ',' || *p == ')'))
	    {
	      char *tmp = (char *)alloca (p - argtypetext + 4);
	      value val;
	      tmp[0] = '(';
	      bcopy (argtypetext, tmp+1, p - argtypetext);
	      tmp[p-argtypetext+1] = ')';
	      tmp[p-argtypetext+2] = '0';
	      tmp[p-argtypetext+3] = '\0';
	      val = parse_and_eval (tmp);
	      argtypes[argcount] = VALUE_TYPE (val);
	      argcount += 1;
	      argtypetext = p + 1;
	    }
	  p += 1;
	}
    }

  if (p[-2] != '.')			/* ... */
    argtypes[argcount] = builtin_type_void;	/* Ellist terminator */
  else
    argtypes[argcount] = NULL;		/* List terminator */

  free (demangled_name);
  smash_to_method_type (TYPE_FN_FIELD_TYPE (f, j), type,
			TYPE_TARGET_TYPE (TYPE_FN_FIELD_TYPE (f, j)),
			argtypes);
  TYPE_FN_FIELD_PHYSNAME (f, j) = mangled_name;
  TYPE_FLAGS (TYPE_FN_FIELD_TYPE (f, j)) &= ~TYPE_FLAG_STUB;
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

  bcopy (valaddr + bitpos / 8, &val, sizeof val);
  SWAP_TARGET_AND_HOST (&val, sizeof val);

  /* Extracting bits depends on endianness of the machine.  */
#ifdef BITS_BIG_ENDIAN
  val = val >> (sizeof val * 8 - bitpos % 8 - bitsize);
#else
  val = val >> (bitpos % 8);
#endif

  val &= (1 << bitsize) - 1;
  return val;
}

/* Modify the value of a bitfield.  ADDR points to a block of memory in
   target byte order; the bitfield starts in the byte pointed to.  FIELDVAL
   is the desired value of the field, in host byte order.  BITPOS and BITSIZE
   indicate which bits (in target bit order) comprise the bitfield.  */

void
modify_field (addr, fieldval, bitpos, bitsize)
     char *addr;
     int fieldval;
     int bitpos, bitsize;
{
  long oword;

  /* Reject values too big to fit in the field in question.
     Otherwise adjoining fields may be corrupted.  */
  if (fieldval & ~((1<<bitsize)-1))
    error ("Value %d does not fit in %d bits.", fieldval, bitsize);
  
  bcopy (addr, &oword, sizeof oword);
  SWAP_TARGET_AND_HOST (&oword, sizeof oword);		/* To host format */

  /* Shifting for bit field depends on endianness of the target machine.  */
#ifdef BITS_BIG_ENDIAN
  bitpos = sizeof (oword) * 8 - bitpos - bitsize;
#endif

  oword &= ~(((1 << bitsize) - 1) << bitpos);
  oword |= fieldval << bitpos;

  SWAP_TARGET_AND_HOST (&oword, sizeof oword);		/* To target format */
  bcopy (&oword, addr, sizeof oword);
}

/* Convert C numbers into newly allocated values */

value
value_from_long (type, num)
     struct type *type;
     register LONGEST num;
{
  register value val = allocate_value (type);
  register enum type_code code = TYPE_CODE (type);
  register int len = TYPE_LENGTH (type);

  if (code == TYPE_CODE_INT || code == TYPE_CODE_ENUM)
    {
      if (len == sizeof (char))
	* (char *) VALUE_CONTENTS_RAW (val) = num;
      else if (len == sizeof (short))
	* (short *) VALUE_CONTENTS_RAW (val) = num;
      else if (len == sizeof (int))
	* (int *) VALUE_CONTENTS_RAW (val) = num;
      else if (len == sizeof (long))
	* (long *) VALUE_CONTENTS_RAW (val) = num;
#ifdef LONG_LONG
      else if (len == sizeof (long long))
	* (long long *) VALUE_CONTENTS_RAW (val) = num;
#endif
      else
	error ("Integer type encountered with unexpected data length.");
    }
  else
    error ("Unexpected type encountered for integer constant.");

  /* num was in host byte order.  So now put the value's contents
     into target byte order.  */
  SWAP_TARGET_AND_HOST (VALUE_CONTENTS_RAW (val), len);

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
	* (float *) VALUE_CONTENTS_RAW (val) = num;
      else if (len == sizeof (double))
	* (double *) VALUE_CONTENTS_RAW (val) = num;
      else
	error ("Floating type encountered with unexpected data length.");
    }
  else
    error ("Unexpected type encountered for floating constant.");

  /* num was in host byte order.  So now put the value's contents
     into target byte order.  */
  SWAP_TARGET_AND_HOST (VALUE_CONTENTS_RAW (val), len);

  return val;
}

/* Deal with the value that is "about to be returned".  */

/* Return the value that a function returning now
   would be returning to its caller, assuming its type is VALTYPE.
   RETBUF is where we look for what ought to be the contents
   of the registers (in raw form).  This is because it is often
   desirable to restore old values to those registers
   after saving the contents of interest, and then call
   this function using the saved values.
   struct_return is non-zero when the function in question is
   using the structure return conventions on the machine in question;
   0 when it is using the value returning conventions (this often
   means returning pointer to where structure is vs. returning value). */

value
value_being_returned (valtype, retbuf, struct_return)
     register struct type *valtype;
     char retbuf[REGISTER_BYTES];
     int struct_return;
     /*ARGSUSED*/
{
  register value val;
  CORE_ADDR addr;

#if defined (EXTRACT_STRUCT_VALUE_ADDRESS)
  /* If this is not defined, just use EXTRACT_RETURN_VALUE instead.  */
  if (struct_return) {
    addr = EXTRACT_STRUCT_VALUE_ADDRESS (retbuf);
    if (!addr)
      error ("Function return value unknown");
    return value_at (valtype, addr);
  }
#endif

  val = allocate_value (valtype);
  EXTRACT_RETURN_VALUE (valtype, retbuf, VALUE_CONTENTS_RAW (val));

  return val;
}

/* Should we use EXTRACT_STRUCT_VALUE_ADDRESS instead of
   EXTRACT_RETURN_VALUE?  GCC_P is true if compiled with gcc
   and TYPE is the type (which is known to be struct, union or array).

   On most machines, the struct convention is used unless we are
   using gcc and the type is of a special size.  */
#if !defined (USE_STRUCT_CONVENTION)
#define USE_STRUCT_CONVENTION(gcc_p, type)\
  (!((gcc_p) && (TYPE_LENGTH (value_type) == 1                \
		 || TYPE_LENGTH (value_type) == 2             \
	         || TYPE_LENGTH (value_type) == 4             \
		 || TYPE_LENGTH (value_type) == 8             \
		 )                                            \
     ))
#endif

/* Return true if the function specified is using the structure returning
   convention on this machine to return arguments, or 0 if it is using
   the value returning convention.  FUNCTION is the value representing
   the function, FUNCADDR is the address of the function, and VALUE_TYPE
   is the type returned by the function.  GCC_P is nonzero if compiled
   with GCC.  */

int
using_struct_return (function, funcaddr, value_type, gcc_p)
     value function;
     CORE_ADDR funcaddr;
     struct type *value_type;
     int gcc_p;
     /*ARGSUSED*/
{
  register enum type_code code = TYPE_CODE (value_type);

  if (code == TYPE_CODE_ERROR)
    error ("Function return type unknown.");

  if (code == TYPE_CODE_STRUCT ||
      code == TYPE_CODE_UNION ||
      code == TYPE_CODE_ARRAY)
    return USE_STRUCT_CONVENTION (gcc_p, value_type);

  return 0;
}

/* Store VAL so it will be returned if a function returns now.
   Does not verify that VAL's type matches what the current
   function wants to return.  */

void
set_return_value (val)
     value val;
{
  register enum type_code code = TYPE_CODE (VALUE_TYPE (val));
  double dbuf;
  LONGEST lbuf;

  if (code == TYPE_CODE_ERROR)
    error ("Function return type unknown.");

  if (code == TYPE_CODE_STRUCT
      || code == TYPE_CODE_UNION)
    error ("Specifying a struct or union return value is not supported.");

  /* FIXME, this is bogus.  We don't know what the return conventions
     are, or how values should be promoted.... */
  if (code == TYPE_CODE_FLT)
    {
      dbuf = value_as_double (val);

      STORE_RETURN_VALUE (VALUE_TYPE (val), (char *)&dbuf);
    }
  else
    {
      lbuf = value_as_long (val);
      STORE_RETURN_VALUE (VALUE_TYPE (val), (char *)&lbuf);
    }
}

void
_initialize_values ()
{
  add_info ("convenience", convenience_info,
	    "Debugger convenience (\"$foo\") variables.\n\
These variables are created when you assign them values;\n\
thus, \"print $foo=1\" gives \"$foo\" the value 1.  Values may be any type.\n\n\
A few convenience variables are given values automatically:\n\
\"$_\"holds the last address examined with \"x\" or \"info lines\",\n\
\"$__\" holds the contents of the last address examined with \"x\".");

  add_info ("values", value_history_info,
	    "Elements of value history around item number IDX (or last ten).");
  add_info_alias ("history", "values", 0);
}
