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

#include "defs.h"
#include <ctype.h>
#include "symtab.h"
#include "frame.h"
#include "breakpoint.h"

#include "tui.h"
#include "tuiData.h"
#include "tuiStack.h"
#include "tuiWin.h"
#include "tuiGeneralWin.h"
#include "tuiSourceWin.h"
#include "tuiSource.h"
#include "tuiDisassem.h"


/*****************************************
** EXTERNAL FUNCTION DECLS                **
******************************************/

/*****************************************
** EXTERNAL DATA DECLS                    **
******************************************/
extern int current_source_line;
extern struct symtab *current_source_symtab;


/*****************************************
** STATIC LOCAL FUNCTIONS FORWARD DECLS    **
******************************************/

/*****************************************
** STATIC LOCAL DATA                    **
******************************************/


/*****************************************
** PUBLIC FUNCTIONS                        **
******************************************/

/*********************************
** SOURCE/DISASSEM  FUNCTIONS    **
*********************************/

/*
   ** tuiSrcWinIsDisplayed().
 */
int
tuiSrcWinIsDisplayed (void)
{
  return (m_winPtrNotNull (srcWin) && srcWin->generic.isVisible);
}				/* tuiSrcWinIsDisplayed */


/*
   ** tuiAsmWinIsDisplayed().
 */
int
tuiAsmWinIsDisplayed (void)
{
  return (m_winPtrNotNull (disassemWin) && disassemWin->generic.isVisible);
}				/* tuiAsmWinIsDisplayed */


/*
   ** tuiDisplayMainFunction().
   **        Function to display the "main" routine"
 */
void
tuiDisplayMainFunction (void)
{
  if ((sourceWindows ())->count > 0)
    {
      CORE_ADDR addr;

      addr = parse_and_eval_address ("main");
      if (addr == (CORE_ADDR) 0)
	addr = parse_and_eval_address ("MAIN");
      if (addr != (CORE_ADDR) 0)
	{
	  struct symtab_and_line sal;

	  tuiUpdateSourceWindowsWithAddr (addr);
	  sal = find_pc_line (addr, 0);
	  tuiSwitchFilename (sal.symtab->filename);
	}
    }

  return;
}				/* tuiDisplayMainFunction */



/*
   ** tuiUpdateSourceWindow().
   **    Function to display source in the source window.  This function
   **    initializes the horizontal scroll to 0.
 */
void
tuiUpdateSourceWindow (TuiWinInfoPtr winInfo, struct symtab *s,
                       TuiLineOrAddress lineOrAddr, int noerror)
{
  winInfo->detail.sourceInfo.horizontalOffset = 0;
  tuiUpdateSourceWindowAsIs (winInfo, s, lineOrAddr, noerror);

  return;
}				/* tuiUpdateSourceWindow */


/*
   ** tuiUpdateSourceWindowAsIs().
   **        Function to display source in the source/asm window.  This
   **        function shows the source as specified by the horizontal offset.
 */
void
tuiUpdateSourceWindowAsIs (TuiWinInfoPtr winInfo, struct symtab *s,
                           TuiLineOrAddress lineOrAddr, int noerror)
{
  TuiStatus ret;

  if (winInfo->generic.type == SRC_WIN)
    ret = tuiSetSourceContent (s, lineOrAddr.lineNo, noerror);
  else
    ret = tuiSetDisassemContent (s, lineOrAddr.addr);

  if (ret == TUI_FAILURE)
    {
      tuiClearSourceContent (winInfo, EMPTY_SOURCE_PROMPT);
      tuiClearExecInfoContent (winInfo);
    }
  else
    {
      tuiEraseSourceContent (winInfo, NO_EMPTY_SOURCE_PROMPT);
      tuiShowSourceContent (winInfo);
      tuiUpdateExecInfo (winInfo);
      if (winInfo->generic.type == SRC_WIN)
	{
	  current_source_line = lineOrAddr.lineNo +
	    (winInfo->generic.contentSize - 2);
	  current_source_symtab = s;
	  /*
	     ** If the focus was in the asm win, put it in the src
	     ** win if we don't have a split layout
	   */
	  if (tuiWinWithFocus () == disassemWin &&
	      currentLayout () != SRC_DISASSEM_COMMAND)
	    tuiSetWinFocusTo (srcWin);
	}
    }


  return;
}				/* tuiUpdateSourceWindowAsIs */


