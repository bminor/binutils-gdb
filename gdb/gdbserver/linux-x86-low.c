/* GNU/Linux/x86-64 specific low level interface, for the remote server
   for GDB.
   Copyright (C) 2002, 2004, 2005, 2006, 2007, 2008, 2009, 2010
   Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <stddef.h>
#include <signal.h>
#include "server.h"
#include "linux-low.h"
#include "i387-fp.h"
#include "i386-low.h"

#include "gdb_proc_service.h"

/* Defined in auto-generated file reg-i386-linux.c.  */
void init_registers_i386_linux (void);
/* Defined in auto-generated file reg-x86-64-linux.c.  */
void init_registers_x86_64_linux (void);

#include <sys/reg.h>
#include <sys/procfs.h>
#include <sys/ptrace.h>

#ifndef PTRACE_GET_THREAD_AREA
#define PTRACE_GET_THREAD_AREA 25
#endif

/* This definition comes from prctl.h, but some kernels may not have it.  */
#ifndef PTRACE_ARCH_PRCTL
#define PTRACE_ARCH_PRCTL      30
#endif

/* The following definitions come from prctl.h, but may be absent
   for certain configurations.  */
#ifndef ARCH_GET_FS
#define ARCH_SET_GS 0x1001
#define ARCH_SET_FS 0x1002
#define ARCH_GET_FS 0x1003
#define ARCH_GET_GS 0x1004
#endif

/* Per-process arch-specific data we want to keep.  */

struct arch_process_info
{
  struct i386_debug_reg_state debug_reg_state;
};

/* Per-thread arch-specific data we want to keep.  */

struct arch_lwp_info
{
  /* Non-zero if our copy differs from what's recorded in the thread.  */
  int debug_registers_changed;
};

#ifdef __x86_64__

/* Mapping between the general-purpose registers in `struct user'
   format and GDB's register array layout.
   Note that the transfer layout uses 64-bit regs.  */
static /*const*/ int i386_regmap[] = 
{
  RAX * 8, RCX * 8, RDX * 8, RBX * 8,
  RSP * 8, RBP * 8, RSI * 8, RDI * 8,
  RIP * 8, EFLAGS * 8, CS * 8, SS * 8,
  DS * 8, ES * 8, FS * 8, GS * 8
};

#define I386_NUM_REGS (sizeof (i386_regmap) / sizeof (i386_regmap[0]))

/* So code below doesn't have to care, i386 or amd64.  */
#define ORIG_EAX ORIG_RAX

static const int x86_64_regmap[] =
{
  RAX * 8, RBX * 8, RCX * 8, RDX * 8,
  RSI * 8, RDI * 8, RBP * 8, RSP * 8,
  R8 * 8, R9 * 8, R10 * 8, R11 * 8,
  R12 * 8, R13 * 8, R14 * 8, R15 * 8,
  RIP * 8, EFLAGS * 8, CS * 8, SS * 8,
  DS * 8, ES * 8, FS * 8, GS * 8,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1,
  ORIG_RAX * 8
};

#define X86_64_NUM_REGS (sizeof (x86_64_regmap) / sizeof (x86_64_regmap[0]))

#else /* ! __x86_64__ */

/* Mapping between the general-purpose registers in `struct user'
   format and GDB's register array layout.  */
static /*const*/ int i386_regmap[] = 
{
  EAX * 4, ECX * 4, EDX * 4, EBX * 4,
  UESP * 4, EBP * 4, ESI * 4, EDI * 4,
  EIP * 4, EFL * 4, CS * 4, SS * 4,
  DS * 4, ES * 4, FS * 4, GS * 4
};

#define I386_NUM_REGS (sizeof (i386_regmap) / sizeof (i386_regmap[0]))

#endif

/* Called by libthread_db.  */

