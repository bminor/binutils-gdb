/* front end to the simulator.

   Written by Steve Chamberlain of Cygnus Support.
   sac@cygnus.com

   This file is part of H8/300 sim


		THIS SOFTWARE IS NOT COPYRIGHTED

   Cygnus offers the following for use in the public domain.  Cygnus
   makes no warranty with regard to the software or it's performance
   and the user accepts the software "AS IS" with all faults.

   CYGNUS DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD TO
   THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*/

#include "config.h"

#include <varargs.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include "getopt.h"
#include "bfd.h"
#include "remote-sim.h"

void usage();
extern int optind;
extern char *optarg;

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

  while ((i = getopt (ac, av, "c:htv")) != EOF)
    switch (i) 
      {
      case 'c':
	sim_csize (atoi (optarg));
	break;
      case 'h':
	set_h8300h (1);
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
    printf ("run %s\n", name);

  abfd = bfd_openr (name, "coff-h8300");
  if (abfd) 
    {
      if (bfd_check_format(abfd, bfd_object)) 
	{
	  if (abfd->arch_info->mach == bfd_mach_h8300h)
	    set_h8300h (1);

	  for (s = abfd->sections; s; s=s->next) 
	    {
	      char *buffer = malloc(bfd_section_size(abfd,s));
	      bfd_get_section_contents(abfd, s, buffer, 0, bfd_section_size(abfd,s));
	      sim_write(s->vma, buffer, bfd_section_size(abfd,s));
	    }

	  start_address = bfd_get_start_address(abfd);
	  sim_create_inferior (start_address, NULL, NULL);
	  sim_resume(0,0);
	  if (verbose)
	    sim_info (verbose - 1);
	  return 0;
	}
    }

  return 1;
}

void
printf_filtered (va_alist)
     va_dcl
{
  char *msg;
  va_list args;

  va_start (args);
  msg = va_arg (args, char *);
  vfprintf (stdout, msg, args);
  va_end (args);
}

void
usage()
{
  fprintf (stderr, "usage: run [-tv] program\n");
  exit (1);
}
