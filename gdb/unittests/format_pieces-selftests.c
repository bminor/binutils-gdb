/* Self tests for format_pieces for GDB, the GNU debugger.

   Copyright (C) 2018-2025 Free Software Foundation, Inc.

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

#include "gdbsupport/format.h"
#include "gdbsupport/selftest.h"

#if USE_PRINTF_I64
#define LL "I64"
#else
#define LL "ll"
#endif

namespace selftests {
namespace format_pieces {

/* Like format_piece, but with the expected string hardcoded instead of an
   index.  */

struct expected_format_piece
{
  std::string_view str;
  enum argclass argclass;
  int n_int_args;
};

/* Verify that parsing STR gives pieces equal to EXPECTED_PIECES.  */

static void
check (const char *str,
       const std::vector<expected_format_piece> &expected_pieces,
       bool gdb_format = false)
{
  ::format_pieces pieces (&str, gdb_format);

  SELF_CHECK ((pieces.end () - pieces.begin ()) == expected_pieces.size ());

  for (auto it = pieces.begin (); it != pieces.end (); ++it)
    {
      auto &expected = expected_pieces[it - pieces.begin ()];

      SELF_CHECK (expected.str == pieces.piece_str (*it));
      SELF_CHECK (expected.argclass == it->argclass);
      SELF_CHECK (expected.n_int_args == it->n_int_args);
    }
}

static void
test_escape_sequences ()
{
  check ("This is an escape sequence: \\e",
    {
      {"This is an escape sequence: \e", literal_piece, 0},
    });
}

static void
test_format_specifier ()
{
  /* The format string here ends with a % sequence, to ensure we don't
     see a trailing empty literal piece.  */
  check ("Hello\\t %d%llx%%d%d", /* ARI: %ll */
    {
      {"Hello\t ", literal_piece, 0},
      {"%d", int_arg, 0},
      {"%" LL "x", long_long_arg, 0},
      {"%%d", literal_piece, 0},
      {"%d", int_arg, 0},
    });
}

static void
test_gdb_formats ()
{
  check ("Hello\\t \"%p[%pF%ps%*.*d%p]\"",
    {
      {"Hello\\t \"", literal_piece, 0},
      {"%p[", ptr_arg, 0},
      {"%pF", ptr_arg, 0},
      {"%ps", ptr_arg, 0},
      {"%*.*d", int_arg, 2},
      {"%p]", ptr_arg, 0},
      {"\"", literal_piece, 0},
    }, true);
}

/* Test the different size modifiers that can be applied to an integer
   argument.  Test with different integer format specifiers too.  */

static void
test_format_int_sizes ()
{
  check ("Hello\\t %hu%lu%llu%zu", /* ARI: %ll */
    {
      {"Hello\t ", literal_piece, 0},
      {"%hu", int_arg, 0},
      {"%lu", long_arg, 0},
      {"%" LL "u", long_long_arg, 0},
      {"%zu", size_t_arg, 0},
    });

  check ("Hello\\t %hx%lx%llx%zx", /* ARI: %ll */
    {
      {"Hello\t ", literal_piece, 0},
      {"%hx", int_arg, 0},
      {"%lx", long_arg, 0},
      {"%" LL "x", long_long_arg, 0},
      {"%zx", size_t_arg, 0},
    });

  check ("Hello\\t %ho%lo%llo%zo", /* ARI: %ll */
    {
      {"Hello\t ", literal_piece, 0},
      {"%ho", int_arg, 0},
      {"%lo", long_arg, 0},
      {"%" LL "o", long_long_arg, 0},
      {"%zo", size_t_arg, 0},
    });

  check ("Hello\\t %hd%ld%lld%zd", /* ARI: %ll */
    {
      {"Hello\t ", literal_piece, 0},
      {"%hd", int_arg, 0},
      {"%ld", long_arg, 0},
      {"%" LL "d", long_long_arg, 0},
      {"%zd", size_t_arg, 0},
    });
}

static void
test_windows_formats ()
{
  check ("rc%I64d",
    {
     {"rc", literal_piece, 0},
     {"%I64d", long_long_arg, 0},
    });
}

static void
run_tests ()
{
  test_escape_sequences ();
  test_format_specifier ();
  test_gdb_formats ();
  test_format_int_sizes ();
  test_windows_formats ();
}

} /* namespace format_pieces */
} /* namespace selftests */

INIT_GDB_FILE (format_pieces_selftests)
{
  selftests::register_test ("format_pieces",
			    selftests::format_pieces::run_tests);
}
