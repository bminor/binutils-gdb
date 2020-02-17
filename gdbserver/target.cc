/* Target operations for the remote server for GDB.
   Copyright (C) 2002-2020 Free Software Foundation, Inc.

   Contributed by MontaVista Software.

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

#include "server.h"
#include "tracepoint.h"
#include "gdbsupport/byte-vector.h"
#include "hostio.h"

process_stratum_target *the_target;

int
set_desired_thread ()
{
  client_state &cs = get_client_state ();
  thread_info *found = find_thread_ptid (cs.general_thread);

  current_thread = found;
  return (current_thread != NULL);
}

/* The thread that was current before prepare_to_access_memory was
   called.  done_accessing_memory uses this to restore the previous
   selected thread.  */
static ptid_t prev_general_thread;

/* See target.h.  */

int
prepare_to_access_memory (void)
{
  client_state &cs = get_client_state ();

  /* The first thread found.  */
  struct thread_info *first = NULL;
  /* The first stopped thread found.  */
  struct thread_info *stopped = NULL;
  /* The current general thread, if found.  */
  struct thread_info *current = NULL;

  /* Save the general thread value, since prepare_to_access_memory could change
     it.  */
  prev_general_thread = cs.general_thread;

  int res = the_target->pt->prepare_to_access_memory ();
  if (res != 0)
    return res;

  for_each_thread (prev_general_thread.pid (), [&] (thread_info *thread)
    {
      if (mythread_alive (thread->id))
	{
	  if (stopped == NULL && the_target->pt->supports_thread_stopped ()
	      && target_thread_stopped (thread))
	    stopped = thread;

	  if (first == NULL)
	    first = thread;

	  if (current == NULL && prev_general_thread == thread->id)
	    current = thread;
	}
    });

  /* The thread we end up choosing.  */
  struct thread_info *thread;

  /* Prefer a stopped thread.  If none is found, try the current
     thread.  Otherwise, take the first thread in the process.  If
     none is found, undo the effects of
     target->prepare_to_access_memory() and return error.  */
  if (stopped != NULL)
    thread = stopped;
  else if (current != NULL)
    thread = current;
  else if (first != NULL)
    thread = first;
  else
    {
      done_accessing_memory ();
      return 1;
    }

  current_thread = thread;
  cs.general_thread = ptid_of (thread);

  return 0;
}

/* See target.h.  */

void
done_accessing_memory (void)
{
  client_state &cs = get_client_state ();

  the_target->pt->done_accessing_memory ();

  /* Restore the previous selected thread.  */
  cs.general_thread = prev_general_thread;
  switch_to_thread (the_target, cs.general_thread);
}

int
read_inferior_memory (CORE_ADDR memaddr, unsigned char *myaddr, int len)
{
  int res;
  res = the_target->pt->read_memory (memaddr, myaddr, len);
  check_mem_read (memaddr, myaddr, len);
  return res;
}

/* See target/target.h.  */

int
target_read_memory (CORE_ADDR memaddr, gdb_byte *myaddr, ssize_t len)
{
  return read_inferior_memory (memaddr, myaddr, len);
}

/* See target/target.h.  */

int
target_read_uint32 (CORE_ADDR memaddr, uint32_t *result)
{
  return read_inferior_memory (memaddr, (gdb_byte *) result, sizeof (*result));
}

/* See target/target.h.  */

int
target_write_memory (CORE_ADDR memaddr, const unsigned char *myaddr,
		     ssize_t len)
{
  /* Make a copy of the data because check_mem_write may need to
     update it.  */
  gdb::byte_vector buffer (myaddr, myaddr + len);
  check_mem_write (memaddr, buffer.data (), myaddr, len);
  return the_target->pt->write_memory (memaddr, buffer.data (), len);
}

ptid_t
mywait (ptid_t ptid, struct target_waitstatus *ourstatus, int options,
	int connected_wait)
{
  ptid_t ret;

  if (connected_wait)
    server_waiting = 1;

  ret = target_wait (ptid, ourstatus, options);

  /* We don't expose _LOADED events to gdbserver core.  See the
     `dlls_changed' global.  */
  if (ourstatus->kind == TARGET_WAITKIND_LOADED)
    ourstatus->kind = TARGET_WAITKIND_STOPPED;

  /* If GDB is connected through TCP/serial, then GDBserver will most
     probably be running on its own terminal/console, so it's nice to
     print there why is GDBserver exiting.  If however, GDB is
     connected through stdio, then there's no need to spam the GDB
     console with this -- the user will already see the exit through
     regular GDB output, in that same terminal.  */
  if (!remote_connection_is_stdio ())
    {
      if (ourstatus->kind == TARGET_WAITKIND_EXITED)
	fprintf (stderr,
		 "\nChild exited with status %d\n", ourstatus->value.integer);
      else if (ourstatus->kind == TARGET_WAITKIND_SIGNALLED)
	fprintf (stderr, "\nChild terminated with signal = 0x%x (%s)\n",
		 gdb_signal_to_host (ourstatus->value.sig),
		 gdb_signal_to_name (ourstatus->value.sig));
    }

  if (connected_wait)
    server_waiting = 0;

  return ret;
}

