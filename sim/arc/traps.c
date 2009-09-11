/* arc exception, interrupt, and trap (EIT) support
   Copyright (C) 1998, 2003, 2004, 2005 Free Software Foundation, Inc.

   This file is part of GDB, the GNU debugger.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "sim-main.h"
#include "targ-vals.h"
#include <sys/stat.h>
#include <sys/time.h>
#include "gdb/target-io/arc.h"

#define TRAP_FLUSH_CACHE 12

/* The semantic code invokes this for invalid (unrecognized) instructions.  */

SEM_PC
sim_engine_invalid_insn (SIM_CPU *current_cpu, IADDR cia, SEM_PC pc)
{
  SIM_DESC sd = CPU_STATE (current_cpu);

#if 0
  if (STATE_ENVIRONMENT (sd) == OPERATING_ENVIRONMENT)
    {
      h_bsm_set (current_cpu, h_sm_get (current_cpu));
      h_bie_set (current_cpu, h_ie_get (current_cpu));
      h_bcond_set (current_cpu, h_cond_get (current_cpu));
      /* sm not changed */
      h_ie_set (current_cpu, 0);
      h_cond_set (current_cpu, 0);

      h_bpc_set (current_cpu, cia);

      sim_engine_restart (CPU_STATE (current_cpu), current_cpu, NULL,
			  EIT_RSVD_INSN_ADDR);
    }
  else
#endif
    sim_engine_halt (sd, current_cpu, NULL, cia, sim_stopped, SIM_SIGILL);

  return pc;
}

/* Process an address exception.  */

void
arc_core_signal (SIM_DESC sd, SIM_CPU *current_cpu, sim_cia cia,
		  unsigned int map, int nr_bytes, address_word addr,
		  transfer_type transfer, sim_core_signals sig)
{
#if 0 /* FIXME */
  if (STATE_ENVIRONMENT (sd) == OPERATING_ENVIRONMENT)
    {
      m32rbf_h_cr_set (current_cpu, H_CR_BBPC,
		       m32rbf_h_cr_get (current_cpu, H_CR_BPC));
      switch (MACH_NUM (CPU_MACH (current_cpu)))
	{
	case MACH_M32R:
	  m32rbf_h_bpsw_set (current_cpu, m32rbf_h_psw_get (current_cpu));
	  /* sm not changed.  */
	  m32rbf_h_psw_set (current_cpu, m32rbf_h_psw_get (current_cpu) & 0x80);
	  break;
	case MACH_M32RX:
  	  m32rxf_h_bpsw_set (current_cpu, m32rxf_h_psw_get (current_cpu));
  	  /* sm not changed.  */
  	  m32rxf_h_psw_set (current_cpu, m32rxf_h_psw_get (current_cpu) & 0x80);
	  break;
	case MACH_M32R2:
	  m32r2f_h_bpsw_set (current_cpu, m32r2f_h_psw_get (current_cpu));
	  /* sm not changed.  */
	  m32r2f_h_psw_set (current_cpu, m32r2f_h_psw_get (current_cpu) & 0x80);
	  break;
	default:
	  abort ();
	}
	    
      m32rbf_h_cr_set (current_cpu, H_CR_BPC, cia);

      sim_engine_restart (CPU_STATE (current_cpu), current_cpu, NULL,
			  EIT_ADDR_EXCP_ADDR);
    }
  else
#endif
    sim_core_signal (sd, current_cpu, cia, map, nr_bytes, addr,
		     transfer, sig);
}

/* Read/write functions for system call interface.  */

static int
syscall_read_mem (host_callback *cb, struct cb_syscall *sc,
		  unsigned long taddr, char *buf, int bytes)
{
  SIM_DESC sd = (SIM_DESC) sc->p1;
  SIM_CPU *cpu = (SIM_CPU *) sc->p2;

  return sim_core_read_buffer (sd, cpu, read_map, buf, taddr, bytes);
}

static int
syscall_write_mem (host_callback *cb, struct cb_syscall *sc,
		   unsigned long taddr, const char *buf, int bytes)
{
  SIM_DESC sd = (SIM_DESC) sc->p1;
  SIM_CPU *cpu = (SIM_CPU *) sc->p2;

  return sim_core_write_buffer (sd, cpu, write_map, buf, taddr, bytes);
}

static CB_RC arc_syscall (host_callback *, CB_SYSCALL *);

/* Trap support.
   The result is the pc address to continue at.
   Preprocessing like saving the various registers has already been done.  */

