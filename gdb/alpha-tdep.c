/* Target-dependent code for the ALPHA architecture, for GDB, the GNU Debugger.
   Copyright 1993 Free Software Foundation, Inc.

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
#include "symtab.h"
#include "value.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "dis-asm.h"

/* FIXME: Some of this code should perhaps be merged with mips-tdep.c.  */

#define VM_MIN_ADDRESS (CORE_ADDR)0x120000000


/* Forward declarations.  */

static CORE_ADDR
read_next_frame_reg PARAMS ((FRAME, int));

static CORE_ADDR
heuristic_proc_start PARAMS ((CORE_ADDR));

static alpha_extra_func_info_t
heuristic_proc_desc PARAMS ((CORE_ADDR, CORE_ADDR, FRAME));

static alpha_extra_func_info_t
find_proc_desc PARAMS ((CORE_ADDR, FRAME));

static int
alpha_in_lenient_prologue PARAMS ((CORE_ADDR, CORE_ADDR));

static void
reinit_frame_cache_sfunc PARAMS ((char *, int, struct cmd_list_element *));

void
_initialize_alpha_tdep PARAMS ((void));

/* Heuristic_proc_start may hunt through the text section for a long
   time across a 2400 baud serial line.  Allows the user to limit this
   search.  */
static unsigned int heuristic_fence_post = 0;

/* Layout of a stack frame on the alpha:

                |				|
 pdr members:	|  7th ... nth arg,		|
                |  `pushed' by caller.		|
                |				|
----------------|-------------------------------|<--  old_sp == vfp
   ^  ^  ^  ^	|				|
   |  |  |  |	|				|
   |  |localoff	|  Copies of 1st .. 6th		|
   |  |  |  |	|  argument if necessary.	|
   |  |  |  v	|				|
   |  |  |  ---	|-------------------------------|<-- FRAME_LOCALS_ADDRESS
   |  |  |      |				|
   |  |  |      |  Locals and temporaries.	|
   |  |  |      |				|
   |  |  |      |-------------------------------|
   |  |  |      |				|
   |-fregoffset	|  Saved float registers.	|
   |  |  |      |  F9				|
   |  |  |      |   .				|
   |  |  |      |   .				|
   |  |  |      |  F2				|
   |  |  v      |				|
   |  |  -------|-------------------------------|
   |  |         |				|
   |  |         |  Saved registers.		|
   |  |         |  S6				|
   |-regoffset	|   .				|
   |  |         |   .				|
   |  |         |  S0				|
   |  |         |  pdr.pcreg			|
   |  v         |				|
   |  ----------|-------------------------------|
   |            |				|
 frameoffset    |  Argument build area, gets	|
   |            |  7th ... nth arg for any	|
   |            |  called procedure.		|
   v            |  				|
   -------------|-------------------------------|<-- sp
                |				|
*/

#define PROC_LOW_ADDR(proc) ((proc)->pdr.adr) /* least address */
#define PROC_HIGH_ADDR(proc) ((proc)->pdr.iline) /* upper address bound */
#define PROC_DUMMY_FRAME(proc) ((proc)->pdr.iopt) /* frame for CALL_DUMMY */
#define PROC_FRAME_OFFSET(proc) ((proc)->pdr.frameoffset)
#define PROC_FRAME_REG(proc) ((proc)->pdr.framereg)
#define PROC_REG_MASK(proc) ((proc)->pdr.regmask)
#define PROC_FREG_MASK(proc) ((proc)->pdr.fregmask)
#define PROC_REG_OFFSET(proc) ((proc)->pdr.regoffset)
#define PROC_FREG_OFFSET(proc) ((proc)->pdr.fregoffset)
#define PROC_PC_REG(proc) ((proc)->pdr.pcreg)
#define PROC_LOCALOFF(proc) ((proc)->pdr.localoff)
#define PROC_SYMBOL(proc) (*(struct symbol**)&(proc)->pdr.isym)
#define _PROC_MAGIC_ 0x0F0F0F0F
#define PROC_DESC_IS_DUMMY(proc) ((proc)->pdr.isym == _PROC_MAGIC_)
#define SET_PROC_DESC_IS_DUMMY(proc) ((proc)->pdr.isym = _PROC_MAGIC_)

