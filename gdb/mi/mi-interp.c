/* MI Interpreter Definitions and Commands for GDB, the GNU debugger.

   Copyright (C) 2002, 2003, 2004, 2005, 2007, 2008
   Free Software Foundation, Inc.

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
#include "gdb_string.h"
#include "interps.h"
#include "event-top.h"
#include "event-loop.h"
#include "inferior.h"
#include "ui-out.h"
#include "top.h"
#include "exceptions.h"
#include "mi-main.h"
#include "mi-cmds.h"
#include "mi-out.h"
#include "mi-console.h"
#include "observer.h"
#include "gdbthread.h"

struct mi_interp
{
  /* MI's output channels */
  struct ui_file *out;
  struct ui_file *err;
  struct ui_file *log;
  struct ui_file *targ;
  struct ui_file *event_channel;

  /* This is the interpreter for the mi... */
  struct interp *mi2_interp;
  struct interp *mi1_interp;
  struct interp *mi_interp;
};

/* These are the interpreter setup, etc. functions for the MI interpreter */
static void mi_execute_command_wrapper (char *cmd);
static void mi_command_loop (int mi_version);

/* These are hooks that we put in place while doing interpreter_exec
   so we can report interesting things that happened "behind the mi's
   back" in this command */
static int mi_interp_query_hook (const char *ctlstr, va_list ap)
     ATTR_FORMAT (printf, 1, 0);

static void mi3_command_loop (void);
static void mi2_command_loop (void);
static void mi1_command_loop (void);

static void mi_insert_notify_hooks (void);
static void mi_remove_notify_hooks (void);

static void mi_new_thread (struct thread_info *t);

static void *
mi_interpreter_init (int top_level)
{
  struct mi_interp *mi = XMALLOC (struct mi_interp);

  /* HACK: We need to force stdout/stderr to point at the console.  This avoids
     any potential side effects caused by legacy code that is still
     using the TUI / fputs_unfiltered_hook.  So we set up output channels for
     this now, and swap them in when we are run. */

  raw_stdout = stdio_fileopen (stdout);

  /* Create MI channels */
  mi->out = mi_console_file_new (raw_stdout, "~", '"');
  mi->err = mi_console_file_new (raw_stdout, "&", '"');
  mi->log = mi->err;
  mi->targ = mi_console_file_new (raw_stdout, "@", '"');
  mi->event_channel = mi_console_file_new (raw_stdout, "=", 0);

  if (top_level)
    observer_attach_new_thread (mi_new_thread);

  return mi;
}

static int
mi_interpreter_resume (void *data)
{
  struct mi_interp *mi = data;
  /* As per hack note in mi_interpreter_init, swap in the output channels... */

  gdb_setup_readline ();

  /* These overwrite some of the initialization done in
     _intialize_event_loop.  */
  call_readline = gdb_readline2;
  input_handler = mi_execute_command_wrapper;
  add_file_handler (input_fd, stdin_event_handler, 0);
  async_command_editing_p = 0;
  /* FIXME: This is a total hack for now.  PB's use of the MI
     implicitly relies on a bug in the async support which allows
     asynchronous commands to leak through the commmand loop.  The bug
     involves (but is not limited to) the fact that sync_execution was
     erroneously initialized to 0.  Duplicate by initializing it thus
     here...  */
  sync_execution = 0;

  gdb_stdout = mi->out;
  /* Route error and log output through the MI */
  gdb_stderr = mi->err;
  gdb_stdlog = mi->log;
  /* Route target output through the MI. */
  gdb_stdtarg = mi->targ;
  /* Route target error through the MI as well. */
  gdb_stdtargerr = mi->targ;

  /* Replace all the hooks that we know about.  There really needs to
     be a better way of doing this... */
  clear_interpreter_hooks ();

  deprecated_show_load_progress = mi_load_progress;

  /* If we're _the_ interpreter, take control. */
  if (current_interp_named_p (INTERP_MI1))
    deprecated_command_loop_hook = mi1_command_loop;
  else if (current_interp_named_p (INTERP_MI2))
    deprecated_command_loop_hook = mi2_command_loop;
  else if (current_interp_named_p (INTERP_MI3))
    deprecated_command_loop_hook = mi3_command_loop;
  else
    deprecated_command_loop_hook = mi2_command_loop;

  return 1;
}

static int
mi_interpreter_suspend (void *data)
{
  gdb_disable_readline ();
  return 1;
}

static struct gdb_exception
mi_interpreter_exec (void *data, const char *command)
{
  static struct gdb_exception ok;
  char *tmp = alloca (strlen (command) + 1);
  strcpy (tmp, command);
  mi_execute_command_wrapper (tmp);
  return exception_none;
}

/* Never display the default gdb prompt in mi case.  */
static int
mi_interpreter_prompt_p (void *data)
{
  return 0;
}

