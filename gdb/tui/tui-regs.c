/* TUI display registers in window.

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
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "frame.h"
#include "regcache.h"
#include "inferior.h"
#include "target.h"
#include "gdb_string.h"
#include "tui/tui-layout.h"
#include "tui/tui-win.h"
#include "tui/tui-windata.h"
#include "tui/tui-wingeneral.h"
#include "tui/tui-file.h"

#ifdef HAVE_NCURSES_H       
#include <ncurses.h>
#else
#ifdef HAVE_CURSES_H
#include <curses.h>
#endif
#endif

/*****************************************
** LOCAL DEFINITIONS                    **
******************************************/
#define DOUBLE_FLOAT_LABEL_WIDTH    6
#define DOUBLE_FLOAT_LABEL_FMT      "%6.6s: "
#define DOUBLE_FLOAT_VALUE_WIDTH    30	/*min of 16 but may be in sci notation */

#define SINGLE_FLOAT_LABEL_WIDTH    6
#define SINGLE_FLOAT_LABEL_FMT      "%6.6s: "
#define SINGLE_FLOAT_VALUE_WIDTH    25	/* min of 8 but may be in sci notation */

#define SINGLE_LABEL_WIDTH    16
#define SINGLE_LABEL_FMT      "%10.10s: "
#define SINGLE_VALUE_WIDTH    20 /* minimum of 8 but may be in sci notation */

/* In the code HP gave Cygnus, this was actually a function call to a
   PA-specific function, which was supposed to determine whether the
   target was a 64-bit or 32-bit processor.  However, the 64-bit
   support wasn't complete, so we didn't merge that in, so we leave
   this here as a stub.  */
#define IS_64BIT 0

/*****************************************
** STATIC DATA                          **
******************************************/


/*****************************************
** STATIC LOCAL FUNCTIONS FORWARD DECLS    **
******************************************/
static enum tui_status _tuiSetRegsContent
  (int, int, struct frame_info *, enum tui_register_display_type, int);
static const char *_tuiRegisterName (int);
static enum tui_status _tuiGetRegisterRawValue (int, char *, struct frame_info *);
static void _tuiSetRegisterElement
  (int, struct frame_info *, struct tui_data_element *, int);
static void _tuiDisplayRegister (int, struct tui_gen_win_info *, enum precision_type);
static void _tuiRegisterFormat
  (char *, int, int, struct tui_data_element *, enum precision_type);
static enum tui_status _tuiSetGeneralRegsContent (int);
static enum tui_status _tuiSetSpecialRegsContent (int);
static enum tui_status _tuiSetGeneralAndSpecialRegsContent (int);
static enum tui_status _tuiSetFloatRegsContent (enum tui_register_display_type, int);
static int _tuiRegValueHasChanged
  (struct tui_data_element *, struct frame_info *, char *);
static void _tuiShowFloat_command (char *, int);
static void _tuiShowGeneral_command (char *, int);
static void _tuiShowSpecial_command (char *, int);
static void _tui_vShowRegisters_commandSupport (enum tui_register_display_type);
static void _tuiToggleFloatRegs_command (char *, int);
static void _tuiScrollRegsForward_command (char *, int);
static void _tuiScrollRegsBackward_command (char *, int);



/*****************************************
** PUBLIC FUNCTIONS                     **
******************************************/

/* Answer the number of the last line in the regs display.  If there
   are no registers (-1) is returned.  */
int
tui_last_regs_line_no (void)
{
  register int numLines = (-1);

  if (TUI_DATA_WIN->detail.data_display_info.regs_content_count > 0)
    {
      numLines = (TUI_DATA_WIN->detail.data_display_info.regs_content_count /
		  TUI_DATA_WIN->detail.data_display_info.regs_column_count);
      if (TUI_DATA_WIN->detail.data_display_info.regs_content_count %
	  TUI_DATA_WIN->detail.data_display_info.regs_column_count)
	numLines++;
    }
  return numLines;
}


/* Answer the line number that the register element at elementNo is
   on.  If elementNo is greater than the number of register elements
   there are, -1 is returned.  */
