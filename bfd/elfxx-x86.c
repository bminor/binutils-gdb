/* x86 specific support for ELF
   Copyright (C) 2017 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

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

#include "elfxx-x86.h"

/* _TLS_MODULE_BASE_ needs to be treated especially when linking
   executables.  Rather than setting it to the beginning of the TLS
   section, we have to set it to the end.    This function may be called
   multiple times, it is idempotent.  */

void
_bfd_x86_elf_set_tls_module_base (struct bfd_link_info *info)
{
  struct elf_x86_link_hash_table *htab;
  struct bfd_link_hash_entry *base;
  const struct elf_backend_data *bed;

  if (!bfd_link_executable (info))
    return;

  bed = get_elf_backend_data (info->output_bfd);
  htab = elf_x86_hash_table (info, bed->target_id);
  if (htab == NULL)
    return;

  base = htab->tls_module_base;
  if (base == NULL)
    return;

  base->u.def.value = htab->elf.tls_size;
}

/* Return the base VMA address which should be subtracted from real addresses
   when resolving @dtpoff relocation.
   This is PT_TLS segment p_vaddr.  */

bfd_vma
_bfd_x86_elf_dtpoff_base (struct bfd_link_info *info)
{
  /* If tls_sec is NULL, we should have signalled an error already.  */
  if (elf_hash_table (info)->tls_sec == NULL)
    return 0;
  return elf_hash_table (info)->tls_sec->vma;
}

/* Find any dynamic relocs that apply to read-only sections.  */

bfd_boolean
_bfd_x86_elf_readonly_dynrelocs (struct elf_link_hash_entry *h,
				 void *inf)
{
  struct elf_x86_link_hash_entry *eh;
  struct elf_dyn_relocs *p;

  /* Skip local IFUNC symbols. */
  if (h->forced_local && h->type == STT_GNU_IFUNC)
    return TRUE;

  eh = (struct elf_x86_link_hash_entry *) h;
  for (p = eh->dyn_relocs; p != NULL; p = p->next)
    {
      asection *s = p->sec->output_section;

      if (s != NULL && (s->flags & SEC_READONLY) != 0)
	{
	  struct bfd_link_info *info = (struct bfd_link_info *) inf;

	  info->flags |= DF_TEXTREL;

	  if ((info->warn_shared_textrel && bfd_link_pic (info))
	      || info->error_textrel)
	    /* xgettext:c-format */
	    info->callbacks->einfo (_("%P: %B: warning: relocation against `%s' in readonly section `%A'\n"),
				    p->sec->owner, h->root.root.string,
				    p->sec);

	  /* Not an error, just cut short the traversal.  */
	  return FALSE;
	}
    }
  return TRUE;
}

/* Find and/or create a hash entry for local symbol.  */

struct elf_link_hash_entry *
_bfd_elf_x86_get_local_sym_hash (struct elf_x86_link_hash_table *htab,
				 bfd *abfd, const Elf_Internal_Rela *rel,
				 bfd_boolean create)
{
  struct elf_x86_link_hash_entry e, *ret;
  asection *sec = abfd->sections;
  hashval_t h = ELF_LOCAL_SYMBOL_HASH (sec->id,
				       htab->r_sym (rel->r_info));
  void **slot;

  e.elf.indx = sec->id;
  e.elf.dynstr_index = htab->r_sym (rel->r_info);
  slot = htab_find_slot_with_hash (htab->loc_hash_table, &e, h,
				   create ? INSERT : NO_INSERT);

  if (!slot)
    return NULL;

  if (*slot)
    {
      ret = (struct elf_x86_link_hash_entry *) *slot;
      return &ret->elf;
    }

  ret = (struct elf_x86_link_hash_entry *)
	objalloc_alloc ((struct objalloc *) htab->loc_hash_memory,
			sizeof (struct elf_x86_link_hash_entry));
  if (ret)
    {
      memset (ret, 0, sizeof (*ret));
      ret->elf.indx = sec->id;
      ret->elf.dynstr_index = htab->r_sym (rel->r_info);
      ret->elf.dynindx = -1;
      ret->plt_got.offset = (bfd_vma) -1;
      *slot = ret;
    }
  return &ret->elf;
}

