/* Target-dependent code for GDB, the GNU debugger.
   Copyright 1986, 1987, 1989, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 2000
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

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "symtab.h"
#include "target.h"
#include "gdbcore.h"
#include "gdbcmd.h"
#include "symfile.h"
#include "objfiles.h"
#include "xcoffsolib.h"
#include "arch-utils.h"

#include "bfd/libbfd.h"		/* for bfd_default_set_arch_mach */
#include "coff/internal.h"	/* for libcoff.h */
#include "bfd/libcoff.h"	/* for xcoff_data */

#include "elf-bfd.h"

#include "ppc-tdep.h"

/* If the kernel has to deliver a signal, it pushes a sigcontext
   structure on the stack and then calls the signal handler, passing
   the address of the sigcontext in an argument register. Usually
   the signal handler doesn't save this register, so we have to
   access the sigcontext structure via an offset from the signal handler
   frame.
   The following constants were determined by experimentation on AIX 3.2.  */
#define SIG_FRAME_PC_OFFSET 96
#define SIG_FRAME_LR_OFFSET 108
#define SIG_FRAME_FP_OFFSET 284

/* To be used by skip_prologue. */

struct rs6000_framedata
  {
    int offset;			/* total size of frame --- the distance
				   by which we decrement sp to allocate
				   the frame */
    int saved_gpr;		/* smallest # of saved gpr */
    int saved_fpr;		/* smallest # of saved fpr */
    int alloca_reg;		/* alloca register number (frame ptr) */
    char frameless;		/* true if frameless functions. */
    char nosavedpc;		/* true if pc not saved. */
    int gpr_offset;		/* offset of saved gprs from prev sp */
    int fpr_offset;		/* offset of saved fprs from prev sp */
    int lr_offset;		/* offset of saved lr */
    int cr_offset;		/* offset of saved cr */
  };

/* Description of a single register. */

struct reg
  {
    char *name;			/* name of register */
    unsigned char sz32;		/* size on 32-bit arch, 0 if nonextant */
    unsigned char sz64;		/* size on 64-bit arch, 0 if nonextant */
    unsigned char fpr;		/* whether register is floating-point */
  };

/* Private data that this module attaches to struct gdbarch. */

struct gdbarch_tdep
  {
    int wordsize;		/* size in bytes of fixed-point word */
    int osabi;			/* OS / ABI from ELF header */
    int *regoff;		/* byte offsets in register arrays */
    const struct reg *regs;	/* from current variant */
  };

/* Return the current architecture's gdbarch_tdep structure. */

#define TDEP	gdbarch_tdep (current_gdbarch)

/* Breakpoint shadows for the single step instructions will be kept here. */

static struct sstep_breaks
  {
    /* Address, or 0 if this is not in use.  */
    CORE_ADDR address;
    /* Shadow contents.  */
    char data[4];
  }
stepBreaks[2];

/* Hook for determining the TOC address when calling functions in the
   inferior under AIX. The initialization code in rs6000-nat.c sets
   this hook to point to find_toc_address.  */

CORE_ADDR (*rs6000_find_toc_address_hook) (CORE_ADDR) = NULL;

/* Hook to set the current architecture when starting a child process. 
   rs6000-nat.c sets this. */

void (*rs6000_set_host_arch_hook) (int) = NULL;

/* Static function prototypes */

static CORE_ADDR branch_dest (int opcode, int instr, CORE_ADDR pc,
			      CORE_ADDR safety);
static CORE_ADDR skip_prologue (CORE_ADDR, CORE_ADDR,
                                struct rs6000_framedata *);
static void frame_get_saved_regs (struct frame_info * fi,
				  struct rs6000_framedata * fdatap);
static CORE_ADDR frame_initial_stack_address (struct frame_info *);

/* Read a LEN-byte address from debugged memory address MEMADDR. */

static CORE_ADDR
read_memory_addr (CORE_ADDR memaddr, int len)
{
  return read_memory_unsigned_integer (memaddr, len);
}

static CORE_ADDR
rs6000_skip_prologue (CORE_ADDR pc)
{
  struct rs6000_framedata frame;
  pc = skip_prologue (pc, 0, &frame);
  return pc;
}


/* Fill in fi->saved_regs */

struct frame_extra_info
{
  /* Functions calling alloca() change the value of the stack
     pointer. We need to use initial stack pointer (which is saved in
     r31 by gcc) in such cases. If a compiler emits traceback table,
     then we should use the alloca register specified in traceback
     table. FIXME. */
  CORE_ADDR initial_sp;		/* initial stack pointer. */
};

void
rs6000_init_extra_frame_info (int fromleaf, struct frame_info *fi)
{
  fi->extra_info = (struct frame_extra_info *)
    frame_obstack_alloc (sizeof (struct frame_extra_info));
  fi->extra_info->initial_sp = 0;
  if (fi->next != (CORE_ADDR) 0
      && fi->pc < TEXT_SEGMENT_BASE)
    /* We're in get_prev_frame */
    /* and this is a special signal frame.  */
    /* (fi->pc will be some low address in the kernel, */
    /*  to which the signal handler returns).  */
    fi->signal_handler_caller = 1;
}

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

/* In this implementation for RS/6000, we do *not* save sp. I am
   not sure if it will be needed. The following function takes care of gpr's
   and fpr's only. */

void
rs6000_frame_init_saved_regs (struct frame_info *fi)
{
  frame_get_saved_regs (fi, NULL);
}

static CORE_ADDR
rs6000_frame_args_address (struct frame_info *fi)
{
  if (fi->extra_info->initial_sp != 0)
    return fi->extra_info->initial_sp;
  else
    return frame_initial_stack_address (fi);
}

/* Immediately after a function call, return the saved pc.
   Can't go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.  */

static CORE_ADDR
rs6000_saved_pc_after_call (struct frame_info *fi)
{
  return read_register (PPC_LR_REGNUM);
}

/* Calculate the destination of a branch/jump.  Return -1 if not a branch.  */

static CORE_ADDR
branch_dest (int opcode, int instr, CORE_ADDR pc, CORE_ADDR safety)
{
  CORE_ADDR dest;
  int immediate;
  int absolute;
  int ext_op;

  absolute = (int) ((instr >> 1) & 1);

  switch (opcode)
    {
    case 18:
      immediate = ((instr & ~3) << 6) >> 6;	/* br unconditional */
      if (absolute)
	dest = immediate;
      else
	dest = pc + immediate;
      break;

    case 16:
      immediate = ((instr & ~3) << 16) >> 16;	/* br conditional */
      if (absolute)
	dest = immediate;
      else
	dest = pc + immediate;
      break;

    case 19:
      ext_op = (instr >> 1) & 0x3ff;

      if (ext_op == 16)		/* br conditional register */
	{
	  dest = read_register (PPC_LR_REGNUM) & ~3;

	  /* If we are about to return from a signal handler, dest is
	     something like 0x3c90.  The current frame is a signal handler
	     caller frame, upon completion of the sigreturn system call
	     execution will return to the saved PC in the frame.  */
	  if (dest < TEXT_SEGMENT_BASE)
	    {
	      struct frame_info *fi;

	      fi = get_current_frame ();
	      if (fi != NULL)
		dest = read_memory_addr (fi->frame + SIG_FRAME_PC_OFFSET,
					 TDEP->wordsize);
	    }
	}

      else if (ext_op == 528)	/* br cond to count reg */
	{
	  dest = read_register (PPC_CTR_REGNUM) & ~3;

	  /* If we are about to execute a system call, dest is something
	     like 0x22fc or 0x3b00.  Upon completion the system call
	     will return to the address in the link register.  */
	  if (dest < TEXT_SEGMENT_BASE)
	    dest = read_register (PPC_LR_REGNUM) & ~3;
	}
      else
	return -1;
      break;

    default:
      return -1;
    }
  return (dest < TEXT_SEGMENT_BASE) ? safety : dest;
}


/* Sequence of bytes for breakpoint instruction.  */

#define BIG_BREAKPOINT { 0x7d, 0x82, 0x10, 0x08 }
#define LITTLE_BREAKPOINT { 0x08, 0x10, 0x82, 0x7d }

static unsigned char *
rs6000_breakpoint_from_pc (CORE_ADDR *bp_addr, int *bp_size)
{
  static unsigned char big_breakpoint[] = BIG_BREAKPOINT;
  static unsigned char little_breakpoint[] = LITTLE_BREAKPOINT;
  *bp_size = 4;
  if (TARGET_BYTE_ORDER == BIG_ENDIAN)
    return big_breakpoint;
  else
    return little_breakpoint;
}


/* AIX does not support PT_STEP. Simulate it. */

void
rs6000_software_single_step (unsigned int signal, int insert_breakpoints_p)
{
#define	INSNLEN(OPCODE)	 4

  static char le_breakp[] = LITTLE_BREAKPOINT;
  static char be_breakp[] = BIG_BREAKPOINT;
  char *breakp = TARGET_BYTE_ORDER == BIG_ENDIAN ? be_breakp : le_breakp;
  int ii, insn;
  CORE_ADDR loc;
  CORE_ADDR breaks[2];
  int opcode;

  if (insert_breakpoints_p)
    {

      loc = read_pc ();

      insn = read_memory_integer (loc, 4);

      breaks[0] = loc + INSNLEN (insn);
      opcode = insn >> 26;
      breaks[1] = branch_dest (opcode, insn, loc, breaks[0]);

      /* Don't put two breakpoints on the same address. */
      if (breaks[1] == breaks[0])
	breaks[1] = -1;

      stepBreaks[1].address = 0;

      for (ii = 0; ii < 2; ++ii)
	{

	  /* ignore invalid breakpoint. */
	  if (breaks[ii] == -1)
	    continue;

	  read_memory (breaks[ii], stepBreaks[ii].data, 4);

	  write_memory (breaks[ii], breakp, 4);
	  stepBreaks[ii].address = breaks[ii];
	}

    }
  else
    {

      /* remove step breakpoints. */
      for (ii = 0; ii < 2; ++ii)
	if (stepBreaks[ii].address != 0)
	  write_memory
	    (stepBreaks[ii].address, stepBreaks[ii].data, 4);

    }
  errno = 0;			/* FIXME, don't ignore errors! */
  /* What errors?  {read,write}_memory call error().  */
}


