/* POWER/PowerPC XCOFF linker support.
   Copyright 1995 Free Software Foundation, Inc.
   Written by Ian Lance Taylor <ian@cygnus.com>, Cygnus Support.

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

#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"
#include "libbfd.h"
#include "coff/internal.h"
#include "libcoff.h"

/* This file holds the XCOFF linker code.  A lot of it is very similar
   to the COFF linker code.  However, it is different enough that I
   chose to avoid trying to hack up the COFF code to support XCOFF.
   That leads to a certain amount of duplicated code, alas.  */

#define STRING_SIZE_SIZE (4)

/* Get the XCOFF hash table entries for a BFD.  */
#define obj_xcoff_sym_hashes(bfd) \
  ((struct xcoff_link_hash_entry **) obj_coff_sym_hashes (bfd))

/* An entry in the XCOFF linker hash table.  */

struct xcoff_link_hash_entry
{
  struct bfd_link_hash_entry root;

  /* Whether this symbol came from an XCOFF input file.  */
  boolean xcoff;

  /* Symbol index in output file.  Set to -1 initially.  Set to -2 if
     there is a reloc against this symbol.  */
  long indx;

  /* Whether we have created a TOC entry for this symbol.  */
  boolean toc;
};

/* The XCOFF linker hash table.  */

struct xcoff_link_hash_table
{
  struct bfd_link_hash_table root;

  /* The .debug string hash table.  We need to compute this while
     reading the input files, so that we know how large the .debug
     section will be before we assign section positions.  */
  struct bfd_strtab_hash *debug_strtab;

  /* The .debug section we will use for the final output.  */
  asection *debug_section;
};

/* Information we keep for each section in the output file.  */

struct xcoff_link_section_info
{
  /* The relocs to be output.  */
  struct internal_reloc *relocs;
  /* For each reloc against a global symbol whose index was not known
     when the reloc was handled, the global hash table entry.  */
  struct xcoff_link_hash_entry **rel_hashes;
};

/* Information that we pass around while doing the final link step.  */

struct xcoff_final_link_info
{
  /* General link information.  */
  struct bfd_link_info *info;
  /* Output BFD.  */
  bfd *output_bfd;
  /* Hash table for long symbol names.  */
  struct bfd_strtab_hash *strtab;
  /* Array of information kept for each output section, indexed by the
     target_index field.  */
  struct xcoff_link_section_info *section_info;
  /* Symbol index of last C_FILE symbol (-1 if none).  */
  long last_file_index;
  /* Contents of last C_FILE symbol.  */
  struct internal_syment last_file;
  /* Symbol index of TOC symbol.  */
  long toc_symindx;
  /* Buffer large enough to hold swapped symbols of any input file.  */
  struct internal_syment *internal_syms;
  /* Buffer large enough to hold output indices of symbols of any
     input file.  */
  long *sym_indices;
  /* Buffer large enough to hold output symbols for any input file.  */
  bfd_byte *outsyms;
  /* Buffer large enough to hold external line numbers for any input
     section.  */
  bfd_byte *linenos;
  /* Buffer large enough to hold any input section.  */
  bfd_byte *contents;
  /* Buffer large enough to hold external relocs of any input section.  */
  bfd_byte *external_relocs;
};

static struct bfd_hash_entry *xcoff_link_hash_newfunc
  PARAMS ((struct bfd_hash_entry *, struct bfd_hash_table *, const char *));
static boolean xcoff_link_add_object_symbols
  PARAMS ((bfd *, struct bfd_link_info *));
static boolean xcoff_link_check_archive_element
  PARAMS ((bfd *, struct bfd_link_info *, boolean *));
static boolean xcoff_link_check_ar_symbols
  PARAMS ((bfd *, struct bfd_link_info *, boolean *));
static boolean xcoff_link_add_symbols PARAMS ((bfd *, struct bfd_link_info *));
static boolean xcoff_link_input_bfd
  PARAMS ((struct xcoff_final_link_info *, bfd *));
static boolean xcoff_write_global_symbol
  PARAMS ((struct xcoff_link_hash_entry *, PTR));
static boolean xcoff_reloc_link_order
  PARAMS ((bfd *, struct xcoff_final_link_info *, asection *,
	   struct bfd_link_order *));
static int xcoff_sort_relocs PARAMS ((const PTR, const PTR));

/* Routine to create an entry in an XCOFF link hash table.  */

static struct bfd_hash_entry *
xcoff_link_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  struct xcoff_link_hash_entry *ret = (struct xcoff_link_hash_entry *) entry;

  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (ret == (struct xcoff_link_hash_entry *) NULL)
    ret = ((struct xcoff_link_hash_entry *)
	   bfd_hash_allocate (table, sizeof (struct xcoff_link_hash_entry)));
  if (ret == (struct xcoff_link_hash_entry *) NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return (struct bfd_hash_entry *) ret;
    }

  /* Call the allocation method of the superclass.  */
  ret = ((struct xcoff_link_hash_entry *)
	 _bfd_link_hash_newfunc ((struct bfd_hash_entry *) ret,
				 table, string));
  if (ret != NULL)
    {
      /* Set local fields.  */
      ret->xcoff = false;
      ret->indx = -1;
      ret->toc = false;
    }

  return (struct bfd_hash_entry *) ret;
}

/* Create a XCOFF link hash table.  */

struct bfd_link_hash_table *
_bfd_xcoff_bfd_link_hash_table_create (abfd)
     bfd *abfd;
{
  struct xcoff_link_hash_table *ret;

  ret = ((struct xcoff_link_hash_table *)
	 bfd_alloc (abfd, sizeof (struct xcoff_link_hash_table)));
  if (ret == (struct xcoff_link_hash_table *) NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return (struct bfd_link_hash_table *) NULL;
    }
  if (! _bfd_link_hash_table_init (&ret->root, abfd, xcoff_link_hash_newfunc))
    {
      bfd_release (abfd, ret);
      return (struct bfd_link_hash_table *) NULL;
    }

  ret->debug_strtab = _bfd_xcoff_stringtab_init ();
  ret->debug_section = NULL;

  return &ret->root;
}

/* Look up an entry in an XCOFF link hash table.  */

#define xcoff_link_hash_lookup(table, string, create, copy, follow) \
  ((struct xcoff_link_hash_entry *) \
   bfd_link_hash_lookup (&(table)->root, (string), (create), (copy),\
			 (follow)))

/* Traverse an XCOFF link hash table.  */

#define xcoff_link_hash_traverse(table, func, info)			\
  (bfd_link_hash_traverse						\
   (&(table)->root,							\
    (boolean (*) PARAMS ((struct bfd_link_hash_entry *, PTR))) (func),	\
    (info)))

/* Get the XCOFF link hash table from the info structure.  This is
   just a cast.  */

#define xcoff_hash_table(p) ((struct xcoff_link_hash_table *) ((p)->hash))

/* Given an XCOFF BFD, add symbols to the global hash table as
   appropriate.  */

boolean
_bfd_xcoff_bfd_link_add_symbols (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  switch (bfd_get_format (abfd))
    {
    case bfd_object:
      return xcoff_link_add_object_symbols (abfd, info);
    case bfd_archive:
      return (_bfd_generic_link_add_archive_symbols
	      (abfd, info, xcoff_link_check_archive_element));
    default:
      bfd_set_error (bfd_error_wrong_format);
      return false;
    }
}

/* Add symbols from an XCOFF object file.  */

static boolean
xcoff_link_add_object_symbols (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  if (! _bfd_coff_get_external_symbols (abfd))
    return false;
  if (! xcoff_link_add_symbols (abfd, info))
    return false;
  if (! info->keep_memory)
    {
      if (! _bfd_coff_free_symbols (abfd))
	return false;
    }
  return true;
}

/* Check a single archive element to see if we need to include it in
   the link.  *PNEEDED is set according to whether this element is
   needed in the link or not.  This is called via
   _bfd_generic_link_add_archive_symbols.  */

static boolean
xcoff_link_check_archive_element (abfd, info, pneeded)
     bfd *abfd;
     struct bfd_link_info *info;
     boolean *pneeded;
{
  if (! _bfd_coff_get_external_symbols (abfd))
    return false;

  if (! xcoff_link_check_ar_symbols (abfd, info, pneeded))
    return false;

  if (*pneeded)
    {
      if (! xcoff_link_add_symbols (abfd, info))
	return false;
    }

  if (! info->keep_memory || ! *pneeded)
    {
      if (! _bfd_coff_free_symbols (abfd))
	return false;
    }

  return true;
}

/* Look through the symbols to see if this object file should be
   included in the link.  */

static boolean
xcoff_link_check_ar_symbols (abfd, info, pneeded)
     bfd *abfd;
     struct bfd_link_info *info;
     boolean *pneeded;
{
  bfd_size_type symesz;
  bfd_byte *esym;
  bfd_byte *esym_end;

  *pneeded = false;

  symesz = bfd_coff_symesz (abfd);
  esym = (bfd_byte *) obj_coff_external_syms (abfd);
  esym_end = esym + obj_raw_syment_count (abfd) * symesz;
  while (esym < esym_end)
    {
      struct internal_syment sym;

      bfd_coff_swap_sym_in (abfd, (PTR) esym, (PTR) &sym);

      if (sym.n_sclass == C_EXT && sym.n_scnum != N_UNDEF)
	{
	  const char *name;
	  char buf[SYMNMLEN + 1];
	  struct bfd_link_hash_entry *h;

	  /* This symbol is externally visible, and is defined by this
             object file.  */

	  name = _bfd_coff_internal_syment_name (abfd, &sym, buf);
	  if (name == NULL)
	    return false;
	  h = bfd_link_hash_lookup (info->hash, name, false, false, true);

	  /* We are only interested in symbols that are currently
	     undefined.  If a symbol is currently known to be common,
	     XCOFF linkers do not bring in an object file which
	     defines it.  */
	  if (h != (struct bfd_link_hash_entry *) NULL
	      && h->type == bfd_link_hash_undefined)
	    {
	      if (! (*info->callbacks->add_archive_element) (info, abfd, name))
		return false;
	      *pneeded = true;
	      return true;
	    }
	}

      esym += (sym.n_numaux + 1) * symesz;
    }

  /* We do not need this object file.  */
  return true;
}

/* Add all the symbols from an object file to the hash table.

   XCOFF is a weird format.  A normal XCOFF .o files will have three
   COFF sections--.text, .data, and .bss--but each COFF section will
   contain many csects.  These csects are described in the symbol
   table.  From the linker's point of view, each csect must be
   considered a section in its own right.  For example, a TOC entry is
   handled as a small XMC_TC csect.  The linker must be able to merge
   different TOC entries together, which means that it must be able to
   extract the XMC_TC csects from the .data section of the input .o
   file.

   From the point of view of our linker, this is, of course, a hideous
   nightmare.  We cope by actually creating sections for each csect,
   and discarding the original sections.  We then have to handle the
   relocation entries carefully, since the only way to tell which
   csect they belong to is to examine the address.  */