int
tui_line_from_reg_element_no (int elementNo)
{
  if (elementNo < TUI_DATA_WIN->detail.data_display_info.regs_content_count)
    {
      int i, line = (-1);

      i = 1;
      while (line == (-1))
	{
	  if (elementNo <
	      (TUI_DATA_WIN->detail.data_display_info.regs_column_count * i))
	    line = i - 1;
	  else
	    i++;
	}

      return line;
    }
  else
    return (-1);
}


/* Answer the index of the first element in lineNo.  If lineNo is past
   the register area (-1) is returned.  */
int
tui_first_reg_element_no_inline (int lineNo)
{
  if ((lineNo * TUI_DATA_WIN->detail.data_display_info.regs_column_count)
      <= TUI_DATA_WIN->detail.data_display_info.regs_content_count)
    return ((lineNo + 1) *
	    TUI_DATA_WIN->detail.data_display_info.regs_column_count) -
      TUI_DATA_WIN->detail.data_display_info.regs_column_count;
  else
    return (-1);
}


/*
   ** tuiLastRegElementNoInLine()
   **        Answer the index of the last element in lineNo.  If lineNo is past
   **        the register area (-1) is returned.
 */
int
tuiLastRegElementNoInLine (int lineNo)
{
  if ((lineNo * TUI_DATA_WIN->detail.data_display_info.regs_column_count) <=
      TUI_DATA_WIN->detail.data_display_info.regs_content_count)
    return ((lineNo + 1) *
	    TUI_DATA_WIN->detail.data_display_info.regs_column_count) - 1;
  else
    return (-1);
}				/* tuiLastRegElementNoInLine */


/* Calculate the number of columns that should be used to display the
   registers.  */
int
tui_calculate_regs_column_count (enum tui_register_display_type dpyType)
{
  int colCount, colWidth;

  if (IS_64BIT || dpyType == TUI_DFLOAT_REGS)
    colWidth = DOUBLE_FLOAT_VALUE_WIDTH + DOUBLE_FLOAT_LABEL_WIDTH;
  else
    {
      if (dpyType == TUI_SFLOAT_REGS)
	colWidth = SINGLE_FLOAT_VALUE_WIDTH + SINGLE_FLOAT_LABEL_WIDTH;
      else
	colWidth = SINGLE_VALUE_WIDTH + SINGLE_LABEL_WIDTH;
    }
  colCount = (TUI_DATA_WIN->generic.width - 2) / colWidth;

  return colCount;
}				/* tuiCalulateRegs_column_count */


/* Show the registers int the data window as indicated by dpyType.  If
   there is any other registers being displayed, then they are
   cleared.  What registers are displayed is dependent upon dpyType.  */
void
tui_show_registers (enum tui_register_display_type dpyType)
{
  enum tui_status ret = TUI_FAILURE;
  int refreshValuesOnly = FALSE;

  /* Say that registers should be displayed, even if there is a problem */
  TUI_DATA_WIN->detail.data_display_info.display_regs = TRUE;

  if (target_has_registers)
    {
      refreshValuesOnly =
	(dpyType == TUI_DATA_WIN->detail.data_display_info.regs_display_type);
      switch (dpyType)
	{
	case TUI_GENERAL_REGS:
	  ret = _tuiSetGeneralRegsContent (refreshValuesOnly);
	  break;
	case TUI_SFLOAT_REGS:
	case TUI_DFLOAT_REGS:
	  ret = _tuiSetFloatRegsContent (dpyType, refreshValuesOnly);
	  break;

/* could ifdef out */

	case TUI_SPECIAL_REGS:
	  ret = _tuiSetSpecialRegsContent (refreshValuesOnly);
	  break;
	case TUI_GENERAL_AND_SPECIAL_REGS:
	  ret = _tuiSetGeneralAndSpecialRegsContent (refreshValuesOnly);
	  break;

/* end of potential if def */

	default:
	  break;
	}
    }
  if (ret == TUI_FAILURE)
    {
      TUI_DATA_WIN->detail.data_display_info.regs_display_type = TUI_UNDEFINED_REGS;
      tui_erase_data_content (NO_REGS_STRING);
    }
  else
    {
      int i;

      /* Clear all notation of changed values */
      for (i = 0; (i < TUI_DATA_WIN->detail.data_display_info.regs_content_count); i++)
	{
	  struct tui_gen_win_info * dataItemWin;

	  dataItemWin = &TUI_DATA_WIN->detail.data_display_info.
	    regs_content[i]->which_element.data_window;
	  (&((struct tui_win_element *)
	     dataItemWin->content[0])->which_element.data)->highlight = FALSE;
	}
      TUI_DATA_WIN->detail.data_display_info.regs_display_type = dpyType;
      tui_display_all_data ();
    }
  (tui_layout_def ())->regs_display_type = dpyType;

  return;
}


