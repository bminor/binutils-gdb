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

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sframe-backtrace-api.h"

#ifdef __has_attribute
# if !__has_attribute (noclone)
#  define ATTRIBUTE_NOCLONE
# endif
#endif
#ifndef ATTRIBUTE_NOCLONE
# define ATTRIBUTE_NOCLONE __attribute__((noclone))
#endif

#define BT_BUF_SIZE 16

/* funclist for running tailcall.  */
const char *const func_list[] =
{
  "show_bt",
  "dec",
  "dec",
  "main"
};

void show_bt ()
{
  void *buffer[BT_BUF_SIZE];
  int j, nptrs, err;
  char **strings;

  /* Call the unwinder to get an array of return addresses.  */
  nptrs = sframe_backtrace (buffer, BT_BUF_SIZE, &err);
  if (nptrs == -1 || nptrs != 4)
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

  /* Verify the results.  */
  for (j = 0; j < nptrs; j++)
    if (!strstr (strings[j], func_list[j]))
      break;

  free(strings);

  printf ("%s: tailcall test\n", j == nptrs ? "PASS" : "FAIL");
}

/* An example of tail recursive function.  */
void __attribute__((__noinline__)) ATTRIBUTE_NOCLONE
dec (int n)
{
  void *buffer[BT_BUF_SIZE];
  int nptrs, err;

  /* Call the unwinder to get an array of return addresses.  */
  nptrs = sframe_backtrace (buffer, BT_BUF_SIZE, &err);
  if (nptrs == -1)
    {
      printf ("SFrame error: %s (%d)\n", sframe_bt_errmsg (err), nptrs);
      return;
    }

  if (n < 0)
     return;

  if (n == 2)
    show_bt ();

  /* The last executed statement is recursive call.  */
  dec (n-1);
}

int
main (void)
{
  dec (3);
}
