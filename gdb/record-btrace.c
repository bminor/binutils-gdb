/* Branch trace support for GDB, the GNU debugger.

   Copyright (C) 2013-2014 Free Software Foundation, Inc.

   Contributed by Intel Corp. <markus.t.metzger@intel.com>

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
#include "record.h"
#include "gdbthread.h"
#include "target.h"
#include "gdbcmd.h"
#include "disasm.h"
#include "observer.h"
#include "exceptions.h"
#include "cli/cli-utils.h"
#include "source.h"
#include "ui-out.h"
#include "symtab.h"
#include "filenames.h"

/* The target_ops of record-btrace.  */
static struct target_ops record_btrace_ops;

/* A new thread observer enabling branch tracing for the new thread.  */
static struct observer *record_btrace_thread_observer;

/* Print a record-btrace debug message.  Use do ... while (0) to avoid
   ambiguities when used in if statements.  */

#define DEBUG(msg, args...)						\
  do									\
    {									\
      if (record_debug != 0)						\
        fprintf_unfiltered (gdb_stdlog,					\
			    "[record-btrace] " msg "\n", ##args);	\
    }									\
  while (0)


/* Update the branch trace for the current thread and return a pointer to its
   branch trace information struct.

   Throws an error if there is no thread or no trace.  This function never
   returns NULL.  */

static struct btrace_thread_info *
require_btrace (void)
{
  struct thread_info *tp;
  struct btrace_thread_info *btinfo;

  DEBUG ("require");

  tp = find_thread_ptid (inferior_ptid);
  if (tp == NULL)
    error (_("No thread."));

  btrace_fetch (tp);

  btinfo = &tp->btrace;

  if (btinfo->begin == NULL)
    error (_("No trace."));

  return btinfo;
}

/* Enable branch tracing for one thread.  Warn on errors.  */

static void
record_btrace_enable_warn (struct thread_info *tp)
{
  volatile struct gdb_exception error;

  TRY_CATCH (error, RETURN_MASK_ERROR)
    btrace_enable (tp);

  if (error.message != NULL)
    warning ("%s", error.message);
}

/* Callback function to disable branch tracing for one thread.  */

static void
record_btrace_disable_callback (void *arg)
{
  struct thread_info *tp;

  tp = arg;

  btrace_disable (tp);
}

/* Enable automatic tracing of new threads.  */

static void
record_btrace_auto_enable (void)
{
  DEBUG ("attach thread observer");

  record_btrace_thread_observer
    = observer_attach_new_thread (record_btrace_enable_warn);
}

/* Disable automatic tracing of new threads.  */

static void
record_btrace_auto_disable (void)
{
  /* The observer may have been detached, already.  */
  if (record_btrace_thread_observer == NULL)
    return;

  DEBUG ("detach thread observer");

  observer_detach_new_thread (record_btrace_thread_observer);
  record_btrace_thread_observer = NULL;
}

/* The to_open method of target record-btrace.  */

static void
record_btrace_open (char *args, int from_tty)
{
  struct cleanup *disable_chain;
  struct thread_info *tp;

  DEBUG ("open");

  record_preopen ();

  if (!target_has_execution)
    error (_("The program is not being run."));

  if (!target_supports_btrace ())
    error (_("Target does not support branch tracing."));

  gdb_assert (record_btrace_thread_observer == NULL);

  disable_chain = make_cleanup (null_cleanup, NULL);
  ALL_THREADS (tp)
    if (args == NULL || *args == 0 || number_is_in_list (args, tp->num))
      {
	btrace_enable (tp);

	make_cleanup (record_btrace_disable_callback, tp);
      }

  record_btrace_auto_enable ();

  push_target (&record_btrace_ops);

  observer_notify_record_changed (current_inferior (),  1);

  discard_cleanups (disable_chain);
}

/* The to_stop_recording method of target record-btrace.  */

static void
record_btrace_stop_recording (void)
{
  struct thread_info *tp;

  DEBUG ("stop recording");

  record_btrace_auto_disable ();

  ALL_THREADS (tp)
    if (tp->btrace.target != NULL)
      btrace_disable (tp);
}

/* The to_close method of target record-btrace.  */

static void
record_btrace_close (void)
{
  /* Make sure automatic recording gets disabled even if we did not stop
     recording before closing the record-btrace target.  */
  record_btrace_auto_disable ();

  /* We already stopped recording.  */
}

/* The to_info_record method of target record-btrace.  */

static void
record_btrace_info (void)
{
  struct btrace_thread_info *btinfo;
  struct thread_info *tp;
  unsigned int insns, calls;

  DEBUG ("info");

  tp = find_thread_ptid (inferior_ptid);
  if (tp == NULL)
    error (_("No thread."));

  btrace_fetch (tp);

  insns = 0;
  calls = 0;

  btinfo = &tp->btrace;
  if (btinfo->begin != NULL)
    {
      struct btrace_call_iterator call;
      struct btrace_insn_iterator insn;

      btrace_call_end (&call, btinfo);
      btrace_call_prev (&call, 1);
      calls = btrace_call_number (&call) + 1;

      btrace_insn_end (&insn, btinfo);
      btrace_insn_prev (&insn, 1);
      insns = btrace_insn_number (&insn) + 1;
    }

  printf_unfiltered (_("Recorded %u instructions in %u functions for thread "
		       "%d (%s).\n"), insns, calls, tp->num,
		     target_pid_to_str (tp->ptid));
}

/* Print an unsigned int.  */

static void
ui_out_field_uint (struct ui_out *uiout, const char *fld, unsigned int val)
{
  ui_out_field_fmt (uiout, fld, "%u", val);
}

/* Disassemble a section of the recorded instruction trace.  */

static void
btrace_insn_history (struct ui_out *uiout,
		     const struct btrace_insn_iterator *begin,
		     const struct btrace_insn_iterator *end, int flags)
{
  struct gdbarch *gdbarch;
  struct btrace_insn_iterator it;

  DEBUG ("itrace (0x%x): [%u; %u)", flags, btrace_insn_number (begin),
	 btrace_insn_number (end));

  gdbarch = target_gdbarch ();

  for (it = *begin; btrace_insn_cmp (&it, end) != 0; btrace_insn_next (&it, 1))
    {
      const struct btrace_insn *insn;

      insn = btrace_insn_get (&it);

      /* Print the instruction index.  */
      ui_out_field_uint (uiout, "index", btrace_insn_number (&it));
      ui_out_text (uiout, "\t");

      /* Disassembly with '/m' flag may not produce the expected result.
	 See PR gdb/11833.  */
      gdb_disassembly (gdbarch, uiout, NULL, flags, 1, insn->pc, insn->pc + 1);
    }
}

/* The to_insn_history method of target record-btrace.  */

static void
record_btrace_insn_history (int size, int flags)
{
  struct btrace_thread_info *btinfo;
  struct btrace_insn_history *history;
  struct btrace_insn_iterator begin, end;
  struct cleanup *uiout_cleanup;
  struct ui_out *uiout;
  unsigned int context, covered;

  uiout = current_uiout;
  uiout_cleanup = make_cleanup_ui_out_tuple_begin_end (uiout,
						       "insn history");
  context = abs (size);
  if (context == 0)
    error (_("Bad record instruction-history-size."));

  btinfo = require_btrace ();
  history = btinfo->insn_history;
  if (history == NULL)
    {
      /* No matter the direction, we start with the tail of the trace.  */
      btrace_insn_end (&begin, btinfo);
      end = begin;

      DEBUG ("insn-history (0x%x): %d", flags, size);

      covered = btrace_insn_prev (&begin, context);
    }
  else
    {
      begin = history->begin;
      end = history->end;

      DEBUG ("insn-history (0x%x): %d, prev: [%u; %u)", flags, size,
	     btrace_insn_number (&begin), btrace_insn_number (&end));

      if (size < 0)
	{
	  end = begin;
	  covered = btrace_insn_prev (&begin, context);
	}
      else
	{
	  begin = end;
	  covered = btrace_insn_next (&end, context);
	}
    }

  if (covered > 0)
    btrace_insn_history (uiout, &begin, &end, flags);
  else
    {
      if (size < 0)
	printf_unfiltered (_("At the start of the branch trace record.\n"));
      else
	printf_unfiltered (_("At the end of the branch trace record.\n"));
    }

  btrace_set_insn_history (btinfo, &begin, &end);
  do_cleanups (uiout_cleanup);
}

/* The to_insn_history_range method of target record-btrace.  */

static void
record_btrace_insn_history_range (ULONGEST from, ULONGEST to, int flags)
{
  struct btrace_thread_info *btinfo;
  struct btrace_insn_history *history;
  struct btrace_insn_iterator begin, end;
  struct cleanup *uiout_cleanup;
  struct ui_out *uiout;
  unsigned int low, high;
  int found;

  uiout = current_uiout;
  uiout_cleanup = make_cleanup_ui_out_tuple_begin_end (uiout,
						       "insn history");
  low = from;
  high = to;

  DEBUG ("insn-history (0x%x): [%u; %u)", flags, low, high);

  /* Check for wrap-arounds.  */
  if (low != from || high != to)
    error (_("Bad range."));

  if (high <= low)
    error (_("Bad range."));

  btinfo = require_btrace ();

  found = btrace_find_insn_by_number (&begin, btinfo, low);
  if (found == 0)
    error (_("Range out of bounds."));

  /* Silently truncate the range, if necessary.  */
  found = btrace_find_insn_by_number (&end, btinfo, high);
  if (found == 0)
    btrace_insn_end (&end, btinfo);

  btrace_insn_history (uiout, &begin, &end, flags);
  btrace_set_insn_history (btinfo, &begin, &end);

  do_cleanups (uiout_cleanup);
}

/* The to_insn_history_from method of target record-btrace.  */

static void
record_btrace_insn_history_from (ULONGEST from, int size, int flags)
{
  ULONGEST begin, end, context;

  context = abs (size);

  if (size < 0)
    {
      end = from;

      if (from < context)
	begin = 0;
      else
	begin = from - context;
    }
  else
    {
      begin = from;
      end = from + context;

      /* Check for wrap-around.  */
      if (end < begin)
	end = ULONGEST_MAX;
    }

  record_btrace_insn_history_range (begin, end, flags);
}

/* Print the instruction number range for a function call history line.  */

static void
btrace_call_history_insn_range (struct ui_out *uiout,
				const struct btrace_function *bfun)
{
  unsigned int begin, end, size;

  size = VEC_length (btrace_insn_s, bfun->insn);
  gdb_assert (size > 0);

  begin = bfun->insn_offset;
  end = begin + size - 1;

  ui_out_field_uint (uiout, "insn begin", begin);
  ui_out_text (uiout, "-");
  ui_out_field_uint (uiout, "insn end", end);
}

/* Print the source line information for a function call history line.  */

static void
btrace_call_history_src_line (struct ui_out *uiout,
			      const struct btrace_function *bfun)
{
  struct symbol *sym;
  int begin, end;

  sym = bfun->sym;
  if (sym == NULL)
    return;

  ui_out_field_string (uiout, "file",
		       symtab_to_filename_for_display (sym->symtab));

  begin = bfun->lbegin;
  end = bfun->lend;

  if (end < begin)
    return;

  ui_out_text (uiout, ":");
  ui_out_field_int (uiout, "min line", begin);

  if (end == begin)
    return;

  ui_out_text (uiout, "-");
  ui_out_field_int (uiout, "max line", end);
}

/* Disassemble a section of the recorded function trace.  */

static void
btrace_call_history (struct ui_out *uiout,
		     const struct btrace_call_iterator *begin,
		     const struct btrace_call_iterator *end,
		     enum record_print_flag flags)
{
  struct btrace_call_iterator it;

  DEBUG ("ftrace (0x%x): [%u; %u)", flags, btrace_call_number (begin),
	 btrace_call_number (end));

  for (it = *begin; btrace_call_cmp (&it, end) < 0; btrace_call_next (&it, 1))
    {
      const struct btrace_function *bfun;
      struct minimal_symbol *msym;
      struct symbol *sym;

      bfun = btrace_call_get (&it);
      msym = bfun->msym;
      sym = bfun->sym;

      /* Print the function index.  */
      ui_out_field_uint (uiout, "index", bfun->number);
      ui_out_text (uiout, "\t");

      if ((flags & RECORD_PRINT_INSN_RANGE) != 0)
	{
	  btrace_call_history_insn_range (uiout, bfun);
	  ui_out_text (uiout, "\t");
	}

      if ((flags & RECORD_PRINT_SRC_LINE) != 0)
	{
	  btrace_call_history_src_line (uiout, bfun);
	  ui_out_text (uiout, "\t");
	}

      if (sym != NULL)
	ui_out_field_string (uiout, "function", SYMBOL_PRINT_NAME (sym));
      else if (msym != NULL)
	ui_out_field_string (uiout, "function", SYMBOL_PRINT_NAME (msym));

      ui_out_text (uiout, "\n");
    }
}

/* The to_call_history method of target record-btrace.  */

static void
record_btrace_call_history (int size, int flags)
{
  struct btrace_thread_info *btinfo;
  struct btrace_call_history *history;
  struct btrace_call_iterator begin, end;
  struct cleanup *uiout_cleanup;
  struct ui_out *uiout;
  unsigned int context, covered;

  uiout = current_uiout;
  uiout_cleanup = make_cleanup_ui_out_tuple_begin_end (uiout,
						       "insn history");
  context = abs (size);
  if (context == 0)
    error (_("Bad record function-call-history-size."));

  btinfo = require_btrace ();
  history = btinfo->call_history;
  if (history == NULL)
    {
      /* No matter the direction, we start with the tail of the trace.  */
      btrace_call_end (&begin, btinfo);
      end = begin;

      DEBUG ("call-history (0x%x): %d", flags, size);

      covered = btrace_call_prev (&begin, context);
    }
  else
    {
      begin = history->begin;
      end = history->end;

      DEBUG ("call-history (0x%x): %d, prev: [%u; %u)", flags, size,
	     btrace_call_number (&begin), btrace_call_number (&end));

      if (size < 0)
	{
	  end = begin;
	  covered = btrace_call_prev (&begin, context);
	}
      else
	{
	  begin = end;
	  covered = btrace_call_next (&end, context);
	}
    }

  if (covered > 0)
    btrace_call_history (uiout, &begin, &end, flags);
  else
    {
      if (size < 0)
	printf_unfiltered (_("At the start of the branch trace record.\n"));
      else
	printf_unfiltered (_("At the end of the branch trace record.\n"));
    }

  btrace_set_call_history (btinfo, &begin, &end);
  do_cleanups (uiout_cleanup);
}

/* The to_call_history_range method of target record-btrace.  */

static void
record_btrace_call_history_range (ULONGEST from, ULONGEST to, int flags)
{
  struct btrace_thread_info *btinfo;
  struct btrace_call_history *history;
  struct btrace_call_iterator begin, end;
  struct cleanup *uiout_cleanup;
  struct ui_out *uiout;
  unsigned int low, high;
  int found;

  uiout = current_uiout;
  uiout_cleanup = make_cleanup_ui_out_tuple_begin_end (uiout,
						       "func history");
  low = from;
  high = to;

  DEBUG ("call-history (0x%x): [%u; %u)", flags, low, high);

  /* Check for wrap-arounds.  */
  if (low != from || high != to)
    error (_("Bad range."));

  if (high <= low)
    error (_("Bad range."));

  btinfo = require_btrace ();

  found = btrace_find_call_by_number (&begin, btinfo, low);
  if (found == 0)
    error (_("Range out of bounds."));

  /* Silently truncate the range, if necessary.  */
  found = btrace_find_call_by_number (&end, btinfo, high);
  if (found == 0)
    btrace_call_end (&end, btinfo);

  btrace_call_history (uiout, &begin, &end, flags);
  btrace_set_call_history (btinfo, &begin, &end);

  do_cleanups (uiout_cleanup);
}

/* The to_call_history_from method of target record-btrace.  */

static void
record_btrace_call_history_from (ULONGEST from, int size, int flags)
{
  ULONGEST begin, end, context;

  context = abs (size);

  if (size < 0)
    {
      end = from;

      if (from < context)
	begin = 0;
      else
	begin = from - context;
    }
  else
    {
      begin = from;
      end = from + context;

      /* Check for wrap-around.  */
      if (end < begin)
	end = ULONGEST_MAX;
    }

  record_btrace_call_history_range (begin, end, flags);
}

/* Initialize the record-btrace target ops.  */

static void
init_record_btrace_ops (void)
{
  struct target_ops *ops;

  ops = &record_btrace_ops;
  ops->to_shortname = "record-btrace";
  ops->to_longname = "Branch tracing target";
  ops->to_doc = "Collect control-flow trace and provide the execution history.";
  ops->to_open = record_btrace_open;
  ops->to_close = record_btrace_close;
  ops->to_detach = record_detach;
  ops->to_disconnect = record_disconnect;
  ops->to_mourn_inferior = record_mourn_inferior;
  ops->to_kill = record_kill;
  ops->to_create_inferior = find_default_create_inferior;
  ops->to_stop_recording = record_btrace_stop_recording;
  ops->to_info_record = record_btrace_info;
  ops->to_insn_history = record_btrace_insn_history;
  ops->to_insn_history_from = record_btrace_insn_history_from;
  ops->to_insn_history_range = record_btrace_insn_history_range;
  ops->to_call_history = record_btrace_call_history;
  ops->to_call_history_from = record_btrace_call_history_from;
  ops->to_call_history_range = record_btrace_call_history_range;
  ops->to_stratum = record_stratum;
  ops->to_magic = OPS_MAGIC;
}

/* Alias for "target record".  */

static void
cmd_record_btrace_start (char *args, int from_tty)
{
  if (args != NULL && *args != 0)
    error (_("Invalid argument."));

  execute_command ("target record-btrace", from_tty);
}

void _initialize_record_btrace (void);

/* Initialize btrace commands.  */

void
_initialize_record_btrace (void)
{
  add_cmd ("btrace", class_obscure, cmd_record_btrace_start,
	   _("Start branch trace recording."),
	   &record_cmdlist);
  add_alias_cmd ("b", "btrace", class_obscure, 1, &record_cmdlist);

  init_record_btrace_ops ();
  add_target (&record_btrace_ops);
}
