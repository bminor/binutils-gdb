/* BFD backend for SunOS binaries.
   Copyright (C) 1990, 91, 92, 93, 94 Free Software Foundation, Inc.
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

#define TARGETNAME "a.out-sunos-big"
#define MY(OP) CAT(sunos_big_,OP)

#include "bfd.h"
#include "bfdlink.h"
#include "libaout.h"

/* Static routines defined in this file.  */

static boolean sunos_read_dynamic_info PARAMS ((bfd *));
static long sunos_get_dynamic_symtab_upper_bound PARAMS ((bfd *));
static long sunos_canonicalize_dynamic_symtab PARAMS ((bfd *, asymbol **));
static long sunos_get_dynamic_reloc_upper_bound PARAMS ((bfd *));
static long sunos_canonicalize_dynamic_reloc
  PARAMS ((bfd *, arelent **, asymbol **));
static struct bfd_hash_entry *sunos_link_hash_newfunc
  PARAMS ((struct bfd_hash_entry *, struct bfd_hash_table *, const char *));
static struct bfd_link_hash_table *sunos_link_hash_table_create
  PARAMS ((bfd *));
static boolean sunos_add_dynamic_symbols
  PARAMS ((bfd *, struct bfd_link_info *));
static boolean sunos_add_one_symbol
  PARAMS ((struct bfd_link_info *, bfd *, const char *, flagword, asection *,
	   bfd_vma, const char *, boolean, boolean,
	   struct bfd_link_hash_entry **));
static boolean sunos_scan_relocs
  PARAMS ((struct bfd_link_info *, bfd *, asection *, bfd_size_type));
static boolean sunos_scan_std_relocs
  PARAMS ((struct bfd_link_info *, bfd *, asection *,
	   const struct reloc_std_external *, bfd_size_type));
static boolean sunos_scan_ext_relocs
  PARAMS ((struct bfd_link_info *, bfd *, asection *,
	   const struct reloc_ext_external *, bfd_size_type));
static boolean sunos_link_dynamic_object
  PARAMS ((struct bfd_link_info *, bfd *));
static boolean sunos_write_dynamic_symbol
  PARAMS ((bfd *, struct bfd_link_info *, struct aout_link_hash_entry *));
static boolean sunos_check_dynamic_reloc
  PARAMS ((struct bfd_link_info *, bfd *, asection *,
	   struct aout_link_hash_entry *, PTR, boolean *));
static boolean sunos_finish_dynamic_link
  PARAMS ((bfd *, struct bfd_link_info *));

#define MY_get_dynamic_symtab_upper_bound sunos_get_dynamic_symtab_upper_bound
#define MY_canonicalize_dynamic_symtab sunos_canonicalize_dynamic_symtab
#define MY_get_dynamic_reloc_upper_bound sunos_get_dynamic_reloc_upper_bound
#define MY_canonicalize_dynamic_reloc sunos_canonicalize_dynamic_reloc
#define MY_bfd_link_hash_table_create sunos_link_hash_table_create
#define MY_add_dynamic_symbols sunos_add_dynamic_symbols
#define MY_add_one_symbol sunos_add_one_symbol
#define MY_link_dynamic_object sunos_link_dynamic_object
#define MY_write_dynamic_symbol sunos_write_dynamic_symbol
#define MY_check_dynamic_reloc sunos_check_dynamic_reloc
#define MY_finish_dynamic_link sunos_finish_dynamic_link

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
  long dynsym_count;
  /* Read in nlists for dynamic symbols.  */
  struct external_nlist *dynsym;
  /* asymbol structures for dynamic symbols.  */
  aout_symbol_type *canonical_dynsym;
  /* Read in dynamic string table.  */
  char *dynstr;
  /* Number of dynamic relocs.  */
  long dynrel_count;
  /* Read in dynamic relocs.  This may be reloc_std_external or
     reloc_ext_external.  */
  PTR dynrel;
  /* arelent structures for dynamic relocs.  */
  arelent *canonical_dynrel;
};

/* The hash table of dynamic symbols is composed of two word entries.
   See include/aout/sun4.h for details.  */

#define HASH_ENTRY_SIZE (2 * BYTES_IN_WORD)

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
  asection *dynsec;
  file_ptr dynoff;
  struct external_sun4_dynamic dyninfo;
  unsigned long dynver;
  struct external_sun4_dynamic_link linkinfo;

  if (obj_aout_dynamic_info (abfd) != (PTR) NULL)
    return true;

  if ((abfd->flags & DYNAMIC) == 0)
    {
      bfd_set_error (bfd_error_invalid_operation);
      return false;
    }

  info = ((struct sunos_dynamic_info *)
	  bfd_zalloc (abfd, sizeof (struct sunos_dynamic_info)));
  if (!info)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  info->valid = false;
  info->dynsym = NULL;
  info->dynstr = NULL;
  info->canonical_dynsym = NULL;
  info->dynrel = NULL;
  info->canonical_dynrel = NULL;
  obj_aout_dynamic_info (abfd) = (PTR) info;

  /* This code used to look for the __DYNAMIC symbol to locate the dynamic
     linking information.
     However this inhibits recovering the dynamic symbols from a
     stripped object file, so blindly assume that the dynamic linking
     information is located at the start of the data section.
     We could verify this assumption later by looking through the dynamic
     symbols for the __DYNAMIC symbol.  */
  if ((abfd->flags & DYNAMIC) == 0)
    return true;
  if (! bfd_get_section_contents (abfd, obj_datasec (abfd), (PTR) &dyninfo,
				  (file_ptr) 0, sizeof dyninfo))
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

/* Return the amount of memory required for the dynamic symbols.  */

static long
sunos_get_dynamic_symtab_upper_bound (abfd)
     bfd *abfd;
{
  struct sunos_dynamic_info *info;

  if (! sunos_read_dynamic_info (abfd))
    return -1;

  info = (struct sunos_dynamic_info *) obj_aout_dynamic_info (abfd);
  if (! info->valid)
    {
      bfd_set_error (bfd_error_no_symbols);
      return -1;
    }

  return (info->dynsym_count + 1) * sizeof (asymbol *);
}

/* Read in the dynamic symbols.  */

static long
sunos_canonicalize_dynamic_symtab (abfd, storage)
     bfd *abfd;
     asymbol **storage;
{
  struct sunos_dynamic_info *info;
  long i;

  /* Get the general dynamic information.  */
  if (obj_aout_dynamic_info (abfd) == NULL)
    {
      if (! sunos_read_dynamic_info (abfd))
	  return -1;
    }

  info = (struct sunos_dynamic_info *) obj_aout_dynamic_info (abfd);
  if (! info->valid)
    {
      bfd_set_error (bfd_error_no_symbols);
      return -1;
    }

  /* Get the dynamic nlist structures.  */
  if (info->dynsym == (struct external_nlist *) NULL)
    {
      info->dynsym = ((struct external_nlist *)
		      bfd_alloc (abfd,
				 (info->dynsym_count
				  * EXTERNAL_NLIST_SIZE)));
      if (info->dynsym == NULL && info->dynsym_count != 0)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return -1;
	}
      if (bfd_seek (abfd, info->dyninfo.ld_stab, SEEK_SET) != 0
	  || (bfd_read ((PTR) info->dynsym, info->dynsym_count,
			EXTERNAL_NLIST_SIZE, abfd)
	      != info->dynsym_count * EXTERNAL_NLIST_SIZE))
	{
	  if (info->dynsym != NULL)
	    {
	      bfd_release (abfd, info->dynsym);
	      info->dynsym = NULL;
	    }
	  return -1;
	}
    }

  /* Get the dynamic strings.  */
  if (info->dynstr == (char *) NULL)
    {
      info->dynstr = (char *) bfd_alloc (abfd, info->dyninfo.ld_symb_size);
      if (info->dynstr == NULL && info->dyninfo.ld_symb_size != 0)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return -1;
	}
      if (bfd_seek (abfd, info->dyninfo.ld_symbols, SEEK_SET) != 0
	  || (bfd_read ((PTR) info->dynstr, 1, info->dyninfo.ld_symb_size,
			abfd)
	      != info->dyninfo.ld_symb_size))
	{
	  if (info->dynstr != NULL)
	    {
	      bfd_release (abfd, info->dynstr);
	      info->dynstr = NULL;
	    }
	  return -1;
	}
    }

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
    table = (bfd_byte *) malloc (table_size);
    if (table == NULL && table_size != 0)
      abort ();
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
	while (GET_WORD (abfd, table + hash * HASH_ENTRY_SIZE) != i)
	  {
	    hash = GET_WORD (abfd,
			     table + hash * HASH_ENTRY_SIZE + BYTES_IN_WORD);
	    if (hash == 0 || hash >= table_size / HASH_ENTRY_SIZE)
	      abort ();
	  }
      }
    free (table);
  }
