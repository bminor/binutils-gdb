/* TUI display registers in window.

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
#include "arch-utils.h"
#include "tui/tui.h"
#include "tui/tui-data.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "frame.h"
#include "regcache.h"
#include "inferior.h"
#include "target.h"
#include "tui/tui-layout.h"
#include "tui/tui-win.h"
#include "tui/tui-wingeneral.h"
#include "tui/tui-file.h"
#include "tui/tui-regs.h"
#include "tui/tui-io.h"
#include "reggroups.h"
#include "valprint.h"
#include "completer.h"

#include "gdb_curses.h"

static void tui_display_register (struct tui_data_item_window *data);

static void tui_show_register_group (tui_data_window *win_info,
				     struct reggroup *group,
				     struct frame_info *frame,
				     int refresh_values_only);

static void tui_get_register (struct frame_info *frame,
			      struct tui_data_item_window *data,
			      int regnum, bool *changedp);


/* See tui-regs.h.  */

tui_data_item_window::~tui_data_item_window ()
{
  xfree (value);
  xfree (content);
}

/* See tui-regs.h.  */

int
tui_data_window::last_regs_line_no () const
{
  int num_lines = (-1);

  if (!regs_content.empty ())
    {
      num_lines = regs_content.size () / regs_column_count;
      if (regs_content.size () % regs_column_count)
	num_lines++;
    }
  return num_lines;
}

/* See tui-regs.h.  */

int
tui_data_window::line_from_reg_element_no (int element_no) const
{
  if (element_no < regs_content.size ())
    {
      int i, line = (-1);

      i = 1;
      while (line == (-1))
	{
	  if (element_no < regs_column_count * i)
	    line = i - 1;
	  else
	    i++;
	}

      return line;
    }
  else
    return (-1);
}

/* See tui-regs.h.  */

int
tui_data_window::first_reg_element_no_inline (int line_no) const
{
  if (line_no * regs_column_count <= regs_content.size ())
    return ((line_no + 1) * regs_column_count) - regs_column_count;
  else
    return (-1);
}

/* A helper function to display the register window in the appropriate
   way.  */

static void
tui_reg_layout ()
{
  enum tui_layout_type cur_layout = tui_current_layout ();
  enum tui_layout_type new_layout;
  if (cur_layout == SRC_COMMAND || cur_layout == SRC_DATA_COMMAND)
    new_layout = SRC_DATA_COMMAND;
  else
    new_layout = DISASSEM_DATA_COMMAND;
  tui_set_layout (new_layout);
}

/* Show the registers of the given group in the data window
   and refresh the window.  */
void
tui_show_registers (struct reggroup *group)
{
  /* Make sure the curses mode is enabled.  */
  tui_enable ();

  /* Make sure the register window is visible.  If not, select an
     appropriate layout.  */
  if (TUI_DATA_WIN == NULL || !TUI_DATA_WIN->is_visible)
    tui_reg_layout ();

  if (group == 0)
    group = general_reggroup;

  /* Say that registers should be displayed, even if there is a
     problem.  */
  TUI_DATA_WIN->display_regs = true;

  if (target_has_registers && target_has_stack && target_has_memory)
    {
      tui_show_register_group (TUI_DATA_WIN, group, get_selected_frame (NULL),
			       group == TUI_DATA_WIN->current_group);

      /* Clear all notation of changed values.  */
      for (auto &&data_item_win : TUI_DATA_WIN->regs_content)
	{
	  if (data_item_win != nullptr)
	    data_item_win->highlight = false;
	}
      TUI_DATA_WIN->current_group = group;
      TUI_DATA_WIN->display_all_data ();
    }
  else
    {
      TUI_DATA_WIN->current_group = 0;
      TUI_DATA_WIN->erase_data_content (NO_REGS_STRING);
    }
}


/* Set the data window to display the registers of the register group
   using the given frame.  Values are refreshed only when
   refresh_values_only is TRUE.  */

