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
#include "tui/tui-data.h"
#include "tui/tui-windata.h"
#include "tui/tui-wingeneral.h"
#include "tui/tui-stack.h"
#include "tui/tui-regs.h"
#include "tui/tui-win.h"
#include "tui/tui-winsource.h"
#include "tui/tui-disasm.h"
#include "tui/tui-layout.h"
#include "gdb_curses.h"

/*******************************
** Static Local Decls
********************************/
static void show_layout (enum tui_layout_type);
static tui_gen_win_info *init_and_make_win (tui_gen_win_info *,
					    enum tui_win_type,
					    int, int, int, int,
					    enum tui_box);
static void show_source_or_disasm_and_command (enum tui_layout_type);
static struct tui_win_info *make_source_or_disasm_window (enum tui_win_type, 
							  int, int);
static struct tui_win_info *make_command_window (int, int);
static struct tui_win_info *make_source_window (int, int);
static struct tui_win_info *make_disasm_window (int, int);
static void make_data_window (struct tui_win_info **, int, int);
static void show_source_command (void);
static void show_disasm_command (void);
static void show_source_disasm_command (void);
static void show_data (enum tui_layout_type);
static enum tui_layout_type next_layout (void);
static enum tui_layout_type prev_layout (void);
static void tui_layout_command (const char *, int);
static void extract_display_start_addr (struct gdbarch **, CORE_ADDR *);


/***************************************
** DEFINITIONS
***************************************/

#define LAYOUT_USAGE     "Usage: layout prev | next | <layout_name> \n"

/* Show the screen layout defined.  */
static void
show_layout (enum tui_layout_type layout)
{
  enum tui_layout_type cur_layout = tui_current_layout ();

  if (layout != cur_layout)
    {
      /* Since the new layout may cause changes in window size, we
         should free the content and reallocate on next display of
         source/asm.  */
      tui_clear_source_windows ();
      if (layout == SRC_DATA_COMMAND 
	  || layout == DISASSEM_DATA_COMMAND)
	{
	  show_data (layout);
	  tui_refresh_all (tui_win_list);
	}
      else
	{
	  /* First make the current layout be invisible.  */
	  tui_make_all_invisible ();
	  tui_make_invisible (tui_locator_win_info_ptr ());

	  switch (layout)
	    {
	      /* Now show the new layout.  */
	    case SRC_COMMAND:
	      show_source_command ();
	      tui_add_to_source_windows (TUI_SRC_WIN);
	      break;
	    case DISASSEM_COMMAND:
	      show_disasm_command ();
	      tui_add_to_source_windows (TUI_DISASM_WIN);
	      break;
	    case SRC_DISASSEM_COMMAND:
	      show_source_disasm_command ();
	      tui_add_to_source_windows (TUI_SRC_WIN);
	      tui_add_to_source_windows (TUI_DISASM_WIN);
	      break;
	    default:
	      break;
	    }
	}
    }
}


/* Function to set the layout to SRC_COMMAND, DISASSEM_COMMAND,
   SRC_DISASSEM_COMMAND, SRC_DATA_COMMAND, or DISASSEM_DATA_COMMAND.  */
enum tui_status
tui_set_layout (enum tui_layout_type layout_type)
{
  enum tui_status status = TUI_SUCCESS;

  if (layout_type != UNDEFINED_LAYOUT)
    {
      enum tui_layout_type cur_layout = tui_current_layout (),
	new_layout = UNDEFINED_LAYOUT;
      int regs_populate = FALSE;
      struct gdbarch *gdbarch;
      CORE_ADDR addr;
      struct tui_win_info *win_with_focus = tui_win_with_focus ();
      struct tui_layout_def *layout_def = tui_layout_def ();

      extract_display_start_addr (&gdbarch, &addr);

      new_layout = layout_type;

      regs_populate = (new_layout == SRC_DATA_COMMAND
		       || new_layout == DISASSEM_DATA_COMMAND);
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
		  layout_def->display_mode = SRC_WIN;
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
		  layout_def->display_mode = DISASSEM_WIN;
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
		  layout_def->display_mode = SRC_WIN;
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
		  layout_def->display_mode = DISASSEM_WIN;
		  break;
		default:
		  break;
		}
	    }
	  /*
	   * Now update the window content.
	   */
	  if (!regs_populate
	      && (new_layout == SRC_DATA_COMMAND
		  || new_layout == DISASSEM_DATA_COMMAND))
	    tui_display_all_data ();

	  tui_update_source_windows_with_addr (gdbarch, addr);

	  if (regs_populate)
	    tui_show_registers (TUI_DATA_WIN->current_group);
	}
    }
  else
    status = TUI_FAILURE;

  return status;
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
	  tui_clear_source_windows_detail ();
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
	  tui_clear_source_windows_detail ();
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


