/* Machine-dependent code which would otherwise be in inflow.c and core.c,
   for GDB, the GNU debugger.  This code is for the HP PA-RISC cpu.
   Copyright 1986, 1987, 1989, 1990, 1991, 1992 Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

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
#include "value.h"

/* For argument passing to the inferior */
#include "symtab.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <sys/ioctl.h>

#ifdef COFF_ENCAPSULATE
#include "a.out.encap.h"
#else
#include <a.out.h>
#endif
#ifndef N_SET_MAGIC
#define N_SET_MAGIC(exec, val) ((exec).a_magic = (val))
#endif

/*#include <sys/user.h>		After a.out.h  */
#include <sys/file.h>
#include <sys/stat.h>
#include <machine/psl.h>
#include "wait.h"

#include "gdbcore.h"
#include "gdbcmd.h"
#include "target.h"
#include "symfile.h"
#include "objfiles.h"


/* Routines to extract various sized constants out of hppa 
   instructions. */

/* This assumes that no garbage lies outside of the lower bits of 
   value. */

int
sign_extend (val, bits)
     unsigned val, bits;
{
  return (int)(val >> bits - 1 ? (-1 << bits) | val : val);
}

/* For many immediate values the sign bit is the low bit! */

int
low_sign_extend (val, bits)
     unsigned val, bits;
{
  return (int)((val & 0x1 ? (-1 << (bits - 1)) : 0) | val >> 1);
}
/* extract the immediate field from a ld{bhw}s instruction */

unsigned
get_field (val, from, to)
     unsigned val, from, to;
{
  val = val >> 31 - to;
  return val & ((1 << 32 - from) - 1);
}

unsigned
set_field (val, from, to, new_val)
     unsigned *val, from, to;
{
  unsigned mask = ~((1 << (to - from + 1)) << (31 - from));
  return *val = *val & mask | (new_val << (31 - from));
}

/* extract a 3-bit space register number from a be, ble, mtsp or mfsp */

extract_3 (word)
     unsigned word;
{
  return GET_FIELD (word, 18, 18) << 2 | GET_FIELD (word, 16, 17);
}
       
extract_5_load (word)
     unsigned word;
{
  return low_sign_extend (word >> 16 & MASK_5, 5);
}

/* extract the immediate field from a st{bhw}s instruction */

int
extract_5_store (word)
     unsigned word;
{
  return low_sign_extend (word & MASK_5, 5);
}

/* extract an 11 bit immediate field */

int
extract_11 (word)
     unsigned word;
{
  return low_sign_extend (word & MASK_11, 11);
}

/* extract a 14 bit immediate field */

int
extract_14 (word)
     unsigned word;
{
  return low_sign_extend (word & MASK_14, 14);
}

/* deposit a 14 bit constant in a word */

unsigned
deposit_14 (opnd, word)
     int opnd;
     unsigned word;
{
  unsigned sign = (opnd < 0 ? 1 : 0);

  return word | ((unsigned)opnd << 1 & MASK_14)  | sign;
}

/* extract a 21 bit constant */

int
extract_21 (word)
     unsigned word;
{
  int val;

  word &= MASK_21;
  word <<= 11;
  val = GET_FIELD (word, 20, 20);
  val <<= 11;
  val |= GET_FIELD (word, 9, 19);
  val <<= 2;
  val |= GET_FIELD (word, 5, 6);
  val <<= 5;
  val |= GET_FIELD (word, 0, 4);
  val <<= 2;
  val |= GET_FIELD (word, 7, 8);
  return sign_extend (val, 21) << 11;
}

/* deposit a 21 bit constant in a word. Although 21 bit constants are
   usually the top 21 bits of a 32 bit constant, we assume that only
   the low 21 bits of opnd are relevant */

unsigned
deposit_21 (opnd, word)
     unsigned opnd, word;
{
  unsigned val = 0;

  val |= GET_FIELD (opnd, 11 + 14, 11 + 18);
  val <<= 2;
  val |= GET_FIELD (opnd, 11 + 12, 11 + 13);
  val <<= 2;
  val |= GET_FIELD (opnd, 11 + 19, 11 + 20);
  val <<= 11;
  val |= GET_FIELD (opnd, 11 + 1, 11 + 11);
  val <<= 1;
  val |= GET_FIELD (opnd, 11 + 0, 11 + 0);
  return word | val;
}

