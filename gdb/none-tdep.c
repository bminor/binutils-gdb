/* Target-dependent code for none, architecture independent.

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
#include "gdbtypes.h"
#include "none-tdep.h"
#include "target.h"
#include "gdbthread.h"
#include "gdbcore.h"
#include "regcache.h"
#include "regset.h"
#include "elf/common.h"
#include "elf-bfd.h"            /* for elfcore_write_* */
#include "inferior.h"
#include "cli/cli-utils.h"
#include "arch-utils.h"
#include "gdb_obstack.h"
#include "observable.h"
#include "objfiles.h"
#include "infcall.h"
#include "gdbcmd.h"
#include "gdb_regex.h"
#include "gdbsupport/enum-flags.h"
#include "gdbsupport/gdb_optional.h"

#include <ctype.h>

using none_collect_thread_registers_ftype
  = void (const struct regcache *, ptid_t, bfd *,
	  gdb::unique_xmalloc_ptr<char> &, int *, enum gdb_signal);

/* The ARM none corefile setup is based on the ARM GNU/Linux design.  */

/* Structure for passing information from
   none_collect_thread_registers via an iterator to
   none_collect_regset_section_cb.  */

struct none_collect_regset_section_cb_data
{
  none_collect_regset_section_cb_data (struct gdbarch *gdbarch,
				       const struct regcache *regcache,
				       bfd *obfd,
				       gdb::unique_xmalloc_ptr<char> &note_data,
				       int *note_size,
				       unsigned long lwp,
				       enum gdb_signal stop_signal)
    : gdbarch (gdbarch), regcache (regcache), obfd (obfd),
      note_data (note_data), note_size (note_size),
      lwp (lwp), stop_signal (stop_signal)
  {}

  struct gdbarch *gdbarch;
  const struct regcache *regcache;
  bfd *obfd;
  gdb::unique_xmalloc_ptr<char> &note_data;
  int *note_size;
  unsigned long lwp;
  enum gdb_signal stop_signal;
  bool abort_iteration = false;
};

/* Callback for iterate_over_regset_sections that records a single
   regset in the corefile note section.  */

static void
none_collect_regset_section_cb (const char *sect_name, int supply_size,
				int collect_size, const struct regset *regset,
				const char *human_name, void *cb_data)
{
  struct none_collect_regset_section_cb_data *data
    = (struct none_collect_regset_section_cb_data *) cb_data;
  bool variable_size_section = (regset != nullptr
				&& regset->flags & REGSET_VARIABLE_SIZE);

  if (!variable_size_section)
    gdb_assert (supply_size == collect_size);

  if (data->abort_iteration)
    return;

  gdb_assert (regset && regset->collect_regset);

  /* This is intentionally zero-initialized by using std::vector, so
     that any padding bytes in the core file will show as 0.  */
  std::vector<gdb_byte> buf (collect_size);

  regset->collect_regset (regset, data->regcache, -1, buf.data (),
			  collect_size);

  /* PRSTATUS still needs to be treated specially.  */
  if (strcmp (sect_name, ".reg") == 0)
    data->note_data.reset (elfcore_write_prstatus
			     (data->obfd, data->note_data.release (),
			      data->note_size, data->lwp,
			      gdb_signal_to_host (data->stop_signal),
			      buf.data ()));
  else
    data->note_data.reset (elfcore_write_register_note
			     (data->obfd, data->note_data.release (),
			      data->note_size, sect_name, buf.data (),
			      collect_size));

  if (data->note_data == nullptr)
    data->abort_iteration = 1;
}

/* Records the thread's register state for the corefile note
   section.  */

static void
none_collect_thread_registers (const struct regcache *regcache,
			       ptid_t ptid, bfd *obfd,
			       gdb::unique_xmalloc_ptr<char> &note_data,
			       int *note_size, enum gdb_signal stop_signal)
{
  struct gdbarch *gdbarch = regcache->arch ();
  unsigned long lwp;

  /* For remote targets the LWP may not be available, so use the TID.  */
  lwp = ptid.lwp ();
  if (lwp == 0)
    lwp = ptid.tid ();

  none_collect_regset_section_cb_data data (gdbarch, regcache, obfd, note_data,
					    note_size, lwp, stop_signal);

  gdbarch_iterate_over_regset_sections (gdbarch,
					none_collect_regset_section_cb,
					&data, regcache);
}

