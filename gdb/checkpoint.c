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

/* TBD: actually manage the list here?
   How would that interplay with forks as written now?
*/

#include "defs.h"
#include "target.h"
#include "gdbcmd.h"
#include "checkpoint.h"
#include "value.h"
#include "regcache.h"
#include "inferior.h"

/*
 * Checkpoint list management (exported).
 */

static struct checkpoint_info *checkpoint_list, *checkpoint_iterator;
static int checkpoint_count;

struct checkpoint_info *
checkpoint_insert (void *client_data)
{
  struct checkpoint_info *cp = XZALLOC (struct checkpoint_info);

  cp->checkpoint_id = ++checkpoint_count;
  cp->client_data = client_data;
  cp->next = checkpoint_list;
  checkpoint_list = cp;
  return cp;
}

struct checkpoint_info *
checkpoint_first (void)
{
  if (checkpoint_list == NULL)
    return NULL;
  else
    {
      checkpoint_iterator = checkpoint_list;
      return checkpoint_iterator;
    }
}

struct checkpoint_info *
checkpoint_next (void)
{
  if (checkpoint_iterator == NULL)
    return NULL;
  else if (checkpoint_iterator->next == NULL)
    return NULL;
  else
    {
      checkpoint_iterator = checkpoint_iterator->next;
      return checkpoint_iterator;
    }
}

void
checkpoint_unlink (struct checkpoint_info *cp)
{
  if (cp == checkpoint_list)
    checkpoint_list = cp->next;
  else
    {
      struct checkpoint_info *prev;

      for (prev = checkpoint_first (); prev != NULL;
	   prev = checkpoint_next ())
	if (prev->next == cp)
	  break;

      if (prev == NULL)
	internal_error (__FILE__, __LINE__, 
			_("checkpoint is not linked!"));

      prev->next = cp->next;
    }
  xfree (cp);
}

struct checkpoint_info *
checkpoint_find_id (int id)
{
  struct checkpoint_info *cp;

  for (cp = checkpoint_first (); cp != NULL; cp = checkpoint_next ())
    if (cp->checkpoint_id == id)
      return cp;

  return NULL;
}

/*
 * Checkpoint commands (private).
 */

#if 0 /* First cut */

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

#else

static void
checkpoint_command (char *args, int from_tty)
{
  struct checkpoint_info *cp;

  if (!target_set_checkpoint)
    error (_("checkpoint command not implemented for this target."));

  cp = checkpoint_insert (NULL);
  if (cp != NULL)
    {
      if (from_tty)
	printf_filtered (_("Adding checkpoint #%d"), cp->checkpoint_id);
      cp->client_data = target_set_checkpoint (cp, from_tty);
      if (from_tty)
	puts_filtered (_("\n"));
    }
  else
    error (_("insert checkpoint failed"));
}

static void
delete_checkpoint_command (char *args, int from_tty)
{
  struct checkpoint_info *cp;

  if (!target_unset_checkpoint)
    error (_("delete checkpoint command not implemented for this target."));

  if (!args || !*args)
    error (_("Requires argument (checkpoint id to delete)"));

  /* FIXME: only accepts one argument, see breakpoint.  */
  cp = checkpoint_find_id (parse_and_eval_long (args));
  if (cp == NULL)
    error (_("Not found: checkpoint id %s"), args);

  target_unset_checkpoint (cp, from_tty);
  checkpoint_unlink (cp);
}

static void
info_checkpoints_command (char *args, int from_tty)
{
  struct checkpoint_info *cp;
  int requested = -1;
  int printed = 0;

  if (!target_info_checkpoints)
    error (_("info checkpoint command not implemented for this target."));

  cp = checkpoint_first ();
  if (cp == NULL)
    {
      printf_filtered ("No checkpoints.\n");
      return;
    }

  if (args && *args)
    requested = (int) parse_and_eval_long (args);

  do
    {
      if (requested == -1 || requested == cp->checkpoint_id)
	{
	  target_info_checkpoints (cp, from_tty);
	  printed = 1;
	}
      cp = checkpoint_next ();
    } while (cp != NULL);

  if (!printed)
    printf_filtered (_("No checkpoint number %d.\n"), requested);
}

static void
restart_command (char *args, int from_tty)
{
  extern void nullify_last_target_wait_ptid ();
  struct checkpoint_info *cp;

  if (!target_restore_checkpoint)
    error (_("restart command not implemented for this target."));

  if (!args || !*args)
    error (_("Requres argument (checkpoint id to restart)"));

  cp = checkpoint_find_id (parse_and_eval_long (args));
  if (cp == NULL)
    error (_("Not found: checkpoint id %s"), args);

  target_restore_checkpoint (cp, from_tty);

  registers_changed ();
  reinit_frame_cache ();
  stop_pc = regcache_read_pc (get_current_regcache ());
  nullify_last_target_wait_ptid ();
  if (from_tty)
    print_stack_frame (get_selected_frame (NULL), 1, SRC_AND_LOC);
}

#endif


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
