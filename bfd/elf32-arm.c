/* start-sanitize-armelf */
/* 32-bit ELF support for ARM
   Copyright 1993, 1995, 1998 Free Software Foundation, Inc.

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

#include "elf/arm.h"

static reloc_howto_type *elf32_arm_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void elf32_arm_info_to_howto
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rela *));
static boolean elf32_arm_set_private_flags
  PARAMS ((bfd *, flagword));
static boolean elf32_arm_copy_private_bfd_data
  PARAMS ((bfd *, bfd *));
static boolean elf32_arm_merge_private_bfd_data
  PARAMS ((bfd *, bfd *));
static boolean elf32_arm_print_private_bfd_data
  PARAMS ((bfd *, PTR));

#define USE_RELA
#define TARGET_UNDERSCORE '_'

static reloc_howto_type elf32_arm_howto_table[] =
{
  /* No relocation */
  HOWTO (R_ARM_NONE,               /* type */
	 0,                        /* rightshift */
	 0,                        /* size (0 = byte, 1 = short, 2 = long) */
	 0,                        /* bitsize */
	 false,                    /* pc_relative */
	 0,                        /* bitpos */
	 complain_overflow_dont,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,    /* special_function */
	 "R_ARM_NONE",             /* name */
	 false,                    /* partial_inplace */
	 0,                        /* src_mask */
	 0,                        /* dst_mask */
	 false),                   /* pcrel_offset */

  HOWTO (R_ARM_PC24,               /* type */
	 2,                        /* rightshift */
	 2,                        /* size (0 = byte, 1 = short, 2 = long) */
	 24,                       /* bitsize */
	 true,                     /* pc_relative */
	 0,                        /* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,    /* special_function */
	 "R_ARM_PC24",             /* name */
	 false,                    /* partial_inplace */
	 0x00ffffff,               /* src_mask */
	 0x00ffffff,               /* dst_mask */
	 true),                    /* pcrel_offset */

  /* 32 bit absolute */
  HOWTO (R_ARM_ABS32,                  /* type */
	 0,                            /* rightshift */
	 2,                            /* size (0 = byte, 1 = short, 2 = long) */
	 32,                           /* bitsize */
	 false,                        /* pc_relative */
	 0,                            /* bitpos */
	 complain_overflow_bitfield,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,        /* special_function */
	 "R_ARM_ABS32",                /* name */
	 false,                        /* partial_inplace */
	 0xffffffff,                   /* src_mask */
	 0xffffffff,                   /* dst_mask */
	 false),                        /* pcrel_offset */

  /* standard 32bit pc-relative reloc */
  HOWTO (R_ARM_REL32,                  /* type */
	 0,                            /* rightshift */
	 2,                            /* size (0 = byte, 1 = short, 2 = long) */
	 32,                           /* bitsize */
	 true,                         /* pc_relative */
	 0,                            /* bitpos */
	 complain_overflow_bitfield,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,        /* special_function */
	 "R_ARM_REL32",                /* name */
	 false,                        /* partial_inplace */
	 0xffffffff,                   /* src_mask */
	 0xffffffff,                   /* dst_mask */
	 true),                        /* pcrel_offset */

  /* 8 bit absolute */
  HOWTO (R_ARM_ABS8,                   /* type */
	 0,                            /* rightshift */
	 0,                            /* size (0 = byte, 1 = short, 2 = long) */
	 8,                            /* bitsize */
	 false,                        /* pc_relative */
	 0,                            /* bitpos */
	 complain_overflow_bitfield,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,        /* special_function */
	 "R_ARM_ABS8",                 /* name */
	 false,                        /* partial_inplace */
	 0x000000ff,                   /* src_mask */
	 0x000000ff,                   /* dst_mask */
	 false),                       /* pcrel_offset */

   /* 16 bit absolute */
  HOWTO (R_ARM_ABS16,                  /* type */
	 0,                            /* rightshift */
	 1,                            /* size (0 = byte, 1 = short, 2 = long) */
	 16,                            /* bitsize */
	 false,                        /* pc_relative */
	 0,                            /* bitpos */
	 complain_overflow_bitfield,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,        /* special_function */
	 "R_ARM_ABS16",                /* name */
	 false,                        /* partial_inplace */
	 0,                            /* src_mask */
	 0,                            /* dst_mask */
	 false),                       /* pcrel_offset */

  /* 12 bit absolute */
  HOWTO (R_ARM_ABS12,                  /* type */
	 0,                            /* rightshift */
	 2,                            /* size (0 = byte, 1 = short, 2 = long) */
	 12,                            /* bitsize */
	 false,                        /* pc_relative */
	 0,                            /* bitpos */
	 complain_overflow_bitfield,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,        /* special_function */
	 "R_ARM_ABS12",                /* name */
	 false,                        /* partial_inplace */
	 0x000008ff,                   /* src_mask */
	 0x000008ff,                   /* dst_mask */
	 false),                       /* pcrel_offset */

  HOWTO (R_ARM_THM_ABS5,               /* type */
	 0,                            /* rightshift */
	 2,                            /* size (0 = byte, 1 = short, 2 = long) */
	 5,                            /* bitsize */
	 false,                        /* pc_relative */
	 0,                            /* bitpos */
	 complain_overflow_bitfield,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,        /* special_function */
	 "R_ARM_THM_ABS5",             /* name */
	 false,                        /* partial_inplace */
	 0x000007e0,                   /* src_mask */
	 0x000007e0,                   /* dst_mask */
	 false),                       /* pcrel_offset */

  HOWTO (R_ARM_THM_PC22,           /* type */
	 1,                        /* rightshift */
	 2,                        /* size (0 = byte, 1 = short, 2 = long) */
	 22,                       /* bitsize */
	 true,                     /* pc_relative */
	 0,                        /* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,    /* special_function */
	 "R_ARM_THM_PC22",         /* name */
	 false,                    /* partial_inplace */
	 0x07ff07ff,               /* src_mask */
	 0x07ff07ff,               /* dst_mask */
	 true),                    /* pcrel_offset */

  HOWTO (R_ARM_SBREL32,            /* type */
	 0,                        /* rightshift */
	 0,                        /* size (0 = byte, 1 = short, 2 = long) */
	 0,                        /* bitsize */
	 false,                    /* pc_relative */
	 0,                        /* bitpos */
	 complain_overflow_dont,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,    /* special_function */
	 "R_ARM_SBREL32",          /* name */
	 false,                    /* partial_inplace */
	 0,                        /* src_mask */
	 0,                        /* dst_mask */
	 false),                   /* pcrel_offset */

  HOWTO (R_ARM_AMP_VCALL9,         /* type */
	 1,                        /* rightshift */
	 1,                        /* size (0 = byte, 1 = short, 2 = long) */
	 8,                        /* bitsize */
	 true,                     /* pc_relative */
	 0,                        /* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,    /* special_function */
	 "R_ARM_AMP_VCALL9",       /* name */
	 false,                    /* partial_inplace */
	 0x000000ff,               /* src_mask */
	 0x000000ff,               /* dst_mask */
	 true),                    /* pcrel_offset */

  /* 12 bit pc relative */
  HOWTO (R_ARM_THM_PC11,               /* type */
	 1,                            /* rightshift */
	 1,                            /* size (0 = byte, 1 = short, 2 = long) */
	 11,                           /* bitsize */
	 true,                         /* pc_relative */
	 0,                            /* bitpos */
	 complain_overflow_signed,     /* complain_on_overflow */
	 bfd_elf_generic_reloc,        /* special_function */
	 "R_ARM_THM_PC11",             /* name */
	 false,                        /* partial_inplace */
	 0x000007ff,                   /* src_mask */
	 0x000007ff,                   /* dst_mask */
	 true),                        /* pcrel_offset */

  /* 12 bit pc relative */
  HOWTO (R_ARM_THM_PC9,                /* type */
	 1,                            /* rightshift */
	 1,                            /* size (0 = byte, 1 = short, 2 = long) */
	 8,                            /* bitsize */
	 true,                         /* pc_relative */
	 0,                            /* bitpos */
	 complain_overflow_signed,     /* complain_on_overflow */
	 bfd_elf_generic_reloc,        /* special_function */
	 "R_ARM_THM_PC9",              /* name */
	 false,                        /* partial_inplace */
	 0x000000ff,                   /* src_mask */
	 0x000000ff,                   /* dst_mask */
	 true),                        /* pcrel_offset */