/*
   ** tuiUpdateSourceWindowsWithAddr().
   **        Function to ensure that the source and/or disassemly windows
   **        reflect the input address.
 */
void
tuiUpdateSourceWindowsWithAddr (CORE_ADDR addr)
{
  if (addr != 0)
    {
      struct symtab_and_line sal;
      TuiLineOrAddress l;
      
      switch (currentLayout ())
	{
	case DISASSEM_COMMAND:
	case DISASSEM_DATA_COMMAND:
	  tuiShowDisassem (addr);
	  break;
	case SRC_DISASSEM_COMMAND:
	  tuiShowDisassemAndUpdateSource (addr);
	  break;
	default:
	  sal = find_pc_line (addr, 0);
	  l.lineNo = sal.line;
	  tuiShowSource (sal.symtab, l, FALSE);
	  break;
	}
    }
  else
    {
      int i;

      for (i = 0; i < (sourceWindows ())->count; i++)
	{
	  TuiWinInfoPtr winInfo = (TuiWinInfoPtr) (sourceWindows ())->list[i];

	  tuiClearSourceContent (winInfo, EMPTY_SOURCE_PROMPT);
	  tuiClearExecInfoContent (winInfo);
	}
    }

  return;
}				/* tuiUpdateSourceWindowsWithAddr */

/*
   ** tuiUpdateSourceWindowsWithLine().
   **        Function to ensure that the source and/or disassemly windows
   **        reflect the input address.
 */
void
tuiUpdateSourceWindowsWithLine (struct symtab *s, int line)
{
  CORE_ADDR pc;
  TuiLineOrAddress l;
  
  switch (currentLayout ())
    {
    case DISASSEM_COMMAND:
    case DISASSEM_DATA_COMMAND:
      find_line_pc (s, line, &pc);
      tuiUpdateSourceWindowsWithAddr (pc);
      break;
    default:
      l.lineNo = line;
      tuiShowSource (s, l, FALSE);
      if (currentLayout () == SRC_DISASSEM_COMMAND)
	{
	  find_line_pc (s, line, &pc);
	  tuiShowDisassem (pc);
	}
      break;
    }

  return;
}				/* tuiUpdateSourceWindowsWithLine */

/*
   ** tuiClearSourceContent().
 */
void
tuiClearSourceContent (TuiWinInfoPtr winInfo, int displayPrompt)
{
  if (m_winPtrNotNull (winInfo))
    {
      register int i;

      winInfo->generic.contentInUse = FALSE;
      tuiEraseSourceContent (winInfo, displayPrompt);
      for (i = 0; i < winInfo->generic.contentSize; i++)
	{
	  TuiWinElementPtr element =
	  (TuiWinElementPtr) winInfo->generic.content[i];
	  element->whichElement.source.hasBreak = FALSE;
	  element->whichElement.source.isExecPoint = FALSE;
	}
    }

  return;
}				/* tuiClearSourceContent */


/*
   ** tuiClearAllSourceWinsContent().
 */
void
tuiClearAllSourceWinsContent (int displayPrompt)
{
  int i;

  for (i = 0; i < (sourceWindows ())->count; i++)
    tuiClearSourceContent ((TuiWinInfoPtr) (sourceWindows ())->list[i],
			   displayPrompt);

  return;
}				/* tuiClearAllSourceWinsContent */


/*
   ** tuiEraseSourceContent().
 */
void
tuiEraseSourceContent (TuiWinInfoPtr winInfo, int displayPrompt)
{
  int xPos;
  int halfWidth = (winInfo->generic.width - 2) / 2;

  if (winInfo->generic.handle != (WINDOW *) NULL)
    {
      werase (winInfo->generic.handle);
      checkAndDisplayHighlightIfNeeded (winInfo);
      if (displayPrompt == EMPTY_SOURCE_PROMPT)
	{
	  char *noSrcStr;

	  if (winInfo->generic.type == SRC_WIN)
	    noSrcStr = NO_SRC_STRING;
	  else
	    noSrcStr = NO_DISASSEM_STRING;
	  if (strlen (noSrcStr) >= halfWidth)
	    xPos = 1;
	  else
	    xPos = halfWidth - strlen (noSrcStr);
	  mvwaddstr (winInfo->generic.handle,
		     (winInfo->generic.height / 2),
		     xPos,
		     noSrcStr);

	  /* elz: added this function call to set the real contents of
	     the window to what is on the  screen, so that later calls
	     to refresh, do display
	     the correct stuff, and not the old image */

	  tuiSetSourceContentNil (winInfo, noSrcStr);
	}
      tuiRefreshWin (&winInfo->generic);
    }
  return;
}				/* tuiEraseSourceContent */


