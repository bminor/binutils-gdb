/* Target-dependent code for the MIPS architecture, for GDB, the GNU Debugger.
   Copyright 1988, 1989, 1990, 1991, 1992, 1993, 1994
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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
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

#include "opcode/mips.h"

#define VM_MIN_ADDRESS (unsigned)0x400000

/* FIXME: Put this declaration in frame.h.  */
extern struct obstack frame_cache_obstack;

#if 0
static int mips_in_lenient_prologue PARAMS ((CORE_ADDR, CORE_ADDR));
#endif

/* Some MIPS boards don't support floating point, so we permit the
   user to turn it off.  */
int mips_fpu = 1;

/* Heuristic_proc_start may hunt through the text section for a long
   time across a 2400 baud serial line.  Allows the user to limit this
   search.  */
static unsigned int heuristic_fence_post = 0;

#define PROC_LOW_ADDR(proc) ((proc)->pdr.adr) /* least address */
#define PROC_HIGH_ADDR(proc) ((proc)->pdr.iline) /* upper address bound */
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


/* Guaranteed to set fci->saved_regs to some values (it never leaves it
   NULL).  */

void
mips_find_saved_regs (fci)
     FRAME fci;
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
      char buf[4];
      unsigned long inst;

      /* Bitmasks; set if we have found a save for the register.  */
      unsigned long gen_save_found = 0;
      unsigned long float_save_found = 0;

      for (addr = PROC_LOW_ADDR (proc_desc);
	   addr < fci->pc /*&& (gen_mask != gen_save_found
			      || float_mask != float_save_found)*/;
	   addr += 4)
	{
	  status = read_memory_nobpt (addr, buf, 4);
	  if (status)
	    memory_error (status, addr);
	  inst = extract_unsigned_integer (buf, 4);
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
  for (ireg= 31; gen_mask; --ireg, gen_mask <<= 1)
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
  reg_position += 4;
  for (ireg = 31; float_mask; --ireg, float_mask <<= 1)
    if (float_mask & 0x80000000)
      {
	fci->saved_regs->regs[FP0_REGNUM+ireg] = reg_position;
	reg_position -= MIPS_REGSIZE;
      }

  fci->saved_regs->regs[PC_REGNUM] = fci->saved_regs->regs[RA_REGNUM];
}

