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

#include "gdb_curses.h"

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
static enum tui_status tui_set_regs_content
  (int, int, struct frame_info *, enum tui_register_display_type, int);
static const char *tui_register_name (int);
static enum tui_status tui_get_register_raw_value (int, char *, struct frame_info *);
static void tui_set_register_element
  (int, struct frame_info *, struct tui_data_element *, int);
static void tui_display_register (int, struct tui_gen_win_info *, enum precision_type);
static void tui_register_format
  (char *, int, int, struct tui_data_element *, enum precision_type);
static enum tui_status tui_set_general_regs_content (int);
static enum tui_status tui_set_special_regs_content (int);
static enum tui_status tui_set_general_and_special_regs_content (int);
static enum tui_status tui_set_float_regs_content (enum tui_register_display_type, int);
static int tui_reg_value_has_changed
  (struct tui_data_element *, struct frame_info *, char *);
static void tui_show_float_command (char *, int);
static void tui_show_general_command (char *, int);
static void tui_show_special_command (char *, int);
static void tui_v_show_registers_command_support (enum tui_register_display_type);
static void _tui_toggle_float_regs_command (char *, int);
static void tui_scroll_regs_forward_command (char *, int);
static void tui_scroll_regs_backward_command (char *, int);



/*****************************************
** PUBLIC FUNCTIONS                     **
******************************************/

/* Answer the number of the last line in the regs display.  If there
   are no registers (-1) is returned.  */
int
tui_last_regs_line_no (void)
{
  int num_lines = (-1);

  if (TUI_DATA_WIN->detail.data_display_info.regs_content_count > 0)
    {
      num_lines = (TUI_DATA_WIN->detail.data_display_info.regs_content_count /
		  TUI_DATA_WIN->detail.data_display_info.regs_column_count);
      if (TUI_DATA_WIN->detail.data_display_info.regs_content_count %
	  TUI_DATA_WIN->detail.data_display_info.regs_column_count)
	num_lines++;
    }
  return num_lines;
}


/* Answer the line number that the register element at element_no is
   on.  If element_no is greater than the number of register elements
   there are, -1 is returned.  */
