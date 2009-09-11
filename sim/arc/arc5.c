/* arc simulator support code
   Copyright (C) 1996, 1997, 1998, 2003, 2007, 2008, 2009
   Free Software Foundation, Inc.

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

#define WANT_CPU a5f
#define WANT_CPU_A5F

#include "sim-main.h"
#include "cgen-mem.h"
#include "cgen-ops.h"

#include "arc-sim-registers.h"


#define STATUS_REGNUM         0
#define IDENTITY_REGNUM       4
#define BYTES_IN_REGISTER     4


/* N.B. the a5f_h_auxr_get/set functions use the GET_H_AUXR/SET_H_AUXR macros:
        these have a special case for register number 0 (STATUS) which result in
        an 'invalid insn' internal error being raised (see CR 95530). 
  
        Those macros are no doubt intended to be used in the execution of the
        object code running on the simulator (where access to the obsolete
        STATUS register, instead of the newer STATUS32 register, would indeed be
        an error), but they are also being used by this code that the debugger
        uses to interact with the simulator (via the sim_fetch_register /
        sim_store_register functions).
  
        So we wish to avoid calling a5f_h_auxr_get/set for the STATUS register.
        Fortunately, the register is read-only so we can simply ignore a write;
        for a read, we must "short-circuit" the access by going straight to the
        default case of the GET_H_AUXR macro (which is "CPU (h_auxr[index])").  */


/* The contents of BUF are in target byte order.  */

int
a5f_fetch_register (SIM_CPU *current_cpu, int rn, unsigned char *buf, int len)
{
  int               hw_regnum;
  ARC_RegisterClass reg_class;
  SI                contents;

// printf("*** a5f_fetch_register %d\n", rn);

  arc_hw_register_mapping(rn, &hw_regnum, &reg_class);

  switch (reg_class)
    {
    case ARC_UNKNOWN_REGISTER:
      return 0;

    case ARC_CORE_REGISTER:
      /* R61 and R62 are reserved - so just return 0.  */
      if (hw_regnum == 61 || hw_regnum == 62)
          return 0;

      /* N.B. the simulator does not model PCL (R63) if the instruction does
	 not refer to it. - so get the current
              instruction address from PC and compute PCL from it.  */
      contents = (hw_regnum == 63) ? a5f_h_pc_get (current_cpu) & -4
                                   : a5f_h_cr_get (current_cpu, hw_regnum);
      break;

    case ARC_AUX_REGISTER:
      if (hw_regnum == IDENTITY_REGNUM)
      {
        /* Construct a value from the target CPU architecture.  */
        switch (CPU_MACH(current_cpu)->num)
          {
          case MACH_A5    : contents = 0x10;  break;
          case MACH_ARC600: contents = 0x20;  break;
          case MACH_ARC700: contents = 0x30;  break;
          default         : contents = 0x00;  break;
          }
      }
      else
        /* If it is the STATUS register, make the access directly.  */
        contents = (hw_regnum == STATUS_REGNUM) ? CPU (h_auxr[STATUS_REGNUM])
                                                : a5f_h_auxr_get (current_cpu, hw_regnum);
      break;

    case ARC_PROGRAM_COUNTER:
      contents = a5f_h_pc_get (current_cpu);
      break;
    }

  SETTWI (buf, contents);
  return BYTES_IN_REGISTER;
}


/* The contents of BUF are in target byte order.  */

int
a5f_store_register (SIM_CPU *current_cpu, int rn, unsigned char *buf, int len)
{
  int               hw_regnum;
  ARC_RegisterClass reg_class;

// printf("*** a5f_store_register %d\n", rn);

  arc_hw_register_mapping(rn, &hw_regnum, &reg_class);

  switch (reg_class)
    {
    case ARC_UNKNOWN_REGISTER:
      break;

    case ARC_CORE_REGISTER:
      /* R61 and R62 are reserved, and R63 is PCL which is read-only - so just
         do nothing for them. */
      if (hw_regnum < 61)
      {
          a5f_h_cr_set (current_cpu, hw_regnum, GETTWI (buf));
          return BYTES_IN_REGISTER;
      }
      break;

    case ARC_AUX_REGISTER:
      /* The STATUS aux reg is read/only, so ignore the write attempt.  */
      if (hw_regnum != STATUS_REGNUM)
      {
          /* Do any required conversion on the given value before writing it to
             the register (this ensures that bits in reserved fields have the
             on-write values required by the architecture.  */
          arc_aux_register_conversion(rn, buf);

          a5f_h_auxr_set (current_cpu, hw_regnum, GETTWI (buf));
          return BYTES_IN_REGISTER;
      }
      break;

    case ARC_PROGRAM_COUNTER:
      a5f_h_pc_set (current_cpu, GETTWI (buf));
      return BYTES_IN_REGISTER;
    }

  return 0;
}


/* Initialize cycle counting for an insn.
   FIRST_P is non-zero if this is the first insn in a set of parallel
   insns.  */

void
a5f_model_insn_before (SIM_CPU *cpu, int first_p)
{
}

/* Record the cycles computed for an insn.
   LAST_P is non-zero if this is the last insn in a set of parallel insns,
   and we update the total cycle count.
   CYCLES is the cycle count of the insn.  */

void
a5f_model_insn_after (SIM_CPU *cpu, int last_p, int cycles)
{
}

int
a5f_model_A5_u_exec (SIM_CPU *cpu, const IDESC *idesc,
			     int unit_num, int referenced,
			     INT sr, INT sr2, INT dr)
{
#if 0
  check_load_stall (cpu, sr);
  check_load_stall (cpu, sr2);
#endif
  return idesc->timing->units[unit_num].done;
}
