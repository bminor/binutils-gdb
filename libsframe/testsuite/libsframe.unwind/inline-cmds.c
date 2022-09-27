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

/* This is only ever run if it is compiled with a new-enough GCC, but
   we don't want the compilation to fail if compiled by some other
   compiler.  */

/* This is a revised version of gdb/testsuite/gdb.opt/inline-cmds.c.  */

#ifdef __GNUC__
#define ATTR __attribute__((always_inline))
#else
#define ATTR
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include "sframe-backtrace-api.h"

#define BT_BUF_SIZE 10

int x, y;
volatile int z = 0;
volatile int result;

void noinline(void);

inline ATTR void outer_inline1(void)
{
  noinline ();
}

inline ATTR void outer_inline2(void)
{
  outer_inline1 ();
}

int main (void)
{ /* start of main */
  int val;

  x = 7;
  y = 8;

  outer_inline2 ();

  return 0;
}


/* funclist for inline-cmds.  */
const char *const func_list[] =
{
  "noinline",
  "main"
};

inline ATTR void inlined_fn(void)
{
  x += y + z;

  void *buffer[BT_BUF_SIZE];
  char **strings;
  /* Call the unwinder to get an array of return addresses.  */
  int j, err;
  int nptrs = sframe_backtrace (buffer, BT_BUF_SIZE, &err);
  if (nptrs == -1 || nptrs != 2)
    {
      printf ("SFrame error: %s (%d)\n", sframe_bt_errmsg (err), nptrs);
      return;
    }

  /* Get these addresses symbolically.  */
  strings = backtrace_symbols (buffer, nptrs);
  if (strings == NULL)
    {
      printf ("Error in backtrace_symbols");
      return;
    }

  /* Verify the results.  */
  for (j = 0; j < nptrs; j++)
    if (!strstr (strings[j], func_list[j]))
      break;

    free(strings);
    printf ("%s: inline-cmds test\n", j == nptrs ? "PASS" : "FAIL");
}

void noinline(void)
{
  inlined_fn (); /* inlined */
}