/* Function to display the registers in the content from
   'startElementNo' until the end of the register content or the end
   of the display height.  No checking for displaying past the end of
   the registers is done here.  */
void
tui_display_registers_from (int startElementNo)
{
  if (TUI_DATA_WIN->detail.data_display_info.regs_content != (tui_win_content) NULL &&
      TUI_DATA_WIN->detail.data_display_info.regs_content_count > 0)
    {
      register int i = startElementNo;
      int j, valueCharsWide, itemWinWidth, curY, labelWidth;
      enum precision_type precision;

      precision = (TUI_DATA_WIN->detail.data_display_info.regs_display_type
		   == TUI_DFLOAT_REGS) ?
	double_precision : unspecified_precision;
      if (IS_64BIT ||
	  TUI_DATA_WIN->detail.data_display_info.regs_display_type == TUI_DFLOAT_REGS)
	{
	  valueCharsWide = DOUBLE_FLOAT_VALUE_WIDTH;
	  labelWidth = DOUBLE_FLOAT_LABEL_WIDTH;
	}
      else
	{
	  if (TUI_DATA_WIN->detail.data_display_info.regs_display_type ==
	      TUI_SFLOAT_REGS)
	    {
	      valueCharsWide = SINGLE_FLOAT_VALUE_WIDTH;
	      labelWidth = SINGLE_FLOAT_LABEL_WIDTH;
	    }
	  else
	    {
	      valueCharsWide = SINGLE_VALUE_WIDTH;
	      labelWidth = SINGLE_LABEL_WIDTH;
	    }
	}
      itemWinWidth = valueCharsWide + labelWidth;
      /*
         ** Now create each data "sub" window, and write the display into it.
       */
      curY = 1;
      while (i < TUI_DATA_WIN->detail.data_display_info.regs_content_count &&
	     curY <= TUI_DATA_WIN->generic.viewport_height)
	{
	  for (j = 0;
	       (j < TUI_DATA_WIN->detail.data_display_info.regs_column_count &&
		i < TUI_DATA_WIN->detail.data_display_info.regs_content_count); j++)
	    {
	      struct tui_gen_win_info * dataItemWin;
	      struct tui_data_element * dataElementPtr;

	      /* create the window if necessary */
	      dataItemWin = &TUI_DATA_WIN->detail.data_display_info.
		regs_content[i]->which_element.data_window;
	      dataElementPtr = &((struct tui_win_element *)
				 dataItemWin->content[0])->which_element.data;
	      if (dataItemWin->handle == (WINDOW *) NULL)
		{
		  dataItemWin->height = 1;
		  dataItemWin->width = (precision == double_precision) ?
		    itemWinWidth + 2 : itemWinWidth + 1;
		  dataItemWin->origin.x = (itemWinWidth * j) + 1;
		  dataItemWin->origin.y = curY;
		  tui_make_window (dataItemWin, DONT_BOX_WINDOW);
                  scrollok (dataItemWin->handle, FALSE);
		}
              touchwin (dataItemWin->handle);

	      /*
	         ** Get the printable representation of the register
	         ** and display it
	       */
	      _tuiDisplayRegister (
			    dataElementPtr->item_no, dataItemWin, precision);
	      i++;		/* next register */
	    }
	  curY++;		/* next row; */
	}
    }

  return;
}


/*
   ** tuiDisplayRegElementAtLine().
   **        Function to display the registers in the content from
   **        'startElementNo' on 'startLineNo' until the end of the
   **        register content or the end of the display height.
   **        This function checks that we won't display off the end
   **        of the register display.
 */
