/* run front end support for all the simulators.
   Copyright (C) 1992, 1993 1994, 1995, 1996 Free Software Foundation, Inc.

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

/* Steve Chamberlain sac@cygnus.com,
   and others at Cygnus.  */

#include "config.h"
#include "tconfig.h"

#include <signal.h>
#include <stdio.h>
#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

#include "libiberty.h"
#include "bfd.h"
#include "callback.h"
#include "remote-sim.h"

static void usage PARAMS ((void));
extern int optind;
extern char *optarg;

bfd *exec_bfd;

int target_byte_order;

extern host_callback default_callback;

static char *myname;


/* NOTE: sim_size() and sim_trace() are going away */
extern void sim_size PARAMS ((int i));
extern int sim_trace PARAMS ((SIM_DESC sd));

extern int getopt ();


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
  char *name;
  static char *no_args[2];
  char **sim_argv = &no_args[0];
  char **prog_args;
  enum sim_stop reason;
  int sigrc;
  SIM_DESC sd;

  myname = av[0] + strlen (av[0]);
  while (myname > av[0] && myname[-1] != '/')
    --myname;

  /* The first element of sim_open's argv is the program name.  */
  no_args[0] = av[0];

  /* FIXME: This is currently being rewritten to have each simulator
     do all argv processing.  */

#ifdef SIM_H8300 /* FIXME: quick hack */
  while ((i = getopt (ac, av, "a:c:m:p:s:htv")) != EOF) 
#else
  while ((i = getopt (ac, av, "a:c:m:p:s:tv")) != EOF) 
#endif
    switch (i)
      {
      case 'a':
	/* FIXME: Temporary hack.  */
	{
	  int len = strlen (av[0]) + strlen (optarg);
	  char *argbuf = (char *) alloca (len + 2);
	  sprintf (argbuf, "%s %s", av[0], optarg);
	  sim_argv = buildargv (argbuf);
	}
	break;
#ifdef SIM_HAVE_SIMCACHE
      case 'c':
	sim_set_simcache_size (atoi (optarg));
	break;
#endif
      case 'm':
	/* FIXME: Rename to sim_set_mem_size.  */
	sim_size (atoi (optarg));
	break;
#ifdef SIM_HAVE_PROFILE
      case 'p':
	sim_set_profile (atoi (optarg));
	break;
      case 's':
	sim_set_profile_size (atoi (optarg));
	break;
#endif
      case 't':
	trace = 1;
	/* FIXME: need to allow specification of what to trace.  */
	/* sim_set_trace (1); */
	break;
      case 'v':
	/* Things that are printed with -v are the kinds of things that
	   gcc -v prints.  This is not meant to include detailed tracing
	   or debugging information, just summaries.  */
	verbose = 1;
	/* sim_set_verbose (1); */
	break;
	/* FIXME: Quick hack, to be replaced by more general facility.  */
#ifdef SIM_H8300
      case 'h':
	set_h8300h (1);
	break;
#endif
      default:
	usage ();
      }

  ac -= optind;
  av += optind;

  name = *av;
  prog_args = av + 1;

  if (verbose)
    {
      printf ("%s %s\n", myname, name);
    }

  exec_bfd = abfd = bfd_openr (name, 0);
  if (!abfd) 
    {
      fprintf (stderr, "%s: can't open %s: %s\n", 
	       myname, name, bfd_errmsg (bfd_get_error ()));
      exit (1);
    }

  if (!bfd_check_format (abfd, bfd_object))
    {
      fprintf (stderr, "%s: can't load %s: %s\n",
	       myname, name, bfd_errmsg (bfd_get_error ()));
      exit (1);
    }

  /* This must be set before sim_open is called, because gdb assumes that
     the simulator endianness is known immediately after the sim_open call.  */
  target_byte_order = bfd_big_endian (abfd) ? 4321 : 1234;

  sim_set_callbacks (NULL, &default_callback);
  default_callback.init (&default_callback);

  /* Ensure that any run-time initialisation that needs to be
     performed by the simulator can occur. */
  sd = sim_open (sim_argv);

  for (s = abfd->sections; s; s = s->next)
    {
      if (s->flags & SEC_LOAD)
	{
	  unsigned char *buffer = (unsigned char *)malloc ((size_t)(bfd_section_size (abfd, s)));
	  if (buffer != NULL)
	    {
	      bfd_get_section_contents (abfd,
					s,
					buffer,
					0,
					bfd_section_size (abfd, s));
	      sim_write (sd, s->vma, buffer, bfd_section_size (abfd, s));
	      /* FIXME: How come we don't free buffer?  */
	    }
	  else
	    {
	      fprintf (stderr, "%s: failed to allocate section buffer: %s\n", 
		       myname, bfd_errmsg (bfd_get_error ()));
	      exit (1);
	    }
	}
    }

  start_address = bfd_get_start_address (abfd);
  sim_create_inferior (sd, start_address, prog_args, NULL);

  if (trace)
    {
      int done = 0;
      while (!done)
	{
	  done = sim_trace (sd);
	}
    }
  else
    {
      sim_resume (sd, 0, 0);
    }
  if (verbose)
    sim_info (sd, 0);

  sim_stop_reason (sd, &reason, &sigrc);

  sim_close (sd, 0);

  /* If reason is sim_exited, then sigrc holds the exit code which we want
     to return.  If reason is sim_stopped or sim_signalled, then sigrc holds
     the signal that the simulator received; we want to return that to
     indicate failure.  */

#ifdef SIM_H8300 /* FIXME: Ugh.  grep for SLEEP in compile.c  */
  if (sigrc == SIGILL)
    abort ();
  sigrc = 0;
#else
  /* Why did we stop? */
  switch (reason)
    {
    case sim_signalled:
    case sim_stopped:
      if (sigrc != 0)
        fprintf (stderr, "program stopped with signal %d.\n", sigrc);
      break;

    case sim_exited:
      break;
    }
#endif

  return sigrc;
}

static void
usage ()
{
  fprintf (stderr, "Usage: %s [options] program [program args]\n", myname);
  fprintf (stderr, "Options:\n");
  fprintf (stderr, "-a args         Pass `args' to simulator.\n");
#ifdef SIM_HAVE_SIMCACHE
  fprintf (stderr, "-c size         Set simulator cache size to `size'.\n");
#endif
#ifdef SIM_H8300
  fprintf (stderr, "-h              Executable is for h8/300h or h8/300s.\n");
#endif
  fprintf (stderr, "-m size         Set memory size of simulator, in bytes.\n");
#ifdef SIM_HAVE_PROFILE
  fprintf (stderr, "-p freq         Set profiling frequency.\n");
  fprintf (stderr, "-s size         Set profiling size.\n");
#endif
  fprintf (stderr, "-t              Perform instruction tracing.\n");
  fprintf (stderr, "                Note: Very few simulators support tracing.\n");
  fprintf (stderr, "-v              Verbose output.\n");
  fprintf (stderr, "\n");
  fprintf (stderr, "program args    Arguments to pass to simulated program.\n");
  fprintf (stderr, "                Note: Very few simulators support this.\n");
  exit (1);
}