#endif /* CHECK_DYNAMIC_HASH */

  /* Get the asymbol structures corresponding to the dynamic nlist
     structures.  */
  if (info->canonical_dynsym == (aout_symbol_type *) NULL)
    {
      info->canonical_dynsym = ((aout_symbol_type *)
				bfd_alloc (abfd,
					   (info->dynsym_count
					    * sizeof (aout_symbol_type))));
      if (info->canonical_dynsym == NULL && info->dynsym_count != 0)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return -1;
	}

      if (! aout_32_translate_symbol_table (abfd, info->canonical_dynsym,
					    info->dynsym, info->dynsym_count,
					    info->dynstr,
					    info->dyninfo.ld_symb_size,
					    true))
	{
	  if (info->canonical_dynsym != NULL)
	    {
	      bfd_release (abfd, info->canonical_dynsym);
	      info->canonical_dynsym = NULL;
	    }
	  return -1;
	}
    }

  /* Return pointers to the dynamic asymbol structures.  */
  for (i = 0; i < info->dynsym_count; i++)
    *storage++ = (asymbol *) (info->canonical_dynsym + i);
  *storage = NULL;

  return info->dynsym_count;
}

/* Return the amount of memory required for the dynamic relocs.  */

static long
sunos_get_dynamic_reloc_upper_bound (abfd)
     bfd *abfd;
{
  struct sunos_dynamic_info *info;

  if (! sunos_read_dynamic_info (abfd))
    return -1;

  info = (struct sunos_dynamic_info *) obj_aout_dynamic_info (abfd);
  if (! info->valid)
    {
      bfd_set_error (bfd_error_no_symbols);
      return -1;
    }

  return (info->dynrel_count + 1) * sizeof (arelent *);
}

/* Read in the dynamic relocs.  */

static long
sunos_canonicalize_dynamic_reloc (abfd, storage, syms)
     bfd *abfd;
     arelent **storage;
     asymbol **syms;
{
  struct sunos_dynamic_info *info;
  long i;

  /* Get the general dynamic information.  */
  if (obj_aout_dynamic_info (abfd) == (PTR) NULL)
    {
      if (! sunos_read_dynamic_info (abfd))
	return -1;
    }

  info = (struct sunos_dynamic_info *) obj_aout_dynamic_info (abfd);
  if (! info->valid)
    {
      bfd_set_error (bfd_error_no_symbols);
      return -1;
    }

  /* Get the dynamic reloc information.  */
  if (info->dynrel == NULL)
    {
      info->dynrel = (PTR) bfd_alloc (abfd,
				      (info->dynrel_count
				       * obj_reloc_entry_size (abfd)));
      if (info->dynrel == NULL && info->dynrel_count != 0)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return -1;
	}
      if (bfd_seek (abfd, info->dyninfo.ld_rel, SEEK_SET) != 0
	  || (bfd_read ((PTR) info->dynrel, info->dynrel_count,
			obj_reloc_entry_size (abfd), abfd)
	      != info->dynrel_count * obj_reloc_entry_size (abfd)))
	{
	  if (info->dynrel != NULL)
	    {
	      bfd_release (abfd, info->dynrel);
	      info->dynrel = NULL;
	    }
	  return -1;
	}
    }

  /* Get the arelent structures corresponding to the dynamic reloc
     information.  */
  if (info->canonical_dynrel == (arelent *) NULL)
    {
      arelent *to;

      info->canonical_dynrel = ((arelent *)
				bfd_alloc (abfd,
					   (info->dynrel_count
					    * sizeof (arelent))));
      if (info->canonical_dynrel == NULL && info->dynrel_count != 0)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return -1;
	}
      
      to = info->canonical_dynrel;

      if (obj_reloc_entry_size (abfd) == RELOC_EXT_SIZE)
	{
	  register struct reloc_ext_external *p;
	  struct reloc_ext_external *pend;

	  p = (struct reloc_ext_external *) info->dynrel;
	  pend = p + info->dynrel_count;
	  for (; p < pend; p++, to++)
	    NAME(aout,swap_ext_reloc_in) (abfd, p, to, syms);
	}
      else
	{
	  register struct reloc_std_external *p;
	  struct reloc_std_external *pend;

	  p = (struct reloc_std_external *) info->dynrel;
	  pend = p + info->dynrel_count;
	  for (; p < pend; p++, to++)
	    NAME(aout,swap_std_reloc_in) (abfd, p, to, syms);
	}
    }

  /* Return pointers to the dynamic arelent structures.  */
  for (i = 0; i < info->dynrel_count; i++)
    *storage++ = info->canonical_dynrel + i;
  *storage = NULL;

  return info->dynrel_count;
}

/* Code to handle linking of SunOS shared libraries.  */

/* A SPARC procedure linkage table entry is 12 bytes.  The first entry
   in the table is a jump which is filled in by the runtime linker.
   The remaining entries are branches back to the first entry,
   followed by an index into the relocation table encoded to look like
   a sethi of %g0.  */

#define SPARC_PLT_ENTRY_SIZE (12)

static const bfd_byte sparc_plt_first_entry[SPARC_PLT_ENTRY_SIZE] =
{
  /* sethi %hi(0),%g1; address filled in by runtime linker.  */
  0x3, 0, 0, 0,
  /* jmp %g1; offset filled in by runtime linker.  */
  0x81, 0xc0, 0x60, 0,
  /* nop */
  0x1, 0, 0, 0
};

/* save %sp, -96, %sp */
#define SPARC_PLT_ENTRY_WORD0 0x9de3bfa0
/* call; address filled in later.  */
#define SPARC_PLT_ENTRY_WORD1 0x40000000
/* sethi; reloc index filled in later.  */
#define SPARC_PLT_ENTRY_WORD2 0x01000000

/* An m68k procedure linkage table entry is 8 bytes.  The first entry
   in the table is a jump which is filled in the by the runtime
   linker.  The remaining entries are branches back to the first
   entry, followed by a two byte index into the relocation table.  */

#define M68K_PLT_ENTRY_SIZE (8)

static const bfd_byte m68k_plt_first_entry[M68K_PLT_ENTRY_SIZE] =
{
  /* jmps @# */
  0x4e, 0xf9,
  /* Filled in by runtime linker with a magic address.  */
  0, 0, 0, 0,
  /* Not used?  */
  0, 0
};

/* bsrl */
#define M68K_PLT_ENTRY_WORD0 (0x61ff)
/* Remaining words filled in later.  */

/* An entry in the SunOS linker hash table.  */

struct sunos_link_hash_entry
{
  struct aout_link_hash_entry root;

  /* If this is a dynamic symbol, this is its index into the dynamic
     symbol table.  This is initialized to -1.  As the linker looks at
     the input files, it changes this to -2 if it will be added to the
     dynamic symbol table.  After all the input files have been seen,
     the linker will know whether to build a dynamic symbol table; if
     it does build one, this becomes the index into the table.  */
  long dynindx;

  /* If this is a dynamic symbol, this is the index of the name in the
     dynamic symbol string table.  */
  long dynstr_index;

  /* Some linker flags.  */
  unsigned char flags;
  /* Symbol is referenced by a regular object.  */
#define SUNOS_REF_REGULAR 01
  /* Symbol is defined by a regular object.  */
#define SUNOS_DEF_REGULAR 02
  /* Symbol is referenced by a dynamic object.  */
#define SUNOS_REF_DYNAMIC 010
  /* Symbol is defined by a dynamic object.  */
#define SUNOS_DEF_DYNAMIC 020
};

/* The SunOS linker hash table.  */

struct sunos_link_hash_table
{
  struct aout_link_hash_table root;

  /* The first dynamic object found during the link.  */
  bfd *dynobj;

  /* The number of dynamic symbols.  */
  size_t dynsymcount;

  /* The number of buckets in the hash table.  */
  size_t bucketcount;
};

/* Routine to create an entry in an SunOS link hash table.  */