/* return pc value after skipping a function prologue and also return
   information about a function frame.

   in struct rs6000_framedata fdata:
   - frameless is TRUE, if function does not have a frame.
   - nosavedpc is TRUE, if function does not save %pc value in its frame.
   - offset is the initial size of this stack frame --- the amount by
   which we decrement the sp to allocate the frame.
   - saved_gpr is the number of the first saved gpr.
   - saved_fpr is the number of the first saved fpr.
   - alloca_reg is the number of the register used for alloca() handling.
   Otherwise -1.
   - gpr_offset is the offset of the first saved gpr from the previous frame.
   - fpr_offset is the offset of the first saved fpr from the previous frame.
   - lr_offset is the offset of the saved lr
   - cr_offset is the offset of the saved cr
 */

#define SIGNED_SHORT(x) 						\
  ((sizeof (short) == 2)						\
   ? ((int)(short)(x))							\
   : ((int)((((x) & 0xffff) ^ 0x8000) - 0x8000)))

#define GET_SRC_REG(x) (((x) >> 21) & 0x1f)

static CORE_ADDR
skip_prologue (CORE_ADDR pc, CORE_ADDR lim_pc, struct rs6000_framedata *fdata)
{
  CORE_ADDR orig_pc = pc;
  CORE_ADDR last_prologue_pc;
  char buf[4];
  unsigned long op;
  long offset = 0;
  int lr_reg = -1;
  int cr_reg = -1;
  int reg;
  int framep = 0;
  int minimal_toc_loaded = 0;
  int prev_insn_was_prologue_insn = 1;

  memset (fdata, 0, sizeof (struct rs6000_framedata));
  fdata->saved_gpr = -1;
  fdata->saved_fpr = -1;
  fdata->alloca_reg = -1;
  fdata->frameless = 1;
  fdata->nosavedpc = 1;

  pc -= 4;
  while (lim_pc == 0 || pc < lim_pc - 4)
    {
      pc += 4;

      /* Sometimes it isn't clear if an instruction is a prologue
         instruction or not.  When we encounter one of these ambiguous
	 cases, we'll set prev_insn_was_prologue_insn to 0 (false).
	 Otherwise, we'll assume that it really is a prologue instruction. */
      if (prev_insn_was_prologue_insn)
	last_prologue_pc = pc;
      prev_insn_was_prologue_insn = 1;

      if (target_read_memory (pc, buf, 4))
	break;
      op = extract_signed_integer (buf, 4);

      if ((op & 0xfc1fffff) == 0x7c0802a6)
	{			/* mflr Rx */
	  lr_reg = (op & 0x03e00000) | 0x90010000;
	  continue;

	}
      else if ((op & 0xfc1fffff) == 0x7c000026)
	{			/* mfcr Rx */
	  cr_reg = (op & 0x03e00000) | 0x90010000;
	  continue;

	}
      else if ((op & 0xfc1f0000) == 0xd8010000)
	{			/* stfd Rx,NUM(r1) */
	  reg = GET_SRC_REG (op);
	  if (fdata->saved_fpr == -1 || fdata->saved_fpr > reg)
	    {
	      fdata->saved_fpr = reg;
	      fdata->fpr_offset = SIGNED_SHORT (op) + offset;
	    }
	  continue;

	}
      else if (((op & 0xfc1f0000) == 0xbc010000) ||	/* stm Rx, NUM(r1) */
	       (((op & 0xfc1f0000) == 0x90010000 ||	/* st rx,NUM(r1) */
		 (op & 0xfc1f0003) == 0xf8010000) &&	/* std rx,NUM(r1) */
		(op & 0x03e00000) >= 0x01a00000))	/* rx >= r13 */
	{

	  reg = GET_SRC_REG (op);
	  if (fdata->saved_gpr == -1 || fdata->saved_gpr > reg)
	    {
	      fdata->saved_gpr = reg;
	      if ((op & 0xfc1f0003) == 0xf8010000)
		op = (op >> 1) << 1;
	      fdata->gpr_offset = SIGNED_SHORT (op) + offset;
	    }
	  continue;

	}
      else if ((op & 0xffff0000) == 0x60000000)
        {
	  			/* nop */
	  /* Allow nops in the prologue, but do not consider them to
	     be part of the prologue unless followed by other prologue
	     instructions. */
	  prev_insn_was_prologue_insn = 0;
	  continue;

	}
      else if ((op & 0xffff0000) == 0x3c000000)
	{			/* addis 0,0,NUM, used
				   for >= 32k frames */
	  fdata->offset = (op & 0x0000ffff) << 16;
	  fdata->frameless = 0;
	  continue;

	}
      else if ((op & 0xffff0000) == 0x60000000)
	{			/* ori 0,0,NUM, 2nd ha
				   lf of >= 32k frames */
	  fdata->offset |= (op & 0x0000ffff);
	  fdata->frameless = 0;
	  continue;

	}
      else if (lr_reg != -1 && (op & 0xffff0000) == lr_reg)
	{			/* st Rx,NUM(r1) 
				   where Rx == lr */
	  fdata->lr_offset = SIGNED_SHORT (op) + offset;
	  fdata->nosavedpc = 0;
	  lr_reg = 0;
	  continue;

	}
      else if (cr_reg != -1 && (op & 0xffff0000) == cr_reg)
	{			/* st Rx,NUM(r1) 
				   where Rx == cr */
	  fdata->cr_offset = SIGNED_SHORT (op) + offset;
	  cr_reg = 0;
	  continue;

	}
      else if (op == 0x48000005)
	{			/* bl .+4 used in 
				   -mrelocatable */
	  continue;

	}
      else if (op == 0x48000004)
	{			/* b .+4 (xlc) */
	  break;

	}
      else if (((op & 0xffff0000) == 0x801e0000 ||	/* lwz 0,NUM(r30), used
							   in V.4 -mrelocatable */
		op == 0x7fc0f214) &&	/* add r30,r0,r30, used
					   in V.4 -mrelocatable */
	       lr_reg == 0x901e0000)
	{
	  continue;

	}
      else if ((op & 0xffff0000) == 0x3fc00000 ||	/* addis 30,0,foo@ha, used
							   in V.4 -mminimal-toc */
	       (op & 0xffff0000) == 0x3bde0000)
	{			/* addi 30,30,foo@l */
	  continue;

	}
      else if ((op & 0xfc000001) == 0x48000001)
	{			/* bl foo, 
				   to save fprs??? */

	  fdata->frameless = 0;
	  /* Don't skip over the subroutine call if it is not within the first
	     three instructions of the prologue.  */
	  if ((pc - orig_pc) > 8)
	    break;

	  op = read_memory_integer (pc + 4, 4);

	  /* At this point, make sure this is not a trampoline function
	     (a function that simply calls another functions, and nothing else).
	     If the next is not a nop, this branch was part of the function
	     prologue. */

	  if (op == 0x4def7b82 || op == 0)	/* crorc 15, 15, 15 */
	    break;		/* don't skip over 
				   this branch */
	  continue;

	  /* update stack pointer */
	}
      else if ((op & 0xffff0000) == 0x94210000 ||	/* stu r1,NUM(r1) */
	       (op & 0xffff0003) == 0xf8210001)		/* stdu r1,NUM(r1) */
	{
	  fdata->frameless = 0;
	  if ((op & 0xffff0003) == 0xf8210001)
	    op = (op >> 1) << 1;
	  fdata->offset = SIGNED_SHORT (op);
	  offset = fdata->offset;
	  continue;

	}
      else if (op == 0x7c21016e)
	{			/* stwux 1,1,0 */
	  fdata->frameless = 0;
	  offset = fdata->offset;
	  continue;

	  /* Load up minimal toc pointer */
	}
      else if ((op >> 22) == 0x20f
	       && !minimal_toc_loaded)
	{			/* l r31,... or l r30,... */
	  minimal_toc_loaded = 1;
	  continue;

	  /* move parameters from argument registers to local variable
             registers */
 	}
      else if ((op & 0xfc0007fe) == 0x7c000378 &&	/* mr(.)  Rx,Ry */
               (((op >> 21) & 31) >= 3) &&              /* R3 >= Ry >= R10 */
               (((op >> 21) & 31) <= 10) &&
               (((op >> 16) & 31) >= fdata->saved_gpr)) /* Rx: local var reg */
	{
	  continue;

	  /* store parameters in stack */
	}
      else if ((op & 0xfc1f0000) == 0x90010000 ||	/* st rx,NUM(r1) */
	       (op & 0xfc1f0003) == 0xf8010000 ||	/* std rx,NUM(r1) */
	       (op & 0xfc1f0000) == 0xd8010000 ||	/* stfd Rx,NUM(r1) */
	       (op & 0xfc1f0000) == 0xfc010000)		/* frsp, fp?,NUM(r1) */
	{
	  continue;

	  /* store parameters in stack via frame pointer */
	}
      else if (framep &&
	       ((op & 0xfc1f0000) == 0x901f0000 ||	/* st rx,NUM(r1) */
		(op & 0xfc1f0000) == 0xd81f0000 ||	/* stfd Rx,NUM(r1) */
		(op & 0xfc1f0000) == 0xfc1f0000))
	{			/* frsp, fp?,NUM(r1) */
	  continue;

	  /* Set up frame pointer */
	}
      else if (op == 0x603f0000	/* oril r31, r1, 0x0 */
	       || op == 0x7c3f0b78)
	{			/* mr r31, r1 */
	  fdata->frameless = 0;
	  framep = 1;
	  fdata->alloca_reg = 31;
	  continue;

	  /* Another way to set up the frame pointer.  */
	}
      else if ((op & 0xfc1fffff) == 0x38010000)
	{			/* addi rX, r1, 0x0 */
	  fdata->frameless = 0;
	  framep = 1;
	  fdata->alloca_reg = (op & ~0x38010000) >> 21;
	  continue;

	}
      else
	{
	  break;
	}
    }

#if 0
/* I have problems with skipping over __main() that I need to address
 * sometime. Previously, I used to use misc_function_vector which
 * didn't work as well as I wanted to be.  -MGO */

  /* If the first thing after skipping a prolog is a branch to a function,
     this might be a call to an initializer in main(), introduced by gcc2.
     We'd like to skip over it as well. Fortunately, xlc does some extra
     work before calling a function right after a prologue, thus we can
     single out such gcc2 behaviour. */


  if ((op & 0xfc000001) == 0x48000001)
    {				/* bl foo, an initializer function? */
      op = read_memory_integer (pc + 4, 4);

      if (op == 0x4def7b82)
	{			/* cror 0xf, 0xf, 0xf (nop) */

	  /* check and see if we are in main. If so, skip over this initializer
	     function as well. */

	  tmp = find_pc_misc_function (pc);
	  if (tmp >= 0 && STREQ (misc_function_vector[tmp].name, "main"))
	    return pc + 8;
	}
    }
#endif /* 0 */

  fdata->offset = -fdata->offset;
  return last_prologue_pc;
}


