/* Native support code for HPUX PA-RISC.
   Copyright 1986, 1987, 1989, 1990, 1991, 1992, 1993, 1998
   Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include "defs.h"
#include "inferior.h"
#include "target.h"
#include <sys/ptrace.h>
#include "gdbcore.h"
#include <wait.h>
#include <signal.h>

extern CORE_ADDR text_end;

static void fetch_register PARAMS ((int));

void
fetch_inferior_registers (regno)
     int regno;
{
  if (regno == -1)
    for (regno = 0; regno < NUM_REGS; regno++)
      fetch_register (regno);
  else
    fetch_register (regno);
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

void
store_inferior_registers (regno)
     int regno;
{
  register unsigned int regaddr;
  char buf[80];
  extern char registers[];
  register int i;
  unsigned int offset = U_REGS_OFFSET;
  int scratch;

  if (regno >= 0)
    {
      if (CANNOT_STORE_REGISTER (regno))
	return;
      regaddr = register_addr (regno, offset);
      errno = 0;
      if (regno == PCOQ_HEAD_REGNUM || regno == PCOQ_TAIL_REGNUM)
        {
          scratch = *(int *) &registers[REGISTER_BYTE (regno)] | 0x3;
          call_ptrace (PT_WUREGS, inferior_pid, (PTRACE_ARG3_TYPE) regaddr,
                  scratch);
          if (errno != 0)
            {
	      /* Error, even if attached.  Failing to write these two
		 registers is pretty serious.  */
              sprintf (buf, "writing register number %d", regno);
              perror_with_name (buf);
            }
        }
      else
	for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof(int))
	  {
	    errno = 0;
	    call_ptrace (PT_WUREGS, inferior_pid, (PTRACE_ARG3_TYPE) regaddr,
		    *(int *) &registers[REGISTER_BYTE (regno) + i]);
	    if (errno != 0)
	      {
		/* Warning, not error, in case we are attached; sometimes the
		   kernel doesn't let us at the registers.  */
		char *err = safe_strerror (errno);
		char *msg = alloca (strlen (err) + 128);
		sprintf (msg, "writing register %s: %s",
			 REGISTER_NAME (regno), err);
		warning (msg);
		return;
	      }
	    regaddr += sizeof(int);
	  }
    }
  else
    for (regno = 0; regno < NUM_REGS; regno++)
      store_inferior_registers (regno);
}

/* Fetch one register.  */

static void
fetch_register (regno)
     int regno;
{
  register unsigned int regaddr;
  char buf[MAX_REGISTER_RAW_SIZE];
  register int i;

  /* Offset of registers within the u area.  */
  unsigned int offset;

  offset = U_REGS_OFFSET;

  regaddr = register_addr (regno, offset);
  for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
    {
      errno = 0;
      *(int *) &buf[i] = call_ptrace (PT_RUREGS, inferior_pid,
				 (PTRACE_ARG3_TYPE) regaddr, 0);
      regaddr += sizeof (int);
      if (errno != 0)
	{
	  /* Warning, not error, in case we are attached; sometimes the
	   * kernel doesn't let us at the registers.
	   */
	  char *err = safe_strerror (errno);
	  char *msg = alloca (strlen (err) + 128);
	  sprintf (msg, "reading register %s: %s", REGISTER_NAME (regno), err);
	  warning (msg);
	  goto error_exit;
	}
    }
  if (regno == PCOQ_HEAD_REGNUM || regno == PCOQ_TAIL_REGNUM)
    buf[3] &= ~0x3;
  supply_register (regno, buf);
 error_exit:;
}

/* Copy LEN bytes to or from inferior's memory starting at MEMADDR
   to debugger memory starting at MYADDR.   Copy to inferior if
   WRITE is nonzero.
  
   Returns the length copied, which is either the LEN argument or zero.
   This xfer function does not do partial moves, since child_ops
   doesn't allow memory operations to cross below us in the target stack
   anyway.  */

