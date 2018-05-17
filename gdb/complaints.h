/* Definitions for complaint handling during symbol reading in GDB.

   Copyright (C) 1990-2018 Free Software Foundation, Inc.

   This file is part of GDB.

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


#if !defined (COMPLAINTS_H)
#define COMPLAINTS_H

/* Opaque object used to track the number of complaints of a
   particular category.  */
struct complaints;

/* Helper for complaint.  */
extern void complaint_internal (const char *fmt, ...)
  ATTRIBUTE_PRINTF (1, 2);

/* Register a complaint.  This is a macro around complaint_internal to
   avoid computing complaint's arguments when complaints are disabled.
   Running FMT via gettext [i.e., _(FMT)] can be quite expensive, for
   example.  */
#define complaint(FMT, ...)					\
  do								\
    {								\
      extern int stop_whining;					\
								\
      if (stop_whining > 0)					\
	complaint_internal (FMT, ##__VA_ARGS__);		\
    }								\
  while (0)

/* Clear out / initialize all complaint counters that have ever been
   incremented.  If LESS_VERBOSE is 1, be less verbose about
   successive complaints, since the messages are appearing all
   together during a command that is reporting a contiguous block of
   complaints (rather than being interleaved with other messages).  If
   noisy is 1, we are in a noisy command, and our caller will print
   enough context for the user to figure it out.  */

extern void clear_complaints (int less_verbose);


#endif /* !defined (COMPLAINTS_H) */
