/* TUI display source/assembly window.
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

#ifndef _TUI_SOURCEWIN_H
#define _TUI_SOURCEWIN_H

extern void tuiDisplayMainFunction (void);
extern void tuiUpdateSourceWindow (TuiWinInfoPtr, struct symtab *, Opaque,
				   int);
extern void tuiUpdateSourceWindowAsIs (TuiWinInfoPtr, struct symtab *, Opaque,
				       int);
extern void tuiUpdateSourceWindowsWithAddr (Opaque);
extern void tui_vUpdateSourceWindowsWithAddr (va_list);
extern void tuiUpdateSourceWindowsWithLine (struct symtab *, int);
extern void tui_vUpdateSourceWindowsWithLine (va_list);
extern void tuiUpdateSourceWindowsFromLocator (void);
extern void tuiClearSourceContent (TuiWinInfoPtr, int);
extern void tuiClearAllSourceWinsContent (int);
extern void tuiEraseSourceContent (TuiWinInfoPtr, int);
extern void tuiEraseAllSourceWinsContent (int);
extern void tuiSetSourceContentNil (TuiWinInfoPtr, char *);
extern void tuiShowSourceContent (TuiWinInfoPtr);
extern void tuiShowAllSourceWinsContent (void);
extern void tuiHorizontalSourceScroll (TuiWinInfoPtr, TuiScrollDirection,
				       int);
extern void tuiUpdateOnEnd (void);

extern TuiStatus tuiSetExecInfoContent (TuiWinInfoPtr);
extern void tuiShowExecInfoContent (TuiWinInfoPtr);
extern void tuiShowAllExecInfosContent (void);
extern void tuiEraseExecInfoContent (TuiWinInfoPtr);
extern void tuiEraseAllExecInfosContent (void);
extern void tuiClearExecInfoContent (TuiWinInfoPtr);
extern void tuiClearAllExecInfosContent (void);
extern void tuiUpdateExecInfo (TuiWinInfoPtr);
extern void tuiUpdateAllExecInfos (void);

extern void tuiSetIsExecPointAt (Opaque, TuiWinInfoPtr);
extern void tuiSetHasBreakAt (struct breakpoint *, TuiWinInfoPtr, int);
extern void tuiAllSetHasBreakAt (struct breakpoint *, int);
extern void tui_vAllSetHasBreakAt (va_list);
extern TuiStatus tuiAllocSourceBuffer (TuiWinInfoPtr);
extern int tuiLineIsDisplayed (Opaque, TuiWinInfoPtr, int);


/*
   ** Constant definitions
 */
#define        SCROLL_THRESHOLD            2	/* threshold for lazy scroll */


/*
   ** Macros 
 */
#define    m_tuiSetBreakAt(bp, winInfo)       tuiSetHasBreakAt((bp, winInfo, TRUE)
#define    m_tuiClearBreakAt(bp, winInfo)     tuiSetHasBreakAt(bp, winInfo, FALSE)

#define    m_tuiAllSetBreakAt(bp)             tuiAllSetHasBreakAt(bp, TRUE)
#define    m_tuiAllClearBreakAt(bp)           tuiAllSetHasBreakAt(bp, FALSE)

#define    m_tuiSrcLineDisplayed(lineNo)      tuiLineIsDisplayed((Opaque)(lineNo), srcWin, FALSE)
#define    m_tuiSrcAddrDisplayed(addr)        tuiLineIsDisplayed((Opaque)(addr), disassemWin, FALSE)
#define    m_tuiSrcLineDisplayedWithinThreshold(lineNo) \
                                            tuiLineIsDisplayed((Opaque)(lineNo), srcWin, TRUE)
#define    m_tuiSrcAddrDisplayedWithinThreshold(addr) \
                                            tuiLineIsDisplayed((Opaque)(addr), disassemWin, TRUE)
#define m_tuiLineDisplayedWithinThreshold(winInfo, lineOrAddr)                                 \
                                    ( (winInfo == srcWin) ?                                    \
                                        m_tuiSrcLineDisplayedWithinThreshold(lineOrAddr) :    \
                                        m_tuiSrcAddrDisplayedWithinThreshold(lineOrAddr) )



#endif
/*_TUI_SOURCEWIN_H */
