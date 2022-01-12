/* Everything about exec catchpoints, for GDB.

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

/* Exec catchpoints.  */

/* An instance of this type is used to represent an exec catchpoint.
   A breakpoint is really of this type iff its ops pointer points to
   CATCH_EXEC_BREAKPOINT_OPS.  */

struct exec_catchpoint : public breakpoint
{
  /* Filename of a program whose exec triggered this catchpoint.
     This field is only valid immediately after this catchpoint has
     triggered.  */
  gdb::unique_xmalloc_ptr<char> exec_pathname;
};

static int
insert_catch_exec (struct bp_location *bl)
{
  return target_insert_exec_catchpoint (inferior_ptid.pid ());
}

static int
remove_catch_exec (struct bp_location *bl, enum remove_bp_reason reason)
{
  return target_remove_exec_catchpoint (inferior_ptid.pid ());
}

static int
breakpoint_hit_catch_exec (const struct bp_location *bl,
			   const address_space *aspace, CORE_ADDR bp_addr,
			   const target_waitstatus &ws)
{
  struct exec_catchpoint *c = (struct exec_catchpoint *) bl->owner;

  if (ws.kind () != TARGET_WAITKIND_EXECD)
    return 0;

  c->exec_pathname = make_unique_xstrdup (ws.execd_pathname ());
  return 1;
}

static enum print_stop_action
print_it_catch_exec (bpstat *bs)
{
  struct ui_out *uiout = current_uiout;
  struct breakpoint *b = bs->breakpoint_at;
  struct exec_catchpoint *c = (struct exec_catchpoint *) b;

  annotate_catchpoint (b->number);
  maybe_print_thread_hit_breakpoint (uiout);
  if (b->disposition == disp_del)
    uiout->text ("Temporary catchpoint ");
  else
    uiout->text ("Catchpoint ");
  if (uiout->is_mi_like_p ())
    {
      uiout->field_string ("reason", async_reason_lookup (EXEC_ASYNC_EXEC));
      uiout->field_string ("disp", bpdisp_text (b->disposition));
    }
  uiout->field_signed ("bkptno", b->number);
  uiout->text (" (exec'd ");
  uiout->field_string ("new-exec", c->exec_pathname.get ());
  uiout->text ("), ");

  return PRINT_SRC_AND_LOC;
}

static void
print_one_catch_exec (struct breakpoint *b, struct bp_location **last_loc)
{
  struct exec_catchpoint *c = (struct exec_catchpoint *) b;
  struct value_print_options opts;
  struct ui_out *uiout = current_uiout;

  get_user_print_options (&opts);

  /* Field 4, the address, is omitted (which makes the columns
     not line up too nicely with the headers, but the effect
     is relatively readable).  */
  if (opts.addressprint)
    uiout->field_skip ("addr");
  annotate_field (5);
  uiout->text ("exec");
  if (c->exec_pathname != NULL)
    {
      uiout->text (", program \"");
      uiout->field_string ("what", c->exec_pathname.get ());
      uiout->text ("\" ");
    }

  if (uiout->is_mi_like_p ())
    uiout->field_string ("catch-type", "exec");
}

static void
print_mention_catch_exec (struct breakpoint *b)
{
  printf_filtered (_("Catchpoint %d (exec)"), b->number);
}

/* Implement the "print_recreate" breakpoint_ops method for exec
   catchpoints.  */

static void
print_recreate_catch_exec (struct breakpoint *b, struct ui_file *fp)
{
  fprintf_unfiltered (fp, "catch exec");
  print_recreate_thread (b, fp);
}

static struct breakpoint_ops catch_exec_breakpoint_ops;

/* This function attempts to parse an optional "if <cond>" clause
   from the arg string.  If one is not found, it returns NULL.

   Else, it returns a pointer to the condition string.  (It does not
   attempt to evaluate the string against a particular block.)  And,
   it updates arg to point to the first character following the parsed
   if clause in the arg string.  */

const char *
ep_parse_optional_if_clause (const char **arg)
{
  const char *cond_string;

  if (((*arg)[0] != 'i') || ((*arg)[1] != 'f') || !isspace ((*arg)[2]))
    return NULL;

  /* Skip the "if" keyword.  */
  (*arg) += 2;

  /* Skip any extra leading whitespace, and record the start of the
     condition string.  */
  *arg = skip_spaces (*arg);
  cond_string = *arg;

  /* Assume that the condition occupies the remainder of the arg
     string.  */
  (*arg) += strlen (cond_string);

  return cond_string;
}

/* Commands to deal with catching events, such as signals, exceptions,
   process start/exit, etc.  */

static void
catch_exec_command_1 (const char *arg, int from_tty,
		      struct cmd_list_element *command)
{
  struct gdbarch *gdbarch = get_current_arch ();
  const char *cond_string = NULL;
  bool temp = command->context () == CATCH_TEMPORARY;

  if (!arg)
    arg = "";
  arg = skip_spaces (arg);

  /* The allowed syntax is:
     catch exec
     catch exec if <cond>

     First, check if there's an if clause.  */
  cond_string = ep_parse_optional_if_clause (&arg);

  if ((*arg != '\0') && !isspace (*arg))
    error (_("Junk at end of arguments."));

  std::unique_ptr<exec_catchpoint> c (new exec_catchpoint ());
  init_catchpoint (c.get (), gdbarch, temp, cond_string,
		   &catch_exec_breakpoint_ops);
  c->exec_pathname.reset ();

  install_breakpoint (0, std::move (c), 1);
}

static void
initialize_ops ()
{
  struct breakpoint_ops *ops;

  initialize_breakpoint_ops ();

  /* Exec catchpoints.  */
  ops = &catch_exec_breakpoint_ops;
  *ops = base_breakpoint_ops;
  ops->insert_location = insert_catch_exec;
  ops->remove_location = remove_catch_exec;
  ops->breakpoint_hit = breakpoint_hit_catch_exec;
  ops->print_it = print_it_catch_exec;
  ops->print_one = print_one_catch_exec;
  ops->print_mention = print_mention_catch_exec;
  ops->print_recreate = print_recreate_catch_exec;
}

void _initialize_break_catch_exec ();
void
_initialize_break_catch_exec ()
{
  initialize_ops ();

  add_catch_command ("exec", _("Catch calls to exec."),
		     catch_exec_command_1,
		     NULL,
		     CATCH_PERMANENT,
		     CATCH_TEMPORARY);
}
