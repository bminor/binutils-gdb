/* SEC_MERGE support.
   Copyright 2001 Free Software Foundation, Inc.
   Written by Jakub Jelinek <jakub@redhat.com>.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This file contains support for merging duplicate entities within sections,
   as used in ELF SHF_MERGE.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#include <ctype.h>

/* An entry in the section merge hash table.  */

struct sec_merge_hash_entry
{
  struct bfd_hash_entry root;
  /* Length of this entry.  */
  unsigned int len;
  /* Index within the merged section.  */
  bfd_size_type index;
  /* Which section is it in.  */
  asection *sec;
  /* Next entity in the hash table.  */
  struct sec_merge_hash_entry *next;
};

/* The section merge hash table.  */

struct sec_merge_hash
{
  struct bfd_hash_table table;
  /* Next available index.  */
  bfd_size_type size;
  /* First entity in the SEC_MERGE sections of this type.  */
  struct sec_merge_hash_entry *first;
  /* Last entity in the SEC_MERGE sections of this type.  */
  struct sec_merge_hash_entry *last;
  /* Entity size.  */
  unsigned int entsize;
  /* Start of each string needs to be aligned to 1 << alignment_power
     octets.  */
  unsigned int alignment_power;
  /* Are entries fixed size or zero terminated strings?  */
  boolean strings;
};

struct sec_merge_info
{
  /* Chain of sec_merge_infos.  */
  struct sec_merge_info *next;
  /* A hash table used to hold section content.  */
  struct sec_merge_hash *htab;
  /* The last section attempted for merge.  */
  asection *last;
};

struct sec_merge_sec_info
{
  /* A hash table used to hold section content.  */
  struct sec_merge_hash *htab;
  /* First string in this section.  */
  struct sec_merge_hash_entry *first;
  /* Original section content.  */
  unsigned char contents[1];
};

static struct bfd_hash_entry *sec_merge_hash_newfunc
  PARAMS ((struct bfd_hash_entry *, struct bfd_hash_table *, const char *));
static struct sec_merge_hash_entry *sec_merge_hash_lookup
  PARAMS ((struct sec_merge_hash *, const char *, boolean));
static struct sec_merge_hash *sec_merge_init
  PARAMS ((unsigned int, unsigned int, boolean));
static struct sec_merge_hash_entry *sec_merge_add
  PARAMS ((struct sec_merge_hash *, const char *));
static boolean sec_merge_emit
  PARAMS ((bfd *, struct sec_merge_hash *, struct sec_merge_hash_entry *));

/* Routine to create an entry in a section merge hashtab.  */

static struct bfd_hash_entry *
sec_merge_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  struct sec_merge_hash_entry *ret = (struct sec_merge_hash_entry *) entry;

  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (ret == (struct sec_merge_hash_entry *) NULL)
    ret = ((struct sec_merge_hash_entry *)
	   bfd_hash_allocate (table, sizeof (struct sec_merge_hash_entry)));
  if (ret == (struct sec_merge_hash_entry *) NULL)
    return NULL;

  /* Call the allocation method of the superclass.  */
  ret = ((struct sec_merge_hash_entry *)
	 bfd_hash_newfunc ((struct bfd_hash_entry *) ret, table, string));

  if (ret)
    {
      /* Initialize the local fields.  */
      ret->index = (bfd_size_type) -1;
      ret->sec = NULL;
      ret->next = NULL;
    }

  return (struct bfd_hash_entry *)ret;
}

/* Look up an entry in a section merge hash table.  */

