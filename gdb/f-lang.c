/* Fortran language support routines for GDB, the GNU debugger.

   Copyright (C) 1993-2021 Free Software Foundation, Inc.

   Contributed by Motorola.  Adapted from the C parser by Farooq Butt
   (fmbutt@engage.sps.mot.com).

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
#include "gdbtypes.h"
#include "expression.h"
#include "parser-defs.h"
#include "language.h"
#include "varobj.h"
#include "gdbcore.h"
#include "f-lang.h"
#include "valprint.h"
#include "value.h"
#include "cp-support.h"
#include "charset.h"
#include "c-lang.h"
#include "target-float.h"
#include "gdbarch.h"
#include "gdbcmd.h"
#include "f-array-walker.h"

#include <math.h>

/* Whether GDB should repack array slices created by the user.  */
static bool repack_array_slices = false;

/* Implement 'show fortran repack-array-slices'.  */
static void
show_repack_array_slices (struct ui_file *file, int from_tty,
			  struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Repacking of Fortran array slices is %s.\n"),
		    value);
}

/* Debugging of Fortran's array slicing.  */
static bool fortran_array_slicing_debug = false;

/* Implement 'show debug fortran-array-slicing'.  */
static void
show_fortran_array_slicing_debug (struct ui_file *file, int from_tty,
				  struct cmd_list_element *c,
				  const char *value)
{
  fprintf_filtered (file, _("Debugging of Fortran array slicing is %s.\n"),
		    value);
}

/* Local functions */

static value *fortran_prepare_argument (struct expression *exp, int *pos,
					int arg_num, bool is_internal_call_p,
					struct type *func_type,
					enum noside noside);

/* Return the encoding that should be used for the character type
   TYPE.  */

const char *
f_language::get_encoding (struct type *type)
{
  const char *encoding;

  switch (TYPE_LENGTH (type))
    {
    case 1:
      encoding = target_charset (type->arch ());
      break;
    case 4:
      if (type_byte_order (type) == BFD_ENDIAN_BIG)
	encoding = "UTF-32BE";
      else
	encoding = "UTF-32LE";
      break;

    default:
      error (_("unrecognized character type"));
    }

  return encoding;
}



/* Table of operators and their precedences for printing expressions.  */

const struct op_print f_language::op_print_tab[] =
{
  {"+", BINOP_ADD, PREC_ADD, 0},
  {"+", UNOP_PLUS, PREC_PREFIX, 0},
  {"-", BINOP_SUB, PREC_ADD, 0},
  {"-", UNOP_NEG, PREC_PREFIX, 0},
  {"*", BINOP_MUL, PREC_MUL, 0},
  {"/", BINOP_DIV, PREC_MUL, 0},
  {"DIV", BINOP_INTDIV, PREC_MUL, 0},
  {"MOD", BINOP_REM, PREC_MUL, 0},
  {"=", BINOP_ASSIGN, PREC_ASSIGN, 1},
  {".OR.", BINOP_LOGICAL_OR, PREC_LOGICAL_OR, 0},
  {".AND.", BINOP_LOGICAL_AND, PREC_LOGICAL_AND, 0},
  {".NOT.", UNOP_LOGICAL_NOT, PREC_PREFIX, 0},
  {".EQ.", BINOP_EQUAL, PREC_EQUAL, 0},
  {".NE.", BINOP_NOTEQUAL, PREC_EQUAL, 0},
  {".LE.", BINOP_LEQ, PREC_ORDER, 0},
  {".GE.", BINOP_GEQ, PREC_ORDER, 0},
  {".GT.", BINOP_GTR, PREC_ORDER, 0},
  {".LT.", BINOP_LESS, PREC_ORDER, 0},
  {"**", UNOP_IND, PREC_PREFIX, 0},
  {"@", BINOP_REPEAT, PREC_REPEAT, 0},
  {NULL, OP_NULL, PREC_REPEAT, 0}
};


/* Create an array containing the lower bounds (when LBOUND_P is true) or
   the upper bounds (when LBOUND_P is false) of ARRAY (which must be of
   array type).  GDBARCH is the current architecture.  */

static struct value *
fortran_bounds_all_dims (bool lbound_p,
			 struct gdbarch *gdbarch,
			 struct value *array)
{
  type *array_type = check_typedef (value_type (array));
  int ndimensions = calc_f77_array_dims (array_type);

  /* Allocate a result value of the correct type.  */
  struct type *range
    = create_static_range_type (nullptr,
				builtin_type (gdbarch)->builtin_int,
				1, ndimensions);
  struct type *elm_type = builtin_type (gdbarch)->builtin_long_long;
  struct type *result_type = create_array_type (nullptr, elm_type, range);
  struct value *result = allocate_value (result_type);

  /* Walk the array dimensions backwards due to the way the array will be
     laid out in memory, the first dimension will be the most inner.  */
  LONGEST elm_len = TYPE_LENGTH (elm_type);
  for (LONGEST dst_offset = elm_len * (ndimensions - 1);
       dst_offset >= 0;
       dst_offset -= elm_len)
    {
      LONGEST b;

      /* Grab the required bound.  */
      if (lbound_p)
	b = f77_get_lowerbound (array_type);
      else
	b = f77_get_upperbound (array_type);

      /* And copy the value into the result value.  */
      struct value *v = value_from_longest (elm_type, b);
      gdb_assert (dst_offset + TYPE_LENGTH (value_type (v))
		  <= TYPE_LENGTH (value_type (result)));
      gdb_assert (TYPE_LENGTH (value_type (v)) == elm_len);
      value_contents_copy (result, dst_offset, v, 0, elm_len);

      /* Peel another dimension of the array.  */
      array_type = TYPE_TARGET_TYPE (array_type);
    }

  return result;
}

/* Return the lower bound (when LBOUND_P is true) or the upper bound (when
   LBOUND_P is false) for dimension DIM_VAL (which must be an integer) of
   ARRAY (which must be an array).  GDBARCH is the current architecture.  */

static struct value *
fortran_bounds_for_dimension (bool lbound_p,
			      struct gdbarch *gdbarch,
			      struct value *array,
			      struct value *dim_val)
{
  /* Check the requested dimension is valid for this array.  */
  type *array_type = check_typedef (value_type (array));
  int ndimensions = calc_f77_array_dims (array_type);
  long dim = value_as_long (dim_val);
  if (dim < 1 || dim > ndimensions)
    {
      if (lbound_p)
	error (_("LBOUND dimension must be from 1 to %d"), ndimensions);
      else
	error (_("UBOUND dimension must be from 1 to %d"), ndimensions);
    }

  /* The type for the result.  */
  struct type *bound_type = builtin_type (gdbarch)->builtin_long_long;

  /* Walk the dimensions backwards, due to the ordering in which arrays are
     laid out the first dimension is the most inner.  */
  for (int i = ndimensions - 1; i >= 0; --i)
    {
      /* If this is the requested dimension then we're done.  Grab the
	 bounds and return.  */
      if (i == dim - 1)
	{
	  LONGEST b;

	  if (lbound_p)
	    b = f77_get_lowerbound (array_type);
	  else
	    b = f77_get_upperbound (array_type);

	  return value_from_longest (bound_type, b);
	}

      /* Peel off another dimension of the array.  */
      array_type = TYPE_TARGET_TYPE (array_type);
    }

  gdb_assert_not_reached ("failed to find matching dimension");
}


/* Return the number of dimensions for a Fortran array or string.  */

int
calc_f77_array_dims (struct type *array_type)
{
  int ndimen = 1;
  struct type *tmp_type;

  if ((array_type->code () == TYPE_CODE_STRING))
    return 1;

  if ((array_type->code () != TYPE_CODE_ARRAY))
    error (_("Can't get dimensions for a non-array type"));

  tmp_type = array_type;

  while ((tmp_type = TYPE_TARGET_TYPE (tmp_type)))
    {
      if (tmp_type->code () == TYPE_CODE_ARRAY)
	++ndimen;
    }
  return ndimen;
}

/* A class used by FORTRAN_VALUE_SUBARRAY when repacking Fortran array
   slices.  This is a base class for two alternative repacking mechanisms,
   one for when repacking from a lazy value, and one for repacking from a
   non-lazy (already loaded) value.  */
