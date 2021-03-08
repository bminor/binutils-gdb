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

} /* namespace expr */

#endif /* ADA_EXP_H */
