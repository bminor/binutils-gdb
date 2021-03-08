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

}/* namespace expr */

#endif /* C_EXP_H */