/* FILL ME IN  (#13-249) */


  HOWTO (R_ARM_RREL32,             /* type */
	 0,                        /* rightshift */
	 0,                        /* size (0 = byte, 1 = short, 2 = long) */
	 0,                        /* bitsize */
	 false,                    /* pc_relative */
	 0,                        /* bitpos */
	 complain_overflow_dont,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,    /* special_function */
	 "R_ARM_RREL32",           /* name */
	 false,                    /* partial_inplace */
	 0,                        /* src_mask */
	 0,                        /* dst_mask */
	 false),                   /* pcrel_offset */

  HOWTO (R_ARM_RABS32,             /* type */
	 0,                        /* rightshift */
	 0,                        /* size (0 = byte, 1 = short, 2 = long) */
	 0,                        /* bitsize */
	 false,                    /* pc_relative */
	 0,                        /* bitpos */
	 complain_overflow_dont,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,    /* special_function */
	 "R_ARM_RABS32",           /* name */
	 false,                    /* partial_inplace */
	 0,                        /* src_mask */
	 0,                        /* dst_mask */
	 false),                   /* pcrel_offset */

  HOWTO (R_ARM_RPC24,              /* type */
	 0,                        /* rightshift */
	 0,                        /* size (0 = byte, 1 = short, 2 = long) */
	 0,                        /* bitsize */
	 false,                    /* pc_relative */
	 0,                        /* bitpos */
	 complain_overflow_dont,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,    /* special_function */
	 "R_ARM_RPC24",            /* name */
	 false,                    /* partial_inplace */
	 0,                        /* src_mask */
	 0,                        /* dst_mask */
	 false),                   /* pcrel_offset */

  HOWTO (R_ARM_RBASE,              /* type */
	 0,                        /* rightshift */
	 0,                        /* size (0 = byte, 1 = short, 2 = long) */
	 0,                        /* bitsize */
	 false,                    /* pc_relative */
	 0,                        /* bitpos */
	 complain_overflow_dont,   /* complain_on_overflow */
	 bfd_elf_generic_reloc,    /* special_function */
	 "R_ARM_RBASE",            /* name */
	 false,                    /* partial_inplace */
	 0,                        /* src_mask */
	 0,                        /* dst_mask */
	 false),                   /* pcrel_offset */

};
struct elf32_arm_reloc_map
{
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct elf32_arm_reloc_map elf32_arm_reloc_map[] =
{
  { BFD_RELOC_NONE,                  R_ARM_NONE, },
  { BFD_RELOC_ARM_PCREL_BRANCH,      R_ARM_PC24, },
  { BFD_RELOC_32,                    R_ARM_ABS32, },
  { BFD_RELOC_32_PCREL,              R_ARM_REL32, },
  { BFD_RELOC_8,                     R_ARM_ABS8, },
  { BFD_RELOC_16,                    R_ARM_ABS16, },
  { BFD_RELOC_ARM_OFFSET_IMM,        R_ARM_ABS12, },
  { BFD_RELOC_ARM_THUMB_OFFSET,      R_ARM_THM_ABS5, },
  { BFD_RELOC_THUMB_PCREL_BRANCH23,  R_ARM_THM_PC22, },
  { BFD_RELOC_NONE,                  R_ARM_SBREL32 , },
  { BFD_RELOC_NONE,                  R_ARM_AMP_VCALL9, },
  { BFD_RELOC_THUMB_PCREL_BRANCH12,  R_ARM_THM_PC11, },
  { BFD_RELOC_THUMB_PCREL_BRANCH9,   R_ARM_THM_PC9, }
};

static reloc_howto_type *
elf32_arm_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  unsigned int i;
 
