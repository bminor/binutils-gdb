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


#ifndef _EMUL_NETBSD_C_
#define _EMUL_NETBSD_C_

#include "emul_generic.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/dirent.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#define WITH_NetBSD_HOST (NetBSD >= 199306)
#if WITH_NetBSD_HOST /* here NetBSD as that is what we're emulating */
#include <sys/syscall.h> /* FIXME - should not be including this one */
#include <sys/sysctl.h>
extern int getdirentries(int fd, char *buf, int nbytes, long *basep);
#endif

#if (BSD < 199306) /* here BSD as just a bug */
extern int errno;
#endif

#ifndef STATIC_INLINE_EMUL_NETBSD
#define STATIC_INLINE_EMUL_NETBSD STATIC_INLINE
#endif


#if WITH_NetBSD_HOST
#define SYS(X) ASSERT(call == (SYS_##X))
#else
#define SYS(X)
#endif

#if WITH_NetBSD_HOST && (PATH_MAX != 1024)
#error "PATH_MAX not 1024"
#elif !defined(PATH_MAX)
#define PATH_MAX 1024
#endif


STATIC_INLINE_EMUL_NETBSD void
write_stat(unsigned_word addr,
	   struct stat buf,
	   cpu *processor,
	   unsigned_word cia)
{
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
#if WITH_NetBSD_HOST
  H2T(buf.st_flags);
  H2T(buf.st_gen);
#endif
  emul_write_buffer(&buf, addr, sizeof(buf), processor, cia);
}

  
#if NetBSD
STATIC_INLINE_EMUL_NETBSD void
write_statfs(unsigned_word addr,
	     struct statfs buf,
	     cpu *processor,
	     unsigned_word cia)
{
  H2T(buf.f_type);
  H2T(buf.f_flags);
  H2T(buf.f_bsize);
  H2T(buf.f_iosize);
  H2T(buf.f_blocks);
  H2T(buf.f_bfree);
  H2T(buf.f_bavail);
  H2T(buf.f_files);
  H2T(buf.f_ffree);
  H2T(buf.f_fsid.val[0]);
  H2T(buf.f_fsid.val[1]);
  H2T(buf.f_owner);
  /* f_spare[4]; */
  /* f_fstypename[MFSNAMELEN]; */
  /* f_mntonname[MNAMELEN]; */
  /* f_mntfromname[MNAMELEN]; */
  emul_write_buffer(&buf, addr, sizeof(buf), processor, cia);
}
#endif

  
STATIC_INLINE_EMUL_NETBSD void
write_timeval(unsigned_word addr,
	      struct timeval t,
	      cpu *processor,
	      unsigned_word cia)
{
  H2T(t.tv_sec);
  H2T(t.tv_usec);
  emul_write_buffer(&t, addr, sizeof(t), processor, cia);
}

  
STATIC_INLINE_EMUL_NETBSD void
write_timezone(unsigned_word addr,
	       struct timezone tz,
	       cpu *processor,
	       unsigned_word cia)
{
  H2T(tz.tz_minuteswest);
  H2T(tz.tz_dsttime);
  emul_write_buffer(&tz, addr, sizeof(tz), processor, cia);
}

  
#if WITH_NetBSD_HOST
STATIC_INLINE_EMUL_NETBSD void
write_direntries(unsigned_word addr,
		 char *buf,
		 int nbytes,
		 cpu *processor,
		 unsigned_word cia)
{
  while (nbytes > 0) {
    struct dirent *out;
    struct dirent *in = (struct dirent*)buf;
    ASSERT(in->d_reclen <= nbytes);
    out = (struct dirent*)zalloc(in->d_reclen);
    bcopy(in, out, in->d_reclen);
    H2T(out->d_fileno);
    H2T(out->d_reclen);
    H2T(out->d_type);
    H2T(out->d_namlen);
    emul_write_buffer(out, addr, in->d_reclen, processor, cia);
    nbytes -= in->d_reclen;
    addr += in->d_reclen;
    buf += in->d_reclen;
    zfree(out);
  }
}
#endif


STATIC_INLINE_EMUL_NETBSD void
write_rusage(unsigned_word addr,
	     struct rusage rusage,
	     cpu *processor,
	     unsigned_word cia)
{
  H2T(rusage.ru_utime.tv_sec); /* user time used */
  H2T(rusage.ru_utime.tv_usec);
  H2T(rusage.ru_stime.tv_sec); /* system time used */
  H2T(rusage.ru_stime.tv_usec);
  H2T(rusage.ru_maxrss);          /* integral max resident set size */
  H2T(rusage.ru_ixrss);           /* integral shared text memory size */
  H2T(rusage.ru_idrss);           /* integral unshared data size */
  H2T(rusage.ru_isrss);           /* integral unshared stack size */
  H2T(rusage.ru_minflt);          /* page reclaims */
  H2T(rusage.ru_majflt);          /* page faults */
  H2T(rusage.ru_nswap);           /* swaps */
  H2T(rusage.ru_inblock);         /* block input operations */
  H2T(rusage.ru_oublock);         /* block output operations */
  H2T(rusage.ru_msgsnd);          /* messages sent */
  H2T(rusage.ru_msgrcv);          /* messages received */
  H2T(rusage.ru_nsignals);        /* signals received */
  H2T(rusage.ru_nvcsw);           /* voluntary context switches */
  H2T(rusage.ru_nivcsw);          /* involuntary context switches */
  emul_write_buffer(&rusage, addr, sizeof(rusage), processor, cia);
}

  
STATIC_INLINE_EMUL_NETBSD void
do_exit(emulation *emul,
	unsigned call,
	const int arg0,
	cpu *processor,
	unsigned_word cia)
{
  int status = (int)cpu_registers(processor)->gpr[arg0];
  SYS(exit);
  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%d)\n", status);

  cpu_halt(processor, cia, was_exited, status);
}