/* extract a 12 bit constant from branch instructions */

int
extract_12 (word)
     unsigned word;
{
  return sign_extend (GET_FIELD (word, 19, 28) |
		      GET_FIELD (word, 29, 29) << 10 |
		      (word & 0x1) << 11, 12) << 2;
}

/* extract a 17 bit constant from branch instructions, returning the
   19 bit signed value. */

int
extract_17 (word)
     unsigned word;
{
  return sign_extend (GET_FIELD (word, 19, 28) |
		      GET_FIELD (word, 29, 29) << 10 |
		      GET_FIELD (word, 11, 15) << 11 |
		      (word & 0x1) << 16, 17) << 2;
}

static int use_unwind = 0;

/* Lookup the unwind (stack backtrace) info for the given PC.  We search all
   of the objfiles seeking the unwind table entry for this PC.  Each objfile
   contains a sorted list of struct unwind_table_entry.  Since we do a binary
   search of the unwind tables, we depend upon them to be sorted.  */

static struct unwind_table_entry *
find_unwind_entry(pc)
     CORE_ADDR pc;
{
  int first, middle, last;
  struct objfile *objfile;

  ALL_OBJFILES (objfile)
    {
      struct obj_unwind_info *ui;

      ui = OBJ_UNWIND_INFO (objfile);

      if (!ui)
	continue;

      /* First, check the cache */

      if (ui->cache
	  && pc >= ui->cache->region_start
	  && pc <= ui->cache->region_end)
	return ui->cache;

      /* Not in the cache, do a binary search */

      first = 0;
      last = ui->last;

      while (first <= last)
	{
	  middle = (first + last) / 2;
	  if (pc >= ui->table[middle].region_start
	      && pc <= ui->table[middle].region_end)
	    {
	      ui->cache = &ui->table[middle];
	      return &ui->table[middle];
	    }

	  if (pc < ui->table[middle].region_start)
	    last = middle - 1;
	  else
	    first = middle + 1;
	}
    }				/* ALL_OBJFILES() */
  return NULL;
}

static int
find_return_regnum(pc)
     CORE_ADDR pc;
{
  struct unwind_table_entry *u;

  u = find_unwind_entry (pc);

  if (!u)
    return RP_REGNUM;

  if (u->Millicode)
    return 31;

  return RP_REGNUM;
}

int
find_proc_framesize(pc)
     CORE_ADDR pc;
{
  struct unwind_table_entry *u;

  if (!use_unwind)
    return -1;

  u = find_unwind_entry (pc);

  if (!u)
    return -1;

  return u->Total_frame_size << 3;
}

int
rp_saved(pc)
{
  struct unwind_table_entry *u;

  u = find_unwind_entry (pc);

  if (!u)
    return 0;

  if (u->Save_RP)
    return 1;
  else
    return 0;
}

CORE_ADDR
saved_pc_after_call (frame)
     FRAME frame;
{
  int ret_regnum;

  ret_regnum = find_return_regnum (get_frame_pc (frame));

  return read_register (ret_regnum) & ~0x3;
}

CORE_ADDR
frame_saved_pc (frame)
     FRAME frame;
{
  CORE_ADDR pc = get_frame_pc (frame);

  if (frameless_look_for_prologue (frame))
    {
      int ret_regnum;

      ret_regnum = find_return_regnum (pc);

      return read_register (ret_regnum) & ~0x3;
    }
  else if (rp_saved (pc))
    return read_memory_integer (frame->frame - 20, 4) & ~0x3;
  else
    return read_register (RP_REGNUM) & ~0x3;
}

/* We need to correct the PC and the FP for the outermost frame when we are
   in a system call.  */

void
init_extra_frame_info (fromleaf, frame)
     int fromleaf;
     struct frame_info *frame;
{
  int flags;
  int framesize;

  if (frame->next)		/* Only do this for outermost frame */
    return;

  flags = read_register (FLAGS_REGNUM);
  if (flags & 2)	/* In system call? */
    frame->pc = read_register (31) & ~0x3;