void
tuiDisplayRegElementAtLine (int startElementNo, int startLineNo)
{
  if (TUI_DATA_WIN->detail.data_display_info.regs_content != (tui_win_content) NULL &&
      TUI_DATA_WIN->detail.data_display_info.regs_content_count > 0)
    {
      register int elementNo = startElementNo;

      if (startElementNo != 0 && startLineNo != 0)
	{
	  register int lastLineNo, firstLineOnLastPage;

	  lastLineNo = tui_last_regs_line_no ();
	  firstLineOnLastPage = lastLineNo - (TUI_DATA_WIN->generic.height - 2);
	  if (firstLineOnLastPage < 0)
	    firstLineOnLastPage = 0;
	  /*
	     ** If there is no other data displayed except registers,
	     ** and the elementNo causes us to scroll past the end of the
	     ** registers, adjust what element to really start the display at.
	   */
	  if (TUI_DATA_WIN->detail.data_display_info.data_content_count <= 0 &&
	      startLineNo > firstLineOnLastPage)
	    elementNo = tui_first_reg_element_no_inline (firstLineOnLastPage);
	}
      tui_display_registers_from (elementNo);
    }

  return;
}				/* tuiDisplayRegElementAtLine */



/* Function to display the registers starting at line lineNo in the
   data window.  Answers the line number that the display actually
   started from.  If nothing is displayed (-1) is returned.  */
int
tui_display_registers_from_line (int lineNo, int forceDisplay)
{
  if (TUI_DATA_WIN->detail.data_display_info.regs_content_count > 0)
    {
      int line, elementNo;

      if (lineNo < 0)
	line = 0;
      else if (forceDisplay)
	{			/*
				   ** If we must display regs (forceDisplay is true), then make
				   ** sure that we don't display off the end of the registers.
				 */
	  if (lineNo >= tui_last_regs_line_no ())
	    {
	      if ((line = tui_line_from_reg_element_no (
		 TUI_DATA_WIN->detail.data_display_info.regs_content_count - 1)) < 0)
		line = 0;
	    }
	  else
	    line = lineNo;
	}
      else
	line = lineNo;

      elementNo = tui_first_reg_element_no_inline (line);
      if (elementNo < TUI_DATA_WIN->detail.data_display_info.regs_content_count)
	tuiDisplayRegElementAtLine (elementNo, line);
      else
	line = (-1);

      return line;
    }

  return (-1);			/* nothing was displayed */
}


/* This function check all displayed registers for changes in values,
   given a particular frame.  If the values have changed, they are
   updated with the new value and highlighted.  */
void
tui_check_register_values (struct frame_info *frame)
{
  if (TUI_DATA_WIN != NULL && TUI_DATA_WIN->generic.is_visible)
    {
      if (TUI_DATA_WIN->detail.data_display_info.regs_content_count <= 0 &&
	  TUI_DATA_WIN->detail.data_display_info.display_regs)
	tui_show_registers ((tui_layout_def ())->regs_display_type);
      else
	{
	  int i, j;
	  char rawBuf[MAX_REGISTER_SIZE];

	  for (i = 0;
	       (i < TUI_DATA_WIN->detail.data_display_info.regs_content_count); i++)
	    {
	      struct tui_data_element * dataElementPtr;
	      struct tui_gen_win_info * dataItemWinPtr;
	      int wasHilighted;

	      dataItemWinPtr = &TUI_DATA_WIN->detail.data_display_info.
		regs_content[i]->which_element.data_window;
	      dataElementPtr = &((struct tui_win_element *)
			     dataItemWinPtr->content[0])->which_element.data;
	      wasHilighted = dataElementPtr->highlight;
	      dataElementPtr->highlight =
		_tuiRegValueHasChanged (dataElementPtr, frame, &rawBuf[0]);
	      if (dataElementPtr->highlight)
		{
                  int size;

                  size = DEPRECATED_REGISTER_RAW_SIZE (dataElementPtr->item_no);
		  for (j = 0; j < size; j++)
		    ((char *) dataElementPtr->value)[j] = rawBuf[j];
		  _tuiDisplayRegister (
					dataElementPtr->item_no,
					dataItemWinPtr,
			((TUI_DATA_WIN->detail.data_display_info.regs_display_type ==
			  TUI_DFLOAT_REGS) ?
			 double_precision : unspecified_precision));
		}
	      else if (wasHilighted)
		{
		  dataElementPtr->highlight = FALSE;
		  _tuiDisplayRegister (
					dataElementPtr->item_no,
					dataItemWinPtr,
			((TUI_DATA_WIN->detail.data_display_info.regs_display_type ==
			  TUI_DFLOAT_REGS) ?
			 double_precision : unspecified_precision));
		}
	    }
	}
    }
  return;
}


