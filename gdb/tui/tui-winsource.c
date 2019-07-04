/* TUI display source/assembly window.

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
#include "value.h"
#include "source.h"
#include "objfiles.h"
#include "filenames.h"

#include "tui/tui.h"
#include "tui/tui-data.h"
#include "tui/tui-io.h"
#include "tui/tui-stack.h"
#include "tui/tui-win.h"
#include "tui/tui-wingeneral.h"
#include "tui/tui-winsource.h"
#include "tui/tui-source.h"
#include "tui/tui-disasm.h"
#include "gdb_curses.h"

/* Function to display the "main" routine.  */
void
tui_display_main ()
{
  if (!tui_source_windows ().empty ())
    {
      struct gdbarch *gdbarch;
      CORE_ADDR addr;

      tui_get_begin_asm_address (&gdbarch, &addr);
      if (addr != (CORE_ADDR) 0)
	{
	  struct symtab *s;

	  tui_update_source_windows_with_addr (gdbarch, addr);
	  s = find_pc_line_symtab (addr);
          if (s != NULL)
             tui_update_locator_fullname (symtab_to_fullname (s));
          else
             tui_update_locator_fullname ("??");
	}
    }
}



/* Function to display source in the source window.  This function
   initializes the horizontal scroll to 0.  */
void
tui_update_source_window (struct tui_source_window_base *win_info,
			  struct gdbarch *gdbarch,
			  struct symtab *s,
			  struct tui_line_or_address line_or_addr,
			  int noerror)
{
  win_info->horizontal_offset = 0;
  tui_update_source_window_as_is (win_info, gdbarch, s, line_or_addr, noerror);
}


/* Function to display source in the source/asm window.  This function
   shows the source as specified by the horizontal offset.  */
void
tui_update_source_window_as_is (struct tui_source_window_base *win_info, 
				struct gdbarch *gdbarch,
				struct symtab *s,
				struct tui_line_or_address line_or_addr, 
				int noerror)
{
  enum tui_status ret;

  if (win_info->type == SRC_WIN)
    ret = tui_set_source_content (win_info, s, line_or_addr.u.line_no,
				  noerror);
  else
    ret = tui_set_disassem_content (win_info, gdbarch, line_or_addr.u.addr);

  if (ret == TUI_FAILURE)
    {
      tui_clear_source_content (win_info);
      tui_clear_exec_info_content (win_info);
    }
  else
    {
      tui_update_breakpoint_info (win_info, nullptr, false);
      tui_show_source_content (win_info);
      tui_update_exec_info (win_info);
      if (win_info->type == SRC_WIN)
	{
	  symtab_and_line sal;

	  sal.line = line_or_addr.u.line_no +
	    (win_info->content.size () - 2);
	  sal.symtab = s;
	  sal.pspace = SYMTAB_PSPACE (s);
	  set_current_source_symtab_and_line (sal);
	  /* If the focus was in the asm win, put it in the src win if
	     we don't have a split layout.  */
	  if (tui_win_with_focus () == TUI_DISASM_WIN
	      && tui_current_layout () != SRC_DISASSEM_COMMAND)
	    tui_set_win_focus_to (win_info);
	}
    }
}


/* Function to ensure that the source and/or disassemly windows
   reflect the input address.  */
void
tui_update_source_windows_with_addr (struct gdbarch *gdbarch, CORE_ADDR addr)
{
  if (addr != 0)
    {
      struct symtab_and_line sal;
      struct tui_line_or_address l;
      
      switch (tui_current_layout ())
	{
	case DISASSEM_COMMAND:
	case DISASSEM_DATA_COMMAND:
	  tui_show_disassem (gdbarch, addr);
	  break;
	case SRC_DISASSEM_COMMAND:
	  tui_show_disassem_and_update_source (gdbarch, addr);
	  break;
	default:
	  sal = find_pc_line (addr, 0);
	  l.loa = LOA_LINE;
	  l.u.line_no = sal.line;
	  tui_show_symtab_source (TUI_SRC_WIN, gdbarch, sal.symtab, l, FALSE);
	  break;
	}
    }
  else
    {
      for (struct tui_source_window_base *win_info : tui_source_windows ())
	{
	  tui_clear_source_content (win_info);
	  tui_clear_exec_info_content (win_info);
	}
    }
}