static void
tui_show_register_group (tui_data_window *win_info,
			 struct reggroup *group,
                         struct frame_info *frame, 
			 int refresh_values_only)
{
  struct gdbarch *gdbarch = get_frame_arch (frame);
  int nr_regs;
  int regnum, pos;
  char title[80];

  /* Make a new title showing which group we display.  */
  snprintf (title, sizeof (title) - 1, "Register group: %s",
            reggroup_name (group));
  xfree (win_info->title);
  win_info->title = xstrdup (title);

  /* See how many registers must be displayed.  */
  nr_regs = 0;
  for (regnum = 0; regnum < gdbarch_num_cooked_regs (gdbarch); regnum++)
    {
      const char *name;

      /* Must be in the group.  */
      if (!gdbarch_register_reggroup_p (gdbarch, regnum, group))
	continue;

      /* If the register name is empty, it is undefined for this
	 processor, so don't display anything.  */
      name = gdbarch_register_name (gdbarch, regnum);
      if (name == 0 || *name == '\0')
	continue;

      nr_regs++;
    }

  if (!refresh_values_only)
    win_info->regs_content.clear ();

  if (nr_regs < win_info->regs_content.size ())
    win_info->regs_content.resize (nr_regs);
  else
    {
      for (int i = win_info->regs_content.size (); i < nr_regs; ++i)
	win_info->regs_content.emplace_back (new tui_data_item_window ());
    }

  /* Now set the register names and values.  */
  pos = 0;
  for (regnum = 0; regnum < gdbarch_num_cooked_regs (gdbarch); regnum++)
    {
      struct tui_data_item_window *data_item_win;
      const char *name;

      /* Must be in the group.  */
      if (!gdbarch_register_reggroup_p (gdbarch, regnum, group))
	continue;

      /* If the register name is empty, it is undefined for this
	 processor, so don't display anything.  */
      name = gdbarch_register_name (gdbarch, regnum);
      if (name == 0 || *name == '\0')
	continue;

      data_item_win = win_info->regs_content[pos].get ();
      if (data_item_win)
	{
	  if (!refresh_values_only)
	    {
	      data_item_win->item_no = regnum;
	      data_item_win->name = name;
	      data_item_win->highlight = false;
	    }
	  tui_get_register (frame, data_item_win, regnum, 0);
	}
      pos++;
    }
}

/* See tui-regs.h.  */

void
tui_data_window::display_registers_from (int start_element_no)
{
  if (!regs_content.empty ())
    {
      int j, item_win_width, cur_y;

      int max_len = 0;
      for (auto &&data_item_win : regs_content)
        {
          char *p;
          int len;

          len = 0;
          p = data_item_win->content;
          if (p != 0)
            while (*p)
              {
                if (*p++ == '\t')
                  len = 8 * ((len / 8) + 1);
                else
                  len++;
              }

          if (len > max_len)
            max_len = len;
        }
      item_win_width = max_len + 1;
      int i = start_element_no;

      regs_column_count = (width - 2) / item_win_width;
      if (regs_column_count == 0)
        regs_column_count = 1;
      item_win_width = (width - 2) / regs_column_count;

      /* Now create each data "sub" window, and write the display into
	 it.  */
      cur_y = 1;
      while (i < regs_content.size ()
	     && cur_y <= viewport_height)
	{
	  for (j = 0;
	       j < regs_column_count && i < regs_content.size ();
	       j++)
	    {
	      struct tui_data_item_window *data_item_win;

	      /* Create the window if necessary.  */
	      data_item_win = regs_content[i].get ();
              if (data_item_win->handle != NULL
                  && (data_item_win->height != 1
                      || data_item_win->width != item_win_width
                      || data_item_win->origin.x != (item_win_width * j) + 1
                      || data_item_win->origin.y != cur_y))
                {
                  tui_delete_win (data_item_win->handle);
                  data_item_win->handle = 0;
                }
                  
	      if (data_item_win->handle == NULL)
		{
		  data_item_win->height = 1;
		  data_item_win->width = item_win_width;
		  data_item_win->origin.x = (item_win_width * j) + 1;
		  data_item_win->origin.y = cur_y;
		  tui_make_window (data_item_win, DONT_BOX_WINDOW);
                  scrollok (data_item_win->handle, FALSE);
		}
              touchwin (data_item_win->handle);

	      /* Get the printable representation of the register
                 and display it.  */
              tui_display_register (data_item_win);
	      i++;		/* Next register.  */
	    }
	  cur_y++;		/* Next row.  */
	}
    }
}

