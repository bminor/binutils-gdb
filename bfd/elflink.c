/* ELF linking support for BFD.
   Copyright 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004
   Free Software Foundation, Inc.

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
#define ARCH_SIZE 0
#include "elf-bfd.h"

bfd_boolean
_bfd_elf_create_got_section (bfd *abfd, struct bfd_link_info *info)
{
  flagword flags;
  asection *s;
  struct elf_link_hash_entry *h;
  struct bfd_link_hash_entry *bh;
  const struct elf_backend_data *bed = get_elf_backend_data (abfd);
  int ptralign;

  /* This function may be called more than once.  */
  s = bfd_get_section_by_name (abfd, ".got");
  if (s != NULL && (s->flags & SEC_LINKER_CREATED) != 0)
    return TRUE;

  switch (bed->s->arch_size)
    {
    case 32:
      ptralign = 2;
      break;

    case 64:
      ptralign = 3;
      break;

    default:
      bfd_set_error (bfd_error_bad_value);
      return FALSE;
    }

  flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY
	   | SEC_LINKER_CREATED);

  s = bfd_make_section (abfd, ".got");
  if (s == NULL
      || !bfd_set_section_flags (abfd, s, flags)
      || !bfd_set_section_alignment (abfd, s, ptralign))
    return FALSE;

  if (bed->want_got_plt)
    {
      s = bfd_make_section (abfd, ".got.plt");
      if (s == NULL
	  || !bfd_set_section_flags (abfd, s, flags)
	  || !bfd_set_section_alignment (abfd, s, ptralign))
	return FALSE;
    }

  if (bed->want_got_sym)
    {
      /* Define the symbol _GLOBAL_OFFSET_TABLE_ at the start of the .got
	 (or .got.plt) section.  We don't do this in the linker script
	 because we don't want to define the symbol if we are not creating
	 a global offset table.  */
      bh = NULL;
      if (!(_bfd_generic_link_add_one_symbol
	    (info, abfd, "_GLOBAL_OFFSET_TABLE_", BSF_GLOBAL, s,
	     bed->got_symbol_offset, NULL, FALSE, bed->collect, &bh)))
	return FALSE;
      h = (struct elf_link_hash_entry *) bh;
      h->elf_link_hash_flags |= ELF_LINK_HASH_DEF_REGULAR;
      h->type = STT_OBJECT;

      if (! info->executable
	  && ! _bfd_elf_link_record_dynamic_symbol (info, h))
	return FALSE;

      elf_hash_table (info)->hgot = h;
    }

  /* The first bit of the global offset table is the header.  */
  s->_raw_size += bed->got_header_size + bed->got_symbol_offset;

  return TRUE;
}

/* Create some sections which will be filled in with dynamic linking
   information.  ABFD is an input file which requires dynamic sections
   to be created.  The dynamic sections take up virtual memory space
   when the final executable is run, so we need to create them before
   addresses are assigned to the output sections.  We work out the
   actual contents and size of these sections later.  */

bfd_boolean
_bfd_elf_link_create_dynamic_sections (bfd *abfd, struct bfd_link_info *info)
{
  flagword flags;
  register asection *s;
  struct elf_link_hash_entry *h;
  struct bfd_link_hash_entry *bh;
  const struct elf_backend_data *bed;

  if (! is_elf_hash_table (info->hash))
    return FALSE;

  if (elf_hash_table (info)->dynamic_sections_created)
    return TRUE;

  /* Make sure that all dynamic sections use the same input BFD.  */
  if (elf_hash_table (info)->dynobj == NULL)
    elf_hash_table (info)->dynobj = abfd;
  else
    abfd = elf_hash_table (info)->dynobj;

  /* Note that we set the SEC_IN_MEMORY flag for all of these
     sections.  */
  flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS
	   | SEC_IN_MEMORY | SEC_LINKER_CREATED);

  /* A dynamically linked executable has a .interp section, but a
     shared library does not.  */
  if (info->executable)
    {
      s = bfd_make_section (abfd, ".interp");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY))
	return FALSE;
    }

  if (! info->traditional_format)
    {
      s = bfd_make_section (abfd, ".eh_frame_hdr");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
	  || ! bfd_set_section_alignment (abfd, s, 2))
	return FALSE;
      elf_hash_table (info)->eh_info.hdr_sec = s;
    }

  bed = get_elf_backend_data (abfd);

  /* Create sections to hold version informations.  These are removed
     if they are not needed.  */
  s = bfd_make_section (abfd, ".gnu.version_d");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
      || ! bfd_set_section_alignment (abfd, s, bed->s->log_file_align))
    return FALSE;

  s = bfd_make_section (abfd, ".gnu.version");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
      || ! bfd_set_section_alignment (abfd, s, 1))
    return FALSE;

  s = bfd_make_section (abfd, ".gnu.version_r");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
      || ! bfd_set_section_alignment (abfd, s, bed->s->log_file_align))
    return FALSE;

  s = bfd_make_section (abfd, ".dynsym");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
      || ! bfd_set_section_alignment (abfd, s, bed->s->log_file_align))
    return FALSE;

  s = bfd_make_section (abfd, ".dynstr");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY))
    return FALSE;

  /* Create a strtab to hold the dynamic symbol names.  */
  if (elf_hash_table (info)->dynstr == NULL)
    {
      elf_hash_table (info)->dynstr = _bfd_elf_strtab_init ();
      if (elf_hash_table (info)->dynstr == NULL)
	return FALSE;
    }

  s = bfd_make_section (abfd, ".dynamic");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags)
      || ! bfd_set_section_alignment (abfd, s, bed->s->log_file_align))
    return FALSE;

  /* The special symbol _DYNAMIC is always set to the start of the
     .dynamic section.  This call occurs before we have processed the
     symbols for any dynamic object, so we don't have to worry about
     overriding a dynamic definition.  We could set _DYNAMIC in a
     linker script, but we only want to define it if we are, in fact,
     creating a .dynamic section.  We don't want to define it if there
     is no .dynamic section, since on some ELF platforms the start up
     code examines it to decide how to initialize the process.  */
  bh = NULL;
  if (! (_bfd_generic_link_add_one_symbol
	 (info, abfd, "_DYNAMIC", BSF_GLOBAL, s, 0, NULL, FALSE,
	  get_elf_backend_data (abfd)->collect, &bh)))
    return FALSE;
  h = (struct elf_link_hash_entry *) bh;
  h->elf_link_hash_flags |= ELF_LINK_HASH_DEF_REGULAR;
  h->type = STT_OBJECT;

  if (! info->executable
      && ! _bfd_elf_link_record_dynamic_symbol (info, h))
    return FALSE;

  s = bfd_make_section (abfd, ".hash");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
      || ! bfd_set_section_alignment (abfd, s, bed->s->log_file_align))
    return FALSE;
  elf_section_data (s)->this_hdr.sh_entsize = bed->s->sizeof_hash_entry;

  /* Let the backend create the rest of the sections.  This lets the
     backend set the right flags.  The backend will normally create
     the .got and .plt sections.  */
  if (! (*bed->elf_backend_create_dynamic_sections) (abfd, info))
    return FALSE;

  elf_hash_table (info)->dynamic_sections_created = TRUE;

  return TRUE;
}

/* Create dynamic sections when linking against a dynamic object.  */

bfd_boolean
_bfd_elf_create_dynamic_sections (bfd *abfd, struct bfd_link_info *info)
{
  flagword flags, pltflags;
  asection *s;
  const struct elf_backend_data *bed = get_elf_backend_data (abfd);

  /* We need to create .plt, .rel[a].plt, .got, .got.plt, .dynbss, and
     .rel[a].bss sections.  */

  flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY
	   | SEC_LINKER_CREATED);

  pltflags = flags;
  pltflags |= SEC_CODE;
  if (bed->plt_not_loaded)
    pltflags &= ~ (SEC_CODE | SEC_LOAD | SEC_HAS_CONTENTS);
  if (bed->plt_readonly)
    pltflags |= SEC_READONLY;

  s = bfd_make_section (abfd, ".plt");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, pltflags)
      || ! bfd_set_section_alignment (abfd, s, bed->plt_alignment))
    return FALSE;

  if (bed->want_plt_sym)
    {
      /* Define the symbol _PROCEDURE_LINKAGE_TABLE_ at the start of the
	 .plt section.  */
      struct elf_link_hash_entry *h;
      struct bfd_link_hash_entry *bh = NULL;

      if (! (_bfd_generic_link_add_one_symbol
	     (info, abfd, "_PROCEDURE_LINKAGE_TABLE_", BSF_GLOBAL, s, 0, NULL,
	      FALSE, get_elf_backend_data (abfd)->collect, &bh)))
	return FALSE;
      h = (struct elf_link_hash_entry *) bh;
      h->elf_link_hash_flags |= ELF_LINK_HASH_DEF_REGULAR;
      h->type = STT_OBJECT;

      if (! info->executable
	  && ! _bfd_elf_link_record_dynamic_symbol (info, h))
	return FALSE;
    }

  s = bfd_make_section (abfd,
			bed->default_use_rela_p ? ".rela.plt" : ".rel.plt");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
      || ! bfd_set_section_alignment (abfd, s, bed->s->log_file_align))
    return FALSE;

  if (! _bfd_elf_create_got_section (abfd, info))
    return FALSE;

  if (bed->want_dynbss)
    {
      /* The .dynbss section is a place to put symbols which are defined
	 by dynamic objects, are referenced by regular objects, and are
	 not functions.  We must allocate space for them in the process
	 image and use a R_*_COPY reloc to tell the dynamic linker to
	 initialize them at run time.  The linker script puts the .dynbss
	 section into the .bss section of the final image.  */
      s = bfd_make_section (abfd, ".dynbss");
      if (s == NULL
	  || ! bfd_set_section_flags (abfd, s, SEC_ALLOC | SEC_LINKER_CREATED))
	return FALSE;

      /* The .rel[a].bss section holds copy relocs.  This section is not
     normally needed.  We need to create it here, though, so that the
     linker will map it to an output section.  We can't just create it
     only if we need it, because we will not know whether we need it
     until we have seen all the input files, and the first time the
     main linker code calls BFD after examining all the input files
     (size_dynamic_sections) the input sections have already been
     mapped to the output sections.  If the section turns out not to
     be needed, we can discard it later.  We will never need this
     section when generating a shared object, since they do not use
     copy relocs.  */
      if (! info->shared)
	{
	  s = bfd_make_section (abfd,
				(bed->default_use_rela_p
				 ? ".rela.bss" : ".rel.bss"));
	  if (s == NULL
	      || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
	      || ! bfd_set_section_alignment (abfd, s, bed->s->log_file_align))
	    return FALSE;
	}
    }

  return TRUE;
}

/* Record a new dynamic symbol.  We record the dynamic symbols as we
   read the input files, since we need to have a list of all of them
   before we can determine the final sizes of the output sections.
   Note that we may actually call this function even though we are not
   going to output any dynamic symbols; in some cases we know that a
   symbol should be in the dynamic symbol table, but only if there is
   one.  */

bfd_boolean
_bfd_elf_link_record_dynamic_symbol (struct bfd_link_info *info,
				     struct elf_link_hash_entry *h)
{
  if (h->dynindx == -1)
    {
      struct elf_strtab_hash *dynstr;
      char *p;
      const char *name;
      bfd_size_type indx;

      /* XXX: The ABI draft says the linker must turn hidden and
	 internal symbols into STB_LOCAL symbols when producing the
	 DSO. However, if ld.so honors st_other in the dynamic table,
	 this would not be necessary.  */
      switch (ELF_ST_VISIBILITY (h->other))
	{
	case STV_INTERNAL:
	case STV_HIDDEN:
	  if (h->root.type != bfd_link_hash_undefined
	      && h->root.type != bfd_link_hash_undefweak)
	    {
	      h->elf_link_hash_flags |= ELF_LINK_FORCED_LOCAL;
	      return TRUE;
	    }

	default:
	  break;
	}

      h->dynindx = elf_hash_table (info)->dynsymcount;
      ++elf_hash_table (info)->dynsymcount;

      dynstr = elf_hash_table (info)->dynstr;
      if (dynstr == NULL)
	{
	  /* Create a strtab to hold the dynamic symbol names.  */
	  elf_hash_table (info)->dynstr = dynstr = _bfd_elf_strtab_init ();
	  if (dynstr == NULL)
	    return FALSE;
	}

      /* We don't put any version information in the dynamic string
	 table.  */
      name = h->root.root.string;
      p = strchr (name, ELF_VER_CHR);
      if (p != NULL)
	/* We know that the p points into writable memory.  In fact,
	   there are only a few symbols that have read-only names, being
	   those like _GLOBAL_OFFSET_TABLE_ that are created specially
	   by the backends.  Most symbols will have names pointing into
	   an ELF string table read from a file, or to objalloc memory.  */
	*p = 0;

      indx = _bfd_elf_strtab_add (dynstr, name, p != NULL);

      if (p != NULL)
	*p = ELF_VER_CHR;

      if (indx == (bfd_size_type) -1)
	return FALSE;
      h->dynstr_index = indx;
    }

  return TRUE;
}

/* Record an assignment to a symbol made by a linker script.  We need
   this in case some dynamic object refers to this symbol.  */

bfd_boolean
bfd_elf_record_link_assignment (bfd *output_bfd ATTRIBUTE_UNUSED,
				struct bfd_link_info *info,
				const char *name,
				bfd_boolean provide)
{
  struct elf_link_hash_entry *h;

  if (!is_elf_hash_table (info->hash))
    return TRUE;

  h = elf_link_hash_lookup (elf_hash_table (info), name, TRUE, TRUE, FALSE);
  if (h == NULL)
    return FALSE;

  /* Since we're defining the symbol, don't let it seem to have not
     been defined.  record_dynamic_symbol and size_dynamic_sections
     may depend on this.  */
  if (h->root.type == bfd_link_hash_undefweak
      || h->root.type == bfd_link_hash_undefined)
    h->root.type = bfd_link_hash_new;

  if (h->root.type == bfd_link_hash_new)
    h->elf_link_hash_flags &= ~ELF_LINK_NON_ELF;

  /* If this symbol is being provided by the linker script, and it is
     currently defined by a dynamic object, but not by a regular
     object, then mark it as undefined so that the generic linker will
     force the correct value.  */
  if (provide
      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) != 0
      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
    h->root.type = bfd_link_hash_undefined;

  /* If this symbol is not being provided by the linker script, and it is
     currently defined by a dynamic object, but not by a regular object,
     then clear out any version information because the symbol will not be
     associated with the dynamic object any more.  */
  if (!provide
      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) != 0
      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
    h->verinfo.verdef = NULL;

  h->elf_link_hash_flags |= ELF_LINK_HASH_DEF_REGULAR;

  if (((h->elf_link_hash_flags & (ELF_LINK_HASH_DEF_DYNAMIC
				  | ELF_LINK_HASH_REF_DYNAMIC)) != 0
       || info->shared)
      && h->dynindx == -1)
    {
      if (! _bfd_elf_link_record_dynamic_symbol (info, h))
	return FALSE;

      /* If this is a weak defined symbol, and we know a corresponding
	 real symbol from the same dynamic object, make sure the real
	 symbol is also made into a dynamic symbol.  */
      if (h->weakdef != NULL
	  && h->weakdef->dynindx == -1)
	{
	  if (! _bfd_elf_link_record_dynamic_symbol (info, h->weakdef))
	    return FALSE;
	}
    }

  return TRUE;
}

/* Record a new local dynamic symbol.  Returns 0 on failure, 1 on
   success, and 2 on a failure caused by attempting to record a symbol
   in a discarded section, eg. a discarded link-once section symbol.  */

int
elf_link_record_local_dynamic_symbol (struct bfd_link_info *info,
				      bfd *input_bfd,
				      long input_indx)
{
  bfd_size_type amt;
  struct elf_link_local_dynamic_entry *entry;
  struct elf_link_hash_table *eht;
  struct elf_strtab_hash *dynstr;
  unsigned long dynstr_index;
  char *name;
  Elf_External_Sym_Shndx eshndx;
  char esym[sizeof (Elf64_External_Sym)];

  if (! is_elf_hash_table (info->hash))
    return 0;

  /* See if the entry exists already.  */
  for (entry = elf_hash_table (info)->dynlocal; entry ; entry = entry->next)
    if (entry->input_bfd == input_bfd && entry->input_indx == input_indx)
      return 1;

  amt = sizeof (*entry);
  entry = bfd_alloc (input_bfd, amt);
  if (entry == NULL)
    return 0;

  /* Go find the symbol, so that we can find it's name.  */
  if (!bfd_elf_get_elf_syms (input_bfd, &elf_tdata (input_bfd)->symtab_hdr,
			     1, input_indx, &entry->isym, esym, &eshndx))
    {
      bfd_release (input_bfd, entry);
      return 0;
    }

  if (entry->isym.st_shndx != SHN_UNDEF
      && (entry->isym.st_shndx < SHN_LORESERVE
	  || entry->isym.st_shndx > SHN_HIRESERVE))
    {
      asection *s;

      s = bfd_section_from_elf_index (input_bfd, entry->isym.st_shndx);
      if (s == NULL || bfd_is_abs_section (s->output_section))
	{
	  /* We can still bfd_release here as nothing has done another
	     bfd_alloc.  We can't do this later in this function.  */
	  bfd_release (input_bfd, entry);
	  return 2;
	}
    }

  name = (bfd_elf_string_from_elf_section
	  (input_bfd, elf_tdata (input_bfd)->symtab_hdr.sh_link,
	   entry->isym.st_name));

  dynstr = elf_hash_table (info)->dynstr;
  if (dynstr == NULL)
    {
      /* Create a strtab to hold the dynamic symbol names.  */
      elf_hash_table (info)->dynstr = dynstr = _bfd_elf_strtab_init ();
      if (dynstr == NULL)
	return 0;
    }

  dynstr_index = _bfd_elf_strtab_add (dynstr, name, FALSE);
  if (dynstr_index == (unsigned long) -1)
    return 0;
  entry->isym.st_name = dynstr_index;

  eht = elf_hash_table (info);

  entry->next = eht->dynlocal;
  eht->dynlocal = entry;
  entry->input_bfd = input_bfd;
  entry->input_indx = input_indx;
  eht->dynsymcount++;

  /* Whatever binding the symbol had before, it's now local.  */
  entry->isym.st_info
    = ELF_ST_INFO (STB_LOCAL, ELF_ST_TYPE (entry->isym.st_info));

  /* The dynindx will be set at the end of size_dynamic_sections.  */

  return 1;
}

