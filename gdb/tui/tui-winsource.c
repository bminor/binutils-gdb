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

  return;
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
    ret = tui_set_source_content (s, line_or_addr.u.line_no, noerror);
  else
    ret = tui_set_disassem_content (gdbarch, line_or_addr.u.addr);

  if (ret == TUI_FAILURE)
    {
      tui_clear_source_content (win_info, EMPTY_SOURCE_PROMPT);
      tui_clear_exec_info_content (win_info);
    }
  else
    {
      tui_update_breakpoint_info (win_info, 0);
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
	    tui_set_win_focus_to (TUI_SRC_WIN);
	}
    }


  return;
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
	  tui_show_symtab_source (gdbarch, sal.symtab, l, FALSE);
	  break;
	}
    }
  else
    {
      for (struct tui_source_window_base *win_info : tui_source_windows ())
	{
	  tui_clear_source_content (win_info, EMPTY_SOURCE_PROMPT);
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
      tui_show_symtab_source (gdbarch, s, l, FALSE);
      if (tui_current_layout () == SRC_DISASSEM_COMMAND)
	{
	  find_line_pc (s, line, &pc);
	  tui_show_disassem (gdbarch, pc);
	}
      break;
    }

  return;
}

void
tui_clear_source_content (struct tui_source_window_base *win_info,
			  int display_prompt)
{
  if (win_info != NULL)
    {
      int i;

      win_info->content_in_use = FALSE;
      tui_erase_source_content (win_info, display_prompt);
      for (i = 0; i < win_info->content.size (); i++)
	{
	  struct tui_source_element *element = &win_info->content[i];

	  element->has_break = FALSE;
	  element->is_exec_point = false;
	}
    }
}


void
tui_erase_source_content (struct tui_source_window_base *win_info,
			  int display_prompt)
{
  int x_pos;
  int half_width = (win_info->width - 2) / 2;

  if (win_info->handle != NULL)
    {
      werase (win_info->handle);
      tui_check_and_display_highlight_if_needed (win_info);
      if (display_prompt == EMPTY_SOURCE_PROMPT)
	{
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

	  /* elz: Added this function call to set the real contents of
	     the window to what is on the screen, so that later calls
	     to refresh, do display the correct stuff, and not the old
	     image.  */

	  tui_set_source_content_nil (win_info, no_src_str);
	}
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
    tui_erase_source_content (win_info, TRUE);

  tui_check_and_display_highlight_if_needed (win_info);
  win_info->refresh_window ();
  win_info->content_in_use = TRUE;
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


/* Set or clear the has_break flag in the line whose line is
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

/* Update the execution windows to show the active breakpoints.
   This is called whenever a breakpoint is inserted, removed or
   has its state changed.  */
void
tui_update_all_breakpoint_info ()
{
  for (tui_source_window_base *win : tui_source_windows ())
    {
      if (tui_update_breakpoint_info (win, FALSE))
        {
          tui_update_exec_info (win);
        }
    }
}


/* Scan the source window and the breakpoints to update the has_break
   information for each line.

   Returns 1 if something changed and the execution window must be
   refreshed.  */

int
tui_update_breakpoint_info (struct tui_source_window_base *win, 
			    int current_only)
{
  int i;
  int need_refresh = 0;
  tui_source_window_base *src = (tui_source_window_base *) win;

  for (i = 0; i < win->content.size (); i++)
    {
      struct breakpoint *bp;
      extern struct breakpoint *breakpoint_chain;
      int mode;
      struct tui_source_element *line;

      line = &win->content[i];
      if (current_only && !line->is_exec_point)
         continue;

      /* Scan each breakpoint to see if the current line has something to
         do with it.  Identify enable/disabled breakpoints as well as
         those that we already hit.  */
      mode = 0;
      for (bp = breakpoint_chain;
           bp != NULL;
           bp = bp->next)
        {
	  struct bp_location *loc;

	  gdb_assert (line->line_or_addr.loa == LOA_LINE
		      || line->line_or_addr.loa == LOA_ADDRESS);

	  for (loc = bp->loc; loc != NULL; loc = loc->next)
	    {
	      if ((win == TUI_SRC_WIN
		   && loc->symtab != NULL
		   && filename_cmp (src->fullname,
				    symtab_to_fullname (loc->symtab)) == 0
		   && line->line_or_addr.loa == LOA_LINE
		   && loc->line_number == line->line_or_addr.u.line_no)
		  || (win == TUI_DISASM_WIN
		      && line->line_or_addr.loa == LOA_ADDRESS
		      && loc->address == line->line_or_addr.u.addr))
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
      if (line->has_break != mode)
        {
          line->has_break = mode;
          need_refresh = 1;
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
  if (win_info->execution_info != NULL)
    {
      tui_exec_info_content *content
	= win_info->execution_info->maybe_allocate_content (win_info->height);

      tui_update_breakpoint_info (win_info, 1);
      for (int i = 0; i < win_info->content.size (); i++)
	{
	  tui_exec_info_content &element = content[i];
	  struct tui_source_element *src_element;
	  int mode;

	  src_element = &win_info->content[i];

	  memset (element, ' ', sizeof (tui_exec_info_content));
	  element[TUI_EXECINFO_SIZE - 1] = 0;

	  /* Now update the exec info content based upon the state
	     of each line as indicated by the source content.  */
	  mode = src_element->has_break;
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
}


void
tui_show_exec_info_content (struct tui_source_window_base *win_info)
{
  struct tui_exec_info_window *exec_info = win_info->execution_info;
  const tui_exec_info_content *content = exec_info->get_content ();

  werase (exec_info->handle);
  exec_info->refresh_window ();
  for (int cur_line = 1; cur_line <= win_info->content.size (); cur_line++)
    mvwaddstr (exec_info->handle,
	       cur_line,
	       0,
	       content[cur_line - 1]);
  exec_info->refresh_window ();
  exec_info->content_in_use = TRUE;
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
  win_info->execution_info->content_in_use = FALSE;
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


/*****************************************
** STATIC LOCAL FUNCTIONS               **
******************************************/
