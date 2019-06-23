/* General window behavior.

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
#include "tui/tui-win.h"

#include "gdb_curses.h"

/***********************
** PUBLIC FUNCTIONS
***********************/

/* See tui-data.h.  */

void
tui_gen_win_info::refresh_window ()
{
  if (handle != NULL)
    wrefresh (handle);
}

/* See tui-data.h.  */

void
tui_data_window::refresh_window ()
{
  if (!regs_content.empty ())
    {
      for (auto &&win : regs_content)
	{
	  if (win != NULL && win->handle != NULL)
	    wrefresh (win->handle);
	}
    }
  else
    tui_gen_win_info::refresh_window ();
}

/* Function to delete the curses window, checking for NULL.  */
void
tui_delete_win (WINDOW *window)
{
  if (window != NULL)
    delwin (window);
}


/* Draw a border arround the window.  */
static void
box_win (struct tui_gen_win_info *win_info, 
	 int highlight_flag)
{
  if (win_info && win_info->handle)
    {
      WINDOW *win;
      int attrs;

      win = win_info->handle;
      if (highlight_flag == HILITE)
        attrs = tui_active_border_attrs;
      else
        attrs = tui_border_attrs;

      wattron (win, attrs);
#ifdef HAVE_WBORDER
      wborder (win, tui_border_vline, tui_border_vline,
               tui_border_hline, tui_border_hline,
               tui_border_ulcorner, tui_border_urcorner,
               tui_border_llcorner, tui_border_lrcorner);
#else
      box (win, tui_border_vline, tui_border_hline);
#endif
      if (win_info->title)
        mvwaddstr (win, 0, 3, win_info->title);
      wattroff (win, attrs);
    }
}


void
tui_unhighlight_win (struct tui_win_info *win_info)
{
  if (win_info != NULL 
      && win_info->can_highlight
      && win_info->handle != NULL)
    {
      box_win (win_info, NO_HILITE);
      wrefresh (win_info->handle);
      win_info->set_highlight (false);
    }
}


void
tui_highlight_win (struct tui_win_info *win_info)
{
  if (win_info != NULL
      && win_info->can_highlight
      && win_info->handle != NULL)
    {
      box_win (win_info, HILITE);
      wrefresh (win_info->handle);
      win_info->set_highlight (true);
    }
}

void
tui_check_and_display_highlight_if_needed (struct tui_win_info *win_info)
{
  if (win_info != NULL && win_info->type != CMD_WIN)
    {
      if (win_info->is_highlighted)
	tui_highlight_win (win_info);
      else
	tui_unhighlight_win (win_info);

    }
  return;
}


void
tui_make_window (struct tui_gen_win_info *win_info, enum tui_box box_it)
{
  WINDOW *handle;

  handle = newwin (win_info->height,
		   win_info->width,
		   win_info->origin.y,
		   win_info->origin.x);
  win_info->handle = handle;
  if (handle != NULL)
    {
      if (box_it == BOX_WINDOW)
	box_win (win_info, NO_HILITE);
      win_info->is_visible = true;
      scrollok (handle, TRUE);
    }
}


/* We can't really make windows visible, or invisible.  So we have to
   delete the entire window when making it visible, and create it
   again when making it visible.  */
void
tui_gen_win_info::make_visible (bool visible)
{
  if (visible)
    {
      if (!is_visible)
	{
	  tui_make_window (this, (tui_win_is_auxiliary (type)
				  ? DONT_BOX_WINDOW : BOX_WINDOW));
	  is_visible = true;
	}
    }
  else if (!visible
	   && is_visible
	   && handle != NULL)
    {
      is_visible = false;
      tui_delete_win (handle);
      handle = NULL;
    }
}

void
tui_make_visible (struct tui_gen_win_info *win_info)
{
  win_info->make_visible (true);
}

void
tui_make_invisible (struct tui_gen_win_info *win_info)
{
  win_info->make_visible (false);
}

/* See tui-data.h.  */

void
tui_source_window_base::make_visible (bool visible)
{
  if (execution_info != nullptr)
    execution_info->make_visible (visible);
  tui_win_info::make_visible (visible);
}

/* Makes all windows invisible (except the command and locator
   windows).  */
static void
make_all_visible (bool visible)
{
  int i;

  for (i = 0; i < MAX_MAJOR_WINDOWS; i++)
    {
      if (tui_win_list[i] != NULL)
	tui_win_list[i]->make_visible (visible);
    }

  return;
}

void
tui_make_all_visible (void)
{
  make_all_visible (true);
}

void
tui_make_all_invisible (void)
{
  make_all_visible (false);
}

/* See tui-data.h.  */

void
tui_win_info::refresh ()
{
  touchwin (handle);
  refresh_window ();
}

/* See tui-data.h.  */

void
tui_source_window_base::refresh ()
{
  touchwin (execution_info->handle);
  execution_info->refresh_window ();
  tui_win_info::refresh ();
}

/* Function to refresh all the windows currently displayed.  */

void
tui_refresh_all (struct tui_win_info **list)
{
  int type;
  struct tui_locator_window *locator = tui_locator_win_info_ptr ();

  for (type = SRC_WIN; (type < MAX_MAJOR_WINDOWS); type++)
    {
      if (list[type] && list[type]->is_visible)
	list[type]->refresh ();
    }
  if (locator->is_visible)
    {
      touchwin (locator->handle);
      locator->refresh_window ();
    }
}


/*********************************
** Local Static Functions
*********************************/
