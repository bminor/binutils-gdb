/* Machine independent support for SVR4 /proc (process file system) for GDB.
   Copyright (C) 1991 Free Software Foundation, Inc.
   Written by Fred Fish at Cygnus Support.

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
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


/*			N  O  T  E  S

For information on the details of using /proc consult section proc(4)
in the UNIX System V Release 4 System Administrator's Reference Manual.

The general register and floating point register sets are manipulated by
separate ioctl's.  This file makes the assumption that if FP0_REGNUM is
defined, then support for the floating point register set is desired,
regardless of whether or not the actual target has floating point hardware.

 */


#include <stdio.h>

#include "defs.h"

#ifdef USE_PROC_FS	/* Entire file goes away if not using /proc */

#include <stdio.h>
#include <sys/procfs.h>
#include <fcntl.h>
#include <errno.h>

#include "inferior.h"
#include "target.h"

#ifndef PROC_NAME_FMT
#define PROC_NAME_FMT "/proc/%d"
#endif

#if 1	/* FIXME: Gross and ugly hack to resolve coredep.c global */
CORE_ADDR kernel_u_addr;
#endif

/*  All access to the inferior, either one started by gdb or one that has
    been attached to, is controlled by an instance of a procinfo structure,
    defined below.  Since gdb currently only handles one inferior at a time,
    the procinfo structure for the inferior is statically allocated and
    only one exists at any given time.  There is a separate procinfo
    structure for use by the "info proc" command, so that we can print
    useful information about any random process without interfering with
    the inferior's procinfo information. */

struct procinfo {
  int valid;			/* Nonzero if pid, fd, & pathname are valid */
  int pid;			/* Process ID of inferior */
  int fd;			/* File descriptor for /proc entry */
  char *pathname;		/* Pathname to /proc entry */
  int was_stopped;		/* Nonzero if was stopped prior to attach */
  prrun_t prrun;		/* Control state when it is run */
  prstatus_t prstatus;		/* Current process status info */
  gregset_t gregset;		/* General register set */
  fpregset_t fpregset;		/* Floating point register set */
  fltset_t fltset;		/* Current traced hardware fault set */
  sigset_t trace;		/* Current traced signal set */
  sysset_t exitset;		/* Current traced system call exit set */
  sysset_t entryset;		/* Current traced system call entry set */
};

static struct procinfo pi;	/* Inferior's process information */

/* Prototypes for local functions */

static int
proc_address_to_fd PARAMS ((CORE_ADDR, int));

static int
open_proc_file PARAMS ((int, struct procinfo *));

static void
close_proc_file PARAMS ((struct procinfo *));

static void
unconditionally_kill_inferior PARAMS ((void));

static void
proc_init_failed PARAMS ((char *));

static void
proc_info PARAMS ((char *, int));

static void
proc_info_address_map PARAMS ((struct procinfo *, int));

static char *
mappingflags PARAMS ((long));

/* External function prototypes that can't be easily included in any
   header file because the args are typedefs in system include files. */

extern void
supply_gregset PARAMS ((gregset_t *));

extern void
fill_gregset PARAMS ((gregset_t *, int));

extern void
supply_fpregset PARAMS ((fpregset_t *));

extern void
fill_fpregset PARAMS ((fpregset_t *, int));


/*

GLOBAL FUNCTION

	ptrace -- override library version to force errors for /proc version

SYNOPSIS

	int ptrace (int request, int pid, int arg3, int arg4)

DESCRIPTION

	When gdb is configured to use /proc, it should not be calling
	or otherwise attempting to use ptrace.  In order to catch errors
	where use of /proc is configured, but some routine is still calling
	ptrace, we provide a local version of a function with that name
	that does nothing but issue an error message.
*/

int
ptrace (request, pid, arg3, arg4)
     int request;
     int pid;
     int arg3;
     int arg4;
{
  error ("internal error - there is a call to ptrace() somewhere");
  /*NOTREACHED*/
}

/*

GLOBAL FUNCTION

	kill_inferior_fast -- kill inferior while gdb is exiting

SYNOPSIS

	void kill_inferior_fast (void)

DESCRIPTION

	This is used when GDB is exiting.  It gives less chance of error.

NOTES

	Don't attempt to kill attached inferiors since we may be called
	when gdb is in the process of aborting, and killing the attached
	inferior may be very anti-social.  This is particularly true if we
	were attached just so we could use the /proc facilities to get
	detailed information about it's status.

*/

void
kill_inferior_fast ()
{
  if (inferior_pid != 0 && !attach_flag)
    {
      unconditionally_kill_inferior ();
    }
}

