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

/* Return the index of a given output section.  */
static uint16_t
find_section_number (bfd *abfd, asection *sect)
{
  uint16_t i = 1;

  for (asection *s = abfd->sections; s; s = s->next)
    {
      if (s == sect)
	return i;

      /* Empty sections aren't output.  */
      if (s->size != 0)
	i++;
    }

  return 0;
}

/* Stream 4 is the debug information (DBI) stream.  */
static bool
populate_dbi_stream (bfd *stream, bfd *abfd,
		     uint16_t section_header_stream_num,
		     uint16_t sym_rec_stream_num,
		     uint16_t publics_stream_num)
{
  struct pdb_dbi_stream_header h;
  struct optional_dbg_header opt;

  bfd_putl32 (0xffffffff, &h.version_signature);
  bfd_putl32 (DBI_STREAM_VERSION_70, &h.version_header);
  bfd_putl32 (1, &h.age);
  bfd_putl16 (0xffff, &h.global_stream_index);
  bfd_putl16 (0x8e1d, &h.build_number); // MSVC 14.29
  bfd_putl16 (publics_stream_num, &h.public_stream_index);
  bfd_putl16 (0, &h.pdb_dll_version);
  bfd_putl16 (sym_rec_stream_num, &h.sym_record_stream);
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
  bfd_putl16 (section_header_stream_num, &opt.section_header_stream);
  bfd_putl16 (0xffff, &opt.token_map_stream);
  bfd_putl16 (0xffff, &opt.xdata_stream);
  bfd_putl16 (0xffff, &opt.pdata_stream);
  bfd_putl16 (0xffff, &opt.new_fpo_stream);
  bfd_putl16 (0xffff, &opt.orig_section_header_stream);

  if (bfd_bwrite (&opt, sizeof (opt), stream) != sizeof (opt))
    return false;

  return true;
}

/* Used as parameter to qsort, to sort publics by hash.  */
static int
public_compare_hash (const void *s1, const void *s2)
{
  const struct public *p1 = *(const struct public **) s1;
  const struct public *p2 = *(const struct public **) s2;

  if (p1->hash < p2->hash)
    return -1;
  if (p1->hash > p2->hash)
    return 1;

  return 0;
}

/* Used as parameter to qsort, to sort publics by address.  */
static int
public_compare_addr (const void *s1, const void *s2)
{
  const struct public *p1 = *(const struct public **) s1;
  const struct public *p2 = *(const struct public **) s2;

  if (p1->section < p2->section)
    return -1;
  if (p1->section > p2->section)
    return 1;

  if (p1->address < p2->address)
    return -1;
  if (p1->address > p2->address)
    return 1;

  return 0;
}

/* The publics stream is a hash map of S_PUB32 records, which are stored
   in the symbol record stream.  Each S_PUB32 entry represents a symbol
   from the point of view of the linker: a section index, an offset within
   the section, and a mangled name.  Compare with S_GDATA32 and S_GPROC32,
   which are the same thing but generated by the compiler.  */