/* Return the dynindex of a local dynamic symbol.  */

long
_bfd_elf_link_lookup_local_dynindx (struct bfd_link_info *info,
				    bfd *input_bfd,
				    long input_indx)
{
  struct elf_link_local_dynamic_entry *e;

  for (e = elf_hash_table (info)->dynlocal; e ; e = e->next)
    if (e->input_bfd == input_bfd && e->input_indx == input_indx)
      return e->dynindx;
  return -1;
}

/* This function is used to renumber the dynamic symbols, if some of
   them are removed because they are marked as local.  This is called
   via elf_link_hash_traverse.  */

static bfd_boolean
elf_link_renumber_hash_table_dynsyms (struct elf_link_hash_entry *h,
				      void *data)
{
  size_t *count = data;

  if (h->root.type == bfd_link_hash_warning)
    h = (struct elf_link_hash_entry *) h->root.u.i.link;

  if (h->dynindx != -1)
    h->dynindx = ++(*count);

  return TRUE;
}

/* Assign dynsym indices.  In a shared library we generate a section
   symbol for each output section, which come first.  Next come all of
   the back-end allocated local dynamic syms, followed by the rest of
   the global symbols.  */

unsigned long
_bfd_elf_link_renumber_dynsyms (bfd *output_bfd, struct bfd_link_info *info)
{
  unsigned long dynsymcount = 0;

  if (info->shared)
    {
      asection *p;
      for (p = output_bfd->sections; p ; p = p->next)
	if ((p->flags & SEC_EXCLUDE) == 0)
	  elf_section_data (p)->dynindx = ++dynsymcount;
    }

  if (elf_hash_table (info)->dynlocal)
    {
      struct elf_link_local_dynamic_entry *p;
      for (p = elf_hash_table (info)->dynlocal; p ; p = p->next)
	p->dynindx = ++dynsymcount;
    }

  elf_link_hash_traverse (elf_hash_table (info),
			  elf_link_renumber_hash_table_dynsyms,
			  &dynsymcount);

  /* There is an unused NULL entry at the head of the table which
     we must account for in our count.  Unless there weren't any
     symbols, which means we'll have no table at all.  */
  if (dynsymcount != 0)
    ++dynsymcount;

  return elf_hash_table (info)->dynsymcount = dynsymcount;
}

/* This function is called when we want to define a new symbol.  It
   handles the various cases which arise when we find a definition in
   a dynamic object, or when there is already a definition in a
   dynamic object.  The new symbol is described by NAME, SYM, PSEC,
   and PVALUE.  We set SYM_HASH to the hash table entry.  We set
   OVERRIDE if the old symbol is overriding a new definition.  We set
   TYPE_CHANGE_OK if it is OK for the type to change.  We set
   SIZE_CHANGE_OK if it is OK for the size to change.  By OK to
   change, we mean that we shouldn't warn if the type or size does
   change.  */

bfd_boolean
_bfd_elf_merge_symbol (bfd *abfd,
		       struct bfd_link_info *info,
		       const char *name,
		       Elf_Internal_Sym *sym,
		       asection **psec,
		       bfd_vma *pvalue,
		       struct elf_link_hash_entry **sym_hash,
		       bfd_boolean *skip,
		       bfd_boolean *override,
		       bfd_boolean *type_change_ok,
		       bfd_boolean *size_change_ok)
{
  asection *sec;
  struct elf_link_hash_entry *h;
  struct elf_link_hash_entry *flip;
  int bind;
  bfd *oldbfd;
  bfd_boolean newdyn, olddyn, olddef, newdef, newdyncommon, olddyncommon;
  bfd_boolean newweak, oldweak;

  *skip = FALSE;
  *override = FALSE;

  sec = *psec;
  bind = ELF_ST_BIND (sym->st_info);

  if (! bfd_is_und_section (sec))
    h = elf_link_hash_lookup (elf_hash_table (info), name, TRUE, FALSE, FALSE);
  else
    h = ((struct elf_link_hash_entry *)
	 bfd_wrapped_link_hash_lookup (abfd, info, name, TRUE, FALSE, FALSE));
  if (h == NULL)
    return FALSE;
  *sym_hash = h;

  /* This code is for coping with dynamic objects, and is only useful
     if we are doing an ELF link.  */
  if (info->hash->creator != abfd->xvec)
    return TRUE;

  /* For merging, we only care about real symbols.  */

  while (h->root.type == bfd_link_hash_indirect
	 || h->root.type == bfd_link_hash_warning)
    h = (struct elf_link_hash_entry *) h->root.u.i.link;

  /* If we just created the symbol, mark it as being an ELF symbol.
     Other than that, there is nothing to do--there is no merge issue
     with a newly defined symbol--so we just return.  */

  if (h->root.type == bfd_link_hash_new)
    {
      h->elf_link_hash_flags &=~ ELF_LINK_NON_ELF;
      return TRUE;
    }

  /* OLDBFD is a BFD associated with the existing symbol.  */

  switch (h->root.type)
    {
    default:
      oldbfd = NULL;
      break;

    case bfd_link_hash_undefined:
    case bfd_link_hash_undefweak:
      oldbfd = h->root.u.undef.abfd;
      break;

    case bfd_link_hash_defined:
    case bfd_link_hash_defweak:
      oldbfd = h->root.u.def.section->owner;
      break;

    case bfd_link_hash_common:
      oldbfd = h->root.u.c.p->section->owner;
      break;
    }

  /* In cases involving weak versioned symbols, we may wind up trying
     to merge a symbol with itself.  Catch that here, to avoid the
     confusion that results if we try to override a symbol with
     itself.  The additional tests catch cases like
     _GLOBAL_OFFSET_TABLE_, which are regular symbols defined in a
     dynamic object, which we do want to handle here.  */
  if (abfd == oldbfd
      && ((abfd->flags & DYNAMIC) == 0
	  || (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0))
    return TRUE;

  /* NEWDYN and OLDDYN indicate whether the new or old symbol,
     respectively, is from a dynamic object.  */

  if ((abfd->flags & DYNAMIC) != 0)
    newdyn = TRUE;
  else
    newdyn = FALSE;

  if (oldbfd != NULL)
    olddyn = (oldbfd->flags & DYNAMIC) != 0;
  else
    {
      asection *hsec;

      /* This code handles the special SHN_MIPS_{TEXT,DATA} section
	 indices used by MIPS ELF.  */
      switch (h->root.type)
	{
	default:
	  hsec = NULL;
	  break;

	case bfd_link_hash_defined:
	case bfd_link_hash_defweak:
	  hsec = h->root.u.def.section;
	  break;

	case bfd_link_hash_common:
	  hsec = h->root.u.c.p->section;
	  break;
	}

      if (hsec == NULL)
	olddyn = FALSE;
      else
	olddyn = (hsec->symbol->flags & BSF_DYNAMIC) != 0;
    }

  /* NEWDEF and OLDDEF indicate whether the new or old symbol,
     respectively, appear to be a definition rather than reference.  */

  if (bfd_is_und_section (sec) || bfd_is_com_section (sec))
    newdef = FALSE;
  else
    newdef = TRUE;

  if (h->root.type == bfd_link_hash_undefined
      || h->root.type == bfd_link_hash_undefweak
      || h->root.type == bfd_link_hash_common)
    olddef = FALSE;
  else
    olddef = TRUE;

  /* We need to remember if a symbol has a definition in a dynamic
     object or is weak in all dynamic objects. Internal and hidden
     visibility will make it unavailable to dynamic objects.  */
  if (newdyn && (h->elf_link_hash_flags & ELF_LINK_DYNAMIC_DEF) == 0)
    {
      if (!bfd_is_und_section (sec))
	h->elf_link_hash_flags |= ELF_LINK_DYNAMIC_DEF;
      else
	{
	  /* Check if this symbol is weak in all dynamic objects. If it
	     is the first time we see it in a dynamic object, we mark
	     if it is weak. Otherwise, we clear it.  */
	  if ((h->elf_link_hash_flags & ELF_LINK_HASH_REF_DYNAMIC) == 0)
	    {
	      if (bind == STB_WEAK)
		h->elf_link_hash_flags |= ELF_LINK_DYNAMIC_WEAK;
	    }
	  else if (bind != STB_WEAK)
	    h->elf_link_hash_flags &= ~ELF_LINK_DYNAMIC_WEAK;
	}
    }

  /* If the old symbol has non-default visibility, we ignore the new
     definition from a dynamic object.  */
  if (newdyn
      && ELF_ST_VISIBILITY (h->other) != STV_DEFAULT
      && !bfd_is_und_section (sec))
    {
      *skip = TRUE;
      /* Make sure this symbol is dynamic.  */
      h->elf_link_hash_flags |= ELF_LINK_HASH_REF_DYNAMIC;
      /* A protected symbol has external availability. Make sure it is
	 recorded as dynamic.

	 FIXME: Should we check type and size for protected symbol?  */
      if (ELF_ST_VISIBILITY (h->other) == STV_PROTECTED)
	return _bfd_elf_link_record_dynamic_symbol (info, h);
      else
	return TRUE;
    }
  else if (!newdyn
	   && ELF_ST_VISIBILITY (sym->st_other) != STV_DEFAULT
	   && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) != 0)
    {
      /* If the new symbol with non-default visibility comes from a
	 relocatable file and the old definition comes from a dynamic
	 object, we remove the old definition.  */
      if ((*sym_hash)->root.type == bfd_link_hash_indirect)
	h = *sym_hash;

      if ((h->root.und_next || info->hash->undefs_tail == &h->root)
	  && bfd_is_und_section (sec))
	{
	  /* If the new symbol is undefined and the old symbol was
	     also undefined before, we need to make sure
	     _bfd_generic_link_add_one_symbol doesn't mess
	     up the linker hash table undefs list. Since the old
	     definition came from a dynamic object, it is still on the
	     undefs list.  */
	  h->root.type = bfd_link_hash_undefined;
	  /* FIXME: What if the new symbol is weak undefined?  */
	  h->root.u.undef.abfd = abfd;
	}
      else
	{
	  h->root.type = bfd_link_hash_new;
	  h->root.u.undef.abfd = NULL;
	}

      if (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC)
	{
	  h->elf_link_hash_flags &= ~ELF_LINK_HASH_DEF_DYNAMIC;
	  h->elf_link_hash_flags |= (ELF_LINK_HASH_REF_DYNAMIC
				     | ELF_LINK_DYNAMIC_DEF);
	}
      /* FIXME: Should we check type and size for protected symbol?  */
      h->size = 0;
      h->type = 0;
      return TRUE;
    }

  /* Differentiate strong and weak symbols.  */
  newweak = bind == STB_WEAK;
  oldweak = (h->root.type == bfd_link_hash_defweak
	     || h->root.type == bfd_link_hash_undefweak);

  /* If a new weak symbol comes from a regular file and the old symbol
     comes from a dynamic library, we treat the new one as strong.
     Similarly, an old weak symbol from a regular file is treated as
     strong when the new symbol comes from a dynamic library.  Further,
     an old weak symbol from a dynamic library is treated as strong if
     the new symbol is from a dynamic library.  This reflects the way
     glibc's ld.so works.  */
  if (!newdyn && olddyn)
    newweak = FALSE;
  if (newdyn)
    oldweak = FALSE;

  /* It's OK to change the type if either the existing symbol or the
     new symbol is weak.  A type change is also OK if the old symbol
     is undefined and the new symbol is defined.  */

  if (oldweak
      || newweak
      || (newdef
	  && h->root.type == bfd_link_hash_undefined))
    *type_change_ok = TRUE;

  /* It's OK to change the size if either the existing symbol or the
     new symbol is weak, or if the old symbol is undefined.  */

  if (*type_change_ok
      || h->root.type == bfd_link_hash_undefined)
    *size_change_ok = TRUE;

  /* NEWDYNCOMMON and OLDDYNCOMMON indicate whether the new or old
     symbol, respectively, appears to be a common symbol in a dynamic
     object.  If a symbol appears in an uninitialized section, and is
     not weak, and is not a function, then it may be a common symbol
     which was resolved when the dynamic object was created.  We want
     to treat such symbols specially, because they raise special
     considerations when setting the symbol size: if the symbol
     appears as a common symbol in a regular object, and the size in
     the regular object is larger, we must make sure that we use the
     larger size.  This problematic case can always be avoided in C,
     but it must be handled correctly when using Fortran shared
     libraries.

     Note that if NEWDYNCOMMON is set, NEWDEF will be set, and
     likewise for OLDDYNCOMMON and OLDDEF.

     Note that this test is just a heuristic, and that it is quite
     possible to have an uninitialized symbol in a shared object which
     is really a definition, rather than a common symbol.  This could
     lead to some minor confusion when the symbol really is a common
     symbol in some regular object.  However, I think it will be
     harmless.  */

  if (newdyn
      && newdef
      && !newweak
      && (sec->flags & SEC_ALLOC) != 0
      && (sec->flags & SEC_LOAD) == 0
      && sym->st_size > 0
      && ELF_ST_TYPE (sym->st_info) != STT_FUNC)
    newdyncommon = TRUE;
  else
    newdyncommon = FALSE;

  if (olddyn
      && olddef
      && h->root.type == bfd_link_hash_defined
      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) != 0
      && (h->root.u.def.section->flags & SEC_ALLOC) != 0
      && (h->root.u.def.section->flags & SEC_LOAD) == 0
      && h->size > 0
      && h->type != STT_FUNC)
    olddyncommon = TRUE;
  else
    olddyncommon = FALSE;

  /* If both the old and the new symbols look like common symbols in a
     dynamic object, set the size of the symbol to the larger of the
     two.  */

  if (olddyncommon
      && newdyncommon
      && sym->st_size != h->size)
    {
      /* Since we think we have two common symbols, issue a multiple
	 common warning if desired.  Note that we only warn if the
	 size is different.  If the size is the same, we simply let
	 the old symbol override the new one as normally happens with
	 symbols defined in dynamic objects.  */

      if (! ((*info->callbacks->multiple_common)
	     (info, h->root.root.string, oldbfd, bfd_link_hash_common,
	      h->size, abfd, bfd_link_hash_common, sym->st_size)))
	return FALSE;

      if (sym->st_size > h->size)
	h->size = sym->st_size;

      *size_change_ok = TRUE;
    }

  /* If we are looking at a dynamic object, and we have found a
     definition, we need to see if the symbol was already defined by
     some other object.  If so, we want to use the existing
     definition, and we do not want to report a multiple symbol
     definition error; we do this by clobbering *PSEC to be
     bfd_und_section_ptr.

     We treat a common symbol as a definition if the symbol in the
     shared library is a function, since common symbols always
     represent variables; this can cause confusion in principle, but
     any such confusion would seem to indicate an erroneous program or
     shared library.  We also permit a common symbol in a regular
     object to override a weak symbol in a shared object.  */

  if (newdyn
      && newdef
      && (olddef
	  || (h->root.type == bfd_link_hash_common
	      && (newweak
		  || ELF_ST_TYPE (sym->st_info) == STT_FUNC))))
    {
      *override = TRUE;
      newdef = FALSE;
      newdyncommon = FALSE;

      *psec = sec = bfd_und_section_ptr;
      *size_change_ok = TRUE;

      /* If we get here when the old symbol is a common symbol, then
	 we are explicitly letting it override a weak symbol or
	 function in a dynamic object, and we don't want to warn about
	 a type change.  If the old symbol is a defined symbol, a type
	 change warning may still be appropriate.  */

      if (h->root.type == bfd_link_hash_common)
	*type_change_ok = TRUE;
    }

  /* Handle the special case of an old common symbol merging with a
     new symbol which looks like a common symbol in a shared object.
     We change *PSEC and *PVALUE to make the new symbol look like a
     common symbol, and let _bfd_generic_link_add_one_symbol will do
     the right thing.  */

  if (newdyncommon
      && h->root.type == bfd_link_hash_common)
    {
      *override = TRUE;
      newdef = FALSE;
      newdyncommon = FALSE;
      *pvalue = sym->st_size;
      *psec = sec = bfd_com_section_ptr;
      *size_change_ok = TRUE;
    }

  /* If the old symbol is from a dynamic object, and the new symbol is
     a definition which is not from a dynamic object, then the new
     symbol overrides the old symbol.  Symbols from regular files
     always take precedence over symbols from dynamic objects, even if
     they are defined after the dynamic object in the link.

     As above, we again permit a common symbol in a regular object to
     override a definition in a shared object if the shared object
     symbol is a function or is weak.  */

  flip = NULL;
  if (! newdyn
      && (newdef
	  || (bfd_is_com_section (sec)
	      && (oldweak
		  || h->type == STT_FUNC)))
      && olddyn
      && olddef
      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) != 0)
    {
      /* Change the hash table entry to undefined, and let
	 _bfd_generic_link_add_one_symbol do the right thing with the
	 new definition.  */

      h->root.type = bfd_link_hash_undefined;
      h->root.u.undef.abfd = h->root.u.def.section->owner;
      *size_change_ok = TRUE;

      olddef = FALSE;
      olddyncommon = FALSE;

      /* We again permit a type change when a common symbol may be
	 overriding a function.  */

      if (bfd_is_com_section (sec))
	*type_change_ok = TRUE;

      if ((*sym_hash)->root.type == bfd_link_hash_indirect)
	flip = *sym_hash;
      else
	/* This union may have been set to be non-NULL when this symbol
	   was seen in a dynamic object.  We must force the union to be
	   NULL, so that it is correct for a regular symbol.  */
	h->verinfo.vertree = NULL;
    }

  /* Handle the special case of a new common symbol merging with an
     old symbol that looks like it might be a common symbol defined in
     a shared object.  Note that we have already handled the case in
     which a new common symbol should simply override the definition
     in the shared library.  */

  if (! newdyn
      && bfd_is_com_section (sec)
      && olddyncommon)
    {
      /* It would be best if we could set the hash table entry to a
	 common symbol, but we don't know what to use for the section
	 or the alignment.  */
      if (! ((*info->callbacks->multiple_common)
	     (info, h->root.root.string, oldbfd, bfd_link_hash_common,
	      h->size, abfd, bfd_link_hash_common, sym->st_size)))
	return FALSE;

      /* If the presumed common symbol in the dynamic object is
	 larger, pretend that the new symbol has its size.  */

      if (h->size > *pvalue)
	*pvalue = h->size;

      /* FIXME: We no longer know the alignment required by the symbol
	 in the dynamic object, so we just wind up using the one from
	 the regular object.  */

      olddef = FALSE;
      olddyncommon = FALSE;

      h->root.type = bfd_link_hash_undefined;
      h->root.u.undef.abfd = h->root.u.def.section->owner;

      *size_change_ok = TRUE;
      *type_change_ok = TRUE;

      if ((*sym_hash)->root.type == bfd_link_hash_indirect)
	flip = *sym_hash;
      else
	h->verinfo.vertree = NULL;
    }

  if (flip != NULL)
    {
      /* Handle the case where we had a versioned symbol in a dynamic
	 library and now find a definition in a normal object.  In this
	 case, we make the versioned symbol point to the normal one.  */
      const struct elf_backend_data *bed = get_elf_backend_data (abfd);
      flip->root.type = h->root.type;
      h->root.type = bfd_link_hash_indirect;
      h->root.u.i.link = (struct bfd_link_hash_entry *) flip;
      (*bed->elf_backend_copy_indirect_symbol) (bed, flip, h);
      flip->root.u.undef.abfd = h->root.u.undef.abfd;
      if (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC)
	{
	  h->elf_link_hash_flags &= ~ELF_LINK_HASH_DEF_DYNAMIC;
	  flip->elf_link_hash_flags |= ELF_LINK_HASH_REF_DYNAMIC;
	}
    }

  return TRUE;
}

