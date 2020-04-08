/* Internal interfaces for the Windows code
   Copyright (C) 1995-2020 Free Software Foundation, Inc.

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

namespace windows_nat
{

HANDLE current_process_handle;
DWORD current_process_id;
DWORD main_thread_id;
enum gdb_signal last_sig = GDB_SIGNAL_0;
DEBUG_EVENT current_event;
DEBUG_EVENT last_wait_event;
windows_thread_info *current_windows_thread;
DWORD desired_stop_thread_id = -1;
std::vector<pending_stop> pending_stops;
EXCEPTION_RECORD siginfo_er;

windows_thread_info::~windows_thread_info ()
{
  CloseHandle (h);
}

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

const char *
get_image_name (HANDLE h, void *address, int unicode)
{
#ifdef __CYGWIN__
  static char buf[MAX_PATH];
#else
  static char buf[(2 * MAX_PATH) + 1];
#endif
  DWORD size = unicode ? sizeof (WCHAR) : sizeof (char);
  char *address_ptr;
  int len = 0;
  char b[2];
  SIZE_T done;

  /* Attempt to read the name of the dll that was detected.
     This is documented to work only when actively debugging
     a program.  It will not work for attached processes.  */
  if (address == NULL)
    return NULL;

#ifdef _WIN32_WCE
  /* Windows CE reports the address of the image name,
     instead of an address of a pointer into the image name.  */
  address_ptr = address;
#else
  /* See if we could read the address of a string, and that the
     address isn't null.  */
  if (!ReadProcessMemory (h, address,  &address_ptr,
			  sizeof (address_ptr), &done)
      || done != sizeof (address_ptr)
      || !address_ptr)
    return NULL;
#endif

  /* Find the length of the string.  */
  while (ReadProcessMemory (h, address_ptr + len++ * size, &b, size, &done)
	 && (b[0] != 0 || b[size - 1] != 0) && done == size)
    continue;

  if (!unicode)
    ReadProcessMemory (h, address_ptr, buf, len, &done);
  else
    {
      WCHAR *unicode_address = (WCHAR *) alloca (len * sizeof (WCHAR));
      ReadProcessMemory (h, address_ptr, unicode_address, len * sizeof (WCHAR),
			 &done);
#ifdef __CYGWIN__
      wcstombs (buf, unicode_address, MAX_PATH);
#else
      WideCharToMultiByte (CP_ACP, 0, unicode_address, len, buf, sizeof buf,
			   0, 0);
#endif
    }

  return buf;
}

}
