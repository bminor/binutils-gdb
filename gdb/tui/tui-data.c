/* TUI data manipulation routines.

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
#include "symtab.h"
#include "tui/tui.h"
#include "tui/tui-data.h"
#include "tui/tui-wingeneral.h"

#include "gdb_string.h"

#ifdef HAVE_NCURSES_H       
#include <ncurses.h>
#else
#ifdef HAVE_CURSES_H
#include <curses.h>
#endif
#endif

/****************************
** GLOBAL DECLARATIONS
****************************/
struct tui_win_info *(tui_win_list[MAX_MAJOR_WINDOWS]);

/***************************
** Private data
****************************/
static enum tui_layout_type _currentLayout = UNDEFINED_LAYOUT;
static int _termHeight, _termWidth;
static struct tui_gen_win_info _locator;
static struct tui_gen_win_info _execInfo[2];
static struct tui_win_info * _srcWinList[2];
static struct tui_list _sourceWindows = {(void **) _srcWinList, 0};
static int _defaultTabLen = DEFAULT_TAB_LEN;
static struct tui_win_info * _winWithFocus = (struct tui_win_info *) NULL;
static struct tui_layout_def _layoutDef =
{SRC_WIN,			/* displayMode */
 FALSE,				/* split */
 TUI_UNDEFINED_REGS,		/* regsDisplayType */
 TUI_SFLOAT_REGS};		/* floatRegsDisplayType */
static int _winResized = FALSE;


/*********************************
** Static function forward decls
**********************************/
static void freeContent (tui_win_content, int, enum tui_win_type);
static void freeContentElements (tui_win_content, int, enum tui_win_type);



/*********************************
** PUBLIC FUNCTIONS
**********************************/

int
tui_win_is_source_type (enum tui_win_type win_type)
{
  return (win_type == SRC_WIN || win_type == DISASSEM_WIN);
}

int
tui_win_is_auxillary (enum tui_win_type win_type)
{
  return (win_type > MAX_MAJOR_WINDOWS);
}

int
tui_win_has_locator (struct tui_win_info *win_info)
{
  return (win_info != NULL \
	  && win_info->detail.source_info.has_locator);
}

void
tui_set_win_highlight (struct tui_win_info *win_info, int highlight)
{
  if (win_info != NULL)
    win_info->is_highlighted = highlight;
}

/******************************************
** ACCESSORS & MUTATORS FOR PRIVATE DATA
******************************************/

/* Answer a whether the terminal window has been resized or not.   */
int
tui_win_resized (void)
{
  return _winResized;
}


/* Set a whether the terminal window has been resized or not.   */
void
tui_set_win_resized_to (int resized)
{
  _winResized = resized;
}


/* Answer a pointer to the current layout definition.   */
struct tui_layout_def *
tui_layout_def (void)
{
  return &_layoutDef;
}


/* Answer the window with the logical focus.    */
struct tui_win_info *
tui_win_with_focus (void)
{
  return _winWithFocus;
}


/* Set the window that has the logical focus.   */
void
tui_set_win_with_focus (struct tui_win_info * win_info)
{
  _winWithFocus = win_info;
}


/* Answer the length in chars, of tabs.    */
int
tui_default_tab_len (void)
{
  return _defaultTabLen;
}


/* Set the length in chars, of tabs.   */
void
tui_set_default_tab_len (int len)
{
  _defaultTabLen = len;
}


/*
   ** currentSourceWin()
   **        Accessor for the current source window.  Usually there is only
   **        one source window (either source or disassembly), but both can
   **        be displayed at the same time.
 */
struct tui_list *
tui_source_windows (void)
{
  return &_sourceWindows;
}


/* Clear the list of source windows.  Usually there is only one source
   window (either source or disassembly), but both can be displayed at
   the same time.  */
void
tui_clear_source_windows (void)
{
  _sourceWindows.list[0] = NULL;
  _sourceWindows.list[1] = NULL;
  _sourceWindows.count = 0;
}


/* Clear the pertinant detail in the source windows.   */
void
tui_clear_source_windows_detail (void)
{
  int i;

  for (i = 0; i < (tui_source_windows ())->count; i++)
    tui_clear_win_detail ((struct tui_win_info *) (tui_source_windows ())->list[i]);
}


