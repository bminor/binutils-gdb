/* BFD backend for SunOS binaries.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
   Written by Cygnus Support.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define ARCH 32
#define TARGETNAME "a.out-sunos-big"
#define MY(OP) CAT(sunos_big_,OP)

#include "bfd.h"

/* Static routines defined in this file.  */

struct external_nlist;

static boolean sunos_read_dynamic_info PARAMS ((bfd *));
static bfd_size_type MY(read_dynamic_symbols)
     PARAMS ((bfd *, struct external_nlist **, char **, bfd_size_type *));
static bfd_size_type MY(read_dynamic_relocs) PARAMS ((bfd *, PTR *));

#define MY_read_dynamic_symbols MY(read_dynamic_symbols)
#define MY_read_dynamic_relocs MY(read_dynamic_relocs)

/* Include the usual a.out support.  */
#include "aoutf1.h"

/* SunOS shared library support.  We store a pointer to this structure
   in obj_aout_dynamic_info (abfd).  */

struct sunos_dynamic_info
{
  /* Whether we found any dynamic information.  */
  boolean valid;
  /* Dynamic information.  */
  struct internal_sun4_dynamic_link dyninfo;
  /* Number of dynamic symbols.  */
  bfd_size_type dynsym_count;
  /* Read in nlists for dynamic symbols.  */
  struct external_nlist *dynsym;
  /* Read in dynamic string table.  */
  char *dynstr;
  /* Number of dynamic relocs.  */
  bfd_size_type dynrel_count;
  /* Read in dynamic relocs.  This may be reloc_std_external or
     reloc_ext_external.  */
  PTR dynrel;
};

/* Read in the basic dynamic information.  This locates the __DYNAMIC
   structure and uses it to find the dynamic_link structure.  It
   creates and saves a sunos_dynamic_info structure.  If it can't find
   __DYNAMIC, it sets the valid field of the sunos_dynamic_info
   structure to false to avoid doing this work again.  */