/* Answer the height of a window.  If it hasn't been created yet,
   answer what the height of a window would be based upon its type and
   the layout.  */
int
tui_default_win_height (enum tui_win_type type, 
			enum tui_layout_type layout)
{
  int h;

  if (tui_win_list[type] != NULL)
    h = tui_win_list[type]->height;
  else
    {
      switch (layout)
	{
	case SRC_COMMAND:
	case DISASSEM_COMMAND:
	  if (TUI_CMD_WIN == NULL)
	    h = tui_term_height () / 2;
	  else
	    h = tui_term_height () - TUI_CMD_WIN->height;
	  break;
	case SRC_DISASSEM_COMMAND:
	case SRC_DATA_COMMAND:
	case DISASSEM_DATA_COMMAND:
	  if (TUI_CMD_WIN == NULL)
	    h = tui_term_height () / 3;
	  else
	    h = (tui_term_height () - TUI_CMD_WIN->height) / 2;
	  break;
	default:
	  h = 0;
	  break;
	}
    }

  return h;
}


/* Answer the height of a window.  If it hasn't been created yet,
   answer what the height of a window would be based upon its type and
   the layout.  */
int
tui_default_win_viewport_height (enum tui_win_type type,
				 enum tui_layout_type layout)
{
  int h;

  h = tui_default_win_height (type, layout);

  if (tui_win_list[type] == TUI_CMD_WIN)
    h -= 1;
  else
    h -= 2;

