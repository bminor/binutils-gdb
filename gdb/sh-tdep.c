/* Target-dependent code for Hitachi Super-H, for GDB.
   Copyright 1993, 1994, 1995, 1996, 1997, 1998, 2000 Free Software
   Foundation, Inc.

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

/*
   Contributed by Steve Chamberlain
   sac@cygnus.com
 */

#include "defs.h"
#include "frame.h"
#include "obstack.h"
#include "symtab.h"
#include "symfile.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "value.h"
#include "dis-asm.h"
#include "inferior.h"		/* for BEFORE_TEXT_END etc. */
#include "gdb_string.h"
#include "arch-utils.h"

/* A set of original names, to be used when restoring back to generic
   registers from a specific set.  */
/* *INDENT-OFF* */
static char *sh_generic_reg_names[] = {
  "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
  "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
  "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
  "fpul", "fpscr",
  "fr0",  "fr1",  "fr2",  "fr3",  "fr4",  "fr5",  "fr6",  "fr7",
  "fr8",  "fr9",  "fr10", "fr11", "fr12", "fr13", "fr14", "fr15",
  "ssr",  "spc",
  "r0b0", "r1b0", "r2b0", "r3b0", "r4b0", "r5b0", "r6b0", "r7b0",
  "r0b1", "r1b1", "r2b1", "r3b1", "r4b1", "r5b1", "r6b1", "r7b1",
};

static char *sh_reg_names[] = {
  "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
  "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
  "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
  "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
  "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
};

static char *sh3_reg_names[] = {
  "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
  "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
  "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
  "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
  "ssr",  "spc",
  "r0b0", "r1b0", "r2b0", "r3b0", "r4b0", "r5b0", "r6b0", "r7b0",
  "r0b1", "r1b1", "r2b1", "r3b1", "r4b1", "r5b1", "r6b1", "r7b1"
};

static char *sh3e_reg_names[] = {
  "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
  "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
  "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
  "fpul", "fpscr",
  "fr0",  "fr1",  "fr2",  "fr3",  "fr4",  "fr5",  "fr6",  "fr7",
  "fr8",  "fr9",  "fr10", "fr11", "fr12", "fr13", "fr14", "fr15",
  "ssr",  "spc",
  "r0b0", "r1b0", "r2b0", "r3b0", "r4b0", "r5b0", "r6b0", "r7b0",
  "r0b1", "r1b1", "r2b1", "r3b1", "r4b1", "r5b1", "r6b1", "r7b1",
};

static char *sh_dsp_reg_names[] = {
  "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
  "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
  "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
  "",     "dsr",
  "a0g",  "a0",   "a1g",  "a1",   "m0",   "m1",   "x0",   "x1",
  "y0",   "y1",   "",     "",     "",     "",     "",     "mod",
  "",     "",
  "rs",   "re",   "",     "",     "",     "",     "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
};

static char *sh3_dsp_reg_names[] = {
  "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
  "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
  "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
  "",     "dsr",
  "a0g",  "a0",   "a1g",  "a1",   "m0",   "m1",   "x0",   "x1",
  "y0",   "y1",   "",     "",     "",     "",     "",     "mod",
  "ssr",  "spc",
  "rs",   "re",   "",     "",     "",     "",     "",     "",
  "r0b",  "r1b",  "r2b",  "r3b",  "r4b",  "r5b",  "r6b",  "r7b",
};
/* *INDENT-ON* */

#ifdef _WIN32_WCE
char **sh_register_names = sh3_reg_names;
#else
char **sh_register_names = sh_generic_reg_names;
#endif

struct
  {
    char **regnames;
    int mach;
  }
