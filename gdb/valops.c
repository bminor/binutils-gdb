/* Perform non-arithmetic operations on values, for GDB.
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

#include "defs.h"
#include "initialize.h"
#include "param.h"
#include "symtab.h"
#include "value.h"

START_FILE

/* Cast value ARG2 to type TYPE and return as a value.
   More general than a C cast: accepts any two types of the same length,
   and if ARG2 is an lvalue it can be cast into anything at all.  */

value
value_cast (type, arg2)
     struct type *type;
     register value arg2;
{
  register enum type_code code1;
  register enum type_code code2;
  register int scalar;

  /* Coerce arrays but not enums.  Enums will work as-is
     and coercing them would cause an infinite recursion.  */
  if (TYPE_CODE (VALUE_TYPE (arg2)) != TYPE_CODE_ENUM)
    COERCE_ARRAY (arg2);

  code1 = TYPE_CODE (type);
  code2 = TYPE_CODE (VALUE_TYPE (arg2));
  scalar = (code2 == TYPE_CODE_INT || code2 == TYPE_CODE_FLT
	    || code2 == TYPE_CODE_ENUM);

  if (code1 == TYPE_CODE_FLT && scalar)
    return value_from_double (type, value_as_double (arg2));
  else if ((code1 == TYPE_CODE_INT || code1 == TYPE_CODE_ENUM)
	   && (scalar || code2 == TYPE_CODE_PTR))
    return value_from_long (type, value_as_long (arg2));
  else if (TYPE_LENGTH (type) == TYPE_LENGTH (VALUE_TYPE (arg2)))
    {
      VALUE_TYPE (arg2) = type;
      return arg2;
    }
  else if (VALUE_LVAL (arg2) == lval_memory)
    return value_at (type, VALUE_ADDRESS (arg2) + VALUE_OFFSET (arg2));
  else
    error ("Invalid cast.");
}

/* Return the value with a specified type located at specified address.  */

value
value_at (type, addr)
     struct type *type;
     CORE_ADDR addr;
{
  register value val = allocate_value (type);

  read_memory (addr, VALUE_CONTENTS (val), TYPE_LENGTH (type));
  VALUE_LVAL (val) = lval_memory;
  VALUE_ADDRESS (val) = addr;

  return val;
}

/* Store the contents of FROMVAL into the location of TOVAL.
   Return a new value with the location of TOVAL and contents of FROMVAL.  */

value
value_assign (toval, fromval)
     register value toval, fromval;
{
  register struct type *type = VALUE_TYPE (toval);
  register value val;
  char raw_buffer[MAX_REGISTER_RAW_SIZE];
  char virtual_buffer[MAX_REGISTER_VIRTUAL_SIZE];
  int use_buffer = 0;

  COERCE_ARRAY (fromval);

  if (VALUE_LVAL (toval) != lval_internalvar)
    fromval = value_cast (type, fromval);

  /* If TOVAL is a special machine register requiring conversion
     of program values to a special raw format,
     convert FROMVAL's contents now, with result in `raw_buffer',
     and set USE_BUFFER to the number of bytes to write.  */
     
  if (VALUE_REGNO (toval) >= 0
      && REGISTER_CONVERTIBLE (VALUE_REGNO (toval)))
    {
      int regno = VALUE_REGNO (toval);
      if (VALUE_TYPE (fromval) != REGISTER_VIRTUAL_TYPE (regno))
	fromval = value_cast (REGISTER_VIRTUAL_TYPE (regno), fromval);
      bcopy (VALUE_CONTENTS (fromval), virtual_buffer,
	     REGISTER_VIRTUAL_SIZE (regno));
      REGISTER_CONVERT_TO_RAW (regno, virtual_buffer, raw_buffer);
      use_buffer = REGISTER_RAW_SIZE (regno);
    }

  switch (VALUE_LVAL (toval))
    {
    case lval_internalvar:
      set_internalvar (VALUE_INTERNALVAR (toval), fromval);
      break;

    case lval_internalvar_component:
      set_internalvar_component (VALUE_INTERNALVAR (toval),
				 VALUE_OFFSET (toval),
				 VALUE_BITPOS (toval),
				 VALUE_BITSIZE (toval),
				 fromval);
      break;

    case lval_memory:
      if (VALUE_BITSIZE (toval))
	{
	  int val;
	  read_memory (VALUE_ADDRESS (toval) + VALUE_OFFSET (toval),
		       &val, sizeof val);
	  modify_field (&val, value_as_long (fromval),
			VALUE_BITPOS (toval), VALUE_BITSIZE (toval));
	  write_memory (VALUE_ADDRESS (toval) + VALUE_OFFSET (toval),
			&val, sizeof val);
	}
      else if (use_buffer)
	write_memory (VALUE_ADDRESS (toval) + VALUE_OFFSET (toval),
		      raw_buffer, use_buffer);
      else
	write_memory (VALUE_ADDRESS (toval) + VALUE_OFFSET (toval),
		      VALUE_CONTENTS (fromval), TYPE_LENGTH (type));
      break;

    case lval_register:
      if (VALUE_BITSIZE (toval))
	{
	  int val;

	  read_register_bytes (VALUE_ADDRESS (toval) + VALUE_OFFSET (toval),
			       &val, sizeof val);
	  modify_field (&val, value_as_long (fromval),
			VALUE_BITPOS (toval), VALUE_BITSIZE (toval));
	  write_register_bytes (VALUE_ADDRESS (toval) + VALUE_OFFSET (toval),
				&val, sizeof val);
	}
      else if (use_buffer)
	write_register_bytes (VALUE_ADDRESS (toval) + VALUE_OFFSET (toval),
			      raw_buffer, use_buffer);
      else
	write_register_bytes (VALUE_ADDRESS (toval) + VALUE_OFFSET (toval),
			      VALUE_CONTENTS (fromval), TYPE_LENGTH (type));
      break;

    default:
      error ("Left side of = operation is not an lvalue.");
    }

  /* Return a value just like TOVAL except with the contents of FROMVAL.  */

  val = allocate_value (type);
  bcopy (toval, val, VALUE_CONTENTS (val) - (char *) val);
  bcopy (VALUE_CONTENTS (fromval), VALUE_CONTENTS (val), TYPE_LENGTH (type));

  return val;
}

