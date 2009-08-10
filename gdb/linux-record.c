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
#include "gdbtypes.h"
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

#define OUTPUT_REG(val, num)      phex_nz ((val), \
    TYPE_LENGTH (gdbarch_register_type (get_regcache_arch (regcache), (num))))

static int
record_linux_sockaddr (struct regcache *regcache,
                       struct linux_record_tdep *tdep, ULONGEST addr,
                       ULONGEST len)
{
  gdb_byte *a;
  int addrlen;
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);

  if (!addr)
    return 0;

  a = alloca (tdep->size_int);

  if (record_arch_list_add_mem ((CORE_ADDR) len, tdep->size_int))
    return -1;

  /* Get the addrlen.  */
  if (target_read_memory ((CORE_ADDR) len, a, tdep->size_int))
    {
      if (record_debug)
        fprintf_unfiltered (gdb_stdlog,
                            "Process record: error reading "
                            "memory at addr = 0x%s len = %d.\n",
                            phex_nz (len, tdep->size_pointer),
                            tdep->size_int);
        return -1;
    }
  addrlen = (int) extract_unsigned_integer (a, tdep->size_int, byte_order);
  if (addrlen <= 0 || addrlen > tdep->size_sockaddr)
    addrlen = tdep->size_sockaddr;

  if (record_arch_list_add_mem ((CORE_ADDR) addr, addrlen))
    return -1;

  return 0;
}

static int
record_linux_msghdr (struct regcache *regcache,
                     struct linux_record_tdep *tdep, ULONGEST addr)
{
  gdb_byte *a;
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  CORE_ADDR tmpaddr;
  int tmpint;

  if (!addr)
    return 0;

  if (record_arch_list_add_mem ((CORE_ADDR) addr, tdep->size_msghdr))
    return -1;

  a = alloca (tdep->size_msghdr);
  if (target_read_memory ((CORE_ADDR) addr, a, tdep->size_msghdr))
    {
      if (record_debug)
        fprintf_unfiltered (gdb_stdlog,
                            "Process record: error reading "
                            "memory at addr = 0x%s "
                            "len = %d.\n",
                            phex_nz (addr, tdep->size_pointer),
                            tdep->size_msghdr);
        return -1;
    }

  /* msg_name msg_namelen */
  addr = extract_unsigned_integer (a, tdep->size_pointer, byte_order);
  a += tdep->size_pointer;
  if (record_arch_list_add_mem ((CORE_ADDR) addr,
                                (int) extract_unsigned_integer (a,
				                                tdep->size_int,
				                                byte_order)))
    return -1;
  a += tdep->size_int;

  /* msg_iov msg_iovlen */
  addr = extract_unsigned_integer (a, tdep->size_pointer, byte_order);
  a += tdep->size_pointer;
  if (addr)
    {
      ULONGEST i;
      ULONGEST len = extract_unsigned_integer (a, tdep->size_size_t,
                                               byte_order);
      gdb_byte *iov = alloca (tdep->size_iovec);

      for (i = 0; i < len; i++)
        {
          if (target_read_memory ((CORE_ADDR) addr, iov, tdep->size_iovec))
            {
              if (record_debug)
                fprintf_unfiltered (gdb_stdlog,
                                    "Process record: error "
                                    "reading memory at "
                                    "addr = 0x%s "
                                    "len = %d.\n",
                                    phex_nz (addr,tdep->size_pointer),
                                    tdep->size_iovec);
                return -1;
            }
          tmpaddr = (CORE_ADDR) extract_unsigned_integer (iov,
                                                          tdep->size_pointer,
                                                          byte_order);
          tmpint = (int) extract_unsigned_integer (iov + tdep->size_pointer,
                                                   tdep->size_size_t,
                                                   byte_order);
          if (record_arch_list_add_mem (tmpaddr, tmpint));
            return -1;
          addr += tdep->size_iovec;
        }
    }
  a += tdep->size_size_t;

  /* msg_control msg_controllen */
  addr = extract_unsigned_integer (a, tdep->size_pointer, byte_order);
  a += tdep->size_pointer;
  tmpint = (int) extract_unsigned_integer (a, tdep->size_size_t, byte_order);
  if (record_arch_list_add_mem ((CORE_ADDR) addr, tmpint));
    return -1;