static struct sec_merge_hash_entry *
sec_merge_hash_lookup (table, string, create)
     struct sec_merge_hash *table;
     const char *string;
     boolean create;
{
  register const unsigned char *s;
  register unsigned long hash;
  register unsigned int c;
  struct sec_merge_hash_entry *hashp;
  unsigned int len, i;
  unsigned int index;

  hash = 0;
  len = 0;
  s = (const unsigned char *) string;
  if (table->strings)
    {
      if (table->entsize == 1)
	{
	  while ((c = *s++) != '\0')
	    {
	      hash += c + (c << 17);
	      hash ^= hash >> 2;
	      ++len;
	    }
	  hash += len + (len << 17);
	}
      else
	{
	  for (;;)
	    {
	      for (i = 0; i < table->entsize; ++i)
		if (s[i] != '\0')
		  break;
	      if (i == table->entsize)
		break;
	      for (i = 0; i < table->entsize; ++i)
		{
		  c = *s++;
		  hash += c + (c << 17);
		  hash ^= hash >> 2;
		}
	      ++len;
	    }
	  hash += len + (len << 17);
	  len *= table->entsize;
	}
      hash ^= hash >> 2;
      len += table->entsize;
    }      
  else
    {
      for (i = 0; i < table->entsize; ++i)
	{
	  c = *s++;
	  hash += c + (c << 17);
	  hash ^= hash >> 2;
	}
      len = table->entsize;
    }

  index = hash % table->table.size;
  for (hashp = (struct sec_merge_hash_entry *) table->table.table[index];
       hashp != (struct sec_merge_hash_entry *) NULL;
       hashp = (struct sec_merge_hash_entry *) hashp->next)
    {
      if (hashp->root.hash == hash
	  && len == hashp->len
	  && memcmp (hashp->root.string, string, len) == 0)
	return hashp;
    }

  if (! create)
    return (struct sec_merge_hash_entry *) NULL;

  hashp = (struct sec_merge_hash_entry *)
	  sec_merge_hash_newfunc ((struct bfd_hash_entry *) NULL,
				  (struct bfd_hash_table *) table, string);
  if (hashp == (struct sec_merge_hash_entry *) NULL)
    return (struct sec_merge_hash_entry *) NULL;
  hashp->root.string = string;
  hashp->root.hash = hash;
  hashp->len = len;
  hashp->root.next = table->table.table[index];
  table->table.table[index] = (struct bfd_hash_entry *) hashp;

  return hashp;
}

/* Create a new hash table.  */

static struct sec_merge_hash *
sec_merge_init (alignment_power, entsize, strings)
     unsigned int alignment_power, entsize;
     boolean strings;
{
  struct sec_merge_hash *table;

  table = ((struct sec_merge_hash *)
	   bfd_malloc (sizeof (struct sec_merge_hash)));
  if (table == NULL)
    return NULL;

  if (! bfd_hash_table_init (&table->table, sec_merge_hash_newfunc))
    {
      free (table);
      return NULL;
    }

  table->size = 0;
  table->first = NULL;
  table->last = NULL;
  table->alignment_power = alignment_power;
  table->entsize = entsize;
  table->strings = strings;

  return table;
}

/* Get the index of an entity in a hash table, adding it if it is not
   already present.  */

static struct sec_merge_hash_entry *
sec_merge_add (tab, str)
     struct sec_merge_hash *tab;
     const char *str;
{
  register struct sec_merge_hash_entry *entry;

  entry = sec_merge_hash_lookup (tab, str, true);
  if (entry == NULL)
    return NULL;

  if (entry->index == (bfd_size_type) -1)
    {
      entry->index = tab->size;
      tab->size += entry->len;
      tab->size = align_power (tab->size, tab->alignment_power);
      if (tab->first == NULL)
	tab->first = entry;
      else
	tab->last->next = entry;
      tab->last = entry;
    }

  return entry;
}

static boolean
sec_merge_emit (abfd, tab, entry)
     register bfd *abfd;
     struct sec_merge_hash *tab;
     struct sec_merge_hash_entry *entry;
{
  asection *sec = entry->sec;
  char *pad = "";

  if (tab->alignment_power)
    pad = bfd_zmalloc (1 << tab->alignment_power);

  for (; entry != NULL && entry->sec == sec; entry = entry->next)
    {
      register const char *str;
      register size_t len;

      str = entry->root.string;
      len = entry->len;

      if (bfd_write ((PTR) str, 1, len, abfd) != len)
	break;

      if (tab->alignment_power)
	{
	  len &= (1 << tab->alignment_power) - 1;
	  if (len && bfd_write ((PTR) pad, 1,
				(1 << tab->alignment_power) - len,
				abfd) != (1 << tab->alignment_power) - len)
	    break;
	}
    }

  if (tab->alignment_power)
    free (pad);

  return entry == NULL || entry->sec != sec;
}

/* This function is called for each input file from the add_symbols
   pass of the linker.  */

