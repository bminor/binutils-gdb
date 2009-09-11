/* GNU/Linux/ARC specific low level interface, for the remote server for GDB.
   Copyright 1995, 1996, 1998, 1999, 2000, 2001, 2002, 2003, 2004
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

#include <linux/user.h>

#define OFFSET(member,structure) (int)(&( ((struct structure *)0)->member ))

#define arc_num_regs 39

/* arc_regmap[i] is the offset of Ri in Linux's user struct */
static int arc_regmap[] = {
    OFFSET(r0, user_regs_struct),
    OFFSET(r1, user_regs_struct),
    OFFSET(r2, user_regs_struct),
    OFFSET(r3, user_regs_struct),
    OFFSET(r4, user_regs_struct),
    OFFSET(r5, user_regs_struct),
    OFFSET(r6, user_regs_struct),
    OFFSET(r7, user_regs_struct),
    OFFSET(r8, user_regs_struct),
    OFFSET(r9, user_regs_struct),
    OFFSET(r10, user_regs_struct),
    OFFSET(r11, user_regs_struct),
    OFFSET(r12, user_regs_struct),
    OFFSET(r13, user_regs_struct),
    OFFSET(r14, user_regs_struct),
    OFFSET(r15, user_regs_struct),
    OFFSET(r16, user_regs_struct),
    OFFSET(r17, user_regs_struct),
    OFFSET(r18, user_regs_struct),
    OFFSET(r19, user_regs_struct),
    OFFSET(r20, user_regs_struct),
    OFFSET(r21, user_regs_struct),
    OFFSET(r22, user_regs_struct),
    OFFSET(r23, user_regs_struct),
    OFFSET(r24, user_regs_struct),
    OFFSET(r25, user_regs_struct),
    OFFSET(r26, user_regs_struct),
    OFFSET(bta, user_regs_struct),
    OFFSET(lp_start, user_regs_struct),
    OFFSET(lp_end, user_regs_struct),
    OFFSET(lp_count, user_regs_struct),
    OFFSET(status32, user_regs_struct),
    OFFSET(blink, user_regs_struct),
    OFFSET(fp, user_regs_struct),
    OFFSET(sp, user_regs_struct),
    OFFSET(efa, user_regs_struct),
    OFFSET(ret, user_regs_struct),
    OFFSET(orig_r8, user_regs_struct),
    OFFSET(stop_pc, user_regs_struct)
};

static int
arc_cannot_store_register (int regno)
{
  return (regno == 35 || regno >= arc_num_regs); /* FIXME, this is dirty */
}

static int
arc_cannot_fetch_register (int regno)
{
  return (regno >= arc_num_regs);
}

extern int debug_threads;

static CORE_ADDR
arc_get_pc ()
{
  unsigned long pc;
  collect_register_by_name ("stop_pc", &pc);
  if (debug_threads)
    fprintf (stderr, "stop pc is %08lx\n", pc);
  return pc;
}

static void
arc_set_pc (CORE_ADDR pc)
{
  unsigned long newpc = pc;
  supply_register_by_name ("ret", &newpc);
}


/* trap_s 1 */
/* FIXME: dependent on target endianness */
static const unsigned long arc_breakpoint = 0x783E;
#define arc_breakpoint_len 2

static int
arc_breakpoint_at (CORE_ADDR where)
{
  unsigned long insn;

  (*the_target->read_memory) (where, (unsigned char *) &insn, arc_breakpoint_len);
  if (insn == arc_breakpoint)
    return 1;

  /* If necessary, recognize more trap instructions here.  GDB only uses the
     one.  */
  return 0;
}


/* We only place breakpoints in empty marker functions, and thread locking
   is outside of the function.  So rather than importing software single-step,
   we can just run until exit.  */
static CORE_ADDR
arc_reinsert_addr ()
{
  unsigned long pc;
  collect_register_by_name ("blink", &pc);
  return pc;
}

struct linux_target_ops the_low_target = {
  arc_num_regs,
  arc_regmap,
  arc_cannot_fetch_register,
  arc_cannot_store_register,
  arc_get_pc,
  arc_set_pc,
  (const unsigned char *) &arc_breakpoint,
  arc_breakpoint_len,
  arc_reinsert_addr,
  0,
  arc_breakpoint_at,
};
