/* V850-specific support for 32-bit ELF
   Copyright (C) 1994, 1995 Free Software Foundation, Inc.

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

static reloc_howto_type *bfd_elf32_bfd_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void v850_info_to_howto_rel
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rel *));
static bfd_reloc_status_type bfd_elf32_v850_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));



/* Try to minimize the amount of space occupied by relocation tables
   on the ROM (not that the ROM won't be swamped by other ELF overhead).  */
#define USE_REL

enum reloc_type
{
  R_V850_NONE = 0,
  R_V850_9_PCREL,
  R_V850_22_PCREL,
  R_V850_HI16_S,
  R_V850_HI16,
  R_V850_LO16,
  R_V850_32,
  R_V850_16,
  R_V850_8,
  R_V850_SDA_OFFSET,
  R_V850_ZDA_OFFSET,
  R_V850_TDA_OFFSET,
  R_V850_max
};

static reloc_howto_type elf_v850_howto_table[] =
{
  /* This reloc does nothing.  */
  HOWTO (R_V850_NONE,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_V850_NONE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A PC relative 9 bit branch. */
  HOWTO (R_V850_9_PCREL,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf32_v850_reloc,	/* special_function */
	 "R_V850_9_PCREL",	/* name */
	 false,			/* partial_inplace */
	 0x00ffffff,		/* src_mask */
	 0x00ffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A PC relative 22 bit branch. */
  HOWTO (R_V850_22_PCREL,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 22,			/* bitsize */
	 true,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf32_v850_reloc,	/* special_function */
	 "R_V850_22_PCREL",	/* name */
	 false,			/* partial_inplace */
	 0x07ffff80,		/* src_mask */
	 0x07ffff80,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* High 16 bits of symbol value.  */
  HOWTO (R_V850_HI16_S,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,		       /* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf32_v850_reloc, /* special_function */
	 "R_V850_HI16_S",	/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* High 16 bits of symbol value.  */
  HOWTO (R_V850_HI16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,		       /* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf32_v850_reloc, /* special_function */
	 "R_V850_HI16",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Low 16 bits of symbol value.  */
  HOWTO (R_V850_LO16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_V850_LO16",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Simple 32bit reloc.  */
  HOWTO (R_V850_32,	      /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_V850_32",	      /* name */
	 true,			/* partial_inplace */
	 0xffffffff,		    /* src_mask */
	 0xffffffff,		    /* dst_mask */
	 false),		/* pcrel_offset */

  /* Simple 16bit reloc.  */
  HOWTO (R_V850_16,	      /* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_V850_16",	      /* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Simple 8bit reloc.	 */
  HOWTO (R_V850_8,	     /* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,		       /* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_V850_8",	     /* name */
	 true,			/* partial_inplace */
	 0xff,		      /* src_mask */
	 0xff,		      /* dst_mask */
	 false),		/* pcrel_offset */

  /* Offset from the short data area pointer.  */
  HOWTO (R_V850_SDA_OFFSET,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_V850_SDA_OFFSET",	/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Offset from the zero data area pointer.  */
  HOWTO (R_V850_ZDA_OFFSET,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_V850_ZDA_OFFSET",	/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Offset from the tiny data area pointer.  */
  HOWTO (R_V850_TDA_OFFSET,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_V850_TDA_OFFSET",	/* name */
	 true,			/* partial_inplace */
	 0xff,			/* src_mask */
	 0xff,			/* dst_mask */
	 false),		/* pcrel_offset */

};

/* Map BFD reloc types to V850 ELF reloc types.  */

struct v850_reloc_map
{
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct v850_reloc_map v850_reloc_map[] =
{
  { BFD_RELOC_NONE, R_V850_NONE, },
  { BFD_RELOC_V850_9_PCREL, R_V850_9_PCREL, },
  { BFD_RELOC_V850_22_PCREL, R_V850_22_PCREL, },
  { BFD_RELOC_HI16_S, R_V850_HI16_S, },
  { BFD_RELOC_HI16, R_V850_HI16, },
  { BFD_RELOC_LO16, R_V850_LO16, },
  { BFD_RELOC_32, R_V850_32, },
  { BFD_RELOC_16, R_V850_16, },
  { BFD_RELOC_8, R_V850_8, },
  { BFD_RELOC_V850_TDA_OFFSET, R_V850_TDA_OFFSET, },
  { BFD_RELOC_V850_SDA_OFFSET, R_V850_SDA_OFFSET, },
  { BFD_RELOC_V850_ZDA_OFFSET, R_V850_ZDA_OFFSET, },
};

static reloc_howto_type *
bfd_elf32_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = 0;
       i < sizeof (v850_reloc_map) / sizeof (struct v850_reloc_map);
       i++)
    {
      if (v850_reloc_map[i].bfd_reloc_val == code)
	return &elf_v850_howto_table[v850_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}

/* Set the howto pointer for an V850 ELF reloc.  */

static void
v850_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf32_Internal_Rel *dst;
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_V850_max);
  cache_ptr->howto = &elf_v850_howto_table[r_type];
}

static bfd_reloc_status_type
bfd_elf32_v850_reloc (abfd, reloc, symbol, data, isection, obfd, err)
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

      if (reloc->howto->type == R_V850_22_PCREL)
	{
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
	}
      else if (reloc->howto->type == R_V850_9_PCREL)
	{
	  if (relocation > 0xff || relocation < -0x100)
	    return bfd_reloc_overflow;

	  if ((relocation % 2) != 0)
	    return bfd_reloc_dangerous;

	  insn = bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
	  insn &= 0xf870;
	  insn |= ((relocation & 0x1f0) << 7) | ((relocation & 0x0e) << 3);
	  bfd_put_16 (abfd, insn, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;
	}
      else if (reloc->howto->type == R_V850_HI16_S)
	{
	  relocation += bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
	  relocation = (relocation >> 16) + ((relocation & 0x8000) != 0);
	  bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;
	}
      else if (reloc->howto->type == R_V850_HI16)
	{
	  relocation += bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
	  relocation = (relocation >> 16);
	  bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;
	}
      else
	return bfd_reloc_notsupported;
    }

  return bfd_reloc_continue;
}

static boolean bfd_elf32_v850_is_local_label PARAMS ((bfd *, asymbol *));

/*ARGSUSED*/
static boolean
bfd_elf32_v850_is_local_label (abfd, symbol)
     bfd *abfd;
     asymbol *symbol;
{
  return ((symbol->name[0] == '.' && (symbol->name[1] == 'L' || symbol->name[1] == '.'))
	  || (symbol->name[0] == '_' && symbol->name[1] == '.' && symbol->name[2] == 'L'
	      && symbol->name[3] == '_'));
}

/* Perform a relocation as part of a final link.  */
static bfd_reloc_status_type
elf32_v850_bfd_final_link_relocate (howto, input_bfd, output_bfd,
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
  unsigned long r_format = howto->bitsize;
  bfd_byte *hit_data = contents + offset;
  boolean r_pcrel = howto->pc_relative;

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
    default:
      break;
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

      r_type = ELF32_R_TYPE (rel->r_info);

      howto = elf_v850_howto_table + r_type;

      r_symndx = ELF32_R_SYM (rel->r_info);

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
      r = elf32_v850_bfd_final_link_relocate (howto, input_bfd, output_bfd,
					      input_section,
					      contents, rel->r_offset,
					      relocation, rel->r_addend,
					      info, sec, h == NULL);

      if (r != bfd_reloc_ok)
	{
	  switch (r)
	    {
	    default:
	    case bfd_reloc_outofrange:
	      abort ();
	    case bfd_reloc_overflow:
	      {
		const char *name;

		if (h != NULL)
		  name = h->root.root.string;
		else
		  {
		    name = (bfd_elf_string_from_elf_section
			    (input_bfd, symtab_hdr->sh_link, sym->st_name));
		    if (name == NULL)
		      return false;
		    if (*name == '\0')
		      name = bfd_section_name (input_bfd, sec);
		  }
		if (! ((*info->callbacks->reloc_overflow)
		       (info, name, howto->name, (bfd_vma) 0,
			input_bfd, input_section, rel->r_offset)))
		  return false;
	      }
	      break;
	    }
	}
    }

  return true;
}
#define bfd_elf32_bfd_is_local_label	bfd_elf32_v850_is_local_label

#define TARGET_LITTLE_SYM		bfd_elf32_v850_vec
#define TARGET_LITTLE_NAME		"elf32-v850"
#define ELF_ARCH		bfd_arch_v850
#define ELF_MACHINE_CODE	EM_CYGNUS_V850
#define ELF_MAXPAGESIZE		0x1000

#define elf_info_to_howto	0
#define elf_info_to_howto_rel	v850_info_to_howto_rel
#define elf_backend_relocate_section    v850_elf_relocate_section


#define elf_symbol_leading_char '_'

#include "elf32-target.h"
