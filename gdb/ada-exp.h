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
extern struct value *ada_equal_binop (struct type *expect_type,
				      struct expression *exp,
				      enum noside noside, enum exp_opcode op,
				      struct value *arg1, struct value *arg2);
extern struct value *ada_ternop_slice (struct expression *exp,
				       enum noside noside,
				       struct value *array,
				       struct value *low_bound_val,
				       struct value *high_bound_val);
extern struct value *ada_binop_in_bounds (struct expression *exp,
					  enum noside noside,
					  struct value *arg1,
					  struct value *arg2,
					  int n);
extern struct value *ada_binop_minmax (struct type *expect_type,
				       struct expression *exp,
				       enum noside noside, enum exp_opcode op,
				       struct value *arg1,
				       struct value *arg2);
extern struct value *ada_pos_atr (struct type *expect_type,
				  struct expression *exp,
				  enum noside noside, enum exp_opcode op,
				  struct value *arg);
extern struct value *ada_val_atr (enum noside noside, struct type *type,
				  struct value *arg);
extern struct value *ada_binop_exp (struct type *expect_type,
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
using ada_pos_operation = unop_operation<OP_ATR_POS, ada_pos_atr>;

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

using ada_binop_min_operation = binop_operation<OP_ATR_MIN, ada_binop_minmax>;
using ada_binop_max_operation = binop_operation<OP_ATR_MAX, ada_binop_minmax>;

using ada_binop_exp_operation = binop_operation<BINOP_EXP, ada_binop_exp>;

/* Implement the equal and not-equal operations for Ada.  */
class ada_binop_equal_operation
  : public tuple_holding_operation<enum exp_opcode, operation_up, operation_up>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    value *arg1 = std::get<1> (m_storage)->evaluate (nullptr, exp, noside);
    value *arg2 = std::get<2> (m_storage)->evaluate (value_type (arg1),
						     exp, noside);
    return ada_equal_binop (expect_type, exp, noside, std::get<0> (m_storage),
			    arg1, arg2);
  }

  enum exp_opcode opcode () const override
  { return std::get<0> (m_storage); }
};

/* Bitwise operators for Ada.  */
template<enum exp_opcode OP>
class ada_bitwise_operation
  : public maybe_constant_operation<operation_up, operation_up>
{
public:

  using maybe_constant_operation::maybe_constant_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    value *lhs = std::get<0> (m_storage)->evaluate (nullptr, exp, noside);
    value *rhs = std::get<1> (m_storage)->evaluate (nullptr, exp, noside);
    value *result = eval_op_binary (expect_type, exp, noside, OP, lhs, rhs);
    return value_cast (value_type (lhs), result);
  }

  enum exp_opcode opcode () const override
  { return OP; }
};

using ada_bitwise_and_operation = ada_bitwise_operation<BINOP_BITWISE_AND>;
using ada_bitwise_ior_operation = ada_bitwise_operation<BINOP_BITWISE_IOR>;
using ada_bitwise_xor_operation = ada_bitwise_operation<BINOP_BITWISE_XOR>;

/* Ada array- or string-slice operation.  */
class ada_ternop_slice_operation
  : public maybe_constant_operation<operation_up, operation_up, operation_up>
{
public:

  using maybe_constant_operation::maybe_constant_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    value *array = std::get<0> (m_storage)->evaluate (nullptr, exp, noside);
    value *low = std::get<1> (m_storage)->evaluate (nullptr, exp, noside);
    value *high = std::get<2> (m_storage)->evaluate (nullptr, exp, noside);
    return ada_ternop_slice (exp, noside, array, low, high);
  }

  enum exp_opcode opcode () const override
  { return TERNOP_SLICE; }
};

/* Implement BINOP_IN_BOUNDS for Ada.  */
class ada_binop_in_bounds_operation
  : public maybe_constant_operation<operation_up, operation_up, int>
{
public:

  using maybe_constant_operation::maybe_constant_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    value *arg1 = std::get<0> (m_storage)->evaluate (nullptr, exp, noside);
    value *arg2 = std::get<1> (m_storage)->evaluate (nullptr, exp, noside);
    return ada_binop_in_bounds (exp, noside, arg1, arg2,
				std::get<2> (m_storage));
  }

  enum exp_opcode opcode () const override
  { return BINOP_IN_BOUNDS; }
};

/* Implement several unary Ada OP_ATR_* operations.  */
class ada_unop_atr_operation
  : public maybe_constant_operation<operation_up, enum exp_opcode, int>
{
public:

  using maybe_constant_operation::maybe_constant_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override;

  enum exp_opcode opcode () const override
  { return std::get<1> (m_storage); }
};

/* Variant of var_value_operation for Ada.  */
class ada_var_value_operation
  : public var_value_operation
{
public:

  using var_value_operation::var_value_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override;

  value *evaluate_for_cast (struct type *expect_type,
			    struct expression *exp,
			    enum noside noside) override;

protected:

  using operation::do_generate_ax;
};

/* Variant of var_msym_value_operation for Ada.  */
class ada_var_msym_value_operation
  : public var_msym_value_operation
{
public:

  using var_msym_value_operation::var_msym_value_operation;

  value *evaluate_for_cast (struct type *expect_type,
			    struct expression *exp,
			    enum noside noside) override;

protected:

  using operation::do_generate_ax;
};

/* Implement the Ada 'val attribute.  */
class ada_atr_val_operation
  : public tuple_holding_operation<struct type *, operation_up>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override;

  enum exp_opcode opcode () const override
  { return OP_ATR_VAL; }
};

} /* namespace expr */

#endif /* ADA_EXP_H */