/*
   ** tuiEraseAllSourceContent().
 */
void
tuiEraseAllSourceWinsContent (int displayPrompt)
{
  int i;

  for (i = 0; i < (sourceWindows ())->count; i++)
    tuiEraseSourceContent ((TuiWinInfoPtr) (sourceWindows ())->list[i],
			   displayPrompt);

  return;
}				/* tuiEraseAllSourceWinsContent */


/*
   ** tuiShowSourceContent().
 */
void
tuiShowSourceContent (TuiWinInfoPtr winInfo)
{
  int curLine, i, curX;

  tuiEraseSourceContent (winInfo, (winInfo->generic.contentSize <= 0));
  if (winInfo->generic.contentSize > 0)
    {
      char *line;

      for (curLine = 1; (curLine <= winInfo->generic.contentSize); curLine++)
	mvwaddstr (
		    winInfo->generic.handle,
		    curLine,
		    1,
		    ((TuiWinElementPtr)
	  winInfo->generic.content[curLine - 1])->whichElement.source.line);
    }
  checkAndDisplayHighlightIfNeeded (winInfo);
  tuiRefreshWin (&winInfo->generic);
  winInfo->generic.contentInUse = TRUE;

  return;
}				/* tuiShowSourceContent */


/*
   ** tuiShowAllSourceWinsContent()
 */
void
tuiShowAllSourceWinsContent (void)
{
  int i;

  for (i = 0; i < (sourceWindows ())->count; i++)
    tuiShowSourceContent ((TuiWinInfoPtr) (sourceWindows ())->list[i]);

  return;
}				/* tuiShowAllSourceWinsContent */


/*
   ** tuiHorizontalSourceScroll().
   **      Scroll the source forward or backward horizontally
 */
void
tuiHorizontalSourceScroll (TuiWinInfoPtr winInfo,
                           TuiScrollDirection direction,
                           int numToScroll)
{
  if (winInfo->generic.content != (OpaquePtr) NULL)
    {
      int offset;
      struct symtab *s;

      if (current_source_symtab == (struct symtab *) NULL)
	s = find_pc_symtab (selected_frame->pc);
      else
	s = current_source_symtab;

      if (direction == LEFT_SCROLL)
	offset = winInfo->detail.sourceInfo.horizontalOffset + numToScroll;
      else
	{
	  if ((offset =
	     winInfo->detail.sourceInfo.horizontalOffset - numToScroll) < 0)
	    offset = 0;
	}
      winInfo->detail.sourceInfo.horizontalOffset = offset;
      tuiUpdateSourceWindowAsIs (
				  winInfo,
				  s,
				  ((TuiWinElementPtr)
				   winInfo->generic.content[0])->whichElement.source.lineOrAddr,
				  FALSE);
    }

  return;
}				/* tuiHorizontalSourceScroll */


/*
   ** tuiSetHasExecPointAt().
   **        Set or clear the hasBreak flag in the line whose line is lineNo.
 */
void
tuiSetIsExecPointAt (TuiLineOrAddress l, TuiWinInfoPtr winInfo)
{
  int i;
  TuiWinContent content = (TuiWinContent) winInfo->generic.content;

  i = 0;
  while (i < winInfo->generic.contentSize)
    {
      if (content[i]->whichElement.source.lineOrAddr.addr == l.addr)
	content[i]->whichElement.source.isExecPoint = TRUE;
      else
	content[i]->whichElement.source.isExecPoint = FALSE;
      i++;
    }

  return;
}				/* tuiSetIsExecPointAt */

/*
   ** tuiSetHasBreakAt().
   **        Set or clear the hasBreak flag in the line whose line is lineNo.
 */
