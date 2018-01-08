/* Language independent support for printing types for GDB, the GNU debugger.
   Copyright (C) 1986-2018 Free Software Foundation, Inc.

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

#ifndef TYPEPRINT_H
#define TYPEPRINT_H

enum language;
struct ui_file;
struct typedef_hash_table;
struct ext_lang_type_printers;

struct print_offset_data
{
  /* The offset to be applied to bitpos when PRINT_OFFSETS is true.
     This is needed for when we are printing nested structs and want
     to make sure that the printed offset for each field carries over
     the offset of the outter struct.  */
  unsigned int offset_bitpos = 0;

  /* END_BITPOS is the one-past-the-end bit position of the previous
     field (where we expect the current field to be if there is no
     hole).  */
  unsigned int end_bitpos = 0;
};

struct type_print_options
{
  /* True means that no special printing flags should apply.  */
  unsigned int raw : 1;

  /* True means print methods in a class.  */
  unsigned int print_methods : 1;

  /* True means print typedefs in a class.  */
  unsigned int print_typedefs : 1;

  /* True means to print offsets, a la 'pahole'.  */
  unsigned int print_offsets : 1;

  /* The number of nested type definitions to print.  -1 == all.  */
  int print_nested_type_limit;

  /* If not NULL, a local typedef hash table used when printing a
     type.  */
  struct typedef_hash_table *local_typedefs;

  /* If not NULL, a global typedef hash table used when printing a
     type.  */
  struct typedef_hash_table *global_typedefs;

  /* The list of type printers associated with the global typedef
     table.  This is intentionally opaque.  */
  struct ext_lang_type_printers *global_printers;
};

extern const struct type_print_options type_print_raw_options;

void recursively_update_typedef_hash (struct typedef_hash_table *,
				      struct type *);

void add_template_parameters (struct typedef_hash_table *, struct type *);

struct typedef_hash_table *create_typedef_hash (void);

void free_typedef_hash (struct typedef_hash_table *);

struct cleanup *make_cleanup_free_typedef_hash (struct typedef_hash_table *);

struct typedef_hash_table *copy_typedef_hash (struct typedef_hash_table *);

const char *find_typedef_in_hash (const struct type_print_options *,
				  struct type *);

void print_type_scalar (struct type * type, LONGEST, struct ui_file *);

void c_type_print_varspec_suffix (struct type *, struct ui_file *, int,
				  int, int, const struct type_print_options *);

void c_type_print_args (struct type *, struct ui_file *, int, enum language,
			const struct type_print_options *);

/* Print <unknown return type> to stream STREAM.  */

void type_print_unknown_return_type (struct ui_file *stream);

/* Throw an error indicating that the user tried to use a symbol that
   has unknown type.  SYM_PRINT_NAME is the name of the symbol, to be
   included in the error message.  */
extern void error_unknown_type (const char *sym_print_name);

extern void val_print_not_allocated (struct ui_file *stream);

extern void val_print_not_associated (struct ui_file *stream);

#endif