  /* The outermost frame is always derived from PC-framesize */
  framesize = find_proc_framesize(frame->pc);
  if (framesize == -1)
    frame->frame = read_register (FP_REGNUM);
  else
    frame->frame = read_register (SP_REGNUM) - framesize;

  if (!frameless_look_for_prologue (frame)) /* Frameless? */
    return;				    /* No, quit now */

  /* For frameless functions, we need to look at the caller's frame */
  framesize = find_proc_framesize(FRAME_SAVED_PC(frame));
  if (framesize != -1)
    frame->frame -= framesize;
}

FRAME_ADDR
frame_chain (frame)
     struct frame_info *frame;
{
  int framesize;

  framesize = find_proc_framesize(FRAME_SAVED_PC(frame));

  if (framesize != -1)
    return frame->frame - framesize;

  return read_memory_integer (frame->frame, 4);
}

/* To see if a frame chain is valid, see if the caller looks like it
   was compiled with gcc. */

int
frame_chain_valid (chain, thisframe)
     FRAME_ADDR chain;
     FRAME thisframe;
{
  struct minimal_symbol *msym;

  if (!chain)
    return 0;

  msym = lookup_minimal_symbol_by_pc (FRAME_SAVED_PC (thisframe));

  if (msym
      && (strcmp (SYMBOL_NAME (msym), "_start") == 0))
    return 0;
  else
    return 1;
}

#if 0
/* Some helper functions. gcc_p returns 1 if the function beginning at 
   pc appears to have been compiled with gcc. hpux_cc_p returns 1 if
   fn was compiled with hpux cc. gcc functions look like :

   stw     rp,-0x14(sp) ; optional
   or      r4,r0,r1
   or      sp,r0,r4
   stwm    r1,framesize(sp)

   hpux cc functions look like:

   stw     rp,-0x14(sp) ; optional.
   stwm    r3,framesiz(sp)
   */

gcc_p (pc)
     CORE_ADDR pc;
{
  if (read_memory_integer (pc, 4) == 0x6BC23FD9)			
    pc = pc + 4;							
      
  if (read_memory_integer (pc, 4) == 0x8040241
      && read_memory_integer (pc + 4, 4) == 0x81E0244)
    return 1;
  return 0;
}
#endif

/*
 * These functions deal with saving and restoring register state
 * around a function call in the inferior. They keep the stack
 * double-word aligned; eventually, on an hp700, the stack will have
 * to be aligned to a 64-byte boundary.
 */

int
push_dummy_frame ()
{
  register CORE_ADDR sp;
  register int regnum;
  int int_buffer;
  double freg_buffer;

  /* Space for "arguments"; the RP goes in here. */
  sp = read_register (SP_REGNUM) + 48;
  int_buffer = read_register (RP_REGNUM) | 0x3;
  write_memory (sp - 20, (char *)&int_buffer, 4);

  int_buffer = read_register (FP_REGNUM);
  write_memory (sp, (char *)&int_buffer, 4);

  write_register (FP_REGNUM, sp);

  sp += 8;

  for (regnum = 1; regnum < 32; regnum++)
    if (regnum != RP_REGNUM && regnum != FP_REGNUM)
      sp = push_word (sp, read_register (regnum));

  sp += 4;

  for (regnum = FP0_REGNUM; regnum < NUM_REGS; regnum++)
    {
      read_register_bytes (REGISTER_BYTE (regnum), (char *)&freg_buffer, 8);
      sp = push_bytes (sp, (char *)&freg_buffer, 8);
    }
  sp = push_word (sp, read_register (IPSW_REGNUM));
  sp = push_word (sp, read_register (SAR_REGNUM));
  sp = push_word (sp, read_register (PCOQ_HEAD_REGNUM));
  sp = push_word (sp, read_register (PCSQ_HEAD_REGNUM));
  sp = push_word (sp, read_register (PCOQ_TAIL_REGNUM));
  sp = push_word (sp, read_register (PCSQ_TAIL_REGNUM));
  write_register (SP_REGNUM, sp);
}

find_dummy_frame_regs (frame, frame_saved_regs)
     struct frame_info *frame;
     struct frame_saved_regs *frame_saved_regs;
{
  CORE_ADDR fp = frame->frame;
  int i;

