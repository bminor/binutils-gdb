/* Linux-specific ptrace manipulation routines.
   Copyright (C) 2012 Free Software Foundation, Inc.

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

#ifdef GDBSERVER
#include "server.h"
#else
#include "defs.h"
#include "gdb_string.h"
#endif

#include "linux-ptrace.h"
#include "linux-procfs.h"
#include "buffer.h"

/* Find all possible reasons we could fail to attach PID and append these
   newline terminated reason strings to initialized BUFFER.  '\0' termination
   of BUFFER must be done by the caller.  */

void
linux_ptrace_attach_warnings (pid_t pid, struct buffer *buffer)
{
  pid_t tracerpid;

  tracerpid = linux_proc_get_tracerpid (pid);
  if (tracerpid > 0)
    buffer_xml_printf (buffer, _("warning: process %d is already traced "
				 "by process %d\n"),
		       (int) pid, (int) tracerpid);

  if (linux_proc_pid_is_zombie (pid))
    buffer_xml_printf (buffer, _("warning: process %d is a zombie "
				 "- the process has already terminated\n"),
		       (int) pid);
}
