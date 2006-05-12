/* MI Internal Functions for GDB, the GNU debugger.

   Copyright (C) 2003 Free Software Foundation, Inc.

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

#ifndef MI_MAIN_H
#define MI_MAIN_H

extern int mi_dont_register_continuation;
extern char *current_command_token;
extern struct interp *mi_interp;
struct mi_continuation_arg;

extern void
mi_interpreter_exec_continuation (struct continuation_arg *in_arg);

extern struct mi_continuation_arg *
  mi_setup_continuation_arg (struct cleanup *cleanups);

extern void mi_setup_architecture_data (void);

extern void mi_load_progress (const char *section_name,
			      unsigned long sent_so_far,
			      unsigned long total_section,
			      unsigned long total_sent,
			      unsigned long grand_total);
#endif

