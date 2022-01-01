/* Output generating routines for GDB CLI.

   Copyright (C) 1999-2022 Free Software Foundation, Inc.

   Contributed by Cygnus Solutions.
   Written by Fernando Nasser for Cygnus.

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
#include "ui-out.h"
#include "cli-out.h"
#include "completer.h"
#include "readline/readline.h"
#include "cli/cli-style.h"

/* These are the CLI output functions */

/* Mark beginning of a table */

void
cli_ui_out::do_table_begin (int nbrofcols, int nr_rows, const char *tblid)
{
  if (nr_rows == 0)
    m_suppress_output = true;
  else
    /* Only the table suppresses the output and, fortunately, a table
       is not a recursive data structure.  */
    gdb_assert (!m_suppress_output);
}

/* Mark beginning of a table body */

void
cli_ui_out::do_table_body ()
{
  if (m_suppress_output)
    return;

  /* first, close the table header line */
  text ("\n");
}

/* Mark end of a table */

void
cli_ui_out::do_table_end ()
{
  m_suppress_output = false;
}

/* Specify table header */

void
cli_ui_out::do_table_header (int width, ui_align alignment,
			     const std::string &col_name,
			     const std::string &col_hdr)
{
  if (m_suppress_output)
    return;

  do_field_string (0, width, alignment, 0, col_hdr.c_str (),
		   ui_file_style ());
}

/* Mark beginning of a list */

void
cli_ui_out::do_begin (ui_out_type type, const char *id)
{
}

/* Mark end of a list */

void
cli_ui_out::do_end (ui_out_type type)
{
}

/* output an int field */

void
cli_ui_out::do_field_signed (int fldno, int width, ui_align alignment,
			     const char *fldname, LONGEST value)
{
  if (m_suppress_output)
    return;

  do_field_string (fldno, width, alignment, fldname, plongest (value),
		   ui_file_style ());
}

/* output an unsigned field */

void
cli_ui_out::do_field_unsigned (int fldno, int width, ui_align alignment,
			       const char *fldname, ULONGEST value)
{
  if (m_suppress_output)
    return;

  do_field_string (fldno, width, alignment, fldname, pulongest (value),
		   ui_file_style ());
}

/* used to omit a field */

void
cli_ui_out::do_field_skip (int fldno, int width, ui_align alignment,
			   const char *fldname)
{
  if (m_suppress_output)
    return;

  do_field_string (fldno, width, alignment, fldname, "",
		   ui_file_style ());
}

/* other specific cli_field_* end up here so alignment and field
   separators are both handled by cli_field_string */

void
cli_ui_out::do_field_string (int fldno, int width, ui_align align,
			     const char *fldname, const char *string,
			     const ui_file_style &style)
{
  int before = 0;
  int after = 0;

  if (m_suppress_output)
    return;

  if ((align != ui_noalign) && string)
    {
      before = width - strlen (string);
      if (before <= 0)
	before = 0;
      else
	{
	  if (align == ui_right)
	    after = 0;
	  else if (align == ui_left)
	    {
	      after = before;
	      before = 0;
	    }
	  else
	    /* ui_center */
	    {
	      after = before / 2;
	      before -= after;
	    }
	}
    }

  if (before)
    spaces (before);

  if (string)
    {
      if (test_flags (unfiltered_output))
	fputs_styled_unfiltered (string, style, m_streams.back ());
      else
	fputs_styled (string, style, m_streams.back ());
    }

  if (after)
    spaces (after);

  if (align != ui_noalign)
    field_separator ();
}

/* Output field containing ARGS using printf formatting in FORMAT.  */

void
cli_ui_out::do_field_fmt (int fldno, int width, ui_align align,
			  const char *fldname, const ui_file_style &style,
			  const char *format, va_list args)
{
  if (m_suppress_output)
    return;

  std::string str = string_vprintf (format, args);

  do_field_string (fldno, width, align, fldname, str.c_str (), style);
}

void
cli_ui_out::do_spaces (int numspaces)
{
  if (m_suppress_output)
    return;

  if (test_flags (unfiltered_output))
    fprintf_unfiltered (m_streams.back (), "%*s", numspaces, "");
  else
    print_spaces_filtered (numspaces, m_streams.back ());
}

void
cli_ui_out::do_text (const char *string)
{
  if (m_suppress_output)
    return;

  if (test_flags (unfiltered_output))
    fputs_unfiltered (string, m_streams.back ());
  else
    fputs_filtered (string, m_streams.back ());
}

void
cli_ui_out::do_message (const ui_file_style &style,
			const char *format, va_list args)
{
  if (m_suppress_output)
    return;

  /* Use the "no_gdbfmt" variant here to avoid recursion.
     vfprintf_styled calls into cli_ui_out::message to handle the
     gdb-specific printf formats.  */
  vfprintf_styled_no_gdbfmt (m_streams.back (), style,
			     !test_flags (unfiltered_output), format, args);
}

void
cli_ui_out::do_wrap_hint (const char *identstring)
{
  if (m_suppress_output)
    return;

  wrap_here (identstring);
}

void
cli_ui_out::do_flush ()
{
  gdb_flush (m_streams.back ());
}

/* OUTSTREAM as non-NULL will push OUTSTREAM on the stack of output streams
   and make it therefore active.  OUTSTREAM as NULL will pop the last pushed
   output stream; it is an internal error if it does not exist.  */

void
cli_ui_out::do_redirect (ui_file *outstream)
{
  if (outstream != NULL)
    m_streams.push_back (outstream);
  else
    m_streams.pop_back ();
}