/* See tui-regs.h.  */

void
tui_data_window::display_reg_element_at_line (int start_element_no,
					      int start_line_no)
{
  if (!regs_content.empty ())
    {
      int element_no = start_element_no;

      if (start_element_no != 0 && start_line_no != 0)
	{
	  int last_line_no, first_line_on_last_page;

	  last_line_no = last_regs_line_no ();
	  first_line_on_last_page = last_line_no - (height - 2);
	  if (first_line_on_last_page < 0)
	    first_line_on_last_page = 0;

	  /* If the element_no causes us to scroll past the end of the
	     registers, adjust what element to really start the
	     display at.  */
	  if (start_line_no > first_line_on_last_page)
	    element_no = first_reg_element_no_inline (first_line_on_last_page);
	}
      display_registers_from (element_no);
    }
}

/* See tui-regs.h.  */

int
tui_data_window::display_registers_from_line (int line_no)
{
  tui_check_and_display_highlight_if_needed (this);
  if (!regs_content.empty ())
    {
      int element_no;

      if (line_no < 0)
	line_no = 0;
      else
	{
	  /* Make sure that we don't display off the end of the
	     registers.  */
	  if (line_no >= last_regs_line_no ())
	    {
	      line_no = line_from_reg_element_no (regs_content.size () - 1);
	      if (line_no < 0)
		line_no = 0;
	    }
	}

      element_no = first_reg_element_no_inline (line_no);
      if (element_no < regs_content.size ())
	display_reg_element_at_line (element_no, line_no);
      else
	line_no = (-1);

      return line_no;
    }

  return (-1);			/* Nothing was displayed.  */
}


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

/* See tui-regs.h.  */

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
tui_data_window::erase_data_content (const char *prompt)
{
  werase (handle);
  tui_check_and_display_highlight_if_needed (this);
  if (prompt != NULL)
    {
      int half_width = (width - 2) / 2;
      int x_pos;

      if (strlen (prompt) >= half_width)
	x_pos = 1;
      else
	x_pos = half_width - strlen (prompt);
      mvwaddstr (handle, (height / 2), x_pos, (char *) prompt);
    }
  wrefresh (handle);
}

/* See tui-regs.h.  */

void
tui_data_window::display_all_data ()
{
  if (regs_content.empty ())
    erase_data_content (NO_DATA_STRING);
  else
    {
      erase_data_content (NULL);
      delete_data_content_windows ();
      tui_check_and_display_highlight_if_needed (this);
      display_registers_from (0);
    }
}


/* Function to redisplay the contents of the data window.  */
void
tui_data_window::refresh_all ()
{
  erase_data_content (NULL);
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
	      erase_data_content (NULL);
	      display_registers_from_line (first_line);
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
      erase_data_content (NULL);
      delete_data_content_windows ();
      display_registers_from_line (first_line);
    }
}

/* See tui-regs.h.  */

void
tui_data_window::clear_detail ()
{
  regs_content.clear ();
  regs_column_count = 1;
  display_regs = false;
}

/* See tui-regs.h.  */

void
tui_data_window::set_new_height (int height)
{
  /* Delete all data item windows.  */
  for (auto &&win : regs_content)
    {
      tui_delete_win (win->handle);
      win->handle = NULL;
    }
}

/* See tui-regs.h.  */

void
tui_data_window::do_make_visible_with_new_height ()
{
  display_all_data ();
}

/* See tui-regs.h.  */