class fortran_array_repacker_base_impl
  : public fortran_array_walker_base_impl
{
public:
  /* Constructor, DEST is the value we are repacking into.  */
  fortran_array_repacker_base_impl (struct value *dest)
    : m_dest (dest),
      m_dest_offset (0)
  { /* Nothing.  */ }

  /* When we start processing the inner most dimension, this is where we
     will be creating values for each element as we load them and then copy
     them into the M_DEST value.  Set a value mark so we can free these
     temporary values.  */
  void start_dimension (bool inner_p)
  {
    if (inner_p)
      {
	gdb_assert (m_mark == nullptr);
	m_mark = value_mark ();
      }
  }

  /* When we finish processing the inner most dimension free all temporary
     value that were created.  */
  void finish_dimension (bool inner_p, bool last_p)
  {
    if (inner_p)
      {
	gdb_assert (m_mark != nullptr);
	value_free_to_mark (m_mark);
	m_mark = nullptr;
      }
  }

protected:
  /* Copy the contents of array element ELT into M_DEST at the next
     available offset.  */
  void copy_element_to_dest (struct value *elt)
  {
    value_contents_copy (m_dest, m_dest_offset, elt, 0,
			 TYPE_LENGTH (value_type (elt)));
    m_dest_offset += TYPE_LENGTH (value_type (elt));
  }

  /* The value being written to.  */
  struct value *m_dest;

  /* The byte offset in M_DEST at which the next element should be
     written.  */
  LONGEST m_dest_offset;

  /* Set with a call to VALUE_MARK, and then reset after calling
     VALUE_FREE_TO_MARK.  */
  struct value *m_mark = nullptr;
};

/* A class used by FORTRAN_VALUE_SUBARRAY when repacking Fortran array
   slices.  This class is specialised for repacking an array slice from a
   lazy array value, as such it does not require the parent array value to
   be loaded into GDB's memory; the parent value could be huge, while the
   slice could be tiny.  */
class fortran_lazy_array_repacker_impl
  : public fortran_array_repacker_base_impl
{
public:
  /* Constructor.  TYPE is the type of the slice being loaded from the
     parent value, so this type will correctly reflect the strides required
     to find all of the elements from the parent value.  ADDRESS is the
     address in target memory of value matching TYPE, and DEST is the value
     we are repacking into.  */
  explicit fortran_lazy_array_repacker_impl (struct type *type,
					     CORE_ADDR address,
					     struct value *dest)
    : fortran_array_repacker_base_impl (dest),
      m_addr (address)
  { /* Nothing.  */ }

  /* Create a lazy value in target memory representing a single element,
     then load the element into GDB's memory and copy the contents into the
     destination value.  */
  void process_element (struct type *elt_type, LONGEST elt_off, bool last_p)
  {
    copy_element_to_dest (value_at_lazy (elt_type, m_addr + elt_off));
  }

private:
  /* The address in target memory where the parent value starts.  */
  CORE_ADDR m_addr;
};

/* A class used by FORTRAN_VALUE_SUBARRAY when repacking Fortran array
   slices.  This class is specialised for repacking an array slice from a
   previously loaded (non-lazy) array value, as such it fetches the
   element values from the contents of the parent value.  */
class fortran_array_repacker_impl
  : public fortran_array_repacker_base_impl
{
public:
  /* Constructor.  TYPE is the type for the array slice within the parent
     value, as such it has stride values as required to find the elements
     within the original parent value.  ADDRESS is the address in target
     memory of the value matching TYPE.  BASE_OFFSET is the offset from
     the start of VAL's content buffer to the start of the object of TYPE,
     VAL is the parent object from which we are loading the value, and
     DEST is the value into which we are repacking.  */
  explicit fortran_array_repacker_impl (struct type *type, CORE_ADDR address,
					LONGEST base_offset,
					struct value *val, struct value *dest)
    : fortran_array_repacker_base_impl (dest),
      m_base_offset (base_offset),
      m_val (val)
  {
    gdb_assert (!value_lazy (val));
  }

  /* Extract an element of ELT_TYPE at offset (M_BASE_OFFSET + ELT_OFF)
     from the content buffer of M_VAL then copy this extracted value into
     the repacked destination value.  */
  void process_element (struct type *elt_type, LONGEST elt_off, bool last_p)
  {
    struct value *elt
      = value_from_component (m_val, elt_type, (elt_off + m_base_offset));
    copy_element_to_dest (elt);
  }

private:
  /* The offset into the content buffer of M_VAL to the start of the slice
     being extracted.  */
  LONGEST m_base_offset;

  /* The parent value from which we are extracting a slice.  */
  struct value *m_val;
};

/* Called from evaluate_subexp_standard to perform array indexing, and
   sub-range extraction, for Fortran.  As well as arrays this function
   also handles strings as they can be treated like arrays of characters.
   ARRAY is the array or string being accessed.  EXP, POS, and NOSIDE are
   as for evaluate_subexp_standard, and NARGS is the number of arguments
   in this access (e.g. 'array (1,2,3)' would be NARGS 3).  */

