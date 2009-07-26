/* Target-agnostic module for checkpoint commands.

   Copyright (C) 2009 Free Software Foundation, Inc.

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

/* XXX mvs put the actual commands here, and add them to gdb's
   command lists only by request.

   TBD: actually manage the list here?
   How would that interplay with forks as written now?
*/

#include "defs.h"
#include "target.h"
#include "gdbcmd.h"

/* Set a checkpoint (call target_ops method).  */

static void
checkpoint_command (char *args, int from_tty)
{
  if (target_set_checkpoint)
    target_set_checkpoint (args, from_tty);
  else
    error (_("checkpoint command not implemented for this target."));
}

/* Restore a checkpoint (call target_ops method).  */

static void
restart_command (char *args, int from_tty)
{
  if (target_restore_checkpoint)
    target_restore_checkpoint (args, from_tty);
  else
    error (_("restart command not implemented for this target."));
}

/* Delete a checkpoint (call target_ops method).  */

static void
delete_checkpoint_command (char *args, int from_tty)
{
  if (target_unset_checkpoint)
    target_unset_checkpoint (args, from_tty);
  else
    error (_("delete checkpoint command not implemented for this target."));
}

static void
info_checkpoints_command (char *args, int from_tty)
{
  if (target_info_checkpoints)
    target_info_checkpoints (args, from_tty);
  else
    error (_("info checkpoints command not implemented for this target."));
}


/* Initializer function checkpoint_init().
   Note -- not called in the usual gdb module-initializer manner, 
   but only by explicit request by targets that want checkpoints.  */

void
checkpoint_init (void)
{
  add_com ("checkpoint", class_obscure, checkpoint_command, _("\
Create a checkpoint that can be restored later via 'restart'."));

  /* Restart command: restore the context of a specified checkpoint
     process.  */

  add_com ("restart", class_obscure, restart_command, _("\
restart <n>: restore program context from a checkpoint.\n\
Argument 'n' is checkpoint ID, as displayed by 'info checkpoints'."));

  add_cmd ("checkpoint", class_obscure, delete_checkpoint_command, _("\
Delete a previously saved checkpoint."),
	   &deletelist);

  add_info ("checkpoints", info_checkpoints_command,
	    _("IDs of currently known checkpoints."));
}
