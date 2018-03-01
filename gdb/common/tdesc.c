/* Target description support for GDB.

   Copyright (C) 2017 Free Software Foundation, Inc.

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

#ifdef GDBSERVER
#include "server.h"
#else
#include "defs.h"
#endif

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
