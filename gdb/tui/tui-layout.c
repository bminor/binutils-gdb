/* TUI layout window management.

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
#include "command.h"
#include "symtab.h"
#include "frame.h"
#include "source.h"
#include <ctype.h>

#include "tui/tui.h"
#include "tui/tui-command.h"
#include "tui/tui-data.h"
#include "tui/tui-wingeneral.h"
#include "tui/tui-stack.h"
#include "tui/tui-regs.h"
#include "tui/tui-win.h"
#include "tui/tui-winsource.h"
#include "tui/tui-disasm.h"
#include "tui/tui-layout.h"
#include "tui/tui-source.h"
#include "gdb_curses.h"

static void show_layout (enum tui_layout_type);
static enum tui_layout_type next_layout (void);
static enum tui_layout_type prev_layout (void);
static void tui_layout_command (const char *, int);
static void extract_display_start_addr (struct gdbarch **, CORE_ADDR *);


/* The pre-defined layouts.  */
static tui_layout_split *standard_layouts[UNDEFINED_LAYOUT];

/* The layout that is currently applied.  */
static std::unique_ptr<tui_layout_base> applied_layout;

static enum tui_layout_type current_layout = UNDEFINED_LAYOUT;

/* Accessor for the current layout.  */
enum tui_layout_type
tui_current_layout (void)
{
  return current_layout;
}

/* See tui-layout.h.  */

void
tui_apply_current_layout ()
{
  applied_layout->apply (0, 0, tui_term_width (), tui_term_height ());
}

/* See tui-layout.  */

void
tui_adjust_window_height (struct tui_win_info *win, int new_height)
{
  applied_layout->adjust_size (win->name (), new_height);
}

/* Show the screen layout defined.  */
static void
show_layout (enum tui_layout_type layout)
{
  enum tui_layout_type cur_layout = tui_current_layout ();

  if (layout != cur_layout)
    {
      tui_make_all_invisible ();
      applied_layout = standard_layouts[layout]->clone ();
      tui_apply_current_layout ();
      current_layout = layout;
      tui_delete_invisible_windows ();
    }
}


/* Function to set the layout to SRC_COMMAND, DISASSEM_COMMAND,
   SRC_DISASSEM_COMMAND, SRC_DATA_COMMAND, or DISASSEM_DATA_COMMAND.  */
void
tui_set_layout (enum tui_layout_type layout_type)
{
  gdb_assert (layout_type != UNDEFINED_LAYOUT);

  enum tui_layout_type cur_layout = tui_current_layout ();
  struct gdbarch *gdbarch;
  CORE_ADDR addr;
  struct tui_win_info *win_with_focus = tui_win_with_focus ();

  extract_display_start_addr (&gdbarch, &addr);

  enum tui_layout_type new_layout = layout_type;

  if (new_layout != cur_layout)
    {
      show_layout (new_layout);

      /* Now determine where focus should be.  */
      if (win_with_focus != TUI_CMD_WIN)
	{
	  switch (new_layout)
	    {
	    case SRC_COMMAND:
	      tui_set_win_focus_to (TUI_SRC_WIN);
	      break;
	    case DISASSEM_COMMAND:
	      /* The previous layout was not showing code.
		 This can happen if there is no source
		 available:

		 1. if the source file is in another dir OR
		 2. if target was compiled without -g
		 We still want to show the assembly though!  */

	      tui_get_begin_asm_address (&gdbarch, &addr);
	      tui_set_win_focus_to (TUI_DISASM_WIN);
	      break;
	    case SRC_DISASSEM_COMMAND:
	      /* The previous layout was not showing code.
		 This can happen if there is no source
		 available:

		 1. if the source file is in another dir OR
		 2. if target was compiled without -g
		 We still want to show the assembly though!  */

	      tui_get_begin_asm_address (&gdbarch, &addr);
	      if (win_with_focus == TUI_SRC_WIN)
		tui_set_win_focus_to (TUI_SRC_WIN);
	      else
		tui_set_win_focus_to (TUI_DISASM_WIN);
	      break;
	    case SRC_DATA_COMMAND:
	      if (win_with_focus != TUI_DATA_WIN)
		tui_set_win_focus_to (TUI_SRC_WIN);
	      else
		tui_set_win_focus_to (TUI_DATA_WIN);
	      break;
	    case DISASSEM_DATA_COMMAND:
	      /* The previous layout was not showing code.
		 This can happen if there is no source
		 available:

		 1. if the source file is in another dir OR
		 2. if target was compiled without -g
		 We still want to show the assembly though!  */

	      tui_get_begin_asm_address (&gdbarch, &addr);
	      if (win_with_focus != TUI_DATA_WIN)
		tui_set_win_focus_to (TUI_DISASM_WIN);
	      else
		tui_set_win_focus_to (TUI_DATA_WIN);
	      break;
	    default:
	      break;
	    }
	}
      /*
       * Now update the window content.
       */
      tui_update_source_windows_with_addr (gdbarch, addr);
      if (new_layout == SRC_DATA_COMMAND
	  || new_layout == DISASSEM_DATA_COMMAND)
	TUI_DATA_WIN->show_registers (TUI_DATA_WIN->get_current_group ());
    }
}

