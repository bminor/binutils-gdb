/* Definitions for values of C expressions, for GDB.
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

enum lval_type { not_lval, lval_memory, lval_register, lval_internalvar,
		 lval_internalvar_component };

struct value
  {
    enum lval_type lval;
    union
      {
	CORE_ADDR address;
	struct internalvar *internalvar;
      } location;
    int offset;
    int bitsize;
    int bitpos;
    struct type *type;
    struct value *next;
    short repeated;
    short repetitions;
    short regno;
    long contents[1];
  };

typedef struct value *value;

#define VALUE_TYPE(val) (val)->type
#define VALUE_CONTENTS(val) ((char *) (val)->contents)
#define VALUE_LVAL(val) (val)->lval
#define VALUE_ADDRESS(val) (val)->location.address
#define VALUE_INTERNALVAR(val) (val)->location.internalvar
#define VALUE_OFFSET(val) (val)->offset
#define VALUE_BITSIZE(val) (val)->bitsize
#define VALUE_BITPOS(val) (val)->bitpos
#define VALUE_NEXT(val) (val)->next
#define VALUE_REPEATED(val) (val)->repeated
#define VALUE_REPETITIONS(val) (val)->repetitions
#define VALUE_REGNO(val) (val)->regno

/* If ARG is an array, convert it to a pointer.
   If ARG is an enum, convert it to an integer.  */

#define COERCE_ARRAY(arg)    \
{ if (VALUE_REPEATED (arg)						\
      || TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_ARRAY)		\
    arg = value_coerce_array (arg);					\
  if (TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_ENUM)			\
    arg = value_cast (builtin_type_unsigned_int, arg);			\
}

/* If ARG is an enum, convert it to an integer.  */

#define COERCE_ENUM(arg)    \
{ if (TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_ENUM)			\
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

long value_as_long ();
double value_as_double ();
long unpack_long ();
double unpack_double ();
long unpack_field_as_long ();
value value_from_long ();
value value_from_double ();
value value_at ();
value value_of_variable ();
value value_of_register ();
value read_var_value ();
value locate_var_value ();
value allocate_value ();
value allocate_repeat_value ();
value value_string ();

value value_binop ();
value value_add ();
value value_sub ();
value value_coerce_array ();
value value_ind ();
value value_addr ();
value value_assign ();
value value_neg ();
value value_lognot ();
value value_struct_elt ();
value value_field ();
value value_cast ();
value value_repeat ();
value value_subscript ();

value call_function ();
value value_being_returned ();

value evaluate_expression ();
value evaluate_type ();
value parse_and_eval ();
value parse_to_comma_and_eval ();

value access_value_history ();
value value_of_internalvar ();
struct internalvar *lookup_internalvar ();

int value_equal ();
int value_less ();
int value_zerop ();