ps_err_e
ps_get_thread_area (const struct ps_prochandle *ph,
		    lwpid_t lwpid, int idx, void **base)
{
#ifdef __x86_64__
  int use_64bit = register_size (0) == 8;

  if (use_64bit)
    {
      switch (idx)
	{
	case FS:
	  if (ptrace (PTRACE_ARCH_PRCTL, lwpid, base, ARCH_GET_FS) == 0)
	    return PS_OK;
	  break;
	case GS:
	  if (ptrace (PTRACE_ARCH_PRCTL, lwpid, base, ARCH_GET_GS) == 0)
	    return PS_OK;
	  break;
	default:
	  return PS_BADADDR;
	}
      return PS_ERR;
    }
#endif

  {
    unsigned int desc[4];

    if (ptrace (PTRACE_GET_THREAD_AREA, lwpid,
		(void *) (intptr_t) idx, (unsigned long) &desc) < 0)
      return PS_ERR;

    *(int *)base = desc[1];
    return PS_OK;
  }
}

static int
i386_cannot_store_register (int regno)
{
  return regno >= I386_NUM_REGS;
}

static int
i386_cannot_fetch_register (int regno)
{
  return regno >= I386_NUM_REGS;
}

static void
x86_fill_gregset (struct regcache *regcache, void *buf)
{
  int i;

#ifdef __x86_64__
  if (register_size (0) == 8)
    {
      for (i = 0; i < X86_64_NUM_REGS; i++)
	if (x86_64_regmap[i] != -1)
	  collect_register (regcache, i, ((char *) buf) + x86_64_regmap[i]);
      return;
    }
#endif

  for (i = 0; i < I386_NUM_REGS; i++)
    collect_register (regcache, i, ((char *) buf) + i386_regmap[i]);

  collect_register_by_name (regcache, "orig_eax",
			    ((char *) buf) + ORIG_EAX * 4);
}

static void
x86_store_gregset (struct regcache *regcache, const void *buf)
{
  int i;

#ifdef __x86_64__
  if (register_size (0) == 8)
    {
      for (i = 0; i < X86_64_NUM_REGS; i++)
	if (x86_64_regmap[i] != -1)
	  supply_register (regcache, i, ((char *) buf) + x86_64_regmap[i]);
      return;
    }
#endif

  for (i = 0; i < I386_NUM_REGS; i++)
    supply_register (regcache, i, ((char *) buf) + i386_regmap[i]);

  supply_register_by_name (regcache, "orig_eax",
			   ((char *) buf) + ORIG_EAX * 4);
}

static void
x86_fill_fpregset (struct regcache *regcache, void *buf)
{
#ifdef __x86_64__
  i387_cache_to_fxsave (regcache, buf);
#else
  i387_cache_to_fsave (regcache, buf);
#endif
}

static void
x86_store_fpregset (struct regcache *regcache, const void *buf)
{
#ifdef __x86_64__
  i387_fxsave_to_cache (regcache, buf);
#else
  i387_fsave_to_cache (regcache, buf);
#endif
}

#ifndef __x86_64__

static void
x86_fill_fpxregset (struct regcache *regcache, void *buf)
{
  i387_cache_to_fxsave (regcache, buf);
}

static void
x86_store_fpxregset (struct regcache *regcache, const void *buf)
{
  i387_fxsave_to_cache (regcache, buf);
}

#endif

/* ??? The non-biarch i386 case stores all the i387 regs twice.
   Once in i387_.*fsave.* and once in i387_.*fxsave.*.
   This is, presumably, to handle the case where PTRACE_[GS]ETFPXREGS
   doesn't work.  IWBN to avoid the duplication in the case where it
   does work.  Maybe the arch_setup routine could check whether it works
   and update target_regsets accordingly, maybe by moving target_regsets
   to linux_target_ops and set the right one there, rather than having to
   modify the target_regsets global.  */

struct regset_info target_regsets[] =
{
#ifdef HAVE_PTRACE_GETREGS
  { PTRACE_GETREGS, PTRACE_SETREGS, sizeof (elf_gregset_t),
    GENERAL_REGS,
    x86_fill_gregset, x86_store_gregset },
# ifndef __x86_64__
#  ifdef HAVE_PTRACE_GETFPXREGS
  { PTRACE_GETFPXREGS, PTRACE_SETFPXREGS, sizeof (elf_fpxregset_t),
    EXTENDED_REGS,
    x86_fill_fpxregset, x86_store_fpxregset },
#  endif
# endif
  { PTRACE_GETFPREGS, PTRACE_SETFPREGS, sizeof (elf_fpregset_t),
    FP_REGS,
    x86_fill_fpregset, x86_store_fpregset },
#endif /* HAVE_PTRACE_GETREGS */
  { 0, 0, -1, -1, NULL, NULL }
};