/* Add the specified window to the layout in a logical way.  This
   means setting up the most logical layout given the window to be
   added.  */
void
tui_add_win_to_layout (enum tui_win_type type)
{
  enum tui_layout_type cur_layout = tui_current_layout ();

  switch (type)
    {
    case SRC_WIN:
      if (cur_layout != SRC_COMMAND
	  && cur_layout != SRC_DISASSEM_COMMAND
	  && cur_layout != SRC_DATA_COMMAND)
	{
	  if (cur_layout == DISASSEM_DATA_COMMAND)
	    show_layout (SRC_DATA_COMMAND);
	  else
	    show_layout (SRC_COMMAND);
	}
      break;
    case DISASSEM_WIN:
      if (cur_layout != DISASSEM_COMMAND
	  && cur_layout != SRC_DISASSEM_COMMAND
	  && cur_layout != DISASSEM_DATA_COMMAND)
	{
	  if (cur_layout == SRC_DATA_COMMAND)
	    show_layout (DISASSEM_DATA_COMMAND);
	  else
	    show_layout (DISASSEM_COMMAND);
	}
      break;
    case DATA_WIN:
      if (cur_layout != SRC_DATA_COMMAND
	  && cur_layout != DISASSEM_DATA_COMMAND)
	{
	  if (cur_layout == DISASSEM_COMMAND)
	    show_layout (DISASSEM_DATA_COMMAND);
	  else
	    show_layout (SRC_DATA_COMMAND);
	}
      break;
    default:
      break;
    }
}

/* Complete possible layout names.  TEXT is the complete text entered so
   far, WORD is the word currently being completed.  */

static void
layout_completer (struct cmd_list_element *ignore,
		  completion_tracker &tracker,
		  const char *text, const char *word)
{
  static const char *layout_names [] =
    { "src", "asm", "split", "regs", "next", "prev", NULL };

  complete_on_enum (tracker, layout_names, text, word);
}

/* Function to set the layout to SRC, ASM, SPLIT, NEXT, PREV, DATA, or
   REGS. */
static void
tui_layout_command (const char *layout_name, int from_tty)
{
  enum tui_layout_type new_layout = UNDEFINED_LAYOUT;
  enum tui_layout_type cur_layout = tui_current_layout ();

  if (layout_name == NULL || *layout_name == '\0')
    error (_("Usage: layout prev | next | LAYOUT-NAME"));

  /* First check for ambiguous input.  */
  if (strcmp (layout_name, "s") == 0)
    error (_("Ambiguous command input."));

  if (subset_compare (layout_name, "src"))
    new_layout = SRC_COMMAND;
  else if (subset_compare (layout_name, "asm"))
    new_layout = DISASSEM_COMMAND;
  else if (subset_compare (layout_name, "split"))
    new_layout = SRC_DISASSEM_COMMAND;
  else if (subset_compare (layout_name, "regs"))
    {
      if (cur_layout == SRC_COMMAND
	  || cur_layout == SRC_DATA_COMMAND)
	new_layout = SRC_DATA_COMMAND;
      else
	new_layout = DISASSEM_DATA_COMMAND;
    }
  else if (subset_compare (layout_name, "next"))
    new_layout = next_layout ();
  else if (subset_compare (layout_name, "prev"))
    new_layout = prev_layout ();
  else
    error (_("Unrecognized layout: %s"), layout_name);

  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  tui_set_layout (new_layout);
}


