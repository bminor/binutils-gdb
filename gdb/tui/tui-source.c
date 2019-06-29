/* TUI display source window.

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
#include <ctype.h>
#include "symtab.h"
#include "frame.h"
#include "breakpoint.h"
#include "source.h"
#include "objfiles.h"
#include "filenames.h"
#include "source-cache.h"

#include "tui/tui.h"
#include "tui/tui-data.h"
#include "tui/tui-io.h"
#include "tui/tui-stack.h"
#include "tui/tui-winsource.h"
#include "tui/tui-source.h"
#include "gdb_curses.h"

/* A helper function for tui_set_source_content that extracts some
   source text from PTR.  LINE_NO is the line number; FIRST_COL is the
   first column to extract, and LINE_WIDTH is the number of characters
   to display.  Returns a string holding the desired text.  */

static std::string
copy_source_line (const char **ptr, int line_no, int first_col,
		  int line_width)
{
  const char *lineptr = *ptr;

  /* Init the line with the line number.  */
  std::string result = string_printf ("%-6d", line_no);
  int len = result.size ();
  len = len - ((len / tui_tab_width) * tui_tab_width);
  result.append (len, ' ');

  int column = 0;
  char c;
  do
    {
      int skip_bytes;

      c = *lineptr;
      if (c == '\033' && skip_ansi_escape (lineptr, &skip_bytes))
	{
	  /* We always have to preserve escapes.  */
	  result.append (lineptr, lineptr + skip_bytes);
	  lineptr += skip_bytes;
	  continue;
	}

      ++lineptr;
      ++column;

      auto process_tab = [&] ()
	{
	  int max_tab_len = tui_tab_width;

	  --column;
	  for (int j = column % max_tab_len;
	       j < max_tab_len && column < first_col + line_width;
	       column++, j++)
	    if (column >= first_col)
	      result.push_back (' ');
	};

      /* We have to process all the text in order to pick up all the
	 escapes.  */
      if (column <= first_col || column > first_col + line_width)
	{
	  if (c == '\t')
	    process_tab ();
	  continue;
	}

      if (c == '\n' || c == '\r' || c == '\0')
	{
	  /* Nothing.  */
	}
      else if (c < 040 && c != '\t')
	{
	  result.push_back ('^');
	  result.push_back (c + 0100);
	}
      else if (c == 0177)
	{
	  result.push_back ('^');
	  result.push_back ('?');
	}
      else if (c == '\t')
	process_tab ();
      else
	result.push_back (c);
    }
  while (c != '\0' && c != '\n' && c != '\r');

  if (c == '\r' && *lineptr == '\n')
    ++lineptr;
  *ptr = lineptr;

  return result;
}

