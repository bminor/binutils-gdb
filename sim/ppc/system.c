/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#ifndef _SYSTEM_C_
#define _SYSTEM_C_

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <fcntl.h>

#if (NetBSD >= 199306) /* here NetBSD as that is what we're emulating */
#include <sys/syscall.h> /* FIXME - should not be including this one */
#include <sys/sysctl.h>
#endif

#if (BSD < 199306) /* here BSD as just a bug */
extern int errno;
#endif

#include "cpu.h"
#include "idecode.h"
#include "system.h"


#ifndef STATIC_INLINE_SYSTEM
#define STATIC_INLINE_SYSTEM STATIC_INLINE
#endif


#if (NetBSD >= 199306)
#define SYS(X) ASSERT(call == (SYS_##X))
#else
#define SYS(X)
#endif

#if (NetBSD >= 199306 && PATH_MAX != 1024)
#error "PATH_MAX not 1024"
#elif !defined(PATH_MAX)
#define PATH_MAX 1024
#endif


STATIC_INLINE_SYSTEM char *
read_string(cpu *processor,
	    char *dest,
	    unsigned_word addr,
	    unsigned nr_bytes)
{
  unsigned nr_moved = 0;
  if (addr == 0)
    return NULL;
  while (1) {
    if (vm_data_map_read_buffer(cpu_data_map(processor),
				 &dest[nr_moved],
				 addr + nr_moved,
				 sizeof(dest[nr_moved]))
	!= sizeof(dest[nr_moved]))
      return NULL;
    if (dest[nr_moved] == '\0' || nr_moved >= nr_bytes)
      break;
    nr_moved++;
  }
  dest[nr_moved] = '\0';
  return dest;
}


STATIC_INLINE_SYSTEM void
write_status(cpu *processor,
	     int status)
{
  cpu_registers(processor)->gpr[3] = status;
  if (status < 0)
    cpu_registers(processor)->gpr[0] = errno;
  else
    cpu_registers(processor)->gpr[0] = 0;
}


STATIC_INLINE_SYSTEM void
write_stat(cpu *processor,
	   unsigned_word addr,
	   struct stat buf)
{
  int nr_moved;
  H2T(buf.st_dev);
  H2T(buf.st_ino);
  H2T(buf.st_mode);
  H2T(buf.st_nlink);
  H2T(buf.st_uid);
  H2T(buf.st_gid);
  H2T(buf.st_rdev);
  H2T(buf.st_size);
  H2T(buf.st_atime);
  /* H2T(buf.st_spare1); */
  H2T(buf.st_mtime);
  /* H2T(buf.st_spare2); */
  H2T(buf.st_ctime);
  /* H2T(buf.st_spare3); */
  H2T(buf.st_blksize);
  H2T(buf.st_blocks);
#if (NetBSD >= 199306)
  H2T(buf.st_flags);
  H2T(buf.st_gen);
#endif
  nr_moved = vm_data_map_write_buffer(cpu_data_map(processor),
				      &buf,
				      addr,
				      sizeof(buf),
				      0/*violate_ro*/);
  if (nr_moved != sizeof(buf))
    error("write_stat() write failed\n");
}

  
STATIC_INLINE_SYSTEM void
do_exit(unsigned call,
	cpu *processor,
	unsigned_word cia)
{
  int status = (int)cpu_registers(processor)->gpr[3];
  SYS(exit);
  cpu_halt(processor, cia, was_exited, status);
}


