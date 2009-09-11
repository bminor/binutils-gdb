/* collection of junk waiting time to sort out
   Copyright (C) 1996, 1997, 1998, 2003 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

   This file is part of GDB, the GNU debugger.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef ARC_SIM_H
#define ARC_SIM_H


extern int arc_decode_gdb_ctrl_regnum (int);


/* Misc. profile data.  */

typedef struct {
  /* nop insn slot filler count */
  unsigned int fillnop_count;
  /* number of parallel insns */
  unsigned int parallel_count;

  /* FIXME: generalize this to handle all insn lengths, move to common.  */
  /* number of short insns, not including parallel ones */
  unsigned int short_count;
  /* number of long insns */
  unsigned int long_count;

  /* Working area for computing cycle counts.  */
  unsigned long insn_cycles; /* FIXME: delete */
  unsigned long cti_stall;
  unsigned long load_stall;
  unsigned long biggest_cycles;

  /* Bitmask of registers loaded by previous insn.  */
  unsigned int load_regs;
  /* Bitmask of registers loaded by current insn.  */
  unsigned int load_regs_pending;
} ARC_MISC_PROFILE;

/* Initialize the working area.  */
void arc_init_insn_cycles (SIM_CPU *, int);
/* Update the totals for the insn.  */
void arc_record_insn_cycles (SIM_CPU *, int);

/* This is invoked by the nop pattern in the .cpu file.  */
#define PROFILE_COUNT_FILLNOPS(cpu, addr) \
do { \
  if (PROFILE_INSN_P (cpu) \
      && (addr & 3) != 0) \
    ++ CPU_ARC_MISC_PROFILE (cpu)->fillnop_count; \
} while (0)

/* This is invoked by the execute section of mloop{,x}.in.  */
#define PROFILE_COUNT_PARINSNS(cpu) \
do { \
  if (PROFILE_INSN_P (cpu)) \
    ++ CPU_ARC_MISC_PROFILE (cpu)->parallel_count; \
} while (0)

/* This is invoked by the execute section of mloop{,x}.in.  */
#define PROFILE_COUNT_SHORTINSNS(cpu) \
do { \
  if (PROFILE_INSN_P (cpu)) \
    ++ CPU_ARC_MISC_PROFILE (cpu)->short_count; \
} while (0)

/* This is invoked by the execute section of mloop{,x}.in.  */
#define PROFILE_COUNT_LONGINSNS(cpu) \
do { \
  if (PROFILE_INSN_P (cpu)) \
    ++ CPU_ARC_MISC_PROFILE (cpu)->long_count; \
} while (0)

#define GETTWI GETTSI
#define SETTWI SETTSI

/* Additional execution support.  */


/* Hardware/device support.
   ??? Will eventually want to move device stuff to config files.  */

/* Exception, Interrupt, and Trap addresses */
#define EIT_SYSBREAK_ADDR	0x10
#define EIT_RSVD_INSN_ADDR	0x20
#define EIT_ADDR_EXCP_ADDR	0x30
#define EIT_TRAP_BASE_ADDR	0x40
#define EIT_EXTERN_ADDR		0x80
#define EIT_RESET_ADDR		0x7ffffff0
#define EIT_WAKEUP_ADDR		0x7ffffff0

/* Special purpose traps.  */
#define TRAP_SYSCALL	1
#define TRAP_BREAKPOINT	0

/* sim_core_attach device argument.  */
extern device arc_devices;

/* FIXME: Temporary, until device support ready.  */
struct _device { int foo; };

/* Handle the trap insn.  */
USI arc_trap (SIM_CPU *, PCADDR, int, int);

#endif /* ARC_SIM_H */
