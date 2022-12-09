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

struct string
{
  struct string *next;
  uint32_t hash;
  uint32_t offset;
  uint32_t source_file_offset;
  size_t len;
  char s[];
};

struct string_table
{
  struct string *strings_head;
  struct string *strings_tail;
  uint32_t strings_len;
  htab_t hashmap;
};

struct mod_source_files
{
  uint16_t files_count;
  struct string **files;
};

struct source_files_info
{
  uint16_t mod_count;
  struct mod_source_files *mods;
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

/* Validate the DEBUG_S_FILECHKSMS entry within a module's .debug$S
   section, and copy it to the module's symbol stream.  */
static bool
copy_filechksms (uint8_t *data, uint32_t size, char *string_table,
		 struct string_table *strings, uint8_t *out,
		 struct mod_source_files *mod_source)
{
  uint8_t *orig_data = data;
  uint32_t orig_size = size;
  uint16_t num_files = 0;
  struct string **strptr;

  bfd_putl32 (DEBUG_S_FILECHKSMS, out);
  out += sizeof (uint32_t);

  bfd_putl32 (size, out);
  out += sizeof (uint32_t);

  /* Calculate the number of files, and check for any overflows.  */

  while (size > 0)
    {
      struct file_checksum *fc = (struct file_checksum *) data;
      uint8_t padding;
      size_t len;

      if (size < sizeof (struct file_checksum))
	{
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}

      len = sizeof (struct file_checksum) + fc->checksum_length;

      if (size < len)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}

      data += len;
      size -= len;

      if (len % sizeof (uint32_t))
	padding = sizeof (uint32_t) - (len % sizeof (uint32_t));
      else
	padding = 0;

      if (size < padding)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}

      num_files++;

      data += padding;
      size -= padding;
    }

  /* Add the files to mod_source, so that they'll appear in the source
     info substream.  */

  strptr = NULL;
  if (num_files > 0)
    {
      uint16_t new_count = num_files + mod_source->files_count;

      mod_source->files = xrealloc (mod_source->files,
				    sizeof (struct string *) * new_count);

      strptr = mod_source->files + mod_source->files_count;

      mod_source->files_count += num_files;
    }

  /* Actually copy the data.  */

  data = orig_data;
  size = orig_size;

  while (size > 0)
    {
      struct file_checksum *fc = (struct file_checksum *) data;
      uint32_t string_off;
      uint8_t padding;
      size_t len;
      struct string *str = NULL;

      string_off = bfd_getl32 (&fc->file_id);
      len = sizeof (struct file_checksum) + fc->checksum_length;

      if (len % sizeof (uint32_t))
	padding = sizeof (uint32_t) - (len % sizeof (uint32_t));
      else
	padding = 0;

      /* Remap the "file ID", i.e. the offset in the module's string table,
	 so it points to the right place in the main string table.  */

      if (string_table)
	{
	  char *fn = string_table + string_off;
	  size_t fn_len = strlen (fn);
	  uint32_t hash = calc_hash (fn, fn_len);
	  void **slot;

	  slot = htab_find_slot_with_hash (strings->hashmap, fn, hash,
					   NO_INSERT);

	  if (slot)
	    str = (struct string *) *slot;
	}

      *strptr = str;
      strptr++;

      bfd_putl32 (str ? str->offset : 0, &fc->file_id);

      memcpy (out, data, len + padding);

      data += len + padding;
      size -= len + padding;
      out += len + padding;
    }

  return true;
}

/* Add a string to the strings table, if it's not already there.  */
static void
add_string (char *str, size_t len, struct string_table *strings)
{
  uint32_t hash = calc_hash (str, len);
  void **slot;

  slot = htab_find_slot_with_hash (strings->hashmap, str, hash, INSERT);

  if (!*slot)
    {
      struct string *s;

      *slot = xmalloc (offsetof (struct string, s) + len);

      s = (struct string *) *slot;

      s->next = NULL;
      s->hash = hash;
      s->offset = strings->strings_len;
      s->source_file_offset = 0xffffffff;
      s->len = len;
      memcpy (s->s, str, len);

      if (strings->strings_tail)
	strings->strings_tail->next = s;
      else
	strings->strings_head = s;

      strings->strings_tail = s;

      strings->strings_len += len + 1;
    }
}

/* Return the hash of an entry in the string table.  */
static hashval_t
hash_string_table_entry (const void *p)
{
  const struct string *s = (const struct string *) p;

  return s->hash;
}

