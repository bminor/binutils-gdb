/* sframe-backtrace-err.c - SFrame Backtrace Error table.

   Copyright (C) 2022 Free Software Foundation, Inc.

   This file is part of libsframebt.

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
#include "sframe-backtrace-api.h"

/* SFrame backtrace error messages.  */
static const char *const sframe_bt_errlist[] =
{
  "",
  "File does not contain SFrame data",
  "Iterating shared object reading error",
  "Failed to malloc memory space",
  "Failed to realloc memory space",
  "Failed to open file",
  "Failed on resolve canonical file name",
  "Failed to reposition file offset",
  "Failed to read from a file descriptor",
  "Failed to get the user context",
  "Failed to set up decode data",
  "Illegal CFA offset"
};

/* Return the error message associated with the error code.  */

const char *
sframe_bt_errmsg (enum sframe_bt_errcode ecode)
{
  if ((unsigned int)ecode >= ENUM_ARRAY_SIZE (sframe_bt_errlist))
    return "Unknown error";

  return sframe_bt_errlist[ecode];
}
