/* TUI window generic functions.

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

/* This module contains procedures for handling tui window functions
   like resize, scrolling, scrolling, changing focus, etc.

   Author: Susan B. Macchia  */

#include "defs.h"
#include "command.h"
#include "symtab.h"
#include "breakpoint.h"
#include "frame.h"
#include "cli/cli-cmds.h"
#include "top.h"
#include "source.h"

#include "tui/tui.h"
#include "tui/tui-data.h"
#include "tui/tui-wingeneral.h"
#include "tui/tui-stack.h"
#include "tui/tui-regs.h"
#include "tui/tui-disasm.h"
#include "tui/tui-source.h"
#include "tui/tui-winsource.h"
#include "tui/tui-windata.h"

#ifdef HAVE_NCURSES_H       
#include <ncurses.h>
#else
#ifdef HAVE_CURSES_H
#include <curses.h>
#endif
#endif

#include <string.h>
#include <ctype.h>
#include <readline/readline.h>

/*******************************
** Static Local Decls
********************************/
static void _makeVisibleWithNewHeight (struct tui_win_info *);
static void _makeInvisibleAndSetNewHeight (struct tui_win_info *, int);
static enum tui_status _tuiAdjustWinHeights (struct tui_win_info *, int);
static int _newHeightOk (struct tui_win_info *, int);
static void _tuiSetTabWidth_command (char *, int);
static void _tuiRefreshAll_command (char *, int);
static void _tuiSetWinHeight_command (char *, int);
static void _tuiXDBsetWinHeight_command (char *, int);
static void _tuiAllWindowsInfo (char *, int);
static void _tuiSetFocus_command (char *, int);
static void _tuiScrollForward_command (char *, int);
static void _tuiScrollBackward_command (char *, int);
static void _tuiScrollLeft_command (char *, int);
static void _tuiScrollRight_command (char *, int);
static void _parseScrollingArgs (char *, struct tui_win_info * *, int *);


/***************************************
** DEFINITIONS
***************************************/
#define WIN_HEIGHT_USAGE      "Usage: winheight <win_name> [+ | -] <#lines>\n"
#define XDBWIN_HEIGHT_USAGE   "Usage: w <#lines>\n"
#define FOCUS_USAGE           "Usage: focus {<win> | next | prev}\n"

/***************************************
** PUBLIC FUNCTIONS
***************************************/

#ifndef ACS_LRCORNER
#  define ACS_LRCORNER '+'
#endif
#ifndef ACS_LLCORNER
#  define ACS_LLCORNER '+'
#endif
#ifndef ACS_ULCORNER
#  define ACS_ULCORNER '+'
#endif
#ifndef ACS_URCORNER
#  define ACS_URCORNER '+'
#endif
#ifndef ACS_HLINE
#  define ACS_HLINE '-'
#endif
#ifndef ACS_VLINE
#  define ACS_VLINE '|'
#endif

/* Possible values for tui-border-kind variable.  */
static const char *tui_border_kind_enums[] = {
  "space",
  "ascii",
  "acs",
  NULL
};

/* Possible values for tui-border-mode and tui-active-border-mode.  */
static const char *tui_border_mode_enums[] = {
  "normal",
  "standout",
  "reverse",
  "half",
  "half-standout",
  "bold",
  "bold-standout",
  NULL
};

struct tui_translate
{
  const char *name;
  int value;
};

/* Translation table for border-mode variables.
   The list of values must be terminated by a NULL.
   After the NULL value, an entry defines the default.  */
struct tui_translate tui_border_mode_translate[] = {
  { "normal",		A_NORMAL },
  { "standout",		A_STANDOUT },
  { "reverse",		A_REVERSE },
  { "half",		A_DIM },
  { "half-standout",	A_DIM | A_STANDOUT },
  { "bold",		A_BOLD },
  { "bold-standout",	A_BOLD | A_STANDOUT },
  { 0, 0 },
  { "normal",		A_NORMAL }
};

/* Translation tables for border-kind, one for each border
   character (see wborder, border curses operations).
   -1 is used to indicate the ACS because ACS characters
   are determined at run time by curses (depends on terminal).  */
struct tui_translate tui_border_kind_translate_vline[] = {
  { "space",    ' ' },
  { "ascii",    '|' },
  { "acs",      -1 },
  { 0, 0 },
  { "ascii",    '|' }
};

struct tui_translate tui_border_kind_translate_hline[] = {
  { "space",    ' ' },
  { "ascii",    '-' },
  { "acs",      -1 },
  { 0, 0 },
  { "ascii",    '-' }
};

struct tui_translate tui_border_kind_translate_ulcorner[] = {
  { "space",    ' ' },
  { "ascii",    '+' },
  { "acs",      -1 },
  { 0, 0 },
  { "ascii",    '+' }
};

struct tui_translate tui_border_kind_translate_urcorner[] = {
  { "space",    ' ' },
  { "ascii",    '+' },
  { "acs",      -1 },
  { 0, 0 },
  { "ascii",    '+' }
};

struct tui_translate tui_border_kind_translate_llcorner[] = {
  { "space",    ' ' },
  { "ascii",    '+' },
  { "acs",      -1 },
  { 0, 0 },
  { "ascii",    '+' }
};

struct tui_translate tui_border_kind_translate_lrcorner[] = {
  { "space",    ' ' },
  { "ascii",    '+' },
  { "acs",      -1 },
  { 0, 0 },
  { "ascii",    '+' }
};


/* Tui configuration variables controlled with set/show command.  */
const char *tui_active_border_mode = "bold-standout";
const char *tui_border_mode = "normal";
const char *tui_border_kind = "acs";

/* Tui internal configuration variables.  These variables are
   updated by tui_update_variables to reflect the tui configuration
   variables.  */
chtype tui_border_vline;
chtype tui_border_hline;
chtype tui_border_ulcorner;
chtype tui_border_urcorner;
chtype tui_border_llcorner;
chtype tui_border_lrcorner;

int tui_border_attrs;
int tui_active_border_attrs;

/* Identify the item in the translation table.
   When the item is not recognized, use the default entry.  */
static struct tui_translate *
translate (const char *name, struct tui_translate *table)
{
  while (table->name)
    {
      if (name && strcmp (table->name, name) == 0)
        return table;
      table++;
    }

  /* Not found, return default entry.  */
  table++;
  return table;
}

/* Update the tui internal configuration according to gdb settings.
   Returns 1 if the configuration has changed and the screen should
   be redrawn.  */