/*

GLOBAL FUNCTION

	kill_inferior - kill any currently inferior

SYNOPSIS

	void kill_inferior (void)

DESCRIPTION

	Kill any current inferior.

NOTES

	Kills even attached inferiors.  Presumably the user has already
	been prompted that the inferior is an attached one rather than
	one started by gdb.  (FIXME?)

*/

void
kill_inferior ()
{
  if (inferior_pid != 0)
    {
      unconditionally_kill_inferior ();
      target_mourn_inferior ();
    }
}

/*

LOCAL FUNCTION

	unconditionally_kill_inferior - terminate the inferior

SYNOPSIS

	static void unconditionally_kill_inferior (void)

DESCRIPTION

	Kill the current inferior.  Should not be called until it
	is at least tested that there is an inferior.

NOTE

	A possibly useful enhancement would be to first try sending
	the inferior a terminate signal, politely asking it to commit
	suicide, before we murder it.

*/

static void
unconditionally_kill_inferior ()
{
  int signo;
  
  signo = SIGKILL;
  (void) ioctl (pi.fd, PIOCKILL, &signo);
  close_proc_file (&pi);
  wait ((int *) 0);
}

/*

GLOBAL FUNCTION

	child_xfer_memory -- copy data to or from inferior memory space

SYNOPSIS

	int child_xfer_memory (CORE_ADDR memaddr, char *myaddr, int len,
		int dowrite, struct target_ops target)

DESCRIPTION

	Copy LEN bytes to/from inferior's memory starting at MEMADDR
	from/to debugger memory starting at MYADDR.  Copy from inferior
	if DOWRITE is zero or to inferior if DOWRITE is nonzero.
  
	Returns the length copied, which is either the LEN argument or
	zero.  This xfer function does not do partial moves, since child_ops
	doesn't allow memory operations to cross below us in the target stack
	anyway.

NOTES

	The /proc interface makes this an almost trivial task.
 */


