/* MIPS-specific support for 32-bit ELF
   Copyright 1993 Free Software Foundation, Inc.

   Most of the information added by Ian Lance Taylor, Cygnus Support,
   <ian@cygnus.com>.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "seclet.h"
#include "libelf.h"
#include "elf/mips.h"

static bfd_reloc_status_type mips_elf_hi16_reloc PARAMS ((bfd *abfd,
							  arelent *reloc,
							  asymbol *symbol,
							  PTR data,
							  asection *section,
							  bfd *output_bfd));
static bfd_reloc_status_type mips_elf_lo16_reloc PARAMS ((bfd *abfd,
							  arelent *reloc,
							  asymbol *symbol,
							  PTR data,
							  asection *section,
							  bfd *output_bfd));
static bfd_reloc_status_type mips_elf_gprel16_reloc PARAMS ((bfd *abfd,
							     arelent *reloc,
							     asymbol *symbol,
							     PTR data,
							     asection *section,
							     bfd *output_bfd));

#define USE_REL	1		/* MIPS uses REL relocations instead of RELA */

enum reloc_type
{
  R_MIPS_NONE = 0,
  R_MIPS_16,		R_MIPS_32,
  R_MIPS_REL32,		R_MIPS_26,
  R_MIPS_HI16,		R_MIPS_LO16,
  R_MIPS_GPREL16,	R_MIPS_LITERAL,
  R_MIPS_GOT16,		R_MIPS_PC16,
  R_MIPS_CALL16,	R_MIPS_GPREL32,
  R_MIPS_max
};

