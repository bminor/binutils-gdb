/* Generic support for 32-bit ELF
   Copyright 1993, 1995, 1998, 1999 Free Software Foundation, Inc.

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
#include "elf/h8.h"

static reloc_howto_type *elf32_h8_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void elf32_h8_info_to_howto
  PARAMS ((bfd *, arelent *, Elf_Internal_Rela *));
static void elf32_h8_info_to_howto_rel
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rel *));
static int elf32_h8_mach
  PARAMS ((flagword));
static bfd_reloc_status_type elf32_h8_final_link_relocate
  PARAMS ((unsigned long, bfd *, bfd *, asection *,
	   bfd_byte *, bfd_vma, bfd_vma, bfd_vma,
	   struct bfd_link_info *, asection *, int));
static boolean elf32_h8_relocate_section
  PARAMS ((bfd *, struct bfd_link_info *, bfd *, asection *,
	   bfd_byte *, Elf_Internal_Rela *,
	   Elf_Internal_Sym *, asection **));
static bfd_reloc_status_type special 
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));

/* This does not include any relocation information, but should be
   good enough for GDB or objdump to read the file.  */

static reloc_howto_type h8_elf_howto_table[] =
{
#define R_H8_NONE_X 0
  HOWTO (R_H8_NONE,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 special,			/* special_function */
	 "R_H8_NONE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */
#define R_H8_DIR32_X (R_H8_NONE_X + 1)
  HOWTO (R_H8_DIR32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 special,			/* special_function */
	 "R_H8_DIR32",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */
#define R_H8_DIR16_X (R_H8_DIR32_X + 1)
  HOWTO (R_H8_DIR16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 special,			/* special_function */
	 "R_H8_DIR16",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 false),		/* pcrel_offset */
#define R_H8_DIR8_X (R_H8_DIR16_X + 1)
  HOWTO (R_H8_DIR8,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 special,			/* special_function */
	 "R_H8_DIR16",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0x000000ff,		/* dst_mask */
	 false),		/* pcrel_offset */
#define R_H8_DIR16A8_X (R_H8_DIR8_X + 1)
  HOWTO (R_H8_DIR16A8,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 special,			/* special_function */
	 "R_H8_DIR16A8",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 false),		/* pcrel_offset */
#define R_H8_DIR16R8_X (R_H8_DIR16A8_X + 1)
  HOWTO (R_H8_DIR16R8,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 special,			/* special_function */
	 "R_H8_DIR16R8",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 false),		/* pcrel_offset */
#define R_H8_DIR24A8_X (R_H8_DIR16R8_X + 1)
  HOWTO (R_H8_DIR24A8,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 24,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 special,			/* special_function */
	 "R_H8_DIR24A8",	/* name */
	 true,			/* partial_inplace */
	 0xff000000,		/* src_mask */
	 0x00ffffff,		/* dst_mask */
	 false),		/* pcrel_offset */
#define R_H8_DIR24R8_X (R_H8_DIR24A8_X + 1)
  HOWTO (R_H8_DIR24R8,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 24,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 special,			/* special_function */
	 "R_H8_DIR24R8",	/* name */
	 true,			/* partial_inplace */
	 0xff000000,		/* src_mask */
	 0x00ffffff,		/* dst_mask */
	 false),		/* pcrel_offset */
#define R_H8_DIR32A16_X (R_H8_DIR24R8_X + 1)
  HOWTO (R_H8_DIR32A16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 special,			/* special_function */
	 "R_H8_DIR32",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */
#define R_H8_PCREL16_X (R_H8_DIR32A16_X + 1)
  HOWTO (R_H8_PCREL16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 special,			/* special_function */
	 "R_H8_PCREL16",	/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 true),			/* pcrel_offset */
#define R_H8_PCREL8_X (R_H8_PCREL16_X + 1)
  HOWTO (R_H8_PCREL8,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 special,			/* special_function */
	 "R_H8_PCREL8",		/* name */
	 false,			/* partial_inplace */
	 0xff,			/* src_mask */
	 0xff,			/* dst_mask */
	 true),			/* pcrel_offset */
};

/* This structure is used to map BFD reloc codes to H8 ELF relocs.  */

struct elf_reloc_map
{
  bfd_reloc_code_real_type bfd_reloc_val;
  unsigned char howto_index;
};

/* An array mapping BFD reloc codes to SH ELF relocs.  */

static const struct elf_reloc_map h8_reloc_map[] =
{
  { BFD_RELOC_NONE, R_H8_NONE_X },
  { BFD_RELOC_32, R_H8_DIR32_X },
  { BFD_RELOC_16, R_H8_DIR16_X },
  { BFD_RELOC_8, R_H8_DIR8_X },
  { BFD_RELOC_H8_DIR16A8, R_H8_DIR16A8_X },
  { BFD_RELOC_H8_DIR16R8, R_H8_DIR16R8_X },
  { BFD_RELOC_H8_DIR24A8, R_H8_DIR24A8_X },
  { BFD_RELOC_H8_DIR24R8, R_H8_DIR24R8_X },
  { BFD_RELOC_H8_DIR32A16, R_H8_DIR32A16_X },
  { BFD_RELOC_16_PCREL, R_H8_PCREL16_X },
  { BFD_RELOC_8_PCREL, R_H8_PCREL8_X },
};


static reloc_howto_type *
elf32_h8_reloc_type_lookup (abfd, code)
     bfd *abfd ATTRIBUTE_UNUSED;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = 0; i < sizeof (h8_reloc_map) / sizeof (struct elf_reloc_map); i++)
    {
      if (h8_reloc_map[i].bfd_reloc_val == code)
	return &h8_elf_howto_table[(int) h8_reloc_map[i].howto_index];
    }
  return NULL;
}

static void
elf32_h8_info_to_howto (abfd, bfd_reloc, elf_reloc)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *bfd_reloc;
     Elf32_Internal_Rela *elf_reloc;
{
  unsigned int r;
  unsigned int i;

  r = ELF32_R_TYPE (elf_reloc->r_info);
  for (i = 0; i < sizeof (h8_elf_howto_table) / sizeof (reloc_howto_type); i++)
    if (h8_elf_howto_table[i].type== r)
      {
	bfd_reloc->howto = &h8_elf_howto_table[i];
	return;
      }
  abort ();
}

static void
elf32_h8_info_to_howto_rel (abfd, bfd_reloc, elf_reloc)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *bfd_reloc;
     Elf32_Internal_Rel *elf_reloc ATTRIBUTE_UNUSED;
{
  unsigned int r;

  abort ();
  r = ELF32_R_TYPE (elf_reloc->r_info);
  bfd_reloc->howto = &h8_elf_howto_table[r];
}

/* Special handling for H8/300 relocs.
   We only come here for pcrel stuff and return normally if not an -r link.
   When doing -r, we can't do any arithmetic for the pcrel stuff, because
   we support relaxing on the H8/300 series chips.  */
static bfd_reloc_status_type
special (abfd, reloc_entry, symbol, data, input_section, output_bfd,
	 error_message)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *reloc_entry ATTRIBUTE_UNUSED;
     asymbol *symbol ATTRIBUTE_UNUSED;
     PTR data ATTRIBUTE_UNUSED;
     asection *input_section ATTRIBUTE_UNUSED;
     bfd *output_bfd;
     char **error_message ATTRIBUTE_UNUSED;
{
  if (output_bfd == (bfd *) NULL)
    return bfd_reloc_continue;

  /* Adjust the reloc address to that in the output section.  */
  reloc_entry->address += input_section->output_offset;
  return bfd_reloc_ok;
}

/* Perform a relocation as part of a final link.  */
static bfd_reloc_status_type
elf32_h8_final_link_relocate (r_type, input_bfd, output_bfd,
			      input_section, contents, offset, value,
			      addend, info, sym_sec, is_local)
     unsigned long r_type;
     bfd *input_bfd;
     bfd *output_bfd ATTRIBUTE_UNUSED;
     asection *input_section ATTRIBUTE_UNUSED;
     bfd_byte *contents;
     bfd_vma offset;
     bfd_vma value;
     bfd_vma addend;
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
     asection *sym_sec ATTRIBUTE_UNUSED;
     int is_local ATTRIBUTE_UNUSED;
{
  bfd_byte *hit_data = contents + offset;

  switch (r_type)
    {

    case R_H8_NONE:
      return bfd_reloc_ok;

    case R_H8_DIR32:
    case R_H8_DIR32A16:
    case R_H8_DIR24A8:
      value += addend;
      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_H8_DIR16:
    case R_H8_DIR16A8:
    case R_H8_DIR16R8:
      value += addend;
      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    /* AKA R_RELBYTE */
    case R_H8_DIR8:
      value += addend;

      bfd_put_8 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_H8_DIR24R8:
      value += addend;

      /* HIT_DATA is the address for the first byte for the relocated
	 value.  Subtract 1 so that we can manipulate the data in 32bit
	 hunks.  */
      hit_data--;

      /* Clear out the top byte in value.  */
      value &= 0xffffff;

      /* Retrieve the type byte for value from the section contents.  */
      value |= (bfd_get_32 (input_bfd, hit_data) & 0xff000000);

      /* Now scribble it out in one 32bit hunk.  */
      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_H8_PCREL16:
      value -= (input_section->output_section->vma
		+ input_section->output_offset);
      value -= offset;
      value += addend;

      /* The value is relative to the start of the instruction,
	 not the relocation offset.  Subtract 2 to account for
	 this minor issue.  */
      value -= 2;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_H8_PCREL8:
      value -= (input_section->output_section->vma
		+ input_section->output_offset);
      value -= offset;
      value += addend;

      /* The value is relative to the start of the instruction,
	 not the relocation offset.  Subtract 1 to account for
	 this minor issue.  */
      value -= 1;

      bfd_put_8 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    default:
      return bfd_reloc_notsupported;
    }
}

/* Relocate an H8 ELF section.  */
static boolean
elf32_h8_relocate_section (output_bfd, info, input_bfd, input_section,
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
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel, *relend;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);

  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      int r_type;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;

      r_symndx = ELF32_R_SYM (rel->r_info);
      r_type = ELF32_R_TYPE (rel->r_info);

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
		  sec = local_sections[r_symndx];
		  rel->r_addend += sec->output_offset + sym->st_value;
		}
	    }

	  continue;
	}

      /* This is a final link.  */
      h = NULL;
      sym = NULL;
      sec = NULL;
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = (sec->output_section->vma
			+ sec->output_offset
			+ sym->st_value);
	}
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sec = h->root.u.def.section;
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
		      input_section, rel->r_offset, true)))
		return false;
	      relocation = 0;
	    }
	}

      r = elf32_h8_final_link_relocate (r_type, input_bfd, output_bfd,
					input_section,
					contents, rel->r_offset,
					relocation, rel->r_addend,
					info, sec, h == NULL);

      if (r != bfd_reloc_ok)
	{
	  const char *name;
	  const char *msg = (const char *) 0;
	  arelent bfd_reloc;
	  reloc_howto_type *howto;

	  elf32_h8_info_to_howto (input_bfd, &bfd_reloc, rel);
	  howto = bfd_reloc.howto;
	  
	  if (h != NULL)
	    name = h->root.root.string;
	  else
	    {
	      name = (bfd_elf_string_from_elf_section
		      (input_bfd, symtab_hdr->sh_link, sym->st_name));
	      if (name == NULL || *name == '\0')
		name = bfd_section_name (input_bfd, sec);
	    }

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
		      rel->r_offset, true)))
		return false;
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      goto common_error;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      goto common_error;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous error");
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

