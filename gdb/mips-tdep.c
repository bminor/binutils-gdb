/* Target-dependent code for the MIPS architecture, for GDB, the GNU Debugger.
   Copyright 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996
   Free Software Foundation, Inc.
   Contributed by Alessandro Forin(af@cs.cmu.edu) at CMU
   and by Per Bothner(bothner@cs.wisc.edu) at U.Wisconsin.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "gdb_string.h"
#include "frame.h"
#include "inferior.h"
#include "symtab.h"
#include "value.h"
#include "gdbcmd.h"
#include "language.h"
#include "gdbcore.h"
#include "symfile.h"
#include "objfiles.h"
#include "gdbtypes.h"
#include "target.h"

#include "opcode/mips.h"

#define VM_MIN_ADDRESS (CORE_ADDR)0x400000

/* FIXME: Put this declaration in frame.h.  */
extern struct obstack frame_cache_obstack;

/* FIXME! this code assumes 4-byte instructions.  */
#define MIPS_INSTLEN 4		/* Length of an instruction */
#define MIPS_NUMREGS 32		/* Number of integer or float registers */
typedef unsigned long t_inst;	/* Integer big enough to hold an instruction */

#if 0
static int mips_in_lenient_prologue PARAMS ((CORE_ADDR, CORE_ADDR));
#endif

static int gdb_print_insn_mips PARAMS ((bfd_vma, disassemble_info *));

static void mips_print_register PARAMS ((int, int));

static mips_extra_func_info_t
heuristic_proc_desc PARAMS ((CORE_ADDR, CORE_ADDR, struct frame_info *));

static CORE_ADDR heuristic_proc_start PARAMS ((CORE_ADDR));

static CORE_ADDR read_next_frame_reg PARAMS ((struct frame_info *, int));

static void mips_set_fpu_command PARAMS ((char *, int,
					  struct cmd_list_element *));

static void mips_show_fpu_command PARAMS ((char *, int,
					   struct cmd_list_element *));

void mips_set_processor_type_command PARAMS ((char *, int));

int mips_set_processor_type PARAMS ((char *));

static void mips_show_processor_type_command PARAMS ((char *, int));

static void reinit_frame_cache_sfunc PARAMS ((char *, int,
					      struct cmd_list_element *));

static mips_extra_func_info_t
  find_proc_desc PARAMS ((CORE_ADDR pc, struct frame_info *next_frame));

static CORE_ADDR after_prologue PARAMS ((CORE_ADDR pc,
					 mips_extra_func_info_t proc_desc));

/* This value is the model of MIPS in use.  It is derived from the value
   of the PrID register.  */

char *mips_processor_type;

char *tmp_mips_processor_type;

/* Some MIPS boards don't support floating point, so we permit the
   user to turn it off.  */

enum mips_fpu_type mips_fpu;

static char *mips_fpu_string;

/* A set of original names, to be used when restoring back to generic
   registers from a specific set.  */

char *mips_generic_reg_names[] = REGISTER_NAMES;

/* Names of IDT R3041 registers.  */

char *mips_r3041_reg_names[] = {
	"zero",	"at",	"v0",	"v1",	"a0",	"a1",	"a2",	"a3",
	"t0",	"t1",	"t2",	"t3",	"t4",	"t5",	"t6",	"t7",
	"s0",	"s1",	"s2",	"s3",	"s4",	"s5",	"s6",	"s7",
	"t8",	"t9",	"k0",	"k1",	"gp",	"sp",	"s8",	"ra",
	"sr",	"lo",	"hi",	"bad",	"cause","pc",
	"f0",   "f1",   "f2",   "f3",   "f4",   "f5",   "f6",   "f7",
	"f8",   "f9",   "f10",  "f11",  "f12",  "f13",  "f14",  "f15",
	"f16",  "f17",  "f18",  "f19",  "f20",  "f21",  "f22",  "f23",
	"f24",  "f25",  "f26",  "f27",  "f28",  "f29",  "f30",  "f31",
	"fsr",  "fir",  "fp",	"",
	"",	"",	"bus",	"ccfg",	"",	"",	"",	"",
	"",	"",	"port",	"cmp",	"",	"",	"epc",	"prid",
};

/* Names of IDT R3051 registers.  */

char *mips_r3051_reg_names[] = {
	"zero",	"at",	"v0",	"v1",	"a0",	"a1",	"a2",	"a3",
	"t0",	"t1",	"t2",	"t3",	"t4",	"t5",	"t6",	"t7",
	"s0",	"s1",	"s2",	"s3",	"s4",	"s5",	"s6",	"s7",
	"t8",	"t9",	"k0",	"k1",	"gp",	"sp",	"s8",	"ra",
	"sr",	"lo",	"hi",	"bad",	"cause","pc",
	"f0",   "f1",   "f2",   "f3",   "f4",   "f5",   "f6",   "f7",
	"f8",   "f9",   "f10",  "f11",  "f12",  "f13",  "f14",  "f15",
	"f16",  "f17",  "f18",  "f19",  "f20",  "f21",  "f22",  "f23",
	"f24",  "f25",  "f26",  "f27",  "f28",  "f29",  "f30",  "f31",
	"fsr",  "fir",  "fp",	"",
	"inx",	"rand",	"elo",	"",	"ctxt",	"",	"",	"",
	"",	"",	"ehi",	"",	"",	"",	"epc",	"prid",
};

/* Names of IDT R3081 registers.  */

char *mips_r3081_reg_names[] = {
	"zero",	"at",	"v0",	"v1",	"a0",	"a1",	"a2",	"a3",
	"t0",	"t1",	"t2",	"t3",	"t4",	"t5",	"t6",	"t7",
	"s0",	"s1",	"s2",	"s3",	"s4",	"s5",	"s6",	"s7",
	"t8",	"t9",	"k0",	"k1",	"gp",	"sp",	"s8",	"ra",
	"sr",	"lo",	"hi",	"bad",	"cause","pc",
	"f0",   "f1",   "f2",   "f3",   "f4",   "f5",   "f6",   "f7",
	"f8",   "f9",   "f10",  "f11",  "f12",  "f13",  "f14",  "f15",
	"f16",  "f17",  "f18",  "f19",  "f20",  "f21",  "f22",  "f23",
	"f24",  "f25",  "f26",  "f27",  "f28",  "f29",  "f30",  "f31",
	"fsr",  "fir",  "fp",	"",
	"inx",	"rand",	"elo",	"cfg",	"ctxt",	"",	"",	"",
	"",	"",	"ehi",	"",	"",	"",	"epc",	"prid",
};

/* Names of LSI 33k registers.  */

char *mips_lsi33k_reg_names[] = {
	"zero",	"at",	"v0",	"v1",	"a0",	"a1",	"a2",	"a3",
	"t0",	"t1",	"t2",	"t3",	"t4",	"t5",	"t6",	"t7",
	"s0",	"s1",	"s2",	"s3",	"s4",	"s5",	"s6",	"s7",
	"t8",	"t9",	"k0",	"k1",	"gp",	"sp",	"s8",	"ra",
	"epc",	"hi",	"lo",	"sr",	"cause","badvaddr",
	"dcic", "bpc",  "bda",  "",     "",     "",     "",      "",
	"",     "",     "",     "",     "",     "",     "",      "",
	"",     "",     "",     "",     "",     "",     "",      "",
	"",     "",     "",     "",     "",     "",     "",      "",
	"",     "",     "",	"",
	"",	"",	"",	"",	"",	"",	"",	 "",
	"",	"",	"",	"",	"",	"",	"",	 "",
};

struct {
  char *name;
  char **regnames;
} mips_processor_type_table[] = {
  { "generic", mips_generic_reg_names },
  { "r3041", mips_r3041_reg_names },
  { "r3051", mips_r3051_reg_names },
  { "r3071", mips_r3081_reg_names },
  { "r3081", mips_r3081_reg_names },
  { "lsi33k", mips_lsi33k_reg_names },
  { NULL, NULL }
};

/* Heuristic_proc_start may hunt through the text section for a long
   time across a 2400 baud serial line.  Allows the user to limit this
   search.  */

static unsigned int heuristic_fence_post = 0;

