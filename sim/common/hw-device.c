/*  This file is part of the program psim.

    Copyright (C) 1994-1998, Andrew Cagney <cagney@highland.com.au>

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


#include "sim-main.h"

#include "hw-device.h"
#include "hw-properties.h"

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif


/* Address methods */

const hw_unit *
hw_unit_address (struct hw *me)
{
  return &me->unit_address_of_hw;
}



/* IOCTL: */

int
hw_ioctl (struct hw *me,
	  hw_ioctl_request request,
	  ...)
{
  int status;
  va_list ap;
  va_start(ap, request);
  status = me->to_ioctl (me, request, ap);
  va_end(ap);
  return status;
}
      
/* Mechanism for associating allocated memory regions to a device.
   When a device is deleted any remaining memory regions are also
   reclaimed.

   FIXME: Perhaphs this can be generalized, perhaphs it should not
   be. */

struct hw_alloc_data {
  void *alloc;
  int zalloc_p;
  struct hw_alloc_data *next;
};

void *
hw_zalloc (struct hw *me, unsigned long size)
{
  struct hw_alloc_data *memory = ZALLOC (struct hw_alloc_data);
  memory->alloc = zalloc (size);
  memory->zalloc_p = 1;
  memory->next = me->alloc_of_hw;
  me->alloc_of_hw = memory;
  return memory->alloc;
}

void *
hw_malloc (struct hw *me, unsigned long size)
{
  struct hw_alloc_data *memory = ZALLOC (struct hw_alloc_data);
  memory->alloc = zalloc (size);
  memory->zalloc_p = 0;
  memory->next = me->alloc_of_hw;
  me->alloc_of_hw = memory;
  return memory->alloc;
}

void
hw_free (struct hw *me,
	 void *alloc)
{
  struct hw_alloc_data **memory;
  for (memory = &me->alloc_of_hw;
       *memory != NULL;
       memory = &(*memory)->next)
    {
      if ((*memory)->alloc == alloc)
	{
	  struct hw_alloc_data *die = (*memory);
	  (*memory) = die->next;
	  if (die->zalloc_p)
	    zfree (die->alloc);
	  else
	    free (die->alloc);
	  zfree (die);
	  return;
	}
    }
  hw_abort (me, "free of memory not belonging to a device");
}

void
hw_free_all (struct hw *me)
{
  while (me->alloc_of_hw != NULL)
    {
      hw_free (me, me->alloc_of_hw->alloc);
    }
}

char *
hw_strdup (struct hw *me, const char *str)
{
  if (str != NULL)
    {
      char *dup = hw_zalloc (me, strlen (str) + 1);
      strcpy (dup, str);
      return dup;
    }
  else
    {
      return NULL;
    }
}