static CORE_ADDR
x86_get_pc (struct regcache *regcache)
{
  int use_64bit = register_size (0) == 8;

  if (use_64bit)
    {
      unsigned long pc;
      collect_register_by_name (regcache, "rip", &pc);
      return (CORE_ADDR) pc;
    }
  else
    {
      unsigned int pc;
      collect_register_by_name (regcache, "eip", &pc);
      return (CORE_ADDR) pc;
    }
}

static void
x86_set_pc (struct regcache *regcache, CORE_ADDR pc)
{
  int use_64bit = register_size (0) == 8;

  if (use_64bit)
    {
      unsigned long newpc = pc;
      supply_register_by_name (regcache, "rip", &newpc);
    }
  else
    {
      unsigned int newpc = pc;
      supply_register_by_name (regcache, "eip", &newpc);
    }
}

static const unsigned char x86_breakpoint[] = { 0xCC };
#define x86_breakpoint_len 1

static int
x86_breakpoint_at (CORE_ADDR pc)
{
  unsigned char c;

  read_inferior_memory (pc, &c, 1);
  if (c == 0xCC)
    return 1;

  return 0;
}

/* Support for debug registers.  */

static unsigned long
x86_linux_dr_get (ptid_t ptid, int regnum)
{
  int tid;
  unsigned long value;

  tid = ptid_get_lwp (ptid);

  errno = 0;
  value = ptrace (PTRACE_PEEKUSER, tid,
		  offsetof (struct user, u_debugreg[regnum]), 0);
  if (errno != 0)
    error ("Couldn't read debug register");

  return value;
}

static void
x86_linux_dr_set (ptid_t ptid, int regnum, unsigned long value)
{
  int tid;

  tid = ptid_get_lwp (ptid);

  errno = 0;
  ptrace (PTRACE_POKEUSER, tid,
	  offsetof (struct user, u_debugreg[regnum]), value);
  if (errno != 0)
    error ("Couldn't write debug register");
}

/* Update the inferior's debug register REGNUM from STATE.  */

void
i386_dr_low_set_addr (const struct i386_debug_reg_state *state, int regnum)
{
  struct inferior_list_entry *lp;
  CORE_ADDR addr;
  /* Only need to update the threads of this process.  */
  int pid = pid_of (get_thread_lwp (current_inferior));

  if (! (regnum >= 0 && regnum <= DR_LASTADDR - DR_FIRSTADDR))
    fatal ("Invalid debug register %d", regnum);

  addr = state->dr_mirror[regnum];

  for (lp = all_lwps.head; lp; lp = lp->next)
    {
      struct lwp_info *lwp = (struct lwp_info *) lp;

      /* The actual update is done later, we just mark that the register
	 needs updating.  */
      if (pid_of (lwp) == pid)
	lwp->arch_private->debug_registers_changed = 1;
    }
}

/* Update the inferior's DR7 debug control register from STATE.  */

void
i386_dr_low_set_control (const struct i386_debug_reg_state *state)
{
  struct inferior_list_entry *lp;
  /* Only need to update the threads of this process.  */
  int pid = pid_of (get_thread_lwp (current_inferior));

  for (lp = all_lwps.head; lp; lp = lp->next)
    {
      struct lwp_info *lwp = (struct lwp_info *) lp;

      /* The actual update is done later, we just mark that the register
	 needs updating.  */
      if (pid_of (lwp) == pid)
	lwp->arch_private->debug_registers_changed = 1;
    }
}

/* Get the value of the DR6 debug status register from the inferior
   and record it in STATE.  */

void
i386_dr_low_get_status (struct i386_debug_reg_state *state)
{
  struct lwp_info *lwp = get_thread_lwp (current_inferior);
  ptid_t ptid = ptid_of (lwp);

  state->dr_status_mirror = x86_linux_dr_get (ptid, DR_STATUS);
}