/* Compare an entry in the string table with a string.  */
static int
eq_string_table_entry (const void *a, const void *b)
{
  const struct string *s1 = (const struct string *) a;
  const char *s2 = (const char *) b;
  size_t s2_len = strlen (s2);

  if (s2_len != s1->len)
    return 0;

  return memcmp (s1->s, s2, s2_len) == 0;
}

/* Parse the string table within the .debug$S section.  */
static void
parse_string_table (bfd_byte *data, size_t size,
		    struct string_table *strings)
{
  while (true)
    {
      size_t len = strnlen ((char *) data, size);

      add_string ((char *) data, len, strings);

      data += len + 1;

      if (size <= len + 1)
	break;

      size -= len + 1;
    }
}

/* Parse the .debug$S section within an object file.  */
static bool
handle_debugs_section (asection *s, bfd *mod, struct string_table *strings,
		       uint8_t **dataptr, uint32_t *sizeptr,
		       struct mod_source_files *mod_source,
		       bfd *abfd)
{
  bfd_byte *data = NULL;
  size_t off;
  uint32_t c13_size = 0;
  char *string_table = NULL;
  uint8_t *buf, *bufptr;

  if (!bfd_get_full_section_contents (mod, s, &data))
    return false;

  if (!data)
    return false;

  /* Resolve relocations.  Addresses are stored within the .debug$S section as
     a .secidx, .secrel32 pair.  */

  if (s->flags & SEC_RELOC)
    {
      struct internal_reloc *relocs;
      struct internal_syment *symbols;
      asection **sectlist;
      unsigned int syment_count;
      int sect_num;
      struct external_syment *ext;

      syment_count = obj_raw_syment_count (mod);

      relocs =
	_bfd_coff_read_internal_relocs (mod, s, false, NULL, true, NULL);

      symbols = xmalloc (sizeof (struct internal_syment) * syment_count);
      sectlist = xmalloc (sizeof (asection *) * syment_count);

      ext = (struct external_syment *) (coff_data (mod)->external_syms);

      for (unsigned int i = 0; i < syment_count; i++)
	{
	  bfd_coff_swap_sym_in (mod, &ext[i], &symbols[i]);
	}

      sect_num = 1;

      for (asection *sect = mod->sections; sect; sect = sect->next)
	{
	  for (unsigned int i = 0; i < syment_count; i++)
	    {
	      if (symbols[i].n_scnum == sect_num)
		sectlist[i] = sect;
	    }

	  sect_num++;
	}

      if (!bfd_coff_relocate_section (abfd, coff_data (abfd)->link_info, mod,
				      s, data, relocs, symbols, sectlist))
	{
	  free (sectlist);
	  free (symbols);
	  free (data);
	  return false;
	}

      free (sectlist);
      free (symbols);
    }

  if (bfd_getl32 (data) != CV_SIGNATURE_C13)
    {
      free (data);
      return true;
    }

  off = sizeof (uint32_t);

  /* calculate size */

  while (off + sizeof (uint32_t) <= s->size)
    {
      uint32_t type, size;

      type = bfd_getl32 (data + off);

      off += sizeof (uint32_t);

      if (off + sizeof (uint32_t) > s->size)
	{
	  free (data);
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}

      size = bfd_getl32 (data + off);

      off += sizeof (uint32_t);

      if (off + size > s->size)
	{
	  free (data);
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}

      switch (type)
	{
	case DEBUG_S_FILECHKSMS:
	  c13_size += sizeof (uint32_t) + sizeof (uint32_t) + size;

	  if (c13_size % sizeof (uint32_t))
	    c13_size += sizeof (uint32_t) - (c13_size % sizeof (uint32_t));

	  break;

	case DEBUG_S_STRINGTABLE:
	  parse_string_table (data + off, size, strings);

	  string_table = (char *) data + off;

	  break;

	case DEBUG_S_LINES:
	  {
	    uint16_t sect;

	    if (size < sizeof (uint32_t) + sizeof (uint16_t))
	      {
		free (data);
		bfd_set_error (bfd_error_bad_value);
		return false;
	      }

	    sect = bfd_getl16 (data + off + sizeof (uint32_t));

	    /* Skip GC'd symbols.  */
	    if (sect != 0)
	      {
		c13_size += sizeof (uint32_t) + sizeof (uint32_t) + size;

		if (c13_size % sizeof (uint32_t))
		  c13_size +=
		    sizeof (uint32_t) - (c13_size % sizeof (uint32_t));
	      }

	    break;
	  }
	}

      off += size;

      if (off % sizeof (uint32_t))
	off += sizeof (uint32_t) - (off % sizeof (uint32_t));
    }

  if (c13_size == 0)
    {
      free (data);
      return true;
    }

  /* copy data */

  buf = xmalloc (c13_size);
  bufptr = buf;

  off = sizeof (uint32_t);

  while (off + sizeof (uint32_t) <= s->size)
    {
      uint32_t type, size;

      type = bfd_getl32 (data + off);
      off += sizeof (uint32_t);

      size = bfd_getl32 (data + off);
      off += sizeof (uint32_t);

      switch (type)
	{
	case DEBUG_S_FILECHKSMS:
	  if (!copy_filechksms (data + off, size, string_table,
				strings, bufptr, mod_source))
	    {
	      free (data);
	      return false;
	    }

	  bufptr += sizeof (uint32_t) + sizeof (uint32_t) + size;

	  break;

	case DEBUG_S_LINES:
	  {
	    uint16_t sect;

	    sect = bfd_getl16 (data + off + sizeof (uint32_t));

	    /* Skip if GC'd.  */
	    if (sect != 0)
	      {
		bfd_putl32 (type, bufptr);
		bufptr += sizeof (uint32_t);

		bfd_putl32 (size, bufptr);
		bufptr += sizeof (uint32_t);

		memcpy (bufptr, data + off, size);
		bufptr += size;
	      }

	    break;
	  }
	}

      off += size;

      if (off % sizeof (uint32_t))
	off += sizeof (uint32_t) - (off % sizeof (uint32_t));
    }

  free (data);

  if (*dataptr)
    {
      /* Append the C13 info to what's already there, if the module has
	 multiple .debug$S sections.  */

      *dataptr = xrealloc (*dataptr, *sizeptr + c13_size);
      memcpy (*dataptr + *sizeptr, buf, c13_size);

      free (buf);
    }
  else
    {
      *dataptr = buf;
    }

  *sizeptr += c13_size;

  return true;
}