int
child_xfer_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;		/* ignored */
{
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr = memaddr & - sizeof (int);
  /* Round ending address up; get number of longwords that makes.  */
  register int count
    = (((memaddr + len) - addr) + sizeof (int) - 1) / sizeof (int);

  /* Allocate buffer of that many longwords.  */
  /* Note (RT) - This code formerly used alloca, which I have
   * replaced with xmalloc and a matching free() at the end.
   * The problem with alloca() is that there is no guarantee of
   * when it'll be freed, and we were seeing cases of memory
   * leaks on:
   * (gdb) watch x
   * (gdb) cont
   * where the piled-up alloca's for the child_xfer_memory buffers
   * were not getting freed.
   */
  register int *buffer = (int *) xmalloc (count * sizeof (int));

  if (write)
    {
      /* Fill start and end extra bytes of buffer with existing memory data.  */

      if (addr != memaddr || len < (int)sizeof (int)) {
	/* Need part of initial word -- fetch it.  */
        buffer[0] = call_ptrace (addr < text_end ? PT_RIUSER : PT_RDUSER, 
			    inferior_pid, (PTRACE_ARG3_TYPE) addr, 0);
      }

      if (count > 1)		/* FIXME, avoid if even boundary */
	{
	  buffer[count - 1]
	    = call_ptrace (addr < text_end ? PT_RIUSER : PT_RDUSER, inferior_pid,
		      (PTRACE_ARG3_TYPE) (addr + (count - 1) * sizeof (int)),
		      0);
	}

      /* Copy data to be written over corresponding part of buffer */

      memcpy ((char *) buffer + (memaddr & (sizeof (int) - 1)), myaddr, len);

      /* Write the entire buffer.  */

      for (i = 0; i < count; i++, addr += sizeof (int))
	{
          int  pt_status;
          int  pt_request;
	  /* The HP-UX kernel crashes if you use PT_WDUSER to write into the text
	     segment.  FIXME -- does it work to write into the data segment using
	     WIUSER, or do these idiots really expect us to figure out which segment
	     the address is in, so we can use a separate system call for it??!  */
	  errno = 0;
          pt_request = (addr < text_end) ? PT_WIUSER : PT_WDUSER;
	  pt_status = call_ptrace (pt_request,
                                   inferior_pid, 
                                   (PTRACE_ARG3_TYPE) addr,
                                   buffer[i]);

          /* Did we fail?  Might we've guessed wrong about which
             segment this address resides in?  Try the other request,
             and see if that works...
             */
	  if ((pt_status == -1) && errno) {
            errno = 0;
            pt_request = (pt_request == PT_WIUSER) ? PT_WDUSER : PT_WIUSER;
            pt_status = call_ptrace (pt_request,
                                     inferior_pid, 
                                     (PTRACE_ARG3_TYPE) addr,
                                     buffer[i]);

            /* No, we still fail.  Okay, time to punt. */
            if ((pt_status == -1) && errno)
              {
                free(buffer);
                return 0;
              }
	  }
	}
    }
  else
    {
      /* Read all the longwords */
      for (i = 0; i < count; i++, addr += sizeof (int))
	{
	  errno = 0;
	  buffer[i] = call_ptrace (addr < text_end ? PT_RIUSER : PT_RDUSER, 
			      inferior_pid, (PTRACE_ARG3_TYPE) addr, 0);
	  if (errno) {
	    free(buffer);
	    return 0;
	  }
	  QUIT;
	}

      /* Copy appropriate bytes out of the buffer.  */
      memcpy (myaddr, (char *) buffer + (memaddr & (sizeof (int) - 1)), len);
    }
  free(buffer);
  return len;
}


void
child_post_follow_inferior_by_clone ()
{
  int  status;

  /* This function is used when following both the parent and child
     of a fork.  In this case, the debugger clones itself.  The original
     debugger follows the parent, the clone follows the child.  The
     original detaches from the child, delivering a SIGSTOP to it to
     keep it from running away until the clone can attach itself.

     At this point, the clone has attached to the child.  Because of
     the SIGSTOP, we must now deliver a SIGCONT to the child, or it
     won't behave properly. */
  status = kill (inferior_pid, SIGCONT);
}


