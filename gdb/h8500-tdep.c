/* Target-machine dependent code for Hitachi H8/500, for GDB.
   Copyright (C) 1993 Free Software Foundation, Inc.

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

/*
 Contributed by Steve Chamberlain
                sac@cygnus.com
 */

#include "defs.h"
#include "frame.h"
#include "obstack.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "dis-asm.h"
#include "../opcodes/h8500-opc.h"
;
#undef NUM_REGS
#define NUM_REGS 11

#define UNSIGNED_SHORT(X) ((X) & 0xffff)


/* Shape of an H8/500 frame : 


   arg-n
   ..
   arg-2
   arg-1
   return address <2 or 4 bytes>
   old fp	  <2 bytes>
   auto-n
   ..
   auto-1
   saved registers

*/


/* an easy to debug H8 stack frame looks like:
0x6df6		push	r6
0x0d76  	mov.w   r7,r6
0x6dfn          push    reg
0x7905 nnnn  	mov.w  #n,r5    or   0x1b87  subs #2,sp
0x1957       	sub.w  r5,sp

 */

#define IS_PUSH(x) ((x & 0xff00)==0x6d00)
#define IS_LINK_8(x) ((x) == 0x17)
#define IS_LINK_16(x) ((x) == 0x1f)
#define IS_MOVE_FP(x) (x == 0x0d76)
#define IS_MOV_SP_FP(x) (x == 0x0d76)
#define IS_SUB2_SP(x) (x==0x1b87)
#define IS_MOVK_R5(x) (x==0x7905)
#define IS_SUB_R5SP(x) (x==0x1957)

#define LINK_8 0x17
#define LINK_16 0x1f

int minimum_mode = 1;
CORE_ADDR examine_prologue ();

void frame_find_saved_regs ();
CORE_ADDR
h8500_skip_prologue (start_pc)
     CORE_ADDR start_pc;

{
  short int w;

  
  w = read_memory_integer (start_pc, 1);
  if (w == LINK_8)
    {
      start_pc ++;
      w = read_memory_integer (start_pc,1);
    }

  if (w == LINK_16)
    {
      start_pc +=2;
      w = read_memory_integer (start_pc,2);
    }

  /* Skip past a move to FP */
  if (IS_MOVE_FP (w))
    {
      start_pc += 2;
      w = read_memory_short (start_pc);
    }

  /* Skip the stack adjust */

  if (IS_MOVK_R5 (w))
    {
      start_pc += 2;
      w = read_memory_short (start_pc);
    }
  if (IS_SUB_R5SP (w))
    {
      start_pc += 2;
      w = read_memory_short (start_pc);
    }
  while (IS_SUB2_SP (w))
    {
      start_pc += 2;
      w = read_memory_short (start_pc);
    }

  return start_pc;

}

int
print_insn (memaddr, stream)
     CORE_ADDR memaddr;
     FILE *stream;
{
  /* Nothing is bigger than 8 bytes */
  char data[8];
  disassemble_info info;
  read_memory (memaddr, data, sizeof (data));
  GDB_INIT_DISASSEMBLE_INFO(info, stream);
  return print_insn_h8500 (memaddr, data, &info);
}

/* Given a GDB frame, determine the address of the calling function's frame.
   This will be used to create a new GDB frame struct, and then
   INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame.

   For us, the frame address is its stack pointer value, so we look up
   the function prologue to determine the caller's sp value, and return it.  */

FRAME_ADDR
FRAME_CHAIN (thisframe)
     FRAME thisframe;
{
  static int loopcount;
  static int prevr;
  if (!inside_entry_file ((thisframe)->pc))
    {
      int v =  read_memory_integer ((thisframe)->frame, PTR_SIZE) ;

      /* Detect loops in the stack */
      if (v == prevr) loopcount++;
      else loopcount = 0;
      v = prevr;
      if (loopcount > 5) return 0;
    }
  return 0;
}

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.

   We cache the result of doing this in the frame_cache_obstack, since
   it is fairly expensive.  */
#if 0

