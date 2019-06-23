/* TUI data manipulation routines.

   Copyright (C) 1998-2019 Free Software Foundation, Inc.

   Contributed by Hewlett-Packard Company.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef TUI_TUI_DATA_H
#define TUI_TUI_DATA_H

#include "tui/tui.h"	/* For enum tui_win_type.  */
#include "gdb_curses.h"	/* For WINDOW.  */

/* This is a point definition.  */
struct tui_point
{
  int x, y;
};

/* Generic window information.  */
struct tui_gen_win_info
{
protected:

  explicit tui_gen_win_info (enum tui_win_type t)
    : type (t)
  {
  }

public:

  virtual ~tui_gen_win_info ();

  /* Call to refresh this window.  */
  virtual void refresh_window ();

  /* Make this window visible or invisible.  */
  virtual void make_visible (bool visible);

  /* Return the name of this type of window.  */
  virtual const char *name () const
  {
    return "";
  }

  /* Reset this window.  WIN_TYPE must match the existing type of this
     window (it is only passed for self-test purposes).  The other
     parameters are used to set the window's size and position.  */
  void reset (enum tui_win_type win_type,
	      int height, int width,
	      int origin_x, int origin_y);

  /* Window handle.  */
  WINDOW *handle = nullptr;
  /* Type of window.  */
  enum tui_win_type type;
  /* Window width.  */
  int width = 0;
  /* Window height.  */
  int height = 0;
  /* Origin of window.  */
  struct tui_point origin = {0, 0};
  /* Can it be used, or is it already used?  */
  int content_in_use = FALSE;
  /* Viewport height.  */
  int viewport_height = 0;
  /* Index of last visible line.  */
  int last_visible_line = 0;
  /* Whether the window is visible or not.  */
  bool is_visible = false;
  /* Window title to display.  */
  char *title = nullptr;
};

/* Whether or not a window should be drawn with a box.  */
enum tui_box
{
  DONT_BOX_WINDOW = 0,
  BOX_WINDOW
};

/* Constant definitions.  */
#define DEFAULT_TAB_LEN         8
#define NO_SRC_STRING           "[ No Source Available ]"
#define NO_DISASSEM_STRING      "[ No Assembly Available ]"
#define NO_REGS_STRING          "[ Register Values Unavailable ]"
#define NO_DATA_STRING          "[ No Data Values Displayed ]"
#define MAX_CONTENT_COUNT       100
#define SRC_NAME                "src"
#define CMD_NAME                "cmd"
#define DATA_NAME               "regs"
#define DISASSEM_NAME           "asm"
#define TUI_NULL_STR            ""
#define DEFAULT_HISTORY_COUNT	25
#define HILITE                  TRUE
#define NO_HILITE               FALSE
#define WITH_LOCATOR            TRUE
#define NO_LOCATOR              FALSE
#define EMPTY_SOURCE_PROMPT     TRUE
#define NO_EMPTY_SOURCE_PROMPT  FALSE
#define UNDEFINED_ITEM          -1
#define MIN_WIN_HEIGHT          3
#define MIN_CMD_WIN_HEIGHT      3

/* Strings to display in the TUI status line.  */
#define PROC_PREFIX             "In: "
#define LINE_PREFIX             "L"
#define PC_PREFIX               "PC: "
#define SINGLE_KEY              "(SingleKey)"

/* Minimum/Maximum length of some fields displayed in the TUI status
   line.  */
#define MIN_LINE_WIDTH     4	/* Use at least 4 digits for line
				   numbers.  */
#define MIN_PROC_WIDTH    12
#define MAX_TARGET_WIDTH  10
#define MAX_PID_WIDTH     19

/* The kinds of layouts available.  */
enum tui_layout_type
{
  SRC_COMMAND,
  DISASSEM_COMMAND,
  SRC_DISASSEM_COMMAND,
  SRC_DATA_COMMAND,
  DISASSEM_DATA_COMMAND,
  UNDEFINED_LAYOUT
};

enum tui_line_or_address_kind
{
  LOA_LINE,
  LOA_ADDRESS
};

/* Structure describing source line or line address.  */
struct tui_line_or_address
{
  enum tui_line_or_address_kind loa;
  union
    {
      int line_no;
      CORE_ADDR addr;
    } u;
};

/* Current Layout definition.  */
struct tui_layout_def
{
  enum tui_win_type display_mode;
};

/* Elements in the Source/Disassembly Window.  */
struct tui_source_element
{
  tui_source_element ()
  {
    line_or_addr.loa = LOA_LINE;
    line_or_addr.u.line_no = 0;
  }

  ~tui_source_element ()
  {
    xfree (line);
  }

  char *line = nullptr;
  struct tui_line_or_address line_or_addr;
  bool is_exec_point = false;
  int has_break = 0;
};