static reloc_howto_type elf_mips_howto_table[] =
{
  /* No relocation.  */
  HOWTO (R_MIPS_NONE,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MIPS_NONE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* 16 bit relocation.  */
  HOWTO (R_MIPS_16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MIPS_16",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 32 bit relocation.  */
  HOWTO (R_MIPS_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MIPS_32",		/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 32 bit symbol relative relocation.  */
  HOWTO (R_MIPS_REL32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MIPS_REL32",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 26 bit branch address.  */
  HOWTO (R_MIPS_26,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MIPS_26",		/* name */
	 true,			/* partial_inplace */
	 0x3ffffff,		/* src_mask */
	 0x3ffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* High 16 bits of symbol value.  */
  HOWTO (R_MIPS_HI16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 mips_elf_hi16_reloc,	/* special_function */
	 "R_MIPS_HI16",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Low 16 bits of symbol value.  */
  HOWTO (R_MIPS_LO16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 mips_elf_lo16_reloc,	/* special_function */
	 "R_MIPS_LO16",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* GP relative reference.  */
  HOWTO (R_MIPS_GPREL16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mips_elf_gprel16_reloc, /* special_function */
	 "R_MIPS_GPREL16",	/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Reference to literal section.  */
  HOWTO (R_MIPS_LITERAL,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mips_elf_gprel16_reloc, /* special_function */
	 "R_MIPS_LITERAL",	/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Reference to global offset table.  */
  /* FIXME: This is not handled correctly.  */
  HOWTO (R_MIPS_GOT16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MIPS_GOT16",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 16 bit PC relative reference.  */
  HOWTO (R_MIPS_PC16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MIPS_PC16",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 16 bit call through global offset table.  */
  /* FIXME: This is not handled correctly.  */
  HOWTO (R_MIPS_CALL16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MIPS_CALL16",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 32 bit GP relative reference.  */
  /* FIXME: This is not handled correctly.  */
  HOWTO (R_MIPS_GPREL32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MIPS_GPREL32",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false)			/* pcrel_offset */
};

/* Do a R_MIPS_HI16 relocation.  This has to be done in combination
   with a R_MIPS_LO16 reloc, because there is a carry from the LO16 to
   the HI16.  Here we just save the information we need; we do the
   actual relocation when we see the LO16.  MIPS ELF requires that the
   LO16 immediately follow the HI16, so this ought to work.  */

static bfd_byte *mips_hi16_addr;
static bfd_vma mips_hi16_addend;

static bfd_reloc_status_type
mips_elf_hi16_reloc (abfd,
		     reloc_entry,
		     symbol,
		     data,
		     input_section,
		     output_bfd)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
{
  bfd_reloc_status_type ret;
  bfd_vma relocation;

  /* FIXME: The symbol _gp_disp requires special handling, which we do
     not do.  */
  if (strcmp (bfd_asymbol_name (symbol), "_gp_disp") == 0)
    abort ();

  /* If we're relocating, and this an external symbol, we don't want
     to change anything.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && reloc_entry->addend == 0)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  ret = bfd_reloc_ok;
  if (symbol->section == &bfd_und_section
      && output_bfd == (bfd *) NULL)
    ret = bfd_reloc_undefined;

  if (bfd_is_com_section (symbol->section))
    relocation = 0;
  else
    relocation = symbol->value;

  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;
  relocation += reloc_entry->addend;

  if (reloc_entry->address > input_section->_cooked_size)
    return bfd_reloc_outofrange;

  /* Save the information, and let LO16 do the actual relocation.  */
  mips_hi16_addr = (bfd_byte *) data + reloc_entry->address;
  mips_hi16_addend = relocation;

  if (output_bfd != (bfd *) NULL)
    reloc_entry->address += input_section->output_offset;

  return ret;
}

/* Do a R_MIPS_LO16 relocation.  This is a straightforward 16 bit
   inplace relocation; this function exists in order to do the
   R_MIPS_HI16 relocation described above.  */

static bfd_reloc_status_type
mips_elf_lo16_reloc (abfd,
		     reloc_entry,
		     symbol,
		     data,
		     input_section,
		     output_bfd)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
{
  /* FIXME: The symbol _gp_disp requires special handling, which we do
     not do.  */
  if (strcmp (bfd_asymbol_name (symbol), "_gp_disp") == 0)
    abort ();

  if (mips_hi16_addr != (bfd_byte *) NULL)
    {
      unsigned long insn;
      unsigned long val;
      unsigned long vallo;

      /* Do the HI16 relocation.  Note that we actually don't need to
	 know anything about the LO16 itself, except where to find the
	 low 16 bits of the addend needed by the LO16.  */
      insn = bfd_get_32 (abfd, mips_hi16_addr);
      vallo = (bfd_get_32 (abfd, (bfd_byte *) data + reloc_entry->address)
	       & 0xffff);
      val = ((insn & 0xffff) << 16) + vallo;
      val += mips_hi16_addend;

      /* The low order 16 bits are always treated as a signed value.
	 Therefore, a negative value in the low order bits requires an
	 adjustment in the high order bits.  We need to make this
	 adjustment in two ways: once for the bits we took from the
	 data, and once for the bits we are putting back in to the
	 data.  */
      if ((vallo & 0x8000) != 0)
	val -= 0x10000;
      if ((val & 0x8000) != 0)
	val += 0x10000;

      insn = (insn &~ 0xffff) | ((val >> 16) & 0xffff);
      bfd_put_32 (abfd, insn, mips_hi16_addr);

      mips_hi16_addr = (bfd_byte *) NULL;
    }

  /* Now do the LO16 reloc in the usual way.  */
  return bfd_elf_generic_reloc (abfd, reloc_entry, symbol, data,
				input_section, output_bfd);
}

/* Do a R_MIPS_GPREL16 relocation.  This is a 16 bit value which must
   become the offset from the gp register.  This function also handles
   R_MIPS_LITERAL relocations, although those can be handled more
   cleverly because the entries in the .lit8 and .lit4 sections can be
   merged.  */

static bfd_reloc_status_type
mips_elf_gprel16_reloc (abfd,
			reloc_entry,
			symbol,
			data,
			input_section,
			output_bfd)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
{
  boolean relocateable;
  bfd_vma relocation;
  unsigned long val;
  unsigned long insn;

  /* If we're relocating, and this is an external symbol with no
     addend, we don't want to change anything.  We will only have an
     addend if this is a newly created reloc, not read from an ELF
     file.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && reloc_entry->addend == 0)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != (bfd *) NULL)
    relocateable = true;
  else
    {
      relocateable = false;
      output_bfd = symbol->section->output_section->owner;
    }

  if (symbol->section == &bfd_und_section
      && relocateable == false)
    return bfd_reloc_undefined;

  /* We have to figure out the gp value, so that we can adjust the
     symbol value correctly.  We look up the symbol _gp in the output
     BFD.  If we can't find it, we're stuck.  We cache it in the ELF
     target data.  We don't need to adjust the symbol value for an
     external symbol if we are producing relocateable output.  */
  if (elf_gp (output_bfd) == 0
      && (relocateable == false
	  || (symbol->flags & BSF_SECTION_SYM) != 0))
    {
      if (relocateable != false)
	{
	  /* Make up a value.  */
	  elf_gp (output_bfd) =
	    symbol->section->output_section->vma + 0x4000;
	}
      else
	{
	  unsigned int count;
	  asymbol **sym;
	  unsigned int i;

	  count = bfd_get_symcount (output_bfd);
	  sym = bfd_get_outsymbols (output_bfd);

	  if (sym == (asymbol **) NULL)
	    i = count;
	  else
	    {
	      for (i = 0; i < count; i++, sym++)
		{
		  register CONST char *name;

		  name = bfd_asymbol_name (*sym);
		  if (*name == '_' && strcmp (name, "_gp") == 0)
		    {
		      elf_gp (output_bfd) = bfd_asymbol_value (*sym);
		      break;
		    }
		}
	    }

	  if (i >= count)
	    {
	      /* Only get the error once.  */
	      elf_gp (output_bfd) = 4;
	      /* FIXME: How can we get the program name here?  */
	      fprintf (stderr,
		       "GP relative relocation when _gp not defined\n");
	      return bfd_reloc_dangerous;
	    }
	}
    }

  if (bfd_is_com_section (symbol->section))
    relocation = 0;
  else
    relocation = symbol->value;

  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;

  if (reloc_entry->address > input_section->_cooked_size)
    return bfd_reloc_outofrange;

  insn = bfd_get_32 (abfd, (bfd_byte *) data + reloc_entry->address);

  /* Set val to the offset into the section or symbol.  */
  val = ((insn & 0xffff) + reloc_entry->addend) & 0xffff;
  if (val & 0x8000)
    val -= 0x10000;

  /* Adjust val for the final section location and GP value.  If we
     are producing relocateable output, we don't want to do this for
     an external symbol.  */
  if (relocateable == false
      || (symbol->flags & BSF_SECTION_SYM) != 0)
    val += relocation - elf_gp (output_bfd);

  insn = (insn &~ 0xffff) | (val & 0xffff);
  bfd_put_32 (abfd, insn, (bfd_byte *) data + reloc_entry->address);

  if (relocateable != false)
    reloc_entry->address += input_section->output_offset;

  /* Make sure it fit in 16 bits.  */
  if (val >= 0x8000 && val < 0xffff8000)
    return bfd_reloc_outofrange;

  return bfd_reloc_ok;
}

/* A mapping from BFD reloc types to MIPS ELF reloc types.  */

struct elf_reloc_map {
  bfd_reloc_code_real_type bfd_reloc_val;
  enum reloc_type elf_reloc_val;
};

static CONST struct elf_reloc_map mips_reloc_map[] =
{
  { BFD_RELOC_NONE, R_MIPS_NONE, },
  { BFD_RELOC_16, R_MIPS_16 },
  { BFD_RELOC_32, R_MIPS_32 },
  { BFD_RELOC_CTOR, R_MIPS_32 },
  { BFD_RELOC_32_PCREL, R_MIPS_REL32 },
  { BFD_RELOC_MIPS_JMP, R_MIPS_26 },
  { BFD_RELOC_HI16_S, R_MIPS_HI16 },
  { BFD_RELOC_LO16, R_MIPS_LO16 },
  { BFD_RELOC_MIPS_GPREL, R_MIPS_GPREL16 },
  { BFD_RELOC_MIPS_LITERAL, R_MIPS_LITERAL },
  { BFD_RELOC_MIPS_GOT16, R_MIPS_GOT16 },
  { BFD_RELOC_16_PCREL, R_MIPS_PC16 },
  { BFD_RELOC_MIPS_CALL16, R_MIPS_CALL16 },
  { BFD_RELOC_MIPS_GPREL32, R_MIPS_GPREL32 }
};

/* Given a BFD reloc type, return a howto structure.  */

static CONST struct reloc_howto_struct *
bfd_elf32_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  int i;

  for (i = 0; i < sizeof (mips_reloc_map) / sizeof (struct elf_reloc_map); i++)
    {
      if (mips_reloc_map[i].bfd_reloc_val == code)
	return &elf_mips_howto_table[(int) mips_reloc_map[i].elf_reloc_val];
    }
  return NULL;
}

/* Given a MIPS reloc type, fill in an arelent structure.  */

static void
mips_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf32_Internal_Rel *dst;
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_MIPS_max);
  cache_ptr->howto = &elf_mips_howto_table[r_type];

  /* The addend for a GPREL16 or LITERAL relocation comes from the GP
     value for the object file.  We get the addend now, rather than
     when we do the relocation, because the symbol manipulations done
     by the linker may cause us to lose track of the input BFD.  */
  if (((*cache_ptr->sym_ptr_ptr)->flags & BSF_SECTION_SYM) != 0
      && (r_type == (unsigned int) R_MIPS_GPREL16
	  || r_type == (unsigned int) R_MIPS_LITERAL))
    cache_ptr->addend = elf_gp (abfd);
}

/* A .reginfo section holds a single Elf32_RegInfo structure.  These
   routines swap this structure in and out.  They are used outside of
   BFD, so they are globally visible.  */

void
bfd_mips_elf32_swap_reginfo_in (abfd, ex, in)
     bfd *abfd;
     const Elf32_External_RegInfo *ex;
     Elf32_RegInfo *in;
{
  in->ri_gprmask = bfd_h_get_32 (abfd, (bfd_byte *) ex->ri_gprmask);
  in->ri_cprmask[0] = bfd_h_get_32 (abfd, (bfd_byte *) ex->ri_cprmask[0]);
  in->ri_cprmask[1] = bfd_h_get_32 (abfd, (bfd_byte *) ex->ri_cprmask[1]);
  in->ri_cprmask[2] = bfd_h_get_32 (abfd, (bfd_byte *) ex->ri_cprmask[2]);
  in->ri_cprmask[3] = bfd_h_get_32 (abfd, (bfd_byte *) ex->ri_cprmask[3]);
  in->ri_gp_value = bfd_h_get_32 (abfd, (bfd_byte *) ex->ri_gp_value);
}

void
bfd_mips_elf32_swap_reginfo_out (abfd, in, ex)
     bfd *abfd;
     const Elf32_RegInfo *in;
     Elf32_External_RegInfo *ex;
{
  bfd_h_put_32 (abfd, (bfd_vma) in->ri_gprmask,
		(bfd_byte *) ex->ri_gprmask);
  bfd_h_put_32 (abfd, (bfd_vma) in->ri_cprmask[0],
		(bfd_byte *) ex->ri_cprmask[0]);
  bfd_h_put_32 (abfd, (bfd_vma) in->ri_cprmask[1],
		(bfd_byte *) ex->ri_cprmask[1]);
  bfd_h_put_32 (abfd, (bfd_vma) in->ri_cprmask[2],
		(bfd_byte *) ex->ri_cprmask[2]);
  bfd_h_put_32 (abfd, (bfd_vma) in->ri_cprmask[3],
		(bfd_byte *) ex->ri_cprmask[3]);
  bfd_h_put_32 (abfd, (bfd_vma) in->ri_gp_value,
		(bfd_byte *) ex->ri_gp_value);
}

/* Handle a MIPS specific section when reading an object file.  This
   is called when elfcode.h finds a section with an unknown type.
   FIXME: We need to handle the SHF_MIPS_GPREL flag, but I'm not sure
   how to.  */

static boolean
mips_elf_section_from_shdr (abfd, hdr, name)
     bfd *abfd;
     Elf32_Internal_Shdr *hdr;
     char *name;
{
  /* There ought to be a place to keep ELF backend specific flags, but
     at the moment there isn't one.  We just keep track of the
     sections by their name, instead.  Fortunately, the ABI gives
     suggested names for all the MIPS specific sections, so we will
     probably get away with this.  */
  switch (hdr->sh_type)
    {
    case SHT_MIPS_LIBLIST:
      if (strcmp (name, ".liblist") != 0)
	return false;
      break;
    case SHT_MIPS_CONFLICT:
      if (strcmp (name, ".conflict") != 0)
	return false;
      break;
    case SHT_MIPS_GPTAB:
      if (strncmp (name, ".gptab.", sizeof ".gptab." - 1) != 0)
	return false;
      break;
    case SHT_MIPS_UCODE:
      if (strcmp (name, ".ucode") != 0)
	return false;
      break;
    case SHT_MIPS_DEBUG:
      if (strcmp (name, ".mdebug") != 0)
	return false;
      break;
    case SHT_MIPS_REGINFO:
      if (strcmp (name, ".reginfo") != 0
	  || hdr->sh_size != sizeof (Elf32_External_RegInfo))
	return false;
      break;
    default:
      return false;
    }

  if (hdr->rawdata == NULL)
    {
      asection *newsect;

      newsect = bfd_make_section (abfd, name);
      if (newsect != NULL)
	{
	  newsect->filepos = hdr->sh_offset;
	  newsect->flags |= SEC_HAS_CONTENTS;
	  newsect->vma = hdr->sh_addr;
	  newsect->_raw_size = hdr->sh_size;
	  newsect->alignment_power = bfd_log2 (hdr->sh_addralign);

	  if (hdr->sh_flags & SHF_ALLOC)
	    {
	      newsect->flags |= SEC_ALLOC;
	      newsect->flags |= SEC_LOAD;
	    }

	  if (!(hdr->sh_flags & SHF_WRITE))
	    newsect->flags |= SEC_READONLY;

	  if (hdr->sh_flags & SHF_EXECINSTR)
	    newsect->flags |= SEC_CODE;
	  else if (newsect->flags & SEC_ALLOC)
	    newsect->flags |= SEC_DATA;

	  if (hdr->sh_type == SHT_MIPS_DEBUG)
	    newsect->flags |= SEC_DEBUGGING;

	  hdr->rawdata = (void *) newsect;

	  /* FIXME: We should record the sh_info field for a .gptab
	     section.  */

	  /* For a .reginfo section, set the gp value in the tdata
	     information from the contents of this section.  We need
	     the gp value while processing relocs, so we just get it
	     now.  */
	  if (hdr->sh_type == SHT_MIPS_REGINFO)
	    {
	      Elf32_External_RegInfo ext;
	      Elf32_RegInfo s;

	      if (bfd_get_section_contents (abfd, newsect, (PTR) &ext,
					    (file_ptr) 0,
					    sizeof ext) == false)
		return false;
	      bfd_mips_elf32_swap_reginfo_in (abfd, &ext, &s);
	      elf_gp (abfd) = s.ri_gp_value;
	    }
	}
      else
	hdr->rawdata = (void *) bfd_get_section_by_name (abfd, name);
    }

  return true;
}

/* Set the correct type for a MIPS ELF section.  We do this by the
   section name, which is a hack, but ought to work.  */

static boolean
mips_elf_fake_sections (abfd, hdr, sec)
     bfd *abfd;
     Elf32_Internal_Shdr *hdr;
     asection *sec;
{
  register const char *name;

  name = bfd_get_section_name (abfd, sec);

  if (strcmp (name, ".liblist") == 0)
    {
      hdr->sh_type = SHT_MIPS_LIBLIST;
      hdr->sh_info = sec->_raw_size / sizeof (Elf32_Lib);
      /* FIXME: Set the sh_link field.  */
    }
  else if (strcmp (name, ".conflict") == 0)
    hdr->sh_type = SHT_MIPS_CONFLICT;
  else if (strncmp (name, ".gptab.", sizeof ".gptab." - 1) == 0)
    {
      hdr->sh_type = SHT_MIPS_GPTAB;
      /* FIXME: Set the sh_info field.  */
    }
  else if (strcmp (name, ".ucode") == 0)
    hdr->sh_type = SHT_MIPS_UCODE;
  else if (strcmp (name, ".mdebug") == 0)
    hdr->sh_type = SHT_MIPS_DEBUG;
  else if (strcmp (name, ".reginfo") == 0)
    {
      hdr->sh_type = SHT_MIPS_REGINFO;

      /* Force the section size to the correct value, even if the
	 linker thinks it is larger.  The link routine below will only
	 write out this much data for .reginfo.  */
      hdr->sh_size = sec->_raw_size = sizeof (Elf32_External_RegInfo);
    }

  return true;
}

/* Given a BFD section, try to locate the corresponding ELF section
   index.  */

static boolean
mips_elf_section_from_bfd_section (abfd, hdr, sec, retval)
     bfd *abfd;
     Elf32_Internal_Shdr *hdr;
     asection *sec;
     int *retval;
{
  if (strcmp (bfd_get_section_name (abfd, sec), ".scommon") == 0)
    {
      *retval = SHN_MIPS_SCOMMON;
      return true;
    }
  if ((asection *) hdr->rawdata == sec)
    return true;
  return false;
}

/* Work over a section just before writing it out.  We update the GP
   value in the .reginfo section based on the value we are using.
   FIXME: We recognize sections that need the SHF_MIPS_GPREL flag by
   name; there has to be a better way.  */

static boolean
mips_elf_section_processing (abfd, hdr)
     bfd *abfd;
     Elf32_Internal_Shdr *hdr;
{
  if (hdr->sh_type == SHT_MIPS_REGINFO)
    {
      bfd_byte buf[4];

      BFD_ASSERT (hdr->sh_size == sizeof (Elf32_External_RegInfo));
      BFD_ASSERT (hdr->contents == NULL);

      if (bfd_seek (abfd,
		    hdr->sh_offset + sizeof (Elf32_External_RegInfo) - 4,
		    SEEK_SET) == -1)
	return false;
      bfd_h_put_32 (abfd, (bfd_vma) elf_gp (abfd), buf);
      if (bfd_write (buf, (bfd_size_type) 1, (bfd_size_type) 4, abfd) != 4)
	return false;
    }

  if (hdr->rawdata != NULL)
    {
      const char *name = ((asection *) hdr->rawdata)->name;

      if (strcmp (name, ".sdata") == 0)
	{
	  hdr->sh_flags |= SHF_ALLOC | SHF_WRITE | SHF_MIPS_GPREL;
	  hdr->sh_type = SHT_PROGBITS;
	}
      else if (strcmp (name, ".sbss") == 0)
	{
	  hdr->sh_flags |= SHF_ALLOC | SHF_WRITE | SHF_MIPS_GPREL;
	  hdr->sh_type = SHT_NOBITS;
	}
      else if (strcmp (name, ".lit8") == 0
	       || strcmp (name, ".lit4") == 0)
	{
	  hdr->sh_flags |= SHF_ALLOC | SHF_WRITE | SHF_MIPS_GPREL;
	  hdr->sh_type = SHT_PROGBITS;
	}
    }

  return true;
}

/* We need to use a special link routine to handle the .reginfo
   section.  We need to merge all the .reginfo sections together, not
   write them all out sequentially.  */

static boolean
mips_elf_seclet_link (abfd, data, relocateable)
     bfd *abfd;
     PTR data;
     boolean relocateable;
{
  asection *sec;
  Elf32_RegInfo reginfo;

  memset (&reginfo, 0, sizeof reginfo);

  for (sec = abfd->sections; sec != (asection *) NULL; sec = sec->next)
    {
      if (strcmp (sec->name, ".reginfo") == 0)
	{
	  bfd_seclet_type *p;
	  Elf32_External_RegInfo ext;

	  /* We have found the .reginfo section in the output file.
	     Look through all the seclets comprising it and merge the
	     information together.  */
	  for (p = sec->seclets_head;
	       p != (bfd_seclet_type *) NULL;
	       p = p->next)
	    {
	      switch (p->type)
		{
		case bfd_indirect_seclet:
		  {
		    asection *input_section;
		    bfd *input_bfd;
		    Elf32_RegInfo sub;

		    input_section = p->u.indirect.section;
		    input_bfd = input_section->owner;
		    BFD_ASSERT (input_section->_raw_size
				== sizeof (Elf32_External_RegInfo));
		    if (! bfd_get_section_contents (input_bfd, input_section,
						    (PTR) &ext,
						    (file_ptr) 0,
						    sizeof ext))
		      return false;

		    bfd_mips_elf32_swap_reginfo_in (input_bfd, &ext, &sub);

		    reginfo.ri_gprmask |= sub.ri_gprmask;
		    reginfo.ri_cprmask[0] |= sub.ri_cprmask[0];
		    reginfo.ri_cprmask[1] |= sub.ri_cprmask[1];
		    reginfo.ri_cprmask[2] |= sub.ri_cprmask[2];
		    reginfo.ri_cprmask[3] |= sub.ri_cprmask[3];

		    /* ri_gp_value is set by the function
		       mips_elf_section_processing when the section is
		       finally written out.  */
		  }
		  break;

		default:
		  break;
		}
	    }

	  /* Write out the information we have accumulated.  */
	  bfd_mips_elf32_swap_reginfo_out (abfd, &reginfo, &ext);
	  if (! bfd_set_section_contents (abfd, sec, (PTR) &ext,
					  (file_ptr) 0, sizeof ext))
	    return false;

	  /* Force the section size to the value we want.  */
	  sec->_raw_size = sizeof (Elf32_External_RegInfo);

	  /* Force bfd_generic_seclet_link to ignore this section.  */
	  sec->seclets_head = (bfd_seclet_type *) NULL;

	  break;
	}
    }

  return bfd_generic_seclet_link (abfd, data, relocateable);
}

/* MIPS ELF uses two common sections.  One is the usual one, and the
   other is for small objects.  All the small objects are kept
   together, and then referenced via the gp pointer, which yields
   faster assembler code.  This is what we use for the small common
   section.  This approach is copied from ecoff.c.  */
static asection mips_elf_scom_section;
static asymbol mips_elf_scom_symbol;
static asymbol *mips_elf_scom_symbol_ptr;

/* Handle the special MIPS section numbers that a symbol may use.  */

static void
mips_elf_symbol_processing (abfd, asym)
     bfd *abfd;
     asymbol *asym;
{
  elf_symbol_type *elfsym;

  elfsym = (elf_symbol_type *) asym;
  switch (elfsym->internal_elf_sym.st_shndx)
    {
    case SHN_MIPS_ACOMMON:
      /* FIXME: I don't really understand just what this section
	 means or when it would be used.  */
      abort ();
      break;

    case SHN_COMMON:
      /* Common symbols less than the GP size are automatically
	 treated as SHN_MIPS_SCOMMON symbols.  */
      if (asym->value > elf_gp_size (abfd))
	break;
      /* Fall through.  */
    case SHN_MIPS_SCOMMON:
      if (mips_elf_scom_section.name == NULL)
	{
	  /* Initialize the small common section.  */
	  mips_elf_scom_section.name = ".scommon";
	  mips_elf_scom_section.flags = SEC_IS_COMMON;
	  mips_elf_scom_section.output_section = &mips_elf_scom_section;
	  mips_elf_scom_section.symbol = &mips_elf_scom_symbol;
	  mips_elf_scom_section.symbol_ptr_ptr = &mips_elf_scom_symbol_ptr;
	  mips_elf_scom_symbol.name = ".scommon";
	  mips_elf_scom_symbol.flags = BSF_SECTION_SYM;
	  mips_elf_scom_symbol.section = &mips_elf_scom_section;
	  mips_elf_scom_symbol_ptr = &mips_elf_scom_symbol;
	}
      asym->section = &mips_elf_scom_section;
      asym->value = elfsym->internal_elf_sym.st_size;
      break;

    case SHN_MIPS_SUNDEFINED:
      asym->section = &bfd_und_section;
      break;
    }
}

#define TARGET_LITTLE_SYM		bfd_elf32_littlemips_vec
#define TARGET_LITTLE_NAME		"elf32-littlemips"
#define TARGET_BIG_SYM			bfd_elf32_bigmips_vec
#define TARGET_BIG_NAME			"elf32-bigmips"
#define ELF_ARCH			bfd_arch_mips
#define ELF_MAXPAGESIZE			0x10000
#define elf_info_to_howto		0
#define elf_info_to_howto_rel		mips_info_to_howto_rel
#define elf_backend_section_from_shdr	mips_elf_section_from_shdr
#define elf_backend_fake_sections	mips_elf_fake_sections
#define elf_backend_section_from_bfd_section \
					mips_elf_section_from_bfd_section
#define elf_backend_section_processing	mips_elf_section_processing
#define elf_backend_symbol_processing	mips_elf_symbol_processing

#define bfd_elf32_bfd_seclet_link	mips_elf_seclet_link

#include "elf32-target.h"