/* Populate the module stream, which consists of the transformed .debug$S
   data for each object file.  */
static bool
populate_module_stream (bfd *stream, bfd *mod, uint32_t *sym_byte_size,
			struct string_table *strings,
			uint32_t *c13_info_size,
			struct mod_source_files *mod_source,
			bfd *abfd)
{
  uint8_t int_buf[sizeof (uint32_t)];
  uint8_t *c13_info = NULL;

  *sym_byte_size = sizeof (uint32_t);
  *c13_info_size = 0;

  /* Process .debug$S section(s).  */

  for (asection *s = mod->sections; s; s = s->next)
    {
      if (!strcmp (s->name, ".debug$S") && s->size >= sizeof (uint32_t))
	{
	  if (!handle_debugs_section (s, mod, strings, &c13_info,
				      c13_info_size, mod_source, abfd))
	    {
	      free (c13_info);
	      free (mod_source->files);
	      return false;
	    }
	}
    }

  /* Write the signature.  */

  bfd_putl32 (CV_SIGNATURE_C13, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), stream) != sizeof (uint32_t))
    {
      free (c13_info);
      return false;
    }

  if (c13_info)
    {
      if (bfd_bwrite (c13_info, *c13_info_size, stream) != *c13_info_size)
	{
	  free (c13_info);
	  return false;
	}

      free (c13_info);
    }

  /* Write the global refs size.  */

  bfd_putl32 (0, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), stream) != sizeof (uint32_t))
    return false;

  return true;
}

/* Create the module info substream within the DBI.  */
static bool
create_module_info_substream (bfd *abfd, bfd *pdb, void **data,
			      uint32_t *size, struct string_table *strings,
			      struct source_files_info *source)
{
  uint8_t *ptr;
  unsigned int mod_num;

  static const char linker_fn[] = "* Linker *";

  *size = 0;

