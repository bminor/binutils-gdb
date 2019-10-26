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
#include "observable.h"

struct tui_cmd_window;
struct tui_source_window_base;
struct tui_source_window;

/* A deleter that calls delwin.  */
struct curses_deleter
{
  void operator() (WINDOW *win) const
  {
    delwin (win);
  }
};

/* Generic window information.  */
struct tui_gen_win_info
{
protected:

  explicit tui_gen_win_info (enum tui_win_type t)
    : type (t)
  {
  }

  /* This is called after the window is resized, and should update the
     window's contents.  */
  virtual void rerender ()
  {
  }

  virtual void make_window ();

public:
  tui_gen_win_info (tui_gen_win_info &&) = default;

  virtual ~tui_gen_win_info ()
  {
  }

  /* Call to refresh this window.  */
  virtual void refresh_window ();

  /* Make this window visible or invisible.  */
  virtual void make_visible (bool visible);

  /* Return the name of this type of window.  */
  virtual const char *name () const
  {
    return "";
  }

  /* Compute the maximum height of this window.  */
  virtual int max_height () const = 0;

  /* Compute the minimum height of this window.  */
  virtual int min_height () const = 0;

  /* Return true if this window can be boxed.  */
  virtual bool can_box () const
  {
    return false;
  }

  /* Resize this window.  The parameters are used to set the window's
     size and position.  */
  virtual void resize (int height, int width,
		       int origin_x, int origin_y);

  /* Return true if this window is visible.  */
  bool is_visible () const
  {
    return handle != nullptr;
  }

  /* Window handle.  */
  std::unique_ptr<WINDOW, curses_deleter> handle;
  /* Type of window.  */
  enum tui_win_type type;
  /* Window width.  */
  int width = 0;
  /* Window height.  */
  int height = 0;
  /* Origin of window.  */
  int x = 0;
  int y = 0;
  /* Viewport height.  */
  int viewport_height = 0;
};

/* Constant definitions.  */
#define DEFAULT_TAB_LEN         8
#define SRC_NAME                "src"
#define CMD_NAME                "cmd"
#define DATA_NAME               "regs"
#define DISASSEM_NAME           "asm"
#define MIN_WIN_HEIGHT          3
#define MIN_CMD_WIN_HEIGHT      3

/* Strings to display in the TUI status line.  */
#define SINGLE_KEY              "(SingleKey)"

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

  void rerender () override;

  void make_window () override;

public:

  ~tui_win_info () override
  {
  }

  int max_height () const override;

  int min_height () const override
  {
    return MIN_WIN_HEIGHT;
  }

  /* Called after the tab width has been changed.  */
  virtual void update_tab_width ()
  {
  }

  /* Set whether this window is highlighted.  */
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

  /* Return true if this window can be scrolled, false otherwise.  */
  virtual bool can_scroll () const
  {
    return true;
  }

  bool can_box () const override
  {
    return true;
  }

  void check_and_display_highlight_if_needed ();

  /* Window title to display.  */
  std::string title;

  /* Is this window highlighted?  */
  bool is_highlighted = false;
};


/* Global Data.  */
extern struct tui_win_info *tui_win_list[MAX_MAJOR_WINDOWS];

#define TUI_SRC_WIN     ((tui_source_window *) tui_win_list[SRC_WIN])
#define TUI_DISASM_WIN	((tui_source_window_base *) tui_win_list[DISASSEM_WIN])
#define TUI_DATA_WIN    ((tui_data_window *) tui_win_list[DATA_WIN])
#define TUI_CMD_WIN     ((tui_cmd_window *) tui_win_list[CMD_WIN])

/* An iterator that iterates over all windows.  */

class tui_window_iterator
{
public:

  typedef tui_window_iterator self_type;
  typedef struct tui_win_info *value_type;
  typedef struct tui_win_info *&reference;
  typedef struct tui_win_info **pointer;
  typedef std::forward_iterator_tag iterator_category;
  typedef int difference_type;

  explicit tui_window_iterator (enum tui_win_type type)
    : m_type (type)
  {
    advance ();
  }

  tui_window_iterator ()
    : m_type (MAX_MAJOR_WINDOWS)
  {
  }

  bool operator!= (const self_type &other) const
  {
    return m_type != other.m_type;
  }

  value_type operator* () const
  {
    gdb_assert (m_type < MAX_MAJOR_WINDOWS);
    return tui_win_list[m_type];
  }

  self_type &operator++ ()
  {
    ++m_type;
    advance ();
    return *this;
  }

private:

  void advance ()
  {
    while (m_type < MAX_MAJOR_WINDOWS && tui_win_list[m_type] == nullptr)
      ++m_type;
  }

  int m_type;
};

/* A range adapter for iterating over TUI windows.  */

struct all_tui_windows
{
  tui_window_iterator begin () const
  {
    return tui_window_iterator (SRC_WIN);
  }

  tui_window_iterator end () const
  {
    return tui_window_iterator ();
  }
};


/* Data Manipulation Functions.  */
extern enum tui_layout_type tui_current_layout (void);
extern int tui_term_height (void);
extern void tui_set_term_height_to (int);
extern int tui_term_width (void);
extern void tui_set_term_width_to (int);
extern struct tui_locator_window *tui_locator_win_info_ptr (void);
extern struct tui_win_info *tui_win_with_focus (void);
extern void tui_set_win_with_focus (struct tui_win_info *);
extern bool tui_win_resized ();
extern void tui_set_win_resized_to (bool);

extern struct tui_win_info *tui_next_win (struct tui_win_info *);
extern struct tui_win_info *tui_prev_win (struct tui_win_info *);

/* Delete all the invisible windows.  Note that it is an error to call
   this when the command window is invisible -- we don't allow the
   command window to be removed from the layout.  */
extern void tui_delete_invisible_windows ();

extern unsigned int tui_tab_width;

#endif /* TUI_TUI_DATA_H */