/* Object files encode the specific H8 model they were compiled
   for in the ELF flags field.

   Examine that field and return the proper BFD machine type for
   the object file.  */
static int
elf32_h8_mach (flags)
     flagword flags;
{
  switch (flags & EF_H8_MACH)
    {
    case E_H8_MACH_H8300:
    default:
      return bfd_mach_h8300;

    case E_H8_MACH_H8300H:
      return bfd_mach_h8300h;

    case E_H8_MACH_H8300S:
      return bfd_mach_h8300s;
    }
}

/* The final processing done just before writing out a H8 ELF object
   file.  We use this opportunity to encode the BFD machine type
   into the flags field in the object file.  */

void
elf32_h8_final_write_processing (abfd, linker)
     bfd *abfd;
     boolean linker ATTRIBUTE_UNUSED;
{
  unsigned long val;

  switch (bfd_get_mach (abfd))
    {
    default:
    case bfd_mach_h8300:
      val = E_H8_MACH_H8300;
      break;

    case bfd_mach_h8300h:
      val = E_H8_MACH_H8300H;
      break;

    case bfd_mach_h8300s:
      val = E_H8_MACH_H8300S;
      break;
    }

  elf_elfheader (abfd)->e_flags &= ~ (EF_H8_MACH);
  elf_elfheader (abfd)->e_flags |= val;
}

