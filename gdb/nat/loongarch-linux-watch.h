/* Copyright (C) 2021 Free Software Foundation, Inc.
   Contributed by Loongson Ltd.

   This file is part of GDB.

   Based on MIPS target.

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

#ifndef LOONGARCH_LINUX_WATCH_H
#define LOONGARCH_LINUX_WATCH_H 1

#include <asm/ptrace.h>
#include "gdbsupport/break-common.h"

#define MAX_DEBUG_REGISTER 16

/* If macro PTRACE_GET_WATCH_REGS is not defined, kernel header doesn't
   have hardware watchpoint-related structures.  Define them below.  */

#ifndef PTRACE_GET_WATCH_REGS
#define PTRACE_GET_WATCH_REGS 0xd0
#define PTRACE_SET_WATCH_REGS 0xd1

enum pt_watch_style
{
  pt_watch_style_la32,
  pt_watch_style_la64
};

/* A value of zero in a watchlo indicates that it is available.  */

struct la32_watch_regs
{
  uint32_t addr;
  /* Lower 16 bits of watchhi.  */
  uint32_t mask;
  /* Valid mask and I R W bits.
   * bit 0 -- 1 if W bit is usable.
   * bit 1 -- 1 if R bit is usable.
   * bit 2 -- 1 if I bit is usable.
   * bits 3 - 11 -- Valid watchhi mask bits.
   */
  uint8_t irw;
  uint8_t irwstat;
  uint8_t irwmask;
  /* There is confusion across gcc versions about structure alignment,
     so we force 8 byte alignment for these structures so they match
     the kernel even if it was build with a different gcc version.  */
} __attribute__ ((aligned (8)));

struct la64_watch_regs
{
  uint64_t addr;
  uint64_t mask;
  uint8_t irw;
  uint8_t irwstat;
  uint8_t irwmask;
} __attribute__ ((aligned (8)));

struct pt_watch_regs
{
  uint16_t max_valid;
  uint16_t num_valid;
  enum pt_watch_style style;
  union
  {
    struct la32_watch_regs la32[MAX_DEBUG_REGISTER];
    struct la64_watch_regs la64[MAX_DEBUG_REGISTER];
  };
};

#endif /* !PTRACE_GET_WATCH_REGS  */

#define W_BIT 0
#define R_BIT 1
#define I_BIT 2

#define W_MASK (1 << W_BIT)
#define R_MASK (1 << R_BIT)
#define I_MASK (1 << I_BIT)

#define IRW_MASK (I_MASK | R_MASK | W_MASK)

/* We keep list of all watchpoints we should install and calculate the
   watch register values each time the list changes.  This allows for
   easy sharing of watch registers for more than one watchpoint.  */

struct loongarch_watchpoint
{
  CORE_ADDR addr;
  int len;
  enum target_hw_bp_type type;
  struct loongarch_watchpoint *next;
};

uint32_t loongarch_linux_watch_get_num_valid (struct pt_watch_regs *regs);
uint8_t loongarch_linux_watch_get_irwmask (struct pt_watch_regs *regs, int n);
uint8_t loongarch_linux_watch_get_irwstat (struct pt_watch_regs *regs, int n);
CORE_ADDR loongarch_linux_watch_get_addr (struct pt_watch_regs *regs, int n);
void loongarch_linux_watch_set_addr (struct pt_watch_regs *regs, int n,
				     CORE_ADDR value);
CORE_ADDR loongarch_linux_watch_get_mask (struct pt_watch_regs *regs, int n);
void loongarch_linux_watch_set_mask (struct pt_watch_regs *regs, int n,
				     CORE_ADDR value);
uint8_t loongarch_linux_watch_get_irw (struct pt_watch_regs *regs, int n);
void loongarch_linux_watch_set_irw (struct pt_watch_regs *regs, int n,
				    uint8_t value);
int loongarch_linux_watch_try_one_watch (struct pt_watch_regs *regs,
					 CORE_ADDR addr, int len,
					 uint32_t irw);
void loongarch_linux_watch_populate_regs (
  struct loongarch_watchpoint *current_watches, struct pt_watch_regs *regs);
uint32_t loongarch_linux_watch_type_to_irw (enum target_hw_bp_type type);

int loongarch_linux_read_watch_registers (long lwpid,
					  struct pt_watch_regs *watch_readback,
					  int *watch_readback_valid,
					  int force);

#endif /* #define LOONGARCH_LINUX_WATCH_H  */
