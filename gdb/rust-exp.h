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

namespace expr
{

using rust_unop_compl_operation = unop_operation<UNOP_COMPLEMENT,
						  eval_op_rust_complement>;
using rust_array_operation = binop_operation<OP_RUST_ARRAY,
					     eval_op_rust_array>;

} /* namespace expr */

#endif /* RUST_EXP_H */