static boolean
xcoff_link_add_symbols (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  boolean default_copy;
  bfd_size_type symcount;
  struct xcoff_link_hash_entry **sym_hash;
  asection **csect_cache;
  asection *debug_section;
  unsigned long *debug_index = NULL;
  bfd_byte *debug_contents = NULL;
  asection *sub;
  asection *csect;
  unsigned int csect_index;
  asection *first_csect;
  boolean new_debug;
  bfd_size_type symesz;
  bfd_byte *esym;
  bfd_byte *esym_end;
  struct reloc_info_struct
    {
      struct internal_reloc *relocs;
      boolean *used;
    } *reloc_info = NULL;

  if (info->keep_memory)
    default_copy = false;
  else
    default_copy = true;

  symcount = obj_raw_syment_count (abfd);

  /* We keep a list of the linker hash table entries that correspond
     to each external symbol.  */
  sym_hash = ((struct xcoff_link_hash_entry **)
	      bfd_alloc (abfd,
			 (symcount
			  * sizeof (struct xcoff_link_hash_entry *))));
  if (sym_hash == NULL && symcount != 0)
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }
  coff_data (abfd)->sym_hashes = (struct coff_link_hash_entry **) sym_hash;
  memset (sym_hash, 0,
	  (size_t) symcount * sizeof (struct xcoff_link_hash_entry *));

  /* Because of the weird stuff we are doing with XCOFF csects, we can
     not easily determine which section a symbol is in, so we store
     the information in the tdata for the input file.  */
  csect_cache = ((asection **)
		 bfd_alloc (abfd, symcount * sizeof (asection *)));
  if (csect_cache == NULL && symcount != 0)
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }
  xcoff_data (abfd)->csects = csect_cache;
  memset (csect_cache, 0, (size_t) symcount * sizeof (asection *));

  /* XCOFF stores debugging symbol names in the .debug section.  We
     need to determine the size of the final .debug section while
     reading the input files, before the linker starts deciding where
     to put everything.  We build a hash table for the final .debug
     section.  For each symbol whose name goes into it, we store the
     index in the .debug section in the debug_indices array.  */
  debug_section = bfd_get_section_by_name (abfd, ".debug");
  if (debug_section != NULL && debug_section->_raw_size > 0)
    {
      debug_index = ((unsigned long *)
		     bfd_zalloc (abfd, symcount * sizeof (unsigned long)));
      if (debug_index == NULL && symcount != 0)
	{
	  bfd_set_error (bfd_error_no_memory);
	  goto error_return;
	}
      xcoff_data (abfd)->debug_indices = debug_index;

      /* Grab the contents of the current .debug section.  We use
         malloc and copy the names into the string tab hash table,
         rather than bfd_alloc, because I expect that when linking
         many files together, many of the strings will be the same.
         Storing the strings in the hash table should save space in
         that case.  */
      debug_contents = (bfd_byte *) malloc (debug_section->_raw_size);
      if (debug_contents == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  goto error_return;
	}
      if (! bfd_get_section_contents (abfd, debug_section,
				      (PTR) debug_contents, (file_ptr) 0,
				      debug_section->_raw_size))
	goto error_return;
    }

  /* While splitting sections into csects, we need to assign the
     relocs correctly.  The relocs and the csects must both be in
     order by VMA within a given section, so we handle this by
     scanning along the relocs as we process the csects.  We index
     into reloc_info using the section target_index.  */
  reloc_info = ((struct reloc_info_struct *)
		malloc ((abfd->section_count + 1)
			* sizeof (struct reloc_info_struct)));
  if (reloc_info == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }
  memset ((PTR) reloc_info, 0,
	  (abfd->section_count + 1) * sizeof (struct reloc_info_struct));

  /* Read in the relocs for each section which has relocs.  */
  for (sub = abfd->sections; sub != NULL; sub = sub->next)
    {
      if ((sub->flags & SEC_RELOC) != 0)
	{
	  reloc_info[sub->target_index].relocs =
	    _bfd_coff_read_internal_relocs (abfd, sub, true,
					    (bfd_byte *) NULL, false,
					    (struct internal_reloc *) NULL);
	  reloc_info[sub->target_index].used =
	    (boolean *) malloc (sub->reloc_count * sizeof (boolean));
	  if (reloc_info[sub->target_index].used == NULL)
	    {
	      bfd_set_error (bfd_error_no_memory);
	      goto error_return;
	    }
	  memset (reloc_info[sub->target_index].used, 0,
		  sub->reloc_count * sizeof (boolean));
	}
    }

  csect = NULL;
  csect_index = 0;
  first_csect = NULL;
  new_debug = false;

  symesz = bfd_coff_symesz (abfd);
  BFD_ASSERT (symesz == bfd_coff_auxesz (abfd));
  esym = (bfd_byte *) obj_coff_external_syms (abfd);
  esym_end = esym + symcount * symesz;
  while (esym < esym_end)
    {
      struct internal_syment sym;
      union internal_auxent aux;
      PTR name;
      char buf[SYMNMLEN + 1];
      int smtyp;
      flagword flags;
      asection *section;
      bfd_vma value;

      bfd_coff_swap_sym_in (abfd, (PTR) esym, (PTR) &sym);

      *debug_index = (unsigned long) -1;

      /* If the name of this symbol is in the .debug section, and it
         looks like we are going to keep it, add the name to the
         .debug hash table.  */
      if (info->hash->creator == abfd->xvec
	  && sym._n._n_n._n_zeroes == 0
	  && info->strip != strip_all
	  && info->strip != strip_debugger
	  && info->discard != discard_all
	  && bfd_coff_symname_in_debug (abfd, &sym))
	{
	  bfd_size_type indx;

	  /* Add 2 to skip the length.  */
	  name = debug_contents + sym._n._n_n._n_offset;

	  indx = _bfd_stringtab_add (xcoff_hash_table (info)->debug_strtab,
				     name, true, true);
	  if (indx == (bfd_size_type) -1)
	    goto error_return;
	  *debug_index = indx;
	  new_debug = true;
	}

      /* In this pass we are only interested in symbols with csect
         information.  */
      if (sym.n_sclass != C_EXT && sym.n_sclass != C_HIDEXT)
	{
	  if (csect != NULL)
	    *csect_cache = csect;
	  else if (first_csect == NULL)
	    *csect_cache = coff_section_from_bfd_index (abfd, sym.n_scnum);
	  else
	    *csect_cache = NULL;
	  esym += (sym.n_numaux + 1) * symesz;
	  sym_hash += sym.n_numaux + 1;
	  csect_cache += sym.n_numaux + 1;
	  if (debug_index != NULL)
	    debug_index += sym.n_numaux + 1;
	  continue;
	}

      name = _bfd_coff_internal_syment_name (abfd, &sym, buf);
      if (name == NULL)
	goto error_return;

      /* Pick up the csect auxiliary information.  */

      if (sym.n_numaux == 0)
	{
	  (*_bfd_error_handler)
	    ("%s: class %d symbol `%s' has no aux entries",
	     bfd_get_filename (abfd), sym.n_sclass, name);
	  bfd_set_error (bfd_error_bad_value);
	  goto error_return;
	}

      bfd_coff_swap_aux_in (abfd,
			    (PTR) (esym + symesz * sym.n_numaux),
			    sym.n_type, sym.n_sclass,
			    sym.n_numaux - 1, sym.n_numaux,
			    (PTR) &aux);

      smtyp = SMTYP_SMTYP (aux.x_csect.x_smtyp);

      flags = BSF_GLOBAL;
      section = NULL;
      value = 0;

      switch (smtyp)
	{
	default:
	  (*_bfd_error_handler)
	    ("%s: symbol `%s' has unrecognized csect type %d",
	     bfd_get_filename (abfd), name, smtyp);
	  bfd_set_error (bfd_error_bad_value);
	  goto error_return;

	case XTY_ER:
	  /* This is an external reference.  */
	  if (sym.n_sclass == C_HIDEXT
	      || sym.n_scnum != N_UNDEF
	      || aux.x_csect.x_scnlen.l != 0)
	    {
	      (*_bfd_error_handler)
		("%s: bad XTY_ER symbol `%s': class %d scnum %d scnlen %d",
		 bfd_get_filename (abfd), name, sym.n_sclass, sym.n_scnum,
		 aux.x_csect.x_scnlen.l);
	      bfd_set_error (bfd_error_bad_value);
	      goto error_return;
	    }
	  section = bfd_und_section_ptr;
	  break;

	case XTY_SD:
	  /* This is a csect definition.  */

	  csect = NULL;
	  csect_index = -1;

	  /* When we see a TOC anchor, we record the TOC value.  */
	  if (aux.x_csect.x_smclas == XMC_TC0)
	    {
	      if (sym.n_sclass != C_HIDEXT
		  || aux.x_csect.x_scnlen.l != 0)
		{
		  (*_bfd_error_handler)
		    ("%s: XMC_TC0 symbol `%s' is class %d scnlen %d",
		     bfd_get_filename (abfd), name, sym.n_sclass,
		     aux.x_csect.x_scnlen.l);
		  bfd_set_error (bfd_error_bad_value);
		  goto error_return;
		}
	      xcoff_data (abfd)->toc = sym.n_value;
	    }

	  /* We must merge TOC entries for the same symbol.  We can
	     merge two TOC entries if they are both C_HIDEXT, they
	     both have the same name, they are both 4 bytes long, and
	     they both have a relocation table entry for an external
	     symbol with the same name.  Unfortunately, this means
	     that we must look through the relocations.  Ick.  */
	  if (aux.x_csect.x_smclas == XMC_TC
	      && sym.n_sclass == C_HIDEXT
	      && aux.x_csect.x_scnlen.l == 4)
	    {
	      asection *enclosing;
	      bfd_size_type relindx;
	      struct internal_reloc *rel;
	      boolean *used;

	      enclosing = coff_section_from_bfd_index (abfd, sym.n_scnum);
	      if (enclosing == NULL)
		return false;

	      /* XCOFF requires that relocs be sorted by address, so
                 we could do a binary search here.  FIXME.  */
	      rel = reloc_info[enclosing->target_index].relocs;
	      used = reloc_info[enclosing->target_index].used;
	      for (relindx = 0;
		   relindx < enclosing->reloc_count;
		   relindx++, rel++, used++)
		{
		  if (! *used
		      && rel->r_vaddr == (bfd_vma) sym.n_value
		      && rel->r_size == 31
		      && rel->r_type == 0)
		    break;
		}
	      if (relindx < enclosing->reloc_count)
		{
		  bfd_byte *erelsym;
		  struct internal_syment relsym;

		  erelsym = ((bfd_byte *) obj_coff_external_syms (abfd)
			     + rel->r_symndx * symesz);
		  bfd_coff_swap_sym_in (abfd, (PTR) erelsym, (PTR) &relsym);
		  if (relsym.n_sclass == C_EXT)
		    {
		      const char *relname;
		      char relbuf[SYMNMLEN + 1];
		      boolean copy;
		      struct xcoff_link_hash_entry *h;

		      /* At this point we know that the TOC entry is
			 for an externally visible symbol.  */
		      relname = _bfd_coff_internal_syment_name (abfd, &relsym,
								relbuf);
		      if (relname == NULL)
			return false;
		      copy = (! info->keep_memory
			      || relsym._n._n_n._n_zeroes != 0
			      || relsym._n._n_n._n_offset == 0);
		      h = xcoff_link_hash_lookup (xcoff_hash_table (info),
						  relname, true, copy, false);
		      if (h == NULL)
			return false;

		      /* At this point h->root.type could be
			 bfd_link_hash_new.  That should be OK, since
			 we know for sure that we will come across
			 this symbol as we step through the file.  */

		      /* Stuff h into *sym_hash.  We normally only set
                         *sym_hash for global variables.  Remembering
                         it here saves us doing this work again later.  */
		      *sym_hash = h;

		      if (h->toc)
			{
			  /* We already have a TOC entry for this
			     symbol, so we can just ignore this one.
			     We need to skip the csect, and the reloc
			     as well.  */
			  *used = true;
			  break;
			}

		      /* We are about to create a TOC entry for this
			 symbol.  */
		      h->toc = true;
		    }
		}
	    }

	  /* We need to create a new section.  We get the name from
	     the csect storage mapping class, so that the linker can
	     accumulate similar csects together.  */
	  {
	    static const char *csect_name_by_class[] =
	      {
		".pr", ".ro", ".db", ".tc", ".ua", ".rw", ".gl", ".xo",
		".sv", ".bs", ".ds", ".uc", ".ti", ".tb", NULL, ".tc0",
		".td"
	      };
	    const char *csect_name;
	    asection *enclosing;
	    struct internal_reloc *rel;
	    bfd_size_type relindx;
	    boolean *used;

	    if ((aux.x_csect.x_smclas >=
		 sizeof csect_name_by_class / sizeof csect_name_by_class[0])
		|| csect_name_by_class[aux.x_csect.x_smclas] == NULL)
	      {
		(*_bfd_error_handler)
		  ("%s: symbol `%s' has unrecognized smclas %d",
		   bfd_get_filename (abfd), name, aux.x_csect.x_smclas);
		bfd_set_error (bfd_error_bad_value);
		goto error_return;
	      }

	    csect_name = csect_name_by_class[aux.x_csect.x_smclas];
	    csect = bfd_make_section_anyway (abfd, csect_name);
	    if (csect == NULL)
	      goto error_return;
	    enclosing = coff_section_from_bfd_index (abfd, sym.n_scnum);
	    if (enclosing == NULL)
	      goto error_return;
	    if ((bfd_vma) sym.n_value < enclosing->vma
		|| ((bfd_vma) sym.n_value + aux.x_csect.x_scnlen.l
		    > enclosing->vma + enclosing->_raw_size))
	      {
		(*_bfd_error_handler)
		  ("%s: csect `%s' not in enclosing section",
		   bfd_get_filename (abfd), name);
		bfd_set_error (bfd_error_bad_value);
		goto error_return;
	      }
	    csect->vma = sym.n_value;
	    csect->filepos = (enclosing->filepos
			      + sym.n_value
			      - enclosing->vma);
	    csect->_raw_size = aux.x_csect.x_scnlen.l;
	    csect->flags |= SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS;
	    csect->alignment_power = SMTYP_ALIGN (aux.x_csect.x_smtyp);

	    /* XCOFF requires that relocs be sorted by address, so we
               could do a binary search here.  FIXME.  (XCOFF
               unfortunately does not require that symbols be sorted
               by address, or this would be a simple merge).  */
	    rel = reloc_info[enclosing->target_index].relocs;
	    used = reloc_info[enclosing->target_index].used;
	    for (relindx = 0;
		 relindx < enclosing->reloc_count;
		 relindx++, rel++, used++)
	      {
		if (! *used
		    && rel->r_vaddr >= csect->vma
		    && rel->r_vaddr < csect->vma + csect->_raw_size)
		  {
		    csect->rel_filepos = (enclosing->rel_filepos
					  + relindx * bfd_coff_relsz (abfd));
		    break;
		  }
	      }
	    while (relindx < enclosing->reloc_count
		   && ! *used
		   && rel->r_vaddr >= csect->vma
		   && rel->r_vaddr < csect->vma + csect->_raw_size)
	      {
		*used = true;
		csect->flags |= SEC_RELOC;
		++csect->reloc_count;
		++relindx;
		++rel;
		++used;
	      }

	    /* There are a number of other fields and section flags
	       which we do not bother to set.  */

	    /* Record the enclosing section in the tdata for this new
	       section.  */
	    csect->used_by_bfd =
	      ((struct coff_section_tdata *)
	       bfd_alloc (abfd, sizeof (struct coff_section_tdata)));
	    if (csect->used_by_bfd == NULL)
	      {
		bfd_set_error (bfd_error_no_memory);
		goto error_return;
	      }
	    coff_section_data (abfd, csect)->tdata =
	      bfd_alloc (abfd, sizeof (struct xcoff_section_tdata));
	    if (coff_section_data (abfd, csect)->tdata == NULL)
	      {
		bfd_set_error (bfd_error_no_memory);
		goto error_return;
	      }
	    xcoff_section_data (abfd, csect)->enclosing = enclosing;

	    csect_index = ((esym
			    - (bfd_byte *) obj_coff_external_syms (abfd))
			   / symesz);

	    if (first_csect == NULL)
	      first_csect = csect;

	    /* If this symbol is C_EXT, we treat it as starting at the
	       beginning of the newly created section.  */
	    if (sym.n_sclass == C_EXT)
	      {
		section = csect;
		value = 0;
	      }
	  }
	  break;

	case XTY_LD:
	  /* This is a label definition.  The x_scnlen field is the
             symbol index of the csect.  I believe that this must
             always follow the appropriate XTY_SD symbol, so I will
             insist on it.  */
	  {
	    boolean bad;

	    bad = false;
	    if (sym.n_sclass != C_EXT
		|| aux.x_csect.x_scnlen.l < 0
		|| (aux.x_csect.x_scnlen.l
		    >= esym - (bfd_byte *) obj_coff_external_syms (abfd)))
	      bad = true;
	    if (! bad)
	      {
		section = xcoff_data (abfd)->csects[aux.x_csect.x_scnlen.l];
		if (section == NULL
		    || (section->flags & SEC_HAS_CONTENTS) == 0)
		  bad = true;
	      }
	    if (bad)
	      {
		(*_bfd_error_handler)
		  ("%s: misplaced XTY_LD `%s'",
		   bfd_get_filename (abfd), name);
		bfd_set_error (bfd_error_bad_value);
		goto error_return;
	      }

	    value = sym.n_value - csect->vma;
	  }
	  break;

	case XTY_CM:
	  /* This is an unitialized csect.  We could base the name on
             the storage mapping class, but we don't bother.  If this
             csect is externally visible, it is a common symbol.  */
	  if (sym.n_sclass == C_EXT)
	    {
	      section = bfd_com_section_ptr;
	      value = aux.x_csect.x_scnlen.l;
	    }
	  else
	    {
	      csect = bfd_make_section_anyway (abfd, ".bss");
	      if (csect == NULL)
		goto error_return;
	      csect->vma = 0;
	      csect->_raw_size = aux.x_csect.x_scnlen.l;
	      csect->flags |= SEC_ALLOC;
	      csect->alignment_power = SMTYP_ALIGN (aux.x_csect.x_smtyp);
	      /* There are a number of other fields and section flags
                 which we do not bother to set.  */

	      csect_index = ((esym
			      - (bfd_byte *) obj_coff_external_syms (abfd))
			     / symesz);

	      if (first_csect == NULL)
		first_csect = csect;
	    }
	  break;
	}

      /* Now we have enough information to add the symbol to the
         linker hash table.  */

      if (sym.n_sclass == C_EXT)
	{
	  boolean copy;

	  BFD_ASSERT (section != NULL);

	  /* We must copy the name into memory if we got it from the
             syment itself, rather than the string table.  */
	  copy = default_copy;
	  if (sym._n._n_n._n_zeroes != 0
	      || sym._n._n_n._n_offset == 0)
	    copy = true;

	  if (! (_bfd_generic_link_add_one_symbol
		 (info, abfd, name, flags, section, value,
		  (const char *) NULL, copy, false,
		  (struct bfd_link_hash_entry **) sym_hash)))
	    goto error_return;

	  if (info->hash->creator == abfd->xvec)
	    (*sym_hash)->xcoff = true;
	}

      *csect_cache = csect;

      esym += (sym.n_numaux + 1) * symesz;
      sym_hash += sym.n_numaux + 1;
      csect_cache += sym.n_numaux + 1;
      if (debug_index != NULL)
	debug_index += sym.n_numaux + 1;
    }

  if (new_debug)
    {
      /* Update the size of the .debug section we are building.  */
      if (xcoff_hash_table (info)->debug_section == NULL)
	{
	  asection *n;

	  n = bfd_make_section_anyway (abfd, ".debug");
	  if (n == NULL)
	    goto error_return;
	  n->flags |= SEC_HAS_CONTENTS;
	  /* Note that reading from this section will fail.  */
	  xcoff_hash_table (info)->debug_section = n;
	}
      xcoff_hash_table (info)->debug_section->_raw_size =
	_bfd_stringtab_size (xcoff_hash_table (info)->debug_strtab);
    }

  /* Make sure that we have seen all the relocs.  */
  for (sub = abfd->sections; sub != first_csect; sub = sub->next)
    {
      /* Reset the section size, since the data is now attached to the
         csects.  */
      sub->_raw_size = 0;

      if ((sub->flags & SEC_RELOC) != 0)
	{
	  bfd_size_type i;

	  for (i = 0; i < sub->reloc_count; i++)
	    {
	      if (! reloc_info[sub->target_index].used[i])
		{
		  (*_bfd_error_handler)
		    ("%s: reloc %s:%d not in csect",
		     bfd_get_filename (abfd), sub->name, i);
		  bfd_set_error (bfd_error_bad_value);
		  goto error_return;
		}
	    }

	  free (reloc_info[sub->target_index].used);
	  reloc_info[sub->target_index].used = NULL;

	  /* Reset SEC_RELOC and the reloc_count, since the reloc
             information is now attached to the csects.  */
	  sub->flags &=~ SEC_RELOC;
	  sub->reloc_count = 0;

	  /* If we are not keeping memory, free the reloc information.  */
	  if (! info->keep_memory
	      && coff_section_data (abfd, sub) != NULL
	      && coff_section_data (abfd, sub)->relocs != NULL)
	    {
	      free (coff_section_data (abfd, sub)->relocs);
	      coff_section_data (abfd, sub)->relocs = NULL;
	    }
	}
    }

  free (reloc_info);

  if (debug_contents != NULL)
    {
      free (debug_contents);
      debug_contents = NULL;
    }

  return true;

 error_return:
  if (debug_contents != NULL)
    free (debug_contents);
  if (reloc_info != NULL)
    {
      for (sub = abfd->sections; sub != NULL; sub = sub->next)
	{
	  if (reloc_info[sub->target_index].used != NULL)
	    free (reloc_info[sub->target_index].used);
	}
    free (reloc_info);
    }
  return false;
}

