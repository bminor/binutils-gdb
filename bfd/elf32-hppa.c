/* BFD back-end for HP PA-RISC ELF files.
   Copyright (C) 1990, 91, 92, 93, 94, 95, 96, 97, 98, 99, 2000
   Free Software Foundation, Inc.

   Written by

	Center for Software Science
	Department of Computer Science
	University of Utah

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
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/hppa.h"
#include "libhppa.h"
#include "elf32-hppa.h"
#define ARCH_SIZE		32
#include "elf-hppa.h"
#include "elf32-hppa.h"


/* We use two hash tables to hold information for linking PA ELF objects.

   The first is the elf32_hppa_link_hash_table which is derived
   from the standard ELF linker hash table.  We use this as a place to
   attach other hash tables and static information.

   The second is the stub hash table which is derived from the
   base BFD hash table.  The stub hash table holds the information
   necessary to build the linker stubs during a link.  */

/* Hash table for linker stubs.  */

struct elf32_hppa_stub_hash_entry
{
  /* Base hash table entry structure.  */
  struct bfd_hash_entry root;

  /* The stub section.  */
  asection *stub_sec;

  /* Offset within stub_sec of the beginning of this stub.  */
  bfd_vma offset;

  /* Given the symbol's value and its section we can determine its final
     value when building the stubs (so the stub knows where to jump.  */
  symvalue target_value;
  asection *target_section;
};

struct elf32_hppa_link_hash_table
{
  /* The main hash table.  */
  struct elf_link_hash_table root;

  /* The stub hash table.  */
  struct bfd_hash_table stub_hash_table;

  /* Current offsets in the stub sections.  */
  bfd_vma *offset;

  /* Global data pointer.  */
  bfd_vma global_value;
};


/* For linker stub hash tables.  */

#define elf32_hppa_stub_hash_lookup(table, string, create, copy) \
  ((struct elf32_hppa_stub_hash_entry *) \
   bfd_hash_lookup ((table), (string), (create), (copy)))

/* Get the PA ELF linker hash table from a link_info structure.  */

#define elf32_hppa_hash_table(p) \
  ((struct elf32_hppa_link_hash_table *) ((p)->hash))


static struct bfd_hash_entry *elf32_hppa_stub_hash_newfunc
  PARAMS ((struct bfd_hash_entry *, struct bfd_hash_table *, const char *));

static struct bfd_link_hash_table *elf32_hppa_link_hash_table_create
  PARAMS ((bfd *));

static char *elf32_hppa_stub_name
  PARAMS ((const char *, const asection *, const asection *,
	   bfd_vma, const struct elf_link_hash_entry *));

static int elf32_hppa_relocate_insn
  PARAMS ((int, bfd_vma, bfd_signed_vma, unsigned int, int,
	   enum hppa_reloc_field_selector_type_alt));

static bfd_reloc_status_type elf32_hppa_bfd_final_link_relocate
  PARAMS ((reloc_howto_type *, bfd *, asection *,
	   bfd_byte *, bfd_vma, bfd_vma, bfd_signed_vma,
	   struct bfd_link_info *, asection *, const char *,
	   struct elf_link_hash_entry *));

static boolean elf32_hppa_relocate_section
  PARAMS ((bfd *, struct bfd_link_info *, bfd *, asection *,
	   bfd_byte *, Elf_Internal_Rela *, Elf_Internal_Sym *, asection **));

static boolean elf32_hppa_add_symbol_hook
  PARAMS ((bfd *, struct bfd_link_info *, const Elf_Internal_Sym *,
	   const char **, flagword *, asection **, bfd_vma *));

static unsigned int elf32_hppa_size_of_stub
  PARAMS ((asection *, bfd_vma, bfd_vma));

static boolean elf32_hppa_build_one_stub
  PARAMS ((struct bfd_hash_entry *, PTR));


/* Assorted hash table functions.  */

/* Initialize an entry in the stub hash table.  */

static struct bfd_hash_entry *
elf32_hppa_stub_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  struct elf32_hppa_stub_hash_entry *ret;

  ret = (struct elf32_hppa_stub_hash_entry *) entry;

  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (ret == NULL)
    ret = ((struct elf32_hppa_stub_hash_entry *)
	   bfd_hash_allocate (table,
			      sizeof (struct elf32_hppa_stub_hash_entry)));
  if (ret == NULL)
    return NULL;

  /* Call the allocation method of the superclass.  */
  ret = ((struct elf32_hppa_stub_hash_entry *)
	 bfd_hash_newfunc ((struct bfd_hash_entry *) ret, table, string));

  if (ret)
    {
      /* Initialize the local fields.  */
      ret->stub_sec = NULL;
      ret->offset = 0;
      ret->target_value = 0;
      ret->target_section = NULL;
    }

  return (struct bfd_hash_entry *) ret;
}

/* Create the derived linker hash table.  The PA ELF port uses the derived
   hash table to keep information specific to the PA ELF linker (without
   using static variables).  */

static struct bfd_link_hash_table *
elf32_hppa_link_hash_table_create (abfd)
     bfd *abfd;
{
  struct elf32_hppa_link_hash_table *ret;

  ret = ((struct elf32_hppa_link_hash_table *) bfd_alloc (abfd, sizeof (*ret)));
  if (ret == NULL)
    return NULL;

  if (!_bfd_elf_link_hash_table_init (&ret->root, abfd,
				      _bfd_elf_link_hash_newfunc))
    {
      bfd_release (abfd, ret);
      return NULL;
    }

  /* Init the stub hash table too.  */
  if (!bfd_hash_table_init (&ret->stub_hash_table,
			    elf32_hppa_stub_hash_newfunc))
    return NULL;

  ret->offset = NULL;
  ret->global_value = 0;

  return &ret->root.root;
}

