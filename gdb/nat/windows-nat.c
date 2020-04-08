/* Internal interfaces for the Windows code
   Copyright (C) 1995-2019 Free Software Foundation, Inc.

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

#include "gdbsupport/common-defs.h"
#include "nat/windows-nat.h"

void
windows_thread_info::suspend ()
{
  if (suspended != 0)
    return;

  if (SuspendThread (h) == (DWORD) -1)
    {
      DWORD err = GetLastError ();

      /* We get Access Denied (5) when trying to suspend
	 threads that Windows started on behalf of the
	 debuggee, usually when those threads are just
	 about to exit.
	 We can get Invalid Handle (6) if the main thread
	 has exited.  */
      if (err != ERROR_INVALID_HANDLE && err != ERROR_ACCESS_DENIED)
	warning (_("SuspendThread (tid=0x%x) failed. (winerr %u)"),
		 (unsigned) tid, (unsigned) err);
      suspended = -1;
    }
  else
    suspended = 1;
}

void
windows_thread_info::resume ()
{
  if (suspended > 0)
    {
      stopped_at_software_breakpoint = false;

      if (ResumeThread (h) == (DWORD) -1)
	{
	  DWORD err = GetLastError ();
	  warning (_("warning: ResumeThread (tid=0x%x) failed. (winerr %u)"),
		   (unsigned) tid, (unsigned) err);
	}
    }
  suspended = 0;
}