#define PROC_LOW_ADDR(proc) ((proc)->pdr.adr) /* least address */
#define PROC_HIGH_ADDR(proc) ((proc)->high_addr) /* upper address bound */
#define PROC_FRAME_OFFSET(proc) ((proc)->pdr.frameoffset)
#define PROC_FRAME_REG(proc) ((proc)->pdr.framereg)
#define PROC_REG_MASK(proc) ((proc)->pdr.regmask)
#define PROC_FREG_MASK(proc) ((proc)->pdr.fregmask)
#define PROC_REG_OFFSET(proc) ((proc)->pdr.regoffset)
#define PROC_FREG_OFFSET(proc) ((proc)->pdr.fregoffset)
#define PROC_PC_REG(proc) ((proc)->pdr.pcreg)
#define PROC_SYMBOL(proc) (*(struct symbol**)&(proc)->pdr.isym)
#define _PROC_MAGIC_ 0x0F0F0F0F
#define PROC_DESC_IS_DUMMY(proc) ((proc)->pdr.isym == _PROC_MAGIC_)
#define SET_PROC_DESC_IS_DUMMY(proc) ((proc)->pdr.isym = _PROC_MAGIC_)

struct linked_proc_info
{
  struct mips_extra_func_info info;
  struct linked_proc_info *next;
} *linked_proc_desc_table = NULL;


/* This returns the PC of the first inst after the prologue.  If we can't
   find the prologue, then return 0.  */

static CORE_ADDR
after_prologue (pc, proc_desc)
     CORE_ADDR pc;
     mips_extra_func_info_t proc_desc;
{
  struct symtab_and_line sal;
  CORE_ADDR func_addr, func_end;

  if (!proc_desc)
    proc_desc = find_proc_desc (pc, NULL);

  if (proc_desc)
    {
      /* If function is frameless, then we need to do it the hard way.  I
	 strongly suspect that frameless always means prologueless... */
      if (PROC_FRAME_REG (proc_desc) == SP_REGNUM
	  && PROC_FRAME_OFFSET (proc_desc) == 0)
	return 0;
    }

  if (!find_pc_partial_function (pc, NULL, &func_addr, &func_end))
    return 0;			/* Unknown */

  sal = find_pc_line (func_addr, 0);

  if (sal.end < func_end)
    return sal.end;

  /* The line after the prologue is after the end of the function.  In this
     case, tell the caller to find the prologue the hard way.  */

  return 0;
}

/* Guaranteed to set fci->saved_regs to some values (it never leaves it
   NULL).  */

void
mips_find_saved_regs (fci)
     struct frame_info *fci;
{
  int ireg;
  CORE_ADDR reg_position;
  /* r0 bit means kernel trap */
  int kernel_trap;
  /* What registers have been saved?  Bitmasks.  */
  unsigned long gen_mask, float_mask;
  mips_extra_func_info_t proc_desc;

  fci->saved_regs = (struct frame_saved_regs *)
    obstack_alloc (&frame_cache_obstack, sizeof(struct frame_saved_regs));
  memset (fci->saved_regs, 0, sizeof (struct frame_saved_regs));

  /* If it is the frame for sigtramp, the saved registers are located
     in a sigcontext structure somewhere on the stack.
     If the stack layout for sigtramp changes we might have to change these
     constants and the companion fixup_sigtramp in mdebugread.c  */
#ifndef SIGFRAME_BASE
/* To satisfy alignment restrictions, sigcontext is located 4 bytes
   above the sigtramp frame.  */
#define SIGFRAME_BASE		MIPS_REGSIZE
/* FIXME!  Are these correct?? */
#define SIGFRAME_PC_OFF		(SIGFRAME_BASE + 2 * MIPS_REGSIZE)
#define SIGFRAME_REGSAVE_OFF	(SIGFRAME_BASE + 3 * MIPS_REGSIZE)
#define SIGFRAME_FPREGSAVE_OFF	\
        (SIGFRAME_REGSAVE_OFF + MIPS_NUMREGS * MIPS_REGSIZE + 3 * MIPS_REGSIZE)
#endif
#ifndef SIGFRAME_REG_SIZE
/* FIXME!  Is this correct?? */
#define SIGFRAME_REG_SIZE	MIPS_REGSIZE
#endif
  if (fci->signal_handler_caller)
    {
      for (ireg = 0; ireg < MIPS_NUMREGS; ireg++)
	{
 	  reg_position = fci->frame + SIGFRAME_REGSAVE_OFF
			 + ireg * SIGFRAME_REG_SIZE;
 	  fci->saved_regs->regs[ireg] = reg_position;
	}
      for (ireg = 0; ireg < MIPS_NUMREGS; ireg++)
	{
 	  reg_position = fci->frame + SIGFRAME_FPREGSAVE_OFF
			 + ireg * SIGFRAME_REG_SIZE;
 	  fci->saved_regs->regs[FP0_REGNUM + ireg] = reg_position;
	}
      fci->saved_regs->regs[PC_REGNUM] = fci->frame + SIGFRAME_PC_OFF;
      return;
    }

  proc_desc = fci->proc_desc;
  if (proc_desc == NULL)
    /* I'm not sure how/whether this can happen.  Normally when we can't
       find a proc_desc, we "synthesize" one using heuristic_proc_desc
       and set the saved_regs right away.  */
    return;

  kernel_trap = PROC_REG_MASK(proc_desc) & 1;
  gen_mask = kernel_trap ? 0xFFFFFFFF : PROC_REG_MASK(proc_desc);
  float_mask = kernel_trap ? 0xFFFFFFFF : PROC_FREG_MASK(proc_desc);

  if (/* In any frame other than the innermost, we assume that all
	 registers have been saved.  This assumes that all register
	 saves in a function happen before the first function
	 call.  */
      fci->next == NULL

      /* In a dummy frame we know exactly where things are saved.  */
      && !PROC_DESC_IS_DUMMY (proc_desc)

      /* Don't bother unless we are inside a function prologue.  Outside the
	 prologue, we know where everything is. */

      && in_prologue (fci->pc, PROC_LOW_ADDR (proc_desc))

      /* Not sure exactly what kernel_trap means, but if it means
	 the kernel saves the registers without a prologue doing it,
	 we better not examine the prologue to see whether registers
	 have been saved yet.  */
      && !kernel_trap)
    {
      /* We need to figure out whether the registers that the proc_desc
	 claims are saved have been saved yet.  */

      CORE_ADDR addr;
      int status;
      char buf[MIPS_INSTLEN];
      t_inst inst;

      /* Bitmasks; set if we have found a save for the register.  */
      unsigned long gen_save_found = 0;
      unsigned long float_save_found = 0;

      for (addr = PROC_LOW_ADDR (proc_desc);
	   addr < fci->pc /*&& (gen_mask != gen_save_found
			      || float_mask != float_save_found)*/;
	   addr += MIPS_INSTLEN)
	{
	  status = read_memory_nobpt (addr, buf, MIPS_INSTLEN);
	  if (status)
	    memory_error (status, addr);
	  inst = extract_unsigned_integer (buf, MIPS_INSTLEN);
	  if (/* sw reg,n($sp) */
	      (inst & 0xffe00000) == 0xafa00000

	      /* sw reg,n($r30) */
	      || (inst & 0xffe00000) == 0xafc00000

	      /* sd reg,n($sp) */
	      || (inst & 0xffe00000) == 0xffa00000)
	    {
	      /* It might be possible to use the instruction to
		 find the offset, rather than the code below which
		 is based on things being in a certain order in the
		 frame, but figuring out what the instruction's offset
		 is relative to might be a little tricky.  */
	      int reg = (inst & 0x001f0000) >> 16;
	      gen_save_found |= (1 << reg);
	    }
	  else if (/* swc1 freg,n($sp) */
		   (inst & 0xffe00000) == 0xe7a00000

		   /* swc1 freg,n($r30) */
		   || (inst & 0xffe00000) == 0xe7c00000

                   /* sdc1 freg,n($sp) */
                   || (inst & 0xffe00000) == 0xf7a00000)

	    {
	      int reg = ((inst & 0x001f0000) >> 16);
	      float_save_found |= (1 << reg);
	    }
	}
      gen_mask = gen_save_found;
      float_mask = float_save_found;
    }

  /* Fill in the offsets for the registers which gen_mask says
     were saved.  */
  reg_position = fci->frame + PROC_REG_OFFSET (proc_desc);
  for (ireg= MIPS_NUMREGS-1; gen_mask; --ireg, gen_mask <<= 1)
    if (gen_mask & 0x80000000)
      {
	fci->saved_regs->regs[ireg] = reg_position;
	reg_position -= MIPS_REGSIZE;
      }
  /* Fill in the offsets for the registers which float_mask says
     were saved.  */
  reg_position = fci->frame + PROC_FREG_OFFSET (proc_desc);

