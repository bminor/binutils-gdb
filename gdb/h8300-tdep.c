/* Target-machine dependent code for Hitachi H8/300, for GDB.

   Copyright 1988, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1998,
   1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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
#include "value.h"
#include "inferior.h"
#include "symfile.h"
#include "arch-utils.h"
#include "regcache.h"
#include "gdbcore.h"
#include "objfiles.h"
#include "gdbcmd.h"
#include "gdb_assert.h"

/* Extra info which is saved in each frame_info. */
struct frame_extra_info
{
  CORE_ADDR from_pc;
  CORE_ADDR args_pointer;
  CORE_ADDR locals_pointer;
};

#define E_NUM_REGS (h8300smode ? 14 : 13)

enum
{
  h8300_reg_size = 2,
  h8300h_reg_size = 4,
  h8300_max_reg_size = 4,
};
#define BINWORD (h8300hmode ? h8300h_reg_size : h8300_reg_size)

enum gdb_regnum
{
  E_R0_REGNUM, E_ER0_REGNUM = E_R0_REGNUM, E_ARG0_REGNUM = E_R0_REGNUM,
  E_R1_REGNUM, E_ER1_REGNUM = E_R1_REGNUM,
  E_R2_REGNUM, E_ER2_REGNUM = E_R2_REGNUM, E_ARGLAST_REGNUM = E_R2_REGNUM,
  E_R3_REGNUM, E_ER3_REGNUM = E_R3_REGNUM,
  E_R4_REGNUM, E_ER4_REGNUM = E_R4_REGNUM,
  E_R5_REGNUM, E_ER5_REGNUM = E_R5_REGNUM,
  E_R6_REGNUM, E_ER6_REGNUM = E_R6_REGNUM, E_FP_REGNUM = E_R6_REGNUM,
  E_SP_REGNUM,
  E_CCR_REGNUM,
  E_PC_REGNUM,
  E_CYCLES_REGNUM,
  E_TICK_REGNUM, E_EXR_REGNUM = E_TICK_REGNUM,
  E_INST_REGNUM, E_TICKS_REGNUM = E_INST_REGNUM,
  E_INSTS_REGNUM
};

#define UNSIGNED_SHORT(X) ((X) & 0xffff)

#define IS_PUSH(x) ((x & 0xfff0)==0x6df0)
#define IS_PUSH_FP(x) (x == 0x6df6)
#define IS_MOVE_FP(x) (x == 0x0d76 || x == 0x0ff6)
#define IS_MOV_SP_FP(x) (x == 0x0d76 || x == 0x0ff6)
#define IS_SUB2_SP(x) (x==0x1b87)
#define IS_SUB4_SP(x) (x==0x1b97)
#define IS_SUBL_SP(x) (x==0x7a37)
#define IS_MOVK_R5(x) (x==0x7905)
#define IS_SUB_R5SP(x) (x==0x1957)

/* If the instruction at PC is an argument register spill, return its
   length.  Otherwise, return zero.

   An argument register spill is an instruction that moves an argument
   from the register in which it was passed to the stack slot in which
   it really lives.  It is a byte, word, or longword move from an
   argument register to a negative offset from the frame pointer.  */

static int
h8300_is_argument_spill (CORE_ADDR pc)
{
  int w = read_memory_unsigned_integer (pc, 2);

  if ((w & 0xfff0) == 0x6ee0                  /* mov.b Rs,@(d:16,er6) */
      && 8 <= (w & 0xf) && (w & 0xf) <= 10)   /* Rs is R0L, R1L, or R2L  */
    {
      int w2 = read_memory_integer (pc + 2, 2);

      /* ... and d:16 is negative.  */
      if (w2 < 0)
        return 4;
    }
  else if (w == 0x7860)
    {
      int w2 = read_memory_integer (pc + 2, 2);

      if ((w2 & 0xfff0) == 0x6aa0)              /* mov.b Rs, @(d:24,er6) */
        {
          LONGEST disp = read_memory_integer (pc + 4, 4);

          /* ... and d:24 is negative.  */
          if (disp < 0 && disp > 0xffffff)
            return 8;
        }
    }
  else if ((w & 0xfff0) == 0x6fe0             /* mov.w Rs,@(d:16,er6) */
           && (w & 0xf) <= 2)                 /* Rs is R0, R1, or R2 */
    {
      int w2 = read_memory_integer (pc + 2, 2);

      /* ... and d:16 is negative.  */
      if (w2 < 0)
        return 4;
    }
  else if (w == 0x78e0)
    {
      int w2 = read_memory_integer (pc + 2, 2);

      if ((w2 & 0xfff0) == 0x6ba0)              /* mov.b Rs, @(d:24,er6) */
        {
          LONGEST disp = read_memory_integer (pc + 4, 4);

          /* ... and d:24 is negative.  */
          if (disp < 0 && disp > 0xffffff)
            return 8;
        }
    }
  else if (w == 0x0100)
    {
      int w2 = read_memory_integer (pc + 2, 2);

      if ((w2 & 0xfff0) == 0x6fe0             /* mov.l Rs,@(d:16,er6) */
          && (w2 & 0xf) <= 2)                /* Rs is ER0, ER1, or ER2 */
        {
          int w3 = read_memory_integer (pc + 4, 2);

          /* ... and d:16 is negative.  */
          if (w3 < 0)
            return 6;
        }
      else if (w2 == 0x78e0)
        {
          int w3 = read_memory_integer (pc + 4, 2);

          if ((w3 & 0xfff0) == 0x6ba0)          /* mov.l Rs, @(d:24,er6) */
            {
              LONGEST disp = read_memory_integer (pc + 6, 4);

              /* ... and d:24 is negative.  */
              if (disp < 0 && disp > 0xffffff)
                return 10;
            }
        }
    }

  return 0;
}

