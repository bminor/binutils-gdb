/* Target dependent code for the Motorola 68000 series.
   Copyright 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1999, 2000, 2001
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
#include "symtab.h"
#include "gdbcore.h"
#include "value.h"
#include "gdb_string.h"
#include "inferior.h"
#include "regcache.h"


#define P_LINKL_FP	0x480e
#define P_LINKW_FP	0x4e56
#define P_PEA_FP	0x4856
#define P_MOVL_SP_FP	0x2c4f
#define P_MOVL		0x207c
#define P_JSR		0x4eb9
#define P_BSR		0x61ff
#define P_LEAL		0x43fb
#define P_MOVML		0x48ef
#define P_FMOVM		0xf237
#define P_TRAP		0x4e40

void m68k_frame_init_saved_regs (struct frame_info *frame_info);

/* The only reason this is here is the tm-altos.h reference below.  It
   was moved back here from tm-m68k.h.  FIXME? */

extern CORE_ADDR
altos_skip_prologue (CORE_ADDR pc)
{
  register int op = read_memory_integer (pc, 2);
  if (op == P_LINKW_FP)
    pc += 4;			/* Skip link #word */
  else if (op == P_LINKL_FP)
    pc += 6;			/* Skip link #long */
  /* Not sure why branches are here.  */
  /* From tm-altos.h */
  else if (op == 0060000)
    pc += 4;			/* Skip bra #word */
  else if (op == 00600377)
    pc += 6;			/* skip bra #long */
  else if ((op & 0177400) == 0060000)
    pc += 2;			/* skip bra #char */
  return pc;
}

int
delta68_in_sigtramp (CORE_ADDR pc, char *name)
{
  if (name != NULL)
    return strcmp (name, "_sigcode") == 0;
  else
    return 0;
}

CORE_ADDR
delta68_frame_args_address (struct frame_info *frame_info)
{
  /* we assume here that the only frameless functions are the system calls
     or other functions who do not put anything on the stack. */
  if (frame_info->signal_handler_caller)
    return frame_info->frame + 12;
  else if (frameless_look_for_prologue (frame_info))
    {
      /* Check for an interrupted system call */
      if (frame_info->next && frame_info->next->signal_handler_caller)
	return frame_info->next->frame + 16;
      else
	return frame_info->frame + 4;
    }
  else
    return frame_info->frame;
}

CORE_ADDR
delta68_frame_saved_pc (struct frame_info *frame_info)
{
  return read_memory_integer (delta68_frame_args_address (frame_info) + 4, 4);
}

/* Return number of args passed to a frame.
   Can return -1, meaning no way to tell.  */

int
isi_frame_num_args (struct frame_info *fi)
{
  int val;
  CORE_ADDR pc = FRAME_SAVED_PC (fi);
  int insn = 0177777 & read_memory_integer (pc, 2);
  val = 0;
  if (insn == 0047757 || insn == 0157374)	/* lea W(sp),sp or addaw #W,sp */
    val = read_memory_integer (pc + 2, 2);
  else if ((insn & 0170777) == 0050217	/* addql #N, sp */
	   || (insn & 0170777) == 0050117)	/* addqw */
    {
      val = (insn >> 9) & 7;
      if (val == 0)
	val = 8;
    }
  else if (insn == 0157774)	/* addal #WW, sp */
    val = read_memory_integer (pc + 2, 4);
  val >>= 2;
  return val;
}

int
delta68_frame_num_args (struct frame_info *fi)
{
  int val;
  CORE_ADDR pc = FRAME_SAVED_PC (fi);
  int insn = 0177777 & read_memory_integer (pc, 2);
  val = 0;
  if (insn == 0047757 || insn == 0157374)	/* lea W(sp),sp or addaw #W,sp */
    val = read_memory_integer (pc + 2, 2);
  else if ((insn & 0170777) == 0050217	/* addql #N, sp */
	   || (insn & 0170777) == 0050117)	/* addqw */
    {
      val = (insn >> 9) & 7;
      if (val == 0)
	val = 8;
    }
  else if (insn == 0157774)	/* addal #WW, sp */
    val = read_memory_integer (pc + 2, 4);
  val >>= 2;
  return val;
}