static void
extract_display_start_addr (struct gdbarch **gdbarch_p, CORE_ADDR *addr_p)
{
  enum tui_layout_type cur_layout = tui_current_layout ();
  struct gdbarch *gdbarch = get_current_arch ();
  CORE_ADDR addr;
  CORE_ADDR pc;
  struct symtab_and_line cursal = get_current_source_symtab_and_line ();

  switch (cur_layout)
    {
    case SRC_COMMAND:
    case SRC_DATA_COMMAND:
      gdbarch = TUI_SRC_WIN->gdbarch;
      find_line_pc (cursal.symtab,
		    TUI_SRC_WIN->start_line_or_addr.u.line_no,
		    &pc);
      addr = pc;
      break;
    case DISASSEM_COMMAND:
    case SRC_DISASSEM_COMMAND:
    case DISASSEM_DATA_COMMAND:
      gdbarch = TUI_DISASM_WIN->gdbarch;
      addr = TUI_DISASM_WIN->start_line_or_addr.u.addr;
      break;
    default:
      addr = 0;
      break;
    }

  *gdbarch_p = gdbarch;
  *addr_p = addr;
}


/* Answer the previous layout to cycle to.  */
static enum tui_layout_type
next_layout (void)
{
  int new_layout;

  new_layout = tui_current_layout ();
  if (new_layout == UNDEFINED_LAYOUT)
    new_layout = SRC_COMMAND;
  else
    {
      new_layout++;
      if (new_layout == UNDEFINED_LAYOUT)
	new_layout = SRC_COMMAND;
    }

  return (enum tui_layout_type) new_layout;
}


/* Answer the next layout to cycle to.  */
static enum tui_layout_type
prev_layout (void)
{
  int new_layout;

  new_layout = tui_current_layout ();
  if (new_layout == SRC_COMMAND)
    new_layout = DISASSEM_DATA_COMMAND;
  else
    {
      new_layout--;
      if (new_layout == UNDEFINED_LAYOUT)
	new_layout = DISASSEM_DATA_COMMAND;
    }

  return (enum tui_layout_type) new_layout;
}

void
tui_gen_win_info::resize (int height_, int width_,
			  int origin_x_, int origin_y_)
{
  if (width == width_ && height == height_
      && x == origin_x_ && y == origin_y_
      && handle != nullptr)
    return;

  width = width_;
  height = height_;
  if (height > 1)
    viewport_height = height - 2;
  else
    viewport_height = 1;
  x = origin_x_;
  y = origin_y_;

  if (handle != nullptr)
    {
#ifdef HAVE_WRESIZE
      wresize (handle.get (), height, width);
      mvwin (handle.get (), y, x);
      wmove (handle.get (), 0, 0);
#else
      handle.reset (nullptr);
#endif
    }

  if (handle == nullptr)
    make_window ();

  rerender ();
}



/* Helper function that returns a TUI window, given its name.  */

static tui_gen_win_info *
tui_get_window_by_name (const std::string &name)
{
  if (name == "src")
    {
      if (tui_win_list[SRC_WIN] == nullptr)
	tui_win_list[SRC_WIN] = new tui_source_window ();
      return tui_win_list[SRC_WIN];
    }
  else if (name == "cmd")
    {
      if (tui_win_list[CMD_WIN] == nullptr)
	tui_win_list[CMD_WIN] = new tui_cmd_window ();
      return tui_win_list[CMD_WIN];
    }
  else if (name == "regs")
    {
      if (tui_win_list[DATA_WIN] == nullptr)
	tui_win_list[DATA_WIN] = new tui_data_window ();
      return tui_win_list[DATA_WIN];
    }
  else if (name == "asm")
    {
      if (tui_win_list[DISASSEM_WIN] == nullptr)
	tui_win_list[DISASSEM_WIN] = new tui_disasm_window ();
      return tui_win_list[DISASSEM_WIN];
    }
  else
    {
      gdb_assert (name == "locator");
      return tui_locator_win_info_ptr ();
    }
}