  /* The freg_offset points to where the first *double* register
     is saved.  So skip to the high-order word. */
  if (! GDB_TARGET_IS_MIPS64)
    reg_position += 4;

  /* FIXME!  this code looks scary... 
   * Looks like it's trying to do stuff with a register, 
   * but .... ???
   */
  for (ireg = MIPS_NUMREGS-1; float_mask; --ireg, float_mask <<= 1)
    if (float_mask & 0x80000000)
      {
	fci->saved_regs->regs[FP0_REGNUM+ireg] = reg_position;
	reg_position -= MIPS_REGSIZE;
      }

  fci->saved_regs->regs[PC_REGNUM] = fci->saved_regs->regs[RA_REGNUM];
}

static CORE_ADDR
read_next_frame_reg(fi, regno)
     struct frame_info *fi;
     int regno;
{
  for (; fi; fi = fi->next)
    {
      /* We have to get the saved sp from the sigcontext
	 if it is a signal handler frame.  */
      if (regno == SP_REGNUM && !fi->signal_handler_caller)
	return fi->frame;
      else
	{
	  if (fi->saved_regs == NULL)
	    mips_find_saved_regs (fi);
	  if (fi->saved_regs->regs[regno])
	    return read_memory_integer(fi->saved_regs->regs[regno], MIPS_REGSIZE);
	}
    }
  return read_register (regno);
}

/* mips_addr_bits_remove - remove useless address bits  */

CORE_ADDR
mips_addr_bits_remove (addr)
    CORE_ADDR addr;
{
#if GDB_TARGET_IS_MIPS64
  if ((addr >> 32 == (CORE_ADDR)0xffffffff)
      && (strcmp(target_shortname,"pmon")==0
	 || strcmp(target_shortname,"ddb")==0
	 || strcmp(target_shortname,"sim")==0))
    {
      /* This hack is a work-around for existing boards using PMON,
	 the simulator, and any other 64-bit targets that doesn't have
	 true 64-bit addressing.  On these targets, the upper 32 bits
	 of addresses are ignored by the hardware.  Thus, the PC or SP
	 are likely to have been sign extended to all 1s by instruction
	 sequences that load 32-bit addresses.  For example, a typical
	 piece of code that loads an address is this:
		lui $r2, <upper 16 bits>
		ori $r2, <lower 16 bits>
	 But the lui sign-extends the value such that the upper 32 bits
	 may be all 1s.  The workaround is simply to mask off these bits.
	 In the future, gcc may be changed to support true 64-bit
	 addressing, and this masking will have to be disabled.  */
        addr &= (CORE_ADDR)0xffffffff;
    }
#endif

  return addr;
}

CORE_ADDR
mips_frame_saved_pc(frame)
     struct frame_info *frame;
{
  CORE_ADDR saved_pc;
  mips_extra_func_info_t proc_desc = frame->proc_desc;
  /* We have to get the saved pc from the sigcontext
     if it is a signal handler frame.  */
  int pcreg = frame->signal_handler_caller ? PC_REGNUM
	      : (proc_desc ? PROC_PC_REG(proc_desc) : RA_REGNUM);

  if (proc_desc && PROC_DESC_IS_DUMMY(proc_desc))
    saved_pc = read_memory_integer(frame->frame - MIPS_REGSIZE, MIPS_REGSIZE);
  else
    saved_pc = read_next_frame_reg(frame, pcreg);

  return ADDR_BITS_REMOVE (saved_pc);
}

static struct mips_extra_func_info temp_proc_desc;
static struct frame_saved_regs temp_saved_regs;

/* This fencepost looks highly suspicious to me.  Removing it also
   seems suspicious as it could affect remote debugging across serial
   lines.  */

static CORE_ADDR
heuristic_proc_start(pc)
    CORE_ADDR pc;
{
    CORE_ADDR start_pc = pc;
    CORE_ADDR fence = start_pc - heuristic_fence_post;

    if (start_pc == 0)	return 0;

    if (heuristic_fence_post == UINT_MAX
	|| fence < VM_MIN_ADDRESS)
      fence = VM_MIN_ADDRESS;

    /* search back for previous return */
    for (start_pc -= MIPS_INSTLEN; ; start_pc -= MIPS_INSTLEN) /* FIXME!! */
	if (start_pc < fence)
	  {
	    /* It's not clear to me why we reach this point when
	       stop_soon_quietly, but with this test, at least we
	       don't print out warnings for every child forked (eg, on
	       decstation).  22apr93 rich@cygnus.com.  */
	    if (!stop_soon_quietly)
	      {
		static int blurb_printed = 0;

		if (fence == VM_MIN_ADDRESS)
		  warning("Hit beginning of text section without finding");
		else
		  warning("Hit heuristic-fence-post without finding");
		
		warning("enclosing function for address 0x%s", paddr (pc));
		if (!blurb_printed)
		  {
		    printf_filtered ("\
This warning occurs if you are debugging a function without any symbols\n\
(for example, in a stripped executable).  In that case, you may wish to\n\
increase the size of the search with the `set heuristic-fence-post' command.\n\
\n\
Otherwise, you told GDB there was a function where there isn't one, or\n\
(more likely) you have encountered a bug in GDB.\n");
		    blurb_printed = 1;
		  }
	      }

	    return 0; 
	  }
	else if (ABOUT_TO_RETURN(start_pc))
	    break;

    start_pc += 2 * MIPS_INSTLEN; /* skip return, and its delay slot */
#if 0
    /* skip nops (usually 1) 0 - is this */
    while (start_pc < pc && read_memory_integer (start_pc, MIPS_INSTLEN) == 0)
	start_pc += MIPS_INSTLEN;
#endif
    return start_pc;
}

static mips_extra_func_info_t
heuristic_proc_desc(start_pc, limit_pc, next_frame)
    CORE_ADDR start_pc, limit_pc;
    struct frame_info *next_frame;
{
    CORE_ADDR sp = read_next_frame_reg (next_frame, SP_REGNUM);
    CORE_ADDR cur_pc;
    unsigned long frame_size;
    unsigned long r30_frame_size = 0;
    int has_frame_reg = 0;
    CORE_ADDR reg30 = 0; /* Value of $r30. Used by gcc for frame-pointer */
    unsigned long reg_mask = 0;

    if (start_pc == 0) return NULL;
    memset (&temp_proc_desc, '\0', sizeof(temp_proc_desc));
    memset (&temp_saved_regs, '\0', sizeof(struct frame_saved_regs));
    PROC_LOW_ADDR (&temp_proc_desc) = start_pc;

