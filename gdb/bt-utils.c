/* Copyright (C) 2021-2025 Free Software Foundation, Inc.

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

#include "bt-utils.h"
#include "command.h"
#include "cli/cli-cmds.h"
#include "ui.h"
#include "cli/cli-decode.h"

/* See bt-utils.h.  */

void
gdb_internal_backtrace_set_cmd (const char *args, int from_tty,
				cmd_list_element *c)
{
  gdb_assert (c->type == set_cmd);
  gdb_assert (c->var.has_value ());
  gdb_assert (c->var->type () == var_boolean);

#ifndef GDB_PRINT_INTERNAL_BACKTRACE
  if (c->var->get<bool> ())
    {
      c->var->set<bool> (false);
      error (_("support for this feature is not compiled into GDB"));
    }
#endif
}

/* See bt-utils.h.  */

void
sig_write (const char *msg)
{
  if (gdb_stderr == nullptr || gdb_stderr->fd () == -1)
    std::ignore = ::write (2, msg, strlen (msg));
  else
    gdb_stderr->write_async_safe (msg, strlen (msg));
}

#ifdef GDB_PRINT_INTERNAL_BACKTRACE
#ifdef GDB_PRINT_INTERNAL_BACKTRACE_USING_LIBBACKTRACE

/* Callback used by libbacktrace if it encounters an error.  */

static void
libbacktrace_error (void *data, const char *errmsg, int errnum)
{
  /* A negative errnum indicates no debug info was available, just
     skip printing a backtrace in this case.  */
  if (errnum < 0)
    return;

  sig_write ("error creating backtrace: ");
  sig_write (errmsg);
  if (errnum > 0)
    {
      char buf[20];
      snprintf (buf, sizeof (buf), ": %d", errnum);
      buf[sizeof (buf) - 1] = '\0';

      sig_write (buf);
    }
  sig_write ("\n");
}

/* Callback used by libbacktrace to print a single stack frame.  */

static int
libbacktrace_print (void *data, uintptr_t pc, const char *filename,
		    int lineno, const char *function)
{
  /* Buffer to print addresses and line numbers into.  An 8-byte address
     with '0x' prefix and a null terminator requires 20 characters.  This
     also feels like it should be enough to represent line numbers in most
     files.  We are also careful to ensure we don't overflow this buffer.  */
  char buf[20];

  snprintf (buf, sizeof (buf), "0x%" PRIxPTR " ", pc);
  buf[sizeof (buf) - 1] = '\0';
  sig_write (buf);
  sig_write (function == nullptr ? "???" : function);
  if (filename != nullptr)
    {
      sig_write ("\n\t");
      sig_write (filename);
      sig_write (":");
      snprintf (buf, sizeof (buf), "%d", lineno);
      buf[sizeof (buf) - 1] = '\0';
      sig_write (buf);
    }
  sig_write ("\n");

  return function != nullptr && strcmp (function, "main") == 0;
}

/* Write a backtrace to GDB's stderr in an async safe manner.  This is a
   backtrace of GDB, not any running inferior, and is to be used when GDB
   crashes or hits some other error condition.  */

static void
gdb_internal_backtrace_1 ()
{
  static struct backtrace_state *state = nullptr;

  if (state == nullptr)
    state = backtrace_create_state (nullptr, 0, libbacktrace_error, nullptr);

  backtrace_full (state, 0, libbacktrace_print, libbacktrace_error, nullptr);
}

#elif defined GDB_PRINT_INTERNAL_BACKTRACE_USING_EXECINFO

/* See the comment on previous version of this function.  */

static void
gdb_internal_backtrace_1 ()
{
  /* Allow up to 25 frames of backtrace.  */
  void *buffer[25];
  int frames = backtrace (buffer, ARRAY_SIZE (buffer));

  int fd = ((gdb_stderr == nullptr || gdb_stderr->fd () == -1)
	    ? 2
	    : gdb_stderr->fd ());
  backtrace_symbols_fd (buffer, frames, fd);
  if (frames == ARRAY_SIZE (buffer))
    sig_write (_("Backtrace might be incomplete.\n"));
}

#else
#error "unexpected internal backtrace policy"
#endif

static const char *str_backtrace = "----- Backtrace -----\n";
static const char *str_backtrace_unavailable = "Backtrace unavailable\n";

#endif /* GDB_PRINT_INTERNAL_BACKTRACE */

/* See bt-utils.h.  */

void
gdb_internal_backtrace_init_str ()
{
#ifdef GDB_PRINT_INTERNAL_BACKTRACE
  str_backtrace = _("----- Backtrace -----\n");
  str_backtrace_unavailable = _("Backtrace unavailable\n");
#endif
}

/* See bt-utils.h.  */

void
gdb_internal_backtrace ()
{
  if (current_ui == nullptr)
    return;

#ifdef GDB_PRINT_INTERNAL_BACKTRACE
  sig_write (str_backtrace);

  gdb_internal_backtrace_1 ();

  sig_write ("---------------------\n");
#endif
}
