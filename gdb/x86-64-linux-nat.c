/* Native-dependent code for Linux/x86-64.
   Copyright 2001
   Free Software Foundation, Inc.
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
#include "i387-nat.h"
#include "gdb_assert.h"
#include "x86-64-tdep.h"

#include <sys/ptrace.h>
#include <sys/debugreg.h>
#include <sys/syscall.h>
#include <sys/procfs.h>

static unsigned long
x86_64_linux_dr_get (int regnum)
{
  int tid;
  unsigned long value;

  /* FIXME: kettenis/2001-01-29: It's not clear what we should do with
     multi-threaded processes here.  For now, pretend there is just
     one thread.  */
  tid = PIDGET (inferior_ptid);

  /* FIXME: kettenis/2001-03-27: Calling perror_with_name if the
     ptrace call fails breaks debugging remote targets.  The correct
     way to fix this is to add the hardware breakpoint and watchpoint
     stuff to the target vectore.  For now, just return zero if the
     ptrace call fails.  */
  errno = 0;
  value = ptrace (PT_READ_U, tid,
		  offsetof (struct user, u_debugreg[regnum]), 0);
  if (errno != 0)
#if 0
    perror_with_name ("Couldn't read debug register");
#else
    return 0;
#endif

  return value;
}

static void
x86_64_linux_dr_set (int regnum, unsigned long value)
{
  int tid;

  /* FIXME: kettenis/2001-01-29: It's not clear what we should do with
     multi-threaded processes here.  For now, pretend there is just
     one thread.  */
  tid = PIDGET (inferior_ptid);

  errno = 0;
  ptrace (PT_WRITE_U, tid,
	  offsetof (struct user, u_debugreg[regnum]), value);
  if (errno != 0)
    perror_with_name ("Couldn't write debug register");
}

void
x86_64_linux_dr_set_control (unsigned long control)
{
  x86_64_linux_dr_set (DR_CONTROL, control);
}

void
x86_64_linux_dr_set_addr (int regnum, CORE_ADDR addr)
{
  gdb_assert (regnum >= 0 && regnum <= DR_LASTADDR - DR_FIRSTADDR);

  x86_64_linux_dr_set (DR_FIRSTADDR + regnum, addr);
}

void
x86_64_linux_dr_reset_addr (int regnum)
{
  gdb_assert (regnum >= 0 && regnum <= DR_LASTADDR - DR_FIRSTADDR);

  x86_64_linux_dr_set (DR_FIRSTADDR + regnum, 0L);
}

unsigned long
x86_64_linux_dr_get_status (void)
{
  return x86_64_linux_dr_get (DR_STATUS);
}


/* The register sets used in Linux ELF core-dumps are identical to the
   register sets used by `ptrace'.  */

#define GETREGS_SUPPLIES(regno) \
  (0 <= (regno) && (regno) <= 17)
#define GETFPREGS_SUPPLIES(regno) \
  (FP0_REGNUM <= (regno) && (regno) <= MXCSR_REGNUM)

#define PTRACE_XFER_TYPE unsigned long


/* Transfering the general-purpose registers between GDB, inferiors
   and core files.  */

/* Fill GDB's register array with the general-purpose register values
   in *GREGSETP.  */

void
supply_gregset (elf_gregset_t * gregsetp)
{
  elf_greg_t *regp = (elf_greg_t *) gregsetp;
  int i;

  for (i = 0; i < X86_64_NUM_GREGS; i++)
    supply_register (i, (char *) (regp + x86_64_regmap[i]));
}

/* Fill register REGNO (if it is a general-purpose register) in
   *GREGSETPS with the value in GDB's register array.  If REGNO is -1,
   do this for all registers.  */

void
fill_gregset (elf_gregset_t * gregsetp, int regno)
{
  elf_greg_t *regp = (elf_greg_t *) gregsetp;
  int i;

  for (i = 0; i < X86_64_NUM_GREGS; i++)
    if ((regno == -1 || regno == i))
      *(regp + x86_64_regmap[i]) =
	*(elf_greg_t *) & registers[REGISTER_BYTE (i)];
}

/* Fetch all general-purpose registers from process/thread TID and
   store their values in GDB's register array.  */