/* Add a window to the list of source windows.  Usually there is only
   one source window (either source or disassembly), but both can be
   displayed at the same time.  */
void
tui_add_to_source_windows (struct tui_win_info * win_info)
{
  if (_sourceWindows.count < 2)
    _sourceWindows.list[_sourceWindows.count++] = (void *) win_info;
}


/* Clear the pertinant detail in the windows.   */
void
tui_clear_win_detail (struct tui_win_info * win_info)
{
  if (win_info != NULL)
    {
      switch (win_info->generic.type)
	{
	case SRC_WIN:
	case DISASSEM_WIN:
	  win_info->detail.source_info.start_line_or_addr.addr = 0;
	  win_info->detail.source_info.horizontal_offset = 0;
	  break;
	case CMD_WIN:
	  win_info->detail.command_info.cur_line =
	    win_info->detail.command_info.curch = 0;
	  break;
	case DATA_WIN:
	  win_info->detail.data_display_info.data_content =
	    (tui_win_content) NULL;
	  win_info->detail.data_display_info.data_content_count = 0;
	  win_info->detail.data_display_info.regs_content =
	    (tui_win_content) NULL;
	  win_info->detail.data_display_info.regs_content_count = 0;
	  win_info->detail.data_display_info.regs_display_type =
	    TUI_UNDEFINED_REGS;
	  win_info->detail.data_display_info.regs_column_count = 1;
	  win_info->detail.data_display_info.display_regs = FALSE;
	  break;
	default:
	  break;
	}
    }

  return;
}				/* clearWinDetail */


/*
   ** sourceExecInfoPtr().
   **        Accessor for the source execution info ptr.
 */
struct tui_gen_win_info *
tui_source_exec_info_win_ptr (void)
{
  return &_execInfo[0];
}				/* sourceExecInfoWinPtr */


/*
   ** disassemExecInfoPtr().
   **        Accessor for the disassem execution info ptr.
 */
struct tui_gen_win_info *
tui_disassem_exec_info_win_ptr (void)
{
  return &_execInfo[1];
}				/* disassemExecInfoWinPtr */


/* Accessor for the locator win info.  Answers a pointer to the static
   locator win info struct.  */
struct tui_gen_win_info *
tui_locator_win_info_ptr (void)
{
  return &_locator;
}


/* Accessor for the termHeight.  */
int
tui_term_height (void)
{
  return _termHeight;
}


/* Mutator for the term height.   */
void
tui_set_term_height_to (int h)
{
  _termHeight = h;
}


/* Accessor for the termWidth.   */
int
tui_term_width (void)
{
  return _termWidth;
}


/* Mutator for the termWidth.  */
void
tui_set_term_width_to (int w)
{
  _termWidth = w;
}


/* Accessor for the current layout.   */
enum tui_layout_type
tui_current_layout (void)
{
  return _currentLayout;
}


/* Mutator for the current layout.  */
void
tui_set_current_layout_to (enum tui_layout_type newLayout)
{
  _currentLayout = newLayout;
}


/*
   ** setGenWinOrigin().
   **        Set the origin of the window
 */
void
setGenWinOrigin (struct tui_gen_win_info * win_info, int x, int y)
{
  win_info->origin.x = x;
  win_info->origin.y = y;

  return;
}				/* setGenWinOrigin */


/*****************************
** OTHER PUBLIC FUNCTIONS
*****************************/


/* Answer the next window in the list, cycling back to the top if
   necessary.  */
struct tui_win_info *
tui_next_win (struct tui_win_info * curWin)
{
  enum tui_win_type type = curWin->generic.type;
  struct tui_win_info * nextWin = (struct tui_win_info *) NULL;

  if (curWin->generic.type == CMD_WIN)
    type = SRC_WIN;
  else
    type = curWin->generic.type + 1;
  while (type != curWin->generic.type && (nextWin == NULL))
    {
      if (tui_win_list[type] && tui_win_list[type]->generic.is_visible)
	nextWin = tui_win_list[type];
      else
	{
	  if (type == CMD_WIN)
	    type = SRC_WIN;
	  else
	    type++;
	}
    }

  return nextWin;
}				/* tuiNextWin */


