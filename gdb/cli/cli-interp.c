/* CLI Definitions for GDB, the GNU debugger.

   Copyright (c) 2002, 2003 Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "defs.h"
#include "interps.h"
#include "wrapper.h"
#include "event-top.h"
#include "ui-out.h"
#include "cli-out.h"
#include "top.h"		/* for "execute_command" */
#include "inferior.h"           /* for "sync_execution" */
#include "mi/mi-console.h"      /* for "mi_console_file_new" */
#include "gdb_string.h"
#include "exceptions.h"

struct ui_out *cli_uiout;

/* These are the ui_out and the interpreter for the console interpreter.  */

/* Longjmp-safe wrapper for "execute_command".  */
struct gdb_exception safe_execute_command (struct ui_out *uiout,
					   char *command, int from_tty);
struct captured_execute_command_args
{
  char *command;
  int from_tty;
};

/* These implement the cli out interpreter: */

static void *
cli_interpreter_init (void)
{
  return NULL;
}

static int
cli_interpreter_resume (void *data)
{
  struct ui_file *stream;

  sync_execution = 1;

  /* gdb_setup_readline will change gdb_stdout.  If the CLI was previously
     writing to gdb_stdout, then set it to the new gdb_stdout afterwards.  */

  stream = cli_out_set_stream (cli_uiout, gdb_stdout);
  if (stream != gdb_stdout)
    {
      cli_out_set_stream (cli_uiout, stream);
      stream = NULL;
    }

  gdb_setup_readline ();

  if (stream != NULL)
    cli_out_set_stream (cli_uiout, gdb_stdout);

  return 1;
}

static int
cli_interpreter_suspend (void *data)
{
  gdb_disable_readline ();
  return 1;
}

/* Don't display the prompt if we are set quiet.  */
static int
cli_interpreter_display_prompt_p (void *data)
{
  if (interp_quiet_p (NULL))
    return 0;
  else
    return 1;
}

static struct gdb_exception
cli_interpreter_exec (void *data, const char *command_str)
{
  struct gdb_exception result;

  /* FIXME: cagney/2003-02-01: Need to const char *propogate
     safe_execute_command.  */
  char *str = strcpy (alloca (strlen (command_str) + 1), command_str);

  /* We don't need old_stream because we actually change the
     interpreters when we do interpreter exec, then swap them back.
     This code assumes that the interpreter is still the one that is
     exec'ing in the cli interpreter, and we are just faking it up.  */
  /* We want 
     the person who set the interpreter to get the uiout right for that
     according to their lights.  If you don't do that, then you can't share
     the cli_interpreter_exec between the console & console-quoted 
     interpreters.  */
  result = safe_execute_command (uiout, str, 1);

  return result;
}

static void
do_captured_execute_command (struct ui_out *uiout, void *data)
{
  struct captured_execute_command_args *args =
    (struct captured_execute_command_args *) data;
  execute_command (args->command, args->from_tty);
}

struct gdb_exception
safe_execute_command (struct ui_out *uiout, char *command, int from_tty)
{
  struct gdb_exception e;
  struct captured_execute_command_args args;
  args.command = command;
  args.from_tty = from_tty;
  e = catch_exception (uiout, do_captured_execute_command, &args,
		       RETURN_MASK_ALL);
  /* FIXME: cagney/2005-01-13: This shouldn't be needed.  Instead the
     caller should print the exception.  */
  exception_print (gdb_stderr, e);
  return e;
}

/* This is the only new function needed for the 
   console-quoted interpreter.  This outputs console text in 
   an mi-quoted form, so an mi-parser won't be fooled by spurious
   * at beginning of line goofs...  */

int
cli_quoted_interpreter_resume (void *data)
{
  static struct ui_file *quoted_stdout = NULL;
  static struct ui_file *quoted_stderr = NULL;

  sync_execution = 1;
  // print_frame_more_info_hook = 0;
  gdb_setup_readline ();
  if (quoted_stdout == NULL)
    {
      struct ui_file *raw_stdout;
      raw_stdout = stdio_fileopen (stdout);
      quoted_stdout = mi_console_file_new (raw_stdout, "~", '"');

      quoted_stderr = mi_console_file_new (raw_stdout, "&", '"');
    }
  gdb_stdout = quoted_stdout;
  gdb_stderr = quoted_stderr;
  gdb_stdlog = gdb_stderr;

  return 1;
}

/* Standard gdb initialization hook.  */
extern initialize_file_ftype _initialize_cli_interp; /* -Wmissing-prototypes */

void
_initialize_cli_interp (void)
{
  static const struct interp_procs procs = {
    cli_interpreter_init,	/* init_proc */
    cli_interpreter_resume,	/* resume_proc */
    cli_interpreter_suspend,	/* suspend_proc */
    cli_interpreter_exec,	/* exec_proc */
    cli_interpreter_display_prompt_p,	/* prompt_proc_p */
    cli_command_loop,
    // cli_interpreter_complete
  };
  struct interp *cli_interp;

  /* And here we initialize the console-quoted
     interpreter.  */
  static const struct interp_procs quoted_procs = {
    cli_interpreter_init,	/* init_proc */
    cli_quoted_interpreter_resume,	/* resume_proc */
    cli_interpreter_suspend,	/* suspend_proc */
    cli_interpreter_exec,	/* exec_proc */
    cli_interpreter_display_prompt_p,	/* prompt_proc_p */
    cli_command_loop,
    // cli_interpreter_complete
  };
  struct ui_out *tmp_ui_out;
  struct ui_file *raw_stdout;
  
  /* Create a default uiout builder for the CLI.  */
  cli_uiout = cli_out_new (gdb_stdout);
  cli_interp = interp_new (INTERP_CONSOLE, NULL, cli_uiout, &procs);

  interp_add (cli_interp);

  raw_stdout = stdio_fileopen (stdout);
  tmp_ui_out = cli_quoted_out_new (raw_stdout);
  cli_interp = interp_new ("console-quoted", NULL, tmp_ui_out,
			   &quoted_procs);
  interp_add (cli_interp); /* second call */
}