int
news_frame_num_args (struct frame_info *fi)
{
  int val;
  CORE_ADDR pc = FRAME_SAVED_PC (fi);
  int insn = 0177777 & read_memory_integer (pc, 2);
  val = 0;
  if (insn == 0047757 || insn == 0157374)	/* lea W(sp),sp or addaw #W,sp */
    val = read_memory_integer (pc + 2, 2);
  else if ((insn & 0170777) == 0050217	/* addql #N, sp */
	   || (insn & 0170777) == 0050117)	/* addqw */
    {
      val = (insn >> 9) & 7;
      if (val == 0)
	val = 8;
    }
  else if (insn == 0157774)	/* addal #WW, sp */
    val = read_memory_integer (pc + 2, 4);
  val >>= 2;
  return val;
}

/* Insert the specified number of args and function address
   into a call sequence of the above form stored at DUMMYNAME.
   We use the BFD routines to store a big-endian value of known size.  */

void
m68k_fix_call_dummy (char *dummy, CORE_ADDR pc, CORE_ADDR fun, int nargs,
		     struct value **args, struct type *type, int gcc_p)
{
  bfd_putb32 (fun, (unsigned char *) dummy + CALL_DUMMY_START_OFFSET + 2);
  bfd_putb32 (nargs * 4,
	      (unsigned char *) dummy + CALL_DUMMY_START_OFFSET + 8);
}


/* Push an empty stack frame, to record the current PC, etc.  */

void
m68k_push_dummy_frame (void)
{
  register CORE_ADDR sp = read_register (SP_REGNUM);
  register int regnum;
  char raw_buffer[12];

  sp = push_word (sp, read_register (PC_REGNUM));
  sp = push_word (sp, read_register (FP_REGNUM));
  write_register (FP_REGNUM, sp);

  /* Always save the floating-point registers, whether they exist on
     this target or not.  */
  for (regnum = FP0_REGNUM + 7; regnum >= FP0_REGNUM; regnum--)
    {
      read_register_bytes (REGISTER_BYTE (regnum), raw_buffer, 12);
      sp = push_bytes (sp, raw_buffer, 12);
    }

  for (regnum = FP_REGNUM - 1; regnum >= 0; regnum--)
    {
      sp = push_word (sp, read_register (regnum));
    }
  sp = push_word (sp, read_register (PS_REGNUM));
  write_register (SP_REGNUM, sp);
}

/* Discard from the stack the innermost frame,
   restoring all saved registers.  */

void
m68k_pop_frame (void)
{
  register struct frame_info *frame = get_current_frame ();
  register CORE_ADDR fp;
  register int regnum;
  char raw_buffer[12];

  fp = FRAME_FP (frame);
  m68k_frame_init_saved_regs (frame);
  for (regnum = FP0_REGNUM + 7; regnum >= FP0_REGNUM; regnum--)
    {
      if (frame->saved_regs[regnum])
	{
	  read_memory (frame->saved_regs[regnum], raw_buffer, 12);
	  write_register_bytes (REGISTER_BYTE (regnum), raw_buffer, 12);
	}
    }
  for (regnum = FP_REGNUM - 1; regnum >= 0; regnum--)
    {
      if (frame->saved_regs[regnum])
	{
	  write_register (regnum,
			  read_memory_integer (frame->saved_regs[regnum], 4));
	}
    }
  if (frame->saved_regs[PS_REGNUM])
    {
      write_register (PS_REGNUM,
		      read_memory_integer (frame->saved_regs[PS_REGNUM], 4));
    }
  write_register (FP_REGNUM, read_memory_integer (fp, 4));
  write_register (PC_REGNUM, read_memory_integer (fp + 4, 4));
  write_register (SP_REGNUM, fp + 8);
  flush_cached_frames ();
}


/* Given an ip value corresponding to the start of a function,
   return the ip of the first instruction after the function 
   prologue.  This is the generic m68k support.  Machines which
   require something different can override the SKIP_PROLOGUE
   macro to point elsewhere.

   Some instructions which typically may appear in a function
   prologue include:

   A link instruction, word form:

   link.w       %a6,&0                  4e56  XXXX

   A link instruction, long form:

   link.l  %fp,&F%1             480e  XXXX  XXXX

   A movm instruction to preserve integer regs:

   movm.l  &M%1,(4,%sp)         48ef  XXXX  XXXX

   A fmovm instruction to preserve float regs:

   fmovm   &FPM%1,(FPO%1,%sp)   f237  XXXX  XXXX  XXXX  XXXX

   Some profiling setup code (FIXME, not recognized yet):

   lea.l   (.L3,%pc),%a1                43fb  XXXX  XXXX  XXXX
   bsr     _mcount                      61ff  XXXX  XXXX

 */