sh_processor_type_table[] =
{
  {
    sh_reg_names, bfd_mach_sh
  }
  ,
  {
    sh_reg_names, bfd_mach_sh2
  }
  ,
  {
    sh_dsp_reg_names, bfd_mach_sh_dsp
  }
  ,
  {
    sh3_reg_names, bfd_mach_sh3
  }
  ,
  {
    sh3_dsp_reg_names, bfd_mach_sh3_dsp
  }
  ,
  {
    sh3e_reg_names, bfd_mach_sh3e
  }
  ,
  {
    NULL, 0
  }
};

/* Prologue looks like
   [mov.l       <regs>,@-r15]...
   [sts.l       pr,@-r15]
   [mov.l       r14,@-r15]
   [mov         r15,r14]

   Actually it can be more complicated than this.  For instance, with
   newer gcc's:

   mov.l   r14,@-r15
   add     #-12,r15
   mov     r15,r14
   mov     r4,r1
   mov     r5,r2
   mov.l   r6,@(4,r14)
   mov.l   r7,@(8,r14)
   mov.b   r1,@r14
   mov     r14,r1
   mov     r14,r1
   add     #2,r1
   mov.w   r2,@r1

 */

/* STS.L PR,@-r15  0100111100100010
   r15-4-->r15, PR-->(r15) */
#define IS_STS(x)  		((x) == 0x4f22)

/* MOV.L Rm,@-r15  00101111mmmm0110
   r15-4-->r15, Rm-->(R15) */
#define IS_PUSH(x) 		(((x) & 0xff0f) == 0x2f06)

#define GET_PUSHED_REG(x)  	(((x) >> 4) & 0xf)

/* MOV r15,r14     0110111011110011
   r15-->r14  */
#define IS_MOV_SP_FP(x)  	((x) == 0x6ef3)

/* ADD #imm,r15    01111111iiiiiiii
   r15+imm-->r15 */
#define IS_ADD_SP(x) 		(((x) & 0xff00) == 0x7f00)

#define IS_MOV_R3(x) 		(((x) & 0xff00) == 0x1a00)
#define IS_SHLL_R3(x)		((x) == 0x4300)

/* ADD r3,r15      0011111100111100
   r15+r3-->r15 */
#define IS_ADD_R3SP(x)		((x) == 0x3f3c)

/* FMOV.S FRm,@-Rn  Rn-4-->Rn, FRm-->(Rn)     1111nnnnmmmm1011
   or
   FMOV DRm,@-Rn    Rn-8-->Rn, DRm-->(Rn)     1111nnnnmmm01011
   or
   FMOV XDm,@-Rn    Rn-8-->Rn, XDm-->(Rn)     1111nnnnmmm11011 */
#define IS_FMOV(x)		(((x) & 0xf00f) == 0xf00b)

/* MOV Rm,Rn            Rm-->Rn          0110nnnnmmmm0011 
   or
   MOV.L Rm,@(disp,Rn)  Rm-->(dispx4+Rn) 0001nnnnmmmmdddd
   or
   MOV.L Rm,@Rn         Rm-->(Rn)        0010nnnnmmmm0010
   where Rm is one of r4,r5,r6,r7 which are the argument registers. */
#define IS_ARG_MOV(x) \
(((((x) & 0xf00f) == 0x6003) && (((x) & 0x00f0) >= 0x0040 && ((x) & 0x00f0) <= 0x0070)) \
|| ((((x) & 0xf000) == 0x1000) && (((x) & 0x00f0) >= 0x0040 && ((x) & 0x00f0) <= 0x0070)) \
|| ((((x) & 0xf00f) == 0x2002) && (((x) & 0x00f0) >= 0x0040 && ((x) & 0x00f0) <= 0x0070)))

/* MOV.L Rm,@(disp,r14)  00011110mmmmdddd
   Rm-->(dispx4+r14) where Rm is one of r4,r5,r6,r7 */
#define IS_MOV_R14(x) \
((((x) & 0xff00) == 0x1e) && (((x) & 0x00f0) >= 0x0040 && ((x) & 0x00f0) <= 0x0070))
                        
#define FPSCR_SZ		(1 << 20)

