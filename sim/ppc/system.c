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


void
system_call(cpu *processor,
	    unsigned_word cia)
{
  switch (cpu_registers(processor)->gpr[0]) {
    

  case 1/*SYS_exit*/:
#if (NetBSD >= 199306) && (SYS_exit != 1)
#  error "SYS_exit"
#endif
    {
      int status = (int)cpu_registers(processor)->gpr[3];
      cpu_halt(processor, cia, was_exited, status);
      break;
    }
    

  case 3/*SYS_read*/:
#if (NetBSD >= 199306) && (SYS_read != 3)
#  error "SYS_read"
#endif
    {
      void *scratch_buffer;
      int d = (int)cpu_registers(processor)->gpr[3];
      unsigned_word buf = cpu_registers(processor)->gpr[4];
      int nbytes = cpu_registers(processor)->gpr[5];
      int status;
      int nr_moved;

      /* get a tempoary bufer */
      scratch_buffer = zalloc(cpu_registers(processor)->gpr[5]);

      /* check if buffer exists by reading it */
      nr_moved = vm_data_map_read_buffer(cpu_data_map(processor),
					 scratch_buffer,
					 buf,
					 nbytes,
					 raw_transfer);
      if (nr_moved != nbytes)
	error("system_call()read - check on buffer failed\n");
      
      /* read */
      if (d == 0) {
	status = fread (scratch_buffer, 1, nbytes, stdin);
	if (status == 0 && ferror (stdin))
	  status = -1;
      } else {
	status = read (d, scratch_buffer, nbytes);
      }

      if (status == -1) {
	cpu_registers(processor)->gpr[0] = errno;
	break;
      } else {
	cpu_registers(processor)->gpr[0] = 0;
	cpu_registers(processor)->gpr[3] = status;

	if (status > 0) {
	  nr_moved = vm_data_map_write_buffer(cpu_data_map(processor),
					      scratch_buffer,
					      buf,
					      status,
					      raw_transfer,
					      0/*violate_ro*/);

	  if (nr_moved != nbytes)
	    error("system_call()read - write to buffer failed\n");
	}
      }
      
      zfree(scratch_buffer);
      
      break;
    }
    

  case 4/*SYS_write*/:
#if (NetBSD >= 199306) && (SYS_write != 4)
#  error "SYS_write"
#endif
    {
      void *scratch_buffer;
      int nr_moved;
      int d = (int)cpu_registers(processor)->gpr[3];
      unsigned_word buf = cpu_registers(processor)->gpr[4];
      int nbytes = cpu_registers(processor)->gpr[5];
      int status;
      
      /* get a tempoary bufer */
      scratch_buffer = zalloc(cpu_registers(processor)->gpr[5]);
      
      /* copy in */
      nr_moved = vm_data_map_read_buffer(cpu_data_map(processor),
					 scratch_buffer,
					 buf,
					 nbytes,
					 raw_transfer);
      if (nr_moved != nbytes) {
	/* FIXME - should handle better */
	error("system_call()write copy failed (nr_moved=%d != nbytes=%d)\n",
	      nr_moved, nbytes);
      }
      
      /* write */
      status = write(d, scratch_buffer, nbytes);
      if (status == -1) {
	cpu_registers(processor)->gpr[0] = errno;
	break;
      }
      cpu_registers(processor)->gpr[0] = 0;
      cpu_registers(processor)->gpr[3] = status;
      
      zfree(scratch_buffer);
      
      break;
    }
    

  case 17/*SYS_break*/:
#if (NetBSD >= 199306) && (SYS_break != 17)
#  error "SYS_break"
#endif
    {
      /* pretend to extend the heap so that it reaches addresss
         new_break while in truth, if growth is needed grow it by a
         page aligned amount */
      unsigned_word new_break = ALIGN_8(cpu_registers(processor)->gpr[3]);
      unsigned_word old_break = core_data_upper_bound(cpu_core(processor));
      signed_word delta = new_break - old_break;
      if (delta > 0)
	core_add_data(cpu_core(processor),
		      ALIGN_PAGE(new_break) - old_break);
      cpu_registers(processor)->gpr[0] = 0;
      cpu_registers(processor)->gpr[3] = new_break;
      break;
    }


  case 20/*SYS_getpid*/:
#if (NetBSD >= 199306) && (SYS_getpid != 20)
#  error "SYS_getpid"
#endif
    {
      cpu_registers(processor)->gpr[3] = (int)getpid();
      break;
    }
    

  case 37/*SYS_kill*/:
#if (NetBSD >= 199306) && (SYS_kill != 37)
#  error "SYS_kill"
#endif
    {
      pid_t pid = cpu_registers(processor)->gpr[3];
      int sig = cpu_registers(processor)->gpr[4];
      TRACE(trace_tbd, ("SYS_kill - more to this than just a kill\n"));
      cpu_halt(processor, cia, was_signalled, sig);
      break;
    }
    

  case 48/*SYS_sigprocmask*/:
#if (NetBSD >= 199306) && (SYS_sigprocmask != 48)
#  error "SYS_sigprocmask"
#endif
    {
      natural_word how = cpu_registers(processor)->gpr[3];
      unsigned_word set = cpu_registers(processor)->gpr[4];
      unsigned_word oset = cpu_registers(processor)->gpr[5];
      TRACE(trace_system, ("SYS_sigprocmask: how=%d, set=0x%x, oset=0x%x\n",
			   how, set, oset));
      cpu_registers(processor)->gpr[0] = 0;
      cpu_registers(processor)->gpr[3] = 0;
      cpu_registers(processor)->gpr[4] = set;
      break;
    }


  case 54/*SYS_ioctl*/:
#if (NetBSD >= 199306) && (SYS_ioctl != 54)
#  error "SYS_ioctl"
#endif
    {
      TRACE(trace_system, ("SYS_ioctl: d=%d, request=0x%x, argp=0x%x\n",
			   cpu_registers(processor)->gpr[3], cpu_registers(processor)->gpr[4], cpu_registers(processor)->gpr[5]));
      cpu_registers(processor)->gpr[0] = 0;
      cpu_registers(processor)->gpr[3] = 0;
      break;
    }


  case 189/*SYS_fstat*/:
#if (NetBSD >= 199306) && (SYS_fstat != 189)
#  error "SYS_fstat"
#endif
    {
      int fd = cpu_registers(processor)->gpr[3];
      unsigned_word stat_buf_addr = cpu_registers(processor)->gpr[4];
      struct stat buf;
      int nr_moved;
      int status;

      /* check buffer all there, by reading it */
      nr_moved = vm_data_map_read_buffer(cpu_data_map(processor),
					 &buf,
					 stat_buf_addr,
					 sizeof(buf),
					 raw_transfer);
      if (nr_moved != sizeof(buf))
	error("system_call()fstat - check on buffer failed\n");

      /* do the fstat call */
      status = fstat(fd, &buf);
      if (status == -1) {
	cpu_registers(processor)->gpr[0] = errno;
	break;
      }
      cpu_registers(processor)->gpr[0] = 0;
      cpu_registers(processor)->gpr[3] = 0;

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
					  stat_buf_addr,
					  sizeof(buf),
					  raw_transfer,
					  0/*violate_ro*/);
      break;
    }


  case 202/*SYS___sysctl*/:
#if (NetBSD >= 199306) && (SYS___sysctl != 202)
#  error "SYS__sysctl"
#endif
    {
      /* call the arguments by their real name */
      unsigned_word name = cpu_registers(processor)->gpr[3];
      natural_word namelen = cpu_registers(processor)->gpr[4];
      unsigned_word oldp = cpu_registers(processor)->gpr[5];
      unsigned_word oldlenp = cpu_registers(processor)->gpr[6];
      natural_word oldlen;
      natural_word mib;
      natural_word int_val;

      /* pluck out the management information base id */
      if (namelen < 1
	  || sizeof(mib) != vm_data_map_read_buffer(cpu_data_map(processor),
						    &mib,
						    name,
						    sizeof(mib),
						    cooked_transfer))
	error("system_call()SYS___sysctl bad name[0]\n");
      name += sizeof(mib);

      /* see what to do with it ... */
      switch (mib) {
      case 6/*CTL_HW*/:
#if (NetBSD >= 199306) && (CTL_HW != 6)
#  error "CTL_HW"
#endif
	if (namelen < 2
	    || sizeof(mib) != vm_data_map_read_buffer(cpu_data_map(processor),
						      &mib,
						      name,
						      sizeof(mib),
						      cooked_transfer))
	  error("system_call()SYS___sysctl - CTL_HW - bad name[1]\n");
	name += sizeof(mib);
	switch (mib) {
	case 7/*HW_PAGESIZE*/:
#if (NetBSD >= 199306) && (HW_PAGESIZE != 7)
#  error "HW_PAGESIZE"
#endif
	  if (sizeof(oldlen) != vm_data_map_read_buffer(cpu_data_map(processor),
							&oldlen,
							oldlenp,
							sizeof(oldlen),
							cooked_transfer))
	    error("system_call()sysctl - CTL_HW.HW_PAGESIZE - oldlen read\n");
	  if (sizeof(natural_word) > oldlen)
	    error("system_call()sysctl - CTL_HW.HW_PAGESIZE - to small\n");
	  int_val = 8192;
	  oldlen = sizeof(int_val);
	  if (sizeof(int_val) != vm_data_map_write_buffer(cpu_data_map(processor),
							  &int_val,
							  oldp,
							  sizeof(int_val),
							  cooked_transfer,
							  0/*violate_ro*/))
	    error("system_call()sysctl - CTL_HW.HW_PAGESIZE - int_val\n");
	  if (sizeof(oldlen) != vm_data_map_write_buffer(cpu_data_map(processor),
							 &oldlen,
							 oldlenp,
							 sizeof(oldlen),
							 cooked_transfer,
							 0/*violate_ro*/))
	    error("system_call()sysctl - CTL_HW.HW_PAGESIZE - oldlen write\n");
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
      cpu_registers(processor)->gpr[0] = 0;
      cpu_registers(processor)->gpr[3] = 0;
      break;
    }


  default:
    error("system_call() unimplemented system call %d, cia=0x%x, arg[0]=0x%x, lr=0x%x\n",
	  cpu_registers(processor)->gpr[0], cia, cpu_registers(processor)->gpr[3], LR);
    break;
    
  }
}

#endif /* _SYSTEM_C_ */
