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
#include "exceptions.h"
#include "gdbtypes.h"
#include "symfile.h"
#include "target.h"
#include "sha1.h"

#include "available.h"

#include "gdb_assert.h"
#include "gdb_string.h"
#include "gdb_obstack.h"
#include "gdb_stdint.h"

/* TODO: Remote target "guess" features from g packet size */

/* TODO: Clarify warning messages.  Remember, they will appear to
   the user with no context after a "target remote".  The user
   doesn't know how we got into this code.  */

/* Architecture TODO to support this:

   - Call available_register_type from _register_type method.
   - Handle unexpected changes to _num_regs.
   - Call record_available_features from _gdbarch_init.
   - Do not override the default _register_byte

  (WARNING: This list is out of date and should be redone before submission.
  And moved into gdbint.texi.)
*/



/* Support for caching XML objects read from the target.

   TODO ITEMS:
   - Support caching to disk.
   - Support compiled-in feature cache.
   - Figure out memory management for feature contents strings.
*/


/* Saved information about cached XML objects.  Each cache entry
   corresponds to a file in the cache, or an object fetched from
   the target with one particular annex.  */

struct xml_cache_entry
{
  const char *annex;
  const char *contents;

  /* This flag is cleared when we begin reading features, and set
     for new features when they are read.  It is used to prevent
     reading the same file from the target twice (for multiple
     xi:include's or DTD references).  */
  int recently_used;

  union
  {
    /* We use a union to represent the checksum in order to guarantee
       sufficient alignment.  */
    uint32_t words[5];
    unsigned char bytes[20];
  } sha1sum;

  struct xml_cache_entry *next;
};

/* A list of all the cached objects.  */

static struct xml_cache_entry *xml_global_cache;

/* Look for a feature in the cache with ANNEX and CHECKSUM.  If
   CHECKSUM is NULL, then look for a feature in the cache which has
   already been used this session.  If no entry is found, return
   NULL.  */

static const char *
find_xml_feature_in_cache (const char *annex, const unsigned char *checksum)
{
  struct xml_cache_entry *ent;

  for (ent = xml_global_cache; ent != NULL; ent = ent->next)
    {
      if (strcmp (ent->annex, annex) != 0)
	continue;

      if (checksum == NULL && !ent->recently_used)
	continue;

      if (checksum != NULL && memcmp (ent->sha1sum.bytes, checksum, 20) != 0)
	continue;

      ent->recently_used = 1;
      return ent->contents;
    }

  return NULL;
}

/* Add CONTENTS, which represents the object named ANNEX, to the
   cache if it is not already cached.  */

static void
add_xml_feature_to_cache (const char *annex, const char *contents)
{
  struct xml_cache_entry new_ent;

  /* FIXME: Again, memory allocation?  */
  new_ent.annex = xstrdup (annex);
  new_ent.contents = xstrdup (contents);

  sha1_buffer (new_ent.contents, strlen (new_ent.contents),
	       new_ent.sha1sum.bytes);

  /* If this entry is already in the cache, do not add it again.  This
     call also marks the cache entry as used.  */
  if (find_xml_feature_in_cache (annex, new_ent.sha1sum.bytes))
    return;

  new_ent.recently_used = 1;
  new_ent.next = xml_global_cache;

  xml_global_cache = xmalloc (sizeof (struct xml_cache_entry));
  memcpy (xml_global_cache, &new_ent, sizeof (struct xml_cache_entry));
}

/* Convert an ASCII checksum string, CHECKSUM, to a binary blob,
   BYTES.  Returns 0 for success, or -1 if a bad character is
   encountered.  CHECKSUM does not need to be NUL terminated.  */

static int
checksum_to_bytes (char *checksum, unsigned char *bytes)
{
  int i;

  for (i = 0; i < 20; i++)
    {
      int n;
      char c1, c2;

      c1 = checksum[2 * i];
      if (c1 >= '0' && c1 <= '9')
	n = c1 - '0';
      else if (c1 >= 'a' && c1 <= 'f')
	n = c1 - 'a' + 10;
      else if (c1 >= 'A' && c1 <= 'F')
	n = c1 - 'A' + 10;
      else
	return -1;

      n *= 16;

      c2 = checksum[2 * i + 1];
      if (c2 >= '0' && c2 <= '9')
	n += c2 - '0';
      else if (c2 >= 'a' && c2 <= 'f')
	n += c2 - 'a' + 10;
      else if (c2 >= 'A' && c2 <= 'F')
	n += c2 - 'A' + 10;
      else
	return -1;

      bytes[i] = n;
    }

  return 0;
}