static struct bfd_hash_entry *
sunos_link_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  struct sunos_link_hash_entry *ret = (struct sunos_link_hash_entry *) entry;

  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (ret == (struct sunos_link_hash_entry *) NULL)
    ret = ((struct sunos_link_hash_entry *)
	   bfd_hash_allocate (table, sizeof (struct sunos_link_hash_entry)));
  if (ret == (struct sunos_link_hash_entry *) NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return (struct bfd_hash_entry *) ret;
    }

  /* Call the allocation method of the superclass.  */
  ret = ((struct sunos_link_hash_entry *)
	 NAME(aout,link_hash_newfunc) ((struct bfd_hash_entry *) ret,
				       table, string));
  if (ret != NULL)
    {
      /* Set local fields.  */
      ret->dynindx = -1;
      ret->dynstr_index = -1;
      ret->flags = 0;
    }

  return (struct bfd_hash_entry *) ret;
}

/* Create a SunOS link hash table.  */

static struct bfd_link_hash_table *
sunos_link_hash_table_create (abfd)
     bfd *abfd;
{
  struct sunos_link_hash_table *ret;

  ret = ((struct sunos_link_hash_table *)
	 malloc (sizeof (struct sunos_link_hash_table)));
  if (ret == (struct sunos_link_hash_table *) NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return (struct bfd_link_hash_table *) NULL;
    }
  if (! NAME(aout,link_hash_table_init) (&ret->root, abfd,
					 sunos_link_hash_newfunc))
    {
      free (ret);
      return (struct bfd_link_hash_table *) NULL;
    }

  ret->dynobj = NULL;
  ret->dynsymcount = 0;
  ret->bucketcount = 0;

  return &ret->root.root;
}

/* Look up an entry in an SunOS link hash table.  */

#define sunos_link_hash_lookup(table, string, create, copy, follow) \
  ((struct sunos_link_hash_entry *) \
   aout_link_hash_lookup (&(table)->root, (string), (create), (copy),\
			  (follow)))

/* Traverse a SunOS link hash table.  */

#define sunos_link_hash_traverse(table, func, info)			\
  (aout_link_hash_traverse						\
   (&(table)->root,							\
    (boolean (*) PARAMS ((struct aout_link_hash_entry *, PTR))) (func),	\
    (info)))

/* Get the SunOS link hash table from the info structure.  This is
   just a cast.  */

#define sunos_hash_table(p) ((struct sunos_link_hash_table *) ((p)->hash))

static boolean sunos_scan_dynamic_symbol
  PARAMS ((struct sunos_link_hash_entry *, PTR));

/* Add dynamic symbols during a link.  This is called by the a.out
   backend linker when it encounters an object with the DYNAMIC flag
   set.  */

static boolean
sunos_add_dynamic_symbols (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  asection *s;

  /* We do not want to include the sections in a dynamic object in the
     output file.  We hack by simply clobbering the list of sections
     in the BFD.  This could be handled more cleanly by, say, a new
     section flag; the existing SEC_NEVER_LOAD flag is not the one we
     want, because that one still implies that the section takes up
     space in the output file.  */
  abfd->sections = NULL;

  /* The native linker seems to just ignore dynamic objects when -r is
     used.  */
  if (info->relocateable)
    return true;

  /* There's no hope of using a dynamic object which does not exactly
     match the format of the output file.  */
  if (info->hash->creator != abfd->xvec)
    {
      bfd_set_error (bfd_error_invalid_operation);
      return false;
    }

  /* If this is the first dynamic object, create some new sections to
     hold dynamic linking information.  We need to put these sections
     somewhere, and the first dynamic object is as good a place as
     any.  The linker script will look for these special section names
     and put them in the right place in the output file.  See
     include/aout/sun4.h for more details of the dynamic linking
     information.  */
  if (sunos_hash_table (info)->dynobj == NULL)
    {
      flagword flags;
      asection *sdyn;

      sunos_hash_table (info)->dynobj = abfd;
      
      flags = SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY;

      /* The .dynamic section holds the basic dynamic information: the
	 sun4_dynamic structure, the dynamic debugger information, and
	 the sun4_dynamic_link structure.  */
      s = bfd_make_section (abfd, ".dynamic");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, flags)
	  || ! bfd_set_section_alignment (abfd, s, 2))
 	return false;
      sdyn = s;

      /* The .need section holds the list of names of shared objets
	 which must be included at runtime.  The address of this
	 section is put in the ld_need field.  */
      s = bfd_make_section (abfd, ".need");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
	  || ! bfd_set_section_alignment (abfd, s, 2))
	return false;

      /* The .rules section holds the path to search for shared
	 objects.  The address of this section is put in the ld_rules
	 field.  */
      s = bfd_make_section (abfd, ".rules");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
	  || ! bfd_set_section_alignment (abfd, s, 2))
	return false;

      /* The .got section holds the global offset table.  I don't
	 really know how this works, actually.  It seems to only be
	 used for PIC code.  The address minus four is put in the
	 ld_got field.  */
      s = bfd_make_section (abfd, ".got");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, flags)
	  || ! bfd_set_section_alignment (abfd, s, 2))
	return false;
      s->_raw_size = BYTES_IN_WORD;

      /* The .plt section holds the procedure linkage table.  The
	 address is put in the ld_plt field.  */
      s = bfd_make_section (abfd, ".plt");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, flags | SEC_CODE)
	  || ! bfd_set_section_alignment (abfd, s, 2))
	return false;

      /* The .dynrel section holds the dynamic relocs.  The address is
	 put in the ld_rel field.  */
      s = bfd_make_section (abfd, ".dynrel");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
	  || ! bfd_set_section_alignment (abfd, s, 2))
	return false;

      /* The .hash section holds the dynamic hash table.  The address
	 is put in the ld_hash field.  */
      s = bfd_make_section (abfd, ".hash");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
	  || ! bfd_set_section_alignment (abfd, s, 2))
	return false;

      /* The .dynsym section holds the dynamic symbols.  The address
	 is put in the ld_stab field.  */
      s = bfd_make_section (abfd, ".dynsym");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
	  || ! bfd_set_section_alignment (abfd, s, 2))
	return false;

      /* The .dynstr section holds the dynamic symbol string table.
	 The address is put in the ld_symbols field.  */
      s = bfd_make_section (abfd, ".dynstr");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
	  || ! bfd_set_section_alignment (abfd, s, 2))
	return false;
    }

  return true;
}

/* Function to add a single symbol to the linker hash table.  This is
   a wrapper around _bfd_generic_link_add_one_symbol which handles the
   tweaking needed for dynamic linking support.  */

static boolean
sunos_add_one_symbol (info, abfd, name, flags, section, value, string,
		      copy, collect, hashp)
     struct bfd_link_info *info;
     bfd *abfd;
     const char *name;
     flagword flags;
     asection *section;
     bfd_vma value;
     const char *string;
     boolean copy;
     boolean collect;
     struct bfd_link_hash_entry **hashp;
{
  struct sunos_link_hash_entry *h;
  int new_flag;

  h = sunos_link_hash_lookup (sunos_hash_table (info), name, true, copy,
			      false);
  if (h == NULL)
    return false;

  if (hashp != NULL)
    *hashp = (struct bfd_link_hash_entry *) h;

  /* Treat a common symbol in a dynamic object as defined in the .bss
     section of the dynamic object.  We don't want to allocate space
     for it in our process image.  */
  if ((abfd->flags & DYNAMIC) != 0
      && bfd_is_com_section (section))
    section = obj_bsssec (abfd);

  if (! bfd_is_und_section (section)
      && h->root.root.type != bfd_link_hash_new
      && h->root.root.type != bfd_link_hash_undefined)
    {
      /* We are defining the symbol, and it is already defined.  This
	 is a potential multiple definition error.  */
      if ((abfd->flags & DYNAMIC) != 0)
	{
	  /* The definition we are adding is from a dynamic object.
	     We do not want this new definition to override the
	     existing definition, so we pretend it is just a
	     reference.  */
	  section = bfd_und_section_ptr;
	}
      else if ((h->root.root.type == bfd_link_hash_defined
		&& h->root.root.u.def.section->owner != NULL
		&& (h->root.root.u.def.section->owner->flags & DYNAMIC) != 0)
	       || (h->root.root.type == bfd_link_hash_common
		   && ((h->root.root.u.c.section->owner->flags & DYNAMIC)
		       != 0)))
	{
	  /* The existing definition is from a dynamic object.  We
	     want to override it with the definition we just found.
	     Clobber the existing definition.  */
	  h->root.root.type = bfd_link_hash_new;
	}
    }

