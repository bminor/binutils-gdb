/* FR30-specific support for 32-bit ELF.
   Copyright (C) 1998 Free Software Foundation, Inc.

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
#include "elf/fr30.h"

/* Forward declarations.  */
static bfd_reloc_status_type fr30_elf_i20_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));

static reloc_howto_type fr30_elf_howto_table [] =
{
  /* This reloc does nothing.  */
  HOWTO (R_FR30_NONE,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_FR30_NONE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* An 8 bit absolute relocation.  */
  HOWTO (R_FR30_8,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 4,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_FR30_8",		/* name */
	 true,			/* partial_inplace */
	 0x0ff0,		/* src_mask */
	 0x0ff0,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 20 bit absolute relocation.  */
  HOWTO (R_FR30_20,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 20,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 fr30_elf_i20_reloc,	/* special_function */
	 "R_FR30_20",		/* name */
	 true,			/* partial_inplace */
	 0x00f0ffff,		/* src_mask */
	 0x00f0ffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 32 bit absolute relocation.  */
  HOWTO (R_FR30_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_FR30_32",		/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 6 bit absolute relocation.  */
  HOWTO (R_FR30_6_IN_4,		/* type */
	 2,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 6,			/* bitsize */
	 false,			/* pc_relative */
	 4,			/* bitpos */
	 complain_overflow_unsigned, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_FR30_6_IN_4",	/* name */
	 true,			/* partial_inplace */
	 0x00f0,		/* src_mask */
	 0x00f0,		/* dst_mask */
	 false),		/* pcrel_offset */
  
  /* An 8 bit absolute relocation.  */
  HOWTO (R_FR30_8_IN_8,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 4,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,/* special_function */
	 "R_FR30_8_IN_8",	/* name */
	 true,			/* partial_inplace */
	 0x0ff0,		/* src_mask */
	 0x0ff0,		/* dst_mask */
	 false),		/* pcrel_offset */
  
  /* A 9 bit absolute relocation.  */
  HOWTO (R_FR30_9_IN_8,		/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 9,			/* bitsize */
	 false,			/* pc_relative */
	 4,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,/* special_function */
	 "R_FR30_9_IN_8",	/* name */
	 true,			/* partial_inplace */
	 0x0ff0,		/* src_mask */
	 0x0ff0,		/* dst_mask */
	 false),		/* pcrel_offset */
  
  /* A 10 bit absolute relocation.  */
  HOWTO (R_FR30_10_IN_8,	/* type */
	 2,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 10,			/* bitsize */
	 false,			/* pc_relative */
	 4,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,/* special_function */
	 "R_FR30_10_IN_8",	/* name */
	 true,			/* partial_inplace */
	 0x0ff0,		/* src_mask */
	 0x0ff0,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A PC relative 9 bit relocation, right shifted by 1.  */
  HOWTO (R_FR30_9_PCREL,	/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 9,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_FR30_9_PCREL",	/* name */
	 false,			/* partial_inplace */
	 0x00ff,		/* src_mask */
	 0x00ff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A PC relative 12 bit relocation, right shifted by 1.  */
  HOWTO (R_FR30_12_PCREL,	/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 12,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_FR30_12_PCREL",	/* name */
	 false,			/* partial_inplace */
	 0x07ff,		/* src_mask */
	 0x07ff,		/* dst_mask */
	 true),			/* pcrel_offset */
};

/* Utility to actually perform an R_FR30_20 reloc.  */

static bfd_reloc_status_type
fr30_elf_i20_reloc (abfd, reloc_entry, symbol, data,
		    input_section, output_bfd, error_message)
     bfd *      abfd;
     arelent *  reloc_entry;
     asymbol *  symbol;
     PTR        data;
     asection * input_section;
     bfd *      output_bfd;
     char **    error_message;
{
  bfd_vma       relocation;
  unsigned long x;
  
  /* This part is from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (! reloc_entry->howto->partial_inplace
	  || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    /* FIXME: See bfd_perform_relocation.  Is this right?  */
    return bfd_reloc_continue;

  relocation =
    symbol->value
    + symbol->section->output_section->vma
    + symbol->section->output_offset
    + reloc_entry->addend;

  if (relocation > ((1U << 20) - 1))
    return bfd_reloc_overflow;

  x = bfd_get_32 (abfd, data + reloc_entry->address);
  x = (x & 0xff0f0000) | (relocation & 0x0000ffff) | ((relocation & 0x000f0000) << 4);
  bfd_put_32 (abfd, x, data + reloc_entry->address);

  return bfd_reloc_ok;
}


/* Map BFD reloc types to FR30 ELF reloc types.  */

struct fr30_reloc_map
{
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct fr30_reloc_map fr30_reloc_map [] =
{
  { BFD_RELOC_NONE,           R_FR30_NONE },
  { BFD_RELOC_8,              R_FR30_8 },
  { BFD_RELOC_FR30_20,        R_FR30_20 },
  { BFD_RELOC_32,             R_FR30_32 },
  { BFD_RELOC_FR30_6_IN_4,    R_FR30_6_IN_4 },
  { BFD_RELOC_FR30_8_IN_8,    R_FR30_8_IN_8 },
  { BFD_RELOC_FR30_9_IN_8,    R_FR30_9_IN_8 },
  { BFD_RELOC_FR30_10_IN_8,   R_FR30_10_IN_8 },
  { BFD_RELOC_FR30_9_PCREL,   R_FR30_9_PCREL },
  { BFD_RELOC_FR30_12_PCREL,  R_FR30_12_PCREL },
};

static reloc_howto_type *
bfd_elf32_bfd_reloc_type_lookup (abfd, code)
     bfd * abfd;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = sizeof (fr30_reloc_map) / sizeof (fr30_reloc_map[0]);
       --i;)
    {
      if (fr30_reloc_map [i].bfd_reloc_val == code)
	return & fr30_elf_howto_table [fr30_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}

/* Set the howto pointer for an FR30 ELF reloc.  */

static void
fr30_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd * abfd;
     arelent * cache_ptr;
     Elf32_Internal_Rela * dst;
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_FR30_max);
  cache_ptr->howto = & fr30_elf_howto_table [r_type];
}

/* Relocate an FR30 ELF section.
   There is some attempt to make this function usable for many architectures,
   both USE_REL and USE_RELA ['twould be nice if such a critter existed],
   if only to serve as a learning tool.

   The RELOCATE_SECTION function is called by the new ELF backend linker
   to handle the relocations for a section.

   The relocs are always passed as Rela structures; if the section
   actually uses Rel structures, the r_addend field will always be
   zero.

   This function is responsible for adjusting the section contents as
   necessary, and (if using Rela relocs and generating a relocateable
   output file) adjusting the reloc addend as necessary.

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
fr30_elf_relocate_section (output_bfd, info, input_bfd, input_section,
			   contents, relocs, local_syms, local_sections)
     bfd *                   output_bfd;
     struct bfd_link_info *  info;
     bfd *                   input_bfd;
     asection *              input_section;
     bfd_byte *              contents;
     Elf_Internal_Rela *     relocs;
     Elf_Internal_Sym *      local_syms;
     asection **             local_sections;
{
  Elf_Internal_Shdr *           symtab_hdr;
  struct elf_link_hash_entry ** sym_hashes;
  Elf_Internal_Rela *           rel;
  Elf_Internal_Rela *           relend;

  symtab_hdr = & elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  relend     = relocs + input_section->reloc_count;
  
  for (rel = relocs; rel < relend; rel ++)
    {
      reloc_howto_type *           howto;
      unsigned long                r_symndx;
      Elf_Internal_Sym *           sym;
      asection *                   sec;
      struct elf_link_hash_entry * h;
      bfd_vma                      relocation;
      bfd_reloc_status_type        r;
      const char *                 name;

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
		  sec = local_sections [r_symndx];
		  rel->r_addend += sec->output_offset + sym->st_value;
		}
	    }

	  continue;
	}

      /* This is a final link.  */
      howto  = fr30_elf_howto_table + ELF32_R_TYPE (rel->r_info);
      h      = NULL;
      sym    = NULL;
      sec    = NULL;
      
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections [r_symndx];
	  relocation = (sec->output_section->vma
			+ sec->output_offset
			+ sym->st_value);
	  
	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);
	  name = (name == NULL) ? bfd_section_name (input_bfd, sec) : name;
#if 0
	  fprintf (stderr, "local: sec: %s, sym: %s (%d), value: %x + %x + %x addend %x\n",
		   sec->name, name, sym->st_name,
		   sec->output_section->vma, sec->output_offset,
		   sym->st_value, rel->r_addend);
#endif
	}
      else
	{
	  h = sym_hashes [r_symndx - symtab_hdr->sh_info];
	  
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;

	  name = h->root.root.string;
	  
	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sec = h->root.u.def.section;
	      relocation = (h->root.u.def.value
			    + sec->output_section->vma
			    + sec->output_offset);
#if 0
	      fprintf (stderr,
		       "defined: sec: %s, name: %s, value: %x + %x + %x gives: %x\n",
		       sec->name, name, h->root.u.def.value,
		       sec->output_section->vma, sec->output_offset, relocation);
#endif
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    {
#if 0
	      fprintf (stderr, "undefined: sec: %s, name: %s\n",
		       sec->name, name);
#endif
	      relocation = 0;
	    }
	  else
	    {
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, h->root.root.string, input_bfd,
		      input_section, rel->r_offset)))
		return false;
#if 0
	      fprintf (stderr, "unknown: name: %s\n", name);
#endif
	      relocation = 0;
	    }
	}

      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
				    contents, rel->r_offset,
				    relocation, rel->r_addend);

      if (r != bfd_reloc_ok)
	{
	  const char * msg = (const char *)0;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      if (! ((*info->callbacks->reloc_overflow)
		     (info, name, howto->name, (bfd_vma) 0,
		      input_bfd, input_section, rel->r_offset)))
		return false;
	      break;

	    case bfd_reloc_undefined:
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, name, input_bfd, input_section,
		      rel->r_offset)))
		return false;
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      goto common_error;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      goto common_error;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous relocation");
	      goto common_error;

	    case bfd_reloc_other:
	      msg = _("could not locate special linker symbol __gp");
	      goto common_error;

	    case bfd_reloc_continue:
	      msg = _("could not locate special linker symbol __ep");
	      goto common_error;

	    case (bfd_reloc_dangerous + 1):
	      msg = _("could not locate special linker symbol __ctbp");
	      goto common_error;
	      
	    default:
	      msg = _("internal error: unknown error");
	      /* fall through */

	    common_error:
	      if (!((*info->callbacks->warning)
		    (info, msg, name, input_bfd, input_section,
		     rel->r_offset)))
		return false;
	      break;
	    }
	}
    }

  return true;
}

#define ELF_ARCH		bfd_arch_fr30
#define ELF_MACHINE_CODE	EM_CYGNUS_FR30
#define ELF_MAXPAGESIZE		0x1000

#define TARGET_BIG_SYM          bfd_elf32_fr30_vec
#define TARGET_BIG_NAME		"elf32-fr30"

#define elf_info_to_howto			0
#define elf_info_to_howto_rel			fr30_info_to_howto_rel
#define elf_backend_relocate_section		fr30_elf_relocate_section
					
#include "elf32-target.h"