STATIC_INLINE_SYSTEM void
do_read(unsigned call,
	cpu *processor,
	unsigned_word cia)
{
  void *scratch_buffer;
  int d = (int)cpu_registers(processor)->gpr[3];
  unsigned_word buf = cpu_registers(processor)->gpr[4];
  int nbytes = cpu_registers(processor)->gpr[5];
  int status;
  int nr_moved;
  SYS(read);
  
  /* get a tempoary bufer */
  scratch_buffer = zalloc(nbytes);
  
  /* check if buffer exists by reading it */
  nr_moved = vm_data_map_read_buffer(cpu_data_map(processor),
				     scratch_buffer,
				     buf,
				     nbytes);
  if (nr_moved != nbytes)
    error("system_call()read - check on buffer failed\n");
  
  /* read */
#if 0
  if (d == 0) {
    status = fread (scratch_buffer, 1, nbytes, stdin);
    if (status == 0 && ferror (stdin))
      status = -1;
  }
#endif
  status = read (d, scratch_buffer, nbytes);
  
  if (status == -1) {
    cpu_registers(processor)->gpr[0] = errno;
  } else {
    cpu_registers(processor)->gpr[3] = status;
    
    if (status > 0) {
      nr_moved = vm_data_map_write_buffer(cpu_data_map(processor),
					  scratch_buffer,
					  buf,
					  status,
					  0/*violate_ro*/);
      if (nr_moved != status)
	error("system_call()read - write to buffer failed\n");
    }
  }
  
  zfree(scratch_buffer);
}


STATIC_INLINE_SYSTEM void
do_write(unsigned call,
	 cpu *processor,
	 unsigned_word cia)
{
  void *scratch_buffer = NULL;
  int nr_moved;
  int d = (int)cpu_registers(processor)->gpr[3];
  unsigned_word buf = cpu_registers(processor)->gpr[4];
  int nbytes = cpu_registers(processor)->gpr[5];
  int status;
  SYS(write);
  
  /* get a tempoary bufer */
  scratch_buffer = zalloc(nbytes); /* FIXME - nbytes == 0 */
  
  /* copy in */
  nr_moved = vm_data_map_read_buffer(cpu_data_map(processor),
				     scratch_buffer,
				     buf,
				     nbytes);
  if (nr_moved != nbytes) {
    /* FIXME - should handle better */
    error("system_call()write copy failed (nr_moved=%d != nbytes=%d)\n",
	  nr_moved, nbytes);
  }
  
  /* write */
  status = write(d, scratch_buffer, nbytes);
  if (status == -1) {
    cpu_registers(processor)->gpr[0] = errno;
  }
  cpu_registers(processor)->gpr[3] = status;
  
  zfree(scratch_buffer);
}


STATIC_INLINE_SYSTEM void
do_open(unsigned call,
	cpu *processor,
	unsigned_word cia)
{
  unsigned_word path_addr = cpu_registers(processor)->gpr[3];
  char path_buf[PATH_MAX];
  char *path = read_string(processor, path_buf, path_addr, PATH_MAX);
  int flags = (int)cpu_registers(processor)->gpr[4];
  int mode = (int)cpu_registers(processor)->gpr[4];
  SYS(open);
  write_status(processor, open(path, flags, mode));
}


STATIC_INLINE_SYSTEM void
do_close(unsigned call,
	 cpu *processor,
	 unsigned_word cia)
{
  int d = (int)cpu_registers(processor)->gpr[3];
  SYS(close);
  write_status(processor, close(d));
}


STATIC_INLINE_SYSTEM void
do_break(unsigned call,
	 cpu *processor,
	 unsigned_word cia)
     /* just pass this onto the `vm' device */
{
  psim *system = cpu_system(processor);
  const device *vm = psim_device(system, "/vm");
  SYS(break);
  vm->callback->ioctl(vm,
		      system,
		      processor,
		      cia,
		      0, /*ioctl*/
		      NULL); /*ioctl-data*/
}


STATIC_INLINE_SYSTEM void
do_getpid(unsigned call,
	      cpu *processor,
	      unsigned_word cia)
{
  SYS(getpid);
  cpu_registers(processor)->gpr[3] = (int)getpid();
}


STATIC_INLINE_SYSTEM void
do_getuid(unsigned call,
	  cpu *processor,
	  unsigned_word cia)
{
  SYS(getuid);
  cpu_registers(processor)->gpr[3] = (int)getuid();
}


STATIC_INLINE_SYSTEM void
do_geteuid(unsigned call,
	   cpu *processor,
	   unsigned_word cia)
{
  SYS(geteuid);
  cpu_registers(processor)->gpr[3] = (int)geteuid();
}


