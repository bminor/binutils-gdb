/* ***DEPRECATED***  The gdblib files must not be calling/using things in any
   of the possible command languages.  If necessary, a hook (that may be
   present or not) must be used and set to the appropriate routine by any
   command language that cares about it.  If you are having to include this
   file you are possibly doing things the old way.  This file will disappear.
   fnasser@redhat.com    */

/* Header file for GDB-specific command-line stuff.
   Copyright (C) 1986-2024 Free Software Foundation, Inc.

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

#if !defined (GDBCMD_H)
#define GDBCMD_H 1

#include "command.h"
#include "ui-out.h"
#include "cli/cli-script.h"
#include "cli/cli-cmds.h"

extern void print_command_line (struct command_line *, unsigned int,
				struct ui_file *);
extern void print_command_lines (struct ui_out *,
				 struct command_line *, unsigned int);

/* Chains containing all defined "set/show style" subcommands.  */
extern struct cmd_list_element *style_set_list;
extern struct cmd_list_element *style_show_list;

#endif /* !defined (GDBCMD_H) */