  /* Do the usual procedure for adding a symbol.  */
  if (! _bfd_generic_link_add_one_symbol (info, abfd, name, flags, section,
					  value, string, copy, collect,
					  hashp))
    return false;

  if (abfd->xvec == info->hash->creator)
    {
      /* Set a flag in the hash table entry indicating the type of
	 reference or definition we just found.  Keep a count of the
	 number of dynamic symbols we find.  A dynamic symbol is one
	 which is referenced or defined by both a regular object and a
	 shared object.  */
      if ((abfd->flags & DYNAMIC) == 0)
	{
	  if (bfd_is_und_section (section))
	    new_flag = SUNOS_REF_REGULAR;
	  else
	    new_flag = SUNOS_DEF_REGULAR;
	}
      else
	{
	  if (bfd_is_und_section (section))
	    new_flag = SUNOS_REF_DYNAMIC;
	  else
	    new_flag = SUNOS_DEF_DYNAMIC;
	}
      h->flags |= new_flag;

      if (h->dynindx == -1
	  && (h->flags & (SUNOS_DEF_REGULAR | SUNOS_REF_REGULAR)) != 0)
	{
	  ++sunos_hash_table (info)->dynsymcount;
	  h->dynindx = -2;
	}
    }

  return true;
}

/* Record an assignment made to a symbol by a linker script.  We need
   this in case some dynamic object refers to this symbol.  */

boolean
bfd_sunos_record_link_assignment (output_bfd, info, name)
     bfd *output_bfd;
     struct bfd_link_info *info;
     const char *name;
{
  struct sunos_link_hash_entry *h;

  /* This is called after we have examined all the input objects.  If
     the symbol does not exist, it merely means that no object refers
     to it, and we can just ignore it at this point.  */
  h = sunos_link_hash_lookup (sunos_hash_table (info), name,
			      false, false, false);
  if (h == NULL)
    return true;

  h->flags |= SUNOS_DEF_REGULAR;

  if (h->dynindx == -1)
    {
      ++sunos_hash_table (info)->dynsymcount;
      h->dynindx = -2;
    }

  return true;
}

/* Set up the sizes and contents of the dynamic sections created in
   sunos_add_dynamic_symbols.  This is called by the SunOS linker
   emulation before_allocation routine.  We must set the sizes of the
   sections before the linker sets the addresses of the various
   sections.  This unfortunately requires reading all the relocs so
   that we can work out which ones need to become dynamic relocs.  If
   info->keep_memory is true, we keep the relocs in memory; otherwise,
   we discard them, and will read them again later.  */

boolean
bfd_sunos_size_dynamic_sections (output_bfd, info, sdynptr, sneedptr,
				 srulesptr)
     bfd *output_bfd;
     struct bfd_link_info *info;
     asection **sdynptr;
     asection **sneedptr;
     asection **srulesptr;
{
  bfd *dynobj;
  size_t dynsymcount;
  asection *s;
  size_t bucketcount;
  size_t hashalloc;
  size_t i;
  bfd *sub;

  *sdynptr = NULL;
  *sneedptr = NULL;
  *srulesptr = NULL;

  dynobj = sunos_hash_table (info)->dynobj;
  dynsymcount = sunos_hash_table (info)->dynsymcount;

  /* If there were no dynamic objects in the link, there is nothing to
     do here.  */
  if (dynobj == NULL)
    return true;

  /* The .dynamic section is always the same size.  */
  s = bfd_get_section_by_name (dynobj, ".dynamic");
  BFD_ASSERT (s != NULL);
  s->_raw_size = (sizeof (struct external_sun4_dynamic)
		  + EXTERNAL_SUN4_DYNAMIC_DEBUGGER_SIZE
		  + sizeof (struct external_sun4_dynamic_link));

  /* Set the size of the .dynsym and .hash sections.  We counted the
     number of dynamic symbols as we read the input files.  We will
     build the dynamic symbol table (.dynsym) and the hash table
     (.hash) when we build the final symbol table, because until then
     we do not know the correct value to give the symbols.  We build
     the dynamic symbol string table (.dynstr) in a traversal of the
     symbol table using sunos_scan_dynamic_symbol.  */
  s = bfd_get_section_by_name (dynobj, ".dynsym");
  BFD_ASSERT (s != NULL);
  s->_raw_size = dynsymcount * sizeof (struct external_nlist);
  s->contents = (bfd_byte *) bfd_alloc (output_bfd, s->_raw_size);
  if (s->contents == NULL && s->_raw_size != 0)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
      
  /* The number of buckets is just the number of symbols divided by
     four.  The compute the final size of the hash table, we must
     actually compute the hash table.  Normally we need exactly as
     many entries in the hash table as there are dynamic symbols, but
     if some of the buckets are not used we will need additional
     entries.  In the worse case, every symbol will hash to the same
     bucket, and we will need BUCKETCOUNT - 1 extra entries.  */
  if (dynsymcount >= 4)
    bucketcount = dynsymcount / 4;
  else if (dynsymcount > 0)
    bucketcount = dynsymcount;
  else
    bucketcount = 1;
  s = bfd_get_section_by_name (dynobj, ".hash");
  BFD_ASSERT (s != NULL);
  hashalloc = (dynsymcount + bucketcount - 1) * HASH_ENTRY_SIZE;
  s->contents = (bfd_byte *) bfd_alloc (dynobj, hashalloc);
  if (s->contents == NULL && dynsymcount > 0)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  memset (s->contents, 0, hashalloc);
  for (i = 0; i < bucketcount; i++)
    PUT_WORD (output_bfd, (bfd_vma) -1, s->contents + i * HASH_ENTRY_SIZE);
  s->_raw_size = bucketcount * HASH_ENTRY_SIZE;

  sunos_hash_table (info)->bucketcount = bucketcount;

  /* Look through all the input BFD's and read their relocs.  It would
     be better if we didn't have to do this, but there is no other way
     to determine the number of dynamic relocs we need, and, more
     importantly, there is no other way to know which symbols should
     get an entry in the procedure linkage table.  */
  for (sub = info->input_bfds; sub != NULL; sub = sub->link_next)
    {
      if ((sub->flags & DYNAMIC) == 0)
	{
	  if (! sunos_scan_relocs (info, sub, obj_textsec (sub),
				   exec_hdr (sub)->a_trsize)
	      || ! sunos_scan_relocs (info, sub, obj_datasec (sub),
				      exec_hdr (sub)->a_drsize))
	    return false;
	}
    }

  /* Scan all the symbols, place them in the dynamic symbol table, and
     build the dynamic hash table.  We reuse dynsymcount as a counter
     for the number of symbols we have added so far.  */
  sunos_hash_table (info)->dynsymcount = 0;
  sunos_link_hash_traverse (sunos_hash_table (info),
			    sunos_scan_dynamic_symbol,
			    (PTR) info);
  BFD_ASSERT (sunos_hash_table (info)->dynsymcount == dynsymcount);

  /* The SunOS native linker seems to align the total size of the
     symbol strings to a multiple of 8.  I don't know if this is
     important, but it can't hurt much.  */
  s = bfd_get_section_by_name (dynobj, ".dynstr");
  BFD_ASSERT (s != NULL);
  if ((s->_raw_size & 7) != 0)
    {
      bfd_size_type add;
      bfd_byte *contents;

      add = 8 - (s->_raw_size & 7);
      contents = (bfd_byte *) realloc (s->contents, s->_raw_size + add);
      if (contents == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}
      memset (contents + s->_raw_size, 0, add);
      s->contents = contents;
      s->_raw_size += add;
    }

  /* Now that we have worked out the sizes of the procedure linkage
     table and the dynamic relocs, allocate storage for them.  */
  s = bfd_get_section_by_name (dynobj, ".plt");
  BFD_ASSERT (s != NULL);
  if (s->_raw_size != 0)
    {
      s->contents = (bfd_byte *) bfd_alloc (dynobj, s->_raw_size);
      if (s->contents == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}

      /* Fill in the first entry in the table.  */
      switch (bfd_get_arch (dynobj))
	{
	case bfd_arch_sparc:
	  memcpy (s->contents, sparc_plt_first_entry, SPARC_PLT_ENTRY_SIZE);
	  break;

	case bfd_arch_m68k:
	  memcpy (s->contents, m68k_plt_first_entry, M68K_PLT_ENTRY_SIZE);
	  break;

	default:
	  abort ();
	}
    }

  s = bfd_get_section_by_name (dynobj, ".dynrel");
  if (s->_raw_size != 0)
    {
      s->contents = (bfd_byte *) bfd_alloc (dynobj, s->_raw_size);
      if (s->contents == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}
    }
  /* We use the reloc_count field to keep track of how many of the
     relocs we have output so far.  */
  s->reloc_count = 0;

  /* Make space for the global offset table.  */
  s = bfd_get_section_by_name (dynobj, ".got");
  s->contents = (bfd_byte *) bfd_alloc (dynobj, s->_raw_size);
  if (s->contents == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }

  *sdynptr = bfd_get_section_by_name (dynobj, ".dynamic");
  *sneedptr = bfd_get_section_by_name (dynobj, ".need");
  *srulesptr = bfd_get_section_by_name (dynobj, ".rules");

  return true;
}

