/* Low level interface for debugging Solaris threads for GDB, the GNU debugger.
   Copyright 1996 Free Software Foundation, Inc.

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

#ifdef gregset_t
#undef gregset_t
#endif

#ifdef fpregset_t
#undef fpregset_t
#endif

#include "/usr/include/thread.h"
#include <proc_service.h>
#include <thread_db.h>
#include "gdbthread.h"
#include "target.h"
#include "inferior.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

static void sol_thread_resume PARAMS ((int pid, int step,
				       enum target_signal signo));

extern struct target_ops sol_thread_ops; /* Forward declaration */

extern int procfs_suppress_run;

struct ps_prochandle
{
  pid_t pid;
};

static struct ps_prochandle main_ph;
static td_thragent_t *main_ta;

static int sol_thread_active = 0;



extern struct target_ops procfs_ops;

/* Convert thread_id to an LWP id.  */

static int thread_to_lwp PARAMS ((int thread_id, int default_lwp));

static int
thread_to_lwp (thread_id, default_lwp)
     int thread_id;
     int default_lwp;
{
  td_thrinfo_t ti;
  td_thrhandle_t th;
  td_err_e val;
  int pid;
  int lwp;

  if (!(thread_id & 0x80000000))
    return thread_id;			/* It's already an LWP id */

  /* It's a thread.  Convert to lwp */

  pid = thread_id & 0xffff;
  thread_id = (thread_id >> 16) & 0x7fff;

  val = td_ta_map_id2thr (main_ta, thread_id, &th);
  if (val != TD_OK)
    error ("thread_to_lwp: td_ta_map_id2thr %d", val);

  val = td_thr_get_info (&th, &ti);

  if (val != TD_OK)
    error ("thread_to_lwp: td_thr_get_info: %d", val);

  if (ti.ti_state != TD_THR_ACTIVE)
    {
      if (default_lwp != -1)
	return default_lwp;
      error ("thread_to_lwp: thread state not active: %d", ti.ti_state);
    }
  
  lwp = (ti.ti_lid << 16) | pid;

  return lwp;
}

/* Convert an LWP id to a thread. */

static int lwp_to_thread PARAMS ((int lwp));

static int
lwp_to_thread (lwp)
     int lwp;
{
  td_thrinfo_t ti;
  td_thrhandle_t th;
  td_err_e val;
  int pid;
  int thread_id;

  if (lwp & 0x80000000)
    return lwp;			/* It's already a thread id */

  /* It's an lwp.  Convert it to a thread id.  */

  pid = lwp & 0xffff;
  lwp = (lwp >> 16) & 0xffff;

  val = td_ta_map_lwp2thr (main_ta, lwp, &th);
  if (val != TD_OK)
    error ("lwp_to_thread: td_thr_get_info: %d.", val);

  val = td_thr_get_info (&th, &ti);

  if (val != TD_OK)
    error ("lwp_to_thread: td_thr_get_info: %d.", val);

  thread_id = (ti.ti_tid << 16) | pid | 0x80000000;

  return thread_id;
}


/* ARGSUSED */
static void
sol_thread_open (arg, from_tty)
     char *arg;
     int from_tty;
{
  procfs_ops.to_open (arg, from_tty);
}

/* Attach to process PID, then initialize for debugging it
   and wait for the trace-trap that results from attaching.  */

static void
sol_thread_attach (args, from_tty)
     char *args;
     int from_tty;
{
  procfs_ops.to_attach (args, from_tty);

  /* XXX - might want to iterate over all the threads and register them. */
}

/* Take a program previously attached to and detaches it.
   The program resumes execution and will no longer stop
   on signals, etc.  We'd better not have left any breakpoints
   in the program or it'll die when it hits one.  For this
   to work, it may be necessary for the process to have been
   previously attached.  It *might* work if the program was
   started via the normal ptrace (PTRACE_TRACEME).  */

static void
sol_thread_detach (args, from_tty)
     char *args;
     int from_tty;
{
  procfs_ops.to_detach (args, from_tty);
}

/* Resume execution of process PID.  If STEP is nozero, then
   just single step it.  If SIGNAL is nonzero, restart it with that
   signal activated.  */