  for (bfd *in = coff_data (abfd)->link_info->input_bfds; in;
       in = in->link.next)
    {
      size_t len = sizeof (struct module_info);

      if (!strcmp (bfd_get_filename (in), "dll stuff"))
	{
	  len += sizeof (linker_fn); /* Object name.  */
	  len++; /* Empty module name.  */
	}
      else if (in->my_archive)
	{
	  char *name = lrealpath (bfd_get_filename (in));

	  len += strlen (name) + 1; /* Object name.  */

	  free (name);

	  name = lrealpath (bfd_get_filename (in->my_archive));

	  len += strlen (name) + 1; /* Archive name.  */

	  free (name);
	}
      else
	{
	  char *name = lrealpath (bfd_get_filename (in));
	  size_t name_len = strlen (name) + 1;

	  len += name_len; /* Object name.  */
	  len += name_len; /* And again as the archive name.  */

	  free (name);
	}

      if (len % 4)
	len += 4 - (len % 4);

      *size += len;

      source->mod_count++;
    }

  *data = xmalloc (*size);

  ptr = *data;

  source->mods = xmalloc (source->mod_count
			  * sizeof (struct mod_source_files));
  memset (source->mods, 0,
	  source->mod_count * sizeof (struct mod_source_files));

  mod_num = 0;

  for (bfd *in = coff_data (abfd)->link_info->input_bfds; in;
       in = in->link.next)
    {
      struct module_info *mod = (struct module_info *) ptr;
      uint16_t stream_num;
      bfd *stream;
      uint32_t sym_byte_size, c13_info_size;
      uint8_t *start = ptr;

      stream = add_stream (pdb, NULL, &stream_num);

      if (!stream)
	{
	  for (unsigned int i = 0; i < source->mod_count; i++)
	    {
	      free (source->mods[i].files);
	    }

	  free (source->mods);
	  free (*data);
	  return false;
	}

      if (!populate_module_stream (stream, in, &sym_byte_size,
				   strings, &c13_info_size,
				   &source->mods[mod_num], abfd))
	{
	  for (unsigned int i = 0; i < source->mod_count; i++)
	    {
	      free (source->mods[i].files);
	    }

	  free (source->mods);
	  free (*data);
	  return false;
	}

      bfd_putl32 (0, &mod->unused1);

      /* These are dummy values - MSVC copies the first section contribution
	 entry here, but doesn't seem to use it for anything.  */
      bfd_putl16 (0xffff, &mod->sc.section);
      bfd_putl16 (0, &mod->sc.padding1);
      bfd_putl32 (0, &mod->sc.offset);
      bfd_putl32 (0xffffffff, &mod->sc.size);
      bfd_putl32 (0, &mod->sc.characteristics);
      bfd_putl16 (0xffff, &mod->sc.module_index);
      bfd_putl16 (0, &mod->sc.padding2);
      bfd_putl32 (0, &mod->sc.data_crc);
      bfd_putl32 (0, &mod->sc.reloc_crc);

      bfd_putl16 (0, &mod->flags);
      bfd_putl16 (stream_num, &mod->module_sym_stream);
      bfd_putl32 (sym_byte_size, &mod->sym_byte_size);
      bfd_putl32 (0, &mod->c11_byte_size);
      bfd_putl32 (c13_info_size, &mod->c13_byte_size);
      bfd_putl16 (0, &mod->source_file_count);
      bfd_putl16 (0, &mod->padding);
      bfd_putl32 (0, &mod->unused2);
      bfd_putl32 (0, &mod->source_file_name_index);
      bfd_putl32 (0, &mod->pdb_file_path_name_index);

      ptr += sizeof (struct module_info);

      if (!strcmp (bfd_get_filename (in), "dll stuff"))
	{
	  /* Object name.  */
	  memcpy (ptr, linker_fn, sizeof (linker_fn));
	  ptr += sizeof (linker_fn);

	  /* Empty module name.  */
	  *ptr = 0;
	  ptr++;
	}
      else if (in->my_archive)
	{
	  char *name = lrealpath (bfd_get_filename (in));
	  size_t name_len = strlen (name) + 1;

	  /* Object name.  */
	  memcpy (ptr, name, name_len);
	  ptr += name_len;

	  free (name);

	  name = lrealpath (bfd_get_filename (in->my_archive));
	  name_len = strlen (name) + 1;

	  /* Archive name.  */
	  memcpy (ptr, name, name_len);
	  ptr += name_len;

	  free (name);
	}
      else
	{
	  char *name = lrealpath (bfd_get_filename (in));
	  size_t name_len = strlen (name) + 1;

	  /* Object name.  */
	  memcpy (ptr, name, name_len);
	  ptr += name_len;

	  /* Object name again as archive name.  */
	  memcpy (ptr, name, name_len);
	  ptr += name_len;

	  free (name);
	}

      /* Pad to next four-byte boundary.  */

      if ((ptr - start) % 4)
	{
	  memset (ptr, 0, 4 - ((ptr - start) % 4));
	  ptr += 4 - ((ptr - start) % 4);
	}

      mod_num++;
    }