void
frame_find_saved_regs (fi, fsr)
     struct frame_info *fi;
     struct frame_saved_regs *fsr;
{
  register CORE_ADDR next_addr;
  register CORE_ADDR *saved_regs;
  register int regnum;
  register struct frame_saved_regs *cache_fsr;
  extern struct obstack frame_cache_obstack;
  CORE_ADDR ip;
  struct symtab_and_line sal;
  CORE_ADDR limit;

  if (!fi->fsr)
    {
      cache_fsr = (struct frame_saved_regs *)
	obstack_alloc (&frame_cache_obstack,
		       sizeof (struct frame_saved_regs));
      bzero (cache_fsr, sizeof (struct frame_saved_regs));

      fi->fsr = cache_fsr;

      /* Find the start and end of the function prologue.  If the PC
	 is in the function prologue, we only consider the part that
	 has executed already.  */

      ip = get_pc_function_start (fi->pc);
      sal = find_pc_line (ip, 0);
      limit = (sal.end && sal.end < fi->pc) ? sal.end : fi->pc;

      /* This will fill in fields in *fi as well as in cache_fsr.  */
      examine_prologue (ip, limit, fi->frame, cache_fsr, fi);
    }

  if (fsr)
    *fsr = *fi->fsr;
}

#endif

/* Fetch the instruction at ADDR, returning 0 if ADDR is beyond LIM or
   is not the address of a valid instruction, the address of the next
   instruction beyond ADDR otherwise.  *PWORD1 receives the first word
   of the instruction.*/

CORE_ADDR
NEXT_PROLOGUE_INSN (addr, lim, pword1)
     CORE_ADDR addr;
     CORE_ADDR lim;
     char *pword1;
{
  if (addr < lim + 8)
    {
      read_memory (addr, pword1, 1);
      read_memory (addr, pword1 + 1, 1);
      return 1;
    }
  return 0;
}

/* Examine the prologue of a function.  `ip' points to the first instruction.
   `limit' is the limit of the prologue (e.g. the addr of the first
   linenumber, or perhaps the program counter if we're stepping through).
   `frame_sp' is the stack pointer value in use in this frame.
   `fsr' is a pointer to a frame_saved_regs structure into which we put
   info about the registers saved by this frame.
   `fi' is a struct frame_info pointer; we fill in various fields in it
   to reflect the offsets of the arg pointer and the locals pointer.  */
#if 0
static CORE_ADDR
examine_prologue (ip, limit, after_prolog_fp, fsr, fi)
     register CORE_ADDR ip;
     register CORE_ADDR limit;
     FRAME_ADDR after_prolog_fp;
     struct frame_saved_regs *fsr;
     struct frame_info *fi;
{
  register CORE_ADDR next_ip;
  int r;
  int i;
  int have_fp = 0;

  register int src;
  register struct pic_prologue_code *pcode;
  char insn[2];
  int size, offset;
  unsigned int reg_save_depth = 2;	/* Number of things pushed onto
				      stack, starts at 2, 'cause the
				      PC is already there */

  unsigned int auto_depth = 0;	/* Number of bytes of autos */

  char in_frame[8];		/* One for each reg */

  memset (in_frame, 1, 8);
  for (r = 0; r < 8; r++)
    {
      fsr->regs[r] = 0;
    }
  if (after_prolog_fp == 0)
    {
      after_prolog_fp = read_register (SP_REGNUM);
    }
  if (ip == 0 || ip & ~0xffffff)
    return 0;

  ok = NEXT_PROLOGUE_INSN (ip, limit, &insn[0]);

  /* Skip over any fp push instructions */
  fsr->regs[6] = after_prolog_fp;

  if (ok && IS_LINK_8 (insn[0]))
    {
      ip++;

      in_frame[6] = reg_save_depth;
      reg_save_depth += 2;
    }