/* Build a name for a long branch stub.  */
static char *
elf32_hppa_stub_name (sym_name, sym_sec, input_section, addend, hash)
     const char *sym_name;
     const asection *sym_sec;
     const asection *input_section;
     bfd_vma addend;
     const struct elf_link_hash_entry *hash;
{
  char *stub_name;
  int len;

  len = strlen (sym_name) + 19;
  if (hash == NULL)
    len += 9;

  stub_name = bfd_malloc (len);
  if (stub_name != NULL)
    {
      sprintf (stub_name, "%08x_%08x_%s",
	       input_section->id & 0xffffffff,
	       (int) addend & 0xffffffff,
	       sym_name);

      /* Tack on an ID so we can uniquely identify
	 this local symbol in the stub hash tables.  */
      if (hash == NULL)
	sprintf (stub_name + len - 10, "_%08x",
		 sym_sec->id & 0xffffffff);
    }
  return stub_name;
}

/* Relocate the given INSN given the various input parameters.  */

static int
elf32_hppa_relocate_insn (insn, sym_value, r_addend, r_type, r_format, r_field)
     int insn;
     bfd_vma sym_value;
     bfd_signed_vma r_addend;
     unsigned int r_type;
     int r_format;
     enum hppa_reloc_field_selector_type_alt r_field;
{
  int value;
#ifdef ELF_ARG_RELOC
#ifndef ELF_ARG_RELOC_INSN
  /* Ick.  Who would want to support this?  */
  int imm;

  switch (r_type)
    {
      /* The offset is partly stored in the instruction for cases
	 where the top ten bits of the addend are used for arg_reloc.
         This is a little tricky, because the immediate value in the
	 instruction not only needs to be pieced together from
	 multiple bit fields, but also needs to be shifted left to
	 restore the original quantity.  Which bits of the offset
	 we can retrieve from the instruction depend on exactly which
	 instruction we are dealing with.  */
    case R_PARISC_PCREL17R:
    case R_PARISC_PCREL17F:
    case R_PARISC_PCREL17C:
    case R_PARISC_DIR17R:
    case R_PARISC_DIR17F:
      /* For these relocs, we choose to use the low 10 bits from the
	 instruction and store the high 22 bits in the reloc addend.
	 It doesn't matter that the bottom 2 bits of the value are
	 always zero, as branches must be to a location which is a
	 multiple of 4.  */
#if 0
      /* It isn't necessary to retrieve the whole immediate, but
	 this documents what we have in the instruction.  */
      imm = (((insn & 0x1f0000) >> 5)
	     | ((insn & 0x0004) << 8)
	     | ((insn & 0x1ff8) >> 3)) - ((insn & 1) << 17);
      imm <<= 2;
      imm = imm & 0x3ff;
#else
      imm = (insn & 0x7f8) >> 1;
#endif
      r_addend = (r_addend << (BFD_ARCH_SIZE-22)) >> (BFD_ARCH_SIZE-32);
      r_addend = r_addend | imm;
      break;

    case R_PARISC_PCREL21L:
    case R_PARISC_DIR21L:
      /* In this case, the instruction stores the high 21 bits of the
	 value, so we pick off the top 10 bits, and use the reloc
	 addend to store the low 22 bits.  */
#if 0
      /* It isn't necessary to retrieve the whole immediate, but
	 this documents what we have in the instruction.  */
      imm = ((  (insn & 0x000ffe) << 8)
	     | ((insn & 0x00c000) >> 7)
	     | ((insn & 0x1f0000) >> 14)
	     | ((insn & 0x003000) >> 12)) - ((insn & 1) << 20);
      imm <<= 11;
      imm = imm & ~ 0x3fffff;
#else
      /* Just pick off the 10 needed bits, ensuring we sign extend.  */
      imm = ((insn & 0x000ff8) << 19) - ((insn & 1) << 31);
#endif
      r_addend = imm | (r_addend & 0x3fffff);
      break;

    default:
      break;
    }
#endif
#endif

  switch (r_type)
    {
    case R_PARISC_PCREL21L:
    case R_PARISC_PCREL17C:
    case R_PARISC_PCREL17F:
    case R_PARISC_PCREL17R:
    case R_PARISC_PCREL14R:
      /* Adjust PC relative offset.  */
      r_addend -= 8;
      break;
    default:
      break;
    }
  value = hppa_field_adjust (sym_value, r_addend, r_field);

  switch (r_type)
    {
    case R_PARISC_PCREL17C:
    case R_PARISC_PCREL17F:
    case R_PARISC_PCREL17R:
    case R_PARISC_DIR17F:
    case R_PARISC_DIR17R:
      /* This is a branch.  Divide the offset by four.
	 Note that we need to decide whether it's a branch or
	 otherwise by inspecting the reloc.  Inspecting insn won't
	 work as insn might be from a .word directive.  */
      value >>= 2;
      break;

    default:
      break;
    }

  return hppa_rebuild_insn (insn, value, r_format);
}

