/* Target-dependent code for GNU/Linux running on i386's, for GDB.

   Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include "value.h"
#include "regcache.h"
#include "inferior.h"
#include "reggroups.h"
#include "gdb_assert.h"

/* For i386_linux_skip_solib_resolver.  */
#include "symtab.h"
#include "symfile.h"
#include "objfiles.h"

#include "solib-svr4.h"		/* For struct link_map_offsets.  */

#include "osabi.h"

#include "i386-tdep.h"
#include "i387-tdep.h"
#include "i386-linux-tdep.h"


/* The register sets used in GNU/Linux ELF core-dumps are identical to
   the register sets in `struct user' that is used for a.out
   core-dumps, and is also used by `ptrace'.  The corresponding types
   are `elf_gregset_t' for the general-purpose registers (with
   `elf_greg_t' the type of a single GP register) and `elf_fpregset_t'
   for the floating-point registers.

   Those types used to be available under the names `gregset_t' and
   `fpregset_t' too, and this file used those names in the past.  But
   those names are now used for the register sets used in the
   `mcontext_t' type, and have a different size and layout.  */

enum user_regs {
  USER_INVALID = -1,
  USER_EBX,
  USER_ECX,
  USER_EDX,
  USER_ESI,
  USER_EDI,
  USER_EBP,
  USER_EAX,
  USER_DS,
  USER_ES,
  USER_FS,
  USER_GS,
  USER_ORIG_EAX,
  USER_EIP,
  USER_CS,
  USER_EFL,
  USER_UESP,
  USER_SS,
  USER_MAX
};

struct regnum_map
{
  enum i386_regnums regnum;
  enum user_regs user;
};

struct regnum_to_user
{
  long nr;
  const struct regnum_map *map;
};

long
i386_linux_greg_offset (int regnum)
{
  /* Mapping between the general-purpose registers in `struct user'
     format and GDB's register array layout.  */
  static const struct regnum_map regnum_map[] = 
  {
    { I386_EAX_REGNUM, USER_EAX },
    { I386_ECX_REGNUM, USER_ECX },
    { I386_EDX_REGNUM, USER_EDX },
    { I386_EBX_REGNUM, USER_EBX },
    { I386_ESP_REGNUM, USER_UESP },
    { I386_EBP_REGNUM, USER_EBP },
    { I386_ESI_REGNUM, USER_ESI },
    { I386_EDI_REGNUM, USER_EDI },
    { I386_EIP_REGNUM, USER_EIP },
    { I386_EFLAGS_REGNUM, USER_EFL },
    { I386_CS_REGNUM, USER_CS },
    { I386_SS_REGNUM, USER_SS },
    { I386_DS_REGNUM, USER_DS },
    { I386_ES_REGNUM, USER_ES },
    { I386_FS_REGNUM, USER_FS },
    { I386_GS_REGNUM, USER_GS },
    { I386_ST0_REGNUM, USER_INVALID },
    { I386_ST1_REGNUM, USER_INVALID },
    { I386_ST2_REGNUM, USER_INVALID },
    { I386_ST3_REGNUM, USER_INVALID },
    { I386_ST4_REGNUM, USER_INVALID },
    { I386_ST5_REGNUM, USER_INVALID },
    { I386_ST6_REGNUM, USER_INVALID },
    { I386_ST7_REGNUM, USER_INVALID },
    { I386_FCTRL_REGNUM, USER_INVALID },
    { I386_FSTAT_REGNUM, USER_INVALID },
    { I386_FTAG_REGNUM, USER_INVALID },
    { I386_FISEG_REGNUM, USER_INVALID },
    { I386_FIOFF_REGNUM, USER_INVALID },
    { I386_FOSEG_REGNUM, USER_INVALID },
    { I386_FOOFF_REGNUM, USER_INVALID },
    { I386_FOP_REGNUM, USER_INVALID },
    { I386_XMM0_REGNUM, USER_INVALID },
    { I386_XMM1_REGNUM, USER_INVALID },
    { I386_XMM2_REGNUM, USER_INVALID },
    { I386_XMM3_REGNUM, USER_INVALID },
    { I386_XMM4_REGNUM, USER_INVALID },
    { I386_XMM5_REGNUM, USER_INVALID },
    { I386_XMM6_REGNUM, USER_INVALID },
    { I386_XMM6_REGNUM, USER_INVALID },
    { I386_MXCSR_REGNUM, USER_INVALID },
    { I386_LINUX_ORIG_EAX_REGNUM, USER_ORIG_EAX }
  };
  const static struct regnum_to_user regnum_to_user =
  {
    ARRAY_SIZE (regnum_map), regnum_map
  };

  gdb_assert (TARGET_ARCHITECTURE->arch == bfd_arch_i386);
  if (regnum < 0)
    return USER_MAX * 4;
  if (regnum >= regnum_to_user.nr)
    return -1;
  gdb_assert (regnum_to_user.map[regnum].regnum == regnum);
  if (regnum_to_user.map[regnum].user < 0)
    return -1;
  return regnum_to_user.map[regnum].user * 4;
}


