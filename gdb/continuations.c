/* Continuations for GDB, the GNU debugger.

   Copyright (C) 1986-2021 Free Software Foundation, Inc.

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

#include "defs.h"
#include "gdbthread.h"
#include "inferior.h"
#include "continuations.h"

/* Add a continuation to the continuation list of INFERIOR.  The new
   continuation will be added at the front.  */

void
add_inferior_continuation (std::function<void ()> &&cont)
{
  struct inferior *inf = current_inferior ();

  inf->continuations.emplace_front (std::move (cont));
}

/* Do all continuations of the current inferior.  */

void
do_all_inferior_continuations ()
{
  struct inferior *inf = current_inferior ();
  while (!inf->continuations.empty ())
    {
      auto iter = inf->continuations.begin ();
      (*iter) ();
      inf->continuations.erase (iter);
    }
}

/* Get rid of all the inferior-wide continuations of INF.  */

void
discard_all_inferior_continuations (struct inferior *inf)
{
  inf->continuations.clear ();
}