/* See tui-layout.h.  */

std::unique_ptr<tui_layout_base>
tui_layout_window::clone () const
{
  tui_layout_window *result = new tui_layout_window (m_contents.c_str ());
  return std::unique_ptr<tui_layout_base> (result);
}

/* See tui-layout.h.  */

void
tui_layout_window::apply (int x_, int y_, int width_, int height_)
{
  x = x_;
  y = y_;
  width = width_;
  height = height_;
  gdb_assert (m_window != nullptr);
  m_window->resize (height, width, x, y);
}

/* See tui-layout.h.  */

void
tui_layout_window::get_sizes (int *min_height, int *max_height)
{
  if (m_window == nullptr)
    m_window = tui_get_window_by_name (m_contents);
  *min_height = m_window->min_height ();
  *max_height = m_window->max_height ();
}

/* See tui-layout.h.  */

bool
tui_layout_window::top_boxed_p () const
{
  gdb_assert (m_window != nullptr);
  return m_window->can_box ();
}

/* See tui-layout.h.  */

bool
tui_layout_window::bottom_boxed_p () const
{
  gdb_assert (m_window != nullptr);
  return m_window->can_box ();
}

/* See tui-layout.h.  */

tui_layout_split *
tui_layout_split::add_split (int weight)
{
  tui_layout_split *result = new tui_layout_split ();
  split s = {weight, std::unique_ptr<tui_layout_base> (result)};
  m_splits.push_back (std::move (s));
  return result;
}

/* See tui-layout.h.  */

void
tui_layout_split::add_window (const char *name, int weight)
{
  tui_layout_window *result = new tui_layout_window (name);
  split s = {weight, std::unique_ptr<tui_layout_base> (result)};
  m_splits.push_back (std::move (s));
}

/* See tui-layout.h.  */

std::unique_ptr<tui_layout_base>
tui_layout_split::clone () const
{
  tui_layout_split *result = new tui_layout_split ();
  for (const split &item : m_splits)
    {
      std::unique_ptr<tui_layout_base> next = item.layout->clone ();
      split s = {item.weight, std::move (next)};
      result->m_splits.push_back (std::move (s));
    }
  return std::unique_ptr<tui_layout_base> (result);
}

/* See tui-layout.h.  */

void
tui_layout_split::get_sizes (int *min_height, int *max_height)
{
  *min_height = 0;
  *max_height = 0;
  for (const split &item : m_splits)
    {
      int new_min, new_max;
      item.layout->get_sizes (&new_min, &new_max);
      *min_height += new_min;
      *max_height += new_max;
    }
}

/* See tui-layout.h.  */

bool
tui_layout_split::top_boxed_p () const
{
  if (m_splits.empty ())
    return false;
  return m_splits[0].layout->top_boxed_p ();
}

/* See tui-layout.h.  */

bool
tui_layout_split::bottom_boxed_p () const
{
  if (m_splits.empty ())
    return false;
  return m_splits.back ().layout->top_boxed_p ();
}

/* See tui-layout.h.  */

void
tui_layout_split::set_weights_from_heights ()
{
  for (int i = 0; i < m_splits.size (); ++i)
    m_splits[i].weight = m_splits[i].layout->height;
}

/* See tui-layout.h.  */