int
tui_update_variables ()
{
  int need_redraw = 0;
  struct tui_translate *entry;

  entry = translate (tui_border_mode, tui_border_mode_translate);
  if (tui_border_attrs != entry->value)
    {
      tui_border_attrs = entry->value;
      need_redraw = 1;
    }
  entry = translate (tui_active_border_mode, tui_border_mode_translate);
  if (tui_active_border_attrs != entry->value)
    {
      tui_active_border_attrs = entry->value;
      need_redraw = 1;
    }

  /* If one corner changes, all characters are changed.
     Only check the first one.  The ACS characters are determined at
     run time by curses terminal management.  */
  entry = translate (tui_border_kind, tui_border_kind_translate_lrcorner);
  if (tui_border_lrcorner != (chtype) entry->value)
    {
      tui_border_lrcorner = (entry->value < 0) ? ACS_LRCORNER : entry->value;
      need_redraw = 1;
    }
  entry = translate (tui_border_kind, tui_border_kind_translate_llcorner);
  tui_border_llcorner = (entry->value < 0) ? ACS_LLCORNER : entry->value;

  entry = translate (tui_border_kind, tui_border_kind_translate_ulcorner);
  tui_border_ulcorner = (entry->value < 0) ? ACS_ULCORNER : entry->value;

  entry = translate (tui_border_kind, tui_border_kind_translate_urcorner);
  tui_border_urcorner = (entry->value < 0) ? ACS_URCORNER : entry->value;

  entry = translate (tui_border_kind, tui_border_kind_translate_hline);
  tui_border_hline = (entry->value < 0) ? ACS_HLINE : entry->value;

  entry = translate (tui_border_kind, tui_border_kind_translate_vline);
  tui_border_vline = (entry->value < 0) ? ACS_VLINE : entry->value;

  return need_redraw;
}

static void
set_tui_cmd (char *args, int from_tty)
{
}

static void
show_tui_cmd (char *args, int from_tty)
{
}

/*
   ** _initialize_tuiWin().
   **        Function to initialize gdb commands, for tui window manipulation.
 */
void
_initialize_tuiWin (void)
{
  struct cmd_list_element *c;
  static struct cmd_list_element *tui_setlist;
  static struct cmd_list_element *tui_showlist;

  /* Define the classes of commands.
     They will appear in the help list in the reverse of this order.  */
  add_cmd ("tui", class_tui, NULL,
	   "Text User Interface commands.",
	   &cmdlist);

  add_prefix_cmd ("tui", class_tui, set_tui_cmd,
                  "TUI configuration variables",
		  &tui_setlist, "set tui ",
		  0/*allow-unknown*/, &setlist);
  add_prefix_cmd ("tui", class_tui, show_tui_cmd,
                  "TUI configuration variables",
		  &tui_showlist, "show tui ",
		  0/*allow-unknown*/, &showlist);

  add_com ("refresh", class_tui, _tuiRefreshAll_command,
           "Refresh the terminal display.\n");
  if (xdb_commands)
    add_com_alias ("U", "refresh", class_tui, 0);
  add_com ("tabset", class_tui, _tuiSetTabWidth_command,
           "Set the width (in characters) of tab stops.\n\
Usage: tabset <n>\n");
  add_com ("winheight", class_tui, _tuiSetWinHeight_command,
           "Set the height of a specified window.\n\
Usage: winheight <win_name> [+ | -] <#lines>\n\
Window names are:\n\
src  : the source window\n\
cmd  : the command window\n\
asm  : the disassembly window\n\
regs : the register display\n");
  add_com_alias ("wh", "winheight", class_tui, 0);
  add_info ("win", _tuiAllWindowsInfo,
            "List of all displayed windows.\n");
  add_com ("focus", class_tui, _tuiSetFocus_command,
           "Set focus to named window or next/prev window.\n\
Usage: focus {<win> | next | prev}\n\
Valid Window names are:\n\
src  : the source window\n\
asm  : the disassembly window\n\
regs : the register display\n\
cmd  : the command window\n");
  add_com_alias ("fs", "focus", class_tui, 0);
  add_com ("+", class_tui, _tuiScrollForward_command,
           "Scroll window forward.\nUsage: + [win] [n]\n");
  add_com ("-", class_tui, _tuiScrollBackward_command,
           "Scroll window backward.\nUsage: - [win] [n]\n");
  add_com ("<", class_tui, _tuiScrollLeft_command,
           "Scroll window forward.\nUsage: < [win] [n]\n");
  add_com (">", class_tui, _tuiScrollRight_command,
           "Scroll window backward.\nUsage: > [win] [n]\n");
  if (xdb_commands)
    add_com ("w", class_xdb, _tuiXDBsetWinHeight_command,
             "XDB compatibility command for setting the height of a command window.\n\
Usage: w <#lines>\n");

  /* Define the tui control variables.  */
  c = add_set_enum_cmd
    ("border-kind", no_class,
     tui_border_kind_enums, &tui_border_kind,
     "Set the kind of border for TUI windows.\n"
     "This variable controls the border of TUI windows:\n"
     "space           use a white space\n"
     "ascii           use ascii characters + - | for the border\n"
     "acs             use the Alternate Character Set\n",
     &tui_setlist);
  add_show_from_set (c, &tui_showlist);

  c = add_set_enum_cmd
    ("border-mode", no_class,
     tui_border_mode_enums, &tui_border_mode,
     "Set the attribute mode to use for the TUI window borders.\n"
     "This variable controls the attributes to use for the window borders:\n"
     "normal          normal display\n"
     "standout        use highlight mode of terminal\n"
     "reverse         use reverse video mode\n"
     "half            use half bright\n"
     "half-standout   use half bright and standout mode\n"
     "bold            use extra bright or bold\n"
     "bold-standout   use extra bright or bold with standout mode\n",
     &tui_setlist);
  add_show_from_set (c, &tui_showlist);

  c = add_set_enum_cmd
    ("active-border-mode", no_class,
     tui_border_mode_enums, &tui_active_border_mode,
     "Set the attribute mode to use for the active TUI window border.\n"
     "This variable controls the attributes to use for the active window border:\n"
     "normal          normal display\n"
     "standout        use highlight mode of terminal\n"
     "reverse         use reverse video mode\n"
     "half            use half bright\n"
     "half-standout   use half bright and standout mode\n"
     "bold            use extra bright or bold\n"
     "bold-standout   use extra bright or bold with standout mode\n",
     &tui_setlist);
  add_show_from_set (c, &tui_showlist);
}

