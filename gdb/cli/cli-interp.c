/* CLI Definitions for GDB
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
#include "interps.h"
#include "wrapper.h"
#include "event-top.h"
#include "ui-out.h"
#include "cli-out.h"
#include "top.h"		/* for "execute_command" */
/* Prototypes for the CLI Interpreter functions */

static int cli_interpreter_init (void *data);
static int cli_interpreter_resume (void *data);
static int cli_interpreter_suspend (void *data);
static int cli_interpreter_exec (void *data, char *command_str);
static int cli_interpreter_display_prompt_p (void);

/* These are the ui_out and the interpreter for the console interpreter. */
static struct ui_out *cli_uiout;
static struct gdb_interpreter *cli_interp;

/* Longjmp-safe wrapper for "execute_command" */
static int do_captured_execute_command (struct ui_out *uiout, void *data);
static enum gdb_rc safe_execute_command (struct ui_out *uiout, char *command,
					 int from_tty);
struct captured_execute_command_args
{
  char *command;
  int from_tty;
};

/* These implement the cli out interpreter: */

static int
cli_interpreter_init (void *data)
{
  return 1;
}

static int
cli_interpreter_resume (void *data)
{
  /*sync_execution = 1;*/
  gdb_setup_readline ();
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
cli_interpreter_display_prompt_p (void)
{
  if (gdb_interpreter_is_quiet_p (NULL))
    return 0;
  else
    return 1;
}

static int
cli_interpreter_exec (void *data, char *command_str)
{
  int result;
  struct ui_file *old_stream;

  /* gdb_stdout could change between the time cli_uiout was initialized
     and now. Since we're probably using a different interpreter which has
     a new ui_file for gdb_stdout, use that one instead of the default.
  
     It is important that it gets reset everytime, since the user could
     set gdb to use a different interpreter. */
  old_stream = cli_out_set_stream (cli_uiout, gdb_stdout);
  result = safe_execute_command (cli_uiout, command_str, 1);
  cli_out_set_stream (cli_uiout, old_stream);
  return result;
}

static int
do_captured_execute_command (struct ui_out *uiout, void *data)
{
  struct captured_execute_command_args *args =
    (struct captured_execute_command_args *) data;
  execute_command (args->command, args->from_tty);
  return GDB_RC_OK;
}

static enum gdb_rc
safe_execute_command (struct ui_out *uiout, char *command, int from_tty)
{
  struct captured_execute_command_args args;
  args.command = command;
  args.from_tty = from_tty;
  return catch_exceptions (uiout, do_captured_execute_command, &args,
			   NULL, RETURN_MASK_ALL);
}

/* standard gdb initialization hook */
void
_initialize_cli_interp (void)
{
  struct gdb_interpreter_procs procs = {
    cli_interpreter_init,	/* init_proc */
    cli_interpreter_resume,	/* resume_proc */
    cli_interpreter_suspend,	/* suspend_proc */
    cli_interpreter_exec,	/* exec_proc */
    cli_interpreter_display_prompt_p /* prompt_proc_p */
  };

  /* Create a default uiout builder for the CLI. */
  cli_uiout = cli_out_new (gdb_stdout);
  cli_interp = gdb_interpreter_new (GDB_INTERPRETER_CONSOLE, NULL, cli_uiout,
				    &procs);
  gdb_interpreter_add (cli_interp);
}