/* This function is called to create an indirect symbol from the
   default for the symbol with the default version if needed. The
   symbol is described by H, NAME, SYM, PSEC, VALUE, and OVERRIDE.  We
   set DYNSYM if the new indirect symbol is dynamic.  */

bfd_boolean
_bfd_elf_add_default_symbol (bfd *abfd,
			     struct bfd_link_info *info,
			     struct elf_link_hash_entry *h,
			     const char *name,
			     Elf_Internal_Sym *sym,
			     asection **psec,
			     bfd_vma *value,
			     bfd_boolean *dynsym,
			     bfd_boolean override)
{
  bfd_boolean type_change_ok;
  bfd_boolean size_change_ok;
  bfd_boolean skip;
  char *shortname;
  struct elf_link_hash_entry *hi;
  struct bfd_link_hash_entry *bh;
  const struct elf_backend_data *bed;
  bfd_boolean collect;
  bfd_boolean dynamic;
  char *p;
  size_t len, shortlen;
  asection *sec;

  /* If this symbol has a version, and it is the default version, we
     create an indirect symbol from the default name to the fully
     decorated name.  This will cause external references which do not
     specify a version to be bound to this version of the symbol.  */
  p = strchr (name, ELF_VER_CHR);
  if (p == NULL || p[1] != ELF_VER_CHR)
    return TRUE;

  if (override)
    {
      /* We are overridden by an old definition. We need to check if we
	 need to create the indirect symbol from the default name.  */
      hi = elf_link_hash_lookup (elf_hash_table (info), name, TRUE,
				 FALSE, FALSE);
      BFD_ASSERT (hi != NULL);
      if (hi == h)
	return TRUE;
      while (hi->root.type == bfd_link_hash_indirect
	     || hi->root.type == bfd_link_hash_warning)
	{
	  hi = (struct elf_link_hash_entry *) hi->root.u.i.link;
	  if (hi == h)
	    return TRUE;
	}
    }

  bed = get_elf_backend_data (abfd);
  collect = bed->collect;
  dynamic = (abfd->flags & DYNAMIC) != 0;

  shortlen = p - name;
  shortname = bfd_hash_allocate (&info->hash->table, shortlen + 1);
  if (shortname == NULL)
    return FALSE;
  memcpy (shortname, name, shortlen);
  shortname[shortlen] = '\0';

  /* We are going to create a new symbol.  Merge it with any existing
     symbol with this name.  For the purposes of the merge, act as
     though we were defining the symbol we just defined, although we
     actually going to define an indirect symbol.  */
  type_change_ok = FALSE;
  size_change_ok = FALSE;
  sec = *psec;
  if (!_bfd_elf_merge_symbol (abfd, info, shortname, sym, &sec, value,
			      &hi, &skip, &override, &type_change_ok,
			      &size_change_ok))
    return FALSE;

  if (skip)
    goto nondefault;

  if (! override)
    {
      bh = &hi->root;
      if (! (_bfd_generic_link_add_one_symbol
	     (info, abfd, shortname, BSF_INDIRECT, bfd_ind_section_ptr,
	      0, name, FALSE, collect, &bh)))
	return FALSE;
      hi = (struct elf_link_hash_entry *) bh;
    }
  else
    {
      /* In this case the symbol named SHORTNAME is overriding the
	 indirect symbol we want to add.  We were planning on making
	 SHORTNAME an indirect symbol referring to NAME.  SHORTNAME
	 is the name without a version.  NAME is the fully versioned
	 name, and it is the default version.

	 Overriding means that we already saw a definition for the
	 symbol SHORTNAME in a regular object, and it is overriding
	 the symbol defined in the dynamic object.

	 When this happens, we actually want to change NAME, the
	 symbol we just added, to refer to SHORTNAME.  This will cause
	 references to NAME in the shared object to become references
	 to SHORTNAME in the regular object.  This is what we expect
	 when we override a function in a shared object: that the
	 references in the shared object will be mapped to the
	 definition in the regular object.  */

      while (hi->root.type == bfd_link_hash_indirect
	     || hi->root.type == bfd_link_hash_warning)
	hi = (struct elf_link_hash_entry *) hi->root.u.i.link;

      h->root.type = bfd_link_hash_indirect;
      h->root.u.i.link = (struct bfd_link_hash_entry *) hi;
      if (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC)
	{
	  h->elf_link_hash_flags &=~ ELF_LINK_HASH_DEF_DYNAMIC;
	  hi->elf_link_hash_flags |= ELF_LINK_HASH_REF_DYNAMIC;
	  if (hi->elf_link_hash_flags
	      & (ELF_LINK_HASH_REF_REGULAR
		 | ELF_LINK_HASH_DEF_REGULAR))
	    {
	      if (! _bfd_elf_link_record_dynamic_symbol (info, hi))
		return FALSE;
	    }
	}

      /* Now set HI to H, so that the following code will set the
	 other fields correctly.  */
      hi = h;
    }

  /* If there is a duplicate definition somewhere, then HI may not
     point to an indirect symbol.  We will have reported an error to
     the user in that case.  */

  if (hi->root.type == bfd_link_hash_indirect)
    {
      struct elf_link_hash_entry *ht;

      ht = (struct elf_link_hash_entry *) hi->root.u.i.link;
      (*bed->elf_backend_copy_indirect_symbol) (bed, ht, hi);

      /* See if the new flags lead us to realize that the symbol must
	 be dynamic.  */
      if (! *dynsym)
	{
	  if (! dynamic)
	    {
	      if (info->shared
		  || ((hi->elf_link_hash_flags
		       & ELF_LINK_HASH_REF_DYNAMIC) != 0))
		*dynsym = TRUE;
	    }
	  else
	    {
	      if ((hi->elf_link_hash_flags
		   & ELF_LINK_HASH_REF_REGULAR) != 0)
		*dynsym = TRUE;
	    }
	}
    }

  /* We also need to define an indirection from the nondefault version
     of the symbol.  */

nondefault:
  len = strlen (name);
  shortname = bfd_hash_allocate (&info->hash->table, len);
  if (shortname == NULL)
    return FALSE;
  memcpy (shortname, name, shortlen);
  memcpy (shortname + shortlen, p + 1, len - shortlen);

  /* Once again, merge with any existing symbol.  */
  type_change_ok = FALSE;
  size_change_ok = FALSE;
  sec = *psec;
  if (!_bfd_elf_merge_symbol (abfd, info, shortname, sym, &sec, value,
			      &hi, &skip, &override, &type_change_ok,
			      &size_change_ok))
    return FALSE;

  if (skip)
    return TRUE;

  if (override)
    {
      /* Here SHORTNAME is a versioned name, so we don't expect to see
	 the type of override we do in the case above unless it is
	 overridden by a versioned definition.  */
      if (hi->root.type != bfd_link_hash_defined
	  && hi->root.type != bfd_link_hash_defweak)
	(*_bfd_error_handler)
	  (_("%s: warning: unexpected redefinition of indirect versioned symbol `%s'"),
	   bfd_archive_filename (abfd), shortname);
    }
  else
    {
      bh = &hi->root;
      if (! (_bfd_generic_link_add_one_symbol
	     (info, abfd, shortname, BSF_INDIRECT,
	      bfd_ind_section_ptr, 0, name, FALSE, collect, &bh)))
	return FALSE;
      hi = (struct elf_link_hash_entry *) bh;

      /* If there is a duplicate definition somewhere, then HI may not
	 point to an indirect symbol.  We will have reported an error
	 to the user in that case.  */

      if (hi->root.type == bfd_link_hash_indirect)
	{
	  (*bed->elf_backend_copy_indirect_symbol) (bed, h, hi);

	  /* See if the new flags lead us to realize that the symbol
	     must be dynamic.  */
	  if (! *dynsym)
	    {
	      if (! dynamic)
		{
		  if (info->shared
		      || ((hi->elf_link_hash_flags
			   & ELF_LINK_HASH_REF_DYNAMIC) != 0))
		    *dynsym = TRUE;
		}
	      else
		{
		  if ((hi->elf_link_hash_flags
		       & ELF_LINK_HASH_REF_REGULAR) != 0)
		    *dynsym = TRUE;
		}
	    }
	}
    }

  return TRUE;
}

/* This routine is used to export all defined symbols into the dynamic
   symbol table.  It is called via elf_link_hash_traverse.  */

bfd_boolean
_bfd_elf_export_symbol (struct elf_link_hash_entry *h, void *data)
{
  struct elf_info_failed *eif = data;

  /* Ignore indirect symbols.  These are added by the versioning code.  */
  if (h->root.type == bfd_link_hash_indirect)
    return TRUE;

  if (h->root.type == bfd_link_hash_warning)
    h = (struct elf_link_hash_entry *) h->root.u.i.link;

  if (h->dynindx == -1
      && (h->elf_link_hash_flags
	  & (ELF_LINK_HASH_DEF_REGULAR | ELF_LINK_HASH_REF_REGULAR)) != 0)
    {
      struct bfd_elf_version_tree *t;
      struct bfd_elf_version_expr *d;

      for (t = eif->verdefs; t != NULL; t = t->next)
	{
	  if (t->globals.list != NULL)
	    {
	      d = (*t->match) (&t->globals, NULL, h->root.root.string);
	      if (d != NULL)
		goto doit;
	    }

	  if (t->locals.list != NULL)
	    {
	      d = (*t->match) (&t->locals, NULL, h->root.root.string);
	      if (d != NULL)
		return TRUE;
	    }
	}

      if (!eif->verdefs)
	{
	doit:
	  if (! _bfd_elf_link_record_dynamic_symbol (eif->info, h))
	    {
	      eif->failed = TRUE;
	      return FALSE;
	    }
	}
    }

  return TRUE;
}

/* Look through the symbols which are defined in other shared
   libraries and referenced here.  Update the list of version
   dependencies.  This will be put into the .gnu.version_r section.
   This function is called via elf_link_hash_traverse.  */

bfd_boolean
_bfd_elf_link_find_version_dependencies (struct elf_link_hash_entry *h,
					 void *data)
{
  struct elf_find_verdep_info *rinfo = data;
  Elf_Internal_Verneed *t;
  Elf_Internal_Vernaux *a;
  bfd_size_type amt;

  if (h->root.type == bfd_link_hash_warning)
    h = (struct elf_link_hash_entry *) h->root.u.i.link;

  /* We only care about symbols defined in shared objects with version
     information.  */
  if ((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) == 0
      || (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) != 0
      || h->dynindx == -1
      || h->verinfo.verdef == NULL)
    return TRUE;

  /* See if we already know about this version.  */
  for (t = elf_tdata (rinfo->output_bfd)->verref; t != NULL; t = t->vn_nextref)
    {
      if (t->vn_bfd != h->verinfo.verdef->vd_bfd)
	continue;

      for (a = t->vn_auxptr; a != NULL; a = a->vna_nextptr)
	if (a->vna_nodename == h->verinfo.verdef->vd_nodename)
	  return TRUE;

      break;
    }

  /* This is a new version.  Add it to tree we are building.  */

  if (t == NULL)
    {
      amt = sizeof *t;
      t = bfd_zalloc (rinfo->output_bfd, amt);
      if (t == NULL)
	{
	  rinfo->failed = TRUE;
	  return FALSE;
	}

      t->vn_bfd = h->verinfo.verdef->vd_bfd;
      t->vn_nextref = elf_tdata (rinfo->output_bfd)->verref;
      elf_tdata (rinfo->output_bfd)->verref = t;
    }

  amt = sizeof *a;
  a = bfd_zalloc (rinfo->output_bfd, amt);

  /* Note that we are copying a string pointer here, and testing it
     above.  If bfd_elf_string_from_elf_section is ever changed to
     discard the string data when low in memory, this will have to be
     fixed.  */
  a->vna_nodename = h->verinfo.verdef->vd_nodename;

  a->vna_flags = h->verinfo.verdef->vd_flags;
  a->vna_nextptr = t->vn_auxptr;

  h->verinfo.verdef->vd_exp_refno = rinfo->vers;
  ++rinfo->vers;

  a->vna_other = h->verinfo.verdef->vd_exp_refno + 1;

  t->vn_auxptr = a;

  return TRUE;
}

/* Figure out appropriate versions for all the symbols.  We may not
   have the version number script until we have read all of the input
   files, so until that point we don't know which symbols should be
   local.  This function is called via elf_link_hash_traverse.  */

