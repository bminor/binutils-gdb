/* Everything about vfork catchpoints, for GDB.

   Copyright (C) 1986-2022 Free Software Foundation, Inc.

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

#include "annotate.h"
#include "arch-utils.h"
#include "breakpoint.h"
#include "cli/cli-decode.h"
#include "inferior.h"
#include "mi/mi-common.h"
#include "target.h"
#include "valprint.h"

/* An instance of this type is used to represent a fork or vfork
   catchpoint.  A breakpoint is really of this type iff its ops pointer points
   to CATCH_FORK_BREAKPOINT_OPS.  */

struct fork_catchpoint : public breakpoint
{
  /* True if the breakpoint is for vfork, false for fork.  */
  bool is_vfork;

  /* Process id of a child process whose forking triggered this
     catchpoint.  This field is only valid immediately after this
     catchpoint has triggered.  */
  ptid_t forked_inferior_pid;
};

/* Implement the "insert" breakpoint_ops method for fork
   catchpoints.  */

static int
insert_catch_fork (struct bp_location *bl)
{
  struct fork_catchpoint *c = (struct fork_catchpoint *) bl->owner;

  if (c->is_vfork)
    return target_insert_vfork_catchpoint (inferior_ptid.pid ());
  else
    return target_insert_fork_catchpoint (inferior_ptid.pid ());
}

/* Implement the "remove" breakpoint_ops method for fork
   catchpoints.  */

static int
remove_catch_fork (struct bp_location *bl, enum remove_bp_reason reason)
{
  struct fork_catchpoint *c = (struct fork_catchpoint *) bl->owner;

  if (c->is_vfork)
    return target_remove_vfork_catchpoint (inferior_ptid.pid ());
  else
    return target_remove_fork_catchpoint (inferior_ptid.pid ());
}

/* Implement the "breakpoint_hit" breakpoint_ops method for fork
   catchpoints.  */

static int
breakpoint_hit_catch_fork (const struct bp_location *bl,
			   const address_space *aspace, CORE_ADDR bp_addr,
			   const target_waitstatus &ws)
{
  struct fork_catchpoint *c = (struct fork_catchpoint *) bl->owner;

  if (ws.kind () != (c->is_vfork
		     ? TARGET_WAITKIND_VFORKED
		     : TARGET_WAITKIND_FORKED))
    return 0;

  c->forked_inferior_pid = ws.child_ptid ();
  return 1;
}

/* Implement the "print_it" breakpoint_ops method for fork
   catchpoints.  */

static enum print_stop_action
print_it_catch_fork (bpstat *bs)
{
  struct ui_out *uiout = current_uiout;
  struct breakpoint *b = bs->breakpoint_at;
  struct fork_catchpoint *c = (struct fork_catchpoint *) bs->breakpoint_at;

  annotate_catchpoint (b->number);
  maybe_print_thread_hit_breakpoint (uiout);
  if (b->disposition == disp_del)
    uiout->text ("Temporary catchpoint ");
  else
    uiout->text ("Catchpoint ");
  if (uiout->is_mi_like_p ())
    {
      uiout->field_string ("reason",
			   async_reason_lookup (c->is_vfork
						? EXEC_ASYNC_VFORK
						: EXEC_ASYNC_FORK));
      uiout->field_string ("disp", bpdisp_text (b->disposition));
    }
  uiout->field_signed ("bkptno", b->number);
  if (c->is_vfork)
    uiout->text (" (vforked process ");
  else
    uiout->text (" (forked process ");
  uiout->field_signed ("newpid", c->forked_inferior_pid.pid ());
  uiout->text ("), ");
  return PRINT_SRC_AND_LOC;
}

/* Implement the "print_one" breakpoint_ops method for fork
   catchpoints.  */

static void
print_one_catch_fork (struct breakpoint *b, struct bp_location **last_loc)
{
  struct fork_catchpoint *c = (struct fork_catchpoint *) b;
  struct value_print_options opts;
  struct ui_out *uiout = current_uiout;

  get_user_print_options (&opts);

  /* Field 4, the address, is omitted (which makes the columns not
     line up too nicely with the headers, but the effect is relatively
     readable).  */
  if (opts.addressprint)
    uiout->field_skip ("addr");
  annotate_field (5);
  const char *name = c->is_vfork ? "vfork" : "fork";
  uiout->text (name);
  if (c->forked_inferior_pid != null_ptid)
    {
      uiout->text (", process ");
      uiout->field_signed ("what", c->forked_inferior_pid.pid ());
      uiout->spaces (1);
    }

  if (uiout->is_mi_like_p ())
    uiout->field_string ("catch-type", name);
}