/* Return nonzero if ABFD represents a valid H8 ELF object file; also
   record the encoded machine type found in the ELF flags.  */

boolean
elf32_h8_object_p (abfd)
     bfd *abfd;
{
  bfd_default_set_arch_mach (abfd, bfd_arch_h8300,
			     elf32_h8_mach (elf_elfheader (abfd)->e_flags));
  return true;
}

/* Merge backend specific data from an object file to the output
   object file when linking.  The only data we need to copy at this
   time is the architecture/machine information.  */

boolean
elf32_h8_merge_private_bfd_data (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return true;

  if (bfd_get_arch (obfd) == bfd_get_arch (ibfd)
      && bfd_get_mach (obfd) < bfd_get_mach (ibfd))
    {
      if (! bfd_set_arch_mach (obfd, bfd_get_arch (ibfd),
                               bfd_get_mach (ibfd)))
        return false;
    }

  return true;
}


#define TARGET_BIG_SYM			bfd_elf32_h8300_vec
#define TARGET_BIG_NAME			"elf32-h8300"
#define ELF_ARCH			bfd_arch_h8300
#define ELF_MACHINE_CODE		EM_H8_300
#define ELF_MAXPAGESIZE			0x1
#define bfd_elf32_bfd_reloc_type_lookup elf32_h8_reloc_type_lookup
#define elf_info_to_howto		elf32_h8_info_to_howto
#define elf_info_to_howto_rel		elf32_h8_info_to_howto_rel

/* So we can set/examine bits in e_flags to get the specific
   H8 architecture in use.  */
#define elf_backend_final_write_processing \
  elf32_h8_final_write_processing
#define elf_backend_object_p \
  elf32_h8_object_p
#define bfd_elf32_bfd_merge_private_bfd_data \
  elf32_h8_merge_private_bfd_data

/* ??? when elf_backend_relocate_section is not defined, elf32-target.h
   defaults to using _bfd_generic_link_hash_table_create, but
   elflink.h:bfd_elf32_size_dynamic_sections uses
   dynobj = elf_hash_table (info)->dynobj;
   and thus requires an elf hash table.  */
#define bfd_elf32_bfd_link_hash_table_create _bfd_elf_link_hash_table_create

/* Use an H8 specific linker, not the ELF generic linker.  */
#define elf_backend_relocate_section elf32_h8_relocate_section

#include "elf32-target.h"
