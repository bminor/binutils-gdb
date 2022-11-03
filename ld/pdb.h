/* pdb.h - header file for generating PDB CodeView debugging files.
   Copyright (C) 2022 Free Software Foundation, Inc.

   This file is part of the GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* Header files referred to below can be found in Microsoft's PDB
   repository: https://github.com/microsoft/microsoft-pdb.  */

#ifndef PDB_H
#define PDB_H

#include "sysdep.h"
#include "bfd.h"
#include <stdbool.h>

#define S_PUB32				0x110e

/* PDBStream70 in pdb1.h */
struct pdb_stream_70
{
  uint32_t version;
  uint32_t signature;
  uint32_t age;
  uint8_t guid[16];
};

#define PDB_STREAM_VERSION_VC70		20000404
#define PDB_STREAM_VERSION_VC140	20140508

/* HDR in tpi.h */
struct pdb_tpi_stream_header
{
  uint32_t version;
  uint32_t header_size;
  uint32_t type_index_begin;
  uint32_t type_index_end;
  uint32_t type_record_bytes;
  uint16_t hash_stream_index;
  uint16_t hash_aux_stream_index;
  uint32_t hash_key_size;
  uint32_t num_hash_buckets;
  uint32_t hash_value_buffer_offset;
  uint32_t hash_value_buffer_length;
  uint32_t index_offset_buffer_offset;
  uint32_t index_offset_buffer_length;
  uint32_t hash_adj_buffer_offset;
  uint32_t hash_adj_buffer_length;
};

#define TPI_STREAM_VERSION_80		20040203

#define TPI_FIRST_INDEX			0x1000

/* NewDBIHdr in dbi.h */
struct pdb_dbi_stream_header
{
  uint32_t version_signature;
  uint32_t version_header;
  uint32_t age;
  uint16_t global_stream_index;
  uint16_t build_number;
  uint16_t public_stream_index;
  uint16_t pdb_dll_version;
  uint16_t sym_record_stream;
  uint16_t pdb_dll_rbld;
  uint32_t mod_info_size;
  uint32_t section_contribution_size;
  uint32_t section_map_size;
  uint32_t source_info_size;
  uint32_t type_server_map_size;
  uint32_t mfc_type_server_index;
  uint32_t optional_dbg_header_size;
  uint32_t ec_substream_size;
  uint16_t flags;
  uint16_t machine;
  uint32_t padding;
};

#define DBI_STREAM_VERSION_70		19990903

/* PSGSIHDR in gsi.h */
struct publics_header
{
  uint32_t sym_hash_size;
  uint32_t addr_map_size;
  uint32_t num_thunks;
  uint32_t thunks_size;
  uint32_t thunk_table;
  uint32_t thunk_table_offset;
  uint32_t num_sects;
};

/* GSIHashHdr in gsi.h */
struct globals_hash_header
{
  uint32_t signature;
  uint32_t version;
  uint32_t entries_size;
  uint32_t buckets_size;
};

/* HRFile in gsi.h */
struct hash_record
{
  uint32_t offset;
  uint32_t reference;
};

#define GLOBALS_HASH_SIGNATURE		0xffffffff
#define GLOBALS_HASH_VERSION_70		0xf12f091a

/* PUBSYM32 in cvinfo.h */
struct pubsym
{
  uint16_t record_length;
  uint16_t record_type;
  uint32_t flags;
  uint32_t offset;
  uint16_t section;
  /* followed by null-terminated string */
} ATTRIBUTE_PACKED;

/* see bitset CV_PUBSYMFLAGS in cvinfo.h */
#define PUBSYM_FUNCTION			0x2

struct optional_dbg_header
{
  uint16_t fpo_stream;
  uint16_t exception_stream;
  uint16_t fixup_stream;
  uint16_t omap_to_src_stream;
  uint16_t omap_from_src_stream;
  uint16_t section_header_stream;
  uint16_t token_map_stream;
  uint16_t xdata_stream;
  uint16_t pdata_stream;
  uint16_t new_fpo_stream;
  uint16_t orig_section_header_stream;
};

#define CV_SIGNATURE_C13		4

/* SC in dbicommon.h */
struct section_contribution
{
  uint16_t section;
  uint16_t padding1;
  uint32_t offset;
  uint32_t size;
  uint32_t characteristics;
  uint16_t module_index;
  uint16_t padding2;
  uint32_t data_crc;
  uint32_t reloc_crc;
};

/* MODI_60_Persist in dbi.h */
struct module_info
{
  uint32_t unused1;
  struct section_contribution sc;
  uint16_t flags;
  uint16_t module_sym_stream;
  uint32_t sym_byte_size;
  uint32_t c11_byte_size;
  uint32_t c13_byte_size;
  uint16_t source_file_count;
  uint16_t padding;
  uint32_t unused2;
  uint32_t source_file_name_index;
  uint32_t pdb_file_path_name_index;
};

extern bool create_pdb_file (bfd *, const char *, const unsigned char *);

#endif