  frame_saved_regs->regs[RP_REGNUM] = fp - 20 & ~0x3;
  frame_saved_regs->regs[FP_REGNUM] = fp;
  frame_saved_regs->regs[1] = fp + 8;
  frame_saved_regs->regs[3] = fp + 12;

  for (fp += 16, i = 5; i < 32; fp += 4, i++)
    frame_saved_regs->regs[i] = fp;

  fp += 4;
  for (i = FP0_REGNUM; i < NUM_REGS; i++, fp += 8)
    frame_saved_regs->regs[i] = fp;

  frame_saved_regs->regs[IPSW_REGNUM] = fp;
  fp += 4;
  frame_saved_regs->regs[SAR_REGNUM] = fp;
  fp += 4;
  frame_saved_regs->regs[PCOQ_HEAD_REGNUM] = fp;
  fp +=4;
  frame_saved_regs->regs[PCSQ_HEAD_REGNUM] = fp;
  fp +=4;
  frame_saved_regs->regs[PCOQ_TAIL_REGNUM] = fp;
  fp +=4;
  frame_saved_regs->regs[PCSQ_TAIL_REGNUM] = fp;
}

int
hp_pop_frame ()
{
  register FRAME frame = get_current_frame ();
  register CORE_ADDR fp;
  register int regnum;
  struct frame_saved_regs fsr;
  struct frame_info *fi;
  double freg_buffer;

  fi = get_frame_info (frame);
  fp = fi->frame;
  get_frame_saved_regs (fi, &fsr);

  if (fsr.regs[IPSW_REGNUM])    /* Restoring a call dummy frame */
    hp_restore_pc_queue (&fsr);

  for (regnum = 31; regnum > 0; regnum--)
    if (fsr.regs[regnum])
      write_register (regnum, read_memory_integer (fsr.regs[regnum], 4));

  for (regnum = NUM_REGS - 1; regnum >= FP0_REGNUM ; regnum--)
    if (fsr.regs[regnum])
      {
	read_memory (fsr.regs[regnum], (char *)&freg_buffer, 8);
        write_register_bytes (REGISTER_BYTE (regnum), (char *)&freg_buffer, 8);
      }

  if (fsr.regs[IPSW_REGNUM])
    write_register (IPSW_REGNUM,
                    read_memory_integer (fsr.regs[IPSW_REGNUM], 4));

  if (fsr.regs[SAR_REGNUM])
    write_register (SAR_REGNUM,
                    read_memory_integer (fsr.regs[SAR_REGNUM], 4));

  if (fsr.regs[PCOQ_TAIL_REGNUM])
    write_register (PCOQ_TAIL_REGNUM,
                    read_memory_integer (fsr.regs[PCOQ_TAIL_REGNUM], 4));

  write_register (FP_REGNUM, read_memory_integer (fp, 4));

  if (fsr.regs[IPSW_REGNUM])    /* call dummy */
    write_register (SP_REGNUM, fp - 48);
  else
    write_register (SP_REGNUM, fp);

  flush_cached_frames ();
  set_current_frame (create_new_frame (read_register (FP_REGNUM),
                                       read_pc ()));
}

/*
 * After returning to a dummy on the stack, restore the instruction
 * queue space registers. */

int
hp_restore_pc_queue (fsr)
     struct frame_saved_regs *fsr;
{
  CORE_ADDR pc = read_pc ();
  CORE_ADDR new_pc = read_memory_integer (fsr->regs[PCOQ_HEAD_REGNUM], 4);
  int pid;
  WAITTYPE w;
  int insn_count;

  /* Advance past break instruction in the call dummy. */
  write_register (PCOQ_HEAD_REGNUM, pc + 4);
  write_register (PCOQ_TAIL_REGNUM, pc + 8);

  /*
   * HPUX doesn't let us set the space registers or the space
   * registers of the PC queue through ptrace. Boo, hiss.
   * Conveniently, the call dummy has this sequence of instructions
   * after the break:
   *    mtsp r21, sr0
   *    ble,n 0(sr0, r22)
   *
   * So, load up the registers and single step until we are in the
   * right place.
   */

  write_register (21, read_memory_integer (fsr->regs[PCSQ_HEAD_REGNUM], 4));
  write_register (22, new_pc);

