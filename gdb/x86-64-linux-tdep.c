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
#include "regcache.h"
#include "osabi.h"

#include "gdb_string.h"
#include "gdb_assert.h"

#include "x86-64-tdep.h"
#include "x86-64-linux-tdep.h"
#include "i386-linux-tdep.h"	/* For I386_LINUX_ORIG_EAX_REGNUM.  */

/* Register indexes to 'struct user' come from <sys/reg.h>.  */

enum user_regs
{
  USER_R15,
  USER_R14,
  USER_R13,
  USER_R12,
  USER_RBP,
  USER_RBX,
  USER_R11,
  USER_R10,
  USER_R9,
  USER_R8,
  USER_RAX,
  USER_RCX,
  USER_RDX,
  USER_RSI,
  USER_RDI,
  USER_ORIG_RAX,
  USER_RIP,
  USER_CS,
  USER_EFLAGS,
  USER_RSP,
  USER_SS,
  USER_FS_BASE,
  USER_GS_BASE,
  USER_DS,
  USER_ES,
  USER_FS,
  USER_GS,
  USER_MAX
};

/* Map from GDB's i386/x86-64 REGNUM indexed general-purpose registers
   to a `struct user' array of registers..  */

struct regnum_map
{
  int regnum;
  int user;
};

struct regnum_to_user
{
  long nr;
  const struct regnum_map *map;
};

long
x86_64_linux_greg_offset (int regnum)
{
  const static struct regnum_map i386_regnum_map[] =
  {
    { I386_EAX_REGNUM, USER_RAX },
    { I386_ECX_REGNUM, USER_RCX },
    { I386_EDX_REGNUM, USER_RDX },
    { I386_EBX_REGNUM, USER_RBX },
    { I386_ESP_REGNUM, USER_RSP },
    { I386_EBP_REGNUM, USER_RBP },
    { I386_ESI_REGNUM, USER_RSI },
    { I386_EDI_REGNUM, USER_RDI },
    { I386_EIP_REGNUM, USER_RIP },
    { I386_EFLAGS_REGNUM, USER_EFLAGS },
    { I386_CS_REGNUM, USER_CS },
    { I386_SS_REGNUM, USER_SS },
    { I386_DS_REGNUM, USER_DS },
    { I386_ES_REGNUM, USER_ES },
    { I386_FS_REGNUM, USER_FS },
    { I386_GS_REGNUM, USER_GS },
    { I386_ST0_REGNUM, -1 },
    { I386_ST1_REGNUM, -1 },
    { I386_ST2_REGNUM, -1 },
    { I386_ST3_REGNUM, -1 },
    { I386_ST4_REGNUM, -1 },
    { I386_ST5_REGNUM, -1 },
    { I386_ST6_REGNUM, -1 },
    { I386_ST7_REGNUM, -1 },
    { I386_FCTRL_REGNUM, -1 },
    { I386_FSTAT_REGNUM, -1 },
    { I386_FTAG_REGNUM, -1 },
    { I386_FISEG_REGNUM, -1 },
    { I386_FIOFF_REGNUM, -1 },
    { I386_FOSEG_REGNUM, -1 },
    { I386_FOOFF_REGNUM, -1 },
    { I386_FOP_REGNUM, -1 },
    { I386_XMM0_REGNUM, -1 },
    { I386_XMM1_REGNUM, -1 },
    { I386_XMM2_REGNUM, -1 },
    { I386_XMM3_REGNUM, -1 },
    { I386_XMM4_REGNUM, -1 },
    { I386_XMM5_REGNUM, -1 },
    { I386_XMM6_REGNUM, -1 },
    { I386_XMM7_REGNUM, -1 },
    { I386_MXCSR_REGNUM, -1 },
    { I386_LINUX_ORIG_EAX_REGNUM, USER_ORIG_RAX },
  };
  const static struct regnum_to_user i386_regnum_to_user =
  {
    ARRAY_SIZE (i386_regnum_map), i386_regnum_map
  };
  const static struct regnum_map x86_64_regnum_map[] =
  {
    { X86_64_RAX_REGNUM, USER_RAX },
    { X86_64_RBX_REGNUM, USER_RBX },
    { X86_64_RCX_REGNUM, USER_RCX },
    { X86_64_RDX_REGNUM, USER_RDX },
    { X86_64_RSI_REGNUM, USER_RSI },
    { X86_64_RDI_REGNUM, USER_RDI },
    { X86_64_RBP_REGNUM, USER_RBP },
    { X86_64_RSP_REGNUM, USER_RSP },
    { X86_64_R8_REGNUM, USER_R8 },
    { X86_64_R9_REGNUM, USER_R9 },
    { X86_64_R10_REGNUM, USER_R10 },
    { X86_64_R11_REGNUM, USER_R11 },
    { X86_64_R12_REGNUM, USER_R12 },
    { X86_64_R13_REGNUM, USER_R13 },
    { X86_64_R14_REGNUM, USER_R14 },
    { X86_64_R15_REGNUM, USER_R15 },
    { X86_64_RIP_REGNUM, USER_RIP },
    { X86_64_EFLAGS_REGNUM, USER_EFLAGS },
    /* { X86_64_CS_REGNUM, USER_CS }, */
    /* { X86_64_SS_REGNUM, USER_SS }, */
    { X86_64_DS_REGNUM, USER_DS },
    { X86_64_ES_REGNUM, USER_ES },
    { X86_64_FS_REGNUM, USER_FS },
    { X86_64_GS_REGNUM, USER_GS },
  };
  const static struct regnum_to_user x86_64_regnum_to_user =
  {
    ARRAY_SIZE (x86_64_regnum_map), x86_64_regnum_map
  };
  const struct regnum_to_user *regnum_to_user;

  gdb_assert (TARGET_ARCHITECTURE->arch == bfd_arch_i386);
  switch (TARGET_ARCHITECTURE->mach)
    {
    case bfd_mach_i386_i386:
    case bfd_mach_i386_i386_intel_syntax:
      regnum_to_user = &i386_regnum_to_user;
      break;
    case bfd_mach_x86_64:
    case bfd_mach_x86_64_intel_syntax:
      regnum_to_user = &x86_64_regnum_to_user;
      break;
    case bfd_mach_i386_i8086:
      /* Better suggestion?  */
      return -1;
    default:
      internal_error (__FILE__, __LINE__, "bad_switch");
    }
  if (regnum < 0)
    return USER_MAX * 8;
  if (regnum >= regnum_to_user->nr)
    return -1;
  gdb_assert (regnum_to_user->map[regnum].regnum == regnum);
  if (regnum_to_user->map[regnum].user < 0)
    return -1;
  return regnum_to_user->map[regnum].user * 8;
}