/* Extend a value VAL to COUNT repetitions of its type.  */

value
value_repeat (arg1, count)
     value arg1;
     int count;
{
  register value val;

  if (VALUE_LVAL (arg1) != lval_memory)
    error ("Only values in memory can be extended with '@'.");
  if (count < 1)
    error ("Invalid number %d of repetitions.", count);

  val = allocate_repeat_value (VALUE_TYPE (arg1), count);

  read_memory (VALUE_ADDRESS (arg1) + VALUE_OFFSET (arg1),
	       VALUE_CONTENTS (val),
	       TYPE_LENGTH (VALUE_TYPE (val)) * count);
  VALUE_LVAL (val) = lval_memory;
  VALUE_ADDRESS (val) = VALUE_ADDRESS (arg1) + VALUE_OFFSET (arg1);

  return val;
}

value
value_of_variable (var)
     struct symbol *var;
{
  return read_var_value (var, (CORE_ADDR) 0);
}

/* Given a value which is an array, return a value which is
   a pointer to its first element.  */

value
value_coerce_array (arg1)
     value arg1;
{
  register struct type *type;
  register value val;

  if (VALUE_LVAL (arg1) != lval_memory)
    error ("Attempt to take address of value not located in memory.");

  /* Get type of elements.  */
  if (TYPE_CODE (VALUE_TYPE (arg1)) == TYPE_CODE_ARRAY)
    type = TYPE_TARGET_TYPE (VALUE_TYPE (arg1));
  else
    /* A phony array made by value_repeat.
       Its type is the type of the elements, not an array type.  */
    type = VALUE_TYPE (arg1);

  /* Get the type of the result.  */
  type = lookup_pointer_type (type);
  val = value_from_long (builtin_type_long,
			 VALUE_ADDRESS (arg1) + VALUE_OFFSET (arg1));
  VALUE_TYPE (val) = type;
  return val;
}

/* Return a pointer value for the object for which ARG1 is the contents.  */

value
value_addr (arg1)
     value arg1;
{
  register struct type *type;
  register value val, arg1_coerced;

  /* Taking the address of an array is really a no-op
     once the array is coerced to a pointer to its first element.  */
  arg1_coerced = arg1;
  COERCE_ARRAY (arg1_coerced);
  if (arg1 != arg1_coerced)
    return arg1_coerced;

  if (VALUE_LVAL (arg1) != lval_memory)
    error ("Attempt to take address of value not located in memory.");

  /* Get the type of the result.  */
  type = lookup_pointer_type (VALUE_TYPE (arg1));
  val = value_from_long (builtin_type_long,
			 VALUE_ADDRESS (arg1) + VALUE_OFFSET (arg1));
  VALUE_TYPE (val) = type;
  return val;
}

/* Given a value of a pointer type, apply the C unary * operator to it.  */

