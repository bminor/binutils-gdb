/* TUI display locator.
   Copyright 1998, 1999, 2000, 2001 Free Software Foundation, Inc.
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

#ifndef _TUI_STACK_H
#define _TUI_STACK_H
/*
   ** This header file supports
 */

extern void tuiSetLocatorInfo (char *, char *, int, Opaque,
			       TuiLocatorElementPtr);
extern void tuiUpdateLocatorFilename (char *);
extern void tui_vUpdateLocatorFilename (va_list);
extern void tuiUpdateLocatorInfoFromFrame
  (struct frame_info *, TuiLocatorElementPtr);
extern void tuiUpdateLocatorDisplay (struct frame_info *);
extern void tuiSetLocatorContent (struct frame_info *);
extern void tuiShowLocatorContent (void);
extern void tuiClearLocatorContent (void);
extern void tuiSwitchFilename (char *);
extern void tuiShowFrameInfo (struct frame_info *);
extern void tui_vShowFrameInfo (va_list);
extern void tuiGetLocatorFilename (TuiGenWinInfoPtr, char **);


#endif
/*_TUI_STACK_H*/