struct linked_proc_info
{
  struct alpha_extra_func_info info;
  struct linked_proc_info *next;
} *linked_proc_desc_table = NULL;


#define READ_FRAME_REG(fi, regno) read_next_frame_reg((fi)->next, regno)

static CORE_ADDR
read_next_frame_reg(fi, regno)
     FRAME fi;
     int regno;
{
  /* If it is the frame for sigtramp we have a pointer to the sigcontext
     on the stack.
     If the stack layout for __sigtramp changes or if sigcontext offsets
     change we might have to update this code.  */
#ifndef SIGFRAME_PC_OFF
#define SIGFRAME_PC_OFF		(2 * 8)
#define SIGFRAME_REGSAVE_OFF	(4 * 8)
#endif
  for (; fi; fi = fi->next)
    {
      if (fi->signal_handler_caller)
	{
	  int offset;
	  CORE_ADDR sigcontext_addr = read_memory_integer(fi->frame, 8);

	  if (regno == PC_REGNUM)
	    offset = SIGFRAME_PC_OFF;
	  else if (regno < 32)
	    offset = SIGFRAME_REGSAVE_OFF + regno * 8;
	  else
	    return 0;
	  return read_memory_integer(sigcontext_addr + offset, 8);
        }
      else if (regno == SP_REGNUM)
	return fi->frame;
      else if (fi->saved_regs->regs[regno])
	return read_memory_integer(fi->saved_regs->regs[regno], 8);
    }
  return read_register(regno);
}

CORE_ADDR
alpha_frame_saved_pc(frame)
     FRAME frame;
{
  alpha_extra_func_info_t proc_desc = frame->proc_desc;
  int pcreg = proc_desc ? PROC_PC_REG(proc_desc) : RA_REGNUM;

  if (proc_desc && PROC_DESC_IS_DUMMY(proc_desc))
      return read_memory_integer(frame->frame - 8, 8);

  return read_next_frame_reg(frame, pcreg);
}

CORE_ADDR
alpha_saved_pc_after_call (frame)
     FRAME frame;
{
  alpha_extra_func_info_t proc_desc = find_proc_desc (frame->pc, frame->next);
  int pcreg = proc_desc ? PROC_PC_REG (proc_desc) : RA_REGNUM;

  return read_register (pcreg);
}


static struct alpha_extra_func_info temp_proc_desc;
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
    for (start_pc -= 4; ; start_pc -= 4)
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
		
		warning("enclosing function for address 0x%lx", pc);
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

    start_pc += 4; /* skip return */
    return start_pc;
}

static alpha_extra_func_info_t
heuristic_proc_desc(start_pc, limit_pc, next_frame)
    CORE_ADDR start_pc, limit_pc;
    FRAME next_frame;
{
    CORE_ADDR sp = next_frame ? next_frame->frame : read_register (SP_REGNUM);
    CORE_ADDR cur_pc;
    int frame_size;
    int has_frame_reg = 0;
    unsigned long reg_mask = 0;

    if (start_pc == 0)
      return NULL;
    memset(&temp_proc_desc, '\0', sizeof(temp_proc_desc));
    memset(&temp_saved_regs, '\0', sizeof(struct frame_saved_regs));
    PROC_LOW_ADDR(&temp_proc_desc) = start_pc;

    if (start_pc + 200 < limit_pc)
      limit_pc = start_pc + 200;
    frame_size = 0;
    for (cur_pc = start_pc; cur_pc < limit_pc; cur_pc += 4)
      {
        char buf[4];
	unsigned long word;
	int status;

	status = read_memory_nobpt (cur_pc, buf, 4); 
	if (status)
	  memory_error (status, cur_pc);
	word = extract_unsigned_integer (buf, 4);

	if ((word & 0xffff0000) == 0x23de0000)		/* lda $sp,n($sp) */
	  frame_size += (-word) & 0xffff;
	else if ((word & 0xfc1f0000) == 0xb41e0000	/* stq reg,n($sp) */
		 && (word & 0xffff0000) != 0xb7fe0000)	/* reg != $zero */
	  {
	    int reg = (word & 0x03e00000) >> 21;
	    reg_mask |= 1 << reg;
	    temp_saved_regs.regs[reg] = sp + (short)word;
	  }
	else if (word == 0x47de040f)			/* bis sp,sp fp */
	  has_frame_reg = 1;
      }
    if (has_frame_reg)
      PROC_FRAME_REG(&temp_proc_desc) = GCC_FP_REGNUM;
    else
      PROC_FRAME_REG(&temp_proc_desc) = SP_REGNUM;
    PROC_FRAME_OFFSET(&temp_proc_desc) = frame_size;
    PROC_REG_MASK(&temp_proc_desc) = reg_mask;
    PROC_PC_REG(&temp_proc_desc) = RA_REGNUM;
    return &temp_proc_desc;
}

