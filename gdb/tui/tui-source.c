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
#include <math.h>
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
#include "tui/tui-win.h"
#include "tui/tui-winsource.h"
#include "tui/tui-source.h"
#include "gdb_curses.h"

/* Function to display source in the source window.  */
enum tui_status
tui_source_window::set_contents (struct gdbarch *arch,
				 struct symtab *s, 
				 struct tui_line_or_address line_or_addr)
{
  gdb_assert (line_or_addr.loa == LOA_LINE);
  int line_no = line_or_addr.u.line_no;

  enum tui_status ret = TUI_FAILURE;

  if (s != NULL)
    {
      int line_width, nlines;

      ret = TUI_SUCCESS;
      line_width = width - TUI_EXECINFO_SIZE - 1;
      /* Take hilite (window border) into account, when
	 calculating the number of lines.  */
      nlines = (line_no + (height - 2)) - line_no;

      std::string srclines;
      const std::vector<off_t> *offsets;
      if (!g_source_cache.get_source_lines (s, line_no, line_no + nlines,
					    &srclines)
	  || !g_source_cache.get_line_charpos (s, &offsets))
	ret = TUI_FAILURE;
      else
	{
	  int cur_line_no, cur_line;
	  struct tui_locator_window *locator
	    = tui_locator_win_info_ptr ();
	  const char *s_filename = symtab_to_filename_for_display (s);

	  title = s_filename;

	  m_fullname = make_unique_xstrdup (symtab_to_fullname (s));

	  cur_line = 0;
	  gdbarch = get_objfile_arch (SYMTAB_OBJFILE (s));
	  start_line_or_addr.loa = LOA_LINE;
	  cur_line_no = start_line_or_addr.u.line_no = line_no;

	  int digits = 0;
	  if (compact_source)
	    {
	      double l = log10 (offsets->size ());
	      digits = 1 + (int) l;
	    }

	  const char *iter = srclines.c_str ();
	  content.resize (nlines);
	  while (cur_line < nlines)
	    {
	      struct tui_source_element *element
		= &content[cur_line];

	      std::string text;
	      if (*iter != '\0')
		text = tui_copy_source_line (&iter, cur_line_no,
					     horizontal_offset,
					     line_width, digits);

	      /* Set whether element is the execution point
		 and whether there is a break point on it.  */
	      element->line_or_addr.loa = LOA_LINE;
	      element->line_or_addr.u.line_no = cur_line_no;
	      element->is_exec_point
		= (filename_cmp (locator->full_name.c_str (),
				 symtab_to_fullname (s)) == 0
		   && cur_line_no == locator->line_no);

	      content[cur_line].line = std::move (text);

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
tui_source_window::show_symtab_source (struct gdbarch *gdbarch,
				       struct symtab *s,
				       struct tui_line_or_address line)
{
  horizontal_offset = 0;
  update_source_window_as_is (gdbarch, s, line);
}


/* Answer whether the source is currently displayed in the source
   window.  */
bool
tui_source_window::showing_source_p (const char *fullname) const
{
  return (!content.empty ()
	  && (filename_cmp (tui_locator_win_info_ptr ()->full_name.c_str (),
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
      const std::vector<off_t> *offsets;
      if (g_source_cache.get_line_charpos (s, &offsets)
	  && l.u.line_no > offsets->size ())
	/* line = s->nlines - win_info->content_size + 1; */
	/* elz: fix for dts 23398.  */
	l.u.line_no = content[0].line_or_addr.u.line_no;
      if (l.u.line_no <= 0)
	l.u.line_no = 1;

      print_source_lines (s, l.u.line_no, l.u.line_no + 1, 0);
    }
}

bool
tui_source_window::location_matches_p (struct bp_location *loc, int line_no)
{
  return (content[line_no].line_or_addr.loa == LOA_LINE
	  && content[line_no].line_or_addr.u.line_no == loc->line_number
	  && loc->symtab != NULL
	  && filename_cmp (m_fullname.get (),
			   symtab_to_fullname (loc->symtab)) == 0);
}

/* See tui-source.h.  */

bool
tui_source_window::line_is_displayed (int line) const
{
  bool is_displayed = false;
  int threshold = SCROLL_THRESHOLD;
  int i = 0;
  while (i < content.size () - threshold && !is_displayed)
    {
      is_displayed
	= (content[i].line_or_addr.loa == LOA_LINE
	   && content[i].line_or_addr.u.line_no == line);
      i++;
    }

  return is_displayed;
}

void
tui_source_window::maybe_update (struct frame_info *fi, symtab_and_line sal,
				 int line_no, CORE_ADDR addr)
{
  int start_line = (line_no - (viewport_height / 2)) + 1;
  if (start_line <= 0)
    start_line = 1;

  bool source_already_displayed = (sal.symtab != 0
				   && showing_source_p (m_fullname.get ()));

  struct tui_line_or_address l;

  l.loa = LOA_LINE;
  l.u.line_no = start_line;
  if (!(source_already_displayed
	&& line_is_displayed (line_no)))
    update_source_window (get_frame_arch (fi), sal.symtab, l);
  else
    {
      l.u.line_no = line_no;
      set_is_exec_point_at (l);
    }
}