/* Should call_function allocate stack space for a struct return?  */
int
sh_use_struct_convention (gcc_p, type)
     int gcc_p;
     struct type *type;
{
  return (TYPE_LENGTH (type) > 1);
}

/* Skip any prologue before the guts of a function */

/* Skip the prologue using the debug information. If this fails we'll
   fall back on the 'guess' method below. */
static CORE_ADDR
after_prologue (pc)
     CORE_ADDR pc;
{
  struct symtab_and_line sal;
  CORE_ADDR func_addr, func_end;

  /* If we can not find the symbol in the partial symbol table, then
     there is no hope we can determine the function's start address
     with this code.  */
  if (!find_pc_partial_function (pc, NULL, &func_addr, &func_end))
    return 0;

  /* Get the line associated with FUNC_ADDR.  */
  sal = find_pc_line (func_addr, 0);

  /* There are only two cases to consider.  First, the end of the source line
     is within the function bounds.  In that case we return the end of the
     source line.  Second is the end of the source line extends beyond the
     bounds of the current function.  We need to use the slow code to
     examine instructions in that case.  */
  if (sal.end < func_end)
    return sal.end;
  else
    return 0;
}

/* Here we look at each instruction in the function, and try to guess
   where the prologue ends. Unfortunately this is not always 
   accurate. */
static CORE_ADDR
skip_prologue_hard_way (start_pc)
     CORE_ADDR start_pc;
{
  CORE_ADDR here, end;
  int updated_fp = 0;

  if (!start_pc)
    return 0;

  for (here = start_pc, end = start_pc + (2 * 28); here < end;)
    {
      int w = read_memory_integer (here, 2);
      here += 2;
      if (IS_FMOV (w) || IS_PUSH (w) || IS_STS (w) || IS_MOV_R3 (w)
	  || IS_ADD_R3SP (w) || IS_ADD_SP (w) || IS_SHLL_R3 (w) 
	  || IS_ARG_MOV (w) || IS_MOV_R14 (w))
	{
	  start_pc = here;
	}
      else if (IS_MOV_SP_FP (w))
	{
	  start_pc = here;
	  updated_fp = 1;
	}
      else
	/* Don't bail out yet, if we are before the copy of sp. */
	if (updated_fp)
	  break;
    }

  return start_pc;
}

CORE_ADDR
sh_skip_prologue (pc)
     CORE_ADDR pc;
{
  CORE_ADDR post_prologue_pc;

  /* See if we can determine the end of the prologue via the symbol table.
     If so, then return either PC, or the PC after the prologue, whichever
     is greater.  */

  post_prologue_pc = after_prologue (pc);

  /* If after_prologue returned a useful address, then use it.  Else
     fall back on the instruction skipping code. */
  if (post_prologue_pc != 0)
    return max (pc, post_prologue_pc);
  else
    return (skip_prologue_hard_way (pc));
}

/* Disassemble an instruction.  */

int
gdb_print_insn_sh (memaddr, info)
     bfd_vma memaddr;
     disassemble_info *info;
{
  if (TARGET_BYTE_ORDER == BIG_ENDIAN)
    return print_insn_sh (memaddr, info);
  else
    return print_insn_shl (memaddr, info);
}

/* Given a GDB frame, determine the address of the calling function's frame.
   This will be used to create a new GDB frame struct, and then
   INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame.

   For us, the frame address is its stack pointer value, so we look up
   the function prologue to determine the caller's sp value, and return it.  */