/* Baton passed to fetch_available_features_from_target.  */

struct fetch_features_baton
{
  struct target_ops *ops;

  struct fetch_features_checksum
  {
    const char *annex;
    unsigned char checksum[20];
  } *checksums;
};

/* Read a string representation of available features from the target,
   using TARGET_OBJECT_AVAILABLE_FEATURES.  The returned string is
   malloc allocated and NUL-terminated.  NAME should be a non-NULL
   string identifying the XML document we want; the top level document
   is "target.xml".  Other calls may be performed for the DTD or
   for xi:include.  */

static char *
fetch_available_features_from_target (const char *name, void *baton_)
{
  struct fetch_features_baton *baton = baton_;
  char *features_str;
  gdb_byte *features_buf;
  LONGEST len;
  const unsigned char *checksum = NULL;
  const char *cached_str;

  if (baton->checksums)
    {
      struct fetch_features_checksum *checksum_ent;

      for (checksum_ent = baton->checksums;
	   checksum_ent->annex != NULL;
	   checksum_ent++)
	if (strcmp (checksum_ent->annex, name) == 0)
	  break;

      if (checksum_ent->annex)
	checksum = checksum_ent->checksum;
    }

  cached_str = find_xml_feature_in_cache (name, checksum);

  /* This function always returns something which the caller is
     responsible for freeing.  So, if we got a match, return a
     copy of it.  */
  if (cached_str)
    return xstrdup (cached_str);

  len = target_read_whole (baton->ops, TARGET_OBJECT_AVAILABLE_FEATURES,
			   name, &features_buf);
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

  if (baton->checksums)
    add_xml_feature_to_cache (name, features_str);

  return features_str;
}

/* Standard method to convert a string representation of available features
   to a binary representation.  The string representation is fetched using
   TARGET_OBJECT_AVAILABLE_FEATURES.  */

/* TODO: Document \n conventions */

struct gdb_feature_set *
available_features_from_target_object (struct target_ops *ops,
				       struct obstack *obstack)
{
  struct fetch_features_baton baton;
  struct gdb_feature_set *features;
  char *features_str, *checksums_str;
  int ret;
  struct xml_cache_entry *ent;
  struct cleanup *back_to = make_cleanup (null_cleanup, NULL);

  /* Reset the recently used flag so that we read any objects
     without checksums from the target.  */
  for (ent = xml_global_cache; ent != NULL; ent = ent->next)
    ent->recently_used = 0;

  /* Initialize the baton.  */
  baton.ops = ops;
  baton.checksums = NULL;

  /* Attempt to read checksums from the target.  */
  checksums_str = fetch_available_features_from_target ("CHECKSUMS", &baton);
  if (checksums_str)
    {
      char *p;
      int n_checksums;

      make_cleanup (xfree, checksums_str);

      /* Allow for one checksum in case there is no trailing newline,
	 and one to serve as a NULL terminator.  */
      n_checksums = 2;

      /* Allocate one additional checksum per newline.  */
      for (p = checksums_str; *p; p++)
	if (*p == '\n')
	  n_checksums++;

      baton.checksums = xmalloc (n_checksums
				 * sizeof (struct fetch_features_checksum));
      make_cleanup (xfree, baton.checksums);

      n_checksums = 0;
      p = checksums_str;
      while (*p)
	{
	  char *field_end;

	  /* Find the first space on the line, marking the end of the
	     checksum.  */
	  field_end = p;
	  while (*field_end && *field_end != '\n'
		 && *field_end != ' ')
	    field_end++;

	  /* Check for a malformed checksum.  */
	  if (*field_end != ' '
	      || field_end - p != 40
	      || checksum_to_bytes (p, baton.checksums[n_checksums].checksum))
	    {
	      /* Skip this line.  */
	      p = field_end;
	      while (*p && *p != '\n')
		p++;
	      if (*p == '\n')
		p++;
	      continue;
	    }

	  *field_end = '\0';

	  /* Skip whitespace after the checksum.  */
	  p = field_end + 1;
	  while (*p == ' ')
	    p++;

	  field_end = p;
	  while (*field_end && *field_end != '\n')
	    field_end++;

	  if (field_end == p)
	    {
	      /* Malformed line; skip it.  */
	      if (*p == '\n')
		p++;
	      continue;
	    }

	  baton.checksums[n_checksums++].annex = p;

	  /* Advance to the next line, inserting a NUL for the end of
	     the annex name if necessary.  */
	  if (*field_end)
	    {
	      *field_end = '\0';
	      p = field_end + 1;
	    }
	  else
	    break;
	}

      baton.checksums[n_checksums].annex = NULL;
    }