struct none_corefile_thread_data
{
  none_corefile_thread_data (struct gdbarch *gdbarch,
			     bfd *obfd,
			     gdb::unique_xmalloc_ptr<char> &note_data,
			     int *note_size,
			     enum gdb_signal stop_signal)
    : gdbarch (gdbarch), obfd (obfd), note_data (note_data),
      note_size (note_size), stop_signal (stop_signal)
  {}

  struct gdbarch *gdbarch;
  bfd *obfd;
  gdb::unique_xmalloc_ptr<char> &note_data;
  int *note_size;
  enum gdb_signal stop_signal;
};

/* Records the thread's register state for the corefile note
   section.  */

static void
none_corefile_thread (struct thread_info *info,
		      struct none_corefile_thread_data *args)
{
  struct regcache *regcache;

  regcache = get_thread_arch_regcache (info->inf->process_target (),
				       info->ptid, args->gdbarch);

  target_fetch_registers (regcache, -1);
  none_collect_thread_registers
    (regcache, info->ptid, args->obfd, args->note_data,
     args->note_size, args->stop_signal);
}

/* Find the signalled thread.  In case there's more than one signalled
   thread, prefer the current thread, if it is signalled.  If no
   thread was signalled, default to the current thread, unless it has
   exited, in which case return nullptr.  */

static thread_info *
find_signalled_thread ()
{
  thread_info *curr_thr = inferior_thread ();
  if (curr_thr->state != THREAD_EXITED
      && curr_thr->suspend.stop_signal != GDB_SIGNAL_0)
    return curr_thr;

  for (thread_info *thr : current_inferior ()->non_exited_threads ())
    if (thr->suspend.stop_signal != GDB_SIGNAL_0)
      return thr;

  /* Default to the current thread, unless it has exited.  */
  if (curr_thr->state != THREAD_EXITED)
    return curr_thr;

  return nullptr;
}

/* Fills the "to_make_corefile_note" target vector.  Builds the note
   section for a corefile, and returns it in a malloc buffer.  */

static gdb::unique_xmalloc_ptr<char>
none_make_corefile_notes_1 (struct gdbarch *gdbarch, bfd *obfd, int *note_size,
			    none_collect_thread_registers_ftype collect)
{
  gdb::unique_xmalloc_ptr<char> note_data;

  /* Process information.  */
  if (get_exec_file (0))
    {
      const char *fname = lbasename (get_exec_file (0));
      std::string psargs = fname;

      if (get_inferior_args ())
	{
	  psargs += " ";
	  psargs += get_inferior_args ();
	}

      note_data.reset (elfcore_write_prpsinfo (obfd, note_data.release (),
					       note_size, fname,
					       psargs.c_str ()));
    }

  if (!note_data)
    return nullptr;

  /* Thread register information.  */
  try
    {
      update_thread_list ();
    }
  catch (const gdb_exception_error &e)
    {
      exception_print (gdb_stderr, e);
    }

  /* Like the kernel, prefer dumping the signalled thread first.
     "First thread" is what tools use to infer the signalled
     thread.  */
  thread_info *signalled_thr = find_signalled_thread ();
  gdb_signal stop_signal;

  if (signalled_thr != nullptr)
    stop_signal = signalled_thr->suspend.stop_signal;
  else
    stop_signal = GDB_SIGNAL_0;

  none_corefile_thread_data thread_args (gdbarch, obfd, note_data, note_size,
					 stop_signal);

  if (signalled_thr != nullptr)
    none_corefile_thread (signalled_thr, &thread_args);
  for (thread_info *thr : current_inferior ()->non_exited_threads ())
    {
      if (thr == signalled_thr)
	continue;

      none_corefile_thread (thr, &thread_args);
    }

  return note_data;
}

static gdb::unique_xmalloc_ptr<char>
none_make_corefile_notes (struct gdbarch *gdbarch, bfd *obfd, int *note_size)
{
  return none_make_corefile_notes_1 (gdbarch, obfd, note_size,
                                     none_collect_thread_registers);
}

/* Setup default core file support for none targets.
   Can be overridden later by OSABI.  */

void
none_init_corefile (struct gdbarch_info info,
		    struct gdbarch *gdbarch)
{
  /* Default core file support.  */
  set_gdbarch_make_corefile_notes (gdbarch, none_make_corefile_notes);
}