/* See target/target.h.  */

void
target_stop_and_wait (ptid_t ptid)
{
  struct target_waitstatus status;
  bool was_non_stop = non_stop;
  struct thread_resume resume_info;

  resume_info.thread = ptid;
  resume_info.kind = resume_stop;
  resume_info.sig = GDB_SIGNAL_0;
  the_target->pt->resume (&resume_info, 1);

  non_stop = true;
  mywait (ptid, &status, 0, 0);
  non_stop = was_non_stop;
}

/* See target/target.h.  */

ptid_t
target_wait (ptid_t ptid, struct target_waitstatus *status, int options)
{
  return the_target->pt->wait (ptid, status, options);
}

/* See target/target.h.  */

void
target_mourn_inferior (ptid_t ptid)
{
  the_target->pt->mourn (find_process_pid (ptid.pid ()));
}

/* See target/target.h.  */

void
target_continue_no_signal (ptid_t ptid)
{
  struct thread_resume resume_info;

  resume_info.thread = ptid;
  resume_info.kind = resume_continue;
  resume_info.sig = GDB_SIGNAL_0;
  the_target->pt->resume (&resume_info, 1);
}

/* See target/target.h.  */

void
target_continue (ptid_t ptid, enum gdb_signal signal)
{
  struct thread_resume resume_info;

  resume_info.thread = ptid;
  resume_info.kind = resume_continue;
  resume_info.sig = gdb_signal_to_host (signal);
  the_target->pt->resume (&resume_info, 1);
}

/* See target/target.h.  */

int
target_supports_multi_process (void)
{
  return the_target->pt->supports_multi_process ();
}

void
set_target_ops (process_stratum_target *target)
{
  the_target = XNEW (process_stratum_target);
  memcpy (the_target, target, sizeof (*the_target));
}

/* Convert pid to printable format.  */

const char *
target_pid_to_str (ptid_t ptid)
{
  static char buf[80];

  if (ptid == minus_one_ptid)
    xsnprintf (buf, sizeof (buf), "<all threads>");
  else if (ptid == null_ptid)
    xsnprintf (buf, sizeof (buf), "<null thread>");
  else if (ptid.tid () != 0)
    xsnprintf (buf, sizeof (buf), "Thread %d.0x%lx",
	       ptid.pid (), ptid.tid ());
  else if (ptid.lwp () != 0)
    xsnprintf (buf, sizeof (buf), "LWP %d.%ld",
	       ptid.pid (), ptid.lwp ());
  else
    xsnprintf (buf, sizeof (buf), "Process %d",
	       ptid.pid ());

  return buf;
}

int
kill_inferior (process_info *proc)
{
  gdb_agent_about_to_close (proc->pid);

  return the_target->pt->kill (proc);
}

/* Default implementation for breakpoint_kind_for_pc.

   The default behavior for targets that don't implement breakpoint_kind_for_pc
   is to use the size of a breakpoint as the kind.  */

int
default_breakpoint_kind_from_pc (CORE_ADDR *pcptr)
{
  int size = 0;

  gdb_assert (the_target->sw_breakpoint_from_kind != NULL);

  (*the_target->sw_breakpoint_from_kind) (0, &size);
  return size;
}

/* Define it.  */

target_terminal_state target_terminal::m_terminal_state
  = target_terminal_state::is_ours;

/* See target/target.h.  */

void
target_terminal::init ()
{
  /* Placeholder needed because of fork_inferior.  Not necessary on
     GDBserver.  */
}

/* See target/target.h.  */

void
target_terminal::inferior ()
{
  /* Placeholder needed because of fork_inferior.  Not necessary on
     GDBserver.  */
}

/* See target/target.h.  */

void
target_terminal::ours ()
{
  /* Placeholder needed because of fork_inferior.  Not necessary on
     GDBserver.  */
}

/* See target/target.h.  */

void
target_terminal::ours_for_output (void)
{
  /* Placeholder.  */
}

/* See target/target.h.  */

void
target_terminal::info (const char *arg, int from_tty)
{
  /* Placeholder.  */
}

/* Default implementations of target ops.
   See target.h for definitions.  */

void
process_target::post_create_inferior ()
{
  /* Nop.  */
}

int
process_target::prepare_to_access_memory ()
{
  return 0;
}

void
process_target::done_accessing_memory ()
{
  /* Nop.  */
}

void
process_target::look_up_symbols ()
{
  /* Nop.  */
}

bool
process_target::supports_read_auxv ()
{
  return false;
}

int
process_target::read_auxv (CORE_ADDR offset, unsigned char *myaddr,
			   unsigned int len)
{
  gdb_assert_not_reached ("target op read_auxv not supported");
}

bool
process_target::supports_z_point_type (char z_type)
{
  return false;
}

int
process_target::insert_point (enum raw_bkpt_type type, CORE_ADDR addr,
			      int size, raw_breakpoint *bp)
{
  return 1;
}

