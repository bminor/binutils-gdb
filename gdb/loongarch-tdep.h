/* Target-dependent code for GNU/Linux LoongArch.

   Copyright (C) 2021 Free Software Foundation, Inc.
   Contributed by Loongson Ltd.

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

#include "arch/loongarch.h"

struct gdbarch_tdep
{
  int ef_abi; /* EF_LARCH_ABI  */

  struct
  {
    int r;
    int ra;
    int sp;
    int pc;
    int badvaddr;

    int f;
    int fcc;
    int fcsr;
    int vr;
    int xr;

    int scr;
    int EFLAG;
    int x86_top;

  } regs;

  /* Return the expected next PC if FRAME is stopped at a syscall
     instruction.  */
  CORE_ADDR (*syscall_next_pc) (struct frame_info *frame);
};

#endif /* LOONGARCH_TDEP_H  */
