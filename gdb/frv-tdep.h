/* Architecture-dependent code for the Fujitsu FR-V, for GDB, the GNU Debugger.
   Copyright 2004 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* Enumerate the possible ABIs for FR-V.  */
enum frv_abi
  {
    FRV_ABI_EABI,
    FRV_ABI_FDPIC
  };

/* Return the FR-V ABI associated with GDBARCH.  */
enum frv_abi frv_abi (struct gdbarch *gdbarch);

/* Fetch the interpreter and executable loadmap addresses (for shared
   library support) for the FDPIC ABI.  Return 0 if successful, -1 if
   not.  (E.g, -1 will be returned if the ABI isn't the FDPIC ABI.)  */
int frv_fdpic_loadmap_addresses (struct gdbarch *gdbarch,
                                 CORE_ADDR *interp_addr, CORE_ADDR *exec_addr);

/* Given a function entry point, find and return the GOT address for the
   containing load module.  */
CORE_ADDR frv_fdpic_find_global_pointer (CORE_ADDR addr);

/* Given a function entry point, find and return the canonical descriptor
   for that function, if one exists.  If no canonical descriptor could
   be found, return 0.  */
CORE_ADDR frv_fdpic_find_canonical_descriptor (CORE_ADDR entry_point);
