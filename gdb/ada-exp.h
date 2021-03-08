/* Definitions for Ada expressions

   Copyright (C) 2020 Free Software Foundation, Inc.

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

#ifndef ADA_EXP_H
#define ADA_EXP_H

#include "expop.h"

extern struct value *ada_unop_neg (struct type *expect_type,
				   struct expression *exp,
				   enum noside noside, enum exp_opcode op,
				   struct value *arg1);
extern struct value *ada_atr_tag (struct type *expect_type,
				  struct expression *exp,
				  enum noside noside, enum exp_opcode op,
				  struct value *arg1);
extern struct value *ada_atr_size (struct type *expect_type,
				   struct expression *exp,
				   enum noside noside, enum exp_opcode op,
				   struct value *arg1);
extern struct value *ada_abs (struct type *expect_type,
			      struct expression *exp,
			      enum noside noside, enum exp_opcode op,
			      struct value *arg1);
extern struct value *ada_unop_in_range (struct type *expect_type,
					struct expression *exp,
					enum noside noside, enum exp_opcode op,
					struct value *arg1, struct type *type);
extern struct value *ada_mult_binop (struct type *expect_type,
				     struct expression *exp,
				     enum noside noside, enum exp_opcode op,
				     struct value *arg1, struct value *arg2);

namespace expr
{

/* In Ada, some generic operations must be wrapped with a handler that
   handles some Ada-specific type conversions.  */
class ada_wrapped_operation
  : public tuple_holding_operation<operation_up>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override;

  enum exp_opcode opcode () const override
  { return std::get<0> (m_storage)->opcode (); }
};

/* An Ada string constant.  */
class ada_string_operation
  : public string_operation
{
public:

  using string_operation::string_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override;
};

/* The Ada TYPE'(EXP) construct.  */
class ada_qual_operation
  : public tuple_holding_operation<operation_up, struct type *>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override;

  enum exp_opcode opcode () const override
  { return UNOP_QUAL; }
};

/* Ternary in-range operator.  */
class ada_ternop_range_operation
  : public tuple_holding_operation<operation_up, operation_up, operation_up>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override;

  enum exp_opcode opcode () const override
  { return TERNOP_IN_RANGE; }
};

using ada_neg_operation = unop_operation<UNOP_NEG, ada_unop_neg>;
using ada_atr_tag_operation = unop_operation<OP_ATR_TAG, ada_atr_tag>;
using ada_atr_size_operation = unop_operation<OP_ATR_SIZE, ada_atr_size>;
using ada_abs_operation = unop_operation<UNOP_ABS, ada_abs>;

/* The in-range operation, given a type.  */
class ada_unop_range_operation
  : public tuple_holding_operation<operation_up, struct type *>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    value *val = std::get<0> (m_storage)->evaluate (nullptr, exp, noside);
    return ada_unop_in_range (expect_type, exp, noside, UNOP_IN_RANGE,
			      val, std::get<1> (m_storage));
  }

  enum exp_opcode opcode () const override
  { return UNOP_IN_RANGE; }
};

/* The Ada + and - operators.  */
class ada_binop_addsub_operation
  : public tuple_holding_operation<enum exp_opcode, operation_up, operation_up>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override;

  enum exp_opcode opcode () const override
  { return std::get<0> (m_storage); }
};

using ada_binop_mul_operation = binop_operation<BINOP_MUL, ada_mult_binop>;
using ada_binop_div_operation = binop_operation<BINOP_DIV, ada_mult_binop>;
using ada_binop_rem_operation = binop_operation<BINOP_REM, ada_mult_binop>;
using ada_binop_mod_operation = binop_operation<BINOP_MOD, ada_mult_binop>;

} /* namespace expr */

#endif /* ADA_EXP_H */
