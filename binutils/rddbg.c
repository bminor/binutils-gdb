/* rddbg.c -- Read debugging information into a generic form.
   Copyright (C) 1995 Free Software Foundation, Inc.
   Written by Ian Lance Taylor <ian@cygnus.com>.

   This file is part of GNU Binutils.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

/* This file reads debugging information into a generic form.  This
   file knows how to dig the debugging information out of an object
   file.  */

#include "bfd.h"
#include "bucomm.h"
#include "libiberty.h"
#include "debug.h"
#include "budbg.h"

static boolean read_section_stabs_debugging_info
  PARAMS ((bfd *, PTR, boolean *));

/* Read debugging information from a BFD.  Returns a generic debugging
   pointer.  */

PTR
read_debugging_info (abfd)
     bfd *abfd;
{
  PTR dhandle;
  boolean found;

  dhandle = debug_init ();
  if (dhandle == NULL)
    return NULL;

  /* All we know about right now is stabs in sections.  */

  if (! read_section_stabs_debugging_info (abfd, dhandle, &found))
    return NULL;

  if (! found)
    {
      fprintf (stderr, "%s: no recognized debugging information\n",
	       bfd_get_filename (abfd));
      return NULL;
    }

  return dhandle;
}

/* Read stabs in sections debugging information from a BFD.  */

static boolean
read_section_stabs_debugging_info (abfd, dhandle, pfound)
     bfd *abfd;
     PTR dhandle;
     boolean *pfound;
{
  static struct
    {
      const char *secname;
      const char *strsecname;
    } names[] = { { ".stab", ".stabstr" } };
  unsigned int i;
  PTR shandle;

  *pfound = false;
  shandle = NULL;

  for (i = 0; i < sizeof names / sizeof names[0]; i++)
    {
      asection *sec, *strsec;

      sec = bfd_get_section_by_name (abfd, names[i].secname);
      strsec = bfd_get_section_by_name (abfd, names[i].strsecname);
      if (sec != NULL && strsec != NULL)
	{
	  bfd_size_type stabsize, strsize;
	  bfd_byte *stabs, *strings;
	  bfd_byte *stab;
	  bfd_size_type stroff, next_stroff;

	  stabsize = bfd_section_size (abfd, sec);
	  stabs = (bfd_byte *) xmalloc (stabsize);
	  if (! bfd_get_section_contents (abfd, sec, stabs, 0, stabsize))
	    {
	      fprintf (stderr, "%s: %s: %s\n",
		       bfd_get_filename (abfd), names[i].secname,
		       bfd_errmsg (bfd_get_error ()));
	      return false;
	    }

	  strsize = bfd_section_size (abfd, strsec);
	  strings = (bfd_byte *) xmalloc (strsize);
	  if (! bfd_get_section_contents (abfd, strsec, strings, 0, strsize))
	    {
	      fprintf (stderr, "%s: %s: %s\n",
		       bfd_get_filename (abfd), names[i].strsecname,
		       bfd_errmsg (bfd_get_error ()));
	      return false;
	    }

	  if (shandle == NULL)
	    {
	      shandle = start_stab (dhandle);
	      if (shandle == NULL)
		return false;
	    }

	  *pfound = true;

	  stroff = 0;
	  next_stroff = 0;
	  for (stab = stabs; stab < stabs + stabsize; stab += 12)
	    {
	      bfd_size_type strx;
	      int type;
	      int other;
	      int desc;
	      bfd_vma value;

	      /* This code presumes 32 bit values.  */

	      strx = bfd_get_32 (abfd, stab);
	      type = bfd_get_8 (abfd, stab + 4);
	      other = bfd_get_8 (abfd, stab + 5);
	      desc = bfd_get_16 (abfd, stab + 6);
	      value = bfd_get_32 (abfd, stab + 8);

	      if (type == 0)
		{
		  /* Special type 0 stabs indicate the offset to the
                     next string table.  */
		  stroff = next_stroff;
		  next_stroff += value;
		}
	      else
		{
		  char *f, *s;

		  f = NULL;
		  s = (char *) strings + stroff + strx;
		  while (s[strlen (s) - 1] == '\\'
			 && stab + 12 < stabs + stabsize)
		    {
		      stab += 12;
		      s[strlen (s) - 1] = '\0';
		      s = concat (s,
				  ((char *) strings
				   + stroff
				   + bfd_get_32 (abfd, stab)),
				  (const char *) NULL);
		      if (f != NULL)
			free (f);
		      f = s;
		    }

		  if (! parse_stab (dhandle, shandle, type, desc, value, s))
		    return false;

		  /* Don't free f, since I think the stabs code
                     expects strings to hang around.  This should be
                     straightened out.  FIXME.  */
		}
	    }

	  free (stabs);

	  /* Don't free strings, since I think the stabs code expects
             the strings to hang around.  This should be straightened
             out.  FIXME.  */
	}
    }

  if (shandle != NULL)
    {
      if (! finish_stab (dhandle, shandle))
	return false;
    }

  return true;
}