static struct value *
fortran_value_subarray (struct value *array, struct expression *exp,
			int *pos, int nargs, enum noside noside)
{
  type *original_array_type = check_typedef (value_type (array));
  bool is_string_p = original_array_type->code () == TYPE_CODE_STRING;

  /* Perform checks for ARRAY not being available.  The somewhat overly
     complex logic here is just to keep backward compatibility with the
     errors that we used to get before FORTRAN_VALUE_SUBARRAY was
     rewritten.  Maybe a future task would streamline the error messages we
     get here, and update all the expected test results.  */
  if (exp->elts[*pos].opcode != OP_RANGE)
    {
      if (type_not_associated (original_array_type))
	error (_("no such vector element (vector not associated)"));
      else if (type_not_allocated (original_array_type))
	error (_("no such vector element (vector not allocated)"));
    }
  else
    {
      if (type_not_associated (original_array_type))
	error (_("array not associated"));
      else if (type_not_allocated (original_array_type))
	error (_("array not allocated"));
    }

  /* First check that the number of dimensions in the type we are slicing
     matches the number of arguments we were passed.  */
  int ndimensions = calc_f77_array_dims (original_array_type);
  if (nargs != ndimensions)
    error (_("Wrong number of subscripts"));

  /* This will be initialised below with the type of the elements held in
     ARRAY.  */
  struct type *inner_element_type;

  /* Extract the types of each array dimension from the original array
     type.  We need these available so we can fill in the default upper and
     lower bounds if the user requested slice doesn't provide that
     information.  Additionally unpacking the dimensions like this gives us
     the inner element type.  */
  std::vector<struct type *> dim_types;
  {
    dim_types.reserve (ndimensions);
    struct type *type = original_array_type;
    for (int i = 0; i < ndimensions; ++i)
      {
	dim_types.push_back (type);
	type = TYPE_TARGET_TYPE (type);
      }
    /* TYPE is now the inner element type of the array, we start the new
       array slice off as this type, then as we process the requested slice
       (from the user) we wrap new types around this to build up the final
       slice type.  */
    inner_element_type = type;
  }

  /* As we analyse the new slice type we need to understand if the data
     being referenced is contiguous.  Do decide this we must track the size
     of an element at each dimension of the new slice array.  Initially the
     elements of the inner most dimension of the array are the same inner
     most elements as the original ARRAY.  */
  LONGEST slice_element_size = TYPE_LENGTH (inner_element_type);

  /* Start off assuming all data is contiguous, this will be set to false
     if access to any dimension results in non-contiguous data.  */
  bool is_all_contiguous = true;

  /* The TOTAL_OFFSET is the distance in bytes from the start of the
     original ARRAY to the start of the new slice.  This is calculated as
     we process the information from the user.  */
  LONGEST total_offset = 0;

  /* A structure representing information about each dimension of the
     resulting slice.  */
  struct slice_dim
  {
    /* Constructor.  */
    slice_dim (LONGEST l, LONGEST h, LONGEST s, struct type *idx)
      : low (l),
	high (h),
	stride (s),
	index (idx)
    { /* Nothing.  */ }

    /* The low bound for this dimension of the slice.  */
    LONGEST low;

    /* The high bound for this dimension of the slice.  */
    LONGEST high;

    /* The byte stride for this dimension of the slice.  */
    LONGEST stride;

    struct type *index;
  };

  /* The dimensions of the resulting slice.  */
  std::vector<slice_dim> slice_dims;

  /* Process the incoming arguments.   These arguments are in the reverse
     order to the array dimensions, that is the first argument refers to
     the last array dimension.  */
  if (fortran_array_slicing_debug)
    debug_printf ("Processing array access:\n");
  for (int i = 0; i < nargs; ++i)
    {
      /* For each dimension of the array the user will have either provided
	 a ranged access with optional lower bound, upper bound, and
	 stride, or the user will have supplied a single index.  */
      struct type *dim_type = dim_types[ndimensions - (i + 1)];
      if (exp->elts[*pos].opcode == OP_RANGE)
	{
	  int pc = (*pos) + 1;
	  enum range_flag range_flag = (enum range_flag) exp->elts[pc].longconst;
	  *pos += 3;

	  LONGEST low, high, stride;
	  low = high = stride = 0;

	  if ((range_flag & RANGE_LOW_BOUND_DEFAULT) == 0)
	    low = value_as_long (evaluate_subexp (nullptr, exp, pos, noside));
	  else
	    low = f77_get_lowerbound (dim_type);
	  if ((range_flag & RANGE_HIGH_BOUND_DEFAULT) == 0)
	    high = value_as_long (evaluate_subexp (nullptr, exp, pos, noside));
	  else
	    high = f77_get_upperbound (dim_type);
	  if ((range_flag & RANGE_HAS_STRIDE) == RANGE_HAS_STRIDE)
	    stride = value_as_long (evaluate_subexp (nullptr, exp, pos, noside));
	  else
	    stride = 1;

	  if (stride == 0)
	    error (_("stride must not be 0"));

	  /* Get information about this dimension in the original ARRAY.  */
	  struct type *target_type = TYPE_TARGET_TYPE (dim_type);
	  struct type *index_type = dim_type->index_type ();
	  LONGEST lb = f77_get_lowerbound (dim_type);
	  LONGEST ub = f77_get_upperbound (dim_type);
	  LONGEST sd = index_type->bit_stride ();
	  if (sd == 0)
	    sd = TYPE_LENGTH (target_type) * 8;

	  if (fortran_array_slicing_debug)
	    {
	      debug_printf ("|-> Range access\n");
	      std::string str = type_to_string (dim_type);
	      debug_printf ("|   |-> Type: %s\n", str.c_str ());
	      debug_printf ("|   |-> Array:\n");
	      debug_printf ("|   |   |-> Low bound: %s\n", plongest (lb));
	      debug_printf ("|   |   |-> High bound: %s\n", plongest (ub));
	      debug_printf ("|   |   |-> Bit stride: %s\n", plongest (sd));
	      debug_printf ("|   |   |-> Byte stride: %s\n", plongest (sd / 8));
	      debug_printf ("|   |   |-> Type size: %s\n",
			    pulongest (TYPE_LENGTH (dim_type)));
	      debug_printf ("|   |   '-> Target type size: %s\n",
			    pulongest (TYPE_LENGTH (target_type)));
	      debug_printf ("|   |-> Accessing:\n");
	      debug_printf ("|   |   |-> Low bound: %s\n",
			    plongest (low));
	      debug_printf ("|   |   |-> High bound: %s\n",
			    plongest (high));
	      debug_printf ("|   |   '-> Element stride: %s\n",
			    plongest (stride));
	    }

	  /* Check the user hasn't asked for something invalid.  */
	  if (high > ub || low < lb)
	    error (_("array subscript out of bounds"));

	  /* Calculate what this dimension of the new slice array will look
	     like.  OFFSET is the byte offset from the start of the
	     previous (more outer) dimension to the start of this
	     dimension.  E_COUNT is the number of elements in this
	     dimension.  REMAINDER is the number of elements remaining
	     between the last included element and the upper bound.  For
	     example an access '1:6:2' will include elements 1, 3, 5 and
	     have a remainder of 1 (element #6).  */
	  LONGEST lowest = std::min (low, high);
	  LONGEST offset = (sd / 8) * (lowest - lb);
	  LONGEST e_count = std::abs (high - low) + 1;
	  e_count = (e_count + (std::abs (stride) - 1)) / std::abs (stride);
	  LONGEST new_low = 1;
	  LONGEST new_high = new_low + e_count - 1;
	  LONGEST new_stride = (sd * stride) / 8;
	  LONGEST last_elem = low + ((e_count - 1) * stride);
	  LONGEST remainder = high - last_elem;
	  if (low > high)
	    {
	      offset += std::abs (remainder) * TYPE_LENGTH (target_type);
	      if (stride > 0)
		error (_("incorrect stride and boundary combination"));
	    }
	  else if (stride < 0)
	    error (_("incorrect stride and boundary combination"));

	  /* Is the data within this dimension contiguous?  It is if the
	     newly computed stride is the same size as a single element of
	     this dimension.  */
	  bool is_dim_contiguous = (new_stride == slice_element_size);
	  is_all_contiguous &= is_dim_contiguous;

	  if (fortran_array_slicing_debug)
	    {
	      debug_printf ("|   '-> Results:\n");
	      debug_printf ("|       |-> Offset = %s\n", plongest (offset));
	      debug_printf ("|       |-> Elements = %s\n", plongest (e_count));
	      debug_printf ("|       |-> Low bound = %s\n", plongest (new_low));
	      debug_printf ("|       |-> High bound = %s\n",
			    plongest (new_high));
	      debug_printf ("|       |-> Byte stride = %s\n",
			    plongest (new_stride));
	      debug_printf ("|       |-> Last element = %s\n",
			    plongest (last_elem));
	      debug_printf ("|       |-> Remainder = %s\n",
			    plongest (remainder));
	      debug_printf ("|       '-> Contiguous = %s\n",
			    (is_dim_contiguous ? "Yes" : "No"));
	    }

	  /* Figure out how big (in bytes) an element of this dimension of
	     the new array slice will be.  */
	  slice_element_size = std::abs (new_stride * e_count);

	  slice_dims.emplace_back (new_low, new_high, new_stride,
				   index_type);

	  /* Update the total offset.  */
	  total_offset += offset;
	}
      else
	{
	  /* There is a single index for this dimension.  */
	  LONGEST index
	    = value_as_long (evaluate_subexp_with_coercion (exp, pos, noside));

	  /* Get information about this dimension in the original ARRAY.  */
	  struct type *target_type = TYPE_TARGET_TYPE (dim_type);
	  struct type *index_type = dim_type->index_type ();
	  LONGEST lb = f77_get_lowerbound (dim_type);
	  LONGEST ub = f77_get_upperbound (dim_type);
	  LONGEST sd = index_type->bit_stride () / 8;
	  if (sd == 0)
	    sd = TYPE_LENGTH (target_type);

	  if (fortran_array_slicing_debug)
	    {
	      debug_printf ("|-> Index access\n");
	      std::string str = type_to_string (dim_type);
	      debug_printf ("|   |-> Type: %s\n", str.c_str ());
	      debug_printf ("|   |-> Array:\n");
	      debug_printf ("|   |   |-> Low bound: %s\n", plongest (lb));
	      debug_printf ("|   |   |-> High bound: %s\n", plongest (ub));
	      debug_printf ("|   |   |-> Byte stride: %s\n", plongest (sd));
	      debug_printf ("|   |   |-> Type size: %s\n",
			    pulongest (TYPE_LENGTH (dim_type)));
	      debug_printf ("|   |   '-> Target type size: %s\n",
			    pulongest (TYPE_LENGTH (target_type)));
	      debug_printf ("|   '-> Accessing:\n");
	      debug_printf ("|       '-> Index: %s\n",
			    plongest (index));
	    }

	  /* If the array has actual content then check the index is in
	     bounds.  An array without content (an unbound array) doesn't
	     have a known upper bound, so don't error check in that
	     situation.  */
	  if (index < lb
	      || (dim_type->index_type ()->bounds ()->high.kind () != PROP_UNDEFINED
		  && index > ub)
	      || (VALUE_LVAL (array) != lval_memory
		  && dim_type->index_type ()->bounds ()->high.kind () == PROP_UNDEFINED))
	    {
	      if (type_not_associated (dim_type))
		error (_("no such vector element (vector not associated)"));
	      else if (type_not_allocated (dim_type))
		error (_("no such vector element (vector not allocated)"));
	      else
		error (_("no such vector element"));
	    }

	  /* Calculate using the type stride, not the target type size.  */
	  LONGEST offset = sd * (index - lb);
	  total_offset += offset;
	}
    }

  if (noside == EVAL_SKIP)
    return array;

  /* Build a type that represents the new array slice in the target memory
     of the original ARRAY, this type makes use of strides to correctly
     find only those elements that are part of the new slice.  */
  struct type *array_slice_type = inner_element_type;
  for (const auto &d : slice_dims)
    {
      /* Create the range.  */
      dynamic_prop p_low, p_high, p_stride;

      p_low.set_const_val (d.low);
      p_high.set_const_val (d.high);
      p_stride.set_const_val (d.stride);

      struct type *new_range
	= create_range_type_with_stride ((struct type *) NULL,
					 TYPE_TARGET_TYPE (d.index),
					 &p_low, &p_high, 0, &p_stride,
					 true);
      array_slice_type
	= create_array_type (nullptr, array_slice_type, new_range);
    }

  if (fortran_array_slicing_debug)
    {
      debug_printf ("'-> Final result:\n");
      debug_printf ("    |-> Type: %s\n",
		    type_to_string (array_slice_type).c_str ());
      debug_printf ("    |-> Total offset: %s\n",
		    plongest (total_offset));
      debug_printf ("    |-> Base address: %s\n",
		    core_addr_to_string (value_address (array)));
      debug_printf ("    '-> Contiguous = %s\n",
		    (is_all_contiguous ? "Yes" : "No"));
    }

  /* Should we repack this array slice?  */
  if (!is_all_contiguous && (repack_array_slices || is_string_p))
    {
      /* Build a type for the repacked slice.  */
      struct type *repacked_array_type = inner_element_type;
      for (const auto &d : slice_dims)
	{
	  /* Create the range.  */
	  dynamic_prop p_low, p_high, p_stride;

	  p_low.set_const_val (d.low);
	  p_high.set_const_val (d.high);
	  p_stride.set_const_val (TYPE_LENGTH (repacked_array_type));

	  struct type *new_range
	    = create_range_type_with_stride ((struct type *) NULL,
					     TYPE_TARGET_TYPE (d.index),
					     &p_low, &p_high, 0, &p_stride,
					     true);
	  repacked_array_type
	    = create_array_type (nullptr, repacked_array_type, new_range);
	}

      /* Now copy the elements from the original ARRAY into the packed
	 array value DEST.  */
      struct value *dest = allocate_value (repacked_array_type);
      if (value_lazy (array)
	  || (total_offset + TYPE_LENGTH (array_slice_type)
	      > TYPE_LENGTH (check_typedef (value_type (array)))))
	{
	  fortran_array_walker<fortran_lazy_array_repacker_impl> p
	    (array_slice_type, value_address (array) + total_offset, dest);
	  p.walk ();
	}
      else
	{
	  fortran_array_walker<fortran_array_repacker_impl> p
	    (array_slice_type, value_address (array) + total_offset,
	     total_offset, array, dest);
	  p.walk ();
	}
      array = dest;
    }
  else
    {
      if (VALUE_LVAL (array) == lval_memory)
	{
	  /* If the value we're taking a slice from is not yet loaded, or
	     the requested slice is outside the values content range then
	     just create a new lazy value pointing at the memory where the
	     contents we're looking for exist.  */
	  if (value_lazy (array)
	      || (total_offset + TYPE_LENGTH (array_slice_type)
		  > TYPE_LENGTH (check_typedef (value_type (array)))))
	    array = value_at_lazy (array_slice_type,
				   value_address (array) + total_offset);
	  else
	    array = value_from_contents_and_address (array_slice_type,
						     (value_contents (array)
						      + total_offset),
						     (value_address (array)
						      + total_offset));
	}
      else if (!value_lazy (array))
	array = value_from_component (array, array_slice_type, total_offset);
      else
	error (_("cannot subscript arrays that are not in memory"));
    }

  return array;
}