/* Watchpoint support.  */

static int
x86_insert_point (char type, CORE_ADDR addr, int len)
{
  struct process_info *proc = current_process ();
  switch (type)
    {
    case '2':
    case '3':
    case '4':
      return i386_low_insert_watchpoint (&proc->private->arch_private->debug_reg_state,
					 type, addr, len);
    default:
      /* Unsupported.  */
      return 1;
    }
}

static int
x86_remove_point (char type, CORE_ADDR addr, int len)
{
  struct process_info *proc = current_process ();
  switch (type)
    {
    case '2':
    case '3':
    case '4':
      return i386_low_remove_watchpoint (&proc->private->arch_private->debug_reg_state,
					 type, addr, len);
    default:
      /* Unsupported.  */
      return 1;
    }
}

static int
x86_stopped_by_watchpoint (void)
{
  struct process_info *proc = current_process ();
  return i386_low_stopped_by_watchpoint (&proc->private->arch_private->debug_reg_state);
}

static CORE_ADDR
x86_stopped_data_address (void)
{
  struct process_info *proc = current_process ();
  CORE_ADDR addr;
  if (i386_low_stopped_data_address (&proc->private->arch_private->debug_reg_state,
				     &addr))
    return addr;
  return 0;
}

/* Called when a new process is created.  */

static struct arch_process_info *
x86_linux_new_process (void)
{
  struct arch_process_info *info = xcalloc (1, sizeof (*info));

  i386_low_init_dregs (&info->debug_reg_state);

  return info;
}

/* Called when a new thread is detected.  */

static struct arch_lwp_info *
x86_linux_new_thread (void)
{
  struct arch_lwp_info *info = xcalloc (1, sizeof (*info));

  info->debug_registers_changed = 1;

  return info;
}

/* Called when resuming a thread.
   If the debug regs have changed, update the thread's copies.  */

static void
x86_linux_prepare_to_resume (struct lwp_info *lwp)
{
  if (lwp->arch_private->debug_registers_changed)
    {
      int i;
      ptid_t ptid = ptid_of (lwp);
      int pid = ptid_get_pid (ptid);
      struct process_info *proc = find_process_pid (pid);
      struct i386_debug_reg_state *state = &proc->private->arch_private->debug_reg_state;

      for (i = DR_FIRSTADDR; i <= DR_LASTADDR; i++)
	x86_linux_dr_set (ptid, i, state->dr_mirror[i]);

      x86_linux_dr_set (ptid, DR_CONTROL, state->dr_control_mirror);

      lwp->arch_private->debug_registers_changed = 0;
    }
}

/* When GDBSERVER is built as a 64-bit application on linux, the
   PTRACE_GETSIGINFO data is always presented in 64-bit layout.  Since
   debugging a 32-bit inferior with a 64-bit GDBSERVER should look the same
   as debugging it with a 32-bit GDBSERVER, we do the 32-bit <-> 64-bit
   conversion in-place ourselves.  */

/* These types below (compat_*) define a siginfo type that is layout
   compatible with the siginfo type exported by the 32-bit userspace
   support.  */

#ifdef __x86_64__

typedef int compat_int_t;
typedef unsigned int compat_uptr_t;

typedef int compat_time_t;
typedef int compat_timer_t;
typedef int compat_clock_t;

struct compat_timeval
{
  compat_time_t tv_sec;
  int tv_usec;
};

typedef union compat_sigval
{
  compat_int_t sival_int;
  compat_uptr_t sival_ptr;
} compat_sigval_t;

typedef struct compat_siginfo
{
  int si_signo;
  int si_errno;
  int si_code;

  union
  {
    int _pad[((128 / sizeof (int)) - 3)];

    /* kill() */
    struct
    {
      unsigned int _pid;
      unsigned int _uid;
    } _kill;

    /* POSIX.1b timers */
    struct
    {
      compat_timer_t _tid;
      int _overrun;
      compat_sigval_t _sigval;
    } _timer;

    /* POSIX.1b signals */
    struct
    {
      unsigned int _pid;
      unsigned int _uid;
      compat_sigval_t _sigval;
    } _rt;

    /* SIGCHLD */
    struct
    {
      unsigned int _pid;
      unsigned int _uid;
      int _status;
      compat_clock_t _utime;
      compat_clock_t _stime;
    } _sigchld;

    /* SIGILL, SIGFPE, SIGSEGV, SIGBUS */
    struct
    {
      unsigned int _addr;
    } _sigfault;

    /* SIGPOLL */
    struct
    {
      int _band;
      int _fd;
    } _sigpoll;
  } _sifields;
} compat_siginfo_t;