/* Actually perform a relocation as part of a final link.  This can get
   rather hairy when linker stubs are needed.  */

static bfd_reloc_status_type
elf32_hppa_bfd_final_link_relocate (howto, input_bfd, input_section,
				    contents, offset, value, addend,
				    info, sym_sec, sym_name, h)
     reloc_howto_type *howto;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     bfd_vma offset;
     bfd_vma value;
     bfd_signed_vma addend;
     struct bfd_link_info *info;
     asection *sym_sec;
     const char *sym_name;
     struct elf_link_hash_entry *h;
{
  int insn;
  unsigned int r_type = howto->type;
  int r_format = howto->bitsize;
  enum hppa_reloc_field_selector_type_alt r_field = e_fsel;
  bfd_byte *hit_data = contents + offset;
  bfd_vma location;

  if (r_type == R_PARISC_NONE)
    return bfd_reloc_ok;

  insn = bfd_get_32 (input_bfd, hit_data);

  /* Find out where we are and where we're going.  */
  location = (offset +
	      input_section->output_offset +
	      input_section->output_section->vma);

  switch (r_type)
    {
    case R_PARISC_PCREL21L:
    case R_PARISC_PCREL17C:
    case R_PARISC_PCREL17F:
    case R_PARISC_PCREL17R:
    case R_PARISC_PCREL14R:
      /* Make it a pc relative offset.  */
      value -= location;
      break;
    default:
      break;
    }

  switch (r_type)
    {
    case R_PARISC_DIR32:
    case R_PARISC_DIR17F:
    case R_PARISC_PCREL17C:
      r_field = e_fsel;
      break;

    case R_PARISC_DIR21L:
    case R_PARISC_PCREL21L:
      r_field = e_lrsel;
      break;

    case R_PARISC_DIR17R:
    case R_PARISC_PCREL17R:
    case R_PARISC_DIR14R:
    case R_PARISC_PCREL14R:
      r_field = e_rrsel;
      break;

    /* For all the DP relative relocations, we need to examine the symbol's
       section.  If it's a code section, then "data pointer relative" makes
       no sense.  In that case we don't adjust the "value", and for 21 bit
       addil instructions, we change the source addend register from %dp to
       %r0.  */
    case R_PARISC_DPREL21L:
      r_field = e_lrsel;
      if (sym_sec != NULL)
	{
	  if ((sym_sec->flags & SEC_CODE) != 0)
	    {
	      if ((insn & ((0x3f << 26) | (0x1f << 21)))
		  == (((int) OP_ADDIL << 26) | (27 << 21)))
		{
		  insn &= ~ (0x1f << 21);
		}
	    }
	  else
	    value -= elf32_hppa_hash_table (info)->global_value;
	}
      break;

    case R_PARISC_DPREL14R:
      r_field = e_rrsel;
      if (sym_sec != NULL && (sym_sec->flags & SEC_CODE) == 0)
	value -= elf32_hppa_hash_table (info)->global_value;
      break;

    case R_PARISC_DPREL14F:
      r_field = e_fsel;
      if (sym_sec != NULL && (sym_sec->flags & SEC_CODE) == 0)
	value -= elf32_hppa_hash_table (info)->global_value;
      break;

    case R_PARISC_PLABEL32:
      r_field = e_fsel;
      break;

    case R_PARISC_PLABEL21L:
      r_field = e_lrsel;
      break;

    case R_PARISC_PLABEL14R:
      r_field = e_rrsel;
      break;

    /* This case is separate as it may involve a lot more work
       to deal with linker stubs.  */
    case R_PARISC_PCREL17F:
      r_field = e_fsel;

      /* bfd_link_hash_undefweak symbols have sym_sec == NULL.  */
      if (sym_sec == NULL)
	break;

      /* Any kind of linker stub needed?  */
      /* bfd_vma value is unsigned, so this is testing for offsets
	 outside the range -0x40000 to +0x3ffff  */
      if (value + addend - 8 + 0x40000 >= 0x80000)
	{
	  struct bfd_hash_table *stub_hash_table;
	  struct elf32_hppa_stub_hash_entry *stub_entry;
	  char *stub_name;

	  stub_name = elf32_hppa_stub_name (sym_name, sym_sec,
					    input_section, addend, h);
	  if (!stub_name)
	    {
	      (*_bfd_error_handler) ("%s: %s",
				     bfd_get_filename (input_bfd),
				     bfd_errmsg (bfd_get_error ()));
	      return bfd_reloc_notsupported;
	    }

	  stub_hash_table = &elf32_hppa_hash_table (info)->stub_hash_table;

	  stub_entry = elf32_hppa_stub_hash_lookup (stub_hash_table,
						    stub_name,
						    false,
						    false);
	  if (stub_entry == NULL)
	    {
	      (*_bfd_error_handler)
		(_("%s: cannot find stub entry %s"),
		 bfd_get_filename (input_bfd),
		 stub_name);
	      free (stub_name);
	      return bfd_reloc_notsupported;
	    }

	  /* Munge up the value and addend for elf32_hppa_relocate_insn.  */
	  value = (stub_entry->offset
		   + stub_entry->stub_sec->output_offset
		   + stub_entry->stub_sec->output_section->vma
		   - location);
	  addend = 0;

	  if (value + addend - 8 + 0x40000 >= 0x80000)
	    {
	      (*_bfd_error_handler)
		(_("%s: cannot reach stub %s, recompile with -ffunction-sections"),
		 bfd_get_filename (input_bfd),
		 stub_name);
	      free (stub_name);
	      return bfd_reloc_notsupported;
	    }

	  free (stub_name);
	}
      break;

    /* Something we don't know how to handle.  */
    default:
      return bfd_reloc_notsupported;
    }