/* Scan the relocs for an input section.  */

static boolean
sunos_scan_relocs (info, abfd, sec, rel_size)
     struct bfd_link_info *info;
     bfd *abfd;
     asection *sec;
     bfd_size_type rel_size;
{
  PTR relocs;
  PTR free_relocs = NULL;

  if (rel_size == 0)
    return true;

  if (! info->keep_memory)
    relocs = free_relocs = malloc (rel_size);
  else
    {
      aout_section_data (sec) =
	((struct aout_section_data_struct *)
	 bfd_alloc (abfd, sizeof (struct aout_section_data_struct)));
      if (aout_section_data (sec) == NULL)
	relocs = NULL;
      else
	relocs = aout_section_data (sec)->relocs = malloc (rel_size);
    }
  if (relocs == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }

  if (bfd_seek (abfd, sec->rel_filepos, SEEK_SET) != 0
      || bfd_read (relocs, 1, rel_size, abfd) != rel_size)
    goto error_return;

  if (obj_reloc_entry_size (abfd) == RELOC_STD_SIZE)
    {
      if (! sunos_scan_std_relocs (info, abfd, sec,
				   (struct reloc_std_external *) relocs,
				   rel_size))
	goto error_return;
    }
  else
    {
      if (! sunos_scan_ext_relocs (info, abfd, sec,
				   (struct reloc_ext_external *) relocs,
				   rel_size))
	goto error_return;
    }

  if (free_relocs != NULL)
    free (free_relocs);

  return true;

 error_return:
  if (free_relocs != NULL)
    free (free_relocs);
  return false;
}

/* Scan the relocs for an input section using standard relocs.  We
   need to figure out what to do for each reloc against a dynamic
   symbol.  If the symbol is in the .text section, an entry is made in
   the procedure linkage table.  Note that this will do the wrong
   thing if the symbol is actually data; I don't think the Sun 3
   native linker handles this case correctly either.  If the symbol is
   not in the .text section, we must preserve the reloc as a dynamic
   reloc.  FIXME: We should also handle the PIC relocs here by
   building global offset table entries.  */

static boolean
sunos_scan_std_relocs (info, abfd, sec, relocs, rel_size)
     struct bfd_link_info *info;
     bfd *abfd;
     asection *sec;
     const struct reloc_std_external *relocs;
     bfd_size_type rel_size;
{
  bfd *dynobj;
  asection *splt;
  asection *srel;
  struct sunos_link_hash_entry **sym_hashes;
  const struct reloc_std_external *rel, *relend;

  /* We only know how to handle m68k plt entries.  */
  if (bfd_get_arch (abfd) != bfd_arch_m68k)
    {
      bfd_set_error (bfd_error_invalid_target);
      return false;
    }

  dynobj = sunos_hash_table (info)->dynobj;
  splt = bfd_get_section_by_name (dynobj, ".plt");
  srel = bfd_get_section_by_name (dynobj, ".dynrel");
  BFD_ASSERT (splt != NULL && srel != NULL);
  sym_hashes = (struct sunos_link_hash_entry **) obj_aout_sym_hashes (abfd);

  relend = relocs + rel_size / RELOC_STD_SIZE;
  for (rel = relocs; rel < relend; rel++)
    {
      int r_index;
      struct sunos_link_hash_entry *h;

      /* We only want relocs against external symbols.  */
      if (abfd->xvec->header_byteorder_big_p)
	{
	  if ((rel->r_type[0] & RELOC_STD_BITS_EXTERN_BIG) == 0)
	    continue;
	}
      else
	{
	  if ((rel->r_type[0] & RELOC_STD_BITS_EXTERN_LITTLE) == 0)
	    continue;
	}

      /* Get the symbol index.  */
      if (abfd->xvec->header_byteorder_big_p)
	{
	  r_index   =  ((rel->r_index[0] << 16)
			| (rel->r_index[1] << 8)
			| rel->r_index[2]);
	}
      else
	{
	  r_index   = ((rel->r_index[2] << 16)
		       | (rel->r_index[1] << 8)
		       | rel->r_index[0]);
	}

      /* Get the hash table entry.  */
      h = sym_hashes[r_index];
      if (h == NULL)
	{
	  /* This should not normally happen, but it will in any case
	     be caught in the relocation phase.  */
	  continue;
	}

      /* At this point common symbols have already been allocated, so
	 we don't have to worry about them.  We need to consider that
	 we may have already seen this symbol and marked it undefined;
	 if the symbols is really undefined, then SUNOS_DEF_DYNAMIC
	 will be zero.  */
      if (h->root.root.type != bfd_link_hash_defined
	  && h->root.root.type != bfd_link_hash_undefined)
	continue;

      if ((h->flags & SUNOS_DEF_DYNAMIC) == 0
	  || (h->flags & SUNOS_DEF_REGULAR) != 0)
	continue;

      BFD_ASSERT ((h->flags & SUNOS_REF_REGULAR) != 0);
      BFD_ASSERT (h->root.root.type == bfd_link_hash_defined
		  ? (h->root.root.u.def.section->owner->flags & DYNAMIC) != 0
		  : (h->root.root.u.undef.abfd->flags & DYNAMIC) != 0);

      /* This reloc is against a symbol defined only by a dynamic
	 object.  */

      if (h->root.root.type == bfd_link_hash_undefined)
	{
	  /* Presumably this symbol was marked as being undefined by
	     an earlier reloc.  */
	  srel->_raw_size += RELOC_STD_SIZE;
	}
      else if ((h->root.root.u.def.section->flags & SEC_CODE) == 0)
	{
	  bfd *sub;

	  /* This reloc is not in the .text section.  It must be
	     copied into the dynamic relocs.  We mark the symbol as
	     being undefined.  */
	  srel->_raw_size += RELOC_STD_SIZE;
	  sub = h->root.root.u.def.section->owner;
	  h->root.root.type = bfd_link_hash_undefined;
	  h->root.root.u.undef.abfd = sub;
	}
      else
	{
	  /* This symbol is in the .text section.  We must give it an
	     entry in the procedure linkage table, if we have not
	     already done so.  We change the definition of the symbol
	     to the .plt section; this will cause relocs against it to
	     be handled correctly.  */
	  if (h->root.root.u.def.section != splt)
	    {
	      if (splt->_raw_size == 0)
		splt->_raw_size = M68K_PLT_ENTRY_SIZE;
	      h->root.root.u.def.section = splt;
	      h->root.root.u.def.value = splt->_raw_size;
	      splt->_raw_size += M68K_PLT_ENTRY_SIZE;

	      /* We will also need a dynamic reloc entry.  */
	      srel->_raw_size += RELOC_STD_SIZE;
	    }
	}
    }

  return true;
}

/* Scan the relocs for an input section using extended relocs.  We
   need to figure out what to do for each reloc against a dynamic
   symbol.  If the reloc is a WDISP30, and the symbol is in the .text
   section, an entry is made in the procedure linkage table.
   Otherwise, we must preserve the reloc as a dynamic reloc.  FIXME:
   We should also handle the PIC relocs here by building global offset
   table entries.  */

