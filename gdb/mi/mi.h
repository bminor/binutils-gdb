/* MI Internal Functions
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

#ifndef MI_H
#define MI_H
/* The mi interpreters. */
struct ui_file;
struct breakpoint;
struct gdb_interpreter;
extern struct gdb_interpreter *mi_interp;
extern struct gdb_interpreter *mi1_interp;

extern void mi_setup_architecture_data (void);
extern void mi_register_gdbarch_swap (void);

/* MI's output channels */
extern struct ui_file *mi_stdout;
extern struct ui_file *mi_stderr;
extern struct ui_file *mi_stdlog;
extern struct ui_file *mi_stdtarg;
extern struct ui_file *mi_event_channel;

/* Events/Hooks */
extern void mi_load_progress (const char *section_name,
			      unsigned long sent_so_far,
			      unsigned long total_section,
			      unsigned long total_sent,
			      unsigned long grand_total);
extern void mi_interp_frame_changed_hook (int new_frame_number);
extern void mi_interp_context_hook (int thread_id);

extern void mi_create_breakpoint (int bpnum);
extern void mi_modify_breakpoint (int bpnum);
extern void mi_delete_breakpoint (int bpnum);
extern void mi_create_tracepoint (int bpnum);
extern void mi_modify_tracepoint (int bpnum);
extern void mi_delete_tracepoint (int bpnum);
extern void mi_architecture_changed (void);
extern void mi_target_changed (void);
extern void mi_selected_frame_level_changed (int level);
extern void mi_context_changed (int thread_id);
#endif /* MI_H */
