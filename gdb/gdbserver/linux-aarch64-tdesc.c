/* GNU/Linux/aarch64 specific target description, for the remote server
   for GDB.
   Copyright (C) 2017-2018 Free Software Foundation, Inc.

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

#include "server.h"
#include "tdesc.h"
#include "arch/aarch64.h"
#include "linux-aarch32-low.h"

/* Create the aarch64 target description.  */

const target_desc *
aarch64_linux_read_description ()
{
  static target_desc *aarch64_tdesc = NULL;
  target_desc **tdesc = &aarch64_tdesc;

  if (*tdesc == NULL)
    {
      *tdesc = aarch64_create_target_description ();

      init_target_desc (*tdesc);

#ifndef IN_PROCESS_AGENT
      static const char *expedite_regs_aarch64[] = { "x29", "sp", "pc", NULL };
      (*tdesc)->expedite_regs = expedite_regs_aarch64;
#endif
    }

  return *tdesc;
}
