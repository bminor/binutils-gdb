/* V850-specific support for 32-bit ELF
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.

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



/* XXX FIXME: This code is littered with 32bit int, 16bit short, 8bit char
   dependencies.  As is the gas & simulator code or the v850.  */


#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/v850.h"

static reloc_howto_type *v850_elf_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void v850_elf_info_to_howto_rel
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rel *));
static bfd_reloc_status_type v850_elf_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));
static boolean v850_elf_is_local_label_name PARAMS ((bfd *, const char *));
static boolean v850_elf_relocate_section PARAMS((bfd *,
						 struct bfd_link_info *,
						 bfd *,
						 asection *,
						 bfd_byte *,
						 Elf_Internal_Rela *,
						 Elf_Internal_Sym *,
						 asection **));

/* Try to minimize the amount of space occupied by relocation tables
   on the ROM (not that the ROM won't be swamped by other ELF overhead).  */
#define USE_REL

static reloc_howto_type v850_elf_howto_table[] =
{
  /* This reloc does nothing.  */
  HOWTO (R_V850_NONE,			/* type */
	 0,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_V850_NONE",			/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* A PC relative 9 bit branch. */
  HOWTO (R_V850_9_PCREL,		/* type */
	 2,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 26,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_9_PCREL",		/* name */
	 false,				/* partial_inplace */
	 0x00ffffff,			/* src_mask */
	 0x00ffffff,			/* dst_mask */
	 true),				/* pcrel_offset */

  /* A PC relative 16 bit load/store. */
  HOWTO (R_V850_16_PCREL,		/* type */
	 0,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 15,				/* bitsize */
	 true,				/* pc_relative */
	 17,				/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_16_PCREL",		/* name */
	 false,				/* partial_inplace */
	 0xfffe0000,			/* src_mask */
	 0xfffe0000,			/* dst_mask */
	 true),				/* pcrel_offset */

  /* A PC relative 22 bit branch. */
  HOWTO (R_V850_22_PCREL,		/* type */
	 2,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 22,				/* bitsize */
	 true,				/* pc_relative */
	 7,				/* bitpos */
	 complain_overflow_signed,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_22_PCREL",		/* name */
	 false,				/* partial_inplace */
	 0x07ffff80,			/* src_mask */
	 0x07ffff80,			/* dst_mask */
	 true),				/* pcrel_offset */

  /* High 16 bits of symbol value.  */
  HOWTO (R_V850_HI16_S,			/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_HI16_S",		/* name */
	 true,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* High 16 bits of symbol value.  */
  HOWTO (R_V850_HI16,			/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_HI16",			/* name */
	 true,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Low 16 bits of symbol value.  */
  HOWTO (R_V850_LO16,			/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_LO16",			/* name */
	 true,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Simple 32bit reloc.  */
  HOWTO (R_V850_32,			/* type */
	 0,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_V850_32",			/* name */
	 true,				/* partial_inplace */
	 0xffffffff,			/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Simple 16bit reloc.  */
  HOWTO (R_V850_16,			/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_V850_16",			/* name */
	 true,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Simple 8bit reloc.	 */
  HOWTO (R_V850_8,			/* type */
	 0,				/* rightshift */
	 0,				/* size (0 = byte, 1 = short, 2 = long) */
	 8,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_V850_8",			/* name */
	 true,				/* partial_inplace */
	 0xff,				/* src_mask */
	 0xff,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* Offset from the short data area pointer.  */
  HOWTO (R_V850_SDA_OFFSET,		/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_SDA_OFFSET",		/* name */
	 true,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Offset from the zero data area pointer.  */
  HOWTO (R_V850_ZDA_OFFSET,		/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_ZDA_OFFSET",		/* name */
	 true,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Offset from the tiny data area pointer.  */
  HOWTO (R_V850_TDA_OFFSET,		/* type */
	 0,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 8,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_TDA_OFFSET",		/* name */
	 true,				/* partial_inplace */
	 0xff,				/* src_mask */
	 0xff,				/* dst_mask */
	 false),			/* pcrel_offset */

};

/* Map BFD reloc types to V850 ELF reloc types.  */

struct v850_elf_reloc_map
{
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct v850_elf_reloc_map v850_elf_reloc_map[] =
{
  { BFD_RELOC_NONE,		R_V850_NONE, },
  { BFD_RELOC_V850_9_PCREL,	R_V850_9_PCREL, },
  { BFD_RELOC_V850_16_PCREL,	R_V850_16_PCREL, },
  { BFD_RELOC_V850_22_PCREL,	R_V850_22_PCREL, },
  { BFD_RELOC_HI16_S,		R_V850_HI16_S, },
  { BFD_RELOC_HI16,		R_V850_HI16, },
  { BFD_RELOC_LO16,		R_V850_LO16, },
  { BFD_RELOC_32,		R_V850_32, },
  { BFD_RELOC_16,		R_V850_16, },
  { BFD_RELOC_8,		R_V850_8, },
  { BFD_RELOC_V850_TDA_OFFSET,	R_V850_TDA_OFFSET, },
  { BFD_RELOC_V850_SDA_OFFSET,	R_V850_SDA_OFFSET, },
  { BFD_RELOC_V850_ZDA_OFFSET,	R_V850_ZDA_OFFSET, },
};


/* Map a bfd relocation into the appropriate howto structure */
static reloc_howto_type *
v850_elf_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = 0;
       i < sizeof (v850_elf_reloc_map) / sizeof (struct v850_elf_reloc_map);
       i++)
    {
      if (v850_elf_reloc_map[i].bfd_reloc_val == code)
	return &v850_elf_howto_table[v850_elf_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}


/* Set the howto pointer for an V850 ELF reloc.  */
static void
v850_elf_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf32_Internal_Rel *dst;
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_V850_max);
  cache_ptr->howto = &v850_elf_howto_table[r_type];
}


/* Look through the relocs for a section during the first phase, and
   allocate space in the global offset table or procedure linkage
   table.  */

static boolean
v850_elf_check_relocs (abfd, info, sec, relocs)
     bfd *abfd;
     struct bfd_link_info *info;
     asection *sec;
     const Elf_Internal_Rela *relocs;
{
  boolean ret = true;
  bfd *dynobj;
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;
  asection *sreloc;
  enum reloc_type r_type;
  int other = 0;
  const char *common = (const char *)0;

  if (info->relocateable)
    return true;

#ifdef DEBUG
  fprintf (stderr, "v850_elf_check_relocs called for section %s in %s\n",
	   bfd_get_section_name (abfd, sec),
	   bfd_get_filename (abfd));
#endif

  dynobj = elf_hash_table (info)->dynobj;
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  sreloc = NULL;

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      unsigned long r_symndx;
      struct elf_link_hash_entry *h;

      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	h = sym_hashes[r_symndx - symtab_hdr->sh_info];

      r_type = (enum reloc_type) ELF32_R_TYPE (rel->r_info);
      switch (r_type)
	{
	default:
	case R_V850_NONE:
	case R_V850_9_PCREL:
	case R_V850_16_PCREL:
	case R_V850_22_PCREL:
	case R_V850_HI16_S:
	case R_V850_HI16:
	case R_V850_LO16:
	case R_V850_32:
	case R_V850_16:
	case R_V850_8:
	  break;

	case R_V850_SDA_OFFSET:
	  other = V850_OTHER_SDA;
	  common = ".scommon";
	  goto small_data_common;

	case R_V850_ZDA_OFFSET:
	  other = V850_OTHER_ZDA;
	  common = ".zcommon";
	  goto small_data_common;

	case R_V850_TDA_OFFSET:
	  other = V850_OTHER_TDA;
	  common = ".tcommon";
	  /* fall through */

#define V850_OTHER_MASK (V850_OTHER_TDA | V850_OTHER_SDA | V850_OTHER_ZDA)

	small_data_common:
	  if (h)
	    {
	      h->other |= other;	/* flag which type of relocation was used */
	      if ((h->other & V850_OTHER_MASK) != (other & V850_OTHER_MASK)
		  && (h->other & V850_OTHER_ERROR) == 0)
		{
		  const char *msg;

		  switch (h->other & V850_OTHER_MASK)
		    {
		    default:
		      msg = "Variable cannot occupy in multiple small data regions";
		      break;
		    case V850_OTHER_SDA | V850_OTHER_ZDA | V850_OTHER_TDA:
		      msg = "Variable can only be in one of the small, zero, and tiny data regions";
		      break;
		    case V850_OTHER_SDA | V850_OTHER_ZDA:
		      msg = "Variable cannot be in both small and zero data regions simultaneously";
		      break;
		    case V850_OTHER_SDA | V850_OTHER_TDA:
		      msg = "Variable cannot be in both small and tiny data regions simultaneously";
		      break;
		    case V850_OTHER_ZDA | V850_OTHER_TDA:
		      msg = "Variable cannot be in both zero and tiny data regions simultaneously";
		      break;
		    }

		  (*info->callbacks->warning) (info, msg, h->root.root.string,
					       abfd, h->root.u.def.section, 0);

		  bfd_set_error (bfd_error_bad_value);
		  h->other |= V850_OTHER_ERROR;
		  ret = false;
		}
	    }

	  if (h && h->root.type == bfd_link_hash_common
	      && h->root.u.c.p
	      && !strcmp (bfd_get_section_name (abfd, h->root.u.c.p->section), "COMMON"))
	    {
	      asection *section = h->root.u.c.p->section = bfd_make_section_old_way (abfd, common);
	      section->flags |= SEC_IS_COMMON;
	    }

#ifdef DEBUG
	  fprintf (stderr, "v850_elf_check_relocs, found %s relocation for %s%s\n",
		   v850_elf_howto_table[ (int)r_type ].name,
		   (h && h->root.root.string) ? h->root.root.string : "<unknown>",
		   (h->root.type == bfd_link_hash_common) ? ", symbol is common" : "");
#endif
	  break;
	}
    }

  return ret;
}


static bfd_reloc_status_type
v850_elf_reloc (abfd, reloc, symbol, data, isection, obfd, err)
     bfd *abfd;
     arelent *reloc;
     asymbol *symbol;
     PTR data;
     asection *isection;
     bfd *obfd;
     char **err;
{
  if (obfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (! reloc->howto->partial_inplace
	  || reloc->addend == 0))
    {
      reloc->address += isection->output_offset;
      return bfd_reloc_ok;
    }
  else if (obfd != NULL)
    {
      return bfd_reloc_continue;
    }

  /* Catch relocs involving undefined symbols.  */
  if (bfd_is_und_section (symbol->section)
      && (symbol->flags & BSF_WEAK) == 0
      && obfd == NULL)
    return bfd_reloc_undefined;

  /* We handle final linking of some relocs ourselves.  */
    {
      long relocation, insn;

      /* Is the address of the relocation really within the section?  */
      if (reloc->address > isection->_cooked_size)
	return bfd_reloc_outofrange;

      /* Work out which section the relocation is targetted at and the
	 initial relocation command value.  */

      /* Get symbol value.  (Common symbols are special.)  */
      if (bfd_is_com_section (symbol->section))
	relocation = 0;
      else
	relocation = symbol->value;

      /* Convert input-section-relative symbol value to absolute + addend.  */
      relocation += symbol->section->output_section->vma;
      relocation += symbol->section->output_offset;
      relocation += reloc->addend;

      if (reloc->howto->pc_relative == true)
	{
	  /* Here the variable relocation holds the final address of the
	     symbol we are relocating against, plus any addend.  */
	  relocation -= isection->output_section->vma + isection->output_offset;

	  /* Deal with pcrel_offset */
	  relocation -= reloc->address;
	}

      /* I've got no clue... */
      reloc->addend = 0;	

      switch (reloc->howto->type)
	{
	default:
	  return bfd_reloc_notsupported;

	case R_V850_22_PCREL:
	  if (relocation > 0x1ffff || relocation < -0x200000)
	    return bfd_reloc_overflow;

	  if ((relocation % 2) != 0)
	    return bfd_reloc_dangerous;

	  insn = bfd_get_32 (abfd, (bfd_byte *) data + reloc->address);
	  insn &= ~0xfffe003f;
	  insn |= (((relocation & 0xfffe) << 16)
		   | ((relocation & 0x3f0000) >> 16));
	  bfd_put_32 (abfd, insn, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;

	case R_V850_16_PCREL:
	  if (relocation > 0x7fff || relocation < -0x10000)
	    return bfd_reloc_overflow;

	  if ((relocation % 2) != 0)
	    return bfd_reloc_dangerous;

	  insn = bfd_get_32 (abfd, (bfd_byte *) data + reloc->address);
	  insn &= 0x1ffff;
	  insn |= ((relocation & 0xfffe) << 16);
	  bfd_put_32 (abfd, insn, (bfd_byte *) data + reloc->address);
	  return bfd_reloc_ok;

	case R_V850_9_PCREL:
	  if (relocation > 0xff || relocation < -0x100)
	    return bfd_reloc_overflow;

	  if ((relocation % 2) != 0)
	    return bfd_reloc_dangerous;

	  insn = bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
	  insn &= ~ 0xf870;
	  insn |= ((relocation & 0x1f0) << 7) | ((relocation & 0x0e) << 3);
	  bfd_put_16 (abfd, insn, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;

	case R_V850_HI16_S:
	  relocation += bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
	  relocation = (relocation >> 16) + ((relocation & 0x8000) != 0);
	  bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;

	case R_V850_HI16:
	  relocation += bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
	  relocation = (relocation >> 16);
	  bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;

	case R_V850_16:
	case R_V850_ZDA_OFFSET:
	  relocation += (short)bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);

	  if ((long)relocation > 0x7fff || (long)relocation < -0x8000)
	    return bfd_reloc_overflow;

	  bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;

#if 0
	case R_V850_SDA_OFFSET:
	  {
	    unsigned long gp;
	    struct bfd_link_hash_entry *h;

	    relocation += (short)bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);

	    /* Get the value of __gp.  */
	    h = bfd_link_hash_lookup (info->hash, "__gp", false, false, true);
	    if (h == (struct bfd_link_hash_entry *) NULL
		|| h->type != bfd_link_hash_defined)
	      return bfd_reloc_undefined;

	    gp = (h->u.def.value
		  + h->u.def.section->output_section->vma
		  + h->u.def.section->output_offset);
	    relocation -= gp;

	    if ((long)relocation > 0x7fff || (long)relocation < -0x8000)
	      return bfd_reloc_overflow;

	    bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
	    return bfd_reloc_ok;
	  }
#endif
	}
    }

  return bfd_reloc_continue;
}


/*ARGSUSED*/
static boolean
v850_elf_is_local_label_name (abfd, name)
     bfd *abfd;
     const char *name;
{
  return ((name[0] == '.' && (name[1] == 'L' || name[1] == '.'))
	  || (name[0] == '_' && name[1] == '.' && name[2] == 'L'
	      && name[3] == '_'));
}


/* Perform a relocation as part of a final link.  */
static bfd_reloc_status_type
v850_elf_final_link_relocate (howto, input_bfd, output_bfd,
				    input_section, contents, offset, value,
				    addend, info, sym_sec, is_local)
     reloc_howto_type *howto;
     bfd *input_bfd;
     bfd *output_bfd;
     asection *input_section;
     bfd_byte *contents;
     bfd_vma offset;
     bfd_vma value;
     bfd_vma addend;
     struct bfd_link_info *info;
     asection *sym_sec;
     int is_local;
{
  unsigned long insn;
  unsigned long r_type = howto->type;
  bfd_byte *hit_data = contents + offset;

  switch (r_type)
    {
    case R_V850_9_PCREL:
      value -= (input_section->output_section->vma
		+ input_section->output_offset);
      value -= offset;

      if ((long)value > 0xff || (long)value < -0x100)
	return bfd_reloc_overflow;

      if ((value % 2) != 0)
	return bfd_reloc_dangerous;

      insn = bfd_get_16 (input_bfd, hit_data);
      insn &= 0x078f;
      insn |= ((value & 0x1f0) << 7) | ((value & 0x0e) << 3);
      bfd_put_16 (input_bfd, insn, hit_data);
      return bfd_reloc_ok;
    
    case R_V850_16_PCREL:
      value -= (input_section->output_section->vma + input_section->output_offset);
      value -= offset;

      if ((long)value > 0x7fff || (long)value < -0x10000)
	return bfd_reloc_overflow;
      
      if ((value % 2) != 0)
	return bfd_reloc_dangerous;
      
      insn = bfd_get_32 (input_bfd, hit_data);
      insn &= 0x1ffff;
      insn |= ((value & 0xfffe) << 16);
      bfd_put_32 (input_bfd, insn, hit_data);
      return bfd_reloc_ok;
      
    case R_V850_22_PCREL:
      value -= (input_section->output_section->vma
		+ input_section->output_offset);
      value -= offset;
    
      if ((long)value > 0x1ffff || (long)value < -0x200000)
	return bfd_reloc_overflow;

      if ((value % 2) != 0)
	return bfd_reloc_dangerous;

      insn = bfd_get_32 (input_bfd, hit_data);
      insn &= 0x1ffc0;
      insn |= (((value & 0xfffe) << 16) | ((value & 0x3f0000) >> 16));
      bfd_put_32 (input_bfd, insn, hit_data);
      return bfd_reloc_ok;
      
    case R_V850_HI16_S:
      value += (short)bfd_get_16 (input_bfd, hit_data);
      value = (value >> 16) + ((value & 0x8000) != 0);

      if ((long)value > 0x7fff || (long)value < -0x8000)
	return bfd_reloc_overflow;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_HI16:
      value += (short)bfd_get_16 (input_bfd, hit_data);
      value >>= 16;

      if ((long)value > 0x7fff || (long)value < -0x8000)
	return bfd_reloc_overflow;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_LO16:
      value += (short)bfd_get_16 (input_bfd, hit_data);
      value &= 0xffff;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_16:
    case R_V850_ZDA_OFFSET:
      value += (short)bfd_get_16 (input_bfd, hit_data);

      if ((long)value > 0x7fff || (long)value < -0x8000)
	return bfd_reloc_overflow;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_32:
      value += bfd_get_32 (input_bfd, hit_data);
      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_8:
      value += (char)bfd_get_8 (input_bfd, hit_data);

      if ((long)value > 0x7f || (long)value < -0x80)
	return bfd_reloc_overflow;

      bfd_put_8 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_SDA_OFFSET:
      {
	unsigned long gp;
	struct bfd_link_hash_entry *h;

	value += (short)bfd_get_16 (input_bfd, hit_data);

	/* Get the value of __gp.  */
	h = bfd_link_hash_lookup (info->hash, "__gp", false,
				  false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
          return bfd_reloc_undefined;

	gp = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);
	value -= gp;

	if ((long)value > 0x7fff || (long)value < -0x8000)
	  return bfd_reloc_overflow;

	bfd_put_16 (input_bfd, value, hit_data);
	return bfd_reloc_ok;
      }

    case R_V850_TDA_OFFSET:
      {
	unsigned long ep;
	struct bfd_link_hash_entry *h;

	insn = bfd_get_16 (input_bfd, hit_data);

	/* Get the value of __ep.  */
	h = bfd_link_hash_lookup (info->hash, "__ep", false,
				  false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
          return bfd_reloc_undefined;

	ep = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);
	value -= ep;


	/* Overflow computation and operand insertion is complicated
	   by valid offsets and insertions changing depending on the
	   instruction being used!  */
	if ((insn & 0x0780) == 0x0500)
	  {
	    value += ((insn & 0x7f) << 1);

	    /* Handle sld.w and sst.w -- 8 bit unsigned offset */
	    if ((long) value > 0xff || (long) value < 0)
	      return bfd_reloc_overflow;

	    if ((value % 2) != 0)
	      return bfd_reloc_dangerous;

	    insn &= 0xff81;
	    insn |= (value >> 1);
	    bfd_put_16 (input_bfd, insn, hit_data);
	    return bfd_reloc_ok;
	  }

	if ((insn & 0x0780) == 0x0400 || (insn & 0x0780) == 0x0480)
	  {
	    value += ((insn & 0x7f) << 1);

	    /* Handle sld.h and sst.h -- 8 bit unsigned offset */
	    if ((long) value > 0xff || (long) value < 0)
	      return bfd_reloc_overflow;

	    if ((value % 2) != 0)
	      return bfd_reloc_dangerous;

	    insn &= 0xff80;
	    insn |= (value >> 1);
	    bfd_put_16 (input_bfd, insn, hit_data);
	    return bfd_reloc_ok;
	  }

	if ((insn & 0x0780) == 0x0300 || (insn & 0x0780) == 0x0380)
	  {
	    value += (insn & 0x7f);

	    /* Handle sld.b and sst.b -- 7 bit unsigned offset */
	    if ((long) value > 0x7f || (long) value < 0)
	      return bfd_reloc_overflow;
	    insn &= 0xff80;
	    insn |= value;
	    bfd_put_16 (input_bfd, insn, hit_data);
	    return bfd_reloc_ok;
	  }

	/* Guess (XXX) that it's a movea instruction or something
	   similar.  */
	value += (short)insn;
        if ((long)value > 0x7fff || (long)value < -0x8000)
          return bfd_reloc_overflow;

        bfd_put_16 (input_bfd, value, hit_data);
        return bfd_reloc_ok;
      }
    
    case R_V850_NONE:
      return bfd_reloc_ok;

    default:
      return bfd_reloc_notsupported;
    }
}


/* Relocate an V850 ELF section.  */
static boolean
v850_elf_relocate_section (output_bfd, info, input_bfd, input_section,
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
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;

      r_symndx = ELF32_R_SYM (rel->r_info);
      r_type = ELF32_R_TYPE (rel->r_info);
      howto = v850_elf_howto_table + r_type;

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
		      input_section, rel->r_offset)))
		return false;
	      relocation = 0;
	    }
	}

      /* FIXME: We should use the addend, but the COFF relocations
         don't.  */
      r = v850_elf_final_link_relocate (howto, input_bfd, output_bfd,
					input_section,
					contents, rel->r_offset,
					relocation, rel->r_addend,
					info, sec, h == NULL);

      if (r != bfd_reloc_ok)
	{
	  const char *name;
	  const char *msg = (const char *)0;

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
		      rel->r_offset)))
		return false;
	      break;

	    case bfd_reloc_outofrange:
	      msg = "internal error: out of range error";
	      goto common_error;

	    case bfd_reloc_notsupported:
	      msg = "internal error: unsupported relocation error";
	      goto common_error;

	    case bfd_reloc_dangerous:
	      msg = "internal error: dangerous error";
	      goto common_error;

	    default:
	      msg = "internal error: unknown error";
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

/* Set the right machine number.  */

static boolean
v850_elf_object_p (abfd)
     bfd *abfd;
{
  switch (get_elf_backend_data (abfd) -> elf_machine_code)
    {
    default:
    case EM_CYGNUS_V850:   (void) bfd_default_set_arch_mach (abfd, bfd_arch_v850, 0); break;
/* start-sanitize-v850e */
    case EM_CYGNUS_V850E:  (void) bfd_default_set_arch_mach (abfd, bfd_arch_v850, bfd_mach_v850e); break;
/* end-sanitize-v850e */
/* start-sanitize-v850eq */
    case EM_CYGNUS_V850EQ: (void) bfd_default_set_arch_mach (abfd, bfd_arch_v850, bfd_mach_v850eq); break;
/* start-sanitize-v850eq */
    }
}


#define TARGET_LITTLE_SYM			bfd_elf32_v850_vec
#define TARGET_LITTLE_NAME			"elf32-v850"
#define ELF_ARCH				bfd_arch_v850
#define ELF_MACHINE_CODE			EM_CYGNUS_V850
/* start-sanitize-v850e */
#undef  ELF_MACHINE_CODE
#define ELF_MACHINE_CODE			EM_CYGNUS_V850E
/* end-sanitize-v850e */
/* start-sanitize-v850eq */
#undef  ELF_MACHINE_CODE
#define ELF_MACHINE_CODE			EM_CYGNUS_V850EQ
/* end-sanitize-v850eq */

#define ELF_MAXPAGESIZE				0x1000
	
#define elf_info_to_howto			0
#define elf_info_to_howto_rel			v850_elf_info_to_howto_rel
#define elf_backend_check_relocs		v850_elf_check_relocs
#define elf_backend_relocate_section    	v850_elf_relocate_section
#define elf_backend_object_p			v850_elf_object_p
#define bfd_elf32_bfd_is_local_label_name	v850_elf_is_local_label_name
#define bfd_elf32_bfd_reloc_type_lookup		v850_elf_reloc_type_lookup

#define elf_symbol_leading_char			'_'

#include "elf32-target.h"
