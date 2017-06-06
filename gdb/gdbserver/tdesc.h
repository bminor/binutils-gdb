/* Target description definitions for remote server for GDB.
   Copyright (C) 2012-2017 Free Software Foundation, Inc.

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

#ifndef TDESC_H
#define TDESC_H

#include "regdef.h"
#include <cstring>

typedef struct reg *tdesc_reg_p;
DEF_VEC_P(tdesc_reg_p);

/* A target description.  */

struct target_desc
{
  /* A vector of elements of register definitions that
     describe the inferior's register set.  */
  VEC(tdesc_reg_p) *reg_defs;

  /* The register cache size, in bytes.  */
  int registers_size;

#ifndef IN_PROCESS_AGENT
  /* An array of register names.  These are the "expedite" registers:
     registers whose values are sent along with stop replies.  */
  const char **expedite_regs = NULL;

  /* Defines what to return when looking for the "target.xml" file in
     response to qXfer:features:read.  Its contents can either be
     verbatim XML code (prefixed with a '@') or else the name of the
     actual XML file to be used in place of "target.xml".  */
  const char *xmltarget = NULL;

public:
  target_desc ()
    : reg_defs (NULL), registers_size (0)
  {}

  ~target_desc ()
  {
    int i;
    struct reg *reg;

    for (i = 0; VEC_iterate (tdesc_reg_p, reg_defs, i, reg); i++)
      xfree (reg);
    VEC_free (tdesc_reg_p, reg_defs);
  }
  bool operator!= (const target_desc &other) const
  {
    if (VEC_length (tdesc_reg_p, reg_defs)
	!= VEC_length (tdesc_reg_p, other.reg_defs))
      return true;

    struct reg *reg;

    for (int ix = 0;
	 VEC_iterate (tdesc_reg_p, reg_defs, ix, reg);
	 ix++)
      {
	struct reg *reg2
	  = VEC_index (tdesc_reg_p, other.reg_defs, ix);

	if (reg != reg2 && *reg != *reg2)
	  return true;
      }

    /* Compare expedite_regs.  */
    int i = 0;
    for (; expedite_regs[i] != NULL; i++)
      {
	if (strcmp (expedite_regs[i], other.expedite_regs[i]) != 0)
	  return true;
      }
    if (other.expedite_regs[i] != NULL)
      return true;

    if (strcmp (xmltarget, other.xmltarget) != 0)
      return true;

    return false;
  }

  bool operator== (const target_desc &other) const
  {
    return !(*this != other);
  }
#endif
};

/* Copy target description SRC to DEST.  */

void copy_target_description (struct target_desc *dest,
			      const struct target_desc *src);

/* Initialize TDESC.  */

void init_target_desc (struct target_desc *tdesc);

/* Return the current inferior's target description.  Never returns
   NULL.  */

const struct target_desc *current_target_desc (void);

#ifndef IN_PROCESS_AGENT
#endif

#define tdesc_feature target_desc

struct tdesc_type;

struct tdesc_feature *tdesc_create_feature (struct target_desc *tdesc,
					    const char *name);

struct tdesc_type *tdesc_create_flags (struct tdesc_feature *feature,
				       const char *name, int size);

void tdesc_add_flag (struct tdesc_type *type, int start,
		     const char *flag_name);

struct tdesc_type *tdesc_named_type (const struct tdesc_feature *feature,
				     const char *id);

#define tdesc_create_union tdesc_named_type
#define tdesc_create_struct tdesc_named_type

void tdesc_add_field (struct tdesc_type *type, const char *field_name,
		      struct tdesc_type *field_type);

void tdesc_set_struct_size (struct tdesc_type *type, int size);

void tdesc_add_bitfield (struct tdesc_type *type, const char *field_name,
			 int start, int end);

void tdesc_create_reg (struct tdesc_feature *feature, const char *name,
		       int regnum, int save_restore, const char *group,
		       int bitsize, const char *type);

struct tdesc_type *tdesc_create_vector (struct tdesc_feature *feature,
					const char *name,
					struct tdesc_type *field_type,
					int count);

#endif /* TDESC_H */