/* Update gdb's knowledge of the terminal size.  */
void
tui_update_gdb_sizes ()
{
  char cmd[50];
  int screenheight, screenwidth;

  rl_get_screen_size (&screenheight, &screenwidth);
  /* Set to TUI command window dimension or use readline values.  */
  sprintf (cmd, "set width %d",
           tui_active ? TUI_CMD_WIN->generic.width : screenwidth);
  execute_command (cmd, 0);
  sprintf (cmd, "set height %d",
           tui_active ? TUI_CMD_WIN->generic.height : screenheight);
  execute_command (cmd, 0);
}


/* Set the logical focus to win_info.    */
void
tui_set_win_focus_to (struct tui_win_info * win_info)
{
  if (win_info != NULL)
    {
      struct tui_win_info * winWithFocus = tui_win_with_focus ();

      if (winWithFocus != NULL
	  && winWithFocus->generic.type != CMD_WIN)
	tui_unhighlight_win (winWithFocus);
      tui_set_win_with_focus (win_info);
      if (win_info->generic.type != CMD_WIN)
	tui_highlight_win (win_info);
    }

  return;
}				/* tuiSetWinFocusTo */


void
tui_scroll_forward (struct tui_win_info * winToScroll, int numToScroll)
{
  if (winToScroll != TUI_CMD_WIN)
    {
      int _numToScroll = numToScroll;

      if (numToScroll == 0)
	_numToScroll = winToScroll->generic.height - 3;
      /*
         ** If we are scrolling the source or disassembly window, do a
         ** "psuedo" scroll since not all of the source is in memory,
         ** only what is in the viewport.  If winToScroll is the
         ** command window do nothing since the term should handle it.
       */
      if (winToScroll == TUI_SRC_WIN)
	tui_vertical_source_scroll (FORWARD_SCROLL, _numToScroll);
      else if (winToScroll == TUI_DISASM_WIN)
	tui_vertical_disassem_scroll (FORWARD_SCROLL, _numToScroll);
      else if (winToScroll == TUI_DATA_WIN)
	tui_vertical_data_scroll (FORWARD_SCROLL, _numToScroll);
    }
}

void
tui_scroll_backward (struct tui_win_info * winToScroll, int numToScroll)
{
  if (winToScroll != TUI_CMD_WIN)
    {
      int _numToScroll = numToScroll;

      if (numToScroll == 0)
	_numToScroll = winToScroll->generic.height - 3;
      /*
         ** If we are scrolling the source or disassembly window, do a
         ** "psuedo" scroll since not all of the source is in memory,
         ** only what is in the viewport.  If winToScroll is the
         ** command window do nothing since the term should handle it.
       */
      if (winToScroll == TUI_SRC_WIN)
	tui_vertical_source_scroll (BACKWARD_SCROLL, _numToScroll);
      else if (winToScroll == TUI_DISASM_WIN)
	tui_vertical_disassem_scroll (BACKWARD_SCROLL, _numToScroll);
      else if (winToScroll == TUI_DATA_WIN)
	tui_vertical_data_scroll (BACKWARD_SCROLL, _numToScroll);
    }
}


void
tui_scroll_left (struct tui_win_info * winToScroll, int numToScroll)
{
  if (winToScroll != TUI_CMD_WIN)
    {
      int _numToScroll = numToScroll;

      if (_numToScroll == 0)
	_numToScroll = 1;
      /*
         ** If we are scrolling the source or disassembly window, do a
         ** "psuedo" scroll since not all of the source is in memory,
         ** only what is in the viewport. If winToScroll is the
         ** command window do nothing since the term should handle it.
       */
      if (winToScroll == TUI_SRC_WIN || winToScroll == TUI_DISASM_WIN)
	tui_horizontal_source_scroll (winToScroll, LEFT_SCROLL, _numToScroll);
    }
}


void
tui_scroll_right (struct tui_win_info * winToScroll, int numToScroll)
{
  if (winToScroll != TUI_CMD_WIN)
    {
      int _numToScroll = numToScroll;

      if (_numToScroll == 0)
	_numToScroll = 1;
      /*
         ** If we are scrolling the source or disassembly window, do a
         ** "psuedo" scroll since not all of the source is in memory,
         ** only what is in the viewport. If winToScroll is the
         ** command window do nothing since the term should handle it.
       */
      if (winToScroll == TUI_SRC_WIN || winToScroll == TUI_DISASM_WIN)
	tui_horizontal_source_scroll (winToScroll, RIGHT_SCROLL, _numToScroll);
    }
}


/* Scroll a window.  Arguments are passed through a va_list.    */
void
tui_scroll (enum tui_scroll_direction direction,
	    struct tui_win_info * winToScroll,
	    int numToScroll)
{
  switch (direction)
    {
    case FORWARD_SCROLL:
      tui_scroll_forward (winToScroll, numToScroll);
      break;
    case BACKWARD_SCROLL:
      tui_scroll_backward (winToScroll, numToScroll);
      break;
    case LEFT_SCROLL:
      tui_scroll_left (winToScroll, numToScroll);
      break;
    case RIGHT_SCROLL:
      tui_scroll_right (winToScroll, numToScroll);
      break;
    default:
      break;
    }
}


void
tui_refresh_all_win (void)
{
  enum tui_win_type type;

  clearok (curscr, TRUE);
  tui_refresh_all (tui_win_list);
  for (type = SRC_WIN; type < MAX_MAJOR_WINDOWS; type++)
    {
      if (tui_win_list[type] && tui_win_list[type]->generic.is_visible)
	{
	  switch (type)
	    {
	    case SRC_WIN:
	    case DISASSEM_WIN:
	      tui_show_source_content (tui_win_list[type]);
	      tui_check_and_display_highlight_if_needed (tui_win_list[type]);
	      tui_erase_exec_info_content (tui_win_list[type]);
	      tui_update_exec_info (tui_win_list[type]);
	      break;
	    case DATA_WIN:
	      tui_refresh_data_win ();
	      break;
	    default:
	      break;
	    }
	}
    }
  tui_show_locator_content ();
}


/*
   ** tuiResizeAll().
   **      Resize all the windows based on the the terminal size.  This
   **      function gets called from within the readline sinwinch handler.
 */
