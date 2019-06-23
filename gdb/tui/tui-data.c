/* TUI data manipulation routines.

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
#include "symtab.h"
#include "tui/tui.h"
#include "tui/tui-data.h"
#include "tui/tui-wingeneral.h"
#include "gdb_curses.h"

/****************************
** GLOBAL DECLARATIONS
****************************/
struct tui_win_info *tui_win_list[MAX_MAJOR_WINDOWS];

/***************************
** Private data
****************************/
static enum tui_layout_type current_layout = UNDEFINED_LAYOUT;
static int term_height, term_width;
static struct tui_locator_window _locator;
static std::vector<tui_source_window_base *> source_windows;
static struct tui_win_info *win_with_focus = NULL;
static struct tui_layout_def layout_def = {
  SRC_WIN,			/* DISPLAY_MODE */
};

static int win_resized = FALSE;


/*********************************
** PUBLIC FUNCTIONS
**********************************/

int
tui_win_is_auxiliary (enum tui_win_type win_type)
{
  return (win_type > MAX_MAJOR_WINDOWS);
}

/******************************************
** ACCESSORS & MUTATORS FOR PRIVATE DATA
******************************************/

/* Answer a whether the terminal window has been resized or not.  */
int
tui_win_resized (void)
{
  return win_resized;
}


/* Set a whether the terminal window has been resized or not.  */
void
tui_set_win_resized_to (int resized)
{
  win_resized = resized;
}


/* Answer a pointer to the current layout definition.  */
struct tui_layout_def *
tui_layout_def (void)
{
  return &layout_def;
}


/* Answer the window with the logical focus.  */
struct tui_win_info *
tui_win_with_focus (void)
{
  return win_with_focus;
}


/* Set the window that has the logical focus.  */
void
tui_set_win_with_focus (struct tui_win_info *win_info)
{
  win_with_focus = win_info;
}


/* Accessor for the current source window.  Usually there is only one
   source window (either source or disassembly), but both can be
   displayed at the same time.  */
std::vector<tui_source_window_base *> &
tui_source_windows ()
{
  return source_windows;
}


/* Clear the list of source windows.  Usually there is only one source
   window (either source or disassembly), but both can be displayed at
   the same time.  */
void
tui_clear_source_windows ()
{
  source_windows.clear ();
}


/* Clear the pertinant detail in the source windows.  */
void
tui_clear_source_windows_detail ()
{
  for (tui_source_window_base *win : tui_source_windows ())
    win->clear_detail ();
}


/* Add a window to the list of source windows.  Usually there is only
   one source window (either source or disassembly), but both can be
   displayed at the same time.  */
void
tui_add_to_source_windows (struct tui_source_window_base *win_info)
{
  if (source_windows.size () < 2)
    source_windows.push_back (win_info);
}

/* See tui-data.h.  */

void
tui_source_window_base::clear_detail ()
{
  gdbarch = NULL;
  start_line_or_addr.loa = LOA_ADDRESS;
  start_line_or_addr.u.addr = 0;
  horizontal_offset = 0;
}

/* See tui-data.h.  */

void
tui_cmd_window::clear_detail ()
{
  wmove (handle, 0, 0);
}

/* See tui-data.h.  */

void
tui_data_window::clear_detail ()
{
  regs_content.clear ();
  regs_column_count = 1;
  display_regs = false;
}

/* Accessor for the locator win info.  Answers a pointer to the static
   locator win info struct.  */
struct tui_locator_window *
tui_locator_win_info_ptr (void)
{
  return &_locator;
}


/* Accessor for the term_height.  */
int
tui_term_height (void)
{
  return term_height;
}


/* Mutator for the term height.  */
void
tui_set_term_height_to (int h)
{
  term_height = h;
}


/* Accessor for the term_width.  */
int
tui_term_width (void)
{
  return term_width;
}


/* Mutator for the term_width.  */
void
tui_set_term_width_to (int w)
{
  term_width = w;
}


/* Accessor for the current layout.  */
enum tui_layout_type
tui_current_layout (void)
{
  return current_layout;
}


/* Mutator for the current layout.  */
void
tui_set_current_layout_to (enum tui_layout_type new_layout)
{
  current_layout = new_layout;
}


/*****************************
** OTHER PUBLIC FUNCTIONS
*****************************/


/* Answer the next window in the list, cycling back to the top if
   necessary.  */
struct tui_win_info *
tui_next_win (struct tui_win_info *cur_win)
{
  int type = cur_win->type;
  struct tui_win_info *next_win = NULL;

  if (cur_win->type == CMD_WIN)
    type = SRC_WIN;
  else
    type = cur_win->type + 1;
  while (type != cur_win->type && (next_win == NULL))
    {
      if (tui_win_list[type]
	  && tui_win_list[type]->is_visible)
	next_win = tui_win_list[type];
      else
	{
	  if (type == CMD_WIN)
	    type = SRC_WIN;
	  else
	    type++;
	}
    }

  return next_win;
}


/* Answer the prev window in the list, cycling back to the bottom if
   necessary.  */
struct tui_win_info *
tui_prev_win (struct tui_win_info *cur_win)
{
  int type = cur_win->type;
  struct tui_win_info *prev = NULL;

  if (cur_win->type == SRC_WIN)
    type = CMD_WIN;
  else
    type = cur_win->type - 1;
  while (type != cur_win->type && (prev == NULL))
    {
      if (tui_win_list[type]
	  && tui_win_list[type]->is_visible)
	prev = tui_win_list[type];
      else
	{
	  if (type == SRC_WIN)
	    type = CMD_WIN;
	  else
	    type--;
	}
    }

  return prev;
}


/* Answer the window represented by name.  */
struct tui_win_info *
tui_partial_win_by_name (const char *name)
{
  struct tui_win_info *win_info = NULL;

  if (name != NULL)
    {
      int i = 0;

      while (i < MAX_MAJOR_WINDOWS && win_info == NULL)
	{
          if (tui_win_list[i] != 0)
            {
              const char *cur_name = tui_win_list[i]->name ();

              if (strlen (name) <= strlen (cur_name)
		  && startswith (cur_name, name))
                win_info = tui_win_list[i];
            }
	  i++;
	}
    }

  return win_info;
}


void
tui_initialize_static_data ()
{
  tui_gen_win_info *win = tui_locator_win_info_ptr ();
  win->width =
    win->height =
    win->origin.x =
    win->origin.y =
    win->viewport_height =
    win->last_visible_line = 0;
  win->handle = NULL;
  win->content_in_use = FALSE;
  win->is_visible = false;
  win->title = 0;
}


tui_win_info::tui_win_info (enum tui_win_type type)
  : tui_gen_win_info (type)
{
}

tui_source_window_base::tui_source_window_base (enum tui_win_type type)
  : tui_win_info (type)
{
  gdb_assert (type == SRC_WIN || type == DISASSEM_WIN);
  start_line_or_addr.loa = LOA_ADDRESS;
  start_line_or_addr.u.addr = 0;
}

tui_gen_win_info::~tui_gen_win_info ()
{
  tui_delete_win (handle);
  xfree (title);
}

tui_source_window_base::~tui_source_window_base ()
{
  xfree (fullname);
  delete execution_info;
}  

/**********************************
** LOCAL STATIC FUNCTIONS        **
**********************************/


tui_data_item_window::~tui_data_item_window ()
{
  xfree (value);
  xfree (content);
}
