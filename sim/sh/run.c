/* run front end support for SH
   Copyright (C) 1987, 1992 Free Software Foundation, Inc.

This file is part of SH SIM

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* Steve Chamberlain
   sac@cygnus.com */

#include <stdio.h>
#include <varargs.h>
#include "bfd.h"
#include "sysdep.h"
#include "remote-sim.h"

void usage();
extern int optind;
extern char *optarg;

int target_byte_order;

int
main (ac, av)
     int ac;
     char **av;
{
  bfd *abfd;
  bfd_vma start_address;
  asection *s;
  int i;
  int verbose = 0;
  int trace = 0;
  char *name = "";

  while ((i = getopt (ac, av, "m:p:s:tv")) != EOF) 
    switch (i)
      {
      case 'm':
	sim_size (atoi (optarg));
	break;
      case 'p':
	sim_set_profile (atoi (optarg));
	break;
      case 's':
	sim_set_profile_size (atoi (optarg));
	break;
      case 't':
	trace = 1;
	break;
      case 'v':
	verbose = 1;
	break;
      default:
	usage();
      }
  ac -= optind;
  av += optind;

  if (ac != 1)
    usage();

  name = *av;

  if (verbose)
    {
      printf ("run %s\n", name);
    }
  abfd = bfd_openr (name, 0);
  if (abfd)
    {
      if (bfd_check_format (abfd, bfd_object))
	{

	  for (s = abfd->sections; s; s = s->next)
	    {
	      unsigned char *buffer = malloc (bfd_section_size (abfd, s));
	      bfd_get_section_contents (abfd,
					s,
					buffer,
					0,
					bfd_section_size (abfd, s));
	      sim_write (s->vma, buffer, bfd_section_size (abfd, s));
	    }

	  start_address = bfd_get_start_address (abfd);
	  sim_create_inferior (start_address, NULL, NULL);

	  target_byte_order = abfd->xvec->byteorder_big_p ? 4321 : 1234;

	  if (trace)
	    {
	      int done = 0;
	      while (!done)
		{
		  done = sim_trace ();
		}
	    }
	  else
	    {
	      sim_resume (0, 0);
	    }
	  if (verbose)
	    sim_info (0);

	  /* Assume we left through the exit system call,
	     in which case r5 has the exit code */
	  {
	    unsigned char b[4];
	    sim_fetch_register (5, b);
	    return b[3];
	  }

	}
    }

  return 1;
}

void
usage()
{
  fprintf (stderr, "usage: run [-tv] program\n");
  exit (1);
}


/* Callbacks used by the simulator proper.  */

void
printf_filtered (va_alist)
     va_dcl
{
  va_list args;
  char *format;

  va_start (args);
  format = va_arg (args, char *);

  vfprintf (stdout, format, args);
  va_end (args);
}
