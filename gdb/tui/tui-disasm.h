/* Disassembly display.

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

#ifndef TUI_TUI_DISASM_H
#define TUI_TUI_DISASM_H

#include "tui/tui.h"
#include "tui/tui-data.h"
#include "tui-winsource.h"

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

  bool location_matches_p (struct bp_location *loc, int line_no) override;

  void maybe_update (struct frame_info *fi, symtab_and_line sal,
		     int line_no, CORE_ADDR addr)
    override;

  void erase_source_content () override
  {
    do_erase_source_content (_("[ No Assembly Available ]"));
  }

protected:

  void do_scroll_vertical (int num_to_scroll) override;

  enum tui_status set_contents
    (struct gdbarch *gdbarch,
     struct symtab *s,
     struct tui_line_or_address line_or_addr) override;

private:
  /* Answer whether a particular line number or address is displayed
     in the current source window.  */
  bool addr_is_displayed (CORE_ADDR addr) const;
};

extern void tui_show_disassem (struct gdbarch *, CORE_ADDR);
extern void tui_show_disassem_and_update_source (struct gdbarch *, CORE_ADDR);
extern void tui_get_begin_asm_address (struct gdbarch **, CORE_ADDR *);

#endif /* TUI_TUI_DISASM_H */