/* Return the name of register REG.  */

static const char *
i386_linux_register_name (int reg)
{
  /* Deal with the extra "orig_eax" pseudo register.  */
  if (reg == I386_LINUX_ORIG_EAX_REGNUM)
    return "orig_eax";

  return i386_register_name (reg);
}

/* Return non-zero, when the register is in the corresponding register
   group.  Put the LINUX_ORIG_EAX register in the system group.  */
static int
i386_linux_register_reggroup_p (struct gdbarch *gdbarch, int regnum,
				struct reggroup *group)
{
  if (regnum == I386_LINUX_ORIG_EAX_REGNUM)
    return (group == system_reggroup
	    || group == save_reggroup
	    || group == restore_reggroup);
  return i386_register_reggroup_p (gdbarch, regnum, group);
}


/* Recognizing signal handler frames.  */

/* GNU/Linux has two flavors of signals.  Normal signal handlers, and
   "realtime" (RT) signals.  The RT signals can provide additional
   information to the signal handler if the SA_SIGINFO flag is set
   when establishing a signal handler using `sigaction'.  It is not
   unlikely that future versions of GNU/Linux will support SA_SIGINFO
   for normal signals too.  */

/* When the i386 Linux kernel calls a signal handler and the
   SA_RESTORER flag isn't set, the return address points to a bit of
   code on the stack.  This function returns whether the PC appears to
   be within this bit of code.

   The instruction sequence for normal signals is
       pop    %eax
       mov    $0x77, %eax
       int    $0x80
   or 0x58 0xb8 0x77 0x00 0x00 0x00 0xcd 0x80.

   Checking for the code sequence should be somewhat reliable, because
   the effect is to call the system call sigreturn.  This is unlikely
   to occur anywhere other than a signal trampoline.

   It kind of sucks that we have to read memory from the process in
   order to identify a signal trampoline, but there doesn't seem to be
   any other way.  The PC_IN_SIGTRAMP macro in tm-linux.h arranges to
   only call us if no function name could be identified, which should
   be the case since the code is on the stack.

   Detection of signal trampolines for handlers that set the
   SA_RESTORER flag is in general not possible.  Unfortunately this is
   what the GNU C Library has been doing for quite some time now.
   However, as of version 2.1.2, the GNU C Library uses signal
   trampolines (named __restore and __restore_rt) that are identical
   to the ones used by the kernel.  Therefore, these trampolines are
   supported too.  */

#define LINUX_SIGTRAMP_INSN0	0x58	/* pop %eax */
#define LINUX_SIGTRAMP_OFFSET0	0
#define LINUX_SIGTRAMP_INSN1	0xb8	/* mov $NNNN, %eax */
#define LINUX_SIGTRAMP_OFFSET1	1
#define LINUX_SIGTRAMP_INSN2	0xcd	/* int */
#define LINUX_SIGTRAMP_OFFSET2	6

static const unsigned char linux_sigtramp_code[] =
{
  LINUX_SIGTRAMP_INSN0,					/* pop %eax */
  LINUX_SIGTRAMP_INSN1, 0x77, 0x00, 0x00, 0x00,		/* mov $0x77, %eax */
  LINUX_SIGTRAMP_INSN2, 0x80				/* int $0x80 */
};

#define LINUX_SIGTRAMP_LEN (sizeof linux_sigtramp_code)

/* If PC is in a sigtramp routine, return the address of the start of
   the routine.  Otherwise, return 0.  */

