/* Target-dependent code for the SPARC 64 for GDB, the GNU debugger.
   Copyright 1986, 1987, 1989, 1991, 1992, 1993 Free Software Foundation, Inc.
   Contributed by Doug Evans (dje@cygnus.com).

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "obstack.h"
#include "target.h"

/*#include "symfile.h" /* for objfiles.h */
/*#include "objfiles.h" /* for find_pc_section */

/* This file contains replacements and additions to sparc-tdep.c only.
   Some of this code has been written for a day when we can merge at least
   some of this with sparc-tdep.c.  Macro TARGET_SPARC64 exists to allow some
   code to potentially be used by both.  */

#define TARGET_SPARC64 1	/* later make a config parm or some such */

/* From infrun.c */
extern int stop_after_trap;

/* Branches with prediction are treated like their non-predicting cousins.  */
/* FIXME: What about floating point branches?  */

typedef enum
{
  Error, not_branch, bicc, bicca, ba, baa, ticc, ta, done_retry
} branch_type;

/* Simulate single-step ptrace call for sun4.  Code written by Gary
   Beihl (beihl@mcc.com).  */

/* npc4 and next_pc describe the situation at the time that the
   step-breakpoint was set, not necessary the current value of NPC_REGNUM.  */
static CORE_ADDR next_pc, npc4, target;
static int brknpc4, brktrg;
typedef char binsn_quantum[BREAKPOINT_MAX];
static binsn_quantum break_mem[3];

/* Non-zero if we just simulated a single-step ptrace call.  This is
   needed because we cannot remove the breakpoints in the inferior
   process until after the `wait' in `wait_for_inferior'.  Used for
   sun4. */

int one_stepped;

/* sparc64_single_step() is called just before we want to resume the inferior,
   if we want to single-step it but there is no hardware or kernel single-step
   support (as on all SPARCs).  We find all the possible targets of the
   coming instruction and breakpoint them.

   single_step is also called just after the inferior stops.  If we had
   set up a simulated single-step, we undo our damage.  */

/* FIXME: When the code is releasable, sparc's single step could become this
   one, removing the duplication.  */

void
sparc64_single_step (ignore)
     int ignore; /* pid, but we don't need it */
{
  branch_type br, isbranch();
  CORE_ADDR pc;
  long pc_instruction;

  if (!one_stepped)
    {
      /* Always set breakpoint for NPC.  */
      next_pc = read_register (NPC_REGNUM);
      npc4 = next_pc + 4; /* branch not taken */

      target_insert_breakpoint (next_pc, break_mem[0]);
      /* printf_unfiltered ("set break at %x\n",next_pc); */

      pc = read_register (PC_REGNUM);
      pc_instruction = read_memory_integer (pc, sizeof(pc_instruction));
      br = isbranch (pc_instruction, pc, &target);
      brknpc4 = brktrg = 0;

      if (br == bicca)
	{
	  /* Conditional annulled branch will either end up at
	     npc (if taken) or at npc+4 (if not taken).
	     Trap npc+4.  */
	  brknpc4 = 1;
	  target_insert_breakpoint (npc4, break_mem[1]);
	}
      else if ((br == baa && target != next_pc)
	       || (TARGET_SPARC64 && br == done_retry))
	{
	  /* Unconditional annulled branch will always end up at
	     the target.  */
	  brktrg = 1;
	  target_insert_breakpoint (target, break_mem[2]);
	}

      /* We are ready to let it go */
      one_stepped = 1;
      return;
    }
  else
    {
      /* Remove breakpoints */
      target_remove_breakpoint (next_pc, break_mem[0]);

      if (brknpc4)
	target_remove_breakpoint (npc4, break_mem[1]);

      if (brktrg)
	target_remove_breakpoint (target, break_mem[2]);

      one_stepped = 0;
    }
}

CORE_ADDR
sparc64_extract_struct_value_address (regbuf)
     char regbuf[REGISTER_BYTES];
{
  CORE_ADDR addr;

  /* FIXME: We assume a non-leaf function.  */
  addr = read_register (I0_REGNUM);
  return addr;
}