CORE_ADDR
m68k_skip_prologue (CORE_ADDR ip)
{
  register CORE_ADDR limit;
  struct symtab_and_line sal;
  register int op;

  /* Find out if there is a known limit for the extent of the prologue.
     If so, ensure we don't go past it.  If not, assume "infinity". */

  sal = find_pc_line (ip, 0);
  limit = (sal.end) ? sal.end : (CORE_ADDR) ~0;

  while (ip < limit)
    {
      op = read_memory_integer (ip, 2);
      op &= 0xFFFF;

      if (op == P_LINKW_FP)
	ip += 4;		/* Skip link.w */
      else if (op == P_PEA_FP)
	ip += 2;		/* Skip pea %fp */
      else if (op == P_MOVL_SP_FP)
	ip += 2;		/* Skip move.l %sp, %fp */
      else if (op == P_LINKL_FP)
	ip += 6;		/* Skip link.l */
      else if (op == P_MOVML)
	ip += 6;		/* Skip movm.l */
      else if (op == P_FMOVM)
	ip += 10;		/* Skip fmovm */
      else
	break;			/* Found unknown code, bail out. */
    }
  return (ip);
}

/* Store the addresses of the saved registers of the frame described by 
   FRAME_INFO in its saved_regs field.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

void
m68k_frame_init_saved_regs (struct frame_info *frame_info)
{
  register int regnum;
  register int regmask;
  register CORE_ADDR next_addr;
  register CORE_ADDR pc;

  /* First possible address for a pc in a call dummy for this frame.  */
  CORE_ADDR possible_call_dummy_start =
    (frame_info)->frame - 28 - FP_REGNUM * 4 - 4 - 8 * 12;

  int nextinsn;

  if (frame_info->saved_regs)
    return;

  frame_saved_regs_zalloc (frame_info);

  memset (frame_info->saved_regs, 0, SIZEOF_FRAME_SAVED_REGS);

  if ((frame_info)->pc >= possible_call_dummy_start
      && (frame_info)->pc <= (frame_info)->frame)
    {

      /* It is a call dummy.  We could just stop now, since we know
         what the call dummy saves and where.  But this code proceeds
         to parse the "prologue" which is part of the call dummy.
         This is needlessly complex and confusing.  FIXME.  */

      next_addr = (frame_info)->frame;
      pc = possible_call_dummy_start;
    }
  else
    {
      pc = get_pc_function_start ((frame_info)->pc);

      nextinsn = read_memory_integer (pc, 2);
      if (P_PEA_FP == nextinsn
	  && P_MOVL_SP_FP == read_memory_integer (pc + 2, 2))
	{
	  /* pea %fp
	     move.l %sp, %fp */
	  next_addr = frame_info->frame;
	  pc += 4;
	}
      else if (P_LINKL_FP == nextinsn)
	/* link.l %fp */
	/* Find the address above the saved   
	   regs using the amount of storage from the link instruction.  */
	{
	  next_addr = (frame_info)->frame + read_memory_integer (pc + 2, 4);
	  pc += 6;
	}
      else if (P_LINKW_FP == nextinsn)
	/* link.w %fp */
	/* Find the address above the saved   
	   regs using the amount of storage from the link instruction.  */
	{
	  next_addr = (frame_info)->frame + read_memory_integer (pc + 2, 2);
	  pc += 4;
	}
      else
	goto lose;

      /* If have an addal #-n, sp next, adjust next_addr.  */
      if ((0177777 & read_memory_integer (pc, 2)) == 0157774)
	next_addr += read_memory_integer (pc += 2, 4), pc += 4;
    }

  for (;;)
    {
      nextinsn = 0xffff & read_memory_integer (pc, 2);
      regmask = read_memory_integer (pc + 2, 2);
      /* fmovemx to -(sp) */
      if (0xf227 == nextinsn && (regmask & 0xff00) == 0xe000)
	{
	  /* Regmask's low bit is for register fp7, the first pushed */
	  for (regnum = FP0_REGNUM + 8; --regnum >= FP0_REGNUM; regmask >>= 1)
	    if (regmask & 1)
	      frame_info->saved_regs[regnum] = (next_addr -= 12);
	  pc += 4;
	}
      /* fmovemx to (fp + displacement) */
      else if (0171056 == nextinsn && (regmask & 0xff00) == 0xf000)
	{
	  register CORE_ADDR addr;

	  addr = (frame_info)->frame + read_memory_integer (pc + 4, 2);
	  /* Regmask's low bit is for register fp7, the first pushed */
	  for (regnum = FP0_REGNUM + 8; --regnum >= FP0_REGNUM; regmask >>= 1)
	    if (regmask & 1)
	      {
		frame_info->saved_regs[regnum] = addr;
		addr += 12;
	      }
	  pc += 6;
	}
      /* moveml to (sp) */
      else if (0044327 == nextinsn)
	{
	  /* Regmask's low bit is for register 0, the first written */
	  for (regnum = 0; regnum < 16; regnum++, regmask >>= 1)
	    if (regmask & 1)
	      {
		frame_info->saved_regs[regnum] = next_addr;
		next_addr += 4;
	      }
	  pc += 4;
	}
      /* moveml to (fp + displacement) */
      else if (0044356 == nextinsn)
	{
	  register CORE_ADDR addr;

	  addr = (frame_info)->frame + read_memory_integer (pc + 4, 2);
	  /* Regmask's low bit is for register 0, the first written */
	  for (regnum = 0; regnum < 16; regnum++, regmask >>= 1)
	    if (regmask & 1)
	      {
		frame_info->saved_regs[regnum] = addr;
		addr += 4;
	      }
	  pc += 6;
	}
      /* moveml to -(sp) */
      else if (0044347 == nextinsn)
	{
	  /* Regmask's low bit is for register 15, the first pushed */
	  for (regnum = 16; --regnum >= 0; regmask >>= 1)
	    if (regmask & 1)
	      frame_info->saved_regs[regnum] = (next_addr -= 4);
	  pc += 4;
	}
      /* movl r,-(sp) */
      else if (0x2f00 == (0xfff0 & nextinsn))
	{
	  regnum = 0xf & nextinsn;
	  frame_info->saved_regs[regnum] = (next_addr -= 4);
	  pc += 2;
	}
      /* fmovemx to index of sp */
      else if (0xf236 == nextinsn && (regmask & 0xff00) == 0xf000)
	{
	  /* Regmask's low bit is for register fp0, the first written */
	  for (regnum = FP0_REGNUM + 8; --regnum >= FP0_REGNUM; regmask >>= 1)
	    if (regmask & 1)
	      {
		frame_info->saved_regs[regnum] = next_addr;
		next_addr += 12;
	      }
	  pc += 10;
	}
      /* clrw -(sp); movw ccr,-(sp) */
      else if (0x4267 == nextinsn && 0x42e7 == regmask)
	{
	  frame_info->saved_regs[PS_REGNUM] = (next_addr -= 4);
	  pc += 4;
	}
      else
	break;
    }