void
child_post_follow_vfork (parent_pid, followed_parent, child_pid, followed_child)
     int  parent_pid;
     int  followed_parent;
     int  child_pid;
     int  followed_child;
{

  /* Are we a debugger that followed the parent of a vfork?  If so,
     then recall that the child's vfork event was delivered to us
     first.  And, that the parent was suspended by the OS until the
     child's exec or exit events were received.

     Upon receiving that child vfork, then, we were forced to remove
     all breakpoints in the child and continue it so that it could
     reach the exec or exit point.

     But also recall that the parent and child of a vfork share the
     same address space.  Thus, removing bp's in the child also
     removed them from the parent.

     Now that the child has safely exec'd or exited, we must restore
     the parent's breakpoints before we continue it.  Else, we may
     cause it run past expected stopping points. */
  if (followed_parent)
    {
      reattach_breakpoints (parent_pid);
    }

  /* Are we a debugger that followed the child of a vfork?  If so,
     then recall that we don't actually acquire control of the child
     until after it has exec'd or exited.
     */
  if (followed_child)
    {
      /* If the child has exited, then there's nothing for us to do.
         In the case of an exec event, we'll let that be handled by
         the normal mechanism that notices and handles exec events, in
         resume(). */

    }
}

/* Format a process id, given a pid.  Be sure to terminate
 * this with a null--it's going to be printed via a "%s".
 */
char *
hppa_pid_to_str( pid )
    pid_t pid;
{
    static char buf[30]; /* Static because address returned */

    sprintf( buf, "process %d\0\0\0\0", pid );
             /* Extra NULLs for paranoia's sake */
             
    return buf;
}

/* Format a thread id, given a tid.  Be sure to terminate
 * this with a null--it's going to be printed via a "%s".
 *
 * Note: This is a core-gdb tid, not the actual system tid.
 *       See infttrace.c for details.
 */
char *
hppa_tid_to_str( tid )
    pid_t tid;
{
    static char buf[30]; /* Static because address returned */

    sprintf( buf, "system thread %d\0\0\0\0", tid );
             /* Extra NULLs for paranoia's sake */
             
    return buf;
}

#if !defined (GDB_NATIVE_HPUX_11)

/* The following code is a substitute for the infttrace.c versions used
   with ttrace() in HPUX 11.  */

/* This value is an arbitrary integer. */
#define PT_VERSION 123456

/* This semaphore is used to coordinate the child and parent processes
   after a fork(), and before an exec() by the child.  See
   parent_attach_all for details.  */

typedef struct {
    int parent_channel[2];  /* Parent "talks" to [1], child "listens" to [0] */
    int child_channel[2];   /* Child "talks" to [1], parent "listens" to [0] */
} startup_semaphore_t;

#define SEM_TALK (1)
#define SEM_LISTEN (0)

static startup_semaphore_t  startup_semaphore;

extern int parent_attach_all PARAMS ((int, PTRACE_ARG3_TYPE, int));

#ifdef PT_SETTRC
/* This function causes the caller's process to be traced by its
   parent.  This is intended to be called after GDB forks itself,
   and before the child execs the target.

   Note that HP-UX ptrace is rather funky in how this is done.
   If the parent wants to get the initial exec event of a child,
   it must set the ptrace event mask of the child to include execs.
   (The child cannot do this itself.)  This must be done after the
   child is forked, but before it execs.

   To coordinate the parent and child, we implement a semaphore using
   pipes.  After SETTRC'ing itself, the child tells the parent that
   it is now traceable by the parent, and waits for the parent's
   acknowledgement.  The parent can then set the child's event mask,
   and notify the child that it can now exec.

   (The acknowledgement by parent happens as a result of a call to
   child_acknowledge_created_inferior.)  */

int
parent_attach_all (pid, addr, data)
     int pid;
     PTRACE_ARG3_TYPE addr;
     int data;
{
  int pt_status = 0;

  /* We need a memory home for a constant.  */
  int tc_magic_child = PT_VERSION;
  int tc_magic_parent = 0;

  /* The remainder of this function is only useful for HPUX 10.0 and
     later, as it depends upon the ability to request notification
     of specific kinds of events by the kernel.  */
#if defined(PT_SET_EVENT_MASK)

  /* Notify the parent that we're potentially ready to exec(). */
  write (startup_semaphore.child_channel[SEM_TALK],
         &tc_magic_child,
         sizeof (tc_magic_child));

  /* Wait for acknowledgement from the parent. */
  read (startup_semaphore.parent_channel[SEM_LISTEN],
        &tc_magic_parent,
        sizeof (tc_magic_parent));
  if (tc_magic_child != tc_magic_parent)
      warning ("mismatched semaphore magic");

  /* Discard our copy of the semaphore. */
  (void) close (startup_semaphore.parent_channel[SEM_LISTEN]);
  (void) close (startup_semaphore.parent_channel[SEM_TALK]);
  (void) close (startup_semaphore.child_channel[SEM_LISTEN]);
  (void) close (startup_semaphore.child_channel[SEM_TALK]);
#endif
  
  return 0;
}
#endif