/* Check instruction at ADDR to see if it is an annulled branch or other
   instruction whose npc isn't pc+4 (eg: trap, done, retry).
   All other instructions will go to NPC or will trap.
   Set *TARGET if we find a candidate branch; set to zero if not. */
   
branch_type
isbranch (instruction, addr, target)
     long instruction;
     CORE_ADDR addr, *target;
{
  branch_type val = not_branch;
  long int offset;		/* Must be signed for sign-extend.  */
  union
    {
      unsigned long int code;
      struct
	{
	  unsigned int op:2;
	  unsigned int a:1;
	  unsigned int cond:4;
	  unsigned int op2:3;
	  unsigned int disp22:22;
	} b;
      struct
	{
	  unsigned int op:2;
	  unsigned int a:1;
	  unsigned int cond:4;
	  unsigned int op2:3;
	  unsigned int cc:2;
	  unsigned int p:1;
	  unsigned int disp19:19;
	} bp;
      struct
	{
	  unsigned int op:2;
	  unsigned int a:1;
	  unsigned int zero:1;
	  unsigned int rcond:3;
	  unsigned int op2:3;
	  unsigned int disp16hi:2;
	  unsigned int p:1;
	  unsigned int rs1:5;
	  unsigned int disp16lo:14;
	} bpr;
      struct
	{
	  unsigned int op:2;
	  unsigned int fcn:5;
	  unsigned int op3:6;
	  unsigned int reserved:19;
	} dr;
    } insn;

  *target = 0;
  insn.code = instruction;

  if (insn.b.op == 0
      && (insn.b.op2 == 1 || insn.b.op2 == 2 || insn.b.op2 ==3
	  || insn.b.op2 == 5 || insn.b.op2 == 6))
    {
      if (insn.b.cond == 8)
	val = insn.b.a ? baa : ba;
      else
	val = insn.b.a ? bicca : bicc;
      switch (insn.b.op2)
	{
	case 1: /* bpcc */
	  offset = 4 * ((int) (insn.bp.disp19 << 13) >> 13);
	  break;
	case 2: /* bicc */
	  offset = 4 * ((int) (insn.b.disp22 << 10) >> 10);
	  break;
	case 3: /* bpr */
	  offset = 4 * ((int) ((insn.bpr.disp16hi << 10)
			       || (insn.bpr.disp16lo << 18)) >> 13);
	  break;
	case 5: /* fbpfcc */
	  offset = 4 * ((int) (insn.bp.disp19 << 13) >> 13);
	  break;
	case 6: /* fbfcc */
	  offset = 4 * ((int) (insn.b.disp22 << 10) >> 10);
	  break;
	}
      *target = addr + offset;
    }
  else if (insn.dr.op == 2 && insn.dr.op3 == 62)
    {
      if (insn.dr.fcn == 0)
	{
	  /* done */
	  *target = read_register (TNPC_REGNUM);
	  val = done_retry;
	}
      else if (insn.dr.fcn == 1)
	{
	  /* retry */
	  *target = read_register (TPC_REGNUM);
	  val = done_retry;
	}
    }

  return val;
}

/* PRINT_REGISTER_HOOK routine.
   Pretty print various registers.  */

static void
dump_ccreg (reg, val)
     char *reg;
     int val;
{
  printf_unfiltered ("%s:%s,%s,%s,%s", reg,
	  val & 8 ? "N" : "NN",
	  val & 4 ? "Z" : "NZ",
	  val & 2 ? "O" : "NO",
	  val & 1 ? "C" : "NC"
  );
}

void
sparc_print_register_hook (regno)
     int regno;
{
  if (((unsigned) (regno) - FP0_REGNUM < FP_MAX_REGNUM - FP0_REGNUM)
       && ((regno) & 1) == 0)
    {
      char doublereg[8];		/* two float regs */
      if (!read_relative_register_raw_bytes ((regno), doublereg))
	{
	  printf_unfiltered("\t");
	  print_floating (doublereg, builtin_type_double, gdb_stdout);
	}
    }
  else if ((regno) == CCR_REGNUM)
    {
      int ccr = read_register (CCR_REGNUM);
      printf_unfiltered("\t");
      dump_ccreg ("xcc", ccr >> 4);
      printf_unfiltered(", ");
      dump_ccreg ("icc", ccr & 15);
    }
}
