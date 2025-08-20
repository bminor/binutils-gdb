/* Parse a printf-style format string.

   Copyright (C) 1986-2025 Free Software Foundation, Inc.

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

#include "format.h"

format_pieces::format_pieces (const char **arg, bool gdb_extensions,
			      bool value_extension)
{
  const char *s = *arg;
  const char *string;

  /* Buffer to hold the escaped-processed version of the string.  */
  std::string de_escaped;

  if (gdb_extensions)
    {
      string = *arg;
      *arg += strlen (*arg);
    }
  else
    {
      /* Parse the format-control string and copy it into the string STRING,
	 processing some kinds of escape sequence.  */
      while (*s != '"' && *s != '\0')
	{
	  int c = *s++;
	  switch (c)
	    {
	    case '\0':
	      continue;

	    case '\\':
	      switch (c = *s++)
		{
		case '\\':
		  de_escaped += '\\';
		  break;
		case 'a':
		  de_escaped += '\a';
		  break;
		case 'b':
		  de_escaped += '\b';
		  break;
		case 'e':
		  de_escaped += '\e';
		  break;
		case 'f':
		  de_escaped += '\f';
		  break;
		case 'n':
		  de_escaped += '\n';
		  break;
		case 'r':
		  de_escaped += '\r';
		  break;
		case 't':
		  de_escaped += '\t';
		  break;
		case 'v':
		  de_escaped += '\v';
		  break;
		case '"':
		  de_escaped += '"';
		  break;
		default:
		  /* ??? TODO: handle other escape sequences.  */
		  error (_("Unrecognized escape character \\%c in format string."),
			 c);
		}
	      break;

	    default:
	      de_escaped += c;
	    }
	}

      string = de_escaped.c_str ();

      /* Whether the format string ended with double-quote or zero, we're
	 done with it; it's up to callers to complain about syntax.  */
      *arg = s;
    }

  /* Now scan the string for %-specs and see what kinds of args they want.
     argclass classifies the %-specs so we can give printf-type functions
     something of the right size.  */
  const char *f = string;
  const char *prev_start = string;

  while (*f)
    if (*f++ == '%')
      {
	int seen_hash = 0, seen_zero = 0, lcount = 0, seen_prec = 0;
	int seen_space = 0, seen_plus = 0;
	int seen_big_l = 0, seen_h = 0, seen_big_h = 0;
	int seen_big_d = 0, seen_double_big_d = 0;
	int seen_size_t = 0;
	int bad = 0;
	int n_int_args = 0;
	bool seen_i64 = false;

	/* Skip over "%%", it will become part of a literal piece.  */
	if (*f == '%')
	  {
	    f++;
	    continue;
	  }

	std::string::size_type sub_start = m_storage.size ();

	m_storage.append (prev_start, f - 1 - prev_start);
	m_storage += '\0';

	if (m_storage[sub_start] != '\0')
	  m_pieces.emplace_back (sub_start, literal_piece, 0);

	const char *percent_loc = f - 1;

	/* Check the validity of the format specifier, and work
	   out what argument it expects.  We only accept C89
	   format strings, with the exception of long long (which
	   we autoconf for).  */

	/* The first part of a format specifier is a set of flag
	   characters.  */
	while (*f != '\0' && strchr ("0-+ #", *f))
	  {
	    if (*f == '#')
	      seen_hash = 1;
	    else if (*f == '0')
	      seen_zero = 1;
	    else if (*f == ' ')
	      seen_space = 1;
	    else if (*f == '+')
	      seen_plus = 1;
	    f++;
	  }

	/* The next part of a format specifier is a width.  */
	if (gdb_extensions && *f == '*')
	  {
	    ++f;
	    ++n_int_args;
	  }
	else
	  {
	    while (*f != '\0' && strchr ("0123456789", *f))
	      f++;
	  }

	/* The next part of a format specifier is a precision.  */
	if (*f == '.')
	  {
	    seen_prec = 1;
	    f++;
	    if (gdb_extensions && *f == '*')
	      {
		++f;
		++n_int_args;
	      }
	    else
	      {
		while (*f != '\0' && strchr ("0123456789", *f))
		  f++;
	      }
	  }

	/* The next part of a format specifier is a length modifier.  */
	switch (*f)
	  {
	  case 'h':
	    seen_h = 1;
	    f++;
	    break;
	  case 'l':
	    f++;
	    lcount++;
	    if (*f == 'l')
	      {
		f++;
		lcount++;
	      }
	    break;
	  case 'L':
	    seen_big_l = 1;
	    f++;
	    break;
	  case 'H':
	    /* Decimal32 modifier.  */
	    seen_big_h = 1;
	    f++;
	    break;
	  case 'D':
	    /* Decimal64 and Decimal128 modifiers.  */
	    f++;

	    /* Check for a Decimal128.  */
	    if (*f == 'D')
	      {
		f++;
		seen_double_big_d = 1;
	      }
	    else
	      seen_big_d = 1;
	    break;
	  case 'z':
	    /* For size_t or ssize_t.  */
	    seen_size_t = 1;
	    f++;
	    break;
	  case 'I':
	    /* Support the Windows '%I64' extension, because an
	       earlier call to format_pieces might have converted %lld
	       to %I64d.  */
	    if (f[1] == '6' && f[2] == '4')
	      {
		f += 3;
		lcount = 2;
		seen_i64 = true;
	      }
	    break;
	}

	argclass this_argclass;

	switch (*f)
	  {
	  case 'u':
	    if (seen_hash)
	      bad = 1;
	    [[fallthrough]];

	  case 'o':
	  case 'x':
	  case 'X':
	    if (seen_space || seen_plus)
	      bad = 1;
	  [[fallthrough]];

	  case 'd':
	  case 'i':
	    if (seen_size_t)
	      this_argclass = size_t_arg;
	    else if (lcount == 0)
	      this_argclass = int_arg;
	    else if (lcount == 1)
	      this_argclass = long_arg;
	    else
	      this_argclass = long_long_arg;

	    if (seen_big_l)
	      bad = 1;
	    break;

	  case 'c':
	    this_argclass = lcount == 0 ? int_arg : wide_char_arg;
	    if (lcount > 1 || seen_h || seen_big_l)
	      bad = 1;
	    if (seen_prec || seen_zero || seen_space || seen_plus)
	      bad = 1;
	    break;

	  case 'p':
	    this_argclass = ptr_arg;
	    if (lcount || seen_h || seen_big_l)
	      bad = 1;
	    if (seen_prec)
	      bad = 1;
	    if (seen_hash || seen_zero || seen_space || seen_plus)
	      bad = 1;

	    if (gdb_extensions)
	      {
		switch (f[1])
		  {
		  case 's':
		  case 'F':
		  case '[':
		  case ']':
		    f++;
		    break;
		  }
	      }

	    break;

	  case 's':
	    this_argclass = lcount == 0 ? string_arg : wide_string_arg;
	    if (lcount > 1 || seen_h || seen_big_l)
	      bad = 1;
	    if (seen_zero || seen_space || seen_plus)
	      bad = 1;
	    break;

	  case 'e':
	  case 'f':
	  case 'g':
	  case 'E':
	  case 'G':
	    if (seen_double_big_d)
	      this_argclass = dec128float_arg;
	    else if (seen_big_d)
	      this_argclass = dec64float_arg;
	    else if (seen_big_h)
	      this_argclass = dec32float_arg;
	    else if (seen_big_l)
	      this_argclass = long_double_arg;
	    else
	      this_argclass = double_arg;

	    if (lcount || seen_h)
	      bad = 1;
	    break;

	  case 'V':
	    if (!value_extension)
	      error (_("Unrecognized format specifier '%c' in printf"), *f);

	    if (lcount > 1 || seen_h || seen_big_h || seen_big_h
		|| seen_big_d || seen_double_big_d || seen_size_t
		|| seen_prec || seen_zero || seen_space || seen_plus)
	      bad = 1;

	    this_argclass = value_arg;

	    if (f[1] == '[')
	      {
		/* Move F forward to the next ']' character if such a
		   character exists, otherwise leave F unchanged.  */
		const char *tmp = strchr (f, ']');
		if (tmp != nullptr)
		  f = tmp;
	      }
	    break;

	  case '*':
	    error (_("`*' not supported for precision or width in printf"));

	  case 'n':
	    error (_("Format specifier `n' not supported in printf"));

	  case '\0':
	    error (_("Incomplete format specifier at end of format string"));

	  default:
	    error (_("Unrecognized format specifier '%c' in printf"), *f);
	  }

	if (bad)
	  error (_("Inappropriate modifiers to "
		   "format specifier '%c' in printf"),
		 *f);

	f++;

	sub_start = m_storage.size ();

	if (lcount > 1 && !seen_i64 && USE_PRINTF_I64)
	  {
	    /* Windows' printf does support long long, but not the usual way.
	       Convert %lld to %I64d.  */
	    int length_before_ll = f - percent_loc - 1 - lcount;

	    m_storage.append (percent_loc, length_before_ll);
	    m_storage += "I64";
	    m_storage += percent_loc[length_before_ll + lcount];
	  }
	else if (this_argclass == wide_string_arg
		 || this_argclass == wide_char_arg)
	  {
	    /* Convert %ls or %lc to %s.  */
	    int length_before_ls = f - percent_loc - 2;

	    m_storage.append (percent_loc, length_before_ls);
	    m_storage += "s";
	  }
	else
	  m_storage.append (percent_loc, f - percent_loc);

	m_storage += '\0';
	prev_start = f;

	m_pieces.emplace_back (sub_start, this_argclass, n_int_args);
      }

  /* Record the remainder of the string.  */

  if (f > prev_start)
    {
      std::string::size_type sub_start = m_storage.size ();
      m_storage.append (prev_start, f - prev_start);
      /* No need for a final '\0', std::string already has one.  */

      m_pieces.emplace_back (sub_start, literal_piece, 0);
    }
}