static void
fetch_regs (int tid)
{
  elf_gregset_t regs;

  if (ptrace (PTRACE_GETREGS, tid, 0, (long) &regs) < 0)
      perror_with_name ("Couldn't get registers");

  supply_gregset (&regs);
}

/* Store all valid general-purpose registers in GDB's register array
   into the process/thread specified by TID.  */

static void
store_regs (int tid, int regno)
{
  elf_gregset_t regs;

  if (ptrace (PTRACE_GETREGS, tid, 0, (long) &regs) < 0)
    perror_with_name ("Couldn't get registers");

  fill_gregset (&regs, regno);

  if (ptrace (PTRACE_SETREGS, tid, 0, (long) &regs) < 0)
    perror_with_name ("Couldn't write registers");
}


/* Transfering floating-point registers between GDB, inferiors and cores.  */

/* Fill GDB's register array with the floating-point register values in
   *FPREGSETP.  */

void
supply_fpregset (elf_fpregset_t * fpregsetp)
{
  i387_supply_fxsave ((char *) fpregsetp);
}

/* Fill register REGNO (if it is a floating-point register) in
   *FPREGSETP with the value in GDB's register array.  If REGNO is -1,
   do this for all registers.  */

void
fill_fpregset (elf_fpregset_t * fpregsetp, int regno)
{
  i387_fill_fxsave ((char *) fpregsetp, regno);
}

/* Fetch all floating-point registers from process/thread TID and store
   thier values in GDB's register array.  */

static void
fetch_fpregs (int tid)
{
  elf_fpregset_t fpregs;

  if (ptrace (PTRACE_GETFPREGS, tid, 0, (long) &fpregs) < 0)
    perror_with_name ("Couldn't get floating point status");

  supply_fpregset (&fpregs);
}

/* Store all valid floating-point registers in GDB's register array
   into the process/thread specified by TID.  */

static void
store_fpregs (int tid, int regno)
{
  elf_fpregset_t fpregs;

  if (ptrace (PTRACE_GETFPREGS, tid, 0, (long) &fpregs) < 0)
    perror_with_name ("Couldn't get floating point status");

  fill_fpregset (&fpregs, regno);

  if (ptrace (PTRACE_SETFPREGS, tid, 0, (long) &fpregs) < 0)
    perror_with_name ("Couldn't write floating point status");
}


/* Transferring arbitrary registers between GDB and inferior.  */

/* Fetch register REGNO from the child process.  If REGNO is -1, do
   this for all registers (including the floating point and SSE
   registers).  */

void
fetch_inferior_registers (int regno)
{
  int tid;

  /* Linux LWP ID's are process ID's.  */
  if ((tid = TIDGET (inferior_ptid)) == 0)
    tid = PIDGET (inferior_ptid);	/* Not a threaded program.  */

  if (regno == -1)
    {
      fetch_regs (tid);
      fetch_fpregs (tid);
      return;
    }

  if (GETREGS_SUPPLIES (regno))
    {
      fetch_regs (tid);
      return;
    }

  if (GETFPREGS_SUPPLIES (regno))
    {
      fetch_fpregs (tid);
      return;
    }

  internal_error (__FILE__, __LINE__,
		  "Got request for bad register number %d.", regno);
}

/* Store register REGNO back into the child process.  If REGNO is -1,
   do this for all registers (including the floating point and SSE
   registers).  */
void
store_inferior_registers (int regno)
{
  int tid;

  /* Linux LWP ID's are process ID's.  */
  if ((tid = TIDGET (inferior_ptid)) == 0)
    tid = PIDGET (inferior_ptid);	/* Not a threaded program.  */

  if (regno == -1)
    {
      store_regs (tid, regno);
      store_fpregs (tid, regno);
      return;
    }

  if (GETREGS_SUPPLIES (regno))
    {
      store_regs (tid, regno);
      return;
    }

  if (GETFPREGS_SUPPLIES (regno))
    {
      store_fpregs (tid, regno);
      return;
    }

  internal_error (__FILE__, __LINE__,
		  "Got request to store bad register number %d.", regno);
}


static const unsigned char linux_syscall[] = { 0x0f, 0x05 };

#define LINUX_SYSCALL_LEN (sizeof linux_syscall)

