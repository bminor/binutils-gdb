/* This testcase is part of GDB, the GNU debugger.

   Copyright 2004-2025 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static void *p;

void
pass (void)
{
}

static void
handler (int sig, siginfo_t *info, void *context)
{
  if (info->si_addr == p)
    pass ();
  _exit (0);
}

int
main (void)
{
  /* Set up unwritable memory.  */
  {
    size_t len;
    len = sysconf(_SC_PAGESIZE);
    p = mmap (0, len, PROT_NONE, MAP_ANON|MAP_PRIVATE, -1, 0);
    if (p == MAP_FAILED)
      {
	perror ("mmap");
	return 1;
      }
  }
  /* Set up the signal handler.  */
  {
    struct sigaction action;
    memset (&action, 0, sizeof (action));
    action.sa_sigaction = handler;
    action.sa_flags |= SA_SIGINFO;
    if (sigaction (SIGSEGV, &action, NULL))
      {
	perror ("sigaction");
	return 1;
      }
  }
  /* Trigger SIGSEGV.  */
  *(int *)p = 0;
  return 0;
}