    if (start_pc + 200 < limit_pc)
      limit_pc = start_pc + 200;
  restart:
    frame_size = 0;
    for (cur_pc = start_pc; cur_pc < limit_pc; cur_pc += MIPS_INSTLEN) {
        char buf[MIPS_INSTLEN];
	unsigned long word;
	int status;

	status = (unsigned long) read_memory_nobpt (cur_pc, buf, MIPS_INSTLEN); /* FIXME!! */ 
	if (status) memory_error (status, cur_pc);
	word = (unsigned long) extract_unsigned_integer (buf, MIPS_INSTLEN); /* FIXME!! */

	if ((word & 0xFFFF0000) == 0x27bd0000		/* addiu $sp,$sp,-i */
	    || (word & 0xFFFF0000) == 0x23bd0000	/* addi $sp,$sp,-i */
	    || (word & 0xFFFF0000) == 0x67bd0000) {	/* daddiu $sp,$sp,-i */
	    if (word & 0x8000)
	      frame_size += (-word) & 0xffff;
	    else
	      /* Exit loop if a positive stack adjustment is found, which
		 usually means that the stack cleanup code in the function
		 epilogue is reached.  */
	      break;
	}
	else if ((word & 0xFFE00000) == 0xafa00000) {	/* sw reg,offset($sp) */
	    int reg = (word & 0x001F0000) >> 16;
	    reg_mask |= 1 << reg;
	    temp_saved_regs.regs[reg] = sp + (word & 0xffff);
	}
	else if ((word & 0xFFE00000) == 0xffa00000) {	/* sd reg,offset($sp) */
	    /* Irix 6.2 N32 ABI uses sd instructions for saving $gp and $ra,
	       but the register size used is only 32 bits. Make the address
	       for the saved register point to the lower 32 bits.  */
	    int reg = (word & 0x001F0000) >> 16;
	    reg_mask |= 1 << reg;
	    temp_saved_regs.regs[reg] = sp + (word & 0xffff) + 8 - MIPS_REGSIZE;
	}
	else if ((word & 0xFFFF0000) == 0x27be0000) { /* addiu $30,$sp,size */
	    /* Old gcc frame, r30 is virtual frame pointer.  */
	    if ((word & 0xffff) != frame_size)
		reg30 = sp + (word & 0xffff);
	    else if (!has_frame_reg) {
		unsigned alloca_adjust;
		has_frame_reg = 1;
		reg30 = read_next_frame_reg(next_frame, 30);
		alloca_adjust = (unsigned)(reg30 - (sp + (word & 0xffff)));
		if (alloca_adjust > 0) {
		    /* FP > SP + frame_size. This may be because
		     * of an alloca or somethings similar.
		     * Fix sp to "pre-alloca" value, and try again.
		     */
		    sp += alloca_adjust;
		    goto restart;
		}
	    }
	}
	else if ((word & 0xFFFFFFFB) == 0x03a0f021) { /* mov $30,$sp */
	    /* New gcc frame, virtual frame pointer is at r30 + frame_size.  */
	    if (!has_frame_reg) {
		unsigned alloca_adjust;
		has_frame_reg = 1;
		reg30 = read_next_frame_reg(next_frame, 30);
		r30_frame_size = frame_size;
		alloca_adjust = (unsigned)(reg30 - sp);
		if (alloca_adjust > 0) {
		    /* FP > SP + frame_size. This may be because
		     * of an alloca or somethings similar.
		     * Fix sp to "pre-alloca" value, and try again.
		     */
		    sp += alloca_adjust;
		    goto restart;
		}
	    }
	}
	else if ((word & 0xFFE00000) == 0xafc00000) { /* sw reg,offset($30) */
	    int reg = (word & 0x001F0000) >> 16;
	    reg_mask |= 1 << reg;
	    temp_saved_regs.regs[reg] = reg30 + (word & 0xffff);
	}
    }
    if (has_frame_reg) {
	PROC_FRAME_REG(&temp_proc_desc) = 30;
	PROC_FRAME_OFFSET(&temp_proc_desc) = r30_frame_size;
    }
    else {
	PROC_FRAME_REG(&temp_proc_desc) = SP_REGNUM;
	PROC_FRAME_OFFSET(&temp_proc_desc) = frame_size;
    }
    PROC_REG_MASK(&temp_proc_desc) = reg_mask;
    PROC_PC_REG(&temp_proc_desc) = RA_REGNUM;
    return &temp_proc_desc;
}

static mips_extra_func_info_t
find_proc_desc (pc, next_frame)
     CORE_ADDR pc;
     struct frame_info *next_frame;
{
  mips_extra_func_info_t proc_desc;
  struct block *b = block_for_pc(pc);
  struct symbol *sym;
  CORE_ADDR startaddr;

  find_pc_partial_function (pc, NULL, &startaddr, NULL);
  if (b == NULL || PC_IN_CALL_DUMMY (pc, 0, 0))
    sym = NULL;
  else
    {
      if (startaddr > BLOCK_START (b))
	/* This is the "pathological" case referred to in a comment in
	   print_frame_info.  It might be better to move this check into
	   symbol reading.  */
	sym = NULL;
      else
	sym = lookup_symbol (MIPS_EFI_SYMBOL_NAME, b, LABEL_NAMESPACE,
			     0, NULL);
    }

  /* If we never found a PDR for this function in symbol reading, then
     examine prologues to find the information.  */
  if (sym && ((mips_extra_func_info_t) SYMBOL_VALUE (sym))->pdr.framereg == -1)
    sym = NULL;

  if (sym)
    {
	/* IF this is the topmost frame AND
	 * (this proc does not have debugging information OR
	 * the PC is in the procedure prologue)
	 * THEN create a "heuristic" proc_desc (by analyzing
	 * the actual code) to replace the "official" proc_desc.
	 */
	proc_desc = (mips_extra_func_info_t) SYMBOL_VALUE (sym);
	if (next_frame == NULL) {
	    struct symtab_and_line val;
	    struct symbol *proc_symbol =
		PROC_DESC_IS_DUMMY(proc_desc) ? 0 : PROC_SYMBOL(proc_desc);

	    if (proc_symbol) {
		val = find_pc_line (BLOCK_START
				    (SYMBOL_BLOCK_VALUE(proc_symbol)),
				    0);
		val.pc = val.end ? val.end : pc;
	    }
	    if (!proc_symbol || pc < val.pc) {
		mips_extra_func_info_t found_heuristic =
		  heuristic_proc_desc (PROC_LOW_ADDR (proc_desc),
				       pc, next_frame);
		if (found_heuristic)
		  proc_desc = found_heuristic;
	    }
	}
    }
  else
    {
      /* Is linked_proc_desc_table really necessary?  It only seems to be used
	 by procedure call dummys.  However, the procedures being called ought
	 to have their own proc_descs, and even if they don't,
	 heuristic_proc_desc knows how to create them! */

      register struct linked_proc_info *link;

      for (link = linked_proc_desc_table; link; link = link->next)
	if (PROC_LOW_ADDR(&link->info) <= pc
	    && PROC_HIGH_ADDR(&link->info) > pc)
	  return &link->info;

      if (startaddr == 0)
	startaddr = heuristic_proc_start (pc);

      proc_desc =
	heuristic_proc_desc (startaddr, pc, next_frame);
    }
  return proc_desc;
}

static CORE_ADDR
get_frame_pointer(frame, proc_desc)
    struct frame_info *frame;
    mips_extra_func_info_t proc_desc;
{
  return ADDR_BITS_REMOVE (read_next_frame_reg (frame,
    PROC_FRAME_REG(proc_desc)) + PROC_FRAME_OFFSET(proc_desc));
}

mips_extra_func_info_t cached_proc_desc;

CORE_ADDR
mips_frame_chain(frame)
    struct frame_info *frame;
{
    mips_extra_func_info_t proc_desc;
    CORE_ADDR saved_pc = FRAME_SAVED_PC(frame);

    if (saved_pc == 0 || inside_entry_file (saved_pc))
      return 0;

    proc_desc = find_proc_desc(saved_pc, frame);
    if (!proc_desc)
      return 0;

    cached_proc_desc = proc_desc;

    /* If no frame pointer and frame size is zero, we must be at end
       of stack (or otherwise hosed).  If we don't check frame size,
       we loop forever if we see a zero size frame.  */
    if (PROC_FRAME_REG (proc_desc) == SP_REGNUM
	&& PROC_FRAME_OFFSET (proc_desc) == 0
	/* The previous frame from a sigtramp frame might be frameless
	   and have frame size zero.  */
	&& !frame->signal_handler_caller)
      return 0;
    else
      return get_frame_pointer (frame, proc_desc);
}

void
init_extra_frame_info(fci)
     struct frame_info *fci;
{
  int regnum;

  /* Use proc_desc calculated in frame_chain */
  mips_extra_func_info_t proc_desc =
    fci->next ? cached_proc_desc : find_proc_desc(fci->pc, fci->next);

  fci->saved_regs = NULL;
  fci->proc_desc =
    proc_desc == &temp_proc_desc ? 0 : proc_desc;
  if (proc_desc)
    {
      /* Fixup frame-pointer - only needed for top frame */
      /* This may not be quite right, if proc has a real frame register.
	 Get the value of the frame relative sp, procedure might have been
	 interrupted by a signal at it's very start.  */
      if (fci->pc == PROC_LOW_ADDR (proc_desc)
	  && !PROC_DESC_IS_DUMMY (proc_desc))
	fci->frame = read_next_frame_reg (fci->next, SP_REGNUM);
      else
	fci->frame = get_frame_pointer (fci->next, proc_desc);

      if (proc_desc == &temp_proc_desc)
	{
	  char *name;

	  /* Do not set the saved registers for a sigtramp frame,
	     mips_find_saved_registers will do that for us.
	     We can't use fci->signal_handler_caller, it is not yet set.  */
	  find_pc_partial_function (fci->pc, &name,
				    (CORE_ADDR *)NULL,(CORE_ADDR *)NULL);
	  if (!IN_SIGTRAMP (fci->pc, name))
	    {
	      fci->saved_regs = (struct frame_saved_regs*)
		obstack_alloc (&frame_cache_obstack,
			       sizeof (struct frame_saved_regs));
	      *fci->saved_regs = temp_saved_regs;
	      fci->saved_regs->regs[PC_REGNUM]
		= fci->saved_regs->regs[RA_REGNUM];
	    }
	}

      /* hack: if argument regs are saved, guess these contain args */
      fci->num_args = -1;	/* assume we can't tell how many args for now */
      for (regnum = MIPS_LAST_ARG_REGNUM; regnum >= A0_REGNUM; regnum--)
	{
	  if (PROC_REG_MASK(proc_desc) & (1 << regnum))
	    {
	      fci->num_args = regnum - A0_REGNUM + 1;
	      break;
	    }
	} 
    }
}

