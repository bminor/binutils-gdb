/* Remote target system call support.
   Copyright 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Solutions.

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
   along with GAS; see the file COPYING.  If not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This interface isn't intended to be specific to any particular kind
   of remote (hardware, simulator, whatever).  As such, support for it
   (e.g. sim/common/callback.c) should *not* live in the simulator source
   tree, nor should it live in the gdb source tree.  K&R C must be
   supported.  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "ansidecl.h"
#ifdef ANSI_PROTOTYPES
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include "callback.h"
#include "remote-sim.h"
#include "targ-vals.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* When doing file read/writes, do this many bytes at a time.  */
#define FILE_XFR_SIZE 4096

/* Maximum length of a path name.  */
#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN 1024
#endif

/* Utility of cb_syscall to fetch a path name from the target.
   The result is 0 for success or a target errno value.  */

static int
get_path (cb, sc, buf, buflen, addr)
     host_callback *cb;
     CB_SYSCALL *sc;
     char *buf;
     int buflen;
     long addr;
{
  char *p, *pend;

  for (p = buf, pend = buf + buflen; p < pend; ++p, ++addr)
    {
      /* No, it isn't expected that this would cause one transaction with
	 the remote target for each byte.  The target could send the
	 path name along with the syscall request, and cache the file
	 name somewhere.  */
      unsigned int count = (*sc->read_mem) (cb, sc, addr, p, 1);
				    
      if (count != 1)
	return TARGET_EINVAL;
      if (*p == 0)
	break;
    }
  if (p == pend)
    return TARGET_ENAMETOOLONG;
  return 0;
}

/* Perform a system call on behalf of the target.  */

