/* Process record and replay target code for GNU/Linux.

   Copyright (C) 2008, 2009 Free Software Foundation, Inc.

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
#include "target.h"
#include "regcache.h"
#include "record.h"
#include "linux-record.h"

/* These macros are the values of the first argument of system call
   "sys_ptrace".  The values of these macros were obtained from Linux
   Kernel source.  */

#define RECORD_PTRACE_PEEKTEXT	1
#define RECORD_PTRACE_PEEKDATA	2
#define RECORD_PTRACE_PEEKUSR	3

/* These macros are the values of the first argument of system call
   "sys_socketcall".  The values of these macros were obtained from
   Linux Kernel source.  */

#define RECORD_SYS_SOCKET	1
#define RECORD_SYS_BIND		2
#define RECORD_SYS_CONNECT	3
#define RECORD_SYS_LISTEN	4
#define RECORD_SYS_ACCEPT	5
#define RECORD_SYS_GETSOCKNAME	6
#define RECORD_SYS_GETPEERNAME	7
#define RECORD_SYS_SOCKETPAIR	8
#define RECORD_SYS_SEND		9
#define RECORD_SYS_RECV		10
#define RECORD_SYS_SENDTO	11
#define RECORD_SYS_RECVFROM	12
#define RECORD_SYS_SHUTDOWN	13
#define RECORD_SYS_SETSOCKOPT	14
#define RECORD_SYS_GETSOCKOPT	15
#define RECORD_SYS_SENDMSG	16
#define RECORD_SYS_RECVMSG	17

/* These macros are the values of the first argument of system call
   "sys_ipc".  The values of these macros were obtained from Linux
   Kernel source.  */

#define RECORD_SEMOP		1
#define RECORD_SEMGET		2
#define RECORD_SEMCTL		3
#define RECORD_SEMTIMEDOP	4
#define RECORD_MSGSND		11
#define RECORD_MSGRCV		12
#define RECORD_MSGGET		13
#define RECORD_MSGCTL		14
#define RECORD_SHMAT		21
#define RECORD_SHMDT		22
#define RECORD_SHMGET		23
#define RECORD_SHMCTL		24

/* These macros are the values of the first argument of system call
   "sys_quotactl".  The values of these macros were obtained from Linux
   Kernel source.  */

#define RECORD_Q_GETFMT		0x800004
#define RECORD_Q_GETINFO	0x800005
#define RECORD_Q_GETQUOTA	0x800007
#define RECORD_Q_XGETQSTAT	(('5' << 8) + 5)
#define RECORD_Q_XGETQUOTA	(('3' << 8) + 3)

/* When the architecture process record get a Linux syscall
   instruction, it will get a Linux syscall number of this
   architecture and convert it to the Linux syscall number "num" which
   is internal to GDB.  Most Linux syscalls across architectures in
   Linux would be similar and mostly differ by sizes of types and
   structures.  This sizes are put to "tdep".

   Record the values of the registers and memory that will be changed
   in current system call.

   Return -1 if something wrong.  */

int
record_linux_system_call (int num, struct regcache *regcache,
			  struct linux_record_tdep *tdep)
{
  uint32_t tmpu32;