int
process_target::remove_point (enum raw_bkpt_type type, CORE_ADDR addr,
			      int size, raw_breakpoint *bp)
{
  return 1;
}

bool
process_target::stopped_by_sw_breakpoint ()
{
  return false;
}

bool
process_target::supports_stopped_by_sw_breakpoint ()
{
  return false;
}

bool
process_target::stopped_by_hw_breakpoint ()
{
  return false;
}

bool
process_target::supports_stopped_by_hw_breakpoint ()
{
  return false;
}

bool
process_target::supports_hardware_single_step ()
{
  return false;
}

bool
process_target::stopped_by_watchpoint ()
{
  return false;
}

CORE_ADDR
process_target::stopped_data_address ()
{
  return 0;
}

bool
process_target::supports_read_offsets ()
{
  return false;
}

int
process_target::read_offsets (CORE_ADDR *text, CORE_ADDR *data)
{
  gdb_assert_not_reached ("target op read_offsets not supported");
}

bool
process_target::supports_get_tls_address ()
{
  return false;
}

int
process_target::get_tls_address (thread_info *thread, CORE_ADDR offset,
				 CORE_ADDR load_module, CORE_ADDR *address)
{
  gdb_assert_not_reached ("target op get_tls_address not supported");
}

void
process_target::hostio_last_error (char *buf)
{
  hostio_last_error_from_errno (buf);
}

bool
process_target::supports_qxfer_osdata ()
{
  return false;
}

int
process_target::qxfer_osdata (const char *annex, unsigned char *readbuf,
			      unsigned const char *writebuf,
			      CORE_ADDR offset, int len)
{
  gdb_assert_not_reached ("target op qxfer_osdata not supported");
}

bool
process_target::supports_qxfer_siginfo ()
{
  return false;
}

int
process_target::qxfer_siginfo (const char *annex, unsigned char *readbuf,
			       unsigned const char *writebuf,
			       CORE_ADDR offset, int len)
{
  gdb_assert_not_reached ("target op qxfer_siginfo not supported");
}

bool
process_target::supports_non_stop ()
{
  return false;
}

bool
process_target::async (bool enable)
{
  return false;
}

int
process_target::start_non_stop (bool enable)
{
  if (enable)
    return -1;
  else
    return 0;
}

bool
process_target::supports_multi_process ()
{
  return false;
}

bool
process_target::supports_fork_events ()
{
  return false;
}

bool
process_target::supports_vfork_events ()
{
  return false;
}

bool
process_target::supports_exec_events ()
{
  return false;
}

void
process_target::handle_new_gdb_connection ()
{
  /* Nop.  */
}

int
process_target::handle_monitor_command (char *mon)
{
  return 0;
}

int
process_target::core_of_thread (ptid_t ptid)
{
  return -1;
}

bool
process_target::supports_read_loadmap ()
{
  return false;
}

int
process_target::read_loadmap (const char *annex, CORE_ADDR offset,
			      unsigned char *myaddr, unsigned int len)
{
  gdb_assert_not_reached ("target op read_loadmap not supported");
}

void
process_target::process_qsupported (char **features, int count)
{
  /* Nop.  */
}

bool
process_target::supports_tracepoints ()
{
  return false;
}

CORE_ADDR
process_target::read_pc (regcache *regcache)
{
  gdb_assert_not_reached ("process_target::read_pc: Unable to find PC");
}

void
process_target::write_pc (regcache *regcache, CORE_ADDR pc)
{
  gdb_assert_not_reached ("process_target::write_pc: Unable to update PC");
}

bool
process_target::supports_thread_stopped ()
{
  return false;
}

bool
process_target::thread_stopped (thread_info *thread)
{
  gdb_assert_not_reached ("target op thread_stopped not supported");
}

bool
process_target::supports_get_tib_address ()
{
  return false;
}

int
process_target::get_tib_address (ptid_t ptid, CORE_ADDR *address)
{
  gdb_assert_not_reached ("target op get_tib_address not supported");
}

void
process_target::pause_all (bool freeze)
{
  /* Nop.  */
}

void
process_target::unpause_all (bool unfreeze)
{
  /* Nop.  */
}

void
process_target::stabilize_threads ()
{
  /* Nop.  */
}

bool
process_target::supports_fast_tracepoints ()
{
  return false;
}

int
process_target::install_fast_tracepoint_jump_pad
  (CORE_ADDR tpoint, CORE_ADDR tpaddr, CORE_ADDR collector,
   CORE_ADDR lockaddr, ULONGEST orig_size, CORE_ADDR *jump_entry,
   CORE_ADDR *trampoline, ULONGEST *trampoline_size,
   unsigned char *jjump_pad_insn, ULONGEST *jjump_pad_insn_size,
   CORE_ADDR *adjusted_insn_addr, CORE_ADDR *adjusted_insn_addr_end,
   char *err)
{
  gdb_assert_not_reached ("target op install_fast_tracepoint_jump_pad "
			  "not supported");
}

int
process_target::get_min_fast_tracepoint_insn_len ()
{
  return 0;
}

struct emit_ops *
process_target::emit_ops ()
{
  return nullptr;
}
