/* TUI layout window management.

   Copyright (C) 1998-2020 Free Software Foundation, Inc.

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
static void show_source_or_disasm_and_command (enum tui_layout_type);
static void show_source_command (void);
static void show_disasm_command (void);
static void show_source_disasm_command (void);
static void show_data (enum tui_layout_type);
static enum tui_layout_type next_layout (void);
static enum tui_layout_type prev_layout (void);
static void tui_layout_command (const char *, int);
static void extract_display_start_addr (struct gdbarch **, CORE_ADDR *);


static enum tui_layout_type current_layout = UNDEFINED_LAYOUT;

/* Accessor for the current layout.  */
enum tui_layout_type
tui_current_layout (void)
{
  return current_layout;
}


/* Show the screen layout defined.  */
static void
show_layout (enum tui_layout_type layout)
{
  enum tui_layout_type cur_layout = tui_current_layout ();

  if (layout != cur_layout)
    {
      tui_make_all_invisible ();
      switch (layout)
	{
	case SRC_DATA_COMMAND:
	case DISASSEM_DATA_COMMAND:
	  show_data (layout);
	  break;
	  /* Now show the new layout.  */
	case SRC_COMMAND:
	  show_source_command ();
	  break;
	case DISASSEM_COMMAND:
	  show_disasm_command ();
	  break;
	case SRC_DISASSEM_COMMAND:
	  show_source_disasm_command ();
	  break;
	default:
	  break;
	}

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

/* Show the Source/Command layout.  */
static void
show_source_command (void)
{
  show_source_or_disasm_and_command (SRC_COMMAND);
}


/* Show the Dissassem/Command layout.  */
static void
show_disasm_command (void)
{
  show_source_or_disasm_and_command (DISASSEM_COMMAND);
}


/* Show the Source/Disassem/Command layout.  */
static void
show_source_disasm_command (void)
{
  int cmd_height, src_height, asm_height;

  if (TUI_CMD_WIN != NULL)
    cmd_height = TUI_CMD_WIN->height;
  else
    cmd_height = tui_term_height () / 3;

  src_height = (tui_term_height () - cmd_height) / 2;
  asm_height = tui_term_height () - (src_height + cmd_height);

  if (TUI_SRC_WIN == NULL)
    tui_win_list[SRC_WIN] = new tui_source_window ();
  TUI_SRC_WIN->resize (src_height,
		       tui_term_width (),
		       0,
		       0);

  struct tui_locator_window *locator = tui_locator_win_info_ptr ();
  gdb_assert (locator != nullptr);

  if (TUI_DISASM_WIN == NULL)
    tui_win_list[DISASSEM_WIN] = new tui_disasm_window ();
  TUI_DISASM_WIN->resize (asm_height,
			  tui_term_width (),
			  0,
			  src_height - 1);
  locator->resize (1, tui_term_width (),
		   0, (src_height + asm_height) - 1);

  if (TUI_CMD_WIN == NULL)
    tui_win_list[CMD_WIN] = new tui_cmd_window ();
  TUI_CMD_WIN->resize (cmd_height,
		       tui_term_width (),
		       0,
		       tui_term_height () - cmd_height);
}


/* Show the Source/Data/Command or the Dissassembly/Data/Command
   layout.  */
static void
show_data (enum tui_layout_type new_layout)
{
  int total_height = (tui_term_height () - TUI_CMD_WIN->height);
  int src_height, data_height;
  enum tui_win_type win_type;

  struct tui_locator_window *locator = tui_locator_win_info_ptr ();
  gdb_assert (locator != nullptr);

  data_height = total_height / 2;
  src_height = total_height - data_height;
  if (tui_win_list[DATA_WIN] == nullptr)
    tui_win_list[DATA_WIN] = new tui_data_window ();
  tui_win_list[DATA_WIN]->resize (data_height, tui_term_width (), 0, 0);

  if (new_layout == SRC_DATA_COMMAND)
    win_type = SRC_WIN;
  else
    win_type = DISASSEM_WIN;

  if (tui_win_list[win_type] == NULL)
    {
      if (win_type == SRC_WIN)
	tui_win_list[win_type] = new tui_source_window ();
      else
	tui_win_list[win_type] = new tui_disasm_window ();
    }

  tui_win_list[win_type]->resize (src_height,
				  tui_term_width (),
				  0,
				  data_height - 1);
  locator->resize (1, tui_term_width (),
		   0, total_height - 1);
  TUI_CMD_WIN->resize (TUI_CMD_WIN->height, tui_term_width (),
		       0, total_height);
}

void
tui_gen_win_info::resize (int height_, int width_,
			  int origin_x_, int origin_y_)
{
  if (width == width_ && height == height_
      && origin.x == origin_x_ && origin.y == origin_y_
      && handle != nullptr)
    return;

  width = width_;
  height = height_;
  if (height > 1)
    viewport_height = height - 2;
  else
    viewport_height = 1;
  origin.x = origin_x_;
  origin.y = origin_y_;

  if (handle != nullptr)
    {
#ifdef HAVE_WRESIZE
      wresize (handle.get (), height, width);
      mvwin (handle.get (), origin.y, origin.x);
      wmove (handle.get (), 0, 0);
#else
      handle.reset (nullptr);
#endif
    }

  if (handle == nullptr)
    make_window ();

  rerender ();
}

/* Show the Source/Command or the Disassem layout.  */
static void
show_source_or_disasm_and_command (enum tui_layout_type layout_type)
{
  struct tui_source_window_base *win_info;
  int src_height, cmd_height;
  struct tui_locator_window *locator = tui_locator_win_info_ptr ();
  gdb_assert (locator != nullptr);

  if (TUI_CMD_WIN != NULL)
    cmd_height = TUI_CMD_WIN->height;
  else
    cmd_height = tui_term_height () / 3;
  src_height = tui_term_height () - cmd_height;

  if (layout_type == SRC_COMMAND)
    {
      if (tui_win_list[SRC_WIN] == nullptr)
	tui_win_list[SRC_WIN] = new tui_source_window ();
      win_info = TUI_SRC_WIN;
    }
  else
    {
      if (tui_win_list[DISASSEM_WIN] == nullptr)
	tui_win_list[DISASSEM_WIN] = new tui_disasm_window ();
      win_info = TUI_DISASM_WIN;
    }

  locator->resize (1, tui_term_width (),
		   0, src_height - 1);
  win_info->resize (src_height - 1,
		    tui_term_width (),
		    0,
		    0);

  if (TUI_CMD_WIN == NULL)
    tui_win_list[CMD_WIN] = new tui_cmd_window ();
  TUI_CMD_WIN->resize (cmd_height,
		       tui_term_width (),
		       0,
		       src_height);
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
}
