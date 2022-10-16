/* Support for generating PDB CodeView debugging files.
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

#include "pdb.h"
#include "bfdlink.h"
#include "ld.h"
#include "ldmisc.h"
#include "libbfd.h"
#include "libiberty.h"
#include "coff/i386.h"
#include "coff/external.h"
#include "coff/internal.h"
#include "coff/pe.h"
#include "libcoff.h"
#include <time.h>

struct public
{
  struct public *next;
  uint32_t offset;
  uint32_t hash;
  unsigned int index;
  uint16_t section;
  uint32_t address;
};

/* Add a new stream to the PDB archive, and return its BFD.  */
static bfd *
add_stream (bfd *pdb, const char *name, uint16_t *stream_num)
{
  bfd *stream;
  uint16_t num;

  stream = bfd_create (name ? name : "", pdb);
  if (!stream)
    return NULL;

  if (!bfd_make_writable (stream))
    {
      bfd_close (stream);
      return false;
    }

  if (!pdb->archive_head)
    {
      bfd_set_archive_head (pdb, stream);
      num = 0;
    }
  else
    {
      bfd *b = pdb->archive_head;

      num = 1;

      while (b->archive_next)
	{
	  num++;
	  b = b->archive_next;
	}

      b->archive_next = stream;
    }

  if (stream_num)
    *stream_num = num;

  return stream;
}

/* Stream 0 ought to be a copy of the MSF directory from the last
   time the PDB file was written.  Because we don't do incremental
   writes this isn't applicable to us, but we fill it with a dummy
   value so as not to confuse radare.  */
static bool
create_old_directory_stream (bfd *pdb)
{
  bfd *stream;
  char buf[sizeof (uint32_t)];

  stream = add_stream (pdb, NULL, NULL);
  if (!stream)
    return false;

  bfd_putl32 (0, buf);

  return bfd_bwrite (buf, sizeof (uint32_t), stream) == sizeof (uint32_t);
}

/* Calculate the hash of a given string.  */
static uint32_t
calc_hash (const char *data, size_t len)
{
  uint32_t hash = 0;

  while (len >= 4)
    {
      hash ^= data[0];
      hash ^= data[1] << 8;
      hash ^= data[2] << 16;
      hash ^= data[3] << 24;

      data += 4;
      len -= 4;
    }

  if (len >= 2)
    {
      hash ^= data[0];
      hash ^= data[1] << 8;

      data += 2;
      len -= 2;
    }

  if (len != 0)
    hash ^= *data;

  hash |= 0x20202020;
  hash ^= (hash >> 11);

  return hash ^ (hash >> 16);
}

/* Stream 1 is the PDB info stream - see
   https://llvm.org/docs/PDB/PdbStream.html.  */