#ifdef PATH_MAX
# define MAX_LOCATOR_ELEMENT_LEN        PATH_MAX
#else
# define MAX_LOCATOR_ELEMENT_LEN        1024
#endif

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

/* Execution info window class.  */

struct tui_exec_info_window : public tui_gen_win_info
{
  tui_exec_info_window ()
    : tui_gen_win_info (EXEC_INFO_WIN)
  {
  }

  ~tui_exec_info_window () override
  {
    xfree (m_content);
  }

  /* Get or allocate contents.  */
  tui_exec_info_content *maybe_allocate_content (int n_elements);

  /* Return the contents.  */
  const tui_exec_info_content *get_content () const
  {
    return m_content;
  }

private:

  tui_exec_info_content *m_content = nullptr;
};

/* Locator window class.  */

struct tui_locator_window : public tui_gen_win_info
{
  tui_locator_window ()
    : tui_gen_win_info (LOCATOR_WIN)
  {
    full_name[0] = 0;
    proc_name[0] = 0;
  }

  char full_name[MAX_LOCATOR_ELEMENT_LEN];
  char proc_name[MAX_LOCATOR_ELEMENT_LEN];
  int line_no = 0;
  CORE_ADDR addr = 0;
  /* Architecture associated with code at this location.  */
  struct gdbarch *gdbarch = nullptr;
};

/* A data item window.  */

struct tui_data_item_window : public tui_gen_win_info
{
  tui_data_item_window ()
    : tui_gen_win_info (DATA_ITEM_WIN)
  {
  }

  ~tui_data_item_window () override;

  const char *name = nullptr;
  /* The register number, or data display number.  */
  int item_no = UNDEFINED_ITEM;
  void *value = nullptr;
  bool highlight = false;
  char *content = nullptr;
};

/* This defines information about each logical window.  */
struct tui_win_info : public tui_gen_win_info
{
protected:

  explicit tui_win_info (enum tui_win_type type);
  DISABLE_COPY_AND_ASSIGN (tui_win_info);

  /* Scroll the contents vertically.  This is only called via
     forward_scroll and backward_scroll.  */
  virtual void do_scroll_vertical (int num_to_scroll) = 0;

  /* Scroll the contents horizontally.  This is only called via
     left_scroll and right_scroll.  */
  virtual void do_scroll_horizontal (int num_to_scroll) = 0;

  /* Called after make_visible_with_new_height sets the new height.
     Should update the window.  */
  virtual void do_make_visible_with_new_height () = 0;

public:

  ~tui_win_info () override
  {
  }

  /* Clear the pertinent detail in the window.  */
  virtual void clear_detail () = 0;

  /* Return true if this window has the locator.  */
  virtual bool has_locator () const
  {
    return false;
  }

  /* Refresh this window and any associated windows.  */
  virtual void refresh ();

  /* Called after all the TUI windows are refreshed, to let this
     window have a chance to update itself further.  */
  virtual void refresh_all ()
  {
  }

  /* Called after a TUI window is given a new height; this updates any
     related auxiliary windows.  */
  virtual void set_new_height (int height)
  {
  }

  /* Compute the maximum height of this window.  */
  virtual int max_height () const;

  /* Called after the tab width has been changed.  */
  virtual void update_tab_width ()
  {
  }

  /* Make the window visible after the height has been changed.  */
  void make_visible_with_new_height ();

  /* Set whether this window is highglighted.  */
  void set_highlight (bool highlight)
  {
    is_highlighted = highlight;
  }

  /* Methods to scroll the contents of this window.  Note that they
     are named with "_scroll" coming at the end because the more
     obvious "scroll_forward" is defined as a macro in term.h.  */
  void forward_scroll (int num_to_scroll);
  void backward_scroll (int num_to_scroll);
  void left_scroll (int num_to_scroll);
  void right_scroll (int num_to_scroll);

  /* Can this window ever be highlighted?  */
  bool can_highlight = true;

  /* Is this window highlighted?  */
  bool is_highlighted = false;
};

/* The base class for all source-like windows, namely the source and
   disassembly windows.  */

struct tui_source_window_base : public tui_win_info
{
protected:
  explicit tui_source_window_base (enum tui_win_type type);
  ~tui_source_window_base () override;
  DISABLE_COPY_AND_ASSIGN (tui_source_window_base);

  void do_scroll_horizontal (int num_to_scroll) override;
  void do_make_visible_with_new_height () override;

public:

  void clear_detail () override;

  /* Return true if this window has the locator.  */
  bool has_locator () const override
  {
    return m_has_locator;
  }

  void make_visible (bool visible) override;
  void refresh () override;
  void refresh_all () override;

  /* Refill the source window's source cache and update it.  If this
     is a disassembly window, then just update it.  */
  void refill ();

  /* Set the location of the execution point.  */
  void set_is_exec_point_at (struct tui_line_or_address l);

  void set_new_height (int height) override;

  void update_tab_width () override;

