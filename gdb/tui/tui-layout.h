/* TUI layout window management.

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

#ifndef TUI_TUI_LAYOUT_H
#define TUI_TUI_LAYOUT_H

#include "tui/tui.h"
#include "tui/tui-data.h"

/* The basic object in a TUI layout.  This represents a single piece
   of screen real estate.  Subclasses determine the exact
   behavior.  */
class tui_layout_base
{
public:

  DISABLE_COPY_AND_ASSIGN (tui_layout_base);

  /* Clone this object.  Ordinarily a layout is cloned before it is
     used, so that any necessary modifications do not affect the
     "skeleton" layout.  */
  virtual std::unique_ptr<tui_layout_base> clone () const = 0;

  /* Change the size and location of this layout.  */
  virtual void apply (int x, int y, int width, int height) = 0;

  /* Return the minimum and maximum height of this layout.  */
  virtual void get_sizes (int *min_height, int *max_height) = 0;

  /* True if the topmost item in this layout is boxed.  */
  virtual bool top_boxed_p () const = 0;

  /* True if the bottommost item in this layout is boxed.  */
  virtual bool bottom_boxed_p () const = 0;

  /* Return the name of this layout's window, or nullptr if this
     layout does not represent a single window.  */
  virtual const char *get_name () const
  {
    return nullptr;
  }

  /* Adjust the size of the window named NAME to NEW_HEIGHT, updating
     the sizes of the other windows around it.  */
  virtual bool adjust_size (const char *name, int new_height) = 0;

  /* The most recent space allocation.  */
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;

protected:

  tui_layout_base () = default;
};

/* A TUI layout object that displays a single window.  The window is
   given by name.  */
class tui_layout_window : public tui_layout_base
{
public:

  explicit tui_layout_window (const char *name)
    : m_contents (name)
  {
  }

  DISABLE_COPY_AND_ASSIGN (tui_layout_window);

  std::unique_ptr<tui_layout_base> clone () const override;

  void apply (int x, int y, int width, int height) override;

  const char *get_name () const override
  {
    return m_contents.c_str ();
  }

  bool adjust_size (const char *name, int new_height) override
  {
    return false;
  }

  bool top_boxed_p () const override;

  bool bottom_boxed_p () const override;

protected:

  void get_sizes (int *min_height, int *max_height) override;

private:

  /* Type of content to display.  */
  std::string m_contents;

  /* When a layout is applied, this is updated to point to the window
     object.  */
  tui_gen_win_info *m_window = nullptr;
};

/* A TUI layout that holds other layouts.  */
class tui_layout_split : public tui_layout_base
{
public:

  tui_layout_split () = default;

  DISABLE_COPY_AND_ASSIGN (tui_layout_split);

  /* Add a new split layout to this layout.  WEIGHT is the desired
     size, which is relative to the other weights given in this
     layout.  */
  tui_layout_split *add_split (int weight);

  /* Add a new window to this layout.  NAME is the name of the window
     to add.  WEIGHT is the desired size, which is relative to the
     other weights given in this layout.  */
  void add_window (const char *name, int weight);

  std::unique_ptr<tui_layout_base> clone () const override;

  void apply (int x, int y, int width, int height) override;

  bool adjust_size (const char *name, int new_height) override;

  bool top_boxed_p () const override;

  bool bottom_boxed_p () const override;

protected:

  void get_sizes (int *min_height, int *max_height) override;

private:

  /* Set the weights from the current heights.  */
  void set_weights_from_heights ();

  struct split
  {
    /* The requested weight.  */
    int weight;
    /* The layout.  */
    std::unique_ptr<tui_layout_base> layout;
  };

  /* The splits.  */
  std::vector<split> m_splits;

  /* True if this layout has already been applied at least once.  */
  bool m_applied = false;
};

extern void tui_add_win_to_layout (enum tui_win_type);
extern void tui_set_layout (enum tui_layout_type);

/* Apply the current layout.  */
extern void tui_apply_current_layout ();

/* Adjust the window height of WIN to NEW_HEIGHT.  */
extern void tui_adjust_window_height (struct tui_win_info *win,
				      int new_height);

#endif /* TUI_TUI_LAYOUT_H */
