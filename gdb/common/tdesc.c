/* Target description support for GDB.

   Copyright (C) 2018 Free Software Foundation, Inc.

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

#include "common-defs.h"
#include "common/tdesc.h"

tdesc_reg::tdesc_reg (struct tdesc_feature *feature, const std::string &name_,
		      int regnum, int save_restore_, const char *group_,
		      int bitsize_, const char *type_)
  : name (name_), target_regnum (regnum),
    save_restore (save_restore_),
    group (group_ != NULL ? group_ : ""),
    bitsize (bitsize_),
    type (type_ != NULL ? type_ : "<unknown>")
{
  /* If the register's type is target-defined, look it up now.  We may not
     have easy access to the containing feature when we want it later.  */
  tdesc_type = tdesc_named_type (feature, type.c_str ());
}

void tdesc_feature::accept (tdesc_element_visitor &v) const
{
  v.visit_pre (this);

  for (const tdesc_type_up &type : types)
    type->accept (v);

  for (const tdesc_reg_up &reg : registers)
    reg->accept (v);

  v.visit_post (this);
}

bool tdesc_feature::operator== (const tdesc_feature &other) const
{
  if (name != other.name)
    return false;

  if (registers.size () != other.registers.size ())
    return false;

  for (int ix = 0; ix < registers.size (); ix++)
    {
      const tdesc_reg_up &reg1 = registers[ix];
      const tdesc_reg_up &reg2 = other.registers[ix];

      if (reg1 != reg2 && *reg1 != *reg2)
	return false;
      }

  if (types.size () != other.types.size ())
    return false;

  for (int ix = 0; ix < types.size (); ix++)
    {
      const tdesc_type_up &type1 = types[ix];
      const tdesc_type_up &type2 = other.types[ix];

      if (type1 != type2 && *type1 != *type2)
	return false;
    }

  return true;
}

/* See common/tdesc.h.  */

void
tdesc_create_reg (struct tdesc_feature *feature, const char *name,
		  int regnum, int save_restore, const char *group,
		  int bitsize, const char *type)
{
  tdesc_reg *reg = new tdesc_reg (feature, name, regnum, save_restore,
				  group, bitsize, type);

  feature->registers.emplace_back (reg);
}