/* Answer the prev window in the list, cycling back to the bottom if
   necessary.  */
struct tui_win_info *
tui_prev_win (struct tui_win_info * curWin)
{
  enum tui_win_type type = curWin->generic.type;
  struct tui_win_info * prev = (struct tui_win_info *) NULL;

  if (curWin->generic.type == SRC_WIN)
    type = CMD_WIN;
  else
    type = curWin->generic.type - 1;
  while (type != curWin->generic.type && (prev == NULL))
    {
      if (tui_win_list[type]->generic.is_visible)
	prev = tui_win_list[type];
      else
	{
	  if (type == SRC_WIN)
	    type = CMD_WIN;
	  else
	    type--;
	}
    }

  return prev;
}


/* Answer the window represented by name.    */
struct tui_win_info *
tui_partial_win_by_name (char *name)
{
  struct tui_win_info * win_info = (struct tui_win_info *) NULL;

  if (name != (char *) NULL)
    {
      int i = 0;

      while (i < MAX_MAJOR_WINDOWS && win_info == NULL)
	{
          if (tui_win_list[i] != 0)
            {
              char *curName = tui_win_name (&tui_win_list[i]->generic);
              if (strlen (name) <= strlen (curName) &&
                  strncmp (name, curName, strlen (name)) == 0)
                win_info = tui_win_list[i];
            }
	  i++;
	}
    }

  return win_info;
}				/* partialWinByName */


/*
   ** winName().
   **      Answer the name of the window
 */
char *
tui_win_name (struct tui_gen_win_info * win_info)
{
  char *name = (char *) NULL;

  switch (win_info->type)
    {
    case SRC_WIN:
      name = SRC_NAME;
      break;
    case CMD_WIN:
      name = CMD_NAME;
      break;
    case DISASSEM_WIN:
      name = DISASSEM_NAME;
      break;
    case DATA_WIN:
      name = DATA_NAME;
      break;
    default:
      name = "";
      break;
    }

  return name;
}				/* winName */


void
tui_initialize_static_data (void)
{
  tui_init_generic_part (tui_source_exec_info_win_ptr ());
  tui_init_generic_part (tui_disassem_exec_info_win_ptr ());
  tui_init_generic_part (tui_locator_win_info_ptr ());
}


struct tui_gen_win_info *
tui_alloc_generic_win_info (void)
{
  struct tui_gen_win_info * win;

  if ((win = (struct tui_gen_win_info *) xmalloc (
		     sizeof (struct tui_gen_win_info *))) != (struct tui_gen_win_info *) NULL)
    tui_init_generic_part (win);

  return win;
}				/* allocGenericWinInfo */


/*
   ** initGenericPart().
 */
void
tui_init_generic_part (struct tui_gen_win_info * win)
{
  win->width =
    win->height =
    win->origin.x =
    win->origin.y =
    win->viewport_height =
    win->content_size =
    win->last_visible_line = 0;
  win->handle = (WINDOW *) NULL;
  win->content = NULL;
  win->content_in_use =
    win->is_visible = FALSE;
  win->title = 0;
}


/*
   ** initContentElement().
 */
void
initContentElement (struct tui_win_element * element, enum tui_win_type type)
{
  element->highlight = FALSE;
  switch (type)
    {
    case SRC_WIN:
    case DISASSEM_WIN:
      element->which_element.source.line = (char *) NULL;
      element->which_element.source.line_or_addr.line_no = 0;
      element->which_element.source.is_exec_point = FALSE;
      element->which_element.source.has_break = FALSE;
      break;
    case DATA_WIN:
      tui_init_generic_part (&element->which_element.data_window);
      element->which_element.data_window.type = DATA_ITEM_WIN;
      ((struct tui_gen_win_info *) & element->which_element.data_window)->content =
	(void **) tui_alloc_content (1, DATA_ITEM_WIN);
      ((struct tui_gen_win_info *)
       & element->which_element.data_window)->content_size = 1;
      break;
    case CMD_WIN:
      element->which_element.command.line = (char *) NULL;
      break;
    case DATA_ITEM_WIN:
      element->which_element.data.name = (char *) NULL;
      element->which_element.data.type = TUI_REGISTER;
      element->which_element.data.item_no = UNDEFINED_ITEM;
      element->which_element.data.value = NULL;
      element->which_element.data.highlight = FALSE;
      break;
    case LOCATOR_WIN:
      element->which_element.locator.file_name[0] =
	element->which_element.locator.proc_name[0] = (char) 0;
      element->which_element.locator.line_no = 0;
      element->which_element.locator.addr = 0;
      break;
    case EXEC_INFO_WIN:
      memset(element->which_element.simple_string, ' ',
             sizeof(element->which_element.simple_string));
      break;
    default:
      break;
    }
  return;
}				/* initContentElement */

