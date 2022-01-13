/* Everything about load/unload catchpoints, for GDB.

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
#include "mi/mi-common.h"
#include "progspace.h"
#include "solist.h"
#include "target.h"
#include "valprint.h"

/* An instance of this type is used to represent an solib catchpoint.
   A breakpoint is really of this type iff its ops pointer points to
   CATCH_SOLIB_BREAKPOINT_OPS.  */

struct solib_catchpoint : public breakpoint
{
  /* True for "catch load", false for "catch unload".  */
  bool is_load;

  /* Regular expression to match, if any.  COMPILED is only valid when
     REGEX is non-NULL.  */
  gdb::unique_xmalloc_ptr<char> regex;
  std::unique_ptr<compiled_regex> compiled;
};

static int
insert_catch_solib (struct bp_location *ignore)
{
  return 0;
}

static int
remove_catch_solib (struct bp_location *ignore, enum remove_bp_reason reason)
{
  return 0;
}

static int
breakpoint_hit_catch_solib (const struct bp_location *bl,
			    const address_space *aspace,
			    CORE_ADDR bp_addr,
			    const target_waitstatus &ws)
{
  struct solib_catchpoint *self = (struct solib_catchpoint *) bl->owner;

  if (ws.kind () == TARGET_WAITKIND_LOADED)
    return 1;

  for (breakpoint *other : all_breakpoints ())
    {
      if (other == bl->owner)
	continue;

      if (other->type != bp_shlib_event)
	continue;

      if (self->pspace != NULL && other->pspace != self->pspace)
	continue;

      for (bp_location *other_bl : other->locations ())
	{
	  if (other->ops->breakpoint_hit (other_bl, aspace, bp_addr, ws))
	    return 1;
	}
    }

  return 0;
}

static void
check_status_catch_solib (struct bpstat *bs)
{
  struct solib_catchpoint *self
    = (struct solib_catchpoint *) bs->breakpoint_at;

  if (self->is_load)
    {
      for (so_list *iter : current_program_space->added_solibs)
	{
	  if (!self->regex
	      || self->compiled->exec (iter->so_name, 0, NULL, 0) == 0)
	    return;
	}
    }
  else
    {
      for (const std::string &iter : current_program_space->deleted_solibs)
	{
	  if (!self->regex
	      || self->compiled->exec (iter.c_str (), 0, NULL, 0) == 0)
	    return;
	}
    }

  bs->stop = 0;
  bs->print_it = print_it_noop;
}

static enum print_stop_action
print_it_catch_solib (bpstat *bs)
{
  struct breakpoint *b = bs->breakpoint_at;
  struct ui_out *uiout = current_uiout;

  annotate_catchpoint (b->number);
  maybe_print_thread_hit_breakpoint (uiout);
  if (b->disposition == disp_del)
    uiout->text ("Temporary catchpoint ");
  else
    uiout->text ("Catchpoint ");
  uiout->field_signed ("bkptno", b->number);
  uiout->text ("\n");
  if (uiout->is_mi_like_p ())
    uiout->field_string ("disp", bpdisp_text (b->disposition));
  print_solib_event (1);
  return PRINT_SRC_AND_LOC;
}

static void
print_one_catch_solib (struct breakpoint *b, struct bp_location **locs)
{
  struct solib_catchpoint *self = (struct solib_catchpoint *) b;
  struct value_print_options opts;
  struct ui_out *uiout = current_uiout;

  get_user_print_options (&opts);
  /* Field 4, the address, is omitted (which makes the columns not
     line up too nicely with the headers, but the effect is relatively
     readable).  */
  if (opts.addressprint)
    {
      annotate_field (4);
      uiout->field_skip ("addr");
    }

  std::string msg;
  annotate_field (5);
  if (self->is_load)
    {
      if (self->regex)
	msg = string_printf (_("load of library matching %s"),
			     self->regex.get ());
      else
	msg = _("load of library");
    }
  else
    {
      if (self->regex)
	msg = string_printf (_("unload of library matching %s"),
			     self->regex.get ());
      else
	msg = _("unload of library");
    }
  uiout->field_string ("what", msg);

  if (uiout->is_mi_like_p ())
    uiout->field_string ("catch-type", self->is_load ? "load" : "unload");
}