static void
mi_interpreter_exec_continuation (struct continuation_arg *arg, int error_p)
{
  bpstat_do_actions (&stop_bpstat);
  /* It's not clear what to do in the case of errror -- should we assume that
     the target is stopped, or that it still runs?  */
  if (!target_executing)
    {
      fputs_unfiltered ("*stopped", raw_stdout);
      mi_out_put (uiout, raw_stdout);
      fputs_unfiltered ("\n", raw_stdout);
      fputs_unfiltered ("(gdb) \n", raw_stdout);
      gdb_flush (raw_stdout);
    }
  else if (target_can_async_p ())
    {
      add_continuation (mi_interpreter_exec_continuation, NULL);
    }
}

enum mi_cmd_result
mi_cmd_interpreter_exec (char *command, char **argv, int argc)
{
  struct interp *interp_to_use;
  int i;
  struct interp_procs *procs;
  char *mi_error_message = NULL;
  struct cleanup *old_chain;

  if (argc < 2)
    error ("mi_cmd_interpreter_exec: Usage: -interpreter-exec interp command");

  interp_to_use = interp_lookup (argv[0]);
  if (interp_to_use == NULL)
    error ("mi_cmd_interpreter_exec: could not find interpreter \"%s\"", argv[0]);

  if (!interp_exec_p (interp_to_use))
    error ("mi_cmd_interpreter_exec: interpreter \"%s\" does not support command execution",
	      argv[0]);

  /* Insert the MI out hooks, making sure to also call the interpreter's hooks
     if it has any. */
  /* KRS: We shouldn't need this... Events should be installed and they should
     just ALWAYS fire something out down the MI channel... */
  mi_insert_notify_hooks ();

  /* Now run the code... */

  old_chain = make_cleanup (null_cleanup, 0);
  for (i = 1; i < argc; i++)
    {
      struct gdb_exception e = interp_exec (interp_to_use, argv[i]);
      if (e.reason < 0)
	{
	  mi_error_message = xstrdup (e.message);
	  make_cleanup (xfree, mi_error_message);
	  break;
	}
    }

  mi_remove_notify_hooks ();

  /* Okay, now let's see if the command set the inferior going...
     Tricky point - have to do this AFTER resetting the interpreter, since
     changing the interpreter will clear out all the continuations for
     that interpreter... */

  if (target_can_async_p () && target_executing)
    {
      fputs_unfiltered ("^running\n", raw_stdout);
      add_continuation (mi_interpreter_exec_continuation, NULL);
    }

  if (mi_error_message != NULL)
    error ("%s", mi_error_message);
  do_cleanups (old_chain);
  return MI_CMD_DONE;
}

/*
 * mi_insert_notify_hooks - This inserts a number of hooks that are meant to produce
 * async-notify ("=") MI messages while running commands in another interpreter
 * using mi_interpreter_exec.  The canonical use for this is to allow access to
 * the gdb CLI interpreter from within the MI, while still producing MI style output
 * when actions in the CLI command change gdb's state.
*/

static void
mi_insert_notify_hooks (void)
{
  deprecated_query_hook = mi_interp_query_hook;
}

static void
mi_remove_notify_hooks (void)
{
  deprecated_query_hook = NULL;
}

static int
mi_interp_query_hook (const char *ctlstr, va_list ap)
{
  return 1;
}

static void
mi_execute_command_wrapper (char *cmd)
{
  mi_execute_command (cmd, stdin == instream);
}

static void
mi1_command_loop (void)
{
  mi_command_loop (1);
}

static void
mi2_command_loop (void)
{
  mi_command_loop (2);
}

static void
mi3_command_loop (void)
{
  mi_command_loop (3);
}

static void
mi_command_loop (int mi_version)
{
  /* Turn off 8 bit strings in quoted output.  Any character with the
     high bit set is printed using C's octal format. */
  sevenbit_strings = 1;
  /* Tell the world that we're alive */
  fputs_unfiltered ("(gdb) \n", raw_stdout);
  gdb_flush (raw_stdout);
  start_event_loop ();
}

static void
mi_new_thread (struct thread_info *t)
{
  struct mi_interp *mi = top_level_interpreter_data ();

  fprintf_unfiltered (mi->event_channel, "thread-created,id=%d", t->num);
  gdb_flush (mi->event_channel);
}

extern initialize_file_ftype _initialize_mi_interp; /* -Wmissing-prototypes */

void
_initialize_mi_interp (void)
{
  static const struct interp_procs procs =
  {
    mi_interpreter_init,	/* init_proc */
    mi_interpreter_resume,	/* resume_proc */
    mi_interpreter_suspend,	/* suspend_proc */
    mi_interpreter_exec,	/* exec_proc */
    mi_interpreter_prompt_p	/* prompt_proc_p */
  };

  /* The various interpreter levels.  */
  interp_add (interp_new (INTERP_MI1, NULL, mi_out_new (1), &procs));
  interp_add (interp_new (INTERP_MI2, NULL, mi_out_new (2), &procs));
  interp_add (interp_new (INTERP_MI3, NULL, mi_out_new (3), &procs));

  /* "mi" selects the most recent released version.  "mi2" was
     released as part of GDB 6.0.  */
  interp_add (interp_new (INTERP_MI, NULL, mi_out_new (2), &procs));
}