/*
   ** initWinInfo().
 */
void
initWinInfo (struct tui_win_info * win_info)
{
  tui_init_generic_part (&win_info->generic);
  win_info->can_highlight =
    win_info->is_highlighted = FALSE;
  switch (win_info->generic.type)
    {
    case SRC_WIN:
    case DISASSEM_WIN:
      win_info->detail.source_info.execution_info = (struct tui_gen_win_info *) NULL;
      win_info->detail.source_info.has_locator = FALSE;
      win_info->detail.source_info.horizontal_offset = 0;
      win_info->detail.source_info.start_line_or_addr.addr = 0;
      win_info->detail.source_info.filename = 0;
      break;
    case DATA_WIN:
      win_info->detail.data_display_info.data_content = (tui_win_content) NULL;
      win_info->detail.data_display_info.data_content_count = 0;
      win_info->detail.data_display_info.regs_content = (tui_win_content) NULL;
      win_info->detail.data_display_info.regs_content_count = 0;
      win_info->detail.data_display_info.regs_display_type =
	TUI_UNDEFINED_REGS;
      win_info->detail.data_display_info.regs_column_count = 1;
      win_info->detail.data_display_info.display_regs = FALSE;
      break;
    case CMD_WIN:
      win_info->detail.command_info.cur_line = 0;
      win_info->detail.command_info.curch = 0;
      break;
    default:
      win_info->detail.opaque = NULL;
      break;
    }

  return;
}				/* initWinInfo */


struct tui_win_info *
tui_alloc_win_info (enum tui_win_type type)
{
  struct tui_win_info * win_info = (struct tui_win_info *) NULL;

  win_info = (struct tui_win_info *) xmalloc (sizeof (struct tui_win_info));
  if ((win_info != NULL))
    {
      win_info->generic.type = type;
      initWinInfo (win_info);
    }

  return win_info;
}				/* allocWinInfo */


/*
   ** allocContent().
   **        Allocates the content and elements in a block.
 */
tui_win_content
tui_alloc_content (int numElements, enum tui_win_type type)
{
  tui_win_content content = (tui_win_content) NULL;
  char *elementBlockPtr = (char *) NULL;
  int i;

  if ((content = (tui_win_content)
  xmalloc (sizeof (struct tui_win_element *) * numElements)) != (tui_win_content) NULL)
    {				/*
				   ** All windows, except the data window, can allocate the elements
				   ** in a chunk.  The data window cannot because items can be
				   ** added/removed from the data display by the user at any time.
				 */
      if (type != DATA_WIN)
	{
	  if ((elementBlockPtr = (char *)
	   xmalloc (sizeof (struct tui_win_element) * numElements)) != (char *) NULL)
	    {
	      for (i = 0; i < numElements; i++)
		{
		  content[i] = (struct tui_win_element *) elementBlockPtr;
		  initContentElement (content[i], type);
		  elementBlockPtr += sizeof (struct tui_win_element);
		}
	    }
	  else
	    {
	      xfree (content);
	      content = (tui_win_content) NULL;
	    }
	}
    }

  return content;
}				/* allocContent */


/* Adds the input number of elements to the windows's content.  If no
   content has been allocated yet, allocContent() is called to do
   this.  The index of the first element added is returned, unless
   there is a memory allocation error, in which case, (-1) is
   returned.  */