STATIC_INLINE_EMUL_NETBSD void
do_read(emulation *emul,
	unsigned call,
	const int arg0,
	cpu *processor,
	unsigned_word cia)
{
  void *scratch_buffer;
  int d = (int)cpu_registers(processor)->gpr[arg0];
  unsigned_word buf = cpu_registers(processor)->gpr[arg0+1];
  int nbytes = cpu_registers(processor)->gpr[arg0+2];
  int status;
  SYS(read);
  
  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%d, 0x%lx, %d", d, (long)buf, nbytes);

  /* get a tempoary bufer */
  scratch_buffer = zalloc(nbytes);
  
  /* check if buffer exists by reading it */
  emul_read_buffer(scratch_buffer, buf, nbytes, processor, cia);
  
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
    
    if (status > 0)
      emul_write_buffer(scratch_buffer, buf, status, processor, cia);
  }
  
  zfree(scratch_buffer);
}


STATIC_INLINE_EMUL_NETBSD void
do_write(emulation *emul,
	 unsigned call,
	 const int arg0,
	 cpu *processor,
	 unsigned_word cia)
{
  void *scratch_buffer = NULL;
  int nr_moved;
  int d = (int)cpu_registers(processor)->gpr[arg0];
  unsigned_word buf = cpu_registers(processor)->gpr[arg0+1];
  int nbytes = cpu_registers(processor)->gpr[arg0+2];
  int status;
  SYS(write);
  
  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%d, 0x%lx, %d", d, (long)buf, nbytes);

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
  emul_write_status(processor, status, errno);
  zfree(scratch_buffer);
}


STATIC_INLINE_EMUL_NETBSD void
do_open(emulation *emul,
	unsigned call,
	const int arg0,
	cpu *processor,
	unsigned_word cia)
{
  unsigned_word path_addr = cpu_registers(processor)->gpr[arg0];
  char path_buf[PATH_MAX];
  char *path = emul_read_string(path_buf, path_addr, PATH_MAX, processor, cia);
  int flags = (int)cpu_registers(processor)->gpr[arg0+1];
  int mode = (int)cpu_registers(processor)->gpr[arg0+2];

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("0x%lx [%s], 0x%x, 0x%x", (long)path_addr, path, flags, mode);

  SYS(open);
  emul_write_status(processor, open(path, flags, mode), errno);
}


STATIC_INLINE_EMUL_NETBSD void
do_close(emulation *emul,
	 unsigned call,
	 const int arg0,
	 cpu *processor,
	 unsigned_word cia)
{
  int d = (int)cpu_registers(processor)->gpr[arg0];

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%d", d);

  SYS(close);
  emul_write_status(processor, close(d), errno);
}


STATIC_INLINE_EMUL_NETBSD void
do_break(emulation *emul,
	 unsigned call,
	 const int arg0,
	 cpu *processor,
	 unsigned_word cia)
     /* just pass this onto the `vm' device */
{
  psim *system = cpu_system(processor);
  const device *vm = psim_device(system, "/vm");

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("0x%lx", (long)cpu_registers(processor)->gpr[arg0]);

  SYS(break);
  vm->callback->ioctl(vm,
		      system,
		      processor,
		      cia,
		      0, /*ioctl*/
		      NULL); /*ioctl-data*/
}


STATIC_INLINE_EMUL_NETBSD void
do_getpid(emulation *emul,
	  unsigned call,
	  const int arg0,
	  cpu *processor,
	  unsigned_word cia)
{
  SYS(getpid);
  cpu_registers(processor)->gpr[3] = (int)getpid();
}


STATIC_INLINE_EMUL_NETBSD void
do_getuid(emulation *emul,
	  unsigned call,
	  const int arg0,
	  cpu *processor,
	  unsigned_word cia)
{
  SYS(getuid);
  cpu_registers(processor)->gpr[3] = (int)getuid();
}


STATIC_INLINE_EMUL_NETBSD void
do_geteuid(emulation *emul,
	   unsigned call,
	   const int arg0,
	   cpu *processor,
	   unsigned_word cia)
{
  SYS(geteuid);
  cpu_registers(processor)->gpr[3] = (int)geteuid();
}


STATIC_INLINE_EMUL_NETBSD void
do_kill(emulation *emul,
	unsigned call,
	const int arg0,
	cpu *processor,
	unsigned_word cia)
{
  pid_t pid = cpu_registers(processor)->gpr[arg0];
  int sig = cpu_registers(processor)->gpr[arg0+1];

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%d, %d", (int)pid, sig);

  SYS(kill);
  printf_filtered("SYS_kill at 0x%x - more to this than just being killed\n",
		  cia);
  cpu_halt(processor, cia, was_signalled, sig);
}