/* Do the final link step.  */

boolean
_bfd_xcoff_bfd_final_link (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  bfd_size_type symesz;
  struct xcoff_final_link_info finfo;
  asection *o;
  struct bfd_link_order *p;
  size_t max_contents_size;
  size_t max_sym_count;
  size_t max_lineno_count;
  size_t max_reloc_count;
  size_t max_output_reloc_count;
  file_ptr rel_filepos;
  unsigned int relsz;
  file_ptr line_filepos;
  unsigned int linesz;
  bfd *sub;
  bfd_byte *external_relocs = NULL;
  char strbuf[STRING_SIZE_SIZE];

  symesz = bfd_coff_symesz (abfd);

  finfo.info = info;
  finfo.output_bfd = abfd;
  finfo.strtab = NULL;
  finfo.section_info = NULL;
  finfo.last_file_index = -1;
  finfo.toc_symindx = -1;
  finfo.internal_syms = NULL;
  finfo.sym_indices = NULL;
  finfo.outsyms = NULL;
  finfo.linenos = NULL;
  finfo.contents = NULL;
  finfo.external_relocs = NULL;

  xcoff_data (abfd)->coff.link_info = info;

  finfo.strtab = _bfd_stringtab_init ();
  if (finfo.strtab == NULL)
    goto error_return;

  /* Compute the file positions for all the sections.  */
  if (! abfd->output_has_begun)
    bfd_coff_compute_section_file_positions (abfd);

  /* Count the line numbers and relocation entries required for the
     output file.  Set the file positions for the relocs.  */
  rel_filepos = obj_relocbase (abfd);
  relsz = bfd_coff_relsz (abfd);
  max_contents_size = 0;
  max_lineno_count = 0;
  max_reloc_count = 0;
  for (o = abfd->sections; o != NULL; o = o->next)
    {
      o->reloc_count = 0;
      o->lineno_count = 0;
      for (p = o->link_order_head; p != NULL; p = p->next)
	{
	  if (p->type == bfd_indirect_link_order)
	    {
	      asection *sec;

	      sec = p->u.indirect.section;

	      if (info->strip == strip_none
		  || info->strip == strip_some)
		o->lineno_count += sec->lineno_count;

	      o->reloc_count += sec->reloc_count;

	      if (sec->_raw_size > max_contents_size)
		max_contents_size = sec->_raw_size;
	      if (sec->lineno_count > max_lineno_count)
		max_lineno_count = sec->lineno_count;
	      if (sec->reloc_count > max_reloc_count)
		max_reloc_count = sec->reloc_count;
	    }
	  else if (p->type == bfd_section_reloc_link_order
		   || p->type == bfd_symbol_reloc_link_order)
	    ++o->reloc_count;
	}
      if (o->reloc_count == 0)
	o->rel_filepos = 0;
      else
	{
	  o->flags |= SEC_RELOC;
	  o->rel_filepos = rel_filepos;
	  rel_filepos += o->reloc_count * relsz;
	}
    }

  /* Allocate space for the pointers we need to keep for the relocs.  */
  {
    unsigned int i;

    /* We use section_count + 1, rather than section_count, because
       the target_index fields are 1 based.  */
    finfo.section_info = ((struct xcoff_link_section_info *)
			  malloc ((abfd->section_count + 1)
				  * sizeof (struct xcoff_link_section_info)));
    if (finfo.section_info == NULL)
      {
	bfd_set_error (bfd_error_no_memory);
	goto error_return;
      }
    for (i = 0; i <= abfd->section_count; i++)
      {
	finfo.section_info[i].relocs = NULL;
	finfo.section_info[i].rel_hashes = NULL;
      }
  }

  /* We now know the size of the relocs, so we can determine the file
     positions of the line numbers.  */
  line_filepos = rel_filepos;
  linesz = bfd_coff_linesz (abfd);
  max_output_reloc_count = 0;
  for (o = abfd->sections; o != NULL; o = o->next)
    {
      if (o->lineno_count == 0)
	o->line_filepos = 0;
      else
	{
	  o->line_filepos = line_filepos;
	  line_filepos += o->lineno_count * linesz;
	}

      if (o->reloc_count != 0)
	{
	  /* We don't know the indices of global symbols until we have
             written out all the local symbols.  For each section in
             the output file, we keep an array of pointers to hash
             table entries.  Each entry in the array corresponds to a
             reloc.  When we find a reloc against a global symbol, we
             set the corresponding entry in this array so that we can
             fix up the symbol index after we have written out all the
             local symbols.

	     Because of this problem, we also keep the relocs in
	     memory until the end of the link.  This wastes memory.
	     We could backpatch the file later, I suppose, although it
	     would be slow.  */
	  finfo.section_info[o->target_index].relocs =
	    ((struct internal_reloc *)
	     malloc (o->reloc_count * sizeof (struct internal_reloc)));
	  finfo.section_info[o->target_index].rel_hashes =
	    ((struct xcoff_link_hash_entry **)
	     malloc (o->reloc_count
		     * sizeof (struct xcoff_link_hash_entry *)));
	  if (finfo.section_info[o->target_index].relocs == NULL
	      || finfo.section_info[o->target_index].rel_hashes == NULL)
	    {
	      bfd_set_error (bfd_error_no_memory);
	      goto error_return;
	    }

	  if (o->reloc_count > max_output_reloc_count)
	    max_output_reloc_count = o->reloc_count;
	}

      /* Reset the reloc and lineno counts, so that we can use them to
	 count the number of entries we have output so far.  */
      o->reloc_count = 0;
      o->lineno_count = 0;
    }

  obj_sym_filepos (abfd) = line_filepos;

  /* Figure out the largest number of symbols in an input BFD.  Take
     the opportunity to clear the output_has_begun fields of all the
     input BFD's.  We want at least 4 symbols, since that is the
     number which xcoff_write_global_symbol may need.  */
  max_sym_count = 4;
  for (sub = info->input_bfds; sub != NULL; sub = sub->link_next)
    {
      size_t sz;

      sub->output_has_begun = false;
      sz = obj_raw_syment_count (sub);
      if (sz > max_sym_count)
	max_sym_count = sz;
    }

  /* Allocate some buffers used while linking.  */
  finfo.internal_syms = ((struct internal_syment *)
			 malloc (max_sym_count
				 * sizeof (struct internal_syment)));
  finfo.sym_indices = (long *) malloc (max_sym_count * sizeof (long));
  finfo.outsyms = ((bfd_byte *)
		   malloc ((size_t) ((max_sym_count + 1) * symesz)));
  finfo.linenos = (bfd_byte *) malloc (max_lineno_count
				       * bfd_coff_linesz (abfd));
  finfo.contents = (bfd_byte *) malloc (max_contents_size);
  finfo.external_relocs = (bfd_byte *) malloc (max_reloc_count * relsz);
  if ((finfo.internal_syms == NULL && max_sym_count > 0)
      || (finfo.sym_indices == NULL && max_sym_count > 0)
      || finfo.outsyms == NULL
      || (finfo.linenos == NULL && max_lineno_count > 0)
      || (finfo.contents == NULL && max_contents_size > 0)
      || (finfo.external_relocs == NULL && max_reloc_count > 0))
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }

  obj_raw_syment_count (abfd) = 0;
  xcoff_data (abfd)->toc = (bfd_vma) -1;

  /* Write out all the global symbols which do not come from XCOFF
     input files.  In COFF these must come at the end, but in XCOFF it
     doesn't seem to matter.  */
  xcoff_link_hash_traverse (xcoff_hash_table (info),
			    xcoff_write_global_symbol,
			    (PTR) &finfo);

  /* We now know the position of everything in the file, except that
     we don't know the size of the symbol table and therefore we don't
     know where the string table starts.  We just build the string
     table in memory as we go along.  We process all the relocations
     for a single input file at once.  */
  for (o = abfd->sections; o != NULL; o = o->next)
    {
      for (p = o->link_order_head; p != NULL; p = p->next)
	{
	  if (p->type == bfd_indirect_link_order
	      && p->u.indirect.section->owner->xvec == abfd->xvec)
	    {
	      sub = p->u.indirect.section->owner;
	      if (! sub->output_has_begun)
		{
		  if (! xcoff_link_input_bfd (&finfo, sub))
		    goto error_return;
		  sub->output_has_begun = true;
		}
	    }
	  else if (p->type == bfd_section_reloc_link_order
		   || p->type == bfd_symbol_reloc_link_order)
	    {
	      if (! xcoff_reloc_link_order (abfd, &finfo, o, p))
		goto error_return;
	    }
	  else
	    {
	      if (! _bfd_default_link_order (abfd, info, o, p))
		goto error_return;
	    }
	}
    }

  /* Free up the buffers used by xcoff_link_input_bfd.  */

  if (finfo.internal_syms != NULL)
    {
      free (finfo.internal_syms);
      finfo.internal_syms = NULL;
    }
  if (finfo.sym_indices != NULL)
    {
      free (finfo.sym_indices);
      finfo.sym_indices = NULL;
    }
  if (finfo.linenos != NULL)
    {
      free (finfo.linenos);
      finfo.linenos = NULL;
    }
  if (finfo.contents != NULL)
    {
      free (finfo.contents);
      finfo.contents = NULL;
    }
  if (finfo.external_relocs != NULL)
    {
      free (finfo.external_relocs);
      finfo.external_relocs = NULL;
    }

  /* The value of the last C_FILE symbol is supposed to be -1.  Write
     it out again.  */
  if (finfo.last_file_index != -1)
    {
      finfo.last_file.n_value = -1;
      bfd_coff_swap_sym_out (abfd, (PTR) &finfo.last_file,
			     (PTR) finfo.outsyms);
      if (bfd_seek (abfd,
		    (obj_sym_filepos (abfd)
		     + finfo.last_file_index * symesz),
		    SEEK_SET) != 0
	  || bfd_write (finfo.outsyms, symesz, 1, abfd) != symesz)
	goto error_return;
    }

  if (finfo.outsyms != NULL)
    {
      free (finfo.outsyms);
      finfo.outsyms = NULL;
    }

  /* Now that we have written out all the global symbols, we know the
     symbol indices to use for relocs against them, and we can finally
     write out the relocs.  */
  external_relocs = (bfd_byte *) malloc (max_output_reloc_count * relsz);
  if (external_relocs == NULL && max_output_reloc_count != 0)
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }

  for (o = abfd->sections; o != NULL; o = o->next)
    {
      struct internal_reloc *irel;
      struct internal_reloc *irelend;
      struct xcoff_link_hash_entry **rel_hash;
      bfd_byte *erel;

      if (o->reloc_count == 0)
	continue;

      /* XCOFF requires that the relocs be sorted by address.  We tend
         to produce them in the order in which their containing csects
         appear in the symbol table, which is not necessarily by
         address.  So we sort them here.  There may be a better way to
         do this.  */
      qsort ((PTR) finfo.section_info[o->target_index].relocs,
	     o->reloc_count, sizeof (struct internal_reloc),
	     xcoff_sort_relocs);

      irel = finfo.section_info[o->target_index].relocs;
      irelend = irel + o->reloc_count;
      rel_hash = finfo.section_info[o->target_index].rel_hashes;
      erel = external_relocs;
      for (; irel < irelend; irel++, rel_hash++, erel += relsz)
	{
	  if (*rel_hash != NULL)
	    {
	      if ((*rel_hash)->indx < 0)
		{
		  if (! ((*info->callbacks->unattached_reloc)
			 (info, (*rel_hash)->root.root.string, abfd, o,
			  irel->r_vaddr)))
		    goto error_return;
		  (*rel_hash)->indx = 0;
		}
	      irel->r_symndx = (*rel_hash)->indx;
	    }
	  bfd_coff_swap_reloc_out (abfd, (PTR) irel, (PTR) erel);
	}

      if (bfd_seek (abfd, o->rel_filepos, SEEK_SET) != 0
	  || bfd_write ((PTR) external_relocs, relsz, o->reloc_count,
			abfd) != relsz * o->reloc_count)
	goto error_return;
    }

  if (external_relocs != NULL)
    {
      free (external_relocs);
      external_relocs = NULL;
    }

  /* Free up the section information.  */
  if (finfo.section_info != NULL)
    {
      unsigned int i;

      for (i = 0; i < abfd->section_count; i++)
	{
	  if (finfo.section_info[i].relocs != NULL)
	    free (finfo.section_info[i].relocs);
	  if (finfo.section_info[i].rel_hashes != NULL)
	    free (finfo.section_info[i].rel_hashes);
	}
      free (finfo.section_info);
      finfo.section_info = NULL;
    }

  /* Write out the string table.  */
  if (bfd_seek (abfd,
		(obj_sym_filepos (abfd)
		 + obj_raw_syment_count (abfd) * symesz),
		SEEK_SET) != 0)
    goto error_return;
  bfd_h_put_32 (abfd,
		_bfd_stringtab_size (finfo.strtab) + STRING_SIZE_SIZE,
		(bfd_byte *) strbuf);
  if (bfd_write (strbuf, 1, STRING_SIZE_SIZE, abfd) != STRING_SIZE_SIZE)
    goto error_return;
  if (! _bfd_stringtab_emit (abfd, finfo.strtab))
    goto error_return;

  _bfd_stringtab_free (finfo.strtab);

  /* Write out the debugging string table.  */
  o = xcoff_hash_table (info)->debug_section;
  if (o != NULL)
    {
      struct bfd_strtab_hash *debug_strtab;

      debug_strtab = xcoff_hash_table (info)->debug_strtab;
      BFD_ASSERT (o->output_section->_raw_size - o->output_offset
		  >= _bfd_stringtab_size (debug_strtab));
      if (bfd_seek (abfd,
		    o->output_section->filepos + o->output_offset,
		    SEEK_SET) != 0)
	goto error_return;
      if (! _bfd_stringtab_emit (abfd, debug_strtab))
	goto error_return;
    }

  /* Setting bfd_get_symcount to 0 will cause write_object_contents to
     not try to write out the symbols.  */
  bfd_get_symcount (abfd) = 0;

  return true;

 error_return:
  if (finfo.strtab != NULL)
    _bfd_stringtab_free (finfo.strtab);
  if (finfo.section_info != NULL)
    {
      unsigned int i;

      for (i = 0; i < abfd->section_count; i++)
	{
	  if (finfo.section_info[i].relocs != NULL)
	    free (finfo.section_info[i].relocs);
	  if (finfo.section_info[i].rel_hashes != NULL)
	    free (finfo.section_info[i].rel_hashes);
	}
      free (finfo.section_info);
    }
  if (finfo.internal_syms != NULL)
    free (finfo.internal_syms);
  if (finfo.sym_indices != NULL)
    free (finfo.sym_indices);
  if (finfo.outsyms != NULL)
    free (finfo.outsyms);
  if (finfo.linenos != NULL)
    free (finfo.linenos);
  if (finfo.contents != NULL)
    free (finfo.contents);
  if (finfo.external_relocs != NULL)
    free (finfo.external_relocs);
  if (external_relocs != NULL)
    free (external_relocs);
  return false;
}

