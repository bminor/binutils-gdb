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

#ifndef AVAILABLE_H
#define AVAILABLE_H 1

struct obstack;

/* A GDB target interface can use these types to communicate to the
   architecture support (gdbarch) what new or optional features
   it supports.

   Each individual gdb_available_feature describes a target-specific
   unit of functionality, often including a group of registers,
   possibly including other bits of which the debugger needs
   explicit knowledge.  GDB may recognize the feature by name,
   or query the target for additional details.  If it recognizes
   the feature by name, it may take advantage of the feature's
   presence in additional ways - e.g. by knowing the calling
   conventions for functions using the new registers.

   If GDB does not recognize the feature by name, and the feature
   requests explicit debugger support, GDB may suggest an upgrade
   to the user.  */

struct gdb_feature_set
{
  struct gdb_available_feature *features;

  struct obstack *obstack;

  /* The checksum of the XML which generated this feature set.  */
  /* FIXME: Should we just save the entire XML instead?  We're going
     to need to have it around to show the frontend in the future.  */
  unsigned char checksum[20];
};

struct gdb_available_feature
{
  /* The name of this feature.  For features, the name
     is recognized by the architecture.  */
  const char *name;

  /* The protocol number used by this target to provide this feature.
     For instance, the base register number for a group of raw
     registers included in a known feature.  If none is necessary this
     may be set to -1.  */
  int protocol_number;

  /* Data private to the architecture associated with this feature.
     This is a NUL-terminated string.  */
  const char *arch_data;

  /* The registers associated with this feature.  */
  struct gdb_available_register *registers;

  /* Chain to the next available feature in this set.  */
  struct gdb_available_feature *next;
};

struct gdb_available_register
{
  /* The name of this feature.  For registers, the name is
     only used by the user interface.  */
  const char *name;

  /* The protocol number used by this target to provide this
     feature.  For instance, the register number used for remote
     p/P packets to access this register.  */
  long protocol_number;

  /* Data private to the architecture associated with this feature.
     This is a NUL-terminated string.  */
  const char *arch_data;

  /* If this flag is set, GDB should never try to write to this
     register.  Otherwise, the user may modify the value in the
     register.  */
  int readonly;

  /* If this flag is set, GDB should save and restore this register
     around calls to an inferior function.  */
  /* FIXME: Richard Earnshaw proposed an alternate, more thourough
     categorization.  Should we use that instead?  */
  int save_restore;

  /* The name of the register group containing this register.  If this
     is "general", "float", or "vector", the corresponding "info" command
     should display this register's value.  It can be an arbitrary
     string, but should be limited to alphanumeric characters and internal
     hyphens.  */
  const char *group;

  /* The size of the register, in bits.  */
  long bitsize;

  /* The type of the register.  This is a target-supplied string,
     corresponding to FIXME FIXME.  */
  const char *type;

  /* GDB internal state for this register; this may vary per copy
     of this code in each gdbarch.  */

  /* GDB register number for this register.  */
  int gdb_regnum;

  /* Chain to the next available register in this feature.  */
  struct gdb_available_register *next;
};

/* Standard method to convert a string representation of available features
   to a binary representation.  The string representation is fetched using
   TARGET_OBJECT_AVAILABLE_FEATURES.  */

struct gdb_feature_set *available_features_from_target_object
  (struct target_ops *, struct obstack *);

/* Standard method to update an architecture based on detected available
   features.  */

void arch_set_available_features (struct gdb_feature_set *);

/* Compare two sets of available features.  */

int features_same_p (const struct gdb_feature_set *,
		     const struct gdb_feature_set *);

/* Set an architecture's feature set.  */

void record_available_features (struct gdbarch *,
				struct gdb_feature_set *);

/* Find a register with the given name, and set its internal register
   number.  */

int available_find_named_register (struct gdb_feature_set *,
				   const char *, int);

/* Find a feature with the given name.  */

int available_find_named_feature (struct gdb_feature_set *,
				  const char *);

/* Find the type of a target-described register.  */

struct type *available_register_type (struct gdbarch *, int);

/* Find the name of a target-described register.  */

const char *available_register_name (struct gdbarch *, int);

/* Find the target-supplied register number of a target-described register.  */

int available_register_target_regnum (struct gdbarch *, int);

/* Find a compiled-in XML file, e.g. the standard DTD.  */

const char *fetch_xml_builtin (const char *);

/* Internal routines shared by available.c and parse-avail.c.  */

typedef char *(*xml_fetch_another) (const char *, void *);

int available_features_from_xml_string (struct gdb_feature_set *,
					const char *,
					xml_fetch_another, void *,
					int);

#endif /* AVAILABLE_H */
