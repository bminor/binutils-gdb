/* Target operations for the remote server for GDB.
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

#include "server.h"

struct target_ops *the_target;

void
read_inferior_memory (CORE_ADDR memaddr, char *myaddr, int len)
{
  (*the_target->read_memory) (memaddr, myaddr, len);
  check_mem_read (memaddr, myaddr, len);
}

int
write_inferior_memory (CORE_ADDR memaddr, char *myaddr, int len)
{
  check_mem_write (memaddr, myaddr, len);
  return (*the_target->write_memory) (memaddr, myaddr, len);
}

void
set_target_ops (struct target_ops *target)
{
  the_target = (struct target_ops *) malloc (sizeof (*the_target));
  memcpy (the_target, target, sizeof (*the_target));
}