/* Fill GDB's register array with the general-purpose register values
   in *GREGSETP.  */

void
x86_64_linux_supply_gregset (char *regp)
{
  int i;
  char buf[MAX_REGISTER_SIZE];
  int bad = 0;

  for (i = 0; i < NUM_REGS; i++)
    {
      long offset = x86_64_linux_greg_offset (i);
      if (offset >= 0)
	supply_register (i, regp + offset);
    }
}

/* Fill register REGNO (if it is a general-purpose register) in
   *GREGSETPS with the value in GDB's register array.  If REGNO is -1,
   do this for all registers.  */

void
x86_64_linux_fill_gregset (char *regp, int regno)
{
  int i;

  for (i = 0; i < NUM_REGS; i++)
    if (regno == -1 || regno == i)
      {
	long offset = x86_64_linux_greg_offset (i);
	if (offset >= 0)
	  {
	    char buf[MAX_REGISTER_SIZE];
	    memset (buf, 0, sizeof buf);
	    /* Assume little endian - LHS of buffer is the correct
               place to put the collected bytes.  */
	    regcache_collect (i, buf);
	    memcpy (regp + offset, buf, 8);
	  }
    }
}

/* The register sets used in GNU/Linux ELF core-dumps are identical to
   the register sets used by `ptrace'.  The corresponding types are
   `elf_gregset_t' for the general-purpose registers (with
   `elf_greg_t' the type of a single GP register) and `elf_fpregset_t'
   for the floating-point registers.  */

static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size,
			 int which, CORE_ADDR ignore)
{
  switch (which)
    {
    case 0:  /* Integer registers.  */
      if (x86_64_linux_greg_offset (-1) > core_reg_size)
	warning ("Wrong size register set in core file.");
      else
	x86_64_linux_supply_gregset (core_reg_sect);
      break;

    case 2:  /* Floating point registers.  */
    case 3:  /* "Extended" floating point registers.  This is gdb-speak
		for SSE/SSE2. */
      if (core_reg_size != 512)
	warning ("Wrong size XMM register set in core file.");
      else
	x86_64_supply_fxsave (core_reg_sect);
      break;

    default:
      /* Don't know what kind of register request this is; just ignore it.  */
      break;
    }
}

static int
x86_64_core_sniffer (struct core_fns *our_fns, bfd *abfd)
{
  int result;

  result = ((bfd_get_flavour (abfd) == our_fns -> core_flavour)
	    && bfd_get_arch (abfd) == bfd_arch_i386
	    && (bfd_get_mach (abfd) == bfd_mach_x86_64
		|| bfd_get_mach (abfd) == bfd_mach_x86_64_intel_syntax));
  return (result);
}