lose:;
  frame_info->saved_regs[SP_REGNUM] = (frame_info)->frame + 8;
  frame_info->saved_regs[FP_REGNUM] = (frame_info)->frame;
  frame_info->saved_regs[PC_REGNUM] = (frame_info)->frame + 4;
#ifdef SIG_SP_FP_OFFSET
  /* Adjust saved SP_REGNUM for fake _sigtramp frames.  */
  if (frame_info->signal_handler_caller && frame_info->next)
    frame_info->saved_regs[SP_REGNUM] =
      frame_info->next->frame + SIG_SP_FP_OFFSET;
#endif
}


#ifdef USE_PROC_FS		/* Target dependent support for /proc */

#include <sys/procfs.h>

/* Prototypes for supply_gregset etc. */
#include "gregset.h"

/*  The /proc interface divides the target machine's register set up into
   two different sets, the general register set (gregset) and the floating
   point register set (fpregset).  For each set, there is an ioctl to get
   the current register set and another ioctl to set the current values.

   The actual structure passed through the ioctl interface is, of course,
   naturally machine dependent, and is different for each set of registers.
   For the m68k for example, the general register set is typically defined
   by:

   typedef int gregset_t[18];

   #define      R_D0    0
   ...
   #define      R_PS    17

   and the floating point set by:

   typedef      struct fpregset {
   int  f_pcr;
   int  f_psr;
   int  f_fpiaddr;
   int  f_fpregs[8][3];         (8 regs, 96 bits each)
   } fpregset_t;

   These routines provide the packing and unpacking of gregset_t and
   fpregset_t formatted data.

 */