/*
   ** tuiToggleFloatRegs().
 */
void
tuiToggleFloatRegs (void)
{
  struct tui_layout_def * layoutDef = tui_layout_def ();

  if (layoutDef->float_regs_display_type == TUI_SFLOAT_REGS)
    layoutDef->float_regs_display_type = TUI_DFLOAT_REGS;
  else
    layoutDef->float_regs_display_type = TUI_SFLOAT_REGS;

  if (TUI_DATA_WIN != NULL && TUI_DATA_WIN->generic.is_visible &&
      (TUI_DATA_WIN->detail.data_display_info.regs_display_type == TUI_SFLOAT_REGS ||
       TUI_DATA_WIN->detail.data_display_info.regs_display_type == TUI_DFLOAT_REGS))
    tui_show_registers (layoutDef->float_regs_display_type);

  return;
}				/* tuiToggleFloatRegs */


void
_initialize_tuiRegs (void)
{
  if (xdb_commands)
    {
      add_com ("fr", class_tui, _tuiShowFloat_command,
	       "Display only floating point registers\n");
      add_com ("gr", class_tui, _tuiShowGeneral_command,
	       "Display only general registers\n");
      add_com ("sr", class_tui, _tuiShowSpecial_command,
	       "Display only special registers\n");
      add_com ("+r", class_tui, _tuiScrollRegsForward_command,
	       "Scroll the registers window forward\n");
      add_com ("-r", class_tui, _tuiScrollRegsBackward_command,
	       "Scroll the register window backward\n");
      add_com ("tf", class_tui, _tuiToggleFloatRegs_command,
	       "Toggle between single and double precision floating point registers.\n");
      add_cmd (TUI_FLOAT_REGS_NAME_LOWER,
	       class_tui,
	       _tuiToggleFloatRegs_command,
	       "Toggle between single and double precision floating point \
registers.\n",
	       &togglelist);
    }
}


/*****************************************
** STATIC LOCAL FUNCTIONS                 **
******************************************/


/*
   ** _tuiRegisterName().
   **        Return the register name.
 */
static const char *
_tuiRegisterName (int regNum)
{
  return REGISTER_NAME (regNum);
}
extern int pagination_enabled;

static void
tui_restore_gdbout (void *ui)
{
  ui_file_delete (gdb_stdout);
  gdb_stdout = (struct ui_file*) ui;
  pagination_enabled = 1;
}

/*
   ** _tuiRegisterFormat
   **        Function to format the register name and value into a buffer,
   **        suitable for printing or display
 */
static void
_tuiRegisterFormat (char *buf, int bufLen, int regNum,
                    struct tui_data_element * dataElement,
                    enum precision_type precision)
{
  struct ui_file *stream;
  struct ui_file *old_stdout;
  const char *name;
  struct cleanup *cleanups;
  char *p;
  int pos;

  name = REGISTER_NAME (regNum);
  if (name == 0)
    {
      strcpy (buf, "");
      return;
    }
  
  pagination_enabled = 0;
  old_stdout = gdb_stdout;
  stream = tui_sfileopen (bufLen);
  gdb_stdout = stream;
  cleanups = make_cleanup (tui_restore_gdbout, (void*) old_stdout);
  gdbarch_print_registers_info (current_gdbarch, stream, deprecated_selected_frame,
                                regNum, 1);

  /* Save formatted output in the buffer.  */
  p = tui_file_get_strbuf (stream);
  pos = 0;
  while (*p && *p == *name++ && bufLen)
    {
      *buf++ = *p++;
      bufLen--;
      pos++;
    }
  while (*p == ' ')
    p++;
  while (pos < 8 && bufLen)
    {
      *buf++ = ' ';
      bufLen--;
      pos++;
    }
  strncpy (buf, p, bufLen);

  /* Remove the possible \n.  */
  p = strchr (buf, '\n');
  if (p)
    *p = 0;

  do_cleanups (cleanups);
}


#define NUM_GENERAL_REGS    32
/*
   ** _tuiSetGeneralRegsContent().
   **      Set the content of the data window to consist of the general registers.
 */
