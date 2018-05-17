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

/* Should each complaint message be self explanatory, or should we
   assume that a series of complaints is being produced?  */

enum complaint_series {
  /* Isolated self explanatory message.  */
  ISOLATED_MESSAGE,

  /* First message of a series, but does not need to include any sort
     of explanation.  */
  SHORT_FIRST_MESSAGE,
};

/* Structure to manage complaints about symbol file contents.  */

struct complain
{
  const char *file;
  int line;
  const char *fmt;
  int counter;
  struct complain *next;
};

struct complaints
{
  struct complain *root;

  enum complaint_series series;
};

static struct complain complaint_sentinel;

static struct complaints symfile_complaint_book = {
  &complaint_sentinel,
  ISOLATED_MESSAGE
};

static struct complain * ATTRIBUTE_PRINTF (4, 0)
find_complaint (struct complaints *complaints, const char *file,
		int line, const char *fmt)
{
  struct complain *complaint;

  /* Find the complaint in the table.  A more efficient search
     algorithm (based on hash table or something) could be used.  But
     that can wait until someone shows evidence that this lookup is
     a real bottle neck.  */
  for (complaint = complaints->root;
       complaint != NULL;
       complaint = complaint->next)
    {
      if (complaint->fmt == fmt
	  && complaint->file == file
	  && complaint->line == line)
	return complaint;
    }

  /* Oops not seen before, fill in a new complaint.  */
  complaint = XNEW (struct complain);
  complaint->fmt = fmt;
  complaint->file = file;
  complaint->line = line;
  complaint->counter = 0;
  complaint->next = NULL;

  /* File it, return it.  */
  complaint->next = complaints->root;
  complaints->root = complaint;
  return complaint;
}


/* How many complaints about a particular thing should be printed
   before we stop whining about it?  Default is no whining at all,
   since so many systems have ill-constructed symbol files.  */

int stop_whining = 0;

/* See complaints.h.  */

void
complaint_internal (const char *fmt, ...)
{
  va_list args;

  struct complain *complaint = find_complaint (&symfile_complaint_book, NULL,
					       0, fmt);
  enum complaint_series series;

  complaint->counter++;
  if (complaint->counter > stop_whining)
    return;

  va_start (args, fmt);
  series = symfile_complaint_book.series;

  /* Pass 'fmt' instead of 'complaint->fmt' to printf-like callees
     from here on, to avoid "format string is not a string literal"
     warnings.  'fmt' is this function's printf-format parameter, so
     the compiler can assume the passed in argument is a literal
     string somewhere up the call chain.  */
  gdb_assert (complaint->fmt == fmt);

  if (complaint->file != NULL)
    internal_vwarning (complaint->file, complaint->line, fmt, args);
  else if (deprecated_warning_hook)
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

  for (p = symfile_complaint_book.root; p != NULL; p = p->next)
    {
      p->counter = 0;
    }

  if (!less_verbose)
    symfile_complaint_book.series = ISOLATED_MESSAGE;
  else
    symfile_complaint_book.series = SHORT_FIRST_MESSAGE;
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
