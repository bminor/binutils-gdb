/* BFD internal declarations for Oasys file format handling */
/* Scrawled by Steve Chamberlain of Cygnus Support.  */

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


typedef struct _oasys_symbol
{
  asymbol symbol;
} oasys_symbol_type;

typedef struct _oasys_reloc {
  arelent relent;
  struct _oasys_reloc *next;
  unsigned int symbol;
} oasys_reloc_type;


#define oasys_symbol(x) ((oasys_symbol_type *)(x))
#define oasys_per_section(x) ((oasys_per_section_type *)(x->used_by_bfd))

typedef struct _oasys_per_section
{
  asection *section;
  bfd_byte *data;
  bfd_vma offset;
  boolean had_vma;
  oasys_reloc_type **reloc_tail_ptr;
  bfd_vma pc;


  file_ptr current_pos;
  unsigned int current_byte;
  boolean initialized;
} oasys_per_section_type;

#define NSECTIONS 10

typedef struct _oasys_ar_obstack {
  file_ptr file_offset;
  bfd *abfd;
} oasys_ar_obstack_type;


typedef struct _oasys_module_info {
  file_ptr pos;
  unsigned int size;
  bfd *abfd;
  char *name;
} oasys_module_info_type;

typedef struct _oasys_ar_data {
  oasys_module_info_type *module;
  unsigned int module_count;
  unsigned int module_index;
} oasys_ar_data_type;

typedef struct _oasys_data {
  struct obstack oasys_obstack;
  char *strings;
  asymbol *symbols;
  unsigned int symbol_string_length;
  asection *sections[OASYS_MAX_SEC_COUNT];
  file_ptr first_data_record;
} oasys_data_type;

#define oasys_data(abfd) ((oasys_data_type *)((abfd)->tdata))
#define oasys_ar_data(abfd) ((oasys_ar_data_type *)((abfd)->tdata))