/* Link an input file into the linker output file.  This function
   handles all the sections and relocations of the input file at once.  */

static boolean
xcoff_link_input_bfd (finfo, input_bfd)
     struct xcoff_final_link_info *finfo;
     bfd *input_bfd;
{
  bfd *output_bfd;
  const char *strings;
  bfd_size_type syment_base;
  unsigned int n_tmask;
  unsigned int n_btshft;
  boolean copy, hash;
  bfd_size_type isymesz;
  bfd_size_type osymesz;
  bfd_size_type linesz;
  bfd_byte *esym;
  bfd_byte *esym_end;
  struct internal_syment *isymp;
  asection **csectpp;
  unsigned long *debug_index;
  long *indexp;
  unsigned long output_index;
  bfd_byte *outsym;
  struct xcoff_link_hash_entry **sym_hash;
  boolean keep_syms;
  asection *o;

  /* Move all the symbols to the output file.  */

  output_bfd = finfo->output_bfd;
  strings = NULL;
  syment_base = obj_raw_syment_count (output_bfd);
  isymesz = bfd_coff_symesz (input_bfd);
  osymesz = bfd_coff_symesz (output_bfd);
  linesz = bfd_coff_linesz (input_bfd);
  BFD_ASSERT (linesz == bfd_coff_linesz (output_bfd));

  n_tmask = coff_data (input_bfd)->local_n_tmask;
  n_btshft = coff_data (input_bfd)->local_n_btshft;

  /* Define macros so that ISFCN, et. al., macros work correctly.  */
#define N_TMASK n_tmask
#define N_BTSHFT n_btshft

  copy = false;
  if (! finfo->info->keep_memory)
    copy = true;
  hash = true;
  if ((output_bfd->flags & BFD_TRADITIONAL_FORMAT) != 0)
    hash = false;

  if (! _bfd_coff_get_external_symbols (input_bfd))
    return false;

  esym = (bfd_byte *) obj_coff_external_syms (input_bfd);
  esym_end = esym + obj_raw_syment_count (input_bfd) * isymesz;
  sym_hash = obj_xcoff_sym_hashes (input_bfd);
  csectpp = xcoff_data (input_bfd)->csects;
  debug_index = xcoff_data (input_bfd)->debug_indices;
  isymp = finfo->internal_syms;
  indexp = finfo->sym_indices;
  output_index = syment_base;
  outsym = finfo->outsyms;

  while (esym < esym_end)
    {
      struct internal_syment isym;
      union internal_auxent aux;
      int smtyp = 0;
      boolean skip;
      boolean require;
      int add;

      bfd_coff_swap_sym_in (input_bfd, (PTR) esym, (PTR) isymp);

      /* If this is a C_EXT or C_HIDEXT symbol, we need the csect
         information.  */
      if (isymp->n_sclass == C_EXT || isymp->n_sclass == C_HIDEXT)
	{
	  BFD_ASSERT (isymp->n_numaux > 0);
	  bfd_coff_swap_aux_in (input_bfd,
				(PTR) (esym + isymesz * isymp->n_numaux),
				isymp->n_type, isymp->n_sclass,
				isymp->n_numaux - 1, isymp->n_numaux,
				(PTR) &aux);
	  smtyp = SMTYP_SMTYP (aux.x_csect.x_smtyp);
	}

      /* Make a copy of *isymp so that the relocate_section function
	 always sees the original values.  This is more reliable than
	 always recomputing the symbol value even if we are stripping
	 the symbol.  */
      isym = *isymp;

      *indexp = -1;

      skip = false;
      require = false;
      add = 1 + isym.n_numaux;

      /* If we are skipping this csect, we want to skip this symbol.  */
      if (*csectpp == NULL)
	skip = true;

      /* An XCOFF linker always skips C_STAT symbols.  */
      if (! skip
	  && isymp->n_sclass == C_STAT)
	skip = true;

      /* We skip all but the first TOC anchor.  */
      if (! skip
	  && isymp->n_sclass == C_HIDEXT
	  && aux.x_csect.x_smclas == XMC_TC0)
	{
	  if (finfo->toc_symindx != -1)
	    skip = true;
	  else
	    {
	      finfo->toc_symindx = output_index;
	      xcoff_data (finfo->output_bfd)->toc =
		((*csectpp)->output_section->vma
		 + (*csectpp)->output_offset
		 + isym.n_value
		 - (*csectpp)->vma);
	      require = true;
	    }
	}

      /* If we are stripping all symbols, we want to skip this one.  */
      if (! skip
	  && finfo->info->strip == strip_all)
	skip = true;

      /* We can skip resolved external references.  */
      if (! skip
	  && isym.n_sclass == C_EXT
	  && smtyp == XTY_ER
	  && (*sym_hash)->root.type != bfd_link_hash_undefined)
	skip = true;

      /* Skip local symbols if we are discarding them.  */
      if (! skip
	  && finfo->info->discard == discard_all
	  && isym.n_sclass != C_EXT
	  && (isym.n_sclass != C_HIDEXT
	      || smtyp != XTY_SD))
	skip = true;

      /* If we stripping debugging symbols, and this is a debugging
         symbol, then skip it.  */
      if (! skip
	  && finfo->info->strip == strip_debugger
	  && isym.n_scnum == N_DEBUG)
	skip = true;

      /* If some symbols are stripped based on the name, work out the
	 name and decide whether to skip this symbol.  We don't handle
	 this correctly for symbols whose names are in the .debug
	 section; to get it right we would need a new bfd_strtab_hash
	 function to return the string given the index.  */
      if (! skip
	  && (finfo->info->strip == strip_some
	      || finfo->info->discard == discard_l)
	  && (debug_index == NULL || *debug_index == (unsigned long) -1))
	{
	  const char *name;
	  char buf[SYMNMLEN + 1];

	  name = _bfd_coff_internal_syment_name (input_bfd, &isym, buf);
	  if (name == NULL)
	    return false;

	  if ((finfo->info->strip == strip_some
	       && (bfd_hash_lookup (finfo->info->keep_hash, name, false,
				    false) == NULL))
	      || (finfo->info->discard == discard_l
		  && (isym.n_sclass != C_EXT
		      && (isym.n_sclass != C_HIDEXT
			  || smtyp != XTY_SD))
		  && strncmp (name, finfo->info->lprefix,
			      finfo->info->lprefix_len) == 0))
	    skip = true;
	}

      /* On the other hand, we can't skip global symbols which have
         relocs against them.  FIXME: This won't always work, because
         we're not always outputting the global symbols at the end, as
         we do for COFF.  */
      if (skip
	  && *sym_hash != NULL
	  && (*sym_hash)->indx == -2
	  && finfo->info->strip != strip_all)
	skip = false;

      /* We can not skip the first TOC anchor.  */
      if (skip
	  && require
	  && finfo->info->strip != strip_all)
	skip = false;

      /* We now know whether we are to skip this symbol or not.  */
      if (! skip)
	{
	  /* Adjust the symbol in order to output it.  */

	  if (isym._n._n_n._n_zeroes == 0
	      && isym._n._n_n._n_offset != 0)
	    {
	      /* This symbol has a long name.  Enter it in the string
		 table we are building.  If *debug_index != -1, the
		 name has already been entered in the .debug section.  */
	      if (debug_index != NULL && *debug_index != (unsigned long) -1)
		isym._n._n_n._n_offset = *debug_index;
	      else
		{
		  const char *name;
		  bfd_size_type indx;

		  name = _bfd_coff_internal_syment_name (input_bfd, &isym,
							 (char *) NULL);
		  if (name == NULL)
		    return false;
		  indx = _bfd_stringtab_add (finfo->strtab, name, hash, copy);
		  if (indx == (bfd_size_type) -1)
		    return false;
		  isym._n._n_n._n_offset = STRING_SIZE_SIZE + indx;
		}
	    }

	  if (isym.n_sclass == C_BSTAT)
	    {
	      unsigned long indx;

	      /* The value of a C_BSTAT symbol is the symbol table
                 index of the containing csect.  */

	      indx = isym.n_value;
	      if (indx < obj_raw_syment_count (input_bfd))
		{
		  long symindx;

		  symindx = finfo->sym_indices[indx];
		  if (symindx < 0)
		    isym.n_value = 0;
		  else
		    isym.n_value = symindx;
		}
	    }
	  else if (isym.n_scnum > 0)
	    {
	      isym.n_scnum = (*csectpp)->output_section->target_index;
	      isym.n_value += ((*csectpp)->output_section->vma
			       + (*csectpp)->output_offset
			       - (*csectpp)->vma);
	    }

	  /* The value of a C_FILE symbol is the symbol index of the
	     next C_FILE symbol.  The value of the last C_FILE symbol
	     is -1.  We try to get this right, below, just before we
	     write the symbols out, but in the general case we may
	     have to write the symbol out twice.  */
	  if (isym.n_sclass == C_FILE)
	    {
	      if (finfo->last_file_index != -1
		  && finfo->last_file.n_value != (long) output_index)
		{
		  /* We must correct the value of the last C_FILE entry.  */
		  finfo->last_file.n_value = output_index;
		  if ((bfd_size_type) finfo->last_file_index >= syment_base)
		    {
		      /* The last C_FILE symbol is in this input file.  */
		      bfd_coff_swap_sym_out (output_bfd,
					     (PTR) &finfo->last_file,
					     (PTR) (finfo->outsyms
						    + ((finfo->last_file_index
							- syment_base)
						       * osymesz)));
		    }
		  else
		    {
		      /* We have already written out the last C_FILE
			 symbol.  We need to write it out again.  We
			 borrow *outsym temporarily.  */
		      bfd_coff_swap_sym_out (output_bfd,
					     (PTR) &finfo->last_file,
					     (PTR) outsym);
		      if (bfd_seek (output_bfd,
				    (obj_sym_filepos (output_bfd)
				     + finfo->last_file_index * osymesz),
				    SEEK_SET) != 0
			  || (bfd_write (outsym, osymesz, 1, output_bfd)
			      != osymesz))
			return false;
		    }
		}

	      finfo->last_file_index = output_index;
	      finfo->last_file = isym;
	    }

	  /* Output the symbol.  */

	  bfd_coff_swap_sym_out (output_bfd, (PTR) &isym, (PTR) outsym);

	  *indexp = output_index;

	  if (isym.n_sclass == C_EXT)
	    {
	      long indx;
	      struct xcoff_link_hash_entry *h;

	      indx = ((esym - (bfd_byte *) obj_coff_external_syms (input_bfd))
		      / isymesz);
	      h = obj_xcoff_sym_hashes (input_bfd)[indx];
	      BFD_ASSERT (h != NULL);
	      h->indx = output_index;
	    }

	  output_index += add;
	  outsym += add * osymesz;
	}

      esym += add * isymesz;
      isymp += add;
      csectpp += add;
      sym_hash += add;
      if (debug_index != NULL)
	debug_index += add;
      ++indexp;
      for (--add; add > 0; --add)
	*indexp++ = -1;
    }

  /* Fix up the aux entries.  This must be done in a separate pass,
     because we don't know the correct symbol indices until we have
     already decided which symbols we are going to keep.  */

  esym = (bfd_byte *) obj_coff_external_syms (input_bfd);
  esym_end = esym + obj_raw_syment_count (input_bfd) * isymesz;
  isymp = finfo->internal_syms;
  indexp = finfo->sym_indices;
  outsym = finfo->outsyms;
  while (esym < esym_end)
    {
      int add;

      add = 1 + isymp->n_numaux;

      if (*indexp < 0)
	esym += add * isymesz;
      else
	{
	  int i;

	  esym += isymesz;
	  outsym += osymesz;

	  for (i = 0; i < isymp->n_numaux && esym < esym_end; i++)
	    {
	      union internal_auxent aux;

	      bfd_coff_swap_aux_in (input_bfd, (PTR) esym, isymp->n_type,
				    isymp->n_sclass, i, isymp->n_numaux,
				    (PTR) &aux);

	      if (isymp->n_sclass == C_FILE)
		{
		  /* This is the file name (or some comment put in by
		     the compiler).  If it is long, we must put it in
		     the string table.  */
		  if (aux.x_file.x_n.x_zeroes == 0
		      && aux.x_file.x_n.x_offset != 0)
		    {
		      const char *filename;
		      bfd_size_type indx;

		      BFD_ASSERT (aux.x_file.x_n.x_offset
				  >= STRING_SIZE_SIZE);
		      if (strings == NULL)
			{
			  strings = _bfd_coff_read_string_table (input_bfd);
			  if (strings == NULL)
			    return false;
			}
		      filename = strings + aux.x_file.x_n.x_offset;
		      indx = _bfd_stringtab_add (finfo->strtab, filename,
						 hash, copy);
		      if (indx == (bfd_size_type) -1)
			return false;
		      aux.x_file.x_n.x_offset = STRING_SIZE_SIZE + indx;
		    }
		}
	      else if ((isymp->n_sclass == C_EXT
			|| isymp->n_sclass == C_HIDEXT)
		       && i + 1 == isymp->n_numaux)
		{
		  /* We don't support type checking.  I don't know if
                     anybody does.  */
		  aux.x_csect.x_parmhash = 0;
		  /* I don't think anybody uses these fields, but we'd
                     better clobber them just in case.  */
		  aux.x_csect.x_stab = 0;
		  aux.x_csect.x_snstab = 0;
		  if (SMTYP_SMTYP (aux.x_csect.x_smtyp) == XTY_LD)
		    {
		      unsigned long indx;

		      indx = aux.x_csect.x_scnlen.l;
		      if (indx < obj_raw_syment_count (input_bfd))
			{
			  long symindx;

			  symindx = finfo->sym_indices[indx];
			  if (symindx < 0)
			    aux.x_sym.x_tagndx.l = 0;
			  else
			    aux.x_sym.x_tagndx.l = symindx;
			}
		    }
		}
	      else if (isymp->n_sclass != C_STAT || isymp->n_type != T_NULL)
		{
		  unsigned long indx;

		  if (ISFCN (isymp->n_type)
		      || ISTAG (isymp->n_sclass)
		      || isymp->n_sclass == C_BLOCK)
		    {
		      indx = aux.x_sym.x_fcnary.x_fcn.x_endndx.l;
		      if (indx > 0
			  && indx < obj_raw_syment_count (input_bfd))
			{
			  /* We look forward through the symbol for
                             the index of the next symbol we are going
                             to include.  I don't know if this is
                             entirely right.  */
			  while (finfo->sym_indices[indx] < 0
				 && indx < obj_raw_syment_count (input_bfd))
			    ++indx;
			  if (indx >= obj_raw_syment_count (input_bfd))
			    indx = output_index;
			  else
			    indx = finfo->sym_indices[indx];
			  aux.x_sym.x_fcnary.x_fcn.x_endndx.l = indx;
			}
		    }

		  indx = aux.x_sym.x_tagndx.l;
		  if (indx > 0 && indx < obj_raw_syment_count (input_bfd))
		    {
		      long symindx;

		      symindx = finfo->sym_indices[indx];
		      if (symindx < 0)
			aux.x_sym.x_tagndx.l = 0;
		      else
			aux.x_sym.x_tagndx.l = symindx;
		    }
		}

	      bfd_coff_swap_aux_out (output_bfd, (PTR) &aux, isymp->n_type,
				     isymp->n_sclass, i, isymp->n_numaux,
				     (PTR) outsym);
	      outsym += osymesz;
	      esym += isymesz;
	    }
	}

      indexp += add;
      isymp += add;
    }

  /* Relocate the line numbers, unless we are stripping them.  */
  if (finfo->info->strip == strip_none
      || finfo->info->strip == strip_some)
    {
      for (o = input_bfd->sections; o != NULL; o = o->next)
	{
	  bfd_vma offset;
	  bfd_byte *eline;
	  bfd_byte *elineend;

	  /* FIXME: If SEC_HAS_CONTENTS is not set for the section,
	     then build_link_order in ldwrite.c will not have created
	     a link order, which means that we will not have seen this
	     input section in _bfd_xcoff_final_link, which means that
	     we will not have allocated space for the line numbers of
	     this section.  I don't think line numbers can be
	     meaningful for a section which does not have
	     SEC_HAS_CONTENTS set, but, if they do, this must be
	     changed.  */
	  if (o->lineno_count == 0
	      || (o->output_section->flags & SEC_HAS_CONTENTS) == 0)
	    continue;

	  if (bfd_seek (input_bfd, o->line_filepos, SEEK_SET) != 0
	      || bfd_read (finfo->linenos, linesz, o->lineno_count,
			   input_bfd) != linesz * o->lineno_count)
	    return false;

	  offset = o->output_section->vma + o->output_offset - o->vma;
	  eline = finfo->linenos;
	  elineend = eline + linesz * o->lineno_count;
	  for (; eline < elineend; eline += linesz)
	    {
	      struct internal_lineno iline;

	      bfd_coff_swap_lineno_in (input_bfd, (PTR) eline, (PTR) &iline);

	      if (iline.l_lnno != 0)
		iline.l_addr.l_paddr += offset;
	      else if (iline.l_addr.l_symndx >= 0
		       && ((unsigned long) iline.l_addr.l_symndx
			   < obj_raw_syment_count (input_bfd)))
		{
		  long indx;

		  indx = finfo->sym_indices[iline.l_addr.l_symndx];

		  if (indx < 0)
		    {
		      /* These line numbers are attached to a symbol
			 which we are stripping.  We should really
			 just discard the line numbers, but that would
			 be a pain because we have already counted
			 them.  */
		      indx = 0;
		    }
		  else
		    {
		      struct internal_syment is;
		      union internal_auxent ia;

		      /* Fix up the lnnoptr field in the aux entry of
			 the symbol.  It turns out that we can't do
			 this when we modify the symbol aux entries,
			 because gas sometimes screws up the lnnoptr
			 field and makes it an offset from the start
			 of the line numbers rather than an absolute
			 file index.  */
		      bfd_coff_swap_sym_in (output_bfd,
					    (PTR) (finfo->outsyms
						   + ((indx - syment_base)
						      * osymesz)),
					    (PTR) &is);
		      if ((ISFCN (is.n_type)
			   || is.n_sclass == C_BLOCK)
			  && ((is.n_sclass == C_EXT
			       || is.n_sclass == C_HIDEXT)
			      ? is.n_numaux >= 2
			      : is.n_numaux >= 1))
			{
			  PTR auxptr;

			  auxptr = (PTR) (finfo->outsyms
					  + ((indx - syment_base + 1)
					     * osymesz));
			  bfd_coff_swap_aux_in (output_bfd, auxptr,
						is.n_type, is.n_sclass,
						0, is.n_numaux, (PTR) &ia);
			  ia.x_sym.x_fcnary.x_fcn.x_lnnoptr =
			    (o->output_section->line_filepos
			     + o->output_section->lineno_count * linesz
			     + eline - finfo->linenos);
			  bfd_coff_swap_aux_out (output_bfd, (PTR) &ia,
						 is.n_type, is.n_sclass, 0,
						 is.n_numaux, auxptr);
			}
		    }

		  iline.l_addr.l_symndx = indx;
		}

	      bfd_coff_swap_lineno_out (output_bfd, (PTR) &iline, (PTR) eline);
	    }

	  if (bfd_seek (output_bfd,
			(o->output_section->line_filepos
			 + o->output_section->lineno_count * linesz),
			SEEK_SET) != 0
	      || bfd_write (finfo->linenos, linesz, o->lineno_count,
			    output_bfd) != linesz * o->lineno_count)
	    return false;

	  o->output_section->lineno_count += o->lineno_count;
	}
    }

  /* If we swapped out a C_FILE symbol, guess that the next C_FILE
     symbol will be the first symbol in the next input file.  In the
     normal case, this will save us from writing out the C_FILE symbol
     again.  */
  if (finfo->last_file_index != -1
      && (bfd_size_type) finfo->last_file_index >= syment_base)
    {
      finfo->last_file.n_value = output_index;
      bfd_coff_swap_sym_out (output_bfd, (PTR) &finfo->last_file,
			     (PTR) (finfo->outsyms
 				    + ((finfo->last_file_index - syment_base)
 				       * osymesz)));
    }

  /* Write the modified symbols to the output file.  */
  if (outsym > finfo->outsyms)
    {
      if (bfd_seek (output_bfd,
		    obj_sym_filepos (output_bfd) + syment_base * osymesz,
		    SEEK_SET) != 0
	  || (bfd_write (finfo->outsyms, outsym - finfo->outsyms, 1,
			output_bfd)
	      != (bfd_size_type) (outsym - finfo->outsyms)))
	return false;

      BFD_ASSERT ((obj_raw_syment_count (output_bfd)
		   + (outsym - finfo->outsyms) / osymesz)
		  == output_index);

      obj_raw_syment_count (output_bfd) = output_index;
    }

  /* Don't let the linker relocation routines discard the symbols.  */
  keep_syms = obj_coff_keep_syms (input_bfd);
  obj_coff_keep_syms (input_bfd) = true;

  /* Relocate the contents of each section.  */
  for (o = input_bfd->sections; o != NULL; o = o->next)
    {
      bfd_byte *contents;

      if ((o->flags & SEC_HAS_CONTENTS) == 0
	  || o->_raw_size == 0)
	continue;

      /* We have set filepos correctly for the sections we created to
         represent csects, so bfd_get_section_contents should work.  */
      if (coff_section_data (input_bfd, o) != NULL
	  && coff_section_data (input_bfd, o)->contents != NULL)
	contents = coff_section_data (input_bfd, o)->contents;
      else
	{
	  if (! bfd_get_section_contents (input_bfd, o, finfo->contents,
					  (file_ptr) 0, o->_raw_size))
	    return false;
	  contents = finfo->contents;
	}

      if ((o->flags & SEC_RELOC) != 0)
	{
	  int target_index;
	  struct internal_reloc *internal_relocs;
	  struct internal_reloc *irel;
	  bfd_vma offset;
	  struct internal_reloc *irelend;
	  struct xcoff_link_hash_entry **rel_hash;

	  /* Read in the relocs.  We have set rel_filepos correctly
             for the sections we created to represent csects, so this
             should work.  */
	  target_index = o->output_section->target_index;
	  internal_relocs = (_bfd_coff_read_internal_relocs
			     (input_bfd, o, false, finfo->external_relocs,
			      true,
			      (finfo->section_info[target_index].relocs
			       + o->output_section->reloc_count)));
	  if (internal_relocs == NULL)
	    return false;

	  /* Call processor specific code to relocate the section
             contents.  */
	  if (! bfd_coff_relocate_section (output_bfd, finfo->info,
					   input_bfd, o,
					   contents,
					   internal_relocs,
					   finfo->internal_syms,
					   xcoff_data (input_bfd)->csects))
	    return false;

	  offset = o->output_section->vma + o->output_offset - o->vma;
	  irel = internal_relocs;
	  irelend = irel + o->reloc_count;
	  rel_hash = (finfo->section_info[target_index].rel_hashes
		      + o->output_section->reloc_count);
	  for (; irel < irelend; irel++, rel_hash++)
	    {
	      struct xcoff_link_hash_entry *h;

	      *rel_hash = NULL;

	      /* Adjust the reloc address and symbol index.  */

	      irel->r_vaddr += offset;

	      if (irel->r_symndx == -1)
		continue;

	      h = obj_xcoff_sym_hashes (input_bfd)[irel->r_symndx];
	      if (h != NULL)
		{
		  /* This is a global symbol.  */
		  if (h->indx >= 0)
		    irel->r_symndx = h->indx;
		  else
		    {
		      /* This symbol is being written at the end of
			 the file, and we do not yet know the symbol
			 index.  We save the pointer to the hash table
			 entry in the rel_hash list.  We set the indx
			 field to -2 to indicate that this symbol must
			 not be stripped.  */
		      *rel_hash = h;
		      h->indx = -2;
		    }
		}
	      else
		{
		  long indx;

		  indx = finfo->sym_indices[irel->r_symndx];

		  if (indx == -1)
		    {
		      struct internal_syment *is;

		      /* Relocations against a TC0 TOC anchor are
                         automatically transformed to be against the
                         TOC anchor in the output file.  */
		      is = finfo->internal_syms + irel->r_symndx;
		      if (is->n_sclass == C_HIDEXT
			  && is->n_numaux > 0)
			{
			  PTR auxptr;
			  union internal_auxent aux;

			  auxptr = (PTR) (((bfd_byte *)
					   obj_coff_external_syms (input_bfd))
					  + ((irel->r_symndx + is->n_numaux)
					     * isymesz));
			  bfd_coff_swap_aux_in (input_bfd, auxptr,
						is->n_type, is->n_sclass,
						is->n_numaux - 1,
						is->n_numaux,
						(PTR) &aux);
			  if (SMTYP_SMTYP (aux.x_csect.x_smtyp) == XTY_SD
			      && aux.x_csect.x_smclas == XMC_TC0)
			    indx = finfo->toc_symindx;
			}
		    }

		  if (indx != -1)
		    irel->r_symndx = indx;
		  else
		    {
		      struct internal_syment *is;
		      const char *name;
		      char buf[SYMNMLEN + 1];

		      /* This reloc is against a symbol we are
			 stripping.  It would be possible to handle
			 this case, but I don't think it's worth it.  */
		      is = finfo->internal_syms + irel->r_symndx;

		      name = (_bfd_coff_internal_syment_name
			      (input_bfd, is, buf));
		      if (name == NULL)
			return false;

		      if (! ((*finfo->info->callbacks->unattached_reloc)
			     (finfo->info, name, input_bfd, o,
			      irel->r_vaddr)))
			return false;
		    }
		}
	    }

	  o->output_section->reloc_count += o->reloc_count;
	}

      /* Write out the modified section contents.  */
      if (! bfd_set_section_contents (output_bfd, o->output_section,
				      contents, o->output_offset,
				      (o->_cooked_size != 0
				       ? o->_cooked_size
				       : o->_raw_size)))
	return false;
    }

  obj_coff_keep_syms (input_bfd) = keep_syms;

  if (! finfo->info->keep_memory)
    {
      if (! _bfd_coff_free_symbols (input_bfd))
	return false;
    }

  return true;
}