static alpha_extra_func_info_t
find_proc_desc(pc, next_frame)
    CORE_ADDR pc;
    FRAME next_frame;
{
  alpha_extra_func_info_t proc_desc;
  struct block *b;
  struct symbol *sym;
  CORE_ADDR startaddr;

  /* Try to get the proc_desc from the linked call dummy proc_descs
     if the pc is in the call dummy.
     This is hairy. In the case of nested dummy calls we have to find the
     right proc_desc, but we might not yet know the frame for the dummy
     as it will be contained in the proc_desc we are searching for.
     So we have to find the proc_desc whose frame is closest to the current
     stack pointer.  */
  if (PC_IN_CALL_DUMMY (pc, 0, 0))
    {
      struct linked_proc_info *link;
      CORE_ADDR sp = next_frame ? next_frame->frame : read_register (SP_REGNUM);
      alpha_extra_func_info_t found_proc_desc = NULL;
      long min_distance = LONG_MAX;

      for (link = linked_proc_desc_table; link; link = link->next)
	{
	  long distance = (CORE_ADDR) PROC_DUMMY_FRAME (&link->info) - sp;
	  if (distance > 0 && distance < min_distance)
	    {
	      min_distance = distance;
	      found_proc_desc = &link->info;
	    }
	}
      if (found_proc_desc != NULL)
	return found_proc_desc;
    }

  b = block_for_pc(pc);
  find_pc_partial_function (pc, NULL, &startaddr, NULL);
  if (b == NULL)
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

  if (sym)
    {
	/* IF (this is the topmost frame OR a frame interrupted by a signal)
	 * AND (this proc does not have debugging information OR
	 * the PC is in the procedure prologue)
	 * THEN create a "heuristic" proc_desc (by analyzing
	 * the actual code) to replace the "official" proc_desc.
	 */
	proc_desc = (alpha_extra_func_info_t)SYMBOL_VALUE(sym);
	if (next_frame == NULL || next_frame->signal_handler_caller) {
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
		alpha_extra_func_info_t found_heuristic =
		    heuristic_proc_desc(PROC_LOW_ADDR(proc_desc),
					pc, next_frame);
		if (found_heuristic)
		  {
		    /* The call to heuristic_proc_desc determines
		       which registers have been saved so far and if the
		       frame is already set up.
		       The heuristic algorithm doesn't work well for other
		       information in the procedure descriptor, so copy
		       it from the found procedure descriptor.  */
		    PROC_LOCALOFF(found_heuristic) = PROC_LOCALOFF(proc_desc);
		    PROC_PC_REG(found_heuristic) = PROC_PC_REG(proc_desc);
		    proc_desc = found_heuristic;
		  }
	    }
	}
    }
  else
    {
      if (startaddr == 0)
	startaddr = heuristic_proc_start (pc);

      proc_desc =
	heuristic_proc_desc (startaddr, pc, next_frame);
    }
  return proc_desc;
}