  return true;
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

/* Create the substream which maps addresses in the image file to locations
   in the original object files.  */
static bool
create_section_contrib_substream (bfd *abfd, void **data, uint32_t *size)
{
  unsigned int num_sc = 0;
  struct section_contribution *sc;
  uint16_t mod_index;
  char *sect_flags;
  file_ptr offset;

  for (bfd *in = coff_data (abfd)->link_info->input_bfds; in;
       in = in->link.next)
    {
      for (asection *s = in->sections; s; s = s->next)
	{
	  if (s->size == 0 || discarded_section (s))
	    continue;

	  num_sc++;
	}
    }

  *size = sizeof (uint32_t) + (num_sc * sizeof (struct section_contribution));
  *data = xmalloc (*size);

  bfd_putl32 (SECTION_CONTRIB_VERSION_60, *data);

  /* Read characteristics of outputted sections.  */

  sect_flags = xmalloc (sizeof (uint32_t) * abfd->section_count);

  offset = bfd_coff_filhsz (abfd) + bfd_coff_aoutsz (abfd);
  offset += offsetof (struct external_scnhdr, s_flags);

  for (unsigned int i = 0; i < abfd->section_count; i++)
    {
      bfd_seek (abfd, offset, SEEK_SET);

      if (bfd_bread (sect_flags + (i * sizeof (uint32_t)), sizeof (uint32_t),
		     abfd) != sizeof (uint32_t))
	{
	  free (*data);
	  free (sect_flags);
	  return false;
	}

      offset += sizeof (struct external_scnhdr);
    }

  sc =
    (struct section_contribution *) ((uint8_t *) *data + sizeof (uint32_t));

  mod_index = 0;
  for (bfd *in = coff_data (abfd)->link_info->input_bfds; in;
       in = in->link.next)
    {
      for (asection *s = in->sections; s; s = s->next)
	{
	  uint16_t sect_num;

	  if (s->size == 0 || discarded_section (s))
	    continue;

	  sect_num = find_section_number (abfd, s->output_section);

	  memcpy (&sc->characteristics,
		  sect_flags + ((sect_num - 1) * sizeof (uint32_t)),
		  sizeof (uint32_t));

	  bfd_putl16 (sect_num, &sc->section);
	  bfd_putl16 (0, &sc->padding1);
	  bfd_putl32 (s->output_offset, &sc->offset);
	  bfd_putl32 (s->size, &sc->size);
	  bfd_putl16 (mod_index, &sc->module_index);
	  bfd_putl16 (0, &sc->padding2);
	  bfd_putl32 (0, &sc->data_crc);
	  bfd_putl32 (0, &sc->reloc_crc);

	  sc++;
	}

      mod_index++;
    }

  free (sect_flags);

  return true;
}

/* The source info substream lives within the DBI stream, and lists the
   source files for each object file (i.e. it's derived from the
   DEBUG_S_FILECHKSMS parts of the .debug$S sections).  This is a bit
   superfluous, as the filenames are also available in the C13 parts of
   the module streams, but MSVC relies on it to work properly.  */
static void
create_source_info_substream (void **data, uint32_t *size,
			      struct source_files_info *source)
{
  uint16_t dedupe_source_files_count = 0;
  uint16_t source_files_count = 0;
  uint32_t strings_len = 0;
  uint8_t *ptr;

  /* Loop through the source files, marking unique filenames.  The pointers
     here are for entries in the main string table, and so have already
     been deduplicated.  */

  for (uint16_t i = 0; i < source->mod_count; i++)
    {
      for (uint16_t j = 0; j < source->mods[i].files_count; j++)
	{
	  if (source->mods[i].files[j])
	    {
	      if (source->mods[i].files[j]->source_file_offset == 0xffffffff)
		{
		  source->mods[i].files[j]->source_file_offset = strings_len;
		  strings_len += source->mods[i].files[j]->len + 1;
		  dedupe_source_files_count++;
		}

	      source_files_count++;
	    }
	}
    }

  *size = sizeof (uint16_t) + sizeof (uint16_t);
  *size += (sizeof (uint16_t) + sizeof (uint16_t)) * source->mod_count;
  *size += sizeof (uint32_t) * source_files_count;
  *size += strings_len;