void
tui_data_window::refresh_window ()
{
  tui_gen_win_info::refresh_window ();
  for (auto &&win : regs_content)
    {
      if (win != NULL)
	win->refresh_window ();
    }
}

/* This function check all displayed registers for changes in values,
   given a particular frame.  If the values have changed, they are
   updated with the new value and highlighted.  */
void
tui_check_register_values (struct frame_info *frame)
{
  if (TUI_DATA_WIN != NULL
      && TUI_DATA_WIN->is_visible)
    {
      if (TUI_DATA_WIN->regs_content.empty ()
	  && TUI_DATA_WIN->display_regs)
	tui_show_registers (TUI_DATA_WIN->current_group);
      else
	{
	  for (auto &&data_item_win_ptr : TUI_DATA_WIN->regs_content)
	    {
	      int was_hilighted;

	      was_hilighted = data_item_win_ptr->highlight;

              tui_get_register (frame, data_item_win_ptr.get (),
                                data_item_win_ptr->item_no,
				&data_item_win_ptr->highlight);

	      if (data_item_win_ptr->highlight || was_hilighted)
		tui_display_register (data_item_win_ptr.get ());
	    }
	}
    }
}

/* Display a register in a window.  If hilite is TRUE, then the value
   will be displayed in reverse video.  */
static void
tui_display_register (struct tui_data_item_window *data)
{
  if (data->handle != NULL)
    {
      int i;

      if (data->highlight)
	/* We ignore the return value, casting it to void in order to avoid
	   a compiler warning.  The warning itself was introduced by a patch
	   to ncurses 5.7 dated 2009-08-29, changing this macro to expand
	   to code that causes the compiler to generate an unused-value
	   warning.  */
	(void) wstandout (data->handle);
      
      wmove (data->handle, 0, 0);
      for (i = 1; i < data->width; i++)
        waddch (data->handle, ' ');
      wmove (data->handle, 0, 0);
      if (data->content)
        waddstr (data->handle, data->content);

      if (data->highlight)
	/* We ignore the return value, casting it to void in order to avoid
	   a compiler warning.  The warning itself was introduced by a patch
	   to ncurses 5.7 dated 2009-08-29, changing this macro to expand
	   to code that causes the compiler to generate an unused-value
	   warning.  */
	(void) wstandend (data->handle);
      data->refresh_window ();
    }
}

/* Helper for "tui reg next", wraps a call to REGGROUP_NEXT, but adds wrap
   around behaviour.  Returns the next register group, or NULL if the
   register window is not currently being displayed.  */

static struct reggroup *
tui_reg_next (struct reggroup *current_group, struct gdbarch *gdbarch)
{
  struct reggroup *group = NULL;

  if (current_group != NULL)
    {
      group = reggroup_next (gdbarch, current_group);
      if (group == NULL)
        group = reggroup_next (gdbarch, NULL);
    }
  return group;
}

/* Helper for "tui reg prev", wraps a call to REGGROUP_PREV, but adds wrap
   around behaviour.  Returns the previous register group, or NULL if the
   register window is not currently being displayed.  */

static struct reggroup *
tui_reg_prev (struct reggroup *current_group, struct gdbarch *gdbarch)
{
  struct reggroup *group = NULL;

  if (current_group != NULL)
    {
      group = reggroup_prev (gdbarch, current_group);
      if (group == NULL)
	group = reggroup_prev (gdbarch, NULL);
    }
  return group;
}

/* Implement the 'tui reg' command.  Changes the register group displayed
   in the tui register window.  Displays the tui register window if it is
   not already on display.  */