void
tuiResizeAll (void)
{
  int heightDiff, widthDiff;
  int screenheight, screenwidth;

  rl_get_screen_size (&screenheight, &screenwidth);
  widthDiff = screenwidth - tui_term_width ();
  heightDiff = screenheight - tui_term_height ();
  if (heightDiff || widthDiff)
    {
      enum tui_layout_type curLayout = tui_current_layout ();
      struct tui_win_info * winWithFocus = tui_win_with_focus ();
      struct tui_win_info *firstWin;
      struct tui_win_info *secondWin;
      struct tui_gen_win_info * locator = tui_locator_win_info_ptr ();
      enum tui_win_type winType;
      int newHeight, splitDiff, cmdSplitDiff, numWinsDisplayed = 2;

      /* turn keypad off while we resize */
      if (winWithFocus != TUI_CMD_WIN)
	keypad (TUI_CMD_WIN->generic.handle, FALSE);
      tui_update_gdb_sizes ();
      tui_set_term_height_to (screenheight);
      tui_set_term_width_to (screenwidth);
      if (curLayout == SRC_DISASSEM_COMMAND ||
	curLayout == SRC_DATA_COMMAND || curLayout == DISASSEM_DATA_COMMAND)
	numWinsDisplayed++;
      splitDiff = heightDiff / numWinsDisplayed;
      cmdSplitDiff = splitDiff;
      if (heightDiff % numWinsDisplayed)
	{
	  if (heightDiff < 0)
	    cmdSplitDiff--;
	  else
	    cmdSplitDiff++;
	}
      /* now adjust each window */
      clear ();
      refresh ();
      switch (curLayout)
	{
	case SRC_COMMAND:
	case DISASSEM_COMMAND:
	  firstWin = (struct tui_win_info *) (tui_source_windows ())->list[0];
	  firstWin->generic.width += widthDiff;
	  locator->width += widthDiff;
	  /* check for invalid heights */
	  if (heightDiff == 0)
	    newHeight = firstWin->generic.height;
	  else if ((firstWin->generic.height + splitDiff) >=
		   (screenheight - MIN_CMD_WIN_HEIGHT - 1))
	    newHeight = screenheight - MIN_CMD_WIN_HEIGHT - 1;
	  else if ((firstWin->generic.height + splitDiff) <= 0)
	    newHeight = MIN_WIN_HEIGHT;
	  else
	    newHeight = firstWin->generic.height + splitDiff;

	  _makeInvisibleAndSetNewHeight (firstWin, newHeight);
	  TUI_CMD_WIN->generic.origin.y = locator->origin.y + 1;
	  TUI_CMD_WIN->generic.width += widthDiff;
	  newHeight = screenheight - TUI_CMD_WIN->generic.origin.y;
	  _makeInvisibleAndSetNewHeight (TUI_CMD_WIN, newHeight);
	  _makeVisibleWithNewHeight (firstWin);
	  _makeVisibleWithNewHeight (TUI_CMD_WIN);
	  if (firstWin->generic.content_size <= 0)
	    tui_erase_source_content (firstWin, EMPTY_SOURCE_PROMPT);
	  break;
	default:
	  if (curLayout == SRC_DISASSEM_COMMAND)
	    {
	      firstWin = TUI_SRC_WIN;
	      firstWin->generic.width += widthDiff;
	      secondWin = TUI_DISASM_WIN;
	      secondWin->generic.width += widthDiff;
	    }
	  else
	    {
	      firstWin = TUI_DATA_WIN;
	      firstWin->generic.width += widthDiff;
	      secondWin = (struct tui_win_info *) (tui_source_windows ())->list[0];
	      secondWin->generic.width += widthDiff;
	    }
	  /* Change the first window's height/width */
	  /* check for invalid heights */
	  if (heightDiff == 0)
	    newHeight = firstWin->generic.height;
	  else if ((firstWin->generic.height +
		    secondWin->generic.height + (splitDiff * 2)) >=
		   (screenheight - MIN_CMD_WIN_HEIGHT - 1))
	    newHeight = (screenheight - MIN_CMD_WIN_HEIGHT - 1) / 2;
	  else if ((firstWin->generic.height + splitDiff) <= 0)
	    newHeight = MIN_WIN_HEIGHT;
	  else
	    newHeight = firstWin->generic.height + splitDiff;
	  _makeInvisibleAndSetNewHeight (firstWin, newHeight);

	  if (firstWin == TUI_DATA_WIN && widthDiff != 0)
	    firstWin->detail.data_display_info.regs_column_count =
	      tui_calculate_regs_column_count (
			  firstWin->detail.data_display_info.regs_display_type);
	  locator->width += widthDiff;

	  /* Change the second window's height/width */
	  /* check for invalid heights */
	  if (heightDiff == 0)
	    newHeight = secondWin->generic.height;
	  else if ((firstWin->generic.height +
		    secondWin->generic.height + (splitDiff * 2)) >=
		   (screenheight - MIN_CMD_WIN_HEIGHT - 1))
	    {
	      newHeight = screenheight - MIN_CMD_WIN_HEIGHT - 1;
	      if (newHeight % 2)
		newHeight = (newHeight / 2) + 1;
	      else
		newHeight /= 2;
	    }
	  else if ((secondWin->generic.height + splitDiff) <= 0)
	    newHeight = MIN_WIN_HEIGHT;
	  else
	    newHeight = secondWin->generic.height + splitDiff;
	  secondWin->generic.origin.y = firstWin->generic.height - 1;
	  _makeInvisibleAndSetNewHeight (secondWin, newHeight);

	  /* Change the command window's height/width */
	  TUI_CMD_WIN->generic.origin.y = locator->origin.y + 1;
	  _makeInvisibleAndSetNewHeight (
			     TUI_CMD_WIN, TUI_CMD_WIN->generic.height + cmdSplitDiff);
	  _makeVisibleWithNewHeight (firstWin);
	  _makeVisibleWithNewHeight (secondWin);
	  _makeVisibleWithNewHeight (TUI_CMD_WIN);
	  if (firstWin->generic.content_size <= 0)
	    tui_erase_source_content (firstWin, EMPTY_SOURCE_PROMPT);
	  if (secondWin->generic.content_size <= 0)
	    tui_erase_source_content (secondWin, EMPTY_SOURCE_PROMPT);
	  break;
	}
      /*
         ** Now remove all invisible windows, and their content so that they get
         ** created again when called for with the new size
       */
      for (winType = SRC_WIN; (winType < MAX_MAJOR_WINDOWS); winType++)
	{
	  if (winType != CMD_WIN && (tui_win_list[winType] != NULL)
	      && !tui_win_list[winType]->generic.is_visible)
	    {
	      tui_free_window (tui_win_list[winType]);
	      tui_win_list[winType] = (struct tui_win_info *) NULL;
	    }
	}
      tui_set_win_resized_to (TRUE);
      /* turn keypad back on, unless focus is in the command window */
      if (winWithFocus != TUI_CMD_WIN)
	keypad (TUI_CMD_WIN->generic.handle, TRUE);
    }
  return;
}				/* tuiResizeAll */