/* The system call number is stored in the %rax register.  */
#define LINUX_SYSCALL_REGNUM 0	/* %rax */

/* We are specifically interested in the sigreturn and rt_sigreturn
   system calls.  */

#ifndef SYS_sigreturn
#define SYS_sigreturn		__NR_sigreturn
#endif
#ifndef SYS_rt_sigreturn
#define SYS_rt_sigreturn	__NR_rt_sigreturn
#endif

/* Offset to saved processor flags, from <asm/sigcontext.h>.  */
#define LINUX_SIGCONTEXT_EFLAGS_OFFSET (152)
/* Offset to saved processor registers from <asm/ucontext.h> */
#define LINUX_UCONTEXT_SIGCONTEXT_OFFSET (36)

/* Resume execution of the inferior process.
   If STEP is nonzero, single-step it.
   If SIGNAL is nonzero, give it that signal.  */

void
child_resume (ptid_t ptid, int step, enum target_signal signal)
{
  int pid = PIDGET (ptid);
  int request = PTRACE_CONT;

  if (pid == -1)
    /* Resume all threads.  */
    /* I think this only gets used in the non-threaded case, where "resume
       all threads" and "resume inferior_ptid" are the same.  */
    pid = PIDGET (inferior_ptid);

  if (step)
    {
      CORE_ADDR pc = read_pc_pid (pid_to_ptid (pid));
      unsigned char buf[LINUX_SYSCALL_LEN];

      request = PTRACE_SINGLESTEP;

      /* Returning from a signal trampoline is done by calling a
         special system call (sigreturn or rt_sigreturn, see
         i386-linux-tdep.c for more information).  This system call
         restores the registers that were saved when the signal was
         raised, including %eflags.  That means that single-stepping
         won't work.  Instead, we'll have to modify the signal context
         that's about to be restored, and set the trace flag there.  */

      /* First check if PC is at a system call.  */
      if (read_memory_nobpt (pc, (char *) buf, LINUX_SYSCALL_LEN) == 0
	  && memcmp (buf, linux_syscall, LINUX_SYSCALL_LEN) == 0)
	{
	  int syscall =
	    read_register_pid (LINUX_SYSCALL_REGNUM, pid_to_ptid (pid));

	  /* Then check the system call number.  */
	  if (syscall == SYS_rt_sigreturn)
	    {
	      CORE_ADDR sp = read_register (SP_REGNUM);
	      CORE_ADDR addr = sp;
	      unsigned long int eflags;

	      addr +=
		sizeof (struct siginfo) + LINUX_UCONTEXT_SIGCONTEXT_OFFSET;

	      /* Set the trace flag in the context that's about to be
	         restored.  */
	      addr += LINUX_SIGCONTEXT_EFLAGS_OFFSET;
	      read_memory (addr, (char *) &eflags, 8);
	      eflags |= 0x0100;
	      write_memory (addr, (char *) &eflags, 8);
	    }
	}
    }

  if (ptrace (request, pid, 0, target_signal_to_host (signal)) == -1)
    perror_with_name ("ptrace");
}


/* Copy LEN bytes to or from inferior's memory starting at MEMADDR
   to debugger memory starting at MYADDR.   Copy to inferior if
   WRITE is nonzero.  TARGET is ignored.

   Returns the length copied, which is either the LEN argument or zero.
   This xfer function does not do partial moves, since child_ops
   doesn't allow memory operations to cross below us in the target stack
   anyway.  */

int
child_xfer_memory (CORE_ADDR memaddr, char *myaddr, int len, int write,
		   struct mem_attrib *attrib ATTRIBUTE_UNUSED,
		   struct target_ops *target)
{
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr = memaddr & -sizeof (PTRACE_XFER_TYPE);
  /* Round ending address up; get number of longwords that makes.  */
  register int count
    = (((memaddr + len) - addr) + sizeof (PTRACE_XFER_TYPE) - 1)
    / sizeof (PTRACE_XFER_TYPE);
  /* Allocate buffer of that many longwords.  */
  register PTRACE_XFER_TYPE *buffer
    = (PTRACE_XFER_TYPE *) alloca (count * sizeof (PTRACE_XFER_TYPE));

