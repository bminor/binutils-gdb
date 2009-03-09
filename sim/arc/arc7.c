/* arc simulator support code
   Copyright (C) 1996, 1997, 1998, 2003, 2007 Free Software Foundation, Inc.

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

#define WANT_CPU arc700f
#define WANT_CPU_ARC700F

#include "sim-main.h"
#include "cgen-mem.h"
#include "cgen-ops.h"

/* The contents of BUF are in target byte order.  */

int
arc700f_fetch_register (SIM_CPU *current_cpu, int rn, unsigned char *buf,
			int len)
{
  return a5f_fetch_register (current_cpu, rn, buf, len);
}

/* The contents of BUF are in target byte order.  */

int
arc700f_store_register (SIM_CPU *current_cpu, int rn, unsigned char *buf,
			int len)
{
  return a5f_store_register (current_cpu, rn, buf, len);
}

/* Initialize cycle counting for an insn.
   FIRST_P is non-zero if this is the first insn in a set of parallel
   insns.  */

void
arc700f_model_insn_before (SIM_CPU *cpu, int first_p)
{
}

/* Record the cycles computed for an insn.
   LAST_P is non-zero if this is the last insn in a set of parallel insns,
   and we update the total cycle count.
   CYCLES is the cycle count of the insn.  */

void
arc700f_model_insn_after (SIM_CPU *cpu, int last_p, int cycles)
{
}

int
arc700f_model_ARC700_u_exec (SIM_CPU *cpu, const IDESC *idesc,
			     int unit_num, int referenced,
			     INT sr, INT sr2, INT dr)
{
#if 0
  check_load_stall (cpu, sr);
  check_load_stall (cpu, sr2);
#endif
  return idesc->timing->units[unit_num].done;
}
