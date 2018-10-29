/* Copyright (C) 2018 Free Software Foundation, Inc.

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
#include "riscv.h"
#include <stdlib.h>

#include "../features/riscv/32bit-cpu.c"
#include "../features/riscv/64bit-cpu.c"
#include "../features/riscv/32bit-fpu.c"
#include "../features/riscv/64bit-fpu.c"

/* See arch/riscv.h.  */

target_desc *
riscv_create_target_description (struct riscv_gdbarch_features features)
{
  target_desc *tdesc = allocate_target_description ();

#ifndef IN_PROCESS_AGENT
  std::string arch_name = "riscv";

  if (features.xlen == 4)
    arch_name.append (":rv32i");
  else if (features.xlen == 8)
    arch_name.append (":rv64i");
  else if (features.xlen == 16)
    arch_name.append (":rv128i");

  if (features.flen == 4)
    arch_name.append ("f");
  else if (features.flen == 8)
    arch_name.append ("d");
  else if (features.flen == 16)
    arch_name.append ("q");

  set_tdesc_architecture (tdesc, arch_name.c_str ());
#endif

  long regnum = 0;

  /* For now we only support creating 32-bit or 64-bit x-registers.  */
  if (features.xlen == 4)
    regnum = create_feature_riscv_32bit_cpu (tdesc, regnum);
  else if (features.xlen == 8)
    regnum = create_feature_riscv_64bit_cpu (tdesc, regnum);

  /* For now we only support creating 32-bit or 64-bit f-registers.  */
  if (features.flen == 4)
    regnum = create_feature_riscv_32bit_fpu (tdesc, regnum);
  else if (features.flen == 8)
    regnum = create_feature_riscv_64bit_fpu (tdesc, regnum);

  return tdesc;
}