static void
sol_thread_resume (pid, step, signo)
     int pid;
     int step;
     enum target_signal signo;
{
  int save_pid;

  save_pid = inferior_pid;

  inferior_pid = thread_to_lwp (inferior_pid, main_ph.pid);

  if (pid != -1)
    pid = thread_to_lwp (pid, -1);

  procfs_ops.to_resume (pid, step, signo);

  inferior_pid = save_pid;
}

/* Wait for any LWPs to stop */

static int
sol_thread_wait (pid, ourstatus)
     int pid;
     struct target_waitstatus *ourstatus;
{
  int statval;
  int rtnval;
  int save_pid;

  if (!sol_thread_active)
    return procfs_ops.to_wait (pid, ourstatus);

  save_pid = inferior_pid;

  inferior_pid = thread_to_lwp (inferior_pid, main_ph.pid);

  if (pid != -1)
    pid = thread_to_lwp (pid, -1);

  rtnval = procfs_ops.to_wait (pid, ourstatus);

  if (rtnval != save_pid
      && !in_thread_list (rtnval))
    {
      fprintf_unfiltered (gdb_stderr, "[New %s]\n",
			  target_pid_to_str (rtnval));
      add_thread (rtnval);
    }

  inferior_pid = save_pid;	/* XXX need to make a cleanup for this in case of error */

  /* During process initialization, we may get here without the thread package
     being initialized, since that can only happen after we've found the shared
     libs.  */

  /* Map the LWP of interest back to the appropriate thread ID */

  rtnval = lwp_to_thread (rtnval);

  return rtnval;
}

static void
sol_thread_fetch_registers (regno)
     int regno;
{
  thread_t thread;
  td_thrhandle_t thandle;
  td_err_e val;
  prgregset_t regset;
  prfpregset_t fpregset;
  int xregsize;
  caddr_t xregset;

  if (!sol_thread_active
      || !(inferior_pid & 0x80000000))
    {
      procfs_ops.to_fetch_registers (regno);
      return;
    }

  /* Convert inferior_pid into a td_thrhandle_t */

  thread = (inferior_pid >> 16) & 0x7fff;

  if (thread == 0)
    error ("sol_thread_fetch_registers:  thread == 0");

  val = td_ta_map_id2thr (main_ta, thread, &thandle);
  if (val != TD_OK)
    error ("sol_thread_fetch_registers: td_ta_map_id2thr: %d", val);

  /* Get the integer regs */

  val = td_thr_getgregs (&thandle, regset);
  if (val == TD_OK)
    supply_gregset (regset);
  else if (val == TD_PARTIALREG)
    {
      /* For the sparc, only i0->i7, l0->l7, pc and sp are saved by a thread
	 context switch.  */

      supply_gregset (regset);	/* This is not entirely correct, as it sets
				   the valid bits for the o, g, ps, y, npc,
				   wim and tbr.  That should be harmless
				   though, as the context switch routine
				   doesn't need to save them.  */
    }
  else
    error ("sol_thread_fetch_registers: td_thr_getgregs %d", val);

  /* And, now the fp regs */

  val = td_thr_getfpregs (&thandle, &fpregset);
  if (val == TD_OK)
    supply_fpregset (&fpregset);
  else if (val != TD_NOFPREGS)
    error ("sol_thread_fetch_registers: td_thr_getfpregs %d", val);

#if 0
/* thread_db doesn't seem to handle this right */
  val = td_thr_getxregsize (&thandle, &xregsize);
  if (val != TD_OK && val != TD_NOXREGS)
    error ("sol_thread_fetch_registers: td_thr_getxregsize %d", val);

  if (val == TD_OK)
    {
      xregset = alloca (xregsize);
      val = td_thr_getxregs (&thandle, xregset);
      if (val != TD_OK)
	error ("sol_thread_fetch_registers: td_thr_getxregs %d", val);
    }
#endif
}