int
tui_add_content_elements (struct tui_gen_win_info * win_info, int numElements)
{
  struct tui_win_element * elementPtr;
  int i, indexStart;

  if (win_info->content == NULL)
    {
      win_info->content = (void **) tui_alloc_content (numElements, win_info->type);
      indexStart = 0;
    }
  else
    indexStart = win_info->content_size;
  if (win_info->content != NULL)
    {
      for (i = indexStart; (i < numElements + indexStart); i++)
	{
	  if ((elementPtr = (struct tui_win_element *)
	       xmalloc (sizeof (struct tui_win_element))) != (struct tui_win_element *) NULL)
	    {
	      win_info->content[i] = (void *) elementPtr;
	      initContentElement (elementPtr, win_info->type);
	      win_info->content_size++;
	    }
	  else			/* things must be really hosed now! We ran out of memory!? */
	    return (-1);
	}
    }

  return indexStart;
}				/* addContentElements */


/* Delete all curses windows associated with win_info, leaving everything
   else intact.  */
void
tuiDelWindow (struct tui_win_info * win_info)
{
  struct tui_gen_win_info * genericWin;

  switch (win_info->generic.type)
    {
    case SRC_WIN:
    case DISASSEM_WIN:
      genericWin = tui_locator_win_info_ptr ();
      if (genericWin != (struct tui_gen_win_info *) NULL)
	{
	  tui_delete_win (genericWin->handle);
	  genericWin->handle = (WINDOW *) NULL;
	  genericWin->is_visible = FALSE;
	}
      if (win_info->detail.source_info.filename)
        {
          xfree (win_info->detail.source_info.filename);
          win_info->detail.source_info.filename = 0;
        }
      genericWin = win_info->detail.source_info.execution_info;
      if (genericWin != (struct tui_gen_win_info *) NULL)
	{
	  tui_delete_win (genericWin->handle);
	  genericWin->handle = (WINDOW *) NULL;
	  genericWin->is_visible = FALSE;
	}
      break;
    case DATA_WIN:
      if (win_info->generic.content != NULL)
	{
	  tui_del_data_windows (win_info->detail.data_display_info.regs_content,
				win_info->detail.data_display_info.regs_content_count);
	  tui_del_data_windows (win_info->detail.data_display_info.data_content,
				win_info->detail.data_display_info.data_content_count);
	}
      break;
    default:
      break;
    }
  if (win_info->generic.handle != (WINDOW *) NULL)
    {
      tui_delete_win (win_info->generic.handle);
      win_info->generic.handle = (WINDOW *) NULL;
      win_info->generic.is_visible = FALSE;
    }
}


void
tui_free_window (struct tui_win_info * win_info)
{
  struct tui_gen_win_info * genericWin;

  switch (win_info->generic.type)
    {
    case SRC_WIN:
    case DISASSEM_WIN:
      genericWin = tui_locator_win_info_ptr ();
      if (genericWin != (struct tui_gen_win_info *) NULL)
	{
	  tui_delete_win (genericWin->handle);
	  genericWin->handle = (WINDOW *) NULL;
	}
      tui_free_win_content (genericWin);
      if (win_info->detail.source_info.filename)
        {
          xfree (win_info->detail.source_info.filename);
          win_info->detail.source_info.filename = 0;
        }
      genericWin = win_info->detail.source_info.execution_info;
      if (genericWin != (struct tui_gen_win_info *) NULL)
	{
	  tui_delete_win (genericWin->handle);
	  genericWin->handle = (WINDOW *) NULL;
	  tui_free_win_content (genericWin);
	}
      break;
    case DATA_WIN:
      if (win_info->generic.content != NULL)
	{
	  tui_free_data_content (win_info->detail.data_display_info.regs_content,
				 win_info->detail.data_display_info.regs_content_count);
	  win_info->detail.data_display_info.regs_content =
	    (tui_win_content) NULL;
	  win_info->detail.data_display_info.regs_content_count = 0;
	  tui_free_data_content (win_info->detail.data_display_info.data_content,
				 win_info->detail.data_display_info.data_content_count);
	  win_info->detail.data_display_info.data_content =
	    (tui_win_content) NULL;
	  win_info->detail.data_display_info.data_content_count = 0;
	  win_info->detail.data_display_info.regs_display_type =
	    TUI_UNDEFINED_REGS;
	  win_info->detail.data_display_info.regs_column_count = 1;
	  win_info->detail.data_display_info.display_regs = FALSE;
	  win_info->generic.content = NULL;
	  win_info->generic.content_size = 0;
	}
      break;
    default:
      break;
    }
  if (win_info->generic.handle != (WINDOW *) NULL)
    {
      tui_delete_win (win_info->generic.handle);
      win_info->generic.handle = (WINDOW *) NULL;
      tui_free_win_content (&win_info->generic);
    }
  if (win_info->generic.title)
    xfree (win_info->generic.title);
  xfree (win_info);
}