/* The cli_ui_out::do_progress_* functions result in the following:
   - printed for tty, SHOULD_PRINT == true:
     <NAME
      [#####                      ]\r>
   - printed for tty, SHOULD_PRINT == false:
     <>
   - printed for not-a-tty:
     <NAME...
     >
*/

void
cli_ui_out::do_progress_start (const std::string &name, bool should_print)
{
  struct ui_file *stream = m_streams.back ();
  cli_progress_info meter;

  meter.last_value = 0;
  meter.name = name;
  if (!stream->isatty ())
    {
      fprintf_unfiltered (stream, "%s...", meter.name.c_str ());
      gdb_flush (stream);
      meter.printing = WORKING;
    }
  else
    {
      /* Don't actually emit anything until the first call notifies us
	 of progress.  This makes it so a second progress message can
	 be started before the first one has been notified, without
	 messy output.  */
      meter.printing = should_print ? START : NO_PRINT;
    }

  m_meters.push_back (std::move (meter));
}

void
cli_ui_out::do_progress_notify (double howmuch)
{
  struct ui_file *stream = m_streams.back ();
  cli_progress_info &meter (m_meters.back ());

  if (meter.printing == NO_PRINT)
    return;

  if (meter.printing == START)
    {
      fprintf_unfiltered (stream, "%s\n", meter.name.c_str ());
      gdb_flush (stream);
      meter.printing = WORKING;
    }

  if (meter.printing == WORKING && howmuch >= 1.0)
    return;

  if (!stream->isatty ())
    return;

  int chars_per_line = get_chars_per_line ();
  if (chars_per_line > 0)
    {
      int i, max;
      int width = chars_per_line - 3;

      max = width * howmuch;
      fprintf_unfiltered (stream, "\r[");
      for (i = 0; i < width; ++i)
	fprintf_unfiltered (stream, i < max ? "#" : " ");
      fprintf_unfiltered (stream, "]");
      gdb_flush (stream);
      meter.printing = PROGRESS;
    }
}

void
cli_ui_out::do_progress_end ()
{
  struct ui_file *stream = m_streams.back ();
  cli_progress_info &meter = m_meters.back ();

  if (!stream->isatty ())
    {
      fprintf_unfiltered (stream, "\n");
      gdb_flush (stream);
    }
  else if (meter.printing == PROGRESS)
    {
      int i;
      int width = get_chars_per_line () - 3;

      fprintf_unfiltered (stream, "\r");
      for (i = 0; i < width + 2; ++i)
	fprintf_unfiltered (stream, " ");
      fprintf_unfiltered (stream, "\r");
      gdb_flush (stream);
    }

  m_meters.pop_back ();
}

/* local functions */

void
cli_ui_out::field_separator ()
{
  if (test_flags (unfiltered_output))
    fputc_unfiltered (' ', m_streams.back ());
  else
    fputc_filtered (' ', m_streams.back ());
}

/* Constructor for cli_ui_out.  */

cli_ui_out::cli_ui_out (ui_file *stream, ui_out_flags flags)
: ui_out (flags),
  m_suppress_output (false)
{
  gdb_assert (stream != NULL);

  m_streams.push_back (stream);
}

cli_ui_out::~cli_ui_out ()
{
}

/* Initialize private members at startup.  */

cli_ui_out *
cli_out_new (struct ui_file *stream)
{
  return new cli_ui_out (stream, ui_source_list);
}

ui_file *
cli_ui_out::set_stream (struct ui_file *stream)
{
  ui_file *old;

  old = m_streams.back ();
  m_streams.back () = stream;

  return old;
}

bool
cli_ui_out::can_emit_style_escape () const
{
  return m_streams.back ()->can_emit_style_escape ();
}

/* CLI interface to display tab-completion matches.  */

/* CLI version of displayer.crlf.  */

static void
cli_mld_crlf (const struct match_list_displayer *displayer)
{
  rl_crlf ();
}

/* CLI version of displayer.putch.  */

static void
cli_mld_putch (const struct match_list_displayer *displayer, int ch)
{
  putc (ch, rl_outstream);
}

/* CLI version of displayer.puts.  */

static void
cli_mld_puts (const struct match_list_displayer *displayer, const char *s)
{
  fputs (s, rl_outstream);
}

/* CLI version of displayer.flush.  */

static void
cli_mld_flush (const struct match_list_displayer *displayer)
{
  fflush (rl_outstream);
}

EXTERN_C void _rl_erase_entire_line (void);

/* CLI version of displayer.erase_entire_line.  */

static void
cli_mld_erase_entire_line (const struct match_list_displayer *displayer)
{
  _rl_erase_entire_line ();
}

/* CLI version of displayer.beep.  */

static void
cli_mld_beep (const struct match_list_displayer *displayer)
{
  rl_ding ();
}

/* CLI version of displayer.read_key.  */

static int
cli_mld_read_key (const struct match_list_displayer *displayer)
{
  return rl_read_key ();
}

/* CLI version of rl_completion_display_matches_hook.
   See gdb_display_match_list for a description of the arguments.  */

void
cli_display_match_list (char **matches, int len, int max)
{
  struct match_list_displayer displayer;

  rl_get_screen_size (&displayer.height, &displayer.width);
  displayer.crlf = cli_mld_crlf;
  displayer.putch = cli_mld_putch;
  displayer.puts = cli_mld_puts;
  displayer.flush = cli_mld_flush;
  displayer.erase_entire_line = cli_mld_erase_entire_line;
  displayer.beep = cli_mld_beep;
  displayer.read_key = cli_mld_read_key;

  gdb_display_match_list (matches, len, max, &displayer);
  rl_forced_update_display ();
}
