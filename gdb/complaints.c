/* Support for complaint handling during symbol reading in GDB.

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

#include "defs.h"
#include "complaints.h"
#include "command.h"
#include "gdbcmd.h"
#include <unordered_map>

/* Should each complaint message be self explanatory, or should we
   assume that a series of complaints is being produced?  */

enum complaint_series {
  /* Isolated self explanatory message.  */
  ISOLATED_MESSAGE,

  /* First message of a series, but does not need to include any sort
     of explanation.  */
  SHORT_FIRST_MESSAGE,
};

/* Map format strings to counters.  */

static std::unordered_map<const char *, int> counters;

/* How to print the next complaint.  */

static complaint_series series;

/* How many complaints about a particular thing should be printed
   before we stop whining about it?  Default is no whining at all,
   since so many systems have ill-constructed symbol files.  */

int stop_whining = 0;

/* See complaints.h.  */

void
complaint_internal (const char *fmt, ...)
{
  va_list args;

  if (counters[fmt]++ > stop_whining)
    return;

  va_start (args, fmt);

  if (deprecated_warning_hook)
    (*deprecated_warning_hook) (fmt, args);
  else
    {
      std::string msg = string_vprintf (fmt, args);
      wrap_here ("");
      begin_line ();
      if (series == ISOLATED_MESSAGE)
	fprintf_filtered (gdb_stderr, "During symbol reading, %s.\n",
			  msg.c_str ());
      else
	fprintf_filtered (gdb_stderr, "%s...", msg.c_str ());
    }

  /* If GDB dumps core, we'd like to see the complaints first.
     Presumably GDB will not be sending so many complaints that this
     becomes a performance hog.  */

  gdb_flush (gdb_stderr);
  va_end (args);
}

/* Clear out / initialize all complaint counters that have ever been
   incremented.  If LESS_VERBOSE is 1, be less verbose about
   successive complaints, since the messages are appearing all
   together during a command that is reporting a contiguous block of
   complaints (rather than being interleaved with other messages).  */

void
clear_complaints (int less_verbose)
{
  struct complain *p;

  counters.clear ();

  if (!less_verbose)
    series = ISOLATED_MESSAGE;
  else
    series = SHORT_FIRST_MESSAGE;
}

static void
complaints_show_value (struct ui_file *file, int from_tty,
		       struct cmd_list_element *cmd, const char *value)
{
  fprintf_filtered (file, _("Max number of complaints about incorrect"
			    " symbols is %s.\n"),
		    value);
}

void
_initialize_complaints (void)
{
  add_setshow_zinteger_cmd ("complaints", class_support, 
			    &stop_whining, _("\
Set max number of complaints about incorrect symbols."), _("\
Show max number of complaints about incorrect symbols."), NULL,
			    NULL, complaints_show_value,
			    &setlist, &showlist);
}
