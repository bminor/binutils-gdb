/* C language support for compilation.

   Copyright (C) 2015, 2016 Free Software Foundation, Inc.

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

#include "gcc-cp-interface.h"

void
gcc_cplus_enter_scope (void *datum, struct gcc_cp_context *gcc_context)
{
  /* FIXME: enter the scope in which the user expression is supposed
     to be parsed.  -lxo  */
}

void
gcc_cplus_leave_scope (void *datum, struct gcc_cp_context *gcc_context)
{
  /* FIXME: leave the scopes entered by gcc_cplus_enter_scope.  -lxo  */
}
