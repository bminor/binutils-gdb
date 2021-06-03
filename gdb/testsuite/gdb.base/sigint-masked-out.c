/* This testcase is part of GDB, the GNU debugger.

   Copyright 2021 Free Software Foundation, Inc.

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

#include <unistd.h>
#include <signal.h>

static void
done ()
{
}

int
main (int argc, char *argv[])
{
  int i;
  sigset_t sigs;

  alarm (30);

  sigfillset (&sigs);
  sigprocmask (SIG_SETMASK, &sigs, NULL);

  done ();

  while (1)
    sleep (1);

  return 0;
}
