/* Copyright (C) 2006-2018 Free Software Foundation, Inc.

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

#ifndef ARCH_TDESC_H
#define ARCH_TDESC_H 1

#ifdef GDBSERVER
#include "server.h"
#else
#include "defs.h"
#endif

struct tdesc_feature;
struct tdesc_type;
struct tdesc_type_builtin;
struct tdesc_type_vector;
struct tdesc_type_with_fields;
struct tdesc_reg;
struct target_desc;

/* The interface to visit different elements of target description.  */

class tdesc_element_visitor
{
public:
  virtual void visit_pre (const target_desc *e)
  {}

  virtual void visit_post (const target_desc *e)
  {}

  virtual void visit_pre (const tdesc_feature *e)
  {}

  virtual void visit_post (const tdesc_feature *e)
  {}

  virtual void visit (const tdesc_type_builtin *e)
  {}

  virtual void visit (const tdesc_type_vector *e)
  {}

  virtual void visit (const tdesc_type_with_fields *e)
  {}

  virtual void visit (const tdesc_reg *e)
  {}
};

class tdesc_element
{
public:
  virtual void accept (tdesc_element_visitor &v) const = 0;
};

/* An individual register from a target description.  */

struct tdesc_reg : tdesc_element
{
  tdesc_reg (struct tdesc_feature *feature, const std::string &name_,
	     int regnum, int save_restore_, const char *group_,
	     int bitsize_, const char *type_);

  virtual ~tdesc_reg () = default;

  DISABLE_COPY_AND_ASSIGN (tdesc_reg);

  /* The name of this register.  In standard features, it may be
     recognized by the architecture support code, or it may be purely
     for the user.  */
  std::string name;

  /* The register number used by this target to refer to this
     register.  This is used for remote p/P packets and to determine
     the ordering of registers in the remote g/G packets.  */
  long target_regnum;

  /* If this flag is set, GDB should save and restore this register
     around calls to an inferior function.  */
  int save_restore;

  /* The name of the register group containing this register, or empty
     if the group should be automatically determined from the
     register's type.  If this is "general", "float", or "vector", the
     corresponding "info" command should display this register's
     value.  It can be an arbitrary string, but should be limited to
     alphanumeric characters and internal hyphens.  Currently other
     strings are ignored (treated as empty).  */
  std::string group;

  /* The size of the register, in bits.  */
  int bitsize;

  /* The type of the register.  This string corresponds to either
     a named type from the target description or a predefined
     type from GDB.  */
  std::string type;

  /* The target-described type corresponding to TYPE, if found.  */
  struct tdesc_type *tdesc_type;

  void accept (tdesc_element_visitor &v) const override
  {
    v.visit (this);
  }

  bool operator== (const tdesc_reg &other) const
  {
    return (name == other.name
       && target_regnum == other.target_regnum
       && save_restore == other.save_restore
       && bitsize == other.bitsize
       && group == other.group
       && type == other.type);
  }

  bool operator!= (const tdesc_reg &other) const
  {
    return !(*this == other);
  }
};

typedef std::unique_ptr<tdesc_reg> tdesc_reg_up;

/* Allocate a new target_desc.  */
target_desc *allocate_target_description (void);

/* Set TARGET_DESC's architecture by NAME.  */
void set_tdesc_architecture (target_desc *target_desc,
			     const char *name);

/* Set TARGET_DESC's osabi by NAME.  */
void set_tdesc_osabi (target_desc *target_desc, const char *name);

/* Return the type associated with ID in the context of FEATURE, or
   NULL if none.  */
struct tdesc_type *tdesc_named_type (const struct tdesc_feature *feature,
				     const char *id);

/* Return the created feature named NAME in target description TDESC.  */
struct tdesc_feature *tdesc_create_feature (struct target_desc *tdesc,
					    const char *name,
					    const char *xml = nullptr);


/* Return the created vector tdesc_type named NAME in FEATURE.  */
struct tdesc_type *tdesc_create_vector (struct tdesc_feature *feature,
					const char *name,
					struct tdesc_type *field_type,
					int count);

/* Return the created struct tdesc_type named NAME in FEATURE.  */
tdesc_type_with_fields *tdesc_create_struct (struct tdesc_feature *feature,
					     const char *name);

/* Return the created union tdesc_type named NAME in FEATURE.  */
tdesc_type_with_fields *tdesc_create_union (struct tdesc_feature *feature,
					    const char *name);

/* Return the created flags tdesc_type named NAME in FEATURE.  */
tdesc_type_with_fields *tdesc_create_flags (struct tdesc_feature *feature,
					    const char *name,
					    int size);

/* Add a new field to TYPE.  FIELD_NAME is its name, and FIELD_TYPE is
   its type.  */
void tdesc_add_field (tdesc_type_with_fields *type, const char *field_name,
		      struct tdesc_type *field_type);

/* Set the total length of TYPE.  Structs which contain bitfields may
   omit the reserved bits, so the end of the last field may not
   suffice.  */
void tdesc_set_struct_size (tdesc_type_with_fields *type, int size);

/* Add a new untyped bitfield to TYPE.
   Untyped bitfields become either uint32 or uint64 depending on the size
   of the underlying type.  */
void tdesc_add_bitfield (tdesc_type_with_fields *type, const char *field_name,
			 int start, int end);

/* A flag is just a typed(bool) single-bit bitfield.
   This function is kept to minimize changes in generated files.  */
void tdesc_add_flag (tdesc_type_with_fields *type, int start,
		     const char *flag_name);

/* Create a register in feature FEATURE.  */
void tdesc_create_reg (struct tdesc_feature *feature, const char *name,
		       int regnum, int save_restore, const char *group,
		       int bitsize, const char *type);

#endif /* ARCH_TDESC_H */
