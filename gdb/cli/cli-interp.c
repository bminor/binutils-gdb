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

/* Prototypes for the CLI Interpreter functions */

int cli_interpreter_init (void *data);
int cli_interpreter_resume (void *data);
int cli_interpreter_do_one_event (void *data);
int cli_interpreter_suspend (void *data);
int cli_interpreter_delete (void *data);
int cli_interpreter_exec (void *data, char *command_str);
int cli_interpreter_display_prompt (void *data, char *new_prompt);

/* These are the ui_out and the interpreter for the console interpreter. */
struct ui_out *cli_uiout;
struct gdb_interpreter *cli_interp;

/* These implement the cli out interpreter: */

int
cli_interpreter_init (void *data)
{
  return 1;
}

int
cli_interpreter_resume (void *data)
{
  /*sync_execution = 1;*/
  gdb_setup_readline ();
  return 1;
}

int
cli_interpreter_do_one_event (void *data)
{
  return 1;
}

int
cli_interpreter_suspend (void *data)
{
  gdb_disable_readline ();
  return 1;
}

int
cli_interpreter_delete (void *data)
{
  return 1;
}

int
cli_interpreter_display_prompt (void *data, char *new_prompt)
{
  if (gdb_interpreter_is_quiet (NULL))
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

int
cli_interpreter_exec (void *data, char *command_str)
{
  return gdb_execute_command (uiout, command_str, 0);
}

/* standard gdb initialization hook */
void
_initialize_cli_interp (void)
{
  struct gdb_interpreter_procs procs = {
    cli_interpreter_init,	/* init_proc */
    cli_interpreter_resume,	/* resume_proc */
    cli_interpreter_do_one_event, /* do_one_event_proc */
    cli_interpreter_suspend,	/* suspend_proc */
    cli_interpreter_delete,	/* delete_proc */
    cli_interpreter_exec,	/* exec_proc */
    cli_interpreter_display_prompt /* prompt_proc */
  };

  cli_uiout = cli_out_new (gdb_stdout);
  cli_interp = gdb_new_interpreter (GDB_INTERPRETER_CONSOLE, NULL, cli_uiout,
				    &procs);
  gdb_add_interpreter (cli_interp);
}
