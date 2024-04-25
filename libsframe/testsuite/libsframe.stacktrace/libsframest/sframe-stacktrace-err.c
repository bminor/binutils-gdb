/* sframe-stacktrace-err.c - SFrame Stacktrace Error table.

   Copyright (C) 2023 Free Software Foundation, Inc.

   This file is part of libsframest.

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

#include "config.h"
#include "sframe-stacktrace-api.h"

/* SFrame stacktrace error messages.  */
static const char *const sframe_bt_errlist[] =
{
  "",
  "Failed to setup SFrame state for stack tracing",
  "Bad arguments; Unhealthy SFrame state possible",
  "Failed to malloc memory space",
  "Failed to realloc memory space",
  "Failed to open file",
  "Failed on resolve canonical file name",
  "Failed to reposition file offset",
  "Failed to read from a file descriptor",
  "Failed to get the user context",
  "Failed to set up decode data",
  "Illegal CFA offset",
  "Corrupt FRE"
};

/* Return the error message associated with the error code.  */

const char *
sframe_bt_errmsg (enum sframe_bt_errcode ecode)
{
  if ((unsigned int)ecode >= ENUM_ARRAY_SIZE (sframe_bt_errlist))
    return "Unknown error";

  return sframe_bt_errlist[ecode];
}

/* Check if there is error code in ERRP.  */

int
sframe_bt_errno (const int *errp)
{
  if (!errp)
    return 0;

  return (*errp != SFRAME_BT_OK);
}

int
sframe_bt_ret_set_errno (int *errp, int error)
{
  if (errp)
    *errp = error;

  return error;
}