static enum tui_status
_tuiSetGeneralRegsContent (int refreshValuesOnly)
{
  return (_tuiSetRegsContent (0,
			      NUM_GENERAL_REGS - 1,
			      deprecated_selected_frame,
			      TUI_GENERAL_REGS,
			      refreshValuesOnly));

}				/* _tuiSetGeneralRegsContent */


#ifndef PCOQ_HEAD_REGNUM
#define START_SPECIAL_REGS  0
#else
#define START_SPECIAL_REGS    PCOQ_HEAD_REGNUM
#endif

/*
   ** _tuiSetSpecialRegsContent().
   **      Set the content of the data window to consist of the special registers.
 */
static enum tui_status
_tuiSetSpecialRegsContent (int refreshValuesOnly)
{
  enum tui_status ret = TUI_FAILURE;
  int endRegNum;

  endRegNum = FP0_REGNUM - 1;
  ret = _tuiSetRegsContent (START_SPECIAL_REGS,
			    endRegNum,
			    deprecated_selected_frame,
			    TUI_SPECIAL_REGS,
			    refreshValuesOnly);

  return ret;
}				/* _tuiSetSpecialRegsContent */


/*
   ** _tuiSetGeneralAndSpecialRegsContent().
   **      Set the content of the data window to consist of the special registers.
 */
static enum tui_status
_tuiSetGeneralAndSpecialRegsContent (int refreshValuesOnly)
{
  enum tui_status ret = TUI_FAILURE;
  int endRegNum = (-1);

  endRegNum = FP0_REGNUM - 1;
  ret = _tuiSetRegsContent (
	 0, endRegNum, deprecated_selected_frame, TUI_SPECIAL_REGS, refreshValuesOnly);

  return ret;
}				/* _tuiSetGeneralAndSpecialRegsContent */

/*
   ** _tuiSetFloatRegsContent().
   **        Set the content of the data window to consist of the float registers.
 */
static enum tui_status
_tuiSetFloatRegsContent (enum tui_register_display_type dpyType, int refreshValuesOnly)
{
  enum tui_status ret = TUI_FAILURE;
  int startRegNum;

  startRegNum = FP0_REGNUM;
  ret = _tuiSetRegsContent (startRegNum,
			    NUM_REGS - 1,
			    deprecated_selected_frame,
			    dpyType,
			    refreshValuesOnly);

  return ret;
}				/* _tuiSetFloatRegsContent */


/*
   ** _tuiRegValueHasChanged().
   **        Answer TRUE if the register's value has changed, FALSE otherwise.
   **        If TRUE, newValue is filled in with the new value.
 */
static int
_tuiRegValueHasChanged (struct tui_data_element * dataElement,
                        struct frame_info *frame,
                        char *newValue)
{
  int hasChanged = FALSE;

  if (dataElement->item_no != UNDEFINED_ITEM &&
      _tuiRegisterName (dataElement->item_no) != (char *) NULL)
    {
      char rawBuf[MAX_REGISTER_SIZE];
      int i;

      if (_tuiGetRegisterRawValue (
			 dataElement->item_no, rawBuf, frame) == TUI_SUCCESS)
	{
          int size = DEPRECATED_REGISTER_RAW_SIZE (dataElement->item_no);
          
	  for (i = 0; (i < size && !hasChanged); i++)
	    hasChanged = (((char *) dataElement->value)[i] != rawBuf[i]);
	  if (hasChanged && newValue != (char *) NULL)
	    {
	      for (i = 0; i < size; i++)
		newValue[i] = rawBuf[i];
	    }
	}
    }
  return hasChanged;
}				/* _tuiRegValueHasChanged */



/*
   ** _tuiGetRegisterRawValue().
   **        Get the register raw value.  The raw value is returned in regValue.
 */
static enum tui_status
_tuiGetRegisterRawValue (int regNum, char *regValue, struct frame_info *frame)
{
  enum tui_status ret = TUI_FAILURE;

  if (target_has_registers)
    {
      get_frame_register (frame, regNum, regValue);
      /* NOTE: cagney/2003-03-13: This is bogus.  It is refering to
         the register cache and not the frame which could have pulled
         the register value off the stack.  */
      if (register_cached (regNum) >= 0)
	ret = TUI_SUCCESS;
    }
  return ret;
}				/* _tuiGetRegisterRawValue */