CB_RC
cb_syscall (cb, sc)
     host_callback *cb;
     CB_SYSCALL *sc;
{
  /* ??? Need to consider target word size.  */
  long result = 0, errcode = 0;

  switch (cb_target_to_host_syscall (cb, sc->func))
    {
#if 0 /* FIXME: wip */
    case CB_SYS_argvlen :
      {
	/* Compute how much space is required to store the argv,envp
	   strings so that the program can allocate the space and then
	   call SYS_argv to fetch the values.  */
	int addr_size = cb->addr_size;
	int argc,envc,arglen,envlen;
	const char **argv = cb->init_argv;
	const char **envp = cb->init_envp;

	argc = arglen = 0;
	if (argv)
	  {
	    for ( ; argv[argc]; ++argc)
	      arglen += strlen (argv[argc]) + 1;
	  }
	envc = envlen = 0;
	if (envp)
	  {
	    for ( ; envp[envc]; ++envc)
	      envlen += strlen (envp[envc]) + 1;
	  }
	result = arglen + envlen;
	break;
      }

    case CB_SYS_argv :
      {
	/* Pointer to target's buffer.  */
	SIM_ADDR tbuf = sc->arg1;
	/* Buffer size.  */
	int bufsize = sc->arg2;
	/* Q is the target address of where all the strings go.  */
	SIM_ADDR q;
	int word_size = cb->word_size;
	int i,argc,envc,len;
	const char **argv = cb->init_argv;
	const char **envp = cb->init_envp;

	argc = 0;
	if (argv)
	  {
	    for ( ; argv[argc]; ++argc)
	      {
		int len = strlen (argv[argc]);
		int written = (*sc->write_mem) (cb, sc, tbuf, argv[argc], len + 1);
		if (written != len)
		  {
		    result = -1;
		    errcode = TARGET_EINVAL;
		    goto FinishSyscall;
		  }
		tbuf = len + 1;
	      }
	  }
	if ((*sc->write_mem) (cb, sc, tbuf, "", 1) != 1)
	  {
	    result = -1;
	    errcode = TARGET_EINVAL;
	    goto FinishSyscall;
	  }
	tbuf++;
	envc = 0;
	if (envp)
	  {
	    for ( ; envp[envc]; ++envc)
	      {
		int len = strlen (envp[envc]);
		int written = (*sc->write_mem) (cb, sc, tbuf, envp[envc], len + 1);
		if (written != len)
		  {
		    result = -1;
		    errcode = TARGET_EINVAL;
		    goto FinishSyscall;
		  }
		tbuf = len + 1;
	      }
	  }
	if ((*sc->write_mem) (cb, sc, tbuf, "", 1) != 1)
	  {
	    result = -1;
	    errcode = TARGET_EINVAL;
	    goto FinishSyscall;
	  }
	result = argc;
	sc->result2 = envc;
	break;
      }
#endif /* wip */

    case CB_SYS_exit :
      /* Caller must catch and handle.  */
      break;

    case CB_SYS_open :
      {
	char path[MAX_PATH_LEN];
	int errcode;

	errcode = get_path (cb, sc, path, MAX_PATH_LEN, sc->arg1);
	if (errcode != 0)
	  {
	    result = -1;
	    errcode = errcode;
	    goto FinishSyscall;
	  }
	result = (*cb->open) (cb, path, sc->arg2 /*, sc->arg3*/);
      }
      break;

    case CB_SYS_close :
      result = (*cb->close) (cb, sc->arg1);
      break;

    case CB_SYS_read :
      {
	/* ??? Perfect handling of error conditions may require only one
	   call to cb->read.  One can't assume all the data is
	   contiguously stored in host memory so that would require
	   malloc'ing/free'ing the space.  Maybe later.  */
	char buf[FILE_XFR_SIZE];
	int fd = sc->arg1;
	SIM_ADDR addr = sc->arg2;
	size_t count = sc->arg3;
	size_t bytes_read = 0;
	int bytes_written;

	while (count > 0)
	  {
	    result = (int) (*cb->read) (cb, fd, buf,
					count < FILE_XFR_SIZE ? count : FILE_XFR_SIZE);
	    if (result == -1)
	      goto FinishSyscall;
	    bytes_written = (*sc->write_mem) (cb, sc, addr, buf, result);
	    if (bytes_written != result)
	      {
		result = -1;
		errcode = TARGET_EINVAL;
		goto FinishSyscall;
	      }
	    bytes_read += result;
	    count -= result;
	    addr += result;
	  }
	result = bytes_read;
      }
      break;

    case CB_SYS_write :
      {
	/* ??? Perfect handling of error conditions may require only one
	   call to cb->write.  One can't assume all the data is
	   contiguously stored in host memory so that would require
	   malloc'ing/free'ing the space.  Maybe later.  */
	char buf[FILE_XFR_SIZE];
	int fd = sc->arg1;
	SIM_ADDR addr = sc->arg2;
	size_t count = sc->arg3;
	int bytes_read;
	size_t bytes_written = 0;

	while (count > 0)
	  {
	    int bytes_to_read = count < FILE_XFR_SIZE ? count : FILE_XFR_SIZE;
	    bytes_read = (*sc->read_mem) (cb, sc, addr, buf, bytes_to_read);
	    if (bytes_read != bytes_to_read)
	      {
		result = -1;
		errcode = TARGET_EINVAL;
		goto FinishSyscall;
	      }
	    if (fd == 1)
	      result = (int) (*cb->write_stdout) (cb, buf, bytes_read);
	    else
	      result = (int) (*cb->write) (cb, fd, buf, bytes_read);
	    if (result == -1)
	      goto FinishSyscall;
	    bytes_written += result;
	    count -= result;
	    addr += result;
	  }
	result = bytes_written;
      }
      break;

    default :
      result = -1;
#ifdef TARGET_ENOSYS
      errcode = TARGET_ENOSYS;
#else
      errcode = TARGET_EINVAL;
#endif
      break;
    }

 FinishSyscall:
  sc->result = result;
  if (errcode == 0)
    sc->errcode = (*cb->get_errno) (cb);
  else
    sc->errcode = errcode;

  return CB_RC_OK;
}