boolean
_bfd_merge_section (abfd, psinfo, sec, psecinfo)
     bfd *abfd;
     PTR *psinfo;
     asection *sec;
     PTR *psecinfo;
{
  boolean first, nul;
  struct sec_merge_info *sinfo;
  struct sec_merge_sec_info *secinfo;
  unsigned char *p, *end;
  bfd_vma mask;
  unsigned int align;
  unsigned int i;

  if (sec->_raw_size == 0
      || (sec->flags & SEC_MERGE) == 0
      || sec->entsize == 0)
    return true;

  if ((sec->flags & SEC_RELOC) != 0)
    {
      /* We aren't prepared to handle relocations in merged sections.  */
      return true;
    }

  if (sec->output_section != NULL
      && bfd_is_abs_section (sec->output_section))
    {
      /* The section is being discarded from the link, so we should
	 just ignore it.  */
      return true;
    }

  align = bfd_get_section_alignment (abfd, sec);
  if ((sec->flags & SEC_STRINGS)
      && ((sec->entsize < (unsigned int)(1 << align)
	   && (sec->entsize & (sec->entsize - 1)))
	  || (sec->entsize > (unsigned int)(1 << align)
	      && (sec->entsize & ((1 << align) - 1)))))
    {
      /* Sanity check.  If string character size is smaller than
	 alignment, then we require character size to be a power
	 of 2, otherwise character size must be integer multiple
	 of alignment.  */
      return true;
    }

  first = false;

  for (sinfo = (struct sec_merge_info *) *psinfo; sinfo; sinfo = sinfo->next)
    if (! ((sinfo->last->flags ^ sec->flags) & (SEC_MERGE | SEC_STRINGS))
	&& sinfo->last->entsize == sec->entsize
	&& ! strcmp (sinfo->last->name, sec->name)
	&& bfd_get_section_alignment (abfd, sinfo->last) == align)
      break;

  if (sinfo == NULL)
    {
      /* Initialize the information we need to keep track of.  */
      first = true;
      sinfo = (struct sec_merge_info *)
	      bfd_alloc (abfd, sizeof (struct sec_merge_info));
      if (sinfo == NULL)
	goto error_return;
      sinfo->next = (struct sec_merge_info *) *psinfo;
      *psinfo = (PTR) sinfo;
      sinfo->htab =
	sec_merge_init ((sec->flags & SEC_STRINGS) ? align : 0,
			sec->entsize, (sec->flags & SEC_STRINGS));
      if (sinfo->htab == NULL)
	goto error_return;
    }

  /* Read the section from abfd.  */

  *psecinfo = bfd_alloc (abfd, sizeof (struct sec_merge_sec_info)
			       + sec->_raw_size - 1);
  if (*psecinfo == NULL)
    goto error_return;

  secinfo = (struct sec_merge_sec_info *)*psecinfo;
  secinfo->htab = sinfo->htab;
  sinfo->htab->size = 0;
  secinfo->first = NULL;

  if (! bfd_get_section_contents (abfd, sec, secinfo->contents, 0,
				  sec->_raw_size))
    goto error_return;

  end = secinfo->contents + sec->_raw_size;
  nul = false;
  mask = ((bfd_vma)1 << sinfo->htab->alignment_power) - 1;
  if (sec->flags & SEC_STRINGS)
    {
      for (p = secinfo->contents; p < end;)
	{
	  struct sec_merge_hash_entry *entry;

	  entry = sec_merge_add (sinfo->htab, p);
	  if (entry->sec == NULL)
	    {
	      if (secinfo->first == NULL)
		secinfo->first = entry;
	      entry->sec = sec;
	    }
	  p += entry->len;
	  if (sec->entsize == 1)
	    {
	      while (p < end && *p == 0)
		{
		  if (!nul && !((p - secinfo->contents) & mask))
		    {
		      nul = true;
		      entry = sec_merge_add (sinfo->htab, "");
		      if (entry->sec == NULL)
		        {
			  if (secinfo->first == NULL)
			    secinfo->first = entry;
			  entry->sec = sec;
		        }
		    }
		  p++;
	        }
	    }
	  else
	    {
	      while (p < end)
		{
		  for (i = 0; i < sec->entsize; i++)
		    if (p[i] != '\0')
		      break;
		  if (i != sec->entsize)
		    break;
		  if (!nul && !((p - secinfo->contents) & mask))
		    {
		      nul = true;
		      entry = sec_merge_add (sinfo->htab, p);
		      if (entry->sec == NULL)
			{
			  if (secinfo->first == NULL)
			    secinfo->first = entry;
			  entry->sec = sec;
			}
		    }
		  p += sec->entsize;
		}
	    }
	}
    }
  else
    {
      for (p = secinfo->contents; p < end; p += sec->entsize)
	{
	  struct sec_merge_hash_entry *entry;

	  entry = sec_merge_add (sinfo->htab, p);
	  if (entry->sec == NULL)
	    {
	      if (secinfo->first == NULL)
		secinfo->first = entry;
	      entry->sec = sec;
	    }
	}
    }

  sec->_cooked_size = sinfo->htab->size;
  if (!sec->_cooked_size)
    sec->flags |= SEC_EXCLUDE;
  sinfo->last = sec;
  return true;

 error_return:
  if (*psecinfo != NULL)
    free (*psecinfo);
  *psecinfo = NULL;
  return false;
}