/* Atari SVR4 has R_SR but not R_PS */

#if !defined (R_PS) && defined (R_SR)
#define R_PS R_SR
#endif

/*  Given a pointer to a general register set in /proc format (gregset_t *),
   unpack the register contents and supply them as gdb's idea of the current
   register values. */

void
supply_gregset (gregset_t *gregsetp)
{
  register int regi;
  register greg_t *regp = (greg_t *) gregsetp;

  for (regi = 0; regi < R_PC; regi++)
    {
      supply_register (regi, (char *) (regp + regi));
    }
  supply_register (PS_REGNUM, (char *) (regp + R_PS));
  supply_register (PC_REGNUM, (char *) (regp + R_PC));
}

void
fill_gregset (gregset_t *gregsetp, int regno)
{
  register int regi;
  register greg_t *regp = (greg_t *) gregsetp;

  for (regi = 0; regi < R_PC; regi++)
    {
      if ((regno == -1) || (regno == regi))
	{
	  *(regp + regi) = *(int *) &registers[REGISTER_BYTE (regi)];
	}
    }
  if ((regno == -1) || (regno == PS_REGNUM))
    {
      *(regp + R_PS) = *(int *) &registers[REGISTER_BYTE (PS_REGNUM)];
    }
  if ((regno == -1) || (regno == PC_REGNUM))
    {
      *(regp + R_PC) = *(int *) &registers[REGISTER_BYTE (PC_REGNUM)];
    }
}

#if defined (FP0_REGNUM)

/*  Given a pointer to a floating point register set in /proc format
   (fpregset_t *), unpack the register contents and supply them as gdb's
   idea of the current floating point register values. */

void
supply_fpregset (fpregset_t *fpregsetp)
{
  register int regi;
  char *from;

  for (regi = FP0_REGNUM; regi < FPC_REGNUM; regi++)
    {
      from = (char *) &(fpregsetp->f_fpregs[regi - FP0_REGNUM][0]);
      supply_register (regi, from);
    }
  supply_register (FPC_REGNUM, (char *) &(fpregsetp->f_pcr));
  supply_register (FPS_REGNUM, (char *) &(fpregsetp->f_psr));
  supply_register (FPI_REGNUM, (char *) &(fpregsetp->f_fpiaddr));
}

/*  Given a pointer to a floating point register set in /proc format
   (fpregset_t *), update the register specified by REGNO from gdb's idea
   of the current floating point register set.  If REGNO is -1, update
   them all. */

void
fill_fpregset (fpregset_t *fpregsetp, int regno)
{
  int regi;
  char *to;
  char *from;

  for (regi = FP0_REGNUM; regi < FPC_REGNUM; regi++)
    {
      if ((regno == -1) || (regno == regi))
	{
	  from = (char *) &registers[REGISTER_BYTE (regi)];
	  to = (char *) &(fpregsetp->f_fpregs[regi - FP0_REGNUM][0]);
	  memcpy (to, from, REGISTER_RAW_SIZE (regi));
	}
    }
  if ((regno == -1) || (regno == FPC_REGNUM))
    {
      fpregsetp->f_pcr = *(int *) &registers[REGISTER_BYTE (FPC_REGNUM)];
    }
  if ((regno == -1) || (regno == FPS_REGNUM))
    {
      fpregsetp->f_psr = *(int *) &registers[REGISTER_BYTE (FPS_REGNUM)];
    }
  if ((regno == -1) || (regno == FPI_REGNUM))
    {
      fpregsetp->f_fpiaddr = *(int *) &registers[REGISTER_BYTE (FPI_REGNUM)];
    }
}

#endif /* defined (FP0_REGNUM) */

#endif /* USE_PROC_FS */

/* Figure out where the longjmp will land.  Slurp the args out of the stack.
   We expect the first arg to be a pointer to the jmp_buf structure from which
   we extract the pc (JB_PC) that we will land at.  The pc is copied into PC.
   This routine returns true on success. */

/* NOTE: cagney/2000-11-08: For this function to be fully multi-arched
   the macro's JB_PC and JB_ELEMENT_SIZE would need to be moved into
   the ``struct gdbarch_tdep'' object and then set on a target ISA/ABI
   dependant basis. */

