/* This testcase is part of GDB, the GNU debugger.

   Copyright 2025 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <assert.h>

/* This is a simple, empty function designed to be a stable target
   for a GDB breakpoint.  Both the parent and child processes will
   call this function after the fork.  */

void
breakpt (void)
{
  /* Nothing.  */
}

int
main (void)
{
  pid_t child_pid;

  /* Create a new process.  */
  child_pid = fork ();

  assert (child_pid >= 0);

  if (child_pid == 0)
    {
      /* This is the child process.  Call the breakpoint function.  */
      breakpt ();

      exit (0);
    }
  else
    {
      /* This is the parent process.  */
      int child_status;

      /* Call the breakpoint function.  */
      breakpt ();

      /* Wait for the child process to terminate.  */
      waitpid (child_pid, &child_status, 0);

      assert (WIFEXITED (child_status));
      assert (WEXITSTATUS (child_status) == 0);

      exit (0);
    }

  /* This line should not be reached.  */
  return 1;
}