STATIC_INLINE_SYSTEM void
do_kill(unsigned call,
	cpu *processor,
	unsigned_word cia)
{
  pid_t pid = cpu_registers(processor)->gpr[3];
  int sig = cpu_registers(processor)->gpr[4];
  SYS(kill);
  error("SYS_kill - more to this than just a kill\n");
  cpu_halt(processor, cia, was_signalled, sig);
}


STATIC_INLINE_SYSTEM void
do_sigprocmask(unsigned call,
	       cpu *processor,
	       unsigned_word cia)
{
  natural_word how = cpu_registers(processor)->gpr[3];
  unsigned_word set = cpu_registers(processor)->gpr[4];
  unsigned_word oset = cpu_registers(processor)->gpr[5];
  SYS(sigprocmask);
  TRACE(trace_system, ("SYS_sigprocmask: how=%d, set=0x%x, oset=0x%x\n",
		       how, set, oset));
  cpu_registers(processor)->gpr[3] = 0;
  cpu_registers(processor)->gpr[4] = set;
}


STATIC_INLINE_SYSTEM void
do_ioctl(unsigned call,
	 cpu *processor,
	 unsigned_word cia)
{
  SYS(ioctl);
  TRACE(trace_system, ("SYS_ioctl: d=%d, request=0x%x, argp=0x%x\n",
		       cpu_registers(processor)->gpr[3], cpu_registers(processor)->gpr[4], cpu_registers(processor)->gpr[5]));
  cpu_registers(processor)->gpr[3] = 0;
}


STATIC_INLINE_SYSTEM void
do_umask(unsigned call,
	 cpu *processor,
	 unsigned_word cia)
{
  SYS(umask);
  cpu_registers(processor)->gpr[3] = umask(cpu_registers(processor)->gpr[3]);
}


STATIC_INLINE_SYSTEM void
do_stat(unsigned call,
	cpu *processor,
	unsigned_word cia)
{
  char path_buf[PATH_MAX];
  unsigned_word path_addr = cpu_registers(processor)->gpr[3];
  unsigned_word stat_buf_addr = cpu_registers(processor)->gpr[4];
  char *path = read_string(processor, path_buf, path_addr, PATH_MAX);
  struct stat buf;
  SYS(stat);
  write_status(processor, stat(path, &buf));
  write_stat(processor, stat_buf_addr, buf);
}


STATIC_INLINE_SYSTEM void
do_fstat(unsigned call,
	 cpu *processor,
	 unsigned_word cia)
{
  int fd = cpu_registers(processor)->gpr[3];
  unsigned_word stat_buf_addr = cpu_registers(processor)->gpr[4];
  struct stat buf;
  SYS(fstat);
  write_status(processor, fstat(fd, &buf));
  write_stat(processor, stat_buf_addr, buf);
}


STATIC_INLINE_SYSTEM void
do_lstat(unsigned call,
	 cpu *processor,
	 unsigned_word cia)
{
  char path_buf[PATH_MAX];
  unsigned_word path_addr = cpu_registers(processor)->gpr[3];
  char *path = read_string(processor, path_buf, path_addr, PATH_MAX);
  unsigned_word stat_buf_addr = cpu_registers(processor)->gpr[4];
  struct stat buf;
  SYS(lstat);
  write_status(processor, stat(path, &buf));
  write_stat(processor, stat_buf_addr, buf);
}


