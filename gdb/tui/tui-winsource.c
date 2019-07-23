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
  auto adapter = tui_source_windows ();
  if (adapter.begin () != adapter.end ())
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
tui_source_window_base::update_source_window
  (struct gdbarch *gdbarch,
   struct symtab *s,
   struct tui_line_or_address line_or_addr)
{
  horizontal_offset = 0;
  update_source_window_as_is (gdbarch, s, line_or_addr);
}


/* Function to display source in the source/asm window.  This function
   shows the source as specified by the horizontal offset.  */
void
tui_source_window_base::update_source_window_as_is
  (struct gdbarch *gdbarch,
   struct symtab *s,
   struct tui_line_or_address line_or_addr)
{
  enum tui_status ret
    = set_contents (gdbarch, s, line_or_addr);

  if (ret == TUI_FAILURE)
    erase_source_content ();
  else
    {
      update_breakpoint_info (nullptr, false);
      show_source_content ();
      update_exec_info ();
      if (type == SRC_WIN)
	{
	  symtab_and_line sal;

	  sal.line = line_or_addr.u.line_no + (content.size () - 2);
	  sal.symtab = s;
	  sal.pspace = SYMTAB_PSPACE (s);
	  set_current_source_symtab_and_line (sal);
	  /* If the focus was in the asm win, put it in the src win if
	     we don't have a split layout.  */
	  if (tui_win_with_focus () == TUI_DISASM_WIN
	      && tui_current_layout () != SRC_DISASSEM_COMMAND)
	    tui_set_win_focus_to (this);
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
	  TUI_SRC_WIN->show_symtab_source (gdbarch, sal.symtab, l);
	  break;
	}
    }
  else
    {
      for (struct tui_source_window_base *win_info : tui_source_windows ())
	win_info->erase_source_content ();
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
      TUI_SRC_WIN->show_symtab_source (gdbarch, s, l);
      if (tui_current_layout () == SRC_DISASSEM_COMMAND)
	{
	  find_line_pc (s, line, &pc);
	  tui_show_disassem (gdbarch, pc);
	}
      break;
    }
}

void
tui_source_window_base::do_erase_source_content (const char *str)
{
  int x_pos;
  int half_width = (width - 2) / 2;

  content.clear ();
  if (handle != NULL)
    {
      werase (handle);
      check_and_display_highlight_if_needed ();

      if (strlen (str) >= half_width)
	x_pos = 1;
      else
	x_pos = half_width - strlen (str);
      mvwaddstr (handle,
		 (height / 2),
		 x_pos,
		 (char *) str);

      refresh_window ();
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

  wmove (win_info->handle, lineno, TUI_EXECINFO_SIZE);
  tui_puts (line->line.get (), win_info->handle);
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
tui_source_window_base::show_source_content ()
{
  gdb_assert (!content.empty ());

  for (int lineno = 1; lineno <= content.size (); lineno++)
    tui_show_source_line (this, lineno);

  check_and_display_highlight_if_needed ();
  refresh_window ();
}

tui_source_window_base::tui_source_window_base (enum tui_win_type type)
  : tui_win_info (type)
{
  gdb_assert (type == SRC_WIN || type == DISASSEM_WIN);
  start_line_or_addr.loa = LOA_ADDRESS;
  start_line_or_addr.u.addr = 0;
}


/* See tui-data.h.  */

void
tui_source_window_base::update_tab_width ()
{
  werase (handle);
  rerender ();
}

void
tui_source_window_base::rerender ()
{
  if (!content.empty ())
    {
      struct tui_line_or_address line_or_addr;
      struct symtab_and_line cursal
	= get_current_source_symtab_and_line ();

      line_or_addr = start_line_or_addr;
      update_source_window (gdbarch, cursal.symtab, line_or_addr);
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
      update_source_window (gdbarch, s, line);
    }
  else
    erase_source_content ();
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

  update_source_window_as_is (gdbarch, s, content[0].line_or_addr);
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
      if (win->update_breakpoint_info (being_deleted, false))
	win->update_exec_info ();
    }
}


/* Scan the source window and the breakpoints to update the break_mode
   information for each line.

   Returns true if something changed and the execution window must be
   refreshed.  */

bool
tui_source_window_base::update_breakpoint_info
  (struct breakpoint *being_deleted, bool current_only)
{
  int i;
  bool need_refresh = false;

  for (i = 0; i < content.size (); i++)
    {
      struct breakpoint *bp;
      extern struct breakpoint *breakpoint_chain;
      struct tui_source_element *line;

      line = &content[i];
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
	      if (location_matches_p (loc, i))
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

/* Function to initialize the content of the execution info window,
   based upon the input window which is either the source or
   disassembly window.  */
void
tui_source_window_base::update_exec_info ()
{
  update_breakpoint_info (nullptr, true);
  for (int i = 0; i < content.size (); i++)
    {
      struct tui_source_element *src_element = &content[i];
      char element[TUI_EXECINFO_SIZE] = "   ";

      /* Now update the exec info content based upon the state
	 of each line as indicated by the source content.  */
      tui_bp_flags mode = src_element->break_mode;
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

      mvwaddstr (handle, i + 1, 1, element);
    }
  refresh_window ();
}