int
hppa_require_attach (pid)
     int pid;
{
  int pt_status;
  CORE_ADDR  pc;
  CORE_ADDR  pc_addr;
  unsigned int regs_offset;

  /* Are we already attached?  There appears to be no explicit way to
     answer this via ptrace, so we try something which should be
     innocuous if we are attached.  If that fails, then we assume
     we're not attached, and so attempt to make it so. */

  errno = 0;
  regs_offset = U_REGS_OFFSET;
  pc_addr = register_addr (PC_REGNUM, regs_offset);
  pc = call_ptrace (PT_READ_U, pid, (PTRACE_ARG3_TYPE) pc_addr, 0);

  if (errno)
    {
      errno = 0;
      pt_status = call_ptrace (PT_ATTACH, pid, (PTRACE_ARG3_TYPE) 0, 0);

      if (errno)
        return -1;

      /* Now we really are attached. */
      errno = 0;
    }
  attach_flag = 1;
  return pid;
}

int
hppa_require_detach (pid, signal)
  int pid;
  int signal;
{
  errno = 0;
  call_ptrace (PT_DETACH, pid, (PTRACE_ARG3_TYPE) 1, signal);
  errno = 0;  /* Ignore any errors. */
  return pid;
}

/* Since ptrace doesn't support memory page-protection events, which
   are used to implement "hardware" watchpoints on HP-UX, these are
   dummy versions, which perform no useful work.  */

void
hppa_enable_page_protection_events (pid)
     int pid;
{
}

void
hppa_disable_page_protection_events (pid)
     int pid;
{
}

int
hppa_insert_hw_watchpoint (pid, start, len, type)
     int pid;
     CORE_ADDR start;
     LONGEST len;
     int type;
{
  error ("Hardware watchpoints not implemented on this platform.");
}

int
hppa_remove_hw_watchpoint (pid, start, len, type)
     int pid;
     CORE_ADDR start;
     LONGEST len;
     enum bptype type;
{
  error ("Hardware watchpoints not implemented on this platform.");
}

int
hppa_can_use_hw_watchpoint (type, cnt, ot)
     enum bptype type;
     int cnt;
     enum bptype ot;
{
  return 0;
}

int
hppa_range_profitable_for_hw_watchpoint (pid, start, len)
     int pid;
     CORE_ADDR start;
     LONGEST len;
{
  error ("Hardware watchpoints not implemented on this platform.");
}

char *
hppa_pid_or_tid_to_str (id)
     pid_t id;
{
  /* In the ptrace world, there are only processes. */
  return hppa_pid_to_str (id);
}

/* This function has no meaning in a non-threaded world.  Thus, we
   return 0 (FALSE).  See the use of "hppa_prepare_to_proceed" in
   hppa-tdep.c. */

pid_t
hppa_switched_threads (pid)
     pid_t pid;
{
  return (pid_t) 0;
}

void
hppa_ensure_vforking_parent_remains_stopped (pid)
     int pid;
{
  /* This assumes that the vforked parent is presently stopped, and
     that the vforked child has just delivered its first exec event.
     Calling kill() this way will cause the SIGTRAP to be delivered as
     soon as the parent is resumed, which happens as soon as the
     vforked child is resumed.  See wait_for_inferior for the use of
     this function.  */
  kill (pid, SIGTRAP);
}

int
hppa_resume_execd_vforking_child_to_get_parent_vfork ()
{
  return 1;  /* Yes, the child must be resumed. */
}