#define cpt_si_pid _sifields._kill._pid
#define cpt_si_uid _sifields._kill._uid
#define cpt_si_timerid _sifields._timer._tid
#define cpt_si_overrun _sifields._timer._overrun
#define cpt_si_status _sifields._sigchld._status
#define cpt_si_utime _sifields._sigchld._utime
#define cpt_si_stime _sifields._sigchld._stime
#define cpt_si_ptr _sifields._rt._sigval.sival_ptr
#define cpt_si_addr _sifields._sigfault._addr
#define cpt_si_band _sifields._sigpoll._band
#define cpt_si_fd _sifields._sigpoll._fd

/* glibc at least up to 2.3.2 doesn't have si_timerid, si_overrun.
   In their place is si_timer1,si_timer2.  */
#ifndef si_timerid
#define si_timerid si_timer1
#endif
#ifndef si_overrun
#define si_overrun si_timer2
#endif

static void
compat_siginfo_from_siginfo (compat_siginfo_t *to, siginfo_t *from)
{
  memset (to, 0, sizeof (*to));

  to->si_signo = from->si_signo;
  to->si_errno = from->si_errno;
  to->si_code = from->si_code;

  if (to->si_code < 0)
    {
      to->cpt_si_ptr = (intptr_t) from->si_ptr;
    }
  else if (to->si_code == SI_USER)
    {
      to->cpt_si_pid = from->si_pid;
      to->cpt_si_uid = from->si_uid;
    }
  else if (to->si_code == SI_TIMER)
    {
      to->cpt_si_timerid = from->si_timerid;
      to->cpt_si_overrun = from->si_overrun;
      to->cpt_si_ptr = (intptr_t) from->si_ptr;
    }
  else
    {
      switch (to->si_signo)
	{
	case SIGCHLD:
	  to->cpt_si_pid = from->si_pid;
	  to->cpt_si_uid = from->si_uid;
	  to->cpt_si_status = from->si_status;
	  to->cpt_si_utime = from->si_utime;
	  to->cpt_si_stime = from->si_stime;
	  break;
	case SIGILL:
	case SIGFPE:
	case SIGSEGV:
	case SIGBUS:
	  to->cpt_si_addr = (intptr_t) from->si_addr;
	  break;
	case SIGPOLL:
	  to->cpt_si_band = from->si_band;
	  to->cpt_si_fd = from->si_fd;
	  break;
	default:
	  to->cpt_si_pid = from->si_pid;
	  to->cpt_si_uid = from->si_uid;
	  to->cpt_si_ptr = (intptr_t) from->si_ptr;
	  break;
	}
    }
}

static void
siginfo_from_compat_siginfo (siginfo_t *to, compat_siginfo_t *from)
{
  memset (to, 0, sizeof (*to));

  to->si_signo = from->si_signo;
  to->si_errno = from->si_errno;
  to->si_code = from->si_code;

  if (to->si_code < 0)
    {
      to->si_ptr = (void *) (intptr_t) from->cpt_si_ptr;
    }
  else if (to->si_code == SI_USER)
    {
      to->si_pid = from->cpt_si_pid;
      to->si_uid = from->cpt_si_uid;
    }
  else if (to->si_code == SI_TIMER)
    {
      to->si_timerid = from->cpt_si_timerid;
      to->si_overrun = from->cpt_si_overrun;
      to->si_ptr = (void *) (intptr_t) from->cpt_si_ptr;
    }
  else
    {
      switch (to->si_signo)
	{
	case SIGCHLD:
	  to->si_pid = from->cpt_si_pid;
	  to->si_uid = from->cpt_si_uid;
	  to->si_status = from->cpt_si_status;
	  to->si_utime = from->cpt_si_utime;
	  to->si_stime = from->cpt_si_stime;
	  break;
	case SIGILL:
	case SIGFPE:
	case SIGSEGV:
	case SIGBUS:
	  to->si_addr = (void *) (intptr_t) from->cpt_si_addr;
	  break;
	case SIGPOLL:
	  to->si_band = from->cpt_si_band;
	  to->si_fd = from->cpt_si_fd;
	  break;
	default:
	  to->si_pid = from->cpt_si_pid;
	  to->si_uid = from->cpt_si_uid;
	  to->si_ptr = (void* ) (intptr_t) from->cpt_si_ptr;
	  break;
	}
    }
}