bool
tui_layout_split::adjust_size (const char *name, int new_height)
{
  /* Look through the children.  If one is a layout holding the named
     window, we're done; or if one actually is the named window,
     update it.  */
  int found_index = -1;
  for (int i = 0; i < m_splits.size (); ++i)
    {
      if (m_splits[i].layout->adjust_size (name, new_height))
	return true;
      const char *win_name = m_splits[i].layout->get_name ();
      if (win_name != nullptr && strcmp (name, win_name) == 0)
	{
	  found_index = i;
	  break;
	}
    }

  if (found_index == -1)
    return false;
  if (m_splits[found_index].layout->height == new_height)
    return true;

  set_weights_from_heights ();
  int delta = m_splits[found_index].weight - new_height;
  m_splits[found_index].weight = new_height;

  /* Distribute the "delta" over the next window; but if the next
     window cannot hold it all, keep going until we either find a
     window that does, or until we loop all the way around.  */
  for (int i = 0; delta != 0 && i < m_splits.size () - 1; ++i)
    {
      int index = (found_index + 1 + i) % m_splits.size ();

      int new_min, new_max;
      m_splits[index].layout->get_sizes (&new_min, &new_max);

      if (delta < 0)
	{
	  /* The primary window grew, so we are trying to shrink other
	     windows.  */
	  int available = m_splits[index].weight - new_min;
	  int shrink_by = std::min (available, -delta);
	  m_splits[index].weight -= shrink_by;
	  delta += shrink_by;
	}
      else
	{
	  /* The primary window shrank, so we are trying to grow other
	     windows.  */
	  int available = new_max - m_splits[index].weight;
	  int grow_by = std::min (available, delta);
	  m_splits[index].weight += grow_by;
	  delta -= grow_by;
	}
    }

  if (delta != 0)
    {
      warning (_("Invalid window height specified"));
      /* Effectively undo any modifications made here.  */
      set_weights_from_heights ();
    }
  else
    {
      /* Simply re-apply the updated layout.  */
      apply (x, y, width, height);
    }

  return true;
}

/* See tui-layout.h.  */

void
tui_layout_split::apply (int x_, int y_, int width_, int height_)
{
  x = x_;
  y = y_;
  width = width_;
  height = height_;

  struct height_info
  {
    int height;
    int min_height;
    int max_height;
    /* True if this window will share a box border with the previous
       window in the list.  */
    bool share_box;
  };

  std::vector<height_info> info (m_splits.size ());

  /* Step 1: Find the min and max height of each sub-layout.
     Fixed-sized layouts are given their desired height, and then the
     remaining space is distributed among the remaining windows
     according to the weights given.  */
  int available_height = height;
  int last_index = -1;
  int total_weight = 0;
  for (int i = 0; i < m_splits.size (); ++i)
    {
      bool cmd_win_already_exists = TUI_CMD_WIN != nullptr;

      /* Always call get_sizes, to ensure that the window is
	 instantiated.  This is a bit gross but less gross than adding
	 special cases for this in other places.  */
      m_splits[i].layout->get_sizes (&info[i].min_height, &info[i].max_height);

      if (!m_applied
	  && cmd_win_already_exists
	  && m_splits[i].layout->get_name () != nullptr
	  && strcmp (m_splits[i].layout->get_name (), "cmd") == 0)
	{
	  /* If this layout has never been applied, then it means the
	     user just changed the layout.  In this situation, it's
	     desirable to keep the size of the command window the
	     same.  Setting the min and max heights this way ensures
	     that the resizing step, below, does the right thing with
	     this window.  */
	  info[i].min_height = TUI_CMD_WIN->height;
	  info[i].max_height = TUI_CMD_WIN->height;
	}

      if (info[i].min_height == info[i].max_height)
	available_height -= info[i].min_height;
      else
	{
	  last_index = i;
	  total_weight += m_splits[i].weight;
	}

      /* Two adjacent boxed windows will share a border, making a bit
	 more height available.  */
      if (i > 0
	  && m_splits[i - 1].layout->bottom_boxed_p ()
	  && m_splits[i].layout->top_boxed_p ())
	info[i].share_box = true;
    }

  /* Step 2: Compute the height of each sub-layout.  Fixed-sized items
     are given their fixed size, while others are resized according to
     their weight.  */
  int used_height = 0;
  for (int i = 0; i < m_splits.size (); ++i)
    {
      /* Compute the height and clamp to the allowable range.  */
      info[i].height = available_height * m_splits[i].weight / total_weight;
      if (info[i].height > info[i].max_height)
	info[i].height = info[i].max_height;
      if (info[i].height < info[i].min_height)
	info[i].height = info[i].min_height;
      /* If there is any leftover height, just redistribute it to the
	 last resizeable window, by dropping it from the allocated
	 height.  We could try to be fancier here perhaps, by
	 redistributing this height among all windows, not just the
	 last window.  */
      if (info[i].min_height != info[i].max_height)
	{
	  used_height += info[i].height;
	  if (info[i].share_box)
	    --used_height;
	}
    }

  /* Allocate any leftover height.  */
  if (available_height >= used_height && last_index != -1)
    info[last_index].height += available_height - used_height;

  /* Step 3: Resize.  */
  int height_accum = 0;
  for (int i = 0; i < m_splits.size (); ++i)
    {
      /* If we fall off the bottom, just make allocations overlap.
	 GIGO.  */
      if (height_accum + info[i].height > height)
	height_accum = height - info[i].height;
      else if (info[i].share_box)
	--height_accum;
      m_splits[i].layout->apply (x, y + height_accum, width, info[i].height);
      height_accum += info[i].height;
    }

  m_applied = true;
}