STATIC_INLINE_EMUL_NETBSD void
do_dup(emulation *emul,
       unsigned call,
       const int arg0,
       cpu *processor,
       unsigned_word cia)
{
  int oldd = cpu_registers(processor)->gpr[arg0];
  int status = dup(oldd);

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%d", oldd);

  SYS(dup);
  emul_write_status(processor, status, errno);
}


STATIC_INLINE_EMUL_NETBSD void
do_getegid(emulation *emul,
	   unsigned call,
	   const int arg0,
	   cpu *processor,
	   unsigned_word cia)
{
  SYS(getegid);
  cpu_registers(processor)->gpr[3] = (int)getegid();
}


STATIC_INLINE_EMUL_NETBSD void
do_getgid(emulation *emul,
	  unsigned call,
	  const int arg0,
	  cpu *processor,
	  unsigned_word cia)
{
  SYS(getgid);
  cpu_registers(processor)->gpr[3] = (int)getgid();
}


STATIC_INLINE_EMUL_NETBSD void
do_sigprocmask(emulation *emul,
	       unsigned call,
	       const int arg0,
	       cpu *processor,
	       unsigned_word cia)
{
  natural_word how = cpu_registers(processor)->gpr[arg0];
  unsigned_word set = cpu_registers(processor)->gpr[arg0+1];
  unsigned_word oset = cpu_registers(processor)->gpr[arg0+2];
  SYS(sigprocmask);

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%ld, 0x%ld, 0x%ld", (long)how, (long)set, (long)oset);

  cpu_registers(processor)->gpr[3] = 0;
  cpu_registers(processor)->gpr[4] = set;
}


STATIC_INLINE_EMUL_NETBSD void
do_ioctl(emulation *emul,
	 unsigned call,
	 const int arg0,
	 cpu *processor,
	 unsigned_word cia)
{
  int d = cpu_registers(processor)->gpr[arg0];
  unsigned request = cpu_registers(processor)->gpr[arg0+1];
  unsigned_word argp_addr = cpu_registers(processor)->gpr[arg0+2];

#if !WITH_NetBSD_HOST
  cpu_registers(processor)->gpr[arg0] = 0; /* just succeed */
#else
  unsigned param_len = IOCPARM_LEN(request);
  unsigned basecmd = IOCBASECMD(request);
  unsigned group = IOCGROUP(request);
  unsigned dir = request & IOC_DIRMASK;
  char *argp = NULL;
  int status;
  SYS(ioctl);
  /* what we haven't done */
  if (dir & IOC_IN /* write into the io device */
      || dir & IOC_OUT
      || !(dir & IOC_VOID))
    error("do_ioctl() read or write of parameter not implemented\n");
  status = ioctl(d, request, NULL);
  emul_write_status(processor, status, errno);
#endif

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%d, 0x%x, 0x%lx", d, request, (long)argp_addr);
}


STATIC_INLINE_EMUL_NETBSD void
do_umask(emulation *emul,
	 unsigned call,
	 const int arg0,
	 cpu *processor,
	 unsigned_word cia)
{
  int mask = cpu_registers(processor)->gpr[arg0];

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("0%o", mask);

  SYS(umask);
  cpu_registers(processor)->gpr[3] = umask(mask);
}


STATIC_INLINE_EMUL_NETBSD void
do_dup2(emulation *emul,
	unsigned call,
	const int arg0,
	cpu *processor,
	unsigned_word cia)
{
  int oldd = cpu_registers(processor)->gpr[arg0];
  int newd = cpu_registers(processor)->gpr[arg0+1];
  int status = dup2(oldd, newd);

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%d, %d", oldd, newd);

  SYS(dup2);
  emul_write_status(processor, status, errno);
}


STATIC_INLINE_EMUL_NETBSD void
do_fcntl(emulation *emul,
	 unsigned call,
	 const int arg0,
	 cpu *processor,
	 unsigned_word cia)
{
  int fd = cpu_registers(processor)->gpr[arg0];
  int cmd = cpu_registers(processor)->gpr[arg0+1];
  int arg = cpu_registers(processor)->gpr[arg0+2];
  int status;

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%d, %d, %d", fd, cmd, arg);

  SYS(fcntl);
  status = fcntl(fd, cmd, arg);
  emul_write_status(processor, status, errno);
}


STATIC_INLINE_EMUL_NETBSD void
do_gettimeofday(emulation *emul,
		unsigned call,
		const int arg0,
		cpu *processor,
		unsigned_word cia)
{
  unsigned_word t_addr = cpu_registers(processor)->gpr[arg0];
  unsigned_word tz_addr = cpu_registers(processor)->gpr[arg0+1];
  struct timeval t;
  struct timezone tz;
  int status = gettimeofday((t_addr != 0 ? &t : NULL),
			    (tz_addr != 0 ? &tz : NULL));

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("0x%lx, 0x%lx", (long)t_addr, (long)tz_addr);

  SYS(gettimeofday);
  emul_write_status(processor, status, errno);
  if (status == 0) {
    if (t_addr != 0)
      write_timeval(t_addr, t, processor, cia);
    if (tz_addr != 0)
      write_timezone(tz_addr, tz, processor, cia);
  }
}


