// OBSOLETE /* Definitions to make GDB run on a mips box under Mach 3.0
// OBSOLETE    Copyright 1992, 1993, 1998, 2000, 2001 Free Software Foundation, Inc.
// OBSOLETE 
// OBSOLETE    This file is part of GDB.
// OBSOLETE 
// OBSOLETE    This program is free software; you can redistribute it and/or modify
// OBSOLETE    it under the terms of the GNU General Public License as published by
// OBSOLETE    the Free Software Foundation; either version 2 of the License, or
// OBSOLETE    (at your option) any later version.
// OBSOLETE 
// OBSOLETE    This program is distributed in the hope that it will be useful,
// OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of
// OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// OBSOLETE    GNU General Public License for more details.
// OBSOLETE 
// OBSOLETE    You should have received a copy of the GNU General Public License
// OBSOLETE    along with this program; if not, write to the Free Software
// OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330,
// OBSOLETE    Boston, MA 02111-1307, USA.  */
// OBSOLETE 
// OBSOLETE /* Mach specific routines for little endian mips (e.g. pmax)
// OBSOLETE  * running Mach 3.0
// OBSOLETE  *
// OBSOLETE  * Author: Jukka Virtanen <jtv@hut.fi>
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE #include "defs.h"
// OBSOLETE #include "inferior.h"
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE #include <stdio.h>
// OBSOLETE 
// OBSOLETE #include <mach.h>
// OBSOLETE #include <mach/message.h>
// OBSOLETE #include <mach/exception.h>
// OBSOLETE #include <mach_error.h>
// OBSOLETE 
// OBSOLETE /* Find offsets to thread states at compile time.
// OBSOLETE  * If your compiler does not grok this, check the hand coded
// OBSOLETE  * offsets and use them.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE #if 1
// OBSOLETE 
// OBSOLETE #define  REG_OFFSET(reg) (int)(&((struct mips_thread_state *)0)->reg)
// OBSOLETE #define CREG_OFFSET(reg) (int)(&((struct mips_float_state *)0)->reg)
// OBSOLETE #define EREG_OFFSET(reg) (int)(&((struct mips_exc_state *)0)->reg)
// OBSOLETE 
// OBSOLETE /* at reg_offset[i] is the offset to the mips_thread_state
// OBSOLETE  * location where the gdb registers[i] is stored.
// OBSOLETE  *
// OBSOLETE  * -1 means mach does not save it anywhere.
// OBSOLETE  */
// OBSOLETE static int reg_offset[] =
// OBSOLETE {
// OBSOLETE   /*  zero              at                v0                v1       */
// OBSOLETE   -1, REG_OFFSET (r1), REG_OFFSET (r2), REG_OFFSET (r3),
// OBSOLETE 
// OBSOLETE   /*  a0                a1                a2                a3       */
// OBSOLETE   REG_OFFSET (r4), REG_OFFSET (r5), REG_OFFSET (r6), REG_OFFSET (r7),
// OBSOLETE 
// OBSOLETE   /*  t0                t1                t2                t3       */
// OBSOLETE   REG_OFFSET (r8), REG_OFFSET (r9), REG_OFFSET (r10), REG_OFFSET (r11),
// OBSOLETE 
// OBSOLETE   /*  t4                t5                t6                t7       */
// OBSOLETE   REG_OFFSET (r12), REG_OFFSET (r13), REG_OFFSET (r14), REG_OFFSET (r15),
// OBSOLETE 
// OBSOLETE   /*  s0                s1                s2                s3       */
// OBSOLETE   REG_OFFSET (r16), REG_OFFSET (r17), REG_OFFSET (r18), REG_OFFSET (r19),
// OBSOLETE 
// OBSOLETE   /*  s4                s5                s6                s7       */
// OBSOLETE   REG_OFFSET (r20), REG_OFFSET (r21), REG_OFFSET (r22), REG_OFFSET (r23),
// OBSOLETE 
// OBSOLETE   /*  t8                t9                k0                k1       */
// OBSOLETE   REG_OFFSET (r24), REG_OFFSET (r25), REG_OFFSET (r26), REG_OFFSET (r27),
// OBSOLETE 
// OBSOLETE   /*  gp                sp            s8(30) == fp(72)      ra       */
// OBSOLETE   REG_OFFSET (r28), REG_OFFSET (r29), REG_OFFSET (r30), REG_OFFSET (r31),
// OBSOLETE 
// OBSOLETE   /*  sr(32) PS_REGNUM   */
// OBSOLETE   EREG_OFFSET (coproc_state),
// OBSOLETE 
// OBSOLETE   /*  lo(33)            hi(34)    */
// OBSOLETE   REG_OFFSET (mdlo), REG_OFFSET (mdhi),
// OBSOLETE 
// OBSOLETE   /*  bad(35)                 cause(36)          pc(37)  */
// OBSOLETE   EREG_OFFSET (address), EREG_OFFSET (cause), REG_OFFSET (pc),
// OBSOLETE 
// OBSOLETE   /*  f0(38)             f1(39)             f2(40)             f3(41)   */
// OBSOLETE   CREG_OFFSET (r0), CREG_OFFSET (r1), CREG_OFFSET (r2), CREG_OFFSET (r3),
// OBSOLETE   CREG_OFFSET (r4), CREG_OFFSET (r5), CREG_OFFSET (r6), CREG_OFFSET (r7),
// OBSOLETE   CREG_OFFSET (r8), CREG_OFFSET (r9), CREG_OFFSET (r10), CREG_OFFSET (r11),
// OBSOLETE   CREG_OFFSET (r12), CREG_OFFSET (r13), CREG_OFFSET (r14), CREG_OFFSET (r15),
// OBSOLETE   CREG_OFFSET (r16), CREG_OFFSET (r17), CREG_OFFSET (r18), CREG_OFFSET (r19),
// OBSOLETE   CREG_OFFSET (r20), CREG_OFFSET (r21), CREG_OFFSET (r22), CREG_OFFSET (r23),
// OBSOLETE   CREG_OFFSET (r24), CREG_OFFSET (r25), CREG_OFFSET (r26), CREG_OFFSET (r27),
// OBSOLETE   CREG_OFFSET (r28), CREG_OFFSET (r29), CREG_OFFSET (r30), CREG_OFFSET (r31),
// OBSOLETE 
// OBSOLETE   /*  fsr(70)           fir(71)         fp(72) == s8(30) */
// OBSOLETE   CREG_OFFSET (csr), CREG_OFFSET (esr), REG_OFFSET (r30)
// OBSOLETE };
// OBSOLETE #else
// OBSOLETE /* If the compiler does not grok the above defines */
// OBSOLETE static int reg_offset[] =
// OBSOLETE {
// OBSOLETE /* mach_thread_state offsets: */
// OBSOLETE   -1, 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56,
// OBSOLETE   60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120,
// OBSOLETE /*sr, lo, hi,addr,cause,pc   */
// OBSOLETE   8, 124, 128, 4, 0, 132,
// OBSOLETE /* mach_float_state offsets: */
// OBSOLETE   0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60,
// OBSOLETE   64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124,
// OBSOLETE /*fsr,fir */
// OBSOLETE   128, 132,
// OBSOLETE /* FP_REGNUM pseudo maps to s8==r30 in mach_thread_state */
// OBSOLETE   116
// OBSOLETE };
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE /* Fetch COUNT contiguous registers from thread STATE starting from REGNUM
// OBSOLETE  * Caller knows that the regs handled in one transaction are of same size.
// OBSOLETE  */
// OBSOLETE #define FETCH_REGS(state, regnum, count) \
// OBSOLETE   memcpy (&deprecated_registers[REGISTER_BYTE (regnum)], \
// OBSOLETE 	  (char *)state+reg_offset[ regnum ], \
// OBSOLETE 	  count*REGISTER_SIZE)
// OBSOLETE 
// OBSOLETE /* Store COUNT contiguous registers to thread STATE starting from REGNUM */
// OBSOLETE #define STORE_REGS(state, regnum, count) \
// OBSOLETE   memcpy ((char *)state+reg_offset[ regnum ], \
// OBSOLETE 	  &deprecated_registers[REGISTER_BYTE (regnum)], \
// OBSOLETE 	  count*REGISTER_SIZE)
// OBSOLETE 
// OBSOLETE #define REGS_ALL    -1
// OBSOLETE #define REGS_NORMAL  1
// OBSOLETE #define REGS_EXC     2
// OBSOLETE #define REGS_COP1    4
// OBSOLETE 
// OBSOLETE /* Hardware regs that matches FP_REGNUM */
// OBSOLETE #define MACH_FP_REGNUM 30
// OBSOLETE 
// OBSOLETE /* Fech thread's registers. if regno == -1, fetch all regs */
// OBSOLETE void
// OBSOLETE fetch_inferior_registers (int regno)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   thread_state_data_t state;
// OBSOLETE   struct mips_exc_state exc_state;
// OBSOLETE 
// OBSOLETE   int stateCnt = MIPS_THREAD_STATE_COUNT;
// OBSOLETE 
// OBSOLETE   int which_regs = 0;		/* A bit mask */
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (current_thread))
// OBSOLETE     error ("fetch inferior registers: Invalid thread");
// OBSOLETE 
// OBSOLETE   if (regno < -1 || regno >= NUM_REGS)
// OBSOLETE     error ("invalid register %d supplied to fetch_inferior_registers", regno);
// OBSOLETE 
// OBSOLETE   if (regno == -1)
// OBSOLETE     which_regs = REGS_ALL;
// OBSOLETE   else if (regno == ZERO_REGNUM)
// OBSOLETE     {
// OBSOLETE       int zero = 0;
// OBSOLETE       supply_register (ZERO_REGNUM, &zero);
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE   else if ((ZERO_REGNUM < regno && regno < PS_REGNUM)
// OBSOLETE 	   || regno == FP_REGNUM
// OBSOLETE 	   || regno == LO_REGNUM
// OBSOLETE 	   || regno == HI_REGNUM
// OBSOLETE 	   || regno == PC_REGNUM)
// OBSOLETE     which_regs = REGS_NORMAL;
// OBSOLETE   else if (FP0_REGNUM <= regno && regno <= FCRIR_REGNUM)
// OBSOLETE     which_regs = REGS_COP1 | REGS_EXC;
// OBSOLETE   else
// OBSOLETE     which_regs = REGS_EXC;
// OBSOLETE 
// OBSOLETE   /* fetch regs saved to mips_thread_state */
// OBSOLETE   if (which_regs & REGS_NORMAL)
// OBSOLETE     {
// OBSOLETE       ret = thread_get_state (current_thread,
// OBSOLETE 			      MIPS_THREAD_STATE,
// OBSOLETE 			      state,
// OBSOLETE 			      &stateCnt);
// OBSOLETE       CHK ("fetch inferior registers: thread_get_state", ret);
// OBSOLETE 
// OBSOLETE       if (which_regs == REGS_NORMAL)
// OBSOLETE 	{
// OBSOLETE 	  /* Fetch also FP_REGNUM if fetching MACH_FP_REGNUM and vice versa */
// OBSOLETE 	  if (regno == MACH_FP_REGNUM || regno == FP_REGNUM)
// OBSOLETE 	    {
// OBSOLETE 	      supply_register (FP_REGNUM,
// OBSOLETE 			       (char *) state + reg_offset[MACH_FP_REGNUM]);
// OBSOLETE 	      supply_register (MACH_FP_REGNUM,
// OBSOLETE 			       (char *) state + reg_offset[MACH_FP_REGNUM]);
// OBSOLETE 	    }
// OBSOLETE 	  else
// OBSOLETE 	    supply_register (regno,
// OBSOLETE 			     (char *) state + reg_offset[regno]);
// OBSOLETE 	  return;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* ZERO_REGNUM is always zero */
// OBSOLETE       *(int *) deprecated_registers = 0;
// OBSOLETE 
// OBSOLETE       /* Copy thread saved regs 1..31 to gdb's reg value array
// OBSOLETE        * Luckily, they are contiquous
// OBSOLETE        */
// OBSOLETE       FETCH_REGS (state, 1, 31);
// OBSOLETE 
// OBSOLETE       /* Copy mdlo and mdhi */
// OBSOLETE       FETCH_REGS (state, LO_REGNUM, 2);
// OBSOLETE 
// OBSOLETE       /* Copy PC */
// OBSOLETE       FETCH_REGS (state, PC_REGNUM, 1);
// OBSOLETE 
// OBSOLETE       /* Mach 3.0 saves FP to MACH_FP_REGNUM.
// OBSOLETE        * For some reason gdb wants to assign a pseudo register for it.
// OBSOLETE        */
// OBSOLETE       FETCH_REGS (state, FP_REGNUM, 1);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Read exc state. Also read if need to fetch floats */
// OBSOLETE   if (which_regs & REGS_EXC)
// OBSOLETE     {
// OBSOLETE       stateCnt = MIPS_EXC_STATE_COUNT;
// OBSOLETE       ret = thread_get_state (current_thread,
// OBSOLETE 			      MIPS_EXC_STATE,
// OBSOLETE 			      (thread_state_t) & exc_state,
// OBSOLETE 			      &stateCnt);
// OBSOLETE       CHK ("fetch inferior regs (exc): thread_get_state", ret);
// OBSOLETE 
// OBSOLETE       /* We need to fetch exc_state to see if the floating
// OBSOLETE        * state is valid for the thread.
// OBSOLETE        */
// OBSOLETE 
// OBSOLETE       /* cproc_state: Which coprocessors the thread uses */
// OBSOLETE       supply_register (PS_REGNUM,
// OBSOLETE 		       (char *) &exc_state + reg_offset[PS_REGNUM]);
// OBSOLETE 
// OBSOLETE       if (which_regs == REGS_EXC || which_regs == REGS_ALL)
// OBSOLETE 	{
// OBSOLETE 	  supply_register (BADVADDR_REGNUM,
// OBSOLETE 			 (char *) &exc_state + reg_offset[BADVADDR_REGNUM]);
// OBSOLETE 
// OBSOLETE 	  supply_register (CAUSE_REGNUM,
// OBSOLETE 			   (char *) &exc_state + reg_offset[CAUSE_REGNUM]);
// OBSOLETE 	  if (which_regs == REGS_EXC)
// OBSOLETE 	    return;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE   if (which_regs & REGS_COP1)
// OBSOLETE     {
// OBSOLETE       /* If the thread does not have saved COPROC1, set regs to zero */
// OBSOLETE 
// OBSOLETE       if (!(exc_state.coproc_state & MIPS_STATUS_USE_COP1))
// OBSOLETE 	bzero (&deprecated_registers[REGISTER_BYTE (FP0_REGNUM)],
// OBSOLETE 	       sizeof (struct mips_float_state));
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  stateCnt = MIPS_FLOAT_STATE_COUNT;
// OBSOLETE 	  ret = thread_get_state (current_thread,
// OBSOLETE 				  MIPS_FLOAT_STATE,
// OBSOLETE 				  state,
// OBSOLETE 				  &stateCnt);
// OBSOLETE 	  CHK ("fetch inferior regs (floats): thread_get_state", ret);
// OBSOLETE 
// OBSOLETE 	  if (regno != -1)
// OBSOLETE 	    {
// OBSOLETE 	      supply_register (regno,
// OBSOLETE 			       (char *) state + reg_offset[regno]);
// OBSOLETE 	      return;
// OBSOLETE 	    }
// OBSOLETE 
// OBSOLETE 	  FETCH_REGS (state, FP0_REGNUM, 34);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* All registers are valid, if not returned yet */
// OBSOLETE   deprecated_registers_fetched ();
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Store gdb's view of registers to the thread.
// OBSOLETE  * All registers are always valid when entering here.
// OBSOLETE  * @@ ahem, maybe that is too strict, we could validate the necessary ones
// OBSOLETE  *    here.
// OBSOLETE  *
// OBSOLETE  * Hmm. It seems that gdb set $reg=value command first reads everything,
// OBSOLETE  * then sets the reg and then stores everything. -> we must make sure
// OBSOLETE  * that the immutable registers are not changed by reading them first.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE store_inferior_registers (register int regno)
// OBSOLETE {
// OBSOLETE   thread_state_data_t state;
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (current_thread))
// OBSOLETE     error ("store inferior registers: Invalid thread");
// OBSOLETE 
// OBSOLETE   /* Check for read only regs.
// OBSOLETE    * @@ If some of these is can be changed, fix this
// OBSOLETE    */
// OBSOLETE   if (regno == ZERO_REGNUM ||
// OBSOLETE       regno == PS_REGNUM ||
// OBSOLETE       regno == BADVADDR_REGNUM ||
// OBSOLETE       regno == CAUSE_REGNUM ||
// OBSOLETE       regno == FCRIR_REGNUM)
// OBSOLETE     {
// OBSOLETE       message ("You can not alter read-only register `%s'",
// OBSOLETE 	       REGISTER_NAME (regno));
// OBSOLETE       fetch_inferior_registers (regno);
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (regno == -1)
// OBSOLETE     {
// OBSOLETE       /* Don't allow these to change */
// OBSOLETE 
// OBSOLETE       /* ZERO_REGNUM */
// OBSOLETE       *(int *) deprecated_registers = 0;
// OBSOLETE 
// OBSOLETE       fetch_inferior_registers (PS_REGNUM);
// OBSOLETE       fetch_inferior_registers (BADVADDR_REGNUM);
// OBSOLETE       fetch_inferior_registers (CAUSE_REGNUM);
// OBSOLETE       fetch_inferior_registers (FCRIR_REGNUM);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (regno == -1 || (ZERO_REGNUM < regno && regno <= PC_REGNUM))
// OBSOLETE     {
// OBSOLETE #if 1
// OBSOLETE       /* Mach 3.0 saves thread's FP to MACH_FP_REGNUM.
// OBSOLETE        * GDB wants assigns a pseudo register FP_REGNUM for frame pointer.
// OBSOLETE        *
// OBSOLETE        * @@@ Here I assume (!) that gdb's FP has the value that
// OBSOLETE        *     should go to threads frame pointer. If not true, this
// OBSOLETE        *     fails badly!!!!!
// OBSOLETE        */
// OBSOLETE       memcpy (&deprecated_registers[REGISTER_BYTE (MACH_FP_REGNUM)],
// OBSOLETE 	      &deprecated_registers[REGISTER_BYTE (FP_REGNUM)],
// OBSOLETE 	      REGISTER_RAW_SIZE (FP_REGNUM));
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE       /* Save gdb's regs 1..31 to thread saved regs 1..31
// OBSOLETE        * Luckily, they are contiquous
// OBSOLETE        */
// OBSOLETE       STORE_REGS (state, 1, 31);
// OBSOLETE 
// OBSOLETE       /* Save mdlo, mdhi */
// OBSOLETE       STORE_REGS (state, LO_REGNUM, 2);
// OBSOLETE 
// OBSOLETE       /* Save PC */
// OBSOLETE       STORE_REGS (state, PC_REGNUM, 1);
// OBSOLETE 
// OBSOLETE       ret = thread_set_state (current_thread,
// OBSOLETE 			      MIPS_THREAD_STATE,
// OBSOLETE 			      state,
// OBSOLETE 			      MIPS_FLOAT_STATE_COUNT);
// OBSOLETE       CHK ("store inferior regs : thread_set_state", ret);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (regno == -1 || regno >= FP0_REGNUM)
// OBSOLETE     {
// OBSOLETE       /* If thread has floating state, save it */
// OBSOLETE       if (read_register (PS_REGNUM) & MIPS_STATUS_USE_COP1)
// OBSOLETE 	{
// OBSOLETE 	  /* Do NOT save FCRIR_REGNUM */
// OBSOLETE 	  STORE_REGS (state, FP0_REGNUM, 33);
// OBSOLETE 
// OBSOLETE 	  ret = thread_set_state (current_thread,
// OBSOLETE 				  MIPS_FLOAT_STATE,
// OBSOLETE 				  state,
// OBSOLETE 				  MIPS_FLOAT_STATE_COUNT);
// OBSOLETE 	  CHK ("store inferior registers (floats): thread_set_state", ret);
// OBSOLETE 	}
// OBSOLETE       else if (regno != -1)
// OBSOLETE 	message
// OBSOLETE 	  ("Thread does not use floating point unit, floating regs not saved");
// OBSOLETE     }
// OBSOLETE }