/* Evaluate FORTRAN_ASSOCIATED expressions.  Both GDBARCH and LANG are
   extracted from the expression being evaluated.  POINTER is the required
   first argument to the 'associated' keyword, and TARGET is the optional
   second argument, this will be nullptr if the user only passed one
   argument to their use of 'associated'.  */

static struct value *
fortran_associated (struct gdbarch *gdbarch, const language_defn *lang,
		    struct value *pointer, struct value *target = nullptr)
{
  struct type *result_type = language_bool_type (lang, gdbarch);

  /* All Fortran pointers should have the associated property, this is
     how we know the pointer is pointing at something or not.  */
  struct type *pointer_type = check_typedef (value_type (pointer));
  if (TYPE_ASSOCIATED_PROP (pointer_type) == nullptr
      && pointer_type->code () != TYPE_CODE_PTR)
    error (_("ASSOCIATED can only be applied to pointers"));

  /* Get an address from POINTER.  Fortran (or at least gfortran) models
     array pointers as arrays with a dynamic data address, so we need to
     use two approaches here, for real pointers we take the contents of the
     pointer as an address.  For non-pointers we take the address of the
     content.  */
  CORE_ADDR pointer_addr;
  if (pointer_type->code () == TYPE_CODE_PTR)
    pointer_addr = value_as_address (pointer);
  else
    pointer_addr = value_address (pointer);

  /* The single argument case, is POINTER associated with anything?  */
  if (target == nullptr)
    {
      bool is_associated = false;

      /* If POINTER is an actual pointer and doesn't have an associated
	 property then we need to figure out whether this pointer is
	 associated by looking at the value of the pointer itself.  We make
	 the assumption that a non-associated pointer will be set to 0.
	 This is probably true for most targets, but might not be true for
	 everyone.  */
      if (pointer_type->code () == TYPE_CODE_PTR
	  && TYPE_ASSOCIATED_PROP (pointer_type) == nullptr)
	is_associated = (pointer_addr != 0);
      else
	is_associated = !type_not_associated (pointer_type);
      return value_from_longest (result_type, is_associated ? 1 : 0);
    }

  /* The two argument case, is POINTER associated with TARGET?  */

  struct type *target_type = check_typedef (value_type (target));

  struct type *pointer_target_type;
  if (pointer_type->code () == TYPE_CODE_PTR)
    pointer_target_type = TYPE_TARGET_TYPE (pointer_type);
  else
    pointer_target_type = pointer_type;

  struct type *target_target_type;
  if (target_type->code () == TYPE_CODE_PTR)
    target_target_type = TYPE_TARGET_TYPE (target_type);
  else
    target_target_type = target_type;

  if (pointer_target_type->code () != target_target_type->code ()
      || (pointer_target_type->code () != TYPE_CODE_ARRAY
	  && (TYPE_LENGTH (pointer_target_type)
	      != TYPE_LENGTH (target_target_type))))
    error (_("arguments to associated must be of same type and kind"));

  /* If TARGET is not in memory, or the original pointer is specifically
     known to be not associated with anything, then the answer is obviously
     false.  Alternatively, if POINTER is an actual pointer and has no
     associated property, then we have to check if its associated by
     looking the value of the pointer itself.  We make the assumption that
     a non-associated pointer will be set to 0.  This is probably true for
     most targets, but might not be true for everyone.  */
  if (value_lval_const (target) != lval_memory
      || type_not_associated (pointer_type)
      || (TYPE_ASSOCIATED_PROP (pointer_type) == nullptr
	  && pointer_type->code () == TYPE_CODE_PTR
	  && pointer_addr == 0))
    return value_from_longest (result_type, 0);

  /* See the comment for POINTER_ADDR above.  */
  CORE_ADDR target_addr;
  if (target_type->code () == TYPE_CODE_PTR)
    target_addr = value_as_address (target);
  else
    target_addr = value_address (target);

