/* Definitions for C expressions

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

#ifndef C_EXP_H
#define C_EXP_H

#include "expop.h"
#include "objc-lang.h"

extern struct value *eval_op_objc_selector (struct type *expect_type,
					    struct expression *exp,
					    enum noside noside,
					    const char *sel);
extern struct value *opencl_value_cast (struct type *type, struct value *arg);

namespace expr
{

class c_string_operation
  : public tuple_holding_operation<enum c_string_type_values,
				   std::vector<std::string>>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override;

  enum exp_opcode opcode () const override
  { return OP_STRING; }
};

class objc_nsstring_operation
  : public tuple_holding_operation<std::string>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    if (noside == EVAL_SKIP)
      return eval_skip_value (exp);
    const std::string &str = std::get<0> (m_storage);
    return value_nsstring (exp->gdbarch, str.c_str (), str.size () + 1);
  }

  enum exp_opcode opcode () const override
  { return OP_OBJC_NSSTRING; }
};

class objc_selector_operation
  : public tuple_holding_operation<std::string>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    if (noside == EVAL_SKIP)
      return eval_skip_value (exp);
    return eval_op_objc_selector (expect_type, exp, noside,
				  std::get<0> (m_storage).c_str ());
  }

  enum exp_opcode opcode () const override
  { return OP_OBJC_SELECTOR; }
};

/* An Objective C message call.  */
class objc_msgcall_operation
  : public tuple_holding_operation<CORE_ADDR, operation_up,
				   std::vector<operation_up>>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override;

  enum exp_opcode opcode () const override
  { return OP_OBJC_MSGCALL; }
};

using opencl_cast_type_operation = cxx_cast_operation<UNOP_CAST_TYPE,
						      opencl_value_cast>;

}/* namespace expr */

#endif /* C_EXP_H */