/* MIPS stack frames are almost impenetrable.  When execution stops,
   we basically have to look at symbol information for the function
   that we stopped in, which tells us *which* register (if any) is
   the base of the frame pointer, and what offset from that register
   the frame itself is at.  

   This presents a problem when trying to examine a stack in memory
   (that isn't executing at the moment), using the "frame" command.  We
   don't have a PC, nor do we have any registers except SP.

   This routine takes two arguments, SP and PC, and tries to make the
   cached frames look as if these two arguments defined a frame on the
   cache.  This allows the rest of info frame to extract the important
   arguments without difficulty.  */

struct frame_info *
setup_arbitrary_frame (argc, argv)
     int argc;
     CORE_ADDR *argv;
{
  if (argc != 2)
    error ("MIPS frame specifications require two arguments: sp and pc");

  return create_new_frame (argv[0], argv[1]);
}


int
mips_pc_in_call_dummy (pc)
     CORE_ADDR pc;
{
  return pc >= CALL_DUMMY_ADDRESS ()
	 && pc <= CALL_DUMMY_ADDRESS () + DECR_PC_AFTER_BREAK;
}

CORE_ADDR
mips_push_arguments(nargs, args, sp, struct_return, struct_addr)
     int nargs;
     value_ptr *args;
     CORE_ADDR sp;
     int struct_return;
     CORE_ADDR struct_addr;
{
  int argreg;
  int float_argreg;
  int argnum;
  int len = 0;
  int stack_offset;

  /* Macros to round N up or down to the next A boundary; A must be
     a power of two. */
#define ROUND_DOWN(n,a) ((n) & ~((a)-1))
#define ROUND_UP(n,a) (((n)+(a)-1) & ~((a)-1))
  
  /* First ensure that the stack and structure return address (if any)
     are properly aligned. The stack has to be 64-bit aligned even
     on 32-bit machines, because doubles must be 64-bit aligned. */
  sp = ROUND_DOWN (sp, 8);
  struct_addr = ROUND_DOWN (struct_addr, MIPS_REGSIZE);
      
  /* Now make space on the stack for the args. We allocate more
     than necessary for EABI, because the first few arguments are
     passed in registers, but that's OK. */
  for (argnum = 0; argnum < nargs; argnum++)
    len += ROUND_UP (TYPE_LENGTH(VALUE_TYPE(args[argnum])), MIPS_REGSIZE);
  sp -= ROUND_UP (len, MIPS_REGSIZE);

  /* Initialize the integer and float register pointers.  */
  argreg = A0_REGNUM;
  float_argreg = FPA0_REGNUM;

  /* the struct_return pointer occupies the first parameter-passing reg */
  if (struct_return)
      write_register (argreg++, struct_addr);

  /* The offset onto the stack at which we will start copying parameters
     (after the registers are used up) begins at 16 in the old ABI.
     This leaves room for the "home" area for register parameters.  */
  stack_offset = MIPS_EABI ? 0 : MIPS_REGSIZE * 4;

  /* Now load as many as possible of the first arguments into
     registers, and push the rest onto the stack.  Loop thru args
     from first to last.  */
  for (argnum = 0; argnum < nargs; argnum++)
    {
      char *val;
      char valbuf[REGISTER_RAW_SIZE(A0_REGNUM)];
      value_ptr arg = args[argnum];
      struct type *arg_type = check_typedef (VALUE_TYPE (arg));
      int len = TYPE_LENGTH (arg_type);
      enum type_code typecode = TYPE_CODE (arg_type);

      /* The EABI passes structures that fit in a register by value.
	 In all other cases, pass the structure by reference.  */
      if (typecode == TYPE_CODE_STRUCT && (!MIPS_EABI || len > MIPS_REGSIZE))
	{
	  store_address (valbuf, MIPS_REGSIZE, VALUE_ADDRESS (arg));
	  len = MIPS_REGSIZE;
	  val = valbuf;
	}
      else
	val = (char *)VALUE_CONTENTS (arg);

      /* 32-bit ABIs always start floating point arguments in an
         even-numbered floating point register.   */
      if (!GDB_TARGET_IS_MIPS64 && typecode == TYPE_CODE_FLT
          && (float_argreg & 1))
	float_argreg++;

      /* Floating point arguments passed in registers have to be
         treated specially.  On 32-bit architectures, doubles
	 are passed in register pairs; the even register gets
	 the low word, and the odd register gets the high word.  */
      if (typecode == TYPE_CODE_FLT
	  && float_argreg <= MIPS_LAST_FP_ARG_REGNUM
	  && mips_fpu != MIPS_FPU_NONE)
	{
	  if (!GDB_TARGET_IS_MIPS64 && len == 8)
	    {
	      int low_offset = TARGET_BYTE_ORDER == BIG_ENDIAN ? 4 : 0;
	      unsigned long regval;

	      regval = extract_unsigned_integer (val+low_offset, 4);
	      write_register (float_argreg++, regval);	/* low word */
	      regval = extract_unsigned_integer (val+4-low_offset, 4);
	      write_register (float_argreg++, regval);	/* high word */

	    }
	  else
	    {
	      CORE_ADDR regval = extract_address (val, len);
	      write_register (float_argreg++, regval);
	    }

	  /* If this is the old ABI, skip one or two general registers.  */
	  if (!MIPS_EABI)
	    argreg += GDB_TARGET_IS_MIPS64 ? 1 : 2;
	}
      else
	{
	  /* Copy the argument to general registers or the stack in
	     register-sized pieces.  Large arguments are split between
	     registers and stack.  */
	  while (len > 0)
	    {
	      int partial_len = len < MIPS_REGSIZE ? len : MIPS_REGSIZE;
	      CORE_ADDR regval = extract_address (val, partial_len);
	      
	      if (argreg <= MIPS_LAST_ARG_REGNUM)
		{
		  /* It's a simple argument being passed in a general register.  */
		  write_register (argreg, regval);
		  argreg++;
    
		  /* If this is the old ABI, prevent subsequent floating
		     point arguments from being passed in floating point
		     registers.  */
		  if (!MIPS_EABI)
		    float_argreg = MIPS_LAST_FP_ARG_REGNUM + 1;
		}
	      else
		{
		  /* Promote this portion of the argument to a register-sized
		     chunk before pushing it on the stack.  */
		  char partial_buf[MIPS_REGSIZE];
		  store_address (partial_buf, MIPS_REGSIZE, regval);
		  write_memory (sp + stack_offset, partial_buf, MIPS_REGSIZE);
		  stack_offset += MIPS_REGSIZE;
		}
    
	      len -= partial_len;
	      val += partial_len;
	    }
	}
    }

  /* Set the return address register to point to the entry
     point of the program, where a breakpoint lies in wait.  */
  write_register (RA_REGNUM, CALL_DUMMY_ADDRESS());

  /* Return adjusted stack pointer.  */
  return sp;
}

void
mips_push_register(CORE_ADDR *sp, int regno)
{
  char buffer[MAX_REGISTER_RAW_SIZE];
  int regsize = REGISTER_RAW_SIZE (regno);

  *sp -= regsize;
  read_register_gen (regno, buffer);
  write_memory (*sp, buffer, regsize);
}

/* MASK(i,j) == (1<<i) + (1<<(i+1)) + ... + (1<<j)). Assume i<=j<(MIPS_NUMREGS-1). */
#define MASK(i,j) (((1 << ((j)+1))-1) ^ ((1 << (i))-1))