static CORE_ADDR
i386_linux_sigtramp_start (CORE_ADDR pc)
{
  unsigned char buf[LINUX_SIGTRAMP_LEN];

  /* We only recognize a signal trampoline if PC is at the start of
     one of the three instructions.  We optimize for finding the PC at
     the start, as will be the case when the trampoline is not the
     first frame on the stack.  We assume that in the case where the
     PC is not at the start of the instruction sequence, there will be
     a few trailing readable bytes on the stack.  */

  if (read_memory_nobpt (pc, (char *) buf, LINUX_SIGTRAMP_LEN) != 0)
    return 0;

  if (buf[0] != LINUX_SIGTRAMP_INSN0)
    {
      int adjust;

      switch (buf[0])
	{
	case LINUX_SIGTRAMP_INSN1:
	  adjust = LINUX_SIGTRAMP_OFFSET1;
	  break;
	case LINUX_SIGTRAMP_INSN2:
	  adjust = LINUX_SIGTRAMP_OFFSET2;
	  break;
	default:
	  return 0;
	}

      pc -= adjust;

      if (read_memory_nobpt (pc, (char *) buf, LINUX_SIGTRAMP_LEN) != 0)
	return 0;
    }

  if (memcmp (buf, linux_sigtramp_code, LINUX_SIGTRAMP_LEN) != 0)
    return 0;

  return pc;
}

/* This function does the same for RT signals.  Here the instruction
   sequence is
       mov    $0xad, %eax
       int    $0x80
   or 0xb8 0xad 0x00 0x00 0x00 0xcd 0x80.

   The effect is to call the system call rt_sigreturn.  */

#define LINUX_RT_SIGTRAMP_INSN0		0xb8 /* mov $NNNN, %eax */
#define LINUX_RT_SIGTRAMP_OFFSET0	0
#define LINUX_RT_SIGTRAMP_INSN1		0xcd /* int */
#define LINUX_RT_SIGTRAMP_OFFSET1	5

static const unsigned char linux_rt_sigtramp_code[] =
{
  LINUX_RT_SIGTRAMP_INSN0, 0xad, 0x00, 0x00, 0x00,	/* mov $0xad, %eax */
  LINUX_RT_SIGTRAMP_INSN1, 0x80				/* int $0x80 */
};

#define LINUX_RT_SIGTRAMP_LEN (sizeof linux_rt_sigtramp_code)

/* If PC is in a RT sigtramp routine, return the address of the start
   of the routine.  Otherwise, return 0.  */

static CORE_ADDR
i386_linux_rt_sigtramp_start (CORE_ADDR pc)
{
  unsigned char buf[LINUX_RT_SIGTRAMP_LEN];

  /* We only recognize a signal trampoline if PC is at the start of
     one of the two instructions.  We optimize for finding the PC at
     the start, as will be the case when the trampoline is not the
     first frame on the stack.  We assume that in the case where the
     PC is not at the start of the instruction sequence, there will be
     a few trailing readable bytes on the stack.  */

  if (read_memory_nobpt (pc, (char *) buf, LINUX_RT_SIGTRAMP_LEN) != 0)
    return 0;

  if (buf[0] != LINUX_RT_SIGTRAMP_INSN0)
    {
      if (buf[0] != LINUX_RT_SIGTRAMP_INSN1)
	return 0;

      pc -= LINUX_RT_SIGTRAMP_OFFSET1;

      if (read_memory_nobpt (pc, (char *) buf, LINUX_RT_SIGTRAMP_LEN) != 0)
	return 0;
    }

  if (memcmp (buf, linux_rt_sigtramp_code, LINUX_RT_SIGTRAMP_LEN) != 0)
    return 0;

  return pc;
}

/* Return whether PC is in a GNU/Linux sigtramp routine.  */

static int
i386_linux_pc_in_sigtramp (CORE_ADDR pc, char *name)
{
  /* If we have NAME, we can optimize the search.  The trampolines are
     named __restore and __restore_rt.  However, they aren't dynamically
     exported from the shared C library, so the trampoline may appear to
     be part of the preceding function.  This should always be sigaction,
     __sigaction, or __libc_sigaction (all aliases to the same function).  */
  if (name == NULL || strstr (name, "sigaction") != NULL)
    return (i386_linux_sigtramp_start (pc) != 0
	    || i386_linux_rt_sigtramp_start (pc) != 0);

  return (strcmp ("__restore", name) == 0
	  || strcmp ("__restore_rt", name) == 0);
}

/* Offset to struct sigcontext in ucontext, from <asm/ucontext.h>.  */
#define I386_LINUX_UCONTEXT_SIGCONTEXT_OFFSET 20

/* Assuming NEXT_FRAME is a frame following a GNU/Linux sigtramp
   routine, return the address of the associated sigcontext structure.  */

