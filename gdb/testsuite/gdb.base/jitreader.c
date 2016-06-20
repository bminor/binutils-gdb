/* Copyright (C) 2009-2016 Free Software Foundation, Inc.

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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include JIT_READER_H  /* Please see jit-reader.exp for an explanation.  */
#include "jithost.h"

GDB_DECLARE_GPL_COMPATIBLE_READER;

enum register_mapping
{
  AMD64_RA = 16,
  AMD64_RSP = 7,
};

struct reader_state
{
  uintptr_t code_begin;
  uintptr_t code_end;
};

static enum gdb_status
read_debug_info (struct gdb_reader_funcs *self,
		 struct gdb_symbol_callbacks *cbs,
                 void *memory, long memory_sz)
{
  struct jithost_abi *symfile = memory;
  struct gdb_object *object = cbs->object_open (cbs);
  struct gdb_symtab *symtab = cbs->symtab_open (cbs, object, "");
  GDB_CORE_ADDR begin = (GDB_CORE_ADDR) symfile->begin;
  GDB_CORE_ADDR end = (GDB_CORE_ADDR) symfile->end;

  cbs->block_open (cbs, symtab, NULL, begin, end, "jit_function_00");

  cbs->symtab_close (cbs, symtab);
  cbs->object_close (cbs, object);
  return GDB_SUCCESS;
}

static void
free_reg_value (struct gdb_reg_value *value)
{
  free (value);
}

static void
write_register (struct gdb_unwind_callbacks *callbacks, int dw_reg,
                uintptr_t value)
{
  const int size = sizeof (uintptr_t);
  struct gdb_reg_value *reg_val =
    malloc (sizeof (struct gdb_reg_value) + size - 1);
  reg_val->defined = 1;
  reg_val->free = free_reg_value;

  memcpy (reg_val->value, &value, size);
  callbacks->reg_set (callbacks, dw_reg, reg_val);
}

static int
read_register (struct gdb_unwind_callbacks *callbacks, int dw_reg,
	       uintptr_t *value)
{
  const int size = sizeof (uintptr_t);
  struct gdb_reg_value *reg_val = callbacks->reg_get (callbacks, dw_reg);
  if (reg_val->size != size || !reg_val->defined)
    {
      reg_val->free (reg_val);
      return 0;
    }
  memcpy (value, reg_val->value, size);
  reg_val->free (reg_val);
  return 1;
}

static enum gdb_status
unwind_frame (struct gdb_reader_funcs *self, struct gdb_unwind_callbacks *cbs)
{
  const int word_size = sizeof (uintptr_t);
  uintptr_t this_sp, this_ip, prev_ip, prev_sp;
  struct reader_state *state = (struct reader_state *) self->priv_data;

  if (!read_register (cbs, AMD64_RA, &this_ip))
    return GDB_FAIL;

  if (this_ip >= state->code_end || this_ip < state->code_begin)
    return GDB_FAIL;

  if (!read_register (cbs, AMD64_RSP, &this_sp))
    return GDB_FAIL;

  if (cbs->target_read (this_sp, &prev_ip, word_size) == GDB_FAIL)
    return GDB_FAIL;

  prev_sp = this_sp + word_size;
  write_register (cbs, AMD64_RA, prev_ip);
  write_register (cbs, AMD64_RSP, prev_sp);
  return GDB_SUCCESS;
}

static struct gdb_frame_id
get_frame_id (struct gdb_reader_funcs *self, struct gdb_unwind_callbacks *cbs)
{
  struct reader_state *state = (struct reader_state *) self->priv_data;
  struct gdb_frame_id frame_id;

  uintptr_t sp;
  read_register (cbs, AMD64_RSP, &sp);

  frame_id.code_address = (GDB_CORE_ADDR) state->code_begin;
  frame_id.stack_address = (GDB_CORE_ADDR) sp;

  return frame_id;
}

static void
destroy_reader (struct gdb_reader_funcs *self)
{
  free (self->priv_data);
  free (self);
}

struct gdb_reader_funcs *
gdb_init_reader (void)
{
  struct reader_state *state = malloc (sizeof (struct reader_state));
  struct gdb_reader_funcs *reader_funcs =
    malloc (sizeof (struct gdb_reader_funcs));

  reader_funcs->reader_version = GDB_READER_INTERFACE_VERSION;
  reader_funcs->priv_data = state;
  reader_funcs->read = read_debug_info;
  reader_funcs->unwind = unwind_frame;
  reader_funcs->get_frame_id = get_frame_id;
  reader_funcs->destroy = destroy_reader;

  return reader_funcs;
}