void
require_notification_of_events (pid)
  int pid;
{
#if defined(PT_SET_EVENT_MASK)
  int pt_status;
  ptrace_event_t ptrace_events;

  /* Instruct the kernel as to the set of events we wish to be
     informed of.  (This support does not exist before HPUX 10.0.
     We'll assume if PT_SET_EVENT_MASK has not been defined by
     <sys/ptrace.h>, then we're being built on pre-10.0.)
     */
  memset (&ptrace_events, 0, sizeof (ptrace_events));

  /* Note: By default, all signals are visible to us.  If we wish
     the kernel to keep certain signals hidden from us, we do it
     by calling sigdelset (ptrace_events.pe_signals, signal) for
     each such signal here, before doing PT_SET_EVENT_MASK.
     */
  sigemptyset (&ptrace_events.pe_signals);

  ptrace_events.pe_set_event = 0;

  ptrace_events.pe_set_event |= PTRACE_SIGNAL;
  ptrace_events.pe_set_event |= PTRACE_EXEC;
  ptrace_events.pe_set_event |= PTRACE_FORK;
  ptrace_events.pe_set_event |= PTRACE_VFORK;
  /* ??rehrauer: Add this one when we're prepared to catch it...
  ptrace_events.pe_set_event |= PTRACE_EXIT;
  */

  errno = 0;
  pt_status = call_ptrace (PT_SET_EVENT_MASK,
                      pid,
                      (PTRACE_ARG3_TYPE) &ptrace_events,
                      sizeof (ptrace_events));
  if (errno)
    perror_with_name ("ptrace");
  if (pt_status < 0)
    return;
#endif
}

void
require_notification_of_exec_events (pid)
  int pid;
{
#if defined(PT_SET_EVENT_MASK)
  int pt_status;
  ptrace_event_t ptrace_events;

  /* Instruct the kernel as to the set of events we wish to be
     informed of.  (This support does not exist before HPUX 10.0.
     We'll assume if PT_SET_EVENT_MASK has not been defined by
     <sys/ptrace.h>, then we're being built on pre-10.0.)
     */
  memset (&ptrace_events, 0, sizeof (ptrace_events));

  /* Note: By default, all signals are visible to us.  If we wish
     the kernel to keep certain signals hidden from us, we do it
     by calling sigdelset (ptrace_events.pe_signals, signal) for
     each such signal here, before doing PT_SET_EVENT_MASK.
     */
  sigemptyset (&ptrace_events.pe_signals);

  ptrace_events.pe_set_event = 0;

  ptrace_events.pe_set_event |= PTRACE_EXEC;
  /* ??rehrauer: Add this one when we're prepared to catch it...
  ptrace_events.pe_set_event |= PTRACE_EXIT;
  */

  errno = 0;
  pt_status = call_ptrace (PT_SET_EVENT_MASK,
                      pid,
                      (PTRACE_ARG3_TYPE) &ptrace_events,
                      sizeof (ptrace_events));
  if (errno)
    perror_with_name ("ptrace");
  if (pt_status < 0)
    return;
#endif
}

/* This function is called by the parent process, with pid being the
   ID of the child process, after the debugger has forked.  */

void
child_acknowledge_created_inferior (pid)
    int pid;
{
  /* We need a memory home for a constant.  */
  int tc_magic_parent = PT_VERSION;
  int tc_magic_child = 0;

  /* Wait for the child to tell us that it has forked. */
  read (startup_semaphore.child_channel[SEM_LISTEN],
        &tc_magic_child,
        sizeof(tc_magic_child));

  /* Notify the child that it can exec.

     In the infttrace.c variant of this function, we set the child's
     event mask after the fork but before the exec.  In the ptrace
     world, it seems we can't set the event mask until after the exec.  */

  write (startup_semaphore.parent_channel[SEM_TALK],
         &tc_magic_parent,
         sizeof (tc_magic_parent));

  /* We'd better pause a bit before trying to set the event mask,
     though, to ensure that the exec has happened.  We don't want to
     wait() on the child, because that'll screw up the upper layers
     of gdb's execution control that expect to see the exec event.

     After an exec, the child is no longer executing gdb code.  Hence,
     we can't have yet another synchronization via the pipes.  We'll
     just sleep for a second, and hope that's enough delay...  */

  sleep (1);

  /* Instruct the kernel as to the set of events we wish to be
     informed of.  */

  require_notification_of_exec_events (pid);

  /* Discard our copy of the semaphore. */
  (void) close (startup_semaphore.parent_channel[SEM_LISTEN]);
  (void) close (startup_semaphore.parent_channel[SEM_TALK]);
  (void) close (startup_semaphore.child_channel[SEM_LISTEN]);
  (void) close (startup_semaphore.child_channel[SEM_TALK]);
}

void
child_post_startup_inferior (pid)
  int pid;

{
  require_notification_of_events (pid);
}