  /* FIXME: Memory management: what happens to features_str?  */

  features_str = fetch_available_features_from_target ("target.xml", &baton);
  if (features_str == NULL)
    return NULL;

  features = OBSTACK_ZALLOC (obstack, struct gdb_feature_set);
  features->obstack = obstack;
  ret = available_features_from_xml_string (features,
					    features_str,
					    fetch_available_features_from_target,
					    &baton, 0);

  do_cleanups (back_to);

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
  /* Two feature sets are the same if and only if they are described
     by the same XML.  */

  if (memcmp (lhs->checksum, rhs->checksum, 20) == 0)
    return 1;
  else
    return 0;
}

/* Switch the architecture (gdbarch) to one which supports FEATURES.  */

void
arch_set_available_features (struct gdb_feature_set *features)
{
  volatile struct gdb_exception e;
  struct gdbarch_info info;

  TRY_CATCH (e, RETURN_MASK_ERROR)
    {
      gdbarch_info_init (&info);
      info.feature_set = features;

      if (!gdbarch_update_p (info))
	internal_error (__FILE__, __LINE__, "could not update architecture");
    }

  if (e.reason == RETURN_ERROR)
    exception_fprintf (gdb_stderr, e,
		       _("warning: could not use supplied target description: "));
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

   This function will update GDBARCH's num_regs.  It is the
   architecture's responsibility to handle this if it has pseudo
   registers.  Before calling this function, num_regs should be
   the number of fixed registers handled by the target code; all
   unassigned registers will be given numbers above that point.  */

void
record_available_features (struct gdbarch *gdbarch,
			   struct gdb_feature_set *base_features)
{
  struct gdb_available_feature *feature;
  struct gdb_available_register *reg;
  struct gdb_feature_set *features;
  int gdb_regnum;

  features = copy_features_to_obstack (gdbarch_obstack (gdbarch),
				       base_features);
  set_gdbarch_feature_set (gdbarch, features);

  gdb_regnum = gdbarch_num_regs (gdbarch);

  for (feature = features->features; feature; feature = feature->next)
    for (reg = feature->registers; reg; reg = reg->next)
      if (reg->gdb_regnum == -1)
	reg->gdb_regnum = gdb_regnum++;

  set_gdbarch_num_regs (gdbarch, gdb_regnum);
}

/* Search FEATURES for a register with GDB register number REGNUM.  */

static struct gdb_available_register *
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

/* Search FEATURES for a register with target-specified name NAME,
   and set its GDB register number to REGNUM.  Return 1 if the
   register was found, and 0 if it was not.  This function should
   only be used while initializing a gdbarch.  */

int
available_find_named_register (struct gdb_feature_set *features,
			       const char *name, int regnum)
{
  struct gdb_available_feature *feature;
  struct gdb_available_register *reg;

  if (features == NULL)
    return 0;

  for (feature = features->features; feature; feature = feature->next)
    for (reg = feature->registers; reg; reg = reg->next)
      if (strcmp (reg->name, name) == 0)
	{
	  reg->gdb_regnum = regnum;
	  return 1;
	}

  /* FIXME: Should we sanity check the target-supplied data here for
     duplicate register names?  Right now GDB can't handle duplicated
     register names at all, but in the future it may.  */

  return 0;
}

/* Search FEATURES for a feature with the well-known name NAME,
   which GDB may have special support for.  */

int
available_find_named_feature (struct gdb_feature_set *features,
			      const char *name)
{
  struct gdb_available_feature *feature;

  if (features == NULL)
    return 0;

  for (feature = features->features; feature; feature = feature->next)
    if (strcmp (feature->name, name) == 0)
      return 1;

  return 0;
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
   REGNUM, or -1 if the register can not be accessed.  */

int
available_register_target_regnum (struct gdbarch *gdbarch, int regnum)
{
  struct gdb_available_register *reg;

  /* If there is no feature set, use the legacy 1:1 mapping.  */
  if (gdbarch_feature_set (gdbarch) == NULL)
    return regnum;

  reg = find_register (gdbarch_feature_set (gdbarch), regnum);
  if (reg == NULL)
    return -1;

  return reg->protocol_number;
}
