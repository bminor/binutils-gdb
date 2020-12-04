/* Displaced stepping related things.

   Copyright (C) 2020 Free Software Foundation, Inc.

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

#include "defs.h"
#include "displaced-stepping.h"

#include "cli/cli-cmds.h"
#include "command.h"
#include "gdbarch.h"
#include "gdbcore.h"
#include "gdbthread.h"
#include "inferior.h"
#include "regcache.h"
#include "target/target.h"

/* Default destructor for displaced_step_copy_insn_closure.  */

displaced_step_copy_insn_closure::~displaced_step_copy_insn_closure ()
  = default;

bool debug_displaced = false;

static void
show_debug_displaced (struct ui_file *file, int from_tty,
		      struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Displace stepping debugging is %s.\n"), value);
}

displaced_step_prepare_status
displaced_step_buffer::prepare (thread_info *thread, CORE_ADDR &displaced_pc)
{
  gdb_assert (!thread->displaced_step_state.in_progress ());

  /* Is a thread currently using the buffer?  */
  if (m_current_thread != nullptr)
    {
      /* If so, it better not be this thread.  */
      gdb_assert (thread != m_current_thread);
      return DISPLACED_STEP_PREPARE_STATUS_UNAVAILABLE;
    }

  regcache *regcache = get_thread_regcache (thread);
  const address_space *aspace = regcache->aspace ();
  gdbarch *arch = regcache->arch ();
  ULONGEST len = gdbarch_max_insn_length (arch);

  if (breakpoint_in_range_p (aspace, m_addr, len))
    {
      /* There's a breakpoint set in the scratch pad location range
	 (which is usually around the entry point).  We'd either
	 install it before resuming, which would overwrite/corrupt the
	 scratch pad, or if it was already inserted, this displaced
	 step would overwrite it.  The latter is OK in the sense that
	 we already assume that no thread is going to execute the code
	 in the scratch pad range (after initial startup) anyway, but
	 the former is unacceptable.  Simply punt and fallback to
	 stepping over this breakpoint in-line.  */
      displaced_debug_printf ("breakpoint set in scratch pad.  "
			      "Stepping over breakpoint in-line instead.");

      return DISPLACED_STEP_PREPARE_STATUS_CANT;
    }

  m_original_pc = regcache_read_pc (regcache);
  displaced_pc = m_addr;

  /* Save the original contents of the displaced stepping buffer.  */
  m_saved_copy.resize (len);

  int status = target_read_memory (m_addr, m_saved_copy.data (), len);
  if (status != 0)
    throw_error (MEMORY_ERROR,
		 _("Error accessing memory address %s (%s) for "
		   "displaced-stepping scratch space."),
		 paddress (arch, m_addr), safe_strerror (status));

  displaced_debug_printf ("saved %s: %s",
			  paddress (arch, m_addr),
			  displaced_step_dump_bytes
			    (m_saved_copy.data (), len).c_str ());

  /* Save this in a local variable first, so it's released if code below
     throws.  */
  displaced_step_copy_insn_closure_up copy_insn_closure
    = gdbarch_displaced_step_copy_insn (arch, m_original_pc, m_addr, regcache);

  if (copy_insn_closure == nullptr)
    {
      /* The architecture doesn't know how or want to displaced step
        this instruction or instruction sequence.  Fallback to
        stepping over the breakpoint in-line.  */
      return DISPLACED_STEP_PREPARE_STATUS_CANT;
    }

  /* Resume execution at the copy.  */
  regcache_write_pc (regcache, m_addr);

  /* This marks the buffer as being in use.  */
  m_current_thread = thread;

  /* Save this, now that we know everything went fine.  */
  m_copy_insn_closure = std::move (copy_insn_closure);

  /* Tell infrun not to try preparing a displaced step again for this inferior.  */
  thread->inf->displaced_step_state.unavailable = true;

  return DISPLACED_STEP_PREPARE_STATUS_OK;
}