  return 0;
}

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
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  ULONGEST tmpulongest;
  CORE_ADDR tmpaddr;
  int tmpint;

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
        q = yquery (_("The next instruction is syscall exit.  "
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
        ULONGEST addr, count;
        regcache_raw_read_unsigned (regcache, tdep->arg2, &addr);
        regcache_raw_read_unsigned (regcache, tdep->arg3, &count);
        if (record_arch_list_add_mem ((CORE_ADDR) addr, (int) count))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size__old_kernel_stat))
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
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (tmpulongest == RECORD_PTRACE_PEEKTEXT
          || tmpulongest == RECORD_PTRACE_PEEKDATA
          || tmpulongest == RECORD_PTRACE_PEEKUSR)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg4,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, 4))
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
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_tms))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (tmpulongest == tdep->ioctl_FIOCLEX
          || tmpulongest == tdep->ioctl_FIONCLEX
          || tmpulongest == tdep->ioctl_FIONBIO
          || tmpulongest == tdep->ioctl_FIOASYNC
          || tmpulongest == tdep->ioctl_TCSETS
          || tmpulongest == tdep->ioctl_TCSETSW
          || tmpulongest == tdep->ioctl_TCSETSF
          || tmpulongest == tdep->ioctl_TCSETA
          || tmpulongest == tdep->ioctl_TCSETAW
          || tmpulongest == tdep->ioctl_TCSETAF
          || tmpulongest == tdep->ioctl_TCSBRK
          || tmpulongest == tdep->ioctl_TCXONC
          || tmpulongest == tdep->ioctl_TCFLSH
          || tmpulongest == tdep->ioctl_TIOCEXCL
          || tmpulongest == tdep->ioctl_TIOCNXCL
          || tmpulongest == tdep->ioctl_TIOCSCTTY
          || tmpulongest == tdep->ioctl_TIOCSPGRP
          || tmpulongest == tdep->ioctl_TIOCSTI
          || tmpulongest == tdep->ioctl_TIOCSWINSZ
          || tmpulongest == tdep->ioctl_TIOCMBIS
          || tmpulongest == tdep->ioctl_TIOCMBIC
          || tmpulongest == tdep->ioctl_TIOCMSET
          || tmpulongest == tdep->ioctl_TIOCSSOFTCAR
          || tmpulongest == tdep->ioctl_TIOCCONS
          || tmpulongest == tdep->ioctl_TIOCSSERIAL
          || tmpulongest == tdep->ioctl_TIOCPKT
          || tmpulongest == tdep->ioctl_TIOCNOTTY
          || tmpulongest == tdep->ioctl_TIOCSETD
          || tmpulongest == tdep->ioctl_TCSBRKP
          || tmpulongest == tdep->ioctl_TIOCTTYGSTRUCT
          || tmpulongest == tdep->ioctl_TIOCSBRK
          || tmpulongest == tdep->ioctl_TIOCCBRK
          || tmpulongest == tdep->ioctl_TCSETS2
          || tmpulongest == tdep->ioctl_TCSETSW2
          || tmpulongest == tdep->ioctl_TCSETSF2
          || tmpulongest == tdep->ioctl_TIOCSPTLCK
          || tmpulongest == tdep->ioctl_TIOCSERCONFIG
          || tmpulongest == tdep->ioctl_TIOCSERGWILD
          || tmpulongest == tdep->ioctl_TIOCSERSWILD
          || tmpulongest == tdep->ioctl_TIOCSLCKTRMIOS
          || tmpulongest == tdep->ioctl_TIOCSERGETMULTI
          || tmpulongest == tdep->ioctl_TIOCSERSETMULTI
          || tmpulongest == tdep->ioctl_TIOCMIWAIT
          || tmpulongest == tdep->ioctl_TIOCSHAYESESP)
        {
          /* Nothing to do.  */
        }
      else if (tmpulongest == tdep->ioctl_TCGETS
               || tmpulongest == tdep->ioctl_TCGETA
               || tmpulongest == tdep->ioctl_TIOCGLCKTRMIOS)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_termios))
            return -1;
        }
      else if (tmpulongest == tdep->ioctl_TIOCGPGRP
               || tmpulongest == tdep->ioctl_TIOCGSID)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_pid_t))
            return -1;
        }
      else if (tmpulongest == tdep->ioctl_TIOCOUTQ
               || tmpulongest == tdep->ioctl_TIOCMGET
               || tmpulongest == tdep->ioctl_TIOCGSOFTCAR
               || tmpulongest == tdep->ioctl_FIONREAD
               || tmpulongest == tdep->ioctl_TIOCINQ
               || tmpulongest == tdep->ioctl_TIOCGETD
               || tmpulongest == tdep->ioctl_TIOCGPTN
               || tmpulongest == tdep->ioctl_TIOCSERGETLSR)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_int))
            return -1;
        }
      else if (tmpulongest == tdep->ioctl_TIOCGWINSZ)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_winsize))
            return -1;
        }
      else if (tmpulongest == tdep->ioctl_TIOCLINUX)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          /* This syscall affect a char size memory.  */
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, 1))
            return -1;
        }
      else if (tmpulongest == tdep->ioctl_TIOCGSERIAL)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_serial_struct))
            return -1;
        }
      else if (tmpulongest == tdep->ioctl_TCGETS2)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_termios2))
            return -1;
        }
      else if (tmpulongest == tdep->ioctl_FIOQSIZE)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_loff_t))
            return -1;
        }
      else if (tmpulongest == tdep->ioctl_TIOCGICOUNT)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_serial_icounter_struct))
            return -1;
        }
      else if (tmpulongest == tdep->ioctl_TIOCGHAYESESP)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_hayes_esp_config))
            return -1;
        }
      else if (tmpulongest == tdep->ioctl_TIOCSERGSTRUCT)
        {
          printf_unfiltered (_("Process record and replay target doesn't "
                               "support ioctl request TIOCSERGSTRUCT\n"));
          return 1;
        }
      else
        {
          printf_unfiltered (_("Process record and replay target doesn't "
                               "support ioctl request 0x%s.\n"),
                             OUTPUT_REG (tmpulongest, tdep->arg2));
          return 1;
        }
      break;

      /* sys_fcntl */
    case 55:
      /* XXX */
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
    sys_fcntl:
      if (tmpulongest == tdep->fcntl_F_GETLK)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_flock))
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
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_oldold_utsname))
        return -1;
      break;

      /* sys_umask */
    case 60:
      /* sys_chroot */
    case 61:
      break;

      /* sys_ustat */
    case 62:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_ustat))
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
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_sigaction))
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
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_sigset_t))
        return -1;
      break;

      /* sys_sethostname */
    case 74:
      /* sys_setrlimit */
    case 75:
      break;

      /* sys_old_getrlimit */
    case 76:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_rlimit))
        return -1;
      break;

      /* sys_getrusage */
    case 77:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_rusage))
        return -1;
      break;

      /* sys_gettimeofday */
    case 78:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_timeval))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_timezone))
        return -1;
      break;

      /* sys_settimeofday */
    case 79:
      break;

      /* sys_getgroups16 */
    case 80:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_gid_t))
        return -1;
      break;

      /* sys_setgroups16 */
    case 81:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_gid_t))
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

        regcache_raw_read_unsigned (regcache, tdep->arg1,
                                    &tmpulongest);
        if (tmpulongest)
          {
            if (target_read_memory (tmpulongest, (gdb_byte *) &sel,
                                    sizeof(sel)))
              {
                if (record_debug)
                  fprintf_unfiltered (gdb_stdlog,
                                      "Process record: error reading memory "
                                      "at addr = 0x%s len = %lu.\n",
                                      OUTPUT_REG (tmpulongest, tdep->arg1),
                                      (unsigned long) sizeof (sel));
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
        ULONGEST len;
        regcache_raw_read_unsigned (regcache, tdep->arg2,
                                    &tmpulongest);
        regcache_raw_read_unsigned (regcache, tdep->arg3, &len);
        if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, (int) len))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_dirent))
        return -1;
      break;

      /* old_mmap */
    case 90:
      break;

      /* sys_munmap */
    case 91:
      {
        int q;
        ULONGEST len;

        regcache_raw_read_unsigned (regcache, tdep->arg1,
                                    &tmpulongest);
        regcache_raw_read_unsigned (regcache, tdep->arg2, &len);
        target_terminal_ours ();
        q = yquery (_("The next instruction is syscall munmap.  "
                      "It will free the memory addr = 0x%s len = %u.  "
                      "It will make record target get error.  "
                      "Do you want to stop the program?"),
                    OUTPUT_REG (tmpulongest, tdep->arg1), (int) len);
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_statfs))
        return -1;
      break;

      /* sys_ioperm */
    case 101:
      break;

      /* sys_socket */
    case 500:
      /* sys_sendto */
    case 503:
      /* sys_sendmsg */
    case 505:
      /* sys_shutdown */
    case 507:
      /* sys_bind */
    case 508:
      /* sys_connect */
    case 501:
      /* sys_listen */
    case 509:
      /* sys_setsockopt */
    case 513:
      break;

      /* sys_accept */
    case 502:
      /* sys_getsockname */
    case 510:
      /* sys_getpeername */
    case 511:
      {
        ULONGEST len;
        regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
        regcache_raw_read_unsigned (regcache, tdep->arg3, &len);
        if (record_linux_sockaddr (regcache, tdep, tmpulongest, len))
          return -1;
      }
      break;

      /* sys_recvfrom */
    case 504:
      {
        ULONGEST len;
        regcache_raw_read_unsigned (regcache, tdep->arg4, &tmpulongest);
        regcache_raw_read_unsigned (regcache, tdep->arg5, &len);
        if (record_linux_sockaddr (regcache, tdep, tmpulongest, len))
          return -1;
      }
      /* sys_recv */
    case 515:
      {
        ULONGEST size;
        regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
        regcache_raw_read_unsigned (regcache, tdep->arg3, &size);
        if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, (int) size))
          return -1;
      }
      break;

      /* sys_recvmsg */
    case 506:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_linux_msghdr (regcache, tdep, tmpulongest))
        return -1;
      break;

      /* sys_socketpair */
    case 512:
      regcache_raw_read_unsigned (regcache, tdep->arg4, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_int))
        return -1;
      break;

      /* sys_getsockopt */
    case 514:
      regcache_raw_read_unsigned (regcache, tdep->arg5, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST optvalp;
          gdb_byte *optlenp = alloca (tdep->size_int);
          if (target_read_memory ((CORE_ADDR) tmpulongest, optlenp,
                                  tdep->size_int))
            {
              if (record_debug)
                fprintf_unfiltered (gdb_stdlog,
                                    "Process record: error reading "
                                    "memory at addr = 0x%s "
                                    "len = %d.\n",
                                    OUTPUT_REG (tmpulongest, tdep->arg5),
                                    tdep->size_int);
              return -1;
            }
          regcache_raw_read_unsigned (regcache, tdep->arg4, &optvalp);
          tmpint = (int) extract_signed_integer (optlenp, tdep->size_int,
                                                 byte_order);
          if (record_arch_list_add_mem ((CORE_ADDR) optvalp, tmpint))
            return -1;
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_int))
            return -1;
        }
      break;

      /* sys_socketcall */
    case 102:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      switch (tmpulongest)
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
            regcache_raw_read_unsigned (regcache, tdep->arg2,
                                        &tmpulongest);
            if (tmpulongest)
              {
                gdb_byte *a = alloca (tdep->size_ulong * 2);
                int addrlen;
                gdb_byte *addrlenp;
                ULONGEST len;

                tmpulongest += tdep->size_ulong;
                if (target_read_memory ((CORE_ADDR) tmpulongest, a,
                                        tdep->size_ulong * 2))
                  {
                    if (record_debug)
                      fprintf_unfiltered (gdb_stdlog,
                                          "Process record: error reading "
                                          "memory at addr = 0x%s len = %d.\n",
                                          OUTPUT_REG (tmpulongest, tdep->arg2),
                                          tdep->size_ulong * 2);
                    return -1;
                  }
                tmpulongest = extract_unsigned_integer (a,
                                                        tdep->size_ulong,
                                                        byte_order);
                len = extract_unsigned_integer (a + tdep->size_ulong,
                                                tdep->size_ulong, byte_order);
                if (record_linux_sockaddr (regcache, tdep, tmpulongest, len))
                  return -1;
              }
          }
          break;

        case RECORD_SYS_SOCKETPAIR:
          {
            gdb_byte *a = alloca (tdep->size_ulong);
            regcache_raw_read_unsigned (regcache, tdep->arg2,
                                        &tmpulongest);
            if (tmpulongest)
              {
                tmpulongest += tdep->size_ulong * 3;
                if (target_read_memory ((CORE_ADDR) tmpulongest, a,
                                        tdep->size_ulong))
                  {
                    if (record_debug)
                      fprintf_unfiltered (gdb_stdlog,
                                          "Process record: error reading "
                                          "memory at addr = 0x%s len = %d.\n",
                                          OUTPUT_REG (tmpulongest, tdep->arg2),
                                          tdep->size_ulong);
                    return -1;
                  }
                tmpaddr
                  = (CORE_ADDR) extract_unsigned_integer (a, tdep->size_ulong,
                                                          byte_order);
                if (record_arch_list_add_mem (tmpaddr, tdep->size_int))
                  return -1;
              }
          }
          break;
        case RECORD_SYS_SEND:
        case RECORD_SYS_SENDTO:
          break;
        case RECORD_SYS_RECVFROM:
          regcache_raw_read_unsigned (regcache, tdep->arg2,
                                      &tmpulongest);
          if (tmpulongest)
            {
              gdb_byte *a = alloca (tdep->size_ulong * 2);
              int addrlen;
              gdb_byte *addrlenp;
              ULONGEST len;

              tmpulongest += tdep->size_ulong * 4;
              if (target_read_memory ((CORE_ADDR) tmpulongest, a,
                                      tdep->size_ulong * 2))
                {
                  if (record_debug)
                    fprintf_unfiltered (gdb_stdlog,
                                        "Process record: error reading "
                                        "memory at addr = 0x%s len = %d.\n",
                                        OUTPUT_REG (tmpulongest, tdep->arg2),
                                        tdep->size_ulong * 2);
                  return -1;
                }
              tmpulongest = extract_unsigned_integer (a, tdep->size_ulong,
                                                      byte_order);
              len = extract_unsigned_integer (a + tdep->size_ulong,
                                              tdep->size_ulong, byte_order);
              if (record_linux_sockaddr (regcache, tdep, tmpulongest, len))
                return -1;
            }
        case RECORD_SYS_RECV:
          regcache_raw_read_unsigned (regcache, tdep->arg2,
                                      &tmpulongest);
          if (tmpulongest)
            {
              gdb_byte *a = alloca (tdep->size_ulong * 2);

              tmpulongest += tdep->size_ulong;
              if (target_read_memory ((CORE_ADDR) tmpulongest, a,
                                      tdep->size_ulong))
                {
                  if (record_debug)
                    fprintf_unfiltered (gdb_stdlog,
                                        "Process record: error reading "
                                        "memory at addr = 0x%s len = %d.\n",
                                        OUTPUT_REG (tmpulongest, tdep->arg2),
                                        tdep->size_ulong);
                    return -1;
                }
              tmpulongest = extract_unsigned_integer (a, tdep->size_ulong,
                                                      byte_order);
              if (tmpulongest)
                {
                  a += tdep->size_ulong;
                  tmpint = (int) extract_unsigned_integer (a, tdep->size_ulong,
                                                           byte_order);
                  if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                                tmpint))
                    return -1;
                }
            }
          break;
        case RECORD_SYS_SHUTDOWN:
        case RECORD_SYS_SETSOCKOPT:
          break;
        case RECORD_SYS_GETSOCKOPT:
          {
            gdb_byte *a = alloca (tdep->size_ulong * 2);
            gdb_byte *av = alloca (tdep->size_int);

            regcache_raw_read_unsigned (regcache, tdep->arg2,
                                        &tmpulongest);
            if (tmpulongest)
              {
                tmpulongest += tdep->size_ulong * 3;
                if (target_read_memory ((CORE_ADDR) tmpulongest, a,
                                        tdep->size_ulong * 2))
                  {
                    if (record_debug)
                      fprintf_unfiltered (gdb_stdlog,
                                          "Process record: error reading "
                                          "memory at addr = 0x%s len = %d.\n",
                                          OUTPUT_REG (tmpulongest, tdep->arg2),
                                          tdep->size_ulong * 2);
                    return -1;
                  }
                tmpulongest = extract_unsigned_integer (a + tdep->size_ulong,
                                                        tdep->size_ulong,
                                                        byte_order);
                if (tmpulongest)
                  {
                    if (target_read_memory ((CORE_ADDR) tmpulongest, av,
                                            tdep->size_int))
                      {
                        if (record_debug)
                          fprintf_unfiltered (gdb_stdlog,
                                              "Process record: error reading "
                                              "memory at addr = 0x%s "
                                              "len = %d.\n",
                                              phex_nz (tmpulongest,
                                                       tdep->size_ulong),
                                              tdep->size_int);
                        return -1;
                      }
                    tmpaddr
                      = (CORE_ADDR) extract_unsigned_integer (a,
                                                              tdep->size_ulong,
                                                              byte_order);
                    tmpint = (int) extract_unsigned_integer (av,
                                                             tdep->size_int,
                                                             byte_order);
                    if (record_arch_list_add_mem (tmpaddr, tmpint))
                      return -1;
                    a += tdep->size_ulong;
                    tmpaddr
                      = (CORE_ADDR) extract_unsigned_integer (a,
                                                              tdep->size_ulong,
                                                              byte_order);
                    if (record_arch_list_add_mem (tmpaddr, tdep->size_int))
                      return -1;
                  }
              }
          }
          break;
        case RECORD_SYS_SENDMSG:
          break;
        case RECORD_SYS_RECVMSG:
          {
            gdb_byte *a = alloca (tdep->size_ulong);

            regcache_raw_read_unsigned (regcache, tdep->arg2,
                                        &tmpulongest);
            if (tmpulongest)
              {
                tmpulongest += tdep->size_ulong;
                if (target_read_memory ((CORE_ADDR) tmpulongest, a,
                                        tdep->size_ulong))
                  {
                    if (record_debug)
                      fprintf_unfiltered (gdb_stdlog,
                                          "Process record: error reading "
                                          "memory at addr = 0x%s len = %d.\n",
                                          OUTPUT_REG (tmpulongest, tdep->arg2),
                                          tdep->size_ulong);
                    return -1;
                  }
                tmpulongest = extract_unsigned_integer (a, tdep->size_ulong,
                                                        byte_order);
                if (record_linux_msghdr (regcache, tdep, tmpulongest))
                  return -1;
              }
          }
          break;
        default:
          printf_unfiltered (_("Process record and replay target "
                               "doesn't support socketcall call 0x%s\n"),
                             OUTPUT_REG (tmpulongest, tdep->arg1));
          return -1;
          break;
        }
      break;

      /* sys_syslog */
    case 103:
      break;

      /* sys_setitimer */
    case 104:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_itimerval))
        return -1;
      break;

      /* sys_getitimer */
    case 105:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_itimerval))
        return -1;
      break;

      /* sys_newstat */
    case 106:
      /* sys_newlstat */
    case 107:
      /* sys_newfstat */
    case 108:
      /* sys_newfstatat */
    case 540:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_stat))
        return -1;
      break;

      /* sys_uname */
    case 109:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_utsname))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_int))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg4, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_rusage))
        return -1;
      break;

      /* sys_swapoff */
    case 115:
      break;

      /* sys_sysinfo */
    case 116:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_sysinfo))
        return -1;
      break;

      /* sys_shmget */
    case 520:
      /* sys_semget */
    case 523:
      /* sys_semop */
    case 524:
      /* sys_msgget */
    case 528:
      /* sys_shmdt */
      /* XXX maybe need do some record works with sys_shmdt.  */
    case 527:
      /* sys_msgsnd */
    case 529:
      /* sys_semtimedop */
    case 532:
      break;

      /* sys_shmat */
    case 521:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_ulong))
        return -1;
      break;

      /* sys_shmctl */
    case 522:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_shmid_ds))
        return -1;
      break;

      /* sys_semctl */
      /* XXX sys_semctl 525 still not support.  */

      /* sys_msgrcv */
    case 530:
      {
        ULONGEST msgp;
        regcache_raw_read_signed (regcache, tdep->arg3, &tmpulongest);
        regcache_raw_read_unsigned (regcache, tdep->arg2, &msgp);
        tmpint = (int) tmpulongest + tdep->size_long;
        if (record_arch_list_add_mem ((CORE_ADDR) msgp, tmpint))
          return -1;
      }
      break;

      /* sys_msgctl */
    case 531:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_msqid_ds))
        return -1;
      break;

      /* sys_ipc */
    case 117:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      tmpulongest &= 0xffff;
      switch (tmpulongest)
        {
        case RECORD_SEMOP:
        case RECORD_SEMGET:
        case RECORD_SEMTIMEDOP:
        case RECORD_MSGSND:
        case RECORD_MSGGET:
          /* XXX maybe need do some record works with RECORD_SHMDT.  */
        case RECORD_SHMDT:
        case RECORD_SHMGET:
          break;
        case RECORD_MSGRCV:
          {
            ULONGEST second;
            ULONGEST ptr;
            regcache_raw_read_signed (regcache, tdep->arg3, &second);
            regcache_raw_read_unsigned (regcache, tdep->arg5, &ptr);
            tmpint = (int) second + tdep->size_long;
            if (record_arch_list_add_mem ((CORE_ADDR) ptr, tmpint))
              return -1;
          }
          break;
        case RECORD_MSGCTL:
          regcache_raw_read_unsigned (regcache, tdep->arg5,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_msqid_ds))
            return -1;
          break;
        case RECORD_SHMAT:
          regcache_raw_read_unsigned (regcache, tdep->arg4,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_ulong))
            return -1;
          break;
        case RECORD_SHMCTL:
          regcache_raw_read_unsigned (regcache, tdep->arg5,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_shmid_ds))
            return -1;
          break;
        default:
          /* XXX RECORD_SEMCTL still not support.  */
          printf_unfiltered (_("Process record and replay target doesn't "
                               "support ipc number %d\n"), (int) tmpulongest);
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
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_new_utsname))
        return -1;
      break;

      /* sys_modify_ldt */
    case 123:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (tmpulongest == 0 || tmpulongest == 2)
        {
          ULONGEST ptr, bytecount;
          regcache_raw_read_unsigned (regcache, tdep->arg2, &ptr);
          regcache_raw_read_unsigned (regcache, tdep->arg3, &bytecount);
          if (record_arch_list_add_mem ((CORE_ADDR) ptr, (int) bytecount))
            return -1;
        }
      break;

      /* sys_adjtimex */
    case 124:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_timex))
        return -1;
      break;

      /* sys_mprotect */
    case 125:
      break;

      /* sys_sigprocmask */
    case 126:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_sigset_t))
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
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      switch (tmpulongest)
        {
        case RECORD_Q_GETFMT:
          regcache_raw_read_unsigned (regcache, tdep->arg4,
                                      &tmpulongest);
          /* __u32 */
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, 4))
            return -1;
          break;
        case RECORD_Q_GETINFO:
          regcache_raw_read_unsigned (regcache, tdep->arg4,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_mem_dqinfo))
            return -1;
          break;
        case RECORD_Q_GETQUOTA:
          regcache_raw_read_unsigned (regcache, tdep->arg4,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_if_dqblk))
            return -1;
          break;
        case RECORD_Q_XGETQSTAT:
        case RECORD_Q_XGETQUOTA:
          regcache_raw_read_unsigned (regcache, tdep->arg4,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_fs_quota_stat))
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
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (tmpulongest == 2)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          /*XXX the size of memory is not very clear.  */
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, 10))
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
      regcache_raw_read_unsigned (regcache, tdep->arg4, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_loff_t))
        return -1;
      break;

      /* sys_getdents */
    case 141:
      {
        ULONGEST count;
        regcache_raw_read_unsigned (regcache, tdep->arg2,
                                    &tmpulongest);
        regcache_raw_read_unsigned (regcache, tdep->arg3, &count);
        if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                      tdep->size_dirent * count))
          return -1;
      }
      break;

      /* sys_select */
    case 142:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_fd_set))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_fd_set))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg4, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_fd_set))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg5, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_timeval))
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
        ULONGEST vec, vlen;

        regcache_raw_read_unsigned (regcache, tdep->arg2, &vec);
        if (vec)
          {
            gdb_byte *iov = alloca (tdep->size_iovec);

            regcache_raw_read_unsigned (regcache, tdep->arg3, &vlen);
            for (tmpulongest = 0; tmpulongest < vlen; tmpulongest++)
              {
                if (target_read_memory ((CORE_ADDR) vec, iov,
                                        tdep->size_iovec))
                  {
                    if (record_debug)
                      fprintf_unfiltered (gdb_stdlog,
                                          "Process record: error reading "
                                          "memory at addr = 0x%s len = %d.\n",
                                          OUTPUT_REG (vec, tdep->arg2),
                                          tdep->size_iovec);
                    return -1;
                  }
                tmpaddr
                  = (CORE_ADDR) extract_unsigned_integer (iov,
                                                          tdep->size_pointer,
                                                          byte_order);
                tmpint
                  = (int) extract_unsigned_integer (iov + tdep->size_pointer,
                                                    tdep->size_size_t,
                                                    byte_order);
                if (record_arch_list_add_mem (tmpaddr, tmpint))
                  return -1;
                vec += tdep->size_iovec;
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_int))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_timespec))
        return -1;
      break;

      /* sys_mremap */
    case 163:
      /* sys_setresuid16 */
    case 164:
      break;

      /* sys_getresuid16 */
    case 165:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_uid_t))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_uid_t))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_uid_t))
        return -1;
      break;

      /* sys_vm86 */
    case 166:
      /* sys_ni_syscall */
    case 167:
      break;

      /* sys_poll */
    case 168:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST nfds;
          regcache_raw_read_unsigned (regcache, tdep->arg2, &nfds);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_pollfd * nfds))
            return -1;
        }
      break;

      /* sys_nfsservctl */
    case 169:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (tmpulongest == 7 || tmpulongest == 8)
        {
          int rsize;
          if (tmpulongest == 7)
            rsize = tdep->size_NFS_FHSIZE;
          else
            rsize = tdep->size_knfsd_fh;
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, rsize))
            return -1;
        }
      break;

      /* sys_setresgid16 */
    case 170:
      break;

      /* sys_getresgid16 */
    case 171:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_gid_t))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_gid_t))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_old_gid_t))
        return -1;
      break;

      /* sys_prctl */
    case 172:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      switch (tmpulongest)
        {
        case 2:
          regcache_raw_read_unsigned (regcache, tdep->arg2,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_int))
            return -1;
          break;
        case 16:
          regcache_raw_read_unsigned (regcache, tdep->arg2,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_TASK_COMM_LEN))
            return -1;
          break;
        }
      break;

      /* sys_rt_sigreturn */
    case 173:
      break;

      /* sys_rt_sigaction */
    case 174:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_sigaction))
        return -1;
      break;

      /* sys_rt_sigprocmask */
    case 175:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_sigset_t))
        return -1;
      break;

      /* sys_rt_sigpending */
    case 176:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST sigsetsize;
          regcache_raw_read_unsigned (regcache, tdep->arg2,&sigsetsize);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        (int) sigsetsize))
            return -1;
        }
      break;

      /* sys_rt_sigtimedwait */
    case 177:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_siginfo_t))
        return -1;
      break;

      /* sys_rt_sigqueueinfo */
    case 178:
      /* sys_rt_sigsuspend */
    case 179:
      break;

      /* sys_pread64 */
    case 180:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST count;
          regcache_raw_read_unsigned (regcache, tdep->arg3,&count);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, (int) count))
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
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST size;
          regcache_raw_read_unsigned (regcache, tdep->arg2, &size);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, (int) size))
            return -1;
        }
      break;

      /* sys_capget */
    case 184:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_cap_user_data_t))
        return -1;
      break;

      /* sys_capset */
    case 185:
      break;

      /* sys_sigaltstack */
    case 186:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_stack_t))
        return -1;
      break;

      /* sys_sendfile */
    case 187:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_off_t))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_rlimit))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_stat64))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST gidsetsize;
          regcache_raw_read_unsigned (regcache, tdep->arg1,
                                      &gidsetsize);
          tmpint = tdep->size_gid_t * (int) gidsetsize;
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tmpint))
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
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_uid_t))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_uid_t))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_uid_t))
        return -1;
      break;

      /* sys_setresgid */
    case 210:
      break;

      /* sys_getresgid */
    case 211:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_gid_t))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_gid_t))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_gid_t))
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
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_PAGE_SIZE))
        return -1;
      break;

      /* sys_madvise */
    case 219:
      break;

      /* sys_getdents64 */
    case 220:
      {
        ULONGEST count;
        regcache_raw_read_unsigned (regcache, tdep->arg2,
                                    &tmpulongest);
        regcache_raw_read_unsigned (regcache, tdep->arg3, &count);
        if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                      tdep->size_dirent64 * count))
          return -1;
      }
      break;

      /* sys_fcntl64 */
    case 221:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (tmpulongest == tdep->fcntl_F_GETLK64)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_flock64))
            return -1;
        }
      else if (tmpulongest != tdep->fcntl_F_SETLK64
               && tmpulongest != tdep->fcntl_F_SETLKW64)
        {
          goto sys_fcntl;
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
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST size;
          regcache_raw_read_unsigned (regcache, tdep->arg4, &size);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, (int) size))
            return -1;
        }
      break;

      /* sys_listxattr */
    case 232:
      /* sys_llistxattr */
    case 233:
      /* sys_flistxattr */
    case 234:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST size;
          regcache_raw_read_unsigned (regcache, tdep->arg3, &size);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, (int) size))
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
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_loff_t))
        return -1;
      break;

      /* sys_futex */
    case 240:
      /* sys_sched_setaffinity */
    case 241:
      break;

      /* sys_sched_getaffinity */
    case 242:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST len;
          regcache_raw_read_unsigned (regcache, tdep->arg2, &len);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, (int) len))
            return -1;
        }
      break;

      /* sys_set_thread_area */
    case 243:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_int))
        return -1;
      break;

      /* sys_get_thread_area */
    case 244:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_user_desc))
        return -1;
      break;

      /* sys_io_setup */
    case 245:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_long))
        return -1;
      break;

      /* sys_io_destroy */
    case 246:
      break;

      /* sys_io_getevents */
    case 247:
      regcache_raw_read_unsigned (regcache, tdep->arg4, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST nr;
          regcache_raw_read_unsigned (regcache, tdep->arg3, &nr);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        nr * tdep->size_io_event))
            return -1;
        }
      break;

      /* sys_io_submit */
    case 248:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST nr, i;
          gdb_byte *iocbp;

          regcache_raw_read_unsigned (regcache, tdep->arg2, &nr);
          iocbp = alloca (nr * tdep->size_pointer);
          if (target_read_memory ((CORE_ADDR) tmpulongest, iocbp,
                                  nr * tdep->size_pointer))
            {
              if (record_debug)
                fprintf_unfiltered (gdb_stdlog,
                                    "Process record: error reading memory "
                                    "at addr = 0x%s len = %u.\n",
                                    OUTPUT_REG (tmpulongest, tdep->arg2),
                                    (int) (nr * tdep->size_pointer));
              return -1;
            }
          for (i = 0; i < nr; i++)
            {
              tmpaddr
                = (CORE_ADDR) extract_unsigned_integer (iocbp,
                                                        tdep->size_pointer,
                                                        byte_order);
              if (record_arch_list_add_mem (tmpaddr, tdep->size_iocb))
                return -1;
              iocbp += tdep->size_pointer;
            }
        }
      break;

      /* sys_io_cancel */
    case 249:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_io_event))
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
        q = yquery (_("The next instruction is syscall exit_group.  "
                      "It will make the program exit.  "
                      "Do you want to stop the program?"));
        target_terminal_inferior ();
        if (q)
          return 1;
      }
      break;

      /* sys_lookup_dcookie */
    case 253:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST len;
          regcache_raw_read_unsigned (regcache, tdep->arg3, &len);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, (int) len))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST maxevents;
          regcache_raw_read_unsigned (regcache, tdep->arg3, &maxevents);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        maxevents * tdep->size_epoll_event))
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
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_int))
        return -1;
      break;

      /* sys_timer_settime */
    case 260:
      regcache_raw_read_unsigned (regcache, tdep->arg4, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_itimerspec))
        return -1;
      break;

      /* sys_timer_gettime */
    case 261:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_itimerspec))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_timespec))
        return -1;
      break;

      /* sys_clock_getres */
    case 266:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_timespec))
        return -1;
      break;

      /* sys_clock_nanosleep */
    case 267:
      regcache_raw_read_unsigned (regcache, tdep->arg4, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_timespec))
        return -1;
      break;

      /* sys_statfs64 */
    case 268:
      /* sys_fstatfs64 */
    case 269:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_statfs64))
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
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_int))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST maxnode;
          regcache_raw_read_unsigned (regcache, tdep->arg3, &maxnode);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        maxnode * tdep->size_long))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST msg_len;
          regcache_raw_read_unsigned (regcache, tdep->arg3, &msg_len);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        (int) msg_len))
            return -1;
        }
      regcache_raw_read_unsigned (regcache, tdep->arg4, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_int))
        return -1;
      break;

      /* sys_mq_notify */
    case 281:
      break;

      /* sys_mq_getsetattr */
    case 282:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_mq_attr))
        return -1;
      break;

      /* sys_kexec_load */
    case 283:
      break;

      /* sys_waitid */
    case 284:
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_siginfo))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg5, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_rusage))
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
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (tmpulongest == 6 || tmpulongest == 11)
        {
          regcache_raw_read_unsigned (regcache, tdep->arg3,
                                      &tmpulongest);
          if (tmpulongest)
            {
              ULONGEST buflen;
              regcache_raw_read_unsigned (regcache, tdep->arg4, &buflen);
              if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                            (int) buflen))
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
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_stat64))
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
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST bufsiz;
          regcache_raw_read_unsigned (regcache, tdep->arg4, &bufsiz);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, (int) bufsiz))
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
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_fd_set))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_fd_set))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg4, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_fd_set))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg5, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_timespec))
        return -1;
      break;

      /* sys_ppoll */
    case 309:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST nfds;
          regcache_raw_read_unsigned (regcache, tdep->arg2, &nfds);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        tdep->size_pollfd * nfds))
            return -1;
        }
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_timespec))
        return -1;
      break;

      /* sys_unshare */
    case 310:
      /* sys_set_robust_list */
    case 311:
      break;

      /* sys_get_robust_list */
    case 312:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_int))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_int))
        return -1;
      break;

      /* sys_splice */
    case 313:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_loff_t))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg4, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_loff_t))
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
      regcache_raw_read_unsigned (regcache, tdep->arg5, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST nr_pages;
          regcache_raw_read_unsigned (regcache, tdep->arg2, &nr_pages);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        nr_pages * tdep->size_int))
            return -1;
        }
      break;

      /* sys_getcpu */
    case 318:
      regcache_raw_read_unsigned (regcache, tdep->arg1, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_int))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tdep->size_int))
        return -1;
      regcache_raw_read_unsigned (regcache, tdep->arg3, &tmpulongest);
      if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                    tdep->size_ulong * 2))
        return -1;
      break;

      /* sys_epoll_pwait */
    case 319:
      regcache_raw_read_unsigned (regcache, tdep->arg2, &tmpulongest);
      if (tmpulongest)
        {
          ULONGEST maxevents;
          regcache_raw_read_unsigned (regcache, tdep->arg3, &maxevents);
          tmpint = (int) maxevents * tdep->size_epoll_event;
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest, tmpint))
            return -1;
        }
      break;

    default:
      printf_unfiltered (_("Process record and replay target doesn't "
                           "support syscall number %u\n"), num);
      return -1;
      break;
    }

  return 0;
}
