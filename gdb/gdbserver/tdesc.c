/* Copyright (C) 2012-2017 Free Software Foundation, Inc.

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

#include "server.h"
#include "tdesc.h"
#include "regdef.h"

void
init_target_desc (struct target_desc *tdesc)
{
  int offset, i;
  struct reg *reg;

  offset = 0;
  for (i = 0; VEC_iterate (tdesc_reg_p, tdesc->reg_defs, i, reg); i++)
    {
      reg->offset = offset;
      offset += reg->size;
    }

  tdesc->registers_size = offset / 8;

  /* Make sure PBUFSIZ is large enough to hold a full register
     packet.  */
  gdb_assert (2 * tdesc->registers_size + 32 <= PBUFSIZ);
}

#ifndef IN_PROCESS_AGENT

static const struct target_desc default_description {};

void
copy_target_description (struct target_desc *dest,
			 const struct target_desc *src)
{
  dest->reg_defs = src->reg_defs;
  dest->expedite_regs = src->expedite_regs;
  dest->registers_size = src->registers_size;
  dest->xmltarget = src->xmltarget;
}

const struct target_desc *
current_target_desc (void)
{
  if (current_thread == NULL)
    return &default_description;

  return current_process ()->tdesc;
}
#endif

struct tdesc_type
{};

/* See arch/tdesc.h.  */

struct tdesc_feature *
tdesc_create_feature (struct target_desc *tdesc, const char *name)
{
  return tdesc;
}

/* See arch/tdesc.h.  */

struct tdesc_type *
tdesc_create_flags (struct tdesc_feature *feature, const char *name,
		    int size)
{
  return NULL;
}

/* See arch/tdesc.h.  */

void
tdesc_add_flag (struct tdesc_type *type, int start,
		const char *flag_name)
{}

/* See arch/tdesc.h.  */

struct tdesc_type *
tdesc_named_type (const struct tdesc_feature *feature, const char *id)
{
  return NULL;
}

/* See arch/tdesc.h.  */

struct tdesc_type *
tdesc_create_union (struct tdesc_feature *feature, const char *id)
{
  return NULL;
}

/* See arch/tdesc.h.  */

struct tdesc_type *
tdesc_create_struct (struct tdesc_feature *feature, const char *id)
{
  return NULL;
}

/* See arch/tdesc.h.  */

void
tdesc_create_reg (struct tdesc_feature *feature, const char *name,
		  int regnum, int save_restore, const char *group,
		  int bitsize, const char *type)
{
  struct target_desc *tdesc = (struct target_desc *) feature;

  while (VEC_length (tdesc_reg_p, tdesc->reg_defs) < regnum)
    {
      struct reg *reg = XCNEW (struct reg);

      reg->name = "";
      reg->size = 0;
      VEC_safe_push (tdesc_reg_p, tdesc->reg_defs, reg);
    }

  gdb_assert (regnum == 0
	      || regnum == VEC_length (tdesc_reg_p, tdesc->reg_defs));

  struct reg *reg = XCNEW (struct reg);

  reg->name = name;
  reg->size = bitsize;
  VEC_safe_push (tdesc_reg_p, tdesc->reg_defs, reg);
}

/* See arch/tdesc.h.  */

struct tdesc_type *
tdesc_create_vector (struct tdesc_feature *feature, const char *name,
		     struct tdesc_type *field_type, int count)
{
  return NULL;
}

void
tdesc_add_bitfield (struct tdesc_type *type, const char *field_name,
		    int start, int end)
{}

/* See arch/tdesc.h.  */

void
tdesc_add_field (struct tdesc_type *type, const char *field_name,
		 struct tdesc_type *field_type)
{}

/* See arch/tdesc.h.  */

void
tdesc_set_struct_size (struct tdesc_type *type, int size)
{
}