  /* bfd_link_hash_undefweak symbols have sym_sec == NULL.  */
  if (sym_sec == NULL)
    {
      BFD_ASSERT (h != NULL && h->root.type == bfd_link_hash_undefweak);
      value = 0;
    }

  insn = elf32_hppa_relocate_insn (insn, value, addend,
				   r_type, r_format, r_field);

  /* Update the instruction word.  */
  bfd_put_32 (input_bfd, insn, hit_data);
  return bfd_reloc_ok;
}

/* Relocate an HPPA ELF section.  */

static boolean
elf32_hppa_relocate_section (output_bfd, info, input_bfd, input_section,
			     contents, relocs, local_syms, local_sections)
     bfd *output_bfd ATTRIBUTE_UNUSED;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     Elf_Internal_Rela *relocs;
     Elf_Internal_Sym *local_syms;
     asection **local_sections;
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;

  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      unsigned int r_type;
      reloc_howto_type *howto;
      unsigned int r_symndx;
      struct elf_link_hash_entry *h;
      Elf_Internal_Sym *sym;
      asection *sym_sec;
      bfd_vma relocation;
      bfd_reloc_status_type r;
      const char *sym_name;

      r_type = ELF32_R_TYPE (rel->r_info);
      if (r_type >= (unsigned int) R_PARISC_UNIMPLEMENTED)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}
      howto = elf_hppa_howto_table + r_type;

      r_symndx = ELF32_R_SYM (rel->r_info);

      if (info->relocateable)
	{
	  /* This is a relocateable link.  We don't have to change
	     anything, unless the reloc is against a section symbol,
	     in which case we have to adjust according to where the
	     section symbol winds up in the output section.  */
	  if (r_symndx < symtab_hdr->sh_info)
	    {
	      sym = local_syms + r_symndx;
	      if (ELF_ST_TYPE (sym->st_info) == STT_SECTION)
		{
		  sym_sec = local_sections[r_symndx];
		  rel->r_addend += sym_sec->output_offset;
		}
	    }

	  continue;
	}

      /* This is a final link.  */
      h = NULL;
      sym = NULL;
      sym_sec = NULL;
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sym_sec = local_sections[r_symndx];
	  relocation = ((ELF_ST_TYPE (sym->st_info) == STT_SECTION
			   ? 0 : sym->st_value)
			 + sym_sec->output_offset
			 + sym_sec->output_section->vma);
	}
      else
	{
	  int indx;

	  indx = r_symndx - symtab_hdr->sh_info;
	  h = elf_sym_hashes (input_bfd)[indx];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sym_sec = h->root.u.def.section;
	      relocation = (h->root.u.def.value
			    + sym_sec->output_offset
			    + sym_sec->output_section->vma);
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    relocation = 0;
	  else
	    {
	      if (!((*info->callbacks->undefined_symbol)
		    (info, h->root.root.string, input_bfd,
		     input_section, rel->r_offset, true)))
		return false;
	      break;
	    }
	}

      if (h != NULL)
	sym_name = h->root.root.string;
      else
	{
	  sym_name = bfd_elf_string_from_elf_section (input_bfd,
						      symtab_hdr->sh_link,
						      sym->st_name);
	  if (sym_name == NULL)
	    return false;
	  if (*sym_name == '\0')
	    sym_name = bfd_section_name (input_bfd, sym_sec);
	}

      r = elf32_hppa_bfd_final_link_relocate (howto, input_bfd,
					      input_section, contents,
					      rel->r_offset, relocation,
					      rel->r_addend, info, sym_sec,
					      sym_name, h);

      switch (r)
	{
	case bfd_reloc_ok:
	  break;

	case bfd_reloc_undefined:
	case bfd_reloc_notsupported:
	  (*_bfd_error_handler)
	    (_("%s: cannot handle relocation %s for %s at 0x%x in %s"),
	     bfd_get_filename (input_bfd),
	     howto->name,
	     sym_name,
	     rel->r_offset,
	     input_section->name);
	  return false;

	default:
	case bfd_reloc_outofrange:
	case bfd_reloc_overflow:
	  {
	    if (!((*info->callbacks->reloc_overflow)
		  (info, sym_name, howto->name, (bfd_vma) 0,
		   input_bfd, input_section, rel->r_offset)))
	      return false;
	  }
	  break;
	}
    }

  return true;
}

/* Called after we have seen all the input files/sections, but before
   final symbol resolution and section placement has been determined.

   We use this hook to (possibly) provide a value for __gp, then we
   fall back to the generic ELF final link routine.  */

boolean
elf32_hppa_final_link (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  if (!info->relocateable)
    {
      struct elf_link_hash_entry *h;
      asection *sec;
      bfd_vma gp_val;

      h = elf_link_hash_lookup (elf_hash_table (info), "$global$",
				false, false, false);

      if (h != NULL
	  && h->root.type == bfd_link_hash_defined)
	{
	  gp_val = h->root.u.def.value;
	  sec = h->root.u.def.section;
	}
      else
	{
	  /* If $global$ isn't defined, we could make one up ourselves
	     from the start of .plt, .dlt, or .data  For the time
	     being, just bomb.  */
	  (*info->callbacks->undefined_symbol)
	    (info, "$global$", abfd, NULL, 0, true);
	  return false;
	}

      elf32_hppa_hash_table (info)->global_value = (gp_val
						    + sec->output_section->vma
						    + sec->output_offset);
    }

  /* Invoke the standard linker.  */
  return bfd_elf_bfd_final_link (abfd, info);
}

