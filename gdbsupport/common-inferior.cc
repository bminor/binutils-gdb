/* Functions to deal with the inferior being executed on GDB or
   GDBserver.

   Copyright (C) 2019-2020 Free Software Foundation, Inc.

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

#include "gdbsupport/common-defs.h"
#include "gdbsupport/common-inferior.h"

/* See common-inferior.h.  */

bool startup_with_shell = true;

/* See common-inferior.h.  */

char *
construct_inferior_arguments (int argc, char **argv)
{
  char *result;

  /* ARGC should always be at least 1, but we double check this
     here.  This is also needed to silence -Werror-stringop
     warnings.  */
  gdb_assert (argc > 0);

  if (startup_with_shell)
    {
#ifdef __MINGW32__
      /* This holds all the characters considered special to the
	 Windows shells.  */
      static const char special[] = "\"!&*|[]{}<>?`~^=;, \t\n";
      static const char quote = '"';
#else
      /* This holds all the characters considered special to the
	 typical Unix shells.  We include `^' because the SunOS
	 /bin/sh treats it as a synonym for `|'.  */
      static const char special[] = "\"!#$&*()\\|[]{}<>?'`~^; \t\n";
      static const char quote = '\'';
#endif
      int i;
      int length = 0;
      char *out, *cp;

      /* We over-compute the size.  It shouldn't matter.  */
      for (i = 0; i < argc; ++i)
	length += 3 * strlen (argv[i]) + 1 + 2 * (argv[i][0] == '\0');

      result = (char *) xmalloc (length);
      out = result;

      for (i = 0; i < argc; ++i)
	{
	  if (i > 0)
	    *out++ = ' ';

	  /* Need to handle empty arguments specially.  */
	  if (argv[i][0] == '\0')
	    {
	      *out++ = quote;
	      *out++ = quote;
	    }
	  else
	    {
#ifdef __MINGW32__
	      int quoted = 0;

	      if (strpbrk (argv[i], special))
		{
		  quoted = 1;
		  *out++ = quote;
		}
#endif
	      for (cp = argv[i]; *cp; ++cp)
		{
		  if (*cp == '\n')
		    {
		      /* A newline cannot be quoted with a backslash (it
			 just disappears), only by putting it inside
			 quotes.  */
		      *out++ = quote;
		      *out++ = '\n';
		      *out++ = quote;
		    }
		  else
		    {
#ifdef __MINGW32__
		      if (*cp == quote)
#else
		      if (strchr (special, *cp) != NULL)
#endif
			*out++ = '\\';
		      *out++ = *cp;
		    }
		}
#ifdef __MINGW32__
	      if (quoted)
		*out++ = quote;
#endif
	    }
	}
      *out = '\0';
    }
  else
    {
      /* In this case we can't handle arguments that contain spaces,
	 tabs, or newlines -- see breakup_args().  */
      int i;
      int length = 0;

      for (i = 0; i < argc; ++i)
	{
	  char *cp = strchr (argv[i], ' ');
	  if (cp == NULL)
	    cp = strchr (argv[i], '\t');
	  if (cp == NULL)
	    cp = strchr (argv[i], '\n');
	  if (cp != NULL)
	    error (_("can't handle command-line "
		     "argument containing whitespace"));
	  length += strlen (argv[i]) + 1;
	}

      result = (char *) xmalloc (length);
      result[0] = '\0';
      for (i = 0; i < argc; ++i)
	{
	  if (i > 0)
	    strcat (result, " ");
	  strcat (result, argv[i]);
	}
    }

  return result;
}