static void
write_memory_ptid (ptid_t ptid, CORE_ADDR memaddr,
		   const gdb_byte *myaddr, int len)
{
  scoped_restore save_inferior_ptid = make_scoped_restore (&inferior_ptid);

  inferior_ptid = ptid;
  write_memory (memaddr, myaddr, len);
}

static bool
displaced_step_instruction_executed_successfully (gdbarch *arch,
						  gdb_signal signal)
{
  if (signal != GDB_SIGNAL_TRAP)
    return false;

  if (target_stopped_by_watchpoint ())
    {
      if (gdbarch_have_nonsteppable_watchpoint (arch)
	  || target_have_steppable_watchpoint ())
	return false;
    }

  return true;
}

displaced_step_finish_status
displaced_step_buffer::finish (gdbarch *arch, thread_info *thread,
			       gdb_signal sig)
{
  gdb_assert (thread->displaced_step_state.in_progress ());
  gdb_assert (thread == m_current_thread);

  /* Move this to a local variable so it's released in case something goes
     wrong.  */
  displaced_step_copy_insn_closure_up copy_insn_closure
    = std::move (m_copy_insn_closure);
  gdb_assert (copy_insn_closure != nullptr);

  /* Reset M_CURRENT_THREAD immediately to mark the buffer as available, in case
     something goes wrong below.  */
  m_current_thread = nullptr;

  /* Now that a buffer gets freed, tell infrun it can ask us to prepare a displaced
     step again for this inferior.  Do that here in case something goes wrong
     below.  */
  thread->inf->displaced_step_state.unavailable = false;

  ULONGEST len = gdbarch_max_insn_length (arch);

  write_memory_ptid (thread->ptid, m_addr,
		     m_saved_copy.data (), len);

  displaced_debug_printf ("restored %s %s",
			  target_pid_to_str (thread->ptid).c_str (),
			  paddress (arch, m_addr));

  regcache *rc = get_thread_regcache (thread);

  bool instruction_executed_successfully
    = displaced_step_instruction_executed_successfully (arch, sig);

  if (instruction_executed_successfully)
    {
      gdbarch_displaced_step_fixup (arch, copy_insn_closure.get (), m_original_pc,
				    m_addr, rc);
      return DISPLACED_STEP_FINISH_STATUS_OK;
    }
  else
    {
      /* Since the instruction didn't complete, all we can do is relocate the
	 PC.  */
      CORE_ADDR pc = regcache_read_pc (rc);
      pc = m_original_pc + (pc - m_addr);
      regcache_write_pc (rc, pc);
      return DISPLACED_STEP_FINISH_STATUS_NOT_EXECUTED;
    }
}

const displaced_step_copy_insn_closure *
displaced_step_buffer::copy_insn_closure_by_addr (CORE_ADDR addr)
{
  if (addr == m_addr)
    return m_copy_insn_closure.get ();
  else
    return nullptr;
}

void
displaced_step_buffer::restore_in_ptid (ptid_t ptid)
{
  if (m_current_thread != nullptr)
    {
      regcache *regcache = get_thread_regcache (m_current_thread);
      gdbarch *arch = regcache->arch ();
      ULONGEST len = gdbarch_max_insn_length (arch);

      write_memory_ptid (ptid, m_addr, m_saved_copy.data (), len);

      displaced_debug_printf ("restored in ptid %s %s",
			      target_pid_to_str (ptid).c_str (),
			      paddress (arch, m_addr));
    }
}

void _initialize_displaced_stepping ();
void
_initialize_displaced_stepping ()
{
  add_setshow_boolean_cmd ("displaced", class_maintenance,
			   &debug_displaced, _("\
Set displaced stepping debugging."), _("\
Show displaced stepping debugging."), _("\
When non-zero, displaced stepping specific debugging is enabled."),
			    NULL,
			    show_debug_displaced,
			    &setdebuglist, &showdebuglist);
}