static void
initialize_layouts ()
{
  standard_layouts[SRC_COMMAND] = new tui_layout_split ();
  standard_layouts[SRC_COMMAND]->add_window ("src", 2);
  standard_layouts[SRC_COMMAND]->add_window ("locator", 0);
  standard_layouts[SRC_COMMAND]->add_window ("cmd", 1);

  standard_layouts[DISASSEM_COMMAND] = new tui_layout_split ();
  standard_layouts[DISASSEM_COMMAND]->add_window ("asm", 2);
  standard_layouts[DISASSEM_COMMAND]->add_window ("locator", 0);
  standard_layouts[DISASSEM_COMMAND]->add_window ("cmd", 1);

  standard_layouts[SRC_DATA_COMMAND] = new tui_layout_split ();
  standard_layouts[SRC_DATA_COMMAND]->add_window ("regs", 1);
  standard_layouts[SRC_DATA_COMMAND]->add_window ("src", 1);
  standard_layouts[SRC_DATA_COMMAND]->add_window ("locator", 0);
  standard_layouts[SRC_DATA_COMMAND]->add_window ("cmd", 1);

  standard_layouts[DISASSEM_DATA_COMMAND] = new tui_layout_split ();
  standard_layouts[DISASSEM_DATA_COMMAND]->add_window ("regs", 1);
  standard_layouts[DISASSEM_DATA_COMMAND]->add_window ("asm", 1);
  standard_layouts[DISASSEM_DATA_COMMAND]->add_window ("locator", 0);
  standard_layouts[DISASSEM_DATA_COMMAND]->add_window ("cmd", 1);

  standard_layouts[SRC_DISASSEM_COMMAND] = new tui_layout_split ();
  standard_layouts[SRC_DISASSEM_COMMAND]->add_window ("src", 1);
  standard_layouts[SRC_DISASSEM_COMMAND]->add_window ("asm", 1);
  standard_layouts[SRC_DISASSEM_COMMAND]->add_window ("locator", 0);
  standard_layouts[SRC_DISASSEM_COMMAND]->add_window ("cmd", 1);
}



/* Function to initialize gdb commands, for tui window layout
   manipulation.  */

void
_initialize_tui_layout (void)
{
  struct cmd_list_element *cmd;

  cmd = add_com ("layout", class_tui, tui_layout_command, _("\
Change the layout of windows.\n\
Usage: layout prev | next | LAYOUT-NAME\n\
Layout names are:\n\
   src   : Displays source and command windows.\n\
   asm   : Displays disassembly and command windows.\n\
   split : Displays source, disassembly and command windows.\n\
   regs  : Displays register window. If existing layout\n\
           is source/command or assembly/command, the \n\
           register window is displayed. If the\n\
           source/assembly/command (split) is displayed, \n\
           the register window is displayed with \n\
           the window that has current logical focus."));
  set_cmd_completer (cmd, layout_completer);

  initialize_layouts ();
}