static bool
populate_publics_stream (bfd *stream, bfd *abfd, bfd *sym_rec_stream)
{
  struct publics_header header;
  struct globals_hash_header hash_header;
  const unsigned int num_buckets = 4096;
  unsigned int num_entries = 0, filled_buckets = 0;
  unsigned int buckets_size, sym_hash_size;
  char int_buf[sizeof (uint32_t)];
  struct public *publics_head = NULL, *publics_tail = NULL;
  struct public **buckets;
  struct public **sorted = NULL;
  bool ret = false;

  buckets = xmalloc (sizeof (struct public *) * num_buckets);
  memset (buckets, 0, sizeof (struct public *) * num_buckets);

  /* Loop through the global symbols in our input files, and write S_PUB32
     records in the symbol record stream for those that make it into the
     final image.  */
  for (bfd *in = coff_data (abfd)->link_info->input_bfds; in;
       in = in->link.next)
    {
      for (unsigned int i = 0; i < in->symcount; i++)
	{
	  struct bfd_symbol *sym = in->outsymbols[i];

	  if (sym->flags & BSF_GLOBAL)
	    {
	      struct pubsym ps;
	      uint16_t record_length;
	      const char *name = sym->name;
	      size_t name_len = strlen (name);
	      struct public *p = xmalloc (sizeof (struct public));
	      unsigned int padding = 0;
	      uint16_t section;
	      uint32_t flags = 0;

	      section =
		find_section_number (abfd, sym->section->output_section);

	      if (section == 0)
		continue;

	      p->next = NULL;
	      p->offset = bfd_tell (sym_rec_stream);
	      p->hash = calc_hash (name, name_len) % num_buckets;
	      p->section = section;
	      p->address = sym->section->output_offset + sym->value;

	      record_length = sizeof (struct pubsym) + name_len + 1;

	      if (record_length % 4)
		padding = 4 - (record_length % 4);

	      /* Assume that all global symbols in executable sections
		 are functions.  */
	      if (sym->section->flags & SEC_CODE)
		flags = PUBSYM_FUNCTION;

	      bfd_putl16 (record_length + padding - sizeof (uint16_t),
			  &ps.record_length);
	      bfd_putl16 (S_PUB32, &ps.record_type);
	      bfd_putl32 (flags, &ps.flags);
	      bfd_putl32 (p->address, &ps.offset);
	      bfd_putl16 (p->section, &ps.section);

	      if (bfd_bwrite (&ps, sizeof (struct pubsym), sym_rec_stream) !=
		  sizeof (struct pubsym))
		goto end;

	      if (bfd_bwrite (name, name_len + 1, sym_rec_stream) !=
		  name_len + 1)
		goto end;

	      for (unsigned int j = 0; j < padding; j++)
		{
		  uint8_t b = 0;

		  if (bfd_bwrite (&b, sizeof (uint8_t), sym_rec_stream) !=
		      sizeof (uint8_t))
		    goto end;
		}

	      if (!publics_head)
		publics_head = p;
	      else
		publics_tail->next = p;

	      publics_tail = p;
	      num_entries++;
	    }
	}
    }


  if (num_entries > 0)
    {
      /* Create an array of pointers, sorted by hash value.  */

      sorted = xmalloc (sizeof (struct public *) * num_entries);

      struct public *p = publics_head;
      for (unsigned int i = 0; i < num_entries; i++)
	{
	  sorted[i] = p;
	  p = p->next;
	}

      qsort (sorted, num_entries, sizeof (struct public *),
	     public_compare_hash);

      /* Populate the buckets.  */

      for (unsigned int i = 0; i < num_entries; i++)
	{
	  if (!buckets[sorted[i]->hash])
	    {
	      buckets[sorted[i]->hash] = sorted[i];
	      filled_buckets++;
	    }

	  sorted[i]->index = i;
	}
    }

  buckets_size = num_buckets / 8;
  buckets_size += sizeof (uint32_t);
  buckets_size += filled_buckets * sizeof (uint32_t);

  sym_hash_size = sizeof (hash_header);
  sym_hash_size += num_entries * sizeof (struct hash_record);
  sym_hash_size += buckets_size;

  /* Output the publics header.  */

  bfd_putl32 (sym_hash_size, &header.sym_hash_size);
  bfd_putl32 (num_entries * sizeof (uint32_t), &header.addr_map_size);
  bfd_putl32 (0, &header.num_thunks);
  bfd_putl32 (0, &header.thunks_size);
  bfd_putl32 (0, &header.thunk_table);
  bfd_putl32 (0, &header.thunk_table_offset);
  bfd_putl32 (0, &header.num_sects);

  if (bfd_bwrite (&header, sizeof (header), stream) != sizeof (header))
    goto end;

  /* Output the global hash header.  */

  bfd_putl32 (GLOBALS_HASH_SIGNATURE, &hash_header.signature);
  bfd_putl32 (GLOBALS_HASH_VERSION_70, &hash_header.version);
  bfd_putl32 (num_entries * sizeof (struct hash_record),
	      &hash_header.entries_size);
  bfd_putl32 (buckets_size, &hash_header.buckets_size);

  if (bfd_bwrite (&hash_header, sizeof (hash_header), stream) !=
      sizeof (hash_header))
    goto end;

  /* Write the entries in hash order.  */

  for (unsigned int i = 0; i < num_entries; i++)
    {
      struct hash_record hr;

      bfd_putl32 (sorted[i]->offset + 1, &hr.offset);
      bfd_putl32 (1, &hr.reference);

      if (bfd_bwrite (&hr, sizeof (hr), stream) != sizeof (hr))
	goto end;
    }

  /* Write the bitmap for filled and unfilled buckets.  */

  for (unsigned int i = 0; i < num_buckets; i += 8)
    {
      uint8_t v = 0;

      for (unsigned int j = 0; j < 8; j++)
	{
	  if (buckets[i + j])
	    v |= 1 << j;
	}

      if (bfd_bwrite (&v, sizeof (v), stream) != sizeof (v))
	goto end;
    }

  /* Add a 4-byte gap.  */

  bfd_putl32 (0, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), stream) != sizeof (uint32_t))
    goto end;

  /* Write the bucket offsets.  */

  for (unsigned int i = 0; i < num_buckets; i++)
    {
      if (buckets[i])
	{
	  /* 0xc is size of internal hash_record structure in
	     Microsoft's parser.  */
	  bfd_putl32 (buckets[i]->index * 0xc, int_buf);

	  if (bfd_bwrite (int_buf, sizeof (uint32_t), stream) !=
	      sizeof (uint32_t))
	    goto end;
	}
    }

  /* Write the address map: offsets into the symbol record stream of
     S_PUB32 records, ordered by address.  */

  if (num_entries > 0)
    {
      qsort (sorted, num_entries, sizeof (struct public *),
	     public_compare_addr);

      for (unsigned int i = 0; i < num_entries; i++)
	{
	  bfd_putl32 (sorted[i]->offset, int_buf);

	  if (bfd_bwrite (int_buf, sizeof (uint32_t), stream) !=
	      sizeof (uint32_t))
	    goto end;
	}
    }

  ret = true;