CORE_ADDR
sh_frame_chain (frame)
     struct frame_info *frame;
{
  if (PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
    return frame->frame;	/* dummy frame same as caller's frame */
  if (frame->pc && !inside_entry_file (frame->pc))
    return read_memory_integer (FRAME_FP (frame) + frame->f_offset, 4);
  else
    return 0;
}

/* Find REGNUM on the stack.  Otherwise, it's in an active register.  One thing
   we might want to do here is to check REGNUM against the clobber mask, and
   somehow flag it as invalid if it isn't saved on the stack somewhere.  This
   would provide a graceful failure mode when trying to get the value of
   caller-saves registers for an inner frame.  */

CORE_ADDR
sh_find_callers_reg (fi, regnum)
     struct frame_info *fi;
     int regnum;
{
  struct frame_saved_regs fsr;

  for (; fi; fi = fi->next)
    if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
      /* When the caller requests PR from the dummy frame, we return PC because
         that's where the previous routine appears to have done a call from. */
      return generic_read_register_dummy (fi->pc, fi->frame, regnum);
    else
      {
	FRAME_FIND_SAVED_REGS (fi, fsr);
	if (!fi->pc)
	  return 0;
	if (fsr.regs[regnum] != 0)
	  return read_memory_integer (fsr.regs[regnum],
				      REGISTER_RAW_SIZE (regnum));
      }
  return read_register (regnum);
}

/* Put here the code to store, into a struct frame_saved_regs, the
   addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special: the address we
   return for it IS the sp for the next frame. */

void
sh_frame_find_saved_regs (fi, fsr)
     struct frame_info *fi;
     struct frame_saved_regs *fsr;
{
  int where[NUM_REGS];
  int rn;
  int have_fp = 0;
  int depth;
  int pc;
  int opc;
  int insn;
  int r3_val = 0;
  char *dummy_regs = generic_find_dummy_frame (fi->pc, fi->frame);

  if (dummy_regs)
    {
      /* DANGER!  This is ONLY going to work if the char buffer format of
         the saved registers is byte-for-byte identical to the 
         CORE_ADDR regs[NUM_REGS] format used by struct frame_saved_regs! */
      memcpy (&fsr->regs, dummy_regs, sizeof (fsr));
      return;
    }

  fi->leaf_function = 1;
  fi->f_offset = 0;

  for (rn = 0; rn < NUM_REGS; rn++)
    where[rn] = -1;

  depth = 0;

  /* Loop around examining the prologue insns until we find something
     that does not appear to be part of the prologue.  But give up
     after 20 of them, since we're getting silly then. */

  pc = get_pc_function_start (fi->pc);
  if (!pc)
    {
      fi->pc = 0;
      return;
    }

  for (opc = pc + (2 * 28); pc < opc; pc += 2)
    {
      insn = read_memory_integer (pc, 2);
      /* See where the registers will be saved to */
      if (IS_PUSH (insn))
	{
	  rn = GET_PUSHED_REG (insn);
	  where[rn] = depth;
	  depth += 4;
	}
      else if (IS_STS (insn))
	{
	  where[PR_REGNUM] = depth;
	  /* If we're storing the pr then this isn't a leaf */
	  fi->leaf_function = 0;
	  depth += 4;
	}
      else if (IS_MOV_R3 (insn))
	{
	  r3_val = ((insn & 0xff) ^ 0x80) - 0x80;
	}
      else if (IS_SHLL_R3 (insn))
	{
	  r3_val <<= 1;
	}
      else if (IS_ADD_R3SP (insn))
	{
	  depth += -r3_val;
	}
      else if (IS_ADD_SP (insn))
	{
	  depth -= ((insn & 0xff) ^ 0x80) - 0x80;
	}
      else if (IS_FMOV (insn))
	{
	  if (read_register (FPSCR_REGNUM) & FPSCR_SZ)
	    {
	      depth += 8;
	    }
	  else
	    {
	      depth += 4;
	    }
	}
      else if (IS_MOV_SP_FP (insn))
	break;
#if 0 /* This used to just stop when it found an instruction that
	 was not considered part of the prologue.  Now, we just
	 keep going looking for likely instructions. */
      else
	break;
#endif
    }

  /* Now we know how deep things are, we can work out their addresses */

