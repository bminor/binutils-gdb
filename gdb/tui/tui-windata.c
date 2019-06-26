/* Data/register window display.

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
#include "tui/tui-wingeneral.h"
#include "tui/tui-regs.h"
#include "tui/tui-windata.h"
#include "gdb_curses.h"


/*****************************************
** STATIC LOCAL FUNCTIONS FORWARD DECLS    **
******************************************/



/*****************************************
** PUBLIC FUNCTIONS                        **
******************************************/


/* Answer the index first element displayed.  If none are displayed,
   then return (-1).  */
int
tui_data_window::first_data_item_displayed ()
{
  for (int i = 0; i < regs_content.size (); i++)
    {
      struct tui_gen_win_info *data_item_win;

      data_item_win = regs_content[i].get ();
      if (data_item_win->handle != NULL && data_item_win->is_visible)
	return i;
    }

  return -1;
}

/* See tui-data.h.  */

void
tui_data_window::delete_data_content_windows ()
{
  for (auto &&win : regs_content)
    {
      tui_delete_win (win->handle);
      win->handle = NULL;
      win->is_visible = false;
    }
}


void
tui_erase_data_content (const char *prompt)
{
  werase (TUI_DATA_WIN->handle);
  tui_check_and_display_highlight_if_needed (TUI_DATA_WIN);
  if (prompt != NULL)
    {
      int half_width = (TUI_DATA_WIN->width - 2) / 2;
      int x_pos;

      if (strlen (prompt) >= half_width)
	x_pos = 1;
      else
	x_pos = half_width - strlen (prompt);
      mvwaddstr (TUI_DATA_WIN->handle,
		 (TUI_DATA_WIN->height / 2),
		 x_pos,
		 (char *) prompt);
    }
  wrefresh (TUI_DATA_WIN->handle);
}

/* See tui-data.h.  */

void
tui_data_window::display_all_data ()
{
  if (regs_content.empty ())
    tui_erase_data_content (NO_DATA_STRING);
  else
    {
      tui_erase_data_content (NULL);
      delete_data_content_windows ();
      tui_check_and_display_highlight_if_needed (this);
      tui_display_registers_from (0);
    }
}


/* Function to redisplay the contents of the data window.  */
void
tui_data_window::refresh_all ()
{
  tui_erase_data_content (NULL);
  if (!regs_content.empty ())
    {
      int first_element = first_data_item_displayed ();

      if (first_element >= 0)	/* Re-use existing windows.  */
	{
	  int first_line = (-1);

	  if (first_element < regs_content.size ())
	    first_line = line_from_reg_element_no (first_element);

	  if (first_line >= 0)
	    {
	      tui_erase_data_content (NULL);
	      tui_display_registers_from_line (first_line);
	    }
	}
    }
}


/* Scroll the data window vertically forward or backward.  */
void
tui_data_window::do_scroll_vertical (int num_to_scroll)
{
  int first_element_no;
  int first_line = (-1);

  first_element_no = first_data_item_displayed ();
  if (first_element_no < regs_content.size ())
    first_line = line_from_reg_element_no (first_element_no);
  else
    { /* Calculate the first line from the element number which is in
        the general data content.  */
    }

  if (first_line >= 0)
    {
      first_line += num_to_scroll;
      tui_erase_data_content (NULL);
      delete_data_content_windows ();
      tui_display_registers_from_line (first_line);
    }
}


/*****************************************
** STATIC LOCAL FUNCTIONS               **
******************************************/