  /* Wrap the following checks inside a do { ... } while (false) loop so
     that we can use `break' to jump out of the loop.  */
  bool is_associated = false;
  do
    {
      /* If the addresses are different then POINTER is definitely not
	 pointing at TARGET.  */
      if (pointer_addr != target_addr)
	break;

      /* If POINTER is a real pointer (i.e. not an array pointer, which are
	 implemented as arrays with a dynamic content address), then this
	 is all the checking that is needed.  */
      if (pointer_type->code () == TYPE_CODE_PTR)
	{
	  is_associated = true;
	  break;
	}

      /* We have an array pointer.  Check the number of dimensions.  */
      int pointer_dims = calc_f77_array_dims (pointer_type);
      int target_dims = calc_f77_array_dims (target_type);
      if (pointer_dims != target_dims)
	break;

      /* Now check that every dimension has the same upper bound, lower
	 bound, and stride value.  */
      int dim = 0;
      while (dim < pointer_dims)
	{
	  LONGEST pointer_lowerbound, pointer_upperbound, pointer_stride;
	  LONGEST target_lowerbound, target_upperbound, target_stride;

	  pointer_type = check_typedef (pointer_type);
	  target_type = check_typedef (target_type);

	  struct type *pointer_range = pointer_type->index_type ();
	  struct type *target_range = target_type->index_type ();

	  if (!get_discrete_bounds (pointer_range, &pointer_lowerbound,
				    &pointer_upperbound))
	    break;

	  if (!get_discrete_bounds (target_range, &target_lowerbound,
				    &target_upperbound))
	    break;

	  if (pointer_lowerbound != target_lowerbound
	      || pointer_upperbound != target_upperbound)
	    break;

	  /* Figure out the stride (in bits) for both pointer and target.
	     If either doesn't have a stride then we take the element size,
	     but we need to convert to bits (hence the * 8).  */
	  pointer_stride = pointer_range->bounds ()->bit_stride ();
	  if (pointer_stride == 0)
	    pointer_stride
	      = type_length_units (check_typedef
				     (TYPE_TARGET_TYPE (pointer_type))) * 8;
	  target_stride = target_range->bounds ()->bit_stride ();
	  if (target_stride == 0)
	    target_stride
	      = type_length_units (check_typedef
				     (TYPE_TARGET_TYPE (target_type))) * 8;
	  if (pointer_stride != target_stride)
	    break;

	  ++dim;
	}

      if (dim < pointer_dims)
	break;

      is_associated = true;
    }
  while (false);

  return value_from_longest (result_type, is_associated ? 1 : 0);
}


/* Special expression evaluation cases for Fortran.  */

static struct value *
evaluate_subexp_f (struct type *expect_type, struct expression *exp,
		   int *pos, enum noside noside)
{
  struct value *arg1 = NULL, *arg2 = NULL;
  enum exp_opcode op;
  int pc;
  struct type *type;

  pc = *pos;
  *pos += 1;
  op = exp->elts[pc].opcode;

  switch (op)
    {
    default:
      *pos -= 1;
      return evaluate_subexp_standard (expect_type, exp, pos, noside);

    case UNOP_ABS:
      arg1 = evaluate_subexp (nullptr, exp, pos, noside);
      if (noside == EVAL_SKIP)
	return eval_skip_value (exp);
      type = value_type (arg1);
      switch (type->code ())
	{
	case TYPE_CODE_FLT:
	  {
	    double d
	      = fabs (target_float_to_host_double (value_contents (arg1),
						   value_type (arg1)));
	    return value_from_host_double (type, d);
	  }
	case TYPE_CODE_INT:
	  {
	    LONGEST l = value_as_long (arg1);
	    l = llabs (l);
	    return value_from_longest (type, l);
	  }
	}
      error (_("ABS of type %s not supported"), TYPE_SAFE_NAME (type));

    case BINOP_MOD:
      arg1 = evaluate_subexp (nullptr, exp, pos, noside);
      arg2 = evaluate_subexp (value_type (arg1), exp, pos, noside);
      if (noside == EVAL_SKIP)
	return eval_skip_value (exp);
      type = value_type (arg1);
      if (type->code () != value_type (arg2)->code ())
	error (_("non-matching types for parameters to MOD ()"));
      switch (type->code ())
	{
	case TYPE_CODE_FLT:
	  {
	    double d1
	      = target_float_to_host_double (value_contents (arg1),
					     value_type (arg1));
	    double d2
	      = target_float_to_host_double (value_contents (arg2),
					     value_type (arg2));
	    double d3 = fmod (d1, d2);
	    return value_from_host_double (type, d3);
	  }
	case TYPE_CODE_INT:
	  {
	    LONGEST v1 = value_as_long (arg1);
	    LONGEST v2 = value_as_long (arg2);
	    if (v2 == 0)
	      error (_("calling MOD (N, 0) is undefined"));
	    LONGEST v3 = v1 - (v1 / v2) * v2;
	    return value_from_longest (value_type (arg1), v3);
	  }
	}
      error (_("MOD of type %s not supported"), TYPE_SAFE_NAME (type));

    case UNOP_FORTRAN_CEILING:
      {
	arg1 = evaluate_subexp (nullptr, exp, pos, noside);
	if (noside == EVAL_SKIP)
	  return eval_skip_value (exp);
	type = value_type (arg1);
	if (type->code () != TYPE_CODE_FLT)
	  error (_("argument to CEILING must be of type float"));
	double val
	  = target_float_to_host_double (value_contents (arg1),
					 value_type (arg1));
	val = ceil (val);
	return value_from_host_double (type, val);
      }

    case UNOP_FORTRAN_FLOOR:
      {
	arg1 = evaluate_subexp (nullptr, exp, pos, noside);
	if (noside == EVAL_SKIP)
	  return eval_skip_value (exp);
	type = value_type (arg1);
	if (type->code () != TYPE_CODE_FLT)
	  error (_("argument to FLOOR must be of type float"));
	double val
	  = target_float_to_host_double (value_contents (arg1),
					 value_type (arg1));
	val = floor (val);
	return value_from_host_double (type, val);
      }

    case UNOP_FORTRAN_ALLOCATED:
      {
	arg1 = evaluate_subexp (nullptr, exp, pos, noside);
	if (noside == EVAL_SKIP)
	  return eval_skip_value (exp);
	type = check_typedef (value_type (arg1));
	if (type->code () != TYPE_CODE_ARRAY)
	  error (_("ALLOCATED can only be applied to arrays"));
	struct type *result_type
	  = builtin_f_type (exp->gdbarch)->builtin_logical;
	LONGEST result_value = type_not_allocated (type) ? 0 : 1;
	return value_from_longest (result_type, result_value);
      }

    case BINOP_FORTRAN_MODULO:
      {
	arg1 = evaluate_subexp (nullptr, exp, pos, noside);
	arg2 = evaluate_subexp (value_type (arg1), exp, pos, noside);
	if (noside == EVAL_SKIP)
	  return eval_skip_value (exp);
	type = value_type (arg1);
	if (type->code () != value_type (arg2)->code ())
	  error (_("non-matching types for parameters to MODULO ()"));
	/* MODULO(A, P) = A - FLOOR (A / P) * P */
	switch (type->code ())
	  {
	  case TYPE_CODE_INT:
	    {
	      LONGEST a = value_as_long (arg1);
	      LONGEST p = value_as_long (arg2);
	      LONGEST result = a - (a / p) * p;
	      if (result != 0 && (a < 0) != (p < 0))
		result += p;
	      return value_from_longest (value_type (arg1), result);
	    }
	  case TYPE_CODE_FLT:
	    {
	      double a
		= target_float_to_host_double (value_contents (arg1),
					       value_type (arg1));
	      double p
		= target_float_to_host_double (value_contents (arg2),
					       value_type (arg2));
	      double result = fmod (a, p);
	      if (result != 0 && (a < 0.0) != (p < 0.0))
		result += p;
	      return value_from_host_double (type, result);
	    }
	  }
	error (_("MODULO of type %s not supported"), TYPE_SAFE_NAME (type));
      }

    case FORTRAN_LBOUND:
    case FORTRAN_UBOUND:
      {
	int nargs = longest_to_int (exp->elts[pc + 1].longconst);
	(*pos) += 2;

	/* This assertion should be enforced by the expression parser.  */
	gdb_assert (nargs == 1 || nargs == 2);

	bool lbound_p = op == FORTRAN_LBOUND;

	/* Check that the first argument is array like.  */
	arg1 = evaluate_subexp (nullptr, exp, pos, noside);
	type = check_typedef (value_type (arg1));
	if (type->code () != TYPE_CODE_ARRAY)
	  {
	    if (lbound_p)
	      error (_("LBOUND can only be applied to arrays"));
	    else
	      error (_("UBOUND can only be applied to arrays"));
	  }

	if (nargs == 1)
	  return fortran_bounds_all_dims (lbound_p, exp->gdbarch, arg1);

	/* User asked for the bounds of a specific dimension of the array.  */
	arg2 = evaluate_subexp (nullptr, exp, pos, noside);
	type = check_typedef (value_type (arg2));
	if (type->code () != TYPE_CODE_INT)
	  {
	    if (lbound_p)
	      error (_("LBOUND second argument should be an integer"));
	    else
	      error (_("UBOUND second argument should be an integer"));
	  }

	return fortran_bounds_for_dimension (lbound_p, exp->gdbarch, arg1,
					     arg2);
      }
      break;

    case FORTRAN_ASSOCIATED:
      {
	int nargs = longest_to_int (exp->elts[pc + 1].longconst);
	(*pos) += 2;

	/* This assertion should be enforced by the expression parser.  */
	gdb_assert (nargs == 1 || nargs == 2);

	arg1 = evaluate_subexp (nullptr, exp, pos, noside);

	if (nargs == 1)
	  {
	    if (noside == EVAL_SKIP)
	      return eval_skip_value (exp);
	    return fortran_associated (exp->gdbarch, exp->language_defn,
				       arg1);
	  }

	arg2 = evaluate_subexp (nullptr, exp, pos, noside);
	if (noside == EVAL_SKIP)
	  return eval_skip_value (exp);
	return fortran_associated (exp->gdbarch, exp->language_defn,
				   arg1, arg2);
      }
      break;

    case BINOP_FORTRAN_CMPLX:
      arg1 = evaluate_subexp (nullptr, exp, pos, noside);
      arg2 = evaluate_subexp (value_type (arg1), exp, pos, noside);
      if (noside == EVAL_SKIP)
	return eval_skip_value (exp);
      type = builtin_f_type(exp->gdbarch)->builtin_complex_s16;
      return value_literal_complex (arg1, arg2, type);

    case UNOP_FORTRAN_KIND:
      arg1 = evaluate_subexp (NULL, exp, pos, EVAL_AVOID_SIDE_EFFECTS);
      type = value_type (arg1);

      switch (type->code ())
	{
	  case TYPE_CODE_STRUCT:
	  case TYPE_CODE_UNION:
	  case TYPE_CODE_MODULE:
	  case TYPE_CODE_FUNC:
	    error (_("argument to kind must be an intrinsic type"));
	}

      if (!TYPE_TARGET_TYPE (type))
	return value_from_longest (builtin_type (exp->gdbarch)->builtin_int,
				   TYPE_LENGTH (type));
      return value_from_longest (builtin_type (exp->gdbarch)->builtin_int,
				 TYPE_LENGTH (TYPE_TARGET_TYPE (type)));


    case OP_F77_UNDETERMINED_ARGLIST:
      /* Remember that in F77, functions, substring ops and array subscript
	 operations cannot be disambiguated at parse time.  We have made
	 all array subscript operations, substring operations as well as
	 function calls come here and we now have to discover what the heck
	 this thing actually was.  If it is a function, we process just as
	 if we got an OP_FUNCALL.  */
      int nargs = longest_to_int (exp->elts[pc + 1].longconst);
      (*pos) += 2;

      /* First determine the type code we are dealing with.  */
      arg1 = evaluate_subexp (nullptr, exp, pos, noside);
      type = check_typedef (value_type (arg1));
      enum type_code code = type->code ();

      if (code == TYPE_CODE_PTR)
	{
	  /* Fortran always passes variable to subroutines as pointer.
	     So we need to look into its target type to see if it is
	     array, string or function.  If it is, we need to switch
	     to the target value the original one points to.  */
	  struct type *target_type = check_typedef (TYPE_TARGET_TYPE (type));

	  if (target_type->code () == TYPE_CODE_ARRAY
	      || target_type->code () == TYPE_CODE_STRING
	      || target_type->code () == TYPE_CODE_FUNC)
	    {
	      arg1 = value_ind (arg1);
	      type = check_typedef (value_type (arg1));
	      code = type->code ();
	    }
	}

      switch (code)
	{
	case TYPE_CODE_ARRAY:
	case TYPE_CODE_STRING:
	  return fortran_value_subarray (arg1, exp, pos, nargs, noside);

	case TYPE_CODE_PTR:
	case TYPE_CODE_FUNC:
	case TYPE_CODE_INTERNAL_FUNCTION:
	  {
	    /* It's a function call.  Allocate arg vector, including
	    space for the function to be called in argvec[0] and a
	    termination NULL.  */
	    struct value **argvec = (struct value **)
	      alloca (sizeof (struct value *) * (nargs + 2));
	    argvec[0] = arg1;
	    int tem = 1;
	    for (; tem <= nargs; tem++)
	      {
		bool is_internal_func = (code == TYPE_CODE_INTERNAL_FUNCTION);
		argvec[tem]
		  = fortran_prepare_argument (exp, pos, (tem - 1),
					      is_internal_func,
					      value_type (arg1), noside);
	      }
	    argvec[tem] = 0;	/* signal end of arglist */
	    if (noside == EVAL_SKIP)
	      return eval_skip_value (exp);
	    return evaluate_subexp_do_call (exp, noside, argvec[0],
					    gdb::make_array_view (argvec + 1,
								  nargs),
					    NULL, expect_type);
	  }

	default:
	  error (_("Cannot perform substring on this type"));
	}
    }

  /* Should be unreachable.  */
  return nullptr;
}