STATIC_INLINE_SYSTEM void
do___sysctl(unsigned call,
	   cpu *processor,
	   unsigned_word cia)
{
  /* call the arguments by their real name */
  unsigned_word name = cpu_registers(processor)->gpr[3];
  natural_word namelen = cpu_registers(processor)->gpr[4];
  unsigned_word oldp = cpu_registers(processor)->gpr[5];
  unsigned_word oldlenp = cpu_registers(processor)->gpr[6];
  natural_word oldlen;
  natural_word mib;
  natural_word int_val;
  SYS(__sysctl);

  /* pluck out the management information base id */
  if (namelen < 1)
    error("system_call()SYS___sysctl bad name[0]\n");
  mib = vm_data_map_read_word(cpu_data_map(processor),
			      name,
			      processor,
			      cia);
  name += sizeof(mib);
  
  /* see what to do with it ... */
  switch (mib) {
  case 6/*CTL_HW*/:
#if (NetBSD >= 199306) && (CTL_HW != 6)
#  error "CTL_HW"
#endif
    if (namelen < 2)
      error("system_call()SYS___sysctl - CTL_HW - bad name[1]\n");
    mib = vm_data_map_read_word(cpu_data_map(processor),
				name,
				processor,
				cia);
    name += sizeof(mib);
    switch (mib) {
    case 7/*HW_PAGESIZE*/:
#if (NetBSD >= 199306) && (HW_PAGESIZE != 7)
#  error "HW_PAGESIZE"
#endif
      oldlen = vm_data_map_read_word(cpu_data_map(processor),
				     oldlenp,
				     processor,
				     cia);
      if (sizeof(natural_word) > oldlen)
	error("system_call()sysctl - CTL_HW.HW_PAGESIZE - to small\n");
      int_val = 8192;
      oldlen = sizeof(int_val);
      vm_data_map_write_word(cpu_data_map(processor),
			     oldp,
			     int_val,
			     processor,
			     cia);
      vm_data_map_write_word(cpu_data_map(processor),
			     oldlenp,
			     oldlen,
			     processor,
			     cia);
      break;
    default:
      error("sysctl() CTL_HW.%d unknown\n", mib);
      break;
    }
    break;
  default:
    error("sysctl() name[0]=%s unknown\n", (int)mib);
    break;
  }
  cpu_registers(processor)->gpr[3] = 0;
}

STATIC_INLINE_SYSTEM void
unimp(unsigned call,
      cpu *processor,
      unsigned_word cia)
{
  error("unimplemented system call %d, cia=0x%x\n", call, cia);
}


typedef void (sys_handler)
     (unsigned call,
      cpu *processor,
      unsigned_word cia);

