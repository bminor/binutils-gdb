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

struct tdesc_feature;
struct tdesc_type;
struct tdesc_type_builtin;
struct tdesc_type_vector;
struct tdesc_type_with_fields;
struct tdesc_reg;
struct target_desc;

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