/* Write out a non-XCOFF global symbol.  */

static boolean
xcoff_write_global_symbol (h, p)
     struct xcoff_link_hash_entry *h;
     PTR p;
{
  struct xcoff_final_link_info *finfo = (struct xcoff_final_link_info *) p;
  bfd *output_bfd;
  bfd_byte *outsym;
  struct internal_syment isym;
  union internal_auxent aux;

  /* XCOFF global symbols will be written out in xcoff_link_input_bfd.
     The only symbols we need to handle here are those from non-XCOFF
     files and from the linker script.  */
  if (h->xcoff)
    return true;

  output_bfd = finfo->output_bfd;
  outsym = finfo->outsyms;

  memset (&aux, 0, sizeof aux);

  h->indx = obj_raw_syment_count (output_bfd);

  if (strlen (h->root.root.string) <= SYMNMLEN)
    strncpy (isym._n._n_name, h->root.root.string, SYMNMLEN);
  else
    {
      boolean hash;
      bfd_size_type indx;

      hash = true;
      if ((output_bfd->flags & BFD_TRADITIONAL_FORMAT) != 0)
	hash = false;
      indx = _bfd_stringtab_add (finfo->strtab, h->root.root.string, hash,
				 false);
      if (indx == (bfd_size_type) -1)
	return false;
      isym._n._n_n._n_zeroes = 0;
      isym._n._n_n._n_offset = STRING_SIZE_SIZE + indx;
    }

  if (h->root.type == bfd_link_hash_undefined
      || h->root.type == bfd_link_hash_undefweak)
    {
      isym.n_value = 0;
      isym.n_scnum = N_UNDEF;
      isym.n_sclass = C_EXT;
      aux.x_csect.x_smtyp = XTY_ER;
    }
  else if (h->root.type == bfd_link_hash_defined
	   || h->root.type == bfd_link_hash_defweak)
    {
      isym.n_value = (h->root.u.def.section->output_section->vma
		      + h->root.u.def.section->output_offset
		      + h->root.u.def.value);
      isym.n_scnum = h->root.u.def.section->output_section->target_index;
      isym.n_sclass = C_HIDEXT;
      aux.x_csect.x_smtyp = XTY_SD;
      /* I don't know what the csect length should be in this case.  */
    }
  else
    abort ();

  isym.n_type = T_NULL;
  isym.n_numaux = 1;

  bfd_coff_swap_sym_out (output_bfd, (PTR) &isym, (PTR) outsym);
  outsym += bfd_coff_symesz (output_bfd);

  aux.x_csect.x_smclas = XMC_UA;

  bfd_coff_swap_aux_out (output_bfd, (PTR) &aux, T_NULL, isym.n_sclass, 0, 1,
			 (PTR) outsym);
  outsym += bfd_coff_auxesz (output_bfd);

  if ((h->root.type == bfd_link_hash_defined
       || h->root.type == bfd_link_hash_defweak)
      && aux.x_csect.x_smclas != XMC_TC0)
    {
      /* We just output an SD symbol.  Now output an LD symbol.  */

      h->indx += 2;

      isym.n_sclass = C_EXT;
      bfd_coff_swap_sym_out (output_bfd, (PTR) &isym, (PTR) outsym);
      outsym += bfd_coff_symesz (output_bfd);

      aux.x_csect.x_smtyp = XTY_LD;
      aux.x_csect.x_scnlen.l = obj_raw_syment_count (output_bfd);

      bfd_coff_swap_aux_out (output_bfd, (PTR) &aux, T_NULL, C_EXT, 0, 1,
			     (PTR) outsym);
      outsym += bfd_coff_auxesz (output_bfd);
    }

  if (bfd_seek (output_bfd,
		(obj_sym_filepos (output_bfd)
		 + (obj_raw_syment_count (output_bfd)
		    * bfd_coff_symesz (output_bfd))),
		SEEK_SET) != 0
      || (bfd_write (finfo->outsyms, outsym - finfo->outsyms, 1, output_bfd)
	  != (bfd_size_type) (outsym - finfo->outsyms)))
    return false;
  obj_raw_syment_count (output_bfd) +=
    (outsym - finfo->outsyms) / bfd_coff_symesz (output_bfd);

  return true;
}

