/* TUI data manipulation routines.

   Copyright 1998, 1999, 2000, 2001, 2002, 2004 Free Software
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

#ifndef TUI_DATA_H
#define TUI_DATA_H

#if defined (HAVE_NCURSES_H)
#include <ncurses.h>
#elif defined (HAVE_CURSES_H)
#include <curses.h>
#endif

/* Generic window information */
struct tui_gen_win_info
{
  WINDOW *handle;	/* window handle */
  enum tui_win_type type;	/* type of window */
  int width;		/* window width */
  int height;		/* window height */
  struct tui_point origin;	/* origin of window */
  void **content;	/* content of window */
  int contentSize;	/* Size of content (# of elements) */
  int contentInUse;	/* Can it be used, or is it already used? */
  int viewportHeight;	/* viewport height */
  int lastVisibleLine;	/* index of last visible line */
  int isVisible;		/* whether the window is visible or not */
  char* title;          /* Window title to display.  */
};

/* Constant definitions */
#define DEFAULT_TAB_LEN                8
#define NO_SRC_STRING                  "[ No Source Available ]"
#define NO_DISASSEM_STRING             "[ No Assembly Available ]"
#define NO_REGS_STRING                 "[ Register Values Unavailable ]"
#define NO_DATA_STRING                 "[ No Data Values Displayed ]"
#define MAX_CONTENT_COUNT              100
#define SRC_NAME                       "SRC"
#define CMD_NAME                       "CMD"
#define DATA_NAME                      "REGS"
#define DISASSEM_NAME                  "ASM"
#define TUI_NULL_STR                   ""
#define DEFAULT_HISTORY_COUNT          25
#define BOX_WINDOW                     TRUE
#define DONT_BOX_WINDOW                FALSE
#define HILITE                         TRUE
#define NO_HILITE                      FALSE
#define WITH_LOCATOR                   TRUE
#define NO_LOCATOR                     FALSE
#define EMPTY_SOURCE_PROMPT            TRUE
#define NO_EMPTY_SOURCE_PROMPT         FALSE
#define UNDEFINED_ITEM                 -1
#define MIN_WIN_HEIGHT                 3
#define MIN_CMD_WIN_HEIGHT             3

/* Strings to display in the TUI status line.  */
#define PROC_PREFIX                    "In: "
#define LINE_PREFIX                    "Line: "
#define PC_PREFIX                      "PC: "
#define SINGLE_KEY                     "(SingleKey)"

/* Minimum/Maximum length of some fields displayed in the TUI status line.  */
#define MIN_LINE_WIDTH     4 /* Use at least 4 digits for line numbers.  */
#define MIN_PROC_WIDTH    12
#define MAX_TARGET_WIDTH  10
#define MAX_PID_WIDTH     14

#define TUI_FLOAT_REGS_NAME                  "$FREGS"
#define TUI_FLOAT_REGS_NAME_LOWER            "$fregs"
#define TUI_GENERAL_REGS_NAME                "$GREGS"
#define TUI_GENERAL_REGS_NAME_LOWER          "$gregs"
#define TUI_SPECIAL_REGS_NAME                "$SREGS"
#define TUI_SPECIAL_REGS_NAME_LOWER          "$sregs"
#define TUI_GENERAL_SPECIAL_REGS_NAME        "$REGS"
#define TUI_GENERAL_SPECIAL_REGS_NAME_LOWER  "$regs"

/* Scroll direction enum.  */
enum tui_scroll_direction
{
  FORWARD_SCROLL,
  BACKWARD_SCROLL,
  LEFT_SCROLL,
  RIGHT_SCROLL
};


/* General list struct.  */
struct tui_list
{
  void **list;
  int count;
};


/* The kinds of layouts available */
enum tui_layout_type
{
  SRC_COMMAND,
  DISASSEM_COMMAND,
  SRC_DISASSEM_COMMAND,
  SRC_DATA_COMMAND,
  DISASSEM_DATA_COMMAND,
  UNDEFINED_LAYOUT
};

/* Basic data types that can be displayed in the data window. */
enum tui_data_type
{
  TUI_REGISTER,
  TUI_SCALAR,
  TUI_COMPLEX,
  TUI_STRUCT
};

