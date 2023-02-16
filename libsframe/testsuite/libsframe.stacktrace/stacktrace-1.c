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

/* This is a revised version of gdb/testsuite/gdb.base/backtrace.c.  */

#ifdef __has_attribute
# if !__has_attribute (noclone)
#  define ATTRIBUTE_NOCLONE
# endif
#endif
#ifndef ATTRIBUTE_NOCLONE
# define ATTRIBUTE_NOCLONE __attribute__((noclone))
#endif

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sframe-stacktrace-api.h"

#define BT_BUF_SIZE 100

#define BT_EXPECTED_NPTRS 3
/* Expected funclist.  */
static const char *const func_list[] =
{
  "show_bt",
  "bar",
  "main"
};

void __attribute__((__noinline__)) ATTRIBUTE_NOCLONE
show_bt ()
{
  void *buffer[BT_BUF_SIZE];
  int j, nptrs, err = 0;
  char **strings;

  /* Call the unwinder to get an array of return addresses.  */
  // nptrs = backtrace (buffer, BT_BUF_SIZE);
  nptrs = sframe_stacktrace (buffer, BT_BUF_SIZE, &err);
  if (err)
    {
      printf ("SFrame error: %s (%d)\n", sframe_bt_errmsg (err), nptrs);
      return;
    }

  /* Get these addresses symbolically.  */
  strings = backtrace_symbols (buffer, nptrs);
  if (strings == NULL) {
      printf ("Error in backtrace_symbols");
      return;
  }

  // for (j = 0; j < nptrs; j++)
    // printf (" %s \n", strings[j]);

  if (nptrs != BT_EXPECTED_NPTRS)
    {
      printf ("Backtace nptrs mismatch: expected = %d, generated = %d \n",
	      BT_EXPECTED_NPTRS, nptrs);
      return;
    }

  /* Verify the results.  */
  for (j = 0; j < nptrs; j++)
    if (!strstr (strings[j], func_list[j]))
      break;

  free(strings);

  printf ("%s: stacktrace-1\n", (j == nptrs) ? "PASS" : "FAIL");
}

int __attribute__((__noinline__)) ATTRIBUTE_NOCLONE
bar ()
{
  show_bt ();
  return 0;
}

int __attribute__((__noinline__)) ATTRIBUTE_NOCLONE
main ()
{
  void *buffer[BT_BUF_SIZE];
  int nptrs, err;

  /* The following call to sframe_stacktrace () also prevents sibling call
     optimization in main ().  */
  /* Call the unwinder to get an array of return addresses.  */
  nptrs = sframe_stacktrace (buffer, BT_BUF_SIZE, &err);
  if (nptrs == -1)
    {
      printf ("SFrame error: %s (%d)\n", sframe_bt_errmsg (err), nptrs);
      return -1;
    }

  return bar ();
}
