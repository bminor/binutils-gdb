/* Target-dependent code for GNU/Linux i386.

   Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2007, 2008, 2009
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

#include "defs.h"
#include "gdbcore.h"
#include "frame.h"
#include "value.h"
#include "regcache.h"
#include "inferior.h"
#include "osabi.h"
#include "reggroups.h"
#include "dwarf2-frame.h"
#include "gdb_string.h"

#include "i386-tdep.h"
#include "i386-linux-tdep.h"
#include "linux-tdep.h"
#include "glibc-tdep.h"
#include "solib-svr4.h"
#include "symtab.h"
#include "arch-utils.h"
#include "regset.h"

#include "record.h"
#include "linux-record.h"
#include <stdint.h>

/* Supported register note sections.  */
static struct core_regset_section i386_linux_regset_sections[] =
{
  { ".reg", 144 },
  { ".reg2", 108 },
  { ".reg-xfp", 512 },
  { NULL, 0 }
};

/* Return the name of register REG.  */

static const char *
i386_linux_register_name (struct gdbarch *gdbarch, int reg)
{
  /* Deal with the extra "orig_eax" pseudo register.  */
  if (reg == I386_LINUX_ORIG_EAX_REGNUM)
    return "orig_eax";

  return i386_register_name (gdbarch, reg);
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
   to occur anywhere other than in a signal trampoline.

   It kind of sucks that we have to read memory from the process in
   order to identify a signal trampoline, but there doesn't seem to be
   any other way.  Therefore we only do the memory reads if no
   function name could be identified, which should be the case since
   the code is on the stack.

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

static const gdb_byte linux_sigtramp_code[] =
{
  LINUX_SIGTRAMP_INSN0,					/* pop %eax */
  LINUX_SIGTRAMP_INSN1, 0x77, 0x00, 0x00, 0x00,		/* mov $0x77, %eax */
  LINUX_SIGTRAMP_INSN2, 0x80				/* int $0x80 */
};

#define LINUX_SIGTRAMP_LEN (sizeof linux_sigtramp_code)

/* If THIS_FRAME is a sigtramp routine, return the address of the
   start of the routine.  Otherwise, return 0.  */

static CORE_ADDR
i386_linux_sigtramp_start (struct frame_info *this_frame)
{
  CORE_ADDR pc = get_frame_pc (this_frame);
  gdb_byte buf[LINUX_SIGTRAMP_LEN];

  /* We only recognize a signal trampoline if PC is at the start of
     one of the three instructions.  We optimize for finding the PC at
     the start, as will be the case when the trampoline is not the
     first frame on the stack.  We assume that in the case where the
     PC is not at the start of the instruction sequence, there will be
     a few trailing readable bytes on the stack.  */

  if (!safe_frame_unwind_memory (this_frame, pc, buf, LINUX_SIGTRAMP_LEN))
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

      if (!safe_frame_unwind_memory (this_frame, pc, buf, LINUX_SIGTRAMP_LEN))
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

static const gdb_byte linux_rt_sigtramp_code[] =
{
  LINUX_RT_SIGTRAMP_INSN0, 0xad, 0x00, 0x00, 0x00,	/* mov $0xad, %eax */
  LINUX_RT_SIGTRAMP_INSN1, 0x80				/* int $0x80 */
};

#define LINUX_RT_SIGTRAMP_LEN (sizeof linux_rt_sigtramp_code)

/* If THIS_FRAME is an RT sigtramp routine, return the address of the
   start of the routine.  Otherwise, return 0.  */

static CORE_ADDR
i386_linux_rt_sigtramp_start (struct frame_info *this_frame)
{
  CORE_ADDR pc = get_frame_pc (this_frame);
  gdb_byte buf[LINUX_RT_SIGTRAMP_LEN];

  /* We only recognize a signal trampoline if PC is at the start of
     one of the two instructions.  We optimize for finding the PC at
     the start, as will be the case when the trampoline is not the
     first frame on the stack.  We assume that in the case where the
     PC is not at the start of the instruction sequence, there will be
     a few trailing readable bytes on the stack.  */

  if (!safe_frame_unwind_memory (this_frame, pc, buf, LINUX_RT_SIGTRAMP_LEN))
    return 0;

  if (buf[0] != LINUX_RT_SIGTRAMP_INSN0)
    {
      if (buf[0] != LINUX_RT_SIGTRAMP_INSN1)
	return 0;

      pc -= LINUX_RT_SIGTRAMP_OFFSET1;

      if (!safe_frame_unwind_memory (this_frame, pc, buf,
				     LINUX_RT_SIGTRAMP_LEN))
	return 0;
    }

  if (memcmp (buf, linux_rt_sigtramp_code, LINUX_RT_SIGTRAMP_LEN) != 0)
    return 0;

  return pc;
}

/* Return whether THIS_FRAME corresponds to a GNU/Linux sigtramp
   routine.  */

static int
i386_linux_sigtramp_p (struct frame_info *this_frame)
{
  CORE_ADDR pc = get_frame_pc (this_frame);
  char *name;

  find_pc_partial_function (pc, &name, NULL, NULL);

  /* If we have NAME, we can optimize the search.  The trampolines are
     named __restore and __restore_rt.  However, they aren't dynamically
     exported from the shared C library, so the trampoline may appear to
     be part of the preceding function.  This should always be sigaction,
     __sigaction, or __libc_sigaction (all aliases to the same function).  */
  if (name == NULL || strstr (name, "sigaction") != NULL)
    return (i386_linux_sigtramp_start (this_frame) != 0
	    || i386_linux_rt_sigtramp_start (this_frame) != 0);

  return (strcmp ("__restore", name) == 0
	  || strcmp ("__restore_rt", name) == 0);
}

/* Return one if the PC of THIS_FRAME is in a signal trampoline which
   may have DWARF-2 CFI.  */

static int
i386_linux_dwarf_signal_frame_p (struct gdbarch *gdbarch,
				 struct frame_info *this_frame)
{
  CORE_ADDR pc = get_frame_pc (this_frame);
  char *name;

  find_pc_partial_function (pc, &name, NULL, NULL);

  /* If a vsyscall DSO is in use, the signal trampolines may have these
     names.  */
  if (name && (strcmp (name, "__kernel_sigreturn") == 0
	       || strcmp (name, "__kernel_rt_sigreturn") == 0))
    return 1;

  return 0;
}

/* Offset to struct sigcontext in ucontext, from <asm/ucontext.h>.  */
#define I386_LINUX_UCONTEXT_SIGCONTEXT_OFFSET 20

/* Assuming THIS_FRAME is a GNU/Linux sigtramp routine, return the
   address of the associated sigcontext structure.  */

static CORE_ADDR
i386_linux_sigcontext_addr (struct frame_info *this_frame)
{
  CORE_ADDR pc;
  CORE_ADDR sp;
  gdb_byte buf[4];

  get_frame_register (this_frame, I386_ESP_REGNUM, buf);
  sp = extract_unsigned_integer (buf, 4);

  pc = i386_linux_sigtramp_start (this_frame);
  if (pc)
    {
      /* The sigcontext structure lives on the stack, right after
	 the signum argument.  We determine the address of the
	 sigcontext structure by looking at the frame's stack
	 pointer.  Keep in mind that the first instruction of the
	 sigtramp code is "pop %eax".  If the PC is after this
	 instruction, adjust the returned value accordingly.  */
      if (pc == get_frame_pc (this_frame))
	return sp + 4;
      return sp;
    }

  pc = i386_linux_rt_sigtramp_start (this_frame);
  if (pc)
    {
      CORE_ADDR ucontext_addr;

      /* The sigcontext structure is part of the user context.  A
	 pointer to the user context is passed as the third argument
	 to the signal handler.  */
      read_memory (sp + 8, buf, 4);
      ucontext_addr = extract_unsigned_integer (buf, 4);
      return ucontext_addr + I386_LINUX_UCONTEXT_SIGCONTEXT_OFFSET;
    }

  error (_("Couldn't recognize signal trampoline."));
  return 0;
}

/* Set the program counter for process PTID to PC.  */

static void
i386_linux_write_pc (struct regcache *regcache, CORE_ADDR pc)
{
  regcache_cooked_write_unsigned (regcache, I386_EIP_REGNUM, pc);

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
  regcache_cooked_write_unsigned (regcache, I386_LINUX_ORIG_EAX_REGNUM, -1);
}

/* Parse the arguments of current system call instruction and record
   the values of the registers and memory that will be changed into
   "record_arch_list".  This instruction is "int 0x80" (Linux
   Kernel2.4) or "sysenter" (Linux Kernel 2.6).

   Return -1 if something wrong.  */

static struct linux_record_tdep i386_linux_record_tdep;

static int
i386_linux_intx80_sysenter_record (struct regcache *regcache)
{
  int ret;
  uint32_t tmpu32;

  regcache_raw_read (regcache, I386_EAX_REGNUM, (gdb_byte *)&tmpu32);

  ret = record_linux_system_call (tmpu32, regcache,
				  &i386_linux_record_tdep);
  if (ret)
    return ret;

  /* Record the return value of the system call.  */
  if (record_arch_list_add_reg (regcache, I386_EAX_REGNUM))
    return -1;

  return 0;
}


/* The register sets used in GNU/Linux ELF core-dumps are identical to
   the register sets in `struct user' that are used for a.out
   core-dumps.  These are also used by ptrace(2).  The corresponding
   types are `elf_gregset_t' for the general-purpose registers (with
   `elf_greg_t' the type of a single GP register) and `elf_fpregset_t'
   for the floating-point registers.

   Those types used to be available under the names `gregset_t' and
   `fpregset_t' too, and GDB used those names in the past.  But those
   names are now used for the register sets used in the `mcontext_t'
   type, which have a different size and layout.  */

/* Mapping between the general-purpose registers in `struct user'
   format and GDB's register cache layout.  */

/* From <sys/reg.h>.  */
static int i386_linux_gregset_reg_offset[] =
{
  6 * 4,			/* %eax */
  1 * 4,			/* %ecx */
  2 * 4,			/* %edx */
  0 * 4,			/* %ebx */
  15 * 4,			/* %esp */
  5 * 4,			/* %ebp */
  3 * 4,			/* %esi */
  4 * 4,			/* %edi */
  12 * 4,			/* %eip */
  14 * 4,			/* %eflags */
  13 * 4,			/* %cs */
  16 * 4,			/* %ss */
  7 * 4,			/* %ds */
  8 * 4,			/* %es */
  9 * 4,			/* %fs */
  10 * 4,			/* %gs */
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1,
  11 * 4			/* "orig_eax" */
};

/* Mapping between the general-purpose registers in `struct
   sigcontext' format and GDB's register cache layout.  */

/* From <asm/sigcontext.h>.  */
static int i386_linux_sc_reg_offset[] =
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

/* These macros are the size of the type that will be used in a system
   call.  The values of these macros were obtained from Linux Kernel
   source.  */
#define I386_LINUX_RECORD_SIZE__old_kernel_stat	32
#define I386_LINUX_RECORD_SIZE_tms			16
#define I386_LINUX_RECORD_SIZE_loff_t			8
#define I386_LINUX_RECORD_SIZE_flock			16
#define I386_LINUX_RECORD_SIZE_oldold_utsname		45
#define I386_LINUX_RECORD_SIZE_ustat			20
#define I386_LINUX_RECORD_SIZE_old_sigaction		140
#define I386_LINUX_RECORD_SIZE_old_sigset_t		128
#define I386_LINUX_RECORD_SIZE_rlimit			8
#define I386_LINUX_RECORD_SIZE_rusage			72
#define I386_LINUX_RECORD_SIZE_timeval		8
#define I386_LINUX_RECORD_SIZE_timezone		8
#define I386_LINUX_RECORD_SIZE_old_gid_t		2
#define I386_LINUX_RECORD_SIZE_old_uid_t		2
#define I386_LINUX_RECORD_SIZE_fd_set			128
#define I386_LINUX_RECORD_SIZE_dirent			268
#define I386_LINUX_RECORD_SIZE_dirent64		276
#define I386_LINUX_RECORD_SIZE_statfs			64
#define I386_LINUX_RECORD_SIZE_statfs64		84
#define I386_LINUX_RECORD_SIZE_sockaddr		16
#define I386_LINUX_RECORD_SIZE_int			4
#define I386_LINUX_RECORD_SIZE_long			4
#define I386_LINUX_RECORD_SIZE_ulong			4
#define I386_LINUX_RECORD_SIZE_msghdr			28
#define I386_LINUX_RECORD_SIZE_itimerval		16
#define I386_LINUX_RECORD_SIZE_stat			88
#define I386_LINUX_RECORD_SIZE_old_utsname		325
#define I386_LINUX_RECORD_SIZE_sysinfo		64
#define I386_LINUX_RECORD_SIZE_msqid_ds		88
#define I386_LINUX_RECORD_SIZE_shmid_ds		84
#define I386_LINUX_RECORD_SIZE_new_utsname		390
#define I386_LINUX_RECORD_SIZE_timex			128
#define I386_LINUX_RECORD_SIZE_mem_dqinfo		24
#define I386_LINUX_RECORD_SIZE_if_dqblk		68
#define I386_LINUX_RECORD_SIZE_fs_quota_stat		68
#define I386_LINUX_RECORD_SIZE_timespec		8
#define I386_LINUX_RECORD_SIZE_pollfd			8
#define I386_LINUX_RECORD_SIZE_NFS_FHSIZE		32
#define I386_LINUX_RECORD_SIZE_knfsd_fh		132
#define I386_LINUX_RECORD_SIZE_TASK_COMM_LEN		16
#define I386_LINUX_RECORD_SIZE_sigaction		140
#define I386_LINUX_RECORD_SIZE_sigset_t		8
#define I386_LINUX_RECORD_SIZE_siginfo_t		128
#define I386_LINUX_RECORD_SIZE_cap_user_data_t	12
#define I386_LINUX_RECORD_SIZE_stack_t		12
#define I386_LINUX_RECORD_SIZE_off_t			I386_LINUX_RECORD_SIZE_long
#define I386_LINUX_RECORD_SIZE_stat64			96
#define I386_LINUX_RECORD_SIZE_gid_t			2
#define I386_LINUX_RECORD_SIZE_uid_t			2
#define I386_LINUX_RECORD_SIZE_PAGE_SIZE		4096
#define I386_LINUX_RECORD_SIZE_flock64		24
#define I386_LINUX_RECORD_SIZE_user_desc		16
#define I386_LINUX_RECORD_SIZE_io_event		32
#define I386_LINUX_RECORD_SIZE_iocb			64
#define I386_LINUX_RECORD_SIZE_epoll_event		12
#define I386_LINUX_RECORD_SIZE_itimerspec		(I386_LINUX_RECORD_SIZE_timespec * 2)
#define I386_LINUX_RECORD_SIZE_mq_attr		32
#define I386_LINUX_RECORD_SIZE_siginfo		128
#define I386_LINUX_RECORD_SIZE_termios		36
#define I386_LINUX_RECORD_SIZE_termios2		44
#define I386_LINUX_RECORD_SIZE_pid_t			4
#define I386_LINUX_RECORD_SIZE_winsize		8
#define I386_LINUX_RECORD_SIZE_char			8
#define I386_LINUX_RECORD_SIZE_serial_struct		60
#define I386_LINUX_RECORD_SIZE_serial_icounter_struct	80
#define I386_LINUX_RECORD_SIZE_hayes_esp_config	12

/* These macros are the values of the second argument of system call
   "sys_ioctl".  The values of these macros were obtained from Linux
   Kernel source.  */
#define I386_LINUX_RECORD_IOCTL_TCGETS		0x5401
#define I386_LINUX_RECORD_IOCTL_TCSETS		0x5402
#define I386_LINUX_RECORD_IOCTL_TCSETSW		0x5403
#define I386_LINUX_RECORD_IOCTL_TCSETSF		0x5404
#define I386_LINUX_RECORD_IOCTL_TCGETA		0x5405
#define I386_LINUX_RECORD_IOCTL_TCSETA		0x5406
#define I386_LINUX_RECORD_IOCTL_TCSETAW		0x5407
#define I386_LINUX_RECORD_IOCTL_TCSETAF		0x5408
#define I386_LINUX_RECORD_IOCTL_TCSBRK		0x5409
#define I386_LINUX_RECORD_IOCTL_TCXONC		0x540A
#define I386_LINUX_RECORD_IOCTL_TCFLSH		0x540B
#define I386_LINUX_RECORD_IOCTL_TIOCEXCL		0x540C
#define I386_LINUX_RECORD_IOCTL_TIOCNXCL		0x540D
#define I386_LINUX_RECORD_IOCTL_TIOCSCTTY		0x540E
#define I386_LINUX_RECORD_IOCTL_TIOCGPGRP		0x540F
#define I386_LINUX_RECORD_IOCTL_TIOCSPGRP		0x5410
#define I386_LINUX_RECORD_IOCTL_TIOCOUTQ		0x5411
#define I386_LINUX_RECORD_IOCTL_TIOCSTI		0x5412
#define I386_LINUX_RECORD_IOCTL_TIOCGWINSZ		0x5413
#define I386_LINUX_RECORD_IOCTL_TIOCSWINSZ		0x5414
#define I386_LINUX_RECORD_IOCTL_TIOCMGET		0x5415
#define I386_LINUX_RECORD_IOCTL_TIOCMBIS		0x5416
#define I386_LINUX_RECORD_IOCTL_TIOCMBIC		0x5417
#define I386_LINUX_RECORD_IOCTL_TIOCMSET		0x5418
#define I386_LINUX_RECORD_IOCTL_TIOCGSOFTCAR		0x5419
#define I386_LINUX_RECORD_IOCTL_TIOCSSOFTCAR		0x541A
#define I386_LINUX_RECORD_IOCTL_FIONREAD		0x541B
#define I386_LINUX_RECORD_IOCTL_TIOCINQ		I386_LINUX_RECORD_IOCTL_FIONREAD
#define I386_LINUX_RECORD_IOCTL_TIOCLINUX		0x541C
#define I386_LINUX_RECORD_IOCTL_TIOCCONS		0x541D
#define I386_LINUX_RECORD_IOCTL_TIOCGSERIAL		0x541E
#define I386_LINUX_RECORD_IOCTL_TIOCSSERIAL		0x541F
#define I386_LINUX_RECORD_IOCTL_TIOCPKT		0x5420
#define I386_LINUX_RECORD_IOCTL_FIONBIO		0x5421
#define I386_LINUX_RECORD_IOCTL_TIOCNOTTY		0x5422
#define I386_LINUX_RECORD_IOCTL_TIOCSETD		0x5423
#define I386_LINUX_RECORD_IOCTL_TIOCGETD		0x5424
#define I386_LINUX_RECORD_IOCTL_TCSBRKP		0x5425
#define I386_LINUX_RECORD_IOCTL_TIOCTTYGSTRUCT 	0x5426
#define I386_LINUX_RECORD_IOCTL_TIOCSBRK		0x5427
#define I386_LINUX_RECORD_IOCTL_TIOCCBRK		0x5428
#define I386_LINUX_RECORD_IOCTL_TIOCGSID		0x5429
#define I386_LINUX_RECORD_IOCTL_TCGETS2		0x802c542a
#define I386_LINUX_RECORD_IOCTL_TCSETS2		0x402c542b
#define I386_LINUX_RECORD_IOCTL_TCSETSW2		0x402c542c
#define I386_LINUX_RECORD_IOCTL_TCSETSF2		0x402c542d
#define I386_LINUX_RECORD_IOCTL_TIOCGPTN		0x80045430
#define I386_LINUX_RECORD_IOCTL_TIOCSPTLCK		0x40045431
#define I386_LINUX_RECORD_IOCTL_FIONCLEX		0x5450
#define I386_LINUX_RECORD_IOCTL_FIOCLEX		0x5451
#define I386_LINUX_RECORD_IOCTL_FIOASYNC		0x5452
#define I386_LINUX_RECORD_IOCTL_TIOCSERCONFIG		0x5453
#define I386_LINUX_RECORD_IOCTL_TIOCSERGWILD		0x5454
#define I386_LINUX_RECORD_IOCTL_TIOCSERSWILD		0x5455
#define I386_LINUX_RECORD_IOCTL_TIOCGLCKTRMIOS	0x5456
#define I386_LINUX_RECORD_IOCTL_TIOCSLCKTRMIOS	0x5457
#define I386_LINUX_RECORD_IOCTL_TIOCSERGSTRUCT	0x5458
#define I386_LINUX_RECORD_IOCTL_TIOCSERGETLSR   	0x5459
#define I386_LINUX_RECORD_IOCTL_TIOCSERGETMULTI	0x545A
#define I386_LINUX_RECORD_IOCTL_TIOCSERSETMULTI	0x545B
#define I386_LINUX_RECORD_IOCTL_TIOCMIWAIT		0x545C
#define I386_LINUX_RECORD_IOCTL_TIOCGICOUNT		0x545D
#define I386_LINUX_RECORD_IOCTL_TIOCGHAYESESP		0x545E
#define I386_LINUX_RECORD_IOCTL_TIOCSHAYESESP		0x545F
#define I386_LINUX_RECORD_IOCTL_FIOQSIZE		0x5460

/* The values of the second argument of system call "sys_fcntl"
   and "sys_fcntl64".  The values of these macros were obtained from
   Linux Kernel source.  */
#define I386_LINUX_RECORD_FCNTL_F_GETLK			5
#define I386_LINUX_RECORD_FCNTL_F_GETLK64		12
#define I386_LINUX_RECORD_FCNTL_F_SETLK64		13
#define I386_LINUX_RECORD_FCNTL_F_SETLKW64		14

static void
i386_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* GNU/Linux uses ELF.  */
  i386_elf_init_abi (info, gdbarch);

  /* Since we have the extra "orig_eax" register on GNU/Linux, we have
     to adjust a few things.  */

  set_gdbarch_write_pc (gdbarch, i386_linux_write_pc);
  set_gdbarch_num_regs (gdbarch, I386_LINUX_NUM_REGS);
  set_gdbarch_register_name (gdbarch, i386_linux_register_name);
  set_gdbarch_register_reggroup_p (gdbarch, i386_linux_register_reggroup_p);

  tdep->gregset_reg_offset = i386_linux_gregset_reg_offset;
  tdep->gregset_num_regs = ARRAY_SIZE (i386_linux_gregset_reg_offset);
  tdep->sizeof_gregset = 17 * 4;

  tdep->jb_pc_offset = 20;	/* From <bits/setjmp.h>.  */

  tdep->sigtramp_p = i386_linux_sigtramp_p;
  tdep->sigcontext_addr = i386_linux_sigcontext_addr;
  tdep->sc_reg_offset = i386_linux_sc_reg_offset;
  tdep->sc_num_regs = ARRAY_SIZE (i386_linux_sc_reg_offset);

  set_gdbarch_process_record (gdbarch, i386_process_record);

  /* Initialize the i386_linux_record_tdep.  */
  i386_linux_record_tdep.size__old_kernel_stat =
    I386_LINUX_RECORD_SIZE__old_kernel_stat;
  i386_linux_record_tdep.size_tms = I386_LINUX_RECORD_SIZE_tms;
  i386_linux_record_tdep.size_loff_t = I386_LINUX_RECORD_SIZE_loff_t;
  i386_linux_record_tdep.size_flock = I386_LINUX_RECORD_SIZE_flock;
  i386_linux_record_tdep.size_oldold_utsname =
    I386_LINUX_RECORD_SIZE_oldold_utsname;
  i386_linux_record_tdep.size_ustat = I386_LINUX_RECORD_SIZE_ustat;
  i386_linux_record_tdep.size_old_sigaction =
    I386_LINUX_RECORD_SIZE_old_sigaction;
  i386_linux_record_tdep.size_old_sigset_t =
    I386_LINUX_RECORD_SIZE_old_sigset_t;
  i386_linux_record_tdep.size_rlimit = I386_LINUX_RECORD_SIZE_rlimit;
  i386_linux_record_tdep.size_rusage = I386_LINUX_RECORD_SIZE_rusage;
  i386_linux_record_tdep.size_timeval = I386_LINUX_RECORD_SIZE_timeval;
  i386_linux_record_tdep.size_timezone = I386_LINUX_RECORD_SIZE_timezone;
  i386_linux_record_tdep.size_old_gid_t = I386_LINUX_RECORD_SIZE_old_gid_t;
  i386_linux_record_tdep.size_old_uid_t = I386_LINUX_RECORD_SIZE_old_uid_t;
  i386_linux_record_tdep.size_fd_set = I386_LINUX_RECORD_SIZE_fd_set;
  i386_linux_record_tdep.size_dirent = I386_LINUX_RECORD_SIZE_dirent;
  i386_linux_record_tdep.size_dirent64 = I386_LINUX_RECORD_SIZE_dirent64;
  i386_linux_record_tdep.size_statfs = I386_LINUX_RECORD_SIZE_statfs;
  i386_linux_record_tdep.size_statfs64 = I386_LINUX_RECORD_SIZE_statfs64;
  i386_linux_record_tdep.size_sockaddr = I386_LINUX_RECORD_SIZE_sockaddr;
  i386_linux_record_tdep.size_int = I386_LINUX_RECORD_SIZE_int;
  i386_linux_record_tdep.size_long = I386_LINUX_RECORD_SIZE_long;
  i386_linux_record_tdep.size_ulong = I386_LINUX_RECORD_SIZE_ulong;
  i386_linux_record_tdep.size_msghdr = I386_LINUX_RECORD_SIZE_msghdr;
  i386_linux_record_tdep.size_itimerval = I386_LINUX_RECORD_SIZE_itimerval;
  i386_linux_record_tdep.size_stat = I386_LINUX_RECORD_SIZE_stat;
  i386_linux_record_tdep.size_old_utsname =
    I386_LINUX_RECORD_SIZE_old_utsname;
  i386_linux_record_tdep.size_sysinfo = I386_LINUX_RECORD_SIZE_sysinfo;
  i386_linux_record_tdep.size_msqid_ds = I386_LINUX_RECORD_SIZE_msqid_ds;
  i386_linux_record_tdep.size_shmid_ds = I386_LINUX_RECORD_SIZE_shmid_ds;
  i386_linux_record_tdep.size_new_utsname =
    I386_LINUX_RECORD_SIZE_new_utsname;
  i386_linux_record_tdep.size_timex = I386_LINUX_RECORD_SIZE_timex;
  i386_linux_record_tdep.size_mem_dqinfo = I386_LINUX_RECORD_SIZE_mem_dqinfo;
  i386_linux_record_tdep.size_if_dqblk = I386_LINUX_RECORD_SIZE_if_dqblk;
  i386_linux_record_tdep.size_fs_quota_stat =
    I386_LINUX_RECORD_SIZE_fs_quota_stat;
  i386_linux_record_tdep.size_timespec = I386_LINUX_RECORD_SIZE_timespec;
  i386_linux_record_tdep.size_pollfd = I386_LINUX_RECORD_SIZE_pollfd;
  i386_linux_record_tdep.size_NFS_FHSIZE = I386_LINUX_RECORD_SIZE_NFS_FHSIZE;
  i386_linux_record_tdep.size_knfsd_fh = I386_LINUX_RECORD_SIZE_knfsd_fh;
  i386_linux_record_tdep.size_TASK_COMM_LEN =
    I386_LINUX_RECORD_SIZE_TASK_COMM_LEN;
  i386_linux_record_tdep.size_sigaction = I386_LINUX_RECORD_SIZE_sigaction;
  i386_linux_record_tdep.size_sigset_t = I386_LINUX_RECORD_SIZE_sigset_t;
  i386_linux_record_tdep.size_siginfo_t = I386_LINUX_RECORD_SIZE_siginfo_t;
  i386_linux_record_tdep.size_cap_user_data_t =
    I386_LINUX_RECORD_SIZE_cap_user_data_t;
  i386_linux_record_tdep.size_stack_t = I386_LINUX_RECORD_SIZE_stack_t;
  i386_linux_record_tdep.size_off_t = I386_LINUX_RECORD_SIZE_off_t;
  i386_linux_record_tdep.size_stat64 = I386_LINUX_RECORD_SIZE_stat64;
  i386_linux_record_tdep.size_gid_t = I386_LINUX_RECORD_SIZE_gid_t;
  i386_linux_record_tdep.size_uid_t = I386_LINUX_RECORD_SIZE_uid_t;
  i386_linux_record_tdep.size_PAGE_SIZE = I386_LINUX_RECORD_SIZE_PAGE_SIZE;
  i386_linux_record_tdep.size_flock64 = I386_LINUX_RECORD_SIZE_flock64;
  i386_linux_record_tdep.size_user_desc = I386_LINUX_RECORD_SIZE_user_desc;
  i386_linux_record_tdep.size_io_event = I386_LINUX_RECORD_SIZE_io_event;
  i386_linux_record_tdep.size_iocb = I386_LINUX_RECORD_SIZE_iocb;
  i386_linux_record_tdep.size_epoll_event =
    I386_LINUX_RECORD_SIZE_epoll_event;
  i386_linux_record_tdep.size_itimerspec = I386_LINUX_RECORD_SIZE_itimerspec;
  i386_linux_record_tdep.size_mq_attr = I386_LINUX_RECORD_SIZE_mq_attr;
  i386_linux_record_tdep.size_siginfo = I386_LINUX_RECORD_SIZE_siginfo;
  i386_linux_record_tdep.size_termios = I386_LINUX_RECORD_SIZE_termios;
  i386_linux_record_tdep.size_termios2 = I386_LINUX_RECORD_SIZE_termios2;
  i386_linux_record_tdep.size_pid_t = I386_LINUX_RECORD_SIZE_pid_t;
  i386_linux_record_tdep.size_winsize = I386_LINUX_RECORD_SIZE_winsize;
  i386_linux_record_tdep.size_char = I386_LINUX_RECORD_SIZE_char;
  i386_linux_record_tdep.size_serial_struct =
    I386_LINUX_RECORD_SIZE_serial_struct;
  i386_linux_record_tdep.size_serial_icounter_struct =
    I386_LINUX_RECORD_SIZE_serial_icounter_struct;
  i386_linux_record_tdep.size_hayes_esp_config =
    I386_LINUX_RECORD_SIZE_hayes_esp_config;

  i386_linux_record_tdep.ioctl_TCGETS = I386_LINUX_RECORD_IOCTL_TCGETS;
  i386_linux_record_tdep.ioctl_TCSETS = I386_LINUX_RECORD_IOCTL_TCSETS;
  i386_linux_record_tdep.ioctl_TCSETSW = I386_LINUX_RECORD_IOCTL_TCSETSW;
  i386_linux_record_tdep.ioctl_TCSETSF = I386_LINUX_RECORD_IOCTL_TCSETSF;
  i386_linux_record_tdep.ioctl_TCGETA = I386_LINUX_RECORD_IOCTL_TCGETA;
  i386_linux_record_tdep.ioctl_TCSETA = I386_LINUX_RECORD_IOCTL_TCSETA;
  i386_linux_record_tdep.ioctl_TCSETAW = I386_LINUX_RECORD_IOCTL_TCSETAW;
  i386_linux_record_tdep.ioctl_TCSETAF = I386_LINUX_RECORD_IOCTL_TCSETAF;
  i386_linux_record_tdep.ioctl_TCSBRK = I386_LINUX_RECORD_IOCTL_TCSBRK;
  i386_linux_record_tdep.ioctl_TCXONC = I386_LINUX_RECORD_IOCTL_TCXONC;
  i386_linux_record_tdep.ioctl_TCFLSH = I386_LINUX_RECORD_IOCTL_TCFLSH;
  i386_linux_record_tdep.ioctl_TIOCEXCL = I386_LINUX_RECORD_IOCTL_TIOCEXCL;
  i386_linux_record_tdep.ioctl_TIOCNXCL = I386_LINUX_RECORD_IOCTL_TIOCNXCL;
  i386_linux_record_tdep.ioctl_TIOCSCTTY = I386_LINUX_RECORD_IOCTL_TIOCSCTTY;
  i386_linux_record_tdep.ioctl_TIOCGPGRP = I386_LINUX_RECORD_IOCTL_TIOCGPGRP;
  i386_linux_record_tdep.ioctl_TIOCSPGRP = I386_LINUX_RECORD_IOCTL_TIOCSPGRP;
  i386_linux_record_tdep.ioctl_TIOCOUTQ = I386_LINUX_RECORD_IOCTL_TIOCOUTQ;
  i386_linux_record_tdep.ioctl_TIOCSTI = I386_LINUX_RECORD_IOCTL_TIOCSTI;
  i386_linux_record_tdep.ioctl_TIOCGWINSZ =
    I386_LINUX_RECORD_IOCTL_TIOCGWINSZ;
  i386_linux_record_tdep.ioctl_TIOCSWINSZ =
    I386_LINUX_RECORD_IOCTL_TIOCSWINSZ;
  i386_linux_record_tdep.ioctl_TIOCMGET = I386_LINUX_RECORD_IOCTL_TIOCMGET;
  i386_linux_record_tdep.ioctl_TIOCMBIS = I386_LINUX_RECORD_IOCTL_TIOCMBIS;
  i386_linux_record_tdep.ioctl_TIOCMBIC = I386_LINUX_RECORD_IOCTL_TIOCMBIC;
  i386_linux_record_tdep.ioctl_TIOCMSET = I386_LINUX_RECORD_IOCTL_TIOCMSET;
  i386_linux_record_tdep.ioctl_TIOCGSOFTCAR =
    I386_LINUX_RECORD_IOCTL_TIOCGSOFTCAR;
  i386_linux_record_tdep.ioctl_TIOCSSOFTCAR =
    I386_LINUX_RECORD_IOCTL_TIOCSSOFTCAR;
  i386_linux_record_tdep.ioctl_FIONREAD = I386_LINUX_RECORD_IOCTL_FIONREAD;
  i386_linux_record_tdep.ioctl_TIOCINQ = I386_LINUX_RECORD_IOCTL_TIOCINQ;
  i386_linux_record_tdep.ioctl_TIOCLINUX = I386_LINUX_RECORD_IOCTL_TIOCLINUX;
  i386_linux_record_tdep.ioctl_TIOCCONS = I386_LINUX_RECORD_IOCTL_TIOCCONS;
  i386_linux_record_tdep.ioctl_TIOCGSERIAL =
    I386_LINUX_RECORD_IOCTL_TIOCGSERIAL;
  i386_linux_record_tdep.ioctl_TIOCSSERIAL =
    I386_LINUX_RECORD_IOCTL_TIOCSSERIAL;
  i386_linux_record_tdep.ioctl_TIOCPKT = I386_LINUX_RECORD_IOCTL_TIOCPKT;
  i386_linux_record_tdep.ioctl_FIONBIO = I386_LINUX_RECORD_IOCTL_FIONBIO;
  i386_linux_record_tdep.ioctl_TIOCNOTTY = I386_LINUX_RECORD_IOCTL_TIOCNOTTY;
  i386_linux_record_tdep.ioctl_TIOCSETD = I386_LINUX_RECORD_IOCTL_TIOCSETD;
  i386_linux_record_tdep.ioctl_TIOCGETD = I386_LINUX_RECORD_IOCTL_TIOCGETD;
  i386_linux_record_tdep.ioctl_TCSBRKP = I386_LINUX_RECORD_IOCTL_TCSBRKP;
  i386_linux_record_tdep.ioctl_TIOCTTYGSTRUCT =
    I386_LINUX_RECORD_IOCTL_TIOCTTYGSTRUCT;
  i386_linux_record_tdep.ioctl_TIOCSBRK = I386_LINUX_RECORD_IOCTL_TIOCSBRK;
  i386_linux_record_tdep.ioctl_TIOCCBRK = I386_LINUX_RECORD_IOCTL_TIOCCBRK;
  i386_linux_record_tdep.ioctl_TIOCGSID = I386_LINUX_RECORD_IOCTL_TIOCGSID;
  i386_linux_record_tdep.ioctl_TCGETS2 = I386_LINUX_RECORD_IOCTL_TCGETS2;
  i386_linux_record_tdep.ioctl_TCSETS2 = I386_LINUX_RECORD_IOCTL_TCSETS2;
  i386_linux_record_tdep.ioctl_TCSETSW2 = I386_LINUX_RECORD_IOCTL_TCSETSW2;
  i386_linux_record_tdep.ioctl_TCSETSF2 = I386_LINUX_RECORD_IOCTL_TCSETSF2;
  i386_linux_record_tdep.ioctl_TIOCGPTN = I386_LINUX_RECORD_IOCTL_TIOCGPTN;
  i386_linux_record_tdep.ioctl_TIOCSPTLCK =
    I386_LINUX_RECORD_IOCTL_TIOCSPTLCK;
  i386_linux_record_tdep.ioctl_FIONCLEX = I386_LINUX_RECORD_IOCTL_FIONCLEX;
  i386_linux_record_tdep.ioctl_FIOCLEX = I386_LINUX_RECORD_IOCTL_FIOCLEX;
  i386_linux_record_tdep.ioctl_FIOASYNC = I386_LINUX_RECORD_IOCTL_FIOASYNC;
  i386_linux_record_tdep.ioctl_TIOCSERCONFIG =
    I386_LINUX_RECORD_IOCTL_TIOCSERCONFIG;
  i386_linux_record_tdep.ioctl_TIOCSERGWILD =
    I386_LINUX_RECORD_IOCTL_TIOCSERGWILD;
  i386_linux_record_tdep.ioctl_TIOCSERSWILD =
    I386_LINUX_RECORD_IOCTL_TIOCSERSWILD;
  i386_linux_record_tdep.ioctl_TIOCGLCKTRMIOS =
    I386_LINUX_RECORD_IOCTL_TIOCGLCKTRMIOS;
  i386_linux_record_tdep.ioctl_TIOCSLCKTRMIOS =
    I386_LINUX_RECORD_IOCTL_TIOCSLCKTRMIOS;
  i386_linux_record_tdep.ioctl_TIOCSERGSTRUCT =
    I386_LINUX_RECORD_IOCTL_TIOCSERGSTRUCT;
  i386_linux_record_tdep.ioctl_TIOCSERGETLSR =
    I386_LINUX_RECORD_IOCTL_TIOCSERGETLSR;
  i386_linux_record_tdep.ioctl_TIOCSERGETMULTI =
    I386_LINUX_RECORD_IOCTL_TIOCSERGETMULTI;
  i386_linux_record_tdep.ioctl_TIOCSERSETMULTI =
    I386_LINUX_RECORD_IOCTL_TIOCSERSETMULTI;
  i386_linux_record_tdep.ioctl_TIOCMIWAIT =
    I386_LINUX_RECORD_IOCTL_TIOCMIWAIT;
  i386_linux_record_tdep.ioctl_TIOCGICOUNT =
    I386_LINUX_RECORD_IOCTL_TIOCGICOUNT;
  i386_linux_record_tdep.ioctl_TIOCGHAYESESP =
    I386_LINUX_RECORD_IOCTL_TIOCGHAYESESP;
  i386_linux_record_tdep.ioctl_TIOCSHAYESESP =
    I386_LINUX_RECORD_IOCTL_TIOCSHAYESESP;
  i386_linux_record_tdep.ioctl_FIOQSIZE = I386_LINUX_RECORD_IOCTL_FIOQSIZE;

  i386_linux_record_tdep.fcntl_F_GETLK = I386_LINUX_RECORD_FCNTL_F_GETLK;
  i386_linux_record_tdep.fcntl_F_GETLK64 = I386_LINUX_RECORD_FCNTL_F_GETLK64;
  i386_linux_record_tdep.fcntl_F_SETLK64 = I386_LINUX_RECORD_FCNTL_F_SETLK64;
  i386_linux_record_tdep.fcntl_F_SETLKW64 =
    I386_LINUX_RECORD_FCNTL_F_SETLKW64;

  i386_linux_record_tdep.arg1 = I386_EBX_REGNUM;
  i386_linux_record_tdep.arg2 = I386_ECX_REGNUM;
  i386_linux_record_tdep.arg3 = I386_EDX_REGNUM;
  i386_linux_record_tdep.arg4 = I386_ESI_REGNUM;
  i386_linux_record_tdep.arg5 = I386_EDI_REGNUM;

  tdep->i386_intx80_record = i386_linux_intx80_sysenter_record;
  tdep->i386_sysenter_record = i386_linux_intx80_sysenter_record;

  /* N_FUN symbols in shared libaries have 0 for their values and need
     to be relocated. */
  set_gdbarch_sofun_address_maybe_missing (gdbarch, 1);

  /* GNU/Linux uses SVR4-style shared libraries.  */
  set_gdbarch_skip_trampoline_code (gdbarch, find_solib_trampoline_target);
  set_solib_svr4_fetch_link_map_offsets
    (gdbarch, svr4_ilp32_fetch_link_map_offsets);

  /* GNU/Linux uses the dynamic linker included in the GNU C Library.  */
  set_gdbarch_skip_solib_resolver (gdbarch, glibc_skip_solib_resolver);

  dwarf2_frame_set_signal_frame_p (gdbarch, i386_linux_dwarf_signal_frame_p);

  /* Enable TLS support.  */
  set_gdbarch_fetch_tls_load_module_address (gdbarch,
                                             svr4_fetch_objfile_link_map);

  /* Install supported register note sections.  */
  set_gdbarch_core_regset_sections (gdbarch, i386_linux_regset_sections);

  /* Displaced stepping.  */
  set_gdbarch_displaced_step_copy_insn (gdbarch,
                                        simple_displaced_step_copy_insn);
  set_gdbarch_displaced_step_fixup (gdbarch, i386_displaced_step_fixup);
  set_gdbarch_displaced_step_free_closure (gdbarch,
                                           simple_displaced_step_free_closure);
  set_gdbarch_displaced_step_location (gdbarch,
                                       displaced_step_at_entry_point);

  set_gdbarch_get_siginfo_type (gdbarch, linux_get_siginfo_type);
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern void _initialize_i386_linux_tdep (void);

void
_initialize_i386_linux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_i386, 0, GDB_OSABI_LINUX,
			  i386_linux_init_abi);
}