/* Create an entry in a x86 ELF linker hash table.  NB: THIS MUST BE IN
   SYNC WITH _bfd_elf_link_hash_newfunc.  */

struct bfd_hash_entry *
_bfd_x86_elf_link_hash_newfunc (struct bfd_hash_entry *entry,
				struct bfd_hash_table *table,
				const char *string)
{
  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (entry == NULL)
    {
      entry = (struct bfd_hash_entry *)
	bfd_hash_allocate (table,
			   sizeof (struct elf_x86_link_hash_entry));
      if (entry == NULL)
	return entry;
    }

  /* Call the allocation method of the superclass.  */
  entry = _bfd_link_hash_newfunc (entry, table, string);
  if (entry != NULL)
    {
      struct elf_x86_link_hash_entry *eh
       = (struct elf_x86_link_hash_entry *) entry;
      struct elf_link_hash_table *htab
	= (struct elf_link_hash_table *) table;

      memset (&eh->elf.size, 0,
	      (sizeof (struct elf_x86_link_hash_entry)
	       - offsetof (struct elf_link_hash_entry, size)));
      /* Set local fields.  */
      eh->elf.indx = -1;
      eh->elf.dynindx = -1;
      eh->elf.got = htab->init_got_refcount;
      eh->elf.plt = htab->init_plt_refcount;
      /* Assume that we have been called by a non-ELF symbol reader.
	 This flag is then reset by the code which reads an ELF input
	 file.  This ensures that a symbol created by a non-ELF symbol
	 reader will have the flag set correctly.  */
      eh->elf.non_elf = 1;
      eh->plt_second.offset = (bfd_vma) -1;
      eh->plt_got.offset = (bfd_vma) -1;
      eh->tlsdesc_got = (bfd_vma) -1;
    }

  return entry;
}

/* Compute a hash of a local hash entry.  We use elf_link_hash_entry
  for local symbol so that we can handle local STT_GNU_IFUNC symbols
  as global symbol.  We reuse indx and dynstr_index for local symbol
  hash since they aren't used by global symbols in this backend.  */

hashval_t
_bfd_x86_elf_local_htab_hash (const void *ptr)
{
  struct elf_link_hash_entry *h
    = (struct elf_link_hash_entry *) ptr;
  return ELF_LOCAL_SYMBOL_HASH (h->indx, h->dynstr_index);
}

/* Compare local hash entries.  */

int
_bfd_x86_elf_local_htab_eq (const void *ptr1, const void *ptr2)
{
  struct elf_link_hash_entry *h1
     = (struct elf_link_hash_entry *) ptr1;
  struct elf_link_hash_entry *h2
    = (struct elf_link_hash_entry *) ptr2;

  return h1->indx == h2->indx && h1->dynstr_index == h2->dynstr_index;
}

/* Destroy an x86 ELF linker hash table.  */

void
_bfd_x86_elf_link_hash_table_free (bfd *obfd)
{
  struct elf_x86_link_hash_table *htab
    = (struct elf_x86_link_hash_table *) obfd->link.hash;

  if (htab->loc_hash_table)
    htab_delete (htab->loc_hash_table);
  if (htab->loc_hash_memory)
    objalloc_free ((struct objalloc *) htab->loc_hash_memory);
  _bfd_elf_link_hash_table_free (obfd);
}

/* Sort relocs into address order.  */

int
_bfd_x86_elf_compare_relocs (const void *ap, const void *bp)
{
  const arelent *a = * (const arelent **) ap;
  const arelent *b = * (const arelent **) bp;

  if (a->address > b->address)
    return 1;
  else if (a->address < b->address)
    return -1;
  else
    return 0;
}