void
tuiSetHasBreakAt (struct breakpoint *bp, TuiWinInfoPtr winInfo, int hasBreak)
{
  int i;
  TuiWinContent content = (TuiWinContent) winInfo->generic.content;

  i = 0;
  while (i < winInfo->generic.contentSize)
    {
      int gotIt;
      TuiGenWinInfoPtr locator = locatorWinInfoPtr ();

      if (winInfo == srcWin)
	{
	  char *fileNameDisplayed = (char *) NULL;

	  if (((TuiWinElementPtr)
	       locator->content[0])->whichElement.locator.fileName !=
	      (char *) NULL)
	    fileNameDisplayed = ((TuiWinElementPtr)
			locator->content[0])->whichElement.locator.fileName;
	  else if (current_source_symtab != (struct symtab *) NULL)
	    fileNameDisplayed = current_source_symtab->filename;

	  gotIt = (fileNameDisplayed != (char *) NULL &&
                   bp->source_file != NULL &&
		   (strcmp (bp->source_file, fileNameDisplayed) == 0) &&
		   content[i]->whichElement.source.lineOrAddr.lineNo ==
		   bp->line_number);
	}
      else
	gotIt = (content[i]->whichElement.source.lineOrAddr.addr
		 == bp->address);
      if (gotIt)
	{
	  content[i]->whichElement.source.hasBreak = hasBreak;
	  break;
	}
      i++;
    }

  return;
}				/* tuiSetHasBreakAt */


/*
   ** tuiAllSetHasBreakAt().
   **        Set or clear the hasBreak flag in all displayed source windows.
 */
void
tuiAllSetHasBreakAt (struct breakpoint *bp, int hasBreak)
{
  int i;

  for (i = 0; i < (sourceWindows ())->count; i++)
    tuiSetHasBreakAt (bp,
		      (TuiWinInfoPtr) (sourceWindows ())->list[i], hasBreak);

  return;
}				/* tuiAllSetHasBreakAt */


/*********************************
** EXECUTION INFO FUNCTIONS        **
*********************************/

/*
   ** tuiSetExecInfoContent().
   **      Function to initialize the content of the execution info window,
   **      based upon the input window which is either the source or
   **      disassembly window.
 */
TuiStatus
tuiSetExecInfoContent (TuiWinInfoPtr winInfo)
{
  TuiStatus ret = TUI_SUCCESS;

  if (winInfo->detail.sourceInfo.executionInfo != (TuiGenWinInfoPtr) NULL)
    {
      TuiGenWinInfoPtr execInfoPtr = winInfo->detail.sourceInfo.executionInfo;

      if (execInfoPtr->content == (OpaquePtr) NULL)
	execInfoPtr->content =
	  (OpaquePtr) allocContent (winInfo->generic.height,
				    execInfoPtr->type);
      if (execInfoPtr->content != (OpaquePtr) NULL)
	{
	  int i;

	  for (i = 0; i < winInfo->generic.contentSize; i++)
	    {
	      TuiWinElementPtr element;
	      TuiWinElementPtr srcElement;

	      element = (TuiWinElementPtr) execInfoPtr->content[i];
	      srcElement = (TuiWinElementPtr) winInfo->generic.content[i];
	      /*
	         ** First check to see if we have a breakpoint that is
	         ** temporary.  If so, and this is our current execution point,
	         ** then clear the break indicator.
	       */
	      if (srcElement->whichElement.source.hasBreak &&
		  srcElement->whichElement.source.isExecPoint)
		{
		  struct breakpoint *bp;
		  int found = FALSE;
		  extern struct breakpoint *breakpoint_chain;

		  for (bp = breakpoint_chain;
		       (bp != (struct breakpoint *) NULL && !found);
		       bp = bp->next)
		    {
		      found =
			(winInfo == srcWin &&
			 bp->line_number ==
		       srcElement->whichElement.source.lineOrAddr.lineNo) ||
			(winInfo == disassemWin &&
			 bp->address == (CORE_ADDR)
			 srcElement->whichElement.source.lineOrAddr.addr);
		      if (found)
			srcElement->whichElement.source.hasBreak =
			  (bp->disposition != disp_del || bp->hit_count <= 0);
		    }
		  if (!found)
		    srcElement->whichElement.source.hasBreak = FALSE;
		}
	      /*
	         ** Now update the exec info content based upon the state
	         ** of each line as indicated by the source content.
	       */
	      if (srcElement->whichElement.source.hasBreak &&
		  srcElement->whichElement.source.isExecPoint)
		element->whichElement.simpleString = breakLocationStr ();
	      else if (srcElement->whichElement.source.hasBreak)
		element->whichElement.simpleString = breakStr ();
	      else if (srcElement->whichElement.source.isExecPoint)
		element->whichElement.simpleString = locationStr ();
	      else
		element->whichElement.simpleString = blankStr ();
	    }
	  execInfoPtr->contentSize = winInfo->generic.contentSize;
	}
      else
	ret = TUI_FAILURE;
    }

  return ret;
}				/* tuiSetExecInfoContent */


