/* Motorola m68k target-dependent support for GNU/Linux.

   Copyright 1996, 1998, 2000, 2001, 2002, 2003 Free Software Foundation,
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
#include "gdb_string.h"
#include "gdbtypes.h"
#include "osabi.h"
#include "regcache.h"
#include "objfiles.h"
#include "symtab.h"
#include "m68k-tdep.h"

/* Offsets (in target ints) into jmp_buf.  */

#define M68K_LINUX_JB_ELEMENT_SIZE 4
#define M68K_LINUX_JB_PC 7

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

static int
m68k_linux_pc_in_sigtramp (CORE_ADDR pc, char *name)
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

  insn0 = ((insn0 << 16) & 0xffffffff) | (insn1 >> 16);
  insn1 = ((insn1 << 16) & 0xffffffff) | (insn2 >> 16);
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
  if (get_next_frame (frame))
    sigcontext_addr
      = read_memory_unsigned_integer (get_frame_base (get_next_frame (frame))
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
  if (m68k_linux_pc_in_sigtramp (get_frame_pc (frame), 0) == 2)
    target_read_memory (sigcontext_addr + UCONTEXT_PC_OFFSET, buf, ptrbytes);
  else
    target_read_memory (sigcontext_addr + SIGCONTEXT_PC_OFFSET, buf, ptrbytes);
  return extract_unsigned_integer (buf, ptrbytes);
}

/* Return the saved program counter for FRAME.  */

static CORE_ADDR
m68k_linux_frame_saved_pc (struct frame_info *frame)
{
  if (get_frame_type (frame) == SIGTRAMP_FRAME)
    return m68k_linux_sigtramp_saved_pc (frame);

  return read_memory_unsigned_integer (get_frame_base (frame) + 4, 4);
}

/* The following definitions are appropriate when using the ELF
   format, where floating point values are returned in fp0, pointer
   values in a0 and other values in d0.  */

/* Extract from an array REGBUF containing the (raw) register state a
   function return value of type TYPE, and copy that, in virtual
   format, into VALBUF.  */

static void
m68k_linux_extract_return_value (struct type *type, char *regbuf, char *valbuf)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    {
      DEPRECATED_REGISTER_CONVERT_TO_VIRTUAL (FP0_REGNUM, type,
					      regbuf + REGISTER_BYTE (FP0_REGNUM),
					      valbuf);
    }
  else if (TYPE_CODE (type) == TYPE_CODE_PTR)
    memcpy (valbuf, regbuf + REGISTER_BYTE (M68K_A0_REGNUM),
	    TYPE_LENGTH (type));
  else
    memcpy (valbuf,
	    regbuf + (TYPE_LENGTH (type) >= 4 ? 0 : 4 - TYPE_LENGTH (type)),
	    TYPE_LENGTH (type));
}

/* Write into appropriate registers a function return value of type
   TYPE, given in virtual format.  */

static void
m68k_linux_store_return_value (struct type *type, char *valbuf)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    {
      char raw_buffer[REGISTER_RAW_SIZE (FP0_REGNUM)];
      DEPRECATED_REGISTER_CONVERT_TO_RAW (type, FP0_REGNUM, valbuf, raw_buffer);
      deprecated_write_register_bytes (REGISTER_BYTE (FP0_REGNUM),
				       raw_buffer, TYPE_LENGTH (type));
    }
  else
    {
      if (TYPE_CODE (type) == TYPE_CODE_PTR)
	deprecated_write_register_bytes (REGISTER_BYTE (M68K_A0_REGNUM),
					 valbuf, TYPE_LENGTH (type));
      deprecated_write_register_bytes (0, valbuf, TYPE_LENGTH (type));
    }
}

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR.  */

static CORE_ADDR
m68k_linux_extract_struct_value_address (char *regbuf)
{
  return *(CORE_ADDR *) (regbuf + REGISTER_BYTE (M68K_A0_REGNUM));
}

static void
m68k_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  tdep->jb_pc = M68K_LINUX_JB_PC;
  tdep->jb_elt_size = M68K_LINUX_JB_ELEMENT_SIZE;

  set_gdbarch_deprecated_frame_saved_pc (gdbarch,
					 m68k_linux_frame_saved_pc);
  set_gdbarch_deprecated_extract_return_value (gdbarch,
					       m68k_linux_extract_return_value);
  set_gdbarch_deprecated_store_return_value (gdbarch,
					     m68k_linux_store_return_value);
  set_gdbarch_deprecated_extract_struct_value_address (gdbarch,
						       m68k_linux_extract_struct_value_address);

  set_gdbarch_pc_in_sigtramp (gdbarch, m68k_linux_pc_in_sigtramp);

  /* Shared library handling.  */
  set_gdbarch_in_solib_call_trampoline (gdbarch, in_plt_section);
  set_gdbarch_skip_trampoline_code (gdbarch, find_solib_trampoline_target);
}

void
_initialize_m68k_linux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_m68k, 0, GDB_OSABI_LINUX,
			  m68k_linux_init_abi);
}