/*
   ** tuiSigwinchHandler()
   **    SIGWINCH signal handler for the tui.  This signal handler is
   **    always called, even when the readline package clears signals
   **    because it is set as the old_sigwinch() (TUI only)
 */
void
tuiSigwinchHandler (int signal)
{
  /*
     ** Say that a resize was done so that the readline can do it
     ** later when appropriate.
   */
  tui_set_win_resized_to (TRUE);

  return;
}				/* tuiSigwinchHandler */



/*************************
** STATIC LOCAL FUNCTIONS
**************************/


/*
   ** _tuiScrollForward_command().
 */
static void
_tuiScrollForward_command (char *arg, int fromTTY)
{
  int numToScroll = 1;
  struct tui_win_info * winToScroll;

  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  if (arg == (char *) NULL)
    _parseScrollingArgs (arg, &winToScroll, (int *) NULL);
  else
    _parseScrollingArgs (arg, &winToScroll, &numToScroll);
  tui_scroll (FORWARD_SCROLL, winToScroll, numToScroll);
}


/*
   ** _tuiScrollBackward_command().
 */
static void
_tuiScrollBackward_command (char *arg, int fromTTY)
{
  int numToScroll = 1;
  struct tui_win_info * winToScroll;

  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  if (arg == (char *) NULL)
    _parseScrollingArgs (arg, &winToScroll, (int *) NULL);
  else
    _parseScrollingArgs (arg, &winToScroll, &numToScroll);
  tui_scroll (BACKWARD_SCROLL, winToScroll, numToScroll);
}


/*
   ** _tuiScrollLeft_command().
 */
static void
_tuiScrollLeft_command (char *arg, int fromTTY)
{
  int numToScroll;
  struct tui_win_info * winToScroll;

  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  _parseScrollingArgs (arg, &winToScroll, &numToScroll);
  tui_scroll (LEFT_SCROLL, winToScroll, numToScroll);
}


/*
   ** _tuiScrollRight_command().
 */
static void
_tuiScrollRight_command (char *arg, int fromTTY)
{
  int numToScroll;
  struct tui_win_info * winToScroll;

  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  _parseScrollingArgs (arg, &winToScroll, &numToScroll);
  tui_scroll (RIGHT_SCROLL, winToScroll, numToScroll);
}


/*
   ** _tuiSetFocus().
   **     Set focus to the window named by 'arg'
 */
static void
_tuiSetFocus (char *arg, int fromTTY)
{
  if (arg != (char *) NULL)
    {
      char *bufPtr = (char *) xstrdup (arg);
      int i;
      struct tui_win_info * win_info = (struct tui_win_info *) NULL;

      for (i = 0; (i < strlen (bufPtr)); i++)
	bufPtr[i] = toupper (arg[i]);

      if (subset_compare (bufPtr, "NEXT"))
	win_info = tui_next_win (tui_win_with_focus ());
      else if (subset_compare (bufPtr, "PREV"))
	win_info = tui_prev_win (tui_win_with_focus ());
      else
	win_info = tui_partial_win_by_name (bufPtr);

      if (win_info == (struct tui_win_info *) NULL || !win_info->generic.is_visible)
	warning ("Invalid window specified. \n\
The window name specified must be valid and visible.\n");
      else
	{
	  tui_set_win_focus_to (win_info);
	  keypad (TUI_CMD_WIN->generic.handle, (win_info != TUI_CMD_WIN));
	}

      if (TUI_DATA_WIN && TUI_DATA_WIN->generic.is_visible)
	tui_refresh_data_win ();
      xfree (bufPtr);
      printf_filtered ("Focus set to %s window.\n",
		       tui_win_name ((struct tui_gen_win_info *) tui_win_with_focus ()));
    }
  else
    warning ("Incorrect Number of Arguments.\n%s", FOCUS_USAGE);

  return;
}				/* _tuiSetFocus */

/*
   ** _tuiSetFocus_command()
 */
static void
_tuiSetFocus_command (char *arg, int fromTTY)
{
  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  _tuiSetFocus (arg, fromTTY);
}


/*
   ** _tuiAllWindowsInfo().
 */
static void
_tuiAllWindowsInfo (char *arg, int fromTTY)
{
  enum tui_win_type type;
  struct tui_win_info * winWithFocus = tui_win_with_focus ();

  for (type = SRC_WIN; (type < MAX_MAJOR_WINDOWS); type++)
    if (tui_win_list[type] && tui_win_list[type]->generic.is_visible)
      {
	if (winWithFocus == tui_win_list[type])
	  printf_filtered ("        %s\t(%d lines)  <has focus>\n",
			   tui_win_name (&tui_win_list[type]->generic),
			   tui_win_list[type]->generic.height);
	else
	  printf_filtered ("        %s\t(%d lines)\n",
			   tui_win_name (&tui_win_list[type]->generic),
			   tui_win_list[type]->generic.height);
      }

  return;
}				/* _tuiAllWindowsInfo */


/*
   ** _tuiRefreshAll_command().
 */
static void
_tuiRefreshAll_command (char *arg, int fromTTY)
{
  /* Make sure the curses mode is enabled.  */
  tui_enable ();

  tui_refresh_all_win ();
}


/*
   ** _tuiSetWinTabWidth_command().
   **        Set the height of the specified window.
 */
static void
_tuiSetTabWidth_command (char *arg, int fromTTY)
{
  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  if (arg != (char *) NULL)
    {
      int ts;

      ts = atoi (arg);
      if (ts > 0)
	tui_set_default_tab_len (ts);
      else
	warning ("Tab widths greater than 0 must be specified.\n");
    }

  return;
}				/* _tuiSetTabWidth_command */


/*
   ** _tuiSetWinHeight().
   **        Set the height of the specified window.
 */