int
tui_line_from_reg_element_no (int element_no)
{
  if (element_no < TUI_DATA_WIN->detail.data_display_info.regs_content_count)
    {
      int i, line = (-1);

      i = 1;
      while (line == (-1))
	{
	  if (element_no <
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


/* Answer the index of the first element in line_no.  If line_no is past
   the register area (-1) is returned.  */
int
tui_first_reg_element_no_inline (int line_no)
{
  if ((line_no * TUI_DATA_WIN->detail.data_display_info.regs_column_count)
      <= TUI_DATA_WIN->detail.data_display_info.regs_content_count)
    return ((line_no + 1) *
	    TUI_DATA_WIN->detail.data_display_info.regs_column_count) -
      TUI_DATA_WIN->detail.data_display_info.regs_column_count;
  else
    return (-1);
}


/* Answer the index of the last element in line_no.  If line_no is
   past the register area (-1) is returned.  */
int
tui_last_reg_element_no_in_line (int line_no)
{
  if ((line_no * TUI_DATA_WIN->detail.data_display_info.regs_column_count) <=
      TUI_DATA_WIN->detail.data_display_info.regs_content_count)
    return ((line_no + 1) *
	    TUI_DATA_WIN->detail.data_display_info.regs_column_count) - 1;
  else
    return (-1);
}


/* Calculate the number of columns that should be used to display the
   registers.  */
int
tui_calculate_regs_column_count (enum tui_register_display_type dpy_type)
{
  int col_count, col_width;

  if (IS_64BIT || dpy_type == TUI_DFLOAT_REGS)
    col_width = DOUBLE_FLOAT_VALUE_WIDTH + DOUBLE_FLOAT_LABEL_WIDTH;
  else
    {
      if (dpy_type == TUI_SFLOAT_REGS)
	col_width = SINGLE_FLOAT_VALUE_WIDTH + SINGLE_FLOAT_LABEL_WIDTH;
      else
	col_width = SINGLE_VALUE_WIDTH + SINGLE_LABEL_WIDTH;
    }
  col_count = (TUI_DATA_WIN->generic.width - 2) / col_width;

  return col_count;
}


/* Show the registers int the data window as indicated by dpy_type.  If
   there is any other registers being displayed, then they are
   cleared.  What registers are displayed is dependent upon dpy_type.  */
void
tui_show_registers (enum tui_register_display_type dpy_type)
{
  enum tui_status ret = TUI_FAILURE;
  int refresh_values_only = FALSE;

  /* Say that registers should be displayed, even if there is a problem */
  TUI_DATA_WIN->detail.data_display_info.display_regs = TRUE;

  if (target_has_registers)
    {
      refresh_values_only =
	(dpy_type == TUI_DATA_WIN->detail.data_display_info.regs_display_type);
      switch (dpy_type)
	{
	case TUI_GENERAL_REGS:
	  ret = tui_set_general_regs_content (refresh_values_only);
	  break;
	case TUI_SFLOAT_REGS:
	case TUI_DFLOAT_REGS:
	  ret = tui_set_float_regs_content (dpy_type, refresh_values_only);
	  break;

/* could ifdef out */

	case TUI_SPECIAL_REGS:
	  ret = tui_set_special_regs_content (refresh_values_only);
	  break;
	case TUI_GENERAL_AND_SPECIAL_REGS:
	  ret = tui_set_general_and_special_regs_content (refresh_values_only);
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
	  struct tui_gen_win_info * data_item_win;

	  data_item_win = &TUI_DATA_WIN->detail.data_display_info.
	    regs_content[i]->which_element.data_window;
	  (&((struct tui_win_element *)
	     data_item_win->content[0])->which_element.data)->highlight = FALSE;
	}
      TUI_DATA_WIN->detail.data_display_info.regs_display_type = dpy_type;
      tui_display_all_data ();
    }
  (tui_layout_def ())->regs_display_type = dpy_type;

  return;
}


/* Function to display the registers in the content from
   'start_element_no' until the end of the register content or the end
   of the display height.  No checking for displaying past the end of
   the registers is done here.  */
void
tui_display_registers_from (int start_element_no)
{
  if (TUI_DATA_WIN->detail.data_display_info.regs_content != (tui_win_content) NULL &&
      TUI_DATA_WIN->detail.data_display_info.regs_content_count > 0)
    {
      int i = start_element_no;
      int j, value_chars_wide, item_win_width, cur_y;
      enum precision_type precision;

      /* Do not rename the following variable into "label_width".
         Unfortunately, term.h on AiX systems defines a macro with
         the same name, which causes a build failure if we use the
         same name for this variable.  */
      int tui_label_width;

      precision = (TUI_DATA_WIN->detail.data_display_info.regs_display_type
		   == TUI_DFLOAT_REGS) ?
	double_precision : unspecified_precision;
      if (IS_64BIT ||
	  TUI_DATA_WIN->detail.data_display_info.regs_display_type == TUI_DFLOAT_REGS)
	{
	  value_chars_wide = DOUBLE_FLOAT_VALUE_WIDTH;
	  tui_label_width = DOUBLE_FLOAT_LABEL_WIDTH;
	}
      else
	{
	  if (TUI_DATA_WIN->detail.data_display_info.regs_display_type ==
	      TUI_SFLOAT_REGS)
	    {
	      value_chars_wide = SINGLE_FLOAT_VALUE_WIDTH;
	      tui_label_width = SINGLE_FLOAT_LABEL_WIDTH;
	    }
	  else
	    {
	      value_chars_wide = SINGLE_VALUE_WIDTH;
	      tui_label_width = SINGLE_LABEL_WIDTH;
	    }
	}
      item_win_width = value_chars_wide + tui_label_width;
      /*
         ** Now create each data "sub" window, and write the display into it.
       */
      cur_y = 1;
      while (i < TUI_DATA_WIN->detail.data_display_info.regs_content_count &&
	     cur_y <= TUI_DATA_WIN->generic.viewport_height)
	{
	  for (j = 0;
	       (j < TUI_DATA_WIN->detail.data_display_info.regs_column_count &&
		i < TUI_DATA_WIN->detail.data_display_info.regs_content_count); j++)
	    {
	      struct tui_gen_win_info * data_item_win;
	      struct tui_data_element * data_element_ptr;

	      /* create the window if necessary */
	      data_item_win = &TUI_DATA_WIN->detail.data_display_info.
		regs_content[i]->which_element.data_window;
	      data_element_ptr = &((struct tui_win_element *)
				 data_item_win->content[0])->which_element.data;
	      if (data_item_win->handle == (WINDOW *) NULL)
		{
		  data_item_win->height = 1;
		  data_item_win->width = (precision == double_precision) ?
		    item_win_width + 2 : item_win_width + 1;
		  data_item_win->origin.x = (item_win_width * j) + 1;
		  data_item_win->origin.y = cur_y;
		  tui_make_window (data_item_win, DONT_BOX_WINDOW);
                  scrollok (data_item_win->handle, FALSE);
		}
              touchwin (data_item_win->handle);

	      /*
	         ** Get the printable representation of the register
	         ** and display it
	       */
	      tui_display_register (
			    data_element_ptr->item_no, data_item_win, precision);
	      i++;		/* next register */
	    }
	  cur_y++;		/* next row; */
	}
    }

  return;
}


/* Function to display the registers in the content from
   'start_element_no' on 'start_line_no' until the end of the register
   content or the end of the display height.  This function checks
   that we won't display off the end of the register display.  */
void
tui_display_reg_element_at_line (int start_element_no, int start_line_no)
{
  if (TUI_DATA_WIN->detail.data_display_info.regs_content != (tui_win_content) NULL &&
      TUI_DATA_WIN->detail.data_display_info.regs_content_count > 0)
    {
      int element_no = start_element_no;

      if (start_element_no != 0 && start_line_no != 0)
	{
	  int last_line_no, first_line_on_last_page;

	  last_line_no = tui_last_regs_line_no ();
	  first_line_on_last_page = last_line_no - (TUI_DATA_WIN->generic.height - 2);
	  if (first_line_on_last_page < 0)
	    first_line_on_last_page = 0;
	  /*
	     ** If there is no other data displayed except registers,
	     ** and the element_no causes us to scroll past the end of the
	     ** registers, adjust what element to really start the display at.
	   */
	  if (TUI_DATA_WIN->detail.data_display_info.data_content_count <= 0 &&
	      start_line_no > first_line_on_last_page)
	    element_no = tui_first_reg_element_no_inline (first_line_on_last_page);
	}
      tui_display_registers_from (element_no);
    }
}



/* Function to display the registers starting at line line_no in the
   data window.  Answers the line number that the display actually
   started from.  If nothing is displayed (-1) is returned.  */
int
tui_display_registers_from_line (int line_no, int force_display)
{
  if (TUI_DATA_WIN->detail.data_display_info.regs_content_count > 0)
    {
      int line, element_no;

      if (line_no < 0)
	line = 0;
      else if (force_display)
	{			/*
				   ** If we must display regs (force_display is true), then make
				   ** sure that we don't display off the end of the registers.
				 */
	  if (line_no >= tui_last_regs_line_no ())
	    {
	      if ((line = tui_line_from_reg_element_no (
		 TUI_DATA_WIN->detail.data_display_info.regs_content_count - 1)) < 0)
		line = 0;
	    }
	  else
	    line = line_no;
	}
      else
	line = line_no;

      element_no = tui_first_reg_element_no_inline (line);
      if (element_no < TUI_DATA_WIN->detail.data_display_info.regs_content_count)
	tui_display_reg_element_at_line (element_no, line);
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
	  char raw_buf[MAX_REGISTER_SIZE];

	  for (i = 0;
	       (i < TUI_DATA_WIN->detail.data_display_info.regs_content_count); i++)
	    {
	      struct tui_data_element * data_element_ptr;
	      struct tui_gen_win_info * data_item_win_ptr;
	      int was_hilighted;

	      data_item_win_ptr = &TUI_DATA_WIN->detail.data_display_info.
		regs_content[i]->which_element.data_window;
	      data_element_ptr = &((struct tui_win_element *)
			     data_item_win_ptr->content[0])->which_element.data;
	      was_hilighted = data_element_ptr->highlight;
	      data_element_ptr->highlight =
		tui_reg_value_has_changed (data_element_ptr, frame, &raw_buf[0]);
	      if (data_element_ptr->highlight)
		{
                  int size;

                  size = DEPRECATED_REGISTER_RAW_SIZE (data_element_ptr->item_no);
		  for (j = 0; j < size; j++)
		    ((char *) data_element_ptr->value)[j] = raw_buf[j];
		  tui_display_register (
					data_element_ptr->item_no,
					data_item_win_ptr,
			((TUI_DATA_WIN->detail.data_display_info.regs_display_type ==
			  TUI_DFLOAT_REGS) ?
			 double_precision : unspecified_precision));
		}
	      else if (was_hilighted)
		{
		  data_element_ptr->highlight = FALSE;
		  tui_display_register (
					data_element_ptr->item_no,
					data_item_win_ptr,
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
   ** tui_toggle_float_regs().
 */
void
tui_toggle_float_regs (void)
{
  struct tui_layout_def * layout_def = tui_layout_def ();

  if (layout_def->float_regs_display_type == TUI_SFLOAT_REGS)
    layout_def->float_regs_display_type = TUI_DFLOAT_REGS;
  else
    layout_def->float_regs_display_type = TUI_SFLOAT_REGS;

  if (TUI_DATA_WIN != NULL && TUI_DATA_WIN->generic.is_visible &&
      (TUI_DATA_WIN->detail.data_display_info.regs_display_type == TUI_SFLOAT_REGS ||
       TUI_DATA_WIN->detail.data_display_info.regs_display_type == TUI_DFLOAT_REGS))
    tui_show_registers (layout_def->float_regs_display_type);

  return;
}				/* tui_toggle_float_regs */


void
_initialize_tui_regs (void)
{
  if (xdb_commands)
    {
      add_com ("fr", class_tui, tui_show_float_command,
	       "Display only floating point registers\n");
      add_com ("gr", class_tui, tui_show_general_command,
	       "Display only general registers\n");
      add_com ("sr", class_tui, tui_show_special_command,
	       "Display only special registers\n");
      add_com ("+r", class_tui, tui_scroll_regs_forward_command,
	       "Scroll the registers window forward\n");
      add_com ("-r", class_tui, tui_scroll_regs_backward_command,
	       "Scroll the register window backward\n");
      add_com ("tf", class_tui, _tui_toggle_float_regs_command,
	       "Toggle between single and double precision floating point registers.\n");
      add_cmd (TUI_FLOAT_REGS_NAME_LOWER,
	       class_tui,
	       _tui_toggle_float_regs_command,
	       "Toggle between single and double precision floating point \
registers.\n",
	       &togglelist);
    }
}


/*****************************************
** STATIC LOCAL FUNCTIONS                 **
******************************************/


/*
   ** tui_register_name().
   **        Return the register name.
 */
static const char *
tui_register_name (int reg_num)
{
  return REGISTER_NAME (reg_num);
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
   ** tui_register_format
   **        Function to format the register name and value into a buffer,
   **        suitable for printing or display
 */
static void
tui_register_format (char *buf, int buf_len, int reg_num,
                    struct tui_data_element * data_element,
                    enum precision_type precision)
{
  struct ui_file *stream;
  struct ui_file *old_stdout;
  const char *name;
  struct cleanup *cleanups;
  char *p;
  int pos;

  name = REGISTER_NAME (reg_num);
  if (name == 0)
    {
      strcpy (buf, "");
      return;
    }
  
  pagination_enabled = 0;
  old_stdout = gdb_stdout;
  stream = tui_sfileopen (buf_len);
  gdb_stdout = stream;
  cleanups = make_cleanup (tui_restore_gdbout, (void*) old_stdout);
  gdbarch_print_registers_info (current_gdbarch, stream, deprecated_selected_frame,
                                reg_num, 1);

  /* Save formatted output in the buffer.  */
  p = tui_file_get_strbuf (stream);
  pos = 0;
  while (*p && *p == *name++ && buf_len)
    {
      *buf++ = *p++;
      buf_len--;
      pos++;
    }
  while (*p == ' ')
    p++;
  while (pos < 8 && buf_len)
    {
      *buf++ = ' ';
      buf_len--;
      pos++;
    }
  strncpy (buf, p, buf_len);

  /* Remove the possible \n.  */
  p = strchr (buf, '\n');
  if (p)
    *p = 0;

  do_cleanups (cleanups);
}


#define NUM_GENERAL_REGS    32
/* Set the content of the data window to consist of the general
   registers.  */
static enum tui_status
tui_set_general_regs_content (int refresh_values_only)
{
  return (tui_set_regs_content (0,
			      NUM_GENERAL_REGS - 1,
			      deprecated_selected_frame,
			      TUI_GENERAL_REGS,
			      refresh_values_only));

}


#ifndef PCOQ_HEAD_REGNUM
#define START_SPECIAL_REGS  0
#else
#define START_SPECIAL_REGS    PCOQ_HEAD_REGNUM
#endif

/* Set the content of the data window to consist of the special
   registers.  */
static enum tui_status
tui_set_special_regs_content (int refresh_values_only)
{
  enum tui_status ret = TUI_FAILURE;
  int end_reg_num;

  end_reg_num = FP0_REGNUM - 1;
  ret = tui_set_regs_content (START_SPECIAL_REGS,
			    end_reg_num,
			    deprecated_selected_frame,
			    TUI_SPECIAL_REGS,
			    refresh_values_only);

  return ret;
}


/* Set the content of the data window to consist of the special
   registers.  */
static enum tui_status
tui_set_general_and_special_regs_content (int refresh_values_only)
{
  enum tui_status ret = TUI_FAILURE;
  int end_reg_num = (-1);

  end_reg_num = FP0_REGNUM - 1;
  ret = tui_set_regs_content (
	 0, end_reg_num, deprecated_selected_frame, TUI_SPECIAL_REGS, refresh_values_only);

  return ret;
}

/* Set the content of the data window to consist of the float
   registers.  */
static enum tui_status
tui_set_float_regs_content (enum tui_register_display_type dpy_type,
			    int refresh_values_only)
{
  enum tui_status ret = TUI_FAILURE;
  int start_reg_num;

  start_reg_num = FP0_REGNUM;
  ret = tui_set_regs_content (start_reg_num,
			    NUM_REGS - 1,
			    deprecated_selected_frame,
			    dpy_type,
			    refresh_values_only);

  return ret;
}


/* Answer TRUE if the register's value has changed, FALSE otherwise.
   If TRUE, new_value is filled in with the new value.  */
static int
tui_reg_value_has_changed (struct tui_data_element * data_element,
			   struct frame_info *frame, char *new_value)
{
  int has_changed = FALSE;

  if (data_element->item_no != UNDEFINED_ITEM &&
      tui_register_name (data_element->item_no) != (char *) NULL)
    {
      char raw_buf[MAX_REGISTER_SIZE];
      int i;

      if (tui_get_register_raw_value (data_element->item_no, raw_buf, frame) == TUI_SUCCESS)
	{
          int size = DEPRECATED_REGISTER_RAW_SIZE (data_element->item_no);
          
	  for (i = 0; (i < size && !has_changed); i++)
	    has_changed = (((char *) data_element->value)[i] != raw_buf[i]);
	  if (has_changed && new_value != (char *) NULL)
	    {
	      for (i = 0; i < size; i++)
		new_value[i] = raw_buf[i];
	    }
	}
    }
  return has_changed;
}



/* Get the register raw value.  The raw value is returned in reg_value.  */
static enum tui_status
tui_get_register_raw_value (int reg_num, char *reg_value, struct frame_info *frame)
{
  enum tui_status ret = TUI_FAILURE;

  if (target_has_registers)
    {
      get_frame_register (frame, reg_num, reg_value);
      /* NOTE: cagney/2003-03-13: This is bogus.  It is refering to
         the register cache and not the frame which could have pulled
         the register value off the stack.  */
      if (register_cached (reg_num) >= 0)
	ret = TUI_SUCCESS;
    }
  return ret;
}



/* Function to initialize a data element with the input and the
   register value.  */
static void
tui_set_register_element (int reg_num, struct frame_info *frame,
			  struct tui_data_element * data_element,
			  int refresh_value_only)
{
  if (data_element != (struct tui_data_element *) NULL)
    {
      if (!refresh_value_only)
	{
	  data_element->item_no = reg_num;
	  data_element->name = tui_register_name (reg_num);
	  data_element->highlight = FALSE;
	}
      if (data_element->value == NULL)
	data_element->value = xmalloc (MAX_REGISTER_SIZE);
      if (data_element->value != NULL)
	tui_get_register_raw_value (reg_num, data_element->value, frame);
    }
}


/* Set the content of the data window to consist of the registers
   numbered from start_reg_num to end_reg_num.  Note that if
   refresh_values_only is TRUE, start_reg_num and end_reg_num are
   ignored.  */
static enum tui_status
tui_set_regs_content (int start_reg_num, int end_reg_num,
                    struct frame_info *frame,
                    enum tui_register_display_type dpy_type,
                    int refresh_values_only)
{
  enum tui_status ret = TUI_FAILURE;
  int num_regs = end_reg_num - start_reg_num + 1;
  int allocated_here = FALSE;

  if (TUI_DATA_WIN->detail.data_display_info.regs_content_count > 0 &&
      !refresh_values_only)
    {
      tui_free_data_content (TUI_DATA_WIN->detail.data_display_info.regs_content,
			     TUI_DATA_WIN->detail.data_display_info.regs_content_count);
      TUI_DATA_WIN->detail.data_display_info.regs_content_count = 0;
    }
  if (TUI_DATA_WIN->detail.data_display_info.regs_content_count <= 0)
    {
      TUI_DATA_WIN->detail.data_display_info.regs_content =
	tui_alloc_content (num_regs, DATA_WIN);
      allocated_here = TRUE;
    }

  if (TUI_DATA_WIN->detail.data_display_info.regs_content != (tui_win_content) NULL)
    {
      int i;

      if (!refresh_values_only || allocated_here)
	{
	  TUI_DATA_WIN->generic.content = NULL;
	  TUI_DATA_WIN->generic.content_size = 0;
	  tui_add_content_elements (&TUI_DATA_WIN->generic, num_regs);
	  TUI_DATA_WIN->detail.data_display_info.regs_content =
	    (tui_win_content) TUI_DATA_WIN->generic.content;
	  TUI_DATA_WIN->detail.data_display_info.regs_content_count = num_regs;
	}
      /*
         ** Now set the register names and values
       */
      for (i = start_reg_num; (i <= end_reg_num); i++)
	{
	  struct tui_gen_win_info * data_item_win;

	  data_item_win = &TUI_DATA_WIN->detail.data_display_info.
	    regs_content[i - start_reg_num]->which_element.data_window;
	  tui_set_register_element (
				   i,
				   frame,
	   &((struct tui_win_element *) data_item_win->content[0])->which_element.data,
				   !allocated_here && refresh_values_only);
	}
      TUI_DATA_WIN->detail.data_display_info.regs_column_count =
	tui_calculate_regs_column_count (dpy_type);
#ifdef LATER
      if (TUI_DATA_WIN->detail.data_display_info.data_content_count > 0)
	{
	  /* delete all the windows? */
	  /* realloc content equal to data_content_count + regs_content_count */
	  /* append TUI_DATA_WIN->detail.data_display_info.data_content to content */
	}
#endif
      TUI_DATA_WIN->generic.content_size =
	TUI_DATA_WIN->detail.data_display_info.regs_content_count +
	TUI_DATA_WIN->detail.data_display_info.data_content_count;
      ret = TUI_SUCCESS;
    }

  return ret;
}


/* Function to display a register in a window.  If hilite is TRUE,
   than the value will be displayed in reverse video.  */
static void
tui_display_register (int reg_num,
                     struct tui_gen_win_info * win_info,		/* the data item window */
                     enum precision_type precision)
{
  if (win_info->handle != (WINDOW *) NULL)
    {
      int i;
      char buf[40];
      int value_chars_wide;
      struct tui_data_element * data_element_ptr = &((tui_win_content)
				    win_info->content)[0]->which_element.data;

      /* Do not rename the following variable into "label_width".
         Unfortunately, term.h on AiX systems defines a macro with
         the same name, which causes a build failure if we use the
         same name for this variable.  */
      int tui_label_width;

      if (IS_64BIT ||
	  TUI_DATA_WIN->detail.data_display_info.regs_display_type == TUI_DFLOAT_REGS)
	{
	  value_chars_wide = DOUBLE_FLOAT_VALUE_WIDTH;
	  tui_label_width = DOUBLE_FLOAT_LABEL_WIDTH;
	}
      else
	{
	  if (TUI_DATA_WIN->detail.data_display_info.regs_display_type ==
	      TUI_SFLOAT_REGS)
	    {
	      value_chars_wide = SINGLE_FLOAT_VALUE_WIDTH;
	      tui_label_width = SINGLE_FLOAT_LABEL_WIDTH;
	    }
	  else
	    {
	      value_chars_wide = SINGLE_VALUE_WIDTH;
	      tui_label_width = SINGLE_LABEL_WIDTH;
	    }
	}

      buf[0] = (char) 0;
      tui_register_format (buf,
			  value_chars_wide + tui_label_width,
			  reg_num,
			  data_element_ptr,
			  precision);

      if (data_element_ptr->highlight)
	wstandout (win_info->handle);

      wmove (win_info->handle, 0, 0);
      for (i = 1; i < win_info->width; i++)
        waddch (win_info->handle, ' ');
      wmove (win_info->handle, 0, 0);
      waddstr (win_info->handle, buf);

      if (data_element_ptr->highlight)
	wstandend (win_info->handle);
      tui_refresh_win (win_info);
    }
}


static void
tui_v_show_registers_command_support (enum tui_register_display_type dpy_type)
{

  if (TUI_DATA_WIN != NULL && TUI_DATA_WIN->generic.is_visible)
    {				/* Data window already displayed, show the registers */
      if (TUI_DATA_WIN->detail.data_display_info.regs_display_type != dpy_type)
	tui_show_registers (dpy_type);
    }
  else
    (tui_layout_def ())->regs_display_type = dpy_type;

  return;
}


static void
tui_show_float_command (char *arg, int from_tty)
{
  if (TUI_DATA_WIN == NULL || !TUI_DATA_WIN->generic.is_visible ||
      (TUI_DATA_WIN->detail.data_display_info.regs_display_type != TUI_SFLOAT_REGS &&
       TUI_DATA_WIN->detail.data_display_info.regs_display_type != TUI_DFLOAT_REGS))
    tui_v_show_registers_command_support ((tui_layout_def ())->float_regs_display_type);
}


static void
tui_show_general_command (char *arg, int from_tty)
{
  tui_v_show_registers_command_support (TUI_GENERAL_REGS);
}


static void
tui_show_special_command (char *arg, int from_tty)
{
  tui_v_show_registers_command_support (TUI_SPECIAL_REGS);
}


static void
_tui_toggle_float_regs_command (char *arg, int from_tty)
{
  if (TUI_DATA_WIN != NULL && TUI_DATA_WIN->generic.is_visible)
    tui_toggle_float_regs ();
  else
    {
      struct tui_layout_def * layout_def = tui_layout_def ();

      if (layout_def->float_regs_display_type == TUI_SFLOAT_REGS)
	layout_def->float_regs_display_type = TUI_DFLOAT_REGS;
      else
	layout_def->float_regs_display_type = TUI_SFLOAT_REGS;
    }
}


static void
tui_scroll_regs_forward_command (char *arg, int from_tty)
{
  tui_scroll (FORWARD_SCROLL, TUI_DATA_WIN, 1);
}


static void
tui_scroll_regs_backward_command (char *arg, int from_tty)
{
  tui_scroll (BACKWARD_SCROLL, TUI_DATA_WIN, 1);
}
