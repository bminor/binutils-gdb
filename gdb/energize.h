/* Energize interface defs for GDB.
   Copyright (C) 1992 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#if !defined (ENERGIZE_H)
#define ENERGIZE_H 1

#ifdef __STDC__
struct cmd_list_element;
struct symbol;
struct type;
struct objfile;
struct breakpoint;
#endif /* __STDC__ */

/* Non-zero means that we're doing the energize interface. */
extern int energize;

/* Get a pty for use with energize */
extern char *energize_getpty PARAMS ((void));

/* Notify energize of new process creation */
extern void energize_new_process PARAMS ((void));

/* Low level wait routine for wait_for_inferior */
extern int energize_wait PARAMS ((int *));

/* Wait routine for processes spawned by the shell command */
extern int energize_shell_wait PARAMS ((int *statusp));

/* Initialize */
extern void energize_initialize PARAMS ((char *, char *));

/* Main loop for energize protocol driver */
extern void energize_main_loop PARAMS ((void));

/* Command hook for energize */
extern void energize_call_command PARAMS ((struct cmd_list_element *,
					   char *, int));

/* Read commands for the command command, and others */
extern char *energize_command_line_input PARAMS ((char *, int));

extern void energize_start_variable_annotation PARAMS ((char *,
							struct symbol *,
							struct type *,
							CORE_ADDR,
							char *));

extern void energize_end_variable_annotation PARAMS ((void));

extern void energize_annotate_function PARAMS ((char *, int, int));

extern void energize_symbol_file PARAMS ((struct objfile *));

/*extern void energize_query PARAMS ((char *, ...));*/
extern void energize_query ();	/* Prototypes for varargs don't work */

extern void energize_acknowledge_query PARAMS ((char *));

extern void energize_fputs PARAMS ((const char *));

extern void energize_condition_breakpoint PARAMS ((struct breakpoint *));

extern void energize_commands_breakpoint PARAMS ((struct breakpoint *));

extern void energize_ignore_breakpoint PARAMS ((struct breakpoint *));

extern void energize_create_breakpoint PARAMS ((struct breakpoint *));

extern void energize_delete_breakpoint PARAMS ((struct breakpoint *));

extern void energize_enable_breakpoint PARAMS ((struct breakpoint *));

extern void energize_disable_breakpoint PARAMS ((struct breakpoint *));

#endif /* !defined (ENERGIZE_H) */
