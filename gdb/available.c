/* Support for runtime-defined target features for GDB.

   Copyright (C) 2006
   Free Software Foundation, Inc.

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
#include "gdbtypes.h"
#include "symfile.h"
#include "target.h"

#include "available.h"

#include "gdb_assert.h"
#include "gdb_string.h"
#include "gdb_obstack.h"

/* TODO: Remote target "guess" features from g packet size */

/* TODO: Clarify warning messages.  Remember, they will appear to
   the user with no context after a "target remote".  The user
   doesn't know how we got into this code.  */

/* Architecture TODO to support this:

   - Call available_register_type from _register_type method.
   - Handle unexpected changes to _num_regs.
   - Call record_available_features from _gdbarch_init.
   - Do not override the default _register_byte
   - Provide gdbarch_remote_num_g_packet_regs
*/

/* FIXME: Everywhere we call internal_error from this file leads to a failure
   to initialize a gdbarch, which leads to later failures when we expect
   e.g. current_regcache to have been initialized.  */

/* Read a string representation of available features from the target,
   using TARGET_OBJECT_AVAILABLE_FEATURES.  The returned string is
   malloc allocated and NUL-terminated.  If NAME is NULL, the overall
   feature set is read; otherwise the specified name is read (e.g.
   resolving xi:include).  */

static char *
fetch_available_features_from_target (const char *name, void *ops_)
{
  struct target_ops *ops = ops_;
  char *features_str;
  gdb_byte *features_buf;
  LONGEST len;

  struct gdb_feature_set *features;
  struct gdb_available_feature **slot;
  int ret;

  len = target_read_whole (ops, TARGET_OBJECT_AVAILABLE_FEATURES,
			   NULL, &features_buf);
  if (len <= 0)
    return NULL;

  /* Since we decode this object as a string, simplify processing by
     making sure it is NUL terminated.  */
  features_str = (char *) features_buf;
  if (features_str[len - 1] != '\0')
    {
      features_str = xrealloc (features_str, len + 1);
      features_str[len] = '\0';
    }

  return features_str;
}

/* Standard method to convert a string representation of available features
   to a binary representation.  The string representation is fetched using
   TARGET_OBJECT_AVAILABLE_FEATURES.  */

struct gdb_feature_set *
available_features_from_target_object (struct target_ops *ops,
				       struct obstack *obstack)
{
  struct gdb_feature_set *features;
  char *features_str, *cur;
  gdb_byte *features_buf;
  LONGEST len;
  struct gdb_available_feature **slot;
  int ret;

  features_str = fetch_available_features_from_target (NULL, ops);

  features = OBSTACK_ZALLOC (obstack, struct gdb_feature_set);
  features->obstack = obstack;
  ret = available_features_from_xml_string (&features->features, obstack,
					    features_str,
					    fetch_available_features_from_target,
					    ops, 0);

  if (ret < 0)
    {
      warning (_("Could not parse features XML from target"));
      return NULL;
    }

  return features;
}

/* Return non-zero if LHS and RHS refer to compatible feature sets.  */

int
features_same_p (const struct gdb_feature_set *lhs,
		 const struct gdb_feature_set *rhs)
{
  const struct gdb_available_feature *lhs_p, *rhs_p;

  lhs_p = lhs->features;
  rhs_p = rhs->features;
  while (lhs_p && rhs_p)
    {
      lhs_p = lhs_p->next;
      rhs_p = rhs_p->next;
    }
  if (lhs_p || rhs_p)
    return 0;

  /* FIXME: This checking assumes that there are no features with
     duplicate names in either set; enforce that when creating sets.  */

  for (lhs_p = lhs->features; lhs_p; lhs_p = lhs_p->next)
    {
      for (rhs_p = rhs->features; rhs_p; rhs_p = rhs_p->next)
	if (strcmp (lhs_p->name, rhs_p->name) == 0)
	  break;
      if (rhs_p == NULL)
	return 0;

      /* FIXME: Check feature contents, especially for "custom" features
	 which don't have a standard meaning!  */
    }

  return 1;
}

/* Switch the architecture (gdbarch) to one which supports FEATURES.  */

void
arch_set_available_features (const struct gdb_feature_set *features)
{
  struct gdbarch_info info;

  gdbarch_info_init (&info);
  info.feature_set = features;
  if (!gdbarch_update_p (info))
    internal_error (__FILE__, __LINE__, "could not update architecture");
}