static CORE_ADDR
i386_linux_sigcontext_addr (struct frame_info *next_frame)
{
  CORE_ADDR pc;
  CORE_ADDR sp;
  char buf[4];

  frame_unwind_register (next_frame, I386_ESP_REGNUM, buf);
  sp = extract_unsigned_integer (buf, 4);

  pc = i386_linux_sigtramp_start (frame_pc_unwind (next_frame));
  if (pc)
    {
      /* The sigcontext structure lives on the stack, right after
	 the signum argument.  We determine the address of the
	 sigcontext structure by looking at the frame's stack
	 pointer.  Keep in mind that the first instruction of the
	 sigtramp code is "pop %eax".  If the PC is after this
	 instruction, adjust the returned value accordingly.  */
      if (pc == frame_pc_unwind (next_frame))
	return sp + 4;
      return sp;
    }

  pc = i386_linux_rt_sigtramp_start (frame_pc_unwind (next_frame));
  if (pc)
    {
      CORE_ADDR ucontext_addr;

      /* The sigcontext structure is part of the user context.  A
	 pointer to the user context is passed as the third argument
	 to the signal handler.  */
      read_memory (sp + 8, buf, 4);
      ucontext_addr = extract_unsigned_integer (buf, 4) + 20;
      return ucontext_addr + I386_LINUX_UCONTEXT_SIGCONTEXT_OFFSET;
    }

  error ("Couldn't recognize signal trampoline.");
  return 0;
}

/* Set the program counter for process PTID to PC.  */

static void
i386_linux_write_pc (CORE_ADDR pc, ptid_t ptid)
{
  write_register_pid (I386_EIP_REGNUM, pc, ptid);

  /* We must be careful with modifying the program counter.  If we
     just interrupted a system call, the kernel might try to restart
     it when we resume the inferior.  On restarting the system call,
     the kernel will try backing up the program counter even though it
     no longer points at the system call.  This typically results in a
     SIGSEGV or SIGILL.  We can prevent this by writing `-1' in the
     "orig_eax" pseudo-register.

     Note that "orig_eax" is saved when setting up a dummy call frame.
     This means that it is properly restored when that frame is
     popped, and that the interrupted system call will be restarted
     when we resume the inferior on return from a function call from
     within GDB.  In all other cases the system call will not be
     restarted.  */
  write_register_pid (I386_LINUX_ORIG_EAX_REGNUM, -1, ptid);
}

/* Calling functions in shared libraries.  */

/* Find the minimal symbol named NAME, and return both the minsym
   struct and its objfile.  This probably ought to be in minsym.c, but
   everything there is trying to deal with things like C++ and
   SOFUN_ADDRESS_MAYBE_TURQUOISE, ...  Since this is so simple, it may
   be considered too special-purpose for general consumption.  */

static struct minimal_symbol *
find_minsym_and_objfile (char *name, struct objfile **objfilep)
{
  struct objfile *objfile;

  ALL_OBJFILES (objfile)
    {
      struct minimal_symbol *msym;

      ALL_OBJFILE_MSYMBOLS (objfile, msym)
	{
	  if (SYMBOL_LINKAGE_NAME (msym)
	      && strcmp (SYMBOL_LINKAGE_NAME (msym), name) == 0)
	    {
	      *objfilep = objfile;
	      return msym;
	    }
	}
    }

  return 0;
}

static CORE_ADDR
skip_gnu_resolver (CORE_ADDR pc)
{
  /* The GNU dynamic linker is part of the GNU C library, so many
     GNU/Linux distributions use it.  (All ELF versions, as far as I
     know.)  An unresolved PLT entry points to "_dl_runtime_resolve",
     which calls "fixup" to patch the PLT, and then passes control to
     the function.

     We look for the symbol `_dl_runtime_resolve', and find `fixup' in
     the same objfile.  If we are at the entry point of `fixup', then
     we set a breakpoint at the return address (at the top of the
     stack), and continue.
  
     It's kind of gross to do all these checks every time we're
     called, since they don't change once the executable has gotten
     started.  But this is only a temporary hack --- upcoming versions
     of GNU/Linux will provide a portable, efficient interface for
     debugging programs that use shared libraries.  */

  struct objfile *objfile;
  struct minimal_symbol *resolver 
    = find_minsym_and_objfile ("_dl_runtime_resolve", &objfile);

  if (resolver)
    {
      struct minimal_symbol *fixup
	= lookup_minimal_symbol ("fixup", NULL, objfile);

      if (fixup && SYMBOL_VALUE_ADDRESS (fixup) == pc)
	return frame_pc_unwind (get_current_frame ()); 
    }

  return 0;
}      