void
child_post_attach (pid)
  int pid;
{
  require_notification_of_events (pid);
}

int
child_insert_fork_catchpoint (pid)
  int pid;
{
  /* This request is only available on HPUX 10.0 and later.  */
#if !defined(PT_SET_EVENT_MASK)
  error ("Unable to catch forks prior to HPUX 10.0");
#else
  /* Enable reporting of fork events from the kernel. */
  /* ??rehrauer: For the moment, we're always enabling these events,
     and just ignoring them if there's no catchpoint to catch them.
     */
  return 0;
#endif
}

int
child_remove_fork_catchpoint (pid)
  int pid;
{
  /* This request is only available on HPUX 10.0 and later.  */
#if !defined(PT_SET_EVENT_MASK)
  error ("Unable to catch forks prior to HPUX 10.0");
#else
  /* Disable reporting of fork events from the kernel. */
  /* ??rehrauer: For the moment, we're always enabling these events,
     and just ignoring them if there's no catchpoint to catch them.  */
  return 0;
#endif
}

int
child_insert_vfork_catchpoint (pid)
  int pid;
{
  /* This request is only available on HPUX 10.0 and later.  */
#if !defined(PT_SET_EVENT_MASK)
  error ("Unable to catch vforks prior to HPUX 10.0");
#else
  /* Enable reporting of vfork events from the kernel. */
  /* ??rehrauer: For the moment, we're always enabling these events,
     and just ignoring them if there's no catchpoint to catch them.  */
  return 0;
#endif
}

int
child_remove_vfork_catchpoint (pid)
  int pid;
{
  /* This request is only available on HPUX 10.0 and later.  */
#if !defined(PT_SET_EVENT_MASK)
  error ("Unable to catch vforks prior to HPUX 10.0");
#else
  /* Disable reporting of vfork events from the kernel. */
  /* ??rehrauer: For the moment, we're always enabling these events,
     and just ignoring them if there's no catchpoint to catch them.  */
  return 0;
#endif
}

int
child_has_forked (pid, childpid)
  int pid;
  int *  childpid;
{
  /* This request is only available on HPUX 10.0 and later.  */
#if !defined(PT_GET_PROCESS_STATE)
  *childpid = 0;
  return 0;
#else
  int pt_status;
  ptrace_state_t  ptrace_state;

  errno = 0;
  pt_status = call_ptrace (PT_GET_PROCESS_STATE,
                      pid,
                      (PTRACE_ARG3_TYPE) &ptrace_state,
                      sizeof (ptrace_state));
  if (errno)
    perror_with_name ("ptrace");
  if (pt_status < 0)
    return 0;

  if (ptrace_state.pe_report_event & PTRACE_FORK)
    {
      *childpid = ptrace_state.pe_other_pid;
      return 1;
    }

  return 0;
#endif
}

int
child_has_vforked (pid, childpid)
  int pid;
  int * childpid;
{
  /* This request is only available on HPUX 10.0 and later.  */
#if !defined(PT_GET_PROCESS_STATE)
  *childpid = 0;
  return 0;

#else
  int pt_status;
  ptrace_state_t  ptrace_state;

  errno = 0;
  pt_status = call_ptrace (PT_GET_PROCESS_STATE,
                      pid,
                      (PTRACE_ARG3_TYPE) &ptrace_state,
                      sizeof (ptrace_state));
  if (errno)
    perror_with_name ("ptrace");
  if (pt_status < 0)
    return 0;

  if (ptrace_state.pe_report_event & PTRACE_VFORK)
    {
      *childpid = ptrace_state.pe_other_pid;
      return 1;
    }

  return 0;
#endif
}

int
child_can_follow_vfork_prior_to_exec ()
{
  /* ptrace doesn't allow this. */
  return 0;
}

int
child_insert_exec_catchpoint (pid)
  int pid;
{
  /* This request is only available on HPUX 10.0 and later.
     */
#if !defined(PT_SET_EVENT_MASK)
  error ("Unable to catch execs prior to HPUX 10.0");

#else
  /* Enable reporting of exec events from the kernel. */
  /* ??rehrauer: For the moment, we're always enabling these events,
     and just ignoring them if there's no catchpoint to catch them.
     */
  return 0;
#endif
}

