/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "psim.h"

extern char **environ;
extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;

void
printf_filtered(char *msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  vprintf(msg, ap);
}

void
error (char *msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  vprintf(msg, ap);
  exit (1);
}

void *
zalloc(long size)
{
  void *memory = malloc(size);
  if (memory == NULL)
    error("zmalloc failed\n");
  bzero(memory, size);
  return memory;
}

void
zfree(void *chunk)
{
  free(chunk);
}

static void
usage(void)
{
  error ("Usage: psim [ -p -c -s -i -t ] <image> [ <image-args> ... ]\n");
}

int
main(int argc, char **argv)
{
  psim *system;
  char **argp;
  const char *name_of_file;
  char *arg_;
  unsigned_word stack_pointer;
  psim_status status;
  int letter;

  /* check for arguments - FIXME use getopt */
  while ((letter = getopt (argc, argv, "cipst")) != EOF)
    {
      switch (argp[0][1]) {
      case 'p':
	trace[trace_cpu] = trace[trace_semantics] = 1;
	break;
      case 'c':
	trace[trace_core] = 1;
	break;
      case 's':
	trace[trace_create_stack] = 1;
	break;
      case 'i':
	trace[trace_icu_device] = 1;
	break;
      case 't':
	trace[trace_device_tree] = 1;
	break;
      default:
	usage();
      }
    }

  if (argp >= argv+argc)
    usage();
  name_of_file = *argp;

  /* create the simulator */
  system = psim_create(name_of_file, ((WITH_SMP > 0) ? WITH_SMP : 1));

  /* fudge the environment so that _=prog-name */
  arg_ = (char*)zalloc(strlen(*argp) + strlen("_=") + 1);
  strcpy(arg_, "_=");
  strcat(arg_, *argp);
  putenv(arg_);

  /* initialize it */
  psim_load(system);
  psim_stack(system, argp, environ);

  psim_run(system);

  /* why did we stop */
  status = psim_get_status(system);
  switch (status.reason) {
  case was_continuing:
    error("psim: continuing while stoped!\n");
    return 0;
  case was_trap:
    error("psim: no trap insn\n");
    return 0;
  case was_exited:
    return status.signal;
  case was_signalled:
    return status.signal;
  default:
    error("unknown halt condition\n");
    return 0;
  }
}
