/* Target-dependent code for GNU/Linux running on x86-64, for GDB.

   Copyright 2001, 2003 Free Software Foundation, Inc.

   Contributed by Jiri Smid, SuSE Labs.

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
#include "inferior.h"
#include "gdbcore.h"
#include "gdb_string.h"
#include "regcache.h"
#include "x86-64-tdep.h"
#include "dwarf2cfi.h"
#include "osabi.h"

#define LINUX_SIGTRAMP_INSN0 (0x48)	/* mov $NNNNNNNN,%rax */
#define LINUX_SIGTRAMP_OFFSET0 (0)
#define LINUX_SIGTRAMP_INSN1 (0x0f)	/* syscall */
#define LINUX_SIGTRAMP_OFFSET1 (7)

static const unsigned char linux_sigtramp_code[] = {
  /*  mov $__NR_rt_sigreturn,%rax */
  LINUX_SIGTRAMP_INSN0, 0xc7, 0xc0, 0x0f, 0x00, 0x00, 0x00,
  /* syscall */
  LINUX_SIGTRAMP_INSN1, 0x05
};

#define LINUX_SIGTRAMP_LEN (sizeof linux_sigtramp_code)

/* If PC is in a sigtramp routine, return the address of the start of
   the routine.  Otherwise, return 0.  */

static CORE_ADDR
x86_64_linux_sigtramp_start (CORE_ADDR pc)
{
  unsigned char buf[LINUX_SIGTRAMP_LEN];
  if (read_memory_nobpt (pc, (char *) buf, LINUX_SIGTRAMP_LEN) != 0)
    return 0;

  if (buf[0] != LINUX_SIGTRAMP_INSN0)
    {
      if (buf[0] != LINUX_SIGTRAMP_INSN1)
	return 0;

      pc -= LINUX_SIGTRAMP_OFFSET1;

      if (read_memory_nobpt (pc, (char *) buf, LINUX_SIGTRAMP_LEN) != 0)
	return 0;
    }

  if (memcmp (buf, linux_sigtramp_code, LINUX_SIGTRAMP_LEN) != 0)
    return 0;

  return pc;
}

#define LINUX_SIGINFO_SIZE 0

/* Offset to struct sigcontext in ucontext, from <asm/ucontext.h>.  */
#define LINUX_UCONTEXT_SIGCONTEXT_OFFSET 40

/* Offset to saved PC in sigcontext, from <asm/sigcontext.h>.  */
#define LINUX_SIGCONTEXT_PC_OFFSET 128
#define LINUX_SIGCONTEXT_FP_OFFSET 120

/* Assuming FRAME is for a GNU/Linux sigtramp routine, return the
   address of the associated sigcontext structure.  */
static CORE_ADDR
x86_64_linux_sigcontext_addr (struct frame_info *frame)
{
  CORE_ADDR pc;
  ULONGEST rsp;

  pc = x86_64_linux_sigtramp_start (get_frame_pc (frame));
  if (pc)
    {
      if (get_next_frame (frame))
	/* If this isn't the top frame, the next frame must be for the
	   signal handler itself.  The sigcontext structure is part of
	   the user context. */
	return get_frame_base (get_next_frame (frame)) + LINUX_SIGINFO_SIZE +
	  LINUX_UCONTEXT_SIGCONTEXT_OFFSET;


      /* This is the top frame. */
      rsp = read_register (SP_REGNUM);
      return rsp + LINUX_SIGINFO_SIZE + LINUX_UCONTEXT_SIGCONTEXT_OFFSET;

    }

  error ("Couldn't recognize signal trampoline.");
  return 0;
}

/* Assuming FRAME is for a GNU/Linux sigtramp routine, return the
   saved program counter.  */

static CORE_ADDR
x86_64_linux_sigtramp_saved_pc (struct frame_info *frame)
{
  CORE_ADDR addr;

  addr = x86_64_linux_sigcontext_addr (frame);
  return read_memory_integer (addr + LINUX_SIGCONTEXT_PC_OFFSET, 8);
}

/* Immediately after a function call, return the saved pc.  */

CORE_ADDR
x86_64_linux_saved_pc_after_call (struct frame_info *frame)
{
  if ((get_frame_type (frame) == SIGTRAMP_FRAME))
    return x86_64_linux_sigtramp_saved_pc (frame);

  return read_memory_integer (read_register (SP_REGNUM), 8);
}

/* Saved Pc.  Get it from sigcontext if within sigtramp.  */
CORE_ADDR
x86_64_linux_frame_saved_pc (struct frame_info *frame)
{
  if ((get_frame_type (frame) == SIGTRAMP_FRAME))
    return x86_64_linux_sigtramp_saved_pc (frame);
  return cfi_get_ra (frame);
}

/* Return whether PC is in a GNU/Linux sigtramp routine.  */

int
x86_64_linux_in_sigtramp (CORE_ADDR pc, char *name)
{
  if (name)
    return strcmp ("__restore_rt", name) == 0;

  return (x86_64_linux_sigtramp_start (pc) != 0);
}

CORE_ADDR
x86_64_linux_frame_chain (struct frame_info *fi)
{
  ULONGEST addr;
  CORE_ADDR fp, pc;

  if (!(get_frame_type (fi) == SIGTRAMP_FRAME))
    {
      fp = cfi_frame_chain (fi);
      if (fp)
	return fp;
      else
	addr = get_frame_base (fi);
    }
  else
    addr = get_frame_base (get_next_frame (fi));

  addr += LINUX_SIGINFO_SIZE + LINUX_UCONTEXT_SIGCONTEXT_OFFSET;

  fp = read_memory_integer (addr + LINUX_SIGCONTEXT_FP_OFFSET, 8) + 8;

  return fp;
}

CORE_ADDR
x86_64_init_frame_pc (int fromleaf, struct frame_info *fi)
{
  CORE_ADDR addr;

  if (get_next_frame (fi)
      && (get_frame_type (get_next_frame (fi)) == SIGTRAMP_FRAME))
    {
      addr = get_frame_base (get_next_frame (get_next_frame (fi)))
	+ LINUX_SIGINFO_SIZE + LINUX_UCONTEXT_SIGCONTEXT_OFFSET;
      return read_memory_integer (addr + LINUX_SIGCONTEXT_PC_OFFSET, 8);
    }
  else
    return cfi_init_frame_pc (fromleaf, fi);
}


static void
x86_64_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  x86_64_init_abi (info, gdbarch);
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern void _initialize_x86_64_linux_tdep (void);

void
_initialize_x86_64_linux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_i386, bfd_mach_x86_64, GDB_OSABI_LINUX,
			  x86_64_linux_init_abi);
}