static struct core_fns x86_64_core_fns = 
{
  bfd_target_elf_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  x86_64_core_sniffer,			/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL					/* next */
};

#define LINUX_SIGTRAMP_INSN0	0x48	/* mov $NNNNNNNN, %rax */
#define LINUX_SIGTRAMP_OFFSET0	0
#define LINUX_SIGTRAMP_INSN1	0x0f	/* syscall */
#define LINUX_SIGTRAMP_OFFSET1	7

static const unsigned char linux_sigtramp_code[] =
{
  /* mov $__NR_rt_sigreturn, %rax */
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

  /* We only recognize a signal trampoline if PC is at the start of
     one of the two instructions.  We optimize for finding the PC at
     the start, as will be the case when the trampoline is not the
     first frame on the stack.  We assume that in the case where the
     PC is not at the start of the instruction sequence, there will be
     a few trailing readable bytes on the stack.  */

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

/* Return whether PC is in a GNU/Linux sigtramp routine.  */

static int
x86_64_linux_pc_in_sigtramp (CORE_ADDR pc, char *name)
{
  /* If we have NAME, we can optimize the search.  The trampoline is
     named __restore_rt.  However, it isn't dynamically exported from
     the shared C library, so the trampoline may appear to be part of
     the preceding function.  This should always be sigaction,
     __sigaction, or __libc_sigaction (all aliases to the same
     function).  */
  if (name == NULL || strstr (name, "sigaction") != NULL)
    return (x86_64_linux_sigtramp_start (pc) != 0);

  return (strcmp ("__restore_rt", name) == 0);
}

/* Offset to struct sigcontext in ucontext, from <asm/ucontext.h>.  */
#define X86_64_LINUX_UCONTEXT_SIGCONTEXT_OFFSET 40

/* Assuming NEXT_FRAME is a frame following a GNU/Linux sigtramp
   routine, return the address of the associated sigcontext structure.  */

static CORE_ADDR
x86_64_linux_sigcontext_addr (struct frame_info *next_frame)
{
  CORE_ADDR sp;
  char buf[8];

  frame_unwind_register (next_frame, SP_REGNUM, buf);
  sp = extract_unsigned_integer (buf, 8);

  /* The sigcontext structure is part of the user context.  A pointer
     to the user context is passed as the third argument to the signal
     handler, i.e. in %rdx.  Unfortunately %rdx isn't preserved across
     function calls so we can't use it.  Fortunately the user context
     is part of the signal frame and the unwound %rsp directly points
     at it.  */
  return sp + X86_64_LINUX_UCONTEXT_SIGCONTEXT_OFFSET;
}


/* From <asm/sigcontext.h>.  */
static int x86_64_linux_sc_reg_offset[X86_64_NUM_GREGS] =
{
  13 * 8,			/* %rax */
  11 * 8,			/* %rbx */
  14 * 8,			/* %rcx */
  12 * 8,			/* %rdx */
  9 * 8,			/* %rsi */
  8 * 8,			/* %rdi */
  10 * 8,			/* %rbp */
  15 * 8,			/* %rsp */
  0 * 8,			/* %r8 */
  1 * 8,			/* %r9 */
  2 * 8,			/* %r10 */
  3 * 8,			/* %r11 */
  4 * 8,			/* %r12 */
  5 * 8,			/* %r13 */
  6 * 8,			/* %r14 */
  7 * 8,			/* %r15 */
  16 * 8,			/* %rip */
  17 * 8,			/* %eflags */
  -1,				/* %ds */
  -1,				/* %es */

  /* FIXME: kettenis/2002030531: The registers %fs and %gs are
     available in `struct sigcontext'.  However, they only occupy two
     bytes instead of four, which makes using them here rather
     difficult.  Leave them out for now.  */
  -1,				/* %fs */
  -1				/* %gs */
};

static void
x86_64_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  x86_64_init_abi (info, gdbarch);

  set_gdbarch_pc_in_sigtramp (gdbarch, x86_64_linux_pc_in_sigtramp);

  tdep->sigcontext_addr = x86_64_linux_sigcontext_addr;
  tdep->sc_reg_offset = x86_64_linux_sc_reg_offset;
  tdep->sc_num_regs = X86_64_NUM_GREGS;
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
extern void _initialize_x86_64_linux_tdep (void);

void
_initialize_x86_64_linux_tdep (void)
{
  add_core_fns (&x86_64_core_fns);

  gdbarch_register_osabi (bfd_arch_i386, bfd_mach_x86_64, GDB_OSABI_LINUX,
			  x86_64_linux_init_abi);
}