/* Handle a link order which is supposed to generate a reloc.  */

static boolean
xcoff_reloc_link_order (output_bfd, finfo, output_section, link_order)
     bfd *output_bfd;
     struct xcoff_final_link_info *finfo;
     asection *output_section;
     struct bfd_link_order *link_order;
{
  reloc_howto_type *howto;
  struct internal_reloc *irel;
  struct xcoff_link_hash_entry **rel_hash_ptr;

  howto = bfd_reloc_type_lookup (output_bfd, link_order->u.reloc.p->reloc);
  if (howto == NULL)
    {
      bfd_set_error (bfd_error_bad_value);
      return false;
    }

  if (link_order->u.reloc.p->addend != 0)
    {
      bfd_size_type size;
      bfd_byte *buf;
      bfd_reloc_status_type rstat;
      boolean ok;

      size = bfd_get_reloc_size (howto);
      buf = (bfd_byte *) bfd_zmalloc (size);
      if (buf == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}

      rstat = _bfd_relocate_contents (howto, output_bfd,
				      link_order->u.reloc.p->addend, buf);
      switch (rstat)
	{
	case bfd_reloc_ok:
	  break;
	default:
	case bfd_reloc_outofrange:
	  abort ();
	case bfd_reloc_overflow:
	  if (! ((*finfo->info->callbacks->reloc_overflow)
		 (finfo->info,
		  (link_order->type == bfd_section_reloc_link_order
		   ? bfd_section_name (output_bfd,
				       link_order->u.reloc.p->u.section)
		   : link_order->u.reloc.p->u.name),
		  howto->name, link_order->u.reloc.p->addend,
		  (bfd *) NULL, (asection *) NULL, (bfd_vma) 0)))
	    {
	      free (buf);
	      return false;
	    }
	  break;
	}
      ok = bfd_set_section_contents (output_bfd, output_section, (PTR) buf,
				     (file_ptr) link_order->offset, size);
      free (buf);
      if (! ok)
	return false;
    }

