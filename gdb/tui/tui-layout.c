/* TUI layout window management.

   Copyright 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free Software
   Foundation, Inc.

   Contributed by Hewlett-Packard Company.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
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

#ifdef HAVE_NCURSES_H       
#include <ncurses.h>
#else
#ifdef HAVE_CURSES_H
#include <curses.h>
#endif
#endif

/*******************************
** Static Local Decls
********************************/
static void showLayout (enum tui_layout_type);
static void _initGenWinInfo (struct tui_gen_win_info *, enum tui_win_type, int, int, int, int);
static void _initAndMakeWin (void **, enum tui_win_type, int, int, int, int, int);
static void _showSourceOrDisassemAndCommand (enum tui_layout_type);
static void _makeSourceOrDisassemWindow (struct tui_win_info * *, enum tui_win_type, int, int);
static void _makeCommandWindow (struct tui_win_info * *, int, int);
static void _makeSourceWindow (struct tui_win_info * *, int, int);
static void _makeDisassemWindow (struct tui_win_info * *, int, int);
static void _makeDataWindow (struct tui_win_info * *, int, int);
static void _showSourceCommand (void);
static void _showDisassemCommand (void);
static void _showSourceDisassemCommand (void);
static void _showData (enum tui_layout_type);
static enum tui_layout_type _nextLayout (void);
static enum tui_layout_type _prevLayout (void);
static void _tuiLayout_command (char *, int);
static void _tuiToggleLayout_command (char *, int);
static void _tuiToggleSplitLayout_command (char *, int);
static CORE_ADDR _extractDisplayStartAddr (void);
static void _tuiHandleXDBLayout (struct tui_layout_def *);


/***************************************
** DEFINITIONS
***************************************/

#define LAYOUT_USAGE     "Usage: layout prev | next | <layout_name> \n"