  for (insn_count = 0; insn_count < 3; insn_count++)
    {
      resume (1, 0);
      target_wait(&w);

      if (!WIFSTOPPED (w))
        {
          stop_signal = WTERMSIG (w);
          terminal_ours_for_output ();
          printf ("\nProgram terminated with signal %d, %s\n",
                  stop_signal, safe_strsignal (stop_signal));
          fflush (stdout);
          return 0;
        }
    }
  fetch_inferior_registers (-1);
  return 1;
}

CORE_ADDR
hp_push_arguments (nargs, args, sp, struct_return, struct_addr)
     int nargs;
     value *args;
     CORE_ADDR sp;
     int struct_return;
     CORE_ADDR struct_addr;
{
  /* array of arguments' offsets */
  int *offset = (int *)alloca(nargs);
  int cum = 0;
  int i, alignment;
  
  for (i = 0; i < nargs; i++)
    {
      cum += TYPE_LENGTH (VALUE_TYPE (args[i]));

    /* value must go at proper alignment. Assume alignment is a
	 power of two.*/
      alignment = hp_alignof (VALUE_TYPE (args[i]));
      if (cum % alignment)
	cum = (cum + alignment) & -alignment;
      offset[i] = -cum;
    }
  sp += min ((cum + 7) & -8, 16);

  for (i = 0; i < nargs; i++)
    write_memory (sp + offset[i], VALUE_CONTENTS (args[i]),
		  TYPE_LENGTH (VALUE_TYPE (args[i])));

  if (struct_return)
    write_register (28, struct_addr);
  return sp + 32;
}

/* return the alignment of a type in bytes. Structures have the maximum
   alignment required by their fields. */

int
hp_alignof (arg)
     struct type *arg;
{
  int max_align, align, i;
  switch (TYPE_CODE (arg))
    {
    case TYPE_CODE_PTR:
    case TYPE_CODE_INT:
    case TYPE_CODE_FLT:
      return TYPE_LENGTH (arg);
    case TYPE_CODE_ARRAY:
      return hp_alignof (TYPE_FIELD_TYPE (arg, 0));
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      max_align = 2;
      for (i = 0; i < TYPE_NFIELDS (arg); i++)
	{
	  /* Bit fields have no real alignment. */
	  if (!TYPE_FIELD_BITPOS (arg, i))
	    {
	      align = hp_alignof (TYPE_FIELD_TYPE (arg, i));
	      max_align = max (max_align, align);
	    }
	}
      return max_align;
    default:
      return 4;
    }
}

/* Print the register regnum, or all registers if regnum is -1 */

pa_do_registers_info (regnum, fpregs)
     int regnum;
     int fpregs;
{
  char raw_regs [REGISTER_BYTES];
  int i;
  
  for (i = 0; i < NUM_REGS; i++)
    read_relative_register_raw_bytes (i, raw_regs + REGISTER_BYTE (i));
  if (regnum == -1)
    pa_print_registers (raw_regs, regnum, fpregs);
  else if (regnum < FP0_REGNUM)
    printf ("%s %x\n", reg_names[regnum], *(long *)(raw_regs +
						    REGISTER_BYTE (regnum)));
  else
    pa_print_fp_reg (regnum);
}

pa_print_registers (raw_regs, regnum, fpregs)
     char *raw_regs;
     int regnum;
     int fpregs;
{
  int i;

  for (i = 0; i < 18; i++)
    printf ("%8.8s: %8x  %8.8s: %8x  %8.8s: %8x  %8.8s: %8x\n",
	    reg_names[i],
	    *(int *)(raw_regs + REGISTER_BYTE (i)),
	    reg_names[i + 18],
	    *(int *)(raw_regs + REGISTER_BYTE (i + 18)),
	    reg_names[i + 36],
	    *(int *)(raw_regs + REGISTER_BYTE (i + 36)),
	    reg_names[i + 54],
	    *(int *)(raw_regs + REGISTER_BYTE (i + 54)));

  if (fpregs)
    for (i = 72; i < NUM_REGS; i++)
      pa_print_fp_reg (i);
}

