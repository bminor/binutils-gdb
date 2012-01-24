/* Test case for forgotten hw-watchpoints after fork()-off of a process.

   Copyright 2012 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#include "watchpoint-fork.h"

volatile int var;

void
marker (void)
{
}

static void
mark_exit (void)
{
}

int
main (void)
{
  setbuf (stdout, NULL);
  printf ("main: %d\n", (int) getpid ());

  /* General hardware breakpoints and watchpoints validity.  */
  marker ();
  var++;
  /* Hardware watchpoints got disarmed here.  */
  forkoff (1);
  /* This watchpoint got lost before.  */
  var++;
  /* A sanity check for double hardware watchpoints removal.  */
  forkoff (2);
  var++;

  mark_exit ();
  return 0;
}
