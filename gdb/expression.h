/* Definitions for expressions stored in reversed prefix form, for GDB.

   Copyright (C) 1986-2021 Free Software Foundation, Inc.

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

#if !defined (EXPRESSION_H)
#define EXPRESSION_H 1

#include "gdbtypes.h"

/* While parsing expressions we need to track the innermost lexical block
   that we encounter.  In some situations we need to track the innermost
   block just for symbols, and in other situations we want to track the
   innermost block for symbols and registers.  These flags are used by the
   innermost block tracker to control which blocks we consider for the
   innermost block.  These flags can be combined together as needed.  */

enum innermost_block_tracker_type
{
  /* Track the innermost block for symbols within an expression.  */
  INNERMOST_BLOCK_FOR_SYMBOLS = (1 << 0),

  /* Track the innermost block for registers within an expression.  */
  INNERMOST_BLOCK_FOR_REGISTERS = (1 << 1)
};
DEF_ENUM_FLAGS_TYPE (enum innermost_block_tracker_type,
		     innermost_block_tracker_types);

/* Definitions for saved C expressions.  */

/* An expression is represented as a vector of union exp_element's.
   Each exp_element is an opcode, except that some opcodes cause
   the following exp_element to be treated as a long or double constant
   or as a variable.  The opcodes are obeyed, using a stack for temporaries.
   The value is left on the temporary stack at the end.  */

/* When it is necessary to include a string,
   it can occupy as many exp_elements as it needs.
   We find the length of the string using strlen,
   divide to find out how many exp_elements are used up,
   and skip that many.  Strings, like numbers, are indicated
   by the preceding opcode.  */

enum exp_opcode : uint8_t
  {
#define OP(name) name ,

#include "std-operator.def"

#undef OP

    /* Existing only to swallow the last comma (',') from last .inc file.  */
    OP_UNUSED_LAST
  };

/* Values of NOSIDE argument to eval_subexp.  */

enum noside
  {
    EVAL_NORMAL,
    EVAL_SKIP,			/* Only effect is to increment pos.
				   Return type information where
				   possible.  */
    EVAL_AVOID_SIDE_EFFECTS	/* Don't modify any variables or
				   call any functions.  The value
				   returned will have the correct
				   type, and will have an
				   approximately correct lvalue
				   type (inaccuracy: anything that is
				   listed as being in a register in
				   the function in which it was
				   declared will be lval_register).
				   Ideally this would not even read
				   target memory, but currently it
				   does in many situations.  */
  };

union exp_element
  {
    enum exp_opcode opcode;
    struct symbol *symbol;
    struct minimal_symbol *msymbol;
    LONGEST longconst;
    gdb_byte floatconst[16];
    /* Really sizeof (union exp_element) characters (or less for the last
       element of a string).  */
    char string;
    struct type *type;
    struct internalvar *internalvar;
    const struct block *block;
    struct objfile *objfile;
  };

struct expression
{
  expression (const struct language_defn *, struct gdbarch *, size_t);
  ~expression ();
  DISABLE_COPY_AND_ASSIGN (expression);

  void resize (size_t);

  /* Return the opcode for the outermost sub-expression of this
     expression.  */
  enum exp_opcode first_opcode () const
  {
      return elts[0].opcode;
  }

  /* Language it was entered in.  */
  const struct language_defn *language_defn;
  /* Architecture it was parsed in.  */
  struct gdbarch *gdbarch;
  int nelts = 0;
  union exp_element *elts;
};

typedef std::unique_ptr<expression> expression_up;

/* Macros for converting between number of expression elements and bytes
   to store that many expression elements.  */

#define EXP_ELEM_TO_BYTES(elements) \
    ((elements) * sizeof (union exp_element))
#define BYTES_TO_EXP_ELEM(bytes) \
    (((bytes) + sizeof (union exp_element) - 1) / sizeof (union exp_element))

/* From parse.c */

class innermost_block_tracker;
extern expression_up parse_expression (const char *,
				       innermost_block_tracker * = nullptr,
				       bool void_context_p = false);

extern expression_up parse_expression_with_language (const char *string,
						     enum language lang);

extern struct type *parse_expression_for_completion
    (const char *, gdb::unique_xmalloc_ptr<char> *, enum type_code *);

class innermost_block_tracker;
extern expression_up parse_exp_1 (const char **, CORE_ADDR pc,
				  const struct block *, int,
				  innermost_block_tracker * = nullptr);

/* From eval.c */

extern struct value *evaluate_subexp_standard
  (struct type *, struct expression *, int *, enum noside);

/* Evaluate a function call.  The function to be called is in CALLEE and
   the arguments passed to the function are in ARGVEC.
   FUNCTION_NAME is the name of the function, if known.
   DEFAULT_RETURN_TYPE is used as the function's return type if the return
   type is unknown.  */

extern struct value *evaluate_subexp_do_call (expression *exp,
					      enum noside noside,
					      value *callee,
					      gdb::array_view<value *> argvec,
					      const char *function_name,
					      type *default_return_type);

/* From expprint.c */

extern void print_expression (struct expression *, struct ui_file *);

extern const char *op_name (enum exp_opcode opcode);

extern const char *op_string (enum exp_opcode);

extern void dump_raw_expression (struct expression *,
				 struct ui_file *, const char *);
extern void dump_prefix_expression (struct expression *, struct ui_file *);

/* In an OP_RANGE expression, either bound could be empty, indicating
   that its value is by default that of the corresponding bound of the
   array or string.  Also, the upper end of the range can be exclusive
   or inclusive.  So we have six sorts of subrange.  This enumeration
   type is to identify this.  */

enum range_flag : unsigned
{
  /* This is a standard range.  Both the lower and upper bounds are
     defined, and the bounds are inclusive.  */
  RANGE_STANDARD = 0,

  /* The low bound was not given.  */
  RANGE_LOW_BOUND_DEFAULT = 1 << 0,

  /* The high bound was not given.  */
  RANGE_HIGH_BOUND_DEFAULT = 1 << 1,

  /* The high bound of this range is exclusive.  */
  RANGE_HIGH_BOUND_EXCLUSIVE = 1 << 2,

  /* The range has a stride.  */
  RANGE_HAS_STRIDE = 1 << 3,
};

DEF_ENUM_FLAGS_TYPE (enum range_flag, range_flags);

#endif /* !defined (EXPRESSION_H) */