  next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn_word);

  /* Is this a move into the fp */
  if (next_ip && IS_MOV_SP_FP (insn_word))
    {
      ip = next_ip;
      next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn_word);
      have_fp = 1;
    }

  /* Skip over any stack adjustment, happens either with a number of
     sub#2,sp or a mov #x,r5 sub r5,sp */

  if (next_ip && IS_SUB2_SP (insn_word))
    {
      while (next_ip && IS_SUB2_SP (insn_word))
	{
	  auto_depth += 2;
	  ip = next_ip;
	  next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn_word);
	}
    }
  else
    {
      if (next_ip && IS_MOVK_R5 (insn_word))
	{
	  ip = next_ip;
	  next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn_word);
	  auto_depth += insn_word;

	  next_ip = NEXT_PROLOGUE_INSN (next_ip, limit, &insn_word);
	  auto_depth += insn_word;

	}
    }
  /* Work out which regs are stored where */
  while (next_ip && IS_PUSH (insn_word))
    {
      ip = next_ip;
      next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn_word);
      fsr->regs[r] = after_prolog_fp + auto_depth;
      auto_depth += 2;
    }

  /* The args are always reffed based from the stack pointer */
  fi->args_pointer = after_prolog_fp;
  /* Locals are always reffed based from the fp */
  fi->locals_pointer = after_prolog_fp;
  /* The PC is at a known place */
  fi->from_pc = read_memory_short (after_prolog_fp + 2);

  /* Rememeber any others too */
  in_frame[PC_REGNUM] = 0;

  if (have_fp)
    /* We keep the old FP in the SP spot */
    fsr->regs[SP_REGNUM] = (read_memory_short (fsr->regs[6]));
  else
    fsr->regs[SP_REGNUM] = after_prolog_fp + auto_depth;

  return (ip);
}
#endif
#if 0
void
init_extra_frame_info (fromleaf, fi)
     int fromleaf;
     struct frame_info *fi;
{
  fi->fsr = 0;			/* Not yet allocated */
  fi->args_pointer = 0;		/* Unknown */
  fi->locals_pointer = 0;	/* Unknown */
  fi->from_pc = 0;

}
#endif

/* Return the saved PC from this frame. */

CORE_ADDR
frame_saved_pc (frame)
     FRAME frame;
{
	return read_memory_integer ((frame)->frame + 2, PTR_SIZE);
}

CORE_ADDR
frame_locals_address (fi)
     struct frame_info *fi;
{
  return fi->frame;
}

/* Return the address of the argument block for the frame
   described by FI.  Returns 0 if the address is unknown.  */

CORE_ADDR
frame_args_address (fi)
     struct frame_info *fi;
{
  return fi->frame + PTR_SIZE;	/* Skip the PC */
}

void
h8300_pop_frame ()
{
  unsigned regnum;
  struct frame_saved_regs fsr;
  struct frame_info *fi;

  FRAME frame = get_current_frame ();

  fi = get_frame_info (frame);
  get_frame_saved_regs (fi, &fsr);

  for (regnum = 0; regnum < 8; regnum++)
    {
      if (fsr.regs[regnum])
	{
	  write_register (regnum, read_memory_short (fsr.regs[regnum]));
	}

      flush_cached_frames ();
      set_current_frame (create_new_frame (read_register (FP_REGNUM),
					   read_pc ()));

    }

}

void
print_register_hook (regno)
{
  if (regno == CCR_REGNUM)
    {
      /* CCR register */

      int C, Z, N, V;
      unsigned char b[2];
      unsigned char l;

      read_relative_register_raw_bytes (regno, b);
      l = b[1];
      printf ("\t");
      printf ("I-%d - ", (l & 0x80) != 0);
      N = (l & 0x8) != 0;
      Z = (l & 0x4) != 0;
      V = (l & 0x2) != 0;
      C = (l & 0x1) != 0;
      printf ("N-%d ", N);
      printf ("Z-%d ", Z);
      printf ("V-%d ", V);
      printf ("C-%d ", C);
      if ((C | Z) == 0)
	printf ("u> ");
      if ((C | Z) == 1)
	printf ("u<= ");
      if ((C == 0))
	printf ("u>= ");
      if (C == 1)
	printf ("u< ");
      if (Z == 0)
	printf ("!= ");
      if (Z == 1)
	printf ("== ");
      if ((N ^ V) == 0)
	printf (">= ");
      if ((N ^ V) == 1)
	printf ("< ");
      if ((Z | (N ^ V)) == 0)
	printf ("> ");
      if ((Z | (N ^ V)) == 1)
	printf ("<= ");
    }
}



