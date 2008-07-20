/* Record and reverse target code for GNU/Linux.

   Copyright (C) 2008 Free Software Foundation, Inc.

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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _LINUX_RECORD_H_
#define _LINUX_RECORD_H_

typedef struct linux_record_tdep_s
{
  /* The size of the type that will be use in system call. */
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

  /* the values of the second argument of system call "sys_ioctl". */
  int ioctl_FIONCLEX;
  int ioctl_FIOCLEX;
  int ioctl_FIONBIO;
  int ioctl_FIOASYNC;
  int ioctl_FIOQSIZE;

  /* The number of the registers that use to be the arguments of system call. */
  int arg1;
  int arg2;
  int arg3;
  int arg4;
  int arg5;
} linux_record_tdep_t;

extern int record_linux_system_call (int num, linux_record_tdep_t * tdep);

#endif /* _LINUX_RECORD_H_ */