bfd_boolean
_bfd_elf_link_assign_sym_version (struct elf_link_hash_entry *h, void *data)
{
  struct elf_assign_sym_version_info *sinfo;
  struct bfd_link_info *info;
  const struct elf_backend_data *bed;
  struct elf_info_failed eif;
  char *p;
  bfd_size_type amt;

  sinfo = data;
  info = sinfo->info;

  if (h->root.type == bfd_link_hash_warning)
    h = (struct elf_link_hash_entry *) h->root.u.i.link;

  /* Fix the symbol flags.  */
  eif.failed = FALSE;
  eif.info = info;
  if (! _bfd_elf_fix_symbol_flags (h, &eif))
    {
      if (eif.failed)
	sinfo->failed = TRUE;
      return FALSE;
    }

  /* We only need version numbers for symbols defined in regular
     objects.  */
  if ((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
    return TRUE;

  bed = get_elf_backend_data (sinfo->output_bfd);
  p = strchr (h->root.root.string, ELF_VER_CHR);
  if (p != NULL && h->verinfo.vertree == NULL)
    {
      struct bfd_elf_version_tree *t;
      bfd_boolean hidden;

      hidden = TRUE;

      /* There are two consecutive ELF_VER_CHR characters if this is
	 not a hidden symbol.  */
      ++p;
      if (*p == ELF_VER_CHR)
	{
	  hidden = FALSE;
	  ++p;
	}

      /* If there is no version string, we can just return out.  */
      if (*p == '\0')
	{
	  if (hidden)
	    h->elf_link_hash_flags |= ELF_LINK_HIDDEN;
	  return TRUE;
	}

      /* Look for the version.  If we find it, it is no longer weak.  */
      for (t = sinfo->verdefs; t != NULL; t = t->next)
	{
	  if (strcmp (t->name, p) == 0)
	    {
	      size_t len;
	      char *alc;
	      struct bfd_elf_version_expr *d;

	      len = p - h->root.root.string;
	      alc = bfd_malloc (len);
	      if (alc == NULL)
		return FALSE;
	      memcpy (alc, h->root.root.string, len - 1);
	      alc[len - 1] = '\0';
	      if (alc[len - 2] == ELF_VER_CHR)
		alc[len - 2] = '\0';

	      h->verinfo.vertree = t;
	      t->used = TRUE;
	      d = NULL;

	      if (t->globals.list != NULL)
		d = (*t->match) (&t->globals, NULL, alc);

	      /* See if there is anything to force this symbol to
		 local scope.  */
	      if (d == NULL && t->locals.list != NULL)
		{
		  d = (*t->match) (&t->locals, NULL, alc);
		  if (d != NULL
		      && h->dynindx != -1
		      && info->shared
		      && ! info->export_dynamic)
		    (*bed->elf_backend_hide_symbol) (info, h, TRUE);
		}

	      free (alc);
	      break;
	    }
	}

      /* If we are building an application, we need to create a
	 version node for this version.  */
      if (t == NULL && info->executable)
	{
	  struct bfd_elf_version_tree **pp;
	  int version_index;

	  /* If we aren't going to export this symbol, we don't need
	     to worry about it.  */
	  if (h->dynindx == -1)
	    return TRUE;

	  amt = sizeof *t;
	  t = bfd_zalloc (sinfo->output_bfd, amt);
	  if (t == NULL)
	    {
	      sinfo->failed = TRUE;
	      return FALSE;
	    }

	  t->name = p;
	  t->name_indx = (unsigned int) -1;
	  t->used = TRUE;

	  version_index = 1;
	  /* Don't count anonymous version tag.  */
	  if (sinfo->verdefs != NULL && sinfo->verdefs->vernum == 0)
	    version_index = 0;
	  for (pp = &sinfo->verdefs; *pp != NULL; pp = &(*pp)->next)
	    ++version_index;
	  t->vernum = version_index;

	  *pp = t;

	  h->verinfo.vertree = t;
	}
      else if (t == NULL)
	{
	  /* We could not find the version for a symbol when
	     generating a shared archive.  Return an error.  */
	  (*_bfd_error_handler)
	    (_("%s: undefined versioned symbol name %s"),
	     bfd_get_filename (sinfo->output_bfd), h->root.root.string);
	  bfd_set_error (bfd_error_bad_value);
	  sinfo->failed = TRUE;
	  return FALSE;
	}

      if (hidden)
	h->elf_link_hash_flags |= ELF_LINK_HIDDEN;
    }

  /* If we don't have a version for this symbol, see if we can find
     something.  */
  if (h->verinfo.vertree == NULL && sinfo->verdefs != NULL)
    {
      struct bfd_elf_version_tree *t;
      struct bfd_elf_version_tree *local_ver;
      struct bfd_elf_version_expr *d;

      /* See if can find what version this symbol is in.  If the
	 symbol is supposed to be local, then don't actually register
	 it.  */
      local_ver = NULL;
      for (t = sinfo->verdefs; t != NULL; t = t->next)
	{
	  if (t->globals.list != NULL)
	    {
	      bfd_boolean matched;

	      matched = FALSE;
	      d = NULL;
	      while ((d = (*t->match) (&t->globals, d,
				       h->root.root.string)) != NULL)
		if (d->symver)
		  matched = TRUE;
		else
		  {
		    /* There is a version without definition.  Make
		       the symbol the default definition for this
		       version.  */
		    h->verinfo.vertree = t;
		    local_ver = NULL;
		    d->script = 1;
		    break;
		  }
	      if (d != NULL)
		break;
	      else if (matched)
		/* There is no undefined version for this symbol. Hide the
		   default one.  */
		(*bed->elf_backend_hide_symbol) (info, h, TRUE);
	    }

	  if (t->locals.list != NULL)
	    {
	      d = NULL;
	      while ((d = (*t->match) (&t->locals, d,
				       h->root.root.string)) != NULL)
		{
		  local_ver = t;
		  /* If the match is "*", keep looking for a more
		     explicit, perhaps even global, match.
		     XXX: Shouldn't this be !d->wildcard instead?  */
		  if (d->pattern[0] != '*' || d->pattern[1] != '\0')
		    break;
		}

	      if (d != NULL)
		break;
	    }
	}

      if (local_ver != NULL)
	{
	  h->verinfo.vertree = local_ver;
	  if (h->dynindx != -1
	      && info->shared
	      && ! info->export_dynamic)
	    {
	      (*bed->elf_backend_hide_symbol) (info, h, TRUE);
	    }
	}
    }

  return TRUE;
}

/* Read and swap the relocs from the section indicated by SHDR.  This
   may be either a REL or a RELA section.  The relocations are
   translated into RELA relocations and stored in INTERNAL_RELOCS,
   which should have already been allocated to contain enough space.
   The EXTERNAL_RELOCS are a buffer where the external form of the
   relocations should be stored.

   Returns FALSE if something goes wrong.  */

static bfd_boolean
elf_link_read_relocs_from_section (bfd *abfd,
				   asection *sec,
				   Elf_Internal_Shdr *shdr,
				   void *external_relocs,
				   Elf_Internal_Rela *internal_relocs)
{
  const struct elf_backend_data *bed;
  void (*swap_in) (bfd *, const bfd_byte *, Elf_Internal_Rela *);
  const bfd_byte *erela;
  const bfd_byte *erelaend;
  Elf_Internal_Rela *irela;
  Elf_Internal_Shdr *symtab_hdr;
  size_t nsyms;

  /* Position ourselves at the start of the section.  */
  if (bfd_seek (abfd, shdr->sh_offset, SEEK_SET) != 0)
    return FALSE;

  /* Read the relocations.  */
  if (bfd_bread (external_relocs, shdr->sh_size, abfd) != shdr->sh_size)
    return FALSE;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  nsyms = symtab_hdr->sh_size / symtab_hdr->sh_entsize;

  bed = get_elf_backend_data (abfd);

  /* Convert the external relocations to the internal format.  */
  if (shdr->sh_entsize == bed->s->sizeof_rel)
    swap_in = bed->s->swap_reloc_in;
  else if (shdr->sh_entsize == bed->s->sizeof_rela)
    swap_in = bed->s->swap_reloca_in;
  else
    {
      bfd_set_error (bfd_error_wrong_format);
      return FALSE;
    }

  erela = external_relocs;
  erelaend = erela + shdr->sh_size;
  irela = internal_relocs;
  while (erela < erelaend)
    {
      bfd_vma r_symndx;

      (*swap_in) (abfd, erela, irela);
      r_symndx = ELF32_R_SYM (irela->r_info);
      if (bed->s->arch_size == 64)
	r_symndx >>= 24;
      if ((size_t) r_symndx >= nsyms)
	{
	  (*_bfd_error_handler)
	    (_("%s: bad reloc symbol index (0x%lx >= 0x%lx) for offset 0x%lx in section `%s'"),
	     bfd_archive_filename (abfd), (unsigned long) r_symndx,
	     (unsigned long) nsyms, irela->r_offset, sec->name);
	  bfd_set_error (bfd_error_bad_value);
	  return FALSE;
	}
      irela += bed->s->int_rels_per_ext_rel;
      erela += shdr->sh_entsize;
    }

  return TRUE;
}

/* Read and swap the relocs for a section O.  They may have been
   cached.  If the EXTERNAL_RELOCS and INTERNAL_RELOCS arguments are
   not NULL, they are used as buffers to read into.  They are known to
   be large enough.  If the INTERNAL_RELOCS relocs argument is NULL,
   the return value is allocated using either malloc or bfd_alloc,
   according to the KEEP_MEMORY argument.  If O has two relocation
   sections (both REL and RELA relocations), then the REL_HDR
   relocations will appear first in INTERNAL_RELOCS, followed by the
   REL_HDR2 relocations.  */

Elf_Internal_Rela *
_bfd_elf_link_read_relocs (bfd *abfd,
			   asection *o,
			   void *external_relocs,
			   Elf_Internal_Rela *internal_relocs,
			   bfd_boolean keep_memory)
{
  Elf_Internal_Shdr *rel_hdr;
  void *alloc1 = NULL;
  Elf_Internal_Rela *alloc2 = NULL;
  const struct elf_backend_data *bed = get_elf_backend_data (abfd);

  if (elf_section_data (o)->relocs != NULL)
    return elf_section_data (o)->relocs;

  if (o->reloc_count == 0)
    return NULL;

  rel_hdr = &elf_section_data (o)->rel_hdr;

  if (internal_relocs == NULL)
    {
      bfd_size_type size;

      size = o->reloc_count;
      size *= bed->s->int_rels_per_ext_rel * sizeof (Elf_Internal_Rela);
      if (keep_memory)
	internal_relocs = bfd_alloc (abfd, size);
      else
	internal_relocs = alloc2 = bfd_malloc (size);
      if (internal_relocs == NULL)
	goto error_return;
    }

  if (external_relocs == NULL)
    {
      bfd_size_type size = rel_hdr->sh_size;

      if (elf_section_data (o)->rel_hdr2)
	size += elf_section_data (o)->rel_hdr2->sh_size;
      alloc1 = bfd_malloc (size);
      if (alloc1 == NULL)
	goto error_return;
      external_relocs = alloc1;
    }

  if (!elf_link_read_relocs_from_section (abfd, o, rel_hdr,
					  external_relocs,
					  internal_relocs))
    goto error_return;
  if (elf_section_data (o)->rel_hdr2
      && (!elf_link_read_relocs_from_section
	  (abfd, o,
	   elf_section_data (o)->rel_hdr2,
	   ((bfd_byte *) external_relocs) + rel_hdr->sh_size,
	   internal_relocs + (NUM_SHDR_ENTRIES (rel_hdr)
			      * bed->s->int_rels_per_ext_rel))))
    goto error_return;

  /* Cache the results for next time, if we can.  */
  if (keep_memory)
    elf_section_data (o)->relocs = internal_relocs;

  if (alloc1 != NULL)
    free (alloc1);

  /* Don't free alloc2, since if it was allocated we are passing it
     back (under the name of internal_relocs).  */

  return internal_relocs;

 error_return:
  if (alloc1 != NULL)
    free (alloc1);
  if (alloc2 != NULL)
    free (alloc2);
  return NULL;
}

/* Compute the size of, and allocate space for, REL_HDR which is the
   section header for a section containing relocations for O.  */

bfd_boolean
_bfd_elf_link_size_reloc_section (bfd *abfd,
				  Elf_Internal_Shdr *rel_hdr,
				  asection *o)
{
  bfd_size_type reloc_count;
  bfd_size_type num_rel_hashes;

  /* Figure out how many relocations there will be.  */
  if (rel_hdr == &elf_section_data (o)->rel_hdr)
    reloc_count = elf_section_data (o)->rel_count;
  else
    reloc_count = elf_section_data (o)->rel_count2;

  num_rel_hashes = o->reloc_count;
  if (num_rel_hashes < reloc_count)
    num_rel_hashes = reloc_count;

  /* That allows us to calculate the size of the section.  */
  rel_hdr->sh_size = rel_hdr->sh_entsize * reloc_count;

  /* The contents field must last into write_object_contents, so we
     allocate it with bfd_alloc rather than malloc.  Also since we
     cannot be sure that the contents will actually be filled in,
     we zero the allocated space.  */
  rel_hdr->contents = bfd_zalloc (abfd, rel_hdr->sh_size);
  if (rel_hdr->contents == NULL && rel_hdr->sh_size != 0)
    return FALSE;

  /* We only allocate one set of hash entries, so we only do it the
     first time we are called.  */
  if (elf_section_data (o)->rel_hashes == NULL
      && num_rel_hashes)
    {
      struct elf_link_hash_entry **p;

      p = bfd_zmalloc (num_rel_hashes * sizeof (struct elf_link_hash_entry *));
      if (p == NULL)
	return FALSE;

      elf_section_data (o)->rel_hashes = p;
    }

  return TRUE;
}

/* Copy the relocations indicated by the INTERNAL_RELOCS (which
   originated from the section given by INPUT_REL_HDR) to the
   OUTPUT_BFD.  */

bfd_boolean
_bfd_elf_link_output_relocs (bfd *output_bfd,
			     asection *input_section,
			     Elf_Internal_Shdr *input_rel_hdr,
			     Elf_Internal_Rela *internal_relocs)
{
  Elf_Internal_Rela *irela;
  Elf_Internal_Rela *irelaend;
  bfd_byte *erel;
  Elf_Internal_Shdr *output_rel_hdr;
  asection *output_section;
  unsigned int *rel_countp = NULL;
  const struct elf_backend_data *bed;
  void (*swap_out) (bfd *, const Elf_Internal_Rela *, bfd_byte *);

  output_section = input_section->output_section;
  output_rel_hdr = NULL;

  if (elf_section_data (output_section)->rel_hdr.sh_entsize
      == input_rel_hdr->sh_entsize)
    {
      output_rel_hdr = &elf_section_data (output_section)->rel_hdr;
      rel_countp = &elf_section_data (output_section)->rel_count;
    }
  else if (elf_section_data (output_section)->rel_hdr2
	   && (elf_section_data (output_section)->rel_hdr2->sh_entsize
	       == input_rel_hdr->sh_entsize))
    {
      output_rel_hdr = elf_section_data (output_section)->rel_hdr2;
      rel_countp = &elf_section_data (output_section)->rel_count2;
    }
  else
    {
      (*_bfd_error_handler)
	(_("%s: relocation size mismatch in %s section %s"),
	 bfd_get_filename (output_bfd),
	 bfd_archive_filename (input_section->owner),
	 input_section->name);
      bfd_set_error (bfd_error_wrong_object_format);
      return FALSE;
    }

  bed = get_elf_backend_data (output_bfd);
  if (input_rel_hdr->sh_entsize == bed->s->sizeof_rel)
    swap_out = bed->s->swap_reloc_out;
  else if (input_rel_hdr->sh_entsize == bed->s->sizeof_rela)
    swap_out = bed->s->swap_reloca_out;
  else
    abort ();

  erel = output_rel_hdr->contents;
  erel += *rel_countp * input_rel_hdr->sh_entsize;
  irela = internal_relocs;
  irelaend = irela + (NUM_SHDR_ENTRIES (input_rel_hdr)
		      * bed->s->int_rels_per_ext_rel);
  while (irela < irelaend)
    {
      (*swap_out) (output_bfd, irela, erel);
      irela += bed->s->int_rels_per_ext_rel;
      erel += input_rel_hdr->sh_entsize;
    }

  /* Bump the counter, so that we know where to add the next set of
     relocations.  */
  *rel_countp += NUM_SHDR_ENTRIES (input_rel_hdr);

  return TRUE;
}

/* Fix up the flags for a symbol.  This handles various cases which
   can only be fixed after all the input files are seen.  This is
   currently called by both adjust_dynamic_symbol and
   assign_sym_version, which is unnecessary but perhaps more robust in
   the face of future changes.  */

bfd_boolean
_bfd_elf_fix_symbol_flags (struct elf_link_hash_entry *h,
			   struct elf_info_failed *eif)
{
  /* If this symbol was mentioned in a non-ELF file, try to set
     DEF_REGULAR and REF_REGULAR correctly.  This is the only way to
     permit a non-ELF file to correctly refer to a symbol defined in
     an ELF dynamic object.  */
  if ((h->elf_link_hash_flags & ELF_LINK_NON_ELF) != 0)
    {
      while (h->root.type == bfd_link_hash_indirect)
	h = (struct elf_link_hash_entry *) h->root.u.i.link;

      if (h->root.type != bfd_link_hash_defined
	  && h->root.type != bfd_link_hash_defweak)
	h->elf_link_hash_flags |= (ELF_LINK_HASH_REF_REGULAR
				   | ELF_LINK_HASH_REF_REGULAR_NONWEAK);
      else
	{
	  if (h->root.u.def.section->owner != NULL
	      && (bfd_get_flavour (h->root.u.def.section->owner)
		  == bfd_target_elf_flavour))
	    h->elf_link_hash_flags |= (ELF_LINK_HASH_REF_REGULAR
				       | ELF_LINK_HASH_REF_REGULAR_NONWEAK);
	  else
	    h->elf_link_hash_flags |= ELF_LINK_HASH_DEF_REGULAR;
	}

      if (h->dynindx == -1
	  && ((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) != 0
	      || (h->elf_link_hash_flags & ELF_LINK_HASH_REF_DYNAMIC) != 0))
	{
	  if (! _bfd_elf_link_record_dynamic_symbol (eif->info, h))
	    {
	      eif->failed = TRUE;
	      return FALSE;
	    }
	}
    }
  else
    {
      /* Unfortunately, ELF_LINK_NON_ELF is only correct if the symbol
	 was first seen in a non-ELF file.  Fortunately, if the symbol
	 was first seen in an ELF file, we're probably OK unless the
	 symbol was defined in a non-ELF file.  Catch that case here.
	 FIXME: We're still in trouble if the symbol was first seen in
	 a dynamic object, and then later in a non-ELF regular object.  */
      if ((h->root.type == bfd_link_hash_defined
	   || h->root.type == bfd_link_hash_defweak)
	  && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0
	  && (h->root.u.def.section->owner != NULL
	      ? (bfd_get_flavour (h->root.u.def.section->owner)
		 != bfd_target_elf_flavour)
	      : (bfd_is_abs_section (h->root.u.def.section)
		 && (h->elf_link_hash_flags
		     & ELF_LINK_HASH_DEF_DYNAMIC) == 0)))
	h->elf_link_hash_flags |= ELF_LINK_HASH_DEF_REGULAR;
    }

  /* If this is a final link, and the symbol was defined as a common
     symbol in a regular object file, and there was no definition in
     any dynamic object, then the linker will have allocated space for
     the symbol in a common section but the ELF_LINK_HASH_DEF_REGULAR
     flag will not have been set.  */
  if (h->root.type == bfd_link_hash_defined
      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0
      && (h->elf_link_hash_flags & ELF_LINK_HASH_REF_REGULAR) != 0
      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) == 0
      && (h->root.u.def.section->owner->flags & DYNAMIC) == 0)
    h->elf_link_hash_flags |= ELF_LINK_HASH_DEF_REGULAR;

  /* If -Bsymbolic was used (which means to bind references to global
     symbols to the definition within the shared object), and this
     symbol was defined in a regular object, then it actually doesn't
     need a PLT entry.  Likewise, if the symbol has non-default
     visibility.  If the symbol has hidden or internal visibility, we
     will force it local.  */
  if ((h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_PLT) != 0
      && eif->info->shared
      && is_elf_hash_table (eif->info->hash)
      && (eif->info->symbolic
	  || ELF_ST_VISIBILITY (h->other) != STV_DEFAULT)
      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) != 0)
    {
      const struct elf_backend_data *bed;
      bfd_boolean force_local;

      bed = get_elf_backend_data (elf_hash_table (eif->info)->dynobj);

      force_local = (ELF_ST_VISIBILITY (h->other) == STV_INTERNAL
		     || ELF_ST_VISIBILITY (h->other) == STV_HIDDEN);
      (*bed->elf_backend_hide_symbol) (eif->info, h, force_local);
    }

  /* If a weak undefined symbol has non-default visibility, we also
     hide it from the dynamic linker.  */
  if (ELF_ST_VISIBILITY (h->other) != STV_DEFAULT
      && h->root.type == bfd_link_hash_undefweak)
    {
      const struct elf_backend_data *bed;
      bed = get_elf_backend_data (elf_hash_table (eif->info)->dynobj);
      (*bed->elf_backend_hide_symbol) (eif->info, h, TRUE);
    }

  /* If this is a weak defined symbol in a dynamic object, and we know
     the real definition in the dynamic object, copy interesting flags
     over to the real definition.  */
  if (h->weakdef != NULL)
    {
      struct elf_link_hash_entry *weakdef;

      weakdef = h->weakdef;
      if (h->root.type == bfd_link_hash_indirect)
	h = (struct elf_link_hash_entry *) h->root.u.i.link;

      BFD_ASSERT (h->root.type == bfd_link_hash_defined
		  || h->root.type == bfd_link_hash_defweak);
      BFD_ASSERT (weakdef->root.type == bfd_link_hash_defined
		  || weakdef->root.type == bfd_link_hash_defweak);
      BFD_ASSERT (weakdef->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC);

      /* If the real definition is defined by a regular object file,
	 don't do anything special.  See the longer description in
	 _bfd_elf_adjust_dynamic_symbol, below.  */
      if ((weakdef->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) != 0)
	h->weakdef = NULL;
      else
	{
	  const struct elf_backend_data *bed;

	  bed = get_elf_backend_data (elf_hash_table (eif->info)->dynobj);
	  (*bed->elf_backend_copy_indirect_symbol) (bed, weakdef, h);
	}
    }

  return TRUE;
}