/* Function to display source in the source window.  */
enum tui_status
tui_set_source_content (tui_source_window_base *win_info,
			struct symtab *s, 
			int line_no,
			int noerror)
{
  enum tui_status ret = TUI_FAILURE;

  if (s != NULL)
    {
      int line_width, nlines;

      ret = TUI_SUCCESS;
      tui_alloc_source_buffer (win_info);
      line_width = win_info->width - 1;
      /* Take hilite (window border) into account, when
	 calculating the number of lines.  */
      nlines = (line_no + (win_info->height - 2)) - line_no;

      std::string srclines;
      if (!g_source_cache.get_source_lines (s, line_no, line_no + nlines,
					    &srclines))
	{
	  if (!noerror)
	    {
	      const char *filename = symtab_to_filename_for_display (s);
	      char *name = (char *) alloca (strlen (filename) + 100);

	      sprintf (name, "%s:%d", filename, line_no);
	      print_sys_errmsg (name, errno);
	    }
	  ret = TUI_FAILURE;
	}
      else
	{
	  int cur_line_no, cur_line;
	  struct tui_locator_window *locator
	    = tui_locator_win_info_ptr ();
	  const char *s_filename = symtab_to_filename_for_display (s);

	  xfree (win_info->title);
	  win_info->title = xstrdup (s_filename);

	  xfree (win_info->fullname);
	  win_info->fullname = xstrdup (symtab_to_fullname (s));

	  cur_line = 0;
	  win_info->gdbarch = get_objfile_arch (SYMTAB_OBJFILE (s));
	  win_info->start_line_or_addr.loa = LOA_LINE;
	  cur_line_no = win_info->start_line_or_addr.u.line_no = line_no;

	  const char *iter = srclines.c_str ();
	  win_info->content.resize (nlines);
	  while (cur_line < nlines)
	    {
	      struct tui_source_element *element
		= &win_info->content[cur_line];

	      std::string text;
	      if (*iter != '\0')
		text = copy_source_line (&iter, cur_line_no,
					 win_info->horizontal_offset,
					 line_width);

	      /* Set whether element is the execution point
		 and whether there is a break point on it.  */
	      element->line_or_addr.loa = LOA_LINE;
	      element->line_or_addr.u.line_no = cur_line_no;
	      element->is_exec_point
		= (filename_cmp (locator->full_name,
				 symtab_to_fullname (s)) == 0
		   && cur_line_no == locator->line_no);

	      xfree (win_info->content[cur_line].line);
	      win_info->content[cur_line].line
		= xstrdup (text.c_str ());

	      cur_line++;
	      cur_line_no++;
	    }
	  ret = TUI_SUCCESS;
	}
    }
  return ret;
}


/* Function to display source in the source window.  This function
   initializes the horizontal scroll to 0.  */
void
tui_show_symtab_source (tui_source_window_base *win_info,
			struct gdbarch *gdbarch, struct symtab *s,
			struct tui_line_or_address line, 
			int noerror)
{
  win_info->horizontal_offset = 0;
  tui_update_source_window_as_is (win_info, gdbarch, s, line, noerror);
}


/* Answer whether the source is currently displayed in the source
   window.  */
bool
tui_source_window::showing_source_p (const char *fullname) const
{
  return (!content.empty ()
	  && (filename_cmp (tui_locator_win_info_ptr ()->full_name,
			    fullname) == 0));
}


/* Scroll the source forward or backward vertically.  */
void
tui_source_window::do_scroll_vertical (int num_to_scroll)
{
  if (!content.empty ())
    {
      struct tui_line_or_address l;
      struct symtab *s;
      struct symtab_and_line cursal = get_current_source_symtab_and_line ();

      if (cursal.symtab == NULL)
	s = find_pc_line_symtab (get_frame_pc (get_selected_frame (NULL)));
      else
	s = cursal.symtab;

      l.loa = LOA_LINE;
      l.u.line_no = content[0].line_or_addr.u.line_no
	+ num_to_scroll;
      if (l.u.line_no > s->nlines)
	/* line = s->nlines - win_info->content_size + 1; */
	/* elz: fix for dts 23398.  */
	l.u.line_no = content[0].line_or_addr.u.line_no;
      if (l.u.line_no <= 0)
	l.u.line_no = 1;

      print_source_lines (s, l.u.line_no, l.u.line_no + 1, 0);
    }
}

tui_source_window::tui_source_window ()
  : tui_source_window_base (SRC_WIN)
{
  gdb::observers::source_styling_changed.attach
    (std::bind (&tui_source_window::style_changed, this),
     m_observable);
}

tui_source_window::~tui_source_window ()
{
  gdb::observers::source_styling_changed.detach (m_observable);
}

void
tui_source_window::style_changed ()
{
  if (tui_active && is_visible)
    refill ();
}

bool
tui_source_window::location_matches_p (struct bp_location *loc, int line_no)
{
  return (content[line_no].line_or_addr.loa == LOA_LINE
	  && content[line_no].line_or_addr.u.line_no == loc->line_number
	  && loc->symtab != NULL
	  && filename_cmp (fullname,
			   symtab_to_fullname (loc->symtab)) == 0);
}
