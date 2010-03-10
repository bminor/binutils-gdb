/* Handle FR-V (FDPIC) shared libraries for GDB, the GNU Debugger.
   Copyright (C) 2004-2022 Free Software Foundation, Inc.

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

#include "solib-fdpic.h"
#include "frv-tdep.h"
#include "elf/frv.h"

#define arch_abi frv_abi
#define ARCH_ABI_FDPIC FRV_ABI_FDPIC
#define R_ARCH_FUNCDESC R_FRV_FUNCDESC
#define FDPIC_INTERP_REGNUM fdpic_loadmap_interp_regnum
#define FDPIC_EXEC_REGNUM fdpic_loadmap_exec_regnum

/* Provide a prototype to silence -Wmissing-prototypes and for init.c.  */
extern initialize_file_ftype
_initialize_fdpic_solib;

#include "solib-fdpic.c"