void
mips_push_dummy_frame()
{
  int ireg;
  struct linked_proc_info *link = (struct linked_proc_info*)
      xmalloc(sizeof(struct linked_proc_info));
  mips_extra_func_info_t proc_desc = &link->info;
  CORE_ADDR sp = ADDR_BITS_REMOVE (read_register (SP_REGNUM));
  CORE_ADDR old_sp = sp;
  link->next = linked_proc_desc_table;
  linked_proc_desc_table = link;

/* FIXME!   are these correct ? */
#define PUSH_FP_REGNUM 16 /* must be a register preserved across calls */
#define GEN_REG_SAVE_MASK MASK(1,16)|MASK(24,28)|(1<<(MIPS_NUMREGS-1))
#define FLOAT_REG_SAVE_MASK MASK(0,19)
#define FLOAT_SINGLE_REG_SAVE_MASK \
  ((1<<18)|(1<<16)|(1<<14)|(1<<12)|(1<<10)|(1<<8)|(1<<6)|(1<<4)|(1<<2)|(1<<0))
  /*
   * The registers we must save are all those not preserved across
   * procedure calls. Dest_Reg (see tm-mips.h) must also be saved.
   * In addition, we must save the PC, PUSH_FP_REGNUM, MMLO/-HI
   * and FP Control/Status registers.
   * 
   *
   * Dummy frame layout:
   *  (high memory)
   * 	Saved PC
   *	Saved MMHI, MMLO, FPC_CSR
   *	Saved R31
   *	Saved R28
   *	...
   *	Saved R1
   *    Saved D18 (i.e. F19, F18)
   *    ...
   *    Saved D0 (i.e. F1, F0)
   *	CALL_DUMMY (subroutine stub; see tm-mips.h)
   *	Parameter build area (not yet implemented)
   *  (low memory)
   */

  /* Save special registers (PC, MMHI, MMLO, FPC_CSR) */
  write_register (PUSH_FP_REGNUM, sp);
  PROC_FRAME_REG(proc_desc) = PUSH_FP_REGNUM;
  PROC_FRAME_OFFSET(proc_desc) = 0;
  mips_push_register (&sp, PC_REGNUM);
  mips_push_register (&sp, HI_REGNUM);
  mips_push_register (&sp, LO_REGNUM);
  mips_push_register (&sp, mips_fpu == MIPS_FPU_NONE ? 0 : FCRCS_REGNUM);

  /* Save general CPU registers */
  PROC_REG_MASK(proc_desc) = GEN_REG_SAVE_MASK;
  PROC_REG_OFFSET(proc_desc) = sp - old_sp; /* offset of (Saved R31) from FP */
  for (ireg = 32; --ireg >= 0; )
    if (PROC_REG_MASK(proc_desc) & (1 << ireg))
      mips_push_register (&sp, ireg);

  /* Save floating point registers starting with high order word */
  PROC_FREG_MASK(proc_desc) = 
    mips_fpu == MIPS_FPU_DOUBLE ? FLOAT_REG_SAVE_MASK
    : mips_fpu == MIPS_FPU_SINGLE ? FLOAT_SINGLE_REG_SAVE_MASK : 0;
  PROC_FREG_OFFSET(proc_desc) = sp - old_sp; /* offset of (Saved D18) from FP */
  for (ireg = 32; --ireg >= 0; )
    if (PROC_FREG_MASK(proc_desc) & (1 << ireg))
      mips_push_register (&sp, ireg + FP0_REGNUM);

  /* Update the stack pointer.  Set the procedure's starting and ending
     addresses to point to the place on the stack where we'll be writing the
     dummy code (in mips_push_arguments). */
  write_register (SP_REGNUM, sp);
  PROC_LOW_ADDR(proc_desc) = CALL_DUMMY_ADDRESS();
  PROC_HIGH_ADDR(proc_desc) =  CALL_DUMMY_ADDRESS() + 4;
  SET_PROC_DESC_IS_DUMMY(proc_desc);
  PROC_PC_REG(proc_desc) = RA_REGNUM;
}

void
mips_pop_frame()
{
  register int regnum;
  struct frame_info *frame = get_current_frame ();
  CORE_ADDR new_sp = FRAME_FP (frame);

  mips_extra_func_info_t proc_desc = frame->proc_desc;

  write_register (PC_REGNUM, FRAME_SAVED_PC(frame));
  if (frame->saved_regs == NULL)
    mips_find_saved_regs (frame);
  if (proc_desc)
    {
      for (regnum = MIPS_NUMREGS; --regnum >= 0; )
	if (PROC_REG_MASK(proc_desc) & (1 << regnum))
	  write_register (regnum,
			  read_memory_integer (frame->saved_regs->regs[regnum],
					       MIPS_REGSIZE)); 
      for (regnum = MIPS_NUMREGS; --regnum >= 0; )
	if (PROC_FREG_MASK(proc_desc) & (1 << regnum))
	  write_register (regnum + FP0_REGNUM,
			  read_memory_integer (frame->saved_regs->regs[regnum + FP0_REGNUM], MIPS_REGSIZE)); 
    }
  write_register (SP_REGNUM, new_sp);
  flush_cached_frames ();

  if (proc_desc && PROC_DESC_IS_DUMMY(proc_desc))
    {
      struct linked_proc_info *pi_ptr, *prev_ptr;

      for (pi_ptr = linked_proc_desc_table, prev_ptr = NULL;
	   pi_ptr != NULL;
	   prev_ptr = pi_ptr, pi_ptr = pi_ptr->next)
	{
	  if (&pi_ptr->info == proc_desc)
	    break;
	}

      if (pi_ptr == NULL)
	error ("Can't locate dummy extra frame info\n");

      if (prev_ptr != NULL)
	prev_ptr->next = pi_ptr->next;
      else
	linked_proc_desc_table = pi_ptr->next;

      free (pi_ptr);

      write_register (HI_REGNUM,
	        read_memory_integer (new_sp - 2*MIPS_REGSIZE, MIPS_REGSIZE));
      write_register (LO_REGNUM,
	        read_memory_integer (new_sp - 3*MIPS_REGSIZE, MIPS_REGSIZE));
      if (mips_fpu != MIPS_FPU_NONE)
	write_register (FCRCS_REGNUM,
	        read_memory_integer (new_sp - 4*MIPS_REGSIZE, MIPS_REGSIZE));
    }
}

static void
mips_print_register (regnum, all)
     int regnum, all;
{
  char raw_buffer[MAX_REGISTER_RAW_SIZE];

  /* Get the data in raw format.  */
  if (read_relative_register_raw_bytes (regnum, raw_buffer))
    {
      printf_filtered ("%s: [Invalid]", reg_names[regnum]);
      return;
    }

  /* If an even floating pointer register, also print as double. */
  if (regnum >= FP0_REGNUM && regnum < FP0_REGNUM+MIPS_NUMREGS
      && !((regnum-FP0_REGNUM) & 1))
    {
      char dbuffer[MAX_REGISTER_RAW_SIZE]; 

      read_relative_register_raw_bytes (regnum, dbuffer);
      read_relative_register_raw_bytes (regnum+1, dbuffer+4); /* FIXME!! */
#ifdef REGISTER_CONVERT_TO_TYPE
      REGISTER_CONVERT_TO_TYPE(regnum, builtin_type_double, dbuffer);
#endif
      printf_filtered ("(d%d: ", regnum-FP0_REGNUM);
      val_print (builtin_type_double, dbuffer, 0,
		 gdb_stdout, 0, 1, 0, Val_pretty_default);
      printf_filtered ("); ");
    }
  fputs_filtered (reg_names[regnum], gdb_stdout);

  /* The problem with printing numeric register names (r26, etc.) is that
     the user can't use them on input.  Probably the best solution is to
     fix it so that either the numeric or the funky (a2, etc.) names
     are accepted on input.  */
  if (regnum < MIPS_NUMREGS)
    printf_filtered ("(r%d): ", regnum);
  else
    printf_filtered (": ");

  /* If virtual format is floating, print it that way.  */
  if (TYPE_CODE (REGISTER_VIRTUAL_TYPE (regnum)) == TYPE_CODE_FLT)
    val_print (REGISTER_VIRTUAL_TYPE (regnum), raw_buffer, 0,
	       gdb_stdout, 0, 1, 0, Val_pretty_default);
  /* Else print as integer in hex.  */
  else
    print_scalar_formatted (raw_buffer, REGISTER_VIRTUAL_TYPE (regnum),
			    'x', 0, gdb_stdout);
}

/* Replacement for generic do_registers_info.  */