static bool
populate_info_stream (bfd *pdb, bfd *info_stream, const unsigned char *guid)
{
  bool ret = false;
  struct pdb_stream_70 h;
  uint32_t num_entries, num_buckets;
  uint32_t names_length, stream_num;
  char int_buf[sizeof (uint32_t)];

  struct hash_entry
  {
    uint32_t offset;
    uint32_t value;
  };

  struct hash_entry **buckets = NULL;

  /* Write header.  */

  bfd_putl32 (PDB_STREAM_VERSION_VC70, &h.version);
  bfd_putl32 (time (NULL), &h.signature);
  bfd_putl32 (1, &h.age);

  bfd_putl32 (bfd_getb32 (guid), h.guid);
  bfd_putl16 (bfd_getb16 (&guid[4]), &h.guid[4]);
  bfd_putl16 (bfd_getb16 (&guid[6]), &h.guid[6]);
  memcpy (&h.guid[8], &guid[8], 8);

  if (bfd_bwrite (&h, sizeof (h), info_stream) != sizeof (h))
    return false;

  /* Write hash list of named streams.  This is a "rollover" hash, i.e.
     if a bucket is filled an entry gets placed in the next free
     slot.  */

  num_entries = 0;
  for (bfd *b = pdb->archive_head; b; b = b->archive_next)
    {
      if (strcmp (b->filename, ""))
	num_entries++;
    }

  num_buckets = num_entries * 2;

  names_length = 0;
  stream_num = 0;

  if (num_buckets > 0)
    {
      buckets = xmalloc (sizeof (struct hash_entry *) * num_buckets);
      memset (buckets, 0, sizeof (struct hash_entry *) * num_buckets);

      for (bfd *b = pdb->archive_head; b; b = b->archive_next)
	{
	  if (strcmp (b->filename, ""))
	    {
	      size_t len = strlen (b->filename);
	      uint32_t hash = (uint16_t) calc_hash (b->filename, len);
	      uint32_t bucket_num = hash % num_buckets;

	      while (buckets[bucket_num])
		{
		  bucket_num++;

		  if (bucket_num == num_buckets)
		    bucket_num = 0;
		}

	      buckets[bucket_num] = xmalloc (sizeof (struct hash_entry));

	      buckets[bucket_num]->offset = names_length;
	      buckets[bucket_num]->value = stream_num;

	      names_length += len + 1;
	    }

	  stream_num++;
	}
    }

  /* Write the strings list - the hash keys are indexes into this.  */

  bfd_putl32 (names_length, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), info_stream) !=
      sizeof (uint32_t))
    goto end;

  for (bfd *b = pdb->archive_head; b; b = b->archive_next)
    {
      if (!strcmp (b->filename, ""))
	continue;

      size_t len = strlen (b->filename) + 1;

      if (bfd_bwrite (b->filename, len, info_stream) != len)
	goto end;
    }

  /* Write the number of entries and buckets.  */

  bfd_putl32 (num_entries, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), info_stream) !=
      sizeof (uint32_t))
    goto end;

  bfd_putl32 (num_buckets, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), info_stream) !=
      sizeof (uint32_t))
    goto end;

  /* Write the present bitmap.  */

  bfd_putl32 ((num_buckets + 31) / 32, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), info_stream) !=
      sizeof (uint32_t))
    goto end;

  for (unsigned int i = 0; i < num_buckets; i += 32)
    {
      uint32_t v = 0;

      for (unsigned int j = 0; j < 32; j++)
	{
	  if (i + j >= num_buckets)
	    break;

	  if (buckets[i + j])
	    v |= 1 << j;
	}

      bfd_putl32 (v, int_buf);

      if (bfd_bwrite (int_buf, sizeof (uint32_t), info_stream) !=
	  sizeof (uint32_t))
	goto end;
    }

  /* Write the (empty) deleted bitmap.  */

  bfd_putl32 (0, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), info_stream) !=
      sizeof (uint32_t))
    goto end;

  /* Write the buckets.  */

  for (unsigned int i = 0; i < num_buckets; i++)
    {
      if (buckets[i])
	{
	  bfd_putl32 (buckets[i]->offset, int_buf);

	  if (bfd_bwrite (int_buf, sizeof (uint32_t), info_stream) !=
	      sizeof (uint32_t))
	    goto end;

	  bfd_putl32 (buckets[i]->value, int_buf);

	  if (bfd_bwrite (int_buf, sizeof (uint32_t), info_stream) !=
	      sizeof (uint32_t))
	    goto end;
	}
    }

  bfd_putl32 (0, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), info_stream) !=
      sizeof (uint32_t))
    goto end;

  bfd_putl32 (PDB_STREAM_VERSION_VC140, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), info_stream) !=
      sizeof (uint32_t))
    goto end;

  ret = true;

end:
  for (unsigned int i = 0; i < num_buckets; i++)
    {
      if (buckets[i])
	free (buckets[i]);
    }

  free (buckets);

  return ret;
}

/* Stream 2 is the type information (TPI) stream, and stream 4 is
   the ID information (IPI) stream.  They differ only in which records
   go in which stream. */
static bool
create_type_stream (bfd *pdb)
{
  bfd *stream;
  struct pdb_tpi_stream_header h;

  stream = add_stream (pdb, NULL, NULL);
  if (!stream)
    return false;

  bfd_putl32 (TPI_STREAM_VERSION_80, &h.version);
  bfd_putl32 (sizeof (h), &h.header_size);
  bfd_putl32 (TPI_FIRST_INDEX, &h.type_index_begin);
  bfd_putl32 (TPI_FIRST_INDEX, &h.type_index_end);
  bfd_putl32 (0, &h.type_record_bytes);
  bfd_putl16 (0xffff, &h.hash_stream_index);
  bfd_putl16 (0xffff, &h.hash_aux_stream_index);
  bfd_putl32 (4, &h.hash_key_size);
  bfd_putl32 (0x3ffff, &h.num_hash_buckets);
  bfd_putl32 (0, &h.hash_value_buffer_offset);
  bfd_putl32 (0, &h.hash_value_buffer_length);
  bfd_putl32 (0, &h.index_offset_buffer_offset);
  bfd_putl32 (0, &h.index_offset_buffer_length);
  bfd_putl32 (0, &h.hash_adj_buffer_offset);
  bfd_putl32 (0, &h.hash_adj_buffer_length);

  if (bfd_bwrite (&h, sizeof (h), stream) != sizeof (h))
    return false;

  return true;
}

/* Return the PE architecture number for the image.  */
static uint16_t
get_arch_number (bfd *abfd)
{
  if (abfd->arch_info->arch != bfd_arch_i386)
    return 0;

  if (abfd->arch_info->mach & bfd_mach_x86_64)
    return IMAGE_FILE_MACHINE_AMD64;

  return IMAGE_FILE_MACHINE_I386;
}