/* Make the backend pick a good value for a dynamic symbol.  This is
   called via elf_link_hash_traverse, and also calls itself
   recursively.  */

bfd_boolean
_bfd_elf_adjust_dynamic_symbol (struct elf_link_hash_entry *h, void *data)
{
  struct elf_info_failed *eif = data;
  bfd *dynobj;
  const struct elf_backend_data *bed;

  if (! is_elf_hash_table (eif->info->hash))
    return FALSE;

  if (h->root.type == bfd_link_hash_warning)
    {
      h->plt = elf_hash_table (eif->info)->init_offset;
      h->got = elf_hash_table (eif->info)->init_offset;

      /* When warning symbols are created, they **replace** the "real"
	 entry in the hash table, thus we never get to see the real
	 symbol in a hash traversal.  So look at it now.  */
      h = (struct elf_link_hash_entry *) h->root.u.i.link;
    }

  /* Ignore indirect symbols.  These are added by the versioning code.  */
  if (h->root.type == bfd_link_hash_indirect)
    return TRUE;

  /* Fix the symbol flags.  */
  if (! _bfd_elf_fix_symbol_flags (h, eif))
    return FALSE;

  /* If this symbol does not require a PLT entry, and it is not
     defined by a dynamic object, or is not referenced by a regular
     object, ignore it.  We do have to handle a weak defined symbol,
     even if no regular object refers to it, if we decided to add it
     to the dynamic symbol table.  FIXME: Do we normally need to worry
     about symbols which are defined by one dynamic object and
     referenced by another one?  */
  if ((h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_PLT) == 0
      && ((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) != 0
	  || (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) == 0
	  || ((h->elf_link_hash_flags & ELF_LINK_HASH_REF_REGULAR) == 0
	      && (h->weakdef == NULL || h->weakdef->dynindx == -1))))
    {
      h->plt = elf_hash_table (eif->info)->init_offset;
      return TRUE;
    }

  /* If we've already adjusted this symbol, don't do it again.  This
     can happen via a recursive call.  */
  if ((h->elf_link_hash_flags & ELF_LINK_HASH_DYNAMIC_ADJUSTED) != 0)
    return TRUE;

  /* Don't look at this symbol again.  Note that we must set this
     after checking the above conditions, because we may look at a
     symbol once, decide not to do anything, and then get called
     recursively later after REF_REGULAR is set below.  */
  h->elf_link_hash_flags |= ELF_LINK_HASH_DYNAMIC_ADJUSTED;

  /* If this is a weak definition, and we know a real definition, and
     the real symbol is not itself defined by a regular object file,
     then get a good value for the real definition.  We handle the
     real symbol first, for the convenience of the backend routine.

     Note that there is a confusing case here.  If the real definition
     is defined by a regular object file, we don't get the real symbol
     from the dynamic object, but we do get the weak symbol.  If the
     processor backend uses a COPY reloc, then if some routine in the
     dynamic object changes the real symbol, we will not see that
     change in the corresponding weak symbol.  This is the way other
     ELF linkers work as well, and seems to be a result of the shared
     library model.

     I will clarify this issue.  Most SVR4 shared libraries define the
     variable _timezone and define timezone as a weak synonym.  The
     tzset call changes _timezone.  If you write
       extern int timezone;
       int _timezone = 5;
       int main () { tzset (); printf ("%d %d\n", timezone, _timezone); }
     you might expect that, since timezone is a synonym for _timezone,
     the same number will print both times.  However, if the processor
     backend uses a COPY reloc, then actually timezone will be copied
     into your process image, and, since you define _timezone
     yourself, _timezone will not.  Thus timezone and _timezone will
     wind up at different memory locations.  The tzset call will set
     _timezone, leaving timezone unchanged.  */

  if (h->weakdef != NULL)
    {
      /* If we get to this point, we know there is an implicit
	 reference by a regular object file via the weak symbol H.
	 FIXME: Is this really true?  What if the traversal finds
	 H->WEAKDEF before it finds H?  */
      h->weakdef->elf_link_hash_flags |= ELF_LINK_HASH_REF_REGULAR;

      if (! _bfd_elf_adjust_dynamic_symbol (h->weakdef, eif))
	return FALSE;
    }

  /* If a symbol has no type and no size and does not require a PLT
     entry, then we are probably about to do the wrong thing here: we
     are probably going to create a COPY reloc for an empty object.
     This case can arise when a shared object is built with assembly
     code, and the assembly code fails to set the symbol type.  */
  if (h->size == 0
      && h->type == STT_NOTYPE
      && (h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_PLT) == 0)
    (*_bfd_error_handler)
      (_("warning: type and size of dynamic symbol `%s' are not defined"),
       h->root.root.string);

  dynobj = elf_hash_table (eif->info)->dynobj;
  bed = get_elf_backend_data (dynobj);
  if (! (*bed->elf_backend_adjust_dynamic_symbol) (eif->info, h))
    {
      eif->failed = TRUE;
      return FALSE;
    }

  return TRUE;
}

/* Adjust all external symbols pointing into SEC_MERGE sections
   to reflect the object merging within the sections.  */

bfd_boolean
_bfd_elf_link_sec_merge_syms (struct elf_link_hash_entry *h, void *data)
{
  asection *sec;

  if (h->root.type == bfd_link_hash_warning)
    h = (struct elf_link_hash_entry *) h->root.u.i.link;

  if ((h->root.type == bfd_link_hash_defined
       || h->root.type == bfd_link_hash_defweak)
      && ((sec = h->root.u.def.section)->flags & SEC_MERGE)
      && sec->sec_info_type == ELF_INFO_TYPE_MERGE)
    {
      bfd *output_bfd = data;

      h->root.u.def.value =
	_bfd_merged_section_offset (output_bfd,
				    &h->root.u.def.section,
				    elf_section_data (sec)->sec_info,
				    h->root.u.def.value, 0);
    }

  return TRUE;
}

/* Returns false if the symbol referred to by H should be considered
   to resolve local to the current module, and true if it should be
   considered to bind dynamically.  */