void
mips_do_registers_info (regnum, fpregs)
     int regnum;
     int fpregs;
{
  if (regnum != -1)
    {
      if (*(reg_names[regnum]) == '\0')
	error ("Not a valid register for the current processor type");

      mips_print_register (regnum, 0);
      printf_filtered ("\n");
    }
  else
    {
      int did_newline = 0;

      for (regnum = 0; regnum < NUM_REGS; )
	{
	  if (((!fpregs) && regnum >= FP0_REGNUM && regnum <= FCRIR_REGNUM)
	      || *(reg_names[regnum]) == '\0')
	    {
	      regnum++;
	      continue;
	    }
	  mips_print_register (regnum, 1);
	  regnum++;
	  printf_filtered ("; ");
	  did_newline = 0;
	  if ((regnum & 3) == 0)
	    {
	      printf_filtered ("\n");
	      did_newline = 1;
	    }
	}
      if (!did_newline)
	printf_filtered ("\n");
    }
}

/* Return number of args passed to a frame. described by FIP.
   Can return -1, meaning no way to tell.  */

int
mips_frame_num_args (frame)
	struct frame_info *frame;
{
#if 0 /* FIXME Use or lose this! */
  struct chain_info_t *p;

  p = mips_find_cached_frame (FRAME_FP (frame));
  if (p->valid)
    return p->the_info.numargs;
#endif
  return -1;
}

/* Is this a branch with a delay slot?  */

static int is_delayed PARAMS ((unsigned long));

static int
is_delayed (insn)
     unsigned long insn;
{
  int i;
  for (i = 0; i < NUMOPCODES; ++i)
    if (mips_opcodes[i].pinfo != INSN_MACRO
	&& (insn & mips_opcodes[i].mask) == mips_opcodes[i].match)
      break;
  return (i < NUMOPCODES
	  && (mips_opcodes[i].pinfo & (INSN_UNCOND_BRANCH_DELAY
				       | INSN_COND_BRANCH_DELAY
				       | INSN_COND_BRANCH_LIKELY)));
}

int
mips_step_skips_delay (pc)
     CORE_ADDR pc;
{
  char buf[MIPS_INSTLEN];

  if (target_read_memory (pc, buf, MIPS_INSTLEN) != 0)
    /* If error reading memory, guess that it is not a delayed branch.  */
    return 0;
  return is_delayed ((unsigned long)extract_unsigned_integer (buf, MIPS_INSTLEN));
}

/* To skip prologues, I use this predicate.  Returns either PC itself
   if the code at PC does not look like a function prologue; otherwise
   returns an address that (if we're lucky) follows the prologue.  If
   LENIENT, then we must skip everything which is involved in setting
   up the frame (it's OK to skip more, just so long as we don't skip
   anything which might clobber the registers which are being saved.
   We must skip more in the case where part of the prologue is in the
   delay slot of a non-prologue instruction).  */

CORE_ADDR
mips_skip_prologue (pc, lenient)
     CORE_ADDR pc;
     int lenient;
{
    t_inst inst;
    unsigned offset; 
    int seen_sp_adjust = 0;
    int load_immediate_bytes = 0;
    CORE_ADDR post_prologue_pc;

    /* See if we can determine the end of the prologue via the symbol table.
       If so, then return either PC, or the PC after the prologue, whichever
       is greater.  */

    post_prologue_pc = after_prologue (pc, NULL);

    if (post_prologue_pc != 0)
      return max (pc, post_prologue_pc);

    /* Can't determine prologue from the symbol table, need to examine
       instructions.  */

    /* Skip the typical prologue instructions. These are the stack adjustment
       instruction and the instructions that save registers on the stack
       or in the gcc frame.  */
    for (offset = 0; offset < 100; offset += MIPS_INSTLEN)
      {
	char buf[MIPS_INSTLEN];
	int status;

	status = read_memory_nobpt (pc + offset, buf, MIPS_INSTLEN);
	if (status)
	  memory_error (status, pc + offset);
	inst = (unsigned long)extract_unsigned_integer (buf, MIPS_INSTLEN);

#if 0
	if (lenient && is_delayed (inst))
	  continue;
#endif

	if ((inst & 0xffff0000) == 0x27bd0000	/* addiu $sp,$sp,offset */
	    || (inst & 0xffff0000) == 0x67bd0000) /* daddiu $sp,$sp,offset */
	    seen_sp_adjust = 1;
	else if (inst == 0x03a1e823 || 	        /* subu $sp,$sp,$at */
		 inst == 0x03a8e823)   	        /* subu $sp,$sp,$t0 */
	    seen_sp_adjust = 1;
	else if (((inst & 0xFFE00000) == 0xAFA00000 /* sw reg,n($sp) */
		  || (inst & 0xFFE00000) == 0xFFA00000) /* sd reg,n($sp) */
		 && (inst & 0x001F0000))	/* reg != $zero */
	    continue;
						
	else if ((inst & 0xFFE00000) == 0xE7A00000) /* swc1 freg,n($sp) */
	    continue;
	else if ((inst & 0xF3E00000) == 0xA3C00000 && (inst & 0x001F0000))
						/* sx reg,n($s8) */
	    continue;				/* reg != $zero */
 
        /* move $s8,$sp.  With different versions of gas this will be either
           `addu $s8,$sp,$zero' or `or $s8,$sp,$zero'.  Accept either.  */
        else if (inst == 0x03A0F021 || inst == 0x03a0f025)
	    continue;

	else if ((inst & 0xFF9F07FF) == 0x00800021) /* move reg,$a0-$a3 */
	    continue;
	else if ((inst & 0xffff0000) == 0x3c1c0000) /* lui $gp,n */
	    continue;
	else if ((inst & 0xffff0000) == 0x279c0000) /* addiu $gp,$gp,n */
	    continue;
	else if (inst == 0x0399e021		/* addu $gp,$gp,$t9 */
		 || inst == 0x033ce021)		/* addu $gp,$t9,$gp */
	  continue;
	/* The following instructions load $at or $t0 with an immediate
	   value in preparation for a stack adjustment via
	   subu $sp,$sp,[$at,$t0]. These instructions could also initialize
	   a local variable, so we accept them only before a stack adjustment
	   instruction was seen.  */
	else if (!seen_sp_adjust)
	  {
	    if ((inst & 0xffff0000) == 0x3c010000 ||	  /* lui $at,n */
		(inst & 0xffff0000) == 0x3c080000)	  /* lui $t0,n */
	      {
		load_immediate_bytes += MIPS_INSTLEN; /* FIXME!! */
		continue;
	      }
	    else if ((inst & 0xffff0000) == 0x34210000 || /* ori $at,$at,n */
		     (inst & 0xffff0000) == 0x35080000 || /* ori $t0,$t0,n */
		     (inst & 0xffff0000) == 0x34010000 || /* ori $at,$zero,n */
		     (inst & 0xffff0000) == 0x34080000)   /* ori $t0,$zero,n */
	      {
		load_immediate_bytes += MIPS_INSTLEN; /* FIXME!! */
		continue;
	      }
	    else
	      break;
	  }
	else
	  break;
    }

    /* In a frameless function, we might have incorrectly
       skipped some load immediate instructions. Undo the skipping
       if the load immediate was not followed by a stack adjustment.  */
    if (load_immediate_bytes && !seen_sp_adjust)
      offset -= load_immediate_bytes;
    return pc + offset;
}

#if 0
/* The lenient prologue stuff should be superceded by the code in
   init_extra_frame_info which looks to see whether the stores mentioned
   in the proc_desc have actually taken place.  */

/* Is address PC in the prologue (loosely defined) for function at
   STARTADDR?  */

static int
mips_in_lenient_prologue (startaddr, pc)
     CORE_ADDR startaddr;
     CORE_ADDR pc;
{
  CORE_ADDR end_prologue = mips_skip_prologue (startaddr, 1);
  return pc >= startaddr && pc < end_prologue;
}
#endif

/* Given a return value in `regbuf' with a type `valtype', 
   extract and copy its value into `valbuf'.  */
void
mips_extract_return_value (valtype, regbuf, valbuf)
    struct type *valtype;
    char regbuf[REGISTER_BYTES];
    char *valbuf;
{
  int regnum;
  int offset = 0;
  
  regnum = 2;
  if (TYPE_CODE (valtype) == TYPE_CODE_FLT
       && (mips_fpu == MIPS_FPU_DOUBLE
	   || (mips_fpu == MIPS_FPU_SINGLE && TYPE_LENGTH (valtype) <= 4))) /* FIXME!! */
    regnum = FP0_REGNUM;

  if (TARGET_BYTE_ORDER == BIG_ENDIAN
      && TYPE_CODE (valtype) != TYPE_CODE_FLT
      && TYPE_LENGTH (valtype) < REGISTER_RAW_SIZE (regnum))
    offset = REGISTER_RAW_SIZE (regnum) - TYPE_LENGTH (valtype);

  memcpy (valbuf, regbuf + REGISTER_BYTE (regnum) + offset,
	  TYPE_LENGTH (valtype));
#ifdef REGISTER_CONVERT_TO_TYPE
  REGISTER_CONVERT_TO_TYPE(regnum, valtype, valbuf);
#endif
}

