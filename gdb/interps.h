/* Manages interpreters for gdb.
   Copyright 2000,2002 Free Software Foundation, Inc.
   Written by Jim Ingham <jingham@apple.com> of Apple Computer, Inc.

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
   Boston, MA 02111-1307, USA. */

#ifndef GDB_INTERPRETER_H
#define GDB_INTERPRETER_H

typedef int (*interp_init_ftype) (void *data);
typedef int (*interp_resume_ftype) (void *data);
typedef int (*interp_suspend_ftype) (void *data);
typedef int (*interp_prompt_ftype) (void);
typedef int (*interp_exec_ftype) (void *data, char *command);

struct ui_out;
struct gdb_interpreter;

struct gdb_interpreter_procs
{
  interp_init_ftype init_proc;
  interp_resume_ftype resume_proc;
  interp_suspend_ftype suspend_proc;
  interp_exec_ftype exec_proc;
  interp_prompt_ftype prompt_proc_p;
};

extern struct gdb_interpreter
  *gdb_new_interpreter (char *name, void *data, struct ui_out *uiout,
			struct gdb_interpreter_procs *procs);

extern int gdb_add_interpreter (struct gdb_interpreter *interp);
extern int gdb_set_interpreter (struct gdb_interpreter *interp);
extern struct gdb_interpreter *gdb_lookup_interpreter (char *name);
extern struct gdb_interpreter *gdb_current_interpreter ();
extern struct ui_out *gdb_interpreter_ui_out (struct gdb_interpreter *interp);
extern int gdb_current_interpreter_is_named (char *interp_name);
extern int gdb_interpreter_exec (char *command_str);
extern int gdb_interpreter_display_prompt_p (void);
extern int gdb_interpreter_set_quiet (struct gdb_interpreter *interp,
				      int quiet);
extern int gdb_interpreter_is_quiet (struct gdb_interpreter *interp);
extern struct gdb_interpreter_procs *gdb_interpreter_get_procs (struct
								gdb_interpreter
								*interp);
extern void *gdb_interpreter_get_data (struct gdb_interpreter *interp);

void clear_interpreter_hooks ();

/* well-known interpreters */
#define GDB_INTERPRETER_CONSOLE		"console"
#define GDB_INTERPRETER_MI1             "mi1"
#define GDB_INTERPRETER_MI2		"mi2"
#define GDB_INTERPRETER_MI		"mi"
#endif /* GDB_INTERPRETER_H */