static boolean
sunos_scan_ext_relocs (info, abfd, sec, relocs, rel_size)
     struct bfd_link_info *info;
     bfd *abfd;
     asection *sec;
     const struct reloc_ext_external *relocs;
     bfd_size_type rel_size;
{
  bfd *dynobj;
  asection *splt;
  asection *srel;
  struct sunos_link_hash_entry **sym_hashes;
  const struct reloc_ext_external *rel, *relend;

  /* We only know how to handle SPARC plt entries.  */
  if (bfd_get_arch (abfd) != bfd_arch_sparc)
    {
      bfd_set_error (bfd_error_invalid_target);
      return false;
    }

  dynobj = sunos_hash_table (info)->dynobj;
  splt = bfd_get_section_by_name (dynobj, ".plt");
  srel = bfd_get_section_by_name (dynobj, ".dynrel");
  BFD_ASSERT (splt != NULL && srel != NULL);
  sym_hashes = (struct sunos_link_hash_entry **) obj_aout_sym_hashes (abfd);

  relend = relocs + rel_size / RELOC_EXT_SIZE;
  for (rel = relocs; rel < relend; rel++)
    {
      int r_index;
      int r_type;
      struct sunos_link_hash_entry *h;

      /* We only want relocs against external symbols.  */
      if (abfd->xvec->header_byteorder_big_p)
	{
	  if ((rel->r_type[0] & RELOC_EXT_BITS_EXTERN_BIG) == 0)
	    continue;
	}
      else
	{
	  if ((rel->r_type[0] & RELOC_EXT_BITS_EXTERN_LITTLE) == 0)
	    continue;
	}

      /* Get the symbol index and reloc type.  */
      if (abfd->xvec->header_byteorder_big_p)
	{
	  r_index   =  ((rel->r_index[0] << 16)
			| (rel->r_index[1] << 8)
			| rel->r_index[2]);
	  r_type   = ((rel->r_type[0] & RELOC_EXT_BITS_TYPE_BIG)
		      >> RELOC_EXT_BITS_TYPE_SH_BIG);
	}
      else
	{
	  r_index   = ((rel->r_index[2] << 16)
		       | (rel->r_index[1] << 8)
		       | rel->r_index[0]);
	  r_type   = ((rel->r_type[0] & RELOC_EXT_BITS_TYPE_LITTLE)
		      >> RELOC_EXT_BITS_TYPE_SH_LITTLE);
	}

      /* Get the hash table entry.  */
      h = sym_hashes[r_index];
      if (h == NULL)
	{
	  /* This should not normally happen, but it will in any case
	     be caught in the relocation phase.  */
	  continue;
	}

      /* At this point common symbols have already been allocated, so
	 we don't have to worry about them.  We need to consider that
	 we may have already seen this symbol and marked it undefined;
	 if the symbols is really undefined, then SUNOS_DEF_DYNAMIC
	 will be zero.  */
      if (h->root.root.type != bfd_link_hash_defined
	  && h->root.root.type != bfd_link_hash_undefined)
	continue;

      if ((h->flags & SUNOS_DEF_DYNAMIC) == 0
	  || (h->flags & SUNOS_DEF_REGULAR) != 0)
	continue;

      BFD_ASSERT ((h->flags & SUNOS_REF_REGULAR) != 0);
      BFD_ASSERT (h->root.root.type == bfd_link_hash_defined
		  ? (h->root.root.u.def.section->owner->flags & DYNAMIC) != 0
		  : (h->root.root.u.undef.abfd->flags & DYNAMIC) != 0);

      /* This reloc is against a symbol defined only by a dynamic
	 object.  */

      if (h->root.root.type == bfd_link_hash_undefined)
	{
	  /* Presumably this symbol was marked as being undefined by
	     an earlier reloc.  */
	  srel->_raw_size += RELOC_EXT_SIZE;
	}
      else if ((h->root.root.u.def.section->flags & SEC_CODE) == 0)
	{
	  bfd *sub;

	  /* This reloc is not in the .text section.  It must be
	     copied into the dynamic relocs.  We mark the symbol as
	     being undefined.  */
	  srel->_raw_size += RELOC_EXT_SIZE;
	  sub = h->root.root.u.def.section->owner;
	  h->root.root.type = bfd_link_hash_undefined;
	  h->root.root.u.undef.abfd = sub;
	}
      else
	{
	  /* This symbol is in the .text section.  We must give it an
	     entry in the procedure linkage table, if we have not
	     already done so.  We change the definition of the symbol
	     to the .plt section; this will cause relocs against it to
	     be handled correctly.  */
	  if (h->root.root.u.def.section != splt)
	    {
	      if (splt->_raw_size == 0)
		splt->_raw_size = SPARC_PLT_ENTRY_SIZE;
	      h->root.root.u.def.section = splt;
	      h->root.root.u.def.value = splt->_raw_size;
	      splt->_raw_size += SPARC_PLT_ENTRY_SIZE;

	      /* We will also need a dynamic reloc entry.  */
	      srel->_raw_size += RELOC_EXT_SIZE;
	    }
	}
    }

  return true;
}

/* Build the hash table of dynamic symbols, and to mark as written all
   symbols from dynamic objects which we do not plan to write out.  */

static boolean
sunos_scan_dynamic_symbol (h, data)
     struct sunos_link_hash_entry *h;
     PTR data;
{
  struct bfd_link_info *info = (struct bfd_link_info *) data;

  /* Set the written flag for symbols we do not want to write out as
     part of the regular symbol table.  This is all symbols which are
     not defined in a regular object file.  For some reason symbols
     which are referenced by a regular object and defined by a dynamic
     object do not seem to show up in the regular symbol table.  */
  if ((h->flags & SUNOS_DEF_REGULAR) == 0)
    h->root.written = true;

  /* If this symbol is defined by a dynamic object and referenced by a
     regular object, see whether we gave it a reasonable value while
     scanning the relocs.  */

  if ((h->flags & SUNOS_DEF_REGULAR) == 0
      && (h->flags & SUNOS_DEF_DYNAMIC) != 0
      && (h->flags & SUNOS_REF_REGULAR) != 0)
    {
      if (h->root.root.type == bfd_link_hash_defined
	  && ((h->root.root.u.def.section->owner->flags & DYNAMIC) != 0)
	  && h->root.root.u.def.section->output_section == NULL)
	{
	  bfd *sub;

	  /* This symbol is currently defined in a dynamic section
	     which is not being put into the output file.  This
	     implies that there is no reloc against the symbol.  I'm
	     not sure why this case would ever occur.  In any case, we
	     change the symbol to be undefined.  */
	  sub = h->root.root.u.def.section->owner;
	  h->root.root.type = bfd_link_hash_undefined;
	  h->root.root.u.undef.abfd = sub;
	}
    }

  /* If this symbol is defined or referenced by a regular file, add it
     to the dynamic symbols.  */
  if ((h->flags & (SUNOS_DEF_REGULAR | SUNOS_REF_REGULAR)) != 0)
    {
      asection *s;
      size_t len;
      bfd_byte *contents;
      unsigned char *name;
      unsigned long hash;
      bfd *dynobj;

      BFD_ASSERT (h->dynindx == -2);

      h->dynindx = sunos_hash_table (info)->dynsymcount;
      ++sunos_hash_table (info)->dynsymcount;

      len = strlen (h->root.root.root.string);

      /* We don't bother to construct a BFD hash table for the strings
	 which are the names of the dynamic symbols.  Using a hash
	 table for the regular symbols is beneficial, because the
	 regular symbols includes the debugging symbols, which have
	 long names and are often duplicated in several object files.
	 There are no debugging symbols in the dynamic symbols.  */
      s = bfd_get_section_by_name (sunos_hash_table (info)->dynobj,
				   ".dynstr");
      BFD_ASSERT (s != NULL);
      if (s->contents == NULL)
	contents = (bfd_byte *) malloc (len + 1);
      else
	contents = (bfd_byte *) realloc (s->contents, s->_raw_size + len + 1);
      if (contents == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}
      s->contents = contents;

      h->dynstr_index = s->_raw_size;
      strcpy (contents + s->_raw_size, h->root.root.root.string);
      s->_raw_size += len + 1;

      /* Add it to the dynamic hash table.  */
      name = (unsigned char *) h->root.root.root.string;
      hash = 0;
      while (*name != '\0')
	hash = (hash << 1) + *name++;
      hash &= 0x7fffffff;
      hash %= sunos_hash_table (info)->bucketcount;

      dynobj = sunos_hash_table (info)->dynobj;
      s = bfd_get_section_by_name (dynobj, ".hash");
      BFD_ASSERT (s != NULL);

      if (GET_SWORD (dynobj, s->contents + hash * HASH_ENTRY_SIZE) == -1)
	PUT_WORD (dynobj, h->dynindx, s->contents + hash * HASH_ENTRY_SIZE);
      else
	{
	  bfd_vma next;

	  next = GET_WORD (dynobj,
			   (s->contents
			    + hash * HASH_ENTRY_SIZE
			    + BYTES_IN_WORD));
	  PUT_WORD (dynobj, s->_raw_size / HASH_ENTRY_SIZE,
		    s->contents + hash * HASH_ENTRY_SIZE + BYTES_IN_WORD);
	  PUT_WORD (dynobj, h->dynindx, s->contents + s->_raw_size);
	  PUT_WORD (dynobj, next, s->contents + s->_raw_size + BYTES_IN_WORD);
	  s->_raw_size += HASH_ENTRY_SIZE;
	}
    }

  return true;
}