/* Given a return value in `regbuf' with a type `valtype', 
   write it's value into the appropriate register.  */
void
mips_store_return_value (valtype, valbuf)
    struct type *valtype;
    char *valbuf;
{
  int regnum;
  char raw_buffer[MAX_REGISTER_RAW_SIZE];
  
  regnum = 2;
  if (TYPE_CODE (valtype) == TYPE_CODE_FLT
       && (mips_fpu == MIPS_FPU_DOUBLE
	   || (mips_fpu == MIPS_FPU_SINGLE && TYPE_LENGTH (valtype) <= 4))) /* FIXME!! */
    regnum = FP0_REGNUM;

  memcpy(raw_buffer, valbuf, TYPE_LENGTH (valtype));

#ifdef REGISTER_CONVERT_FROM_TYPE
  REGISTER_CONVERT_FROM_TYPE(regnum, valtype, raw_buffer);
#endif

  write_register_bytes(REGISTER_BYTE (regnum), raw_buffer, TYPE_LENGTH (valtype));
}

/* Exported procedure: Is PC in the signal trampoline code */

int
in_sigtramp (pc, ignore)
     CORE_ADDR pc;
     char *ignore;		/* function name */
{
  if (sigtramp_address == 0)
    fixup_sigtramp ();
  return (pc >= sigtramp_address && pc < sigtramp_end);
}

/* Command to set FPU type.  mips_fpu_string will have been set to the
   user's argument.  Set mips_fpu based on mips_fpu_string, and then
   canonicalize mips_fpu_string.  */

/*ARGSUSED*/
static void
mips_set_fpu_command (args, from_tty, c)
     char *args;
     int from_tty;
     struct cmd_list_element *c;
{
  char *err = NULL;

  if (mips_fpu_string == NULL || *mips_fpu_string == '\0')
    mips_fpu = MIPS_FPU_DOUBLE;
  else if (strcasecmp (mips_fpu_string, "double") == 0
	   || strcasecmp (mips_fpu_string, "on") == 0
	   || strcasecmp (mips_fpu_string, "1") == 0
	   || strcasecmp (mips_fpu_string, "yes") == 0)
    mips_fpu = MIPS_FPU_DOUBLE;
  else if (strcasecmp (mips_fpu_string, "none") == 0
	   || strcasecmp (mips_fpu_string, "off") == 0
	   || strcasecmp (mips_fpu_string, "0") == 0
	   || strcasecmp (mips_fpu_string, "no") == 0)
    mips_fpu = MIPS_FPU_NONE;
  else if (strcasecmp (mips_fpu_string, "single") == 0)
    mips_fpu = MIPS_FPU_SINGLE;
  else
    err = strsave (mips_fpu_string);

  if (mips_fpu_string != NULL)
    free (mips_fpu_string);

  switch (mips_fpu)
    {
    case MIPS_FPU_DOUBLE:
      mips_fpu_string = strsave ("double");
      break;
    case MIPS_FPU_SINGLE:
      mips_fpu_string = strsave ("single");
      break;
    case MIPS_FPU_NONE:
      mips_fpu_string = strsave ("none");
      break;
    }

  if (err != NULL)
    {
      struct cleanup *cleanups = make_cleanup (free, err);
      error ("Unknown FPU type `%s'.  Use `double', `none', or `single'.",
	     err);
      do_cleanups (cleanups);
    }
}

static void
mips_show_fpu_command (args, from_tty, c)
     char *args;
     int from_tty;
     struct cmd_list_element *c;
{
}

/* Command to set the processor type.  */

void
mips_set_processor_type_command (args, from_tty)
     char *args;
     int from_tty;
{
  int i;

  if (tmp_mips_processor_type == NULL || *tmp_mips_processor_type == '\0')
    {
      printf_unfiltered ("The known MIPS processor types are as follows:\n\n");
      for (i = 0; mips_processor_type_table[i].name != NULL; ++i)
	printf_unfiltered ("%s\n", mips_processor_type_table[i].name);

      /* Restore the value.  */
      tmp_mips_processor_type = strsave (mips_processor_type);

      return;
    }
  
  if (!mips_set_processor_type (tmp_mips_processor_type))
    {
      error ("Unknown processor type `%s'.", tmp_mips_processor_type);
      /* Restore its value.  */
      tmp_mips_processor_type = strsave (mips_processor_type);
    }
}

static void
mips_show_processor_type_command (args, from_tty)
     char *args;
     int from_tty;
{
}

/* Modify the actual processor type. */

int
mips_set_processor_type (str)
     char *str;
{
  int i, j;

  if (str == NULL)
    return 0;

  for (i = 0; mips_processor_type_table[i].name != NULL; ++i)
    {
      if (strcasecmp (str, mips_processor_type_table[i].name) == 0)
	{
	  mips_processor_type = str;

	  for (j = 0; j < NUM_REGS; ++j)
	    reg_names[j] = mips_processor_type_table[i].regnames[j];

	  return 1;

	  /* FIXME tweak fpu flag too */
	}
    }

  return 0;
}

/* Attempt to identify the particular processor model by reading the
   processor id.  */

char *
mips_read_processor_type ()
{
  CORE_ADDR prid;

  prid = read_register (PRID_REGNUM);

  if ((prid & ~0xf) == 0x700)
    return savestring ("r3041", strlen("r3041"));

  return NULL;
}

/* Just like reinit_frame_cache, but with the right arguments to be
   callable as an sfunc.  */

static void
reinit_frame_cache_sfunc (args, from_tty, c)
     char *args;
     int from_tty;
     struct cmd_list_element *c;
{
  reinit_frame_cache ();
}

static int
gdb_print_insn_mips (memaddr, info)
     bfd_vma memaddr;
     disassemble_info *info;
{
  if (TARGET_BYTE_ORDER == BIG_ENDIAN)
    return print_insn_big_mips (memaddr, info);
  else
    return print_insn_little_mips (memaddr, info);
}

void
_initialize_mips_tdep ()
{
  struct cmd_list_element *c;

  tm_print_insn = gdb_print_insn_mips;

  /* Let the user turn off floating point and set the fence post for
     heuristic_proc_start.  */

  c = add_set_cmd ("mipsfpu", class_support, var_string_noescape,
		   (char *) &mips_fpu_string,
		   "Set use of floating point coprocessor.\n\
Set to `none' to avoid using floating point instructions when calling\n\
functions or dealing with return values.  Set to `single' to use only\n\
single precision floating point as on the R4650.  Set to `double' for\n\
normal floating point support.",
		   &setlist);
  c->function.sfunc = mips_set_fpu_command;
  c = add_show_from_set (c, &showlist);
  c->function.sfunc = mips_show_fpu_command;

  mips_fpu = MIPS_FPU_DOUBLE;
  mips_fpu_string = strsave ("double");

  c = add_set_cmd ("processor", class_support, var_string_noescape,
		   (char *) &tmp_mips_processor_type,
		   "Set the type of MIPS processor in use.\n\
Set this to be able to access processor-type-specific registers.\n\
",
		   &setlist);
  c->function.cfunc = mips_set_processor_type_command;
  c = add_show_from_set (c, &showlist);
  c->function.cfunc = mips_show_processor_type_command;

  tmp_mips_processor_type = strsave (DEFAULT_MIPS_TYPE);
  mips_set_processor_type_command (strsave (DEFAULT_MIPS_TYPE), 0);

  /* We really would like to have both "0" and "unlimited" work, but
     command.c doesn't deal with that.  So make it a var_zinteger
     because the user can always use "999999" or some such for unlimited.  */
  c = add_set_cmd ("heuristic-fence-post", class_support, var_zinteger,
		   (char *) &heuristic_fence_post,
		   "\
Set the distance searched for the start of a function.\n\
If you are debugging a stripped executable, GDB needs to search through the\n\
program for the start of a function.  This command sets the distance of the\n\
search.  The only need to set it is when debugging a stripped executable.",
		   &setlist);
  /* We need to throw away the frame cache when we set this, since it
     might change our ability to get backtraces.  */
  c->function.sfunc = reinit_frame_cache_sfunc;
  add_show_from_set (c, &showlist);
}