bfd_boolean
_bfd_x86_elf_link_check_relocs (bfd *abfd, struct bfd_link_info *info)
{
  if (!bfd_link_relocatable (info))
    {
      /* Check for __tls_get_addr reference.  */
      struct elf_x86_link_hash_table *htab;
      const struct elf_backend_data *bed = get_elf_backend_data (abfd);
      htab = elf_x86_hash_table (info, bed->target_id);
      if (htab)
	{
	  struct elf_link_hash_entry *h
	    = elf_link_hash_lookup (elf_hash_table (info),
				    htab->tls_get_addr,
				    FALSE, FALSE, FALSE);
	  if (h != NULL)
	    ((struct elf_x86_link_hash_entry *) h)->tls_get_addr = 1;
	}
    }

  /* Invoke the regular ELF backend linker to do all the work.  */
  return _bfd_elf_link_check_relocs (abfd, info);
}

bfd_boolean
_bfd_x86_elf_always_size_sections (bfd *output_bfd,
				   struct bfd_link_info *info)
{
  asection *tls_sec = elf_hash_table (info)->tls_sec;

  if (tls_sec)
    {
      struct elf_link_hash_entry *tlsbase;

      tlsbase = elf_link_hash_lookup (elf_hash_table (info),
				      "_TLS_MODULE_BASE_",
				      FALSE, FALSE, FALSE);

      if (tlsbase && tlsbase->type == STT_TLS)
	{
	  struct elf_x86_link_hash_table *htab;
	  struct bfd_link_hash_entry *bh = NULL;
	  const struct elf_backend_data *bed
	    = get_elf_backend_data (output_bfd);

	  htab = elf_x86_hash_table (info, bed->target_id);
	  if (htab == NULL)
	    return FALSE;

	  if (!(_bfd_generic_link_add_one_symbol
		(info, output_bfd, "_TLS_MODULE_BASE_", BSF_LOCAL,
		 tls_sec, 0, NULL, FALSE,
		 bed->collect, &bh)))
	    return FALSE;

	  htab->tls_module_base = bh;

	  tlsbase = (struct elf_link_hash_entry *)bh;
	  tlsbase->def_regular = 1;
	  tlsbase->other = STV_HIDDEN;
	  tlsbase->root.linker_def = 1;
	  (*bed->elf_backend_hide_symbol) (info, tlsbase, TRUE);
	}
    }

  return TRUE;
}

void
_bfd_x86_elf_merge_symbol_attribute (struct elf_link_hash_entry *h,
				     const Elf_Internal_Sym *isym,
				     bfd_boolean definition,
				     bfd_boolean dynamic ATTRIBUTE_UNUSED)
{
  if (definition)
    {
      struct elf_x86_link_hash_entry *eh
	= (struct elf_x86_link_hash_entry *) h;
      eh->def_protected = (ELF_ST_VISIBILITY (isym->st_other)
			   == STV_PROTECTED);
    }
}

/* Copy the extra info we tack onto an elf_link_hash_entry.  */

void
_bfd_x86_elf_copy_indirect_symbol (struct bfd_link_info *info,
				   struct elf_link_hash_entry *dir,
				   struct elf_link_hash_entry *ind)
{
  struct elf_x86_link_hash_entry *edir, *eind;

  edir = (struct elf_x86_link_hash_entry *) dir;
  eind = (struct elf_x86_link_hash_entry *) ind;