alpha_extra_func_info_t cached_proc_desc;

FRAME_ADDR
alpha_frame_chain(frame)
    FRAME frame;
{
    alpha_extra_func_info_t proc_desc;
    CORE_ADDR saved_pc = FRAME_SAVED_PC(frame);

    if (saved_pc == 0 || inside_entry_file (saved_pc))
      return 0;

    proc_desc = find_proc_desc(saved_pc, frame);
    if (!proc_desc)
      return 0;

    cached_proc_desc = proc_desc;

    /* Fetch the frame pointer for a dummy frame from the procedure
       descriptor.  */
    if (PROC_DESC_IS_DUMMY(proc_desc))
      return (FRAME_ADDR) PROC_DUMMY_FRAME(proc_desc);

    /* If no frame pointer and frame size is zero, we must be at end
       of stack (or otherwise hosed).  If we don't check frame size,
       we loop forever if we see a zero size frame.  */
    if (PROC_FRAME_REG (proc_desc) == SP_REGNUM
	&& PROC_FRAME_OFFSET (proc_desc) == 0
	/* The alpha __sigtramp routine is frameless and has a frame size
	   of zero. Luckily it is the only procedure which has PC_REGNUM
	   as PROC_PC_REG.  */
	&& PROC_PC_REG (proc_desc) != PC_REGNUM
	/* The previous frame from a sigtramp frame might be frameless
	   and have frame size zero.  */
	&& !frame->signal_handler_caller)
      return 0;
    else
      return read_next_frame_reg(frame, PROC_FRAME_REG(proc_desc))
	+ PROC_FRAME_OFFSET(proc_desc);
}

void
init_extra_frame_info(fci)
     struct frame_info *fci;
{
  extern struct obstack frame_cache_obstack;
  /* Use proc_desc calculated in frame_chain */
  alpha_extra_func_info_t proc_desc =
    fci->next ? cached_proc_desc : find_proc_desc(fci->pc, fci->next);

