/* Disassembly display.
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
#include "symtab.h"
#include "breakpoint.h"
#include "frame.h"

#include "tui.h"
#include "tuiData.h"
#include "tuiWin.h"
#include "tuiLayout.h"
#include "tuiSourceWin.h"
#include "tuiStack.h"
#include "tui-file.h"


/*****************************************
** STATIC LOCAL FUNCTIONS FORWARD DECLS    **
******************************************/

static struct breakpoint *_hasBreak (CORE_ADDR);


/*****************************************
** PUBLIC FUNCTIONS                        **
******************************************/

/*
   ** tuiSetDisassemContent().
   **        Function to set the disassembly window's content.
 */
TuiStatus
tuiSetDisassemContent (struct symtab *s, CORE_ADDR startAddr)
{
  TuiStatus ret = TUI_FAILURE;
  struct ui_file *gdb_dis_out;

  if (startAddr != 0)
    {
      register int i, desc;

      if ((ret = tuiAllocSourceBuffer (disassemWin)) == TUI_SUCCESS)
	{
	  register int offset = disassemWin->detail.sourceInfo.horizontalOffset;
	  register int threshold, curLine = 0, lineWidth, maxLines;
	  CORE_ADDR newpc, pc;
	  disassemble_info asmInfo;
	  TuiGenWinInfoPtr locator = locatorWinInfoPtr ();
extern void strcat_address (CORE_ADDR, char *, int);
extern void strcat_address_numeric (CORE_ADDR, int, char *, int);
	  int curLen = 0;
	  int tab_len = tuiDefaultTabLen ();

	  maxLines = disassemWin->generic.height - 2;	/* account for hilite */
	  lineWidth = disassemWin->generic.width - 1;
	  threshold = (lineWidth - 1) + offset;

	  /* now init the ui_file structure */
	  gdb_dis_out = tui_sfileopen (threshold);

          asmInfo = tm_print_insn_info;
          asmInfo.stream = gdb_dis_out;

	  disassemWin->detail.sourceInfo.startLineOrAddr.addr = startAddr;

	  /* Now construct each line */
	  for (curLine = 0, pc = startAddr; (curLine < maxLines);)
	    {
	      TuiWinElementPtr element = (TuiWinElementPtr) disassemWin->generic.content[curLine];
	      struct breakpoint *bp;

	      print_address (pc, gdb_dis_out);

	      curLen = strlen (tui_file_get_strbuf (gdb_dis_out));
	      i = curLen - ((curLen / tab_len) * tab_len);

	      /* adjust buffer length if necessary */
	      tui_file_adjust_strbuf ((tab_len - i > 0) ? (tab_len - i) : 0, gdb_dis_out);

	      /* Add spaces to make the instructions start onthe same column */
	      while (i < tab_len)
		{
		  tui_file_get_strbuf (gdb_dis_out)[curLen] = ' ';
		  i++;
		  curLen++;
		}
	      tui_file_get_strbuf (gdb_dis_out)[curLen] = '\0';

	      newpc = pc + ((*tm_print_insn) (pc, &asmInfo));

	      /* Now copy the line taking the offset into account */
	      if (strlen (tui_file_get_strbuf (gdb_dis_out)) > offset)
		strcpy (element->whichElement.source.line,
			&(tui_file_get_strbuf (gdb_dis_out)[offset]));
	      else
		element->whichElement.source.line[0] = '\0';
	      element->whichElement.source.lineOrAddr.addr = pc;
	      element->whichElement.source.isExecPoint =
		(pc == (CORE_ADDR) ((TuiWinElementPtr) locator->content[0])->whichElement.locator.addr);
	      bp = _hasBreak (pc);
	      element->whichElement.source.hasBreak =
		(bp != (struct breakpoint *) NULL &&
		 (!element->whichElement.source.isExecPoint ||
		  (bp->disposition != disp_del || bp->hit_count <= 0)));
	      curLine++;
	      pc = newpc;
	      /* reset the buffer to empty */
	      tui_file_get_strbuf (gdb_dis_out)[0] = '\0';
	    }
	  ui_file_delete (gdb_dis_out);
	  gdb_dis_out = NULL;
	  disassemWin->generic.contentSize = curLine;
	  ret = TUI_SUCCESS;
	}
    }

  return ret;
}				/* tuiSetDisassemContent */


/*
   ** tuiShowDisassem().
   **        Function to display the disassembly window with disassembled code.
 */