  for (rn = 0; rn < NUM_REGS; rn++)
    {
      if (where[rn] >= 0)
	{
	  if (rn == FP_REGNUM)
	    have_fp = 1;

	  fsr->regs[rn] = fi->frame - where[rn] + depth - 4;
	}
      else
	{
	  fsr->regs[rn] = 0;
	}
    }

  if (have_fp)
    {
      fsr->regs[SP_REGNUM] = read_memory_integer (fsr->regs[FP_REGNUM], 4);
    }
  else
    {
      fsr->regs[SP_REGNUM] = fi->frame - 4;
    }

  fi->f_offset = depth - where[FP_REGNUM] - 4;
  /* Work out the return pc - either from the saved pr or the pr
     value */
}

/* initialize the extra info saved in a FRAME */

void
sh_init_extra_frame_info (fromleaf, fi)
     int fromleaf;
     struct frame_info *fi;
{
  struct frame_saved_regs fsr;

  if (fi->next)
    fi->pc = FRAME_SAVED_PC (fi->next);

  if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
    {
      /* We need to setup fi->frame here because run_stack_dummy gets it wrong
         by assuming it's always FP.  */
      fi->frame = generic_read_register_dummy (fi->pc, fi->frame,
					       SP_REGNUM);
      fi->return_pc = generic_read_register_dummy (fi->pc, fi->frame,
						   PC_REGNUM);
      fi->f_offset = -(CALL_DUMMY_LENGTH + 4);
      fi->leaf_function = 0;
      return;
    }
  else
    {
      FRAME_FIND_SAVED_REGS (fi, fsr);
      fi->return_pc = sh_find_callers_reg (fi, PR_REGNUM);
    }
}

/* Discard from the stack the innermost frame,
   restoring all saved registers.  */

void
sh_pop_frame ()
{
  register struct frame_info *frame = get_current_frame ();
  register CORE_ADDR fp;
  register int regnum;
  struct frame_saved_regs fsr;

  if (PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
    generic_pop_dummy_frame ();
  else
    {
      fp = FRAME_FP (frame);
      get_frame_saved_regs (frame, &fsr);

      /* Copy regs from where they were saved in the frame */
      for (regnum = 0; regnum < NUM_REGS; regnum++)
	if (fsr.regs[regnum])
	  write_register (regnum, read_memory_integer (fsr.regs[regnum], 4));

      write_register (PC_REGNUM, frame->return_pc);
      write_register (SP_REGNUM, fp + 4);
    }
  flush_cached_frames ();
}

/* Function: push_arguments
   Setup the function arguments for calling a function in the inferior.

   On the Hitachi SH architecture, there are four registers (R4 to R7)
   which are dedicated for passing function arguments.  Up to the first
   four arguments (depending on size) may go into these registers.
   The rest go on the stack.

   Arguments that are smaller than 4 bytes will still take up a whole
   register or a whole 32-bit word on the stack, and will be 
   right-justified in the register or the stack word.  This includes
   chars, shorts, and small aggregate types.

   Arguments that are larger than 4 bytes may be split between two or 
   more registers.  If there are not enough registers free, an argument
   may be passed partly in a register (or registers), and partly on the
   stack.  This includes doubles, long longs, and larger aggregates. 
   As far as I know, there is no upper limit to the size of aggregates 
   that will be passed in this way; in other words, the convention of 
   passing a pointer to a large aggregate instead of a copy is not used.

   An exceptional case exists for struct arguments (and possibly other
   aggregates such as arrays) if the size is larger than 4 bytes but 
   not a multiple of 4 bytes.  In this case the argument is never split 
   between the registers and the stack, but instead is copied in its
   entirety onto the stack, AND also copied into as many registers as 
   there is room for.  In other words, space in registers permitting, 
   two copies of the same argument are passed in.  As far as I can tell,
   only the one on the stack is used, although that may be a function 
   of the level of compiler optimization.  I suspect this is a compiler
   bug.  Arguments of these odd sizes are left-justified within the 
   word (as opposed to arguments smaller than 4 bytes, which are 
   right-justified).


   If the function is to return an aggregate type such as a struct, it 
   is either returned in the normal return value register R0 (if its 
   size is no greater than one byte), or else the caller must allocate
   space into which the callee will copy the return value (if the size
   is greater than one byte).  In this case, a pointer to the return 
   value location is passed into the callee in register R2, which does 
   not displace any of the other arguments passed in via registers R4
   to R7.   */

CORE_ADDR
sh_push_arguments (nargs, args, sp, struct_return, struct_addr)
     int nargs;
     value_ptr *args;
     CORE_ADDR sp;
     unsigned char struct_return;
     CORE_ADDR struct_addr;
{
  int stack_offset, stack_alloc;
  int argreg;
  int argnum;
  struct type *type;
  CORE_ADDR regval;
  char *val;
  char valbuf[4];
  int len;
  int odd_sized_struct;

  /* first force sp to a 4-byte alignment */
  sp = sp & ~3;

  /* The "struct return pointer" pseudo-argument has its own dedicated 
     register */
  if (struct_return)
    write_register (STRUCT_RETURN_REGNUM, struct_addr);

  /* Now make sure there's space on the stack */
  for (argnum = 0, stack_alloc = 0;
       argnum < nargs; argnum++)
    stack_alloc += ((TYPE_LENGTH (VALUE_TYPE (args[argnum])) + 3) & ~3);
  sp -= stack_alloc;		/* make room on stack for args */


  /* Now load as many as possible of the first arguments into
     registers, and push the rest onto the stack.  There are 16 bytes
     in four registers available.  Loop thru args from first to last.  */

  argreg = ARG0_REGNUM;
  for (argnum = 0, stack_offset = 0; argnum < nargs; argnum++)
    {
      type = VALUE_TYPE (args[argnum]);
      len = TYPE_LENGTH (type);
      memset (valbuf, 0, sizeof (valbuf));
      if (len < 4)
	{			/* value gets right-justified in the register or stack word */
	  memcpy (valbuf + (4 - len),
		  (char *) VALUE_CONTENTS (args[argnum]), len);
	  val = valbuf;
	}
      else
	val = (char *) VALUE_CONTENTS (args[argnum]);

      if (len > 4 && (len & 3) != 0)
	odd_sized_struct = 1;	/* such structs go entirely on stack */
      else
	odd_sized_struct = 0;
      while (len > 0)
	{
	  if (argreg > ARGLAST_REGNUM || odd_sized_struct)
	    {			/* must go on the stack */
	      write_memory (sp + stack_offset, val, 4);
	      stack_offset += 4;
	    }
	  /* NOTE WELL!!!!!  This is not an "else if" clause!!!
	     That's because some *&^%$ things get passed on the stack
	     AND in the registers!   */
	  if (argreg <= ARGLAST_REGNUM)
	    {			/* there's room in a register */
	      regval = extract_address (val, REGISTER_RAW_SIZE (argreg));
	      write_register (argreg++, regval);
	    }
	  /* Store the value 4 bytes at a time.  This means that things
	     larger than 4 bytes may go partly in registers and partly
	     on the stack.  */
	  len -= REGISTER_RAW_SIZE (argreg);
	  val += REGISTER_RAW_SIZE (argreg);
	}
    }
  return sp;
}

/* Function: push_return_address (pc)
   Set up the return address for the inferior function call.
   Needed for targets where we don't actually execute a JSR/BSR instruction */

CORE_ADDR
sh_push_return_address (pc, sp)
     CORE_ADDR pc;
     CORE_ADDR sp;
{
  write_register (PR_REGNUM, CALL_DUMMY_ADDRESS ());
  return sp;
}

/* Function: fix_call_dummy
   Poke the callee function's address into the destination part of 
   the CALL_DUMMY.  The address is actually stored in a data word 
   following the actualy CALL_DUMMY instructions, which will load
   it into a register using PC-relative addressing.  This function
   expects the CALL_DUMMY to look like this:

   mov.w @(2,PC), R8
   jsr   @R8
   nop
   trap
   <destination>
 */

#if 0
void
sh_fix_call_dummy (dummy, pc, fun, nargs, args, type, gcc_p)
     char *dummy;
     CORE_ADDR pc;
     CORE_ADDR fun;
     int nargs;
     value_ptr *args;
     struct type *type;
     int gcc_p;
{
  *(unsigned long *) (dummy + 8) = fun;
}
#endif


/* Modify the actual processor type. */

int
sh_target_architecture_hook (ap)
     const bfd_arch_info_type *ap;
{
  int i, j;

  if (ap->arch != bfd_arch_sh)
    return 0;

  for (i = 0; sh_processor_type_table[i].regnames != NULL; i++)
    {
      if (sh_processor_type_table[i].mach == ap->mach)
	{
	  sh_register_names = sh_processor_type_table[i].regnames;
	  return 1;
	}
    }

  internal_error ("Architecture `%s' unreconized", ap->printable_name);
}

/* Print the registers in a form similar to the E7000 */

static void
sh_show_regs (args, from_tty)
     char *args;
     int from_tty;
{
  int cpu;
  if (TARGET_ARCHITECTURE->arch == bfd_arch_sh)
    cpu = TARGET_ARCHITECTURE->mach;
  else
    cpu = 0;

  printf_filtered ("PC=%s SR=%08lx PR=%08lx MACH=%08lx MACHL=%08lx\n",
		   paddr (read_register (PC_REGNUM)),
		   (long) read_register (SR_REGNUM),
		   (long) read_register (PR_REGNUM),
		   (long) read_register (MACH_REGNUM),
		   (long) read_register (MACL_REGNUM));

  printf_filtered ("GBR=%08lx VBR=%08lx",
		   (long) read_register (GBR_REGNUM),
		   (long) read_register (VBR_REGNUM));
  if (cpu == bfd_mach_sh3 || cpu == bfd_mach_sh3e || cpu == bfd_mach_sh3_dsp
      || cpu == bfd_mach_sh4)
    {
      printf_filtered (" SSR=%08lx SPC=%08lx",
		       (long) read_register (SSR_REGNUM),
		       (long) read_register (SPC_REGNUM));
      if (cpu == bfd_mach_sh3e || cpu == bfd_mach_sh4)
	{
	  printf_filtered (" FPUL=%08lx FPSCR=%08lx",
			   (long) read_register (FPUL_REGNUM),
			   (long) read_register (FPSCR_REGNUM));
	}
    }
  if (cpu == bfd_mach_sh_dsp || cpu == bfd_mach_sh3_dsp)
    printf_filtered (" DSR=%08lx", (long) read_register (DSR_REGNUM));

  printf_filtered ("\nR0-R7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (0),
		   (long) read_register (1),
		   (long) read_register (2),
		   (long) read_register (3),
		   (long) read_register (4),
		   (long) read_register (5),
		   (long) read_register (6),
		   (long) read_register (7));
  printf_filtered ("R8-R15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (8),
		   (long) read_register (9),
		   (long) read_register (10),
		   (long) read_register (11),
		   (long) read_register (12),
		   (long) read_register (13),
		   (long) read_register (14),
		   (long) read_register (15));
  if (cpu == bfd_mach_sh3e || cpu == bfd_mach_sh4)
    {
      int pr = cpu == bfd_mach_sh4 && (read_register (FPSCR_REGNUM) & 0x80000);

      printf_filtered ((pr
			? "DR0-DR6  %08lx%08lx %08lx%08lx %08lx%08lx %08lx%08lx\n"
			: "FP0-FP7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"),
		       (long) read_register (FP0_REGNUM + 0),
		       (long) read_register (FP0_REGNUM + 1),
		       (long) read_register (FP0_REGNUM + 2),
		       (long) read_register (FP0_REGNUM + 3),
		       (long) read_register (FP0_REGNUM + 4),
		       (long) read_register (FP0_REGNUM + 5),
		       (long) read_register (FP0_REGNUM + 6),
		       (long) read_register (FP0_REGNUM + 7));
      printf_filtered ((pr
			? "DR8-DR14 %08lx%08lx %08lx%08lx %08lx%08lx %08lx%08lx\n"
			: "FP8-FP15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"),
		       (long) read_register (FP0_REGNUM + 8),
		       (long) read_register (FP0_REGNUM + 9),
		       (long) read_register (FP0_REGNUM + 10),
		       (long) read_register (FP0_REGNUM + 11),
		       (long) read_register (FP0_REGNUM + 12),
		       (long) read_register (FP0_REGNUM + 13),
		       (long) read_register (FP0_REGNUM + 14),
		       (long) read_register (FP0_REGNUM + 15));
    }
  /* FIXME: sh4 has more registers */
  if (cpu == bfd_mach_sh_dsp || cpu == bfd_mach_sh3_dsp)
    {
      printf_filtered ("A0G=%02lx A0=%08lx M0=%08lx X0=%08lx Y0=%08lx RS=%08lx MOD=%08lx\n",
		       (long) read_register (A0G_REGNUM) & 0xff,
		       (long) read_register (A0_REGNUM),
		       (long) read_register (M0_REGNUM),
		       (long) read_register (X0_REGNUM),
		       (long) read_register (Y0_REGNUM),
		       (long) read_register (RS_REGNUM),
		       (long) read_register (MOD_REGNUM));
      printf_filtered ("A1G=%02lx A1=%08lx M1=%08lx X1=%08lx Y1=%08lx RE=%08lx\n",
		       (long) read_register (A1G_REGNUM) & 0xff,
		       (long) read_register (A1_REGNUM),
		       (long) read_register (M1_REGNUM),
		       (long) read_register (X1_REGNUM),
		       (long) read_register (Y1_REGNUM),
		       (long) read_register (RE_REGNUM));
    }
}

/* Function: extract_return_value
   Find a function's return value in the appropriate registers (in regbuf),
   and copy it into valbuf.  */

void
sh_extract_return_value (type, regbuf, valbuf)
     struct type *type;
     void *regbuf;
     void *valbuf;
{
  int len = TYPE_LENGTH (type);

  if (len <= 4)
    memcpy (valbuf, ((char *) regbuf) + 4 - len, len);
  else if (len <= 8)
    memcpy (valbuf, ((char *) regbuf) + 8 - len, len);
  else
    error ("bad size for return value");
}

/* If the architecture is sh4 or sh3e, store a function's return value
   in the R0 general register or in the FP0 floating point register,
   depending on the type of the return value. In all the other cases
   the result is stored in r0. */
void
sh_store_return_value (struct type *type, void *valbuf)
{
  int cpu;
  if (TARGET_ARCHITECTURE->arch == bfd_arch_sh)
    cpu = TARGET_ARCHITECTURE->mach;
  else
    cpu = 0;
  if (cpu == bfd_mach_sh3e || cpu == bfd_mach_sh4)
    {
      if (TYPE_CODE (type) == TYPE_CODE_FLT) 
	write_register_bytes (REGISTER_BYTE (FP0_REGNUM), valbuf, TYPE_LENGTH (type));
      else
	write_register_bytes (REGISTER_BYTE (0), valbuf, TYPE_LENGTH (type));
    }
  else
    write_register_bytes (REGISTER_BYTE (0), valbuf, TYPE_LENGTH (type));
}

void
_initialize_sh_tdep ()
{
  struct cmd_list_element *c;

  tm_print_insn = gdb_print_insn_sh;

  target_architecture_hook = sh_target_architecture_hook;

  add_com ("regs", class_vars, sh_show_regs, "Print all registers");
}