/* Stream 4 is the debug information (DBI) stream.  */
static bool
populate_dbi_stream (bfd *stream, bfd *abfd)
{
  struct pdb_dbi_stream_header h;
  struct optional_dbg_header opt;

  bfd_putl32 (0xffffffff, &h.version_signature);
  bfd_putl32 (DBI_STREAM_VERSION_70, &h.version_header);
  bfd_putl32 (1, &h.age);
  bfd_putl16 (0xffff, &h.global_stream_index);
  bfd_putl16 (0x8e1d, &h.build_number); // MSVC 14.29
  bfd_putl16 (0xffff, &h.public_stream_index);
  bfd_putl16 (0, &h.pdb_dll_version);
  bfd_putl16 (0xffff, &h.sym_record_stream);
  bfd_putl16 (0, &h.pdb_dll_rbld);
  bfd_putl32 (0, &h.mod_info_size);
  bfd_putl32 (0, &h.section_contribution_size);
  bfd_putl32 (0, &h.section_map_size);
  bfd_putl32 (0, &h.source_info_size);
  bfd_putl32 (0, &h.type_server_map_size);
  bfd_putl32 (0, &h.mfc_type_server_index);
  bfd_putl32 (sizeof (opt), &h.optional_dbg_header_size);
  bfd_putl32 (0, &h.ec_substream_size);
  bfd_putl16 (0, &h.flags);
  bfd_putl16 (get_arch_number (abfd), &h.machine);
  bfd_putl32 (0, &h.padding);

  if (bfd_bwrite (&h, sizeof (h), stream) != sizeof (h))
    return false;

  bfd_putl16 (0xffff, &opt.fpo_stream);
  bfd_putl16 (0xffff, &opt.exception_stream);
  bfd_putl16 (0xffff, &opt.fixup_stream);
  bfd_putl16 (0xffff, &opt.omap_to_src_stream);
  bfd_putl16 (0xffff, &opt.omap_from_src_stream);
  bfd_putl16 (0xffff, &opt.section_header_stream);
  bfd_putl16 (0xffff, &opt.token_map_stream);
  bfd_putl16 (0xffff, &opt.xdata_stream);
  bfd_putl16 (0xffff, &opt.pdata_stream);
  bfd_putl16 (0xffff, &opt.new_fpo_stream);
  bfd_putl16 (0xffff, &opt.orig_section_header_stream);

  if (bfd_bwrite (&opt, sizeof (opt), stream) != sizeof (opt))
    return false;

  return true;
}

/* Create a PDB debugging file for the PE image file abfd with the build ID
   guid, stored at pdb_name.  */
bool
create_pdb_file (bfd *abfd, const char *pdb_name, const unsigned char *guid)
{
  bfd *pdb;
  bool ret = false;
  bfd *info_stream, *dbi_stream, *names_stream;

  pdb = bfd_openw (pdb_name, "pdb");
  if (!pdb)
    {
      einfo (_("%P: warning: cannot create PDB file: %s\n"),
	     bfd_errmsg (bfd_get_error ()));
      return false;
    }

  bfd_set_format (pdb, bfd_archive);

  if (!create_old_directory_stream (pdb))
    {
      einfo (_("%P: warning: cannot create old directory stream "
	       "in PDB file: %s\n"), bfd_errmsg (bfd_get_error ()));
      goto end;
    }

  info_stream = add_stream (pdb, NULL, NULL);

  if (!info_stream)
    {
      einfo (_("%P: warning: cannot create info stream "
	       "in PDB file: %s\n"), bfd_errmsg (bfd_get_error ()));
      goto end;
    }

  if (!create_type_stream (pdb))
    {
      einfo (_("%P: warning: cannot create TPI stream "
	       "in PDB file: %s\n"), bfd_errmsg (bfd_get_error ()));
      goto end;
    }

  dbi_stream = add_stream (pdb, NULL, NULL);

  if (!dbi_stream)
    {
      einfo (_("%P: warning: cannot create DBI stream "
	       "in PDB file: %s\n"), bfd_errmsg (bfd_get_error ()));
      goto end;
    }

  if (!create_type_stream (pdb))
    {
      einfo (_("%P: warning: cannot create IPI stream "
	       "in PDB file: %s\n"), bfd_errmsg (bfd_get_error ()));
      goto end;
    }

  names_stream = add_stream (pdb, "/names", NULL);

  if (!names_stream)
    {
      einfo (_("%P: warning: cannot create /names stream "
	       "in PDB file: %s\n"), bfd_errmsg (bfd_get_error ()));
      goto end;
    }

  if (!populate_dbi_stream (dbi_stream, abfd))
    {
      einfo (_("%P: warning: cannot populate DBI stream "
	       "in PDB file: %s\n"), bfd_errmsg (bfd_get_error ()));
      goto end;
    }

  if (!populate_info_stream (pdb, info_stream, guid))
    {
      einfo (_("%P: warning: cannot populate info stream "
	       "in PDB file: %s\n"), bfd_errmsg (bfd_get_error ()));
      goto end;
    }

  ret = true;

end:
  bfd_close (pdb);

  return ret;
}
