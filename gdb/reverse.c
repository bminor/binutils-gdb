/* Reverse execution and reverse debugging.

   Copyright (C) 2006 Free Software Foundation, Inc.

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
#include "gdb_string.h"
#include "target.h"
#include "top.h"
#include "cli/cli-cmds.h"
#include "cli/cli-decode.h"

/* User interface for reverse debugging:
   Set exec-direction / show exec-direction commands
   (returns error unles target implements to_set_execdir method).  */

static const char exec_forward[] = "forward";
static const char exec_reverse[] = "reverse";
static const char *exec_direction = exec_forward;
static const char *exec_direction_names[] = {
  exec_forward,
  exec_reverse,
  NULL
};

static void
set_exec_direction_func (char *args, int from_tty, 
			 struct cmd_list_element *cmd)
{
  if (target_get_execution_direction () != EXEC_ERROR)
    {
      enum exec_direction_kind dir = EXEC_ERROR;

      if (!strcmp (exec_direction, exec_forward))
	dir = EXEC_FORWARD;
      else if (!strcmp (exec_direction, exec_reverse))
	dir = EXEC_REVERSE;

      if (target_set_execution_direction (dir) != EXEC_ERROR)
	return;
    }
  error (_("Target `%s' does not support execution-direction."),
	 target_shortname);
}

static void
show_exec_direction_func (struct ui_file *out, int from_tty, 
			  struct cmd_list_element *cmd, const char *value)
{
  enum exec_direction_kind dir = target_get_execution_direction ();

  switch (dir) {
  case EXEC_FORWARD:
    fprintf_filtered (out, "Forward.\n");
    break;
  case EXEC_REVERSE:
    fprintf_filtered (out, "Reverse.\n");
    break;
  case EXEC_ERROR:
  default:
    error (_("Target `%s' does not support execution-direction."),
	   target_shortname);
    break;
  }
}

/* User interface:
   reverse-step, reverse-next etc.
   (returns error unles target implements to_set_execdir method).  */

static void execdir_default (void *notused)
{
  /* Return execution direction to default state.  */
  target_set_execution_direction (EXEC_FORWARD);
}

static void
exec_reverse_once (char *cmd, char *args, int from_tty)
{
  /* String buffer for command consing.  */
  char reverse_command[512];
  enum exec_direction_kind dir = target_get_execution_direction ();

  if (dir == EXEC_ERROR)
    error (_("Target %s does not support this command."), target_shortname);

  if (dir == EXEC_REVERSE)
    error (_("Already in reverse mode.  Use '%s' or 'set exec-dir forward'."),
	   cmd);

  if (target_set_execution_direction (EXEC_REVERSE) == EXEC_ERROR)
    error (_("Target %s does not support this command."), target_shortname);

  make_cleanup (execdir_default, NULL);
  sprintf (reverse_command, "%s %s", cmd, args ? args : "");
  execute_command (reverse_command, from_tty);
}

static void
reverse_step (char *args, int from_tty)
{
  exec_reverse_once ("step", args, from_tty);
}

static void
reverse_stepi (char *args, int from_tty)
{
  exec_reverse_once ("stepi", args, from_tty);
}

static void
reverse_next (char *args, int from_tty)
{
  exec_reverse_once ("next", args, from_tty);
}

static void
reverse_nexti (char *args, int from_tty)
{
  exec_reverse_once ("nexti", args, from_tty);
}

static void
reverse_continue (char *args, int from_tty)
{
  exec_reverse_once ("continue", args, from_tty);
}

static void
reverse_finish (char *args, int from_tty)
{
  exec_reverse_once ("finish", args, from_tty);
}

void
_initialize_reverse (void)
{
  add_setshow_enum_cmd ("exec-direction", class_run, exec_direction_names,
			&exec_direction, "Set direction of execution.\n\
Options are 'forward' or 'reverse'.",
			"Show direction of execution (forward/reverse).",
			"Tells gdb whether to execute forward or backward.",
			set_exec_direction_func, show_exec_direction_func,
			&setlist, &showlist);

  add_com ("reverse-step", class_run, reverse_step, _("\
Step program backward until it reaches the beginning of another source line.\n\
Argument N means do this N times (or till program stops for another reason).")
	   );
  add_com_alias ("rs", "reverse-step", class_alias, 1);

  add_com ("reverse-next", class_run, reverse_next, _("\
Step program backward, proceeding through subroutine calls.\n\
Like the \"reverse-step\" command as long as subroutine calls do not happen;\n\
when they do, the call is treated as one instruction.\n\
Argument N means do this N times (or till program stops for another reason).")
	   );
  add_com_alias ("rn", "reverse-next", class_alias, 1);

  add_com ("reverse-stepi", class_run, reverse_stepi, _("\
Step backward exactly one instruction.\n\
Argument N means do this N times (or till program stops for another reason).")
	   );
  add_com_alias ("rsi", "reverse-stepi", class_alias, 0);

  add_com ("reverse-nexti", class_run, reverse_nexti, _("\
Step backward one instruction, but proceed through called subroutines.\n\
Argument N means do this N times (or till program stops for another reason).")
	   );
  add_com_alias ("rni", "reverse-nexti", class_alias, 0);

  add_com ("reverse-continue", class_run, reverse_continue, _("\
Continue program being debugged, running in reverse.\n\
If proceeding from breakpoint, a number N may be used as an argument,\n\
which means to set the ignore count of that breakpoint to N - 1 (so that\n\
the breakpoint won't break until the Nth time it is reached)."));
  add_com_alias ("rc", "reverse-continue", class_alias, 0);

  add_com ("reverse-finish", class_run, reverse_finish, _("\
Execute backward until just before selected stack frame is called."));
}