/* Link a dynamic object.  We actually don't have anything to do at
   this point.  This entry point exists to prevent the regular linker
   code from doing anything with the object.  */

/*ARGSUSED*/
static boolean
sunos_link_dynamic_object (info, abfd)
     struct bfd_link_info *info;
     bfd *abfd;
{
  return true;
}


/* Write out a dynamic symbol.  This is called by the final traversal
   over the symbol table.  */

static boolean
sunos_write_dynamic_symbol (output_bfd, info, harg)
     bfd *output_bfd;
     struct bfd_link_info *info;
     struct aout_link_hash_entry *harg;
{
  struct sunos_link_hash_entry *h = (struct sunos_link_hash_entry *) harg;
  boolean plt;
  int type;
  bfd_vma val;
  asection *s;
  struct external_nlist *outsym;

  if (h->dynindx < 0)
    return true;

  plt = false;
  switch (h->root.root.type)
    {
    default:
    case bfd_link_hash_new:
      abort ();
      /* Avoid variable not initialized warnings.  */
      return true;
    case bfd_link_hash_undefined:
      type = N_UNDF | N_EXT;
      val = 0;
      break;
    case bfd_link_hash_defined:
      {
	asection *sec;
	asection *output_section;

	sec = h->root.root.u.def.section;
	output_section = sec->output_section;
	BFD_ASSERT (bfd_is_abs_section (output_section)
		    || output_section->owner == output_bfd);
	if (strcmp (sec->name, ".plt") == 0)
	  {
	    plt = true;
	    type = N_UNDF | N_EXT;
	    val = 0;
	  }
	else
	  {
	    if (output_section == obj_textsec (output_bfd))
	      type = N_TEXT | N_EXT;
	    else if (output_section == obj_datasec (output_bfd))
	      type = N_DATA | N_EXT;
	    else if (output_section == obj_bsssec (output_bfd))
	      type = N_BSS | N_EXT;
	    else
	      type = N_ABS | N_EXT;
	    val = (h->root.root.u.def.value
		   + output_section->vma
		   + sec->output_offset);
	  }
      }
      break;
    case bfd_link_hash_common:
      type = N_UNDF | N_EXT;
      val = h->root.root.u.c.size;
      break;
    case bfd_link_hash_weak:
      type = N_WEAKU;
      val = 0;
      break;
    case bfd_link_hash_indirect:
    case bfd_link_hash_warning:
      /* FIXME: Ignore these for now.  The circumstances under which
	 they should be written out are not clear to me.  */
      return true;
    }

  s = bfd_get_section_by_name (sunos_hash_table (info)->dynobj, ".dynsym");
  BFD_ASSERT (s != NULL);
  outsym = ((struct external_nlist *)
	    (s->contents + h->dynindx * EXTERNAL_NLIST_SIZE));

  bfd_h_put_8 (output_bfd, type, outsym->e_type);
  bfd_h_put_8 (output_bfd, 0, outsym->e_other);

  /* FIXME: The native linker doesn't use 0 for desc.  It seems to use
     one less than the desc value in the shared library, although that
     seems unlikely.  */
  bfd_h_put_16 (output_bfd, 0, outsym->e_desc);

  PUT_WORD (output_bfd, h->dynstr_index, outsym->e_strx);
  PUT_WORD (output_bfd, val, outsym->e_value);

  /* If this symbol is in the procedure linkage table, fill in the
     table entry.  */
  if (plt)
    {
      bfd_byte *p;
      asection *s;
      bfd_vma r_address;

      p = h->root.root.u.def.section->contents + h->root.root.u.def.value;

      s = bfd_get_section_by_name (sunos_hash_table (info)->dynobj, ".dynrel");
      BFD_ASSERT (s != NULL);

      r_address = (h->root.root.u.def.section->output_section->vma
		   + h->root.root.u.def.section->output_offset
		   + h->root.root.u.def.value);

      switch (bfd_get_arch (output_bfd))
	{
	case bfd_arch_sparc:
	  bfd_put_32 (output_bfd, SPARC_PLT_ENTRY_WORD0, p);
	  bfd_put_32 (output_bfd,
		      (SPARC_PLT_ENTRY_WORD1
		       + (((- (h->root.root.u.def.value + 4) >> 2)
			   & 0x3fffffff))),
		      p + 4);
	  bfd_put_32 (output_bfd, SPARC_PLT_ENTRY_WORD2 + s->reloc_count,
		      p + 8);
	  break;

	case bfd_arch_m68k:
	  bfd_put_16 (output_bfd, M68K_PLT_ENTRY_WORD0, p);
	  bfd_put_32 (output_bfd, (- (h->root.root.u.def.value + 2)), p + 2);
	  bfd_put_16 (output_bfd, s->reloc_count, p + 6);
	  r_address += 2;
	  break;

	default:
	  abort ();
	}

      /* We also need to add a jump table reloc.  */
      p = s->contents + s->reloc_count * obj_reloc_entry_size (output_bfd);
      if (obj_reloc_entry_size (output_bfd) == RELOC_STD_SIZE)
	{
	  struct reloc_std_external *srel;

	  srel = (struct reloc_std_external *) p;
	  PUT_WORD (output_bfd, r_address, srel->r_address);
	  if (output_bfd->xvec->header_byteorder_big_p)
	    {
	      srel->r_index[0] = h->dynindx >> 16;
	      srel->r_index[1] = h->dynindx >> 8;
	      srel->r_index[2] = h->dynindx;
	      srel->r_type[0] = (RELOC_STD_BITS_EXTERN_BIG
				 | RELOC_STD_BITS_JMPTABLE_BIG);
	    }
	  else
	    {
	      srel->r_index[2] = h->dynindx >> 16;
	      srel->r_index[1] = h->dynindx >> 8;
	      srel->r_index[0] = h->dynindx;
	      srel->r_type[0] = (RELOC_STD_BITS_EXTERN_LITTLE
				 | RELOC_STD_BITS_JMPTABLE_LITTLE);
	    }
	}
      else
	{
	  struct reloc_ext_external *erel;

	  erel = (struct reloc_ext_external *) p;
	  PUT_WORD (output_bfd, r_address, erel->r_address);
	  if (output_bfd->xvec->header_byteorder_big_p)
	    {
	      erel->r_index[0] = h->dynindx >> 16;
	      erel->r_index[1] = h->dynindx >> 8;
	      erel->r_index[2] = h->dynindx;
	      erel->r_type[0] = (RELOC_EXT_BITS_EXTERN_BIG
				 | (22 << RELOC_EXT_BITS_TYPE_SH_BIG));
	    }
	  else
	    {
	      erel->r_index[2] = h->dynindx >> 16;
	      erel->r_index[1] = h->dynindx >> 8;
	      erel->r_index[0] = h->dynindx;
	      erel->r_type[0] = (RELOC_EXT_BITS_EXTERN_LITTLE
				 | (22 << RELOC_EXT_BITS_TYPE_SH_LITTLE));
	    }
	  PUT_WORD (output_bfd, (bfd_vma) 0, erel->r_addend);
	}

      ++s->reloc_count;
    }

  return true;
}

/* This is called for each reloc against an external symbol.  If this
   is a reloc which are are going to copy as a dynamic reloc, then
   copy it over, and tell the caller to not bother processing this
   reloc.  */