  *data = xmalloc (*size);

  ptr = (uint8_t *) *data;

  /* Write header (module count and source file count).  */

  bfd_putl16 (source->mod_count, ptr);
  ptr += sizeof (uint16_t);

  bfd_putl16 (dedupe_source_files_count, ptr);
  ptr += sizeof (uint16_t);

  /* Write "ModIndices".  As the LLVM documentation puts it, "this array is
     present, but does not appear to be useful".  */

  for (uint16_t i = 0; i < source->mod_count; i++)
    {
      bfd_putl16 (i, ptr);
      ptr += sizeof (uint16_t);
    }

  /* Write source file count for each module.  */

  for (uint16_t i = 0; i < source->mod_count; i++)
    {
      bfd_putl16 (source->mods[i].files_count, ptr);
      ptr += sizeof (uint16_t);
    }

  /* For each module, write the offsets within the string table
     for each source file.  */

  for (uint16_t i = 0; i < source->mod_count; i++)
    {
      for (uint16_t j = 0; j < source->mods[i].files_count; j++)
	{
	  if (source->mods[i].files[j])
	    {
	      bfd_putl32 (source->mods[i].files[j]->source_file_offset, ptr);
	      ptr += sizeof (uint32_t);
	    }
	}
    }

  /* Write the string table.  We set source_file_offset to a dummy value for
     each entry we write, so we don't write duplicate filenames.  */

  for (uint16_t i = 0; i < source->mod_count; i++)
    {
      for (uint16_t j = 0; j < source->mods[i].files_count; j++)
	{
	  if (source->mods[i].files[j]
	      && source->mods[i].files[j]->source_file_offset != 0xffffffff)
	    {
	      memcpy (ptr, source->mods[i].files[j]->s,
		      source->mods[i].files[j]->len);
	      ptr += source->mods[i].files[j]->len;

	      *ptr = 0;
	      ptr++;

	      source->mods[i].files[j]->source_file_offset = 0xffffffff;
	    }
	}
    }
}

/* Stream 4 is the debug information (DBI) stream.  */
static bool
populate_dbi_stream (bfd *stream, bfd *abfd, bfd *pdb,
		     uint16_t section_header_stream_num,
		     uint16_t sym_rec_stream_num,
		     uint16_t publics_stream_num,
		     struct string_table *strings)
{
  struct pdb_dbi_stream_header h;
  struct optional_dbg_header opt;
  void *mod_info, *sc, *source_info;
  uint32_t mod_info_size, sc_size, source_info_size;
  struct source_files_info source;

  source.mod_count = 0;
  source.mods = NULL;

  if (!create_module_info_substream (abfd, pdb, &mod_info, &mod_info_size,
				     strings, &source))
    return false;

  if (!create_section_contrib_substream (abfd, &sc, &sc_size))
    {
      for (unsigned int i = 0; i < source.mod_count; i++)
	{
	  free (source.mods[i].files);
	}
      free (source.mods);

      free (mod_info);
      return false;
    }

  create_source_info_substream (&source_info, &source_info_size, &source);

  for (unsigned int i = 0; i < source.mod_count; i++)
    {
      free (source.mods[i].files);
    }
  free (source.mods);

  bfd_putl32 (0xffffffff, &h.version_signature);
  bfd_putl32 (DBI_STREAM_VERSION_70, &h.version_header);
  bfd_putl32 (1, &h.age);
  bfd_putl16 (0xffff, &h.global_stream_index);
  bfd_putl16 (0x8e1d, &h.build_number); // MSVC 14.29
  bfd_putl16 (publics_stream_num, &h.public_stream_index);
  bfd_putl16 (0, &h.pdb_dll_version);
  bfd_putl16 (sym_rec_stream_num, &h.sym_record_stream);
  bfd_putl16 (0, &h.pdb_dll_rbld);
  bfd_putl32 (mod_info_size, &h.mod_info_size);
  bfd_putl32 (sc_size, &h.section_contribution_size);
  bfd_putl32 (0, &h.section_map_size);
  bfd_putl32 (source_info_size, &h.source_info_size);
  bfd_putl32 (0, &h.type_server_map_size);
  bfd_putl32 (0, &h.mfc_type_server_index);
  bfd_putl32 (sizeof (opt), &h.optional_dbg_header_size);
  bfd_putl32 (0, &h.ec_substream_size);
  bfd_putl16 (0, &h.flags);
  bfd_putl16 (get_arch_number (abfd), &h.machine);
  bfd_putl32 (0, &h.padding);

