/* Copyright (C) 1992-2013 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "target-dcache.h"
#include "gdbcmd.h"
#include "progspace.h"

/* The target dcache is kept per-address-space.  This key lets us
   associate the cache with the address space.  */

static const struct address_space_data *target_dcache_aspace_key;

/* Clean up dcache, represented by ARG, which is associated with
   ASPACE.  */

static void
target_dcache_cleanup (struct address_space *aspace, void *arg)
{
  dcache_free (arg);
}

/* Target dcache is initialized or not.  */

int
target_dcache_init_p (void)
{
  DCACHE *dcache = address_space_data (current_program_space->aspace,
				       target_dcache_aspace_key);

  return (dcache != NULL);
}

/* Invalidate the target dcache.  */

void
target_dcache_invalidate (void)
{
  DCACHE *dcache = address_space_data (current_program_space->aspace,
				       target_dcache_aspace_key);

  if (dcache != NULL)
    dcache_invalidate (dcache);
}

/* Return the target dcache.  Return NULL if target dcache is not
   initialized yet.  */

DCACHE *
target_dcache_get (void)
{
  DCACHE *dcache = address_space_data (current_program_space->aspace,
				       target_dcache_aspace_key);

  return dcache;
}

/* Return the target dcache.  If it is not initialized yet, initialize
   it.  */

DCACHE *
target_dcache_get_or_init (void)
{
  DCACHE *dcache = address_space_data (current_program_space->aspace,
				       target_dcache_aspace_key);

  if (dcache == NULL)
    {
      dcache = dcache_init ();
      set_address_space_data (current_program_space->aspace,
			      target_dcache_aspace_key, dcache);
    }

  return dcache;
}

/* The option sets this.  */
static int stack_cache_enabled_p_1 = 1;
/* And set_stack_cache_enabled_p updates this.
   The reason for the separation is so that we don't flush the cache for
   on->on transitions.  */
static int stack_cache_enabled_p = 1;

/* This is called *after* the stack-cache has been set.
   Flush the cache for off->on and on->off transitions.
   There's no real need to flush the cache for on->off transitions,
   except cleanliness.  */

static void
set_stack_cache_enabled_p (char *args, int from_tty,
			   struct cmd_list_element *c)
{
  if (stack_cache_enabled_p != stack_cache_enabled_p_1)
    target_dcache_invalidate ();

  stack_cache_enabled_p = stack_cache_enabled_p_1;
}

static void
show_stack_cache_enabled_p (struct ui_file *file, int from_tty,
			    struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Cache use for stack accesses is %s.\n"), value);
}

/* Return true if "stack cache" is enabled, otherwise, return false.  */

int
stack_cache_enabled (void)
{
  return stack_cache_enabled_p;
}

/* -Wmissing-prototypes */
extern initialize_file_ftype _initialize_target_dcache;

void
_initialize_target_dcache (void)
{
  add_setshow_boolean_cmd ("stack-cache", class_support,
			   &stack_cache_enabled_p_1, _("\
Set cache use for stack access."), _("\
Show cache use for stack access."), _("\
When on, use the data cache for all stack access, regardless of any\n\
configured memory regions.  This improves remote performance significantly.\n\
By default, caching for stack access is on."),
			   set_stack_cache_enabled_p,
			   show_stack_cache_enabled_p,
			   &setlist, &showlist);

  target_dcache_aspace_key
    = register_address_space_data_with_cleanup (NULL,
						target_dcache_cleanup);
}