/*ARGSUSED*/
static boolean
sunos_check_dynamic_reloc (info, input_bfd, input_section, harg, reloc, skip)
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     struct aout_link_hash_entry *harg;
     PTR reloc;
     boolean *skip;
{
  struct sunos_link_hash_entry *h = (struct sunos_link_hash_entry *) harg;
  bfd *dynobj;
  asection *srel;
  bfd_byte *p;

  *skip = false;

  dynobj = sunos_hash_table (info)->dynobj;

  if (dynobj == NULL
      || h->dynindx == -1
      || h->root.root.type != bfd_link_hash_undefined
      || (h->flags & SUNOS_DEF_REGULAR) != 0
      || (h->flags & SUNOS_DEF_DYNAMIC) == 0
      || (h->root.root.u.undef.abfd->flags & DYNAMIC) == 0)
    return true;

  /* It looks this is a reloc we are supposed to copy.  */

  srel = bfd_get_section_by_name (dynobj, ".dynrel");
  BFD_ASSERT (srel != NULL);

  p = srel->contents + srel->reloc_count * obj_reloc_entry_size (dynobj);

  /* Copy the reloc over.  */
  memcpy (p, reloc, obj_reloc_entry_size (dynobj));

  /* Adjust the address and symbol index.  */
  if (obj_reloc_entry_size (dynobj) == RELOC_STD_SIZE)
    {
      struct reloc_std_external *srel;

      srel = (struct reloc_std_external *) p;
      PUT_WORD (dynobj,
		(GET_WORD (dynobj, srel->r_address)
		 + input_section->output_section->vma
		 + input_section->output_offset),
		srel->r_address);
      if (dynobj->xvec->header_byteorder_big_p)
	{
	  srel->r_index[0] = h->dynindx >> 16;
	  srel->r_index[1] = h->dynindx >> 8;
	  srel->r_index[2] = h->dynindx;
	}
      else
	{
	  srel->r_index[2] = h->dynindx >> 16;
	  srel->r_index[1] = h->dynindx >> 8;
	  srel->r_index[0] = h->dynindx;
	}
    }
  else
    {
      struct reloc_ext_external *erel;

      erel = (struct reloc_ext_external *) p;
      PUT_WORD (dynobj,
		(GET_WORD (dynobj, erel->r_address)
		 + input_section->output_section->vma
		 + input_section->output_offset),
		erel->r_address);
      if (dynobj->xvec->header_byteorder_big_p)
	{
	  erel->r_index[0] = h->dynindx >> 16;
	  erel->r_index[1] = h->dynindx >> 8;
	  erel->r_index[2] = h->dynindx;
	}
      else
	{
	  erel->r_index[2] = h->dynindx >> 16;
	  erel->r_index[1] = h->dynindx >> 8;
	  erel->r_index[0] = h->dynindx;
	}
    }

  ++srel->reloc_count;

  *skip = true;

  return true;
}

/* Finish up the dynamic linking information.  */

static boolean
sunos_finish_dynamic_link (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  bfd *dynobj;
  asection *o;
  asection *s;
  asection *sdyn;
  struct external_sun4_dynamic esd;
  struct external_sun4_dynamic_link esdl;

  dynobj = sunos_hash_table (info)->dynobj;
  if (dynobj == NULL)
    return true;

  sdyn = bfd_get_section_by_name (dynobj, ".dynamic");
  BFD_ASSERT (sdyn != NULL);

  /* Finish up the .need section.  The linker emulation code filled it
     in, but with offsets from the start of the section instead of
     real addresses.  Now that we know the section location, we can
     fill in the final values.  */
  s = bfd_get_section_by_name (dynobj, ".need");
  BFD_ASSERT (s != NULL);
  if (s->_raw_size != 0)
    {
      file_ptr filepos;
      bfd_byte *p;

      filepos = s->output_section->filepos + s->output_offset;
      p = s->contents;
      while (1)
	{
	  bfd_vma val;

	  PUT_WORD (dynobj, GET_WORD (dynobj, p) + filepos, p);
	  val = GET_WORD (dynobj, p + 12);
	  if (val == 0)
	    break;
	  PUT_WORD (dynobj, val + filepos, p + 12);
	  p += 16;
	}
    }

  /* The first entry in the .got section is the address of the dynamic
     information.  */
  s = bfd_get_section_by_name (dynobj, ".got");
  BFD_ASSERT (s != NULL);
  PUT_WORD (dynobj, sdyn->output_section->vma + sdyn->output_offset,
	    s->contents);

  for (o = dynobj->sections; o != NULL; o = o->next)
    {
      if ((o->flags & SEC_HAS_CONTENTS) != 0
	  && o->contents != NULL)
	{
	  BFD_ASSERT (o->output_section != NULL
		      && o->output_section->owner == abfd);
	  if (! bfd_set_section_contents (abfd, o->output_section,
					  o->contents, o->output_offset,
					  o->_raw_size))
	    return false;
	}
    }

  /* Finish up the dynamic link information.  */
  PUT_WORD (dynobj, (bfd_vma) 3, esd.ld_version);
  PUT_WORD (dynobj,
	    sdyn->output_section->vma + sdyn->output_offset + sizeof esd,
	    esd.ldd);
  PUT_WORD (dynobj,
	    (sdyn->output_section->vma
	     + sdyn->output_offset
	     + sizeof esd
	     + EXTERNAL_SUN4_DYNAMIC_DEBUGGER_SIZE),
	    esd.ld);

  if (! bfd_set_section_contents (abfd, sdyn->output_section, &esd,
				  sdyn->output_offset, sizeof esd))
    return false;


  PUT_WORD (dynobj, (bfd_vma) 0, esdl.ld_loaded);

  s = bfd_get_section_by_name (dynobj, ".need");
  BFD_ASSERT (s != NULL);
  if (s->_raw_size == 0)
    PUT_WORD (dynobj, (bfd_vma) 0, esdl.ld_need);
  else
    PUT_WORD (dynobj, s->output_section->filepos + s->output_offset,
	      esdl.ld_need);

  s = bfd_get_section_by_name (dynobj, ".rules");
  BFD_ASSERT (s != NULL);
  if (s->_raw_size == 0)
    PUT_WORD (dynobj, (bfd_vma) 0, esdl.ld_rules);
  else
    PUT_WORD (dynobj, s->output_section->filepos + s->output_offset,
	      esdl.ld_rules);

  s = bfd_get_section_by_name (dynobj, ".got");
  BFD_ASSERT (s != NULL);
  PUT_WORD (dynobj, s->output_section->vma + s->output_offset, esdl.ld_got);

  s = bfd_get_section_by_name (dynobj, ".plt");
  BFD_ASSERT (s != NULL);
  PUT_WORD (dynobj, s->output_section->vma + s->output_offset, esdl.ld_plt);
  PUT_WORD (dynobj, s->_raw_size, esdl.ld_plt_sz);

  s = bfd_get_section_by_name (dynobj, ".dynrel");
  BFD_ASSERT (s != NULL);
  BFD_ASSERT (s->reloc_count * obj_reloc_entry_size (dynobj) == s->_raw_size);
  PUT_WORD (dynobj, s->output_section->filepos + s->output_offset,
	    esdl.ld_rel);

  s = bfd_get_section_by_name (dynobj, ".hash");
  BFD_ASSERT (s != NULL);
  PUT_WORD (dynobj, s->output_section->filepos + s->output_offset,
	    esdl.ld_hash);

  s = bfd_get_section_by_name (dynobj, ".dynsym");
  BFD_ASSERT (s != NULL);
  PUT_WORD (dynobj, s->output_section->filepos + s->output_offset,
	    esdl.ld_stab);

  PUT_WORD (dynobj, (bfd_vma) 0, esdl.ld_stab_hash);

  PUT_WORD (dynobj, (bfd_vma) sunos_hash_table (info)->bucketcount,
	    esdl.ld_buckets);

  s = bfd_get_section_by_name (dynobj, ".dynstr");
  BFD_ASSERT (s != NULL);
  PUT_WORD (dynobj, s->output_section->filepos + s->output_offset,
	    esdl.ld_symbols);
  PUT_WORD (dynobj, s->_raw_size, esdl.ld_symb_size);

  /* The size of the text area is the size of the .text section
     rounded up to a page boundary.  FIXME: Should the page size be
     conditional on something?  */
  PUT_WORD (dynobj,
	    BFD_ALIGN (obj_textsec (abfd)->_raw_size, 0x2000),
	    esdl.ld_text);
  
  if (! bfd_set_section_contents (abfd, sdyn->output_section, &esdl,
				  (sdyn->output_offset
				   + sizeof esd
				   + EXTERNAL_SUN4_DYNAMIC_DEBUGGER_SIZE),
				  sizeof esdl))
    return false;

  abfd->flags |= DYNAMIC;

  return true;
}
