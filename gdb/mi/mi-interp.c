/* MI Interpreter Definitions and Commands
   Copyright 2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "gdb_string.h"
#include "interps.h"
#include "event-top.h"
#include "event-loop.h"
#include "inferior.h"
#include "ui-out.h"
#include "top.h"

#include "mi.h"
#include "mi-cmds.h"
#include "mi-out.h"
#include "mi-console.h"

/* MI's output channels */
struct ui_file *mi_stdout;
struct ui_file *mi_stderr;
struct ui_file *mi_stdlog;
struct ui_file *mi_stdtarg;
struct ui_file *mi_event_channel;

/* This is the interpreter for the mi... */
struct gdb_interpreter *mi2_interp;
struct gdb_interpreter *mi1_interp;
struct gdb_interpreter *mi_interp;

/* These are the interpreter setup, etc. functions for the MI interpreter */
static int mi_interpreter_init (void *data);
static int mi_interpreter_resume (void *data);
static int mi_interpreter_suspend (void *data);
static int mi_interpreter_exec (void *data, char *command);
static int mi_interpreter_prompt_p (void);

static void mi_execute_command_wrapper (char *cmd);
static void mi_command_loop (int mi_version);
static char *mi_input (char *);

/* These are hooks that we put in place while doing interpreter_exec
   so we can report interesting things that happened "behind the mi's
   back" in this command */
static int mi_interp_query_hook (const char *ctlstr, va_list ap);
static char *mi_interp_read_one_line_hook (char *prompt, int repeat,
					   char *anno);

static void mi2_command_loop (void);
static void mi1_command_loop (void);

static void mi_insert_notify_hooks (void);
static void mi_remove_notify_hooks (void);

static int
mi_interpreter_init (void *data)
{
  static struct gdb_events handlers;

  /* Why is this a part of the mi architecture? */

  mi_setup_architecture_data ();

  /* HACK: We need to force stdout/stderr to point at the console.  This avoids
     any potential side effects caused by legacy code that is still
     using the TUI / fputs_unfiltered_hook.  So we set up output channels for
     this now, and swap them in when we are run. */

  raw_stdout = stdio_fileopen (stdout);

  /* Create MI channels */
  mi_stdout = mi_console_file_new (raw_stdout, "~", '"');
  mi_stderr = mi_console_file_new (raw_stdout, "&", '"');
  mi_stdlog = mi_stderr;
  mi_stdtarg = mi_console_file_new (raw_stdout, "@", '"');
  mi_event_channel = mi_console_file_new (raw_stdout, "=", 0);

  return 1;
}

static int
mi_interpreter_resume (void *data)
{
  /* As per hack note in mi_interpreter_init, swap in the output channels... */

  gdb_setup_readline ();
  mi_register_gdbarch_swap ();

  if (event_loop_p)
    {
      /* These overwrite some of the initialization done in
         _intialize_event_loop. */
      call_readline = gdb_readline2;
      input_handler = mi_execute_command_wrapper;
      add_file_handler (input_fd, stdin_event_handler, 0);
      async_command_editing_p = 0;
      /* FIXME: This is a total hack for now.  PB's use of the MI implicitly
         relies on a bug in the async support which allows asynchronous
         commands to leak through the commmand loop.  The bug involves
         (but is not limited to) the fact that sync_execution was
         erroneously initialized to 0.  Duplicate by initializing it
         thus here... */
      sync_execution = 0;
    }

  gdb_stdout = mi_stdout;
  /* Route error and log output through the MI */
  gdb_stderr = mi_stderr;
  gdb_stdlog = mi_stdlog;
  /* Route target output through the MI. */
  gdb_stdtarg = mi_stdtarg;

  /* Replace all the hooks that we know about.  There really needs to
     be a better way of doing this... */
  clear_interpreter_hooks ();
  set_gdb_event_hooks (&mi_event_handlers);

  show_load_progress = mi_load_progress;

  /* If we're _the_ interpreter, take control. */
  if (gdb_interpreter_current_is_named_p (GDB_INTERPRETER_MI2))
    command_loop_hook = mi2_command_loop;
  else if (gdb_interpreter_current_is_named_p (GDB_INTERPRETER_MI1))
    command_loop_hook = mi1_command_loop;
  else if (gdb_interpreter_current_is_named_p (GDB_INTERPRETER_MI))
    command_loop_hook = mi2_command_loop;
  else
    return 0;

  return 1;
}