  /* Does the locator belong to this window?  */
  bool m_has_locator = false;
  /* Execution information window.  */
  struct tui_exec_info_window *execution_info = nullptr;
  /* Used for horizontal scroll.  */
  int horizontal_offset = 0;
  struct tui_line_or_address start_line_or_addr;

  /* It is the resolved form as returned by symtab_to_fullname.  */
  char *fullname = nullptr;

  /* Architecture associated with code at this location.  */
  struct gdbarch *gdbarch = nullptr;

  std::vector<tui_source_element> content;
};

/* A TUI source window.  */

struct tui_source_window : public tui_source_window_base
{
  tui_source_window ()
    : tui_source_window_base (SRC_WIN)
  {
  }

  DISABLE_COPY_AND_ASSIGN (tui_source_window);

  const char *name () const override
  {
    return SRC_NAME;
  }

protected:

  void do_scroll_vertical (int num_to_scroll) override;
};

/* A TUI disassembly window.  */

struct tui_disasm_window : public tui_source_window_base
{
  tui_disasm_window ()
    : tui_source_window_base (DISASSEM_WIN)
  {
  }

  DISABLE_COPY_AND_ASSIGN (tui_disasm_window);

  const char *name () const override
  {
    return DISASSEM_NAME;
  }

protected:

  void do_scroll_vertical (int num_to_scroll) override;
};

struct tui_data_window : public tui_win_info
{
  tui_data_window ()
    : tui_win_info (DATA_WIN)
  {
  }

  DISABLE_COPY_AND_ASSIGN (tui_data_window);

  void clear_detail () override;
  void refresh_all () override;

  void set_new_height (int height) override;

  void refresh_window () override;

  const char *name () const override
  {
    return DATA_NAME;
  }

  /* Windows that are used to display registers.  */
  std::vector<std::unique_ptr<tui_data_item_window>> regs_content;
  int regs_column_count = 0;
  /* Should regs be displayed at all?  */
  bool display_regs = false;
  struct reggroup *current_group = nullptr;

protected:

  void do_scroll_vertical (int num_to_scroll) override;
  void do_scroll_horizontal (int num_to_scroll) override
  {
  }
  void do_make_visible_with_new_height () override;

  /* Return the index of the first element displayed.  If none are
     displayed, then return -1.  */
  int first_data_item_displayed ();
};

struct tui_cmd_window : public tui_win_info
{
  tui_cmd_window ()
    : tui_win_info (CMD_WIN)
  {
    can_highlight = false;
  }

  DISABLE_COPY_AND_ASSIGN (tui_cmd_window);

  void clear_detail () override;

  void make_visible (bool visible) override
  {
  }

  int max_height () const override;

  void refresh_window () override
  {
  }

  const char *name () const override
  {
    return CMD_NAME;
  }

  int start_line = 0;

protected:

  void do_scroll_vertical (int num_to_scroll) override
  {
  }

  void do_scroll_horizontal (int num_to_scroll) override
  {
  }

  void do_make_visible_with_new_height () override;
};

extern int tui_win_is_auxiliary (enum tui_win_type win_type);


/* Global Data.  */
extern struct tui_win_info *tui_win_list[MAX_MAJOR_WINDOWS];

#define TUI_SRC_WIN     ((tui_source_window_base *) tui_win_list[SRC_WIN])
#define TUI_DISASM_WIN	((tui_source_window_base *) tui_win_list[DISASSEM_WIN])
#define TUI_DATA_WIN    ((tui_data_window *) tui_win_list[DATA_WIN])
#define TUI_CMD_WIN     ((tui_cmd_window *) tui_win_list[CMD_WIN])

/* Data Manipulation Functions.  */
extern void tui_initialize_static_data (void);
extern struct tui_win_info *tui_partial_win_by_name (const char *);
extern enum tui_layout_type tui_current_layout (void);
extern void tui_set_current_layout_to (enum tui_layout_type);
extern int tui_term_height (void);
extern void tui_set_term_height_to (int);
extern int tui_term_width (void);
extern void tui_set_term_width_to (int);
extern struct tui_locator_window *tui_locator_win_info_ptr (void);
extern std::vector<tui_source_window_base *> &tui_source_windows ();
extern void tui_clear_source_windows (void);
extern void tui_clear_source_windows_detail (void);
extern void tui_add_to_source_windows (struct tui_source_window_base *);
extern struct tui_win_info *tui_win_with_focus (void);
extern void tui_set_win_with_focus (struct tui_win_info *);
extern struct tui_layout_def *tui_layout_def (void);
extern int tui_win_resized (void);
extern void tui_set_win_resized_to (int);

extern struct tui_win_info *tui_next_win (struct tui_win_info *);
extern struct tui_win_info *tui_prev_win (struct tui_win_info *);

extern unsigned int tui_tab_width;

#endif /* TUI_TUI_DATA_H */
