/* Print in infix form a struct expression.

   Copyright (C) 1986-2022 Free Software Foundation, Inc.

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
#include "value.h"
#include "language.h"
#include "parser-defs.h"
#include "user-regs.h"		/* For user_reg_map_regnum_to_name.  */
#include "target.h"
#include "block.h"
#include "objfiles.h"
#include "valprint.h"
#include "cli/cli-style.h"
#include "c-lang.h"
#include "expop.h"
#include "ada-exp.h"

#include <ctype.h>

/* Default name for the standard operator OPCODE (i.e., one defined in
   the definition of enum exp_opcode).  */

const char *
op_name (enum exp_opcode opcode)
{
  switch (opcode)
    {
    default:
      {
	static char buf[30];

	xsnprintf (buf, sizeof (buf), "<unknown %d>", opcode);
	return buf;
      }
#define OP(name)	\
    case name:		\
      return #name ;
#include "std-operator.def"
#undef OP
    }
}

void
dump_prefix_expression (struct expression *exp, struct ui_file *stream)
{
  exp->op->dump (stream, 0);
}

namespace expr
{

void
dump_for_expression (struct ui_file *stream, int depth, enum exp_opcode op)
{
  fprintf_filtered (stream, _("%*sOperation: %s\n"), depth, "", op_name (op));
}

void
dump_for_expression (struct ui_file *stream, int depth, const std::string &str)
{
  fprintf_filtered (stream, _("%*sString: %s\n"), depth, "", str.c_str ());
}

void
dump_for_expression (struct ui_file *stream, int depth, struct type *type)
{
  fprintf_filtered (stream, _("%*sType: "), depth, "");
  type_print (type, nullptr, stream, 0);
  fprintf_filtered (stream, "\n");
}

void
dump_for_expression (struct ui_file *stream, int depth, CORE_ADDR addr)
{
  fprintf_filtered (stream, _("%*sConstant: %s\n"), depth, "",
		     core_addr_to_string (addr));
}

void
dump_for_expression (struct ui_file *stream, int depth, internalvar *ivar)
{
  fprintf_filtered (stream, _("%*sInternalvar: $%s\n"), depth, "",
		     internalvar_name (ivar));
}

void
dump_for_expression (struct ui_file *stream, int depth, symbol *sym)
{
  fprintf_filtered (stream, _("%*sSymbol: %s\n"), depth, "",
		     sym->print_name ());
}

void
dump_for_expression (struct ui_file *stream, int depth,
		     bound_minimal_symbol msym)
{
  fprintf_filtered (stream, _("%*sMinsym %s in objfile %s\n"), depth, "",
		    msym.minsym->print_name (), objfile_name (msym.objfile));
}

void
dump_for_expression (struct ui_file *stream, int depth, const block *bl)
{
  fprintf_filtered (stream, _("%*sBlock: %p\n"), depth, "", bl);
}

void
dump_for_expression (struct ui_file *stream, int depth,
		     const block_symbol &sym)
{
  fprintf_filtered (stream, _("%*sBlock symbol:\n"), depth, "");
  dump_for_expression (stream, depth + 1, sym.symbol);
  dump_for_expression (stream, depth + 1, sym.block);
}

void
dump_for_expression (struct ui_file *stream, int depth,
		     type_instance_flags flags)
{
  fprintf_filtered (stream, _("%*sType flags: "), depth, "");
  if (flags & TYPE_INSTANCE_FLAG_CONST)
    fputs_filtered ("const ", stream);
  if (flags & TYPE_INSTANCE_FLAG_VOLATILE)
    fputs_filtered ("volatile", stream);
  fprintf_filtered (stream, "\n");
}

void
dump_for_expression (struct ui_file *stream, int depth,
		     enum c_string_type_values flags)
{
  fprintf_filtered (stream, _("%*sC string flags: "), depth, "");
  switch (flags & ~C_CHAR)
    {
    case C_WIDE_STRING:
      fputs_filtered (_("wide "), stream);
      break;
    case C_STRING_16:
      fputs_filtered (_("u16 "), stream);
      break;
    case C_STRING_32:
      fputs_filtered (_("u32 "), stream);
      break;
    default:
      fputs_filtered (_("ordinary "), stream);
      break;
    }

  if ((flags & C_CHAR) != 0)
    fputs_filtered (_("char"), stream);
  else
    fputs_filtered (_("string"), stream);
  fputs_filtered ("\n", stream);
}

void
dump_for_expression (struct ui_file *stream, int depth,
		     enum range_flag flags)
{
  fprintf_filtered (stream, _("%*sRange:"), depth, "");
  if ((flags & RANGE_LOW_BOUND_DEFAULT) != 0)
    fputs_filtered (_("low-default "), stream);
  if ((flags & RANGE_HIGH_BOUND_DEFAULT) != 0)
    fputs_filtered (_("high-default "), stream);
  if ((flags & RANGE_HIGH_BOUND_EXCLUSIVE) != 0)
    fputs_filtered (_("high-exclusive "), stream);
  if ((flags & RANGE_HAS_STRIDE) != 0)
    fputs_filtered (_("has-stride"), stream);
  fprintf_filtered (stream, "\n");
}

void
dump_for_expression (struct ui_file *stream, int depth,
		     const std::unique_ptr<ada_component> &comp)
{
  comp->dump (stream, depth);
}

void
float_const_operation::dump (struct ui_file *stream, int depth) const
{
  fprintf_filtered (stream, _("%*sFloat: "), depth, "");
  print_floating (m_data.data (), m_type, stream);
  fprintf_filtered (stream, "\n");
}

} /* namespace expr */
