/* Abstract base class inherited by all process_stratum targets

   Copyright (C) 2018 Free Software Foundation, Inc.

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
#include "process-stratum-target.h"
#include "inferior.h"

process_stratum_target::~process_stratum_target ()
{
}

struct address_space *
process_stratum_target::thread_address_space (ptid_t ptid)
{
  /* Fall-back to the "main" address space of the inferior.  */
  inferior *inf = find_inferior_ptid (ptid);

  if (inf == NULL || inf->aspace == NULL)
    internal_error (__FILE__, __LINE__,
		    _("Can't determine the current "
		      "address space of thread %s\n"),
		    target_pid_to_str (ptid));

  return inf->aspace;
}

struct gdbarch *
process_stratum_target::thread_architecture (ptid_t ptid)
{
  inferior *inf = find_inferior_ptid (ptid);
  gdb_assert (inf != NULL);
  return inf->gdbarch;
}
