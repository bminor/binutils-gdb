/* Motorola m68k target-dependent support for GNU/Linux.

   Copyright 1996, 1998, 2000, 2001, 2002 Free Software Foundation,
   Inc.

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
#include "gdbcore.h"
#include "frame.h"
#include "target.h"


/* Check whether insn1 and insn2 are parts of a signal trampoline.  */

#define IS_SIGTRAMP(insn1, insn2)					\
  (/* addaw #20,sp; moveq #119,d0; trap #0 */				\
   (insn1 == 0xdefc0014 && insn2 == 0x70774e40)				\
   /* moveq #119,d0; trap #0 */						\
   || insn1 == 0x70774e40)

#define IS_RT_SIGTRAMP(insn1, insn2)					\
  (/* movel #173,d0; trap #0 */						\
   (insn1 == 0x203c0000 && insn2 == 0x00ad4e40)				\
   /* moveq #82,d0; notb d0; trap #0 */					\
   || (insn1 == 0x70524600 && (insn2 >> 16) == 0x4e40))

/* Return non-zero if PC points into the signal trampoline.  For the sake
   of m68k_linux_frame_saved_pc we also distinguish between non-RT and RT
   signal trampolines.  */

int
m68k_linux_in_sigtramp (CORE_ADDR pc)
{
  CORE_ADDR sp;
  char buf[12];
  unsigned long insn0, insn1, insn2;

  if (read_memory_nobpt (pc - 4, buf, sizeof (buf)))
    return 0;
  insn1 = extract_unsigned_integer (buf + 4, 4);
  insn2 = extract_unsigned_integer (buf + 8, 4);
  if (IS_SIGTRAMP (insn1, insn2))
    return 1;
  if (IS_RT_SIGTRAMP (insn1, insn2))
    return 2;

  insn0 = extract_unsigned_integer (buf, 4);
  if (IS_SIGTRAMP (insn0, insn1))
    return 1;
  if (IS_RT_SIGTRAMP (insn0, insn1))
    return 2;

  insn0 = (insn0 << 16) | (insn1 >> 16);
  insn1 = (insn1 << 16) | (insn2 >> 16);
  if (IS_SIGTRAMP (insn0, insn1))
    return 1;
  if (IS_RT_SIGTRAMP (insn0, insn1))
    return 2;

  return 0;
}

/* Offset to saved PC in sigcontext, from <asm/sigcontext.h>.  */
#define SIGCONTEXT_PC_OFFSET 26

/* Offset to saved PC in ucontext, from <asm/ucontext.h>.  */
#define UCONTEXT_PC_OFFSET 88

/* Get saved user PC for sigtramp from sigcontext or ucontext.  */

static CORE_ADDR
m68k_linux_sigtramp_saved_pc (struct frame_info *frame)
{
  CORE_ADDR sigcontext_addr;
  char buf[TARGET_PTR_BIT / TARGET_CHAR_BIT];
  int ptrbytes = TARGET_PTR_BIT / TARGET_CHAR_BIT;
  int sigcontext_offs = (2 * TARGET_INT_BIT) / TARGET_CHAR_BIT;

  /* Get sigcontext address, it is the third parameter on the stack.  */
  if (frame->next)
    sigcontext_addr
      = read_memory_unsigned_integer (FRAME_ARGS_ADDRESS (frame->next)
				      + FRAME_ARGS_SKIP
				      + sigcontext_offs,
				      ptrbytes);
  else
    sigcontext_addr
      = read_memory_unsigned_integer (read_register (SP_REGNUM)
				      + sigcontext_offs,
				      ptrbytes);

  /* Don't cause a memory_error when accessing sigcontext in case the
     stack layout has changed or the stack is corrupt.  */
  if (m68k_linux_in_sigtramp (frame->pc) == 2)
    target_read_memory (sigcontext_addr + UCONTEXT_PC_OFFSET, buf, ptrbytes);
  else
    target_read_memory (sigcontext_addr + SIGCONTEXT_PC_OFFSET, buf, ptrbytes);
  return extract_unsigned_integer (buf, ptrbytes);
}

/* Return the saved program counter for FRAME.  */

CORE_ADDR
m68k_linux_frame_saved_pc (struct frame_info *frame)
{
  if (get_frame_type (frame) == SIGTRAMP_FRAME)
    return m68k_linux_sigtramp_saved_pc (frame);

  return read_memory_unsigned_integer (frame->frame + 4, 4);
}