void
tuiShowDisassem (CORE_ADDR startAddr)
{
  struct symtab *s = find_pc_symtab (startAddr);
  TuiWinInfoPtr winWithFocus = tuiWinWithFocus ();
  TuiLineOrAddress val;

  val.addr = startAddr;
  tuiAddWinToLayout (DISASSEM_WIN);
  tuiUpdateSourceWindow (disassemWin, s, val, FALSE);
  /*
     ** if the focus was in the src win, put it in the asm win, if the
     ** source view isn't split
   */
  if (currentLayout () != SRC_DISASSEM_COMMAND && winWithFocus == srcWin)
    tuiSetWinFocusTo (disassemWin);

  return;
}				/* tuiShowDisassem */


/*
   ** tuiShowDisassemAndUpdateSource().
   **        Function to display the disassembly window.
 */
void
tuiShowDisassemAndUpdateSource (CORE_ADDR startAddr)
{
  struct symtab_and_line sal;

  tuiShowDisassem (startAddr);
  if (currentLayout () == SRC_DISASSEM_COMMAND)
    {
      TuiLineOrAddress val;
      TuiGenWinInfoPtr locator = locatorWinInfoPtr ();
      /*
         ** Update what is in the source window if it is displayed too,
         ** note that it follows what is in the disassembly window and visa-versa
       */
      sal = find_pc_line (startAddr, 0);
      val.lineNo = sal.line;
      tuiUpdateSourceWindow (srcWin, sal.symtab, val, TRUE);
      if (sal.symtab)
	{
	  current_source_symtab = sal.symtab;
	  tuiUpdateLocatorFilename (sal.symtab->filename);
	}
      else
	tuiUpdateLocatorFilename ("?");
    }

  return;
}				/* tuiShowDisassemAndUpdateSource */

/*
   ** tuiGetBeginAsmAddress().
 */
CORE_ADDR
tuiGetBeginAsmAddress (void)
{
  TuiGenWinInfoPtr locator;
  TuiLocatorElementPtr element;
  CORE_ADDR addr;

  locator = locatorWinInfoPtr ();
  element = &((TuiWinElementPtr) locator->content[0])->whichElement.locator;

  if (element->addr == 0)
    {
      /*the target is not executing, because the pc is 0 */

      addr = parse_and_eval_address ("main");

      if (addr == 0)
	addr = parse_and_eval_address ("MAIN");

    }
  else				/* the target is executing */
    addr = element->addr;

  return addr;
}				/* tuiGetBeginAsmAddress */


/*
   ** tuiVerticalDisassemScroll().
   **      Scroll the disassembly forward or backward vertically
 */
void
tuiVerticalDisassemScroll (TuiScrollDirection scrollDirection,
                           int numToScroll)
{
  if (disassemWin->generic.content != (OpaquePtr) NULL)
    {
      CORE_ADDR pc, lowAddr;
      TuiWinContent content;
      struct symtab *s;

      content = (TuiWinContent) disassemWin->generic.content;
      if (current_source_symtab == (struct symtab *) NULL)
	s = find_pc_symtab (selected_frame->pc);
      else
	s = current_source_symtab;

      pc = content[0]->whichElement.source.lineOrAddr.addr;
      if (find_pc_partial_function (pc, (char **) NULL, &lowAddr,
				    (CORE_ADDR) 0) == 0)
	error ("No function contains program counter for selected frame.\n");
      else
	{
	  register int line = 0;
	  register CORE_ADDR newLow;
	  bfd_byte buffer[4];
	  TuiLineOrAddress val;

	  newLow = pc;
	  if (scrollDirection == FORWARD_SCROLL)
	    {
	      for (; line < numToScroll; line++)
		newLow += sizeof (bfd_getb32 (buffer));
	    }
	  else
	    {
	      for (; newLow != 0 && line < numToScroll; line++)
		newLow -= sizeof (bfd_getb32 (buffer));
	    }
	  val.addr = newLow;
	  tuiUpdateSourceWindowAsIs (disassemWin, s, val, FALSE);
	}
    }

  return;
}				/* tuiVerticalDisassemScroll */



/*****************************************
** STATIC LOCAL FUNCTIONS                 **
******************************************/
/*
   ** _hasBreak().
   **      Answer whether there is a break point at the input line in the
   **      source file indicated
 */
static struct breakpoint *
_hasBreak (CORE_ADDR addr)
{
  struct breakpoint *bpWithBreak = (struct breakpoint *) NULL;
  struct breakpoint *bp;
  extern struct breakpoint *breakpoint_chain;


  for (bp = breakpoint_chain;
       (bp != (struct breakpoint *) NULL &&
	bpWithBreak == (struct breakpoint *) NULL);
       bp = bp->next)
    if (addr == bp->address)
      bpWithBreak = bp;

  return bpWithBreak;
}				/* _hasBreak */
