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
#include "sframe-backtrace-api.h"

#define BT_BUF_SIZE 100

/* Expected funclist.  */
static const char *const func_list[] =
{
  "show_bt",
  "baz",
  "bar",
  "foo",
  "main"
};

void __attribute__((__noinline__)) ATTRIBUTE_NOCLONE
show_bt ()
{
  void *buffer[BT_BUF_SIZE];
  int j, nptrs, err;
  char **strings;

  /* Call the unwinder to get an array of return addresses.  */
  nptrs = sframe_backtrace (buffer, BT_BUF_SIZE, &err);
  if (nptrs != 5)
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

  printf ("%s: backtrace test\n", j == nptrs ? "PASS" : "FAIL");
}

int __attribute__((__noinline__)) ATTRIBUTE_NOCLONE
baz ()
{
  void *buffer[BT_BUF_SIZE];
  int nptrs, err;

  /* Call the unwinder to get an array of return addresses.  */
  nptrs = sframe_backtrace (buffer, BT_BUF_SIZE, &err);
  if (nptrs == -1)
    {
      printf ("SFrame error: %s (%d)\n", sframe_bt_errmsg (err), nptrs);
      return -1;
    }

  show_bt ();
  return 0;
}

int __attribute__((__noinline__)) ATTRIBUTE_NOCLONE
bar ()
{
  void *buffer[BT_BUF_SIZE];
  int nptrs, err;

  /* Call the unwinder to get an array of return addresses.  */
  nptrs = sframe_backtrace (buffer, BT_BUF_SIZE, &err);
  if (nptrs == -1)
    {
      printf ("SFrame error: %s (%d)\n", sframe_bt_errmsg (err), nptrs);
      return -1;
    }

  return baz ();
}

int __attribute__((__noinline__)) ATTRIBUTE_NOCLONE
foo ()
{
  void *buffer[BT_BUF_SIZE];
  int nptrs, err;

  /* Call the unwinder to get an array of return addresses.  */
  nptrs = sframe_backtrace (buffer, BT_BUF_SIZE, &err);
  if (nptrs == -1)
    {
      printf ("SFrame error: %s (%d)\n", sframe_bt_errmsg (err), nptrs);
      return -1;
    }

  return bar ();
}

int __attribute__((__noinline__)) ATTRIBUTE_NOCLONE
main ()
{
  void *buffer[BT_BUF_SIZE];
  int nptrs, err;

  /* Call the unwinder to get an array of return addresses.  */
  nptrs = sframe_backtrace (buffer, BT_BUF_SIZE, &err);
  if (nptrs == -1)
    {
      printf ("SFrame error: %s (%d)\n", sframe_bt_errmsg (err), nptrs);
      return -1;
    }

  return foo ();
}
