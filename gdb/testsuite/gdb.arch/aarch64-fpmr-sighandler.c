/* This file is part of GDB, the GNU debugger.

   Copyright 2008-2025 Free Software Foundation, Inc.

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

#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void
set_fpmr (uint64_t value)
{
  register uint64_t x0_val asm ("x0") = value;
  /* msr	fpmr, x0 */
  __asm__ volatile (".inst	0xd51b4440" : : );
}

void
handler (int sig)
{
  set_fpmr (0xff008041);
  exit (0);
}

int
main ()
{
  /* Ensure no signals are blocked.  */
  sigset_t newset;
  sigemptyset (&newset);
  sigprocmask (SIG_SETMASK, &newset, NULL);

  signal (SIGILL, handler);

  set_fpmr (0x3fff7fc049);

  /* 0x06000000 : Causes an illegal instruction.  Value undefined as per ARM
     Architecture Reference Manual ARMv8, Section C4.1.  */
  __asm __volatile (".inst 0x06000000");

  return 0;
}