static struct gdb_feature_set *
copy_features_to_obstack (struct obstack *obstack,
			  const struct gdb_feature_set *features)
{
  struct gdb_feature_set *result;
  struct gdb_available_feature **slot, *orig_feature;

  result = obstack_alloc (obstack, sizeof (struct gdb_feature_set));
  result->obstack = obstack;

  slot = &result->features;
  for (orig_feature = features->features;
       orig_feature;
       orig_feature = orig_feature->next)
    {
      struct gdb_available_feature *feature;
      struct gdb_available_register **reg_slot, *orig_reg;

      feature = OBSTACK_ZALLOC (obstack, struct gdb_available_feature);
      *slot = feature;
      slot = &feature->next;

      memcpy (feature, orig_feature, sizeof (struct gdb_available_feature));
      feature->name = obsavestring (feature->name, strlen (feature->name),
				    obstack);
      if (feature->arch_data)
	feature->arch_data = obsavestring (feature->arch_data,
					   strlen (feature->arch_data),
					   obstack);

      reg_slot = &feature->registers;
      for (orig_reg = orig_feature->registers;
	   orig_reg;
	   orig_reg = orig_reg->next)
	{
	  struct gdb_available_register *reg;

	  reg = OBSTACK_ZALLOC (obstack, struct gdb_available_register);
	  *reg_slot = reg;
	  reg_slot = &reg->next;

	  memcpy (reg, orig_reg, sizeof (struct gdb_available_register));
	  reg->name = obsavestring (reg->name, strlen (reg->name), obstack);
	  if (reg->arch_data)
	    reg->arch_data = obsavestring (reg->arch_data,
					   strlen (reg->arch_data),
					   obstack);
	  if (reg->group)
	    reg->group = obsavestring (reg->group, strlen (reg->group),
				       obstack);
	  if (reg->type)
	    reg->type = obsavestring (reg->type, strlen (reg->type),
				      obstack);
	}
    }

  return result;
}

/* Set an architecture's feature set.  Store BASE_FEATURES in GDBARCH,
   and on the correct obstack.

   This function will update num_regs.  It is the architecture's
   responsibility to handle this if it has pseudo registers.

   FIXME: This interface may need to go away; what if we want to add
   a single additional feature to that provided by the target?  */

void
record_available_features (struct gdbarch *gdbarch,
			   const struct gdb_feature_set *base_features)
{
  struct gdb_available_feature *feature;
  struct gdb_available_register *reg;
  struct gdb_feature_set *features;
  int gdb_regnum, protocol_number;

  features = copy_features_to_obstack (gdbarch_obstack (gdbarch),
				       base_features);
  set_gdbarch_feature_set (gdbarch, features);

  gdb_regnum = gdbarch_num_regs (gdbarch);

  for (feature = features->features; feature; feature = feature->next)
    {
      protocol_number = feature->protocol_number;
      for (reg = feature->registers; reg; reg = reg->next)
	{
	  reg->gdb_regnum = gdb_regnum++;
	  reg->protocol_number = protocol_number++;
	}
    }

  set_gdbarch_num_regs (gdbarch, gdb_regnum);
}

/* Search FEATURES for a register with GDB register number REGNUM.  */

struct gdb_available_register *
find_register (const struct gdb_feature_set *features, int regnum)
{
  struct gdb_available_feature *feature;
  struct gdb_available_register *reg;

  if (features == NULL)
    return NULL;

  for (feature = features->features; feature; feature = feature->next)
    for (reg = feature->registers; reg; reg = reg->next)
      if (reg->gdb_regnum == regnum)
	return reg;

  return NULL;
}

/* Return the type of target-described register REGNUM, if the feature set
   for GDBARCH describes that register.  Otherwise return NULL.  */

struct type *
available_register_type (struct gdbarch *gdbarch, int regnum)
{
  struct gdb_available_register *reg;

  reg = find_register (gdbarch_feature_set (gdbarch), regnum);
  if (reg == NULL)
    return NULL;

  if (reg->type && strcmp (reg->type, "float") == 0)
    {
      if (reg->bitsize == gdbarch_float_bit (gdbarch))
	return builtin_type_float;
      else if (reg->bitsize == gdbarch_double_bit (gdbarch))
	return builtin_type_double;
      else if (reg->bitsize == gdbarch_long_double_bit (gdbarch))
	return builtin_type_long_double;
    }

  if (reg->type && strcmp (reg->type, "int") != 0)
    {
      /* FIXME: Warn the user about an unknown type + size?  */
    }

  /* Use an integer type; default to "long".  */
  if (reg->bitsize == gdbarch_long_bit (gdbarch))
    return builtin_type_long;
  else if (reg->bitsize == TARGET_CHAR_BIT)
    return builtin_type_signed_char;
  else if (reg->bitsize == gdbarch_short_bit (gdbarch))
    return builtin_type_short;
  else if (reg->bitsize == gdbarch_int_bit (gdbarch))
    return builtin_type_int;
  else if (reg->bitsize == gdbarch_long_long_bit (gdbarch))
    return builtin_type_long_long;
  else if (reg->bitsize == gdbarch_ptr_bit (gdbarch))
    /* A bit desperate by this point... */
    return builtin_type_void_data_ptr;
  else
    {
      /* FIXME: Create a new integer type of the appropriate size?  */
      internal_error (__FILE__, __LINE__,
		      _("GDB does not support %ld-bit registers on this target"),
		      reg->bitsize);
    }
}

/* Return the name of target-described register REGNUM, if the feature set
   for GDBARCH describes that register.  Otherwise return NULL.  */

const char *
available_register_name (struct gdbarch *gdbarch, int regnum)
{
  struct gdb_available_register *reg;

  reg = find_register (gdbarch_feature_set (gdbarch), regnum);
  if (reg == NULL)
    return NULL;

  return reg->name;
}

/* Return the target-supplied register of target-described register
   REGNUM, if the feature set for GDBARCH describes that register.
   Otherwise return REGNUM (the legacy 1:1 mapping).  */

int
available_register_target_regnum (struct gdbarch *gdbarch, int regnum)
{
  struct gdb_available_register *reg;

  reg = find_register (gdbarch_feature_set (gdbarch), regnum);
  if (reg == NULL)
    return regnum;

  return reg->protocol_number;
}