STATIC_INLINE_EMUL_NETBSD void
do_getrusage(emulation *emul,
	     unsigned call,
	     const int arg0,
	     cpu *processor,
	     unsigned_word cia)
{
  int who = cpu_registers(processor)->gpr[arg0];
  unsigned_word rusage_addr = cpu_registers(processor)->gpr[arg0+1];
  struct rusage rusage;
  int status = getrusage(who, (rusage_addr != 0 ? &rusage : NULL));

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%d, 0x%lx", who, (long)rusage_addr);

  SYS(getrusage);
  emul_write_status(processor, status, errno);
  if (status == 0) {
    if (rusage_addr != 0)
      write_rusage(rusage_addr, rusage, processor, cia);
  }
}


#if !WITH_NetBSD_HOST
#define do_fstatfs 0
#else
STATIC_INLINE_EMUL_NETBSD void
do_fstatfs(emulation *emul,
	   unsigned call,
	   const int arg0,
	   cpu *processor,
	   unsigned_word cia)
{
  int fd = cpu_registers(processor)->gpr[arg0];
  unsigned_word buf_addr = cpu_registers(processor)->gpr[arg0+1];
  struct statfs buf;
  int status;

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    printf_filtered ("%d, 0x%lx", fd, (long)buf_addr);

  SYS(fstatfs);
  status = fstatfs(fd, (buf_addr == 0 ? NULL : &buf));
  emul_write_status(processor, status, errno);
  if (status == 0) {
    if (buf_addr != 0)
      write_statfs(buf_addr, buf, processor, cia);
  }
}
#endif


STATIC_INLINE_EMUL_NETBSD void
do_stat(emulation *emul,
	unsigned call,
	const int arg0,
	cpu *processor,
	unsigned_word cia)
{
  char path_buf[PATH_MAX];
  unsigned_word path_addr = cpu_registers(processor)->gpr[arg0];
  unsigned_word stat_buf_addr = cpu_registers(processor)->gpr[arg0+1];
  char *path = emul_read_string(path_buf, path_addr, PATH_MAX, processor, cia);
  struct stat buf;
  int status;
  SYS(stat);
  status = stat(path, &buf);
  emul_write_status(processor, status, errno);
  if (status == 0)
    write_stat(stat_buf_addr, buf, processor, cia);
}


STATIC_INLINE_EMUL_NETBSD void
do_fstat(emulation *emul,
	 unsigned call,
	 const int arg0,
	 cpu *processor,
	 unsigned_word cia)
{
  int fd = cpu_registers(processor)->gpr[arg0];
  unsigned_word stat_buf_addr = cpu_registers(processor)->gpr[arg0+1];
  struct stat buf;
  SYS(fstat);
  emul_write_status(processor, fstat(fd, &buf), errno);
  write_stat(stat_buf_addr, buf, processor, cia);
}


STATIC_INLINE_EMUL_NETBSD void
do_lstat(emulation *emul,
	 unsigned call,
	 const int arg0,
	 cpu *processor,
	 unsigned_word cia)
{
  char path_buf[PATH_MAX];
  unsigned_word path_addr = cpu_registers(processor)->gpr[arg0];
  char *path = emul_read_string(path_buf, path_addr, PATH_MAX, processor, cia);
  unsigned_word stat_buf_addr = cpu_registers(processor)->gpr[arg0+1];
  struct stat buf;
  SYS(lstat);
  emul_write_status(processor, stat(path, &buf), errno);
  write_stat(stat_buf_addr, buf, processor, cia);
}


#if !WITH_NetBSD_HOST
#define do_getdirentries 0
#else
STATIC_INLINE_EMUL_NETBSD void
do_getdirentries(emulation *emul,
		 unsigned call,
		 const int arg0,
		 cpu *processor,
		 unsigned_word cia)
{
  int fd = cpu_registers(processor)->gpr[arg0];
  unsigned_word buf_addr = cpu_registers(processor)->gpr[arg0+1];
  char *buf;
  int nbytes = cpu_registers(processor)->gpr[arg0+2];
  unsigned_word basep_addr = cpu_registers(processor)->gpr[arg0+3];
  long basep;
  int status;
  SYS(getdirentries);
  if (buf_addr != 0 && nbytes >= 0)
    buf = zalloc(nbytes);
  else
    buf = NULL;
  status = getdirentries(fd,
			 (buf_addr == 0 ? NULL : buf),
			 nbytes,
			 (basep_addr == 0 ? NULL : &basep));
  emul_write_status(processor, status, errno);
  if (basep_addr != 0)
    emul_write_word(basep_addr, basep, processor, cia);
  if (status > 0)
    write_direntries(buf_addr, buf, status, processor, cia);
  if (buf != NULL)
    zfree(buf);
}
#endif