  for (i = 0;
       i < sizeof (elf32_arm_reloc_map) / sizeof (struct elf32_arm_reloc_map);
       i++)
    {
      if (elf32_arm_reloc_map[i].bfd_reloc_val == code)
        return &elf32_arm_howto_table[elf32_arm_reloc_map[i].elf_reloc_val];
    }
 
  return NULL;
}

static void
elf32_arm_info_to_howto (abfd, bfd_reloc, elf_reloc)
     bfd *abfd;
     arelent *bfd_reloc;
     Elf32_Internal_Rela *elf_reloc;
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (elf_reloc->r_info);
  /* fixme: need range test */
  /* BFD_ASSERT (r_type < (unsigned int) R_ELF32_ARM_MAX); */
  bfd_reloc->howto = &elf32_arm_howto_table[r_type];
}

/* Perform a relocation as part of a final link.  */
static bfd_reloc_status_type
elf32_arm_final_link_relocate (howto, input_bfd, output_bfd,
				 input_section, contents, offset, value,
				 addend, info, sym_sec, sym_flags)
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
     unsigned char sym_flags;
{
  unsigned long r_type = howto->type;
  bfd_byte *hit_data = contents + offset;

  switch (r_type)
    {

    case R_ARM_NONE:
      return bfd_reloc_ok;

    case R_ARM_PC24:
      value -= (input_section->output_offset + offset + 8);
      value += addend;
      value = value >> 2;

      value &= 0xffffff;
      value |= (bfd_get_32 (input_bfd, hit_data) & 0xff000000);
      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_ABS32:
      value += addend;
 
      if (sym_flags == C_THUMBSTATFUNC
          || sym_flags == C_THUMBEXTFUNC)
        value = value | 1;

      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_REL32:
      value -= (input_section->output_section->vma
               + input_section->output_offset);
      value += addend;

      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_ABS8:
      value += addend;
 
      if ((long) value > 0x7f || (long) value < -0x80)
        return bfd_reloc_overflow;
 
      bfd_put_8 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_ABS16:
      value += addend;

      if ((long) value > 0x7fff || (long) value < -0x8000)
        return bfd_reloc_overflow;
 
      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_ABS12:
      /* Support ldr and str instruction for the arm */
      /* Also thumb b (unconditional branch) */
      value += addend;

      if ((long) value > 0x7ff || (long) value < -0x800)
        return bfd_reloc_overflow;

      value |= (bfd_get_32 (input_bfd, hit_data) & 0xfffff000);
      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_THM_ABS5:
      /* Support ldr and str instructions for the thumb. */
      value += addend;

      if ((long) value > 0x1f || (long) value < -0x10)
        return bfd_reloc_overflow;

      value |= bfd_get_16 (input_bfd, hit_data) & 0xf82f;
      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;


    case R_ARM_THM_PC22:
    /* thumb BL (branch long instruction). */
      {
      bfd_vma         relocation;
      boolean         overflow         = false;
      bfd_vma         insn             = bfd_get_32 (input_bfd, hit_data);
      bfd_vma         src_mask         = 0x007FFFFE;
      bfd_signed_vma  reloc_signed_max = (1 << (howto->bitsize - 1)) - 1;
      bfd_signed_vma  reloc_signed_min = ~reloc_signed_max;
      bfd_vma         check;
      bfd_signed_vma  signed_check;
      bfd_vma         add;
      bfd_signed_vma  signed_add;
 
      relocation  = value + addend;
      relocation -= (input_section->output_section->vma + input_section->output_offset);
      relocation -= offset;
 
      check = relocation >> howto->rightshift;
 
      /* If this is a signed value, the rightshift just dropped
         leading 1 bits (assuming twos complement).  */
      if ((bfd_signed_vma) relocation >= 0)
         signed_check = check;
      else
         signed_check = (check | ((bfd_vma) - 1 & ~((bfd_vma) - 1 >> howto->rightshift)));
 
      /* Get the value from the object file.  */
      if (bfd_big_endian (input_bfd))
         add = (((insn) & 0x07ff0000) >> 4) | (((insn) & 0x7ff) << 1);
      else
         add = ((((insn) & 0x7ff) << 12) | (((insn) & 0x07ff0000) >> 15));

      /* Get the value from the object file with an appropriate sign.
         The expression involving howto->src_mask isolates the upper
         bit of src_mask.  If that bit is set in the value we are
         adding, it is negative, and we subtract out that number times
         two.  If src_mask includes the highest possible bit, then we
         can not get the upper bit, but that does not matter since
         signed_add needs no adjustment to become negative in that case.  */
 
      signed_add = add;
 
      if ((add & (((~ src_mask) >> 1) & src_mask)) != 0)
         signed_add -= (((~ src_mask) >> 1) & src_mask) << 1;

      /* Add the value from the object file, shifted so that it is a
         straight number.  */
      /* howto->bitpos == 0 */
 
      signed_check += signed_add;
      relocation += signed_add;
 
      /* Assumes two's complement.  */
      if (signed_check > reloc_signed_max
          || signed_check < reloc_signed_min)
         overflow = true;
 
      /* Put RELOCATION into the correct bits:  */
 
      if (bfd_big_endian (input_bfd))
        relocation = (((relocation & 0xffe) >> 1)  | ((relocation << 4) & 0x07ff0000));
      else
        relocation = (((relocation & 0xffe) << 15) | ((relocation >> 12) & 0x7ff));

      /* Add RELOCATION to the correct bits of X:  */
      insn = ((insn & ~howto->dst_mask) | relocation);
 
      /* Put the relocated value back in the object file:  */
      bfd_put_32 (input_bfd, insn, hit_data);
 
      return (overflow ? bfd_reloc_overflow : bfd_reloc_ok);
      }
      break;
      
    case R_ARM_SBREL32:
      return bfd_reloc_notsupported;

    case R_ARM_AMP_VCALL9:
      return bfd_reloc_notsupported;

    case R_ARM_RSBREL32:
      return bfd_reloc_notsupported;

    case R_ARM_THM_RPC22:
      return bfd_reloc_notsupported;

    case R_ARM_RREL32:
      return bfd_reloc_notsupported;

    case R_ARM_RABS32:
      return bfd_reloc_notsupported;

    case R_ARM_RPC24:
      return bfd_reloc_notsupported;

    case R_ARM_RBASE:
      return bfd_reloc_notsupported;

    default:
      return bfd_reloc_notsupported;
    }
}
/* Relocate an ARM ELF section.  */
static boolean
elf32_arm_relocate_section (output_bfd, info, input_bfd, input_section,
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
      howto = elf32_arm_howto_table + r_type;

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

      r = elf32_arm_final_link_relocate (howto, input_bfd, output_bfd,
					   input_section,
					   contents, rel->r_offset,
					   relocation, rel->r_addend,
					   info, sec, (h ? h->other : sym->st_other));


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

/* Function to keep ARM specific flags in the ELF header. */
static boolean
elf32_arm_set_private_flags (abfd, flags)
     bfd *    abfd;
     flagword flags;
{
  if (elf_flags_init (abfd)
      && elf_elfheader (abfd)->e_flags != flags)
    {
      if (flags & EF_INTERWORK)
	_bfd_error_handler (_("\
Warning: Not setting interwork flag of %s since it has already been specified as non-interworking"),
			    bfd_get_filename (abfd));
      else
	_bfd_error_handler (_("\
Warning: Clearing the interwork flag of %s due to outside request"),
			      bfd_get_filename (abfd));
    }
  else
    {
      elf_elfheader (abfd)->e_flags = flags;
      elf_flags_init (abfd) = true;
    }
  
  return true;
}

/* Copy backend specific data from one object module to another */
static boolean
elf32_arm_copy_private_bfd_data (ibfd, obfd)
     bfd * ibfd;
     bfd * obfd;
{
  flagword in_flags;
  flagword out_flags;
  
  if (   bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return true;

  in_flags  = elf_elfheader (ibfd)->e_flags;
  out_flags = elf_elfheader (obfd)->e_flags;
  
  if (elf_flags_init (obfd)  && in_flags != out_flags)
    {
      /* Cannot mix PIC and non-PIC code.  */
      if ((in_flags & EF_PIC) != (out_flags & EF_PIC))
	return false;
      
      /* Cannot mix APCS26 and APCS32 code.  */
      if ((in_flags & EF_APCS_26) != (out_flags & EF_APCS_26))
	return false;
      
      /* Cannot mix float APCS and non-float APCS code.  */
      if ((in_flags & EF_APCS_FLOAT) != (out_flags & EF_APCS_FLOAT))
	return false;

      /* If the src and dest have different interworking flags
	 then turn off the interworking bit.  */
      if ((in_flags & EF_INTERWORK) != (out_flags & EF_INTERWORK))
	{
	  if (out_flags & EF_INTERWORK)
	    _bfd_error_handler (_("\
Warning: Clearing the interwork flag in %s because non-interworking code in %s has been linked with it"),
				bfd_get_filename (obfd), bfd_get_filename (ibfd));
	  
	  in_flags &= ~ EF_INTERWORK;
	}
    }

  elf_elfheader (obfd)->e_flags = in_flags;
  elf_flags_init (obfd) = true;
  
  return true;
}

/* Merge backend specific data from an object file to the output
   object file when linking.  */
static boolean
elf32_arm_merge_private_bfd_data (ibfd, obfd)
     bfd * ibfd;
     bfd * obfd;
{
  flagword out_flags;
  flagword in_flags;

  if (   bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return true;

  /* The input BFD must have had its flags initialised.  */
  /* The following seems bogus to me -- The flags are initialized in
     the assembler but I don't think an elf_flags_init field is
     written into the object */
  /* BFD_ASSERT (elf_flags_init (ibfd)); */
  
  in_flags  = elf_elfheader (ibfd)->e_flags;
  out_flags = elf_elfheader (obfd)->e_flags;

  if (! elf_flags_init (obfd))
    {
      /* If the input is the default architecture then do not
	 bother setting the flags for the output architecture,
	 instead allow future merges to do this.  If no future
	 merges ever set these flags then they will retain their
	 unitialised values, which surprise surprise, correspond
	 to the default values.  */
      if (bfd_get_arch_info (ibfd)->the_default)
	return true;
      
      elf_flags_init (obfd) = true;
      elf_elfheader (obfd)->e_flags = in_flags;

      if (bfd_get_arch (obfd) == bfd_get_arch (ibfd)
	  && bfd_get_arch_info (obfd)->the_default)
	return bfd_set_arch_mach (obfd, bfd_get_arch (ibfd), bfd_get_mach (ibfd));

      return true;
    }

  /* Check flag compatibility.  */
  if (in_flags == out_flags)
    return true;

  /* Complain about various flag mismatches.  */
  
  if ((in_flags & EF_APCS_26) != (out_flags & EF_APCS_26))
    _bfd_error_handler (_("\
Error: %s compiled for APCS-%d, whereas %s is compiled for APCS-%d"),
			bfd_get_filename (ibfd),
			in_flags & EF_APCS_26 ? 26 : 32,
			bfd_get_filename (obfd),
			out_flags & EF_APCS_26 ? 26 : 32);
  
  if ((in_flags & EF_APCS_FLOAT) != (out_flags & EF_APCS_FLOAT))
    _bfd_error_handler (_("\
Error: %s passes floats in %s registers, whereas %s passes them in %s registers"),
			bfd_get_filename (ibfd),
			in_flags & EF_APCS_FLOAT ? _("float") : _("integer"),
			bfd_get_filename (obfd),
			out_flags & EF_APCS_26 ? _("float") : _("integer"));
  
  if ((in_flags & EF_PIC) != (out_flags & EF_PIC))
    _bfd_error_handler (_("\
Error: %s is compiled as position %s code, whereas %s is not"),
			bfd_get_filename (ibfd),
			in_flags & EF_PIC ? _("independent") : _("dependent"),
			bfd_get_filename (obfd));

  /* Interworking mismatch is only a warning. */
  if ((in_flags & EF_INTERWORK) != (out_flags & EF_INTERWORK))
    {
      _bfd_error_handler (_("\
Warning: %s %s interworking, whereas %s %s"),
			  bfd_get_filename (ibfd),
			  in_flags & EF_INTERWORK ? _("supports") : _("does not support"),
			  bfd_get_filename (obfd),
			  out_flags & EF_INTERWORK ? _("does not") : _("does"));
      return true;
    }

  return false;
}

/* Display the flags field */
static boolean
elf32_arm_print_private_bfd_data (abfd, ptr)
     bfd *   abfd;
     PTR     ptr;
{
  FILE * file = (FILE *) ptr;
  
  BFD_ASSERT (abfd != NULL && ptr != NULL);

  /* Print normal ELF private data.  */
  _bfd_elf_print_private_bfd_data (abfd, ptr);

  /* Ignore init flag - it may not be set, despite the flags field containing valid data.  */
  
  /* xgettext:c-format */
  fprintf (file, _("private flags = %lx:"), elf_elfheader (abfd)->e_flags);

  if (elf_elfheader (abfd)->e_flags & EF_INTERWORK)
    fprintf (file, _(" [interworking enabled]"));
  else
    fprintf (file, _(" [interworking not enabled]"));
    
  if (elf_elfheader (abfd)->e_flags & EF_APCS_26)
    fprintf (file, _(" [APCS-26]"));
  else
    fprintf (file, _(" [APCS-32]"));
      
  if (elf_elfheader (abfd)->e_flags & EF_APCS_FLOAT)
    fprintf (file, _(" [floats passed in float registers]"));
  else
    fprintf (file, _(" [floats passed in intgere registers]"));
      
  if (elf_elfheader (abfd)->e_flags & EF_PIC)
    fprintf (file, _(" [position independent]"));
  else
    fprintf (file, _(" [absolute position]"));
      
  fputc ('\n', file);
  
  return true;
}


#define TARGET_LITTLE_SYM		bfd_elf32_littlearm_vec
#define TARGET_LITTLE_NAME		"elf32-littlearm"
#define TARGET_BIG_SYM			bfd_elf32_bigarm_vec
#define TARGET_BIG_NAME			"elf32-bigarm"
#define ELF_ARCH			bfd_arch_arm
#define ELF_MACHINE_CODE		EM_ARM

#define bfd_elf32_bfd_reloc_type_lookup 	elf32_arm_reloc_type_lookup
#define elf_info_to_howto			elf32_arm_info_to_howto
#define elf_info_to_howto_rel			0
#define elf_backend_relocate_section    	elf32_arm_relocate_section
#define bfd_elf32_bfd_copy_private_bfd_data 	elf32_arm_copy_private_bfd_data
#define bfd_elf32_bfd_merge_private_bfd_data 	elf32_arm_merge_private_bfd_data
#define bfd_elf32_bfd_set_private_flags		elf32_arm_set_private_flags
#define bfd_elf32_bfd_print_private_bfd_data	elf32_arm_print_private_bfd_data

#define elf_symbol_leading_char '_'

#include "elf32-target.h"
/* end-sanitize-armelf */