/* Write out the merged section.  */

boolean
_bfd_write_merged_section (output_bfd, sec, psecinfo)
     bfd *output_bfd;
     asection *sec;
     PTR psecinfo;
{
  struct sec_merge_sec_info *secinfo;

  secinfo = (struct sec_merge_sec_info *) psecinfo;

  if (!secinfo->first)
    return true;

  if (bfd_seek (output_bfd,
		(sec->output_section->filepos + sec->output_offset),
		SEEK_SET) != 0)
    return false;

  if (! sec_merge_emit (output_bfd, secinfo->htab, secinfo->first))
    return false;

  return true;
}

/* Adjust an address in the SEC_MERGE section.  Given OFFSET within
   *PSEC, this returns the new offset in the adjusted SEC_MERGE
   section and writes the new section back into *PSEC.  */

bfd_vma
_bfd_merged_section_offset (output_bfd, psec, psecinfo, offset, addend)
     bfd *output_bfd ATTRIBUTE_UNUSED;
     asection **psec;
     PTR psecinfo;
     bfd_vma offset, addend;
{
  struct sec_merge_sec_info *secinfo;
  struct sec_merge_hash_entry *entry;
  unsigned char *p;
  asection *sec = *psec;

  secinfo = (struct sec_merge_sec_info *) psecinfo;

  if (offset + addend >= sec->_raw_size)
    {
      if (offset + addend > sec->_raw_size)
	(*_bfd_error_handler) (_("%s: access beyond end of merged section (%ld + %ld)"),
			       bfd_get_filename (sec->owner), (long)offset,
			       (long) addend);
      return (secinfo->first ? sec->_cooked_size : 0);
    }

  if (secinfo->htab->strings)
    {
      if (sec->entsize == 1)
	{
	  p = secinfo->contents + offset + addend - 1;
	  while (*p && p >= secinfo->contents)
	    --p;
	  ++p;
	}
      else
	{
	  p = secinfo->contents
	      + ((offset + addend) / sec->entsize) * sec->entsize;
	  p -= sec->entsize;
	  while (p >= secinfo->contents)
	    {
	      unsigned int i;

	      for (i = 0; i < sec->entsize; ++i)
		if (p[i] != '\0')
		  break;
	      if (i == sec->entsize)
		break;
	      p -= sec->entsize;
	    }
	  p += sec->entsize;
	}
    }
  else
    {
      p = secinfo->contents
	  + ((offset + addend) / sec->entsize) * sec->entsize;
    }
  entry = sec_merge_hash_lookup (secinfo->htab, p, false);
  if (!entry)
    {
      if (! secinfo->htab->strings)
	abort ();
      /* This should only happen if somebody points into the padding
	 after a NUL character but before next entity.  */
      if (*p)
	abort ();
      if (! secinfo->htab->first)
	abort ();
      entry = secinfo->htab->first;
      p = secinfo->contents
	  + ((offset + addend) / sec->entsize + 1) * sec->entsize
	  - entry->len;
    }

  *psec = entry->sec;
  return entry->index + (secinfo->contents + offset - p);
}