pa_print_fp_reg (i)
     int i;
{
  unsigned char raw_buffer[MAX_REGISTER_RAW_SIZE];
  unsigned char virtual_buffer[MAX_REGISTER_VIRTUAL_SIZE];
  REGISTER_TYPE val;

  /* Get the data in raw format, then convert also to virtual format.  */
  read_relative_register_raw_bytes (i, raw_buffer);
  REGISTER_CONVERT_TO_VIRTUAL (i, raw_buffer, virtual_buffer);

  fputs_filtered (reg_names[i], stdout);
  print_spaces_filtered (15 - strlen (reg_names[i]), stdout);

  val_print (REGISTER_VIRTUAL_TYPE (i), virtual_buffer, 0, stdout, 0,
	     1, 0, Val_pretty_default);
  printf_filtered ("\n");
}

/* Function calls that pass into a new compilation unit must pass through a
   small piece of code that does long format (`external' in HPPA parlance)
   jumps.  We figure out where the trampoline is going to end up, and return
   the PC of the final destination.  If we aren't in a trampoline, we just
   return NULL. 

   For computed calls, we just extract the new PC from r22.  */

CORE_ADDR
skip_trampoline_code (pc, name)
     CORE_ADDR pc;
     char *name;
{
  long inst0, inst1;
  static CORE_ADDR dyncall = 0;
  struct minimal_symbol *msym;

/* FIXME XXX - dyncall must be initialized whenever we get a new exec file */

  if (!dyncall)
    {
      msym = lookup_minimal_symbol ("$$dyncall", NULL);
      if (msym)
	dyncall = SYMBOL_VALUE_ADDRESS (msym);
      else
	dyncall = -1;
    }

  if (pc == dyncall)
    return (CORE_ADDR)(read_register (22) & ~0x3);

  inst0 = read_memory_integer (pc, 4);
  inst1 = read_memory_integer (pc+4, 4);

  if (   (inst0 & 0xffe00000) == 0x20200000 /* ldil xxx, r1 */
      && (inst1 & 0xffe0e002) == 0xe0202002) /* be,n yyy(sr4, r1) */
    pc = extract_21 (inst0) + extract_17 (inst1);
  else
    pc = (CORE_ADDR)NULL;

  return pc;
}

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

/* skip (stw rp, -20(0,sp)); copy 4,1; copy sp, 4; stwm 1,framesize(sp) 
   for gcc, or (stw rp, -20(0,sp); stwm 1, framesize(sp) for hcc */

CORE_ADDR
skip_prologue(pc)
     CORE_ADDR pc;
{
  int inst;
  int status;

  status = target_read_memory (pc, (char *)&inst, 4);
  SWAP_TARGET_AND_HOST (&inst, sizeof (inst));
  if (status != 0)
    return pc;

  if (inst == 0x6BC23FD9)	/* stw rp,-20(sp) */
    {
      if (read_memory_integer (pc + 4, 4) == 0x8040241)	/* copy r4,r1 */
	pc += 16;
      else if ((read_memory_integer (pc + 4, 4) & ~MASK_14) == 0x68810000) /* stw r1,(r4) */
	pc += 8;
    }
  else if (read_memory_integer (pc, 4) == 0x8040241) /* copy r4,r1 */
    pc += 12;
  else if ((read_memory_integer (pc, 4) & ~MASK_14) == 0x68810000) /* stw r1,(r4) */
    pc += 4;

  return pc;
}

static void
unwind_command (exp, from_tty)
     char *exp;
     int from_tty;
{
  CORE_ADDR address;
  union
    {
      int *foo;
      struct unwind_table_entry *u;
    } xxx;

  /* If we have an expression, evaluate it and use it as the address.  */

  if (exp != 0 && *exp != 0)
    address = parse_and_eval_address (exp);
  else
    return;

  xxx.u = find_unwind_entry (address);

  if (!xxx.u)
    {
      printf ("Can't find unwind table entry for PC 0x%x\n", address);
      return;
    }

  printf ("%08x\n%08X\n%08X\n%08X\n", xxx.foo[0], xxx.foo[1], xxx.foo[2],
	  xxx.foo[3]);
}

void
_initialize_hppah_tdep ()
{
  add_com ("unwind", class_obscure, unwind_command, "Print unwind info\n");
  add_show_from_set
    (add_set_cmd ("use_unwind", class_obscure, var_boolean,
		  (char *)&use_unwind,
		  "Set the usage of unwind info", &setlist),
     &showlist);
}