  if (eind->dyn_relocs != NULL)
    {
      if (edir->dyn_relocs != NULL)
	{
	  struct elf_dyn_relocs **pp;
	  struct elf_dyn_relocs *p;

	  /* Add reloc counts against the indirect sym to the direct sym
	     list.  Merge any entries against the same section.  */
	  for (pp = &eind->dyn_relocs; (p = *pp) != NULL; )
	    {
	      struct elf_dyn_relocs *q;

	      for (q = edir->dyn_relocs; q != NULL; q = q->next)
		if (q->sec == p->sec)
		  {
		    q->pc_count += p->pc_count;
		    q->count += p->count;
		    *pp = p->next;
		    break;
		  }
	      if (q == NULL)
		pp = &p->next;
	    }
	  *pp = edir->dyn_relocs;
	}

      edir->dyn_relocs = eind->dyn_relocs;
      eind->dyn_relocs = NULL;
    }

  if (ind->root.type == bfd_link_hash_indirect
      && dir->got.refcount <= 0)
    {
      edir->tls_type = eind->tls_type;
      eind->tls_type = GOT_UNKNOWN;
    }

  /* Copy gotoff_ref so that elf_i386_adjust_dynamic_symbol will
     generate a R_386_COPY reloc.  */
  edir->gotoff_ref |= eind->gotoff_ref;

  edir->has_got_reloc |= eind->has_got_reloc;
  edir->has_non_got_reloc |= eind->has_non_got_reloc;

  if (ELIMINATE_COPY_RELOCS
      && ind->root.type != bfd_link_hash_indirect
      && dir->dynamic_adjusted)
    {
      /* If called to transfer flags for a weakdef during processing
	 of elf_adjust_dynamic_symbol, don't copy non_got_ref.
	 We clear it ourselves for ELIMINATE_COPY_RELOCS.  */
      if (dir->versioned != versioned_hidden)
	dir->ref_dynamic |= ind->ref_dynamic;
      dir->ref_regular |= ind->ref_regular;
      dir->ref_regular_nonweak |= ind->ref_regular_nonweak;
      dir->needs_plt |= ind->needs_plt;
      dir->pointer_equality_needed |= ind->pointer_equality_needed;
    }
  else
    {
      if (eind->func_pointer_refcount > 0)
	{
	  edir->func_pointer_refcount += eind->func_pointer_refcount;
	  eind->func_pointer_refcount = 0;
	}

      _bfd_elf_link_hash_copy_indirect (info, dir, ind);
    }
}

/* Remove undefined weak symbol from the dynamic symbol table if it
   is resolved to 0.   */

bfd_boolean
_bfd_x86_elf_fixup_symbol (struct bfd_link_info *info,
			   struct elf_link_hash_entry *h)
{
  if (h->dynindx != -1)
    {
      const struct elf_backend_data *bed
	= get_elf_backend_data (info->output_bfd);
      if (UNDEFINED_WEAK_RESOLVED_TO_ZERO (info,
					   bed->target_id,
					   elf_x86_hash_entry (h)->has_got_reloc,
					   elf_x86_hash_entry (h)))
	{
	  h->dynindx = -1;
	  _bfd_elf_strtab_delref (elf_hash_table (info)->dynstr,
				  h->dynstr_index);
	}
    }
  return TRUE;
}

/* Return TRUE if symbol should be hashed in the `.gnu.hash' section.  */

bfd_boolean
_bfd_x86_elf_hash_symbol (struct elf_link_hash_entry *h)
{
  if (h->plt.offset != (bfd_vma) -1
      && !h->def_regular
      && !h->pointer_equality_needed)
    return FALSE;

  return _bfd_elf_hash_symbol (h);
}

/* Parse x86 GNU properties.  */