  if (bfd_bwrite (&h, sizeof (h), stream) != sizeof (h))
    {
      free (source_info);
      free (sc);
      free (mod_info);
      return false;
    }

  if (bfd_bwrite (mod_info, mod_info_size, stream) != mod_info_size)
    {
      free (source_info);
      free (sc);
      free (mod_info);
      return false;
    }

  free (mod_info);

  if (bfd_bwrite (sc, sc_size, stream) != sc_size)
    {
      free (source_info);
      free (sc);
      return false;
    }

  free (sc);

  if (bfd_bwrite (source_info, source_info_size, stream) != source_info_size)
    {
      free (source_info);
      return false;
    }

  free (source_info);

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
      if (!in->outsymbols)
	continue;

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

/* Populate the "/names" named stream, which contains the string table.  */
static bool
populate_names_stream (bfd *stream, struct string_table *strings)
{
  char int_buf[sizeof (uint32_t)];
  struct string_table_header h;
  uint32_t num_strings = 0, num_buckets;
  struct string **buckets;

  bfd_putl32 (STRING_TABLE_SIGNATURE, &h.signature);
  bfd_putl32 (STRING_TABLE_VERSION, &h.version);

  if (bfd_bwrite (&h, sizeof (h), stream) != sizeof (h))
    return false;

  bfd_putl32 (strings->strings_len, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), stream) != sizeof (uint32_t))
    return false;

  int_buf[0] = 0;

  if (bfd_bwrite (int_buf, 1, stream) != 1)
    return false;

  for (struct string *s = strings->strings_head; s; s = s->next)
    {
      if (bfd_bwrite (s->s, s->len, stream) != s->len)
	return false;

      if (bfd_bwrite (int_buf, 1, stream) != 1)
	return false;

      num_strings++;
    }

  num_buckets = num_strings * 2;

  buckets = xmalloc (sizeof (struct string *) * num_buckets);
  memset (buckets, 0, sizeof (struct string *) * num_buckets);

  for (struct string *s = strings->strings_head; s; s = s->next)
    {
      uint32_t bucket_num = s->hash % num_buckets;

      while (buckets[bucket_num])
	{
	  bucket_num++;

	  if (bucket_num == num_buckets)
	    bucket_num = 0;
	}

      buckets[bucket_num] = s;
    }

  bfd_putl32 (num_buckets, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), stream) != sizeof (uint32_t))
    {
      free (buckets);
      return false;
    }

  for (unsigned int i = 0; i < num_buckets; i++)
    {
      if (buckets[i])
	bfd_putl32 (buckets[i]->offset, int_buf);
      else
	bfd_putl32 (0, int_buf);

      if (bfd_bwrite (int_buf, sizeof (uint32_t), stream) !=
	  sizeof (uint32_t))
	{
	  free (buckets);
	  return false;
	}
    }

  free (buckets);

  bfd_putl32 (num_strings, int_buf);

  if (bfd_bwrite (int_buf, sizeof (uint32_t), stream) != sizeof (uint32_t))
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
  bfd *info_stream, *dbi_stream, *names_stream, *sym_rec_stream,
    *publics_stream;
  uint16_t section_header_stream_num, sym_rec_stream_num, publics_stream_num;
  struct string_table strings;

  pdb = bfd_openw (pdb_name, "pdb");
  if (!pdb)
    {
      einfo (_("%P: warning: cannot create PDB file: %E\n"));
      return false;
    }

  strings.strings_head = NULL;
  strings.strings_tail = NULL;
  strings.strings_len = 1;
  strings.hashmap = htab_create_alloc (0, hash_string_table_entry,
				       eq_string_table_entry, free,
				       xcalloc, free);

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

  if (!populate_dbi_stream (dbi_stream, abfd, pdb, section_header_stream_num,
			    sym_rec_stream_num, publics_stream_num,
			    &strings))
    {
      einfo (_("%P: warning: cannot populate DBI stream "
	       "in PDB file: %E\n"));
      goto end;
    }

  add_string ("", 0, &strings);

  if (!populate_names_stream (names_stream, &strings))
    {
      einfo (_("%P: warning: cannot populate names stream "
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

  htab_delete (strings.hashmap);

  return ret;
}