bfd_boolean
_bfd_elf_dynamic_symbol_p (struct elf_link_hash_entry *h,
			   struct bfd_link_info *info,
			   bfd_boolean ignore_protected)
{
  bfd_boolean binding_stays_local_p;

  if (h == NULL)
    return FALSE;

  while (h->root.type == bfd_link_hash_indirect
	 || h->root.type == bfd_link_hash_warning)
    h = (struct elf_link_hash_entry *) h->root.u.i.link;

  /* If it was forced local, then clearly it's not dynamic.  */
  if (h->dynindx == -1)
    return FALSE;
  if (h->elf_link_hash_flags & ELF_LINK_FORCED_LOCAL)
    return FALSE;

  /* Identify the cases where name binding rules say that a
     visible symbol resolves locally.  */
  binding_stays_local_p = info->executable || info->symbolic;

  switch (ELF_ST_VISIBILITY (h->other))
    {
    case STV_INTERNAL:
    case STV_HIDDEN:
      return FALSE;

    case STV_PROTECTED:
      /* Proper resolution for function pointer equality may require
	 that these symbols perhaps be resolved dynamically, even though
	 we should be resolving them to the current module.  */
      if (!ignore_protected)
	binding_stays_local_p = TRUE;
      break;

    default:
      break;
    }

  /* If it isn't defined locally, then clearly it's dynamic.  */
  if ((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
    return TRUE;

  /* Otherwise, the symbol is dynamic if binding rules don't tell
     us that it remains local.  */
  return !binding_stays_local_p;
}

/* Return true if the symbol referred to by H should be considered
   to resolve local to the current module, and false otherwise.  Differs
   from (the inverse of) _bfd_elf_dynamic_symbol_p in the treatment of
   undefined symbols and weak symbols.  */

bfd_boolean
_bfd_elf_symbol_refs_local_p (struct elf_link_hash_entry *h,
			      struct bfd_link_info *info,
			      bfd_boolean local_protected)
{
  /* If it's a local sym, of course we resolve locally.  */
  if (h == NULL)
    return TRUE;

  /* If we don't have a definition in a regular file, then we can't
     resolve locally.  The sym is either undefined or dynamic.  */
  if ((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
    return FALSE;

  /* Forced local symbols resolve locally.  */
  if ((h->elf_link_hash_flags & ELF_LINK_FORCED_LOCAL) != 0)
    return TRUE;

  /* As do non-dynamic symbols.  */
  if (h->dynindx == -1)
    return TRUE;

  /* At this point, we know the symbol is defined and dynamic.  In an
     executable it must resolve locally, likewise when building symbolic
     shared libraries.  */
  if (info->executable || info->symbolic)
    return TRUE;

  /* Now deal with defined dynamic symbols in shared libraries.  Ones
     with default visibility might not resolve locally.  */
  if (ELF_ST_VISIBILITY (h->other) == STV_DEFAULT)
    return FALSE;

  /* However, STV_HIDDEN or STV_INTERNAL ones must be local.  */
  if (ELF_ST_VISIBILITY (h->other) != STV_PROTECTED)
    return TRUE;

  /* Function pointer equality tests may require that STV_PROTECTED
     symbols be treated as dynamic symbols, even when we know that the
     dynamic linker will resolve them locally.  */
  return local_protected;
}

/* Caches some TLS segment info, and ensures that the TLS segment vma is
   aligned.  Returns the first TLS output section.  */

struct bfd_section *
_bfd_elf_tls_setup (bfd *obfd, struct bfd_link_info *info)
{
  struct bfd_section *sec, *tls;
  unsigned int align = 0;

  for (sec = obfd->sections; sec != NULL; sec = sec->next)
    if ((sec->flags & SEC_THREAD_LOCAL) != 0)
      break;
  tls = sec;

  for (; sec != NULL && (sec->flags & SEC_THREAD_LOCAL) != 0; sec = sec->next)
    if (sec->alignment_power > align)
      align = sec->alignment_power;

  elf_hash_table (info)->tls_sec = tls;

  /* Ensure the alignment of the first section is the largest alignment,
     so that the tls segment starts aligned.  */
  if (tls != NULL)
    tls->alignment_power = align;

  return tls;
}

/* Return TRUE iff this is a non-common, definition of a non-function symbol.  */
static bfd_boolean
is_global_data_symbol_definition (bfd *abfd ATTRIBUTE_UNUSED,
				  Elf_Internal_Sym *sym)
{
  /* Local symbols do not count, but target specific ones might.  */
  if (ELF_ST_BIND (sym->st_info) != STB_GLOBAL
      && ELF_ST_BIND (sym->st_info) < STB_LOOS)
    return FALSE;

  /* Function symbols do not count.  */
  if (ELF_ST_TYPE (sym->st_info) == STT_FUNC)
    return FALSE;

  /* If the section is undefined, then so is the symbol.  */
  if (sym->st_shndx == SHN_UNDEF)
    return FALSE;

  /* If the symbol is defined in the common section, then
     it is a common definition and so does not count.  */
  if (sym->st_shndx == SHN_COMMON)
    return FALSE;

  /* If the symbol is in a target specific section then we
     must rely upon the backend to tell us what it is.  */
  if (sym->st_shndx >= SHN_LORESERVE && sym->st_shndx < SHN_ABS)
    /* FIXME - this function is not coded yet:

       return _bfd_is_global_symbol_definition (abfd, sym);

       Instead for now assume that the definition is not global,
       Even if this is wrong, at least the linker will behave
       in the same way that it used to do.  */
    return FALSE;

  return TRUE;
}

/* Search the symbol table of the archive element of the archive ABFD
   whose archive map contains a mention of SYMDEF, and determine if
   the symbol is defined in this element.  */
static bfd_boolean
elf_link_is_defined_archive_symbol (bfd * abfd, carsym * symdef)
{
  Elf_Internal_Shdr * hdr;
  bfd_size_type symcount;
  bfd_size_type extsymcount;
  bfd_size_type extsymoff;
  Elf_Internal_Sym *isymbuf;
  Elf_Internal_Sym *isym;
  Elf_Internal_Sym *isymend;
  bfd_boolean result;

  abfd = _bfd_get_elt_at_filepos (abfd, symdef->file_offset);
  if (abfd == NULL)
    return FALSE;

  if (! bfd_check_format (abfd, bfd_object))
    return FALSE;

  /* If we have already included the element containing this symbol in the
     link then we do not need to include it again.  Just claim that any symbol
     it contains is not a definition, so that our caller will not decide to
     (re)include this element.  */
  if (abfd->archive_pass)
    return FALSE;

  /* Select the appropriate symbol table.  */
  if ((abfd->flags & DYNAMIC) == 0 || elf_dynsymtab (abfd) == 0)
    hdr = &elf_tdata (abfd)->symtab_hdr;
  else
    hdr = &elf_tdata (abfd)->dynsymtab_hdr;

  symcount = hdr->sh_size / get_elf_backend_data (abfd)->s->sizeof_sym;

  /* The sh_info field of the symtab header tells us where the
     external symbols start.  We don't care about the local symbols.  */
  if (elf_bad_symtab (abfd))
    {
      extsymcount = symcount;
      extsymoff = 0;
    }
  else
    {
      extsymcount = symcount - hdr->sh_info;
      extsymoff = hdr->sh_info;
    }

  if (extsymcount == 0)
    return FALSE;

  /* Read in the symbol table.  */
  isymbuf = bfd_elf_get_elf_syms (abfd, hdr, extsymcount, extsymoff,
				  NULL, NULL, NULL);
  if (isymbuf == NULL)
    return FALSE;

  /* Scan the symbol table looking for SYMDEF.  */
  result = FALSE;
  for (isym = isymbuf, isymend = isymbuf + extsymcount; isym < isymend; isym++)
    {
      const char *name;

      name = bfd_elf_string_from_elf_section (abfd, hdr->sh_link,
					      isym->st_name);
      if (name == NULL)
	break;

      if (strcmp (name, symdef->name) == 0)
	{
	  result = is_global_data_symbol_definition (abfd, isym);
	  break;
	}
    }

  free (isymbuf);

  return result;
}

/* Add an entry to the .dynamic table.  */

bfd_boolean
_bfd_elf_add_dynamic_entry (struct bfd_link_info *info,
			    bfd_vma tag,
			    bfd_vma val)
{
  struct elf_link_hash_table *hash_table;
  const struct elf_backend_data *bed;
  asection *s;
  bfd_size_type newsize;
  bfd_byte *newcontents;
  Elf_Internal_Dyn dyn;

  hash_table = elf_hash_table (info);
  if (! is_elf_hash_table (hash_table))
    return FALSE;

  bed = get_elf_backend_data (hash_table->dynobj);
  s = bfd_get_section_by_name (hash_table->dynobj, ".dynamic");
  BFD_ASSERT (s != NULL);

  newsize = s->_raw_size + bed->s->sizeof_dyn;
  newcontents = bfd_realloc (s->contents, newsize);
  if (newcontents == NULL)
    return FALSE;

  dyn.d_tag = tag;
  dyn.d_un.d_val = val;
  bed->s->swap_dyn_out (hash_table->dynobj, &dyn, newcontents + s->_raw_size);

  s->_raw_size = newsize;
  s->contents = newcontents;

  return TRUE;
}

/* Add a DT_NEEDED entry for this dynamic object if DO_IT is true,
   otherwise just check whether one already exists.  Returns -1 on error,
   1 if a DT_NEEDED tag already exists, and 0 on success.  */

int
_bfd_elf_add_dt_needed_tag (struct bfd_link_info *info,
			    const char *soname,
			    bfd_boolean do_it)
{
  struct elf_link_hash_table *hash_table;
  bfd_size_type oldsize;
  bfd_size_type strindex;

  hash_table = elf_hash_table (info);
  oldsize = _bfd_elf_strtab_size (hash_table->dynstr);
  strindex = _bfd_elf_strtab_add (hash_table->dynstr, soname, FALSE);
  if (strindex == (bfd_size_type) -1)
    return -1;

  if (oldsize == _bfd_elf_strtab_size (hash_table->dynstr))
    {
      asection *sdyn;
      const struct elf_backend_data *bed;
      bfd_byte *extdyn;

      bed = get_elf_backend_data (hash_table->dynobj);
      sdyn = bfd_get_section_by_name (hash_table->dynobj, ".dynamic");
      BFD_ASSERT (sdyn != NULL);

      for (extdyn = sdyn->contents;
	   extdyn < sdyn->contents + sdyn->_raw_size;
	   extdyn += bed->s->sizeof_dyn)
	{
	  Elf_Internal_Dyn dyn;

	  bed->s->swap_dyn_in (hash_table->dynobj, extdyn, &dyn);
	  if (dyn.d_tag == DT_NEEDED
	      && dyn.d_un.d_val == strindex)
	    {
	      _bfd_elf_strtab_delref (hash_table->dynstr, strindex);
	      return 1;
	    }
	}
    }

  if (do_it)
    {
      if (!_bfd_elf_add_dynamic_entry (info, DT_NEEDED, strindex))
	return -1;
    }
  else
    /* We were just checking for existence of the tag.  */
    _bfd_elf_strtab_delref (hash_table->dynstr, strindex);

  return 0;
}

/* Sort symbol by value and section.  */
int
_bfd_elf_sort_symbol (const void *arg1, const void *arg2)
{
  const struct elf_link_hash_entry *h1;
  const struct elf_link_hash_entry *h2;
  bfd_signed_vma vdiff;

  h1 = *(const struct elf_link_hash_entry **) arg1;
  h2 = *(const struct elf_link_hash_entry **) arg2;
  vdiff = h1->root.u.def.value - h2->root.u.def.value;
  if (vdiff != 0)
    return vdiff > 0 ? 1 : -1;
  else
    {
      long sdiff = h1->root.u.def.section - h2->root.u.def.section;
      if (sdiff != 0)
	return sdiff > 0 ? 1 : -1;
    }
  return 0;
}

/* This function is used to adjust offsets into .dynstr for
   dynamic symbols.  This is called via elf_link_hash_traverse.  */

static bfd_boolean
elf_adjust_dynstr_offsets (struct elf_link_hash_entry *h, void *data)
{
  struct elf_strtab_hash *dynstr = data;

  if (h->root.type == bfd_link_hash_warning)
    h = (struct elf_link_hash_entry *) h->root.u.i.link;

  if (h->dynindx != -1)
    h->dynstr_index = _bfd_elf_strtab_offset (dynstr, h->dynstr_index);
  return TRUE;
}

/* Assign string offsets in .dynstr, update all structures referencing
   them.  */

bfd_boolean
_bfd_elf_finalize_dynstr (bfd *output_bfd, struct bfd_link_info *info)
{
  struct elf_link_hash_table *hash_table = elf_hash_table (info);
  struct elf_link_local_dynamic_entry *entry;
  struct elf_strtab_hash *dynstr = hash_table->dynstr;
  bfd *dynobj = hash_table->dynobj;
  asection *sdyn;
  bfd_size_type size;
  const struct elf_backend_data *bed;
  bfd_byte *extdyn;

  _bfd_elf_strtab_finalize (dynstr);
  size = _bfd_elf_strtab_size (dynstr);

  bed = get_elf_backend_data (dynobj);
  sdyn = bfd_get_section_by_name (dynobj, ".dynamic");
  BFD_ASSERT (sdyn != NULL);

  /* Update all .dynamic entries referencing .dynstr strings.  */
  for (extdyn = sdyn->contents;
       extdyn < sdyn->contents + sdyn->_raw_size;
       extdyn += bed->s->sizeof_dyn)
    {
      Elf_Internal_Dyn dyn;

      bed->s->swap_dyn_in (dynobj, extdyn, &dyn);
      switch (dyn.d_tag)
	{
	case DT_STRSZ:
	  dyn.d_un.d_val = size;
	  break;
	case DT_NEEDED:
	case DT_SONAME:
	case DT_RPATH:
	case DT_RUNPATH:
	case DT_FILTER:
	case DT_AUXILIARY:
	  dyn.d_un.d_val = _bfd_elf_strtab_offset (dynstr, dyn.d_un.d_val);
	  break;
	default:
	  continue;
	}
      bed->s->swap_dyn_out (dynobj, &dyn, extdyn);
    }

  /* Now update local dynamic symbols.  */
  for (entry = hash_table->dynlocal; entry ; entry = entry->next)
    entry->isym.st_name = _bfd_elf_strtab_offset (dynstr,
						  entry->isym.st_name);

  /* And the rest of dynamic symbols.  */
  elf_link_hash_traverse (hash_table, elf_adjust_dynstr_offsets, dynstr);

  /* Adjust version definitions.  */
  if (elf_tdata (output_bfd)->cverdefs)
    {
      asection *s;
      bfd_byte *p;
      bfd_size_type i;
      Elf_Internal_Verdef def;
      Elf_Internal_Verdaux defaux;

      s = bfd_get_section_by_name (dynobj, ".gnu.version_d");
      p = s->contents;
      do
	{
	  _bfd_elf_swap_verdef_in (output_bfd, (Elf_External_Verdef *) p,
				   &def);
	  p += sizeof (Elf_External_Verdef);
	  for (i = 0; i < def.vd_cnt; ++i)
	    {
	      _bfd_elf_swap_verdaux_in (output_bfd,
					(Elf_External_Verdaux *) p, &defaux);
	      defaux.vda_name = _bfd_elf_strtab_offset (dynstr,
							defaux.vda_name);
	      _bfd_elf_swap_verdaux_out (output_bfd,
					 &defaux, (Elf_External_Verdaux *) p);
	      p += sizeof (Elf_External_Verdaux);
	    }
	}
      while (def.vd_next);
    }

  /* Adjust version references.  */
  if (elf_tdata (output_bfd)->verref)
    {
      asection *s;
      bfd_byte *p;
      bfd_size_type i;
      Elf_Internal_Verneed need;
      Elf_Internal_Vernaux needaux;

      s = bfd_get_section_by_name (dynobj, ".gnu.version_r");
      p = s->contents;
      do
	{
	  _bfd_elf_swap_verneed_in (output_bfd, (Elf_External_Verneed *) p,
				    &need);
	  need.vn_file = _bfd_elf_strtab_offset (dynstr, need.vn_file);
	  _bfd_elf_swap_verneed_out (output_bfd, &need,
				     (Elf_External_Verneed *) p);
	  p += sizeof (Elf_External_Verneed);
	  for (i = 0; i < need.vn_cnt; ++i)
	    {
	      _bfd_elf_swap_vernaux_in (output_bfd,
					(Elf_External_Vernaux *) p, &needaux);
	      needaux.vna_name = _bfd_elf_strtab_offset (dynstr,
							 needaux.vna_name);
	      _bfd_elf_swap_vernaux_out (output_bfd,
					 &needaux,
					 (Elf_External_Vernaux *) p);
	      p += sizeof (Elf_External_Vernaux);
	    }
	}
      while (need.vn_next);
    }

  return TRUE;
}

/* Add symbols from an ELF archive file to the linker hash table.  We
   don't use _bfd_generic_link_add_archive_symbols because of a
   problem which arises on UnixWare.  The UnixWare libc.so is an
   archive which includes an entry libc.so.1 which defines a bunch of
   symbols.  The libc.so archive also includes a number of other
   object files, which also define symbols, some of which are the same
   as those defined in libc.so.1.  Correct linking requires that we
   consider each object file in turn, and include it if it defines any
   symbols we need.  _bfd_generic_link_add_archive_symbols does not do
   this; it looks through the list of undefined symbols, and includes
   any object file which defines them.  When this algorithm is used on
   UnixWare, it winds up pulling in libc.so.1 early and defining a
   bunch of symbols.  This means that some of the other objects in the
   archive are not included in the link, which is incorrect since they
   precede libc.so.1 in the archive.

   Fortunately, ELF archive handling is simpler than that done by
   _bfd_generic_link_add_archive_symbols, which has to allow for a.out
   oddities.  In ELF, if we find a symbol in the archive map, and the
   symbol is currently undefined, we know that we must pull in that
   object file.

   Unfortunately, we do have to make multiple passes over the symbol
   table until nothing further is resolved.  */

bfd_boolean
_bfd_elf_link_add_archive_symbols (bfd *abfd,
				   struct bfd_link_info *info)
{
  symindex c;
  bfd_boolean *defined = NULL;
  bfd_boolean *included = NULL;
  carsym *symdefs;
  bfd_boolean loop;
  bfd_size_type amt;

  if (! bfd_has_map (abfd))
    {
      /* An empty archive is a special case.  */
      if (bfd_openr_next_archived_file (abfd, NULL) == NULL)
	return TRUE;
      bfd_set_error (bfd_error_no_armap);
      return FALSE;
    }

  /* Keep track of all symbols we know to be already defined, and all
     files we know to be already included.  This is to speed up the
     second and subsequent passes.  */
  c = bfd_ardata (abfd)->symdef_count;
  if (c == 0)
    return TRUE;
  amt = c;
  amt *= sizeof (bfd_boolean);
  defined = bfd_zmalloc (amt);
  included = bfd_zmalloc (amt);
  if (defined == NULL || included == NULL)
    goto error_return;

  symdefs = bfd_ardata (abfd)->symdefs;

  do
    {
      file_ptr last;
      symindex i;
      carsym *symdef;
      carsym *symdefend;

      loop = FALSE;
      last = -1;

      symdef = symdefs;
      symdefend = symdef + c;
      for (i = 0; symdef < symdefend; symdef++, i++)
	{
	  struct elf_link_hash_entry *h;
	  bfd *element;
	  struct bfd_link_hash_entry *undefs_tail;
	  symindex mark;

	  if (defined[i] || included[i])
	    continue;
	  if (symdef->file_offset == last)
	    {
	      included[i] = TRUE;
	      continue;
	    }

	  h = elf_link_hash_lookup (elf_hash_table (info), symdef->name,
				    FALSE, FALSE, FALSE);

	  if (h == NULL)
	    {
	      char *p, *copy;
	      size_t len, first;

	      /* If this is a default version (the name contains @@),
		 look up the symbol again with only one `@' as well
		 as without the version.  The effect is that references
		 to the symbol with and without the version will be
		 matched by the default symbol in the archive.  */

	      p = strchr (symdef->name, ELF_VER_CHR);
	      if (p == NULL || p[1] != ELF_VER_CHR)
		continue;

	      /* First check with only one `@'.  */
	      len = strlen (symdef->name);
	      copy = bfd_alloc (abfd, len);
	      if (copy == NULL)
		goto error_return;
	      first = p - symdef->name + 1;
	      memcpy (copy, symdef->name, first);
	      memcpy (copy + first, symdef->name + first + 1, len - first);

	      h = elf_link_hash_lookup (elf_hash_table (info), copy,
					FALSE, FALSE, FALSE);

	      if (h == NULL)
		{
		  /* We also need to check references to the symbol
		     without the version.  */

		  copy[first - 1] = '\0';
		  h = elf_link_hash_lookup (elf_hash_table (info),
					    copy, FALSE, FALSE, FALSE);
		}

	      bfd_release (abfd, copy);
	    }

	  if (h == NULL)
	    continue;

	  if (h->root.type == bfd_link_hash_common)
	    {
	      /* We currently have a common symbol.  The archive map contains
		 a reference to this symbol, so we may want to include it.  We
		 only want to include it however, if this archive element
		 contains a definition of the symbol, not just another common
		 declaration of it.

		 Unfortunately some archivers (including GNU ar) will put
		 declarations of common symbols into their archive maps, as
		 well as real definitions, so we cannot just go by the archive
		 map alone.  Instead we must read in the element's symbol
		 table and check that to see what kind of symbol definition
		 this is.  */
	      if (! elf_link_is_defined_archive_symbol (abfd, symdef))
		continue;
	    }
	  else if (h->root.type != bfd_link_hash_undefined)
	    {
	      if (h->root.type != bfd_link_hash_undefweak)
		defined[i] = TRUE;
	      continue;
	    }

	  /* We need to include this archive member.  */
	  element = _bfd_get_elt_at_filepos (abfd, symdef->file_offset);
	  if (element == NULL)
	    goto error_return;

	  if (! bfd_check_format (element, bfd_object))
	    goto error_return;

	  /* Doublecheck that we have not included this object
	     already--it should be impossible, but there may be
	     something wrong with the archive.  */
	  if (element->archive_pass != 0)
	    {
	      bfd_set_error (bfd_error_bad_value);
	      goto error_return;
	    }
	  element->archive_pass = 1;

	  undefs_tail = info->hash->undefs_tail;

	  if (! (*info->callbacks->add_archive_element) (info, element,
							 symdef->name))
	    goto error_return;
	  if (! bfd_link_add_symbols (element, info))
	    goto error_return;

	  /* If there are any new undefined symbols, we need to make
	     another pass through the archive in order to see whether
	     they can be defined.  FIXME: This isn't perfect, because
	     common symbols wind up on undefs_tail and because an
	     undefined symbol which is defined later on in this pass
	     does not require another pass.  This isn't a bug, but it
	     does make the code less efficient than it could be.  */
	  if (undefs_tail != info->hash->undefs_tail)
	    loop = TRUE;

	  /* Look backward to mark all symbols from this object file
	     which we have already seen in this pass.  */
	  mark = i;
	  do
	    {
	      included[mark] = TRUE;
	      if (mark == 0)
		break;
	      --mark;
	    }
	  while (symdefs[mark].file_offset == symdef->file_offset);

	  /* We mark subsequent symbols from this object file as we go
	     on through the loop.  */
	  last = symdef->file_offset;
	}
    }
  while (loop);

  free (defined);
  free (included);

  return TRUE;

 error_return:
  if (defined != NULL)
    free (defined);
  if (included != NULL)
    free (included);
  return FALSE;
}

/* This function will be called though elf_link_hash_traverse to store
   all hash value of the exported symbols in an array.  */

static bfd_boolean
elf_collect_hash_codes (struct elf_link_hash_entry *h, void *data)
{
  unsigned long **valuep = data;
  const char *name;
  char *p;
  unsigned long ha;
  char *alc = NULL;

  if (h->root.type == bfd_link_hash_warning)
    h = (struct elf_link_hash_entry *) h->root.u.i.link;

  /* Ignore indirect symbols.  These are added by the versioning code.  */
  if (h->dynindx == -1)
    return TRUE;

  name = h->root.root.string;
  p = strchr (name, ELF_VER_CHR);
  if (p != NULL)
    {
      alc = bfd_malloc (p - name + 1);
      memcpy (alc, name, p - name);
      alc[p - name] = '\0';
      name = alc;
    }

  /* Compute the hash value.  */
  ha = bfd_elf_hash (name);

  /* Store the found hash value in the array given as the argument.  */
  *(*valuep)++ = ha;

  /* And store it in the struct so that we can put it in the hash table
     later.  */
  h->elf_hash_value = ha;

  if (alc != NULL)
    free (alc);

  return TRUE;
}

/* Array used to determine the number of hash table buckets to use
   based on the number of symbols there are.  If there are fewer than
   3 symbols we use 1 bucket, fewer than 17 symbols we use 3 buckets,
   fewer than 37 we use 17 buckets, and so forth.  We never use more
   than 32771 buckets.  */

static const size_t elf_buckets[] =
{
  1, 3, 17, 37, 67, 97, 131, 197, 263, 521, 1031, 2053, 4099, 8209,
  16411, 32771, 0
};

/* Compute bucket count for hashing table.  We do not use a static set
   of possible tables sizes anymore.  Instead we determine for all
   possible reasonable sizes of the table the outcome (i.e., the
   number of collisions etc) and choose the best solution.  The
   weighting functions are not too simple to allow the table to grow
   without bounds.  Instead one of the weighting factors is the size.
   Therefore the result is always a good payoff between few collisions
   (= short chain lengths) and table size.  */
static size_t
compute_bucket_count (struct bfd_link_info *info)
{
  size_t dynsymcount = elf_hash_table (info)->dynsymcount;
  size_t best_size = 0;
  unsigned long int *hashcodes;
  unsigned long int *hashcodesp;
  unsigned long int i;
  bfd_size_type amt;

  /* Compute the hash values for all exported symbols.  At the same
     time store the values in an array so that we could use them for
     optimizations.  */
  amt = dynsymcount;
  amt *= sizeof (unsigned long int);
  hashcodes = bfd_malloc (amt);
  if (hashcodes == NULL)
    return 0;
  hashcodesp = hashcodes;

  /* Put all hash values in HASHCODES.  */
  elf_link_hash_traverse (elf_hash_table (info),
			  elf_collect_hash_codes, &hashcodesp);

  /* We have a problem here.  The following code to optimize the table
     size requires an integer type with more the 32 bits.  If
     BFD_HOST_U_64_BIT is set we know about such a type.  */
#ifdef BFD_HOST_U_64_BIT
  if (info->optimize)
    {
      unsigned long int nsyms = hashcodesp - hashcodes;
      size_t minsize;
      size_t maxsize;
      BFD_HOST_U_64_BIT best_chlen = ~((BFD_HOST_U_64_BIT) 0);
      unsigned long int *counts ;
      bfd *dynobj = elf_hash_table (info)->dynobj;
      const struct elf_backend_data *bed = get_elf_backend_data (dynobj);

      /* Possible optimization parameters: if we have NSYMS symbols we say
	 that the hashing table must at least have NSYMS/4 and at most
	 2*NSYMS buckets.  */
      minsize = nsyms / 4;
      if (minsize == 0)
	minsize = 1;
      best_size = maxsize = nsyms * 2;

      /* Create array where we count the collisions in.  We must use bfd_malloc
	 since the size could be large.  */
      amt = maxsize;
      amt *= sizeof (unsigned long int);
      counts = bfd_malloc (amt);
      if (counts == NULL)
	{
	  free (hashcodes);
	  return 0;
	}

      /* Compute the "optimal" size for the hash table.  The criteria is a
	 minimal chain length.  The minor criteria is (of course) the size
	 of the table.  */
      for (i = minsize; i < maxsize; ++i)
	{
	  /* Walk through the array of hashcodes and count the collisions.  */
	  BFD_HOST_U_64_BIT max;
	  unsigned long int j;
	  unsigned long int fact;

	  memset (counts, '\0', i * sizeof (unsigned long int));

	  /* Determine how often each hash bucket is used.  */
	  for (j = 0; j < nsyms; ++j)
	    ++counts[hashcodes[j] % i];

	  /* For the weight function we need some information about the
	     pagesize on the target.  This is information need not be 100%
	     accurate.  Since this information is not available (so far) we
	     define it here to a reasonable default value.  If it is crucial
	     to have a better value some day simply define this value.  */
# ifndef BFD_TARGET_PAGESIZE
#  define BFD_TARGET_PAGESIZE	(4096)
# endif

	  /* We in any case need 2 + NSYMS entries for the size values and
	     the chains.  */
	  max = (2 + nsyms) * (bed->s->arch_size / 8);

# if 1
	  /* Variant 1: optimize for short chains.  We add the squares
	     of all the chain lengths (which favors many small chain
	     over a few long chains).  */
	  for (j = 0; j < i; ++j)
	    max += counts[j] * counts[j];

	  /* This adds penalties for the overall size of the table.  */
	  fact = i / (BFD_TARGET_PAGESIZE / (bed->s->arch_size / 8)) + 1;
	  max *= fact * fact;
# else
	  /* Variant 2: Optimize a lot more for small table.  Here we
	     also add squares of the size but we also add penalties for
	     empty slots (the +1 term).  */
	  for (j = 0; j < i; ++j)
	    max += (1 + counts[j]) * (1 + counts[j]);

	  /* The overall size of the table is considered, but not as
	     strong as in variant 1, where it is squared.  */
	  fact = i / (BFD_TARGET_PAGESIZE / (bed->s->arch_size / 8)) + 1;
	  max *= fact;
# endif

	  /* Compare with current best results.  */
	  if (max < best_chlen)
	    {
	      best_chlen = max;
	      best_size = i;
	    }
	}

      free (counts);
    }
  else
#endif /* defined (BFD_HOST_U_64_BIT) */
    {
      /* This is the fallback solution if no 64bit type is available or if we
	 are not supposed to spend much time on optimizations.  We select the
	 bucket count using a fixed set of numbers.  */
      for (i = 0; elf_buckets[i] != 0; i++)
	{
	  best_size = elf_buckets[i];
	  if (dynsymcount < elf_buckets[i + 1])
	    break;
	}
    }

  /* Free the arrays we needed.  */
  free (hashcodes);

  return best_size;
}

/* Set up the sizes and contents of the ELF dynamic sections.  This is
   called by the ELF linker emulation before_allocation routine.  We
   must set the sizes of the sections before the linker sets the
   addresses of the various sections.  */

bfd_boolean
bfd_elf_size_dynamic_sections (bfd *output_bfd,
			       const char *soname,
			       const char *rpath,
			       const char *filter_shlib,
			       const char * const *auxiliary_filters,
			       struct bfd_link_info *info,
			       asection **sinterpptr,
			       struct bfd_elf_version_tree *verdefs)
{
  bfd_size_type soname_indx;
  bfd *dynobj;
  const struct elf_backend_data *bed;
  struct elf_assign_sym_version_info asvinfo;

  *sinterpptr = NULL;

  soname_indx = (bfd_size_type) -1;

  if (!is_elf_hash_table (info->hash))
    return TRUE;

  if (info->execstack)
    elf_tdata (output_bfd)->stack_flags = PF_R | PF_W | PF_X;
  else if (info->noexecstack)
    elf_tdata (output_bfd)->stack_flags = PF_R | PF_W;
  else
    {
      bfd *inputobj;
      asection *notesec = NULL;
      int exec = 0;

      for (inputobj = info->input_bfds;
	   inputobj;
	   inputobj = inputobj->link_next)
	{
	  asection *s;

	  if (inputobj->flags & DYNAMIC)
	    continue;
	  s = bfd_get_section_by_name (inputobj, ".note.GNU-stack");
	  if (s)
	    {
	      if (s->flags & SEC_CODE)
		exec = PF_X;
	      notesec = s;
	    }
	  else
	    exec = PF_X;
	}
      if (notesec)
	{
	  elf_tdata (output_bfd)->stack_flags = PF_R | PF_W | exec;
	  if (exec && info->relocatable
	      && notesec->output_section != bfd_abs_section_ptr)
	    notesec->output_section->flags |= SEC_CODE;
	}
    }

  /* Any syms created from now on start with -1 in
     got.refcount/offset and plt.refcount/offset.  */
  elf_hash_table (info)->init_refcount = elf_hash_table (info)->init_offset;

  /* The backend may have to create some sections regardless of whether
     we're dynamic or not.  */
  bed = get_elf_backend_data (output_bfd);
  if (bed->elf_backend_always_size_sections
      && ! (*bed->elf_backend_always_size_sections) (output_bfd, info))
    return FALSE;

  dynobj = elf_hash_table (info)->dynobj;

  /* If there were no dynamic objects in the link, there is nothing to
     do here.  */
  if (dynobj == NULL)
    return TRUE;

  if (! _bfd_elf_maybe_strip_eh_frame_hdr (info))
    return FALSE;

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      struct elf_info_failed eif;
      struct elf_link_hash_entry *h;
      asection *dynstr;
      struct bfd_elf_version_tree *t;
      struct bfd_elf_version_expr *d;
      bfd_boolean all_defined;

      *sinterpptr = bfd_get_section_by_name (dynobj, ".interp");
      BFD_ASSERT (*sinterpptr != NULL || !info->executable);

      if (soname != NULL)
	{
	  soname_indx = _bfd_elf_strtab_add (elf_hash_table (info)->dynstr,
					     soname, TRUE);
	  if (soname_indx == (bfd_size_type) -1
	      || !_bfd_elf_add_dynamic_entry (info, DT_SONAME, soname_indx))
	    return FALSE;
	}

      if (info->symbolic)
	{
	  if (!_bfd_elf_add_dynamic_entry (info, DT_SYMBOLIC, 0))
	    return FALSE;
	  info->flags |= DF_SYMBOLIC;
	}

      if (rpath != NULL)
	{
	  bfd_size_type indx;

	  indx = _bfd_elf_strtab_add (elf_hash_table (info)->dynstr, rpath,
				      TRUE);
	  if (indx == (bfd_size_type) -1
	      || !_bfd_elf_add_dynamic_entry (info, DT_RPATH, indx))
	    return FALSE;

	  if  (info->new_dtags)
	    {
	      _bfd_elf_strtab_addref (elf_hash_table (info)->dynstr, indx);
	      if (!_bfd_elf_add_dynamic_entry (info, DT_RUNPATH, indx))
		return FALSE;
	    }
	}

      if (filter_shlib != NULL)
	{
	  bfd_size_type indx;

	  indx = _bfd_elf_strtab_add (elf_hash_table (info)->dynstr,
				      filter_shlib, TRUE);
	  if (indx == (bfd_size_type) -1
	      || !_bfd_elf_add_dynamic_entry (info, DT_FILTER, indx))
	    return FALSE;
	}

      if (auxiliary_filters != NULL)
	{
	  const char * const *p;

	  for (p = auxiliary_filters; *p != NULL; p++)
	    {
	      bfd_size_type indx;

	      indx = _bfd_elf_strtab_add (elf_hash_table (info)->dynstr,
					  *p, TRUE);
	      if (indx == (bfd_size_type) -1
		  || !_bfd_elf_add_dynamic_entry (info, DT_AUXILIARY, indx))
		return FALSE;
	    }
	}

      eif.info = info;
      eif.verdefs = verdefs;
      eif.failed = FALSE;

      /* If we are supposed to export all symbols into the dynamic symbol
	 table (this is not the normal case), then do so.  */
      if (info->export_dynamic)
	{
	  elf_link_hash_traverse (elf_hash_table (info),
				  _bfd_elf_export_symbol,
				  &eif);
	  if (eif.failed)
	    return FALSE;
	}

      /* Make all global versions with definition.  */
      for (t = verdefs; t != NULL; t = t->next)
	for (d = t->globals.list; d != NULL; d = d->next)
	  if (!d->symver && d->symbol)
	    {
	      const char *verstr, *name;
	      size_t namelen, verlen, newlen;
	      char *newname, *p;
	      struct elf_link_hash_entry *newh;

	      name = d->symbol;
	      namelen = strlen (name);
	      verstr = t->name;
	      verlen = strlen (verstr);
	      newlen = namelen + verlen + 3;

	      newname = bfd_malloc (newlen);
	      if (newname == NULL)
		return FALSE;
	      memcpy (newname, name, namelen);

	      /* Check the hidden versioned definition.  */
	      p = newname + namelen;
	      *p++ = ELF_VER_CHR;
	      memcpy (p, verstr, verlen + 1);
	      newh = elf_link_hash_lookup (elf_hash_table (info),
					   newname, FALSE, FALSE,
					   FALSE);
	      if (newh == NULL
		  || (newh->root.type != bfd_link_hash_defined
		      && newh->root.type != bfd_link_hash_defweak))
		{
		  /* Check the default versioned definition.  */
		  *p++ = ELF_VER_CHR;
		  memcpy (p, verstr, verlen + 1);
		  newh = elf_link_hash_lookup (elf_hash_table (info),
					       newname, FALSE, FALSE,
					       FALSE);
		}
	      free (newname);

	      /* Mark this version if there is a definition and it is
		 not defined in a shared object.  */
	      if (newh != NULL
		  && ((newh->elf_link_hash_flags
		       & ELF_LINK_HASH_DEF_DYNAMIC) == 0)
		  && (newh->root.type == bfd_link_hash_defined
		      || newh->root.type == bfd_link_hash_defweak))
		d->symver = 1;
	    }

      /* Attach all the symbols to their version information.  */
      asvinfo.output_bfd = output_bfd;
      asvinfo.info = info;
      asvinfo.verdefs = verdefs;
      asvinfo.failed = FALSE;

      elf_link_hash_traverse (elf_hash_table (info),
			      _bfd_elf_link_assign_sym_version,
			      &asvinfo);
      if (asvinfo.failed)
	return FALSE;

      if (!info->allow_undefined_version)
	{
	  /* Check if all global versions have a definition.  */
	  all_defined = TRUE;
	  for (t = verdefs; t != NULL; t = t->next)
	    for (d = t->globals.list; d != NULL; d = d->next)
	      if (!d->symver && !d->script)
		{
		  (*_bfd_error_handler)
		    (_("%s: undefined version: %s"),
		     d->pattern, t->name);
		  all_defined = FALSE;
		}

	  if (!all_defined)
	    {
	      bfd_set_error (bfd_error_bad_value);
	      return FALSE;
	    }
	}

      /* Find all symbols which were defined in a dynamic object and make
	 the backend pick a reasonable value for them.  */
      elf_link_hash_traverse (elf_hash_table (info),
			      _bfd_elf_adjust_dynamic_symbol,
			      &eif);
      if (eif.failed)
	return FALSE;

      /* Add some entries to the .dynamic section.  We fill in some of the
	 values later, in elf_bfd_final_link, but we must add the entries
	 now so that we know the final size of the .dynamic section.  */

      /* If there are initialization and/or finalization functions to
	 call then add the corresponding DT_INIT/DT_FINI entries.  */
      h = (info->init_function
	   ? elf_link_hash_lookup (elf_hash_table (info),
				   info->init_function, FALSE,
				   FALSE, FALSE)
	   : NULL);
      if (h != NULL
	  && (h->elf_link_hash_flags & (ELF_LINK_HASH_REF_REGULAR
					| ELF_LINK_HASH_DEF_REGULAR)) != 0)
	{
	  if (!_bfd_elf_add_dynamic_entry (info, DT_INIT, 0))
	    return FALSE;
	}
      h = (info->fini_function
	   ? elf_link_hash_lookup (elf_hash_table (info),
				   info->fini_function, FALSE,
				   FALSE, FALSE)
	   : NULL);
      if (h != NULL
	  && (h->elf_link_hash_flags & (ELF_LINK_HASH_REF_REGULAR
					| ELF_LINK_HASH_DEF_REGULAR)) != 0)
	{
	  if (!_bfd_elf_add_dynamic_entry (info, DT_FINI, 0))
	    return FALSE;
	}

      if (bfd_get_section_by_name (output_bfd, ".preinit_array") != NULL)
	{
	  /* DT_PREINIT_ARRAY is not allowed in shared library.  */
	  if (! info->executable)
	    {
	      bfd *sub;
	      asection *o;

	      for (sub = info->input_bfds; sub != NULL;
		   sub = sub->link_next)
		for (o = sub->sections; o != NULL; o = o->next)
		  if (elf_section_data (o)->this_hdr.sh_type
		      == SHT_PREINIT_ARRAY)
		    {
		      (*_bfd_error_handler)
			(_("%s: .preinit_array section is not allowed in DSO"),
			 bfd_archive_filename (sub));
		      break;
		    }

	      bfd_set_error (bfd_error_nonrepresentable_section);
	      return FALSE;
	    }

	  if (!_bfd_elf_add_dynamic_entry (info, DT_PREINIT_ARRAY, 0)
	      || !_bfd_elf_add_dynamic_entry (info, DT_PREINIT_ARRAYSZ, 0))
	    return FALSE;
	}
      if (bfd_get_section_by_name (output_bfd, ".init_array") != NULL)
	{
	  if (!_bfd_elf_add_dynamic_entry (info, DT_INIT_ARRAY, 0)
	      || !_bfd_elf_add_dynamic_entry (info, DT_INIT_ARRAYSZ, 0))
	    return FALSE;
	}
      if (bfd_get_section_by_name (output_bfd, ".fini_array") != NULL)
	{
	  if (!_bfd_elf_add_dynamic_entry (info, DT_FINI_ARRAY, 0)
	      || !_bfd_elf_add_dynamic_entry (info, DT_FINI_ARRAYSZ, 0))
	    return FALSE;
	}

      dynstr = bfd_get_section_by_name (dynobj, ".dynstr");
      /* If .dynstr is excluded from the link, we don't want any of
	 these tags.  Strictly, we should be checking each section
	 individually;  This quick check covers for the case where
	 someone does a /DISCARD/ : { *(*) }.  */
      if (dynstr != NULL && dynstr->output_section != bfd_abs_section_ptr)
	{
	  bfd_size_type strsize;

	  strsize = _bfd_elf_strtab_size (elf_hash_table (info)->dynstr);
	  if (!_bfd_elf_add_dynamic_entry (info, DT_HASH, 0)
	      || !_bfd_elf_add_dynamic_entry (info, DT_STRTAB, 0)
	      || !_bfd_elf_add_dynamic_entry (info, DT_SYMTAB, 0)
	      || !_bfd_elf_add_dynamic_entry (info, DT_STRSZ, strsize)
	      || !_bfd_elf_add_dynamic_entry (info, DT_SYMENT,
					      bed->s->sizeof_sym))
	    return FALSE;
	}
    }

  /* The backend must work out the sizes of all the other dynamic
     sections.  */
  if (bed->elf_backend_size_dynamic_sections
      && ! (*bed->elf_backend_size_dynamic_sections) (output_bfd, info))
    return FALSE;

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      bfd_size_type dynsymcount;
      asection *s;
      size_t bucketcount = 0;
      size_t hash_entry_size;
      unsigned int dtagcount;

      /* Set up the version definition section.  */
      s = bfd_get_section_by_name (dynobj, ".gnu.version_d");
      BFD_ASSERT (s != NULL);

      /* We may have created additional version definitions if we are
	 just linking a regular application.  */
      verdefs = asvinfo.verdefs;

      /* Skip anonymous version tag.  */
      if (verdefs != NULL && verdefs->vernum == 0)
	verdefs = verdefs->next;

      if (verdefs == NULL)
	_bfd_strip_section_from_output (info, s);
      else
	{
	  unsigned int cdefs;
	  bfd_size_type size;
	  struct bfd_elf_version_tree *t;
	  bfd_byte *p;
	  Elf_Internal_Verdef def;
	  Elf_Internal_Verdaux defaux;

	  cdefs = 0;
	  size = 0;

	  /* Make space for the base version.  */
	  size += sizeof (Elf_External_Verdef);
	  size += sizeof (Elf_External_Verdaux);
	  ++cdefs;

	  for (t = verdefs; t != NULL; t = t->next)
	    {
	      struct bfd_elf_version_deps *n;

	      size += sizeof (Elf_External_Verdef);
	      size += sizeof (Elf_External_Verdaux);
	      ++cdefs;

	      for (n = t->deps; n != NULL; n = n->next)
		size += sizeof (Elf_External_Verdaux);
	    }

	  s->_raw_size = size;
	  s->contents = bfd_alloc (output_bfd, s->_raw_size);
	  if (s->contents == NULL && s->_raw_size != 0)
	    return FALSE;

	  /* Fill in the version definition section.  */

	  p = s->contents;

	  def.vd_version = VER_DEF_CURRENT;
	  def.vd_flags = VER_FLG_BASE;
	  def.vd_ndx = 1;
	  def.vd_cnt = 1;
	  def.vd_aux = sizeof (Elf_External_Verdef);
	  def.vd_next = (sizeof (Elf_External_Verdef)
			 + sizeof (Elf_External_Verdaux));

	  if (soname_indx != (bfd_size_type) -1)
	    {
	      _bfd_elf_strtab_addref (elf_hash_table (info)->dynstr,
				      soname_indx);
	      def.vd_hash = bfd_elf_hash (soname);
	      defaux.vda_name = soname_indx;
	    }
	  else
	    {
	      const char *name;
	      bfd_size_type indx;

	      name = basename (output_bfd->filename);
	      def.vd_hash = bfd_elf_hash (name);
	      indx = _bfd_elf_strtab_add (elf_hash_table (info)->dynstr,
					  name, FALSE);
	      if (indx == (bfd_size_type) -1)
		return FALSE;
	      defaux.vda_name = indx;
	    }
	  defaux.vda_next = 0;

	  _bfd_elf_swap_verdef_out (output_bfd, &def,
				    (Elf_External_Verdef *) p);
	  p += sizeof (Elf_External_Verdef);
	  _bfd_elf_swap_verdaux_out (output_bfd, &defaux,
				     (Elf_External_Verdaux *) p);
	  p += sizeof (Elf_External_Verdaux);

	  for (t = verdefs; t != NULL; t = t->next)
	    {
	      unsigned int cdeps;
	      struct bfd_elf_version_deps *n;
	      struct elf_link_hash_entry *h;
	      struct bfd_link_hash_entry *bh;

	      cdeps = 0;
	      for (n = t->deps; n != NULL; n = n->next)
		++cdeps;

	      /* Add a symbol representing this version.  */
	      bh = NULL;
	      if (! (_bfd_generic_link_add_one_symbol
		     (info, dynobj, t->name, BSF_GLOBAL, bfd_abs_section_ptr,
		      0, NULL, FALSE,
		      get_elf_backend_data (dynobj)->collect, &bh)))
		return FALSE;
	      h = (struct elf_link_hash_entry *) bh;
	      h->elf_link_hash_flags &= ~ ELF_LINK_NON_ELF;
	      h->elf_link_hash_flags |= ELF_LINK_HASH_DEF_REGULAR;
	      h->type = STT_OBJECT;
	      h->verinfo.vertree = t;

	      if (! _bfd_elf_link_record_dynamic_symbol (info, h))
		return FALSE;

	      def.vd_version = VER_DEF_CURRENT;
	      def.vd_flags = 0;
	      if (t->globals.list == NULL
		  && t->locals.list == NULL
		  && ! t->used)
		def.vd_flags |= VER_FLG_WEAK;
	      def.vd_ndx = t->vernum + 1;
	      def.vd_cnt = cdeps + 1;
	      def.vd_hash = bfd_elf_hash (t->name);
	      def.vd_aux = sizeof (Elf_External_Verdef);
	      def.vd_next = 0;
	      if (t->next != NULL)
		def.vd_next = (sizeof (Elf_External_Verdef)
			       + (cdeps + 1) * sizeof (Elf_External_Verdaux));

	      _bfd_elf_swap_verdef_out (output_bfd, &def,
					(Elf_External_Verdef *) p);
	      p += sizeof (Elf_External_Verdef);

	      defaux.vda_name = h->dynstr_index;
	      _bfd_elf_strtab_addref (elf_hash_table (info)->dynstr,
				      h->dynstr_index);
	      defaux.vda_next = 0;
	      if (t->deps != NULL)
		defaux.vda_next = sizeof (Elf_External_Verdaux);
	      t->name_indx = defaux.vda_name;

	      _bfd_elf_swap_verdaux_out (output_bfd, &defaux,
					 (Elf_External_Verdaux *) p);
	      p += sizeof (Elf_External_Verdaux);

	      for (n = t->deps; n != NULL; n = n->next)
		{
		  if (n->version_needed == NULL)
		    {
		      /* This can happen if there was an error in the
			 version script.  */
		      defaux.vda_name = 0;
		    }
		  else
		    {
		      defaux.vda_name = n->version_needed->name_indx;
		      _bfd_elf_strtab_addref (elf_hash_table (info)->dynstr,
					      defaux.vda_name);
		    }
		  if (n->next == NULL)
		    defaux.vda_next = 0;
		  else
		    defaux.vda_next = sizeof (Elf_External_Verdaux);

		  _bfd_elf_swap_verdaux_out (output_bfd, &defaux,
					     (Elf_External_Verdaux *) p);
		  p += sizeof (Elf_External_Verdaux);
		}
	    }

	  if (!_bfd_elf_add_dynamic_entry (info, DT_VERDEF, 0)
	      || !_bfd_elf_add_dynamic_entry (info, DT_VERDEFNUM, cdefs))
	    return FALSE;

	  elf_tdata (output_bfd)->cverdefs = cdefs;
	}

      if ((info->new_dtags && info->flags) || (info->flags & DF_STATIC_TLS))
	{
	  if (!_bfd_elf_add_dynamic_entry (info, DT_FLAGS, info->flags))
	    return FALSE;
	}
      else if (info->flags & DF_BIND_NOW)
	{
	  if (!_bfd_elf_add_dynamic_entry (info, DT_BIND_NOW, 0))
	    return FALSE;
	}

      if (info->flags_1)
	{
	  if (info->executable)
	    info->flags_1 &= ~ (DF_1_INITFIRST
				| DF_1_NODELETE
				| DF_1_NOOPEN);
	  if (!_bfd_elf_add_dynamic_entry (info, DT_FLAGS_1, info->flags_1))
	    return FALSE;
	}

      /* Work out the size of the version reference section.  */

      s = bfd_get_section_by_name (dynobj, ".gnu.version_r");
      BFD_ASSERT (s != NULL);
      {
	struct elf_find_verdep_info sinfo;

	sinfo.output_bfd = output_bfd;
	sinfo.info = info;
	sinfo.vers = elf_tdata (output_bfd)->cverdefs;
	if (sinfo.vers == 0)
	  sinfo.vers = 1;
	sinfo.failed = FALSE;

	elf_link_hash_traverse (elf_hash_table (info),
				_bfd_elf_link_find_version_dependencies,
				&sinfo);

	if (elf_tdata (output_bfd)->verref == NULL)
	  _bfd_strip_section_from_output (info, s);
	else
	  {
	    Elf_Internal_Verneed *t;
	    unsigned int size;
	    unsigned int crefs;
	    bfd_byte *p;

	    /* Build the version definition section.  */
	    size = 0;
	    crefs = 0;
	    for (t = elf_tdata (output_bfd)->verref;
		 t != NULL;
		 t = t->vn_nextref)
	      {
		Elf_Internal_Vernaux *a;

		size += sizeof (Elf_External_Verneed);
		++crefs;
		for (a = t->vn_auxptr; a != NULL; a = a->vna_nextptr)
		  size += sizeof (Elf_External_Vernaux);
	      }

	    s->_raw_size = size;
	    s->contents = bfd_alloc (output_bfd, s->_raw_size);
	    if (s->contents == NULL)
	      return FALSE;

	    p = s->contents;
	    for (t = elf_tdata (output_bfd)->verref;
		 t != NULL;
		 t = t->vn_nextref)
	      {
		unsigned int caux;
		Elf_Internal_Vernaux *a;
		bfd_size_type indx;

		caux = 0;
		for (a = t->vn_auxptr; a != NULL; a = a->vna_nextptr)
		  ++caux;

		t->vn_version = VER_NEED_CURRENT;
		t->vn_cnt = caux;
		indx = _bfd_elf_strtab_add (elf_hash_table (info)->dynstr,
					    elf_dt_name (t->vn_bfd) != NULL
					    ? elf_dt_name (t->vn_bfd)
					    : basename (t->vn_bfd->filename),
					    FALSE);
		if (indx == (bfd_size_type) -1)
		  return FALSE;
		t->vn_file = indx;
		t->vn_aux = sizeof (Elf_External_Verneed);
		if (t->vn_nextref == NULL)
		  t->vn_next = 0;
		else
		  t->vn_next = (sizeof (Elf_External_Verneed)
				+ caux * sizeof (Elf_External_Vernaux));

		_bfd_elf_swap_verneed_out (output_bfd, t,
					   (Elf_External_Verneed *) p);
		p += sizeof (Elf_External_Verneed);

		for (a = t->vn_auxptr; a != NULL; a = a->vna_nextptr)
		  {
		    a->vna_hash = bfd_elf_hash (a->vna_nodename);
		    indx = _bfd_elf_strtab_add (elf_hash_table (info)->dynstr,
						a->vna_nodename, FALSE);
		    if (indx == (bfd_size_type) -1)
		      return FALSE;
		    a->vna_name = indx;
		    if (a->vna_nextptr == NULL)
		      a->vna_next = 0;
		    else
		      a->vna_next = sizeof (Elf_External_Vernaux);

		    _bfd_elf_swap_vernaux_out (output_bfd, a,
					       (Elf_External_Vernaux *) p);
		    p += sizeof (Elf_External_Vernaux);
		  }
	      }

	    if (!_bfd_elf_add_dynamic_entry (info, DT_VERNEED, 0)
		|| !_bfd_elf_add_dynamic_entry (info, DT_VERNEEDNUM, crefs))
	      return FALSE;

	    elf_tdata (output_bfd)->cverrefs = crefs;
	  }
      }

      /* Assign dynsym indicies.  In a shared library we generate a
	 section symbol for each output section, which come first.
	 Next come all of the back-end allocated local dynamic syms,
	 followed by the rest of the global symbols.  */

      dynsymcount = _bfd_elf_link_renumber_dynsyms (output_bfd, info);

      /* Work out the size of the symbol version section.  */
      s = bfd_get_section_by_name (dynobj, ".gnu.version");
      BFD_ASSERT (s != NULL);
      if (dynsymcount == 0
	  || (verdefs == NULL && elf_tdata (output_bfd)->verref == NULL))
	{
	  _bfd_strip_section_from_output (info, s);
	  /* The DYNSYMCOUNT might have changed if we were going to
	     output a dynamic symbol table entry for S.  */
	  dynsymcount = _bfd_elf_link_renumber_dynsyms (output_bfd, info);
	}
      else
	{
	  s->_raw_size = dynsymcount * sizeof (Elf_External_Versym);
	  s->contents = bfd_zalloc (output_bfd, s->_raw_size);
	  if (s->contents == NULL)
	    return FALSE;

	  if (!_bfd_elf_add_dynamic_entry (info, DT_VERSYM, 0))
	    return FALSE;
	}

      /* Set the size of the .dynsym and .hash sections.  We counted
	 the number of dynamic symbols in elf_link_add_object_symbols.
	 We will build the contents of .dynsym and .hash when we build
	 the final symbol table, because until then we do not know the
	 correct value to give the symbols.  We built the .dynstr
	 section as we went along in elf_link_add_object_symbols.  */
      s = bfd_get_section_by_name (dynobj, ".dynsym");
      BFD_ASSERT (s != NULL);
      s->_raw_size = dynsymcount * bed->s->sizeof_sym;
      s->contents = bfd_alloc (output_bfd, s->_raw_size);
      if (s->contents == NULL && s->_raw_size != 0)
	return FALSE;

      if (dynsymcount != 0)
	{
	  Elf_Internal_Sym isym;

	  /* The first entry in .dynsym is a dummy symbol.  */
	  isym.st_value = 0;
	  isym.st_size = 0;
	  isym.st_name = 0;
	  isym.st_info = 0;
	  isym.st_other = 0;
	  isym.st_shndx = 0;
	  bed->s->swap_symbol_out (output_bfd, &isym, s->contents, 0);
	}

      /* Compute the size of the hashing table.  As a side effect this
	 computes the hash values for all the names we export.  */
      bucketcount = compute_bucket_count (info);

      s = bfd_get_section_by_name (dynobj, ".hash");
      BFD_ASSERT (s != NULL);
      hash_entry_size = elf_section_data (s)->this_hdr.sh_entsize;
      s->_raw_size = ((2 + bucketcount + dynsymcount) * hash_entry_size);
      s->contents = bfd_zalloc (output_bfd, s->_raw_size);
      if (s->contents == NULL)
	return FALSE;

      bfd_put (8 * hash_entry_size, output_bfd, bucketcount, s->contents);
      bfd_put (8 * hash_entry_size, output_bfd, dynsymcount,
	       s->contents + hash_entry_size);

      elf_hash_table (info)->bucketcount = bucketcount;

      s = bfd_get_section_by_name (dynobj, ".dynstr");
      BFD_ASSERT (s != NULL);

      _bfd_elf_finalize_dynstr (output_bfd, info);

      s->_raw_size = _bfd_elf_strtab_size (elf_hash_table (info)->dynstr);

      for (dtagcount = 0; dtagcount <= info->spare_dynamic_tags; ++dtagcount)
	if (!_bfd_elf_add_dynamic_entry (info, DT_NULL, 0))
	  return FALSE;
    }

  return TRUE;
}