/* Types of register displays */
enum tui_register_display_type
{
  TUI_UNDEFINED_REGS,
  TUI_GENERAL_REGS,
  TUI_SFLOAT_REGS,
  TUI_DFLOAT_REGS,
  TUI_SPECIAL_REGS,
  TUI_GENERAL_AND_SPECIAL_REGS
};

/* Structure describing source line or line address */
union tui_line_or_address
{
  int lineNo;
  CORE_ADDR addr;
};

/* Current Layout definition */
struct tui_layout_def
{
  enum tui_win_type displayMode;
  int split;
  enum tui_register_display_type regsDisplayType;
  enum tui_register_display_type floatRegsDisplayType;
};

/* Elements in the Source/Disassembly Window */
struct tui_source_element
{
  char *line;
  union tui_line_or_address lineOrAddr;
  int isExecPoint;
  int hasBreak;
};


/* Elements in the data display window content */
struct tui_data_element
{
  const char *name;
  int itemNo;			/* the register number, or data display number */
  enum tui_data_type type;
  void *value;
  int highlight;
};


/* Elements in the command window content */
struct tui_command_element
{
  char *line;
};


#define MAX_LOCATOR_ELEMENT_LEN        100

/* Elements in the locator window content */
struct tui_locator_element
{
  char fileName[MAX_LOCATOR_ELEMENT_LEN];
  char procName[MAX_LOCATOR_ELEMENT_LEN];
  int lineNo;
  CORE_ADDR addr;
};

/* Flags to tell what kind of breakpoint is at current line.  */
#define TUI_BP_ENABLED      0x01
#define TUI_BP_DISABLED     0x02
#define TUI_BP_HIT          0x04
#define TUI_BP_CONDITIONAL  0x08
#define TUI_BP_HARDWARE     0x10

/* Position of breakpoint markers in the exec info string.  */
#define TUI_BP_HIT_POS      0
#define TUI_BP_BREAK_POS    1
#define TUI_EXEC_POS        2
#define TUI_EXECINFO_SIZE   4

typedef char tui_exec_info_content[TUI_EXECINFO_SIZE];

/* An content element in a window */
union tui_which_element
{
  struct tui_source_element source;	/* the source elements */
  struct tui_gen_win_info dataWindow;	/* data display elements */
  struct tui_data_element data;	/* elements of dataWindow */
  struct tui_command_element command;	/* command elements */
  struct tui_locator_element locator;	/* locator elements */
  tui_exec_info_content simpleString;	/* simple char based elements */
};

struct tui_win_element
{
  int highlight;
  union tui_which_element whichElement;
};


/* This describes the content of the window. */
typedef struct tui_win_element **tui_win_content;


/* This struct defines the specific information about a data display window */
struct tui_data_info
{
  tui_win_content dataContent;	/* start of data display content */
  int dataContentCount;
  tui_win_content regsContent;	/* start of regs display content */
  int regsContentCount;
  enum tui_register_display_type regsDisplayType;
  int regsColumnCount;
  int displayRegs;		/* Should regs be displayed at all? */
};


struct tui_source_info
{
  int hasLocator;		/* Does locator belongs to this window? */
  /* Execution information window.  */
  struct tui_gen_win_info *executionInfo;
  int horizontalOffset;	/* used for horizontal scroll */
  union tui_line_or_address startLineOrAddr;
  char* filename;
};


struct tui_command_info
{
  int curLine;		/* The current line position */
  int curch;			/* The current cursor position */
  int start_line;
};


/* This defines information about each logical window */
struct tui_win_info
{
  struct tui_gen_win_info generic;	/* general window information */
  union
  {
    struct tui_source_info sourceInfo;
    struct tui_data_info dataDisplayInfo;
    struct tui_command_info commandInfo;
    void *opaque;
  }
  detail;
  int canHighlight;		/* Can this window ever be highlighted? */
  int isHighlighted;		/* Is this window highlighted? */
};

/* MACROS (prefixed with m_) */