#if 0
register_byte (N)
{
  return reginfo[N].offset;
}
#endif
register_raw_size (N)
{
  if (N <= R7) return 2;
  return 4;
}

register_virtual_size (N)
{
  if (N <= R7) return 2;
  return 4;
}



register_convert_to_raw (regnum, from, to)
     int regnum;
     char *from;
     char *to;
{
  switch (regnum)
    {
    case PR0:
    case PR1:
    case PR2:
    case PR3:
    case PR4:
    case PR5:
    case PR6:
    case PR7:
    case PC_REGNUM:
      to[0] = 0;
      to[1] = from[1];
      to[2] = from[2];
      to[3] = from[3];
      break;
    default:
      to[0] = from[0];
      to[1] = from[1];
      break;
    }
}


register_convert_to_virtual (regnum, from, to)
     int regnum;
     char *from;
     char *to;
{
  switch (regnum)
    {
    case PR0:
    case PR1:
    case PR2:
    case PR3:
    case PR4:
    case PR5:
    case PR6:
    case PR7:
    case PC_REGNUM:
      to[0] = 0;
      to[1] = from[1];
      to[2] = from[2];
      to[3] = from[3];
      break;
    default:
      to[0] = from[0];
      to[1] = from[1];
      break;
    }
}

struct type *
register_virtual_type (N)
{
  switch (N)
    {
      /* Although these are actually word size registers, we treat them
	 like longs so that we can deal with any implicit segmentation */
    case PR0:
    case PR1:
    case PR2:
    case PR3:
    case PR4:
    case PR5:
    case PR6:
    case PR7:
    case PC_REGNUM:
      return builtin_type_unsigned_long;
    case SEG_C:
    case SEG_E:
    case SEG_D:
    case SEG_T:
      return builtin_type_unsigned_char;
    case R0:
    case R1:
    case R2:
    case R3:
    case R4:
    case R5:
    case R6:
    case R7:
    case CCR_REGNUM:
      return builtin_type_unsigned_short;


    default:
      abort();
    }
}




/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

void
frame_find_saved_regs (frame_info, frame_saved_regs)
     struct frame_info *frame_info;
     struct frame_saved_regs *frame_saved_regs;

{
  register int regnum;
  register int regmask;
  register CORE_ADDR next_addr;
  register CORE_ADDR pc;
  unsigned char thebyte;

  bzero (frame_saved_regs, sizeof *frame_saved_regs);

  if ((frame_info)->pc >= (frame_info)->frame - CALL_DUMMY_LENGTH - FP_REGNUM * 4 - 4
      && (frame_info)->pc <= (frame_info)->frame)
    {
      next_addr = (frame_info)->frame;
      pc = (frame_info)->frame - CALL_DUMMY_LENGTH - FP_REGNUM * 4 - 4;
    }
  else
    {
      pc = get_pc_function_start ((frame_info)->pc);
      /* Verify we have a link a6 instruction next;
	 if not we lose.  If we win, find the address above the saved
	 regs using the amount of storage from the link instruction.
	 */

      thebyte  = read_memory_integer(pc, 1);
      if (0x1f == thebyte)
	next_addr = (frame_info)->frame + read_memory_integer (pc += 1, 2), pc += 2;
      else if (0x17 == thebyte)
	next_addr = (frame_info)->frame + read_memory_integer (pc += 1, 1), pc += 1;
      else
	goto lose;
#if 0
      fixme steve
	/* If have an add:g.waddal #-n, sp next, adjust next_addr.  */
	if ((0x0c0177777 & read_memory_integer (pc, 2)) == 0157774)
	  next_addr += read_memory_integer (pc += 2, 4), pc += 4;
#endif
    }