static sys_handler *(handlers[]) = {
  unimp, /* SYS_syscall 0 */
  do_exit, /* 1*/
  unimp, /* SYS_fork 2 */
  do_read, /* 3 */
  do_write, /* 4 */
  do_open, /* 5 */
  do_close, /* 6 */
  unimp, /* SYS_wait4	7 */
  unimp, /* 8 is old creat */
  unimp, /* SYS_link	9 */
  unimp, /* SYS_unlink	10 */
  unimp, /* 11 is obsolete execv */
  unimp, /* SYS_chdir	12 */
  unimp, /* SYS_fchdir	13 */
  unimp, /* SYS_mknod	14 */
  unimp, /* SYS_chmod	15 */
  unimp, /* SYS_chown	16 */
  do_break, /* 17 */
  unimp, /* SYS_getfsstat	18 */
  unimp, /* 19 is old lseek */
  do_getpid, /* 20 */
  unimp, /* SYS_mount	21 */
  unimp, /* SYS_unmount	22 */
  unimp, /* SYS_setuid	23 */
  do_getuid, /* 24 */
  do_geteuid, /* 25 */
  unimp, /* SYS_ptrace	26 */
  unimp, /* SYS_recvmsg	27 */
  unimp, /* SYS_sendmsg	28 */
  unimp, /* SYS_recvfrom	29 */
  unimp, /* SYS_accept	30 */
  unimp, /* SYS_getpeername	31 */
  unimp, /* SYS_getsockname	32 */
  unimp, /* SYS_access	33 */
  unimp, /* SYS_chflags	34 */
  unimp, /* SYS_fchflags	35 */
  unimp, /* SYS_sync	36 */
  do_kill, /* 37 */
  unimp, /* 38 is old stat */
  unimp, /* SYS_getppid	39 */
  unimp, /* 40 is old lstat */
  unimp, /* SYS_dup	41 */
  unimp, /* SYS_pipe	42 */
  unimp, /* SYS_getegid	43 */
  unimp, /* SYS_profil	44 */
  unimp, /* SYS_ktrace	45 */
  unimp, /* SYS_sigaction	46 */
  unimp, /* SYS_getgid	47 */
  do_sigprocmask, /* 48 */
  unimp, /* SYS_getlogin	49 */
  unimp, /* SYS_setlogin	50 */
  unimp, /* SYS_acct	51 */
  unimp, /* SYS_sigpending	52 */
  unimp, /* SYS_sigaltstack	53 */
  do_ioctl, /* 54 */
  unimp, /* SYS_reboot	55 */
  unimp, /* SYS_revoke	56 */
  unimp, /* SYS_symlink	57 */
  unimp, /* SYS_readlink	58 */
  unimp, /* SYS_execve	59 */
  do_umask, /* 60 */
  unimp, /* SYS_chroot	61 */
  unimp, /* 62 is old fstat */
  unimp, /* 63 is old getkerninfo */
  unimp, /* 64 is old getpagesize */
  unimp, /* SYS_msync	65 */
  unimp, /* SYS_vfork	66 */
  unimp, /* 67 is obsolete vread */
  unimp, /* 68 is obsolete vwrite */
  unimp, /* SYS_sbrk	69 */
  unimp, /* SYS_sstk	70 */
  unimp, /* 71 is old mmap */
  unimp, /* SYS_vadvise	72 */
  unimp, /* SYS_munmap	73 */
  unimp, /* SYS_mprotect	74 */
  unimp, /* SYS_madvise	75 */
  unimp, /* 76 is obsolete vhangup */
  unimp, /* 77 is obsolete vlimit */
  unimp, /* SYS_mincore	78 */
  unimp, /* SYS_getgroups	79 */
  unimp, /* SYS_setgroups	80 */
  unimp, /* SYS_getpgrp	81 */
  unimp, /* SYS_setpgid	82 */
  unimp, /* SYS_setitimer	83 */
  unimp, /* 84 is old wait */
  unimp, /* SYS_swapon	85 */
  unimp, /* SYS_getitimer	86 */
  unimp, /* 87 is old gethostname */
  unimp, /* 88 is old sethostname */
  unimp, /* 89 is old getdtablesize */
  unimp, /* SYS_dup2	90 */
  unimp, /* 91 */
  unimp, /* SYS_fcntl	92 */
  unimp, /* SYS_select	93 */
  unimp, /* 94 */
  unimp, /* SYS_fsync	95 */
  unimp, /* SYS_setpriority	96 */
  unimp, /* SYS_socket	97 */
  unimp, /* SYS_connect	98 */
  unimp, /* 99 is old accept */
  unimp, /* SYS_getpriority	100 */
  unimp, /* 101 is old send */
  unimp, /* 102 is old recv */
  unimp, /* SYS_sigreturn	103 */
  unimp, /* SYS_bind	104 */
  unimp, /* SYS_setsockopt	105 */
  unimp, /* SYS_listen	106 */
  unimp, /* 107 is obsolete vtimes */
  unimp, /* 108 is old sigvec */
  unimp, /* 109 is old sigblock */
  unimp, /* 110 is old sigsetmask */
  unimp, /* SYS_sigsuspend	111 */
  unimp, /* 112 is old sigstack */
  unimp, /* 113 is old recvmsg */
  unimp, /* 114 is old sendmsg */
  unimp, /* SYS_vtrace	115 - is obsolete vtrace */
  unimp, /* SYS_gettimeofday	116 */
  unimp, /* SYS_getrusage	117 */
  unimp, /* SYS_getsockopt	118 */
  unimp, /* SYS_resuba	119 */
  unimp, /* SYS_readv	120 */
  unimp, /* SYS_writev	121 */
  unimp, /* SYS_settimeofday	122 */
  unimp, /* SYS_fchown	123 */
  unimp, /* SYS_fchmod	124 */
  unimp, /* 125 is old recvfrom */
  unimp, /* 126 is old setreuid */
  unimp, /* 127 is old setregid */
  unimp, /* SYS_rename	128 */
  unimp, /* 129 is old truncate */
  unimp, /* 130 is old ftruncate */
  unimp, /* SYS_flock	131 */
  unimp, /* SYS_mkfifo	132 */
  unimp, /* SYS_sendto	133 */
  unimp, /* SYS_shutdown	134 */
  unimp, /* SYS_socketpair	135 */
  unimp, /* SYS_mkdir	136 */
  unimp, /* SYS_rmdir	137 */
  unimp, /* SYS_utimes	138 */
  unimp, /* 139 is obsolete 4.2 sigreturn */
  unimp, /* SYS_adjtime	140 */
  unimp, /* 141 is old getpeername */
  unimp, /* 142 is old gethostid */
  unimp, /* 143 is old sethostid */
  unimp, /* 144 is old getrlimit */
  unimp, /* 145 is old setrlimit */
  unimp, /* 146 is old killpg */
  unimp, /* SYS_setsid	147 */
  unimp, /* SYS_quotactl	148 */
  unimp, /* 149 is old quota */
  unimp, /* 150 is old getsockname */
  unimp, /* 151 */
  unimp, /* 152 */
  unimp, /* 153 */
  unimp, /* 154 */
  unimp, /* SYS_nfssvc	155 */
  unimp, /* 156 is old getdirentries */
  unimp, /* SYS_statfs	157 */
  unimp, /* SYS_fstatfs	158 */
  unimp, /* 159 */
  unimp, /* 160 */
  unimp, /* SYS_getfh	161 */
  unimp, /* 162 is old getdomainname */
  unimp, /* 163 is old setdomainname */
  unimp, /* 164 is old uname */
  unimp, /* SYS_sysarch	165 */
  unimp, /* 166 */
  unimp, /* 167 */
  unimp, /* 168 */
  unimp, /* SYS_semsys	169 */
  unimp, /* SYS_msgsys	170 */
  unimp, /* SYS_shmsys	171 */
  unimp, /* 172 */
  unimp, /* 173 */
  unimp, /* 174 */
  unimp, /* 175 */
  unimp, /* 176 */
  unimp, /* 177 */
  unimp, /* 178 */
  unimp, /* 179 */
  unimp, /* 180 */
  unimp, /* SYS_setgid	181 */
  unimp, /* SYS_setegid	182 */
  unimp, /* SYS_seteuid	183 */
  unimp, /* SYS_lfs_bmapv	184 */
  unimp, /* SYS_lfs_markv	185 */
  unimp, /* SYS_lfs_segclean	186 */
  unimp, /* SYS_lfs_segwait	187 */
  do_stat, /* 188 */
  do_fstat, /* 189 */
  do_lstat, /* 190 */
  unimp, /* SYS_pathconf	191 */
  unimp, /* SYS_fpathconf	192 */
  unimp, /* 193 */
  unimp, /* SYS_getrlimit	194 */
  unimp, /* SYS_setrlimit	195 */
  unimp, /* SYS_getdirentries	196 */
  unimp, /* SYS_mmap	197 */
  unimp, /* SYS___syscall	198 */
  unimp, /* SYS_lseek	199 */
  unimp, /* SYS_truncate	200 */
  unimp, /* SYS_ftruncate	201 */
  do___sysctl, /* 202 */
  unimp, /* SYS_mlock	203 */
  unimp, /* SYS_munlock 204 */
};
    
INLINE_SYSTEM void
system_call(cpu *processor,
	    unsigned_word cia)
{
  unsigned call = cpu_registers(processor)->gpr[0];
  if (call >= sizeof(handlers)/sizeof(handlers[0]))
    error("system call %d out-of-range\n", call);
  cpu_registers(processor)->gpr[0] = 0; /* default success */
  handlers[call](call, processor, cia);
}

#endif /* _SYSTEM_C_ */