static void
print_mention_catch_solib (struct breakpoint *b)
{
  struct solib_catchpoint *self = (struct solib_catchpoint *) b;

  gdb_printf (_("Catchpoint %d (%s)"), b->number,
	      self->is_load ? "load" : "unload");
}

static void
print_recreate_catch_solib (struct breakpoint *b, struct ui_file *fp)
{
  struct solib_catchpoint *self = (struct solib_catchpoint *) b;

  gdb_printf (fp, "%s %s",
	      b->disposition == disp_del ? "tcatch" : "catch",
	      self->is_load ? "load" : "unload");
  if (self->regex)
    gdb_printf (fp, " %s", self->regex.get ());
  gdb_printf (fp, "\n");
}

static struct breakpoint_ops catch_solib_breakpoint_ops;

/* See breakpoint.h.  */

void
add_solib_catchpoint (const char *arg, bool is_load, bool is_temp, bool enabled)
{
  struct gdbarch *gdbarch = get_current_arch ();

  if (!arg)
    arg = "";
  arg = skip_spaces (arg);

  std::unique_ptr<solib_catchpoint> c (new solib_catchpoint ());

  if (*arg != '\0')
    {
      c->compiled.reset (new compiled_regex (arg, REG_NOSUB,
					     _("Invalid regexp")));
      c->regex = make_unique_xstrdup (arg);
    }

  c->is_load = is_load;
  init_catchpoint (c.get (), gdbarch, is_temp, NULL,
		   &catch_solib_breakpoint_ops);

  c->enable_state = enabled ? bp_enabled : bp_disabled;

  install_breakpoint (0, std::move (c), 1);
}

/* A helper function that does all the work for "catch load" and
   "catch unload".  */

static void
catch_load_or_unload (const char *arg, int from_tty, int is_load,
		      struct cmd_list_element *command)
{
  const int enabled = 1;
  bool temp = command->context () == CATCH_TEMPORARY;

  add_solib_catchpoint (arg, is_load, temp, enabled);
}

static void
catch_load_command_1 (const char *arg, int from_tty,
		      struct cmd_list_element *command)
{
  catch_load_or_unload (arg, from_tty, 1, command);
}

static void
catch_unload_command_1 (const char *arg, int from_tty,
			struct cmd_list_element *command)
{
  catch_load_or_unload (arg, from_tty, 0, command);
}

static void
initialize_ops ()
{
  struct breakpoint_ops *ops;

  initialize_breakpoint_ops ();

  /* Solib-related catchpoints.  */
  ops = &catch_solib_breakpoint_ops;
  *ops = base_breakpoint_ops;
  ops->insert_location = insert_catch_solib;
  ops->remove_location = remove_catch_solib;
  ops->breakpoint_hit = breakpoint_hit_catch_solib;
  ops->check_status = check_status_catch_solib;
  ops->print_it = print_it_catch_solib;
  ops->print_one = print_one_catch_solib;
  ops->print_mention = print_mention_catch_solib;
  ops->print_recreate = print_recreate_catch_solib;

}

void _initialize_break_catch_load ();
void
_initialize_break_catch_load ()
{
  initialize_ops ();

  add_catch_command ("load", _("Catch loads of shared libraries.\n\
Usage: catch load [REGEX]\n\
If REGEX is given, only stop for libraries matching the regular expression."),
		     catch_load_command_1,
		     NULL,
		     CATCH_PERMANENT,
		     CATCH_TEMPORARY);
  add_catch_command ("unload", _("Catch unloads of shared libraries.\n\
Usage: catch unload [REGEX]\n\
If REGEX is given, only stop for libraries matching the regular expression."),
		     catch_unload_command_1,
		     NULL,
		     CATCH_PERMANENT,
		     CATCH_TEMPORARY);
}