  fci->saved_regs = (struct frame_saved_regs*)
    obstack_alloc (&frame_cache_obstack, sizeof(struct frame_saved_regs));
  memset (fci->saved_regs, 0, sizeof (struct frame_saved_regs));
  fci->proc_desc =
    proc_desc == &temp_proc_desc ? 0 : proc_desc;
  if (proc_desc)
    {
      int ireg;
      CORE_ADDR reg_position;
      unsigned long mask;
      int returnreg;

      /* Get the locals offset from the procedure descriptor, it is valid
	 even if we are in the middle of the prologue.  */
      fci->localoff = PROC_LOCALOFF(proc_desc);

      /* Fixup frame-pointer - only needed for top frame */
      /* Fetch the frame pointer for a dummy frame from the procedure
	 descriptor.  */
      if (PROC_DESC_IS_DUMMY(proc_desc))
	fci->frame = (FRAME_ADDR) PROC_DUMMY_FRAME(proc_desc);
      /* This may not be quite right, if proc has a real frame register.
	 Get the value of the frame relative sp, procedure might have been
	 interrupted by a signal at it's very start.  */
      else if (fci->pc == PROC_LOW_ADDR(proc_desc))
	fci->frame = READ_FRAME_REG(fci, SP_REGNUM);
      else
	fci->frame = READ_FRAME_REG(fci, PROC_FRAME_REG(proc_desc))
		      + PROC_FRAME_OFFSET(proc_desc);

      /* If this is the innermost frame, and we are still in the
	 prologue (loosely defined), then the registers may not have
	 been saved yet.  */
      if (fci->next == NULL
          && !PROC_DESC_IS_DUMMY(proc_desc)
	  && alpha_in_lenient_prologue (PROC_LOW_ADDR (proc_desc), fci->pc))
	{
	  /* Can't just say that the registers are not saved, because they
	     might get clobbered halfway through the prologue.
	     heuristic_proc_desc already has the right code to figure out
	     exactly what has been saved, so use it.  As far as I know we
	     could be doing this (as we do on the 68k, for example)
	     regardless of whether we are in the prologue; I'm leaving in
	     the check for being in the prologue only out of conservatism
	     (I'm not sure whether heuristic_proc_desc handles all cases,
	     for example).

	     This stuff is ugly (and getting uglier by the minute).  Probably
	     the best way to clean it up is to ignore the proc_desc's from
	     the symbols altogher, and get all the information we need by
	     examining the prologue (provided we can make the prologue
	     examining code good enough to get all the cases...).  */
	  proc_desc =
	    heuristic_proc_desc (PROC_LOW_ADDR (proc_desc),
				 fci->pc,
				 fci->next);
	}

      if (proc_desc == &temp_proc_desc)
	*fci->saved_regs = temp_saved_regs;
      else
	{
	  /* Find which general-purpose registers were saved.
	     The return address register is the first saved register,
	     the other registers follow in ascending order.  */
	  reg_position = fci->frame + PROC_REG_OFFSET(proc_desc);
	  mask = PROC_REG_MASK(proc_desc) & 0xffffffffL;
	  returnreg = PROC_PC_REG(proc_desc);
	  if (mask & (1 << returnreg))
	    {
	      fci->saved_regs->regs[returnreg] = reg_position;
	      reg_position += 8;
	    }
	  for (ireg = 0; mask; ireg++, mask >>= 1)
	    if (mask & 1)
	      {
		if (ireg == returnreg)
		  continue;
		fci->saved_regs->regs[ireg] = reg_position;
		reg_position += 8;
	      }
	  /* find which floating-point registers were saved */
	  reg_position = fci->frame + PROC_FREG_OFFSET(proc_desc);
	  mask = PROC_FREG_MASK(proc_desc) & 0xffffffffL;
	  for (ireg = 0; mask; ireg++, mask >>= 1)
	    if (mask & 1)
	      {
		fci->saved_regs->regs[FP0_REGNUM+ireg] = reg_position;
		reg_position += 8;
	      }
	}

      fci->saved_regs->regs[PC_REGNUM] = fci->saved_regs->regs[PROC_PC_REG(proc_desc)];
    }
}

/* ALPHA stack frames are almost impenetrable.  When execution stops,
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

FRAME
setup_arbitrary_frame (argc, argv)
     int argc;
     FRAME_ADDR *argv;
{
  if (argc != 2)
    error ("ALPHA frame specifications require two arguments: sp and pc");

  return create_new_frame (argv[0], argv[1]);
}

/* The alpha passes the first six arguments in the registers, the rest on
   the stack. The register arguments are eventually transferred to the
   argument transfer area immediately below the stack by the called function
   anyway. So we `push' at least six arguments on the stack, `reload' the
   argument registers and then adjust the stack pointer to point past the
   sixth argument. This algorithm simplifies the passing of a large struct
   which extends from the registers to the stack.
   If the called function is returning a structure, the address of the
   structure to be returned is passed as a hidden first argument.  */

