/* Data/register window display.

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
#include "tui/tui.h"
#include "tui/tui-data.h"
#include "tui/tui-wingeneral.h"
#include "tui/tui-regs.h"

#ifdef HAVE_NCURSES_H       
#include <ncurses.h>
#else
#ifdef HAVE_CURSES_H
#include <curses.h>
#endif
#endif


/*****************************************
** STATIC LOCAL FUNCTIONS FORWARD DECLS    **
******************************************/



/*****************************************
** PUBLIC FUNCTIONS                        **
******************************************/


/*
   ** tuiFirstDataItemDisplayed()
   **    Answer the index first element displayed.
   **    If none are displayed, then return (-1).
 */
int
tuiFirstDataItemDisplayed (void)
{
  int elementNo = (-1);
  int i;

  for (i = 0; (i < TUI_DATA_WIN->generic.content_size && elementNo < 0); i++)
    {
      struct tui_gen_win_info * dataItemWin;

      dataItemWin = &((tui_win_content)
		      TUI_DATA_WIN->generic.content)[i]->which_element.data_window;
      if (dataItemWin->handle != (WINDOW *) NULL && dataItemWin->is_visible)
	elementNo = i;
    }

  return elementNo;
}				/* tuiFirstDataItemDisplayed */


/*
   ** tuiFirstDataElementNoInLine()
   **        Answer the index of the first element in lineNo.  If lineNo is
   **        past the data area (-1) is returned.
 */
int
tuiFirstDataElementNoInLine (int lineNo)
{
  int firstElementNo = (-1);

  /*
     ** First see if there is a register on lineNo, and if so, set the
     ** first element number
   */
  if ((firstElementNo = tui_first_reg_element_no_inline (lineNo)) == -1)
    {				/*
				   ** Looking at the general data, the 1st element on lineNo
				 */
    }

  return firstElementNo;
}				/* tuiFirstDataElementNoInLine */


/*
   ** tuiDeleteDataContentWindows()
   **        Function to delete all the item windows in the data window.
   **        This is usually done when the data window is scrolled.
 */
void
tuiDeleteDataContentWindows (void)
{
  int i;
  struct tui_gen_win_info * dataItemWinPtr;

  for (i = 0; (i < TUI_DATA_WIN->generic.content_size); i++)
    {
      dataItemWinPtr = &((tui_win_content)
		      TUI_DATA_WIN->generic.content)[i]->which_element.data_window;
      tui_delete_win (dataItemWinPtr->handle);
      dataItemWinPtr->handle = (WINDOW *) NULL;
      dataItemWinPtr->is_visible = FALSE;
    }

  return;
}				/* tuiDeleteDataContentWindows */


void
tui_erase_data_content (char *prompt)
{
  werase (TUI_DATA_WIN->generic.handle);
  tui_check_and_display_highlight_if_needed (TUI_DATA_WIN);
  if (prompt != (char *) NULL)
    {
      int halfWidth = (TUI_DATA_WIN->generic.width - 2) / 2;
      int xPos;

      if (strlen (prompt) >= halfWidth)
	xPos = 1;
      else
	xPos = halfWidth - strlen (prompt);
      mvwaddstr (TUI_DATA_WIN->generic.handle,
		 (TUI_DATA_WIN->generic.height / 2),
		 xPos,
		 prompt);
    }
  wrefresh (TUI_DATA_WIN->generic.handle);
}


/* This function displays the data that is in the data window's
   content.  It does not set the content.  */
void
tui_display_all_data (void)
{
  if (TUI_DATA_WIN->generic.content_size <= 0)
    tui_erase_data_content (NO_DATA_STRING);
  else
    {
      tui_erase_data_content ((char *) NULL);
      tuiDeleteDataContentWindows ();
      tui_check_and_display_highlight_if_needed (TUI_DATA_WIN);
      tui_display_registers_from (0);
      /*
         ** Then display the other data
       */
      if (TUI_DATA_WIN->detail.data_display_info.data_content !=
	  (tui_win_content) NULL &&
	  TUI_DATA_WIN->detail.data_display_info.data_content_count > 0)
	{
	}
    }
}


/*
   ** tuiDisplayDataFromLine()
   **        Function to display the data starting at line, lineNo, in the
   **        data window.
 */