/* See the comments for SKIP_SOLIB_RESOLVER at the top of infrun.c.
   This function:
   1) decides whether a PLT has sent us into the linker to resolve
      a function reference, and 
   2) if so, tells us where to set a temporary breakpoint that will
      trigger when the dynamic linker is done.  */

CORE_ADDR
i386_linux_skip_solib_resolver (CORE_ADDR pc)
{
  CORE_ADDR result;

  /* Plug in functions for other kinds of resolvers here.  */
  result = skip_gnu_resolver (pc);
  if (result)
    return result;

  return 0;
}

/* Fetch (and possibly build) an appropriate link_map_offsets
   structure for native GNU/Linux x86 targets using the struct offsets
   defined in link.h (but without actual reference to that file).

   This makes it possible to access GNU/Linux x86 shared libraries
   from a GDB that was not built on an GNU/Linux x86 host (for cross
   debugging).  */

static struct link_map_offsets *
i386_linux_svr4_fetch_link_map_offsets (void)
{
  static struct link_map_offsets lmo;
  static struct link_map_offsets *lmp = NULL;

  if (lmp == NULL)
    {
      lmp = &lmo;

      lmo.r_debug_size = 8;	/* The actual size is 20 bytes, but
				   this is all we need.  */
      lmo.r_map_offset = 4;
      lmo.r_map_size   = 4;

      lmo.link_map_size = 20;	/* The actual size is 552 bytes, but
				   this is all we need.  */
      lmo.l_addr_offset = 0;
      lmo.l_addr_size   = 4;

      lmo.l_name_offset = 4;
      lmo.l_name_size   = 4;

      lmo.l_next_offset = 12;
      lmo.l_next_size   = 4;

      lmo.l_prev_offset = 16;
      lmo.l_prev_size   = 4;
    }

  return lmp;
}


/* From <asm/sigcontext.h>.  */
static int i386_linux_sc_reg_offset[I386_NUM_GREGS] =
{
  11 * 4,			/* %eax */
  10 * 4,			/* %ecx */
  9 * 4,			/* %edx */
  8 * 4,			/* %ebx */
  7 * 4,			/* %esp */
  6 * 4,			/* %ebp */
  5 * 4,			/* %esi */
  4 * 4,			/* %edi */
  14 * 4,			/* %eip */
  16 * 4,			/* %eflags */
  15 * 4,			/* %cs */
  18 * 4,			/* %ss */
  3 * 4,			/* %ds */
  2 * 4,			/* %es */
  1 * 4,			/* %fs */
  0 * 4				/* %gs */
};

static void
i386_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* GNU/Linux uses ELF.  */
  i386_elf_init_abi (info, gdbarch);

  /* We support the SSE registers on GNU/Linux.  */
  tdep->num_xmm_regs = I386_NUM_XREGS - 1;
  /* set_gdbarch_num_regs (gdbarch, I386_SSE_NUM_REGS); */

  /* Since we have the extra "orig_eax" register on GNU/Linux, we have
     to adjust a few things.  */

  set_gdbarch_write_pc (gdbarch, i386_linux_write_pc);
  set_gdbarch_num_regs (gdbarch, I386_SSE_NUM_REGS + 1);
  set_gdbarch_register_name (gdbarch, i386_linux_register_name);
  set_gdbarch_register_reggroup_p (gdbarch, i386_linux_register_reggroup_p);

  tdep->jb_pc_offset = 20;	/* From <bits/setjmp.h>.  */

  tdep->sigcontext_addr = i386_linux_sigcontext_addr;
  tdep->sc_reg_offset = i386_linux_sc_reg_offset;
  tdep->sc_num_regs = I386_NUM_GREGS;

  /* When the i386 Linux kernel calls a signal handler, the return
     address points to a bit of code on the stack.  This function is
     used to identify this bit of code as a signal trampoline in order
     to support backtracing through calls to signal handlers.  */
  set_gdbarch_pc_in_sigtramp (gdbarch, i386_linux_pc_in_sigtramp);

  set_solib_svr4_fetch_link_map_offsets (gdbarch,
				       i386_linux_svr4_fetch_link_map_offsets);
}



/* Interpreting register set info found in core files and ptrace
   buffers.  */

/* Fill GDB's register array with the floating-point register values in
   *FPREGSETP.  */