  /* Store the reloc information in the right place.  It will get
     swapped and written out at the end of the final_link routine.  */

  irel = (finfo->section_info[output_section->target_index].relocs
	  + output_section->reloc_count);
  rel_hash_ptr = (finfo->section_info[output_section->target_index].rel_hashes
		  + output_section->reloc_count);

  memset (irel, 0, sizeof (struct internal_reloc));
  *rel_hash_ptr = NULL;

  irel->r_vaddr = output_section->vma + link_order->offset;

  if (link_order->type == bfd_section_reloc_link_order)
    {
      /* We need to somehow locate a symbol in the right section.  The
         symbol must either have a value of zero, or we must adjust
         the addend by the value of the symbol.  FIXME: Write this
         when we need it.  The old linker couldn't handle this anyhow.  */
      abort ();
      *rel_hash_ptr = NULL;
      irel->r_symndx = 0;
    }
  else
    {
      struct xcoff_link_hash_entry *h;

      h = xcoff_link_hash_lookup (xcoff_hash_table (finfo->info),
				  link_order->u.reloc.p->u.name,
				  false, false, true);
      if (h != NULL)
	{
	  if (h->indx >= 0)
	    irel->r_symndx = h->indx;
	  else
	    {
	      /* Set the index to -2 to force this symbol to get
		 written out.  */
	      h->indx = -2;
	      *rel_hash_ptr = h;
	      irel->r_symndx = 0;
	    }
	}
      else
	{
	  if (! ((*finfo->info->callbacks->unattached_reloc)
		 (finfo->info, link_order->u.reloc.p->u.name, (bfd *) NULL,
		  (asection *) NULL, (bfd_vma) 0)))
	    return false;
	  irel->r_symndx = 0;
	}
    }

