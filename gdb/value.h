/* Definitions for values of C expressions, for GDB.
   Copyright 1986, 1987, 1989, 1992 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#if !defined (VALUE_H)
#define VALUE_H 1
/*
 * The structure which defines the type of a value.  It should never
 * be possible for a program lval value to survive over a call to the inferior
 * (ie to be put into the history list or an internal variable).
 */
enum lval_type {
  /* Not an lval.  */
  not_lval,
  /* In memory.  Could be a saved register.  */
  lval_memory,
  /* In a register.  */
  lval_register,
  /* In a gdb internal variable.  */
  lval_internalvar,
  /* Part of a gdb internal variable (structure field).  */
  lval_internalvar_component,
  /* In a register series in a frame not the current one, which may have been
     partially saved or saved in different places (otherwise would be
     lval_register or lval_memory).  */
  lval_reg_frame_relative
};

struct value
  {
    /* Type of value; either not an lval, or one of the various
       different possible kinds of lval.  */
    enum lval_type lval;
    /* Location of value (if lval).  */
    union
      {
	/* Address in inferior or byte of registers structure.  */
	CORE_ADDR address;
	/* Pointer to interrnal variable.  */
	struct internalvar *internalvar;
	/* Number of register.  Only used with
	   lval_reg_frame_relative.  */
	int regnum;
      } location;
    /* Describes offset of a value within lval a structure in bytes.  */
    int offset;	
    /* Only used for bitfields; number of bits contained in them.  */
    int bitsize;
    /* Only used for bitfields; position of start of field.  */
    int bitpos;
    /* Frame value is relative to.  In practice, this address is only
       used if the value is stored in several registers in other than
       the current frame, and these registers have not all been saved
       at the same place in memory.  This will be described in the
       lval enum above as "lval_reg_frame_relative".  */
    CORE_ADDR frame_addr;
    /* Type of the value.  */
    struct type *type;
    /* Values are stored in a chain, so that they can be deleted
       easily over calls to the inferior.  Values assigned to internal
       variables or put into the value history are taken off this
       list.  */
    struct value *next;
    /* If an lval is forced to repeat, a new value is created with
       these fields set.  The new value is not an lval.  */
    short repeated;
    short repetitions;
    /* Register number if the value is from a register.  Is not kept
       if you take a field of a structure that is stored in a
       register.  Shouldn't it be?  */
    short regno;
    /* If zero, contents of this value are in the contents field.
       If nonzero, contents are in inferior memory at address
       in the location.address field plus the offset field
       (and the lval field should be lval_memory).  */
    char lazy;
    /* If nonzero, this is the value of a variable which does not
       actually exist in the program.  */
    char optimized_out;
    /* Actual contents of the value.  For use of this value; setting
       it uses the stuff above.  Not valid if lazy is nonzero.
       Target byte-order.  We force it to be aligned properly for any
       possible value.  */
    union {
      long contents[1];
      double force_double_align;
#ifdef LONG_LONG
      long long force_longlong_align;
#endif
    } aligner;

  };

typedef struct value *value;

#define VALUE_TYPE(val) (val)->type
#define VALUE_LAZY(val) (val)->lazy
/* VALUE_CONTENTS and VALUE_CONTENTS_RAW both return the address of
   the gdb buffer used to hold a copy of the contents of the lval.  
   VALUE_CONTENTS is used when the contents of the buffer are needed --
   it uses value_fetch_lazy() to load the buffer from the process being 
   debugged if it hasn't already been loaded.  VALUE_CONTENTS_RAW is 
   used when data is being stored into the buffer, or when it is 
   certain that the contents of the buffer are valid.  */
#define VALUE_CONTENTS_RAW(val) ((char *) (val)->aligner.contents)
#define VALUE_CONTENTS(val) ((void)(VALUE_LAZY(val) && value_fetch_lazy(val)),\
			     VALUE_CONTENTS_RAW(val))
extern int value_fetch_lazy ();
#define VALUE_LVAL(val) (val)->lval
#define VALUE_ADDRESS(val) (val)->location.address
#define VALUE_INTERNALVAR(val) (val)->location.internalvar
#define VALUE_FRAME_REGNUM(val) ((val)->location.regnum)
#define VALUE_FRAME(val) ((val)->frame_addr)
#define VALUE_OFFSET(val) (val)->offset
#define VALUE_BITSIZE(val) (val)->bitsize
#define VALUE_BITPOS(val) (val)->bitpos
#define VALUE_NEXT(val) (val)->next
#define VALUE_REPEATED(val) (val)->repeated
#define VALUE_REPETITIONS(val) (val)->repetitions
#define VALUE_REGNO(val) (val)->regno
#define VALUE_OPTIMIZED_OUT(val) ((val)->optimized_out)