static int
read_next_frame_reg(fi, regno)
     FRAME fi;
     int regno;
{
  /* If it is the frame for sigtramp we have a complete sigcontext
     somewhere above the frame and we get the saved registers from there.
     If the stack layout for sigtramp changes we might have to change these
     constants and the companion fixup_sigtramp in mdebugread.c  */
#ifndef SIGFRAME_BASE
/* To satisfy alignment restrictions the sigcontext is located 4 bytes
   above the sigtramp frame.  */
#define SIGFRAME_BASE		4
#define SIGFRAME_PC_OFF		(SIGFRAME_BASE + 2 * 4)
#define SIGFRAME_REGSAVE_OFF	(SIGFRAME_BASE + 3 * 4)
#endif
#ifndef SIGFRAME_REG_SIZE
#define SIGFRAME_REG_SIZE	4
#endif
  for (; fi; fi = fi->next)
    {
      if (fi->signal_handler_caller)
	{
	  int offset;
	  if (regno == PC_REGNUM) offset = SIGFRAME_PC_OFF;
	  else if (regno < 32) offset = (SIGFRAME_REGSAVE_OFF
					 + regno * SIGFRAME_REG_SIZE);
	  else return 0;
	  return read_memory_integer(fi->frame + offset, MIPS_REGSIZE);
	}
      else if (regno == SP_REGNUM) return fi->frame;
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

int
mips_frame_saved_pc(frame)
     FRAME frame;
{
  mips_extra_func_info_t proc_desc = frame->proc_desc;
  /* We have to get the saved pc from the sigcontext
     if it is a signal handler frame.  */
  int pcreg = frame->signal_handler_caller ? PC_REGNUM
	      : (proc_desc ? PROC_PC_REG(proc_desc) : RA_REGNUM);

  if (proc_desc && PROC_DESC_IS_DUMMY(proc_desc))
      return read_memory_integer(frame->frame - 4, 4);

  return read_next_frame_reg(frame, pcreg);
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
		
		warning("enclosing function for address 0x%x", pc);
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

    start_pc += 8; /* skip return, and its delay slot */
#if 0
    /* skip nops (usually 1) 0 - is this */
    while (start_pc < pc && read_memory_integer (start_pc, 4) == 0)
	start_pc += 4;
#endif
    return start_pc;
}

static mips_extra_func_info_t
heuristic_proc_desc(start_pc, limit_pc, next_frame)
    CORE_ADDR start_pc, limit_pc;
    FRAME next_frame;
{
    CORE_ADDR sp = read_next_frame_reg (next_frame, SP_REGNUM);
    CORE_ADDR cur_pc;
    int frame_size;
    int has_frame_reg = 0;
    int reg30 = 0; /* Value of $r30. Used by gcc for frame-pointer */
    unsigned long reg_mask = 0;

    if (start_pc == 0) return NULL;
    memset(&temp_proc_desc, '\0', sizeof(temp_proc_desc));
    memset(&temp_saved_regs, '\0', sizeof(struct frame_saved_regs));
    PROC_LOW_ADDR(&temp_proc_desc) = start_pc;

    if (start_pc + 200 < limit_pc) limit_pc = start_pc + 200;
  restart:
    frame_size = 0;
    for (cur_pc = start_pc; cur_pc < limit_pc; cur_pc += 4) {
        char buf[4];
	unsigned long word;
	int status;

	status = read_memory_nobpt (cur_pc, buf, 4); 
	if (status) memory_error (status, cur_pc);
	word = extract_unsigned_integer (buf, 4);

	if ((word & 0xFFFF0000) == 0x27bd0000) /* addiu $sp,$sp,-i */
	    frame_size += (-word) & 0xFFFF;
	else if ((word & 0xFFFF0000) == 0x23bd0000) /* addu $sp,$sp,-i */
	    frame_size += (-word) & 0xFFFF;
	else if ((word & 0xFFE00000) == 0xafa00000) { /* sw reg,offset($sp) */
	    int reg = (word & 0x001F0000) >> 16;
	    reg_mask |= 1 << reg;
	    temp_saved_regs.regs[reg] = sp + (word & 0xffff);
	}
	else if ((word & 0xFFFF0000) == 0x27be0000) { /* addiu $30,$sp,size */
	    if ((word & 0xffff) != frame_size)
		reg30 = sp + (word & 0xffff);
	    else if (!has_frame_reg) {
		int alloca_adjust;
		has_frame_reg = 1;
		reg30 = read_next_frame_reg(next_frame, 30);
		alloca_adjust = reg30 - (sp + (word & 0xffff));
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
	PROC_FRAME_OFFSET(&temp_proc_desc) = 0;
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
find_proc_desc(pc, next_frame)
    CORE_ADDR pc;
    FRAME next_frame;
{
  mips_extra_func_info_t proc_desc;
  struct block *b = block_for_pc(pc);
  struct symbol *sym;
  CORE_ADDR startaddr;

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
	/* IF this is the topmost frame AND
	 * (this proc does not have debugging information OR
	 * the PC is in the procedure prologue)
	 * THEN create a "heuristic" proc_desc (by analyzing
	 * the actual code) to replace the "official" proc_desc.
	 */
	proc_desc = (mips_extra_func_info_t)SYMBOL_VALUE(sym);
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
		    heuristic_proc_desc(PROC_LOW_ADDR(proc_desc),
					pc, next_frame);
		if (found_heuristic) proc_desc = found_heuristic;
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

mips_extra_func_info_t cached_proc_desc;

FRAME_ADDR
mips_frame_chain(frame)
    FRAME frame;
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
      return read_next_frame_reg(frame, PROC_FRAME_REG(proc_desc))
	+ PROC_FRAME_OFFSET(proc_desc);
}

void
init_extra_frame_info(fci)
     struct frame_info *fci;
{
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
	fci->frame =
	  read_next_frame_reg (fci->next, PROC_FRAME_REG (proc_desc))
	    + PROC_FRAME_OFFSET (proc_desc);

      if (proc_desc == &temp_proc_desc)
	{
	  fci->saved_regs = (struct frame_saved_regs*)
	    obstack_alloc (&frame_cache_obstack,
			   sizeof (struct frame_saved_regs));
	  *fci->saved_regs = temp_saved_regs;
	  fci->saved_regs->regs[PC_REGNUM] = fci->saved_regs->regs[RA_REGNUM];
	}

      /* hack: if argument regs are saved, guess these contain args */
      if ((PROC_REG_MASK(proc_desc) & 0xF0) == 0) fci->num_args = -1;
      else if ((PROC_REG_MASK(proc_desc) & 0x80) == 0) fci->num_args = 4;
      else if ((PROC_REG_MASK(proc_desc) & 0x40) == 0) fci->num_args = 3;
      else if ((PROC_REG_MASK(proc_desc) & 0x20) == 0) fci->num_args = 2;
      else if ((PROC_REG_MASK(proc_desc) & 0x10) == 0) fci->num_args = 1;
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

FRAME
setup_arbitrary_frame (argc, argv)
     int argc;
     FRAME_ADDR *argv;
{
  if (argc != 2)
    error ("MIPS frame specifications require two arguments: sp and pc");

  return create_new_frame (argv[0], argv[1]);
}


CORE_ADDR
mips_push_arguments(nargs, args, sp, struct_return, struct_addr)
  int nargs;
  value_ptr *args;
  CORE_ADDR sp;
  int struct_return;
  CORE_ADDR struct_addr;
{
  register i;
  int accumulate_size = struct_return ? MIPS_REGSIZE : 0;
  struct mips_arg { char *contents; int len; int offset; };
  struct mips_arg *mips_args =
      (struct mips_arg*)alloca((nargs + 4) * sizeof(struct mips_arg));
  register struct mips_arg *m_arg;
  int fake_args = 0;

  for (i = 0, m_arg = mips_args; i < nargs; i++, m_arg++) {
    value_ptr arg = value_arg_coerce (args[i]);
    m_arg->len = TYPE_LENGTH (VALUE_TYPE (arg));
    /* This entire mips-specific routine is because doubles must be aligned
     * on 8-byte boundaries. It still isn't quite right, because MIPS decided
     * to align 'struct {int a, b}' on 4-byte boundaries (even though this
     * breaks their varargs implementation...). A correct solution
     * requires an simulation of gcc's 'alignof' (and use of 'alignof'
     * in stdarg.h/varargs.h).
     * On the 64 bit r4000 we always pass the first four arguments
     * using eight bytes each, so that we can load them up correctly
     * in CALL_DUMMY.
     */
    if (m_arg->len > 4)
      accumulate_size = (accumulate_size + 7) & -8;
    m_arg->offset = accumulate_size;
    m_arg->contents = VALUE_CONTENTS(arg);
#ifndef GDB_TARGET_IS_MIPS64
    accumulate_size = (accumulate_size + m_arg->len + 3) & -4;
#else
    if (accumulate_size >= 4 * MIPS_REGSIZE)
      accumulate_size = (accumulate_size + m_arg->len + 3) &~ 4;
    else
      {
	static char zeroes[8] = { 0 };
	int len = m_arg->len;

	if (len < 8)
	  {
#if TARGET_BYTE_ORDER == BIG_ENDIAN
	    m_arg->offset += 8 - len;
#endif
	    ++m_arg;
	    m_arg->len = 8 - len;
	    m_arg->contents = zeroes;
#if TARGET_BYTE_ORDER == BIG_ENDIAN
	    m_arg->offset = accumulate_size;
#else
	    m_arg->offset = accumulate_size + len;
#endif
	    ++fake_args;
	  }
	accumulate_size = (accumulate_size + len + 7) & ~8;
      }
#endif
  }
  accumulate_size = (accumulate_size + 7) & (-8);
  if (accumulate_size < 4 * MIPS_REGSIZE)
    accumulate_size = 4 * MIPS_REGSIZE;
  sp -= accumulate_size;
  for (i = nargs + fake_args; m_arg--, --i >= 0; )
    write_memory(sp + m_arg->offset, m_arg->contents, m_arg->len);
  if (struct_return)
    {
      char buf[TARGET_PTR_BIT / HOST_CHAR_BIT];

      store_address (buf, sizeof buf, struct_addr);
      write_memory (sp, buf, sizeof buf);
    }
  return sp;
}

/* MASK(i,j) == (1<<i) + (1<<(i+1)) + ... + (1<<j)). Assume i<=j<31. */
#define MASK(i,j) (((1 << ((j)+1))-1) ^ ((1 << (i))-1))

void
mips_push_dummy_frame()
{
  char buffer[MAX_REGISTER_RAW_SIZE];
  int ireg;
  struct linked_proc_info *link = (struct linked_proc_info*)
      xmalloc(sizeof(struct linked_proc_info));
  mips_extra_func_info_t proc_desc = &link->info;
  CORE_ADDR sp = read_register (SP_REGNUM);
  CORE_ADDR save_address;
  link->next = linked_proc_desc_table;
  linked_proc_desc_table = link;
#define PUSH_FP_REGNUM 16 /* must be a register preserved across calls */
#define GEN_REG_SAVE_MASK MASK(1,16)|MASK(24,28)|(1<<31)
#define GEN_REG_SAVE_COUNT 22
#define FLOAT_REG_SAVE_MASK MASK(0,19)
#define FLOAT_REG_SAVE_COUNT 20
#define SPECIAL_REG_SAVE_COUNT 4
  /*
   * The registers we must save are all those not preserved across
   * procedure calls. Dest_Reg (see tm-mips.h) must also be saved.
   * In addition, we must save the PC, and PUSH_FP_REGNUM.
   * (Ideally, we should also save MDLO/-HI and FP Control/Status reg.)
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
  PROC_REG_MASK(proc_desc) = GEN_REG_SAVE_MASK;
  PROC_FREG_MASK(proc_desc) = mips_fpu ? FLOAT_REG_SAVE_MASK : 0;
  PROC_REG_OFFSET(proc_desc) = /* offset of (Saved R31) from FP */
      -sizeof(long) - 4 * SPECIAL_REG_SAVE_COUNT;
  PROC_FREG_OFFSET(proc_desc) = /* offset of (Saved D18) from FP */
      -sizeof(double) - 4 * (SPECIAL_REG_SAVE_COUNT + GEN_REG_SAVE_COUNT);
  /* save general registers */
  save_address = sp + PROC_REG_OFFSET(proc_desc);
  for (ireg = 32; --ireg >= 0; )
    if (PROC_REG_MASK(proc_desc) & (1 << ireg))
      {
	read_register_gen (ireg, buffer);

	/* Need to fix the save_address decrement below, and also make sure
	   that we don't run into problems with the size of the dummy frame
	   or any of the offsets within it.  */
	if (REGISTER_RAW_SIZE (ireg) > 4)
	  error ("Cannot call functions on mips64");

	write_memory (save_address, buffer, REGISTER_RAW_SIZE (ireg));
	save_address -= 4;
      }
  /* save floating-points registers starting with high order word */
  save_address = sp + PROC_FREG_OFFSET(proc_desc) + 4;
  for (ireg = 32; --ireg >= 0; )
    if (PROC_FREG_MASK(proc_desc) & (1 << ireg))
      {
	read_register_gen (ireg + FP0_REGNUM, buffer);

	if (REGISTER_RAW_SIZE (ireg + FP0_REGNUM) > 4)
	  error ("Cannot call functions on mips64");

	write_memory (save_address, buffer,
		      REGISTER_RAW_SIZE (ireg + FP0_REGNUM));
	save_address -= 4;
      }
  write_register (PUSH_FP_REGNUM, sp);
  PROC_FRAME_REG(proc_desc) = PUSH_FP_REGNUM;
  PROC_FRAME_OFFSET(proc_desc) = 0;
  read_register_gen (PC_REGNUM, buffer);
  write_memory (sp - 4, buffer, REGISTER_RAW_SIZE (PC_REGNUM));
  read_register_gen (HI_REGNUM, buffer);
  write_memory (sp - 8, buffer, REGISTER_RAW_SIZE (HI_REGNUM));
  read_register_gen (LO_REGNUM, buffer);
  write_memory (sp - 12, buffer, REGISTER_RAW_SIZE (LO_REGNUM));
  if (mips_fpu)
    read_register_gen (FCRCS_REGNUM, buffer);
  else
    memset (buffer, 0, REGISTER_RAW_SIZE (FCRCS_REGNUM));
  write_memory (sp - 16, buffer, REGISTER_RAW_SIZE (FCRCS_REGNUM));
  sp -= 4 * (GEN_REG_SAVE_COUNT
	     + (mips_fpu ? FLOAT_REG_SAVE_COUNT : 0)
	     + SPECIAL_REG_SAVE_COUNT);
  write_register (SP_REGNUM, sp);
  PROC_LOW_ADDR(proc_desc) = sp - CALL_DUMMY_SIZE + CALL_DUMMY_START_OFFSET;
  PROC_HIGH_ADDR(proc_desc) = sp;
  SET_PROC_DESC_IS_DUMMY(proc_desc);
  PROC_PC_REG(proc_desc) = RA_REGNUM;
}

void
mips_pop_frame()
{
  register int regnum;
  FRAME frame = get_current_frame ();
  CORE_ADDR new_sp = frame->frame;

  mips_extra_func_info_t proc_desc = frame->proc_desc;

  write_register (PC_REGNUM, FRAME_SAVED_PC(frame));
  if (frame->saved_regs == NULL)
    mips_find_saved_regs (frame);
  if (proc_desc)
    {
      for (regnum = 32; --regnum >= 0; )
	if (PROC_REG_MASK(proc_desc) & (1 << regnum))
	  write_register (regnum,
			  read_memory_integer (frame->saved_regs->regs[regnum],
					       4));
      for (regnum = 32; --regnum >= 0; )
	if (PROC_FREG_MASK(proc_desc) & (1 << regnum))
	  write_register (regnum + FP0_REGNUM,
			  read_memory_integer (frame->saved_regs->regs[regnum + FP0_REGNUM], 4));
    }
  write_register (SP_REGNUM, new_sp);
  flush_cached_frames ();
  /* We let mips_init_extra_frame_info figure out the frame pointer */
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

      write_register (HI_REGNUM, read_memory_integer(new_sp - 8, 4));
      write_register (LO_REGNUM, read_memory_integer(new_sp - 12, 4));
      if (mips_fpu)
	write_register (FCRCS_REGNUM, read_memory_integer(new_sp - 16, 4));
    }
}

static void
mips_print_register (regnum, all)
     int regnum, all;
{
  unsigned char raw_buffer[MAX_REGISTER_RAW_SIZE];
  struct type *our_type =
    init_type (TYPE_CODE_INT,
	       /* We will fill in the length for each register.  */
	       0,
	       TYPE_FLAG_UNSIGNED,
	       NULL,
	       NULL);

  /* Get the data in raw format.  */
  if (read_relative_register_raw_bytes (regnum, raw_buffer))
    {
      printf_filtered ("%s: [Invalid]", reg_names[regnum]);
      return;
    }

  /* If an even floating pointer register, also print as double. */
  if (regnum >= FP0_REGNUM && regnum < FP0_REGNUM+32
      && !((regnum-FP0_REGNUM) & 1)) {
    char dbuffer[MAX_REGISTER_RAW_SIZE]; 

    read_relative_register_raw_bytes (regnum, dbuffer);
    read_relative_register_raw_bytes (regnum+1, dbuffer+4);
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
  if (regnum < 32)
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
  if (regnum != -1) {
      mips_print_register (regnum, 0);
      printf_filtered ("\n");
  }
  else {
      for (regnum = 0; regnum < NUM_REGS; ) {
	  if ((!fpregs) && regnum >= FP0_REGNUM && regnum <= FCRIR_REGNUM) {
	    regnum++;
	    continue;
	  }
	  mips_print_register (regnum, 1);
	  regnum++;
	  if ((regnum & 3) == 0 || regnum == NUM_REGS)
	      printf_filtered (";\n");
	  else
	      printf_filtered ("; ");
      }
  }
}
/* Return number of args passed to a frame. described by FIP.
   Can return -1, meaning no way to tell.  */

int
mips_frame_num_args(fip)
	FRAME fip;
{
#if 0
	struct chain_info_t *p;

	p = mips_find_cached_frame(FRAME_FP(fip));
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
  char buf[4];

  if (target_read_memory (pc, buf, 4) != 0)
    /* If error reading memory, guess that it is not a delayed branch.  */
    return 0;
  return is_delayed (extract_unsigned_integer (buf, 4));
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
    unsigned long inst;
    int offset;
    int seen_sp_adjust = 0;
    int load_immediate_bytes = 0;

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

#if 0
	if (lenient && is_delayed (inst))
	  continue;
#endif

	if ((inst & 0xffff0000) == 0x27bd0000)	/* addiu $sp,$sp,offset */
	    seen_sp_adjust = 1;
	else if (inst == 0x03a1e823 || 	        /* subu $sp,$sp,$at */
		 inst == 0x03a8e823)   	        /* subu $sp,$sp,$t0 */
	    seen_sp_adjust = 1;
	else if ((inst & 0xFFE00000) == 0xAFA00000 && (inst & 0x001F0000))
	    continue;				/* sw reg,n($sp) */
						/* reg != $zero */
	else if ((inst & 0xFFE00000) == 0xE7A00000) /* swc1 freg,n($sp) */
	    continue;
	else if ((inst & 0xF3E00000) == 0xA3C00000 && (inst & 0x001F0000))
						/* sx reg,n($s8) */
	    continue;				/* reg != $zero */
	else if (inst == 0x03A0F021)		/* move $s8,$sp */
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
		load_immediate_bytes += 4;
		continue;
	      }
	    else if ((inst & 0xffff0000) == 0x34210000 || /* ori $at,$at,n */
		     (inst & 0xffff0000) == 0x35080000 || /* ori $t0,$t0,n */
		     (inst & 0xffff0000) == 0x34010000 || /* ori $at,$zero,n */
		     (inst & 0xffff0000) == 0x34080000)   /* ori $t0,$zero,n */
	      {
		load_immediate_bytes += 4;
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
  
  regnum = TYPE_CODE (valtype) == TYPE_CODE_FLT && mips_fpu ? FP0_REGNUM : 2;

  memcpy (valbuf, regbuf + REGISTER_BYTE (regnum), TYPE_LENGTH (valtype));
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
  
  regnum = TYPE_CODE (valtype) == TYPE_CODE_FLT && mips_fpu ? FP0_REGNUM : 2;
  memcpy(raw_buffer, valbuf, TYPE_LENGTH (valtype));

#ifdef REGISTER_CONVERT_FROM_TYPE
  REGISTER_CONVERT_FROM_TYPE(regnum, valtype, raw_buffer);
#endif

  write_register_bytes(REGISTER_BYTE (regnum), raw_buffer, TYPE_LENGTH (valtype));
}

/* These exist in mdebugread.c.  */
extern CORE_ADDR sigtramp_address, sigtramp_end;
extern void fixup_sigtramp PARAMS ((void));

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

static void reinit_frame_cache_sfunc PARAMS ((char *, int,
					      struct cmd_list_element *));

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
_initialize_mips_tdep ()
{
  struct cmd_list_element *c;

  /* Let the user turn off floating point and set the fence post for
     heuristic_proc_start.  */

  add_show_from_set
    (add_set_cmd ("mipsfpu", class_support, var_boolean,
		  (char *) &mips_fpu,
		  "Set use of floating point coprocessor.\n\
Turn off to avoid using floating point instructions when calling functions\n\
or dealing with return values.", &setlist),
     &showlist);

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
