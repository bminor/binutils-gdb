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