static void
tui_reg_command (const char *args, int from_tty)
{
  struct gdbarch *gdbarch = get_current_arch ();

  if (args != NULL)
    {
      struct reggroup *group, *match = NULL;
      size_t len = strlen (args);

      /* Make sure the curses mode is enabled.  */
      tui_enable ();

      /* Make sure the register window is visible.  If not, select an
	 appropriate layout.  We need to do this before trying to run the
	 'next' or 'prev' commands.  */
      if (TUI_DATA_WIN == NULL || !TUI_DATA_WIN->is_visible)
	tui_reg_layout ();

      struct reggroup *current_group = NULL;
      if (TUI_DATA_WIN != NULL)
	current_group = TUI_DATA_WIN->current_group;
      if (strncmp (args, "next", len) == 0)
	match = tui_reg_next (current_group, gdbarch);
      else if (strncmp (args, "prev", len) == 0)
	match = tui_reg_prev (current_group, gdbarch);

      /* This loop matches on the initial part of a register group
	 name.  If this initial part in ARGS matches only one register
	 group then the switch is made.  */
      for (group = reggroup_next (gdbarch, NULL);
	   group != NULL;
	   group = reggroup_next (gdbarch, group))
	{
	  if (strncmp (reggroup_name (group), args, len) == 0)
	    {
	      if (match != NULL)
		error (_("ambiguous register group name '%s'"), args);
	      match = group;
	    }
	}

      if (match == NULL)
	error (_("unknown register group '%s'"), args);

      tui_show_registers (match);
    }
  else
    {
      struct reggroup *group;
      int first;

      printf_unfiltered (_("\"tui reg\" must be followed by the name of "
			   "either a register group,\nor one of 'next' "
			   "or 'prev'.  Known register groups are:\n"));

      for (first = 1, group = reggroup_next (gdbarch, NULL);
	   group != NULL;
	   first = 0, group = reggroup_next (gdbarch, group))
	{
	  if (!first)
	    printf_unfiltered (", ");
	  printf_unfiltered ("%s", reggroup_name (group));
	}

      printf_unfiltered ("\n");
    }
}

/* Complete names of register groups, and add the special "prev" and "next"
   names.  */

static void
tui_reggroup_completer (struct cmd_list_element *ignore,
			completion_tracker &tracker,
			const char *text, const char *word)
{
  static const char *extra[] = { "next", "prev", NULL };
  size_t len = strlen (word);
  const char **tmp;

  reggroup_completer (ignore, tracker, text, word);

  /* XXXX use complete_on_enum instead?  */
  for (tmp = extra; *tmp != NULL; ++tmp)
    {
      if (strncmp (word, *tmp, len) == 0)
	tracker.add_completion (make_unique_xstrdup (*tmp));
    }
}

/* Get the register from the frame and return a printable
   representation of it.  */

static char *
tui_register_format (struct frame_info *frame, int regnum)
{
  struct gdbarch *gdbarch = get_frame_arch (frame);

  string_file stream;

  scoped_restore save_pagination
    = make_scoped_restore (&pagination_enabled, 0);
  scoped_restore save_stdout
    = make_scoped_restore (&gdb_stdout, &stream);

  gdbarch_print_registers_info (gdbarch, &stream, frame, regnum, 1);

  /* Remove the possible \n.  */
  std::string &str = stream.string ();
  if (!str.empty () && str.back () == '\n')
    str.resize (str.size () - 1);

  /* Expand tabs into spaces, since ncurses on MS-Windows doesn't.  */
  return tui_expand_tabs (str.c_str (), 0);
}

/* Get the register value from the given frame and format it for the
   display.  When changep is set, check if the new register value has
   changed with respect to the previous call.  */
static void
tui_get_register (struct frame_info *frame,
                  struct tui_data_item_window *data, 
		  int regnum, bool *changedp)
{
  if (changedp)
    *changedp = false;
  if (target_has_registers)
    {
      char *prev_content = data->content;

      data->content = tui_register_format (frame, regnum);

      if (changedp != NULL
	  && strcmp (prev_content, data->content) != 0)
	*changedp = true;

      xfree (prev_content);
    }
}

void
_initialize_tui_regs (void)
{
  struct cmd_list_element **tuicmd, *cmd;

  tuicmd = tui_get_cmd_list ();

  cmd = add_cmd ("reg", class_tui, tui_reg_command, _("\
TUI command to control the register window."), tuicmd);
  set_cmd_completer (cmd, tui_reggroup_completer);
}
