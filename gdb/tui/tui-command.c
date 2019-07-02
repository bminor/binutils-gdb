/* Specific command window processing.

   Copyright (C) 1998-2019 Free Software Foundation, Inc.

   Contributed by Hewlett-Packard Company.

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
#include "tui/tui.h"
#include "tui/tui-data.h"
#include "tui/tui-win.h"
#include "tui/tui-io.h"
#include "tui/tui-command.h"

#include "gdb_curses.h"

/* See tui-command.h.  */

void
tui_cmd_window::clear_detail ()
{
  wmove (handle, 0, 0);
}

/* See tui-command.h.  */

void
tui_cmd_window::do_make_visible_with_new_height ()
{
#ifdef HAVE_WRESIZE
  wresize (handle, height, width);
#endif
  mvwin (handle, origin.y, origin.x);
  wmove (handle, 0, 0);
}

/* See tui-command.h.  */

int
tui_cmd_window::max_height () const
{
  return tui_term_height () - 4;
}

/* See tui-command.h.  */

void
tui_refresh_cmd_win (void)
{
  WINDOW *w = TUI_CMD_WIN->handle;

  wrefresh (w);

  /* FIXME: It's not clear why this is here.
     It was present in the original tui_puts code and is kept in order to
     not introduce some subtle breakage.  */
  fflush (stdout);
}