static boolean
sunos_read_dynamic_info (abfd)
     bfd *abfd;
{
  struct sunos_dynamic_info *info;
  struct external_nlist dynsym;
  char buf[sizeof "__DYNAMIC"];
  asection *dynsec;
  file_ptr dynoff;
  struct external_sun4_dynamic dyninfo;
  unsigned long dynver;
  struct external_sun4_dynamic_link linkinfo;

  if (obj_aout_dynamic_info (abfd) != (PTR) NULL)
    return true;

  info = ((struct sunos_dynamic_info *)
	  bfd_zalloc (abfd, sizeof (struct sunos_dynamic_info)));
  if (!info)
    {
      bfd_error = no_memory;
      return false;
    }
  info->valid = false;
  info->dynsym = NULL;
  info->dynstr = NULL;
  info->dynrel = NULL;
  obj_aout_dynamic_info (abfd) = (PTR) info;

  /* We look for the __DYNAMIC symbol to locate the dynamic linking
     information.  It should be the first symbol if it is defined.  If
     we can't find it, don't sweat it.  */
  if ((abfd->flags & DYNAMIC) == 0
      || bfd_get_symcount (abfd) <= 0
      || bfd_seek (abfd, obj_sym_filepos (abfd), SEEK_SET) != 0
      || (bfd_read ((PTR) &dynsym, 1, EXTERNAL_NLIST_SIZE, abfd)
	  != EXTERNAL_NLIST_SIZE)
      || ((dynsym.e_type[0] & N_TYPE) != N_DATA
	  && (dynsym.e_type[0] & N_TYPE) != N_TEXT)
      || bfd_seek (abfd,
		   obj_str_filepos (abfd) + GET_WORD (abfd, dynsym.e_strx),
		   SEEK_SET) != 0
      || bfd_read ((PTR) buf, 1, sizeof buf, abfd) != sizeof buf
      || buf[sizeof buf - 1] != '\0'
      || strcmp (buf, "__DYNAMIC") != 0)
    return true;

  if ((dynsym.e_type[0] & N_TYPE) == N_DATA)
    dynsec = obj_datasec (abfd);
  else
    dynsec = obj_textsec (abfd);
  if (! bfd_get_section_contents (abfd, dynsec, (PTR) &dyninfo,
				  (GET_WORD (abfd, dynsym.e_value)
				   - bfd_get_section_vma (abfd, dynsec)),
				  sizeof dyninfo))
    return true;

  dynver = GET_WORD (abfd, dyninfo.ld_version);
  if (dynver != 2 && dynver != 3)
    return true;

  dynoff = GET_WORD (abfd, dyninfo.ld);

  /* dynoff is a virtual address.  It is probably always in the .data
     section, but this code should work even if it moves.  */
  if (dynoff < bfd_get_section_vma (abfd, obj_datasec (abfd)))
    dynsec = obj_textsec (abfd);
  else
    dynsec = obj_datasec (abfd);
  dynoff -= bfd_get_section_vma (abfd, dynsec);
  if (dynoff < 0 || dynoff > bfd_section_size (abfd, dynsec))
    return true;

  /* This executable appears to be dynamically linked in a way that we
     can understand.  */
  if (! bfd_get_section_contents (abfd, dynsec, (PTR) &linkinfo, dynoff,
				  (bfd_size_type) sizeof linkinfo))
    return true;

  /* Swap in the dynamic link information.  */
  info->dyninfo.ld_loaded = GET_WORD (abfd, linkinfo.ld_loaded);
  info->dyninfo.ld_need = GET_WORD (abfd, linkinfo.ld_need);
  info->dyninfo.ld_rules = GET_WORD (abfd, linkinfo.ld_rules);
  info->dyninfo.ld_got = GET_WORD (abfd, linkinfo.ld_got);
  info->dyninfo.ld_plt = GET_WORD (abfd, linkinfo.ld_plt);
  info->dyninfo.ld_rel = GET_WORD (abfd, linkinfo.ld_rel);
  info->dyninfo.ld_hash = GET_WORD (abfd, linkinfo.ld_hash);
  info->dyninfo.ld_stab = GET_WORD (abfd, linkinfo.ld_stab);
  info->dyninfo.ld_stab_hash = GET_WORD (abfd, linkinfo.ld_stab_hash);
  info->dyninfo.ld_buckets = GET_WORD (abfd, linkinfo.ld_buckets);
  info->dyninfo.ld_symbols = GET_WORD (abfd, linkinfo.ld_symbols);
  info->dyninfo.ld_symb_size = GET_WORD (abfd, linkinfo.ld_symb_size);
  info->dyninfo.ld_text = GET_WORD (abfd, linkinfo.ld_text);
  info->dyninfo.ld_plt_sz = GET_WORD (abfd, linkinfo.ld_plt_sz);

  /* The only way to get the size of the symbol information appears to
     be to determine the distance between it and the string table.  */
  info->dynsym_count = ((info->dyninfo.ld_symbols - info->dyninfo.ld_stab)
			/ EXTERNAL_NLIST_SIZE);
  BFD_ASSERT (info->dynsym_count * EXTERNAL_NLIST_SIZE
	      == info->dyninfo.ld_symbols - info->dyninfo.ld_stab);

  /* Similarly, the relocs end at the hash table.  */
  info->dynrel_count = ((info->dyninfo.ld_hash - info->dyninfo.ld_rel)
			/ obj_reloc_entry_size (abfd));
  BFD_ASSERT (info->dynrel_count * obj_reloc_entry_size (abfd)
	      == info->dyninfo.ld_hash - info->dyninfo.ld_rel);

  info->valid = true;

  return true;
}

/* Read in the dynamic symbols.  */

