/* TXVU-specific support for 32-bit ELF.
   Copyright (C) 1997, 1998 Free Software Foundation, Inc.

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
#include "elf/txvu.h"

static reloc_howto_type *bfd_elf32_bfd_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void txvu_info_to_howto_rel
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rela *));
static boolean txvu_elf_relocate_section
  PARAMS ((bfd *, struct bfd_link_info *, bfd *, asection *, bfd_byte *,
	   Elf_Internal_Rela *, Elf_Internal_Sym *, asection **));

/* Use RELA, not REL.  REL incurs complications one would rather not
   deal with.  */
/*#define USE_REL*/

static reloc_howto_type txvu_elf_howto_table[] =
{
  /* This reloc does nothing.  */
  HOWTO (R_TXVU_NONE,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TXVU_NONE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A PC Relative 11-bit relocation, shifted by 3.
     Fortunately, the lower instruction has the lower address in the 64 bit
     pair.  Thus we can treat the address of the instruction as a whole
     as the address of the lower instruction (the one with the branch),
     and pretend the instruction size is 32 bits.
     Otherwise, we'd have to either perform the relocation on all 64 bits
     (since the pc relative address is the start of the pair), or set the
     address of the reloc as the address of the lower instruction and do what
     is done for R_M32R_10_PCREL which is to mask off the lower 3 bits of the
     pc before performing the reloc.  */
  HOWTO (R_TXVU_11_PCREL,	/* type */
	 3,	                /* rightshift */
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */
	 11,	                /* bitsize */
	 true,	                /* pc_relative */
	 0,	                /* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_TXVU_11_PCREL",	/* name */
	 false,	                /* partial_inplace */
	 0x1ff,		        /* src_mask */
	 0x1ff,   		/* dst_mask */
	 true),			/* pcrel_offset */
};

/* Map BFD reloc types to TXVU ELF reloc types.  */

struct txvu_reloc_map
{
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct txvu_reloc_map txvu_reloc_map[] =
{
  { BFD_RELOC_NONE, R_TXVU_NONE },
  { BFD_RELOC_TXVU_11_PCREL, R_TXVU_11_PCREL }
};

static reloc_howto_type *
bfd_elf32_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = 0;
       i < sizeof (txvu_reloc_map) / sizeof (struct txvu_reloc_map);
       i++)
    {
      if (txvu_reloc_map[i].bfd_reloc_val == code)
	return &txvu_elf_howto_table[txvu_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}

/* Set the howto pointer for an TXVU ELF reloc.  */

static void
txvu_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf32_Internal_Rela *dst;
{
  unsigned int r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_TXVU_max);
  cache_ptr->howto = &txvu_elf_howto_table[r_type];
}

/* Relocate a TXVU ELF section.

   The RELOCATE_SECTION function is called by the new ELF backend linker
   to handle the relocations for a section.

   The relocs are always passed as Rela structures; if the section
   actually uses Rel structures, the r_addend field will always be
   zero.

   This function is responsible for adjust the section contents as
   necessary, and (if using Rela relocs and generating a
   relocateable output file) adjusting the reloc addend as
   necessary.

   This function does not have to worry about setting the reloc
   address or the reloc symbol index.

   LOCAL_SYMS is a pointer to the swapped in local symbols.

   LOCAL_SECTIONS is an array giving the section in the input file
   corresponding to the st_shndx field of each local symbol.

   The global hash table entry for the global symbols can be found
   via elf_sym_hashes (input_bfd).

   When generating relocateable output, this function must handle
   STB_LOCAL/STT_SECTION symbols specially.  The output symbol is
   going to be the section symbol corresponding to the output
   section, which means that the addend must be adjusted
   accordingly.  */

static boolean
txvu_elf_relocate_section (output_bfd, info, input_bfd, input_section,
			   contents, relocs, local_syms, local_sections)
     bfd *output_bfd;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     Elf_Internal_Rela *relocs;
     Elf_Internal_Sym *local_syms;
     asection **local_sections;
{
  Elf_Internal_Shdr *symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  struct elf_link_hash_entry **sym_hashes = elf_sym_hashes (input_bfd);
  Elf_Internal_Rela *rel, *relend;
  bfd *dynobj = elf_hash_table (info)->dynobj;
  /* Assume success.  */
  boolean ret = true;

  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      int r_type;
      reloc_howto_type *howto;
      unsigned long r_symndx;
      /* We can't modify r_addend here as elf_link_input_bfd has an assert to
	 ensure it's zero (we use REL relocs, not RELA).  Therefore this
	 should be assigning zero to `addend', but for clarity we use
	 `r_addend'.  */
      /* ??? The previous comment is old, revisit and delete.  */
      bfd_vma addend = rel->r_addend;
      bfd_vma offset = rel->r_offset;
      struct elf_link_hash_entry *h;
      Elf_Internal_Sym *sym;
      asection *sec;
      const char *sym_name;
      bfd_reloc_status_type r;
      const char *errmsg = NULL;

      r_type = ELF32_R_TYPE (rel->r_info);
      if (r_type < 0 || r_type >= (int) R_TXVU_max)
	{
	  (*_bfd_error_handler) ("%s: unknown relocation type %d",
				 bfd_get_filename (input_bfd),
				 (int) r_type);
	  bfd_set_error (bfd_error_bad_value);
	  ret = false;
	  continue;
	}

      howto = txvu_elf_howto_table + r_type;
      r_symndx = ELF32_R_SYM (rel->r_info);

      if (info->relocateable)
	{
	  /* This is a relocateable link.  We don't have to change
	     anything, unless the reloc is against a section symbol,
	     in which case we have to adjust according to where the
	     section symbol winds up in the output section.  */
	  sec = NULL;
	  if (r_symndx >= symtab_hdr->sh_info)
	    {
	      /* External symbol.  */
	      continue;
	    }

	  /* Local symbol.  */
	  sym = local_syms + r_symndx;
	  sym_name = "<local symbol>";
	  /* STT_SECTION: symbol is associated with a section.  */
	  if (ELF_ST_TYPE (sym->st_info) != STT_SECTION)
	    {
	      /* Symbol isn't associated with a section.  Nothing to do.  */
	      continue;
	    }

	  sec = local_sections[r_symndx];
	  addend += sec->output_offset + sym->st_value;
	  rel->r_addend = addend;

	  /* Addends are stored with relocs.  We're done.  */
	  continue;
	}
      else
	{
	  bfd_vma relocation;

	  /* This is a final link.  */
	  h = NULL;
	  sym = NULL;
	  sec = NULL;

	  if (r_symndx < symtab_hdr->sh_info)
	    {
	      /* Local symbol.  */
	      sym = local_syms + r_symndx;
	      sec = local_sections[r_symndx];
	      sym_name = "<local symbol>";
	      relocation = (sec->output_section->vma
			    + sec->output_offset
			    + sym->st_value);
	    }
	  else
	    {
	      /* External symbol.  */
	      h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	      while (h->root.type == bfd_link_hash_indirect
		     || h->root.type == bfd_link_hash_warning)
		h = (struct elf_link_hash_entry *) h->root.u.i.link;
	      sym_name = h->root.root.string;

	      if (h->root.type == bfd_link_hash_defined
		  || h->root.type == bfd_link_hash_defweak)
		{
		  sec = h->root.u.def.section;
		  if (sec->output_section == NULL)
		    relocation = 0;
		  else
		    relocation = (h->root.u.def.value
				  + sec->output_section->vma
				  + sec->output_offset);
		}
	      else if (h->root.type == bfd_link_hash_undefweak)
		relocation = 0;
	      else
		{
		  if (! ((*info->callbacks->undefined_symbol)
			 (info, h->root.root.string, input_bfd,
			  input_section, offset)))
		    return false;
		  relocation = 0;
		}
	    }

	  /* Sanity check the address.  */
	  if (offset > input_section->_raw_size)
	    {
	      r = bfd_reloc_outofrange;
	      goto check_reloc;
	    }

	  switch ((int) r_type)
	    {
	      /* insert reloc handling code here */
	    default :
	      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
					    contents, offset,
					    relocation, addend);
	      break;
	    }
	}

    check_reloc:

      if (r != bfd_reloc_ok)
	{
	  /* FIXME: This should be generic enough to go in a utility.  */
	  const char *name;

	  if (h != NULL)
	    name = h->root.root.string;
	  else
	    {
	      name = (bfd_elf_string_from_elf_section
		      (input_bfd, symtab_hdr->sh_link, sym->st_name));
	      if (name == NULL || *name == '\0')
		name = bfd_section_name (input_bfd, sec);
	    }

	  if (errmsg != NULL)
	    goto common_error;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      if (! ((*info->callbacks->reloc_overflow)
		     (info, name, howto->name, (bfd_vma) 0,
		      input_bfd, input_section, offset)))
		return false;
	      break;

	    case bfd_reloc_undefined:
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, name, input_bfd, input_section,
		      offset)))
		return false;
	      break;

	    case bfd_reloc_outofrange:
	      errmsg = "internal error: out of range error";
	      goto common_error;

	    case bfd_reloc_notsupported:
	      errmsg = "internal error: unsupported relocation error";
	      goto common_error;

	    case bfd_reloc_dangerous:
	      errmsg = "internal error: dangerous error";
	      goto common_error;

	    default:
	      errmsg = "internal error: unknown error";
	      /* fall through */

	    common_error:
	      if (!((*info->callbacks->warning)
		    (info, errmsg, name, input_bfd, input_section,
		     offset)))
		return false;
	      break;
	    }
	}
    }

  return ret;
}

#define ELF_ARCH		bfd_arch_txvu
#define ELF_MACHINE_CODE	EM_CYGNUS_TXVU
#define ELF_MAXPAGESIZE		0x1000

#define TARGET_BIG_SYM          bfd_elf32_txvu_vec
#define TARGET_BIG_NAME		"elf32-txvu"

#define elf_info_to_howto			txvu_info_to_howto_rel
#define elf_backend_relocate_section		txvu_elf_relocate_section
					
#include "elf32-target.h"