  if (write)
    {
      /* Fill start and end extra bytes of buffer with existing memory data.  */
      if (addr != memaddr || len < (int) sizeof (PTRACE_XFER_TYPE))
	{
	  /* Need part of initial word -- fetch it.  */
	  ptrace (PT_READ_I, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) addr, buffer);
	}

      if (count > 1)		/* FIXME, avoid if even boundary */
	{
	  ptrace (PT_READ_I, PIDGET (inferior_ptid),
		  ((PTRACE_ARG3_TYPE)
		   (addr + (count - 1) * sizeof (PTRACE_XFER_TYPE))),
		  buffer + count - 1);
	}

      /* Copy data to be written over corresponding part of buffer */

      memcpy ((char *) buffer + (memaddr & (sizeof (PTRACE_XFER_TYPE) - 1)),
	      myaddr, len);

      /* Write the entire buffer.  */

      for (i = 0; i < count; i++, addr += sizeof (PTRACE_XFER_TYPE))
	{
	  errno = 0;
	  ptrace (PT_WRITE_D, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) addr, buffer[i]);
	  if (errno)
	    {
	      /* Using the appropriate one (I or D) is necessary for
	         Gould NP1, at least.  */
	      errno = 0;
	      ptrace (PT_WRITE_I, PIDGET (inferior_ptid),
		      (PTRACE_ARG3_TYPE) addr, buffer[i]);
	    }
	  if (errno)
	    return 0;
	}
#ifdef CLEAR_INSN_CACHE
      CLEAR_INSN_CACHE ();
#endif
    }
  else
    {
      /* Read all the longwords */
      for (i = 0; i < count; i++, addr += sizeof (PTRACE_XFER_TYPE))
	{
	  errno = 0;
	  ptrace (PT_READ_I, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) addr, buffer + i);
	  if (errno)
	    return 0;
	}

      /* Copy appropriate bytes out of the buffer.  */
      memcpy (myaddr,
	      (char *) buffer + (memaddr & (sizeof (PTRACE_XFER_TYPE) - 1)),
	      len);
    }
  return len;
}

/* Interpreting register set info found in core files.  */

/* Provide registers to GDB from a core file.

   CORE_REG_SECT points to an array of bytes, which are the contents
   of a `note' from a core file which BFD thinks might contain
   register contents.  CORE_REG_SIZE is its size.

   WHICH says which register set corelow suspects this is:
     0 --- the general-purpose register set, in elf_gregset_t format
     2 --- the floating-point register set, in elf_fpregset_t format

   REG_ADDR isn't used on Linux.  */

static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size,
		      int which, CORE_ADDR reg_addr)
{
  elf_gregset_t gregset;
  elf_fpregset_t fpregset;
  switch (which)
    {
    case 0:
      if (core_reg_size != sizeof (gregset))
	warning ("Wrong size gregset in core file.");
      else
	{
	  memcpy (&gregset, core_reg_sect, sizeof (gregset));
	  supply_gregset (&gregset);
	}
      break;

    case 2:
      if (core_reg_size != sizeof (fpregset))
	warning ("Wrong size fpregset in core file.");
      else
	{
	  memcpy (&fpregset, core_reg_sect, sizeof (fpregset));
	  supply_fpregset (&fpregset);
	}
      break;

    default:
      /* We've covered all the kinds of registers we know about here,
         so this must be something we wouldn't know what to do with
         anyway.  Just ignore it.  */
      break;
    }
}

/* Register that we are able to handle Linux ELF core file formats.  */

static struct core_fns linux_elf_core_fns = {
  bfd_target_elf_flavour,	/* core_flavour */
  default_check_format,		/* check_format */
  default_core_sniffer,		/* core_sniffer */
  fetch_core_registers,		/* core_read_registers */
  NULL				/* next */
};


#if !defined (offsetof)
#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER)
#endif

/* Record the value of the debug control register.  */
static long debug_control_mirror;

/* Record which address associates with which register.  */
static CORE_ADDR address_lookup[DR_LASTADDR - DR_FIRSTADDR + 1];

void
_initialize_x86_64_linux_nat (void)
{
  add_core_fns (&linux_elf_core_fns);
}