int
child_xfer_memory (memaddr, myaddr, len, dowrite, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int dowrite;
     struct target_ops *target; /* ignored */
{
  int nbytes = 0;

  if (lseek (pi.fd, (off_t) memaddr, 0) == (off_t) memaddr)
    {
      if (dowrite)
	{
	  nbytes = write (pi.fd, myaddr, len);
	}
      else
	{
	  nbytes = read (pi.fd, myaddr, len);
	}
      if (nbytes < 0)
	{
	  nbytes = 0;
	}
    }
  return (nbytes);
}

/*

GLOBAL FUNCTION

	store_inferior_registers -- copy register values back to inferior

SYNOPSIS

	void store_inferior_registers (int regno)

DESCRIPTION

	Store our current register values back into the inferior.  If
	REGNO is -1 then store all the register, otherwise store just
	the value specified by REGNO.

NOTES

	If we are storing only a single register, we first have to get all
	the current values from the process, overwrite the desired register
	in the gregset with the one we want from gdb's registers, and then
	send the whole set back to the process.  For writing all the
	registers, all we have to do is generate the gregset and send it to
	the process.

	Also note that the process has to be stopped on an event of interest
	for this to work, which basically means that it has to have been
	run under the control of one of the other /proc ioctl calls and not
	ptrace.  Since we don't use ptrace anyway, we don't worry about this
	fine point, but it is worth noting for future reference.

	Gdb is confused about what this function is supposed to return.
	Some versions return a value, others return nothing.  Some are
	declared to return a value and actually return nothing.  Gdb ignores
	anything returned.  (FIXME)

 */

void
store_inferior_registers (regno)
     int regno;
{
  if (regno != -1)
    {
      (void) ioctl (pi.fd, PIOCGREG, &pi.gregset);
    }
  fill_gregset (&pi.gregset, regno);
  (void) ioctl (pi.fd, PIOCSREG, &pi.gregset);

#if defined (FP0_REGNUM)

  /* Now repeat everything using the floating point register set, if the
     target has floating point hardware. Since we ignore the returned value,
     we'll never know whether it worked or not anyway. */

  if (regno != -1)
    {
      (void) ioctl (pi.fd, PIOCGFPREG, &pi.fpregset);
    }
  fill_fpregset (&pi.fpregset, regno);
  (void) ioctl (pi.fd, PIOCSFPREG, &pi.fpregset);

#endif	/* FP0_REGNUM */

}

/*

GLOBAL FUNCTION

	inferior_proc_init - initialize access to a /proc entry

SYNOPSIS

	void inferior_proc_init (int pid)

DESCRIPTION

	When gdb starts an inferior, this function is called in the parent
	process immediately after the fork.  It waits for the child to stop
	on the return from the exec system call (the child itself takes care
	of ensuring that this is set up), then sets up the set of signals
	and faults that are to be traced.

NOTES

	If proc_init_failed ever gets called, control returns to the command
	processing loop via the standard error handling code.
 */

void
inferior_proc_init (pid)
     int pid;
{
  if (!open_proc_file (pid, &pi))
    {
      proc_init_failed ("can't open process file");
    }
  else
    {
      (void) memset (&pi.prrun, 0, sizeof (pi.prrun));
      prfillset (&pi.prrun.pr_trace);
      prfillset (&pi.prrun.pr_fault);
      prdelset (&pi.prrun.pr_fault, FLTPAGE);
      if (ioctl (pi.fd, PIOCWSTOP, &pi.prstatus) < 0)
	{
	  proc_init_failed ("PIOCWSTOP failed");
	}
      else if (ioctl (pi.fd, PIOCSTRACE, &pi.prrun.pr_trace) < 0)
	{
	  proc_init_failed ("PIOCSTRACE failed");
	}
      else if (ioctl (pi.fd, PIOCSFAULT, &pi.prrun.pr_fault) < 0)
	{
	  proc_init_failed ("PIOCSFAULT failed");
	}
    }
}

/*

GLOBAL FUNCTION

	proc_set_exec_trap -- arrange for exec'd child to halt at startup

SYNOPSIS

	void proc_set_exec_trap (void)

DESCRIPTION

	This function is called in the child process when starting up
	an inferior, prior to doing the exec of the actual inferior.
	It sets the child process's exitset to make exit from the exec
	system call an event of interest to stop on, and then simply
	returns.  The child does the exec, the system call returns, and
	the child stops at the first instruction, ready for the gdb
	parent process to take control of it.

NOTE

	We need to use all local variables since the child may be sharing
	it's data space with the parent, if vfork was used rather than
	fork.
 */

void
proc_set_exec_trap ()
{
  sysset_t exitset;
  auto char procname[32];
  int fd;
  
  (void) sprintf (procname, PROC_NAME_FMT, getpid ());
  if ((fd = open (procname, O_RDWR)) < 0)
    {
      perror (procname);
      fflush (stderr);
      _exit (127);
    }
  premptyset (&exitset);
  praddset (&exitset, SYS_exec);
  praddset (&exitset, SYS_execve);
  if (ioctl (fd, PIOCSEXIT, &exitset) < 0)
    {
      perror (procname);
      fflush (stderr);
      _exit (127);
    }
}

/*

GLOBAL FUNCTION

	proc_iterate_over_mappings -- call function for every mapped space

SYNOPSIS

	int proc_iterate_over_mappings (int (*func)())

DESCRIPTION

	Given a pointer to a function, call that function for every
	mapped address space, passing it an open file descriptor for
	the file corresponding to that mapped address space (if any)
	and the base address of the mapped space.  Quit when we hit
	the end of the mappings or the function returns nonzero.
 */

int
proc_iterate_over_mappings (func)
     int (*func) PARAMS ((int, CORE_ADDR));
{
  int nmap;
  int fd;
  int funcstat = 0;
  struct prmap *prmaps;
  struct prmap *prmap;
  CORE_ADDR baseaddr = 0;

  if (pi.valid && (ioctl (pi.fd, PIOCNMAP, &nmap) == 0))
    {
      prmaps = (struct prmap *) alloca ((nmap + 1) * sizeof (*prmaps));
      if (ioctl (pi.fd, PIOCMAP, prmaps) == 0)
	{
	  for (prmap = prmaps; prmap -> pr_size && funcstat == 0; ++prmap)
	    {
	      fd = proc_address_to_fd ((CORE_ADDR) prmap -> pr_vaddr, 0);
	      funcstat = (*func) (fd, (CORE_ADDR) prmap -> pr_vaddr);
	      close (fd);
	    }
	}
    }
  return (funcstat);
}

/*

GLOBAL FUNCTION

	proc_base_address -- find base address for segment containing address

SYNOPSIS

	CORE_ADDR proc_base_address (CORE_ADDR addr)

DESCRIPTION

	Given an address of a location in the inferior, find and return
	the base address of the mapped segment containing that address.

	This is used for example, by the shared library support code,
	where we have the pc value for some location in the shared library
	where we are stopped, and need to know the base address of the
	segment containing that address.
*/


#if 0	/* Currently unused */

CORE_ADDR
proc_base_address (addr)
CORE_ADDR addr;
{
  int nmap;
  struct prmap *prmaps;
  struct prmap *prmap;
  CORE_ADDR baseaddr = 0;

  if (pi.valid && (ioctl (pi.fd, PIOCNMAP, &nmap) == 0))
    {
      prmaps = (struct prmap *) alloca ((nmap + 1) * sizeof (*prmaps));
      if (ioctl (pi.fd, PIOCMAP, prmaps) == 0)
	{
	  for (prmap = prmaps; prmap -> pr_size; ++prmap)
	    {
	      if ((prmap -> pr_vaddr <= (caddr_t) addr) &&
		  (prmap -> pr_vaddr + prmap -> pr_size > (caddr_t) addr))
		{
		  baseaddr = (CORE_ADDR) prmap -> pr_vaddr;
		  break;
		}
	    }
	}
    }
  return (baseaddr);
}

#endif	/* 0 */

/*

GLOBAL_FUNCTION

	proc_address_to_fd -- return open fd for file mapped to address

SYNOPSIS

	int proc_address_to_fd (CORE_ADDR addr, complain)

DESCRIPTION

	Given an address in the current inferior's address space, use the
	/proc interface to find an open file descriptor for the file that
	this address was mapped in from.  Return -1 if there is no current
	inferior.  Print a warning message if there is an inferior but
	the address corresponds to no file (IE a bogus address).

*/

static int
proc_address_to_fd (addr, complain)
     CORE_ADDR addr;
     int complain;
{
  int fd = -1;

  if (pi.valid)
    {
      if ((fd = ioctl (pi.fd, PIOCOPENM, (caddr_t *) &addr)) < 0)
	{
	  if (complain)
	    {
	      print_sys_errmsg (pi.pathname, errno);
	      warning ("can't find mapped file for address 0x%x", addr);
	    }
	}
    }
  return (fd);
}


#ifdef ATTACH_DETACH

/*

GLOBAL FUNCTION

	attach -- attach to an already existing process

SYNOPSIS

	int attach (int pid)

DESCRIPTION

	Attach to an already existing process with the specified process
	id.  If the process is not already stopped, query whether to
	stop it or not.

NOTES

	The option of stopping at attach time is specific to the /proc
	versions of gdb.  Versions using ptrace force the attachee
	to stop.

*/

int
attach (pid)
     int pid;
{
  if (!open_proc_file (pid, &pi))
    {
      perror_with_name (pi.pathname);
      /* NOTREACHED */
    }
  
  /*  Get current status of process and if it is not already stopped,
      then stop it.  Remember whether or not it was stopped when we first
      examined it. */
  
  if (ioctl (pi.fd, PIOCSTATUS, &pi.prstatus) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      close_proc_file (&pi);
      error ("PIOCSTATUS failed");
    }
  if (pi.prstatus.pr_flags & (PR_STOPPED | PR_ISTOP))
    {
      pi.was_stopped = 1;
    }
  else
    {
      pi.was_stopped = 0;
      if (query ("Process is currently running, stop it? "))
	{
	  if (ioctl (pi.fd, PIOCSTOP, &pi.prstatus) < 0)
	    {
	      print_sys_errmsg (pi.pathname, errno);
	      close_proc_file (&pi);
	      error ("PIOCSTOP failed");
	    }
	}
    }
  
  /*  Remember some things about the inferior that we will, or might, change
      so that we can restore them when we detach. */
  
  (void) ioctl (pi.fd, PIOCGTRACE, &pi.trace);
  (void) ioctl (pi.fd, PIOCGFAULT, &pi.fltset);
  (void) ioctl (pi.fd, PIOCGENTRY, &pi.entryset);
  (void) ioctl (pi.fd, PIOCGEXIT, &pi.exitset);
  
  /* Set up trace and fault sets, as gdb expects them. */
  
  (void) memset (&pi.prrun, 0, sizeof (pi.prrun));
  prfillset (&pi.prrun.pr_trace);
  prfillset (&pi.prrun.pr_fault);
  prdelset (&pi.prrun.pr_fault, FLTPAGE);
  if (ioctl (pi.fd, PIOCSFAULT, &pi.prrun.pr_fault))
    {
      print_sys_errmsg ("PIOCSFAULT failed", errno);
    }
  if (ioctl (pi.fd, PIOCSTRACE, &pi.prrun.pr_trace))
    {
      print_sys_errmsg ("PIOCSTRACE failed", errno);
    }
  attach_flag = 1;
  return (pid);
}

/*

GLOBAL FUNCTION

	detach -- detach from an attached-to process

SYNOPSIS

	void detach (int signal)

DESCRIPTION

	Detach from the current attachee.

	If signal is non-zero, the attachee is started running again and sent
	the specified signal.

	If signal is zero and the attachee was not already stopped when we
	attached to it, then we make it runnable again when we detach.

	Otherwise, we query whether or not to make the attachee runnable
	again, since we may simply want to leave it in the state it was in
	when we attached.

	We report any problems, but do not consider them errors, since we
	MUST detach even if some things don't seem to go right.  This may not
	be the ideal situation.  (FIXME).
 */

void
detach (signal)
     int signal;
{
  if (signal)
    {
      struct siginfo siginfo;
      siginfo.si_signo = signal;
      siginfo.si_code = 0;
      siginfo.si_errno = 0;
      if (ioctl (pi.fd, PIOCSSIG, &siginfo) < 0)
	{
	  print_sys_errmsg (pi.pathname, errno);
	  printf ("PIOCSSIG failed.\n");
	}
    }
  if (ioctl (pi.fd, PIOCSEXIT, &pi.exitset) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      printf ("PIOCSEXIT failed.\n");
    }
  if (ioctl (pi.fd, PIOCSENTRY, &pi.entryset) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      printf ("PIOCSENTRY failed.\n");
    }
  if (ioctl (pi.fd, PIOCSTRACE, &pi.trace) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      printf ("PIOCSTRACE failed.\n");
    }
  if (ioctl (pi.fd, PIOCSFAULT, &pi.fltset) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      printf ("PIOCSFAULT failed.\n");
    }
  if (ioctl (pi.fd, PIOCSTATUS, &pi.prstatus) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      printf ("PIOCSTATUS failed.\n");
    }
  else
    {
      if (signal || (pi.prstatus.pr_flags & (PR_STOPPED | PR_ISTOP)))
	{
	  if (signal || !pi.was_stopped ||
	      query ("Was stopped when attached, make it runnable again? "))
	    {
	      (void) memset (&pi.prrun, 0, sizeof (pi.prrun));
	      pi.prrun.pr_flags = PRCFAULT;
	      if (ioctl (pi.fd, PIOCRUN, &pi.prrun))
		{
		  print_sys_errmsg (pi.pathname, errno);
		  printf ("PIOCRUN failed.\n");
		}
	    }
	}
    }
  close_proc_file (&pi);
  attach_flag = 0;
}

#endif	/* ATTACH_DETACH */

/*

GLOBAL FUNCTION

	proc_wait -- emulate wait() as much as possible

SYNOPSIS

	int proc_wait (int *statloc)

DESCRIPTION

	Try to emulate wait() as much as possible.  Not sure why we can't
	just use wait(), but it seems to have problems when applied to a
	process being controlled with the /proc interface.

NOTES

	We have a race problem here with no obvious solution.  We need to let
	the inferior run until it stops on an event of interest, which means
	that we need to use the PIOCWSTOP ioctl.  However, we cannot use this
	ioctl if the process is already stopped on something that is not an
	event of interest, or the call will hang indefinitely.  Thus we first
	use PIOCSTATUS to see if the process is not stopped.  If not, then we
	use PIOCWSTOP.  But during the window between the two, if the process
	stops for any reason that is not an event of interest (such as a job
	control signal) then gdb will hang.  One possible workaround is to set
	an alarm to wake up every minute of so and check to see if the process
	is still running, and if so, then reissue the PIOCWSTOP.  But this is
	a real kludge, so has not been implemented.  FIXME: investigate
	alternatives.

	FIXME:  Investigate why wait() seems to have problems with programs
	being control by /proc routines.

 */

int
proc_wait (statloc)
     int *statloc;
{
  short what;
  short why;
  int statval = 0;
  int checkerr = 0;
  int rtnval = -1;
  
  if (ioctl (pi.fd, PIOCSTATUS, &pi.prstatus) < 0)
    {
      checkerr++;
    }
  else if (!(pi.prstatus.pr_flags & (PR_STOPPED | PR_ISTOP)))
    {
      if (ioctl (pi.fd, PIOCWSTOP, &pi.prstatus) < 0)
	{
	  checkerr++;
	}
    }    
  if (checkerr)
    {
      if (errno == ENOENT)
	{
	  rtnval = wait (&statval);
	  if (rtnval != inferior_pid)
	    {
	      error ("PIOCWSTOP, wait failed, returned %d", rtnval);
	      /* NOTREACHED */
	    }
	}
      else
	{
	  print_sys_errmsg (pi.pathname, errno);
	  error ("PIOCSTATUS or PIOCWSTOP failed.");
	  /* NOTREACHED */
	}
    }
  else if (pi.prstatus.pr_flags & (PR_STOPPED | PR_ISTOP))
    {
      rtnval = pi.prstatus.pr_pid;
      why = pi.prstatus.pr_why;
      what = pi.prstatus.pr_what;
      if (why == PR_SIGNALLED)
	{
	  statval = (what << 8) | 0177;
	}
      else if ((why == PR_SYSEXIT) &&
	       (what == SYS_exec || what == SYS_execve))
	{
	  statval = (SIGTRAP << 8) | 0177;
	}
      else if (why == PR_REQUESTED)
	{
	  statval = (SIGSTOP << 8) | 0177;
	}
      else if (why == PR_JOBCONTROL)
	{
	  statval = (what << 8) | 0177;
	}
      else if (why == PR_FAULTED)
	{
	  switch (what)
	    {
	    case FLTPRIV:
	    case FLTILL:
	      statval = (SIGILL << 8) | 0177;
	      break;
	    case FLTBPT:
	    case FLTTRACE:
	      statval = (SIGTRAP << 8) | 0177;
	      break;
	    case FLTSTACK:
	    case FLTACCESS:
	    case FLTBOUNDS:
	      statval = (SIGSEGV << 8) | 0177;
	      break;
	    case FLTIOVF:
	    case FLTIZDIV:
	    case FLTFPE:
	      statval = (SIGFPE << 8) | 0177;
	      break;
	    case FLTPAGE:		/* Recoverable page fault */
	    default:
	      rtnval = -1;
	      error ("PIOCWSTOP, unknown why %d, what %d", why, what);
	      /* NOTREACHED */
	    }
	}
      else
	{
	  rtnval = -1;
	  error ("PIOCWSTOP, unknown why %d, what %d", why, what);
	  /* NOTREACHED */
	}
    }
  else
    {
      error ("PIOCWSTOP, stopped for unknown/unhandled reason, flags %#x", 
	     pi.prstatus.pr_flags);
	  /* NOTREACHED */
    }
  if (statloc)
    {
      *statloc = statval;
    }
  return (rtnval);
}

/*

GLOBAL FUNCTION

	child_resume -- resume execution of the inferior process

SYNOPSIS

	void child_resume (int step, int signal)

DESCRIPTION

	Resume execution of the inferior process.  If STEP is nozero, then
	just single step it.  If SIGNAL is nonzero, restart it with that
	signal activated.

NOTE

	It may not be absolutely necessary to specify the PC value for
	restarting, but to be safe we use the value that gdb considers
	to be current.  One case where this might be necessary is if the
	user explicitly changes the PC value that gdb considers to be
	current.  FIXME:  Investigate if this is necessary or not.
 */

void
child_resume (step, signal)
     int step;
     int signal;
{
  errno = 0;
  pi.prrun.pr_flags = PRSVADDR | PRSTRACE | PRSFAULT | PRCFAULT;
  pi.prrun.pr_vaddr = (caddr_t) *(int *) &registers[REGISTER_BYTE (PC_REGNUM)];
  if (signal)
    {
      if (signal != pi.prstatus.pr_cursig)
	{
	  struct siginfo siginfo;
	  siginfo.si_signo = signal;
	  siginfo.si_code = 0;
	  siginfo.si_errno = 0;
	  (void) ioctl (pi.fd, PIOCSSIG, &siginfo);
	}
    }
  else
    {
      pi.prrun.pr_flags |= PRCSIG;
    }
  if (step)
    {
      pi.prrun.pr_flags |= PRSTEP;
    }
  if (ioctl (pi.fd, PIOCRUN, &pi.prrun) != 0)
    {
      perror_with_name (pi.pathname);
      /* NOTREACHED */
    }
}

/*

GLOBAL FUNCTION

	fetch_inferior_registers -- fetch current registers from inferior

SYNOPSIS

	void fetch_inferior_registers (int regno)

DESCRIPTION

	Read the current values of the inferior's registers, both the
	general register set and floating point registers (if supported)
	and update gdb's idea of their current values.

*/

void
fetch_inferior_registers (regno)
     int regno;
{
  if (ioctl (pi.fd, PIOCGREG, &pi.gregset) != -1)
    {
      supply_gregset (&pi.gregset);
    }
#if defined (FP0_REGNUM)
  if (ioctl (pi.fd, PIOCGFPREG, &pi.fpregset) != -1)
    {
      supply_fpregset (&pi.fpregset);
    }
#endif
}

/*

GLOBAL FUNCTION

	fetch_core_registers -- fetch current registers from core file data

SYNOPSIS

	void fetch_core_registers (char *core_reg_sect, unsigned core_reg_size,
				   int which, unsigned in reg_addr)

DESCRIPTION

	Read the values of either the general register set (WHICH equals 0)
	or the floating point register set (WHICH equals 2) from the core
	file data (pointed to by CORE_REG_SECT), and update gdb's idea of
	their current values.  The CORE_REG_SIZE parameter is ignored.

NOTES

	Use the indicated sizes to validate the gregset and fpregset
	structures.
*/

void
fetch_core_registers (core_reg_sect, core_reg_size, which, reg_addr)
  char *core_reg_sect;
  unsigned core_reg_size;
  int which;
  unsigned int reg_addr;	/* Unused in this version */
{

  if (which == 0)
    {
      if (core_reg_size != sizeof (pi.gregset))
	{
	  warning ("wrong size gregset struct in core file");
	}
      else
	{
	  (void) memcpy ((char *) &pi.gregset, core_reg_sect,
			 sizeof (pi.gregset));
	  supply_gregset (&pi.gregset);
	}
    }
  else if (which == 2)
    {
      if (core_reg_size != sizeof (pi.fpregset))
	{
	  warning ("wrong size fpregset struct in core file");
	}
      else
	{
	  (void) memcpy ((char *) &pi.fpregset, core_reg_sect,
			 sizeof (pi.fpregset));
#if defined (FP0_REGNUM)
	  supply_fpregset (&pi.fpregset);
#endif
	}
    }
}

/*

LOCAL FUNCTION

	proc_init_failed - called whenever /proc access initialization fails

SYNOPSIS

	static void proc_init_failed (char *why)

DESCRIPTION

	This function is called whenever initialization of access to a /proc
	entry fails.  It prints a suitable error message, does some cleanup,
	and then invokes the standard error processing routine which dumps
	us back into the command loop.
 */

static void
proc_init_failed (why)
     char *why;
{
  print_sys_errmsg (pi.pathname, errno);
  (void) kill (pi.pid, SIGKILL);
  close_proc_file (&pi);
  error (why);
  /* NOTREACHED */
}

/*

LOCAL FUNCTION

	close_proc_file - close any currently open /proc entry

SYNOPSIS

	static void close_proc_file (struct procinfo *pip)

DESCRIPTION

	Close any currently open /proc entry and mark the process information
	entry as invalid.  In order to ensure that we don't try to reuse any
	stale information, the pid, fd, and pathnames are explicitly
	invalidated, which may be overkill.

 */

static void
close_proc_file (pip)
     struct procinfo *pip;
{
  pip -> pid = 0;
  if (pip -> valid)
    {
      (void) close (pip -> fd);
    }
  pip -> fd = -1;
  if (pip -> pathname)
    {
      free (pip -> pathname);
      pip -> pathname = NULL;
    }
  pip -> valid = 0;
}

/*

LOCAL FUNCTION

	open_proc_file - open a /proc entry for a given process id

SYNOPSIS

	static int open_proc_file (pid, struct procinfo *pip)

DESCRIPTION

	Given a process id, close the existing open /proc entry (if any)
	and open one for the new process id.  Once it is open, then
	mark the local process information structure as valid, which
	guarantees that the pid, fd, and pathname fields match an open
	/proc entry.  Returns zero if the open fails, nonzero otherwise.

	Note that the pathname is left intact, even when the open fails,
	so that callers can use it to construct meaningful error messages
	rather than just "file open failed".
 */

static int
open_proc_file (pid, pip)
     int pid;
     struct procinfo *pip;
{
  pip -> valid = 0;
  if (pip -> valid)
    {
      (void) close (pip -> fd);
    }
  if (pip -> pathname == NULL)
    {
      pip -> pathname = xmalloc (32);
    }
  sprintf (pip -> pathname, PROC_NAME_FMT, pid);
  if ((pip -> fd = open (pip -> pathname, O_RDWR)) >= 0)
    {
      pip -> valid = 1;
      pip -> pid = pid;
    }
  return (pip -> valid);
}

static char *
mappingflags (flags)
     long flags;
{
  static char asciiflags[7];
  
  strcpy (asciiflags, "------");
  if (flags & MA_STACK)  asciiflags[0] = 's';
  if (flags & MA_BREAK)  asciiflags[1] = 'b';
  if (flags & MA_SHARED) asciiflags[2] = 's';
  if (flags & MA_READ)   asciiflags[3] = 'r';
  if (flags & MA_WRITE)  asciiflags[4] = 'w';
  if (flags & MA_EXEC)   asciiflags[5] = 'x';
  return (asciiflags);
}

static void
proc_info_address_map (pip, verbose)
     struct procinfo *pip;
     int verbose;
{
  int nmap;
  struct prmap *prmaps;
  struct prmap *prmap;

  printf_filtered ("Mapped address spaces:\n\n");
  printf_filtered ("\t%10s %10s %10s %10s %6s\n",
		   "Start Addr",
		   "  End Addr",
		   "      Size",
		   "    Offset",
		   "Flags");
  if (ioctl (pip -> fd, PIOCNMAP, &nmap) == 0)
    {
      prmaps = (struct prmap *) alloca ((nmap + 1) * sizeof (*prmaps));
      if (ioctl (pip -> fd, PIOCMAP, prmaps) == 0)
	{
	  for (prmap = prmaps; prmap -> pr_size; ++prmap)
	    {
	      printf_filtered ("\t%#10x %#10x %#10x %#10x %6s\n",
			       prmap -> pr_vaddr,
			       prmap -> pr_vaddr + prmap -> pr_size - 1,
			       prmap -> pr_size,
			       prmap -> pr_off,
			       mappingflags (prmap -> pr_mflags));
	    }
	}
    }
  printf_filtered ("\n\n");
}

/*

LOCAL FUNCTION

	proc_info -- implement the "info proc" command

SYNOPSIS

	void proc_info (char *args, int from_tty)

DESCRIPTION

	Implement gdb's "info proc" command by using the /proc interface
	to print status information about any currently running process.

	Examples of the use of "info proc" are:

	info proc		Print short info about current inferior.
	info proc verbose	Print verbose info about current inferior.
	info proc 123		Print short info about process pid 123.
	info proc 123 verbose	Print verbose info about process pid 123.

 */

static void
proc_info (args, from_tty)
     char *args;
     int from_tty;
{
  int verbose = 0;
  int pid;
  struct procinfo pii;
  struct procinfo *pip;
  struct cleanup *old_chain;
  char *nexttok;

  old_chain = make_cleanup (null_cleanup, 0);

  /* Default to using the current inferior if no pid specified */

  pip = &pi;

  /* Parse the args string, looking for "verbose" (or any abbrev) and
     for a specific pid.  If a specific pid is found, the process
     file is opened. */
  
  if (args != NULL)
    {
      while ((nexttok = strtok (args, " \t")) != NULL)
	{
	  args = NULL;
	  if (strncmp (nexttok, "verbose", strlen (nexttok)) == 0)
	    {
	      verbose++;
	    }
	  else if ((pii.pid = atoi (nexttok)) > 0)
	    {
	      pid = pii.pid;
	      pip = &pii;
	      (void) memset (&pii, 0, sizeof (pii));
	      if (!open_proc_file (pid, pip))
		{
		  perror_with_name (pip -> pathname);
		  /* NOTREACHED */
		}
	      make_cleanup (close_proc_file, pip);
	    }
	}
    }

  /* If we don't have a valid open process at this point, then we have no
     inferior or didn't specify a specific pid. */

  if (!pip -> valid)
    {
      error ("No process.  Run an inferior or specify an explicit pid.");
    }
  if (ioctl (pip -> fd, PIOCSTATUS, &(pip -> prstatus)) < 0)
    {
      print_sys_errmsg (pip -> pathname, errno);
      error ("PIOCSTATUS failed");
    }

  printf_filtered ("\nStatus information for %s:\n\n", pip -> pathname);
  proc_info_address_map (pip, verbose);
#if 0
  proc_info_flags (pip, verbose);
  proc_info_why (pip, verbose);
  proc_info_what (pip, verbose);
  proc_info_info (pip, verbose);
  proc_info_cursig (pip, verbose);
  proc_info_sigpend (pip, verbose);
  proc_info_sighold (pip, verbose);
  proc_info_altstack (pip, verbose);
  proc_info_action (pip, verbose);
  proc_info_id (pip, verbose);
  proc_info_times (pip, verbose);
  proc_info_clname (pip,verbose);
  proc_info_instr (pip, verbose);
  proc_info_reg (pip, verbose);
#endif  

  /* All done, deal with closing any temporary process info structure,
     freeing temporary memory , etc. */

  do_cleanups (old_chain);
}

/*

GLOBAL FUNCTION

	_initialize_proc_fs -- initialize the process file system stuff

SYNOPSIS

	void _initialize_proc_fs (void)

DESCRIPTION

	Do required initializations during gdb startup for using the
	/proc file system interface.

*/

static char *proc_desc =
"Show current process status information using /proc entry.\n\
With no arguments, prints short form.  With 'verbose' prints long form.";

void
_initialize_proc_fs ()
{
  add_info ("proc", proc_info, proc_desc);
}

#endif	/* USE_PROC_FS */
