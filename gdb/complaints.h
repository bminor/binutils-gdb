/* Definitions for complaint handling during symbol reading in GDB.

   Copyright 1990, 1991, 1992, 1995, 1998, 2000, 2002 Free Software
   Foundation, Inc.

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


#if !defined (COMPLAINTS_H)
#define COMPLAINTS_H

/* Opaque object used to track the number of complaints of a
   particular category.  */
struct complaints;

/* Predefined categories.  */
extern struct complaints *symfile_complaints;

/* Register a complaint.  */
extern void complaint (struct complaints **complaints, const char *fmt,
		       ...) ATTR_FORMAT (printf, 2, 3);
extern void internal_complaint (struct complaints **complaints,
				const char *file, int line, const char *fmt,
				...) ATTR_FORMAT (printf, 4, 5);

/* Clear out / initialize all complaint counters that have ever been
   incremented.  If LESS_VERBOSE is 1, be less verbose about
   successive complaints, since the messages are appearing all
   together during a command that is reporting a contiguous block of
   complaints (rather than being interleaved with other messages).  If
   noisy is 1, we are in a noisy command, and our caller will print
   enough context for the user to figure it out.  */

extern void clear_complaints (struct complaints **complaints,
			      int less_verbose, int noisy);


/* Legacy interfaces to keep the old code working (until it is all
   converted to the above).  While the structure below contains a
   number of fields, all but .message are ignored.

   Support for complaining about things in the symbol file that aren't
   catastrophic.

   Each such thing gets a counter.  The first time we have the problem,
   during a symbol read, we report it.  At the end of symbol reading,
   if verbose, we report how many of each problem we had.  */

struct complaint
{
  const char *message;
  unsigned counter_ignored;
  struct complaint *next_ignored;
};

extern void complain (struct complaint *, ...);

#endif /* !defined (COMPLAINTS_H) */
