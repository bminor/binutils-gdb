/* Miscellaneous simulator utilities.
   Copyright (C) 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of GDB, the GNU debugger.

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

#include "sim-main.h"
#include "sim-assert.h"
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h> /* needed by sys/resource.h */
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#include "libiberty.h"
#include "bfd.h"

/* Global pointer to all state data.
   Set by sim_resume.  */
struct sim_state *current_state;

/* Allocate zero filled memory with xmalloc.  */

void *
zalloc (unsigned long size)
{
  void *memory = (void *) xmalloc (size);
  memset (memory, 0, size);
  return memory;
}

void
zfree (void *data)
{
  free (data);
}

/* Allocate a sim_state struct.  */

SIM_DESC
sim_state_alloc (void)
{
  SIM_DESC sd = zalloc (sizeof (struct sim_state));
  sd->base.magic = SIM_MAGIC_NUMBER;
  return sd;
}

/* Free a sim_state struct.  */

void
sim_state_free (SIM_DESC sd)
{
  ASSERT (sd->base.magic == SIM_MAGIC_NUMBER);
  zfree (sd);
}

/* Turn VALUE into a string with commas.  */

char *
sim_add_commas (char *buf, int sizeof_buf, unsigned long value)
{
  int comma = 3;
  char *endbuf = buf + sizeof_buf - 1;

  *--endbuf = '\0';
  do {
    if (comma-- == 0)
      {
	*--endbuf = ',';
	comma = 2;
      }

    *--endbuf = (value % 10) + '0';
  } while ((value /= 10) != 0);

  return endbuf;
}

/* Make a copy of ARGV.
   This can also be used to copy the environment vector.
   The result is a pointer to the malloc'd copy or NULL if insufficient
   memory available.  */

char **
sim_copy_argv (argv)
     char **argv;
{
  int i,argc,len;
  char **copy,*p;

  argc = len = 0;
  if (argv)
    {
      for ( ; argv[argc]; ++argc)
	len += strlen (argv[argc]) + 1;
    }

  copy = (char **) malloc ((argc + 1) * sizeof (char *) + len);
  if (copy == NULL)
    return NULL;

  p = (char *) copy + (argc + 1) * sizeof (char *);
  for (i = 0; i < argc; ++i)
    {
      copy[i] = p;
      strcpy (p, argv[i]);
      p += strlen (argv[i]) + 1;
    }
  copy[argc] = 0;

  return copy;
}

/* Analyze a bfd and set various fields in the state struct.  */

void
sim_analyze_program (sd, prog_bfd)
     SIM_DESC sd;
     bfd *prog_bfd;
{
  asection *s;

  STATE_PROG_BFD (sd) = prog_bfd;
  STATE_START_ADDR (sd) = bfd_get_start_address (prog_bfd);

  for (s = prog_bfd->sections; s; s = s->next)
    if (strcmp (bfd_get_section_name (prog_bfd, s), ".text") == 0)
      {
	STATE_TEXT_SECTION (sd) = s;
	STATE_TEXT_START (sd) = bfd_get_section_vma (prog_bfd, s);
	STATE_TEXT_END (sd) = STATE_TEXT_START (sd) + bfd_section_size (prog_bfd, s);
	break;
      }
}

/* Simulator timing support.  */

/* Called before sim_elapsed_time_since to get a reference point.  */

SIM_ELAPSED_TIME
sim_elapsed_time_get ()
{
#ifdef HAVE_GETRUSAGE
  struct rusage mytime;
  if (getrusage (RUSAGE_SELF, &mytime) == 0)
    return (SIM_ELAPSED_TIME) (((double) mytime.ru_utime.tv_sec * 1000) + (((double) mytime.ru_utime.tv_usec + 500) / 1000));
  return 0;
#else
#ifdef HAVE_TIME
  return (SIM_ELAPSED_TIME) time ((time_t) 0);
#else
  return 0;
#endif
#endif
}

/* Return the elapsed time in milliseconds since START.
   The actual time may be cpu usage (prefered) or wall clock.  */

unsigned long
sim_elapsed_time_since (start)
     SIM_ELAPSED_TIME start;
{
#ifdef HAVE_GETRUSAGE
  return sim_elapsed_time_get () - start;
#else
#ifdef HAVE_TIME
  return (sim_elapsed_time_get () - start) * 1000;
#else
  return 0;
#endif
#endif
}