CORE_ADDR
alpha_push_arguments (nargs, args, sp, struct_return, struct_addr)
  int nargs;
  value *args;
  CORE_ADDR sp;
  int struct_return;
  CORE_ADDR struct_addr;
{
  register i;
  int accumulate_size = struct_return ? 8 : 0;
  int arg_regs_size = ALPHA_NUM_ARG_REGS * 8;
  struct alpha_arg { char *contents; int len; int offset; };
  struct alpha_arg *alpha_args =
      (struct alpha_arg*)alloca (nargs * sizeof (struct alpha_arg));
  register struct alpha_arg *m_arg;
  char raw_buffer[sizeof (CORE_ADDR)];
  int required_arg_regs;

  for (i = 0, m_arg = alpha_args; i < nargs; i++, m_arg++)
    {
      value arg = value_arg_coerce (args[i]);
      /* Cast argument to long if necessary as the compiler does it too.  */
      if (TYPE_LENGTH (VALUE_TYPE (arg)) < TYPE_LENGTH (builtin_type_long))
        arg = value_cast (builtin_type_long, arg);
      m_arg->len = TYPE_LENGTH (VALUE_TYPE (arg));
      m_arg->offset = accumulate_size;
      accumulate_size = (accumulate_size + m_arg->len + 7) & ~7;
      m_arg->contents = VALUE_CONTENTS(arg);
    }

  /* Determine required argument register loads, loading an argument register
     is expensive as it uses three ptrace calls.  */
  required_arg_regs = accumulate_size / 8;
  if (required_arg_regs > ALPHA_NUM_ARG_REGS)
    required_arg_regs = ALPHA_NUM_ARG_REGS;

  /* Make room for the arguments on the stack.  */
  if (accumulate_size < arg_regs_size)
    accumulate_size = arg_regs_size; 
  sp -= accumulate_size;

  /* Keep sp aligned to a multiple of 16 as the compiler does it too.  */
  sp &= ~15;

  /* `Push' arguments on the stack.  */
  for (i = nargs; m_arg--, --i >= 0; )
    write_memory(sp + m_arg->offset, m_arg->contents, m_arg->len);
  if (struct_return)
    {
      store_address (raw_buffer, sizeof (CORE_ADDR), struct_addr);
      write_memory (sp, raw_buffer, sizeof (CORE_ADDR));
    }

  /* Load the argument registers.  */
  for (i = 0; i < required_arg_regs; i++)
    {
      LONGEST val;

      val = read_memory_integer (sp + i * 8, 8);
      write_register (A0_REGNUM + i, val);
      write_register (FPA0_REGNUM + i, val);
    }

  return sp + arg_regs_size;
}

