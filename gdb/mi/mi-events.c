/* MI Event Handlers
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
#include "ui-out.h"
#include "interps.h"
#include "gdb.h"
#include "breakpoint.h"

#include "mi.h"

void 
mi_interp_stack_changed_hook (void)
{
  struct ui_out *saved_ui_out = uiout;
  struct mi_out *tmp_mi_out;

  if (gdb_interpreter_current_is_named_p (GDB_INTERPRETER_MI1))
    uiout = gdb_interpreter_ui_out (mi1_interp);
  else
    uiout = gdb_interpreter_ui_out (mi_interp);

  ui_out_list_begin (uiout, "MI_HOOK_RESULT");
  ui_out_field_string (uiout, "HOOK_TYPE", "stack_changed");
  ui_out_list_end (uiout);
  uiout = saved_ui_out;
}

static void
event_notify (const char *string, ...)
{
  va_list args;

  if (!gdb_interpreter_current_is_named_p (GDB_INTERPRETER_MI1)
      && !gdb_interpreter_current_is_named_p (GDB_INTERPRETER_MI2))
    {
      va_start (args, string);
      vfprintf_unfiltered (mi_event_channel, string, args);
      va_end (args);
      gdb_flush (mi_event_channel);
    }
}

/* breakpoint-create,number=bpnum */
void
mi_create_breakpoint (int bpnum)
{
  event_notify ("breakpoint-create,number=\"%d\"", bpnum);
}

void
mi_modify_breakpoint (int bpnum)
{
  event_notify ("breakpoint-modify,number=\"%d\"", bpnum);
}

void
mi_delete_breakpoint (int bpnum)
{
  event_notify ("breakpoint-delete,number=\"%d\"", bpnum);
}

void
mi_create_tracepoint (int tpnum)
{
  event_notify ("tracepoint-create,number=\"%d\"", tpnum);
}

void
mi_modify_tracepoint (int tpnum)
{
  event_notify ("tracepoint-modify,number=\"%d\"", tpnum);
}

void
mi_delete_tracepoint (int tpnum)
{
  event_notify ("tracepoint-delete,number=\"%d\"", tpnum);
}

void
mi_architecture_changed (void)
{
  event_notify ("architecture-changed");
}

void
mi_target_changed (void)
{
  event_notify ("target-changed");
}

void
mi_selected_frame_level_changed (int level)
{
  event_notify ("selected-frame-level-changed,level=\"%d\"", level);
}

void
mi_context_changed (int thread_id)
{
  event_notify ("context-changed,thread=\"%d\"", thread_id);
}

struct gdb_events mi_event_handlers =
  {
    mi_create_breakpoint,
    mi_delete_breakpoint,
    mi_modify_breakpoint,
    mi_create_tracepoint,
    mi_delete_tracepoint,
    mi_modify_tracepoint,
    mi_architecture_changed,
    mi_target_changed,
    mi_selected_frame_level_changed,
    mi_context_changed
  };

