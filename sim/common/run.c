/* run front end support for all the simulators.
   Copyright (C) 1992, 1993 1994, 1995 Free Software Foundation, Inc.

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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


/* Steve Chamberlain
   sac@cygnus.com */

#include <signal.h>
#include <stdio.h>
#include <varargs.h>
#include "bfd.h"
#include "remote-sim.h"
#include "callback.h"
#ifndef SIGQUIT
#define SIGQUIT SIGTERM
#endif

void usage();
extern int optind;
extern char *optarg;

int target_byte_order;

extern host_callback default_callback;
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
  enum sim_stop reason;
  int sigrc;

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
  sim_set_callbacks (&default_callback);
  default_callback.init (&default_callback);
  if (abfd)
    {
      if (bfd_check_format (abfd, bfd_object))
	{

	  for (s = abfd->sections; s; s = s->next)
	    {
	      unsigned char *buffer = (unsigned char *)malloc (bfd_section_size (abfd, s));
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

	  sim_stop_reason (&reason, &sigrc);

	  if (sigrc == SIGQUIT)
	    {
	      return sim_get_quit_code();
	    }
	  else
	    return sigrc;
	}
    }

  return 1;
}

void
usage()
{
  fprintf (stderr, "usage: run [-tv][-m size] program\n");
  exit (1);
}