/* Convert a REF to the object referenced. */

#define COERCE_REF(arg)    \
{ if (TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_REF)			\
    arg = value_at_lazy (TYPE_TARGET_TYPE (VALUE_TYPE (arg)),		\
			 unpack_long (VALUE_TYPE (arg),			\
				      VALUE_CONTENTS (arg)));}

/* If ARG is an array, convert it to a pointer.
   If ARG is an enum, convert it to an integer.
   If ARG is a function, convert it to a function pointer.

   References are dereferenced.  */

#define COERCE_ARRAY(arg)    \
{ COERCE_REF(arg);							\
  if (VALUE_REPEATED (arg)						\
      || TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_ARRAY)		\
    arg = value_coerce_array (arg);					\
  if (TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_FUNC)                   \
    arg = value_coerce_function (arg);                                  \
  if (TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_ENUM)			\
    arg = value_cast (builtin_type_unsigned_int, arg);			\
}

/* If ARG is an enum, convert it to an integer.  */

#define COERCE_ENUM(arg)    \
{ if (TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_REF)			\
    arg = value_ind (arg);						\
  if (TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_ENUM)			\
    arg = value_cast (builtin_type_unsigned_int, arg);			\
}

/* Internal variables (variables for convenience of use of debugger)
   are recorded as a chain of these structures.  */

struct internalvar
{
  struct internalvar *next;
  char *name;
  value value;
};

#include "symtab.h"
LONGEST value_as_long (
#ifdef __STDC__
		       value
#endif
		       );
double value_as_double (
#ifdef __STDC__
			value
#endif
			);
CORE_ADDR value_as_pointer (
#ifdef __STDC__
			    value
#endif
			    );
LONGEST unpack_long (
#ifdef __STDC__
		     struct type *, char *
#endif
		     );
double unpack_double (
#ifdef __STDC__
		      struct type *, char *, int *
#endif
		      );
CORE_ADDR unpack_pointer (
#ifdef __STDC__
			  struct type *, char *
#endif
			  );
long unpack_field_as_long ();
value value_from_longest ();
value value_from_double ();
value value_at ();
value value_at_lazy ();
value value_from_register ();
value value_of_variable ();
value value_of_register ();
value read_var_value ();
value locate_var_value ();
value allocate_value ();
value allocate_repeat_value ();
value value_mark ();
void value_free_to_mark ();
value value_string ();

value value_binop ();
value value_add ();
value value_sub ();
value value_coerce_array ();
value value_coerce_function ();
value value_ind ();
value value_addr ();
value value_assign ();
value value_neg ();
value value_lognot ();
value value_struct_elt (), value_struct_elt_for_address ();
value value_field (), value_primitive_field ();
value value_cast ();
value value_zero ();
value value_repeat ();
value value_subscript ();
value value_from_vtable_info ();

value value_being_returned ();
int using_struct_return ();
void set_return_value ();

value evaluate_expression ();
value evaluate_type ();
value parse_and_eval ();
value parse_to_comma_and_eval ();
struct type *parse_and_eval_type ();
extern CORE_ADDR parse_and_eval_address ();
extern CORE_ADDR parse_and_eval_address_1 ();

value access_value_history ();
value value_of_internalvar ();
void set_internalvar ();
void set_internalvar_component ();
struct internalvar *lookup_internalvar ();

int value_equal ();
int value_less ();
int value_zerop ();

/* C++ */
value value_of_this ();
value value_static_field ();
value value_x_binop ();
value value_x_unop ();
value value_fn_field ();
value value_virtual_fn_field ();
int binop_user_defined_p ();
int unop_user_defined_p ();
int typecmp ();
void fill_in_vptr_fieldno ();
int destructor_name_p ();

#define value_free(val) free (val)
void free_all_values ();
void release_value ();
int record_latest_value ();

void registers_changed ();
void read_register_bytes ();
void write_register_bytes ();
void read_register_gen ();
CORE_ADDR read_register ();
void write_register ();
void supply_register ();
void get_saved_register ();

void modify_field ();
void type_print ();
void type_print_1 ();

/* Possibilities for prettyprint parameters to routines which print
   things.  */
enum val_prettyprint {
  Val_no_prettyprint = 0,
  Val_prettyprint,
  /* Use the default setting which the user has specified.  */
  Val_pretty_default
  };

char *baseclass_addr ();
void print_floating ();
int value_print ();
int val_print ();
void print_variable_value ();
void typedef_print ();
char *internalvar_name ();
void clear_value_history ();
void clear_internalvars ();

extern value
call_function_by_hand PARAMS ((value, int value *));

#endif /* value.h not already included.  */