static void
_tuiSetWinHeight (char *arg, int fromTTY)
{
  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  if (arg != (char *) NULL)
    {
      char *buf = xstrdup (arg);
      char *bufPtr = buf;
      char *wname = (char *) NULL;
      int newHeight, i;
      struct tui_win_info * win_info;

      wname = bufPtr;
      bufPtr = strchr (bufPtr, ' ');
      if (bufPtr != (char *) NULL)
	{
	  *bufPtr = (char) 0;

	  /*
	     ** Validate the window name
	   */
	  for (i = 0; i < strlen (wname); i++)
	    wname[i] = toupper (wname[i]);
	  win_info = tui_partial_win_by_name (wname);

	  if (win_info == (struct tui_win_info *) NULL || !win_info->generic.is_visible)
	    warning ("Invalid window specified. \n\
The window name specified must be valid and visible.\n");
	  else
	    {
	      /* Process the size */
	      while (*(++bufPtr) == ' ')
		;

	      if (*bufPtr != (char) 0)
		{
		  int negate = FALSE;
		  int fixedSize = TRUE;
		  int inputNo;;

		  if (*bufPtr == '+' || *bufPtr == '-')
		    {
		      if (*bufPtr == '-')
			negate = TRUE;
		      fixedSize = FALSE;
		      bufPtr++;
		    }
		  inputNo = atoi (bufPtr);
		  if (inputNo > 0)
		    {
		      if (negate)
			inputNo *= (-1);
		      if (fixedSize)
			newHeight = inputNo;
		      else
			newHeight = win_info->generic.height + inputNo;
		      /*
		         ** Now change the window's height, and adjust all
		         ** other windows around it
		       */
		      if (_tuiAdjustWinHeights (win_info,
						newHeight) == TUI_FAILURE)
			warning ("Invalid window height specified.\n%s",
				 WIN_HEIGHT_USAGE);
		      else
                        tui_update_gdb_sizes ();
		    }
		  else
		    warning ("Invalid window height specified.\n%s",
			     WIN_HEIGHT_USAGE);
		}
	    }
	}
      else
	printf_filtered (WIN_HEIGHT_USAGE);

      if (buf != (char *) NULL)
	xfree (buf);
    }
  else
    printf_filtered (WIN_HEIGHT_USAGE);

  return;
}				/* _tuiSetWinHeight */

/*
   ** _tuiSetWinHeight_command().
   **        Set the height of the specified window, with va_list.
 */
static void
_tuiSetWinHeight_command (char *arg, int fromTTY)
{
  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  _tuiSetWinHeight (arg, fromTTY);
}


/*
   ** _tuiXDBsetWinHeight().
   **        XDB Compatibility command for setting the window height.  This will
   **        increase or decrease the command window by the specified amount.
 */
static void
_tuiXDBsetWinHeight (char *arg, int fromTTY)
{
  /* Make sure the curses mode is enabled.  */
  tui_enable ();
  if (arg != (char *) NULL)
    {
      int inputNo = atoi (arg);

      if (inputNo > 0)
	{			/* Add 1 for the locator */
	  int newHeight = tui_term_height () - (inputNo + 1);

	  if (!_newHeightOk (tui_win_list[CMD_WIN], newHeight) ||
	      _tuiAdjustWinHeights (tui_win_list[CMD_WIN],
				    newHeight) == TUI_FAILURE)
	    warning ("Invalid window height specified.\n%s",
		     XDBWIN_HEIGHT_USAGE);
	}
      else
	warning ("Invalid window height specified.\n%s",
		 XDBWIN_HEIGHT_USAGE);
    }
  else
    warning ("Invalid window height specified.\n%s", XDBWIN_HEIGHT_USAGE);

  return;
}				/* _tuiXDBsetWinHeight */

/*
   ** _tuiSetWinHeight_command().
   **        Set the height of the specified window, with va_list.
 */
static void
_tuiXDBsetWinHeight_command (char *arg, int fromTTY)
{
  _tuiXDBsetWinHeight (arg, fromTTY);
}


/*
   ** _tuiAdjustWinHeights().
   **        Function to adjust all window heights around the primary
 */
static enum tui_status
_tuiAdjustWinHeights (struct tui_win_info * primaryWinInfo, int newHeight)
{
  enum tui_status status = TUI_FAILURE;

  if (_newHeightOk (primaryWinInfo, newHeight))
    {
      status = TUI_SUCCESS;
      if (newHeight != primaryWinInfo->generic.height)
	{
	  int diff;
	  struct tui_win_info * win_info;
	  struct tui_gen_win_info * locator = tui_locator_win_info_ptr ();
	  enum tui_layout_type curLayout = tui_current_layout ();

	  diff = (newHeight - primaryWinInfo->generic.height) * (-1);
	  if (curLayout == SRC_COMMAND || curLayout == DISASSEM_COMMAND)
	    {
	      struct tui_win_info * srcWinInfo;

	      _makeInvisibleAndSetNewHeight (primaryWinInfo, newHeight);
	      if (primaryWinInfo->generic.type == CMD_WIN)
		{
		  win_info = (struct tui_win_info *) (tui_source_windows ())->list[0];
		  srcWinInfo = win_info;
		}
	      else
		{
		  win_info = tui_win_list[CMD_WIN];
		  srcWinInfo = primaryWinInfo;
		}
	      _makeInvisibleAndSetNewHeight (win_info,
					     win_info->generic.height + diff);
	      TUI_CMD_WIN->generic.origin.y = locator->origin.y + 1;
	      _makeVisibleWithNewHeight (win_info);
	      _makeVisibleWithNewHeight (primaryWinInfo);
	      if (srcWinInfo->generic.content_size <= 0)
		tui_erase_source_content (srcWinInfo, EMPTY_SOURCE_PROMPT);
	    }
	  else
	    {
	      struct tui_win_info *firstWin;
	      struct tui_win_info *secondWin;

	      if (curLayout == SRC_DISASSEM_COMMAND)
		{
		  firstWin = TUI_SRC_WIN;
		  secondWin = TUI_DISASM_WIN;
		}
	      else
		{
		  firstWin = TUI_DATA_WIN;
		  secondWin = (struct tui_win_info *) (tui_source_windows ())->list[0];
		}
	      if (primaryWinInfo == TUI_CMD_WIN)
		{		/*
				   ** Split the change in height accross the 1st & 2nd windows
				   ** adjusting them as well.
				 */
		  int firstSplitDiff = diff / 2;	/* subtract the locator */
		  int secondSplitDiff = firstSplitDiff;

		  if (diff % 2)
		    {
		      if (firstWin->generic.height >
			  secondWin->generic.height)
			if (diff < 0)
			  firstSplitDiff--;
			else
			  firstSplitDiff++;
		      else
			{
			  if (diff < 0)
			    secondSplitDiff--;
			  else
			    secondSplitDiff++;
			}
		    }
		  /* make sure that the minimum hieghts are honored */
		  while ((firstWin->generic.height + firstSplitDiff) < 3)
		    {
		      firstSplitDiff++;
		      secondSplitDiff--;
		    }
		  while ((secondWin->generic.height + secondSplitDiff) < 3)
		    {
		      secondSplitDiff++;
		      firstSplitDiff--;
		    }
		  _makeInvisibleAndSetNewHeight (
						  firstWin,
				 firstWin->generic.height + firstSplitDiff);
		  secondWin->generic.origin.y = firstWin->generic.height - 1;
		  _makeInvisibleAndSetNewHeight (
		    secondWin, secondWin->generic.height + secondSplitDiff);
		  TUI_CMD_WIN->generic.origin.y = locator->origin.y + 1;
		  _makeInvisibleAndSetNewHeight (TUI_CMD_WIN, newHeight);
		}
	      else
		{
		  if ((TUI_CMD_WIN->generic.height + diff) < 1)
		    {		/*
				   ** If there is no way to increase the command window
				   ** take real estate from the 1st or 2nd window.
				 */
		      if ((TUI_CMD_WIN->generic.height + diff) < 1)
			{
			  int i;
			  for (i = TUI_CMD_WIN->generic.height + diff;
			       (i < 1); i++)
			    if (primaryWinInfo == firstWin)
			      secondWin->generic.height--;
			    else
			      firstWin->generic.height--;
			}
		    }
		  if (primaryWinInfo == firstWin)
		    _makeInvisibleAndSetNewHeight (firstWin, newHeight);
		  else
		    _makeInvisibleAndSetNewHeight (
						    firstWin,
						  firstWin->generic.height);
		  secondWin->generic.origin.y = firstWin->generic.height - 1;
		  if (primaryWinInfo == secondWin)
		    _makeInvisibleAndSetNewHeight (secondWin, newHeight);
		  else
		    _makeInvisibleAndSetNewHeight (
				      secondWin, secondWin->generic.height);
		  TUI_CMD_WIN->generic.origin.y = locator->origin.y + 1;
		  if ((TUI_CMD_WIN->generic.height + diff) < 1)
		    _makeInvisibleAndSetNewHeight (TUI_CMD_WIN, 1);
		  else
		    _makeInvisibleAndSetNewHeight (
				     TUI_CMD_WIN, TUI_CMD_WIN->generic.height + diff);
		}
	      _makeVisibleWithNewHeight (TUI_CMD_WIN);
	      _makeVisibleWithNewHeight (secondWin);
	      _makeVisibleWithNewHeight (firstWin);
	      if (firstWin->generic.content_size <= 0)
		tui_erase_source_content (firstWin, EMPTY_SOURCE_PROMPT);
	      if (secondWin->generic.content_size <= 0)
		tui_erase_source_content (secondWin, EMPTY_SOURCE_PROMPT);
	    }
	}
    }

  return status;
}				/* _tuiAdjustWinHeights */