/* Fill the XMM registers in the register array with dummy values.  For
   cases where we don't have access to the XMM registers.  I think
   this is cleaner than printing a warning.  For a cleaner solution,
   we should gdbarchify the i386 family.  */

static void
dummy_sse_values (void)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  int reg;
  /* Assume i386 is always little endian.  */
  static const char mxcsr[MAX_REGISTER_SIZE] = { 0x80, 0x1f };
  /* C doesn't have a syntax for NaN's (0xffffffffff), so generate it
     on the fly.  */
  char nan[MAX_REGISTER_SIZE];
  memset (nan, -1, sizeof nan);

  for (reg = 0; reg < tdep->num_xmm_regs; reg++)
    supply_register (XMM0_REGNUM + reg, (char *) nan);
  if (tdep->num_xmm_regs > 0)
    supply_register (MXCSR_REGNUM, (char *) &mxcsr);
}

void 
i386_linux_supply_fpregset (void *fpregset)
{
  i387_supply_fsave (fpregset);
  dummy_sse_values ();
}

/* Fill GDB's register array with the general-purpose register values
   in *GREGSETP.  */

void
i386_linux_supply_gregset (void *gregset)
{
  bfd_byte *regp = gregset;
  int i;

  for (i = 0; i < I386_NUM_GREGS; i++)
    {
      long offset = i386_linux_greg_offset (i);
      if (offset >= 0)
	supply_register (i, regp + offset);
    }

  if (I386_LINUX_ORIG_EAX_REGNUM < NUM_REGS)
    {
      long offset = i386_linux_greg_offset (I386_LINUX_ORIG_EAX_REGNUM);
      if (offset >= 0)
	supply_register (I386_LINUX_ORIG_EAX_REGNUM, regp + offset);
    }
}

/* Fill GDB's register array with the floating-point and SSE register
   values in *FPXREGSETP.  */

void
i386_linux_supply_fpxregset (void *fpxregsetp)
{
  i387_supply_fxsave (fpxregsetp);
}


/* Provide registers to GDB from a core file.

   (We can't use the generic version of this function in
   core-regset.c, because GNU/Linux has *three* different kinds of
   register set notes.  core-regset.c would have to call
   supply_fpxregset, which most platforms don't have.)

   CORE_REG_SECT points to an array of bytes, which are the contents
   of a `note' from a core file which BFD thinks might contain
   register contents.  CORE_REG_SIZE is its size.

   WHICH says which register set corelow suspects this is:
     0 --- the general-purpose register set, in elf_gregset_t format
     2 --- the floating-point register set, in elf_fpregset_t format
     3 --- the extended floating-point register set, in elf_fpxregset_t format

   REG_ADDR isn't used on GNU/Linux.  */

static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size,
				 int which, CORE_ADDR reg_addr)
{
  switch (which)
    {
    case 0:
      if (core_reg_size < i386_linux_greg_offset (-1))
 	warning ("Wrong size register set in core file.");
      else
	i386_linux_supply_gregset (core_reg_sect);
      break;

    case 2:
      if (core_reg_size < 108)
	warning ("Wrong size fpregset in core file.");
      else
	i386_linux_supply_fpregset (core_reg_sect);
      break;

    case 3:
      if (core_reg_size < 512)
	warning ("Wrong size fpxregset in core file.");
      else
	i386_linux_supply_fpxregset (core_reg_sect);
      break;

    default:
      /* We've covered all the kinds of registers we know about here,
         so this must be something we wouldn't know what to do with
         anyway.  Just ignore it.  */
      break;
    }
}

static int
i386_linux_core_sniffer (struct core_fns *our_fns, bfd *abfd)
{
  int result;

  result = ((bfd_get_flavour (abfd) == our_fns -> core_flavour)
	    && bfd_get_arch (abfd) == bfd_arch_i386
	    && (bfd_get_mach (abfd) == bfd_mach_i386_i386
		|| bfd_get_mach (abfd) == bfd_mach_i386_i386_intel_syntax));
  return (result);
}

static struct core_fns i386_linux_core_fns = 
{
  bfd_target_elf_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  i386_linux_core_sniffer,		/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL					/* next */
};


/* Provide a prototype to silence -Wmissing-prototypes.  */
extern void _initialize_i386_linux_tdep (void);

void
_initialize_i386_linux_tdep (void)
{
  add_core_fns (&i386_linux_core_fns);
  gdbarch_register_osabi (bfd_arch_i386, 0, GDB_OSABI_LINUX,
			  i386_linux_init_abi);
}