/* Special expression lengths for Fortran.  */

static void
operator_length_f (const struct expression *exp, int pc, int *oplenp,
		   int *argsp)
{
  int oplen = 1;
  int args = 0;

  switch (exp->elts[pc - 1].opcode)
    {
    default:
      operator_length_standard (exp, pc, oplenp, argsp);
      return;

    case UNOP_FORTRAN_KIND:
    case UNOP_FORTRAN_FLOOR:
    case UNOP_FORTRAN_CEILING:
    case UNOP_FORTRAN_ALLOCATED:
      oplen = 1;
      args = 1;
      break;

    case BINOP_FORTRAN_CMPLX:
    case BINOP_FORTRAN_MODULO:
      oplen = 1;
      args = 2;
      break;

    case FORTRAN_ASSOCIATED:
    case FORTRAN_LBOUND:
    case FORTRAN_UBOUND:
      oplen = 3;
      args = longest_to_int (exp->elts[pc - 2].longconst);
      break;

    case OP_F77_UNDETERMINED_ARGLIST:
      oplen = 3;
      args = 1 + longest_to_int (exp->elts[pc - 2].longconst);
      break;
    }

  *oplenp = oplen;
  *argsp = args;
}

/* Helper for PRINT_SUBEXP_F.  Arguments are as for PRINT_SUBEXP_F, except
   the extra argument NAME which is the text that should be printed as the
   name of this operation.  */

static void
print_unop_subexp_f (struct expression *exp, int *pos,
		     struct ui_file *stream, enum precedence prec,
		     const char *name)
{
  (*pos)++;
  fprintf_filtered (stream, "%s(", name);
  print_subexp (exp, pos, stream, PREC_SUFFIX);
  fputs_filtered (")", stream);
}

/* Helper for PRINT_SUBEXP_F.  Arguments are as for PRINT_SUBEXP_F, except
   the extra argument NAME which is the text that should be printed as the
   name of this operation.  */

static void
print_binop_subexp_f (struct expression *exp, int *pos,
		      struct ui_file *stream, enum precedence prec,
		      const char *name)
{
  (*pos)++;
  fprintf_filtered (stream, "%s(", name);
  print_subexp (exp, pos, stream, PREC_SUFFIX);
  fputs_filtered (",", stream);
  print_subexp (exp, pos, stream, PREC_SUFFIX);
  fputs_filtered (")", stream);
}

/* Helper for PRINT_SUBEXP_F.  Arguments are as for PRINT_SUBEXP_F, except
   the extra argument NAME which is the text that should be printed as the
   name of this operation.  */

static void
print_unop_or_binop_subexp_f (struct expression *exp, int *pos,
			      struct ui_file *stream, enum precedence prec,
			      const char *name)
{
  unsigned nargs = longest_to_int (exp->elts[*pos + 1].longconst);
  (*pos) += 3;
  fprintf_filtered (stream, "%s (", name);
  for (unsigned tem = 0; tem < nargs; tem++)
    {
      if (tem != 0)
	fputs_filtered (", ", stream);
      print_subexp (exp, pos, stream, PREC_ABOVE_COMMA);
    }
  fputs_filtered (")", stream);
}

/* Special expression printing for Fortran.  */

static void
print_subexp_f (struct expression *exp, int *pos,
		struct ui_file *stream, enum precedence prec)
{
  int pc = *pos;
  enum exp_opcode op = exp->elts[pc].opcode;