USI
arc_trap (SIM_CPU *current_cpu, PCADDR pc, int insn_len, int num)
{
  SIM_DESC sd = CPU_STATE (current_cpu);
  host_callback *cb = STATE_CALLBACK (sd);

#ifndef LIBC_FIXED
if (num == TRAP_BREAKPOINT)
  num = TRAP_SYSCALL;
#endif
#ifdef SIM_HAVE_BREAKPOINTS
  /* Check for breakpoints "owned" by the simulator first, regardless
     of --environment.  */
  if (num == TRAP_BREAKPOINT)
    {
      /* First try sim-break.c.  If it's a breakpoint the simulator "owns"
	 it doesn't return.  Otherwise it returns and let's us try.  */
      sim_handle_breakpoint (sd, current_cpu, pc);
      /* Fall through.  */
    }
#endif

  if (STATE_ENVIRONMENT (sd) == OPERATING_ENVIRONMENT)
    {
#if 0
      /* The new pc is the trap vector entry.
	 We assume there's a branch there to some handler.
      /* USI new_pc = EIT_TRAP_BASE_ADDR + num * 4; */
      USI new_pc = /* A5 / ARC600 */
	a5f_h_auxr_get (current_cpu, 37 /* INT_VECTOR_BASE */) + 0x10;
      USI new_pc = /* ARC700 */
	a5f_h_auxr_get (current_cpu, 37 /* INT_VECTOR_BASE */) + 0x128;
      return new_pc;
#endif
abort ();
    }

  switch (num)
    {
    case TRAP_SYSCALL :
      {
	CB_SYSCALL s;

	CB_SYSCALL_INIT (&s);
	s.func = a5f_h_cr_get (current_cpu, 8);
	s.arg1 = a5f_h_cr_get (current_cpu, 0);
	s.arg2 = a5f_h_cr_get (current_cpu, 1);
	s.arg3 = a5f_h_cr_get (current_cpu, 2);
	s.arg4 = a5f_h_cr_get (current_cpu, 3);

	if (s.func == TARGET_SYS_exit)
	  {
	    sim_engine_halt (sd, current_cpu, NULL, pc, sim_exited, s.arg1);
	  }

	s.p1 = (PTR) sd;
	s.p2 = (PTR) current_cpu;
	s.read_mem = syscall_read_mem;
	s.write_mem = syscall_write_mem;
	arc_syscall (cb, &s);
	a5f_h_cr_set (current_cpu, 0, s.errcode ? -s.errcode : s.result);
	break;
      }

    case TRAP_BREAKPOINT:
      sim_engine_halt (sd, current_cpu, NULL, pc,
		       sim_stopped, SIM_SIGTRAP);
      break;

#if 0
    case TRAP_FLUSH_CACHE:
      /* Do nothing.  */
      break;
#endif

    default :
      {
#if 0
	/* USI new_pc = EIT_TRAP_BASE_ADDR + num * 4; */
        /* Use cr5 as EVB (EIT Vector Base) register.  */
        USI new_pc = m32rbf_h_cr_get (current_cpu, 5) + 0x40 + num * 4;
	return new_pc;
#endif
abort();
      }
    }

  return pc + insn_len;
}

void
arc_breakpoint (SIM_CPU *current_cpu, PCADDR pc, int insn_len)
{
  SIM_DESC sd = CPU_STATE (current_cpu);

  sim_engine_halt (sd, current_cpu, NULL, pc, sim_stopped, SIM_SIGTRAP);
}

/* If the generic simulator call said ENOSYS, then let's try the
   ones we know ourselves.

   The convention is to provide *very limited* functionality on an
   as-needed basis, only what's covered by the test-suite, tests
   added when functionality changes and abort with a descriptive
   message for *everything* else.  Where there's no test-case, we
   just abort.  */