end:
  free (buckets);

  while (publics_head)
    {
      struct public *p = publics_head->next;

      free (publics_head);
      publics_head = p;
    }

  free (sorted);

  return ret;
}

/* The section header stream contains a copy of the section headers
   from the PE file, in the same format.  */
static bool
create_section_header_stream (bfd *pdb, bfd *abfd, uint16_t *num)
{
  bfd *stream;
  unsigned int section_count;
  file_ptr scn_base;
  size_t len;
  char *buf;

  stream = add_stream (pdb, NULL, num);
  if (!stream)
    return false;

  section_count = abfd->section_count;

  /* Empty sections aren't output.  */
  for (asection *sect = abfd->sections; sect; sect = sect->next)
    {
      if (sect->size == 0)
	section_count--;
    }

  if (section_count == 0)
    return true;

  /* Copy section table from output - it's already been written at this
     point.  */

  scn_base = bfd_coff_filhsz (abfd) + bfd_coff_aoutsz (abfd);

  bfd_seek (abfd, scn_base, SEEK_SET);

  len = section_count * sizeof (struct external_scnhdr);
  buf = xmalloc (len);

  if (bfd_bread (buf, len, abfd) != len)
    {
      free (buf);
      return false;
    }

  if (bfd_bwrite (buf, len, stream) != len)
    {
      free (buf);
      return false;
    }

  free (buf);

  return true;
}

/* Create a PDB debugging file for the PE image file abfd with the build ID
   guid, stored at pdb_name.  */
bool
create_pdb_file (bfd *abfd, const char *pdb_name, const unsigned char *guid)
{
  bfd *pdb;
  bool ret = false;
  bfd *info_stream, *dbi_stream, *names_stream, *sym_rec_stream,
    *publics_stream;
  uint16_t section_header_stream_num, sym_rec_stream_num, publics_stream_num;

  pdb = bfd_openw (pdb_name, "pdb");
  if (!pdb)
    {
      einfo (_("%P: warning: cannot create PDB file: %E\n"));
      return false;
    }

  bfd_set_format (pdb, bfd_archive);

  if (!create_old_directory_stream (pdb))
    {
      einfo (_("%P: warning: cannot create old directory stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  info_stream = add_stream (pdb, NULL, NULL);

  if (!info_stream)
    {
      einfo (_("%P: warning: cannot create info stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  if (!create_type_stream (pdb))
    {
      einfo (_("%P: warning: cannot create TPI stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  dbi_stream = add_stream (pdb, NULL, NULL);

  if (!dbi_stream)
    {
      einfo (_("%P: warning: cannot create DBI stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  if (!create_type_stream (pdb))
    {
      einfo (_("%P: warning: cannot create IPI stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  names_stream = add_stream (pdb, "/names", NULL);

  if (!names_stream)
    {
      einfo (_("%P: warning: cannot create /names stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  sym_rec_stream = add_stream (pdb, NULL, &sym_rec_stream_num);

  if (!sym_rec_stream)
    {
      einfo (_("%P: warning: cannot create symbol record stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  publics_stream = add_stream (pdb, NULL, &publics_stream_num);

  if (!publics_stream)
    {
      einfo (_("%P: warning: cannot create publics stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  if (!create_section_header_stream (pdb, abfd, &section_header_stream_num))
    {
      einfo (_("%P: warning: cannot create section header stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  if (!populate_dbi_stream (dbi_stream, abfd, section_header_stream_num,
			    sym_rec_stream_num, publics_stream_num))
    {
      einfo (_("%P: warning: cannot populate DBI stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  if (!populate_publics_stream (publics_stream, abfd, sym_rec_stream))
    {
      einfo (_("%P: warning: cannot populate publics stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  if (!populate_info_stream (pdb, info_stream, guid))
    {
      einfo (_("%P: warning: cannot populate info stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  ret = true;

end:
  bfd_close (pdb);

  return ret;
}