void
alpha_push_dummy_frame()
{
  int ireg;
  struct linked_proc_info *link = (struct linked_proc_info*)
      xmalloc(sizeof (struct linked_proc_info));
  alpha_extra_func_info_t proc_desc = &link->info;
  CORE_ADDR sp = read_register (SP_REGNUM);
  CORE_ADDR save_address;
  char raw_buffer[MAX_REGISTER_RAW_SIZE];
  unsigned long mask;

  link->next = linked_proc_desc_table;
  linked_proc_desc_table = link;

  /*
   * The registers we must save are all those not preserved across
   * procedure calls.
   * In addition, we must save the PC and RA.
   *
   * Dummy frame layout:
   *  (high memory)
   * 	Saved PC
   *    Saved F30
   *    ...
   *    Saved F0
   *	Saved R29
   *	...
   *	Saved R0
   *	Saved R26 (RA)
   *	Parameter build area
   *  (low memory)
   */

/* MASK(i,j) == (1<<i) + (1<<(i+1)) + ... + (1<<j)). Assume i<=j<31. */
#define MASK(i,j) (((1L << ((j)+1)) - 1) ^ ((1L << (i)) - 1))
#define GEN_REG_SAVE_MASK (MASK(0,8) | MASK(16,29))
#define GEN_REG_SAVE_COUNT 24
#define FLOAT_REG_SAVE_MASK (MASK(0,1) | MASK(10,30))
#define FLOAT_REG_SAVE_COUNT 23
  /* The special register is the PC as we have no bit for it in the save masks.
     alpha_frame_saved_pc knows where the pc is saved in a dummy frame.  */
#define SPECIAL_REG_SAVE_COUNT 1

  PROC_REG_MASK(proc_desc) = GEN_REG_SAVE_MASK;
  PROC_FREG_MASK(proc_desc) = FLOAT_REG_SAVE_MASK;
  /* PROC_REG_OFFSET is the offset from the dummy frame to the saved RA,
     but keep SP aligned to a multiple of 16.  */
  PROC_REG_OFFSET(proc_desc) =
    - ((8 * (SPECIAL_REG_SAVE_COUNT
	    + GEN_REG_SAVE_COUNT
	    + FLOAT_REG_SAVE_COUNT)
	+ 15) & ~15);
  PROC_FREG_OFFSET(proc_desc) =
    PROC_REG_OFFSET(proc_desc) + 8 * GEN_REG_SAVE_COUNT;

  /* Save general registers.
     The return address register is the first saved register, all other
     registers follow in ascending order.
     The PC is saved immediately below the SP.  */
  save_address = sp + PROC_REG_OFFSET(proc_desc);
  store_address (raw_buffer, 8, read_register (RA_REGNUM));
  write_memory (save_address, raw_buffer, 8);
  save_address += 8;
  mask = PROC_REG_MASK(proc_desc) & 0xffffffffL;
  for (ireg = 0; mask; ireg++, mask >>= 1)
    if (mask & 1)
      {
	if (ireg == RA_REGNUM)
	  continue;
	store_address (raw_buffer, 8, read_register (ireg));
	write_memory (save_address, raw_buffer, 8);
	save_address += 8;
      }

  store_address (raw_buffer, 8, read_register (PC_REGNUM));
  write_memory (sp - 8, raw_buffer, 8);

  /* Save floating point registers.  */
  save_address = sp + PROC_FREG_OFFSET(proc_desc);
  mask = PROC_FREG_MASK(proc_desc) & 0xffffffffL;
  for (ireg = 0; mask; ireg++, mask >>= 1)
    if (mask & 1)
      {
	store_address (raw_buffer, 8, read_register (ireg + FP0_REGNUM));
	write_memory (save_address, raw_buffer, 8);
	save_address += 8;
      }

  /* Set and save the frame address for the dummy.  
     This is tricky. The only registers that are suitable for a frame save
     are those that are preserved across procedure calls (s0-s6). But if
     a read system call is interrupted and then a dummy call is made
     (see testsuite/gdb.t17/interrupt.exp) the dummy call hangs till the read
     is satisfied. Then it returns with the s0-s6 registers set to the values
     on entry to the read system call and our dummy frame pointer would be
     destroyed. So we save the dummy frame in the proc_desc and handle the
     retrieval of the frame pointer of a dummy specifically. The frame register
     is set to the virtual frame (pseudo) register, it's value will always
     be read as zero and will help us to catch any errors in the dummy frame
     retrieval code.  */
  PROC_DUMMY_FRAME(proc_desc) = sp;
  PROC_FRAME_REG(proc_desc) = FP_REGNUM;
  PROC_FRAME_OFFSET(proc_desc) = 0;
  sp += PROC_REG_OFFSET(proc_desc);
  write_register (SP_REGNUM, sp);

  PROC_LOW_ADDR(proc_desc) = entry_point_address ();
  PROC_HIGH_ADDR(proc_desc) = PROC_LOW_ADDR(proc_desc) + 4;

  SET_PROC_DESC_IS_DUMMY(proc_desc);
  PROC_PC_REG(proc_desc) = RA_REGNUM;
}

void
alpha_pop_frame()
{
  register int regnum;
  FRAME frame = get_current_frame ();
  CORE_ADDR new_sp = frame->frame;

  alpha_extra_func_info_t proc_desc = frame->proc_desc;

  write_register (PC_REGNUM, FRAME_SAVED_PC(frame));
  if (proc_desc)
    {
      for (regnum = 32; --regnum >= 0; )
	if (PROC_REG_MASK(proc_desc) & (1 << regnum))
	  write_register (regnum,
			  read_memory_integer (frame->saved_regs->regs[regnum],
					       8));
      for (regnum = 32; --regnum >= 0; )
	if (PROC_FREG_MASK(proc_desc) & (1 << regnum))
	  write_register (regnum + FP0_REGNUM,
			  read_memory_integer (frame->saved_regs->regs[regnum + FP0_REGNUM], 8));
    }
  write_register (SP_REGNUM, new_sp);
  flush_cached_frames ();
  /* We let init_extra_frame_info figure out the frame pointer */
  set_current_frame (create_new_frame (0, read_pc ()));

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
    }
}

