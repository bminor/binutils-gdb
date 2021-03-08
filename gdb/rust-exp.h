/* Definitions for Rust expressions

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

#ifndef RUST_EXP_H
#define RUST_EXP_H

#include "expop.h"

extern struct value *eval_op_rust_complement (struct type *expect_type,
					      struct expression *exp,
					      enum noside noside,
					      enum exp_opcode opcode,
					      struct value *value);
extern struct value *eval_op_rust_array (struct type *expect_type,
					 struct expression *exp,
					 enum noside noside,
					 enum exp_opcode opcode,
					 struct value *ncopies,
					 struct value *elt);
extern struct value *eval_op_rust_ind (struct type *expect_type,
				       struct expression *exp,
				       enum noside noside,
				       enum exp_opcode opcode,
				       struct value *value);
extern struct value *rust_subscript (struct type *expect_type,
				     struct expression *exp,
				     enum noside noside, bool for_addr,
				     struct value *lhs, struct value *rhs);

namespace expr
{

using rust_unop_compl_operation = unop_operation<UNOP_COMPLEMENT,
						  eval_op_rust_complement>;
using rust_array_operation = binop_operation<OP_RUST_ARRAY,
					     eval_op_rust_array>;

/* The Rust indirection operation.  */
class rust_unop_ind_operation
  : public unop_ind_operation
{
public:

  using unop_ind_operation::unop_ind_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    if (noside != EVAL_NORMAL)
      return unop_ind_operation::evaluate (expect_type, exp, noside);

    value *arg1 = std::get<0> (m_storage)->evaluate (nullptr, exp, noside);
    return eval_op_rust_ind (expect_type, exp, noside, UNOP_IND, arg1);
  }
};

/* Subscript operator for Rust.  */
class rust_subscript_operation
  : public tuple_holding_operation<operation_up, operation_up>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    value *arg1 = std::get<0> (m_storage)->evaluate (nullptr, exp, noside);
    value *arg2 = std::get<1> (m_storage)->evaluate (nullptr, exp, noside);
    return rust_subscript (expect_type, exp, noside, false, arg1, arg2);
  }

  value *slice (struct type *expect_type,
		struct expression *exp,
		enum noside noside)
  {
    value *arg1 = std::get<0> (m_storage)->evaluate (nullptr, exp, noside);
    value *arg2 = std::get<1> (m_storage)->evaluate (nullptr, exp, noside);
    return rust_subscript (expect_type, exp, noside, true, arg1, arg2);
  }

  enum exp_opcode opcode () const override
  { return BINOP_SUBSCRIPT; }
};

class rust_unop_addr_operation
  : public tuple_holding_operation<operation_up>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    operation *oper = std::get<0> (m_storage).get ();
    rust_subscript_operation *sub_op
      = dynamic_cast<rust_subscript_operation *> (oper);
    if (sub_op != nullptr)
      return sub_op->slice (expect_type, exp, noside);
    return oper->evaluate_for_address (exp, noside);
  }

  enum exp_opcode opcode () const override
  { return UNOP_ADDR; }
};

} /* namespace expr */

#endif /* RUST_EXP_H */
