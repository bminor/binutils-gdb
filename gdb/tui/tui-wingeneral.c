/* General window behavior.

   Copyright 1998, 1999, 2000, 2001, 2002, 2003 Free Software Foundation,
   Inc.

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
#include "tui/tui-win.h"

#ifdef HAVE_NCURSES_H       
#include <ncurses.h>
#else
#ifdef HAVE_CURSES_H
#include <curses.h>
#endif
#endif

/***********************
** PUBLIC FUNCTIONS
***********************/

/* Refresh the window.   */
void
tui_refresh_win (TuiGenWinInfoPtr winInfo)
{
  if (winInfo->type == DATA_WIN && winInfo->contentSize > 0)
    {
      int i;

      for (i = 0; (i < winInfo->contentSize); i++)
	{
	  TuiGenWinInfoPtr dataItemWinPtr;

	  dataItemWinPtr = &((TuiWinContent)
			     winInfo->content)[i]->whichElement.dataWindow;
	  if (m_genWinPtrNotNull (dataItemWinPtr) &&
	      dataItemWinPtr->handle != (WINDOW *) NULL)
	    wrefresh (dataItemWinPtr->handle);
	}
    }
  else if (winInfo->type == CMD_WIN)
    {
      /* Do nothing */
    }
  else
    {
      if (winInfo->handle != (WINDOW *) NULL)
	wrefresh (winInfo->handle);
    }

  return;
}


/* Function to delete the curses window, checking for NULL.   */
void
tui_delete_win (WINDOW * window)
{
  if (window != (WINDOW *) NULL)
    delwin (window);

  return;
}


/* Draw a border arround the window.  */
void
boxWin (TuiGenWinInfoPtr winInfo, int highlightFlag)
{
  if (winInfo && winInfo->handle)
    {
      WINDOW *win;
      int attrs;

      win = winInfo->handle;
      if (highlightFlag == HILITE)
        attrs = tui_active_border_attrs;
      else
        attrs = tui_border_attrs;

      wattron (win, attrs);
      wborder (win, tui_border_vline, tui_border_vline,
               tui_border_hline, tui_border_hline,
               tui_border_ulcorner, tui_border_urcorner,
               tui_border_llcorner, tui_border_lrcorner);
      if (winInfo->title)
        mvwaddstr (win, 0, 3, winInfo->title);
      wattroff (win, attrs);
    }
}


void
tui_unhighlight_win (TuiWinInfoPtr winInfo)
{
  if (m_winPtrNotNull (winInfo) && winInfo->generic.handle != (WINDOW *) NULL)
    {
      boxWin ((TuiGenWinInfoPtr) winInfo, NO_HILITE);
      wrefresh (winInfo->generic.handle);
      m_setWinHighlightOff (winInfo);
    }
}


void
tui_highlight_win (TuiWinInfoPtr winInfo)
{
  if (m_winPtrNotNull (winInfo) &&
      winInfo->canHighlight && winInfo->generic.handle != (WINDOW *) NULL)
    {
      boxWin ((TuiGenWinInfoPtr) winInfo, HILITE);
      wrefresh (winInfo->generic.handle);
      m_setWinHighlightOn (winInfo);
    }
}

void
tui_check_and_display_highlight_if_needed (TuiWinInfoPtr winInfo)
{
  if (m_winPtrNotNull (winInfo) && winInfo->generic.type != CMD_WIN)
    {
      if (winInfo->isHighlighted)
	tui_highlight_win (winInfo);
      else
	tui_unhighlight_win (winInfo);

    }
  return;
}


void
tui_make_window (TuiGenWinInfoPtr winInfo, int boxIt)
{
  WINDOW *handle;

  handle = newwin (winInfo->height,
		   winInfo->width,
		   winInfo->origin.y,
		   winInfo->origin.x);
  winInfo->handle = handle;
  if (handle != (WINDOW *) NULL)
    {
      if (boxIt == BOX_WINDOW)
	boxWin (winInfo, NO_HILITE);
      winInfo->isVisible = TRUE;
      scrollok (handle, TRUE);
    }
}


/* We can't really make windows visible, or invisible.  So we have to
   delete the entire window when making it visible, and create it
   again when making it visible.  */
static void
make_visible (struct tui_gen_win_info *win_info, int visible)
{
  /* Don't tear down/recreate command window */
  if (win_info->type == CMD_WIN)
    return;

  if (visible)
    {
      if (!win_info->isVisible)
	{
	  tui_make_window (win_info,
			   (win_info->type != CMD_WIN
			    && !m_winIsAuxillary (win_info->type)));
	  win_info->isVisible = TRUE;
	}
    }
  else if (!visible &&
	   win_info->isVisible && win_info->handle != (WINDOW *) NULL)
    {
      win_info->isVisible = FALSE;
      tui_delete_win (win_info->handle);
      win_info->handle = (WINDOW *) NULL;
    }

  return;
}

void
tui_make_visible (struct tui_gen_win_info *win_info)
{
  make_visible (win_info, 1);
}

void
tui_make_invisible (struct tui_gen_win_info *win_info)
{
  make_visible (win_info, 0);
}


/* Makes all windows invisible (except the command and locator windows).   */
static void
make_all_visible (int visible)
{
  int i;

  for (i = 0; i < MAX_MAJOR_WINDOWS; i++)
    {
      if (m_winPtrNotNull (winList[i]) &&
	  ((winList[i])->generic.type) != CMD_WIN)
	{
	  if (m_winIsSourceType ((winList[i])->generic.type))
	    make_visible ((winList[i])->detail.sourceInfo.executionInfo,
			  visible);
	  make_visible ((TuiGenWinInfoPtr) winList[i], visible);
	}
    }

  return;
}

void
tui_make_all_visible (void)
{
  make_all_visible (1);
}

void
tui_make_all_invisible (void)
{
  make_all_visible (0);
}

/* Function to refresh all the windows currently displayed.  */

void
tui_refresh_all (TuiWinInfoPtr * list)
{
  TuiWinType type;
  TuiGenWinInfoPtr locator = tui_locator_win_info_ptr ();

  for (type = SRC_WIN; (type < MAX_MAJOR_WINDOWS); type++)
    {
      if (list[type] && list[type]->generic.isVisible)
	{
	  if (type == SRC_WIN || type == DISASSEM_WIN)
	    {
	      touchwin (list[type]->detail.sourceInfo.executionInfo->handle);
	      tui_refresh_win (list[type]->detail.sourceInfo.executionInfo);
	    }
	  touchwin (list[type]->generic.handle);
	  tui_refresh_win (&list[type]->generic);
	}
    }
  if (locator->isVisible)
    {
      touchwin (locator->handle);
      tui_refresh_win (locator);
    }

  return;
}				/* refreshAll */


/*********************************
** Local Static Functions
*********************************/
