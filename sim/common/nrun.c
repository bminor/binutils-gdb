/* New version of run front end support for simulators.
   Copyright (C) 1997 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <signal.h>
#include "sim-main.h"

#include "bfd.h"

#ifdef HAVE_ENVIRON
extern char **environ;
#endif

static void usage (void);

extern host_callback default_callback;

static char *myname;

static SIM_DESC sd;

static RETSIGTYPE
cntrl_c (int sig)
{
  if (! sim_stop (sd))
    {
      fprintf (stderr, "Quit!\n");
      exit (1);
    }
}

int
main (int argc, char **argv)
{
  char *name;
  char **prog_argv = NULL;
  struct _bfd *prog_bfd;
  enum sim_stop reason;
  int sigrc;
  RETSIGTYPE (*prev_sigint) ();

  myname = argv[0] + strlen (argv[0]);
  while (myname > argv[0] && myname[-1] != '/')
    --myname;

  /* Create an instance of the simulator.  */
  default_callback.init (&default_callback);
  sd = sim_open (SIM_OPEN_STANDALONE, &default_callback, NULL, argv);
  if (sd == 0)
    exit (1);
  if (STATE_MAGIC (sd) != SIM_MAGIC_NUMBER)
    {
      fprintf (stderr, "Internal error - bad magic number in simulator struct\n");
      abort ();
    }

  /* Was there a program to run?  */
  prog_argv = STATE_PROG_ARGV (sd);
  prog_bfd = STATE_PROG_BFD (sd);
  if (prog_argv == NULL || *prog_argv == NULL)
    usage ();

  name = *prog_argv;

  /* For simulators that don't open prog during sim_open() */
  if (prog_bfd == NULL)
    {
      prog_bfd = bfd_openr (name, 0);
      if (prog_bfd == NULL)
	fprintf (stderr, "%s: can't open \"%s\": %s\n", 
		 myname, name, bfd_errmsg (bfd_get_error ()));
    }

  if (STATE_VERBOSE_P (sd))
    printf ("%s %s\n", myname, name);

  /* Load the program into the simulator.  */
  if (sim_load (sd, name, prog_bfd, 0) == SIM_RC_FAIL)
    exit (1);

  /* Prepare the program for execution.  */
#ifdef HAVE_ENVIRON
  sim_create_inferior (sd, prog_bfd, prog_argv, environ);
#else
  sim_create_inferior (sd, prog_bfd, prog_argv, NULL);
#endif

  /* Run the program.  */
  prev_sigint = signal (SIGINT, cntrl_c);
  sim_resume (sd, 0, 0);
  signal (SIGINT, prev_sigint);

  /* Print any stats the simulator collected.  */
  sim_info (sd, 0);

  /* Find out why the program exited.  */
  sim_stop_reason (sd, &reason, &sigrc);

  /* Shutdown the simulator.  */
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

    default:
      fprintf (stderr, "program in undefined state (%d:%d)\n", reason, sigrc);
      break;

    }
#endif

  return sigrc;
}

static void
usage ()
{
  fprintf (stderr, "Usage: %s [options] program [program args]\n", myname);
  fprintf (stderr, "Run `%s --help' for full list of options.\n", myname);
  exit (1);
}