/*
   ** _makeInvisibleAndSetNewHeight().
   **        Function make the target window (and auxillary windows associated
   **        with the targer) invisible, and set the new height and location.
 */
static void
_makeInvisibleAndSetNewHeight (struct tui_win_info * win_info, int height)
{
  int i;
  struct tui_gen_win_info * genWinInfo;

  tui_make_invisible (&win_info->generic);
  win_info->generic.height = height;
  if (height > 1)
    win_info->generic.viewport_height = height - 1;
  else
    win_info->generic.viewport_height = height;
  if (win_info != TUI_CMD_WIN)
    win_info->generic.viewport_height--;

  /* Now deal with the auxillary windows associated with win_info */
  switch (win_info->generic.type)
    {
    case SRC_WIN:
    case DISASSEM_WIN:
      genWinInfo = win_info->detail.source_info.execution_info;
      tui_make_invisible (genWinInfo);
      genWinInfo->height = height;
      genWinInfo->origin.y = win_info->generic.origin.y;
      if (height > 1)
	genWinInfo->viewport_height = height - 1;
      else
	genWinInfo->viewport_height = height;
      if (win_info != TUI_CMD_WIN)
	genWinInfo->viewport_height--;

      if (tui_win_has_locator (win_info))
	{
	  genWinInfo = tui_locator_win_info_ptr ();
	  tui_make_invisible (genWinInfo);
	  genWinInfo->origin.y = win_info->generic.origin.y + height;
	}
      break;
    case DATA_WIN:
      /* delete all data item windows */
      for (i = 0; i < win_info->generic.content_size; i++)
	{
	  genWinInfo = (struct tui_gen_win_info *) & ((struct tui_win_element *)
		      win_info->generic.content[i])->which_element.data_window;
	  tui_delete_win (genWinInfo->handle);
	  genWinInfo->handle = (WINDOW *) NULL;
	}
      break;
    default:
      break;
    }
}


/*
   ** _makeVisibleWithNewHeight().
   **        Function to make the windows with new heights visible.
   **        This means re-creating the windows' content since the window
   **        had to be destroyed to be made invisible.
 */
static void
_makeVisibleWithNewHeight (struct tui_win_info * win_info)
{
  struct symtab *s;

  tui_make_visible (&win_info->generic);
  tui_check_and_display_highlight_if_needed (win_info);
  switch (win_info->generic.type)
    {
    case SRC_WIN:
    case DISASSEM_WIN:
      tui_free_win_content (win_info->detail.source_info.execution_info);
      tui_make_visible (win_info->detail.source_info.execution_info);
      if (win_info->generic.content != NULL)
	{
	  union tui_line_or_address lineOrAddr;
	  struct symtab_and_line cursal
	    = get_current_source_symtab_and_line ();

	  if (win_info->generic.type == SRC_WIN)
	    lineOrAddr.line_no =
	      win_info->detail.source_info.start_line_or_addr.line_no;
	  else
	    lineOrAddr.addr =
	      win_info->detail.source_info.start_line_or_addr.addr;
	  tui_free_win_content (&win_info->generic);
	  tui_update_source_window (win_info, cursal.symtab, lineOrAddr, TRUE);
	}
      else if (deprecated_selected_frame != (struct frame_info *) NULL)
	{
	  union tui_line_or_address line;
	  struct symtab_and_line cursal = get_current_source_symtab_and_line ();


	  s = find_pc_symtab (get_frame_pc (deprecated_selected_frame));
	  if (win_info->generic.type == SRC_WIN)
	    line.line_no = cursal.line;
	  else
	    {
	      find_line_pc (s, cursal.line, &line.addr);
	    }
	  tui_update_source_window (win_info, s, line, TRUE);
	}
      if (tui_win_has_locator (win_info))
	{
	  tui_make_visible (tui_locator_win_info_ptr ());
	  tui_show_locator_content ();
	}
      break;
    case DATA_WIN:
      tui_display_all_data ();
      break;
    case CMD_WIN:
      win_info->detail.command_info.cur_line = 0;
      win_info->detail.command_info.curch = 0;
      wmove (win_info->generic.handle,
	     win_info->detail.command_info.cur_line,
	     win_info->detail.command_info.curch);
      break;
    default:
      break;
    }

  return;
}				/* _makeVisibleWithNewHeight */