static CORE_ADDR
h8300_skip_prologue (CORE_ADDR start_pc)
{
  short int w;
  int adjust = 0;

  /* Skip past all push and stm insns.  */
  while (1)
    {
      w = read_memory_unsigned_integer (start_pc, 2);
      /* First look for push insns.  */
      if (w == 0x0100 || w == 0x0110 || w == 0x0120 || w == 0x0130)
	{
	  w = read_memory_unsigned_integer (start_pc + 2, 2);
	  adjust = 2;
	}

      if (IS_PUSH (w))
	{
	  start_pc += 2 + adjust;
	  w = read_memory_unsigned_integer (start_pc, 2);
	  continue;
	}
      adjust = 0;
      break;
    }

  /* Skip past a move to FP, either word or long sized */
  w = read_memory_unsigned_integer (start_pc, 2);
  if (w == 0x0100)
    {
      w = read_memory_unsigned_integer (start_pc + 2, 2);
      adjust += 2;
    }

  if (IS_MOVE_FP (w))
    {
      start_pc += 2 + adjust;
      w = read_memory_unsigned_integer (start_pc, 2);
    }

  /* Check for loading either a word constant into r5;
     long versions are handled by the SUBL_SP below.  */
  if (IS_MOVK_R5 (w))
    {
      start_pc += 2;
      w = read_memory_unsigned_integer (start_pc, 2);
    }

  /* Now check for subtracting r5 from sp, word sized only.  */
  if (IS_SUB_R5SP (w))
    {
      start_pc += 2 + adjust;
      w = read_memory_unsigned_integer (start_pc, 2);
    }

  /* Check for subs #2 and subs #4. */
  while (IS_SUB2_SP (w) || IS_SUB4_SP (w))
    {
      start_pc += 2 + adjust;
      w = read_memory_unsigned_integer (start_pc, 2);
    }

  /* Check for a 32bit subtract.  */
  if (IS_SUBL_SP (w))
    start_pc += 6 + adjust;

  /* Check for spilling an argument register to the stack frame.
     This could also be an initializing store from non-prologue code,
     but I don't think there's any harm in skipping that.  */
  for (;;)
    {
      int spill_size = h8300_is_argument_spill (start_pc);
      if (spill_size == 0)
        break;
      start_pc += spill_size;
    }

  return start_pc;
}

static int
gdb_print_insn_h8300 (bfd_vma memaddr, disassemble_info * info)
{
  if (h8300smode)
    return print_insn_h8300s (memaddr, info);
  else if (h8300hmode)
    return print_insn_h8300h (memaddr, info);
  else
    return print_insn_h8300 (memaddr, info);
}

/* Fetch the instruction at ADDR, returning 0 if ADDR is beyond LIM or
   is not the address of a valid instruction, the address of the next
   instruction beyond ADDR otherwise.  *PWORD1 receives the first word
   of the instruction. */

