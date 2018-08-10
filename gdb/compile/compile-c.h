/* Header file for GDB compile C-language support.
   Copyright (C) 2014-2018 Free Software Foundation, Inc.

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

#ifndef GDB_COMPILE_C_H
#define GDB_COMPILE_C_H

#include "common/enum-flags.h"
#include "hashtab.h"

/* enum-flags wrapper.  */

DEF_ENUM_FLAGS_TYPE (enum gcc_qualifiers, gcc_qualifiers_flags);

/* A callback suitable for use as the GCC C symbol oracle.  */

extern gcc_c_oracle_function gcc_convert_symbol;

/* A callback suitable for use as the GCC C address oracle.  */

extern gcc_c_symbol_address_function gcc_symbol_address;

/* A subclass of compile_instance that is specific to the C front
   end.  */

struct compile_c_instance
{
  /* Base class.  Note that the base class vtable actually points to a
     gcc_c_fe_vtable.  */
  struct compile_instance base;

  /* Map from gdb types to gcc types.  */
  htab_t type_map;

  /* Map from gdb symbols to gcc error messages to emit.  */
  htab_t symbol_err_map;
};

/* A helper macro that takes a compile_c_instance and returns its
   corresponding gcc_c_context.  */

#define C_CTX(I) ((struct gcc_c_context *) ((I)->base.fe))

/* Emit code to compute the address for all the local variables in
   scope at PC in BLOCK.  Returns a malloc'd vector, indexed by gdb
   register number, where each element indicates if the corresponding
   register is needed to compute a local variable.  */

extern gdb::unique_xmalloc_ptr<unsigned char>
  generate_c_for_variable_locations
     (struct compile_c_instance *compiler,
      string_file &stream,
      struct gdbarch *gdbarch,
      const struct block *block,
      CORE_ADDR pc);

/* Get the GCC mode attribute value for a given type size.  */

extern const char *c_get_mode_for_size (int size);

/* Given a dynamic property, return an xmallocd name that is used to
   represent its size.  The result must be freed by the caller.  The
   contents of the resulting string will be the same each time for
   each call with the same argument.  */

struct dynamic_prop;
extern std::string c_get_range_decl_name (const struct dynamic_prop *prop);

#endif /* GDB_COMPILE_C_H  */