/*
   ** _tuiSetRegisterElement().
   **       Function to initialize a data element with the input and
   **       the register value.
 */
static void
_tuiSetRegisterElement (int regNum, struct frame_info *frame,
                        struct tui_data_element * dataElement,
                        int refreshValueOnly)
{
  if (dataElement != (struct tui_data_element *) NULL)
    {
      if (!refreshValueOnly)
	{
	  dataElement->item_no = regNum;
	  dataElement->name = _tuiRegisterName (regNum);
	  dataElement->highlight = FALSE;
	}
      if (dataElement->value == NULL)
	dataElement->value = xmalloc (MAX_REGISTER_SIZE);
      if (dataElement->value != NULL)
	_tuiGetRegisterRawValue (regNum, dataElement->value, frame);
    }

  return;
}				/* _tuiSetRegisterElement */


/*
   ** _tuiSetRegsContent().
   **        Set the content of the data window to consist of the registers
   **        numbered from startRegNum to endRegNum.  Note that if
   **        refreshValuesOnly is TRUE, startRegNum and endRegNum are ignored.
 */
static enum tui_status
_tuiSetRegsContent (int startRegNum, int endRegNum,
                    struct frame_info *frame,
                    enum tui_register_display_type dpyType,
                    int refreshValuesOnly)
{
  enum tui_status ret = TUI_FAILURE;
  int numRegs = endRegNum - startRegNum + 1;
  int allocatedHere = FALSE;

  if (TUI_DATA_WIN->detail.data_display_info.regs_content_count > 0 &&
      !refreshValuesOnly)
    {
      tui_free_data_content (TUI_DATA_WIN->detail.data_display_info.regs_content,
			     TUI_DATA_WIN->detail.data_display_info.regs_content_count);
      TUI_DATA_WIN->detail.data_display_info.regs_content_count = 0;
    }
  if (TUI_DATA_WIN->detail.data_display_info.regs_content_count <= 0)
    {
      TUI_DATA_WIN->detail.data_display_info.regs_content =
	tui_alloc_content (numRegs, DATA_WIN);
      allocatedHere = TRUE;
    }

  if (TUI_DATA_WIN->detail.data_display_info.regs_content != (tui_win_content) NULL)
    {
      int i;

      if (!refreshValuesOnly || allocatedHere)
	{
	  TUI_DATA_WIN->generic.content = NULL;
	  TUI_DATA_WIN->generic.content_size = 0;
	  tui_add_content_elements (&TUI_DATA_WIN->generic, numRegs);
	  TUI_DATA_WIN->detail.data_display_info.regs_content =
	    (tui_win_content) TUI_DATA_WIN->generic.content;
	  TUI_DATA_WIN->detail.data_display_info.regs_content_count = numRegs;
	}
      /*
         ** Now set the register names and values
       */
      for (i = startRegNum; (i <= endRegNum); i++)
	{
	  struct tui_gen_win_info * dataItemWin;

	  dataItemWin = &TUI_DATA_WIN->detail.data_display_info.
	    regs_content[i - startRegNum]->which_element.data_window;
	  _tuiSetRegisterElement (
				   i,
				   frame,
	   &((struct tui_win_element *) dataItemWin->content[0])->which_element.data,
				   !allocatedHere && refreshValuesOnly);
	}
      TUI_DATA_WIN->detail.data_display_info.regs_column_count =
	tui_calculate_regs_column_count (dpyType);
#ifdef LATER
      if (TUI_DATA_WIN->detail.data_display_info.data_content_count > 0)
	{
	  /* delete all the windows? */
	  /* realloc content equal to data_content_count + regs_content_count */
	  /* append TUI_DATA_WIN->detail.data_display_info.dataContent to content */
	}
#endif
      TUI_DATA_WIN->generic.content_size =
	TUI_DATA_WIN->detail.data_display_info.regs_content_count +
	TUI_DATA_WIN->detail.data_display_info.data_content_count;
      ret = TUI_SUCCESS;
    }

  return ret;
}				/* _tuiSetRegsContent */


/*
   ** _tuiDisplayRegister().
   **        Function to display a register in a window.  If hilite is TRUE,
   **        than the value will be displayed in reverse video
 */
