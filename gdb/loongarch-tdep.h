/* Target-dependent header for the LoongArch architecture, for GDB.

   Copyright (C) 2022 Free Software Foundation, Inc.

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

#ifndef LOONGARCH_TDEP_H
#define LOONGARCH_TDEP_H

#include "gdbarch.h"
#include "arch/loongarch.h"
#include "regset.h"

#include "elf/loongarch.h"
#include "opcode/loongarch.h"

/* Register set definitions.  */
extern const struct regset loongarch_gregset;

/* Target-dependent structure in gdbarch.  */
struct loongarch_gdbarch_tdep : gdbarch_tdep
{
  /* Features about the abi that impact how the gdbarch is configured.  */
  struct loongarch_gdbarch_features abi_features;

  struct
  {
    int r;	/* General register. */
    int ra;	/* Return Address.  */
    int sp;	/* Stack Pointer.  */
    int pc;	/* Program Counter.  */
    int badv;	/* Bad vaddr for addressing exception.  */
  } regs;	/* LoongArch registers  */
};

#endif /* LOONGARCH_TDEP_H  */