value
value_ind (arg1)
     value arg1;
{
  COERCE_ARRAY (arg1);

  /* Allow * on an integer so we can cast it to whatever we want.  */
  if (TYPE_CODE (VALUE_TYPE (arg1)) == TYPE_CODE_INT)
    return value_at (builtin_type_long, 
		     (CORE_ADDR) value_as_long (arg1));
  else if (TYPE_CODE (VALUE_TYPE (arg1)) == TYPE_CODE_PTR)
    return value_at (TYPE_TARGET_TYPE (VALUE_TYPE (arg1)),
		     (CORE_ADDR) value_as_long (arg1));
  error ("Attempt to take contents of a non-pointer value.");
}

/* Pushing small parts of stack frames.  */

/* Push one word (the size of object that a register holds).  */

CORE_ADDR
push_word (sp, buffer)
     CORE_ADDR sp;
     REGISTER_TYPE buffer;
{
  register int len = sizeof (REGISTER_TYPE);

#if 1 INNER_THAN 2
  sp -= len;
  write_memory (sp, &buffer, len);
#else /* stack grows upward */
  write_memory (sp, &buffer, len);
  sp += len;
#endif /* stack grows upward */

  return sp;
}

/* Push LEN bytes with data at BUFFER.  */

CORE_ADDR
push_bytes (sp, buffer, len)
     CORE_ADDR sp;
     char *buffer;
     int len;
{
#if 1 INNER_THAN 2
  sp -= len;
  write_memory (sp, buffer, len);
#else /* stack grows upward */
  write_memory (sp, buffer, len);
  sp += len;
#endif /* stack grows upward */

  return sp;
}

/* Push onto the stack the specified value VALUE.  */

CORE_ADDR
value_push (sp, arg)
     register CORE_ADDR sp;
     value arg;
{
  register int len = TYPE_LENGTH (VALUE_TYPE (arg));

#if 1 INNER_THAN 2
  sp -= len;
  write_memory (sp, VALUE_CONTENTS (arg), len);
#else /* stack grows upward */
  write_memory (sp, VALUE_CONTENTS (arg), len);
  sp += len;
#endif /* stack grows upward */

  return sp;
}

/* Perform the standard coercions that are specified
   for arguments to be passed to C functions.  */

value
value_arg_coerce (arg)
     value arg;
{
  register struct type *type;

  COERCE_ENUM (arg);

  type = VALUE_TYPE (arg);

  if (TYPE_CODE (type) == TYPE_CODE_INT
      && TYPE_LENGTH (type) < sizeof (int))
    return value_cast (builtin_type_int, arg);

  if (type == builtin_type_float)
    return value_cast (builtin_type_double, arg);

  return arg;
}

/* Push the value ARG, first coercing it as an argument
   to a C function.  */

CORE_ADDR
value_arg_push (sp, arg)
     register CORE_ADDR sp;
     value arg;
{
  return value_push (sp, value_arg_coerce (arg));
}

/* Perform a function call in the inferior.
   ARGS is a vector of values of arguments (NARGS of them).
   FUNCTION is a value, the function to be called.
   Returns a value representing what the function returned.
   May fail to return, if a breakpoint or signal is hit
   during the execution of the function.  */

value
call_function (function, nargs, args)
     value function;
     int nargs;
     value *args;
{
  register CORE_ADDR sp;
  register int i;
  CORE_ADDR start_sp;
  static REGISTER_TYPE dummy[] = CALL_DUMMY;
  REGISTER_TYPE dummy1[sizeof dummy / sizeof (REGISTER_TYPE)];
  CORE_ADDR old_sp;
  struct type *value_type;

  PUSH_DUMMY_FRAME;

  {
    register CORE_ADDR funaddr;
    register struct type *ftype = VALUE_TYPE (function);
    register enum type_code code = TYPE_CODE (ftype);

    /* Determine address to call.  */
    if (code == TYPE_CODE_FUNC)
      {
	funaddr = VALUE_ADDRESS (function);
	value_type = TYPE_TARGET_TYPE (ftype);
      }
    else if (code == TYPE_CODE_PTR)
      {
	funaddr = value_as_long (function);
	if (TYPE_CODE (TYPE_TARGET_TYPE (ftype))
	    == TYPE_CODE_FUNC)
	  value_type = TYPE_TARGET_TYPE (TYPE_TARGET_TYPE (ftype));
	else
	  value_type = builtin_type_int;
      }
    else if (code == TYPE_CODE_INT)
      {
	/* Handle the case of functions lacking debugging info.
	   Their values are characters since their addresses are char */
	if (TYPE_LENGTH (ftype) == 1)
	  funaddr = value_as_long (value_addr (function));
	else
	  /* Handle integer used as address of a function.  */
	  funaddr = value_as_long (function);

	value_type = builtin_type_int;
      }
    else
      error ("Invalid data type for function to be called.");

    /* Create a call sequence customized for this function
       and the number of arguments for it.  */
    bcopy (dummy, dummy1, sizeof dummy);
    FIX_CALL_DUMMY (dummy1, funaddr, nargs);
  }

  old_sp = sp = read_register (SP_REGNUM);

#if 1 INNER_THAN 2  /* Stack grows down */
  sp -= sizeof dummy;
  write_memory (sp, dummy1, sizeof dummy);
  start_sp = sp;
  for (i = nargs - 1; i >= 0; i--)
    sp = value_arg_push (sp, args[i]);
#else  /* Stack grows up */
  start_sp = sp;
  write_memory (sp, dummy1, sizeof dummy);
  sp += sizeof dummy;
  for (i = 0; i < nargs; i++)
    sp = value_arg_push (sp, args[i]);
#endif  /* Stack grows up */

  write_register (SP_REGNUM, sp);

  /* Figure out the value returned by the function.  */
  {
    char retbuf[REGISTER_BYTES];

    /* Execute the stack dummy routine, calling FUNCTION.
       When it is done, discard the empty frame
       after storing the contents of all regs into retbuf.  */
    run_stack_dummy (start_sp + CALL_DUMMY_START_OFFSET, retbuf);

    return value_being_returned (value_type, retbuf);
  }
}

