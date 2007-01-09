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
#include "gdbcmd.h"
#include "target.h"
#include "target-descriptions.h"
#include "vec.h"
#include "xml-tdesc.h"

#include "gdb_assert.h"

/* Types.  */

typedef struct property
{
  char *key;
  char *value;
} property_s;
DEF_VEC_O(property_s);

struct target_desc
{
  /* The architecture reported by the target, if any.  */
  const struct bfd_arch_info *arch;

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

/* Other global variables.  */

/* The filename to read a target description from.  */

static char *target_description_filename;

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

  /* First try to fetch an XML description from the user-specified
     file.  */
  current_target_desc = NULL;
  if (target_description_filename != NULL
      && *target_description_filename != '\0')
    current_target_desc
      = file_read_description_xml (target_description_filename);

  /* Next try to read the description from the current target using
     target objects.  */
  if (current_target_desc == NULL)
    current_target_desc = target_read_description_xml (&current_target);

  /* If that failed try a target-specific hook.  */
  if (current_target_desc == NULL)
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


/* Direct accessors for feature sets.  */

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

/* Return the BFD architecture associated with this target
   description, or NULL if no architecture was specified.  */

const struct bfd_arch_info *
tdesc_architecture (const struct target_desc *target_desc)
{
  return target_desc->arch;
}


/* Methods for constructing a target description.  */

struct target_desc *
allocate_target_description (void)
{
  return XZALLOC (struct target_desc);
}

static void
free_target_description (void *arg)
{
  struct target_desc *target_desc = arg;
  struct property *prop;
  int ix;

  for (ix = 0;
       VEC_iterate (property_s, target_desc->properties, ix, prop);
       ix++)
    {
      xfree (prop->key);
      xfree (prop->value);
    }
  VEC_free (property_s, target_desc->properties);

  xfree (target_desc);
}

struct cleanup *
make_cleanup_free_target_description (struct target_desc *target_desc)
{
  return make_cleanup (free_target_description, target_desc);
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

  new_prop.key = xstrdup (key);
  new_prop.value = xstrdup (value);
  VEC_safe_push (property_s, target_desc->properties, &new_prop);
}

void
set_tdesc_architecture (struct target_desc *target_desc,
			const struct bfd_arch_info *arch)
{
  target_desc->arch = arch;
}


static struct cmd_list_element *tdesc_set_cmdlist, *tdesc_show_cmdlist;
static struct cmd_list_element *tdesc_unset_cmdlist;

/* Helper functions for the CLI commands.  */

static void
set_tdesc_cmd (char *args, int from_tty)
{
  help_list (tdesc_set_cmdlist, "set tdesc ", -1, gdb_stdout);
}

static void
show_tdesc_cmd (char *args, int from_tty)
{
  cmd_show_list (tdesc_show_cmdlist, from_tty, "");
}

static void
unset_tdesc_cmd (char *args, int from_tty)
{
  help_list (tdesc_unset_cmdlist, "unset tdesc ", -1, gdb_stdout);
}

static void
set_tdesc_filename_cmd (char *args, int from_tty,
			struct cmd_list_element *c)
{
  target_clear_description ();
  target_find_description ();
}

static void
show_tdesc_filename_cmd (struct ui_file *file, int from_tty,
			 struct cmd_list_element *c,
			 const char *value)
{
  if (value != NULL && *value != '\0')
    printf_filtered (_("\
The target description will be read from \"%s\".\n"),
		     value);
  else
    printf_filtered (_("\
The target description will be read from the target.\n"));
}

static void
unset_tdesc_filename_cmd (char *args, int from_tty)
{
  xfree (target_description_filename);
  target_description_filename = NULL;
  target_clear_description ();
  target_find_description ();
}

void
_initialize_target_descriptions (void)
{
  add_prefix_cmd ("tdesc", class_maintenance, set_tdesc_cmd, _("\
Set target description specific variables."),
		  &tdesc_set_cmdlist, "set tdesc ",
		  0 /* allow-unknown */, &setlist);
  add_prefix_cmd ("tdesc", class_maintenance, show_tdesc_cmd, _("\
Show target description specific variables."),
		  &tdesc_show_cmdlist, "show tdesc ",
		  0 /* allow-unknown */, &showlist);
  add_prefix_cmd ("tdesc", class_maintenance, unset_tdesc_cmd, _("\
Unset target description specific variables."),
		  &tdesc_unset_cmdlist, "unset tdesc ",
		  0 /* allow-unknown */, &unsetlist);

  add_setshow_filename_cmd ("filename", class_obscure,
			    &target_description_filename,
			    _("\
Set the file to read for an XML target description"), _("\
Show the file to read for an XML target description"), _("\
When set, GDB will read the target description from a local\n\
file instead of querying the remote target."),
			    set_tdesc_filename_cmd,
			    show_tdesc_filename_cmd,
			    &tdesc_set_cmdlist, &tdesc_show_cmdlist);

  add_cmd ("filename", class_obscure, unset_tdesc_filename_cmd, _("\
Unset the file to read for an XML target description.  When unset,\n\
GDB will read the description from the target."),
	   &tdesc_unset_cmdlist);
}