  switch (num)
    {
      /* sys_restart_syscall */
    case 0:
      break;

      /* sys_exit */
    case 1:
      {
	int q;
	target_terminal_ours ();
	q =
	  yquery (_ ("The next instruction is syscall exit.  "
		     "It will make the program exit.  "
		     "Do you want to stop the program?"));
	target_terminal_inferior ();
	if (q)
	  return 1;
      }
      break;

      /* sys_fork */
    case 2:
      break;

      /* sys_read */
    case 3:
      {
	uint32_t addr, count;
	regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & addr);
	regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & count);
	if (record_arch_list_add_mem (addr, count))
	  return -1;
      }
      break;

      /* sys_write */
    case 4:
      /* sys_open */
    case 5:
      /* sys_close */
    case 6:
      /* sys_waitpid */
    case 7:
      /* sys_creat */
    case 8:
      /* sys_link */
    case 9:
      /* sys_unlink */
    case 10:
      /* sys_execve */
    case 11:
      /* sys_chdir */
    case 12:
      /* sys_time */
    case 13:
      /* sys_mknod */
    case 14:
      /* sys_chmod */
    case 15:
      /* sys_lchown16 */
    case 16:
      /* sys_ni_syscall */
    case 17:
      break;

      /* sys_stat */
    case 18:
      /* sys_fstat */
    case 28:
      /* sys_lstat */
    case 84:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size__old_kernel_stat))
	return -1;
      break;

      /* sys_lseek */
    case 19:
      /* sys_getpid */
    case 20:
      /* sys_mount */
    case 21:
      /* sys_oldumount */
    case 22:
      /* sys_setuid16 */
    case 23:
      /* sys_getuid16 */
    case 24:
      /* sys_stime */
    case 25:
      break;

      /* sys_ptrace */
    case 26:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (tmpu32 == RECORD_PTRACE_PEEKTEXT
	  || tmpu32 == RECORD_PTRACE_PEEKDATA
	  || tmpu32 == RECORD_PTRACE_PEEKUSR)
	{
	  regcache_raw_read (regcache, tdep->arg4,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, 4))
	    return -1;
	}
      break;

      /* sys_alarm */
    case 27:
      /* sys_pause */
    case 29:
      /* sys_utime    */
    case 30:
      /* sys_ni_syscall */
    case 31:
      /* sys_ni_syscall */
    case 32:
      /* sys_access */
    case 33:
      /* sys_nice */
    case 34:
      /* sys_ni_syscall */
    case 35:
      /* sys_sync */
    case 36:
      /* sys_kill */
    case 37:
      /* sys_rename */
    case 38:
      /* sys_mkdir */
    case 39:
      /* sys_rmdir */
    case 40:
      /* sys_dup */
    case 41:
      /* sys_pipe */
    case 42:
      break;

      /* sys_times */
    case 43:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_tms))
	return -1;
      break;

      /* sys_ni_syscall */
    case 44:
      /* sys_brk */
    case 45:
      /* sys_setgid16 */
    case 46:
      /* sys_getgid16 */
    case 47:
      /* sys_signal */
    case 48:
      /* sys_geteuid16 */
    case 49:
      /* sys_getegid16 */
    case 50:
      /* sys_acct */
    case 51:
      /* sys_umount */
    case 52:
      /* sys_ni_syscall */
    case 53:
      break;

      /* sys_ioctl */
    case 54:
      /* XXX Need to add a lot of support of other ioctl requests.  */
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (tmpu32 == tdep->ioctl_FIOCLEX || tmpu32 == tdep->ioctl_FIONCLEX
	  || tmpu32 == tdep->ioctl_FIONBIO || tmpu32 == tdep->ioctl_FIOASYNC
	  || tmpu32 == tdep->ioctl_TCSETS || tmpu32 == tdep->ioctl_TCSETSW
	  || tmpu32 == tdep->ioctl_TCSETSF || tmpu32 == tdep->ioctl_TCSETA
	  || tmpu32 == tdep->ioctl_TCSETAW || tmpu32 == tdep->ioctl_TCSETAF
	  || tmpu32 == tdep->ioctl_TCSBRK || tmpu32 == tdep->ioctl_TCXONC
	  || tmpu32 == tdep->ioctl_TCFLSH || tmpu32 == tdep->ioctl_TIOCEXCL
	  || tmpu32 == tdep->ioctl_TIOCNXCL
	  || tmpu32 == tdep->ioctl_TIOCSCTTY
	  || tmpu32 == tdep->ioctl_TIOCSPGRP || tmpu32 == tdep->ioctl_TIOCSTI
	  || tmpu32 == tdep->ioctl_TIOCSWINSZ
	  || tmpu32 == tdep->ioctl_TIOCMBIS || tmpu32 == tdep->ioctl_TIOCMBIC
	  || tmpu32 == tdep->ioctl_TIOCMSET
	  || tmpu32 == tdep->ioctl_TIOCSSOFTCAR
	  || tmpu32 == tdep->ioctl_TIOCCONS
	  || tmpu32 == tdep->ioctl_TIOCSSERIAL
	  || tmpu32 == tdep->ioctl_TIOCPKT || tmpu32 == tdep->ioctl_TIOCNOTTY
	  || tmpu32 == tdep->ioctl_TIOCSETD || tmpu32 == tdep->ioctl_TCSBRKP
	  || tmpu32 == tdep->ioctl_TIOCTTYGSTRUCT
	  || tmpu32 == tdep->ioctl_TIOCSBRK || tmpu32 == tdep->ioctl_TIOCCBRK
	  || tmpu32 == tdep->ioctl_TCSETS2 || tmpu32 == tdep->ioctl_TCSETSW2
	  || tmpu32 == tdep->ioctl_TCSETSF2
	  || tmpu32 == tdep->ioctl_TIOCSPTLCK
	  || tmpu32 == tdep->ioctl_TIOCSERCONFIG
	  || tmpu32 == tdep->ioctl_TIOCSERGWILD
	  || tmpu32 == tdep->ioctl_TIOCSERSWILD
	  || tmpu32 == tdep->ioctl_TIOCSLCKTRMIOS
	  || tmpu32 == tdep->ioctl_TIOCSERGETMULTI
	  || tmpu32 == tdep->ioctl_TIOCSERSETMULTI
	  || tmpu32 == tdep->ioctl_TIOCMIWAIT
	  || tmpu32 == tdep->ioctl_TIOCSHAYESESP)
	{
	  /* Nothing to do.  */
	}
      else if (tmpu32 == tdep->ioctl_TCGETS || tmpu32 == tdep->ioctl_TCGETA
	       || tmpu32 == tdep->ioctl_TIOCGLCKTRMIOS)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_termios))
	    return -1;
	}
      else if (tmpu32 == tdep->ioctl_TIOCGPGRP
	       || tmpu32 == tdep->ioctl_TIOCGSID)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_pid_t))
	    return -1;
	}
      else if (tmpu32 == tdep->ioctl_TIOCOUTQ
	       || tmpu32 == tdep->ioctl_TIOCMGET
	       || tmpu32 == tdep->ioctl_TIOCGSOFTCAR
	       || tmpu32 == tdep->ioctl_FIONREAD
	       || tmpu32 == tdep->ioctl_TIOCINQ
	       || tmpu32 == tdep->ioctl_TIOCGETD
	       || tmpu32 == tdep->ioctl_TIOCGPTN
	       || tmpu32 == tdep->ioctl_TIOCSERGETLSR)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	    return -1;
	}
      else if (tmpu32 == tdep->ioctl_TIOCGWINSZ)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_winsize))
	    return -1;
	}
      else if (tmpu32 == tdep->ioctl_TIOCLINUX)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_char))
	    return -1;
	}
      else if (tmpu32 == tdep->ioctl_TIOCGSERIAL)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_serial_struct))
	    return -1;
	}
      else if (tmpu32 == tdep->ioctl_TCGETS2)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_termios2))
	    return -1;
	}
      else if (tmpu32 == tdep->ioctl_FIOQSIZE)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_loff_t))
	    return -1;
	}
      else if (tmpu32 == tdep->ioctl_TIOCGICOUNT)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem
	      (tmpu32, tdep->size_serial_icounter_struct))
	    return -1;
	}
      else if (tmpu32 == tdep->ioctl_TIOCGHAYESESP)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_hayes_esp_config))
	    return -1;
	}
      else if (tmpu32 == tdep->ioctl_TIOCSERGSTRUCT)
	{
	  printf_unfiltered (_("Process record and replay target doesn't "
			       "support ioctl request TIOCSERGSTRUCT\n"));
	  return 1;
	}
      else
	{
	  printf_unfiltered (_("Process record and replay target doesn't "
			       "support ioctl request 0x%08x.\n"),
			     tmpu32);
	  return 1;
	}
      break;

      /* sys_fcntl */
    case 55:
      /* XXX */
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
    sys_fcntl:
      if (tmpu32 == F_GETLK)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_flock))
	    return -1;
	}
      break;

      /* sys_ni_syscall */
    case 56:
      /* sys_setpgid */
    case 57:
      /* sys_ni_syscall */
    case 58:
      break;

      /* sys_olduname */
    case 59:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_oldold_utsname))
	return -1;
      break;

      /* sys_umask */
    case 60:
      /* sys_chroot */
    case 61:
      break;

      /* sys_ustat */
    case 62:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_ustat))
	return -1;
      break;

      /* sys_dup2 */
    case 63:
      /* sys_getppid */
    case 64:
      /* sys_getpgrp */
    case 65:
      /* sys_setsid */
    case 66:
      break;

      /* sys_sigaction */
    case 67:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_sigaction))
	return -1;
      break;

      /* sys_sgetmask */
    case 68:
      /* sys_ssetmask */
    case 69:
      /* sys_setreuid16 */
    case 70:
      /* sys_setregid16 */
    case 71:
      /* sys_sigsuspend */
    case 72:
      break;

      /* sys_sigpending */
    case 73:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_sigset_t))
	return -1;
      break;

      /* sys_sethostname */
    case 74:
      /* sys_setrlimit */
    case 75:
      break;

      /* sys_old_getrlimit */
    case 76:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_rlimit))
	return -1;
      break;

      /* sys_getrusage */
    case 77:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_rusage))
	return -1;
      break;

      /* sys_gettimeofday */
    case 78:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_timeval))
	return -1;
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_timezone))
	return -1;
      break;

      /* sys_settimeofday */
    case 79:
      break;

      /* sys_getgroups16 */
    case 80:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_gid_t))
	return -1;
      break;

      /* sys_setgroups16 */
    case 81:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_gid_t))
	return -1;
      break;

      /* old_select */
    case 82:
      {
	struct sel_arg_struct
	{
	  CORE_ADDR n;
	  CORE_ADDR inp;
	  CORE_ADDR outp;
	  CORE_ADDR exp;
	  CORE_ADDR tvp;
	} sel;

	regcache_raw_read (regcache, tdep->arg1,
			   (gdb_byte *) & tmpu32);
	if (tmpu32)
	  {
	    if (target_read_memory (tmpu32, (gdb_byte *) & sel, sizeof (sel)))
	      {
		if (record_debug)
		  fprintf_unfiltered (gdb_stdlog,
				      "Process record: error reading memory "
				      "at addr = 0x%s len = %d.\n",
				      paddr_nz (tmpu32),
				      (int) sizeof (sel));
		return -1;
	      }
	    if (record_arch_list_add_mem (sel.inp, tdep->size_fd_set))
	      return -1;
	    if (record_arch_list_add_mem (sel.outp, tdep->size_fd_set))
	      return -1;
	    if (record_arch_list_add_mem (sel.exp, tdep->size_fd_set))
	      return -1;
	    if (record_arch_list_add_mem (sel.tvp, tdep->size_timeval))
	      return -1;
	  }
      }
      break;

      /* sys_symlink */
    case 83:
      break;

      /* sys_readlink */
    case 85:
      {
	uint32_t len;
	regcache_raw_read (regcache, tdep->arg2,
			   (gdb_byte *) & tmpu32);
	regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & len);
	if (record_arch_list_add_mem (tmpu32, len))
	  return -1;
      }
      break;

      /* sys_uselib */
    case 86:
      /* sys_swapon */
    case 87:
      break;

      /* sys_reboot */
    case 88:
      {
	int q;
	target_terminal_ours ();
	q =
	  yquery (_("The next instruction is syscall reboot.  "
		    "It will restart the computer.  "
		    "Do you want to stop the program?"));
	target_terminal_inferior ();
	if (q)
	  return 1;
      }
      break;

      /* old_readdir */
    case 89:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_dirent))
	return -1;
      break;

      /* old_mmap */
    case 90:
      break;

      /* sys_munmap */
    case 91:
      {
	int q;
	uint32_t len;

	regcache_raw_read (regcache, tdep->arg1,
			   (gdb_byte *) & tmpu32);
	regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & len);
	target_terminal_ours ();
	q =
	  yquery (_("The next instruction is syscall munmap.  "
		    "It will free the memory addr = 0x%s len = %d.  "
		    "It will make record target get error.  "
		    "Do you want to stop the program?"),
		  paddr_nz (tmpu32), len);
	target_terminal_inferior ();
	if (q)
	  return 1;
      }
      break;

      /* sys_truncate */
    case 92:
      /* sys_ftruncate */
    case 93:
      /* sys_fchmod */
    case 94:
      /* sys_fchown16 */
    case 95:
      /* sys_getpriority */
    case 96:
      /* sys_setpriority */
    case 97:
      /* sys_ni_syscall */
    case 98:
      break;

      /* sys_statfs */
    case 99:
      /* sys_fstatfs */
    case 100:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_statfs))
	return -1;
      break;

      /* sys_ioperm */
    case 101:
      break;

      /* sys_socketcall */
    case 102:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      switch (tmpu32)
	{
	case RECORD_SYS_SOCKET:
	case RECORD_SYS_BIND:
	case RECORD_SYS_CONNECT:
	case RECORD_SYS_LISTEN:
	  break;
	case RECORD_SYS_ACCEPT:
	case RECORD_SYS_GETSOCKNAME:
	case RECORD_SYS_GETPEERNAME:
	  {
	    uint32_t a[3];
	    regcache_raw_read (regcache, tdep->arg2,
			       (gdb_byte *) & tmpu32);
	    if (tmpu32)
	      {
		if (target_read_memory (tmpu32, (gdb_byte *) a, sizeof (a)))
		  {
		    if (record_debug)
		      fprintf_unfiltered (gdb_stdlog,
					  "Process record: error reading "
					  "memory at addr = 0x%s len = %d.\n",
					  paddr_nz (tmpu32),
					  (int) sizeof (a));
		    return -1;
		  }
		if (record_arch_list_add_mem (a[1], tdep->size_sockaddr))
		  return -1;
		if (record_arch_list_add_mem (a[2], tdep->size_int))
		  return -1;
	      }
	  }
	  break;

	case RECORD_SYS_SOCKETPAIR:
	  {
	    uint32_t a[4];
	    regcache_raw_read (regcache, tdep->arg2,
			       (gdb_byte *) & tmpu32);
	    if (tmpu32)
	      {
		if (target_read_memory (tmpu32, (gdb_byte *) a, sizeof (a)))
		  {
		    if (record_debug)
		      fprintf_unfiltered (gdb_stdlog,
					  "Process record: error reading "
					  "memory at addr = 0x%s len = %d.\n",
					  paddr_nz (tmpu32),
				          (int) sizeof (a));
		    return -1;
		  }
		if (record_arch_list_add_mem (a[3], tdep->size_int))
		  return -1;
	      }
	  }
	  break;
	case RECORD_SYS_SEND:
	case RECORD_SYS_SENDTO:
	  break;
	case RECORD_SYS_RECV:
	  {
	    uint32_t a[3];
	    regcache_raw_read (regcache, tdep->arg2,
			       (gdb_byte *) & tmpu32);
	    if (tmpu32)
	      {
		if (target_read_memory (tmpu32, (gdb_byte *) a, sizeof (a)))
		  {
		    if (record_debug)
		      fprintf_unfiltered (gdb_stdlog,
					  "Process record: error reading "
					  "memory at addr = 0x%s len = %d.\n",
					  paddr_nz (tmpu32),
					  (int) sizeof (a));
		    return -1;
		  }
		if (a[2])
		  {
		    if (target_read_memory
			(a[2], (gdb_byte *) & (a[2]), sizeof (a[2])))
		      {
			if (record_debug)
			  fprintf_unfiltered (gdb_stdlog,
					      "Process record: error reading "
					      "memory at addr = 0x%s "
					      "len = %d.\n",
					      paddr_nz (a[2]),
					      (int) sizeof (a[2]));
			return -1;
		      }
		    if (record_arch_list_add_mem (a[1], a[2]))
		      return -1;
		  }
	      }
	  }
	  break;
	case RECORD_SYS_RECVFROM:
	  {
	    uint32_t a[6];
	    regcache_raw_read (regcache, tdep->arg2,
			       (gdb_byte *) & tmpu32);
	    if (tmpu32)
	      {
		if (target_read_memory (tmpu32, (gdb_byte *) a, sizeof (a)))
		  {
		    if (record_debug)
		      fprintf_unfiltered (gdb_stdlog,
					  "Process record: error reading "
					  "memory at addr = 0x%s len = %d.\n",
					  paddr_nz (tmpu32),
					  (int) sizeof (a));
		    return -1;
		  }
		if (a[2])
		  {
		    if (target_read_memory
			(a[2], (gdb_byte *) & (a[2]), sizeof (a[2])))
		      {
			if (record_debug)
			  fprintf_unfiltered (gdb_stdlog,
					      "Process record: error reading "
					      "memory at addr = 0x%s "
					      "len = %d.\n",
					      paddr_nz (a[2]),
					      (int) sizeof (a[2]));
			return -1;
		      }
		    if (record_arch_list_add_mem (a[1], a[2]))
		      return -1;
		    if (record_arch_list_add_mem (a[4], tdep->size_sockaddr))
		      return -1;
		    if (record_arch_list_add_mem (a[5], tdep->size_int))
		      return -1;
		  }
	      }
	  }
	  break;
	case RECORD_SYS_SHUTDOWN:
	case RECORD_SYS_SETSOCKOPT:
	  break;
	case RECORD_SYS_GETSOCKOPT:
	  {
	    uint32_t a[5];
	    uint32_t av;

	    regcache_raw_read (regcache, tdep->arg2,
			       (gdb_byte *) & tmpu32);
	    if (tmpu32)
	      {
		if (target_read_memory (tmpu32, (gdb_byte *) a, sizeof (a)))
		  {
		    if (record_debug)
		      fprintf_unfiltered (gdb_stdlog,
					  "Process record: error reading "
					  "memory at addr = 0x%s len = %d.\n",
					  paddr_nz (tmpu32),
					  (int) sizeof (a));
		    return -1;
		  }
		if (a[4])
		  {
		    if (target_read_memory
			(a[4], (gdb_byte *) & av, sizeof (av)))
		      {
			if (record_debug)
			  fprintf_unfiltered (gdb_stdlog,
					      "Process record: error reading "
					      "memory at addr = 0x%s "
					      "len = %d.\n",
					      paddr_nz (a[4]),
					      (int) sizeof (av));
			return -1;
		      }
		    if (record_arch_list_add_mem (a[3], av))
		      return -1;
		    if (record_arch_list_add_mem (a[4], tdep->size_int))
		      return -1;
		  }
	      }
	  }
	  break;
	case RECORD_SYS_SENDMSG:
	  break;
	case RECORD_SYS_RECVMSG:
	  {
	    uint32_t a[2], i;
	    struct record_msghdr
	    {
	      uint32_t msg_name;
	      uint32_t msg_namelen;
	      uint32_t msg_iov;
	      uint32_t msg_iovlen;
	      uint32_t msg_control;
	      uint32_t msg_controllen;
	      uint32_t msg_flags;
	    } rec;
	    struct record_iovec
	    {
	      uint32_t iov_base;
	      uint32_t iov_len;
	    } iov;

	    regcache_raw_read (regcache, tdep->arg2,
			       (gdb_byte *) & tmpu32);
	    if (tmpu32)
	      {
		if (target_read_memory (tmpu32, (gdb_byte *) a, sizeof (a)))
		  {
		    if (record_debug)
		      fprintf_unfiltered (gdb_stdlog,
					  "Process record: error reading "
					  "memory at addr = 0x%s len = %d.\n",
					  paddr_nz (tmpu32),
					  (int) sizeof (a));
		    return -1;
		  }
		if (record_arch_list_add_mem (a[1], tdep->size_msghdr))
		  return -1;
		if (a[1])
		  {
		    if (target_read_memory
			(a[1], (gdb_byte *) & rec, sizeof (rec)))
		      {
			if (record_debug)
			  fprintf_unfiltered (gdb_stdlog,
					      "Process record: error reading "
					      "memory at addr = 0x%s "
					      "len = %d.\n",
					      paddr_nz (a[1]),
					      (int) sizeof (rec));
			return -1;
		      }
		    if (record_arch_list_add_mem
			(rec.msg_name, rec.msg_namelen))
		      return -1;
		    if (record_arch_list_add_mem
			(rec.msg_control, rec.msg_controllen))
		      return -1;
		    if (rec.msg_iov)
		      {
			for (i = 0; i < rec.msg_iovlen; i++)
			  {
			    if (target_read_memory
				(rec.msg_iov, (gdb_byte *) & iov,
				 sizeof (iov)))
			      {
				if (record_debug)
				  fprintf_unfiltered (gdb_stdlog,
						      "Process record: error "
						      "reading memory at "
						      "addr = 0x%s "
						      "len = %d.\n",
						      paddr_nz (rec.
								msg_iov),
						      (int) sizeof (iov));
				return -1;
			      }
			    if (record_arch_list_add_mem
				(iov.iov_base, iov.iov_len))
			      return -1;
			    rec.msg_iov += sizeof (struct record_iovec);
			  }
		      }
		  }
	      }
	  }
	  break;
	default:
	  printf_unfiltered (_("Process record and replay target "
			       "doesn't support socketcall call 0x%08x\n"),
			     tmpu32);
	  return -1;
	  break;
	}
      break;

      /* sys_syslog */
    case 103:
      break;

      /* sys_setitimer */
    case 104:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_itimerval))
	return -1;
      break;

      /* sys_getitimer */
    case 105:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_itimerval))
	return -1;
      break;

      /* sys_newstat */
    case 106:
      /* sys_newlstat */
    case 107:
      /* sys_newfstat */
    case 108:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_stat))
	return -1;
      break;

      /* sys_uname */
    case 109:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_utsname))
	return -1;
      break;

      /* sys_iopl */
    case 110:
      /* sys_vhangup */
    case 111:
      /* sys_ni_syscall */
    case 112:
      /* sys_vm86old */
    case 113:
      break;

      /* sys_wait4 */
    case 114:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	return -1;
      regcache_raw_read (regcache, tdep->arg4, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_rusage))
	return -1;
      break;

      /* sys_swapoff */
    case 115:
      break;

      /* sys_sysinfo */
    case 116:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_sysinfo))
	return -1;
      break;

      /* sys_ipc */
    case 117:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      switch (tmpu32)
	{
	case RECORD_MSGRCV:
	  {
	    int32_t second;
	    uint32_t ptr;
	    regcache_raw_read (regcache, tdep->arg3,
			       (gdb_byte *) & second);
	    regcache_raw_read (regcache, tdep->arg5,
			       (gdb_byte *) & ptr);
	    if (record_arch_list_add_mem (ptr, second + tdep->size_long))
	      return -1;
	  }
	  break;
	case RECORD_MSGCTL:
	  regcache_raw_read (regcache, tdep->arg5,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_msqid_ds))
	    return -1;
	  break;
	case RECORD_SHMAT:
	  regcache_raw_read (regcache, tdep->arg4,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_ulong))
	    return -1;
	  break;
	case RECORD_SHMCTL:
	  regcache_raw_read (regcache, tdep->arg5,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_shmid_ds))
	    return -1;
	  break;
	}
      break;

      /* sys_fsync */
    case 118:
      /* sys_sigreturn */
    case 119:
      /* sys_clone */
    case 120:
      /* sys_setdomainname */
    case 121:
      break;

      /* sys_newuname */
    case 122:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_new_utsname))
	return -1;
      break;

      /* sys_modify_ldt */
    case 123:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (tmpu32 == 0 || tmpu32 == 2)
	{
	  uint32_t ptr, bytecount;
	  regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & ptr);
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & bytecount);
	  if (record_arch_list_add_mem (ptr, bytecount))
	    return -1;
	}
      break;

      /* sys_adjtimex */
    case 124:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_timex))
	return -1;
      break;

      /* sys_mprotect */
    case 125:
      break;

      /* sys_sigprocmask */
    case 126:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_sigset_t))
	return -1;
      break;

      /* sys_ni_syscall */
    case 127:
      /* sys_init_module */
    case 128:
      /* sys_delete_module */
    case 129:
      /* sys_ni_syscall */
    case 130:
      break;

      /* sys_quotactl */
    case 131:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      switch (tmpu32)
	{
	case RECORD_Q_GETFMT:
	  regcache_raw_read (regcache, tdep->arg4,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, 4))
	    return -1;
	  break;
	case RECORD_Q_GETINFO:
	  regcache_raw_read (regcache, tdep->arg4,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_mem_dqinfo))
	    return -1;
	  break;
	case RECORD_Q_GETQUOTA:
	  regcache_raw_read (regcache, tdep->arg4,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_if_dqblk))
	    return -1;
	  break;
	case RECORD_Q_XGETQSTAT:
	case RECORD_Q_XGETQUOTA:
	  regcache_raw_read (regcache, tdep->arg4,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_fs_quota_stat))
	    return -1;
	  break;
	}
      break;

      /* sys_getpgid */
    case 132:
      /* sys_fchdir */
    case 133:
      /* sys_bdflush */
    case 134:
      break;

      /* sys_sysfs */
    case 135:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (tmpu32 == 2)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  /*XXX the size of memory is not very clear.  */
	  if (record_arch_list_add_mem (tmpu32, 10))
	    return -1;
	}
      break;

      /* sys_personality */
    case 136:
      /* sys_ni_syscall */
    case 137:
      /* sys_setfsuid16 */
    case 138:
      /* sys_setfsgid16 */
    case 139:
      break;

      /* sys_llseek */
    case 140:
      regcache_raw_read (regcache, tdep->arg4, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_loff_t))
	return -1;
      break;

      /* sys_getdents */
    case 141:
      {
	uint32_t count;
	regcache_raw_read (regcache, tdep->arg2,
			   (gdb_byte *) & tmpu32);
	regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & count);
	if (record_arch_list_add_mem (tmpu32, tdep->size_dirent * count))
	  return -1;
      }
      break;

      /* sys_select */
    case 142:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_fd_set))
	return -1;
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_fd_set))
	return -1;
      regcache_raw_read (regcache, tdep->arg4, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_fd_set))
	return -1;
      regcache_raw_read (regcache, tdep->arg5, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_timeval))
	return -1;
      break;

      /* sys_flock */
    case 143:
      /* sys_msync */
    case 144:
      break;

      /* sys_readv */
    case 145:
      {
	uint32_t vec;
	uint32_t vlen;
	struct record_iovec
	{
	  uint32_t iov_base;
	  uint32_t iov_len;
	} iov;
	regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & vec);
	if (vec)
	  {
	    regcache_raw_read (regcache, tdep->arg3,
			       (gdb_byte *) & vlen);
	    for (tmpu32 = 0; tmpu32 < vlen; tmpu32++)
	      {
		if (target_read_memory
		    (vec, (gdb_byte *) & iov, sizeof (struct record_iovec)))
		  {
		    if (record_debug)
		      fprintf_unfiltered (gdb_stdlog,
					  "Process record: error reading "
					  "memory at addr = 0x%s len = %d.\n",
					  paddr_nz (vec),
					  (int) sizeof (struct record_iovec));
		    return -1;
		  }
		if (record_arch_list_add_mem (iov.iov_base, iov.iov_len))
		  return -1;
		vec += sizeof (struct record_iovec);
	      }
	  }
      }
      break;

      /* sys_writev */
    case 146:
      /* sys_getsid */
    case 147:
      /* sys_fdatasync */
    case 148:
      /* sys_sysctl */
    case 149:
      /* sys_mlock */
    case 150:
      /* sys_munlock */
    case 151:
      /* sys_mlockall */
    case 152:
      /* sys_munlockall */
    case 153:
      /* sys_sched_setparam */
    case 154:
      break;

      /* sys_sched_getparam */
    case 155:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	return -1;
      break;

      /* sys_sched_setscheduler */
    case 156:
      /* sys_sched_getscheduler */
    case 157:
      /* sys_sched_yield */
    case 158:
      /* sys_sched_get_priority_max */
    case 159:
      /* sys_sched_get_priority_min */
    case 160:
      break;

      /* sys_sched_rr_get_interval */
    case 161:
      /* sys_nanosleep */
    case 162:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_timespec))
	return -1;
      break;

      /* sys_mremap */
    case 163:
      /* sys_setresuid16 */
    case 164:
      break;

      /* sys_getresuid16 */
    case 165:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_uid_t))
	return -1;
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_uid_t))
	return -1;
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_uid_t))
	return -1;
      break;

      /* sys_vm86 */
    case 166:
      /* sys_ni_syscall */
    case 167:
      break;

      /* sys_poll */
    case 168:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t nfds;
	  regcache_raw_read (regcache, tdep->arg2,
			     (gdb_byte *) & nfds);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_pollfd * nfds))
	    return -1;
	}
      break;

      /* sys_nfsservctl */
    case 169:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (tmpu32 == 7 || tmpu32 == 8)
	{
	  uint32_t rsize;
	  if (tmpu32 == 7)
	    rsize = tdep->size_NFS_FHSIZE;
	  else
	    rsize = tdep->size_knfsd_fh;
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, rsize))
	    return -1;
	}
      break;

      /* sys_setresgid16 */
    case 170:
      break;

      /* sys_getresgid16 */
    case 171:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_gid_t))
	return -1;
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_gid_t))
	return -1;
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_old_gid_t))
	return -1;
      break;

      /* sys_prctl */
    case 172:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      switch (tmpu32)
	{
	case 2:
	  regcache_raw_read (regcache, tdep->arg2,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	    return -1;
	  break;
	case 16:
	  regcache_raw_read (regcache, tdep->arg2,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_TASK_COMM_LEN))
	    return -1;
	  break;
	}
      break;

      /* sys_rt_sigreturn */
    case 173:
      break;

      /* sys_rt_sigaction */
    case 174:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_sigaction))
	return -1;
      break;

      /* sys_rt_sigprocmask */
    case 175:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_sigset_t))
	return -1;
      break;

      /* sys_rt_sigpending */
    case 176:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t sigsetsize;
	  regcache_raw_read (regcache, tdep->arg2,
			     (gdb_byte *) & sigsetsize);
	  if (record_arch_list_add_mem (tmpu32, sigsetsize))
	    return -1;
	}
      break;

      /* sys_rt_sigtimedwait */
    case 177:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_siginfo_t))
	return -1;
      break;

      /* sys_rt_sigqueueinfo */
    case 178:
      /* sys_rt_sigsuspend */
    case 179:
      break;

      /* sys_pread64 */
    case 180:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t count;
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & count);
	  if (record_arch_list_add_mem (tmpu32, count))
	    return -1;
	}
      break;

      /* sys_pwrite64 */
    case 181:
      /* sys_chown16 */
    case 182:
      break;

      /* sys_getcwd */
    case 183:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t size;
	  regcache_raw_read (regcache, tdep->arg2,
			     (gdb_byte *) & size);
	  if (record_arch_list_add_mem (tmpu32, size))
	    return -1;
	}
      break;

      /* sys_capget */
    case 184:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_cap_user_data_t))
	return -1;
      break;

      /* sys_capset */
    case 185:
      break;

      /* sys_sigaltstack */
    case 186:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_stack_t))
	return -1;
      break;

      /* sys_sendfile */
    case 187:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_off_t))
	return -1;
      break;

      /* sys_ni_syscall */
    case 188:
      /* sys_ni_syscall */
    case 189:
      /* sys_vfork */
    case 190:
      break;

      /* sys_getrlimit */
    case 191:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_rlimit))
	return -1;
      break;

      /* sys_mmap2 */
    case 192:
      break;

      /* sys_truncate64 */
    case 193:
      /* sys_ftruncate64 */
    case 194:
      break;

      /* sys_stat64 */
    case 195:
      /* sys_lstat64 */
    case 196:
      /* sys_fstat64 */
    case 197:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_stat64))
	return -1;
      break;

      /* sys_lchown */
    case 198:
      /* sys_getuid */
    case 199:
      /* sys_getgid */
    case 200:
      /* sys_geteuid */
    case 201:
      /* sys_getegid */
    case 202:
      /* sys_setreuid */
    case 203:
      /* sys_setregid */
    case 204:
      break;

      /* sys_getgroups */
    case 205:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  int gidsetsize;
	  regcache_raw_read (regcache, tdep->arg1,
			     (gdb_byte *) & gidsetsize);
	  if (record_arch_list_add_mem
	      (tmpu32, tdep->size_gid_t * gidsetsize))
	    return -1;
	}
      break;

      /* sys_setgroups */
    case 206:
      /* sys_fchown */
    case 207:
      /* sys_setresuid */
    case 208:
      break;

      /* sys_getresuid */
    case 209:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_uid_t))
	return -1;
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_uid_t))
	return -1;
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_uid_t))
	return -1;
      break;

      /* sys_setresgid */
    case 210:
      break;

      /* sys_getresgid */
    case 211:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_gid_t))
	return -1;
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_gid_t))
	return -1;
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_gid_t))
	return -1;
      break;

      /* sys_chown */
    case 212:
      /* sys_setuid */
    case 213:
      /* sys_setgid */
    case 214:
      /* sys_setfsuid */
    case 215:
      /* sys_setfsgid */
    case 216:
      /* sys_pivot_root */
    case 217:
      break;

      /* sys_mincore */
    case 218:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_PAGE_SIZE))
	return -1;
      break;

      /* sys_madvise */
    case 219:
      break;

      /* sys_getdents64 */
    case 220:
      {
	uint32_t count;
	regcache_raw_read (regcache, tdep->arg2,
			   (gdb_byte *) & tmpu32);
	regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & count);
	if (record_arch_list_add_mem (tmpu32, tdep->size_dirent64 * count))
	  return -1;
      }
      break;

      /* sys_fcntl64 */
    case 221:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      switch (tmpu32)
	{
	case F_GETLK64:
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_flock64))
	    return -1;
	  break;
	case F_SETLK64:
	case F_SETLKW64:
	  break;
	default:
	  goto sys_fcntl;
	  break;
	}
      break;

      /* sys_ni_syscall */
    case 222:
      /* sys_ni_syscall */
    case 223:
      /* sys_gettid */
    case 224:
      /* sys_readahead */
    case 225:
      /* sys_setxattr */
    case 226:
      /* sys_lsetxattr */
    case 227:
      /* sys_fsetxattr */
    case 228:
      break;

      /* sys_getxattr */
    case 229:
      /* sys_lgetxattr */
    case 230:
      /* sys_fgetxattr */
    case 231:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t size;
	  regcache_raw_read (regcache, tdep->arg4,
			     (gdb_byte *) & size);
	  if (record_arch_list_add_mem (tmpu32, size))
	    return -1;
	}
      break;

      /* sys_listxattr */
    case 232:
      /* sys_llistxattr */
    case 233:
      /* sys_flistxattr */
    case 234:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t size;
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & size);
	  if (record_arch_list_add_mem (tmpu32, size))
	    return -1;
	}
      break;

      /* sys_removexattr */
    case 235:
      /* sys_lremovexattr */
    case 236:
      /* sys_fremovexattr */
    case 237:
      /* sys_tkill */
    case 238:
      break;

      /* sys_sendfile64 */
    case 239:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_loff_t))
	return -1;
      break;

      /* sys_futex */
    case 240:
      /* sys_sched_setaffinity */
    case 241:
      break;

      /* sys_sched_getaffinity */
    case 242:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t len;
	  regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & len);
	  if (record_arch_list_add_mem (tmpu32, len))
	    return -1;
	}
      break;

      /* sys_set_thread_area */
    case 243:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	return -1;
      break;

      /* sys_get_thread_area */
    case 244:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_user_desc))
	return -1;
      break;

      /* sys_io_setup */
    case 245:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_long))
	return -1;
      break;

      /* sys_io_destroy */
    case 246:
      break;

      /* sys_io_getevents */
    case 247:
      regcache_raw_read (regcache, tdep->arg4, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  int32_t nr;
	  regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & nr);
	  if (record_arch_list_add_mem (tmpu32, nr * tdep->size_io_event))
	    return -1;
	}
      break;

      /* sys_io_submit */
    case 248:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  int32_t i, nr;
	  uint32_t *iocbp;
	  regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & nr);
	  iocbp = (uint32_t *) alloca (nr * tdep->size_int);
	  if (target_read_memory
	      (tmpu32, (gdb_byte *) iocbp, nr * tdep->size_int))
	    {
	      if (record_debug)
		fprintf_unfiltered (gdb_stdlog,
				    "Process record: error reading memory "
				    "at addr = 0x%s len = %d.\n",
				    paddr_nz (tmpu32), nr * tdep->size_int);
	      return -1;
	    }
	  for (i = 0; i < nr; i++)
	    {
	      if (record_arch_list_add_mem (iocbp[i], tdep->size_iocb))
		return -1;
	    }
	}
      break;

      /* sys_io_cancel */
    case 249:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_io_event))
	return -1;
      break;

      /* sys_fadvise64 */
    case 250:
      /* sys_ni_syscall */
    case 251:
      break;

      /* sys_exit_group */
    case 252:
      {
	int q;
	target_terminal_ours ();
	q =
	  yquery (_("The next instruction is syscall exit_group.  "
		    "It will make the program exit.  "
		    "Do you want to stop the program?"));
	target_terminal_inferior ();
	if (q)
	  return 1;
      }
      break;

      /* sys_lookup_dcookie */
    case 253:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t len;
	  regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & len);
	  if (record_arch_list_add_mem (tmpu32, len))
	    return -1;
	}
      break;

      /* sys_epoll_create */
    case 254:
      /* sys_epoll_ctl */
    case 255:
      break;

      /* sys_epoll_wait */
    case 256:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  int32_t maxevents;
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & maxevents);
	  if (record_arch_list_add_mem
	      (tmpu32, maxevents * tdep->size_epoll_event))
	    return -1;
	}
      break;

      /* sys_remap_file_pages */
    case 257:
      /* sys_set_tid_address */
    case 258:
      break;

      /* sys_timer_create */
    case 259:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	return -1;
      break;

      /* sys_timer_settime */
    case 260:
      regcache_raw_read (regcache, tdep->arg4, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_itimerspec))
	return -1;
      break;

      /* sys_timer_gettime */
    case 261:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_itimerspec))
	return -1;
      break;

      /* sys_timer_getoverrun */
    case 262:
      /* sys_timer_delete */
    case 263:
      /* sys_clock_settime */
    case 264:
      break;

      /* sys_clock_gettime */
    case 265:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_timespec))
	return -1;
      break;

      /* sys_clock_getres */
    case 266:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_timespec))
	return -1;
      break;

      /* sys_clock_nanosleep */
    case 267:
      regcache_raw_read (regcache, tdep->arg4, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_timespec))
	return -1;
      break;

      /* sys_statfs64 */
    case 268:
      /* sys_fstatfs64 */
    case 269:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_statfs64))
	return -1;
      break;

      /* sys_tgkill */
    case 270:
      /* sys_utimes */
    case 271:
      /* sys_fadvise64_64 */
    case 272:
      /* sys_ni_syscall */
    case 273:
      /* sys_mbind */
    case 274:
      break;

      /* sys_get_mempolicy */
    case 275:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	return -1;
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t maxnode;
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & maxnode);
	  if (record_arch_list_add_mem (tmpu32, maxnode * tdep->size_long))
	    return -1;
	}
      break;

      /* sys_set_mempolicy */
    case 276:
      /* sys_mq_open */
    case 277:
      /* sys_mq_unlink */
    case 278:
      /* sys_mq_timedsend */
    case 279:
      break;

      /* sys_mq_timedreceive */
    case 280:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t msg_len;
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & msg_len);
	  if (record_arch_list_add_mem (tmpu32, msg_len))
	    return -1;
	}
      regcache_raw_read (regcache, tdep->arg4, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	return -1;
      break;

      /* sys_mq_notify */
    case 281:
      break;

      /* sys_mq_getsetattr */
    case 282:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_mq_attr))
	return -1;
      break;

      /* sys_kexec_load */
    case 283:
      break;

      /* sys_waitid */
    case 284:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_siginfo))
	return -1;
      regcache_raw_read (regcache, tdep->arg5, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_rusage))
	return -1;
      break;

      /* sys_ni_syscall */
    case 285:
      /* sys_add_key */
    case 286:
      /* sys_request_key */
    case 287:
      break;

      /* sys_keyctl */
    case 288:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (tmpu32 == 6 || tmpu32 == 11)
	{
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & tmpu32);
	  if (tmpu32)
	    {
	      uint32_t buflen;
	      regcache_raw_read (regcache, tdep->arg4,
				 (gdb_byte *) & buflen);
	      if (record_arch_list_add_mem (tmpu32, buflen))
		return -1;
	    }
	}
      break;

      /* sys_ioprio_set */
    case 289:
      /* sys_ioprio_get */
    case 290:
      /* sys_inotify_init */
    case 291:
      /* sys_inotify_add_watch */
    case 292:
      /* sys_inotify_rm_watch */
    case 293:
      /* sys_migrate_pages */
    case 294:
      /* sys_openat */
    case 295:
      /* sys_mkdirat */
    case 296:
      /* sys_mknodat */
    case 297:
      /* sys_fchownat */
    case 298:
      /* sys_futimesat */
    case 299:
      break;

      /* sys_fstatat64 */
    case 300:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_stat64))
	return -1;
      break;

      /* sys_unlinkat */
    case 301:
      /* sys_renameat */
    case 302:
      /* sys_linkat */
    case 303:
      /* sys_symlinkat */
    case 304:
      break;

      /* sys_readlinkat */
    case 305:
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  int32_t bufsiz;
	  regcache_raw_read (regcache, tdep->arg4,
			     (gdb_byte *) & bufsiz);
	  if (record_arch_list_add_mem (tmpu32, bufsiz))
	    return -1;
	}
      break;

      /* sys_fchmodat */
    case 306:
      /* sys_faccessat */
    case 307:
      break;

      /* sys_pselect6 */
    case 308:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_fd_set))
	return -1;
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_fd_set))
	return -1;
      regcache_raw_read (regcache, tdep->arg4, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_fd_set))
	return -1;
      regcache_raw_read (regcache, tdep->arg5, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_timespec))
	return -1;
      break;

      /* sys_ppoll */
    case 309:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t nfds;
	  regcache_raw_read (regcache, tdep->arg2,
			     (gdb_byte *) & nfds);
	  if (record_arch_list_add_mem (tmpu32, tdep->size_pollfd * nfds))
	    return -1;
	}
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_timespec))
	return -1;
      break;

      /* sys_unshare */
    case 310:
      /* sys_set_robust_list */
    case 311:
      break;

      /* sys_get_robust_list */
    case 312:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	return -1;
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	return -1;
      break;

      /* sys_splice */
    case 313:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_loff_t))
	return -1;
      regcache_raw_read (regcache, tdep->arg4, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_loff_t))
	return -1;
      break;

      /* sys_sync_file_range */
    case 314:
      /* sys_tee */
    case 315:
      /* sys_vmsplice */
    case 316:
      break;

      /* sys_move_pages */
    case 317:
      regcache_raw_read (regcache, tdep->arg5, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  uint32_t nr_pages;
	  regcache_raw_read (regcache, tdep->arg2,
			     (gdb_byte *) & nr_pages);
	  if (record_arch_list_add_mem (tmpu32, nr_pages * tdep->size_int))
	    return -1;
	}
      break;

      /* sys_getcpu */
    case 318:
      regcache_raw_read (regcache, tdep->arg1, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	return -1;
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_int))
	return -1;
      regcache_raw_read (regcache, tdep->arg3, (gdb_byte *) & tmpu32);
      if (record_arch_list_add_mem (tmpu32, tdep->size_ulong * 2))
	return -1;
      break;

      /* sys_epoll_pwait */
    case 319:
      regcache_raw_read (regcache, tdep->arg2, (gdb_byte *) & tmpu32);
      if (tmpu32)
	{
	  int32_t maxevents;
	  regcache_raw_read (regcache, tdep->arg3,
			     (gdb_byte *) & maxevents);
	  if (record_arch_list_add_mem
	      (tmpu32, maxevents * tdep->size_epoll_event))
	    return -1;
	}
      break;

    default:
      printf_unfiltered (_("Process record and replay target doesn't "
			   "support syscall number 0x%08x\n"),
			 tmpu32);
      return -1;
      break;
    }

  return 0;
}