/* Create a value for a string constant:
   Call the function malloc in the inferior to get space for it,
   then copy the data into that space
   and then return the address with type char *.
   PTR points to the string constant data; LEN is number of characters.  */

value
value_string (ptr, len)
     char *ptr;
     int len;
{
  register value val;
  register struct symbol *sym;
  value blocklen;
  register char *copy = (char *) alloca (len + 1);
  char *i = ptr;
  register char *o = copy, *ibeg = ptr;
  register int c;

  /* Copy the string into COPY, processing escapes.
     We could not conveniently process them in expread
     because the string there wants to be a substring of the input.  */

  while (i - ibeg < len)
    {
      c = *i++;
      if (c == '\\')
	{
	  c = parse_escape (&i);
	  if (c == -1)
	    continue;
	}
      *o++ = c;
    }
  *o = 0;

  /* Get the length of the string after escapes are processed.  */

  len = o - copy;

  /* Find the address of malloc in the inferior.  */

  sym = lookup_symbol ("malloc", 0, VAR_NAMESPACE);
  if (sym != 0)
    {
      if (SYMBOL_CLASS (sym) != LOC_BLOCK)
	error ("\"malloc\" exists in this program but is not a function.");
      val = value_of_variable (sym);
    }
  else
    {
      register int i;
      for (i = 0; i < misc_function_count; i++)
	if (!strcmp (misc_function_vector[i].name, "malloc"))
	  break;
      if (i < misc_function_count)
	val = value_from_long (builtin_type_long,
			       misc_function_vector[i].address);
      else
	error ("String constants require the program to have a function \"malloc\".");
    }

  blocklen = value_from_long (builtin_type_int, len + 1);
  val = call_function (val, 1, &blocklen);
  if (value_zerop (val))
    error ("No memory available for string constant.");
  write_memory (value_as_long (val), copy, len + 1);
  VALUE_TYPE (val) = lookup_pointer_type (builtin_type_char);
  return val;
}

/* Given ARG1, a value of type (pointer to a)* structure/union,
   extract the component named NAME from the ultimate target structure/union
   and return it as a value with its appropriate type.
   ERR is used in the error message if ARG1's type is wrong.  */
   
value
value_struct_elt (arg1, name, err)
     register value arg1;
     char *name;
     char *err;
{
  register struct type *t;
  register int i;

  COERCE_ARRAY (arg1);

  t = VALUE_TYPE (arg1);

  /* Follow pointers until we get to a non-pointer.  */

  while (TYPE_CODE (t) == TYPE_CODE_PTR)
    {
      arg1 = value_ind (arg1);
      COERCE_ARRAY (arg1);
      t = VALUE_TYPE (arg1);
    }

  if (TYPE_CODE (t) != TYPE_CODE_STRUCT
      &&
      TYPE_CODE (t) != TYPE_CODE_UNION)
    error ("Attempt to extract a component of a value that is not a %s.", err);

  for (i = TYPE_NFIELDS (t) - 1; i >= 0; i--)
    {
      if (!strcmp (TYPE_FIELD_NAME (t, i), name))
	break;
    }

  if (i < 0)
    error ("Structure has no component named %s.", name);

  return value_field (arg1, i);
}

static
initialize ()
{ }

END_FILE