int
m68k_get_longjmp_target (CORE_ADDR *pc)
{
#if defined (JB_PC) && defined (JB_ELEMENT_SIZE)
  char *buf;
  CORE_ADDR sp, jb_addr;

  buf = alloca (TARGET_PTR_BIT / TARGET_CHAR_BIT);
  sp = read_register (SP_REGNUM);

  if (target_read_memory (sp + SP_ARG0,	/* Offset of first arg on stack */
			  buf, TARGET_PTR_BIT / TARGET_CHAR_BIT))
    return 0;

  jb_addr = extract_address (buf, TARGET_PTR_BIT / TARGET_CHAR_BIT);

  if (target_read_memory (jb_addr + JB_PC * JB_ELEMENT_SIZE, buf,
			  TARGET_PTR_BIT / TARGET_CHAR_BIT))
    return 0;

  *pc = extract_address (buf, TARGET_PTR_BIT / TARGET_CHAR_BIT);

  return 1;
#else
  internal_error (__FILE__, __LINE__,
		  "m68k_get_longjmp_target: not implemented");
  return 0;
#endif
}

/* Immediately after a function call, return the saved pc before the frame
   is setup.  For sun3's, we check for the common case of being inside of a
   system call, and if so, we know that Sun pushes the call # on the stack
   prior to doing the trap. */

CORE_ADDR
m68k_saved_pc_after_call (struct frame_info *frame)
{
#ifdef SYSCALL_TRAP
  int op;

  op = read_memory_integer (frame->pc - SYSCALL_TRAP_OFFSET, 2);

  if (op == SYSCALL_TRAP)
    return read_memory_integer (read_register (SP_REGNUM) + 4, 4);
  else
#endif /* SYSCALL_TRAP */
    return read_memory_integer (read_register (SP_REGNUM), 4);
}

/* Function: m68k_gdbarch_init
   Initializer function for the m68k gdbarch vector.
   Called by gdbarch.  Sets up the gdbarch vector(s) for this target. */

static struct gdbarch *
m68k_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  static LONGEST call_dummy_words[7] = { 0xf227e0ff, 0x48e7fffc, 0x426742e7,
    0x4eb93232, 0x3232dffc, 0x69696969,
    (0x4e404e71 | (BPT_VECTOR << 16))
  };
  struct gdbarch_tdep *tdep = NULL;
  struct gdbarch *gdbarch;

  /* find a candidate among the list of pre-declared architectures. */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return (arches->gdbarch);

#if 0
  tdep = (struct gdbarch_tdep *) xmalloc (sizeof (struct gdbarch_tdep));
#endif

  gdbarch = gdbarch_alloc (&info, 0);

  set_gdbarch_frame_init_saved_regs (gdbarch, m68k_frame_init_saved_regs);

  set_gdbarch_use_generic_dummy_frames (gdbarch, 0);
  set_gdbarch_call_dummy_location (gdbarch, ON_STACK);
  set_gdbarch_call_dummy_breakpoint_offset_p (gdbarch, 1);
  set_gdbarch_call_dummy_breakpoint_offset (gdbarch, 24);
  set_gdbarch_pc_in_call_dummy (gdbarch, pc_in_call_dummy_on_stack);
  set_gdbarch_call_dummy_p (gdbarch, 1);
  set_gdbarch_call_dummy_stack_adjust_p (gdbarch, 0);
  set_gdbarch_call_dummy_length (gdbarch, 28);
  set_gdbarch_call_dummy_start_offset (gdbarch, 12);

  set_gdbarch_call_dummy_words (gdbarch, call_dummy_words);
  set_gdbarch_sizeof_call_dummy_words (gdbarch, sizeof (call_dummy_words));
  set_gdbarch_call_dummy_stack_adjust_p (gdbarch, 0);
  set_gdbarch_fix_call_dummy (gdbarch, m68k_fix_call_dummy);
  set_gdbarch_push_dummy_frame (gdbarch, m68k_push_dummy_frame);
  set_gdbarch_pop_frame (gdbarch, m68k_pop_frame);

  return gdbarch;
}


static void
m68k_dump_tdep (struct gdbarch *current_gdbarch, struct ui_file *file)
{

}

void
_initialize_m68k_tdep (void)
{
  gdbarch_register (bfd_arch_m68k, m68k_gdbarch_init, m68k_dump_tdep);
  tm_print_insn = print_insn_m68k;
}
