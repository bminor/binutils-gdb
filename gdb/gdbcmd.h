/* Header file for GDB-specific command-line stuff.
   Copyright (C) 1986, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "command.h"

/* Chain containing all defined commands.  */

extern struct cmd_list_element *cmdlist;

/* Chain containing all defined info subcommands.  */

extern struct cmd_list_element *infolist;

/* Chain containing all defined enable subcommands. */

extern struct cmd_list_element *enablelist;

/* Chain containing all defined disable subcommands. */

extern struct cmd_list_element *disablelist;

/* Chain containing all defined delete subcommands. */

extern struct cmd_list_element *deletelist;

/* Chain containing all defined "enable breakpoint" subcommands. */

extern struct cmd_list_element *enablebreaklist;

/* Chain containing all defined set subcommands */

extern struct cmd_list_element *setlist;

/* Chain containing all defined show subcommands.  */
extern struct cmd_list_element *showlist;

/* Chain containing all defined \"set history\".  */

extern struct cmd_list_element *sethistlist;

/* Chain containing all defined \"show history\".  */
extern struct cmd_list_element *showhistlist;

/* Chain containing all defined \"unset history\".  */

extern struct cmd_list_element *unsethistlist;

void execute_command ();
char **noop_completer ();