/*
   ** tuiShowExecInfoContent().
 */
void
tuiShowExecInfoContent (TuiWinInfoPtr winInfo)
{
  TuiGenWinInfoPtr execInfo = winInfo->detail.sourceInfo.executionInfo;
  int curLine;

  werase (execInfo->handle);
  tuiRefreshWin (execInfo);
  for (curLine = 1; (curLine <= execInfo->contentSize); curLine++)
    mvwaddstr (execInfo->handle,
	       curLine,
	       0,
	       ((TuiWinElementPtr)
		execInfo->content[curLine - 1])->whichElement.simpleString);
  tuiRefreshWin (execInfo);
  execInfo->contentInUse = TRUE;

  return;
}				/* tuiShowExecInfoContent */


/*
   ** tuiShowAllExecInfosContent()
 */
void
tuiShowAllExecInfosContent (void)
{
  int i;

  for (i = 0; i < (sourceWindows ())->count; i++)
    tuiShowExecInfoContent ((TuiWinInfoPtr) (sourceWindows ())->list[i]);

  return;
}				/* tuiShowAllExecInfosContent */


/*
   ** tuiEraseExecInfoContent().
 */
void
tuiEraseExecInfoContent (TuiWinInfoPtr winInfo)
{
  TuiGenWinInfoPtr execInfo = winInfo->detail.sourceInfo.executionInfo;

  werase (execInfo->handle);
  tuiRefreshWin (execInfo);

  return;
}				/* tuiEraseExecInfoContent */


/*
   ** tuiEraseAllExecInfosContent()
 */
void
tuiEraseAllExecInfosContent (void)
{
  int i;

  for (i = 0; i < (sourceWindows ())->count; i++)
    tuiEraseExecInfoContent ((TuiWinInfoPtr) (sourceWindows ())->list[i]);

  return;
}				/* tuiEraseAllExecInfosContent */


/*
   ** tuiClearExecInfoContent().
 */
void
tuiClearExecInfoContent (TuiWinInfoPtr winInfo)
{
  winInfo->detail.sourceInfo.executionInfo->contentInUse = FALSE;
  tuiEraseExecInfoContent (winInfo);

  return;
}				/* tuiClearExecInfoContent */


/*
   ** tuiClearAllExecInfosContent()
 */
void
tuiClearAllExecInfosContent (void)
{
  int i;

  for (i = 0; i < (sourceWindows ())->count; i++)
    tuiClearExecInfoContent ((TuiWinInfoPtr) (sourceWindows ())->list[i]);

  return;
}				/* tuiClearAllExecInfosContent */


/*
   ** tuiUpdateExecInfo().
   **        Function to update the execution info window
 */
void
tuiUpdateExecInfo (TuiWinInfoPtr winInfo)
{
  tuiSetExecInfoContent (winInfo);
  tuiShowExecInfoContent (winInfo);
}				/* tuiUpdateExecInfo */


				   /*
				   ** tuiUpdateAllExecInfos()
				 */
void
tuiUpdateAllExecInfos (void)
{
  int i;

  for (i = 0; i < (sourceWindows ())->count; i++)
    tuiUpdateExecInfo ((TuiWinInfoPtr) (sourceWindows ())->list[i]);

  return;
}				/* tuiUpdateAllExecInfos */



/* tuiUpdateOnEnd()
   **       elz: This function clears the execution info from the source windows
   **       and resets the locator to display no line info, procedure info, pc
   **       info.  It is called by stack_publish_stopped_with_no_frame, which
   **       is called then the target terminates execution
 */