enum elf_property_kind
_bfd_x86_elf_parse_gnu_properties (bfd *abfd, unsigned int type,
				   bfd_byte *ptr, unsigned int datasz)
{
  elf_property *prop;

  switch (type)
    {
    case GNU_PROPERTY_X86_ISA_1_USED:
    case GNU_PROPERTY_X86_ISA_1_NEEDED:
    case GNU_PROPERTY_X86_FEATURE_1_AND:
      if (datasz != 4)
	{
	  _bfd_error_handler
	    ((type == GNU_PROPERTY_X86_ISA_1_USED
	      ? _("error: %B: <corrupt x86 ISA used size: 0x%x>")
	      : (type == GNU_PROPERTY_X86_ISA_1_NEEDED
		 ? _("error: %B: <corrupt x86 ISA needed size: 0x%x>")
		 : _("error: %B: <corrupt x86 feature size: 0x%x>"))),
	     abfd, datasz);
	  return property_corrupt;
	}
      prop = _bfd_elf_get_property (abfd, type, datasz);
      /* Combine properties of the same type.  */
      prop->u.number |= bfd_h_get_32 (abfd, ptr);
      prop->pr_kind = property_number;
      break;

    default:
      return property_ignored;
    }

  return property_number;
}

/* Merge x86 GNU property BPROP with APROP.  If APROP isn't NULL,
   return TRUE if APROP is updated.  Otherwise, return TRUE if BPROP
   should be merged with ABFD.  */

bfd_boolean
_bfd_x86_elf_merge_gnu_properties (struct bfd_link_info *info,
				   bfd *abfd ATTRIBUTE_UNUSED,
				   elf_property *aprop,
				   elf_property *bprop)
{
  unsigned int number, features;
  bfd_boolean updated = FALSE;
  unsigned int pr_type = aprop != NULL ? aprop->pr_type : bprop->pr_type;

  switch (pr_type)
    {
    case GNU_PROPERTY_X86_ISA_1_USED:
    case GNU_PROPERTY_X86_ISA_1_NEEDED:
      if (aprop != NULL && bprop != NULL)
	{
	  number = aprop->u.number;
	  aprop->u.number = number | bprop->u.number;
	  updated = number != (unsigned int) aprop->u.number;
	}
      else
	{
	  /* Return TRUE if APROP is NULL to indicate that BPROP should
	     be added to ABFD.  */
	  updated = aprop == NULL;
	}
      break;

    case GNU_PROPERTY_X86_FEATURE_1_AND:
      /* Only one of APROP and BPROP can be NULL:
	 1. APROP & BPROP when both APROP and BPROP aren't NULL.
	 2. If APROP is NULL, remove x86 feature.
	 3. Otherwise, do nothing.
       */
      if (aprop != NULL && bprop != NULL)
	{
	  features = 0;
	  if (info->ibt)
	    features = GNU_PROPERTY_X86_FEATURE_1_IBT;
	  if (info->shstk)
	    features |= GNU_PROPERTY_X86_FEATURE_1_SHSTK;
	  number = aprop->u.number;
	  /* Add GNU_PROPERTY_X86_FEATURE_1_IBT and
	     GNU_PROPERTY_X86_FEATURE_1_SHSTK.  */
	  aprop->u.number = (number & bprop->u.number) | features;
	  updated = number != (unsigned int) aprop->u.number;
	  /* Remove the property if all feature bits are cleared.  */
	  if (aprop->u.number == 0)
	    aprop->pr_kind = property_remove;
	}
      else
	{
	  features = 0;
	  if (info->ibt)
	    features = GNU_PROPERTY_X86_FEATURE_1_IBT;
	  if (info->shstk)
	    features |= GNU_PROPERTY_X86_FEATURE_1_SHSTK;
	  if (features)
	    {
	      /* Add GNU_PROPERTY_X86_FEATURE_1_IBT and
		 GNU_PROPERTY_X86_FEATURE_1_SHSTK.  */
	      if (aprop != NULL)
		{
		  number = aprop->u.number;
		  aprop->u.number = number | features;
		  updated = number != (unsigned int) aprop->u.number;
		}
	      else
		{
		  bprop->u.number |= features;
		  updated = TRUE;
		}
	    }
	  else if (aprop != NULL)
	    {
	      aprop->pr_kind = property_remove;
	      updated = TRUE;
	    }
	}
      break;

    default:
      /* Never should happen.  */
      abort ();
    }

  return updated;
}