static CORE_ADDR
h8300_next_prologue_insn (CORE_ADDR addr, CORE_ADDR lim, unsigned short* pword1)
{
  char buf[2];
  if (addr < lim + 8)
    {
      read_memory (addr, buf, 2);
      *pword1 = extract_signed_integer (buf, 2);

      return addr + 2;
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

/* Any function with a frame looks like this
   SECOND ARG
   FIRST ARG
   RET PC
   SAVED R2
   SAVED R3
   SAVED FP   <-FP POINTS HERE
   LOCALS0
   LOCALS1    <-SP POINTS HERE
 */

static CORE_ADDR
h8300_examine_prologue (register CORE_ADDR ip, register CORE_ADDR limit,
			CORE_ADDR after_prolog_fp, CORE_ADDR *fsr,
			struct frame_info *fi)
{
  register CORE_ADDR next_ip;
  int r;
  int have_fp = 0;
  unsigned short insn_word;
  /* Number of things pushed onto stack, starts at 2/4, 'cause the
     PC is already there */
  unsigned int reg_save_depth = BINWORD;

  unsigned int auto_depth = 0;	/* Number of bytes of autos */

  char in_frame[11];		/* One for each reg */

  int adjust = 0;

  memset (in_frame, 1, 11);
  for (r = 0; r < 8; r++)
    {
      fsr[r] = 0;
    }
  if (after_prolog_fp == 0)
    {
      after_prolog_fp = read_register (E_SP_REGNUM);
    }

  /* If the PC isn't valid, quit now.  */
  if (ip == 0 || ip & (h8300hmode ? ~0xffffff : ~0xffff))
    return 0;

  next_ip = h8300_next_prologue_insn (ip, limit, &insn_word);

  if (insn_word == 0x0100)
    {
      insn_word = read_memory_unsigned_integer (ip + 2, 2);
      adjust = 2;
    }

  /* Skip over any fp push instructions */
  fsr[E_FP_REGNUM] = after_prolog_fp;
  while (next_ip && IS_PUSH_FP (insn_word))
    {
      ip = next_ip + adjust;

      in_frame[insn_word & 0x7] = reg_save_depth;
      next_ip = h8300_next_prologue_insn (ip, limit, &insn_word);
      reg_save_depth += 2 + adjust;
    }

  /* Is this a move into the fp */
  if (next_ip && IS_MOV_SP_FP (insn_word))
    {
      ip = next_ip;
      next_ip = h8300_next_prologue_insn (ip, limit, &insn_word);
      have_fp = 1;
    }

  /* Skip over any stack adjustment, happens either with a number of
     sub#2,sp or a mov #x,r5 sub r5,sp */

  if (next_ip && (IS_SUB2_SP (insn_word) || IS_SUB4_SP (insn_word)))
    {
      while (next_ip && (IS_SUB2_SP (insn_word) || IS_SUB4_SP (insn_word)))
	{
	  auto_depth += IS_SUB2_SP (insn_word) ? 2 : 4;
	  ip = next_ip;
	  next_ip = h8300_next_prologue_insn (ip, limit, &insn_word);
	}
    }
  else
    {
      if (next_ip && IS_MOVK_R5 (insn_word))
	{
	  ip = next_ip;
	  next_ip = h8300_next_prologue_insn (ip, limit, &insn_word);
	  auto_depth += insn_word;

	  next_ip = h8300_next_prologue_insn (next_ip, limit, &insn_word);
	  auto_depth += insn_word;
	}
      if (next_ip && IS_SUBL_SP (insn_word))
	{
	  ip = next_ip;
	  auto_depth += read_memory_unsigned_integer (ip, 4);
	  ip += 4;

	  next_ip = h8300_next_prologue_insn (ip, limit, &insn_word);
	}
    }

  /* Now examine the push insns to determine where everything lives
     on the stack.  */
  while (1)
    {
      adjust = 0;
      if (!next_ip)
	break;

      if (insn_word == 0x0100)
	{
	  ip = next_ip;
	  next_ip = h8300_next_prologue_insn (ip, limit, &insn_word);
	  adjust = 2;
	}

      if (IS_PUSH (insn_word))
	{
	  auto_depth += 2 + adjust;
	  fsr[insn_word & 0x7] = after_prolog_fp - auto_depth;
	  ip = next_ip;
	  next_ip = h8300_next_prologue_insn (ip, limit, &insn_word);
	  continue;
	}

      /* Now check for push multiple insns.  */
      if (insn_word == 0x0110 || insn_word == 0x0120 || insn_word == 0x0130)
	{
	  int count = ((insn_word >> 4) & 0xf) + 1;
	  int start, i;

	  ip = next_ip;
	  next_ip = h8300_next_prologue_insn (ip, limit, &insn_word);
	  start = insn_word & 0x7;

	  for (i = start; i < start + count; i++)
	    {
	      auto_depth += 4;
	      fsr[i] = after_prolog_fp - auto_depth;
	    }
	}
      break;
    }

  /* The args are always reffed based from the stack pointer */
  fi->extra_info->args_pointer = after_prolog_fp;
  /* Locals are always reffed based from the fp */
  fi->extra_info->locals_pointer = after_prolog_fp;
  /* The PC is at a known place */
  fi->extra_info->from_pc =
    read_memory_unsigned_integer (after_prolog_fp + BINWORD, BINWORD);

  /* Rememeber any others too */
  in_frame[E_PC_REGNUM] = 0;

  if (have_fp)
    /* We keep the old FP in the SP spot */
    fsr[E_SP_REGNUM] = read_memory_unsigned_integer (fsr[E_FP_REGNUM], BINWORD);
  else
    fsr[E_SP_REGNUM] = after_prolog_fp + auto_depth;

  return (ip);
}

static void
h8300_frame_init_saved_regs (struct frame_info *fi)
{
  CORE_ADDR func_addr, func_end;

  if (!fi->saved_regs)
    {
      frame_saved_regs_zalloc (fi);

      /* Find the beginning of this function, so we can analyze its
	 prologue. */
      if (find_pc_partial_function (fi->pc, NULL, &func_addr, &func_end))
        {
	  struct symtab_and_line sal = find_pc_line (func_addr, 0);
	  CORE_ADDR limit = (sal.end && sal.end < fi->pc) ? sal.end : fi->pc;
	  /* This will fill in fields in fi. */
	  h8300_examine_prologue (func_addr, limit, fi->frame, fi->saved_regs, fi);
	}
      /* Else we're out of luck (can't debug completely stripped code). 
	 FIXME. */
    }
}

/* Given a GDB frame, determine the address of the calling function's frame.
   This will be used to create a new GDB frame struct, and then
   INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame.

   For us, the frame address is its stack pointer value, so we look up
   the function prologue to determine the caller's sp value, and return it.  */

static CORE_ADDR
h8300_frame_chain (struct frame_info *thisframe)
{
  if (DEPRECATED_PC_IN_CALL_DUMMY (thisframe->pc, thisframe->frame, thisframe->frame))
    {				/* initialize the from_pc now */
      thisframe->extra_info->from_pc =
	deprecated_read_register_dummy (thisframe->pc, thisframe->frame,
					E_PC_REGNUM);
      return thisframe->frame;
    }
  return thisframe->saved_regs[E_SP_REGNUM];
}

/* Return the saved PC from this frame.

   If the frame has a memory copy of SRP_REGNUM, use that.  If not,
   just use the register SRP_REGNUM itself.  */

static CORE_ADDR
h8300_frame_saved_pc (struct frame_info *frame)
{
  if (DEPRECATED_PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
    return deprecated_read_register_dummy (frame->pc, frame->frame,
					   E_PC_REGNUM);
  else
    return frame->extra_info->from_pc;
}

static void
h8300_init_extra_frame_info (int fromleaf, struct frame_info *fi)
{
  if (!fi->extra_info)
    {
      fi->extra_info = (struct frame_extra_info *)
        frame_obstack_alloc (sizeof (struct frame_extra_info));
      fi->extra_info->from_pc = 0;
      fi->extra_info->args_pointer = 0;		/* Unknown */
      fi->extra_info->locals_pointer = 0;	/* Unknown */
      
      if (!fi->pc)
        {
	  if (fi->next)
	    fi->pc = h8300_frame_saved_pc (fi->next);
	}
      h8300_frame_init_saved_regs (fi);
    }
}

static CORE_ADDR
h8300_frame_locals_address (struct frame_info *fi)
{
  if (DEPRECATED_PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
    return (CORE_ADDR) 0;	/* Not sure what else to do... */
  return fi->extra_info->locals_pointer;
}

/* Return the address of the argument block for the frame
   described by FI.  Returns 0 if the address is unknown.  */

static CORE_ADDR
h8300_frame_args_address (struct frame_info *fi)
{
  if (DEPRECATED_PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
    return (CORE_ADDR) 0;	/* Not sure what else to do... */
  return fi->extra_info->args_pointer;
}

/* Round N up or down to the nearest multiple of UNIT.
   Evaluate N only once, UNIT several times.
   UNIT must be a power of two.  */
#define round_up(n, unit)   (((n) + (unit) - 1) & -(unit))
#define round_down(n, unit) ((n) & -(unit))

/* Function: push_arguments
   Setup the function arguments for calling a function in the inferior.
   In this discussion, a `word' is 16 bits on the H8/300s, and 32 bits
   on the H8/300H.

   There are actually two ABI's here: -mquickcall (the default) and
   -mno-quickcall.  With -mno-quickcall, all arguments are passed on
   the stack after the return address, word-aligned.  With
   -mquickcall, GCC tries to use r0 -- r2 to pass registers.  Since
   GCC doesn't indicate in the object file which ABI was used to
   compile it, GDB only supports the default --- -mquickcall.

   Here are the rules for -mquickcall, in detail:

   Each argument, whether scalar or aggregate, is padded to occupy a
   whole number of words.  Arguments smaller than a word are padded at
   the most significant end; those larger than a word are padded at
   the least significant end.

   The initial arguments are passed in r0 -- r2.  Earlier arguments go in
   lower-numbered registers.  Multi-word arguments are passed in
   consecutive registers, with the most significant end in the
   lower-numbered register.

   If an argument doesn't fit entirely in the remaining registers, it
   is passed entirely on the stack.  Stack arguments begin just after
   the return address.  Once an argument has overflowed onto the stack
   this way, all subsequent arguments are passed on the stack.

   The above rule has odd consequences.  For example, on the h8/300s,
   if a function takes two longs and an int as arguments:
   - the first long will be passed in r0/r1,
   - the second long will be passed entirely on the stack, since it
     doesn't fit in r2,
   - and the int will be passed on the stack, even though it could fit
     in r2.

   A weird exception: if an argument is larger than a word, but not a
   whole number of words in length (before padding), it is passed on
   the stack following the rules for stack arguments above, even if
   there are sufficient registers available to hold it.  Stranger
   still, the argument registers are still `used up' --- even though
   there's nothing in them.

   So, for example, on the h8/300s, if a function expects a three-byte
   structure and an int, the structure will go on the stack, and the
   int will go in r2, not r0.
  
   If the function returns an aggregate type (struct, union, or class)
   by value, the caller must allocate space to hold the return value,
   and pass the callee a pointer to this space as an invisible first
   argument, in R0.

   For varargs functions, the last fixed argument and all the variable
   arguments are always passed on the stack.  This means that calls to
   varargs functions don't work properly unless there is a prototype
   in scope.

   Basically, this ABI is not good, for the following reasons:
   - You can't call vararg functions properly unless a prototype is in scope.
   - Structure passing is inconsistent, to no purpose I can see.
   - It often wastes argument registers, of which there are only three
     to begin with.  */

static CORE_ADDR
h8300_push_arguments (int nargs, struct value **args, CORE_ADDR sp,
		      int struct_return, CORE_ADDR struct_addr)
{
  int stack_align, stack_alloc, stack_offset;
  int wordsize = BINWORD;
  int reg;
  int argument;

  /* First, make sure the stack is properly aligned.  */
  sp = round_down (sp, wordsize);

  /* Now make sure there's space on the stack for the arguments.  We
     may over-allocate a little here, but that won't hurt anything.  */
  stack_alloc = 0;
  for (argument = 0; argument < nargs; argument++)
    stack_alloc += round_up (TYPE_LENGTH (VALUE_TYPE (args[argument])),
                             wordsize);
  sp -= stack_alloc;

  /* Now load as many arguments as possible into registers, and push
     the rest onto the stack.  */
  reg = E_ARG0_REGNUM;
  stack_offset = 0;

  /* If we're returning a structure by value, then we must pass a
     pointer to the buffer for the return value as an invisible first
     argument.  */
  if (struct_return)
    write_register (reg++, struct_addr);

  for (argument = 0; argument < nargs; argument++)
    {
      struct type *type = VALUE_TYPE (args[argument]);
      int len = TYPE_LENGTH (type);
      char *contents = (char *) VALUE_CONTENTS (args[argument]);

      /* Pad the argument appropriately.  */
      int padded_len = round_up (len, wordsize);
      char *padded = alloca (padded_len);

      memset (padded, 0, padded_len);
      memcpy (len < wordsize ? padded + padded_len - len : padded,
              contents, len);

      /* Could the argument fit in the remaining registers?  */
      if (padded_len <= (E_ARGLAST_REGNUM - reg + 1) * wordsize)
        {
          /* Are we going to pass it on the stack anyway, for no good
             reason?  */
          if (len > wordsize && len % wordsize)
            {
              /* I feel so unclean.  */
              write_memory (sp + stack_offset, padded, padded_len);
              stack_offset += padded_len;

              /* That's right --- even though we passed the argument
                 on the stack, we consume the registers anyway!  Love
                 me, love my dog.  */
              reg += padded_len / wordsize;
            }
          else
            {
              /* Heavens to Betsy --- it's really going in registers!
                 It would be nice if we could use write_register_bytes
                 here, but on the h8/300s, there are gaps between
                 the registers in the register file.  */
              int offset;

              for (offset = 0; offset < padded_len; offset += wordsize)
                {
                  ULONGEST word = extract_address (padded + offset, wordsize);
                  write_register (reg++, word);
                }
            }
        }
      else
        {
          /* It doesn't fit in registers!  Onto the stack it goes.  */
          write_memory (sp + stack_offset, padded, padded_len);
          stack_offset += padded_len;

          /* Once one argument has spilled onto the stack, all
             subsequent arguments go on the stack.  */
          reg = E_ARGLAST_REGNUM + 1;
        }
    }

  return sp;
}

/* Function: push_return_address
   Setup the return address for a dummy frame, as called by
   call_function_by_hand.  Only necessary when you are using an
   empty CALL_DUMMY, ie. the target will not actually be executing
   a JSR/BSR instruction.  */

static CORE_ADDR
h8300_push_return_address (CORE_ADDR pc, CORE_ADDR sp)
{
  unsigned char buf[4];
  int wordsize = BINWORD;

  sp -= wordsize;
  store_unsigned_integer (buf, wordsize, CALL_DUMMY_ADDRESS ());
  write_memory (sp, buf, wordsize);
  return sp;
}

/* Function: h8300_pop_frame
   Restore the machine to the state it had before the current frame 
   was created.  Usually used either by the "RETURN" command, or by
   call_function_by_hand after the dummy_frame is finished. */

static void
h8300_pop_frame (void)
{
  unsigned regno;
  struct frame_info *frame = get_current_frame ();

  if (DEPRECATED_PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
    {
      generic_pop_dummy_frame ();
    }
  else
    {
      for (regno = 0; regno < 8; regno++)
	{
	  /* Don't forget E_SP_REGNUM is a frame_saved_regs struct is the
	     actual value we want, not the address of the value we want.  */
	  if (frame->saved_regs[regno] && regno != E_SP_REGNUM)
	    write_register (regno,
			    read_memory_integer (frame->saved_regs[regno],
			    			 BINWORD));
	  else if (frame->saved_regs[regno] && regno == E_SP_REGNUM)
	    write_register (regno, frame->frame + 2 * BINWORD);
	}

      /* Don't forget to update the PC too!  */
      write_register (E_PC_REGNUM, frame->extra_info->from_pc);
    }
  flush_cached_frames ();
}

/* Function: extract_return_value
   Figure out where in REGBUF the called function has left its return value.
   Copy that into VALBUF.  Be sure to account for CPU type.   */

static void
h8300_extract_return_value (struct type *type, char *regbuf, char *valbuf)
{
  int wordsize = BINWORD;
  int len = TYPE_LENGTH (type);

  switch (len)
    {
    case 1:			/* (char) */
    case 2:			/* (short), (int) */
      memcpy (valbuf, regbuf + REGISTER_BYTE (0) + (wordsize - len), len);
      break;
    case 4:			/* (long), (float) */
      if (wordsize == 4)
	{
	  memcpy (valbuf, regbuf + REGISTER_BYTE (0), 4);
	}
      else
	{
	  memcpy (valbuf, regbuf + REGISTER_BYTE (0), 2);
	  memcpy (valbuf + 2, regbuf + REGISTER_BYTE (1), 2);
	}
      break;
    case 8:			/* (double) (doesn't seem to happen, which is good,
				   because this almost certainly isn't right.  */
      error ("I don't know how a double is returned.");
      break;
    }
}

/* Function: store_return_value
   Place the appropriate value in the appropriate registers.
   Primarily used by the RETURN command.  */

static void
h8300_store_return_value (struct type *type, char *valbuf)
{
  int regval;
  int wordsize = BINWORD;
  int len = TYPE_LENGTH (type);

  switch (len)
    {
    case 1:			/* char */
    case 2:			/* short, int */
      regval = extract_address (valbuf, len);
      write_register (0, regval);
      break;
    case 4:			/* long, float */
      regval = extract_address (valbuf, len);
      if (wordsize == 4)
	{
	  write_register (0, regval);
	}
      else
	{
	  write_register (0, regval >> 16);
	  write_register (1, regval & 0xffff);
	}
      break;
    case 8:			/* presumeably double, but doesn't seem to happen */
      error ("I don't know how to return a double.");
      break;
    }
}

static struct cmd_list_element *setmachinelist;

static const char *
h8300_register_name (int regno)
{
  /* The register names change depending on whether the h8300h processor
     type is selected. */
  static char *h8300_register_names[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6",
    "sp", "ccr","pc","cycles", "tick", "inst", ""
  };
  static char *h8300s_register_names[] = {
    "er0", "er1", "er2", "er3", "er4", "er5", "er6",
    "sp", "ccr", "pc", "cycles", "exr", "tick", "inst"
  };
  char **register_names =
  		h8300smode ? h8300s_register_names : h8300_register_names;
  if (regno < 0 || regno >= E_NUM_REGS)
    internal_error (__FILE__, __LINE__,
		    "h8300_register_name: illegal register number %d", regno);
  else
    return register_names[regno];
}

static void
h8300_print_register (struct gdbarch *gdbarch, struct ui_file *file,
		      struct frame_info *frame, int regno)
{
  ULONGEST rval;
  long val;
  const char *name = h8300_register_name (regno);

  if (!name || !*name)
    return;

  /* FIXME: cagney/2002-10-22: The code below assumes that VAL is at
     least 4 bytes (32 bits) in size and hence is large enough to hold
     the largest h8300 register.  Should instead be using ULONGEST and
     the phex() functions.  */
  gdb_assert (sizeof (val) >= 4);
  frame_read_unsigned_register (frame, regno, &rval);
  val = rval;

  fprintf_filtered (file, "%-14s ", name);
  if (h8300hmode)
    {
      if (val)
	fprintf_filtered (file, "0x%08lx   %-8ld", val, val);
      else
	fprintf_filtered (file, "0x%-8lx   %-8ld", val, val);
    }
  else
    {
      if (val)
	fprintf_filtered (file, "0x%04lx   %-4ld", val, val);
      else
	fprintf_filtered (file, "0x%-4lx   %-4ld", val, val);
    }
  if (regno == E_CCR_REGNUM)
    {
      /* CCR register */
      int C, Z, N, V;
      unsigned char b[h8300h_reg_size];
      unsigned char l;
      frame_register_read (deprecated_selected_frame, regno, b);
      l = b[REGISTER_VIRTUAL_SIZE (E_CCR_REGNUM) - 1];
      fprintf_filtered (file, "\t");
      fprintf_filtered (file, "I-%d ", (l & 0x80) != 0);
      fprintf_filtered (file, "UI-%d ", (l & 0x40) != 0);
      fprintf_filtered (file, "H-%d ", (l & 0x20) != 0);
      fprintf_filtered (file, "U-%d ", (l & 0x10) != 0);
      N = (l & 0x8) != 0;
      Z = (l & 0x4) != 0;
      V = (l & 0x2) != 0;
      C = (l & 0x1) != 0;
      fprintf_filtered (file, "N-%d ", N);
      fprintf_filtered (file, "Z-%d ", Z);
      fprintf_filtered (file, "V-%d ", V);
      fprintf_filtered (file, "C-%d ", C);
      if ((C | Z) == 0)
	fprintf_filtered (file, "u> ");
      if ((C | Z) == 1)
	fprintf_filtered (file, "u<= ");
      if ((C == 0))
	fprintf_filtered (file, "u>= ");
      if (C == 1)
	fprintf_filtered (file, "u< ");
      if (Z == 0)
	fprintf_filtered (file, "!= ");
      if (Z == 1)
	fprintf_filtered (file, "== ");
      if ((N ^ V) == 0)
	fprintf_filtered (file, ">= ");
      if ((N ^ V) == 1)
	fprintf_filtered (file, "< ");
      if ((Z | (N ^ V)) == 0)
	fprintf_filtered (file, "> ");
      if ((Z | (N ^ V)) == 1)
	fprintf_filtered (file, "<= ");
    }
  else if (regno == E_EXR_REGNUM && h8300smode)
    {
      /* EXR register */
      unsigned char b[h8300h_reg_size];
      unsigned char l;
      frame_register_read (deprecated_selected_frame, regno, b);
      l = b[REGISTER_VIRTUAL_SIZE (E_EXR_REGNUM) - 1];
      fprintf_filtered (file, "\t");
      fprintf_filtered (file, "T-%d - - - ", (l & 0x80) != 0);
      fprintf_filtered (file, "I2-%d ", (l & 4) != 0);
      fprintf_filtered (file, "I1-%d ", (l & 2) != 0);
      fprintf_filtered (file, "I0-%d", (l & 1) != 0);
    }
  fprintf_filtered (file, "\n");
}

static void
h8300_print_registers_info (struct gdbarch *gdbarch, struct ui_file *file,
			    struct frame_info *frame, int regno, int cpregs)
{
  if (regno < 0)
    for (regno = 0; regno < E_NUM_REGS; ++regno)
      h8300_print_register (gdbarch, file, frame, regno);
  else
    h8300_print_register (gdbarch, file, frame, regno);
}

static CORE_ADDR
h8300_saved_pc_after_call (struct frame_info *ignore)
{
  return read_memory_unsigned_integer (read_register (E_SP_REGNUM), BINWORD);
}

static int
h8300_register_byte (int regno)
{
  if (regno < 0 || regno >= E_NUM_REGS)
    internal_error (__FILE__, __LINE__,
		    "h8300_register_byte: illegal register number %d", regno);
  else
    return regno * BINWORD;
}

static int
h8300_register_raw_size (int regno)
{
  if (regno < 0 || regno >= E_NUM_REGS)
    internal_error (__FILE__, __LINE__,
		    "h8300_register_raw_size: illegal register number %d",
		    regno);
  else
    return BINWORD;
}

static struct type *
h8300_register_virtual_type (int regno)
{
  if (regno < 0 || regno >= E_NUM_REGS)
    internal_error (__FILE__, __LINE__,
		    "h8300_register_virtual_type: illegal register number %d",
		    regno);
  else
    return h8300hmode ?
	   builtin_type_unsigned_long : builtin_type_unsigned_short;
}

static void
h8300_store_struct_return (CORE_ADDR addr, CORE_ADDR sp)
{ 
  write_register (0, addr);
}

static int
h8300_use_struct_convention (int gcc_p, struct type *type)
{
  return 1;
}

static CORE_ADDR
h8300_extract_struct_value_address (char *regbuf)
{
  return extract_address (regbuf + h8300_register_byte (E_ARG0_REGNUM),
			  h8300_register_raw_size (E_ARG0_REGNUM));
}

const static unsigned char *
h8300_breakpoint_from_pc (CORE_ADDR *pcptr, int *lenptr)
{
  /*static unsigned char breakpoint[] = { 0x7A, 0xFF };*/	/* ??? */
  static unsigned char breakpoint[] = { 0x01, 0x80 };		/* Sleep */

  *lenptr = sizeof (breakpoint);
  return breakpoint;
}

static void
h8300_print_float_info (struct gdbarch *gdbarch, struct ui_file *file,
			struct frame_info *frame, const char *args)
{
  fprintf_filtered (file, "\
No floating-point info available for this processor.\n");
}

static struct gdbarch *
h8300_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  static LONGEST call_dummy_words[1] = { 0 };
  struct gdbarch_tdep *tdep = NULL;
  struct gdbarch *gdbarch;

  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;

#if 0
  tdep = (struct gdbarch_tdep *) xmalloc (sizeof (struct gdbarch_tdep));
#endif

  if (info.bfd_arch_info->arch != bfd_arch_h8300)
    return NULL;

  switch (info.bfd_arch_info->mach)
    {
      case bfd_mach_h8300:
	h8300smode = 0;
	h8300hmode = 0;
	break;
      case bfd_mach_h8300h:
	h8300smode = 0;
	h8300hmode = 1;
	break;
      case bfd_mach_h8300s:
	h8300smode = 1;
	h8300hmode = 1;
	break;
    }

  gdbarch = gdbarch_alloc (&info, 0);

  /*
   * Basic register fields and methods.
   */

  set_gdbarch_num_regs (gdbarch, E_NUM_REGS);
  set_gdbarch_num_pseudo_regs (gdbarch, 0);
  set_gdbarch_sp_regnum (gdbarch, E_SP_REGNUM);
  set_gdbarch_fp_regnum (gdbarch, E_FP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, E_PC_REGNUM);
  set_gdbarch_register_name (gdbarch, h8300_register_name);
  set_gdbarch_register_size (gdbarch, BINWORD);
  set_gdbarch_register_bytes (gdbarch, E_NUM_REGS * BINWORD);
  set_gdbarch_register_byte (gdbarch, h8300_register_byte);
  set_gdbarch_register_raw_size (gdbarch, h8300_register_raw_size);
  set_gdbarch_max_register_raw_size (gdbarch, h8300h_reg_size);
  set_gdbarch_register_virtual_size (gdbarch, h8300_register_raw_size);
  set_gdbarch_max_register_virtual_size (gdbarch, h8300h_reg_size);
  set_gdbarch_register_virtual_type (gdbarch, h8300_register_virtual_type);
  set_gdbarch_print_registers_info (gdbarch, h8300_print_registers_info);
  set_gdbarch_print_float_info (gdbarch, h8300_print_float_info);

  /*
   * Frame Info
   */
  set_gdbarch_init_extra_frame_info (gdbarch, h8300_init_extra_frame_info);
  set_gdbarch_frame_init_saved_regs (gdbarch, h8300_frame_init_saved_regs);
  set_gdbarch_frame_chain (gdbarch, h8300_frame_chain);
  set_gdbarch_saved_pc_after_call (gdbarch, h8300_saved_pc_after_call);
  set_gdbarch_frame_saved_pc (gdbarch, h8300_frame_saved_pc);
  set_gdbarch_skip_prologue (gdbarch, h8300_skip_prologue);
  set_gdbarch_frame_chain_valid (gdbarch, func_frame_chain_valid);
  set_gdbarch_frame_args_address (gdbarch, h8300_frame_args_address);
  set_gdbarch_frame_locals_address (gdbarch, h8300_frame_locals_address);

  /* 
   * Miscelany
   */
  /* Stack grows up. */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  /* PC stops zero byte after a trap instruction
     (which means: exactly on trap instruction). */
  set_gdbarch_decr_pc_after_break (gdbarch, 0);
  /* This value is almost never non-zero... */
  set_gdbarch_function_start_offset (gdbarch, 0);
  /* This value is almost never non-zero... */
  set_gdbarch_frame_args_skip (gdbarch, 0);
  /* OK to default this value to 'unknown'. */
  set_gdbarch_frame_num_args (gdbarch, frame_num_args_unknown);
  set_gdbarch_frameless_function_invocation (gdbarch,
					     frameless_look_for_prologue);

  /* W/o prototype, coerce float args to double. */
  /* set_gdbarch_coerce_float_to_double (gdbarch, standard_coerce_float_to_double); */

  /*
   * Call Dummies
   * 
   * These values and methods are used when gdb calls a target function.  */
  set_gdbarch_push_dummy_frame (gdbarch, generic_push_dummy_frame);
  set_gdbarch_push_return_address (gdbarch, h8300_push_return_address);
  set_gdbarch_deprecated_extract_return_value (gdbarch, h8300_extract_return_value);
  set_gdbarch_push_arguments (gdbarch, h8300_push_arguments);
  set_gdbarch_pop_frame (gdbarch, h8300_pop_frame);
  set_gdbarch_store_struct_return (gdbarch, h8300_store_struct_return);
  set_gdbarch_deprecated_store_return_value (gdbarch, h8300_store_return_value);
  set_gdbarch_deprecated_extract_struct_value_address (gdbarch, h8300_extract_struct_value_address);
  set_gdbarch_use_struct_convention (gdbarch, h8300_use_struct_convention);
  set_gdbarch_call_dummy_address (gdbarch, entry_point_address);
  set_gdbarch_call_dummy_start_offset (gdbarch, 0);
  set_gdbarch_call_dummy_breakpoint_offset (gdbarch, 0);
  set_gdbarch_call_dummy_breakpoint_offset_p (gdbarch, 1);
  set_gdbarch_call_dummy_length (gdbarch, 0);
  set_gdbarch_call_dummy_p (gdbarch, 1);
  set_gdbarch_call_dummy_words (gdbarch, call_dummy_words);
  set_gdbarch_sizeof_call_dummy_words (gdbarch, 0);
  set_gdbarch_call_dummy_stack_adjust_p (gdbarch, 0);
  /* set_gdbarch_call_dummy_stack_adjust */
  set_gdbarch_fix_call_dummy (gdbarch, generic_fix_call_dummy);
  set_gdbarch_breakpoint_from_pc (gdbarch, h8300_breakpoint_from_pc);

  set_gdbarch_int_bit (gdbarch, 2 * TARGET_CHAR_BIT);
  set_gdbarch_long_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_ptr_bit (gdbarch, BINWORD * TARGET_CHAR_BIT);
  set_gdbarch_addr_bit (gdbarch, BINWORD * TARGET_CHAR_BIT);

  /* set_gdbarch_stack_align (gdbarch, SOME_stack_align); */
  set_gdbarch_extra_stack_alignment_needed (gdbarch, 0);
  set_gdbarch_believe_pcc_promotion (gdbarch, 1);

  return gdbarch;
}

void
_initialize_h8300_tdep (void)
{
  tm_print_insn = gdb_print_insn_h8300;
  register_gdbarch_init (bfd_arch_h8300, h8300_gdbarch_init);
}
