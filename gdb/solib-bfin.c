/* Handle Blackfin (FDPIC) shared libraries for GDB, the GNU Debugger.
   Copyright (C) 2004, 2006, 2007, 2008, 2009, 2010
   Free Software Foundation, Inc.

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
#include "bfin-tdep.h"
#include "elf/bfin.h"

#define arch_abi bfin_abi
#define ARCH_ABI_FDPIC BFIN_ABI_FDPIC
#define R_ARCH_FUNCDESC R_BFIN_FUNCDESC
#define FDPIC_INTERP_REGNUM BFIN_FDPIC_INTERP_REGNUM
#define FDPIC_EXEC_REGNUM BFIN_FDPIC_EXEC_REGNUM
#define LINKER_SYM(sym) "_"sym

/* Provide a prototype to silence -Wmissing-prototypes and for init.c.  */
extern initialize_file_ftype
_initialize_fdpic_solib;

#include "solib-fdpic.c"