  thebyte = read_memory_integer(pc, 1);
  if (thebyte == 0x12) {
    /* Got stm */
    pc++;
    regmask = read_memory_integer(pc,1);
    pc++;
    for (regnum = 0; regnum < 8; regnum ++, regmask >>=1) 
      {
	if (regmask & 1) 
	  {
	    (frame_saved_regs)->regs[regnum] = (next_addr += 2) - 2;
	  }
      }
    thebyte = read_memory_integer(pc, 1);
  }
  /* Maybe got a load of pushes */
  while (thebyte == 0xbf) {
    pc++;
    regnum = read_memory_integer(pc,1) & 0x7;
    pc++;
    (frame_saved_regs)->regs[regnum] = (next_addr += 2) - 2;    
    thebyte = read_memory_integer(pc, 1);
  }

 lose:;
  
  /* Remember the address of the frame pointer */
  (frame_saved_regs)->regs[FP_REGNUM] = (frame_info)->frame;

  /* This is where the old sp is hidden */
  (frame_saved_regs)->regs[SP_REGNUM] = (frame_info)->frame;

  /* And the PC - remember the pushed FP is always two bytes long */
  (frame_saved_regs)->regs[PC_REGNUM] = (frame_info)->frame + 2;
}

saved_pc_after_call(frame)
{
  int x;
  int a = read_register(SP_REGNUM);
  x = read_memory_integer (a, PTR_SIZE);
  return x;
}


/* Nonzero if instruction at PC is a return instruction.  */

about_to_return(pc)
{
  int b1 = read_memory_integer(pc,1);

  switch (b1) 
    {
    case 0x14:			/* rtd #8 */
    case 0x1c:			/* rtd #16 */
    case 0x19:			/* rts */
    case 0x1a:			/* rte */
      return 1;
    case 0x11:
      {
	int b2 = read_memory_integer(pc+1,1);
	switch (b2) 
	  {
	  case 0x18:		/* prts */
	  case 0x14:		/* prtd #8 */
	  case 0x16:		/* prtd #16 */
	    return 1;
	  }
      }
    }
  return 0;
}


void
h8500_set_pointer_size (newsize)
     int newsize;
{
  static int oldsize = 0;

  if (oldsize != newsize)
    {
      printf ("pointer size set to %d bits\n", newsize);
      oldsize = newsize;
      if (newsize == 32)
	{
	  minimum_mode = 0;
	}
      else
	{
	  minimum_mode = 1;
	}
      _initialize_gdbtypes ();
    }
}


struct cmd_list_element *setmemorylist;


static void
segmented_command (args, from_tty)
     char *args;
     int from_tty;
{
  h8500_set_pointer_size (32);
}

static void
unsegmented_command (args, from_tty)
     char *args;
     int from_tty;
{
  h8500_set_pointer_size (16);
}

static void
set_memory (args, from_tty)
     char *args;
     int from_tty;
{
  printf ("\"set memory\" must be followed by the name of a memory subcommand.\n");
  help_list (setmemorylist, "set memory ", -1, stdout);
}


_initialize_h8500_tdep ()
{
  /* Sanitity check a few things */
  if (FP_REGNUM != GPR6
      || SP_REGNUM != GPR7
      || CCR_REGNUM != GCCR
      || PC_REGNUM != GPC
      || SEG_C != GSEGC
      || SEG_D != GSEGD
      || SEG_E != GSEGE
      || SEG_T != GSEGT
      || PR0 != GPR0
      || PR1 != GPR1
      || PR2 != GPR2
      || PR3 != GPR3
      || PR4 != GPR4
      || PR5 != GPR5
      || PR6 != GPR6
      || PR7 != GPR7)
    abort ();

  add_prefix_cmd ("memory", no_class, set_memory,
		  "set the memory model", &setmemorylist, "set memory ", 0,
		  &setlist);
  add_cmd ("segmented", class_support, segmented_command,
	   "Set segmented memory model.", &setmemorylist);
  add_cmd ("unsegmented", class_support, unsegmented_command,
	   "Set unsegmented memory model.", &setmemorylist);

}