void
tui_free_all_source_wins_content (void)
{
  int i;

  for (i = 0; i < (tui_source_windows ())->count; i++)
    {
      struct tui_win_info * win_info = (struct tui_win_info *) (tui_source_windows ())->list[i];

      if (win_info != NULL)
	{
	  tui_free_win_content (&(win_info->generic));
	  tui_free_win_content (win_info->detail.source_info.execution_info);
	}
    }
}


void
tui_free_win_content (struct tui_gen_win_info * win_info)
{
  if (win_info->content != NULL)
    {
      freeContent ((tui_win_content) win_info->content,
		   win_info->content_size,
		   win_info->type);
      win_info->content = NULL;
    }
  win_info->content_size = 0;

  return;
}				/* freeWinContent */


void
tui_del_data_windows (tui_win_content content, int contentSize)
{
  int i;

  /*
     ** Remember that data window content elements are of type struct tui_gen_win_info *,
     ** each of which whose single element is a data element.
   */
  for (i = 0; i < contentSize; i++)
    {
      struct tui_gen_win_info * genericWin = &content[i]->which_element.data_window;

      if (genericWin != (struct tui_gen_win_info *) NULL)
	{
	  tui_delete_win (genericWin->handle);
	  genericWin->handle = (WINDOW *) NULL;
	  genericWin->is_visible = FALSE;
	}
    }
}


void
tui_free_data_content (tui_win_content content, int contentSize)
{
  int i;

  /*
     ** Remember that data window content elements are of type struct tui_gen_win_info *,
     ** each of which whose single element is a data element.
   */
  for (i = 0; i < contentSize; i++)
    {
      struct tui_gen_win_info * genericWin = &content[i]->which_element.data_window;

      if (genericWin != (struct tui_gen_win_info *) NULL)
	{
	  tui_delete_win (genericWin->handle);
	  genericWin->handle = (WINDOW *) NULL;
	  tui_free_win_content (genericWin);
	}
    }
  freeContent (content,
	       contentSize,
	       DATA_WIN);

  return;
}				/* freeDataContent */


/**********************************
** LOCAL STATIC FUNCTIONS        **
**********************************/


/*
   ** freeContent().
 */
static void
freeContent (tui_win_content content, int contentSize, enum tui_win_type winType)
{
  if (content != (tui_win_content) NULL)
    {
      freeContentElements (content, contentSize, winType);
      xfree (content);
    }

  return;
}				/* freeContent */


/*
   ** freeContentElements().
 */
static void
freeContentElements (tui_win_content content, int contentSize, enum tui_win_type type)
{
  if (content != (tui_win_content) NULL)
    {
      int i;

      if (type == SRC_WIN || type == DISASSEM_WIN)
	{
	  /* free whole source block */
	  xfree (content[0]->which_element.source.line);
	}
      else
	{
	  for (i = 0; i < contentSize; i++)
	    {
	      struct tui_win_element * element;

	      element = content[i];
	      if (element != (struct tui_win_element *) NULL)
		{
		  switch (type)
		    {
		    case DATA_WIN:
		      xfree (element);
		      break;
		    case DATA_ITEM_WIN:
		      /*
		         ** Note that data elements are not allocated
		         ** in a single block, but individually, as needed.
		       */
		      if (element->which_element.data.type != TUI_REGISTER)
			xfree ((void *)element->which_element.data.name);
		      xfree (element->which_element.data.value);
		      xfree (element);
		      break;
		    case CMD_WIN:
		      xfree (element->which_element.command.line);
		      break;
		    default:
		      break;
		    }
		}
	    }
	}
      if (type != DATA_WIN && type != DATA_ITEM_WIN)
	xfree (content[0]);	/* free the element block */
    }

  return;
}				/* freeContentElements */