/*************************************************************************
  Support for creating pushing a dummy frame into the stack, and popping
  frames, etc. 
*************************************************************************/


/* Pop the innermost frame, go back to the caller. */

static void
rs6000_pop_frame (void)
{
  CORE_ADDR pc, lr, sp, prev_sp, addr;	/* %pc, %lr, %sp */
  struct rs6000_framedata fdata;
  struct frame_info *frame = get_current_frame ();
  int ii, wordsize;

  pc = read_pc ();
  sp = FRAME_FP (frame);

  if (PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
    {
      generic_pop_dummy_frame ();
      flush_cached_frames ();
      return;
    }

  /* Make sure that all registers are valid.  */
  read_register_bytes (0, NULL, REGISTER_BYTES);

  /* figure out previous %pc value. If the function is frameless, it is 
     still in the link register, otherwise walk the frames and retrieve the
     saved %pc value in the previous frame. */

  addr = get_pc_function_start (frame->pc);
  (void) skip_prologue (addr, frame->pc, &fdata);

  wordsize = TDEP->wordsize;
  if (fdata.frameless)
    prev_sp = sp;
  else
    prev_sp = read_memory_addr (sp, wordsize);
  if (fdata.lr_offset == 0)
    lr = read_register (PPC_LR_REGNUM);
  else
    lr = read_memory_addr (prev_sp + fdata.lr_offset, wordsize);

  /* reset %pc value. */
  write_register (PC_REGNUM, lr);

  /* reset register values if any was saved earlier. */

  if (fdata.saved_gpr != -1)
    {
      addr = prev_sp + fdata.gpr_offset;
      for (ii = fdata.saved_gpr; ii <= 31; ++ii)
	{
	  read_memory (addr, &registers[REGISTER_BYTE (ii)], wordsize);
	  addr += wordsize;
	}
    }

  if (fdata.saved_fpr != -1)
    {
      addr = prev_sp + fdata.fpr_offset;
      for (ii = fdata.saved_fpr; ii <= 31; ++ii)
	{
	  read_memory (addr, &registers[REGISTER_BYTE (ii + FP0_REGNUM)], 8);
	  addr += 8;
	}
    }

  write_register (SP_REGNUM, prev_sp);
  target_store_registers (-1);
  flush_cached_frames ();
}

/* Fixup the call sequence of a dummy function, with the real function
   address.  Its arguments will be passed by gdb. */

static void
rs6000_fix_call_dummy (char *dummyname, CORE_ADDR pc, CORE_ADDR fun,
		       int nargs, value_ptr *args, struct type *type,
		       int gcc_p)
{
#define	TOC_ADDR_OFFSET		20
#define	TARGET_ADDR_OFFSET	28

  int ii;
  CORE_ADDR target_addr;

  if (rs6000_find_toc_address_hook != NULL)
    {
      CORE_ADDR tocvalue = (*rs6000_find_toc_address_hook) (fun);
      write_register (PPC_TOC_REGNUM, tocvalue);
    }
}

/* Pass the arguments in either registers, or in the stack. In RS/6000,
   the first eight words of the argument list (that might be less than
   eight parameters if some parameters occupy more than one word) are
   passed in r3..r10 registers.  float and double parameters are
   passed in fpr's, in addition to that. Rest of the parameters if any
   are passed in user stack. There might be cases in which half of the
   parameter is copied into registers, the other half is pushed into
   stack.

   Stack must be aligned on 64-bit boundaries when synthesizing
   function calls.

   If the function is returning a structure, then the return address is passed
   in r3, then the first 7 words of the parameters can be passed in registers,
   starting from r4. */

static CORE_ADDR
rs6000_push_arguments (int nargs, value_ptr *args, CORE_ADDR sp,
		       int struct_return, CORE_ADDR struct_addr)
{
  int ii;
  int len = 0;
  int argno;			/* current argument number */
  int argbytes;			/* current argument byte */
  char tmp_buffer[50];
  int f_argno = 0;		/* current floating point argno */
  int wordsize = TDEP->wordsize;

  value_ptr arg = 0;
  struct type *type;

  CORE_ADDR saved_sp;

  /* The first eight words of ther arguments are passed in registers. Copy
     them appropriately.

     If the function is returning a `struct', then the first word (which 
     will be passed in r3) is used for struct return address. In that
     case we should advance one word and start from r4 register to copy 
     parameters. */

  ii = struct_return ? 1 : 0;

/* 
   effectively indirect call... gcc does...

   return_val example( float, int);

   eabi: 
   float in fp0, int in r3
   offset of stack on overflow 8/16
   for varargs, must go by type.
   power open:
   float in r3&r4, int in r5
   offset of stack on overflow different 
   both: 
   return in r3 or f0.  If no float, must study how gcc emulates floats;
   pay attention to arg promotion.  
   User may have to cast\args to handle promotion correctly 
   since gdb won't know if prototype supplied or not.
 */

  for (argno = 0, argbytes = 0; argno < nargs && ii < 8; ++ii)
    {
      int reg_size = REGISTER_RAW_SIZE (ii + 3);

      arg = args[argno];
      type = check_typedef (VALUE_TYPE (arg));
      len = TYPE_LENGTH (type);

      if (TYPE_CODE (type) == TYPE_CODE_FLT)
	{

	  /* floating point arguments are passed in fpr's, as well as gpr's.
	     There are 13 fpr's reserved for passing parameters. At this point
	     there is no way we would run out of them. */

	  if (len > 8)
	    printf_unfiltered (
				"Fatal Error: a floating point parameter #%d with a size > 8 is found!\n", argno);

	  memcpy (&registers[REGISTER_BYTE (FP0_REGNUM + 1 + f_argno)],
		  VALUE_CONTENTS (arg),
		  len);
	  ++f_argno;
	}

      if (len > reg_size)
	{

	  /* Argument takes more than one register. */
	  while (argbytes < len)
	    {
	      memset (&registers[REGISTER_BYTE (ii + 3)], 0, reg_size);
	      memcpy (&registers[REGISTER_BYTE (ii + 3)],
		      ((char *) VALUE_CONTENTS (arg)) + argbytes,
		      (len - argbytes) > reg_size
		        ? reg_size : len - argbytes);
	      ++ii, argbytes += reg_size;

	      if (ii >= 8)
		goto ran_out_of_registers_for_arguments;
	    }
	  argbytes = 0;
	  --ii;
	}
      else
	{			/* Argument can fit in one register. No problem. */
	  int adj = TARGET_BYTE_ORDER == BIG_ENDIAN ? reg_size - len : 0;
	  memset (&registers[REGISTER_BYTE (ii + 3)], 0, reg_size);
	  memcpy ((char *)&registers[REGISTER_BYTE (ii + 3)] + adj, 
	          VALUE_CONTENTS (arg), len);
	}
      ++argno;
    }

ran_out_of_registers_for_arguments:

  saved_sp = read_sp ();
#ifndef ELF_OBJECT_FORMAT
  /* location for 8 parameters are always reserved. */
  sp -= wordsize * 8;

  /* another six words for back chain, TOC register, link register, etc. */
  sp -= wordsize * 6;

  /* stack pointer must be quadword aligned */
  sp &= -16;
#endif

  /* if there are more arguments, allocate space for them in 
     the stack, then push them starting from the ninth one. */

  if ((argno < nargs) || argbytes)
    {
      int space = 0, jj;

      if (argbytes)
	{
	  space += ((len - argbytes + 3) & -4);
	  jj = argno + 1;
	}
      else
	jj = argno;

      for (; jj < nargs; ++jj)
	{
	  value_ptr val = args[jj];
	  space += ((TYPE_LENGTH (VALUE_TYPE (val))) + 3) & -4;
	}

      /* add location required for the rest of the parameters */
      space = (space + 15) & -16;
      sp -= space;

      /* This is another instance we need to be concerned about securing our
         stack space. If we write anything underneath %sp (r1), we might conflict
         with the kernel who thinks he is free to use this area. So, update %sp
         first before doing anything else. */

      write_register (SP_REGNUM, sp);

      /* if the last argument copied into the registers didn't fit there 
         completely, push the rest of it into stack. */

      if (argbytes)
	{
	  write_memory (sp + 24 + (ii * 4),
			((char *) VALUE_CONTENTS (arg)) + argbytes,
			len - argbytes);
	  ++argno;
	  ii += ((len - argbytes + 3) & -4) / 4;
	}

      /* push the rest of the arguments into stack. */
      for (; argno < nargs; ++argno)
	{

	  arg = args[argno];
	  type = check_typedef (VALUE_TYPE (arg));
	  len = TYPE_LENGTH (type);


	  /* float types should be passed in fpr's, as well as in the stack. */
	  if (TYPE_CODE (type) == TYPE_CODE_FLT && f_argno < 13)
	    {

	      if (len > 8)
		printf_unfiltered (
				    "Fatal Error: a floating point parameter #%d with a size > 8 is found!\n", argno);

	      memcpy (&registers[REGISTER_BYTE (FP0_REGNUM + 1 + f_argno)],
		      VALUE_CONTENTS (arg),
		      len);
	      ++f_argno;
	    }

	  write_memory (sp + 24 + (ii * 4), (char *) VALUE_CONTENTS (arg), len);
	  ii += ((len + 3) & -4) / 4;
	}
    }
  else
    /* Secure stack areas first, before doing anything else. */
    write_register (SP_REGNUM, sp);

  /* set back chain properly */
  store_address (tmp_buffer, 4, saved_sp);
  write_memory (sp, tmp_buffer, 4);

  target_store_registers (-1);
  return sp;
}

/* Function: ppc_push_return_address (pc, sp)
   Set up the return address for the inferior function call. */

static CORE_ADDR
ppc_push_return_address (CORE_ADDR pc, CORE_ADDR sp)
{
  write_register (PPC_LR_REGNUM, CALL_DUMMY_ADDRESS ());
  return sp;
}

/* Extract a function return value of type TYPE from raw register array
   REGBUF, and copy that return value into VALBUF in virtual format. */

static void
rs6000_extract_return_value (struct type *valtype, char *regbuf, char *valbuf)
{
  int offset = 0;

  if (TYPE_CODE (valtype) == TYPE_CODE_FLT)
    {

      double dd;
      float ff;
      /* floats and doubles are returned in fpr1. fpr's have a size of 8 bytes.
         We need to truncate the return value into float size (4 byte) if
         necessary. */

      if (TYPE_LENGTH (valtype) > 4)	/* this is a double */
	memcpy (valbuf,
		&regbuf[REGISTER_BYTE (FP0_REGNUM + 1)],
		TYPE_LENGTH (valtype));
      else
	{			/* float */
	  memcpy (&dd, &regbuf[REGISTER_BYTE (FP0_REGNUM + 1)], 8);
	  ff = (float) dd;
	  memcpy (valbuf, &ff, sizeof (float));
	}
    }
  else
    {
      /* return value is copied starting from r3. */
      if (TARGET_BYTE_ORDER == BIG_ENDIAN
	  && TYPE_LENGTH (valtype) < REGISTER_RAW_SIZE (3))
	offset = REGISTER_RAW_SIZE (3) - TYPE_LENGTH (valtype);

      memcpy (valbuf,
	      regbuf + REGISTER_BYTE (3) + offset,
	      TYPE_LENGTH (valtype));
    }
}

/* Keep structure return address in this variable.
   FIXME:  This is a horrid kludge which should not be allowed to continue
   living.  This only allows a single nested call to a structure-returning
   function.  Come on, guys!  -- gnu@cygnus.com, Aug 92  */

static CORE_ADDR rs6000_struct_return_address;

/* Indirect function calls use a piece of trampoline code to do context
   switching, i.e. to set the new TOC table. Skip such code if we are on
   its first instruction (as when we have single-stepped to here). 
   Also skip shared library trampoline code (which is different from
   indirect function call trampolines).
   Result is desired PC to step until, or NULL if we are not in
   trampoline code.  */

CORE_ADDR
rs6000_skip_trampoline_code (CORE_ADDR pc)
{
  register unsigned int ii, op;
  CORE_ADDR solib_target_pc;

  static unsigned trampoline_code[] =
  {
    0x800b0000,			/*     l   r0,0x0(r11)  */
    0x90410014,			/*    st   r2,0x14(r1)  */
    0x7c0903a6,			/* mtctr   r0           */
    0x804b0004,			/*     l   r2,0x4(r11)  */
    0x816b0008,			/*     l  r11,0x8(r11)  */
    0x4e800420,			/*  bctr                */
    0x4e800020,			/*    br                */
    0
  };

  /* If pc is in a shared library trampoline, return its target.  */
  solib_target_pc = find_solib_trampoline_target (pc);
  if (solib_target_pc)
    return solib_target_pc;

  for (ii = 0; trampoline_code[ii]; ++ii)
    {
      op = read_memory_integer (pc + (ii * 4), 4);
      if (op != trampoline_code[ii])
	return 0;
    }
  ii = read_register (11);	/* r11 holds destination addr   */
  pc = read_memory_addr (ii, TDEP->wordsize); /* (r11) value */
  return pc;
}

/* Determines whether the function FI has a frame on the stack or not.  */

int
rs6000_frameless_function_invocation (struct frame_info *fi)
{
  CORE_ADDR func_start;
  struct rs6000_framedata fdata;

  /* Don't even think about framelessness except on the innermost frame
     or if the function was interrupted by a signal.  */
  if (fi->next != NULL && !fi->next->signal_handler_caller)
    return 0;

  func_start = get_pc_function_start (fi->pc);

  /* If we failed to find the start of the function, it is a mistake
     to inspect the instructions. */

  if (!func_start)
    {
      /* A frame with a zero PC is usually created by dereferencing a NULL
         function pointer, normally causing an immediate core dump of the
         inferior. Mark function as frameless, as the inferior has no chance
         of setting up a stack frame.  */
      if (fi->pc == 0)
	return 1;
      else
	return 0;
    }

  (void) skip_prologue (func_start, fi->pc, &fdata);
  return fdata.frameless;
}

/* Return the PC saved in a frame */

CORE_ADDR
rs6000_frame_saved_pc (struct frame_info *fi)
{
  CORE_ADDR func_start;
  struct rs6000_framedata fdata;
  int wordsize = TDEP->wordsize;

  if (fi->signal_handler_caller)
    return read_memory_addr (fi->frame + SIG_FRAME_PC_OFFSET, wordsize);

  if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
    return generic_read_register_dummy (fi->pc, fi->frame, PC_REGNUM);

  func_start = get_pc_function_start (fi->pc);

  /* If we failed to find the start of the function, it is a mistake
     to inspect the instructions. */
  if (!func_start)
    return 0;

  (void) skip_prologue (func_start, fi->pc, &fdata);

  if (fdata.lr_offset == 0 && fi->next != NULL)
    {
      if (fi->next->signal_handler_caller)
	return read_memory_addr (fi->next->frame + SIG_FRAME_LR_OFFSET,
				 wordsize);
      else
	return read_memory_addr (FRAME_CHAIN (fi) + DEFAULT_LR_SAVE,
				 wordsize);
    }

  if (fdata.lr_offset == 0)
    return read_register (PPC_LR_REGNUM);

  return read_memory_addr (FRAME_CHAIN (fi) + fdata.lr_offset, wordsize);
}

/* If saved registers of frame FI are not known yet, read and cache them.
   &FDATAP contains rs6000_framedata; TDATAP can be NULL,
   in which case the framedata are read.  */

static void
frame_get_saved_regs (struct frame_info *fi, struct rs6000_framedata *fdatap)
{
  CORE_ADDR frame_addr;
  struct rs6000_framedata work_fdata;
  int wordsize = TDEP->wordsize;

  if (fi->saved_regs)
    return;

  if (fdatap == NULL)
    {
      fdatap = &work_fdata;
      (void) skip_prologue (get_pc_function_start (fi->pc), fi->pc, fdatap);
    }

  frame_saved_regs_zalloc (fi);

  /* If there were any saved registers, figure out parent's stack
     pointer. */
  /* The following is true only if the frame doesn't have a call to
     alloca(), FIXME. */

  if (fdatap->saved_fpr == 0 && fdatap->saved_gpr == 0
      && fdatap->lr_offset == 0 && fdatap->cr_offset == 0)
    frame_addr = 0;
  else if (fi->prev && fi->prev->frame)
    frame_addr = fi->prev->frame;
  else
    frame_addr = read_memory_addr (fi->frame, wordsize);

  /* if != -1, fdatap->saved_fpr is the smallest number of saved_fpr.
     All fpr's from saved_fpr to fp31 are saved.  */

  if (fdatap->saved_fpr >= 0)
    {
      int i;
      CORE_ADDR fpr_addr = frame_addr + fdatap->fpr_offset;
      for (i = fdatap->saved_fpr; i < 32; i++)
	{
	  fi->saved_regs[FP0_REGNUM + i] = fpr_addr;
	  fpr_addr += 8;
	}
    }

  /* if != -1, fdatap->saved_gpr is the smallest number of saved_gpr.
     All gpr's from saved_gpr to gpr31 are saved.  */

  if (fdatap->saved_gpr >= 0)
    {
      int i;
      CORE_ADDR gpr_addr = frame_addr + fdatap->gpr_offset;
      for (i = fdatap->saved_gpr; i < 32; i++)
	{
	  fi->saved_regs[i] = gpr_addr;
	  gpr_addr += wordsize;
	}
    }

  /* If != 0, fdatap->cr_offset is the offset from the frame that holds
     the CR.  */
  if (fdatap->cr_offset != 0)
    fi->saved_regs[PPC_CR_REGNUM] = frame_addr + fdatap->cr_offset;

  /* If != 0, fdatap->lr_offset is the offset from the frame that holds
     the LR.  */
  if (fdatap->lr_offset != 0)
    fi->saved_regs[PPC_LR_REGNUM] = frame_addr + fdatap->lr_offset;
}

/* Return the address of a frame. This is the inital %sp value when the frame
   was first allocated. For functions calling alloca(), it might be saved in
   an alloca register. */

static CORE_ADDR
frame_initial_stack_address (struct frame_info *fi)
{
  CORE_ADDR tmpaddr;
  struct rs6000_framedata fdata;
  struct frame_info *callee_fi;

  /* if the initial stack pointer (frame address) of this frame is known,
     just return it. */

  if (fi->extra_info->initial_sp)
    return fi->extra_info->initial_sp;

  /* find out if this function is using an alloca register.. */

  (void) skip_prologue (get_pc_function_start (fi->pc), fi->pc, &fdata);

  /* if saved registers of this frame are not known yet, read and cache them. */

  if (!fi->saved_regs)
    frame_get_saved_regs (fi, &fdata);

  /* If no alloca register used, then fi->frame is the value of the %sp for
     this frame, and it is good enough. */

  if (fdata.alloca_reg < 0)
    {
      fi->extra_info->initial_sp = fi->frame;
      return fi->extra_info->initial_sp;
    }

  /* This function has an alloca register. If this is the top-most frame
     (with the lowest address), the value in alloca register is good. */

  if (!fi->next)
    return fi->extra_info->initial_sp = read_register (fdata.alloca_reg);

  /* Otherwise, this is a caller frame. Callee has usually already saved
     registers, but there are exceptions (such as when the callee
     has no parameters). Find the address in which caller's alloca
     register is saved. */

  for (callee_fi = fi->next; callee_fi; callee_fi = callee_fi->next)
    {

      if (!callee_fi->saved_regs)
	frame_get_saved_regs (callee_fi, NULL);

      /* this is the address in which alloca register is saved. */

      tmpaddr = callee_fi->saved_regs[fdata.alloca_reg];
      if (tmpaddr)
	{
	  fi->extra_info->initial_sp =
	    read_memory_addr (tmpaddr, TDEP->wordsize);
	  return fi->extra_info->initial_sp;
	}

      /* Go look into deeper levels of the frame chain to see if any one of
         the callees has saved alloca register. */
    }

  /* If alloca register was not saved, by the callee (or any of its callees)
     then the value in the register is still good. */

  fi->extra_info->initial_sp = read_register (fdata.alloca_reg);
  return fi->extra_info->initial_sp;
}

/* Describe the pointer in each stack frame to the previous stack frame
   (its caller).  */

/* FRAME_CHAIN takes a frame's nominal address
   and produces the frame's chain-pointer. */

/* In the case of the RS/6000, the frame's nominal address
   is the address of a 4-byte word containing the calling frame's address.  */

CORE_ADDR
rs6000_frame_chain (struct frame_info *thisframe)
{
  CORE_ADDR fp, fpp, lr;
  int wordsize = TDEP->wordsize;

  if (PC_IN_CALL_DUMMY (thisframe->pc, thisframe->frame, thisframe->frame))
    return thisframe->frame;	/* dummy frame same as caller's frame */

  if (inside_entry_file (thisframe->pc) ||
      thisframe->pc == entry_point_address ())
    return 0;

  if (thisframe->signal_handler_caller)
    fp = read_memory_addr (thisframe->frame + SIG_FRAME_FP_OFFSET,
			      wordsize);
  else if (thisframe->next != NULL
	   && thisframe->next->signal_handler_caller
	   && FRAMELESS_FUNCTION_INVOCATION (thisframe))
    /* A frameless function interrupted by a signal did not change the
       frame pointer.  */
    fp = FRAME_FP (thisframe);
  else
    fp = read_memory_addr ((thisframe)->frame, wordsize);

  lr = read_register (PPC_LR_REGNUM);
  if (lr == entry_point_address ())
    if (fp != 0 && (fpp = read_memory_addr (fp, wordsize)) != 0)
      if (PC_IN_CALL_DUMMY (lr, fpp, fpp))
	return fpp;

  return fp;
}

/* Return the size of register REG when words are WORDSIZE bytes long.  If REG
   isn't available with that word size, return 0. */

static int
regsize (const struct reg *reg, int wordsize)
{
  return wordsize == 8 ? reg->sz64 : reg->sz32;
}

/* Return the name of register number N, or null if no such register exists
   in the current architecture. */

static char *
rs6000_register_name (int n)
{
  struct gdbarch_tdep *tdep = TDEP;
  const struct reg *reg = tdep->regs + n;

  if (!regsize (reg, tdep->wordsize))
    return NULL;
  return reg->name;
}

/* Index within `registers' of the first byte of the space for
   register N.  */

static int
rs6000_register_byte (int n)
{
  return TDEP->regoff[n];
}

/* Return the number of bytes of storage in the actual machine representation
   for register N if that register is available, else return 0. */

static int
rs6000_register_raw_size (int n)
{
  struct gdbarch_tdep *tdep = TDEP;
  const struct reg *reg = tdep->regs + n;
  return regsize (reg, tdep->wordsize);
}

/* Number of bytes of storage in the program's representation
   for register N.  */

static int
rs6000_register_virtual_size (int n)
{
  return TYPE_LENGTH (REGISTER_VIRTUAL_TYPE (n));
}

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

static struct type *
rs6000_register_virtual_type (int n)
{
  struct gdbarch_tdep *tdep = TDEP;
  const struct reg *reg = tdep->regs + n;

  return reg->fpr ? builtin_type_double :
    regsize (reg, tdep->wordsize) == 8 ? builtin_type_int64 :
      builtin_type_int32;
}

/* For the PowerPC, it appears that the debug info marks float parameters as
   floats regardless of whether the function is prototyped, but the actual
   values are always passed in as doubles.  Tell gdb to always assume that
   floats are passed as doubles and then converted in the callee. */

static int
rs6000_coerce_float_to_double (struct type *formal, struct type *actual)
{
  return 1;
}

/* Return whether register N requires conversion when moving from raw format
   to virtual format.

   The register format for RS/6000 floating point registers is always
   double, we need a conversion if the memory format is float. */

static int
rs6000_register_convertible (int n)
{
  const struct reg *reg = TDEP->regs + n;
  return reg->fpr;
}

/* Convert data from raw format for register N in buffer FROM
   to virtual format with type TYPE in buffer TO. */

static void
rs6000_register_convert_to_virtual (int n, struct type *type,
				    char *from, char *to)
{
  if (TYPE_LENGTH (type) != REGISTER_RAW_SIZE (n))
    {
      double val = extract_floating (from, REGISTER_RAW_SIZE (n));
      store_floating (to, TYPE_LENGTH (type), val);
    }
  else
    memcpy (to, from, REGISTER_RAW_SIZE (n));
}

/* Convert data from virtual format with type TYPE in buffer FROM
   to raw format for register N in buffer TO. */

static void
rs6000_register_convert_to_raw (struct type *type, int n,
				char *from, char *to)
{
  if (TYPE_LENGTH (type) != REGISTER_RAW_SIZE (n))
    {
      double val = extract_floating (from, TYPE_LENGTH (type));
      store_floating (to, REGISTER_RAW_SIZE (n), val);
    }
  else
    memcpy (to, from, REGISTER_RAW_SIZE (n));
}

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function.

   In RS/6000, struct return addresses are passed as an extra parameter in r3.
   In function return, callee is not responsible of returning this address
   back.  Since gdb needs to find it, we will store in a designated variable
   `rs6000_struct_return_address'. */

static void
rs6000_store_struct_return (CORE_ADDR addr, CORE_ADDR sp)
{
  write_register (3, addr);
  rs6000_struct_return_address = addr;
}

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  */

static void
rs6000_store_return_value (struct type *type, char *valbuf)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT)

    /* Floating point values are returned starting from FPR1 and up.
       Say a double_double_double type could be returned in
       FPR1/FPR2/FPR3 triple. */

    write_register_bytes (REGISTER_BYTE (FP0_REGNUM + 1), valbuf,
			  TYPE_LENGTH (type));
  else
    /* Everything else is returned in GPR3 and up. */
    write_register_bytes (REGISTER_BYTE (PPC_GP0_REGNUM + 3), valbuf,
			  TYPE_LENGTH (type));
}

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

static CORE_ADDR
rs6000_extract_struct_value_address (char *regbuf)
{
  return rs6000_struct_return_address;
}

/* Return whether PC is in a dummy function call.

   FIXME: This just checks for the end of the stack, which is broken
   for things like stepping through gcc nested function stubs. */

static int
rs6000_pc_in_call_dummy (CORE_ADDR pc, CORE_ADDR sp, CORE_ADDR fp)
{
  return sp < pc && pc < fp;
}

/* Hook called when a new child process is started. */

void
rs6000_create_inferior (int pid)
{
  if (rs6000_set_host_arch_hook)
    rs6000_set_host_arch_hook (pid);
}

/* Support for CONVERT_FROM_FUNC_PTR_ADDR(ADDR).

   Usually a function pointer's representation is simply the address
   of the function. On the RS/6000 however, a function pointer is
   represented by a pointer to a TOC entry. This TOC entry contains
   three words, the first word is the address of the function, the
   second word is the TOC pointer (r2), and the third word is the
   static chain value.  Throughout GDB it is currently assumed that a
   function pointer contains the address of the function, which is not
   easy to fix.  In addition, the conversion of a function address to
   a function pointer would require allocation of a TOC entry in the
   inferior's memory space, with all its drawbacks.  To be able to
   call C++ virtual methods in the inferior (which are called via
   function pointers), find_function_addr uses this macro to get the
   function address from a function pointer.  */

/* Return nonzero if ADDR (a function pointer) is in the data space and
   is therefore a special function pointer.  */

CORE_ADDR
rs6000_convert_from_func_ptr_addr (CORE_ADDR addr)
{
  struct obj_section *s;

  s = find_pc_section (addr);
  if (s && s->the_bfd_section->flags & SEC_CODE)
    return addr;

  /* ADDR is in the data space, so it's a special function pointer. */
  return read_memory_addr (addr, TDEP->wordsize);
}


/* Handling the various POWER/PowerPC variants.  */


/* The arrays here called registers_MUMBLE hold information about available
   registers.

   For each family of PPC variants, I've tried to isolate out the
   common registers and put them up front, so that as long as you get
   the general family right, GDB will correctly identify the registers
   common to that family.  The common register sets are:

   For the 60x family: hid0 hid1 iabr dabr pir

   For the 505 and 860 family: eie eid nri

   For the 403 and 403GC: icdbdr esr dear evpr cdbcr tsr tcr pit tbhi
   tblo srr2 srr3 dbsr dbcr iac1 iac2 dac1 dac2 dccr iccr pbl1
   pbu1 pbl2 pbu2

   Most of these register groups aren't anything formal.  I arrived at
   them by looking at the registers that occurred in more than one
   processor. */

/* Convenience macros for populating register arrays. */

/* Within another macro, convert S to a string. */

#define STR(s)	#s

/* Return a struct reg defining register NAME that's 32 bits on 32-bit systems
   and 64 bits on 64-bit systems. */
#define R(name)		{ STR(name), 4, 8, 0 }

/* Return a struct reg defining register NAME that's 32 bits on all
   systems. */
#define R4(name)	{ STR(name), 4, 4, 0 }

/* Return a struct reg defining register NAME that's 64 bits on all
   systems. */
#define R8(name)	{ STR(name), 8, 8, 0 }

/* Return a struct reg defining floating-point register NAME. */
#define F(name)		{ STR(name), 8, 8, 1 }

/* Return a struct reg defining register NAME that's 32 bits on 32-bit
   systems and that doesn't exist on 64-bit systems. */
#define R32(name)	{ STR(name), 4, 0, 0 }

/* Return a struct reg defining register NAME that's 64 bits on 64-bit
   systems and that doesn't exist on 32-bit systems. */
#define R64(name)	{ STR(name), 0, 8, 0 }

/* Return a struct reg placeholder for a register that doesn't exist. */
#define R0		{ 0, 0, 0, 0 }

/* UISA registers common across all architectures, including POWER.  */

#define COMMON_UISA_REGS \
  /*  0 */ R(r0), R(r1), R(r2), R(r3), R(r4), R(r5), R(r6), R(r7),  \
  /*  8 */ R(r8), R(r9), R(r10),R(r11),R(r12),R(r13),R(r14),R(r15), \
  /* 16 */ R(r16),R(r17),R(r18),R(r19),R(r20),R(r21),R(r22),R(r23), \
  /* 24 */ R(r24),R(r25),R(r26),R(r27),R(r28),R(r29),R(r30),R(r31), \
  /* 32 */ F(f0), F(f1), F(f2), F(f3), F(f4), F(f5), F(f6), F(f7),  \
  /* 40 */ F(f8), F(f9), F(f10),F(f11),F(f12),F(f13),F(f14),F(f15), \
  /* 48 */ F(f16),F(f17),F(f18),F(f19),F(f20),F(f21),F(f22),F(f23), \
  /* 56 */ F(f24),F(f25),F(f26),F(f27),F(f28),F(f29),F(f30),F(f31), \
  /* 64 */ R(pc), R(ps)

/* UISA-level SPRs for PowerPC.  */
#define PPC_UISA_SPRS \
  /* 66 */ R4(cr),  R(lr), R(ctr), R4(xer), R0

/* Segment registers, for PowerPC.  */
#define PPC_SEGMENT_REGS \
  /* 71 */ R32(sr0),  R32(sr1),  R32(sr2),  R32(sr3),  \
  /* 75 */ R32(sr4),  R32(sr5),  R32(sr6),  R32(sr7),  \
  /* 79 */ R32(sr8),  R32(sr9),  R32(sr10), R32(sr11), \
  /* 83 */ R32(sr12), R32(sr13), R32(sr14), R32(sr15)

/* OEA SPRs for PowerPC.  */
#define PPC_OEA_SPRS \
  /*  87 */ R4(pvr), \
  /*  88 */ R(ibat0u), R(ibat0l), R(ibat1u), R(ibat1l), \
  /*  92 */ R(ibat2u), R(ibat2l), R(ibat3u), R(ibat3l), \
  /*  96 */ R(dbat0u), R(dbat0l), R(dbat1u), R(dbat1l), \
  /* 100 */ R(dbat2u), R(dbat2l), R(dbat3u), R(dbat3l), \
  /* 104 */ R(sdr1),   R64(asr),  R(dar),    R4(dsisr), \
  /* 108 */ R(sprg0),  R(sprg1),  R(sprg2),  R(sprg3),  \
  /* 112 */ R(srr0),   R(srr1),   R(tbl),    R(tbu),    \
  /* 116 */ R4(dec),   R(dabr),   R4(ear)

/* IBM POWER (pre-PowerPC) architecture, user-level view.  We only cover
   user-level SPR's. */
static const struct reg registers_power[] =
{
  COMMON_UISA_REGS,
  /* 66 */ R4(cnd), R(lr), R(cnt), R4(xer), R4(mq)
};

/* PowerPC UISA - a PPC processor as viewed by user-level code.  A UISA-only
   view of the PowerPC. */
static const struct reg registers_powerpc[] =
{
  COMMON_UISA_REGS,
  PPC_UISA_SPRS
};

/* IBM PowerPC 403. */
static const struct reg registers_403[] =
{
  COMMON_UISA_REGS,
  PPC_UISA_SPRS,
  PPC_SEGMENT_REGS,
  PPC_OEA_SPRS,
  /* 119 */ R(icdbdr), R(esr),  R(dear), R(evpr),
  /* 123 */ R(cdbcr),  R(tsr),  R(tcr),  R(pit),
  /* 127 */ R(tbhi),   R(tblo), R(srr2), R(srr3),
  /* 131 */ R(dbsr),   R(dbcr), R(iac1), R(iac2),
  /* 135 */ R(dac1),   R(dac2), R(dccr), R(iccr),
  /* 139 */ R(pbl1),   R(pbu1), R(pbl2), R(pbu2)
};

/* IBM PowerPC 403GC. */
static const struct reg registers_403GC[] =
{
  COMMON_UISA_REGS,
  PPC_UISA_SPRS,
  PPC_SEGMENT_REGS,
  PPC_OEA_SPRS,
  /* 119 */ R(icdbdr), R(esr),  R(dear), R(evpr),
  /* 123 */ R(cdbcr),  R(tsr),  R(tcr),  R(pit),
  /* 127 */ R(tbhi),   R(tblo), R(srr2), R(srr3),
  /* 131 */ R(dbsr),   R(dbcr), R(iac1), R(iac2),
  /* 135 */ R(dac1),   R(dac2), R(dccr), R(iccr),
  /* 139 */ R(pbl1),   R(pbu1), R(pbl2), R(pbu2),
  /* 143 */ R(zpr),    R(pid),  R(sgr),  R(dcwr),
  /* 147 */ R(tbhu),   R(tblu)
};

/* Motorola PowerPC 505. */
static const struct reg registers_505[] =
{
  COMMON_UISA_REGS,
  PPC_UISA_SPRS,
  PPC_SEGMENT_REGS,
  PPC_OEA_SPRS,
  /* 119 */ R(eie), R(eid), R(nri)
};

/* Motorola PowerPC 860 or 850. */
static const struct reg registers_860[] =
{
  COMMON_UISA_REGS,
  PPC_UISA_SPRS,
  PPC_SEGMENT_REGS,
  PPC_OEA_SPRS,
  /* 119 */ R(eie), R(eid), R(nri), R(cmpa),
  /* 123 */ R(cmpb), R(cmpc), R(cmpd), R(icr),
  /* 127 */ R(der), R(counta), R(countb), R(cmpe),
  /* 131 */ R(cmpf), R(cmpg), R(cmph), R(lctrl1),
  /* 135 */ R(lctrl2), R(ictrl), R(bar), R(ic_cst),
  /* 139 */ R(ic_adr), R(ic_dat), R(dc_cst), R(dc_adr),
  /* 143 */ R(dc_dat), R(dpdr), R(dpir), R(immr),
  /* 147 */ R(mi_ctr), R(mi_ap), R(mi_epn), R(mi_twc),
  /* 151 */ R(mi_rpn), R(md_ctr), R(m_casid), R(md_ap),
  /* 155 */ R(md_epn), R(md_twb), R(md_twc), R(md_rpn),
  /* 159 */ R(m_tw), R(mi_dbcam), R(mi_dbram0), R(mi_dbram1),
  /* 163 */ R(md_dbcam), R(md_dbram0), R(md_dbram1)
};

/* Motorola PowerPC 601.  Note that the 601 has different register numbers
   for reading and writing RTCU and RTCL.  However, how one reads and writes a
   register is the stub's problem.  */
static const struct reg registers_601[] =
{
  COMMON_UISA_REGS,
  PPC_UISA_SPRS,
  PPC_SEGMENT_REGS,
  PPC_OEA_SPRS,
  /* 119 */ R(hid0), R(hid1), R(iabr), R(dabr),
  /* 123 */ R(pir), R(mq), R(rtcu), R(rtcl)
};

/* Motorola PowerPC 602. */
static const struct reg registers_602[] =
{
  COMMON_UISA_REGS,
  PPC_UISA_SPRS,
  PPC_SEGMENT_REGS,
  PPC_OEA_SPRS,
  /* 119 */ R(hid0), R(hid1), R(iabr), R0,
  /* 123 */ R0, R(tcr), R(ibr), R(esassr),
  /* 127 */ R(sebr), R(ser), R(sp), R(lt)
};

/* Motorola/IBM PowerPC 603 or 603e. */
static const struct reg registers_603[] =
{
  COMMON_UISA_REGS,
  PPC_UISA_SPRS,
  PPC_SEGMENT_REGS,
  PPC_OEA_SPRS,
  /* 119 */ R(hid0), R(hid1), R(iabr), R0,
  /* 123 */ R0, R(dmiss), R(dcmp), R(hash1),
  /* 127 */ R(hash2), R(imiss), R(icmp), R(rpa)
};

/* Motorola PowerPC 604 or 604e. */
static const struct reg registers_604[] =
{
  COMMON_UISA_REGS,
  PPC_UISA_SPRS,
  PPC_SEGMENT_REGS,
  PPC_OEA_SPRS,
  /* 119 */ R(hid0), R(hid1), R(iabr), R(dabr),
  /* 123 */ R(pir), R(mmcr0), R(pmc1), R(pmc2),
  /* 127 */ R(sia), R(sda)
};

/* Motorola/IBM PowerPC 750 or 740. */
static const struct reg registers_750[] =
{
  COMMON_UISA_REGS,
  PPC_UISA_SPRS,
  PPC_SEGMENT_REGS,
  PPC_OEA_SPRS,
  /* 119 */ R(hid0), R(hid1), R(iabr), R(dabr),
  /* 123 */ R0, R(ummcr0), R(upmc1), R(upmc2),
  /* 127 */ R(usia), R(ummcr1), R(upmc3), R(upmc4),
  /* 131 */ R(mmcr0), R(pmc1), R(pmc2), R(sia),
  /* 135 */ R(mmcr1), R(pmc3), R(pmc4), R(l2cr),
  /* 139 */ R(ictc), R(thrm1), R(thrm2), R(thrm3)
};


/* Information about a particular processor variant.  */

struct variant
  {
    /* Name of this variant.  */
    char *name;

    /* English description of the variant.  */
    char *description;

    /* bfd_arch_info.arch corresponding to variant. */
    enum bfd_architecture arch;

    /* bfd_arch_info.mach corresponding to variant. */
    unsigned long mach;

    /* Table of register names; registers[R] is the name of the register
       number R.  */
    int nregs;
    const struct reg *regs;
  };

#define num_registers(list) (sizeof (list) / sizeof((list)[0]))


/* Information in this table comes from the following web sites:
   IBM:       http://www.chips.ibm.com:80/products/embedded/
   Motorola:  http://www.mot.com/SPS/PowerPC/

   I'm sure I've got some of the variant descriptions not quite right.
   Please report any inaccuracies you find to GDB's maintainer.

   If you add entries to this table, please be sure to allow the new
   value as an argument to the --with-cpu flag, in configure.in.  */

static const struct variant variants[] =
{
  {"powerpc", "PowerPC user-level", bfd_arch_powerpc,
   bfd_mach_ppc, num_registers (registers_powerpc), registers_powerpc},
  {"power", "POWER user-level", bfd_arch_rs6000,
   bfd_mach_rs6k, num_registers (registers_power), registers_power},
  {"403", "IBM PowerPC 403", bfd_arch_powerpc,
   bfd_mach_ppc_403, num_registers (registers_403), registers_403},
  {"601", "Motorola PowerPC 601", bfd_arch_powerpc,
   bfd_mach_ppc_601, num_registers (registers_601), registers_601},
  {"602", "Motorola PowerPC 602", bfd_arch_powerpc,
   bfd_mach_ppc_602, num_registers (registers_602), registers_602},
  {"603", "Motorola/IBM PowerPC 603 or 603e", bfd_arch_powerpc,
   bfd_mach_ppc_603, num_registers (registers_603), registers_603},
  {"604", "Motorola PowerPC 604 or 604e", bfd_arch_powerpc,
   604, num_registers (registers_604), registers_604},
  {"403GC", "IBM PowerPC 403GC", bfd_arch_powerpc,
   bfd_mach_ppc_403gc, num_registers (registers_403GC), registers_403GC},
  {"505", "Motorola PowerPC 505", bfd_arch_powerpc,
   bfd_mach_ppc_505, num_registers (registers_505), registers_505},
  {"860", "Motorola PowerPC 860 or 850", bfd_arch_powerpc,
   bfd_mach_ppc_860, num_registers (registers_860), registers_860},
  {"750", "Motorola/IBM PowerPC 750 or 740", bfd_arch_powerpc,
   bfd_mach_ppc_750, num_registers (registers_750), registers_750},

  /* FIXME: I haven't checked the register sets of the following. */
  {"620", "Motorola PowerPC 620", bfd_arch_powerpc,
   bfd_mach_ppc_620, num_registers (registers_powerpc), registers_powerpc},
  {"a35", "PowerPC A35", bfd_arch_powerpc,
   bfd_mach_ppc_a35, num_registers (registers_powerpc), registers_powerpc},
  {"rs1", "IBM POWER RS1", bfd_arch_rs6000,
   bfd_mach_rs6k_rs1, num_registers (registers_power), registers_power},
  {"rsc", "IBM POWER RSC", bfd_arch_rs6000,
   bfd_mach_rs6k_rsc, num_registers (registers_power), registers_power},
  {"rs2", "IBM POWER RS2", bfd_arch_rs6000,
   bfd_mach_rs6k_rs2, num_registers (registers_power), registers_power},

  {0, 0, 0, 0}
};

#undef num_registers

/* Look up the variant named NAME in the `variants' table.  Return a
   pointer to the struct variant, or null if we couldn't find it.  */

static const struct variant *
find_variant_by_name (char *name)
{
  const struct variant *v;

  for (v = variants; v->name; v++)
    if (!strcmp (name, v->name))
      return v;

  return NULL;
}

/* Return the variant corresponding to architecture ARCH and machine number
   MACH.  If no such variant exists, return null. */

static const struct variant *
find_variant_by_arch (enum bfd_architecture arch, unsigned long mach)
{
  const struct variant *v;

  for (v = variants; v->name; v++)
    if (arch == v->arch && mach == v->mach)
      return v;

  return NULL;
}




static void
process_note_abi_tag_sections (bfd *abfd, asection *sect, void *obj)
{
  int *os_ident_ptr = obj;
  const char *name;
  unsigned int sectsize;

  name = bfd_get_section_name (abfd, sect);
  sectsize = bfd_section_size (abfd, sect);
  if (strcmp (name, ".note.ABI-tag") == 0 && sectsize > 0)
    {
      unsigned int name_length, data_length, note_type;
      char *note = alloca (sectsize);

      bfd_get_section_contents (abfd, sect, note,
                                (file_ptr) 0, (bfd_size_type) sectsize);

      name_length = bfd_h_get_32 (abfd, note);
      data_length = bfd_h_get_32 (abfd, note + 4);
      note_type   = bfd_h_get_32 (abfd, note + 8);

      if (name_length == 4 && data_length == 16 && note_type == 1
          && strcmp (note + 12, "GNU") == 0)
	{
	  int os_number = bfd_h_get_32 (abfd, note + 16);

	  /* The case numbers are from abi-tags in glibc */
	  switch (os_number)
	    {
	    case 0 :
	      *os_ident_ptr = ELFOSABI_LINUX;
	      break;
	    case 1 :
	      *os_ident_ptr = ELFOSABI_HURD;
	      break;
	    case 2 :
	      *os_ident_ptr = ELFOSABI_SOLARIS;
	      break;
	    default :
	      internal_error (
		"process_note_abi_sections: unknown OS number %d", os_number);
	      break;
	    }
	}
    }
}

/* Return one of the ELFOSABI_ constants for BFDs representing ELF
   executables.  If it's not an ELF executable or if the OS/ABI couldn't
   be determined, simply return -1. */

static int
get_elfosabi (bfd *abfd)
{
  int elfosabi = -1;

  if (abfd != NULL && bfd_get_flavour (abfd) == bfd_target_elf_flavour)
    {
      elfosabi = elf_elfheader (abfd)->e_ident[EI_OSABI];

      /* When elfosabi is 0 (ELFOSABI_NONE), this is supposed to indicate
         that we're on a SYSV system.  However, GNU/Linux uses a note section
	 to record OS/ABI info, but leaves e_ident[EI_OSABI] zero.  So we
	 have to check the note sections too. */
      if (elfosabi == 0)
	{
	  bfd_map_over_sections (abfd,
	                         process_note_abi_tag_sections,
				 &elfosabi);
	}
    }

  return elfosabi;
}



/* Initialize the current architecture based on INFO.  If possible, re-use an
   architecture from ARCHES, which is a list of architectures already created
   during this debugging session.

   Called e.g. at program startup, when reading a core file, and when reading
   a binary file. */

static struct gdbarch *
rs6000_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;
  int wordsize, from_xcoff_exec, from_elf_exec, power, i, off;
  struct reg *regs;
  const struct variant *v;
  enum bfd_architecture arch;
  unsigned long mach;
  bfd abfd;
  int osabi, sysv_abi;

  from_xcoff_exec = info.abfd && info.abfd->format == bfd_object &&
    bfd_get_flavour (info.abfd) == bfd_target_xcoff_flavour;

  from_elf_exec = info.abfd && info.abfd->format == bfd_object &&
    bfd_get_flavour (info.abfd) == bfd_target_elf_flavour;

  sysv_abi = info.abfd && bfd_get_flavour (info.abfd) == bfd_target_elf_flavour;

  osabi = get_elfosabi (info.abfd);

  /* Check word size.  If INFO is from a binary file, infer it from that,
     else use the previously-inferred size. */
  if (from_xcoff_exec)
    {
      if (xcoff_data (info.abfd)->xcoff64)
	wordsize = 8;
      else
	wordsize = 4;
    }
  else if (from_elf_exec)
    {
      if (elf_elfheader (info.abfd)->e_ident[EI_CLASS] == ELFCLASS64)
	wordsize = 8;
      else
	wordsize = 4;
    }
  else
    {
      tdep = TDEP;
      if (tdep)
	wordsize = tdep->wordsize;
      else
	wordsize = 4;
    }

  /* Find a candidate among extant architectures. */
  for (arches = gdbarch_list_lookup_by_info (arches, &info);
       arches != NULL;
       arches = gdbarch_list_lookup_by_info (arches->next, &info))
    {
      /* Word size in the various PowerPC bfd_arch_info structs isn't
         meaningful, because 64-bit CPUs can run in 32-bit mode.  So, perform
         separate word size check. */
      tdep = gdbarch_tdep (arches->gdbarch);
      if (tdep && tdep->wordsize == wordsize && tdep->osabi == osabi)
	return arches->gdbarch;
    }

  /* None found, create a new architecture from INFO, whose bfd_arch_info
     validity depends on the source:
       - executable		useless
       - rs6000_host_arch()	good
       - core file		good
       - "set arch"		trust blindly
       - GDB startup		useless but harmless */

  if (!from_xcoff_exec)
    {
      arch = info.bfd_architecture;
      mach = info.bfd_arch_info->mach;
    }
  else
    {
      arch = bfd_arch_powerpc;
      mach = 0;
      bfd_default_set_arch_mach (&abfd, arch, mach);
      info.bfd_arch_info = bfd_get_arch_info (&abfd);
    }
  tdep = xmalloc (sizeof (struct gdbarch_tdep));
  tdep->wordsize = wordsize;
  tdep->osabi = osabi;
  gdbarch = gdbarch_alloc (&info, tdep);
  power = arch == bfd_arch_rs6000;

  /* Select instruction printer. */
  tm_print_insn = arch == power ? print_insn_rs6000 :
    info.byte_order == BIG_ENDIAN ? print_insn_big_powerpc :
      print_insn_little_powerpc;

  /* Choose variant. */
  v = find_variant_by_arch (arch, mach);
  if (!v)
    v = find_variant_by_name (power ? "power" : "powerpc");
  tdep->regs = v->regs;

  /* Calculate byte offsets in raw register array. */
  tdep->regoff = xmalloc (v->nregs * sizeof (int));
  for (i = off = 0; i < v->nregs; i++)
    {
      tdep->regoff[i] = off;
      off += regsize (v->regs + i, wordsize);
    }

  set_gdbarch_read_pc (gdbarch, generic_target_read_pc);
  set_gdbarch_write_pc (gdbarch, generic_target_write_pc);
  set_gdbarch_read_fp (gdbarch, generic_target_read_fp);
  set_gdbarch_write_fp (gdbarch, generic_target_write_fp);
  set_gdbarch_read_sp (gdbarch, generic_target_read_sp);
  set_gdbarch_write_sp (gdbarch, generic_target_write_sp);

  set_gdbarch_num_regs (gdbarch, v->nregs);
  set_gdbarch_sp_regnum (gdbarch, 1);
  set_gdbarch_fp_regnum (gdbarch, 1);
  set_gdbarch_pc_regnum (gdbarch, 64);
  set_gdbarch_register_name (gdbarch, rs6000_register_name);
  set_gdbarch_register_size (gdbarch, wordsize);
  set_gdbarch_register_bytes (gdbarch, off);
  set_gdbarch_register_byte (gdbarch, rs6000_register_byte);
  set_gdbarch_register_raw_size (gdbarch, rs6000_register_raw_size);
  set_gdbarch_max_register_raw_size (gdbarch, 8);
  set_gdbarch_register_virtual_size (gdbarch, rs6000_register_virtual_size);
  set_gdbarch_max_register_virtual_size (gdbarch, 8);
  set_gdbarch_register_virtual_type (gdbarch, rs6000_register_virtual_type);

  set_gdbarch_ptr_bit (gdbarch, wordsize * TARGET_CHAR_BIT);
  set_gdbarch_short_bit (gdbarch, 2 * TARGET_CHAR_BIT);
  set_gdbarch_int_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_long_bit (gdbarch, wordsize * TARGET_CHAR_BIT);
  set_gdbarch_long_long_bit (gdbarch, 8 * TARGET_CHAR_BIT);
  set_gdbarch_float_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_double_bit (gdbarch, 8 * TARGET_CHAR_BIT);
  set_gdbarch_long_double_bit (gdbarch, 8 * TARGET_CHAR_BIT);

  set_gdbarch_use_generic_dummy_frames (gdbarch, 1);
  set_gdbarch_call_dummy_length (gdbarch, 0);
  set_gdbarch_call_dummy_location (gdbarch, AT_ENTRY_POINT);
  set_gdbarch_call_dummy_address (gdbarch, entry_point_address);
  set_gdbarch_call_dummy_breakpoint_offset_p (gdbarch, 1);
  set_gdbarch_call_dummy_breakpoint_offset (gdbarch, 0);
  set_gdbarch_call_dummy_start_offset (gdbarch, 0);
  set_gdbarch_pc_in_call_dummy (gdbarch, generic_pc_in_call_dummy);
  set_gdbarch_call_dummy_p (gdbarch, 1);
  set_gdbarch_call_dummy_stack_adjust_p (gdbarch, 0);
  set_gdbarch_get_saved_register (gdbarch, generic_get_saved_register);
  set_gdbarch_fix_call_dummy (gdbarch, rs6000_fix_call_dummy);
  set_gdbarch_push_dummy_frame (gdbarch, generic_push_dummy_frame);
  set_gdbarch_save_dummy_frame_tos (gdbarch, generic_save_dummy_frame_tos);
  set_gdbarch_push_return_address (gdbarch, ppc_push_return_address);
  set_gdbarch_believe_pcc_promotion (gdbarch, 1);
  set_gdbarch_coerce_float_to_double (gdbarch, rs6000_coerce_float_to_double);

  set_gdbarch_register_convertible (gdbarch, rs6000_register_convertible);
  set_gdbarch_register_convert_to_virtual (gdbarch, rs6000_register_convert_to_virtual);
  set_gdbarch_register_convert_to_raw (gdbarch, rs6000_register_convert_to_raw);

  set_gdbarch_extract_return_value (gdbarch, rs6000_extract_return_value);
  
  if (sysv_abi)
    set_gdbarch_push_arguments (gdbarch, ppc_sysv_abi_push_arguments);
  else
    set_gdbarch_push_arguments (gdbarch, rs6000_push_arguments);

  set_gdbarch_store_struct_return (gdbarch, rs6000_store_struct_return);
  set_gdbarch_store_return_value (gdbarch, rs6000_store_return_value);
  set_gdbarch_extract_struct_value_address (gdbarch, rs6000_extract_struct_value_address);
  set_gdbarch_use_struct_convention (gdbarch, generic_use_struct_convention);

  set_gdbarch_pop_frame (gdbarch, rs6000_pop_frame);

  set_gdbarch_skip_prologue (gdbarch, rs6000_skip_prologue);
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_decr_pc_after_break (gdbarch, 0);
  set_gdbarch_function_start_offset (gdbarch, 0);
  set_gdbarch_breakpoint_from_pc (gdbarch, rs6000_breakpoint_from_pc);

  /* Not sure on this. FIXMEmgo */
  set_gdbarch_frame_args_skip (gdbarch, 8);

  set_gdbarch_frame_chain_valid (gdbarch, file_frame_chain_valid);
  if (osabi == ELFOSABI_LINUX)
    {
      set_gdbarch_frameless_function_invocation (gdbarch,
	ppc_linux_frameless_function_invocation);
      set_gdbarch_frame_chain (gdbarch, ppc_linux_frame_chain);
      set_gdbarch_frame_saved_pc (gdbarch, ppc_linux_frame_saved_pc);

      set_gdbarch_frame_init_saved_regs (gdbarch,
	                                 ppc_linux_frame_init_saved_regs);
      set_gdbarch_init_extra_frame_info (gdbarch,
	                                 ppc_linux_init_extra_frame_info);

      set_gdbarch_memory_remove_breakpoint (gdbarch,
	                                    ppc_linux_memory_remove_breakpoint);
    }
  else
    {
      set_gdbarch_frameless_function_invocation (gdbarch,
	rs6000_frameless_function_invocation);
      set_gdbarch_frame_chain (gdbarch, rs6000_frame_chain);
      set_gdbarch_frame_saved_pc (gdbarch, rs6000_frame_saved_pc);

      set_gdbarch_frame_init_saved_regs (gdbarch, rs6000_frame_init_saved_regs);
      set_gdbarch_init_extra_frame_info (gdbarch, rs6000_init_extra_frame_info);
    }
  set_gdbarch_frame_args_address (gdbarch, rs6000_frame_args_address);
  set_gdbarch_frame_locals_address (gdbarch, rs6000_frame_args_address);
  set_gdbarch_saved_pc_after_call (gdbarch, rs6000_saved_pc_after_call);

  /* We can't tell how many args there are
     now that the C compiler delays popping them.  */
  set_gdbarch_frame_num_args (gdbarch, frame_num_args_unknown);

  return gdbarch;
}

/* Initialization code.  */

void
_initialize_rs6000_tdep (void)
{
  register_gdbarch_init (bfd_arch_rs6000, rs6000_gdbarch_init);
  register_gdbarch_init (bfd_arch_powerpc, rs6000_gdbarch_init);
}
