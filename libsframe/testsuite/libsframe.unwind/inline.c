/* Copyright (C) 2022 Free Software Foundation, Inc.

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

/* This is a revised version of gdb/testsuite/gdb.opt/inline-bt.c.  */

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sframe-backtrace-api.h"

#define ATTR __attribute__((always_inline))

#define BT_BUF_SIZE 32

int x, y;
volatile int z = 0;
volatile int result;

/* funclist.  */
const char *const flist[] =
{
  "main"
};

void bar(void)
{
  x += y;
}

inline ATTR int func1(void)
{
  bar ();
  return x * y;
}

inline ATTR int func2(void)
{
  void *buffer[BT_BUF_SIZE];
  int ok = 0, nptrs, err;
  char **strings;

  /* Call the unwinder to get an array of return addresses.  */
  nptrs = sframe_backtrace (buffer, BT_BUF_SIZE, &err);
  if (nptrs == -1)
    {
      printf ("SFrame error: %s\n", sframe_bt_errmsg (err));
      return -1;
    }

  /* Get these addresses symbolically.  */
  strings = backtrace_symbols (buffer, nptrs);
  if (strings == NULL) {
      printf ("Error in backtrace_symbols");
      return -1;
  }

  /* Verify the results.  */
  if (nptrs == 1 && strstr (strings[0], flist[0]))
    ok = 1;

  free(strings);

  printf ("%s: unwind test\n", ok == 1 ? "PASS" : "FAIL");

  return x * func1 ();
}

int main (void)
{
  int val;

  x = 7;
  y = 8;
  bar ();

  val = func1 ();
  result = val;

  val = func2 ();
  result = val;

  return 0;
}