/* Function to ensure that the source and/or disassemly windows
   reflect the input address.  */
void
tui_update_source_windows_with_line (struct symtab *s, int line)
{
  struct gdbarch *gdbarch;
  CORE_ADDR pc;
  struct tui_line_or_address l;

  if (!s)
    return;

  gdbarch = get_objfile_arch (SYMTAB_OBJFILE (s));

  switch (tui_current_layout ())
    {
    case DISASSEM_COMMAND:
    case DISASSEM_DATA_COMMAND:
      find_line_pc (s, line, &pc);
      tui_update_source_windows_with_addr (gdbarch, pc);
      break;
    default:
      l.loa = LOA_LINE;
      l.u.line_no = line;
      tui_show_symtab_source (TUI_SRC_WIN, gdbarch, s, l, FALSE);
      if (tui_current_layout () == SRC_DISASSEM_COMMAND)
	{
	  find_line_pc (s, line, &pc);
	  tui_show_disassem (gdbarch, pc);
	}
      break;
    }
}

void
tui_clear_source_content (struct tui_source_window_base *win_info)
{
  if (win_info != NULL)
    {
      int i;

      tui_erase_source_content (win_info);
      for (i = 0; i < win_info->content.size (); i++)
	{
	  struct tui_source_element *element = &win_info->content[i];

	  element->break_mode = 0;
	  element->is_exec_point = false;
	}
    }
}


void
tui_erase_source_content (struct tui_source_window_base *win_info)
{
  int x_pos;
  int half_width = (win_info->width - 2) / 2;

  if (win_info->handle != NULL)
    {
      werase (win_info->handle);
      tui_check_and_display_highlight_if_needed (win_info);

      const char *no_src_str;

      if (win_info->type == SRC_WIN)
	no_src_str = NO_SRC_STRING;
      else
	no_src_str = NO_DISASSEM_STRING;
      if (strlen (no_src_str) >= half_width)
	x_pos = 1;
      else
	x_pos = half_width - strlen (no_src_str);
      mvwaddstr (win_info->handle,
		 (win_info->height / 2),
		 x_pos,
		 (char *) no_src_str);

      win_info->content.clear ();
      win_info->refresh_window ();
    }
}


/* Redraw the complete line of a source or disassembly window.  */
static void
tui_show_source_line (struct tui_source_window_base *win_info, int lineno)
{
  struct tui_source_element *line;
  int x;

  line = &win_info->content[lineno - 1];
  if (line->is_exec_point)
    tui_set_reverse_mode (win_info->handle, true);

  wmove (win_info->handle, lineno, 1);
  tui_puts (line->line,
	    win_info->handle);
  if (line->is_exec_point)
    tui_set_reverse_mode (win_info->handle, false);

  /* Clear to end of line but stop before the border.  */
  x = getcurx (win_info->handle);
  while (x + 1 < win_info->width)
    {
      waddch (win_info->handle, ' ');
      x = getcurx (win_info->handle);
    }
}

