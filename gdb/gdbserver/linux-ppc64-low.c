/* GNU/Linux/PowerPC specific low level interface, for the remote server for
   GDB.
   Copyright 1995, 1996, 1998, 1999, 2000, 2001, 2002
   Free Software Foundation, Inc.

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

#include "server.h"
#include "linux-low.h"

#include <asm/ptrace.h>

#define ppc64_num_regs 71

#define FPR(n) ((PT_FPR0 + (n)) * 8)

/* Currently, don't check/send MQ.  */
static int ppc64_regmap[] =
  {
    PT_R0  * 8,    PT_R1  * 8,    PT_R2  * 8,    PT_R3  * 8,
    PT_R4  * 8,    PT_R5  * 8,    PT_R6  * 8,    PT_R7  * 8,
    PT_R8  * 8,    PT_R9  * 8,    PT_R10 * 8,    PT_R11 * 8,
    PT_R12 * 8,    PT_R13 * 8,    PT_R14 * 8,    PT_R15 * 8,
    PT_R16 * 8,    PT_R17 * 8,    PT_R18 * 8,    PT_R19 * 8,
    PT_R20 * 8,    PT_R21 * 8,    PT_R22 * 8,    PT_R23 * 8,
    PT_R24 * 8,    PT_R25 * 8,    PT_R26 * 8,    PT_R27 * 8,
    PT_R28 * 8,    PT_R29 * 8,    PT_R30 * 8,    PT_R31 * 8,
    FPR (0),       FPR (1),       FPR (2),       FPR (3),
    FPR (4),       FPR (5),       FPR (6),       FPR (7),
    FPR (8),       FPR (9),       FPR (10),      FPR (11),
    FPR (12),      FPR (13),      FPR (14),      FPR (15),
    FPR (16),      FPR (17),      FPR (18),      FPR (19),
    FPR (20),      FPR (21),      FPR (22),      FPR (23),
    FPR (24),      FPR (25),      FPR (26),      FPR (27),
    FPR (28),      FPR (29),      FPR (30),      FPR (31),
    PT_NIP * 8,    PT_MSR * 8,    PT_CCR * 8,    PT_LNK * 8,
    PT_CTR * 8,    PT_XER * 8,    PT_FPSCR * 8,
  };

static int
ppc64_cannot_store_register (int regno)
{
  /* Some kernels do not allow us to store fpscr.  */
  if (regno == find_regno ("fpscr"))
    return 2;

  return 0;
}

static int
ppc64_cannot_fetch_register (int regno)
{
  return 0;
}

static CORE_ADDR
ppc64_get_pc (void)
{
  unsigned long pc;

  collect_register_by_name ("pc", &pc);
  return (CORE_ADDR) pc;
}

static void
ppc64_set_pc (CORE_ADDR pc)
{
  unsigned long newpc = pc;

  supply_register_by_name ("pc", &newpc);
}

/* Correct in either endianness.  Note that this file is
   for PowerPC only, not PowerPC64.
   This instruction is "twge r2, r2", which GDB uses as a software
   breakpoint.  */
static const unsigned int ppc64_breakpoint = 0x7d821008;
#define ppc64_breakpoint_len 4

static int
ppc64_breakpoint_at (CORE_ADDR where)
{
  unsigned int insn;

  (*the_target->read_memory) (where, (char *) &insn, 4);
  if (insn == ppc64_breakpoint)
    return 1;
  /* If necessary, recognize more trap instructions here.  GDB only uses the
     one.  */
  return 0;
}

struct linux_target_ops the_low_target = {
  ppc64_num_regs,
  ppc64_regmap,
  ppc64_cannot_fetch_register,
  ppc64_cannot_store_register,
  ppc64_get_pc,
  ppc64_set_pc,
  (const char *) &ppc64_breakpoint,
  ppc64_breakpoint_len,
  NULL,
  0,
  ppc64_breakpoint_at,
};