void
tuiUpdateOnEnd (void)
{
  int i;
  TuiGenWinInfoPtr locator;
  char *filename;
  TuiWinInfoPtr winInfo;

  locator = locatorWinInfoPtr ();

  /* for all the windows (src, asm) */
  for (i = 0; i < (sourceWindows ())->count; i++)
    {
      TuiLineOrAddress l;
      
      winInfo = (TuiWinInfoPtr) (sourceWindows ())->list[i];

      l.addr = -1;
      l.lineNo = -1;
      tuiSetIsExecPointAt (l, winInfo);	/* the target is'n running */
      /* -1 should not match any line number or pc */
      tuiSetExecInfoContent (winInfo);	/*set winInfo so that > is'n displayed */
      tuiShowExecInfoContent (winInfo);		/* display the new contents */
    }

  /*now update the locator */
  tuiClearLocatorDisplay ();
  tuiGetLocatorFilename (locator, &filename);
  tuiSetLocatorInfo (
		      filename,
		      (char *) NULL,
		      0,
		      (CORE_ADDR) 0,
	   &((TuiWinElementPtr) locator->content[0])->whichElement.locator);
  tuiShowLocatorContent ();

  return;
}				/* tuiUpdateOnEnd */



TuiStatus
tuiAllocSourceBuffer (TuiWinInfoPtr winInfo)
{
  register char *srcLine, *srcLineBuf;
  register int i, lineWidth, c, maxLines;
  TuiStatus ret = TUI_FAILURE;

  maxLines = winInfo->generic.height;	/* less the highlight box */
  lineWidth = winInfo->generic.width - 1;
  /*
     ** Allocate the buffer for the source lines.  Do this only once since they
     ** will be re-used for all source displays.  The only other time this will
     ** be done is when a window's size changes.
   */
  if (winInfo->generic.content == (OpaquePtr) NULL)
    {
      srcLineBuf = (char *) xmalloc ((maxLines * lineWidth) * sizeof (char));
      if (srcLineBuf == (char *) NULL)
	fputs_unfiltered (
	   "Unable to Allocate Memory for Source or Disassembly Display.\n",
			   gdb_stderr);
      else
	{
	  /* allocate the content list */
	  if ((winInfo->generic.content =
	  (OpaquePtr) allocContent (maxLines, SRC_WIN)) == (OpaquePtr) NULL)
	    {
	      tuiFree (srcLineBuf);
	      srcLineBuf = (char *) NULL;
	      fputs_unfiltered (
				 "Unable to Allocate Memory for Source or Disassembly Display.\n",
				 gdb_stderr);
	    }
	}
      for (i = 0; i < maxLines; i++)
	((TuiWinElementPtr)
	 winInfo->generic.content[i])->whichElement.source.line =
	  srcLineBuf + (lineWidth * i);
      ret = TUI_SUCCESS;
    }
  else
    ret = TUI_SUCCESS;

  return ret;
}				/* tuiAllocSourceBuffer */


/*
   ** tuiLineIsDisplayed().
   **      Answer whether the a particular line number or address is displayed
   **      in the current source window.
 */
int
tuiLineIsDisplayed (int line, TuiWinInfoPtr winInfo,
                    int checkThreshold)
{
  int isDisplayed = FALSE;
  int i, threshold;

  if (checkThreshold)
    threshold = SCROLL_THRESHOLD;
  else
    threshold = 0;
  i = 0;
  while (i < winInfo->generic.contentSize - threshold && !isDisplayed)
    {
      isDisplayed = (((TuiWinElementPtr)
		      winInfo->generic.content[i])->whichElement.source.lineOrAddr.lineNo
		     == (int) line);
      i++;
    }

  return isDisplayed;
}				/* tuiLineIsDisplayed */


/*
   ** tuiLineIsDisplayed().
   **      Answer whether the a particular line number or address is displayed
   **      in the current source window.
 */
int
tuiAddrIsDisplayed (CORE_ADDR addr, TuiWinInfoPtr winInfo,
		    int checkThreshold)
{
  int isDisplayed = FALSE;
  int i, threshold;

  if (checkThreshold)
    threshold = SCROLL_THRESHOLD;
  else
    threshold = 0;
  i = 0;
  while (i < winInfo->generic.contentSize - threshold && !isDisplayed)
    {
      isDisplayed = (((TuiWinElementPtr)
		      winInfo->generic.content[i])->whichElement.source.lineOrAddr.addr
		     == addr);
      i++;
    }

  return isDisplayed;
}


/*****************************************
** STATIC LOCAL FUNCTIONS               **
******************************************/