  switch (op)
    {
    default:
      print_subexp_standard (exp, pos, stream, prec);
      return;

    case UNOP_FORTRAN_KIND:
      print_unop_subexp_f (exp, pos, stream, prec, "KIND");
      return;

    case UNOP_FORTRAN_FLOOR:
      print_unop_subexp_f (exp, pos, stream, prec, "FLOOR");
      return;

    case UNOP_FORTRAN_CEILING:
      print_unop_subexp_f (exp, pos, stream, prec, "CEILING");
      return;

    case UNOP_FORTRAN_ALLOCATED:
      print_unop_subexp_f (exp, pos, stream, prec, "ALLOCATED");
      return;

    case BINOP_FORTRAN_CMPLX:
      print_binop_subexp_f (exp, pos, stream, prec, "CMPLX");
      return;

    case BINOP_FORTRAN_MODULO:
      print_binop_subexp_f (exp, pos, stream, prec, "MODULO");
      return;

    case FORTRAN_ASSOCIATED:
      print_unop_or_binop_subexp_f (exp, pos, stream, prec, "ASSOCIATED");
      return;

    case FORTRAN_LBOUND:
      print_unop_or_binop_subexp_f (exp, pos, stream, prec, "LBOUND");
      return;

    case FORTRAN_UBOUND:
      print_unop_or_binop_subexp_f (exp, pos, stream, prec, "UBOUND");
      return;

    case OP_F77_UNDETERMINED_ARGLIST:
      (*pos)++;
      print_subexp_funcall (exp, pos, stream);
      return;
    }
}

/* Special expression dumping for Fortran.  */

static int
dump_subexp_body_f (struct expression *exp,
		    struct ui_file *stream, int elt)
{
  int opcode = exp->elts[elt].opcode;
  int oplen, nargs, i;

  switch (opcode)
    {
    default:
      return dump_subexp_body_standard (exp, stream, elt);

    case UNOP_FORTRAN_KIND:
    case UNOP_FORTRAN_FLOOR:
    case UNOP_FORTRAN_CEILING:
    case UNOP_FORTRAN_ALLOCATED:
    case BINOP_FORTRAN_CMPLX:
    case BINOP_FORTRAN_MODULO:
      operator_length_f (exp, (elt + 1), &oplen, &nargs);
      break;

    case FORTRAN_ASSOCIATED:
    case FORTRAN_LBOUND:
    case FORTRAN_UBOUND:
      operator_length_f (exp, (elt + 3), &oplen, &nargs);
      break;

    case OP_F77_UNDETERMINED_ARGLIST:
      return dump_subexp_body_funcall (exp, stream, elt + 1);
    }

  elt += oplen;
  for (i = 0; i < nargs; i += 1)
    elt = dump_subexp (exp, stream, elt);

  return elt;
}

/* Special expression checking for Fortran.  */

static int
operator_check_f (struct expression *exp, int pos,
		  int (*objfile_func) (struct objfile *objfile,
				       void *data),
		  void *data)
{
  const union exp_element *const elts = exp->elts;

  switch (elts[pos].opcode)
    {
    case UNOP_FORTRAN_KIND:
    case UNOP_FORTRAN_FLOOR:
    case UNOP_FORTRAN_CEILING:
    case UNOP_FORTRAN_ALLOCATED:
    case BINOP_FORTRAN_CMPLX:
    case BINOP_FORTRAN_MODULO:
    case FORTRAN_ASSOCIATED:
    case FORTRAN_LBOUND:
    case FORTRAN_UBOUND:
      /* Any references to objfiles are held in the arguments to this
	 expression, not within the expression itself, so no additional
	 checking is required here, the outer expression iteration code
	 will take care of checking each argument.  */
      break;

    default:
      return operator_check_standard (exp, pos, objfile_func, data);
    }

  return 0;
}

/* Expression processing for Fortran.  */
const struct exp_descriptor f_language::exp_descriptor_tab =
{
  print_subexp_f,
  operator_length_f,
  operator_check_f,
  dump_subexp_body_f,
  evaluate_subexp_f
};

/* See language.h.  */

void
f_language::language_arch_info (struct gdbarch *gdbarch,
				struct language_arch_info *lai) const
{
  const struct builtin_f_type *builtin = builtin_f_type (gdbarch);

  /* Helper function to allow shorter lines below.  */
  auto add  = [&] (struct type * t)
  {
    lai->add_primitive_type (t);
  };

  add (builtin->builtin_character);
  add (builtin->builtin_logical);
  add (builtin->builtin_logical_s1);
  add (builtin->builtin_logical_s2);
  add (builtin->builtin_logical_s8);
  add (builtin->builtin_real);
  add (builtin->builtin_real_s8);
  add (builtin->builtin_real_s16);
  add (builtin->builtin_complex_s8);
  add (builtin->builtin_complex_s16);
  add (builtin->builtin_void);

  lai->set_string_char_type (builtin->builtin_character);
  lai->set_bool_type (builtin->builtin_logical_s2, "logical");
}

/* See language.h.  */

unsigned int
f_language::search_name_hash (const char *name) const
{
  return cp_search_name_hash (name);
}

/* See language.h.  */

struct block_symbol
f_language::lookup_symbol_nonlocal (const char *name,
				    const struct block *block,
				    const domain_enum domain) const
{
  return cp_lookup_symbol_nonlocal (this, name, block, domain);
}

/* See language.h.  */

symbol_name_matcher_ftype *
f_language::get_symbol_name_matcher_inner
	(const lookup_name_info &lookup_name) const
{
  return cp_get_symbol_name_matcher (lookup_name);
}

/* Single instance of the Fortran language class.  */

static f_language f_language_defn;

static void *
build_fortran_types (struct gdbarch *gdbarch)
{
  struct builtin_f_type *builtin_f_type
    = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct builtin_f_type);

  builtin_f_type->builtin_void
    = arch_type (gdbarch, TYPE_CODE_VOID, TARGET_CHAR_BIT, "void");

  builtin_f_type->builtin_character
    = arch_type (gdbarch, TYPE_CODE_CHAR, TARGET_CHAR_BIT, "character");

  builtin_f_type->builtin_logical_s1
    = arch_boolean_type (gdbarch, TARGET_CHAR_BIT, 1, "logical*1");

  builtin_f_type->builtin_integer_s2
    = arch_integer_type (gdbarch, gdbarch_short_bit (gdbarch), 0,
			 "integer*2");

  builtin_f_type->builtin_integer_s8
    = arch_integer_type (gdbarch, gdbarch_long_long_bit (gdbarch), 0,
			 "integer*8");

  builtin_f_type->builtin_logical_s2
    = arch_boolean_type (gdbarch, gdbarch_short_bit (gdbarch), 1,
			 "logical*2");

  builtin_f_type->builtin_logical_s8
    = arch_boolean_type (gdbarch, gdbarch_long_long_bit (gdbarch), 1,
			 "logical*8");

  builtin_f_type->builtin_integer
    = arch_integer_type (gdbarch, gdbarch_int_bit (gdbarch), 0,
			 "integer");

  builtin_f_type->builtin_logical
    = arch_boolean_type (gdbarch, gdbarch_int_bit (gdbarch), 1,
			 "logical*4");

  builtin_f_type->builtin_real
    = arch_float_type (gdbarch, gdbarch_float_bit (gdbarch),
		       "real", gdbarch_float_format (gdbarch));
  builtin_f_type->builtin_real_s8
    = arch_float_type (gdbarch, gdbarch_double_bit (gdbarch),
		       "real*8", gdbarch_double_format (gdbarch));
  auto fmt = gdbarch_floatformat_for_type (gdbarch, "real(kind=16)", 128);
  if (fmt != nullptr)
    builtin_f_type->builtin_real_s16
      = arch_float_type (gdbarch, 128, "real*16", fmt);
  else if (gdbarch_long_double_bit (gdbarch) == 128)
    builtin_f_type->builtin_real_s16
      = arch_float_type (gdbarch, gdbarch_long_double_bit (gdbarch),
			 "real*16", gdbarch_long_double_format (gdbarch));
  else
    builtin_f_type->builtin_real_s16
      = arch_type (gdbarch, TYPE_CODE_ERROR, 128, "real*16");

  builtin_f_type->builtin_complex_s8
    = init_complex_type ("complex*8", builtin_f_type->builtin_real);
  builtin_f_type->builtin_complex_s16
    = init_complex_type ("complex*16", builtin_f_type->builtin_real_s8);

  if (builtin_f_type->builtin_real_s16->code () == TYPE_CODE_ERROR)
    builtin_f_type->builtin_complex_s32
      = arch_type (gdbarch, TYPE_CODE_ERROR, 256, "complex*32");
  else
    builtin_f_type->builtin_complex_s32
      = init_complex_type ("complex*32", builtin_f_type->builtin_real_s16);

  return builtin_f_type;
}