int
child_remove_exec_catchpoint (pid)
  int pid;
{
  /* This request is only available on HPUX 10.0 and later.
     */
#if !defined(PT_SET_EVENT_MASK)
  error ("Unable to catch execs prior to HPUX 10.0");

#else
  /* Disable reporting of exec events from the kernel. */
  /* ??rehrauer: For the moment, we're always enabling these events,
     and just ignoring them if there's no catchpoint to catch them.
     */
  return 0;
#endif
}

int
child_has_execd (pid, execd_pathname)
  int pid;
  char **  execd_pathname;
{

  /* This request is only available on HPUX 10.0 and later.
     */
#if !defined(PT_GET_PROCESS_STATE)
  *execd_pathname = NULL;
  return 0;

#else
  int pt_status;
  ptrace_state_t  ptrace_state;

  errno = 0;
  pt_status = call_ptrace (PT_GET_PROCESS_STATE,
                      pid,
                      (PTRACE_ARG3_TYPE) &ptrace_state,
                      sizeof (ptrace_state));
  if (errno)
    perror_with_name ("ptrace");
  if (pt_status < 0)
    return 0;

  if (ptrace_state.pe_report_event & PTRACE_EXEC)
    {
      char *  exec_file = target_pid_to_exec_file (pid);
      *execd_pathname = savestring (exec_file, strlen (exec_file));
      return 1;
    }

  return 0;
#endif
}

int
child_reported_exec_events_per_exec_call ()
{
  return 2;  /* ptrace reports the event twice per call. */
}

int
child_has_syscall_event (pid, kind, syscall_id)
     int pid;
     enum target_waitkind *kind;
     int *syscall_id;
{
  /* This request is only available on HPUX 10.30 and later, via
     the ttrace interface.  */

  *kind = TARGET_WAITKIND_SPURIOUS;
  *syscall_id = -1;
  return 0;
}

char *
child_pid_to_exec_file (pid)
    int pid;
{
  static char  exec_file_buffer[1024];
  int pt_status;
  CORE_ADDR  top_of_stack;
  char  four_chars[4];
  int name_index;
  int i;
  int saved_inferior_pid;
  boolean  done;
  
#ifdef PT_GET_PROCESS_PATHNAME
  /* As of 10.x HP-UX, there's an explicit request to get the pathname. */
  pt_status = call_ptrace (PT_GET_PROCESS_PATHNAME,
                           pid,
                           (PTRACE_ARG3_TYPE) exec_file_buffer,
                           sizeof (exec_file_buffer) - 1);
  if (pt_status == 0)
    return exec_file_buffer;
#endif

  /* It appears that this request is broken prior to 10.30.
     If it fails, try a really, truly amazingly gross hack
     that DDE uses, of pawing through the process' data
     segment to find the pathname.  */

  top_of_stack = 0x7b03a000;
  name_index = 0;
  done = 0;

  /* On the chance that pid != inferior_pid, set inferior_pid
     to pid, so that (grrrr!) implicit uses of inferior_pid get
     the right id.  */

  saved_inferior_pid = inferior_pid;
  inferior_pid = pid;

  /* Try to grab a null-terminated string. */
  while (! done)
    {
      if (target_read_memory (top_of_stack, four_chars, 4) != 0)
	{
	  inferior_pid = saved_inferior_pid;
	  return NULL;
	}
      for (i = 0; i < 4; i++)
	{
	  exec_file_buffer[name_index++] = four_chars[i];
	  done = (four_chars[i] == '\0');
	  if (done)
	    break;
	}
      top_of_stack += 4;
    }

  if (exec_file_buffer[0] == '\0')
    {
      inferior_pid = saved_inferior_pid;
      return NULL;
    }

  inferior_pid = saved_inferior_pid;
  return exec_file_buffer;
}

void
pre_fork_inferior ()
{
  int status;

  status = pipe (startup_semaphore.parent_channel);
  if (status < 0)
    {
      warning ("error getting parent pipe for startup semaphore");
      return;
    }

  status = pipe (startup_semaphore.child_channel);
  if (status < 0)
    {
      warning ("error getting child pipe for startup semaphore");
      return;
    }
}


/* Check to see if the given thread is alive.

   This is a no-op, as ptrace doesn't support threads, so we just
   return "TRUE".  */

int
child_thread_alive (pid)
     int pid;
{
   return 1;
}

#endif /* ! GDB_NATIVE_HPUX_11 */
