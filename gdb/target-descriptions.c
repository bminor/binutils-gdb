/* Target description support for GDB.

   Copyright (C) 2006, 2007 Free Software Foundation, Inc.

   Contributed by CodeSourcery.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "defs.h"
#include "arch-utils.h"
#include "target.h"
#include "target-descriptions.h"
#include "vec.h"

#include "gdb_assert.h"

/* Types.  */

typedef struct property
{
  const char *key;
  const char *value;
} property_s;
DEF_VEC_O(property_s);

struct target_desc
{
  /* Any architecture-specific properties specified by the target.  */
  VEC(property_s) *properties;
};

/* Global state.  These variables are associated with the current
   target; if GDB adds support for multiple simultaneous targets, then
   these variables should become target-specific data.  */

/* A flag indicating that a description has already been fetched from
   the current target, so it should not be queried again.  */

static int target_desc_fetched;

/* The description fetched from the current target, or NULL if the
   current target did not supply any description.  Only valid when
   target_desc_fetched is set.  Only the description initialization
   code should access this; normally, the description should be
   accessed through the gdbarch object.  */

static const struct target_desc *current_target_desc;

/* Fetch the current target's description, and switch the current
   architecture to one which incorporates that description.  */

void
target_find_description (void)
{
  /* If we've already fetched a description from the target, don't do
     it again.  This allows a target to fetch the description early,
     during its to_open or to_create_inferior, if it needs extra
     information about the target to initialize.  */
  if (target_desc_fetched)
    return;

  /* The current architecture should not have any target description
     specified.  It should have been cleared, e.g. when we
     disconnected from the previous target.  */
  gdb_assert (gdbarch_target_desc (current_gdbarch) == NULL);

  current_target_desc = target_read_description (&current_target);

  /* If a non-NULL description was returned, then update the current
     architecture.  */
  if (current_target_desc)
    {
      struct gdbarch_info info;

      gdbarch_info_init (&info);
      info.target_desc = current_target_desc;
      if (!gdbarch_update_p (info))
	warning (_("Could not use target-supplied description"));
    }

  /* Now that we know this description is usable, record that we
     fetched it.  */
  target_desc_fetched = 1;
}

/* Discard any description fetched from the current target, and switch
   the current architecture to one with no target description.  */

void
target_clear_description (void)
{
  struct gdbarch_info info;

  if (!target_desc_fetched)
    return;

  target_desc_fetched = 0;
  current_target_desc = NULL;

  gdbarch_info_init (&info);
  if (!gdbarch_update_p (info))
    internal_error (__FILE__, __LINE__,
		    _("Could not remove target-supplied description"));
}

/* Return the global current target description.  This should only be
   used by gdbarch initialization code; most access should be through
   an existing gdbarch.  */

const struct target_desc *
target_current_description (void)
{
  if (target_desc_fetched)
    return current_target_desc;

  return NULL;
}

/* Return the string value of a property named KEY, or NULL if the
   property was not specified.  */

const char *
tdesc_property (const struct target_desc *target_desc, const char *key)
{
  struct property *prop;
  int ix;

  for (ix = 0; VEC_iterate (property_s, target_desc->properties, ix, prop);
       ix++)
    if (strcmp (prop->key, key) == 0)
      return prop->value;

  return NULL;
}

/* Methods for constructing a target description.  */

struct target_desc *
allocate_target_description (void)
{
  return XZALLOC (struct target_desc);
}

void
set_tdesc_property (struct target_desc *target_desc,
		    const char *key, const char *value)
{
  struct property *prop, new_prop;
  int ix;

  gdb_assert (key != NULL && value != NULL);

  for (ix = 0; VEC_iterate (property_s, target_desc->properties, ix, prop);
       ix++)
    if (strcmp (prop->key, key) == 0)
      internal_error (__FILE__, __LINE__,
		      _("Attempted to add duplicate property \"%s\""), key);

  new_prop.key = key;
  new_prop.value = value;
  VEC_safe_push (property_s, target_desc->properties, &new_prop);
}