/* Implement the "print_mention" breakpoint_ops method for fork
   catchpoints.  */

static void
print_mention_catch_fork (struct breakpoint *b)
{
  struct fork_catchpoint *c = (struct fork_catchpoint *) b;
  printf_filtered (_("Catchpoint %d (%s)"), c->number,
		   c->is_vfork ? "vfork" : "fork");
}

/* Implement the "print_recreate" breakpoint_ops method for fork
   catchpoints.  */

static void
print_recreate_catch_fork (struct breakpoint *b, struct ui_file *fp)
{
  struct fork_catchpoint *c = (struct fork_catchpoint *) b;
  fprintf_unfiltered (fp, "catch %s",
		      c->is_vfork ? "vfork" : "fork");
  print_recreate_thread (b, fp);
}

/* The breakpoint_ops structure to be used in fork catchpoints.  */

static struct breakpoint_ops catch_fork_breakpoint_ops;

static void
create_fork_vfork_event_catchpoint (struct gdbarch *gdbarch,
				    bool temp, const char *cond_string,
				    bool is_vfork)
{
  std::unique_ptr<fork_catchpoint> c (new fork_catchpoint ());

  init_catchpoint (c.get (), gdbarch, temp, cond_string,
		   &catch_fork_breakpoint_ops);
  c->is_vfork = is_vfork;
  c->forked_inferior_pid = null_ptid;

  install_breakpoint (0, std::move (c), 1);
}

typedef enum
{
  catch_fork_temporary, catch_vfork_temporary,
  catch_fork_permanent, catch_vfork_permanent
}
catch_fork_kind;

static void
catch_fork_command_1 (const char *arg, int from_tty,
		      struct cmd_list_element *command)
{
  struct gdbarch *gdbarch = get_current_arch ();
  const char *cond_string = NULL;
  catch_fork_kind fork_kind;

  fork_kind = (catch_fork_kind) (uintptr_t) command->context ();
  bool temp = (fork_kind == catch_fork_temporary
	       || fork_kind == catch_vfork_temporary);

  if (!arg)
    arg = "";
  arg = skip_spaces (arg);

  /* The allowed syntax is:
     catch [v]fork
     catch [v]fork if <cond>

     First, check if there's an if clause.  */
  cond_string = ep_parse_optional_if_clause (&arg);

  if ((*arg != '\0') && !isspace (*arg))
    error (_("Junk at end of arguments."));

  /* If this target supports it, create a fork or vfork catchpoint
     and enable reporting of such events.  */
  switch (fork_kind)
    {
    case catch_fork_temporary:
    case catch_fork_permanent:
      create_fork_vfork_event_catchpoint (gdbarch, temp, cond_string, false);
      break;
    case catch_vfork_temporary:
    case catch_vfork_permanent:
      create_fork_vfork_event_catchpoint (gdbarch, temp, cond_string, true);
      break;
    default:
      error (_("unsupported or unknown fork kind; cannot catch it"));
      break;
    }
}

static void
initialize_ops ()
{
  struct breakpoint_ops *ops;

  initialize_breakpoint_ops ();

  /* Fork catchpoints.  */
  ops = &catch_fork_breakpoint_ops;
  *ops = base_breakpoint_ops;
  ops->insert_location = insert_catch_fork;
  ops->remove_location = remove_catch_fork;
  ops->breakpoint_hit = breakpoint_hit_catch_fork;
  ops->print_it = print_it_catch_fork;
  ops->print_one = print_one_catch_fork;
  ops->print_mention = print_mention_catch_fork;
  ops->print_recreate = print_recreate_catch_fork;
}

void _initialize_break_catch_fork ();
void
_initialize_break_catch_fork ()
{
  initialize_ops ();

  add_catch_command ("fork", _("Catch calls to fork."),
		     catch_fork_command_1,
		     NULL,
		     (void *) (uintptr_t) catch_fork_permanent,
		     (void *) (uintptr_t) catch_fork_temporary);
  add_catch_command ("vfork", _("Catch calls to vfork."),
		     catch_fork_command_1,
		     NULL,
		     (void *) (uintptr_t) catch_vfork_permanent,
		     (void *) (uintptr_t) catch_vfork_temporary);
}