/* Testing macros */
#define        m_genWinPtrIsNull(winInfo) \
                ((winInfo) == (struct tui_gen_win_info *)NULL)
#define        m_genWinPtrNotNull(winInfo) \
                ((winInfo) != (struct tui_gen_win_info *)NULL)
#define        m_winPtrIsNull(winInfo) \
                ((winInfo) == (struct tui_win_info *)NULL)
#define        m_winPtrNotNull(winInfo) \
                ((winInfo) != (struct tui_win_info *)NULL)

#define        m_winIsSourceType(type) \
                (type == SRC_WIN || type == DISASSEM_WIN)
#define        m_winIsAuxillary(winType) \
                (winType > MAX_MAJOR_WINDOWS)
#define        m_hasLocator(winInfo) \
                ( ((winInfo) != (struct tui_win_info *)NULL) ? \
                    (winInfo->detail.sourceInfo.hasLocator) : \
                    FALSE )

#define     m_setWinHighlightOn(winInfo) \
                if ((winInfo) != (struct tui_win_info *)NULL) \
                              (winInfo)->isHighlighted = TRUE
#define     m_setWinHighlightOff(winInfo) \
                if ((winInfo) != (struct tui_win_info *)NULL) \
                              (winInfo)->isHighlighted = FALSE


/* Global Data */
extern struct tui_win_info *(winList[MAX_MAJOR_WINDOWS]);

/* Macros */
#define srcWin            winList[SRC_WIN]
#define disassemWin       winList[DISASSEM_WIN]
#define dataWin           winList[DATA_WIN]
#define cmdWin            winList[CMD_WIN]

/* Data Manipulation Functions */
extern void tui_initialize_static_data (void);
extern struct tui_gen_win_info *tui_alloc_generic_win_info (void);
extern struct tui_win_info *tui_alloc_win_info (enum tui_win_type);
extern void tui_init_generic_part (struct tui_gen_win_info *);
extern void tui_init_win_info (struct tui_win_info *);
extern tui_win_content tui_alloc_content (int, enum tui_win_type);
extern int tui_add_content_elements (struct tui_gen_win_info *, int);
extern void tui_init_content_element (struct tui_win_element *, enum tui_win_type);
extern void tui_free_window (struct tui_win_info *);
extern void tui_free_win_content (struct tui_gen_win_info *);
extern void tui_free_data_content (tui_win_content, int);
extern void tui_free_all_source_wins_content (void);
extern void tui_del_window (struct tui_win_info *);
extern void tui_del_data_windows (tui_win_content, int);
extern struct tui_win_info *tui_partial_win_by_name (char *);
extern char *tui_win_name (struct tui_gen_win_info *);
extern enum tui_layout_type tui_current_layout (void);
extern void tui_set_current_layout_to (enum tui_layout_type);
extern int tui_term_height (void);
extern void tui_set_term_height_to (int);
extern int tui_term_width (void);
extern void tui_set_term_width_to (int);
extern void tui_set_gen_win_origin (struct tui_gen_win_info *, int, int);
extern struct tui_gen_win_info *tui_locator_win_info_ptr (void);
extern struct tui_gen_win_info *tui_source_exec_info_win_ptr (void);
extern struct tui_gen_win_info *tui_disassem_exec_info_win_ptr (void);
extern struct tui_list * tui_source_windows (void);
extern void tui_clear_source_windows (void);
extern void tui_clear_source_windows_detail (void);
extern void tui_clear_win_detail (struct tui_win_info * winInfo);
extern void tui_add_to_source_windows (struct tui_win_info *);
extern int tui_default_tab_len (void);
extern void tui_set_default_tab_len (int);
extern struct tui_win_info *tui_win_with_focus (void);
extern void tui_set_win_with_focus (struct tui_win_info *);
extern struct tui_layout_def * tui_layout_def (void);
extern int tui_win_resized (void);
extern void tui_set_win_resized_to (int);

extern struct tui_win_info *tui_next_win (struct tui_win_info *);
extern struct tui_win_info *tui_prev_win (struct tui_win_info *);

extern void tui_add_to_source_windows (struct tui_win_info * winInfo);

#endif /* TUI_DATA_H */