#endif /* __x86_64__ */

/* Convert a native/host siginfo object, into/from the siginfo in the
   layout of the inferiors' architecture.  Returns true if any
   conversion was done; false otherwise.  If DIRECTION is 1, then copy
   from INF to NATIVE.  If DIRECTION is 0, copy from NATIVE to
   INF.  */

static int
x86_siginfo_fixup (struct siginfo *native, void *inf, int direction)
{
#ifdef __x86_64__
  /* Is the inferior 32-bit?  If so, then fixup the siginfo object.  */
  if (register_size (0) == 4)
    {
      if (sizeof (struct siginfo) != sizeof (compat_siginfo_t))
	fatal ("unexpected difference in siginfo");

      if (direction == 0)
	compat_siginfo_from_siginfo ((struct compat_siginfo *) inf, native);
      else
	siginfo_from_compat_siginfo (native, (struct compat_siginfo *) inf);

      return 1;
    }
#endif

  return 0;
}

/* Initialize gdbserver for the architecture of the inferior.  */

static void
x86_arch_setup (void)
{
#ifdef __x86_64__
  int pid = pid_of (get_thread_lwp (current_inferior));
  char *file = linux_child_pid_to_exec_file (pid);
  int use_64bit = elf_64_file_p (file);

  free (file);

  if (use_64bit < 0)
    {
      /* This can only happen if /proc/<pid>/exe is unreadable,
	 but "that can't happen" if we've gotten this far.
	 Fall through and assume this is a 32-bit program.  */
    }
  else if (use_64bit)
    {
      init_registers_x86_64_linux ();

      /* Amd64 doesn't have HAVE_LINUX_USRREGS.  */
      the_low_target.num_regs = -1;
      the_low_target.regmap = NULL;
      the_low_target.cannot_fetch_register = NULL;
      the_low_target.cannot_store_register = NULL;

      /* Amd64 has 16 xmm regs.  */
      num_xmm_registers = 16;

      return;
    }
#endif

  /* Ok we have a 32-bit inferior.  */

  init_registers_i386_linux ();

  the_low_target.num_regs = I386_NUM_REGS;
  the_low_target.regmap = i386_regmap;
  the_low_target.cannot_fetch_register = i386_cannot_fetch_register;
  the_low_target.cannot_store_register = i386_cannot_store_register;

  /* I386 has 8 xmm regs.  */
  num_xmm_registers = 8;
}

/* This is initialized assuming an amd64 target.
   x86_arch_setup will correct it for i386 or amd64 targets.  */

struct linux_target_ops the_low_target =
{
  x86_arch_setup,
  -1,
  NULL,
  NULL,
  NULL,
  x86_get_pc,
  x86_set_pc,
  x86_breakpoint,
  x86_breakpoint_len,
  NULL,
  1,
  x86_breakpoint_at,
  x86_insert_point,
  x86_remove_point,
  x86_stopped_by_watchpoint,
  x86_stopped_data_address,
  /* collect_ptrace_register/supply_ptrace_register are not needed in the
     native i386 case (no registers smaller than an xfer unit), and are not
     used in the biarch case (HAVE_LINUX_USRREGS is not defined).  */
  NULL,
  NULL,
  /* need to fix up i386 siginfo if host is amd64 */
  x86_siginfo_fixup,
  x86_linux_new_process,
  x86_linux_new_thread,
  x86_linux_prepare_to_resume
};
