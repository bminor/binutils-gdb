/* Specific command window processing.

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
#include <ctype.h>
#include "tui.h"
#include "tuiData.h"
#include "tuiWin.h"
#include "tuiIO.h"

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
   ** tuiDispatchCtrlChar().
   **        Dispatch the correct tui function based upon the control character.
 */
unsigned int
tuiDispatchCtrlChar (unsigned int ch)
{
  TuiWinInfoPtr winInfo = tuiWinWithFocus ();
  WINDOW *w = cmdWin->generic.handle;

  /*
     ** If the command window has the logical focus, or no-one does
     ** assume it is the command window; in this case, pass the
     ** character on through and do nothing here.
   */
  if (winInfo == (TuiWinInfoPtr) NULL || winInfo == cmdWin)
    return ch;
  else
    {
      unsigned int c = 0, chCopy = ch;
      register int i;
      char *term;

      /* If this is an xterm, page next/prev keys aren't returned
         ** by keypad as a single char, so we must handle them here.
         ** Seems like a bug in the curses library?
       */
      term = (char *) getenv ("TERM");
      for (i = 0; (term && term[i]); i++)
	term[i] = toupper (term[i]);
      if ((strcmp (term, "XTERM") == 0) && m_isStartSequence (ch))
	{
	  unsigned int pageCh = 0, tmpChar;

	  tmpChar = 0;
	  while (!m_isEndSequence (tmpChar))
	    {
	      tmpChar = (int) wgetch (w);
	      if (tmpChar == ERR)
		{
		  return ch;
		}
	      if (!tmpChar)
		break;
	      if (tmpChar == 53)
		pageCh = KEY_PPAGE;
	      else if (tmpChar == 54)
		pageCh = KEY_NPAGE;
	      else
		{
		  return 0;
		}
	    }
	  chCopy = pageCh;
	}

      switch (chCopy)
	{
	case KEY_NPAGE:
	  tuiScrollForward (winInfo, 0);
	  break;
	case KEY_PPAGE:
	  tuiScrollBackward (winInfo, 0);
	  break;
	case KEY_DOWN:
	case KEY_SF:
	  tuiScrollForward (winInfo, 1);
	  break;
	case KEY_UP:
	case KEY_SR:
	  tuiScrollBackward (winInfo, 1);
	  break;
	case KEY_RIGHT:
	  tuiScrollLeft (winInfo, 1);
	  break;
	case KEY_LEFT:
	  tuiScrollRight (winInfo, 1);
	  break;
	case '\f':
	  tuiRefreshAll ();
	  break;
	default:
	  c = chCopy;
	  break;
	}
      return c;
    }
}
