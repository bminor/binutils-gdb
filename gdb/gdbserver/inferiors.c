/* Inferior process information for the remote server for GDB.
   Copyright 2002
   Free Software Foundation, Inc.

   Contributed by MontaVista Software.

   This file is part of GDB.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <stdlib.h>

#include "server.h"

struct inferior_info
{
  int pid;
  void *target_data;
  struct inferior_info *next;
};

static struct inferior_info *inferiors;
struct inferior_info *current_inferior;
int signal_pid;

void
add_inferior (int pid)
{
  struct inferior_info *new_inferior
    = (struct inferior_info *) malloc (sizeof (*new_inferior));

  memset (new_inferior, 0, sizeof (*new_inferior));

  new_inferior->pid = pid;

  new_inferior->next = inferiors;
  inferiors = new_inferior;

  if (current_inferior == NULL)
    current_inferior = inferiors;

  if (signal_pid == 0)
    signal_pid = pid;
}

void
clear_inferiors (void)
{
  struct inferior_info *inf = inferiors, *next_inf;

  while (inf)
    {
      next_inf = inf->next;

      if (inf->target_data)
	free (inf->target_data);

      free (inf);
      inf = next_inf;
    }

  inferiors = NULL;
}

void *
inferior_target_data (struct inferior_info *inferior)
{
  return inferior->target_data;
}

void
set_inferior_target_data (struct inferior_info *inferior, void *data)
{
  inferior->target_data = data;
}