/* Undo the generic ELF code's subtraction of section->vma from the
   value of each external symbol.  */

static boolean
elf32_hppa_add_symbol_hook (abfd, info, sym, namep, flagsp, secp, valp)
     bfd *abfd ATTRIBUTE_UNUSED;
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
     const Elf_Internal_Sym *sym ATTRIBUTE_UNUSED;
     const char **namep ATTRIBUTE_UNUSED;
     flagword *flagsp ATTRIBUTE_UNUSED;
     asection **secp;
     bfd_vma *valp;
{
  *valp += (*secp)->vma;
  return true;
}

/* Compute the size of the stub needed to call from INPUT_SEC (OFFSET)
   to DESTINATION.  Return zero if no stub is needed to perform such a
   call.  */

static unsigned int
elf32_hppa_size_of_stub (input_sec, offset, destination)
     asection *input_sec;
     bfd_vma offset;
     bfd_vma destination;
{
  bfd_vma location;

  /* Determine where the call point is.  */
  location = (input_sec->output_offset
	      + input_sec->output_section->vma
	      + offset);

  /* Determine if a long branch stub is needed.  parisc branch offsets
     are relative to the second instruction past the branch, ie. +8
     bytes on from the branch instruction location.  The offset is
     signed, 17 bits wide, and counts in units of 4 bytes.
     bfd_vma is unsigned, so this is testing for offsets outside the
     range -0x40000 to +0x3ffff  */
  if (destination - location - 8 + 0x40000 >= 0x80000)
    return 8;
  return 0;
}

/* Build one linker stub as defined by the stub hash table entry GEN_ENTRY.
   IN_ARG contains the link info pointer.  */

#define LDIL_R1		0x20200000	/* ldil LR'XXX,%r1	  */
#define BE_SR4_R1	0xe0202002	/* be,n RR'XXX(%sr4,%r1)  */

static boolean
elf32_hppa_build_one_stub (gen_entry, in_arg)
     struct bfd_hash_entry *gen_entry;
     PTR in_arg;
{
  struct elf32_hppa_stub_hash_entry *stub_entry;
  struct elf32_hppa_link_hash_table *hppa_link_hash;
  asection *stub_sec;
  bfd *stub_bfd;
  bfd_byte *loc;
  symvalue sym_value;
  int insn;

  /* Massage our args to the form they really have.  */
  stub_entry = (struct elf32_hppa_stub_hash_entry *) gen_entry;
  hppa_link_hash = (struct elf32_hppa_link_hash_table *) in_arg;

  stub_sec = stub_entry->stub_sec;

  /* Make a note of the offset within the stubs for this entry.  */
  stub_entry->offset = hppa_link_hash->offset[stub_sec->index];
  loc = stub_sec->contents + stub_entry->offset;

  sym_value = (stub_entry->target_value
	       + stub_entry->target_section->output_offset
	       + stub_entry->target_section->output_section->vma);

  stub_bfd = stub_sec->owner;

  /* Create the long branch.  A long branch is formed with "ldil"
     loading the upper bits of the target address into a register,
     then branching with "be" which adds in the lower bits.
     The "be" has its delay slot nullified.  */
  insn = hppa_rebuild_insn (LDIL_R1,
			    hppa_field_adjust (sym_value, 0, e_lrsel),
			    21);
  bfd_put_32 (stub_bfd, insn, loc);

  insn = hppa_rebuild_insn (BE_SR4_R1,
			    hppa_field_adjust (sym_value, 0, e_rrsel) >> 2,
			    17);
  bfd_put_32 (stub_bfd, insn, loc + 4);

  hppa_link_hash->offset[stub_sec->index] += 8;

  return true;
}

/* As above, but don't actually build the stub.  Just bump offset so
   we know stub section sizes.  */

static boolean
elf32_hppa_size_one_stub (gen_entry, in_arg)
     struct bfd_hash_entry *gen_entry;
     PTR in_arg;
{
  struct elf32_hppa_stub_hash_entry *stub_entry;
  struct elf32_hppa_link_hash_table *hppa_link_hash;

  /* Massage our args to the form they really have.  */
  stub_entry = (struct elf32_hppa_stub_hash_entry *) gen_entry;
  hppa_link_hash = (struct elf32_hppa_link_hash_table *) in_arg;

  hppa_link_hash->offset[stub_entry->stub_sec->index] += 8;

  return true;
}

/* External entry points for sizing and building linker stubs.  */

/* Build all the stubs associated with the current output file.  The
   stubs are kept in a hash table attached to the main linker hash
   table.  This is called via hppaelf_finish in the linker.  */

boolean
elf32_hppa_build_stubs (stub_bfd, link_info)
     bfd *stub_bfd;
     struct bfd_link_info *link_info;
{
  asection *stub_sec;
  struct bfd_hash_table *table;
  struct elf32_hppa_link_hash_table *hppa_link_hash;