static bfd_size_type
MY(read_dynamic_symbols) (abfd, syms, strs, strsize)
     bfd *abfd;
     struct external_nlist **syms;
     char **strs;
     bfd_size_type *strsize;
{
  struct sunos_dynamic_info *info;

  if (obj_aout_dynamic_info (abfd) == (PTR) NULL)
    {
      if (! sunos_read_dynamic_info (abfd))
	  return (bfd_size_type) -1;
    }

  info = (struct sunos_dynamic_info *) obj_aout_dynamic_info (abfd);
  if (! info->valid || info->dynsym_count == 0)
    return 0;

  if (info->dynsym == (struct external_nlist *) NULL)
    {
      info->dynsym = ((struct external_nlist *)
		      bfd_alloc (abfd,
				 (info->dynsym_count
				  * EXTERNAL_NLIST_SIZE)));
      info->dynstr = (char *) bfd_alloc (abfd, info->dyninfo.ld_symb_size);
      if (!info->dynsym || !info->dynstr)
	{
	  bfd_error = no_memory;
	  return 0;
	}
      if (bfd_seek (abfd, info->dyninfo.ld_stab, SEEK_SET) != 0
	  || (bfd_read ((PTR) info->dynsym, info->dynsym_count,
			EXTERNAL_NLIST_SIZE, abfd)
	      != info->dynsym_count * EXTERNAL_NLIST_SIZE)
	  || bfd_seek (abfd, info->dyninfo.ld_symbols, SEEK_SET) != 0
	  || (bfd_read ((PTR) info->dynstr, 1, info->dyninfo.ld_symb_size,
			abfd)
	      != info->dyninfo.ld_symb_size))
	return (bfd_size_type) -1;
    }

  *syms = info->dynsym;
  *strs = info->dynstr;
  *strsize = info->dyninfo.ld_symb_size;

#ifdef CHECK_DYNAMIC_HASH
  /* Check my understanding of the dynamic hash table by making sure
     that each symbol can be located in the hash table.  */
  {
    bfd_size_type table_size;
    bfd_byte *table;
    bfd_size_type i;

    if (info->dyninfo.ld_buckets > info->dynsym_count)
      abort ();
    table_size = info->dyninfo.ld_stab - info->dyninfo.ld_hash;
    table = (bfd_byte *) alloca (table_size);
    if (bfd_seek (abfd, info->dyninfo.ld_hash, SEEK_SET) != 0
	|| bfd_read ((PTR) table, 1, table_size, abfd) != table_size)
      abort ();
    for (i = 0; i < info->dynsym_count; i++)
      {
	unsigned char *name;
	unsigned long hash;

	name = ((unsigned char *) info->dynstr
		+ GET_WORD (abfd, info->dynsym[i].e_strx));
	hash = 0;
	while (*name != '\0')
	  hash = (hash << 1) + *name++;
	hash &= 0x7fffffff;
	hash %= info->dyninfo.ld_buckets;
	while (GET_WORD (abfd, table + 8 * hash) != i)
	  {
	    hash = GET_WORD (abfd, table + 8 * hash + 4);
	    if (hash == 0 || hash >= table_size / 8)
	      abort ();
	  }
      }
  }
#endif /* CHECK_DYNAMIC_HASH */

  return info->dynsym_count;
}

/* Read in the dynamic relocs for a section.  */

static bfd_size_type
MY(read_dynamic_relocs) (abfd, relocs)
     bfd *abfd;
     PTR *relocs;
{
  struct sunos_dynamic_info *info;

  if (obj_aout_dynamic_info (abfd) == (PTR) NULL)
    {
      if (! sunos_read_dynamic_info (abfd))
	  return (bfd_size_type) -1;
    }

  info = (struct sunos_dynamic_info *) obj_aout_dynamic_info (abfd);
  if (! info->valid || info->dynrel_count == 0)
    return 0;

  if (info->dynrel == NULL)
    {
      info->dynrel = (PTR) bfd_alloc (abfd,
				      (info->dynrel_count
				       * obj_reloc_entry_size (abfd)));
      if (!info->dynrel)
	{
	  bfd_error = no_memory;
	  return (bfd_size_type) -1;
	}
      if (bfd_seek (abfd, info->dyninfo.ld_rel, SEEK_SET) != 0
	  || (bfd_read ((PTR) info->dynrel, info->dynrel_count,
			obj_reloc_entry_size (abfd), abfd)
	      != info->dynrel_count * obj_reloc_entry_size (abfd)))
	return (bfd_size_type) -1;
    }

  *relocs = info->dynrel;

  return info->dynrel_count;
}