STATIC_INLINE_EMUL_NETBSD void
do___syscall(emulation *emul,
	     unsigned call,
	     const int arg0,
	     cpu *processor,
	     unsigned_word cia)
{
  SYS(__syscall);
  emul_do_call(emul,
	       cpu_registers(processor)->gpr[arg0],
	       arg0 + 1,
	       processor,
	       cia);
}


STATIC_INLINE_EMUL_NETBSD void
do_lseek(emulation *emul,
	 unsigned call,
	 const int arg0,
	 cpu *processor,
	 unsigned_word cia)
{
  int fildes = cpu_registers(processor)->gpr[arg0];
  off_t offset = emul_read_gpr64(processor, arg0+2);
  int whence = cpu_registers(processor)->gpr[arg0+4];
  off_t status;
  SYS(lseek);
  status = lseek(fildes, offset, whence);
  if (status == -1)
    emul_write_status(processor, -1, errno);
  else {
    emul_write_gpr64(processor, 3, status);
  }
}


STATIC_INLINE_EMUL_NETBSD void
do___sysctl(emulation *emul,
	    unsigned call,
	    const int arg0,
	    cpu *processor,
	    unsigned_word cia)
{
  /* call the arguments by their real name */
  unsigned_word name = cpu_registers(processor)->gpr[arg0];
  natural_word namelen = cpu_registers(processor)->gpr[arg0+1];
  unsigned_word oldp = cpu_registers(processor)->gpr[arg0+2];
  unsigned_word oldlenp = cpu_registers(processor)->gpr[arg0+3];
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
#if WITH_NetBSD_HOST && (CTL_HW != 6)
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
#if WITH_NetBSD_HOST && (HW_PAGESIZE != 7)
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
      emul_write_word(oldp, int_val, processor, cia);
      emul_write_word(oldlenp, oldlen, processor, cia);
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



static emul_call_descriptor netbsd_descriptors[] = {
  /* 0 */ { 0, "syscall", { 0, }, 0 },
  /* 1 */ { do_exit, "exit", { 0, }, 0 },
  /* 2 */ { 0, "fork", { 0, }, 0 },	  
  /* 3 */ { do_read, "read", { 0, }, 0 },
  /* 4 */ { do_write, "write", { 0, }, 0 },
  /* 5 */ { do_open, "open", { 0, }, 0 },
  /* 6 */ { do_close, "close", { 0, }, 0 },
  /* 7 */ { 0, "wait4", { 0, }, 0 },
  { 0, }, /* 8 is old creat */
  /* 9 */ { 0, "link", { 0, }, 0 },
  /* 10 */ { 0, "unlink", { 0, }, 0 },
  { 0, }, /* 11 is obsolete execv */
  /* 12 */ { 0, "chdir", { 0, }, 0 },
  /* 13 */ { 0, "fchdir", { 0, }, 0 },
  /* 14 */ { 0, "mknod", { 0, }, 0 },
  /* 15 */ { 0, "chmod", { 0, }, 0 },
  /* 16 */ { 0, "chown", { 0, }, 0 },
  /* 17 */ { do_break, "break", { 0, }, 0 },
  /* 18 */ { 0, "getfsstat", { 0, }, 0 },
  { 0, }, /* 19 is old lseek */
  /* 20 */ { do_getpid, "getpid", { 0, }, 0 },
  /* 21 */ { 0, "mount", { 0, }, 0 },
  /* 22 */ { 0, "unmount", { 0, }, 0 },
  /* 23 */ { 0, "setuid", { 0, }, 0 },
  /* 24 */ { do_getuid, "getuid", { 0, }, 0 },
  /* 25 */ { do_geteuid, "geteuid", { 0, }, 0 },
  /* 26 */ { 0, "ptrace", { 0, }, 0 },
  /* 27 */ { 0, "recvmsg", { 0, }, 0 },
  /* 28 */ { 0, "sendmsg", { 0, }, 0 },
  /* 29 */ { 0, "recvfrom", { 0, }, 0 },
  /* 30 */ { 0, "accept", { 0, }, 0 },
  /* 31 */ { 0, "getpeername", { 0, }, 0 },
  /* 32 */ { 0, "getsockname", { 0, }, 0 },
  /* 33 */ { 0, "access", { 0, }, 0 },
  /* 34 */ { 0, "chflags", { 0, }, 0 },
  /* 35 */ { 0, "fchflags", { 0, }, 0 },
  /* 36 */ { 0, "sync", { 0, }, 0 },
  /* 37 */ { do_kill, "kill", { 0, }, 0 },
  { 0, }, /* 38 is old stat */
  /* 39 */ { 0, "getppid", { 0, }, 0 },
  { 0, }, /* 40 is old lstat */
  /* 41 */ { do_dup, "dup", { 0, }, 0 },
  /* 42 */ { 0, "pipe", { 0, }, 0 },
  /* 43 */ { do_getegid, "getegid", { 0, }, 0 },
  /* 44 */ { 0, "profil", { 0, }, 0 },
  /* 45 */ { 0, "ktrace", { 0, }, 0 },
  /* 46 */ { 0, "sigaction", { 0, }, 0 },
  /* 47 */ { do_getgid, "getgid", { 0, }, 0 },
  /* 48 */ { do_sigprocmask, "sigprocmask", { 0, }, 0 },
  /* 49 */ { 0, "getlogin", { 0, }, 0 },
  /* 50 */ { 0, "setlogin", { 0, }, 0 },
  /* 51 */ { 0, "acct", { 0, }, 0 },
  /* 52 */ { 0, "sigpending", { 0, }, 0 },
  /* 53 */ { 0, "sigaltstack", { 0, }, 0 },
  /* 54 */ { do_ioctl, "ioctl", { 0, }, 0 },
  /* 55 */ { 0, "reboot", { 0, }, 0 },
  /* 56 */ { 0, "revoke", { 0, }, 0 },
  /* 57 */ { 0, "symlink", { 0, }, 0 },
  /* 58 */ { 0, "readlink", { 0, }, 0 },
  /* 59 */ { 0, "execve", { 0, }, 0 },
  /* 60 */ { do_umask, "umask", { 0, }, 0 },
  /* 61 */ { 0, "chroot", { 0, }, 0 },
  { 0, }, /* 62 is old fstat */
  { 0, }, /* 63 is old getkerninfo */
  { 0, }, /* 64 is old getpagesize */
  /* 65 */ { 0, "msync", { 0, }, 0 },
  /* 66 */ { 0, "vfork", { 0, }, 0 },
  { 0, }, /* 67 is obsolete vread */
  { 0, }, /* 68 is obsolete vwrite */
  /* 69 */ { 0, "sbrk", { 0, }, 0 },
  /* 70 */ { 0, "sstk", { 0, }, 0 },
  { 0, }, /* 71 is old mmap */
  /* 72 */ { 0, "vadvise", { 0, }, 0 },
  /* 73 */ { 0, "munmap", { 0, }, 0 },
  /* 74 */ { 0, "mprotect", { 0, }, 0 },
  /* 75 */ { 0, "madvise", { 0, }, 0 },
  { 0, }, /* 76 is obsolete vhangup */
  { 0, }, /* 77 is obsolete vlimit */
  /* 78 */ { 0, "mincore", { 0, }, 0 },
  /* 79 */ { 0, "getgroups", { 0, }, 0 },
  /* 80 */ { 0, "setgroups", { 0, }, 0 },
  /* 81 */ { 0, "getpgrp", { 0, }, 0 },
  /* 82 */ { 0, "setpgid", { 0, }, 0 },
  /* 83 */ { 0, "setitimer", { 0, }, 0 },
  { 0, }, /* 84 is old wait */
  /* 85 */ { 0, "swapon", { 0, }, 0 },
  /* 86 */ { 0, "getitimer", { 0, }, 0 },
  { 0, }, /* 87 is old gethostname */
  { 0, }, /* 88 is old sethostname */
  { 0, }, /* 89 is old getdtablesize */
  { do_dup2, "dup2", { 0, }, 0 },
  { 0, }, /* 91 */
  /* 92 */ { do_fcntl, "fcntl", { 0, }, 0 },
  /* 93 */ { 0, "select", { 0, }, 0 },
  { 0, }, /* 94 */
  /* 95 */ { 0, "fsync", { 0, }, 0 },
  /* 96 */ { 0, "setpriority", { 0, }, 0 },
  /* 97 */ { 0, "socket", { 0, }, 0 },
  /* 98 */ { 0, "connect", { 0, }, 0 },
  { 0, }, /* 99 is old accept */
  /* 100 */ { 0, "getpriority", { 0, }, 0 },
  { 0, }, /* 101 is old send */
  { 0, }, /* 102 is old recv */
  /* 103 */ { 0, "sigreturn", { 0, }, 0 },
  /* 104 */ { 0, "bind", { 0, }, 0 },
  /* 105 */ { 0, "setsockopt", { 0, }, 0 },
  /* 106 */ { 0, "listen", { 0, }, 0 },
  { 0, }, /* 107 is obsolete vtimes */
  { 0, }, /* 108 is old sigvec */
  { 0, }, /* 109 is old sigblock */
  { 0, }, /* 110 is old sigsetmask */
  /* 111 */ { 0, "sigsuspend", { 0, }, 0 },
  { 0, }, /* 112 is old sigstack */
  { 0, }, /* 113 is old recvmsg */
  { 0, }, /* 114 is old sendmsg */
  /* - is obsolete vtrace */ { 0, "vtrace	115", { 0, }, 0 },
  /* 116 */ { do_gettimeofday, "gettimeofday", { 0, }, 0 },
  /* 117 */ { do_getrusage, "getrusage", { 0, }, 0 },
  /* 118 */ { 0, "getsockopt", { 0, }, 0 },
  /* 119 */ { 0, "resuba", { 0, }, 0 },
  /* 120 */ { 0, "readv", { 0, }, 0 },
  /* 121 */ { 0, "writev", { 0, }, 0 },
  /* 122 */ { 0, "settimeofday", { 0, }, 0 },
  /* 123 */ { 0, "fchown", { 0, }, 0 },
  /* 124 */ { 0, "fchmod", { 0, }, 0 },
  { 0, }, /* 125 is old recvfrom */
  { 0, }, /* 126 is old setreuid */
  { 0, }, /* 127 is old setregid */
  /* 128 */ { 0, "rename", { 0, }, 0 },
  { 0, }, /* 129 is old truncate */
  { 0, }, /* 130 is old ftruncate */
  /* 131 */ { 0, "flock", { 0, }, 0 },
  /* 132 */ { 0, "mkfifo", { 0, }, 0 },
  /* 133 */ { 0, "sendto", { 0, }, 0 },
  /* 134 */ { 0, "shutdown", { 0, }, 0 },
  /* 135 */ { 0, "socketpair", { 0, }, 0 },
  /* 136 */ { 0, "mkdir", { 0, }, 0 },
  /* 137 */ { 0, "rmdir", { 0, }, 0 },
  /* 138 */ { 0, "utimes", { 0, }, 0 },
  { 0, }, /* 139 is obsolete 4.2 sigreturn */
  /* 140 */ { 0, "adjtime", { 0, }, 0 },
  { 0, }, /* 141 is old getpeername */
  { 0, }, /* 142 is old gethostid */
  { 0, }, /* 143 is old sethostid */
  { 0, }, /* 144 is old getrlimit */
  { 0, }, /* 145 is old setrlimit */
  { 0, }, /* 146 is old killpg */
  /* 147 */ { 0, "setsid", { 0, }, 0 },
  /* 148 */ { 0, "quotactl", { 0, }, 0 },
  { 0, }, /* 149 is old quota */
  { 0, }, /* 150 is old getsockname */
  { 0, }, /* 151 */
  { 0, }, /* 152 */
  { 0, }, /* 153 */
  { 0, }, /* 154 */
  /* 155 */ { 0, "nfssvc", { 0, }, 0 },
  { 0, }, /* 156 is old getdirentries */
  /* 157 */ { 0, "statfs", { 0, }, 0 },
  /* 158 */ { do_fstatfs, "fstatfs", { 0, }, 0 },
  { 0, }, /* 159 */
  { 0, }, /* 160 */
  /* 161 */ { 0, "getfh", { 0, }, 0 },
  { 0, }, /* 162 is old getdomainname */
  { 0, }, /* 163 is old setdomainname */
  { 0, }, /* 164 is old uname */
  /* 165 */ { 0, "sysarch", { 0, }, 0 },
  { 0, }, /* 166 */
  { 0, }, /* 167 */
  { 0, }, /* 168 */
  /* 169 */ { 0, "semsys", { 0, }, 0 },
  /* 170 */ { 0, "msgsys", { 0, }, 0 },
  /* 171 */ { 0, "shmsys", { 0, }, 0 },
  { 0, }, /* 172 */
  { 0, }, /* 173 */
  { 0, }, /* 174 */
  { 0, }, /* 175 */
  { 0, }, /* 176 */
  { 0, }, /* 177 */
  { 0, }, /* 178 */
  { 0, }, /* 179 */
  { 0, }, /* 180 */
  /* 181 */ { 0, "setgid", { 0, }, 0 },
  /* 182 */ { 0, "setegid", { 0, }, 0 },
  /* 183 */ { 0, "seteuid", { 0, }, 0 },
  /* 184 */ { 0, "lfs_bmapv", { 0, }, 0 },
  /* 185 */ { 0, "lfs_markv", { 0, }, 0 },
  /* 186 */ { 0, "lfs_segclean", { 0, }, 0 },
  /* 187 */ { 0, "lfs_segwait", { 0, }, 0 },
  /* 188 */ { do_stat, "stat", { 0, }, 0 },
  /* 189 */ { do_fstat, "fstat", { 0, }, 0 },
  /* 190 */ { do_lstat, "lstat", { 0, }, 0 },
  /* 191 */ { 0, "pathconf", { 0, }, 0 },
  /* 192 */ { 0, "fpathconf", { 0, }, 0 },
  { 0, }, /* 193 */
  /* 194 */ { 0, "getrlimit", { 0, }, 0 },
  /* 195 */ { 0, "setrlimit", { 0, }, 0 },
  /* 196 */ { do_getdirentries, "getdirentries", { 0, }, 0 },
  /* 197 */ { 0, "mmap", { 0, }, 0 },
  /* 198 */ { do___syscall, "__syscall", { 0, }, 0 },
  /* 199 */ { do_lseek, "lseek", { 0, }, 0 },
  /* 200 */ { 0, "truncate", { 0, }, 0 },
  /* 201 */ { 0, "ftruncate", { 0, }, 0 },
  /* 202 */ { do___sysctl, "__sysctl", { 0, }, 0 },
  /* 203 */ { 0, "mlock", { 0, }, 0 },
  /* 204 */ { 0, "munlock", { 0, }, 0 },
};
    
static char *(netbsd_error_names[]) = {
  /* 0 */ "ESUCCESS",
  /* 1 */ "EPERM",
  /* 2 */ "ENOENT",
  /* 3 */ "ESRCH",
  /* 4 */ "EINTR",
  /* 5 */ "EIO",
  /* 6 */ "ENXIO",
  /* 7 */ "E2BIG",
  /* 8 */ "ENOEXEC",
  /* 9 */ "EBADF",
  /* 10 */ "ECHILD",
  /* 11 */ "EDEADLK",
  /* 12 */ "ENOMEM",
  /* 13 */ "EACCES",
  /* 14 */ "EFAULT",
  /* 15 */ "ENOTBLK",
  /* 16 */ "EBUSY",
  /* 17 */ "EEXIST",
  /* 18 */ "EXDEV",
  /* 19 */ "ENODEV",
  /* 20 */ "ENOTDIR",
  /* 21 */ "EISDIR",
  /* 22 */ "EINVAL",
  /* 23 */ "ENFILE",
  /* 24 */ "EMFILE",
  /* 25 */ "ENOTTY",
  /* 26 */ "ETXTBSY",
  /* 27 */ "EFBIG",
  /* 28 */ "ENOSPC",
  /* 29 */ "ESPIPE",
  /* 30 */ "EROFS",
  /* 31 */ "EMLINK",
  /* 32 */ "EPIPE",
  /* 33 */ "EDOM",
  /* 34 */ "ERANGE",
  /* 35 */ "EAGAIN",
  /* 36 */ "EINPROGRESS",
  /* 37 */ "EALREADY",
  /* 38 */ "ENOTSOCK",
  /* 39 */ "EDESTADDRREQ",
  /* 40 */ "EMSGSIZE",
  /* 41 */ "EPROTOTYPE",
  /* 42 */ "ENOPROTOOPT",
  /* 43 */ "EPROTONOSUPPORT",
  /* 44 */ "ESOCKTNOSUPPORT",
  /* 45 */ "EOPNOTSUPP",
  /* 46 */ "EPFNOSUPPORT",
  /* 47 */ "EAFNOSUPPORT",
  /* 48 */ "EADDRINUSE",
  /* 49 */ "EADDRNOTAVAIL",
  /* 50 */ "ENETDOWN",
  /* 51 */ "ENETUNREACH",
  /* 52 */ "ENETRESET",
  /* 53 */ "ECONNABORTED",
  /* 54 */ "ECONNRESET",
  /* 55 */ "ENOBUFS",
  /* 56 */ "EISCONN",
  /* 57 */ "ENOTCONN",
  /* 58 */ "ESHUTDOWN",
  /* 59 */ "ETOOMANYREFS",
  /* 60 */ "ETIMEDOUT",
  /* 61 */ "ECONNREFUSED",
  /* 62 */ "ELOOP",
  /* 63 */ "ENAMETOOLONG",
  /* 64 */ "EHOSTDOWN",
  /* 65 */ "EHOSTUNREACH",
  /* 66 */ "ENOTEMPTY",
  /* 67 */ "EPROCLIM",
  /* 68 */ "EUSERS",
  /* 69 */ "EDQUOT",
  /* 70 */ "ESTALE",
  /* 71 */ "EREMOTE",
  /* 72 */ "EBADRPC",
  /* 73 */ "ERPCMISMATCH",
  /* 74 */ "EPROGUNAVAIL",
  /* 75 */ "EPROGMISMATCH",
  /* 76 */ "EPROCUNAVAIL",
  /* 77 */ "ENOLCK",
  /* 78 */ "ENOSYS",
  /* 79 */ "EFTYPE",
  /* 80 */ "EAUTH",
  /* 81 */ "ENEEDAUTH",
  /* 81 */ "ELAST",
};

static char *(netbsd_signal_names[]) = {
  /* 0 */ 0,
  /* 1 */ "SIGHUP",
  /* 2 */ "SIGINT",
  /* 3 */ "SIGQUIT",
  /* 4 */ "SIGILL",
  /* 5 */ "SIGTRAP",
  /* 6 */ "SIGABRT",
  /* 7 */ "SIGEMT",
  /* 8 */ "SIGFPE",
  /* 9 */ "SIGKILL",
  /* 10 */ "SIGBUS",
  /* 11 */ "SIGSEGV",
  /* 12 */ "SIGSYS",
  /* 13 */ "SIGPIPE",
  /* 14 */ "SIGALRM",
  /* 15 */ "SIGTERM",
  /* 16 */ "SIGURG",
  /* 17 */ "SIGSTOP",
  /* 18 */ "SIGTSTP",
  /* 19 */ "SIGCONT",
  /* 20 */ "SIGCHLD",
  /* 21 */ "SIGTTIN",
  /* 22 */ "SIGTTOU",
  /* 23 */ "SIGIO",
  /* 24 */ "SIGXCPU",
  /* 25 */ "SIGXFSZ",
  /* 26 */ "SIGVTALRM",
  /* 27 */ "SIGPROF",
  /* 28 */ "SIGWINCH",
  /* 29 */ "SIGINFO",
  /* 30 */ "SIGUSR1",
  /* 31 */ "SIGUSR2",
};

emulation emul_netbsd = {
  netbsd_descriptors,
  sizeof(netbsd_descriptors) / sizeof(netbsd_descriptors[0]),
  netbsd_error_names,
  sizeof(netbsd_error_names) / sizeof(netbsd_error_names[0]),
  netbsd_signal_names,
  sizeof(netbsd_signal_names) / sizeof(netbsd_signal_names[0]),
};

#endif /* _EMUL_NETBSD_C_ */