static struct gdbarch_data *f_type_data;

const struct builtin_f_type *
builtin_f_type (struct gdbarch *gdbarch)
{
  return (const struct builtin_f_type *) gdbarch_data (gdbarch, f_type_data);
}

/* Command-list for the "set/show fortran" prefix command.  */
static struct cmd_list_element *set_fortran_list;
static struct cmd_list_element *show_fortran_list;

void _initialize_f_language ();
void
_initialize_f_language ()
{
  f_type_data = gdbarch_data_register_post_init (build_fortran_types);

  add_basic_prefix_cmd ("fortran", no_class,
			_("Prefix command for changing Fortran-specific settings."),
			&set_fortran_list, "set fortran ", 0, &setlist);

  add_show_prefix_cmd ("fortran", no_class,
		       _("Generic command for showing Fortran-specific settings."),
		       &show_fortran_list, "show fortran ", 0, &showlist);

  add_setshow_boolean_cmd ("repack-array-slices", class_vars,
			   &repack_array_slices, _("\
Enable or disable repacking of non-contiguous array slices."), _("\
Show whether non-contiguous array slices are repacked."), _("\
When the user requests a slice of a Fortran array then we can either return\n\
a descriptor that describes the array in place (using the original array data\n\
in its existing location) or the original data can be repacked (copied) to a\n\
new location.\n\
\n\
When the content of the array slice is contiguous within the original array\n\
then the result will never be repacked, but when the data for the new array\n\
is non-contiguous within the original array repacking will only be performed\n\
when this setting is on."),
			   NULL,
			   show_repack_array_slices,
			   &set_fortran_list, &show_fortran_list);

  /* Debug Fortran's array slicing logic.  */
  add_setshow_boolean_cmd ("fortran-array-slicing", class_maintenance,
			   &fortran_array_slicing_debug, _("\
Set debugging of Fortran array slicing."), _("\
Show debugging of Fortran array slicing."), _("\
When on, debugging of Fortran array slicing is enabled."),
			    NULL,
			    show_fortran_array_slicing_debug,
			    &setdebuglist, &showdebuglist);
}

/* Ensures that function argument VALUE is in the appropriate form to
   pass to a Fortran function.  Returns a possibly new value that should
   be used instead of VALUE.

   When IS_ARTIFICIAL is true this indicates an artificial argument,
   e.g. hidden string lengths which the GNU Fortran argument passing
   convention specifies as being passed by value.

   When IS_ARTIFICIAL is false, the argument is passed by pointer.  If the
   value is already in target memory then return a value that is a pointer
   to VALUE.  If VALUE is not in memory (e.g. an integer literal), allocate
   space in the target, copy VALUE in, and return a pointer to the in
   memory copy.  */

static struct value *
fortran_argument_convert (struct value *value, bool is_artificial)
{
  if (!is_artificial)
    {
      /* If the value is not in the inferior e.g. registers values,
	 convenience variables and user input.  */
      if (VALUE_LVAL (value) != lval_memory)
	{
	  struct type *type = value_type (value);
	  const int length = TYPE_LENGTH (type);
	  const CORE_ADDR addr
	    = value_as_long (value_allocate_space_in_inferior (length));
	  write_memory (addr, value_contents (value), length);
	  struct value *val
	    = value_from_contents_and_address (type, value_contents (value),
					       addr);
	  return value_addr (val);
	}
      else
	return value_addr (value); /* Program variables, e.g. arrays.  */
    }
    return value;
}

/* Prepare (and return) an argument value ready for an inferior function
   call to a Fortran function.  EXP and POS are the expressions describing
   the argument to prepare.  ARG_NUM is the argument number being
   prepared, with 0 being the first argument and so on.  FUNC_TYPE is the
   type of the function being called.

   IS_INTERNAL_CALL_P is true if this is a call to a function of type
   TYPE_CODE_INTERNAL_FUNCTION, otherwise this parameter is false.

   NOSIDE has its usual meaning for expression parsing (see eval.c).

   Arguments in Fortran are normally passed by address, we coerce the
   arguments here rather than in value_arg_coerce as otherwise the call to
   malloc (to place the non-lvalue parameters in target memory) is hit by
   this Fortran specific logic.  This results in malloc being called with a
   pointer to an integer followed by an attempt to malloc the arguments to
   malloc in target memory.  Infinite recursion ensues.  */

static value *
fortran_prepare_argument (struct expression *exp, int *pos,
                         int arg_num, bool is_internal_call_p,
                         struct type *func_type, enum noside noside)
{
  if (is_internal_call_p)
    return evaluate_subexp_with_coercion (exp, pos, noside);

  bool is_artificial = ((arg_num >= func_type->num_fields ())
			? true
			: TYPE_FIELD_ARTIFICIAL (func_type, arg_num));

  /* If this is an artificial argument, then either, this is an argument
     beyond the end of the known arguments, or possibly, there are no known
     arguments (maybe missing debug info).

     For these artificial arguments, if the user has prefixed it with '&'
     (for address-of), then lets always allow this to succeed, even if the
     argument is not actually in inferior memory.  This will allow the user
     to pass arguments to a Fortran function even when there's no debug
     information.

     As we already pass the address of non-artificial arguments, all we
     need to do if skip the UNOP_ADDR operator in the expression and mark
     the argument as non-artificial.  */
  if (is_artificial && exp->elts[*pos].opcode == UNOP_ADDR)
    {
      (*pos)++;
      is_artificial = false;
    }

  struct value *arg_val = evaluate_subexp_with_coercion (exp, pos, noside);
  return fortran_argument_convert (arg_val, is_artificial);
}

/* See f-lang.h.  */

struct type *
fortran_preserve_arg_pointer (struct value *arg, struct type *type)
{
  if (value_type (arg)->code () == TYPE_CODE_PTR)
    return value_type (arg);
  return type;
}

/* See f-lang.h.  */

CORE_ADDR
fortran_adjust_dynamic_array_base_address_hack (struct type *type,
						CORE_ADDR address)
{
  gdb_assert (type->code () == TYPE_CODE_ARRAY);

  /* We can't adjust the base address for arrays that have no content.  */
  if (type_not_allocated (type) || type_not_associated (type))
    return address;

  int ndimensions = calc_f77_array_dims (type);
  LONGEST total_offset = 0;

  /* Walk through each of the dimensions of this array type and figure out
     if any of the dimensions are "backwards", that is the base address
     for this dimension points to the element at the highest memory
     address and the stride is negative.  */
  struct type *tmp_type = type;
  for (int i = 0 ; i < ndimensions; ++i)
    {
      /* Grab the range for this dimension and extract the lower and upper
	 bounds.  */
      tmp_type = check_typedef (tmp_type);
      struct type *range_type = tmp_type->index_type ();
      LONGEST lowerbound, upperbound, stride;
      if (!get_discrete_bounds (range_type, &lowerbound, &upperbound))
	error ("failed to get range bounds");

      /* Figure out the stride for this dimension.  */
      struct type *elt_type = check_typedef (TYPE_TARGET_TYPE (tmp_type));
      stride = tmp_type->index_type ()->bounds ()->bit_stride ();
      if (stride == 0)
	stride = type_length_units (elt_type);
      else
	{
	  int unit_size
	    = gdbarch_addressable_memory_unit_size (elt_type->arch ());
	  stride /= (unit_size * 8);
	}

      /* If this dimension is "backward" then figure out the offset
	 adjustment required to point to the element at the lowest memory
	 address, and add this to the total offset.  */
      LONGEST offset = 0;
      if (stride < 0 && lowerbound < upperbound)
	offset = (upperbound - lowerbound) * stride;
      total_offset += offset;
      tmp_type = TYPE_TARGET_TYPE (tmp_type);
    }

  /* Adjust the address of this object and return it.  */
  address += total_offset;
  return address;
}