  for (stub_sec = stub_bfd->sections; stub_sec; stub_sec = stub_sec->next)
    {
      unsigned int size;

      /* Allocate memory to hold the linker stubs.  */
      size = bfd_section_size (stub_bfd, stub_sec);
      stub_sec->contents = (unsigned char *) bfd_zalloc (stub_bfd, size);
      if (stub_sec->contents == NULL && size != 0)
	return false;
    }

  /* Build the stubs as directed by the stub hash table.  */
  hppa_link_hash = elf32_hppa_hash_table (link_info);
  memset (hppa_link_hash->offset, 0,
	  stub_bfd->section_count * sizeof (bfd_vma));

  table = &hppa_link_hash->stub_hash_table;
  bfd_hash_traverse (table, elf32_hppa_build_one_stub, hppa_link_hash);

  return true;
}

/* Determine and set the size of the stub section for a final link.

   The basic idea here is to examine all the relocations looking for
   PC-relative calls to a target that is unreachable with a "bl"
   instruction or calls where the caller and callee disagree on the
   location of their arguments or return value.  Currently, we don't
   support elf arg relocs.  */

boolean
elf32_hppa_size_stubs (stub_bfd, link_info,
		       add_stub_section, layout_sections_again)
     bfd *stub_bfd;
     struct bfd_link_info *link_info;
     asection * (*add_stub_section) PARAMS ((const char *, asection *));
     void (*layout_sections_again) PARAMS ((void));
{
  bfd *input_bfd;
  asection *section;
  Elf_Internal_Sym *local_syms, **all_local_syms;
  asection **stub_section_created;
  unsigned int i, indx, bfd_count, sec_count;
  asection *stub_sec;
  asection *first_init_sec = NULL;
  asection *first_fini_sec = NULL;
  struct elf32_hppa_link_hash_table *hppa_link_hash;
  struct bfd_hash_table *stub_hash_table;
  boolean stub_changed;

  /* Count the number of input BFDs and the total number of input sections.  */
  for (input_bfd = link_info->input_bfds, bfd_count = 0, sec_count = 0;
       input_bfd != NULL;
       input_bfd = input_bfd->link_next)
    {
      bfd_count += 1;
      sec_count += input_bfd->section_count;
    }

  stub_section_created
    = (asection **) bfd_zmalloc (sizeof (asection *) * sec_count);
  if (stub_section_created == NULL)
    return false;

  /* We want to read in symbol extension records only once.  To do this
     we need to read in the local symbols in parallel and save them for
     later use; so hold pointers to the local symbols in an array.  */
  all_local_syms
    = (Elf_Internal_Sym **) bfd_zmalloc (sizeof (Elf_Internal_Sym *)
					 * bfd_count);
  if (all_local_syms == NULL)
    goto error_ret_free_stub;

  /* Walk over all the input BFDs adding entries to the args hash table
     for all the external functions.  */
  for (input_bfd = link_info->input_bfds, indx = 0;
       input_bfd != NULL;
       input_bfd = input_bfd->link_next, indx++)
    {
      Elf_Internal_Shdr *symtab_hdr;
      Elf_Internal_Sym *isym;
      Elf32_External_Sym *ext_syms, *esym;

      /* We'll need the symbol table in a second.  */
      symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
      if (symtab_hdr->sh_info == 0)
	continue;

      /* We need an array of the local symbols attached to the input bfd.
	 Unfortunately, we're going to have to read & swap them in.  */
      local_syms = (Elf_Internal_Sym *)
	bfd_malloc (symtab_hdr->sh_info * sizeof (Elf_Internal_Sym));
      if (local_syms == NULL)
	{
	  goto error_ret_free_local;
	}
      all_local_syms[indx] = local_syms;
      ext_syms = (Elf32_External_Sym *)
	bfd_malloc (symtab_hdr->sh_info * sizeof (Elf32_External_Sym));
      if (ext_syms == NULL)
	{
	  goto error_ret_free_local;
	}

      if (bfd_seek (input_bfd, symtab_hdr->sh_offset, SEEK_SET) != 0
          || (bfd_read (ext_syms, 1,
			(symtab_hdr->sh_info * sizeof (Elf32_External_Sym)),
			input_bfd)
	      != (symtab_hdr->sh_info * sizeof (Elf32_External_Sym))))
	{
	  free (ext_syms);
	  goto error_ret_free_local;
	}

      /* Swap the local symbols in.  */
      isym = local_syms;
      esym = ext_syms;
      for (i = 0; i < symtab_hdr->sh_info; i++, esym++, isym++)
	bfd_elf32_swap_symbol_in (input_bfd, esym, isym);

      /* Now we can free the external symbols.  */
      free (ext_syms);
    }

  stub_hash_table = &elf32_hppa_hash_table (link_info)->stub_hash_table;

  while (1)
    {
      stub_changed = 0;

      /* Now that we have argument location information for all the
	 global functions we can start looking for stubs.  */
      for (input_bfd = link_info->input_bfds, indx = 0, sec_count = 0;
	   input_bfd != NULL;
	   input_bfd = input_bfd->link_next, indx++)
	{
	  Elf_Internal_Shdr *symtab_hdr;

	  /* We'll need the symbol table in a second.  */
	  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
	  if (symtab_hdr->sh_info == 0)
	    continue;

	  local_syms = all_local_syms[indx];

	  /* Walk over each section attached to the input bfd.  */
	  for (section = input_bfd->sections;
	       section != NULL;
	       section = section->next, sec_count++)
	    {
	      Elf_Internal_Shdr *input_rel_hdr;
	      Elf32_External_Rela *external_relocs, *erelaend, *erela;
	      Elf_Internal_Rela *internal_relocs, *irelaend, *irela;

	      /* If there aren't any relocs, then there's nothing to do.  */
	      if ((section->flags & SEC_RELOC) == 0
		  || section->reloc_count == 0)
		continue;

	      /* Allocate space for the external relocations.  */
	      external_relocs
		= ((Elf32_External_Rela *)
		   bfd_malloc (section->reloc_count
			       * sizeof (Elf32_External_Rela)));
	      if (external_relocs == NULL)
		{
		  goto error_ret_free_local;
		}

	      /* Likewise for the internal relocations.  */
	      internal_relocs = ((Elf_Internal_Rela *)
				 bfd_malloc (section->reloc_count
					     * sizeof (Elf_Internal_Rela)));
	      if (internal_relocs == NULL)
		{
		  free (external_relocs);
		  goto error_ret_free_local;
		}

	      /* Read in the external relocs.  */
	      input_rel_hdr = &elf_section_data (section)->rel_hdr;
	      if (bfd_seek (input_bfd, input_rel_hdr->sh_offset, SEEK_SET) != 0
		  || bfd_read (external_relocs, 1,
			       input_rel_hdr->sh_size,
			       input_bfd) != input_rel_hdr->sh_size)
		{
		  free (external_relocs);
		error_ret_free_internal:
		  free (internal_relocs);
		  goto error_ret_free_local;
		}

	      /* Swap in the relocs.  */
	      erela = external_relocs;
	      erelaend = erela + section->reloc_count;
	      irela = internal_relocs;
	      for (; erela < erelaend; erela++, irela++)
		bfd_elf32_swap_reloca_in (input_bfd, erela, irela);

	      /* We're done with the external relocs, free them.  */
	      free (external_relocs);

	      /* Now examine each relocation.  */
	      irela = internal_relocs;
	      irelaend = irela + section->reloc_count;
	      for (; irela < irelaend; irela++)
		{
		  unsigned int r_type, r_indx, size_of_stub;
		  struct elf32_hppa_stub_hash_entry *stub_entry;
		  asection *sym_sec;
		  const char *sym_name;
		  symvalue sym_value;
		  bfd_vma destination;
		  struct elf_link_hash_entry *hash;
		  char *stub_name;

		  r_type = ELF32_R_TYPE (irela->r_info);
		  r_indx = ELF32_R_SYM (irela->r_info);

		  if (r_type >= (unsigned int) R_PARISC_UNIMPLEMENTED)
		    {
		      bfd_set_error (bfd_error_bad_value);
		      goto error_ret_free_internal;
		    }

		  /* Only look for stubs on call instructions.  */
		  if (r_type != (unsigned int) R_PARISC_PCREL17F)
		    continue;

		  /* Now determine the call target, its name, value, section
		     and argument relocation bits.  */
		  sym_sec = NULL;
		  hash = NULL;
		  if (r_indx < symtab_hdr->sh_info)
		    {
		      /* It's a local symbol.  */
		      Elf_Internal_Sym *sym;
		      Elf_Internal_Shdr *hdr;

		      sym = local_syms + r_indx;
		      hdr = elf_elfsections (input_bfd)[sym->st_shndx];
		      sym_sec = hdr->bfd_section;
		      sym_name =
			bfd_elf_string_from_elf_section (input_bfd,
							 symtab_hdr->sh_link,
							 sym->st_name);
		      sym_value = (ELF_ST_TYPE (sym->st_info) == STT_SECTION
				   ? 0 : sym->st_value);
		      destination = (sym_value + irela->r_addend
				     + sym_sec->output_offset
				     + sym_sec->output_section->vma);
		    }
		  else
		    {
		      /* It's an external symbol.  */
		      int e_indx;

		      e_indx = r_indx - symtab_hdr->sh_info;
		      hash = elf_sym_hashes (input_bfd)[e_indx];

		      while (hash->root.type == bfd_link_hash_indirect
			     || hash->root.type == bfd_link_hash_warning)
			hash = (struct elf_link_hash_entry *)
			  hash->root.u.i.link;

		      if (hash->root.type == bfd_link_hash_undefined
			  || hash->root.type == bfd_link_hash_undefweak)
			continue;

		      if (hash->root.type == bfd_link_hash_defined
			  || hash->root.type == bfd_link_hash_defweak)
			{
			  sym_sec = hash->root.u.def.section;
			  sym_name = hash->root.root.string;
			  sym_value = hash->root.u.def.value;
			  destination = (sym_value + irela->r_addend
					 + sym_sec->output_offset
					 + sym_sec->output_section->vma);
			}
		      else
			{
			  bfd_set_error (bfd_error_bad_value);
			  goto error_ret_free_internal;
			}
		    }

		  /* Determine what (if any) linker stub is needed and its
		     size (in bytes).  */
		  size_of_stub = elf32_hppa_size_of_stub (section,
							  irela->r_offset,
							  destination);
		  if (size_of_stub == 0)
		    continue;

		  /* Get the name of this stub.  */
		  stub_name = elf32_hppa_stub_name (sym_name,
						    sym_sec,
						    section,
						    irela->r_addend,
						    hash);
		  if (!stub_name)
		    goto error_ret_free_internal;

		  stub_entry = elf32_hppa_stub_hash_lookup (stub_hash_table,
							    stub_name,
							    false,
							    false);
		  if (stub_entry != NULL)
		    {
		      /* The proper stub has already been created.  */
		      free (stub_name);
		      continue;
		    }

		  stub_sec = stub_section_created[sec_count];
		  if (stub_sec == NULL)
		    {
		      char *s_name;
		      int nstub;
		      int special_sec = 0;

		      /* We only want one stub for .init and .fini
			 because glibc splits the _init and _fini
			 functions into two parts.  We don't want to
			 put a stub in the middle of a function.  */ 
		      if (strncmp (section->name, ".init", 5) == 0)
			{
			  stub_sec = first_init_sec;
			  special_sec = 1;
			}
		      else if (strncmp (section->name, ".fini", 5) == 0)
			{
			  stub_sec = first_fini_sec;
			  special_sec = 2;
			}
		      if (stub_sec == NULL)
			{
			  s_name = bfd_alloc (stub_bfd, 16);
			  if (s_name == NULL)
			    {
			      free (stub_name);
			      goto error_ret_free_internal;
			    }
			  nstub = stub_bfd->section_count;
			  sprintf (s_name, ".stub_%x", nstub);
			  stub_sec = (*add_stub_section) (s_name, section);
			  if (stub_sec == NULL)
			    {
			      free (stub_name);
			      goto error_ret_free_internal;
			    }
			  if (special_sec != 0)
			    {
			      if (special_sec == 1)
				first_init_sec = stub_sec;
			      else
				first_fini_sec = stub_sec;
			    }
			}
		      stub_section_created[sec_count] = stub_sec;
		    }

		  /* Enter this entry into the linker stub
		     hash table.  */
		  stub_entry = elf32_hppa_stub_hash_lookup (stub_hash_table,
							    stub_name,
							    true,
							    false);
		  if (stub_entry == NULL)
		    {
		      (*_bfd_error_handler)
			(_("%s: cannot find stub entry %s"),
			 bfd_get_filename (section->owner),
			 stub_name);
		      free (stub_name);
		      goto error_ret_free_internal;
		    }

		  /* We'll need these to determine the address
		     that the stub will branch to.  */
		  stub_entry->stub_sec = stub_sec;
		  stub_entry->offset = 0;
		  stub_entry->target_value = sym_value;
		  stub_entry->target_section = sym_sec;
		  stub_changed = 1;
		}
	      /* We're done with the internal relocs, free them.  */
	      free (internal_relocs);
	    }
	}

      if (!stub_changed)
	break;

      /* OK, we've added some stubs.  Find out the new size of the
	 stub sections.  */
      hppa_link_hash = elf32_hppa_hash_table (link_info);
      hppa_link_hash->offset = (bfd_vma *)
	bfd_realloc (hppa_link_hash->offset,
		     stub_bfd->section_count * sizeof (bfd_vma));
      if (hppa_link_hash->offset == NULL)
	goto error_ret_free_local;

      memset (hppa_link_hash->offset, 0,
	      stub_bfd->section_count * sizeof (bfd_vma));

      bfd_hash_traverse (stub_hash_table,
			 elf32_hppa_size_one_stub,
			 hppa_link_hash);

      for (stub_sec = stub_bfd->sections;
	   stub_sec != NULL;
	   stub_sec = stub_sec->next)
	{
	  bfd_set_section_size (stub_bfd, stub_sec,
				hppa_link_hash->offset[stub_sec->index]);
	}
      /* Ask the linker to do its stuff.  */
      (*layout_sections_again) ();
    }

  /* We're done with the local symbols, free them.  */
  for (i = 0; i < bfd_count; i++)
    if (all_local_syms[i])
      free (all_local_syms[i]);
  free (all_local_syms);
  free (stub_section_created);
  return true;

 error_ret_free_local:
  for (i = 0; i < bfd_count; i++)
    if (all_local_syms[i])
      free (all_local_syms[i]);
  free (all_local_syms);

 error_ret_free_stub:
  free (stub_section_created);
  return false;
}

/* Misc BFD support code.  */
#define bfd_elf32_bfd_reloc_type_lookup		elf_hppa_reloc_type_lookup
#define bfd_elf32_bfd_is_local_label_name	elf_hppa_is_local_label_name
#define elf_info_to_howto               	elf_hppa_info_to_howto
#define elf_info_to_howto_rel           	elf_hppa_info_to_howto_rel

/* Stuff for the BFD linker.  */
#define elf_backend_relocate_section		elf32_hppa_relocate_section
#define elf_backend_add_symbol_hook		elf32_hppa_add_symbol_hook
#define bfd_elf32_bfd_final_link		elf32_hppa_final_link
#if 0
#define elf_backend_check_relocs		elf32_hppa_check_relocs
#endif
#define bfd_elf32_bfd_link_hash_table_create \
  elf32_hppa_link_hash_table_create
#define elf_backend_fake_sections		elf_hppa_fake_sections


#define TARGET_BIG_SYM		bfd_elf32_hppa_vec
#define TARGET_BIG_NAME		"elf32-hppa"
#define ELF_ARCH		bfd_arch_hppa
#define ELF_MACHINE_CODE	EM_PARISC
#define ELF_MAXPAGESIZE		0x1000

#include "elf32-target.h"