  irel->r_type = howto->type;
  irel->r_size = howto->bitsize - 1;
  if (howto->complain_on_overflow == complain_overflow_signed)
    irel->r_size |= 0x80;

  ++output_section->reloc_count;

  return true;
}

/* Sort relocs by VMA.  This is called via qsort.  */

static int
xcoff_sort_relocs (p1, p2)
     const PTR p1;
     const PTR p2;
{
  const struct internal_reloc *r1 = (const struct internal_reloc *) p1;
  const struct internal_reloc *r2 = (const struct internal_reloc *) p2;

  if (r1->r_vaddr > r2->r_vaddr)
    return 1;
  else if (r1->r_vaddr < r2->r_vaddr)
    return -1;
  else
    return 0;
}

/* This is the relocation function for the RS/6000/POWER/PowerPC.
   This is currently the only processor which uses XCOFF; I hope that
   will never change.  */

boolean
_bfd_ppc_xcoff_relocate_section (output_bfd, info, input_bfd,
				 input_section, contents, relocs, syms,
				 sections)
     bfd *output_bfd;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     struct internal_reloc *relocs;
     struct internal_syment *syms;
     asection **sections;
{
  struct internal_reloc *rel;
  struct internal_reloc *relend;

  rel = relocs;
  relend = rel + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      long symndx;
      struct xcoff_link_hash_entry *h;
      struct internal_syment *sym;
      bfd_vma addend;
      bfd_vma val;
      struct reloc_howto_struct howto;
      bfd_reloc_status_type rstat;

      /* Relocation type 0x0f is a special relocation type which is
         merely used to prevent garbage collection from occurring for
         the csect including the symbol which it references.  */
      if (rel->r_type == 0x0f)
	continue;

      symndx = rel->r_symndx;

      if (symndx == -1)
	{
	  h = NULL;
	  sym = NULL;
	  addend = 0;
	}
      else
	{    
	  h = obj_xcoff_sym_hashes (input_bfd)[symndx];
	  sym = syms + symndx;
	  addend = - sym->n_value;
	}

      /* We build the howto information on the fly.  */

      howto.type = rel->r_type;
      howto.rightshift = 0;
      howto.size = 2;
      howto.bitsize = (rel->r_size & 0x1f) + 1;
      howto.pc_relative = false;
      howto.bitpos = 0;
      if ((rel->r_size & 0x80) != 0)
	howto.complain_on_overflow = complain_overflow_signed;
      else
	howto.complain_on_overflow = complain_overflow_bitfield;
      howto.special_function = NULL;
      howto.name = "internal";
      howto.partial_inplace = true;
      if (howto.bitsize == 32)
	howto.src_mask = howto.dst_mask = 0xffffffff;
      else
	{
	  howto.src_mask = howto.dst_mask = (1 << howto.bitsize) - 1;
	  if (howto.bitsize == 16)
	    howto.size = 1;
	}
      howto.pcrel_offset = false;

      val = 0;

      if (h == NULL)
	{
	  asection *sec;

	  if (symndx == -1)
	    {
	      sec = bfd_abs_section_ptr;
	      val = 0;
	    }
	  else
	    {
	      sec = sections[symndx];
              val = (sec->output_section->vma
		     + sec->output_offset
		     + sym->n_value
		     - sec->vma);
	    }
	}
      else
	{
	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      asection *sec;

	      sec = h->root.u.def.section;
	      val = (h->root.u.def.value
		     + sec->output_section->vma
		     + sec->output_offset);
	    }
	  else if (! info->relocateable)
	    {
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, h->root.root.string, input_bfd, input_section,
		      rel->r_vaddr - input_section->vma)))
		return false;
	    }
	}

      /* I took the relocation type definitions from two documents:
	 the PowerPC AIX Version 4 Application Binary Interface, First
	 Edition (April 1992), and the PowerOpen ABI, Big-Endian
	 32-Bit Hardware Implementation (June 30, 1994).  Differences
	 between the documents are noted below.  */

      switch (rel->r_type)
	{
	case 0x04:
	case 0x14:
	case 0x15:
	  /* These relocs are defined by the PowerPC ABI to be
             relative branches which use half of the difference
             between the symbol and the program counter.  I can't
             quite figure out when this is useful.  These relocs are
             not defined by the PowerOpen ABI.  */
	default:
	  (*_bfd_error_handler)
	    ("%s: unsupported relocation type 0x%02x",
	     bfd_get_filename (input_bfd), (unsigned int) rel->r_type);
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	case 0x00:
	  /* Simple positive relocation.  */
	  break;
	case 0x01:
	  /* Simple negative relocation.  */
	  val = - val;
	  break;
	case 0x02:
	  /* Simple PC relative relocation.  */
	  howto.pc_relative = true;
	  break;
	case 0x03:
	  /* TOC relative relocation.  The value in the instruction in
             the input file is the offset from the input file TOC to
             the desired location.  We want the offset from the final
             TOC to the desired location.  We have:
	         isym = iTOC + in
		 iinsn = in + o
		 osym = oTOC + on
		 oinsn = on + o
	     so we must change insn by on - in.
	     */
	case 0x05:
	  /* Global linkage relocation.  The value of this relocation
             is the address of the entry in the TOC section.  */
	case 0x06:
	  /* Local object TOC address.  I can't figure out the
             difference between this and case 0x05.  */
	case 0x12:
	  /* TOC relative relocation.  A TOC relative load instruction
             which may be changed to a load address instruction.
             FIXME: We don't currently implement this optimization.  */
	case 0x13:
	  /* TOC relative relocation.  This is a TOC relative load
             address instruction which may be changed to a load
             instruction.  FIXME: I don't know if this is the correct
             implementation.  */
	  val = ((val - xcoff_data (output_bfd)->toc)
		 - (sym->n_value - xcoff_data (input_bfd)->toc));
	  addend = 0;
	  break;
	case 0x08:
	  /* Absolute branch.  We don't want to mess with the lower
             two bits of the instruction.  */
	case 0x16:
	  /* The PowerPC ABI defines this as an absolute call which
             may be modified to become a relative call.  The PowerOpen
             ABI does not define this relocation type.  */
	case 0x18:
	  /* Absolute branch which may be modified to become a
             relative branch.  */
	case 0x19:
	  /* The PowerPC ABI defines this as an absolute branch to a
             fixed address which may be modified to an absolute branch
             to a symbol.  The PowerOpen ABI does not define this
             relocation type.  */
	case 0x1b:
	  /* The PowerPC ABI defines this as an absolute branch to a
             fixed address which may be modified to a relative branch.
             The PowerOpen ABI does not define this relocation type.  */
	  howto.src_mask &= ~3;
	  howto.dst_mask = howto.src_mask;
	  break;
	case 0x0a:
	  /* Relative branch.  We don't want to mess with the lower
             two bits of the instruction.  */
	case 0x17:
	  /* The PowerPC ABI defines this as a relative call which may
             be modified to become an absolute call.  The PowerOpen
             ABI does not define this relocation type.  */
	case 0x1a:
	  /* A relative branch which may be modified to become an
             absolute branch.  FIXME: We don't implement this,
             although we should for symbols of storage mapping class
             XMC_XO.  */
	  howto.pc_relative = true;
	  howto.src_mask &= ~3;
	  howto.dst_mask = howto.src_mask;
	  break;
	case 0x0c:
	  /* The PowerPC AIX ABI describes this as a load which may be
             changed to a load address.  The PowerOpen ABI says this
             is the same as case 0x00.  */
	  break;
	case 0x0d:
	  /* The PowerPC AIX ABI describes this as a load address
             which may be changed to a load.  The PowerOpen ABI says
             this is the same as case 0x00.  */
	  break;
	}

      rstat = _bfd_final_link_relocate (&howto, input_bfd, input_section,
					contents,
					rel->r_vaddr - input_section->vma,
					val, addend);

      switch (rstat)
	{
	default:
	  abort ();
	case bfd_reloc_ok:
	  break;
	case bfd_reloc_overflow:
	  {
	    const char *name;
	    char buf[SYMNMLEN + 1];
	    char howto_name[10];

	    if (symndx == -1)
	      name = "*ABS*";
	    else if (h != NULL)
	      name = h->root.root.string;
	    else
	      {
		name = _bfd_coff_internal_syment_name (input_bfd, sym, buf);
		if (name == NULL)
		  return false;
	      }
	    sprintf (howto_name, "0x%02x", rel->r_type);

	    if (! ((*info->callbacks->reloc_overflow)
		   (info, name, howto_name, (bfd_vma) 0, input_bfd,
		    input_section, rel->r_vaddr - input_section->vma)))
	      return false;
	  }
	}
    }

  return true;
}