/* To skip prologues, I use this predicate.  Returns either PC itself
   if the code at PC does not look like a function prologue; otherwise
   returns an address that (if we're lucky) follows the prologue.  If
   LENIENT, then we must skip everything which is involved in setting
   up the frame (it's OK to skip more, just so long as we don't skip
   anything which might clobber the registers which are being saved.
   Currently we must not skip more on the alpha, but we might the lenient
   stuff some day.  */

CORE_ADDR
alpha_skip_prologue (pc, lenient)
     CORE_ADDR pc;
     int lenient;
{
    unsigned long inst;
    int offset;

    /* Skip the typical prologue instructions. These are the stack adjustment
       instruction and the instructions that save registers on the stack
       or in the gcc frame.  */
    for (offset = 0; offset < 100; offset += 4)
      {
	char buf[4];
	int status;

	status = read_memory_nobpt (pc + offset, buf, 4);
	if (status)
	  memory_error (status, pc + offset);
	inst = extract_unsigned_integer (buf, 4);

	/* The alpha has no delay slots. But let's keep the lenient stuff,
	   we might need it for something else in the future.  */
	if (lenient && 0)
	  continue;

	if ((inst & 0xffff0000) == 0x27bb0000)	/* ldah $gp,n($t12) */
	    continue;
	if ((inst & 0xffff0000) == 0x23bd0000)	/* lda $gp,n($gp) */
	    continue;
	if ((inst & 0xffff0000) == 0x23de0000)	/* lda $sp,n($sp) */
	    continue;
	else if ((inst & 0xfc1f0000) == 0xb41e0000
		 && (inst & 0xffff0000) != 0xb7fe0000)
	    continue;				/* stq reg,n($sp) */
						/* reg != $zero */
	else if ((inst & 0xfc1f0000) == 0x9c1e0000
		 && (inst & 0xffff0000) != 0x9ffe0000)
	    continue;				/* stt reg,n($sp) */
						/* reg != $zero */
	else if (inst == 0x47de040f)		/* bis sp,sp,fp */
	    continue;
	else
	    break;
    }
    return pc + offset;
}

/* Is address PC in the prologue (loosely defined) for function at
   STARTADDR?  */

static int
alpha_in_lenient_prologue (startaddr, pc)
     CORE_ADDR startaddr;
     CORE_ADDR pc;
{
  CORE_ADDR end_prologue = alpha_skip_prologue (startaddr, 1);
  return pc >= startaddr && pc < end_prologue;
}

/* Given a return value in `regbuf' with a type `valtype', 
   extract and copy its value into `valbuf'.  */
void
alpha_extract_return_value (valtype, regbuf, valbuf)
    struct type *valtype;
    char regbuf[REGISTER_BYTES];
    char *valbuf;
{
  int regnum;
  
  regnum = TYPE_CODE (valtype) == TYPE_CODE_FLT ? FP0_REGNUM : V0_REGNUM;

  memcpy (valbuf, regbuf + REGISTER_BYTE (regnum), TYPE_LENGTH (valtype));
}

/* Given a return value in `regbuf' with a type `valtype', 
   write it's value into the appropriate register.  */
void
alpha_store_return_value (valtype, valbuf)
    struct type *valtype;
    char *valbuf;
{
  int regnum;
  char raw_buffer[MAX_REGISTER_RAW_SIZE];
  
  regnum = TYPE_CODE (valtype) == TYPE_CODE_FLT ? FP0_REGNUM : V0_REGNUM;
  memcpy(raw_buffer, valbuf, TYPE_LENGTH (valtype));

  write_register_bytes(REGISTER_BYTE (regnum), raw_buffer, TYPE_LENGTH (valtype));
}

/* Print the instruction at address MEMADDR in debugged memory,
   on STREAM.  Returns length of the instruction, in bytes.  */

int
print_insn (memaddr, stream)
     CORE_ADDR memaddr;
     FILE *stream;
{
  disassemble_info info;

  GDB_INIT_DISASSEMBLE_INFO(info, stream);

  return print_insn_alpha (memaddr, &info);
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

void
_initialize_alpha_tdep ()
{
  struct cmd_list_element *c;

  /* Let the user set the fence post for heuristic_proc_start.  */

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