/* Show the screen layout defined.  */
static void
showLayout (enum tui_layout_type layout)
{
  enum tui_layout_type curLayout = tui_current_layout ();

  if (layout != curLayout)
    {
      /*
         ** Since the new layout may cause changes in window size, we
         ** should free the content and reallocate on next display of
         ** source/asm
       */
      tui_free_all_source_wins_content ();
      tui_clear_source_windows ();
      if (layout == SRC_DATA_COMMAND || layout == DISASSEM_DATA_COMMAND)
	{
	  _showData (layout);
	  tui_refresh_all (tui_win_list);
	}
      else
	{
	  /* First make the current layout be invisible */
	  tui_make_all_invisible ();
	  tui_make_invisible (tui_locator_win_info_ptr ());

	  switch (layout)
	    {
	      /* Now show the new layout */
	    case SRC_COMMAND:
	      _showSourceCommand ();
	      tui_add_to_source_windows (TUI_SRC_WIN);
	      break;
	    case DISASSEM_COMMAND:
	      _showDisassemCommand ();
	      tui_add_to_source_windows (TUI_DISASM_WIN);
	      break;
	    case SRC_DISASSEM_COMMAND:
	      _showSourceDisassemCommand ();
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
   SRC_DISASSEM_COMMAND, SRC_DATA_COMMAND, or DISASSEM_DATA_COMMAND.
   If the layout is SRC_DATA_COMMAND, DISASSEM_DATA_COMMAND, or
   UNDEFINED_LAYOUT, then the data window is populated according to
   regs_display_type.  */
enum tui_status
tui_set_layout (enum tui_layout_type layoutType,
		enum tui_register_display_type regs_display_type)
{
  enum tui_status status = TUI_SUCCESS;

  if (layoutType != UNDEFINED_LAYOUT || regs_display_type != TUI_UNDEFINED_REGS)
    {
      enum tui_layout_type curLayout = tui_current_layout (), newLayout = UNDEFINED_LAYOUT;
      int regsPopulate = FALSE;
      CORE_ADDR addr = _extractDisplayStartAddr ();
      struct tui_win_info * newWinWithFocus = (struct tui_win_info *) NULL;
      struct tui_win_info * winWithFocus = tui_win_with_focus ();
      struct tui_layout_def * layoutDef = tui_layout_def ();


      if (layoutType == UNDEFINED_LAYOUT &&
	  regs_display_type != TUI_UNDEFINED_REGS)
	{
	  if (curLayout == SRC_DISASSEM_COMMAND)
	    newLayout = DISASSEM_DATA_COMMAND;
	  else if (curLayout == SRC_COMMAND || curLayout == SRC_DATA_COMMAND)
	    newLayout = SRC_DATA_COMMAND;
	  else if (curLayout == DISASSEM_COMMAND ||
		   curLayout == DISASSEM_DATA_COMMAND)
	    newLayout = DISASSEM_DATA_COMMAND;
	}
      else
	newLayout = layoutType;

      regsPopulate = (newLayout == SRC_DATA_COMMAND ||
		      newLayout == DISASSEM_DATA_COMMAND ||
		      regs_display_type != TUI_UNDEFINED_REGS);
      if (newLayout != curLayout || regs_display_type != TUI_UNDEFINED_REGS)
	{
	  if (newLayout != curLayout)
	    {
	      showLayout (newLayout);
	      /*
	         ** Now determine where focus should be
	       */
	      if (winWithFocus != TUI_CMD_WIN)
		{
		  switch (newLayout)
		    {
		    case SRC_COMMAND:
		      tui_set_win_focus_to (TUI_SRC_WIN);
		      layoutDef->display_mode = SRC_WIN;
		      layoutDef->split = FALSE;
		      break;
		    case DISASSEM_COMMAND:
		      /* the previous layout was not showing
		         ** code. this can happen if there is no
		         ** source available:
		         ** 1. if the source file is in another dir OR
		         ** 2. if target was compiled without -g
		         ** We still want to show the assembly though!
		       */
		      addr = tui_get_begin_asm_address ();
		      tui_set_win_focus_to (TUI_DISASM_WIN);
		      layoutDef->display_mode = DISASSEM_WIN;
		      layoutDef->split = FALSE;
		      break;
		    case SRC_DISASSEM_COMMAND:
		      /* the previous layout was not showing
		         ** code. this can happen if there is no
		         ** source available:
		         ** 1. if the source file is in another dir OR
		         ** 2. if target was compiled without -g
		         ** We still want to show the assembly though!
		       */
		      addr = tui_get_begin_asm_address ();
		      if (winWithFocus == TUI_SRC_WIN)
			tui_set_win_focus_to (TUI_SRC_WIN);
		      else
			tui_set_win_focus_to (TUI_DISASM_WIN);
		      layoutDef->split = TRUE;
		      break;
		    case SRC_DATA_COMMAND:
		      if (winWithFocus != TUI_DATA_WIN)
			tui_set_win_focus_to (TUI_SRC_WIN);
		      else
			tui_set_win_focus_to (TUI_DATA_WIN);
		      layoutDef->display_mode = SRC_WIN;
		      layoutDef->split = FALSE;
		      break;
		    case DISASSEM_DATA_COMMAND:
		      /* the previous layout was not showing
		         ** code. this can happen if there is no
		         ** source available:
		         ** 1. if the source file is in another dir OR
		         ** 2. if target was compiled without -g
		         ** We still want to show the assembly though!
		       */
		      addr = tui_get_begin_asm_address ();
		      if (winWithFocus != TUI_DATA_WIN)
			tui_set_win_focus_to (TUI_DISASM_WIN);
		      else
			tui_set_win_focus_to (TUI_DATA_WIN);
		      layoutDef->display_mode = DISASSEM_WIN;
		      layoutDef->split = FALSE;
		      break;
		    default:
		      break;
		    }
		}
	      if (newWinWithFocus != (struct tui_win_info *) NULL)
		tui_set_win_focus_to (newWinWithFocus);
	      /*
	         ** Now update the window content
	       */
	      if (!regsPopulate &&
		  (newLayout == SRC_DATA_COMMAND ||
		   newLayout == DISASSEM_DATA_COMMAND))
		tui_display_all_data ();

	      tui_update_source_windows_with_addr (addr);
	    }
	  if (regsPopulate)
	    {
	      layoutDef->regs_display_type =
		(regs_display_type == TUI_UNDEFINED_REGS ?
		 TUI_GENERAL_REGS : regs_display_type);
	      tui_show_registers (layoutDef->regs_display_type);
	    }
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
  enum tui_layout_type curLayout = tui_current_layout ();

  switch (type)
    {
    case SRC_WIN:
      if (curLayout != SRC_COMMAND &&
	  curLayout != SRC_DISASSEM_COMMAND &&
	  curLayout != SRC_DATA_COMMAND)
	{
	  tui_clear_source_windows_detail ();
	  if (curLayout == DISASSEM_DATA_COMMAND)
	    showLayout (SRC_DATA_COMMAND);
	  else
	    showLayout (SRC_COMMAND);
	}
      break;
    case DISASSEM_WIN:
      if (curLayout != DISASSEM_COMMAND &&
	  curLayout != SRC_DISASSEM_COMMAND &&
	  curLayout != DISASSEM_DATA_COMMAND)
	{
	  tui_clear_source_windows_detail ();
	  if (curLayout == SRC_DATA_COMMAND)
	    showLayout (DISASSEM_DATA_COMMAND);
	  else
	    showLayout (DISASSEM_COMMAND);
	}
      break;
    case DATA_WIN:
      if (curLayout != SRC_DATA_COMMAND &&
	  curLayout != DISASSEM_DATA_COMMAND)
	{
	  if (curLayout == DISASSEM_COMMAND)
	    showLayout (DISASSEM_DATA_COMMAND);
	  else
	    showLayout (SRC_DATA_COMMAND);
	}
      break;
    default:
      break;
    }

  return;
}				/* tuiAddWinToLayout */


/*
   ** tuiDefaultWinHeight().
   **        Answer the height of a window.  If it hasn't been created yet,
   **        answer what the height of a window would be based upon its
   **        type and the layout.
 */
int
tuiDefaultWinHeight (enum tui_win_type type, enum tui_layout_type layout)
{
  int h;

  if (tui_win_list[type] != (struct tui_win_info *) NULL)
    h = tui_win_list[type]->generic.height;
  else
    {
      switch (layout)
	{
	case SRC_COMMAND:
	case DISASSEM_COMMAND:
	  if (TUI_CMD_WIN == NULL)
	    h = tui_term_height () / 2;
	  else
	    h = tui_term_height () - TUI_CMD_WIN->generic.height;
	  break;
	case SRC_DISASSEM_COMMAND:
	case SRC_DATA_COMMAND:
	case DISASSEM_DATA_COMMAND:
	  if (TUI_CMD_WIN == NULL)
	    h = tui_term_height () / 3;
	  else
	    h = (tui_term_height () - TUI_CMD_WIN->generic.height) / 2;
	  break;
	default:
	  h = 0;
	  break;
	}
    }

  return h;
}				/* tuiDefaultWinHeight */


/* Answer the height of a window.  If it hasn't been created yet,
   answer what the height of a window would be based upon its type and
   the layout.  */
int
tui_default_win_viewport_height (enum tui_win_type type,
				 enum tui_layout_type layout)
{
  int h;

  h = tuiDefaultWinHeight (type, layout);

  if (tui_win_list[type] == TUI_CMD_WIN)
    h -= 1;
  else
    h -= 2;

  return h;
}				/* tuiDefaultWinViewportHeight */


/*
   ** _initialize_tuiLayout().
   **        Function to initialize gdb commands, for tui window layout
   **        manipulation.
 */
void
_initialize_tuiLayout (void)
{
  add_com ("layout", class_tui, _tuiLayout_command,
           "Change the layout of windows.\n\
Usage: layout prev | next | <layout_name> \n\
Layout names are:\n\
   src   : Displays source and command windows.\n\
   asm   : Displays disassembly and command windows.\n\
   split : Displays source, disassembly and command windows.\n\
   regs  : Displays register window. If existing layout\n\
           is source/command or assembly/command, the \n\
           register window is displayed. If the\n\
           source/assembly/command (split) is displayed, \n\
           the register window is displayed with \n\
           the window that has current logical focus.\n");
  if (xdb_commands)
    {
      add_com ("td", class_tui, _tuiToggleLayout_command,
               "Toggle between Source/Command and Disassembly/Command layouts.\n");
      add_com ("ts", class_tui, _tuiToggleSplitLayout_command,
               "Toggle between Source/Command or Disassembly/Command and \n\
Source/Disassembly/Command layouts.\n");
    }
}


/*************************
** STATIC LOCAL FUNCTIONS
**************************/


/*
   ** _tuiSetLayoutTo()
   **    Function to set the layout to SRC, ASM, SPLIT, NEXT, PREV, DATA, REGS,
   **        $REGS, $GREGS, $FREGS, $SREGS.
 */
enum tui_status
tui_set_layout_for_display_command (const char *layoutName)
{
  enum tui_status status = TUI_SUCCESS;

  if (layoutName != (char *) NULL)
    {
      register int i;
      register char *bufPtr;
      enum tui_layout_type newLayout = UNDEFINED_LAYOUT;
      enum tui_register_display_type dpyType = TUI_UNDEFINED_REGS;
      enum tui_layout_type curLayout = tui_current_layout ();

      bufPtr = (char *) xstrdup (layoutName);
      for (i = 0; (i < strlen (layoutName)); i++)
	bufPtr[i] = toupper (bufPtr[i]);

      /* First check for ambiguous input */
      if (strlen (bufPtr) <= 1 && (*bufPtr == 'S' || *bufPtr == '$'))
	{
	  warning ("Ambiguous command input.\n");
	  status = TUI_FAILURE;
	}
      else
	{
	  if (subset_compare (bufPtr, "SRC"))
	    newLayout = SRC_COMMAND;
	  else if (subset_compare (bufPtr, "ASM"))
	    newLayout = DISASSEM_COMMAND;
	  else if (subset_compare (bufPtr, "SPLIT"))
	    newLayout = SRC_DISASSEM_COMMAND;
	  else if (subset_compare (bufPtr, "REGS") ||
		   subset_compare (bufPtr, TUI_GENERAL_SPECIAL_REGS_NAME) ||
		   subset_compare (bufPtr, TUI_GENERAL_REGS_NAME) ||
		   subset_compare (bufPtr, TUI_FLOAT_REGS_NAME) ||
		   subset_compare (bufPtr, TUI_SPECIAL_REGS_NAME))
	    {
	      if (curLayout == SRC_COMMAND || curLayout == SRC_DATA_COMMAND)
		newLayout = SRC_DATA_COMMAND;
	      else
		newLayout = DISASSEM_DATA_COMMAND;

/* could ifdef out the following code. when compile with -z, there are null 
   pointer references that cause a core dump if 'layout regs' is the first 
   layout command issued by the user. HP has asked us to hook up this code 
   - edie epstein
 */
	      if (subset_compare (bufPtr, TUI_FLOAT_REGS_NAME))
		{
		  if (TUI_DATA_WIN->detail.data_display_info.regs_display_type !=
		      TUI_SFLOAT_REGS &&
		      TUI_DATA_WIN->detail.data_display_info.regs_display_type !=
		      TUI_DFLOAT_REGS)
		    dpyType = TUI_SFLOAT_REGS;
		  else
		    dpyType =
		      TUI_DATA_WIN->detail.data_display_info.regs_display_type;
		}
	      else if (subset_compare (bufPtr,
				      TUI_GENERAL_SPECIAL_REGS_NAME))
		dpyType = TUI_GENERAL_AND_SPECIAL_REGS;
	      else if (subset_compare (bufPtr, TUI_GENERAL_REGS_NAME))
		dpyType = TUI_GENERAL_REGS;
	      else if (subset_compare (bufPtr, TUI_SPECIAL_REGS_NAME))
		dpyType = TUI_SPECIAL_REGS;
	      else if (TUI_DATA_WIN)
		{
		  if (TUI_DATA_WIN->detail.data_display_info.regs_display_type !=
		      TUI_UNDEFINED_REGS)
		    dpyType =
		      TUI_DATA_WIN->detail.data_display_info.regs_display_type;
		  else
		    dpyType = TUI_GENERAL_REGS;
		}

/* end of potential ifdef 
 */

/* if ifdefed out code above, then assume that the user wishes to display the 
   general purpose registers 
 */

/*              dpyType = TUI_GENERAL_REGS; 
 */
	    }
	  else if (subset_compare (bufPtr, "NEXT"))
	    newLayout = _nextLayout ();
	  else if (subset_compare (bufPtr, "PREV"))
	    newLayout = _prevLayout ();
	  else
	    status = TUI_FAILURE;
	  xfree (bufPtr);

	  tui_set_layout (newLayout, dpyType);
	}
    }
  else
    status = TUI_FAILURE;

  return status;
}


static CORE_ADDR
_extractDisplayStartAddr (void)
{
  enum tui_layout_type curLayout = tui_current_layout ();
  CORE_ADDR addr;
  CORE_ADDR pc;
  struct symtab_and_line cursal = get_current_source_symtab_and_line ();

  switch (curLayout)
    {
    case SRC_COMMAND:
    case SRC_DATA_COMMAND:
      find_line_pc (cursal.symtab,
		    TUI_SRC_WIN->detail.source_info.start_line_or_addr.line_no,
		    &pc);
      addr = pc;
      break;
    case DISASSEM_COMMAND:
    case SRC_DISASSEM_COMMAND:
    case DISASSEM_DATA_COMMAND:
      addr = TUI_DISASM_WIN->detail.source_info.start_line_or_addr.addr;
      break;
    default:
      addr = 0;
      break;
    }

  return addr;
}				/* _extractDisplayStartAddr */


static void
_tuiHandleXDBLayout (struct tui_layout_def * layoutDef)
{
  if (layoutDef->split)
    {
      tui_set_layout (SRC_DISASSEM_COMMAND, TUI_UNDEFINED_REGS);
      tui_set_win_focus_to (tui_win_list[layoutDef->display_mode]);
    }
  else
    {
      if (layoutDef->display_mode == SRC_WIN)
	tui_set_layout (SRC_COMMAND, TUI_UNDEFINED_REGS);
      else
	tui_set_layout (DISASSEM_DATA_COMMAND, layoutDef->regs_display_type);
    }


  return;
}				/* _tuiHandleXDBLayout */


static void
_tuiToggleLayout_command (char *arg, int fromTTY)
{
  struct tui_layout_def * layoutDef = tui_layout_def ();

  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  if (layoutDef->display_mode == SRC_WIN)
    layoutDef->display_mode = DISASSEM_WIN;
  else
    layoutDef->display_mode = SRC_WIN;

  if (!layoutDef->split)
    _tuiHandleXDBLayout (layoutDef);

}


static void
_tuiToggleSplitLayout_command (char *arg, int fromTTY)
{
  struct tui_layout_def * layoutDef = tui_layout_def ();

  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  layoutDef->split = (!layoutDef->split);
  _tuiHandleXDBLayout (layoutDef);

}


static void
_tuiLayout_command (char *arg, int fromTTY)
{
  /* Make sure the curses mode is enabled.  */
  tui_enable ();

  /* Switch to the selected layout.  */
  if (tui_set_layout_for_display_command (arg) != TUI_SUCCESS)
    warning ("Invalid layout specified.\n%s", LAYOUT_USAGE);

}

/*
   ** _nextLayout().
   **        Answer the previous layout to cycle to.
 */
static enum tui_layout_type
_nextLayout (void)
{
  enum tui_layout_type newLayout;

  newLayout = tui_current_layout ();
  if (newLayout == UNDEFINED_LAYOUT)
    newLayout = SRC_COMMAND;
  else
    {
      newLayout++;
      if (newLayout == UNDEFINED_LAYOUT)
	newLayout = SRC_COMMAND;
    }

  return newLayout;
}				/* _nextLayout */


/*
   ** _prevLayout().
   **        Answer the next layout to cycle to.
 */
static enum tui_layout_type
_prevLayout (void)
{
  enum tui_layout_type newLayout;

  newLayout = tui_current_layout ();
  if (newLayout == SRC_COMMAND)
    newLayout = DISASSEM_DATA_COMMAND;
  else
    {
      newLayout--;
      if (newLayout == UNDEFINED_LAYOUT)
	newLayout = DISASSEM_DATA_COMMAND;
    }

  return newLayout;
}				/* _prevLayout */



/*
   ** _makeCommandWindow().
 */
static void
_makeCommandWindow (struct tui_win_info * * winInfoPtr, int height, int originY)
{
  _initAndMakeWin ((void **) winInfoPtr,
		   CMD_WIN,
		   height,
		   tui_term_width (),
		   0,
		   originY,
		   DONT_BOX_WINDOW);

  (*winInfoPtr)->can_highlight = FALSE;

  return;
}				/* _makeCommandWindow */


/*
   ** _makeSourceWindow().
 */
static void
_makeSourceWindow (struct tui_win_info * * winInfoPtr, int height, int originY)
{
  _makeSourceOrDisassemWindow (winInfoPtr, SRC_WIN, height, originY);

  return;
}				/* _makeSourceWindow */


/*
   ** _makeDisassemWindow().
 */
static void
_makeDisassemWindow (struct tui_win_info * * winInfoPtr, int height, int originY)
{
  _makeSourceOrDisassemWindow (winInfoPtr, DISASSEM_WIN, height, originY);

  return;
}				/* _makeDisassemWindow */


/*
   ** _makeDataWindow().
 */
static void
_makeDataWindow (struct tui_win_info * * winInfoPtr, int height, int originY)
{
  _initAndMakeWin ((void **) winInfoPtr,
		   DATA_WIN,
		   height,
		   tui_term_width (),
		   0,
		   originY,
		   BOX_WINDOW);

  return;
}				/* _makeDataWindow */



/*
   **    _showSourceCommand().
   **        Show the Source/Command layout
 */
static void
_showSourceCommand (void)
{
  _showSourceOrDisassemAndCommand (SRC_COMMAND);

  return;
}				/* _showSourceCommand */


/*
   **    _showDisassemCommand().
   **        Show the Dissassem/Command layout
 */
static void
_showDisassemCommand (void)
{
  _showSourceOrDisassemAndCommand (DISASSEM_COMMAND);

  return;
}				/* _showDisassemCommand */


/*
   **    _showSourceDisassemCommand().
   **        Show the Source/Disassem/Command layout
 */
static void
_showSourceDisassemCommand (void)
{
  if (tui_current_layout () != SRC_DISASSEM_COMMAND)
    {
      int cmdHeight, srcHeight, asmHeight;

      if (TUI_CMD_WIN != NULL)
	cmdHeight = TUI_CMD_WIN->generic.height;
      else
	cmdHeight = tui_term_height () / 3;

      srcHeight = (tui_term_height () - cmdHeight) / 2;
      asmHeight = tui_term_height () - (srcHeight + cmdHeight);

      if (TUI_SRC_WIN == NULL)
	_makeSourceWindow (&TUI_SRC_WIN, srcHeight, 0);
      else
	{
	  _initGenWinInfo (&TUI_SRC_WIN->generic,
			   TUI_SRC_WIN->generic.type,
			   srcHeight,
			   TUI_SRC_WIN->generic.width,
			   TUI_SRC_WIN->detail.source_info.execution_info->width,
			   0);
	  TUI_SRC_WIN->can_highlight = TRUE;
	  _initGenWinInfo (TUI_SRC_WIN->detail.source_info.execution_info,
			   EXEC_INFO_WIN,
			   srcHeight,
			   3,
			   0,
			   0);
	  tui_make_visible (&TUI_SRC_WIN->generic);
	  tui_make_visible (TUI_SRC_WIN->detail.source_info.execution_info);
	  TUI_SRC_WIN->detail.source_info.has_locator = FALSE;;
	}
      if (TUI_SRC_WIN != NULL)
	{
	  struct tui_gen_win_info * locator = tui_locator_win_info_ptr ();

	  tui_show_source_content (TUI_SRC_WIN);
	  if (TUI_DISASM_WIN == NULL)
	    {
	      _makeDisassemWindow (&TUI_DISASM_WIN, asmHeight, srcHeight - 1);
	      _initAndMakeWin ((void **) & locator,
			       LOCATOR_WIN,
			       2 /* 1 */ ,
			       tui_term_width (),
			       0,
			       (srcHeight + asmHeight) - 1,
			       DONT_BOX_WINDOW);
	    }
	  else
	    {
	      _initGenWinInfo (locator,
			       LOCATOR_WIN,
			       2 /* 1 */ ,
			       tui_term_width (),
			       0,
			       (srcHeight + asmHeight) - 1);
	      TUI_DISASM_WIN->detail.source_info.has_locator = TRUE;
	      _initGenWinInfo (
				&TUI_DISASM_WIN->generic,
				TUI_DISASM_WIN->generic.type,
				asmHeight,
				TUI_DISASM_WIN->generic.width,
			TUI_DISASM_WIN->detail.source_info.execution_info->width,
				srcHeight - 1);
	      _initGenWinInfo (TUI_DISASM_WIN->detail.source_info.execution_info,
			       EXEC_INFO_WIN,
			       asmHeight,
			       3,
			       0,
			       srcHeight - 1);
	      TUI_DISASM_WIN->can_highlight = TRUE;
	      tui_make_visible (&TUI_DISASM_WIN->generic);
	      tui_make_visible (TUI_DISASM_WIN->detail.source_info.execution_info);
	    }
	  if (TUI_DISASM_WIN != NULL)
	    {
	      TUI_SRC_WIN->detail.source_info.has_locator = FALSE;
	      TUI_DISASM_WIN->detail.source_info.has_locator = TRUE;
	      tui_make_visible (locator);
	      tui_show_locator_content ();
	      tui_show_source_content (TUI_DISASM_WIN);

	      if (TUI_CMD_WIN == NULL)
		_makeCommandWindow (&TUI_CMD_WIN,
				    cmdHeight,
				    tui_term_height () - cmdHeight);
	      else
		{
		  _initGenWinInfo (&TUI_CMD_WIN->generic,
				   TUI_CMD_WIN->generic.type,
				   TUI_CMD_WIN->generic.height,
				   TUI_CMD_WIN->generic.width,
				   0,
				   TUI_CMD_WIN->generic.origin.y);
		  TUI_CMD_WIN->can_highlight = FALSE;
		  tui_make_visible (&TUI_CMD_WIN->generic);
		}
	      if (TUI_CMD_WIN != NULL)
		tui_refresh_win (&TUI_CMD_WIN->generic);
	    }
	}
      tui_set_current_layout_to (SRC_DISASSEM_COMMAND);
    }

  return;
}				/* _showSourceDisassemCommand */


/*
   **    _showData().
   **        Show the Source/Data/Command or the Dissassembly/Data/Command layout
 */
static void
_showData (enum tui_layout_type newLayout)
{
  int totalHeight = (tui_term_height () - TUI_CMD_WIN->generic.height);
  int srcHeight, dataHeight;
  enum tui_win_type winType;
  struct tui_gen_win_info * locator = tui_locator_win_info_ptr ();


  dataHeight = totalHeight / 2;
  srcHeight = totalHeight - dataHeight;
  tui_make_all_invisible ();
  tui_make_invisible (locator);
  _makeDataWindow (&TUI_DATA_WIN, dataHeight, 0);
  TUI_DATA_WIN->can_highlight = TRUE;
  if (newLayout == SRC_DATA_COMMAND)
    winType = SRC_WIN;
  else
    winType = DISASSEM_WIN;
  if (tui_win_list[winType] == NULL)
    {
      if (winType == SRC_WIN)
	_makeSourceWindow (&tui_win_list[winType], srcHeight, dataHeight - 1);
      else
	_makeDisassemWindow (&tui_win_list[winType], srcHeight, dataHeight - 1);
      _initAndMakeWin ((void **) & locator,
		       LOCATOR_WIN,
		       2 /* 1 */ ,
		       tui_term_width (),
		       0,
		       totalHeight - 1,
		       DONT_BOX_WINDOW);
    }
  else
    {
      _initGenWinInfo (&tui_win_list[winType]->generic,
		       tui_win_list[winType]->generic.type,
		       srcHeight,
		       tui_win_list[winType]->generic.width,
		   tui_win_list[winType]->detail.source_info.execution_info->width,
		       dataHeight - 1);
      _initGenWinInfo (tui_win_list[winType]->detail.source_info.execution_info,
		       EXEC_INFO_WIN,
		       srcHeight,
		       3,
		       0,
		       dataHeight - 1);
      tui_make_visible (&tui_win_list[winType]->generic);
      tui_make_visible (tui_win_list[winType]->detail.source_info.execution_info);
      _initGenWinInfo (locator,
		       LOCATOR_WIN,
		       2 /* 1 */ ,
		       tui_term_width (),
		       0,
		       totalHeight - 1);
    }
  tui_win_list[winType]->detail.source_info.has_locator = TRUE;
  tui_make_visible (locator);
  tui_show_locator_content ();
  tui_add_to_source_windows (tui_win_list[winType]);
  tui_set_current_layout_to (newLayout);

  return;
}				/* _showData */

/*
   ** _initGenWinInfo().
 */
static void
_initGenWinInfo (struct tui_gen_win_info * winInfo, enum tui_win_type type,
                 int height, int width, int originX, int originY)
{
  int h = height;

  winInfo->type = type;
  winInfo->width = width;
  winInfo->height = h;
  if (h > 1)
    {
      winInfo->viewport_height = h - 1;
      if (winInfo->type != CMD_WIN)
	winInfo->viewport_height--;
    }
  else
    winInfo->viewport_height = 1;
  winInfo->origin.x = originX;
  winInfo->origin.y = originY;

  return;
}				/* _initGenWinInfo */

/*
   ** _initAndMakeWin().
 */
static void
_initAndMakeWin (void ** winInfoPtr, enum tui_win_type winType,
                 int height, int width, int originX, int originY, int boxIt)
{
  void *opaqueWinInfo = *winInfoPtr;
  struct tui_gen_win_info * generic;

  if (opaqueWinInfo == NULL)
    {
      if (tui_win_is_auxillary (winType))
	opaqueWinInfo = (void *) tui_alloc_generic_win_info ();
      else
	opaqueWinInfo = (void *) tui_alloc_win_info (winType);
    }
  if (tui_win_is_auxillary (winType))
    generic = (struct tui_gen_win_info *) opaqueWinInfo;
  else
    generic = &((struct tui_win_info *) opaqueWinInfo)->generic;

  if (opaqueWinInfo != NULL)
    {
      _initGenWinInfo (generic, winType, height, width, originX, originY);
      if (!tui_win_is_auxillary (winType))
	{
	  if (generic->type == CMD_WIN)
	    ((struct tui_win_info *) opaqueWinInfo)->can_highlight = FALSE;
	  else
	    ((struct tui_win_info *) opaqueWinInfo)->can_highlight = TRUE;
	}
      tui_make_window (generic, boxIt);
    }
  *winInfoPtr = opaqueWinInfo;
}


/*
   ** _makeSourceOrDisassemWindow().
 */
static void
_makeSourceOrDisassemWindow (struct tui_win_info * * winInfoPtr, enum tui_win_type type,
                             int height, int originY)
{
  struct tui_gen_win_info * execution_info = (struct tui_gen_win_info *) NULL;

  /*
     ** Create the exeuction info window.
   */
  if (type == SRC_WIN)
    execution_info = tui_source_exec_info_win_ptr ();
  else
    execution_info = tui_disassem_exec_info_win_ptr ();
  _initAndMakeWin ((void **) & execution_info,
		   EXEC_INFO_WIN,
		   height,
		   3,
		   0,
		   originY,
		   DONT_BOX_WINDOW);
  /*
     ** Now create the source window.
   */
  _initAndMakeWin ((void **) winInfoPtr,
		   type,
		   height,
		   tui_term_width () - execution_info->width,
		   execution_info->width,
		   originY,
		   BOX_WINDOW);

  (*winInfoPtr)->detail.source_info.execution_info = execution_info;

  return;
}				/* _makeSourceOrDisassemWindow */


/*
   **    _showSourceOrDisassemAndCommand().
   **        Show the Source/Command or the Disassem layout
 */
static void
_showSourceOrDisassemAndCommand (enum tui_layout_type layoutType)
{
  if (tui_current_layout () != layoutType)
    {
      struct tui_win_info * *winInfoPtr;
      int srcHeight, cmdHeight;
      struct tui_gen_win_info * locator = tui_locator_win_info_ptr ();

      if (TUI_CMD_WIN != NULL)
	cmdHeight = TUI_CMD_WIN->generic.height;
      else
	cmdHeight = tui_term_height () / 3;
      srcHeight = tui_term_height () - cmdHeight;


      if (layoutType == SRC_COMMAND)
	winInfoPtr = &TUI_SRC_WIN;
      else
	winInfoPtr = &TUI_DISASM_WIN;

      if ((*winInfoPtr) == NULL)
	{
	  if (layoutType == SRC_COMMAND)
	    _makeSourceWindow (winInfoPtr, srcHeight - 1, 0);
	  else
	    _makeDisassemWindow (winInfoPtr, srcHeight - 1, 0);
	  _initAndMakeWin ((void **) & locator,
			   LOCATOR_WIN,
			   2 /* 1 */ ,
			   tui_term_width (),
			   0,
			   srcHeight - 1,
			   DONT_BOX_WINDOW);
	}
      else
	{
	  _initGenWinInfo (locator,
			   LOCATOR_WIN,
			   2 /* 1 */ ,
			   tui_term_width (),
			   0,
			   srcHeight - 1);
	  (*winInfoPtr)->detail.source_info.has_locator = TRUE;
	  _initGenWinInfo (
			    &(*winInfoPtr)->generic,
			    (*winInfoPtr)->generic.type,
			    srcHeight - 1,
			    (*winInfoPtr)->generic.width,
		      (*winInfoPtr)->detail.source_info.execution_info->width,
			    0);
	  _initGenWinInfo ((*winInfoPtr)->detail.source_info.execution_info,
			   EXEC_INFO_WIN,
			   srcHeight - 1,
			   3,
			   0,
			   0);
	  (*winInfoPtr)->can_highlight = TRUE;
	  tui_make_visible (&(*winInfoPtr)->generic);
	  tui_make_visible ((*winInfoPtr)->detail.source_info.execution_info);
	}
      if ((*winInfoPtr) != NULL)
	{
	  (*winInfoPtr)->detail.source_info.has_locator = TRUE;
	  tui_make_visible (locator);
	  tui_show_locator_content ();
	  tui_show_source_content (*winInfoPtr);

	  if (TUI_CMD_WIN == NULL)
	    {
	      _makeCommandWindow (&TUI_CMD_WIN, cmdHeight, srcHeight);
	      tui_refresh_win (&TUI_CMD_WIN->generic);
	    }
	  else
	    {
	      _initGenWinInfo (&TUI_CMD_WIN->generic,
			       TUI_CMD_WIN->generic.type,
			       TUI_CMD_WIN->generic.height,
			       TUI_CMD_WIN->generic.width,
			       TUI_CMD_WIN->generic.origin.x,
			       TUI_CMD_WIN->generic.origin.y);
	      TUI_CMD_WIN->can_highlight = FALSE;
	      tui_make_visible (&TUI_CMD_WIN->generic);
	    }
	}
      tui_set_current_layout_to (layoutType);
    }

  return;
}				/* _showSourceOrDisassemAndCommand */