static void
sol_thread_store_registers (regno)
     int regno;
{
  thread_t thread;
  td_thrhandle_t thandle;
  td_err_e val;
  prgregset_t regset;
  prfpregset_t fpregset;
  int xregsize;
  caddr_t xregset;

  if (!sol_thread_active
      || !(inferior_pid & 0x80000000))
    {
      procfs_ops.to_store_registers (regno);
      return;
    }

  /* Convert inferior_pid into a td_thrhandle_t */

  thread = (inferior_pid >> 16) & 0x7fff;

  val = td_ta_map_id2thr (main_ta, thread, &thandle);
  if (val != TD_OK)
    error ("sol_thread_store_registers: td_ta_map_id2thr %d", val);

  if (regno != -1)
    {				/* Not writing all the regs */
      val = td_thr_getgregs (&thandle, regset);
      if (val != TD_OK)
	error ("sol_thread_store_registers: td_thr_getgregs %d", val);
      val = td_thr_getfpregs (&thandle, &fpregset);
      if (val != TD_OK)
	error ("sol_thread_store_registers: td_thr_getfpregs %d", val);

#if 0
/* thread_db doesn't seem to handle this right */
      val = td_thr_getxregsize (&thandle, &xregsize);
      if (val != TD_OK && val != TD_NOXREGS)
	error ("sol_thread_store_registers: td_thr_getxregsize %d", val);

      if (val == TD_OK)
	{
	  xregset = alloca (xregsize);
	  val = td_thr_getxregs (&thandle, xregset);
	  if (val != TD_OK)
	    error ("sol_thread_store_registers: td_thr_getxregs %d", val);
	}
#endif
    }

  fill_gregset (regset, regno);
  fill_fpregset (&fpregset, regno);

  val = td_thr_setgregs (&thandle, regset);
  if (val != TD_OK)
    error ("sol_thread_store_registers: td_thr_setgregs %d", val);
  val = td_thr_setfpregs (&thandle, &fpregset);
  if (val != TD_OK)
    error ("sol_thread_store_registers: td_thr_setfpregs %d", val);

#if 0
/* thread_db doesn't seem to handle this right */
  val = td_thr_getxregsize (&thandle, &xregsize);
  if (val != TD_OK && val != TD_NOXREGS)
    error ("sol_thread_store_registers: td_thr_getxregsize %d", val);

  /* Should probably do something about writing the xregs here, but what are
     they? */
#endif
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

static void
sol_thread_prepare_to_store ()
{
  procfs_ops.to_prepare_to_store ();
}

static int
sol_thread_xfer_memory (memaddr, myaddr, len, dowrite, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int dowrite;
     struct target_ops *target; /* ignored */
{
  int retval;
  int save_pid;

  save_pid = inferior_pid;

  if (inferior_pid & 0x80000000)
    inferior_pid = main_ph.pid;	/* It's a thread.  Convert to lwp */

  retval = procfs_ops.to_xfer_memory (memaddr, myaddr, len, dowrite, target);

  inferior_pid = save_pid;

  return retval;
}

/* Print status information about what we're accessing.  */

static void
sol_thread_files_info (ignore)
     struct target_ops *ignore;
{
  procfs_ops.to_files_info (ignore);
}

static void
sol_thread_kill_inferior ()
{
  procfs_ops.to_kill ();
}

static void
sol_thread_notice_signals (pid)
     int pid;
{
  procfs_ops.to_notice_signals (pid);
}

void target_new_objfile PARAMS ((struct objfile *objfile));

/* Fork an inferior process, and start debugging it with /proc.  */

static void
sol_thread_create_inferior (exec_file, allargs, env)
     char *exec_file;
     char *allargs;
     char **env;
{
  procfs_ops.to_create_inferior (exec_file, allargs, env);

  if (sol_thread_active)
    {
      td_thrhandle_t thandle;
      td_err_e val;
      td_thrinfo_t ti;

      main_ph.pid = inferior_pid; /* Save for xfer_memory */

      push_target (&sol_thread_ops);

      inferior_pid = lwp_to_thread (inferior_pid);

      add_thread (inferior_pid);
    }
}

/* This routine is called whenever a new symbol table is read in, or when all
   symbol tables are removed.  */

void
sol_thread_new_objfile (objfile)
     struct objfile *objfile;
{
  td_err_e val;

  if (!objfile)
    {
      sol_thread_active = 0;

      return;
    }

  /* Now, initialize the thread debugging library.  This needs to be done after
     the shared libraries are located because it needs information from the
     user's thread library.  */

  val = td_init ();
  if (val != TD_OK)
    error ("target_new_objfile: td_init: %d", val);

  val = td_ta_new (&main_ph, &main_ta);
  if (val == TD_NOLIBTHREAD)
    return;
  else if (val != TD_OK)
    error ("target_new_objfile: td_ta_new: %d", val);

  sol_thread_active = 1;
}

/* Clean up after the inferior dies.  */

static void
sol_thread_mourn_inferior ()
{
  procfs_ops.to_mourn_inferior ();
}

/* Mark our target-struct as eligible for stray "run" and "attach" commands.  */
static int
sol_thread_can_run ()
{
  return procfs_suppress_run;
}

int
sol_thread_alive (pid)
     int pid;
{
  return 1;
}

void
sol_thread_stop ()
{
  procfs_ops.to_stop ();
}

/* Service routines we must supply to libthread_db */

struct lwp_map
{
  struct lwp_map *next;
  pid_t pid;
  lwpid_t lwp;
  int lwpfd;
};

#if 0
struct lwp_map *lwp_map;

/* Create a /proc file descriptor for the given LWPID */

static ps_err_e
get_lwp_fd (const struct ps_prochandle *ph, const lwpid_t lwpid, int *fd)
{
  struct lwp_map *lp;

  for (lp = lwp_map; lp; lp = lp->next)
    if (lp->pid = ph->pid
	&& lp->lwp == lwpid)
      {
	*fd = lp->lwpfd;

	return PS_OK;
      }
	
  lp = xmalloc (sizeof (struct lwp_map));

  if ((lp->lwpfd = ioctl (ph->fd, PIOCOPENLWP, &lwpid)) < 0)
    {
      print_sys_errmsg ("get_lwp_fd (): PIOCOPENLWP", errno);
      return PS_BADLID;
    }

  lp->pid = ph->pid;
  lp->lwp = lwpid;
  lp->next = lwp_map;
  lwp_map = lp;

  *fd = lp->lwpfd;

  return PS_OK;
}
#endif

ps_err_e
ps_pstop (const struct ps_prochandle *ph)
{
#if 0
  if (ioctl (ph->fd, PIOCSTOP, NULL))
    {
      print_sys_errmsg ("ps_pstop (): PIOCSTOP", errno);
      return PS_ERR;
    }
#endif
  return PS_OK;
}

ps_err_e
ps_pcontinue (const struct ps_prochandle *ph)
{
#if 0
  if (ioctl (ph->fd, PIOCRUN, NULL))
    {
      print_sys_errmsg ("ps_pcontinue (): PIOCRUN", errno);
      return PS_ERR;
    }
#endif
  return PS_OK;
}

ps_err_e
ps_lstop (const struct ps_prochandle *ph, lwpid_t lwpid)
{
  int lwp_fd;
  ps_err_e val;

#if 0
  val = get_lwp_fd (ph, lwpid, &lwp_fd);
  if (val != PS_OK)
    return val;

  if (ioctl (lwp_fd, PIOCSTOP, NULL))
    {
      print_sys_errmsg ("ps_lstop (): PIOCSTOP", errno);
      return PS_ERR;
    }
#endif

  return PS_OK;
}

ps_err_e
ps_lcontinue (const struct ps_prochandle *ph, lwpid_t lwpid)
{
  int lwp_fd;
  ps_err_e val;

#if 0
  val = get_lwp_fd (ph, lwpid, &lwp_fd);
  if (val != PS_OK)
    return val;

  if (ioctl (lwp_fd, PIOCRUN, NULL))
    {
      print_sys_errmsg ("ps_lcontinue (): PIOCRUN", errno);
      return PS_ERR;
    }
#endif

  return PS_OK;
}

ps_err_e
ps_pglobal_lookup (const struct ps_prochandle *ph, const char *ld_object_name,
		   const char *ld_symbol_name, paddr_t *ld_symbol_addr)
{
  struct minimal_symbol *ms;

  ms = lookup_minimal_symbol (ld_symbol_name, NULL, NULL);

  if (!ms)
    return PS_NOSYM;

  *ld_symbol_addr = SYMBOL_VALUE_ADDRESS (ms);

  return PS_OK;
}

static ps_err_e
rw_common (int dowrite, const struct ps_prochandle *ph, paddr_t addr,
	   char *buf, int size)
{
  int save_pid;

  save_pid = inferior_pid;

  if (inferior_pid & 0x80000000)
    inferior_pid = main_ph.pid;	/* It's a thread.  Convert to lwp */

  while (size > 0)
    {
      int cc;

      cc = procfs_ops.to_xfer_memory (addr, buf, size, dowrite, &procfs_ops);

      if (cc < 0)
	{
	  if (dowrite == 0)
	    print_sys_errmsg ("ps_pdread (): read", errno);
	  else
	    print_sys_errmsg ("ps_pdread (): write", errno);

	  inferior_pid = save_pid;

	  return PS_ERR;
	}
      size -= cc;
      buf += cc;
    }

  inferior_pid = save_pid;

  return PS_OK;
}

ps_err_e
ps_pdread (const struct ps_prochandle *ph, paddr_t addr, char *buf, int size)
{
  return rw_common (0, ph, addr, buf, size);
}

ps_err_e
ps_pdwrite (const struct ps_prochandle *ph, paddr_t addr, char *buf, int size)
{
  return rw_common (1, ph, addr, buf, size);
}

ps_err_e
ps_ptread (const struct ps_prochandle *ph, paddr_t addr, char *buf, int size)
{
  return rw_common (0, ph, addr, buf, size);
}

ps_err_e
ps_ptwrite (const struct ps_prochandle *ph, paddr_t addr, char *buf, int size)
{
  return rw_common (1, ph, addr, buf, size);
}

ps_err_e
ps_lgetregs (const struct ps_prochandle *ph, lwpid_t lwpid,
	     prgregset_t gregset)
{
  int save_pid;

  save_pid = inferior_pid;

  inferior_pid = (lwpid << 16) | (inferior_pid & 0xffff);
  
  procfs_ops.to_fetch_registers (-1);
  fill_gregset (gregset, -1);

  inferior_pid = save_pid;

  return PS_OK;
}

ps_err_e
ps_lsetregs (const struct ps_prochandle *ph, lwpid_t lwpid,
	     const prgregset_t gregset)
{
  int save_pid;

  save_pid = inferior_pid;

  inferior_pid = (lwpid << 16) | (inferior_pid & 0xffff);
  
  supply_gregset (gregset);
  procfs_ops.to_store_registers (-1);

  inferior_pid = save_pid;

  return PS_OK;
}

void
ps_plog (const char *fmt, ...)
{
  va_list args;

  va_start (args, fmt);

  vfprintf_filtered (gdb_stderr, fmt, args);
}

ps_err_e
ps_lgetxregsize (const struct ps_prochandle *ph, lwpid_t lwpid, int *xregsize)
{
  int lwp_fd;
  int regsize;
  ps_err_e val;

#if 0
  val = get_lwp_fd (ph, lwpid, &lwp_fd);
  if (val != PS_OK)
    return val;

  if (ioctl (lwp_fd, PIOCGXREGSIZE, &regsize))
    {
      if (errno == EINVAL)
	return PS_NOFREGS;	/* XXX Wrong code, but this is the closest
				   thing in proc_service.h  */

      print_sys_errmsg ("ps_lgetxregsize (): PIOCGXREGSIZE", errno);
      return PS_ERR;
    }
#endif

  return PS_OK;
}

ps_err_e
ps_lgetxregs (const struct ps_prochandle *ph, lwpid_t lwpid, caddr_t xregset)
{
  int lwp_fd;
  ps_err_e val;

#if 0
  val = get_lwp_fd (ph, lwpid, &lwp_fd);
  if (val != PS_OK)
    return val;

  if (ioctl (lwp_fd, PIOCGXREG, xregset))
    {
      print_sys_errmsg ("ps_lgetxregs (): PIOCGXREG", errno);
      return PS_ERR;
    }
#endif

  return PS_OK;
}

ps_err_e
ps_lsetxregs (const struct ps_prochandle *ph, lwpid_t lwpid, caddr_t xregset)
{
  int lwp_fd;
  ps_err_e val;

#if 0
  val = get_lwp_fd (ph, lwpid, &lwp_fd);
  if (val != PS_OK)
    return val;

  if (ioctl (lwp_fd, PIOCSXREG, xregset))
    {
      print_sys_errmsg ("ps_lsetxregs (): PIOCSXREG", errno);
      return PS_ERR;
    }
#endif

  return PS_OK;
}

ps_err_e
ps_lgetfpregs (const struct ps_prochandle *ph, lwpid_t lwpid,
	       prfpregset_t *fpregset)
{
  int save_pid;

  save_pid = inferior_pid;

  inferior_pid = (lwpid << 16) | (inferior_pid & 0xffff);

  procfs_ops.to_fetch_registers (-1);
  fill_fpregset (fpregset, -1);

  inferior_pid = save_pid;

  return PS_OK;
}

ps_err_e
ps_lsetfpregs (const struct ps_prochandle *ph, lwpid_t lwpid,
	       const prfpregset_t *fpregset)
{
  int save_pid;

  save_pid = inferior_pid;

  inferior_pid = (lwpid << 16) | (inferior_pid & 0xffff);
  
  supply_gregset (fpregset);
  procfs_ops.to_store_registers (-1);

  inferior_pid = save_pid;

  return PS_OK;
}

char *
solaris_pid_to_str (pid)
     int pid;
{
  static char buf[100];

  if (pid & 0x80000000)
    {
      int lwp;

      lwp = thread_to_lwp (pid, -2);

      if (lwp != -2)
	sprintf (buf, "Thread %d (LWP %d)", (pid >> 16) & 0x7fff,
		 (lwp >> 16) & 0xffff);
      else
	sprintf (buf, "Thread %d        ", (pid >> 16) & 0x7fff);
    }
  else
    sprintf (buf, "LWP    %d        ", (pid >> 16) & 0xffff);

  return buf;
}

struct target_ops sol_thread_ops = {
  "solaris-threads",		/* to_shortname */
  "Solaris threads and pthread.", /* to_longname */
  "Solaris threads and pthread support.", /* to_doc */
  sol_thread_open,		/* to_open */
  0,				/* to_close */
  sol_thread_attach,			/* to_attach */
  sol_thread_detach, 		/* to_detach */
  sol_thread_resume,			/* to_resume */
  sol_thread_wait,			/* to_wait */
  sol_thread_fetch_registers,	/* to_fetch_registers */
  sol_thread_store_registers,	/* to_store_registers */
  sol_thread_prepare_to_store,	/* to_prepare_to_store */
  sol_thread_xfer_memory,		/* to_xfer_memory */
  sol_thread_files_info,		/* to_files_info */
  memory_insert_breakpoint,	/* to_insert_breakpoint */
  memory_remove_breakpoint,	/* to_remove_breakpoint */
  terminal_init_inferior,	/* to_terminal_init */
  terminal_inferior, 		/* to_terminal_inferior */
  terminal_ours_for_output,	/* to_terminal_ours_for_output */
  terminal_ours,		/* to_terminal_ours */
  child_terminal_info,		/* to_terminal_info */
  sol_thread_kill_inferior,	/* to_kill */
  0,				/* to_load */
  0,				/* to_lookup_symbol */
  sol_thread_create_inferior,	/* to_create_inferior */
  sol_thread_mourn_inferior,	/* to_mourn_inferior */
  sol_thread_can_run,		/* to_can_run */
  sol_thread_notice_signals,	/* to_notice_signals */
  sol_thread_alive,		/* to_thread_alive */
  sol_thread_stop,		/* to_stop */
  process_stratum,		/* to_stratum */
  0,				/* to_next */
  1,				/* to_has_all_memory */
  1,				/* to_has_memory */
  1,				/* to_has_stack */
  1,				/* to_has_registers */
  1,				/* to_has_execution */
  0,				/* sections */
  0,				/* sections_end */
  OPS_MAGIC			/* to_magic */
};

void
_initialize_sol_thread ()
{
  add_target (&sol_thread_ops);

  procfs_suppress_run = 1;
}
