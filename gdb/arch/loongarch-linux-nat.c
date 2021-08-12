/* Copyright (C) 2021 Free Software Foundation, Inc.

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

/* For external ptrace. */
#ifdef GDBSERVER
#include "server.h"
#include "nat/gdb_ptrace.h"
#else
#include "defs.h"
#include "nat/gdb_ptrace.h"
#endif

#include "arch/loongarch.h"
#include "arch/loongarch-linux-nat.h"
#include "loongarch-linux-tdep.h"
#include "elf/common.h"
#include <sys/uio.h>

static uint32_t
loongarch_cpucfg_may_ptrace (uint64_t rj, int tid)
{
  char t_buf[rj * 4 + 4];
  struct iovec iovec = { .iov_base = &t_buf, .iov_len = sizeof (t_buf) };
  if (ptrace (PTRACE_GETREGSET, tid, NT_LARCH_CPUCFG, &iovec) < 0)
    ((uint32_t *) t_buf)[rj] = loongarch_cpucfg (rj);
  return ((uint32_t *) t_buf)[rj];
}

struct target_desc *
loongarch_linux_read_description_runtime (int tid)
{
  int rlen, fpu32, fpu64, lbt, lsx, lasx;

  uint32_t cpucfg1 = loongarch_cpucfg_may_ptrace (1, tid);
  rlen = cpucfg1 & 0x2 /* LA64 */ ? 64 : 32;

  uint32_t cpucfg2 = loongarch_cpucfg_may_ptrace (2, tid);
  fpu32 = 0, fpu64 = 0;
  if (cpucfg2 & 0x4 /* FP_DP */)
    fpu64 = 1;
  else if (cpucfg2 & 0x2 /* FP_SP */)
    fpu32 = 1;
  if (fpu32 || fpu64)
    {
      loongarch_elf_fpregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      if (ptrace (PTRACE_GETREGSET, tid, NT_FPREGSET, &iovec) < 0)
	fpu32 = 0, fpu64 = 0;
    }

  lbt = 0;
  if (cpucfg2 & 0x1c0000 /* LBT_X86 || LBT_ARM || LBT_MIPS */)
    {
      loongarch_elf_lbtregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      if (ptrace (PTRACE_GETREGSET, tid, NT_LARCH_LBT, &iovec) == 0)
	lbt = 1;
    }

  lsx = 0;
  if (cpucfg2 & 0x40 /* LSX */)
    {
      loongarch_elf_lsxregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      if (ptrace (PTRACE_GETREGSET, tid, NT_LARCH_LSX, &iovec) == 0)
	lsx = 1;
    }

  lasx = 0;
  if (cpucfg2 & 0x80 /* LASX */)
    {
      loongarch_elf_lasxregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      if (ptrace (PTRACE_GETREGSET, tid, NT_LARCH_LASX, &iovec) == 0)
	lasx = 1;
    }

  return loongarch_create_target_description (rlen, fpu32, fpu64, lbt, lsx,
					      lasx);
}