  return h;
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


/*************************
** STATIC LOCAL FUNCTIONS
**************************/


/* Function to set the layout to SRC, ASM, SPLIT, NEXT, PREV, DATA, or
   REGS. */
enum tui_status
tui_set_layout_by_name (const char *layout_name)
{
  enum tui_status status = TUI_SUCCESS;

  if (layout_name != NULL)
    {
      int i;
      enum tui_layout_type new_layout = UNDEFINED_LAYOUT;
      enum tui_layout_type cur_layout = tui_current_layout ();

      std::string copy = layout_name;
      for (i = 0; i < copy.size (); i++)
	copy[i] = toupper (copy[i]);
      const char *buf_ptr = copy.c_str ();

      /* First check for ambiguous input.  */
      if (strlen (buf_ptr) <= 1 && *buf_ptr == 'S')
	{
	  warning (_("Ambiguous command input."));
	  status = TUI_FAILURE;
	}
      else
	{
	  if (subset_compare (buf_ptr, "SRC"))
	    new_layout = SRC_COMMAND;
	  else if (subset_compare (buf_ptr, "ASM"))
	    new_layout = DISASSEM_COMMAND;
	  else if (subset_compare (buf_ptr, "SPLIT"))
	    new_layout = SRC_DISASSEM_COMMAND;
	  else if (subset_compare (buf_ptr, "REGS"))
	    {
	      if (cur_layout == SRC_COMMAND
		  || cur_layout == SRC_DATA_COMMAND)
		new_layout = SRC_DATA_COMMAND;
	      else
		new_layout = DISASSEM_DATA_COMMAND;
	    }
	  else if (subset_compare (buf_ptr, "NEXT"))
	    new_layout = next_layout ();
	  else if (subset_compare (buf_ptr, "PREV"))
	    new_layout = prev_layout ();
	  else
	    status = TUI_FAILURE;

	  if (status == TUI_SUCCESS)
	    {
	      /* Make sure the curses mode is enabled.  */
	      tui_enable ();
	      tui_set_layout (new_layout);
	    }
	}
    }
  else
    status = TUI_FAILURE;

  return status;
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


static void
tui_layout_command (const char *arg, int from_tty)
{
  /* Switch to the selected layout.  */
  if (tui_set_layout_by_name (arg) != TUI_SUCCESS)
    warning (_("Invalid layout specified.\n%s"), LAYOUT_USAGE);
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



static struct tui_win_info *
make_command_window (int height, int origin_y)
{
  struct tui_win_info *result
    = (struct tui_win_info *) init_and_make_win (NULL,
						 CMD_WIN,
						 height,
						 tui_term_width (),
						 0,
						 origin_y,
						 DONT_BOX_WINDOW);
  return result;
}


/* make_source_window().
 */
static struct tui_win_info *
make_source_window (int height, int origin_y)
{
  return make_source_or_disasm_window (SRC_WIN, height, origin_y);
}				/* make_source_window */


/* make_disasm_window().
 */
static struct tui_win_info *
make_disasm_window (int height, int origin_y)
{
  return make_source_or_disasm_window (DISASSEM_WIN, height, origin_y);
}				/* make_disasm_window */


static void
make_data_window (struct tui_win_info **win_info_ptr, 
		  int height, int origin_y)
{
  *win_info_ptr
    = (struct tui_win_info *) init_and_make_win (*win_info_ptr,
						 DATA_WIN,
						 height,
						 tui_term_width (),
						 0,
						 origin_y,
						 BOX_WINDOW);
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
  if (tui_current_layout () != SRC_DISASSEM_COMMAND)
    {
      int cmd_height, src_height, asm_height;

      if (TUI_CMD_WIN != NULL)
	cmd_height = TUI_CMD_WIN->height;
      else
	cmd_height = tui_term_height () / 3;

      src_height = (tui_term_height () - cmd_height) / 2;
      asm_height = tui_term_height () - (src_height + cmd_height);

      if (TUI_SRC_WIN == NULL)
	tui_win_list[SRC_WIN] = make_source_window (src_height, 0);
      else
	{
	  TUI_SRC_WIN->reset (TUI_SRC_WIN->type,
			      src_height,
			      TUI_SRC_WIN->width,
			      TUI_SRC_WIN->execution_info->width,
			      0);
	  TUI_SRC_WIN->execution_info->reset (EXEC_INFO_WIN,
					      src_height,
					      3,
					      0,
					      0);
	  tui_make_visible (TUI_SRC_WIN);
	  tui_make_visible (TUI_SRC_WIN->execution_info);
	  TUI_SRC_WIN->m_has_locator = false;
	}

      struct tui_locator_window *locator = tui_locator_win_info_ptr ();
      gdb_assert (locator != nullptr);

      tui_show_source_content (TUI_SRC_WIN);
      if (TUI_DISASM_WIN == NULL)
	{
	  tui_win_list[DISASSEM_WIN]
	    = make_disasm_window (asm_height, src_height - 1);
	  init_and_make_win (locator,
			     LOCATOR_WIN,
			     2 /* 1 */ ,
			     tui_term_width (),
			     0,
			     (src_height + asm_height) - 1,
			     DONT_BOX_WINDOW);
	}
      else
	{
	  locator->reset (LOCATOR_WIN,
			  2 /* 1 */ ,
			  tui_term_width (),
			  0,
			  (src_height + asm_height) - 1);
	  TUI_DISASM_WIN->m_has_locator = true;
	  TUI_DISASM_WIN->reset (TUI_DISASM_WIN->type,
				 asm_height,
				 TUI_DISASM_WIN->width,
				 TUI_DISASM_WIN->execution_info->width,
				 src_height - 1);
	  TUI_DISASM_WIN->execution_info->reset (EXEC_INFO_WIN,
						 asm_height,
						 3,
						 0,
						 src_height - 1);
	  tui_make_visible (TUI_DISASM_WIN);
	  tui_make_visible (TUI_DISASM_WIN->execution_info);
	}
      TUI_SRC_WIN->m_has_locator = false;
      TUI_DISASM_WIN->m_has_locator = true;
      tui_make_visible (locator);
      tui_show_locator_content ();
      tui_show_source_content (TUI_DISASM_WIN);

      if (TUI_CMD_WIN == NULL)
	tui_win_list[CMD_WIN]
	  = make_command_window (cmd_height, tui_term_height () - cmd_height);
      else
	{
	  TUI_CMD_WIN->reset (TUI_CMD_WIN->type,
			      TUI_CMD_WIN->height,
			      TUI_CMD_WIN->width,
			      0,
			      TUI_CMD_WIN->origin.y);
	  tui_make_visible (TUI_CMD_WIN);
	}
      TUI_CMD_WIN->refresh_window ();
      tui_set_current_layout_to (SRC_DISASSEM_COMMAND);
    }
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
  tui_make_all_invisible ();
  tui_make_invisible (locator);
  make_data_window (&tui_win_list[DATA_WIN], data_height, 0);
  if (new_layout == SRC_DATA_COMMAND)
    win_type = SRC_WIN;
  else
    win_type = DISASSEM_WIN;

  tui_source_window_base *base;
  if (tui_win_list[win_type] == NULL)
    {
      if (win_type == SRC_WIN)
	tui_win_list[win_type]
	  = make_source_window (src_height, data_height - 1);
      else
	tui_win_list[win_type]
	  = make_disasm_window (src_height, data_height - 1);
      init_and_make_win (locator,
			     LOCATOR_WIN,
			     2 /* 1 */ ,
			     tui_term_width (),
			     0,
			     total_height - 1,
			     DONT_BOX_WINDOW);
      base = (tui_source_window_base *) tui_win_list[win_type];
    }
  else
    {
      base = (tui_source_window_base *) tui_win_list[win_type];
      tui_win_list[win_type]->reset (tui_win_list[win_type]->type,
				     src_height,
				     tui_win_list[win_type]->width,
				     base->execution_info->width,
				     data_height - 1);
      base->execution_info->reset (EXEC_INFO_WIN,
				   src_height,
				   3,
				   0,
				   data_height - 1);
      tui_make_visible (tui_win_list[win_type]);
      tui_make_visible (base->execution_info);
      locator->reset (LOCATOR_WIN,
		      2 /* 1 */ ,
		      tui_term_width (),
		      0,
		      total_height - 1);
    }
  base->m_has_locator = true;
  tui_make_visible (locator);
  tui_show_locator_content ();
  tui_add_to_source_windows
    ((tui_source_window_base *) tui_win_list[win_type]);
  tui_set_current_layout_to (new_layout);
}

void
tui_gen_win_info::reset (enum tui_win_type win_type,
			 int height_, int width_,
			 int origin_x_, int origin_y_)
{
  int h = height_;

  gdb_assert (type == win_type);

  width = width_;
  height = h;
  if (h > 1)
    {
      viewport_height = h - 1;
      if (type != CMD_WIN)
	viewport_height--;
    }
  else
    viewport_height = 1;
  origin.x = origin_x_;
  origin.y = origin_y_;
}

/* init_and_make_win().
 */
static tui_gen_win_info *
init_and_make_win (tui_gen_win_info *win_info, 
		   enum tui_win_type win_type,
		   int height, int width, 
		   int origin_x, int origin_y,
		   enum tui_box box_it)
{
  if (win_info == NULL)
    {
      switch (win_type)
	{
	case SRC_WIN:
	  win_info = new tui_source_window ();
	  break;

	case DISASSEM_WIN:
	  win_info = new tui_disasm_window ();
	  break;

	case DATA_WIN:
	  win_info = new tui_data_window ();
	  break;

	case CMD_WIN:
	  win_info = new tui_cmd_window ();
	  break;

	case EXEC_INFO_WIN:
	  win_info = new tui_exec_info_window ();
	  break;

	default:
	  gdb_assert_not_reached (_("unhandled window type"));
	}
    }

  win_info->reset (win_type, height, width, origin_x, origin_y);
  tui_make_window (win_info, box_it);

  return win_info;
}


static struct tui_win_info *
make_source_or_disasm_window (enum tui_win_type type,
			      int height, int origin_y)
{
  struct tui_exec_info_window *execution_info
    = (tui_exec_info_window *) init_and_make_win (nullptr,
						  EXEC_INFO_WIN,
						  height,
						  3,
						  0,
						  origin_y,
						  DONT_BOX_WINDOW);

  /* Now create the source window.  */
  struct tui_source_window_base *result
    = ((struct tui_source_window_base *)
       init_and_make_win (NULL,
			  type,
			  height,
			  tui_term_width () - execution_info->width,
			  execution_info->width,
			  origin_y,
			  BOX_WINDOW));
  result->execution_info = execution_info;
  return result;
}


/* Show the Source/Command or the Disassem layout.  */
static void
show_source_or_disasm_and_command (enum tui_layout_type layout_type)
{
  if (tui_current_layout () != layout_type)
    {
      struct tui_win_info **win_info_ptr;
      int src_height, cmd_height;
      struct tui_locator_window *locator = tui_locator_win_info_ptr ();
      gdb_assert (locator != nullptr);

      if (TUI_CMD_WIN != NULL)
	cmd_height = TUI_CMD_WIN->height;
      else
	cmd_height = tui_term_height () / 3;
      src_height = tui_term_height () - cmd_height;

      if (layout_type == SRC_COMMAND)
	win_info_ptr = &tui_win_list[SRC_WIN];
      else
	win_info_ptr = &tui_win_list[DISASSEM_WIN];

      tui_source_window_base *base;
      if ((*win_info_ptr) == NULL)
	{
	  if (layout_type == SRC_COMMAND)
	    *win_info_ptr = make_source_window (src_height - 1, 0);
	  else
	    *win_info_ptr = make_disasm_window (src_height - 1, 0);
	  init_and_make_win (locator,
			     LOCATOR_WIN,
			     2 /* 1 */ ,
			     tui_term_width (),
			     0,
			     src_height - 1,
			     DONT_BOX_WINDOW);
	  base = (tui_source_window_base *) *win_info_ptr;
	}
      else
	{
	  base = (tui_source_window_base *) *win_info_ptr;
	  locator->reset (LOCATOR_WIN,
			  2 /* 1 */ ,
			  tui_term_width (),
			  0,
			  src_height - 1);
	  base->m_has_locator = true;
	  (*win_info_ptr)->reset ((*win_info_ptr)->type,
				  src_height - 1,
				  (*win_info_ptr)->width,
				  base->execution_info->width,
				  0);
	  base->execution_info->reset (EXEC_INFO_WIN,
				       src_height - 1,
				       3,
				       0,
				       0);
	  tui_make_visible (*win_info_ptr);
	  tui_make_visible (base->execution_info);
	}

      base->m_has_locator = true;
      tui_make_visible (locator);
      tui_show_locator_content ();
      tui_show_source_content (base);

      if (TUI_CMD_WIN == NULL)
	{
	  tui_win_list[CMD_WIN] = make_command_window (cmd_height,
						       src_height);
	  TUI_CMD_WIN->refresh_window ();
	}
      else
	{
	  TUI_CMD_WIN->reset (TUI_CMD_WIN->type,
			      TUI_CMD_WIN->height,
			      TUI_CMD_WIN->width,
			      TUI_CMD_WIN->origin.x,
			      TUI_CMD_WIN->origin.y);
	  tui_make_visible (TUI_CMD_WIN);
	}
      tui_set_current_layout_to (layout_type);
    }
}