static int
mi_interpreter_suspend (void *data)
{
  gdb_disable_readline ();
  return 1;
}

static int
mi_interpreter_exec (void *data, char *command)
{
  mi_execute_command_wrapper (command);
  return 1;
}

/* Never display the default gdb prompt in mi case.  */
static int
mi_interpreter_prompt_p (void)
{
  return 0;
}

static void
mi_interpreter_exec_continuation (struct continuation_arg *arg)
{
  bpstat_do_actions (&stop_bpstat);
  if (!target_executing)
    {
      fputs_unfiltered ("*stopped", raw_stdout);
      mi_out_put (uiout, raw_stdout);
      fputs_unfiltered ("\n", raw_stdout);
      fputs_unfiltered ("(gdb) \n", raw_stdout);
      gdb_flush (raw_stdout);
      do_exec_cleanups (ALL_CLEANUPS);
    }
  else if (target_can_async_p ())
    {
      add_continuation (mi_interpreter_exec_continuation, NULL);
    }
}

enum mi_cmd_result
mi_cmd_interpreter_exec (char *command, char **argv, int argc)
{
  struct gdb_interpreter *interp_to_use;
  enum mi_cmd_result result = MI_CMD_DONE;
  int i;
  struct gdb_interpreter_procs *procs;

  if (argc < 2)
    {
      xasprintf (&mi_error_message,
		 "mi_cmd_interpreter_exec: Usage: -interpreter-exec interp command");
      return MI_CMD_ERROR;
    }

  interp_to_use = gdb_interpreter_lookup (argv[0]);
  if (interp_to_use == NULL)
    {
      xasprintf (&mi_error_message,
		 "mi_cmd_interpreter_exec: could not find interpreter \"%s\"",
		 argv[0]);
      return MI_CMD_ERROR;
    }

  procs = gdb_interpreter_get_procs (interp_to_use);
  if (!procs->exec_proc)
    {
      xasprintf (&mi_error_message,
		 "mi_cmd_interpreter_exec: interpreter \"%s\" does not support command execution",
		 argv[0]);
      return MI_CMD_ERROR;
    }

  /* Insert the MI out hooks, making sure to also call the interpreter's hooks
     if it has any. */
  /* KRS: We shouldn't need this... Events should be installed and they should
     just ALWAYS fire something out down the MI channel... */
  mi_insert_notify_hooks ();

  /* Now run the code... */

  for (i = 1; i < argc; i++)
    {
      char *buff = NULL;
      /* Do this in a cleaner way...  We want to force execution to be
         asynchronous for commands that run the target.  */
      if (target_can_async_p () && (strcmp (argv[0], "console") == 0))
	{
	  int len = strlen (argv[i]);
	  buff = xmalloc (len + 2);
	  memcpy (buff, argv[i], len);
	  buff[len] = '&';
	  buff[len + 1] = '\0';
	}

      /* We had to set sync_execution = 0 for the mi (well really for Project
         Builder's use of the mi - particularly so interrupting would work.
         But for console commands to work, we need to initialize it to 1 -
         since that is what the cli expects - before running the command,
         and then set it back to 0 when we are done. */
      sync_execution = 1;
      if (procs->exec_proc (gdb_interpreter_get_data (interp_to_use), argv[i]) < 0)
	{
	  mi_error_last_message ();
	  result = MI_CMD_ERROR;
	  break;
	}
      xfree (buff);
      do_exec_error_cleanups (ALL_CLEANUPS);
      sync_execution = 0;
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

  return result;
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
  query_hook = mi_interp_query_hook;
}

static void
mi_remove_notify_hooks ()
{
  query_hook = NULL;
}

static int
mi_interp_query_hook (const char *ctlstr, va_list ap)
{
  return 1;
}

static char *
mi_interp_read_one_line_hook (char *prompt, int repeat, char *anno)
{
  static char buff[256];
  printf_unfiltered ("=read-one-line,prompt=\"%s\"\n", prompt);
  gdb_flush (gdb_stdout);
  (void) fgets (buff, sizeof (buff), stdin);
  buff[(strlen (buff) - 1)] = 0;
  return buff;
}

static void
output_control_change_notification (char *notification)
{
  printf_unfiltered ("^");
  printf_unfiltered ("%s\n", notification);
  gdb_flush (gdb_stdout);
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
mi_command_loop (int mi_version)
{
#if 0
  /* HACK: Force stdout/stderr to point at the console.  This avoids
     any potential side effects caused by legacy code that is still
     using the TUI / fputs_unfiltered_hook */
  raw_stdout = stdio_fileopen (stdout);
  /* Route normal output through the MIx */
  gdb_stdout = mi_console_file_new (raw_stdout, "~", '"');
  /* Route error and log output through the MI */
  gdb_stderr = mi_console_file_new (raw_stdout, "&", '"');
  gdb_stdlog = gdb_stderr;
  /* Route target output through the MI. */
  gdb_stdtarg = mi_console_file_new (raw_stdout, "@", '"');
  /* HACK: Poke the ui_out table directly.  Should we be creating a
     mi_out object wired up to the above gdb_stdout / gdb_stderr? */
  uiout = mi_out_new (mi_version);
  /* HACK: Override any other interpreter hooks.  We need to create a
     real event table and pass in that. */
  init_ui_hook = 0;
  /* command_loop_hook = 0; */
  print_frame_info_listing_hook = 0;
  query_hook = 0;
  warning_hook = 0;
  create_breakpoint_hook = 0;
  delete_breakpoint_hook = 0;
  modify_breakpoint_hook = 0;
  interactive_hook = 0;
  registers_changed_hook = 0;
  readline_begin_hook = 0;
  readline_hook = 0;
  readline_end_hook = 0;
  register_changed_hook = 0;
  memory_changed_hook = 0;
  context_hook = 0;
  target_wait_hook = 0;
  call_command_hook = 0;
  error_hook = 0;
  error_begin_hook = 0;
  show_load_progress = mi_load_progress;
#endif
  /* Turn off 8 bit strings in quoted output.  Any character with the
     high bit set is printed using C's octal format. */
  sevenbit_strings = 1;
  /* Tell the world that we're alive */
  fputs_unfiltered ("(gdb) \n", raw_stdout);
  gdb_flush (raw_stdout);
  if (!event_loop_p)
    simplified_command_loop (mi_input, mi_execute_command);
  else
    start_event_loop ();
}

static char *
mi_input (char *buf)
{
  return gdb_readline (NULL);
}

void
_initialize_mi_interp (void)
{
  struct gdb_interpreter_procs procs =
    {
      mi_interpreter_init,	/* init_proc */
      mi_interpreter_resume,	/* resume_proc */
      mi_interpreter_suspend,	/* suspend_proc */
      mi_interpreter_exec,	/* exec_proc */
      mi_interpreter_prompt_p	/* prompt_proc_p */
    };

  /* Create MI1 interpreter */
  if (mi1_interp == NULL)
    {
      mi1_interp =
	gdb_interpreter_new (GDB_INTERPRETER_MI1, NULL, mi_out_new (1),
			     &procs);
      if (mi1_interp == NULL)
	error
	  ("Couldn't allocate a new interpreter for the mi1 interpreter\n");
      if (gdb_interpreter_add (mi1_interp) != 1)
	error ("Couldn't add the mi1 interpreter to gdb.\n");
    }

  /* Create MI2 interpreter */
  if (mi2_interp == NULL)
    {
      mi2_interp =
	gdb_interpreter_new (GDB_INTERPRETER_MI2, NULL, mi_out_new (2),
			     &procs);
      if (mi2_interp == NULL)
	error
	  ("Couldn't allocate a new interpreter for the mi2 interpreter\n");
      if (gdb_interpreter_add (mi2_interp) != 1)
	error ("Couldn't add the mi2 interpreter to gdb.\n");
    }

  /* Create MI3 interpreter */
  if (mi_interp == NULL)
    {
      mi_interp =
	gdb_interpreter_new (GDB_INTERPRETER_MI, NULL, mi_out_new (3),
			     &procs);
      if (mi_interp == NULL)
	error
	  ("Couldn't allocate a new interpreter for the mi interpreter\n");
      if (gdb_interpreter_add (mi_interp) != 1)
	error ("Couldn't add the mi interpreter to gdb.\n");
    }
}