void
tuiDisplayDataFromLine (int lineNo)
{
  int _lineNo = lineNo;

  if (lineNo < 0)
    _lineNo = 0;

  tui_check_and_display_highlight_if_needed (TUI_DATA_WIN);

  /* there is no general data, force regs to display (if there are any) */
  if (TUI_DATA_WIN->detail.data_display_info.data_content_count <= 0)
    tui_display_registers_from_line (_lineNo, TRUE);
  else
    {
      int elementNo, startLineNo;
      int regsLastLine = tui_last_regs_line_no ();


      /* display regs if we can */
      if (tui_display_registers_from_line (_lineNo, FALSE) < 0)
	{			/*
				   ** _lineNo is past the regs display, so calc where the
				   ** start data element is
				 */
	  if (regsLastLine < _lineNo)
	    {			/* figure out how many lines each element is to obtain
				   the start elementNo */
	    }
	}
      else
	{			/*
				   ** calculate the starting element of the data display, given
				   ** regsLastLine and how many lines each element is, up to
				   ** _lineNo
				 */
	}
      /* Now display the data , starting at elementNo */
    }

  return;
}				/* tuiDisplayDataFromLine */


/*
   ** tuiDisplayDataFrom()
   **        Display data starting at element elementNo
 */
void
tuiDisplayDataFrom (int elementNo, int reuseWindows)
{
  int firstLine = (-1);

  if (elementNo < TUI_DATA_WIN->detail.data_display_info.regs_content_count)
    firstLine = tui_line_from_reg_element_no (elementNo);
  else
    {				/* calculate the firstLine from the element number */
    }

  if (firstLine >= 0)
    {
      tui_erase_data_content ((char *) NULL);
      if (!reuseWindows)
	tuiDeleteDataContentWindows ();
      tuiDisplayDataFromLine (firstLine);
    }

  return;
}				/* tuiDisplayDataFrom */


/* Function to redisplay the contents of the data window.  */
void
tui_refresh_data_win (void)
{
  tui_erase_data_content ((char *) NULL);
  if (TUI_DATA_WIN->generic.content_size > 0)
    {
      int firstElement = tuiFirstDataItemDisplayed ();

      if (firstElement >= 0)	/* re-use existing windows */
	tuiDisplayDataFrom (firstElement, TRUE);
    }
}


/* Function to check the data values and hilite any that have changed.  */
void
tui_check_data_values (struct frame_info *frame)
{
  tui_check_register_values (frame);

  /* Now check any other data values that there are */
  if (TUI_DATA_WIN != NULL && TUI_DATA_WIN->generic.is_visible)
    {
      int i;

      for (i = 0; TUI_DATA_WIN->detail.data_display_info.data_content_count; i++)
	{
#ifdef LATER
	  TuiDataElementPtr dataElementPtr;
	  struct tui_gen_win_info * dataItemWinPtr;
	  Opaque newValue;

	  dataItemPtr = &TUI_DATA_WIN->detail.data_display_info.
	    data_content[i]->which_element.data_window;
	  dataElementPtr = &((tui_win_content)
			     dataItemWinPtr->content)[0]->which_element.data;
	  if value
	    has changed (dataElementPtr, frame, &newValue)
	    {
	      dataElementPtr->value = newValue;
	      update the display with the new value, hiliting it.
	    }
#endif
	}
    }
}


/* Scroll the data window vertically forward or backward.   */
void
tui_vertical_data_scroll (enum tui_scroll_direction scrollDirection, int numToScroll)
{
  int firstElementNo;
  int firstLine = (-1);

  firstElementNo = tuiFirstDataItemDisplayed ();
  if (firstElementNo < TUI_DATA_WIN->detail.data_display_info.regs_content_count)
    firstLine = tui_line_from_reg_element_no (firstElementNo);
  else
    {				/* calculate the first line from the element number which is in
				   ** the general data content
				 */
    }

  if (firstLine >= 0)
    {
      int lastElementNo, lastLine;

      if (scrollDirection == FORWARD_SCROLL)
	firstLine += numToScroll;
      else
	firstLine -= numToScroll;
      tui_erase_data_content ((char *) NULL);
      tuiDeleteDataContentWindows ();
      tuiDisplayDataFromLine (firstLine);
    }

  return;
}				/* tuiVerticalDataScroll */


/*****************************************
** STATIC LOCAL FUNCTIONS               **
******************************************/