void
tui_show_source_content (struct tui_source_window_base *win_info)
{
  if (!win_info->content.empty ())
    {
      int lineno;

      for (lineno = 1; lineno <= win_info->content.size (); lineno++)
        tui_show_source_line (win_info, lineno);
    }
  else
    tui_erase_source_content (win_info);

  tui_check_and_display_highlight_if_needed (win_info);
  win_info->refresh_window ();
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

tui_source_window_base::tui_source_window_base (enum tui_win_type type)
  : tui_win_info (type),
    execution_info (new tui_exec_info_window ())
{
  gdb_assert (type == SRC_WIN || type == DISASSEM_WIN);
  start_line_or_addr.loa = LOA_ADDRESS;
  start_line_or_addr.u.addr = 0;
}


tui_source_window_base::~tui_source_window_base ()
{
  xfree (fullname);
  delete execution_info;
}  

void
tui_source_window_base::reset (int height, int width,
			       int origin_x, int origin_y)
{
  tui_gen_win_info::reset (height, width - 3,
			   origin_x + 3, origin_y);
  execution_info->reset (height, 3, origin_x, origin_y);
}

/* See tui-data.h.  */

void
tui_source_window_base::refresh_all ()
{
  tui_show_source_content (this);
  tui_check_and_display_highlight_if_needed (this);
  tui_erase_exec_info_content (this);
  tui_update_exec_info (this);
}

/* See tui-data.h.  */

void
tui_source_window_base::update_tab_width ()
{
  /* We don't really change the height of any windows, but
     calling these 2 functions causes a complete regeneration
     and redisplay of the window's contents, which will take
     the new tab width into account.  */
  make_invisible_and_set_new_height (height);
  make_visible_with_new_height ();
}

/* See tui-data.h.  */

void
tui_source_window_base::set_new_height (int height)
{
  execution_info->make_visible (false);
  execution_info->height = height;
  execution_info->origin.y = origin.y;
  if (height > 1)
    execution_info->viewport_height = height - 1;
  else
    execution_info->viewport_height = height;
  execution_info->viewport_height--;

  if (m_has_locator)
    {
      tui_locator_window *gen_win_info = tui_locator_win_info_ptr ();
      gen_win_info->make_visible (false);
      gen_win_info->origin.y = origin.y + height;
    }
}

/* See tui-data.h.  */

void
tui_source_window_base::do_make_visible_with_new_height ()
{
  execution_info->make_visible (true);
  if (!content.empty ())
    {
      struct tui_line_or_address line_or_addr;
      struct symtab_and_line cursal
	= get_current_source_symtab_and_line ();

      line_or_addr = start_line_or_addr;
      tui_update_source_window (this, gdbarch,
				cursal.symtab, line_or_addr, TRUE);
    }
  else if (deprecated_safe_get_selected_frame () != NULL)
    {
      struct tui_line_or_address line;
      struct symtab_and_line cursal
	= get_current_source_symtab_and_line ();
      struct frame_info *frame = deprecated_safe_get_selected_frame ();
      struct gdbarch *gdbarch = get_frame_arch (frame);

      struct symtab *s = find_pc_line_symtab (get_frame_pc (frame));
      if (type == SRC_WIN)
	{
	  line.loa = LOA_LINE;
	  line.u.line_no = cursal.line;
	}
      else
	{
	  line.loa = LOA_ADDRESS;
	  find_line_pc (s, cursal.line, &line.u.addr);
	}
      tui_update_source_window (this, gdbarch, s, line, TRUE);
    }
  if (m_has_locator)
    {
      tui_locator_win_info_ptr ()->make_visible (true);
      tui_show_locator_content ();
    }
}

/* See tui-data.h.  */

void
tui_source_window_base::make_visible (bool visible)
{
  execution_info->make_visible (visible);
  tui_win_info::make_visible (visible);
}

/* See tui-data.h.  */

void
tui_source_window_base::refresh_window ()
{
  execution_info->refresh_window ();
  tui_win_info::refresh_window ();
}

/* See tui-data.h.  */

void
tui_source_window_base::refill ()
{
  symtab *s = nullptr;

  if (type == SRC_WIN)
    {
      symtab_and_line cursal = get_current_source_symtab_and_line ();
      s = (cursal.symtab == NULL
	   ? find_pc_line_symtab (get_frame_pc (get_selected_frame (NULL)))
	   : cursal.symtab);
    }

  tui_update_source_window_as_is (this, gdbarch, s,
				  content[0].line_or_addr,
				  FALSE);
}

/* Scroll the source forward or backward horizontally.  */

void
tui_source_window_base::do_scroll_horizontal (int num_to_scroll)
{
  if (!content.empty ())
    {
      int offset = horizontal_offset + num_to_scroll;
      if (offset < 0)
	offset = 0;
      horizontal_offset = offset;
      refill ();
    }
}


/* Set or clear the is_exec_point flag in the line whose line is
   line_no.  */

void
tui_source_window_base::set_is_exec_point_at (struct tui_line_or_address l)
{
  bool changed = false;
  int i;

  i = 0;
  while (i < content.size ())
    {
      bool new_state;
      struct tui_line_or_address content_loa =
	content[i].line_or_addr;

      gdb_assert (l.loa == LOA_ADDRESS || l.loa == LOA_LINE);
      gdb_assert (content_loa.loa == LOA_LINE
		  || content_loa.loa == LOA_ADDRESS);
      if (content_loa.loa == l.loa
	  && ((l.loa == LOA_LINE && content_loa.u.line_no == l.u.line_no)
              || (l.loa == LOA_ADDRESS && content_loa.u.addr == l.u.addr)))
        new_state = true;
      else
	new_state = false;
      if (new_state != content[i].is_exec_point)
        {
          changed = true;
          content[i].is_exec_point = new_state;
          tui_show_source_line (this, i + 1);
        }
      i++;
    }
  if (changed)
    refill ();
}

/* See tui-winsource.h.  */

void
tui_update_all_breakpoint_info (struct breakpoint *being_deleted)
{
  for (tui_source_window_base *win : tui_source_windows ())
    {
      if (tui_update_breakpoint_info (win, being_deleted, false))
        {
          tui_update_exec_info (win);
        }
    }
}


/* Scan the source window and the breakpoints to update the break_mode
   information for each line.

   Returns true if something changed and the execution window must be
   refreshed.  */

bool
tui_update_breakpoint_info (struct tui_source_window_base *win,
			    struct breakpoint *being_deleted,
			    bool current_only)
{
  int i;
  bool need_refresh = false;

  for (i = 0; i < win->content.size (); i++)
    {
      struct breakpoint *bp;
      extern struct breakpoint *breakpoint_chain;
      struct tui_source_element *line;

      line = &win->content[i];
      if (current_only && !line->is_exec_point)
         continue;

      /* Scan each breakpoint to see if the current line has something to
         do with it.  Identify enable/disabled breakpoints as well as
         those that we already hit.  */
      tui_bp_flags mode = 0;
      for (bp = breakpoint_chain;
           bp != NULL;
           bp = bp->next)
        {
	  struct bp_location *loc;

	  gdb_assert (line->line_or_addr.loa == LOA_LINE
		      || line->line_or_addr.loa == LOA_ADDRESS);

	  if (bp == being_deleted)
	    continue;

	  for (loc = bp->loc; loc != NULL; loc = loc->next)
	    {
	      if (win->location_matches_p (loc, i))
		{
		  if (bp->enable_state == bp_disabled)
		    mode |= TUI_BP_DISABLED;
		  else
		    mode |= TUI_BP_ENABLED;
		  if (bp->hit_count)
		    mode |= TUI_BP_HIT;
		  if (bp->loc->cond)
		    mode |= TUI_BP_CONDITIONAL;
		  if (bp->type == bp_hardware_breakpoint)
		    mode |= TUI_BP_HARDWARE;
		}
	    }
        }
      if (line->break_mode != mode)
        {
          line->break_mode = mode;
          need_refresh = true;
        }
    }
  return need_refresh;
}

/* See tui-data.h.  */

tui_exec_info_content *
tui_exec_info_window::maybe_allocate_content (int n_elements)
{
  if (m_content == nullptr)
    m_content = XNEWVEC (tui_exec_info_content, n_elements);
  return m_content;
}


/* Function to initialize the content of the execution info window,
   based upon the input window which is either the source or
   disassembly window.  */
void
tui_set_exec_info_content (struct tui_source_window_base *win_info)
{
  tui_exec_info_content *content
    = win_info->execution_info->maybe_allocate_content (win_info->height);

  tui_update_breakpoint_info (win_info, nullptr, true);
  for (int i = 0; i < win_info->content.size (); i++)
    {
      tui_exec_info_content &element = content[i];
      struct tui_source_element *src_element;
      tui_bp_flags mode;

      src_element = &win_info->content[i];

      memset (element, ' ', sizeof (tui_exec_info_content));
      element[TUI_EXECINFO_SIZE - 1] = 0;

      /* Now update the exec info content based upon the state
	 of each line as indicated by the source content.  */
      mode = src_element->break_mode;
      if (mode & TUI_BP_HIT)
	element[TUI_BP_HIT_POS] = (mode & TUI_BP_HARDWARE) ? 'H' : 'B';
      else if (mode & (TUI_BP_ENABLED | TUI_BP_DISABLED))
	element[TUI_BP_HIT_POS] = (mode & TUI_BP_HARDWARE) ? 'h' : 'b';

      if (mode & TUI_BP_ENABLED)
	element[TUI_BP_BREAK_POS] = '+';
      else if (mode & TUI_BP_DISABLED)
	element[TUI_BP_BREAK_POS] = '-';

      if (src_element->is_exec_point)
	element[TUI_EXEC_POS] = '>';
    }
}


void
tui_show_exec_info_content (struct tui_source_window_base *win_info)
{
  struct tui_exec_info_window *exec_info = win_info->execution_info;
  const tui_exec_info_content *content = exec_info->get_content ();

  werase (exec_info->handle);
  for (int cur_line = 1; cur_line <= win_info->content.size (); cur_line++)
    mvwaddstr (exec_info->handle,
	       cur_line,
	       0,
	       content[cur_line - 1]);
  exec_info->refresh_window ();
}


void
tui_erase_exec_info_content (struct tui_source_window_base *win_info)
{
  struct tui_gen_win_info *exec_info = win_info->execution_info;

  werase (exec_info->handle);
  exec_info->refresh_window ();
}

void
tui_clear_exec_info_content (struct tui_source_window_base *win_info)
{
  tui_erase_exec_info_content (win_info);
}

/* Function to update the execution info window.  */
void
tui_update_exec_info (struct tui_source_window_base *win_info)
{
  tui_set_exec_info_content (win_info);
  tui_show_exec_info_content (win_info);
}

void
tui_alloc_source_buffer (struct tui_source_window_base *win_info)
{
  int i, line_width, max_lines;

  /* The window width/height includes the highlight box.  Determine actual
     content dimensions, including string null-terminators.  */
  max_lines = win_info->height - 2;
  line_width = win_info->width - 2 + 1;

  /* Allocate the buffer for the source lines.  */
  win_info->content.resize (max_lines);
  for (i = 0; i < max_lines; i++)
    {
      if (win_info->content[i].line == nullptr)
	win_info->content[i].line = (char *) xmalloc (line_width);
    }
}


/* Answer whether a particular line number or address is displayed
   in the current source window.  */
int
tui_line_is_displayed (int line, 
		       struct tui_source_window_base *win_info,
		       int check_threshold)
{
  int is_displayed = FALSE;
  int i, threshold;

  if (check_threshold)
    threshold = SCROLL_THRESHOLD;
  else
    threshold = 0;
  i = 0;
  while (i < win_info->content.size () - threshold
	 && !is_displayed)
    {
      is_displayed
	= win_info->content[i].line_or_addr.loa == LOA_LINE
	  && win_info->content[i].line_or_addr.u.line_no == line;
      i++;
    }

  return is_displayed;
}


/* Answer whether a particular line number or address is displayed
   in the current source window.  */
int
tui_addr_is_displayed (CORE_ADDR addr, 
		       struct tui_source_window_base *win_info,
		       int check_threshold)
{
  int is_displayed = FALSE;
  int i, threshold;

  if (check_threshold)
    threshold = SCROLL_THRESHOLD;
  else
    threshold = 0;
  i = 0;
  while (i < win_info->content.size () - threshold
	 && !is_displayed)
    {
      is_displayed
	= win_info->content[i].line_or_addr.loa == LOA_ADDRESS
	  && win_info->content[i].line_or_addr.u.addr == addr;
      i++;
    }

  return is_displayed;
}
