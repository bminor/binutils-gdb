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
printf_filtered(const char *msg, ...)
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
  error ("Usage: psim [ -a -p -c -C -s -i -I -t ] <image> [ <image-args> ... ]\n");
}

int
main(int argc, char **argv)
{
  psim *system;
  const char *name_of_file;
  char *arg_;
  unsigned_word stack_pointer;
  psim_status status;
  int letter;
  int i;
  int print_info = 0;

  /* check for arguments -- note sim_calls.c also contains argument processing
     code for the simulator linked within gdb.  */
  while ((letter = getopt (argc, argv, "acCiIpst")) != EOF)
    {
      switch (letter) {
      case 'a':
	for (i = 0; i < nr_trace; i++)
	  trace[i] = 1;
	break;
      case 'p':
	trace[trace_cpu] = trace[trace_semantics] = 1;
	break;
      case 'c':
	trace[trace_core] = 1;
	break;
      case 'C':
	trace[trace_console_device] = 1;
	break;
      case 's':
	trace[trace_create_stack] = 1;
	break;
      case 'i':
	trace[trace_icu_device] = 1;
	break;
      case 'I':
	print_info = 1;
	break;
      case 't':
	trace[trace_device_tree] = 1;
	break;
      default:
	usage();
      }
    }

  if (optind >= argc)
    usage();
  name_of_file = argv[optind];

  /* create the simulator */
  system = psim_create(name_of_file, ((WITH_SMP > 0) ? WITH_SMP : 1));

  /* fudge the environment so that _=prog-name */
  arg_ = (char*)zalloc(strlen(argv[optind]) + strlen("_=") + 1);
  strcpy(arg_, "_=");
  strcat(arg_, argv[optind]);
  putenv(arg_);

  /* initialize it */
  psim_load(system);
  psim_stack(system, &argv[optind], environ);

  psim_run(system);

  if (print_info)
    psim_print_info (system, 1);

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