static int
_newHeightOk (struct tui_win_info * primaryWinInfo, int newHeight)
{
  int ok = (newHeight < tui_term_height ());

  if (ok)
    {
      int diff;
      enum tui_layout_type cur_layout = tui_current_layout ();

      diff = (newHeight - primaryWinInfo->generic.height) * (-1);
      if (cur_layout == SRC_COMMAND || cur_layout == DISASSEM_COMMAND)
	{
	  ok = ((primaryWinInfo->generic.type == CMD_WIN &&
		 newHeight <= (tui_term_height () - 4) &&
		 newHeight >= MIN_CMD_WIN_HEIGHT) ||
		(primaryWinInfo->generic.type != CMD_WIN &&
		 newHeight <= (tui_term_height () - 2) &&
		 newHeight >= MIN_WIN_HEIGHT));
	  if (ok)
	    {			/* check the total height */
	      struct tui_win_info * win_info;

	      if (primaryWinInfo == TUI_CMD_WIN)
		win_info = (struct tui_win_info *) (tui_source_windows ())->list[0];
	      else
		win_info = TUI_CMD_WIN;
	      ok = ((newHeight +
		     (win_info->generic.height + diff)) <= tui_term_height ());
	    }
	}
      else
	{
	  int curTotalHeight, totalHeight, minHeight = 0;
	  struct tui_win_info *firstWin;
	  struct tui_win_info *secondWin;

	  if (cur_layout == SRC_DISASSEM_COMMAND)
	    {
	      firstWin = TUI_SRC_WIN;
	      secondWin = TUI_DISASM_WIN;
	    }
	  else
	    {
	      firstWin = TUI_DATA_WIN;
	      secondWin = (struct tui_win_info *) (tui_source_windows ())->list[0];
	    }
	  /*
	     ** We could simply add all the heights to obtain the same result
	     ** but below is more explicit since we subtract 1 for the
	     ** line that the first and second windows share, and add one
	     ** for the locator.
	   */
	  totalHeight = curTotalHeight =
	    (firstWin->generic.height + secondWin->generic.height - 1)
	    + TUI_CMD_WIN->generic.height + 1 /*locator */ ;
	  if (primaryWinInfo == TUI_CMD_WIN)
	    {
	      /* locator included since first & second win share a line */
	      ok = ((firstWin->generic.height +
		     secondWin->generic.height + diff) >=
		    (MIN_WIN_HEIGHT * 2) &&
		    newHeight >= MIN_CMD_WIN_HEIGHT);
	      if (ok)
		{
		  totalHeight = newHeight + (firstWin->generic.height +
					  secondWin->generic.height + diff);
		  minHeight = MIN_CMD_WIN_HEIGHT;
		}
	    }
	  else
	    {
	      minHeight = MIN_WIN_HEIGHT;
	      /*
	         ** First see if we can increase/decrease the command
	         ** window.  And make sure that the command window is
	         ** at least 1 line
	       */
	      ok = ((TUI_CMD_WIN->generic.height + diff) > 0);
	      if (!ok)
		{		/*
				   ** Looks like we have to increase/decrease one of
				   ** the other windows
				 */
		  if (primaryWinInfo == firstWin)
		    ok = (secondWin->generic.height + diff) >= minHeight;
		  else
		    ok = (firstWin->generic.height + diff) >= minHeight;
		}
	      if (ok)
		{
		  if (primaryWinInfo == firstWin)
		    totalHeight = newHeight +
		      secondWin->generic.height +
		      TUI_CMD_WIN->generic.height + diff;
		  else
		    totalHeight = newHeight +
		      firstWin->generic.height +
		      TUI_CMD_WIN->generic.height + diff;
		}
	    }
	  /*
	     ** Now make sure that the proposed total height doesn't exceed
	     ** the old total height.
	   */
	  if (ok)
	    ok = (newHeight >= minHeight && totalHeight <= curTotalHeight);
	}
    }

  return ok;
}				/* _newHeightOk */


/*
   ** _parseScrollingArgs().
 */
static void
_parseScrollingArgs (char *arg, struct tui_win_info * * winToScroll, int *numToScroll)
{
  if (numToScroll)
    *numToScroll = 0;
  *winToScroll = tui_win_with_focus ();

  /*
     ** First set up the default window to scroll, in case there is no
     ** window name arg
   */
  if (arg != (char *) NULL)
    {
      char *buf, *bufPtr;

      /* process the number of lines to scroll */
      buf = bufPtr = xstrdup (arg);
      if (isdigit (*bufPtr))
	{
	  char *numStr;

	  numStr = bufPtr;
	  bufPtr = strchr (bufPtr, ' ');
	  if (bufPtr != (char *) NULL)
	    {
	      *bufPtr = (char) 0;
	      if (numToScroll)
		*numToScroll = atoi (numStr);
	      bufPtr++;
	    }
	  else if (numToScroll)
	    *numToScroll = atoi (numStr);
	}

      /* process the window name if one is specified */
      if (bufPtr != (char *) NULL)
	{
	  char *wname;
	  int i;

	  if (*bufPtr == ' ')
	    while (*(++bufPtr) == ' ')
	      ;

	  if (*bufPtr != (char) 0)
	    wname = bufPtr;
	  else
	    wname = "?";
	  
	  /* Validate the window name */
	  for (i = 0; i < strlen (wname); i++)
	    wname[i] = toupper (wname[i]);
	  *winToScroll = tui_partial_win_by_name (wname);

	  if (*winToScroll == (struct tui_win_info *) NULL ||
	      !(*winToScroll)->generic.is_visible)
	    warning ("Invalid window specified. \n\
The window name specified must be valid and visible.\n");
	  else if (*winToScroll == TUI_CMD_WIN)
	    *winToScroll = (struct tui_win_info *) (tui_source_windows ())->list[0];
	}
      xfree (buf);
    }

  return;
}				/* _parseScrollingArgs */
