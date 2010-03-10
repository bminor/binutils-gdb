/* Handle FDPIC shared libraries for GDB, the GNU Debugger.
   Copyright (C) 2004, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.

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

#ifndef SOLIB_FDPIC_H
#define SOLIB_FDPIC_H

#include "defs.h"
#include "inferior.h"
#include "gdbcore.h"
#include "solib.h"
#include "solist.h"
#include "objfiles.h"
#include "symtab.h"
#include "language.h"
#include "command.h"
#include "gdbcmd.h"
#include "regcache.h"

/* Targets implementing FDPIC shared libraries should define:

	arch_abi (required)

	ARCH_ABI_FDPIC (required)

	R_ARCH_FUNCDESC (required)

	FDPIC_EXEC_REGNUM (required)

	FDPIC_INTERP_REGNUM (required)

	LINKER_SYM (optional)

 */

/* Given a function entry point, find and return the GOT address for the
   containing load module.  */
CORE_ADDR fdpic_find_global_pointer (CORE_ADDR addr);

/* Given a function entry point, find and return the canonical descriptor
   for that function, if one exists.  If no canonical descriptor could
   be found, return 0.  */
CORE_ADDR fdpic_find_canonical_descriptor (CORE_ADDR entry_point);

/* Given an objfile, return the address of its link map.  This value is
   needed for TLS support.  */
CORE_ADDR fdpic_fetch_objfile_link_map (struct objfile *objfile);

/* Fetch the interpreter and executable loadmap addresses (for shared
   library support) for the FDPIC ABI.  Return 0 if successful, -1 if
   not.  (E.g, -1 will be returned if the ABI isn't the FDPIC ABI.)  */
int fdpic_loadmap_addresses (struct gdbarch *gdbarch, CORE_ADDR *interp_addr,
			     CORE_ADDR *exec_addr);

struct target_so_ops;
extern const struct target_so_ops fdpic_so_ops;

#endif