static CB_RC
arc_syscall (host_callback *cb, CB_SYSCALL *s)
{
  SIM_CPU *current_cpu = s->p2;
  SIM_DESC sd = CPU_STATE (current_cpu);

  s->errcode = 0;
  switch (s->func)
    {
    case TARGET_SYS_brk:
      /* Most often, we just return the argument, like the Linux
	 kernel.  */
      s->result = s->arg1;

      if (s->arg1 == 0)
	s->result = current_cpu->endbrk;
      else if (s->arg1 <= sd->memory.heap_end)
	current_cpu->endbrk = s->arg1;
      else
	{
	  s->result = -1;
	  s->errcode = TARGET_ENOMEM;
	  break;
	}
      break;
      {
	struct stat st;
	struct fio_arc_stat buf;
	int retval;
	int errcode;
	int e;
	char *path;

      case TARGET_SYS_stat:
	errcode = get_path (cb, s, s->arg1, &path);
	if (errcode)
	  {
	    s->result = -1;
	    s->errcode = cb_host_to_target_errno (cb, errcode);
	    break;
	  }
	retval = stat (path, &st);
	free (path);
	goto do_stat;
      case TARGET_SYS_fstat:
	retval = fdbad (cb, s->arg1);
	if (retval)
	  {
	    s->result = -1;
	    s->errcode = TARGET_EINVAL;
	    break;
	  }
	retval = fstat (fdmap (cb, s->arg1), &st);
      do_stat:

	s->result = retval;
	e = CURRENT_HOST_BYTE_ORDER ^ CURRENT_TARGET_BYTE_ORDER;
#define E2(v) (e ? swap_2(v) : (v))
#define E4(v) (e ? swap_4(v) : (v))
#define E8(v) (e ? swap_8(v) : (v))

	buf.tgt_st_dev		= E4 (st.st_dev);
	buf.tgt_st_ino		= E4 (st.st_ino);
	buf.tgt_st_mode		= E2 (st.st_mode);
	buf.tgt_st_nlink	= E2 (st.st_nlink);
	buf.tgt_st_uid		= E2 (st.st_uid);
	buf.tgt_st_gid		= E2 (st.st_gid);
	buf.tgt_st_rdev		= E4 (st.st_rdev);
	buf.tgt_st_size		= E4 (st.st_size);
	buf.tgt_st_blksize	= E4 (st.st_blksize);
	buf.tgt_st_blocks	= E4 (st.st_blocks);
	buf.tgt_st_atime	= E8 (st.st_atime);
	buf.tgt_st_mtime	= E8 (st.st_mtime);
	buf.tgt_st_ctime	= E8 (st.st_ctime);
	memset (&buf.tgt_st_reserved, 0, sizeof buf.tgt_st_reserved);
        if ((*s->write_mem) (cb, s, s->arg2, (char *) &buf, sizeof buf)
	    != sizeof buf)
	  {
	    s->result = -1;
	    s->errcode = TARGET_EINVAL;
	    break;
	  }
	if (!retval)
          break;
    handle_error:
	s->errcode = (*cb->get_errno) (cb);
      }
    case TARGET_SYS_gettimeofday:
      {
	struct timeval tv;
	struct timezone tz;
	struct fio_timeval buf;
	int retval;
	int e;

	if (s->arg2)
	  /* FIXME */;
	retval = gettimeofday (&tv, s->arg2 ? &tz : 0);
	s->result = retval;
	e = CURRENT_HOST_BYTE_ORDER ^ CURRENT_TARGET_BYTE_ORDER;
	buf.tgt_tv_sec = E8 (tv.tv_sec);
	buf.tgt_tv_usec = E4 (tv.tv_usec);
        if ((*s->write_mem) (cb, s, s->arg1, (char *) &buf, sizeof buf)
	    != sizeof buf)
	  {
	    s->result = -1;
	    s->errcode = TARGET_EINVAL;
	    break;
	  }
	if (retval)
	  goto handle_error;
        break;
	s->errcode = (*cb->get_errno) (cb);
      }
    case TARGET_SYS_setitimer:
      /* This implements only minimal functionality for prof-freq.c to work.  */
      if (s->arg1 != ITIMER_REAL)
	{
	  s->result = -1;
	  s->errcode = TARGET_EINVAL;
	  break;
	}
      s->result = 0;
      if (s->arg3)
	{
	  struct fio_timeval buf;
	  int e;

	  e = CURRENT_HOST_BYTE_ORDER ^ CURRENT_TARGET_BYTE_ORDER;
	  buf.tgt_tv_sec = E4 (0);
	  buf.tgt_tv_usec = E4 (100);
	  if ((*s->write_mem) (cb, s, s->arg3, (char *) &buf, sizeof buf)
	      != sizeof buf)
	    {
	      s->result = -1;
	      s->errcode = TARGET_EINVAL;
	      break;
	    }
	}
      break;
    case TARGET_SYS_profil:
      if (s->arg1)
	{
	  if (s->arg4 != 0x8000)
	    {
	      s->result = -1;
	      s->errcode = TARGET_EINVAL;
	      break;
	    }
	  a5f_h_ubit_set (current_cpu, 1);
	  a5f_h_prof_offset_set (current_cpu, 0, s->arg1 - (s->arg3 >> 1));
	  a5f_h_e1_set (current_cpu, 1);
	  a5f_h_auxr_set (current_cpu, 0x23 /* LIMIT0 */, 100);
	  a5f_h_auxr_set (current_cpu, 0x22 /* CONTROL0 */, 3);
	}
      else
	a5f_h_auxr_set (current_cpu, 0x22 /* CONTROL0 */, 0);
      s->result = 0;
      break;
    default:
      return cb_syscall (cb, s);
    }
  return CB_RC_OK;
}