static void
_tuiDisplayRegister (int regNum,
                     struct tui_gen_win_info * winInfo,		/* the data item window */
                     enum precision_type precision)
{
  if (winInfo->handle != (WINDOW *) NULL)
    {
      int i;
      char buf[40];
      int valueCharsWide, labelWidth;
      struct tui_data_element * dataElementPtr = &((tui_win_content)
				    winInfo->content)[0]->which_element.data;

      if (IS_64BIT ||
	  TUI_DATA_WIN->detail.data_display_info.regs_display_type == TUI_DFLOAT_REGS)
	{
	  valueCharsWide = DOUBLE_FLOAT_VALUE_WIDTH;
	  labelWidth = DOUBLE_FLOAT_LABEL_WIDTH;
	}
      else
	{
	  if (TUI_DATA_WIN->detail.data_display_info.regs_display_type ==
	      TUI_SFLOAT_REGS)
	    {
	      valueCharsWide = SINGLE_FLOAT_VALUE_WIDTH;
	      labelWidth = SINGLE_FLOAT_LABEL_WIDTH;
	    }
	  else
	    {
	      valueCharsWide = SINGLE_VALUE_WIDTH;
	      labelWidth = SINGLE_LABEL_WIDTH;
	    }
	}

      buf[0] = (char) 0;
      _tuiRegisterFormat (buf,
			  valueCharsWide + labelWidth,
			  regNum,
			  dataElementPtr,
			  precision);

      if (dataElementPtr->highlight)
	wstandout (winInfo->handle);

      wmove (winInfo->handle, 0, 0);
      for (i = 1; i < winInfo->width; i++)
        waddch (winInfo->handle, ' ');
      wmove (winInfo->handle, 0, 0);
      waddstr (winInfo->handle, buf);

      if (dataElementPtr->highlight)
	wstandend (winInfo->handle);
      tui_refresh_win (winInfo);
    }
  return;
}				/* _tuiDisplayRegister */


static void
_tui_vShowRegisters_commandSupport (enum tui_register_display_type dpyType)
{

  if (TUI_DATA_WIN != NULL && TUI_DATA_WIN->generic.is_visible)
    {				/* Data window already displayed, show the registers */
      if (TUI_DATA_WIN->detail.data_display_info.regs_display_type != dpyType)
	tui_show_registers (dpyType);
    }
  else
    (tui_layout_def ())->regs_display_type = dpyType;

  return;
}				/* _tui_vShowRegisters_commandSupport */


static void
_tuiShowFloat_command (char *arg, int fromTTY)
{
  if (TUI_DATA_WIN == NULL || !TUI_DATA_WIN->generic.is_visible ||
      (TUI_DATA_WIN->detail.data_display_info.regs_display_type != TUI_SFLOAT_REGS &&
       TUI_DATA_WIN->detail.data_display_info.regs_display_type != TUI_DFLOAT_REGS))
    _tui_vShowRegisters_commandSupport ((tui_layout_def ())->float_regs_display_type);

  return;
}				/* _tuiShowFloat_command */


static void
_tuiShowGeneral_command (char *arg, int fromTTY)
{
  _tui_vShowRegisters_commandSupport (TUI_GENERAL_REGS);
}


static void
_tuiShowSpecial_command (char *arg, int fromTTY)
{
  _tui_vShowRegisters_commandSupport (TUI_SPECIAL_REGS);
}


static void
_tuiToggleFloatRegs_command (char *arg, int fromTTY)
{
  if (TUI_DATA_WIN != NULL && TUI_DATA_WIN->generic.is_visible)
    tuiToggleFloatRegs ();
  else
    {
      struct tui_layout_def * layoutDef = tui_layout_def ();

      if (layoutDef->float_regs_display_type == TUI_SFLOAT_REGS)
	layoutDef->float_regs_display_type = TUI_DFLOAT_REGS;
      else
	layoutDef->float_regs_display_type = TUI_SFLOAT_REGS;
    }


  return;
}				/* _tuiToggleFloatRegs_command */


static void
_tuiScrollRegsForward_command (char *arg, int fromTTY)
{
  tui_scroll (FORWARD_SCROLL, TUI_DATA_WIN, 1);
}


static void
_tuiScrollRegsBackward_command (char *arg, int fromTTY)
{
  tui_scroll (BACKWARD_SCROLL, TUI_DATA_WIN, 1);
}
