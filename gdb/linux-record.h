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

#ifndef _LINUX_RECORD_H_
#define _LINUX_RECORD_H_

struct linux_record_tdep
{
  /* The size of the type that will be used in a system call.  */
  int size_pointer;
  int size__old_kernel_stat;
  int size_tms;
  int size_loff_t;
  int size_flock;
  int size_oldold_utsname;
  int size_ustat;
  int size_old_sigaction;
  int size_old_sigset_t;
  int size_rlimit;
  int size_rusage;
  int size_timeval;
  int size_timezone;
  int size_old_gid_t;
  int size_old_uid_t;
  int size_fd_set;
  int size_dirent;
  int size_dirent64;
  int size_statfs;
  int size_statfs64;
  int size_sockaddr;
  int size_int;
  int size_long;
  int size_ulong;
  int size_msghdr;
  int size_itimerval;
  int size_stat;
  int size_old_utsname;
  int size_sysinfo;
  int size_msqid_ds;
  int size_shmid_ds;
  int size_new_utsname;
  int size_timex;
  int size_mem_dqinfo;
  int size_if_dqblk;
  int size_fs_quota_stat;
  int size_timespec;
  int size_pollfd;
  int size_NFS_FHSIZE;
  int size_knfsd_fh;
  int size_TASK_COMM_LEN;
  int size_sigaction;
  int size_sigset_t;
  int size_siginfo_t;
  int size_cap_user_data_t;
  int size_stack_t;
  int size_off_t;
  int size_stat64;
  int size_gid_t;
  int size_uid_t;
  int size_PAGE_SIZE;
  int size_flock64;
  int size_user_desc;
  int size_io_event;
  int size_iocb;
  int size_epoll_event;
  int size_itimerspec;
  int size_mq_attr;
  int size_siginfo;
  int size_termios;
  int size_termios2;
  int size_pid_t;
  int size_winsize;
  int size_serial_struct;
  int size_serial_icounter_struct;
  int size_hayes_esp_config;
  int size_size_t;
  int size_iovec;

  /* The values of the second argument of system call "sys_ioctl".  */
  int ioctl_TCGETS;
  int ioctl_TCSETS;
  int ioctl_TCSETSW;
  int ioctl_TCSETSF;
  int ioctl_TCGETA;
  int ioctl_TCSETA;
  int ioctl_TCSETAW;
  int ioctl_TCSETAF;
  int ioctl_TCSBRK;
  int ioctl_TCXONC;
  int ioctl_TCFLSH;
  int ioctl_TIOCEXCL;
  int ioctl_TIOCNXCL;
  int ioctl_TIOCSCTTY;
  int ioctl_TIOCGPGRP;
  int ioctl_TIOCSPGRP;
  int ioctl_TIOCOUTQ;
  int ioctl_TIOCSTI;
  int ioctl_TIOCGWINSZ;
  int ioctl_TIOCSWINSZ;
  int ioctl_TIOCMGET;
  int ioctl_TIOCMBIS;
  int ioctl_TIOCMBIC;
  int ioctl_TIOCMSET;
  int ioctl_TIOCGSOFTCAR;
  int ioctl_TIOCSSOFTCAR;
  int ioctl_FIONREAD;
  int ioctl_TIOCINQ;
  int ioctl_TIOCLINUX;
  int ioctl_TIOCCONS;
  int ioctl_TIOCGSERIAL;
  int ioctl_TIOCSSERIAL;
  int ioctl_TIOCPKT;
  int ioctl_FIONBIO;
  int ioctl_TIOCNOTTY;
  int ioctl_TIOCSETD;
  int ioctl_TIOCGETD;
  int ioctl_TCSBRKP;
  int ioctl_TIOCTTYGSTRUCT;
  int ioctl_TIOCSBRK;
  int ioctl_TIOCCBRK;
  int ioctl_TIOCGSID;
  int ioctl_TCGETS2;
  int ioctl_TCSETS2;
  int ioctl_TCSETSW2;
  int ioctl_TCSETSF2;
  int ioctl_TIOCGPTN;
  int ioctl_TIOCSPTLCK;
  int ioctl_FIONCLEX;
  int ioctl_FIOCLEX;
  int ioctl_FIOASYNC;
  int ioctl_TIOCSERCONFIG;
  int ioctl_TIOCSERGWILD;
  int ioctl_TIOCSERSWILD;
  int ioctl_TIOCGLCKTRMIOS;
  int ioctl_TIOCSLCKTRMIOS;
  int ioctl_TIOCSERGSTRUCT;
  int ioctl_TIOCSERGETLSR;
  int ioctl_TIOCSERGETMULTI;
  int ioctl_TIOCSERSETMULTI;
  int ioctl_TIOCMIWAIT;
  int ioctl_TIOCGICOUNT;
  int ioctl_TIOCGHAYESESP;
  int ioctl_TIOCSHAYESESP;
  int ioctl_FIOQSIZE;

  /* The values of the second argument of system call "sys_fcntl"
     and "sys_fcntl64".  */
  int fcntl_F_GETLK;
  int fcntl_F_GETLK64;
  int fcntl_F_SETLK64;
  int fcntl_F_SETLKW64;

  /* The number of the registers that are used as the arguments of
     a system call.  */
  int arg1;
  int arg2;
  int arg3;
  int arg4;
  int arg5;
  int arg6;
};

extern int record_linux_system_call (int num, struct regcache *regcache,
				     struct linux_record_tdep *tdep);

#endif /* _LINUX_RECORD_H_ */
