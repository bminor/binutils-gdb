/* MIPS-specific support for 32-bit ELF
   Copyright 1993, 1994 Free Software Foundation, Inc.

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
#include "bfdlink.h"
#include "genlink.h"
#include "libelf.h"
#include "elf/mips.h"

/* Get the ECOFF swapping routines.  */
#include "coff/sym.h"
#include "coff/symconst.h"
#include "coff/internal.h"
#include "coff/ecoff.h"
#include "coff/mips.h"
#define ECOFF_32
#include "ecoffswap.h"

static bfd_reloc_status_type mips_elf_hi16_reloc PARAMS ((bfd *abfd,
							  arelent *reloc,
							  asymbol *symbol,
							  PTR data,
							  asection *section,
							  bfd *output_bfd,
							  char **error));
static bfd_reloc_status_type mips_elf_got16_reloc PARAMS ((bfd *abfd,
							   arelent *reloc,
							   asymbol *symbol,
							   PTR data,
							   asection *section,
							   bfd *output_bfd,
							   char **error));
static bfd_reloc_status_type mips_elf_lo16_reloc PARAMS ((bfd *abfd,
							  arelent *reloc,
							  asymbol *symbol,
							  PTR data,
							  asection *section,
							  bfd *output_bfd,
							  char **error));
static bfd_reloc_status_type mips_elf_gprel16_reloc PARAMS ((bfd *abfd,
							     arelent *reloc,
							     asymbol *symbol,
							     PTR data,
							     asection *section,
							     bfd *output_bfd,
							     char **error));

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
	 complain_overflow_dont, /* complain_on_overflow */
	 			/* This needs complex overflow
				   detection, because the upper four
				   bits must match the PC.  */
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
	 mips_elf_got16_reloc,	/* special_function */
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
		     output_bfd,
		     error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  bfd_reloc_status_type ret;
  bfd_vma relocation;

  /* If we're relocating, and this an external symbol, we don't want
     to change anything.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && reloc_entry->addend == 0)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  /* FIXME: The symbol _gp_disp requires special handling, which we do
     not do.  */
  if (strcmp (bfd_asymbol_name (symbol), "_gp_disp") == 0)
    abort ();

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
		     output_bfd,
		     error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  /* FIXME: The symbol _gp_disp requires special handling, which we do
     not do.  */
  if (output_bfd == (bfd *) NULL
      && strcmp (bfd_asymbol_name (symbol), "_gp_disp") == 0)
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
				input_section, output_bfd, error_message);
}

/* Do a R_MIPS_GOT16 reloc.  This is a reloc against the global offset
   table used for PIC code.  If the symbol is an external symbol, the
   instruction is modified to contain the offset of the appropriate
   entry in the global offset table.  If the symbol is a section
   symbol, the next reloc is a R_MIPS_LO16 reloc.  The two 16 bit
   addends are combined to form the real addend against the section
   symbol; the GOT16 is modified to contain the offset of an entry in
   the global offset table, and the LO16 is modified to offset it
   appropriately.  Thus an offset larger than 16 bits requires a
   modified value in the global offset table.

   This implementation suffices for the assembler, but the linker does
   not yet know how to create global offset tables.  */

static bfd_reloc_status_type
mips_elf_got16_reloc (abfd,
		      reloc_entry,
		      symbol,
		      data,
		      input_section,
		      output_bfd,
		      error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  /* If we're relocating, and this an external symbol, we don't want
     to change anything.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && reloc_entry->addend == 0)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  /* If we're relocating, and this is a local symbol, we can handle it
     just like HI16.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) != 0)
    return mips_elf_hi16_reloc (abfd, reloc_entry, symbol, data,
				input_section, output_bfd, error_message);

  abort ();
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
			output_bfd,
			error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
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
	      *error_message =
		(char *) "GP relative relocation when _gp not defined";
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
    return bfd_reloc_overflow;

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

/* Determine whether a symbol is global for the purposes of splitting
   the symbol table into global symbols and local symbols.  At least
   on Irix 5, this split must be between section symbols and all other
   symbols.  On most ELF targets the split is between static symbols
   and externally visible symbols.  */

/*ARGSUSED*/
static boolean
mips_elf_sym_is_global (abfd, sym)
     bfd *abfd;
     asymbol *sym;
{
  return (sym->flags & BSF_SECTION_SYM) == 0 ? true : false;
}

/* Set the right machine number for a MIPS ELF file.  */

static boolean
mips_elf_object_p (abfd)
     bfd *abfd;
{
  switch (elf_elfheader (abfd)->e_flags & EF_MIPS_ARCH)
    {
    default:
    case E_MIPS_ARCH_1:
      /* Just use the default, which was set in elfcode.h.  */
      break;

    case E_MIPS_ARCH_2:
      (void) bfd_default_set_arch_mach (abfd, bfd_arch_mips, 6000);
      break;

    case E_MIPS_ARCH_3:
      (void) bfd_default_set_arch_mach (abfd, bfd_arch_mips, 4000);
      break;
    }

  return true;
}

/* The final processing done just before writing out a MIPS ELF object
   file.  This gets the MIPS architecture right based on the machine
   number.  */

static void
mips_elf_final_write_processing (abfd)
     bfd *abfd;
{
  unsigned long val;

  switch (bfd_get_mach (abfd))
    {
    case 3000:
      val = E_MIPS_ARCH_1;
      break;

    case 6000:
      val = E_MIPS_ARCH_2;
      break;

    case 4000:
      val = E_MIPS_ARCH_3;
      break;

    default:
      return;
    }

  elf_elfheader (abfd)->e_flags &=~ EF_MIPS_ARCH;
  elf_elfheader (abfd)->e_flags |= val;
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
  asection *newsect;

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
    case SHT_MIPS_OPTIONS:
      if (strcmp (name, ".options") != 0)
	return false;
      break;
    default:
      return false;
    }

  if (! _bfd_elf_make_section_from_shdr (abfd, hdr, name))
    return false;
  newsect = (asection *) hdr->rawdata;

  if (hdr->sh_type == SHT_MIPS_DEBUG)
    {
      if (! bfd_set_section_flags (abfd, newsect,
				   (bfd_get_section_flags (abfd, newsect)
				    | SEC_DEBUGGING)))
	return false;
    }

  /* FIXME: We should record sh_info for a .gptab section.  */

  /* For a .reginfo section, set the gp value in the tdata information
     from the contents of this section.  We need the gp value while
     processing relocs, so we just get it now.  */
  if (hdr->sh_type == SHT_MIPS_REGINFO)
    {
      Elf32_External_RegInfo ext;
      Elf32_RegInfo s;

      if (! bfd_get_section_contents (abfd, newsect, (PTR) &ext,
				      (file_ptr) 0, sizeof ext))
	return false;
      bfd_mips_elf32_swap_reginfo_in (abfd, &ext, &s);
      elf_gp (abfd) = s.ri_gp_value;
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
    {
      hdr->sh_type = SHT_MIPS_DEBUG;
      hdr->sh_entsize = 1;
    }
  else if (strcmp (name, ".reginfo") == 0)
    {
      hdr->sh_type = SHT_MIPS_REGINFO;
      hdr->sh_entsize = 1;

      /* Force the section size to the correct value, even if the
	 linker thinks it is larger.  The link routine below will only
	 write out this much data for .reginfo.  */
      hdr->sh_size = sec->_raw_size = sizeof (Elf32_External_RegInfo);
    }
  else if (strcmp (name, ".options") == 0)
    {
      hdr->sh_type = SHT_MIPS_OPTIONS;
      hdr->sh_entsize = 1;
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

/* Read ECOFF debugging information from a .mdebug section into a
   ecoff_debug_info structure.  */

static boolean
mips_elf_read_ecoff_info (abfd, section, debug)
     bfd *abfd;
     asection *section;
     struct ecoff_debug_info *debug;
{
  HDRR *symhdr;
  const struct ecoff_debug_swap *swap;
  char *ext_hdr = NULL;

  swap = get_elf_backend_data (abfd)->elf_backend_ecoff_debug_swap;

  ext_hdr = (char *) malloc (swap->external_hdr_size);
  if (ext_hdr == NULL && swap->external_hdr_size != 0)
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }

  if (bfd_get_section_contents (abfd, section, ext_hdr, (file_ptr) 0,
				swap->external_hdr_size)
      == false)
    goto error_return;

  symhdr = &debug->symbolic_header;
  (*swap->swap_hdr_in) (abfd, ext_hdr, symhdr);

  /* The symbolic header contains absolute file offsets and sizes to
     read.  */
#define READ(ptr, offset, count, size, type)				\
  if (symhdr->count == 0)						\
    debug->ptr = NULL;							\
  else									\
    {									\
      debug->ptr = (type) malloc (size * symhdr->count);		\
      if (debug->ptr == NULL)						\
	{								\
	  bfd_set_error (bfd_error_no_memory);				\
	  goto error_return;						\
	}								\
      if (bfd_seek (abfd, (file_ptr) symhdr->offset, SEEK_SET) != 0	\
	  || (bfd_read (debug->ptr, size, symhdr->count,		\
			abfd) != size * symhdr->count))			\
	goto error_return;						\
    }

  READ (external_ext, cbExtOffset, iextMax, swap->external_ext_size, PTR);
  READ (line, cbLineOffset, cbLine, sizeof (unsigned char), unsigned char *);
  READ (external_dnr, cbDnOffset, idnMax, swap->external_dnr_size, PTR);
  READ (external_pdr, cbPdOffset, ipdMax, swap->external_pdr_size, PTR);
  READ (external_sym, cbSymOffset, isymMax, swap->external_sym_size, PTR);
  READ (external_opt, cbOptOffset, ioptMax, swap->external_opt_size, PTR);
  READ (external_aux, cbAuxOffset, iauxMax, sizeof (union aux_ext),
	union aux_ext *);
  READ (ss, cbSsOffset, issMax, sizeof (char), char *);
  READ (ssext, cbSsExtOffset, issExtMax, sizeof (char), char *);
  READ (external_fdr, cbFdOffset, ifdMax, swap->external_fdr_size, PTR);
  READ (external_rfd, cbRfdOffset, crfd, swap->external_rfd_size, PTR);

  debug->fdr = NULL;
  debug->adjust = NULL;

  return true;

 error_return:
  if (ext_hdr != NULL)
    free (ext_hdr);
  if (debug->external_ext != NULL)
    free (debug->external_ext);
  if (debug->line != NULL)
    free (debug->line);
  if (debug->external_dnr != NULL)
    free (debug->external_dnr);
  if (debug->external_pdr != NULL)
    free (debug->external_pdr);
  if (debug->external_sym != NULL)
    free (debug->external_sym);
  if (debug->external_opt != NULL)
    free (debug->external_opt);
  if (debug->external_aux != NULL)
    free (debug->external_aux);
  if (debug->ss != NULL)
    free (debug->ss);
  if (debug->ssext != NULL)
    free (debug->ssext);
  if (debug->external_fdr != NULL)
    free (debug->external_fdr);
  if (debug->external_rfd != NULL)
    free (debug->external_rfd);
  return false;
}

/* Get EXTR information for a symbol.  */

static boolean
mips_elf_get_extr (sym, esym)
     asymbol *sym;
     EXTR *esym;
{
  const struct ecoff_debug_swap *swap;

  if (sym->flags & BSF_SECTION_SYM)
    return false;

  if (bfd_asymbol_flavour (sym) != bfd_target_elf_flavour
      || ((elf_symbol_type *) sym)->tc_data.mips_extr == NULL)
    {
      esym->jmptbl = 0;
      esym->cobol_main = 0;
      esym->weakext = 0;
      esym->reserved = 0;
      esym->ifd = ifdNil;
      /* FIXME: we can do better than this for st and sc.  */
      esym->asym.st = stGlobal;
      esym->asym.sc = scAbs;
      esym->asym.reserved = 0;
      esym->asym.index = indexNil;
      return true;
    }

  swap = (get_elf_backend_data (bfd_asymbol_bfd (sym))
	  ->elf_backend_ecoff_debug_swap);
  (*swap->swap_ext_in) (bfd_asymbol_bfd (sym),
			((elf_symbol_type *) sym)->tc_data.mips_extr,
			esym);

  return true;
}

/* Set the symbol index for an external symbol.  This is actually not
   needed for ELF.  */

/*ARGSUSED*/
static void
mips_elf_set_index (sym, indx)
     asymbol *sym;
     bfd_size_type indx;
{
}

/* We need to use a special link routine to handle the .reginfo and
   the .mdebug sections.  We need to merge all instances of these
   sections together, not write them all out sequentially.  */

static boolean
mips_elf_final_link (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  bfd *sub;
  size_t outsymalloc;
  struct generic_write_global_symbol_info wginfo;
  asection **secpp;
  asection *o;
  struct bfd_link_order *p;
  asection *reginfo_sec, *mdebug_sec;
  Elf32_RegInfo reginfo;
  struct ecoff_debug_info debug;
  const struct ecoff_debug_swap *swap
    = get_elf_backend_data (abfd)->elf_backend_ecoff_debug_swap;
  HDRR *symhdr = &debug.symbolic_header;
  PTR mdebug_handle = NULL;

  abfd->outsymbols = (asymbol **) NULL;
  abfd->symcount = 0;
  outsymalloc = 0;

  /* Build the output symbol table.  This also reads in the symbols
     for all the input BFDs, keeping them in the outsymbols field.  */
  for (sub = info->input_bfds; sub != (bfd *) NULL; sub = sub->link_next)
    if (! _bfd_generic_link_output_symbols (abfd, sub, info, &outsymalloc))
      return false;

  /* Accumulate the global symbols.  */
  wginfo.info = info;
  wginfo.output_bfd = abfd;
  wginfo.psymalloc = &outsymalloc;
  _bfd_generic_link_hash_traverse (_bfd_generic_hash_table (info),
				   _bfd_generic_link_write_global_symbol,
				   (PTR) &wginfo);

  /* Drop the .options section, since it has special semantics which I
     haven't bothered to figure out.  Also drop the .gptab sections,
     which also require special handling which is not currently done.
     Removing the .gptab sections is required for Irix 5
     compatibility; I don't know about .options.  */
  secpp = &abfd->sections;
  while (*secpp != NULL)
    {
      if (strcmp ((*secpp)->name, ".options") == 0
	  || strncmp ((*secpp)->name, ".gptab", 6) == 0)
	{
	  *secpp = (*secpp)->next;
	  --abfd->section_count;
	}
      else
	secpp = &(*secpp)->next;
    }

  /* Go through the sections and collect the .reginfo and .mdebug
     information.  We don't write out the information until we have
     set the section sizes, because the ELF backend only assigns space
     in the file once.  */
  reginfo_sec = NULL;
  mdebug_sec = NULL;
  for (o = abfd->sections; o != (asection *) NULL; o = o->next)
    {
      if (strcmp (o->name, ".reginfo") == 0)
	{
	  memset (&reginfo, 0, sizeof reginfo);

	  /* We have found the .reginfo section in the output file.
	     Look through all the link_orders comprising it and merge
	     the information together.  */
	  for (p = o->link_order_head;
	       p != (struct bfd_link_order *) NULL;
	       p = p->next)
	    {
	      asection *input_section;
	      bfd *input_bfd;
	      Elf32_External_RegInfo ext;
	      Elf32_RegInfo sub;

	      if (p->type != bfd_indirect_link_order)
		continue;

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

	  /* Force the section size to the value we want.  */
	  o->_raw_size = sizeof (Elf32_External_RegInfo);

	  /* Skip this section later on.  */
	  o->link_order_head = (struct bfd_link_order *) NULL;

	  reginfo_sec = o;
	}

      if (strcmp (o->name, ".mdebug") == 0)
	{
	  /* We have found the .mdebug section in the output file.
	     Look through all the link_orders comprising it and merge
	     the information together.  */
	  symhdr->magic = swap->sym_magic;
	  /* FIXME: What should the version stamp be?  */
	  symhdr->vstamp = 0;
	  symhdr->ilineMax = 0;
	  symhdr->cbLine = 0;
	  symhdr->idnMax = 0;
	  symhdr->ipdMax = 0;
	  symhdr->isymMax = 0;
	  symhdr->ioptMax = 0;
	  symhdr->iauxMax = 0;
	  symhdr->issMax = 0;
	  symhdr->issExtMax = 0;
	  symhdr->ifdMax = 0;
	  symhdr->crfd = 0;
	  symhdr->iextMax = 0;

	  /* We accumulate the debugging information itself in the
	     debug_info structure.  */
	  debug.line = NULL;
	  debug.external_dnr = NULL;
	  debug.external_pdr = NULL;
	  debug.external_sym = NULL;
	  debug.external_opt = NULL;
	  debug.external_aux = NULL;
	  debug.ss = NULL;
	  debug.ssext = debug.ssext_end = NULL;
	  debug.external_fdr = NULL;
	  debug.external_rfd = NULL;
	  debug.external_ext = debug.external_ext_end = NULL;

	  mdebug_handle = bfd_ecoff_debug_init (abfd, &debug, swap, info);
	  if (mdebug_handle == (PTR) NULL)
	    return false;

	  for (p = o->link_order_head;
	       p != (struct bfd_link_order *) NULL;
	       p = p->next)
	    {
	      asection *input_section;
	      bfd *input_bfd;
	      const struct ecoff_debug_swap *input_swap;
	      struct ecoff_debug_info input_debug;

	      if (p->type != bfd_indirect_link_order)
		continue;

	      input_section = p->u.indirect.section;
	      input_bfd = input_section->owner;

	      if (bfd_get_flavour (input_bfd) != bfd_target_elf_flavour
		  || (get_elf_backend_data (input_bfd)
		      ->elf_backend_ecoff_debug_swap) == NULL)
		{
		  /* I don't know what a non MIPS ELF bfd would be
		     doing with a .mdebug section, but I don't really
		     want to deal with it.  */
		  continue;
		}

	      input_swap = (get_elf_backend_data (input_bfd)
			    ->elf_backend_ecoff_debug_swap);

	      BFD_ASSERT (p->size == input_section->_raw_size);

	      /* The ECOFF linking code expects that we have already
		 read in the debugging information and set up an
		 ecoff_debug_info structure, so we do that now.  */
	      if (! mips_elf_read_ecoff_info (input_bfd, input_section,
					      &input_debug))
		return false;

	      if (! (bfd_ecoff_debug_accumulate
		     (mdebug_handle, abfd, &debug, swap, input_bfd,
		      &input_debug, input_swap, info)))
		return false;

	      /* Loop through the external symbols.  For each one with
		 interesting information, try to find the symbol on
		 the symbol table of abfd and save the information in
		 order to put it into the final external symbols.  */
	      if (info->hash->creator == input_bfd->xvec)
		{
		  char *eraw_src;
		  char *eraw_end;

		  eraw_src = input_debug.external_ext;
		  eraw_end = (eraw_src
			      + (input_debug.symbolic_header.iextMax
				 * input_swap->external_ext_size));
		  for (;
		       eraw_src < eraw_end;
		       eraw_src += input_swap->external_ext_size)
		    {
		      EXTR ext;
		      const char *name;
		      struct generic_link_hash_entry *h;
		      elf_symbol_type *elf_sym;

		      (*input_swap->swap_ext_in) (input_bfd, (PTR) eraw_src,
						  &ext);
		      if (ext.asym.sc == scNil
			  || ext.asym.sc == scUndefined
			  || ext.asym.sc == scSUndefined)
			continue;

		      name = input_debug.ssext + ext.asym.iss;
		      h = ((struct generic_link_hash_entry *)
			   bfd_link_hash_lookup (info->hash, name, false,
						 false, true));
		      if (h == (struct generic_link_hash_entry *) NULL
			  || h->sym == (asymbol *) NULL)
			continue;

		      elf_sym = (elf_symbol_type *) (h->sym);

		      if (elf_sym->tc_data.mips_extr != NULL)
			continue;

		      if (ext.ifd != -1)
			{
			  BFD_ASSERT (ext.ifd
				      < input_debug.symbolic_header.ifdMax);
			  ext.ifd = input_debug.ifdmap[ext.ifd];
			}

		      (*input_swap->swap_ext_out) (input_bfd, &ext,
						   (PTR) eraw_src);
		      elf_sym->tc_data.mips_extr = (PTR) eraw_src;
		    }
		}

	      /* Free up the information we just read, except for the
		 external symbols which we may have pointers to.  */
	      free (input_debug.line);
	      free (input_debug.external_dnr);
	      free (input_debug.external_pdr);
	      free (input_debug.external_sym);
	      free (input_debug.external_opt);
	      free (input_debug.external_aux);
	      free (input_debug.ss);
	      free (input_debug.ssext);
	      free (input_debug.external_fdr);
	      free (input_debug.external_rfd);
	    }

	  /* Build the external symbol information.  */
	  if (! bfd_ecoff_debug_externals (abfd, &debug, swap,
					   info->relocateable,
					   mips_elf_get_extr,
					   mips_elf_set_index))
	    return false;

	  /* Set the size of the section.  */
	  o->_raw_size = bfd_ecoff_debug_size (abfd, &debug, swap);

	  /* Skip this section later on.  */
	  o->link_order_head = (struct bfd_link_order *) NULL;

	  mdebug_sec = o;
	}
    }

  if (info->relocateable)
    {
      /* Allocate space for the output relocs for each section.  */
      for (o = abfd->sections;
	   o != (asection *) NULL;
	   o = o->next)
	{
	  o->reloc_count = 0;
	  for (p = o->link_order_head;
	       p != (struct bfd_link_order *) NULL;
	       p = p->next)
	    {
	      if (p->type == bfd_section_reloc_link_order
		  || p->type == bfd_symbol_reloc_link_order)
		++o->reloc_count;
	      else if (p->type == bfd_indirect_link_order)
		{
		  asection *input_section;
		  bfd *input_bfd;
		  long relsize;
		  arelent **relocs;
		  long reloc_count;

		  input_section = p->u.indirect.section;
		  input_bfd = input_section->owner;
		  relsize = bfd_get_reloc_upper_bound (input_bfd,
						       input_section);
		  if (relsize < 0)
		    return false;
		  relocs = (arelent **) malloc (relsize);
		  if (!relocs && relsize != 0)
		    {
		      bfd_set_error (bfd_error_no_memory);
		      return false;
		    }
		  reloc_count =
		    bfd_canonicalize_reloc (input_bfd, input_section,
					    relocs,
					    bfd_get_outsymbols (input_bfd));
		  if (reloc_count < 0)
		    return false;
		  BFD_ASSERT (reloc_count == input_section->reloc_count);
		  o->reloc_count += reloc_count;
		  free (relocs);
		}
	    }
	  if (o->reloc_count > 0)
	    {
	      o->orelocation = ((arelent **)
				bfd_alloc (abfd,
					   (o->reloc_count
					    * sizeof (arelent *))));
	      if (!o->orelocation)
		{
		  bfd_set_error (bfd_error_no_memory);
		  return false;
		}
	      o->flags |= SEC_RELOC;
	      /* Reset the count so that it can be used as an index
		 when putting in the output relocs.  */
	      o->reloc_count = 0;
	    }
	}
    }

  /* Write out the information we have accumulated.  */
  if (reginfo_sec != (asection *) NULL)
    {
      Elf32_External_RegInfo ext;

      bfd_mips_elf32_swap_reginfo_out (abfd, &reginfo, &ext);
      if (! bfd_set_section_contents (abfd, reginfo_sec, (PTR) &ext,
				      (file_ptr) 0, sizeof ext))
	return false;
    }

  if (mdebug_sec != (asection *) NULL)
    {
      if (! abfd->output_has_begun)
	{
	  /* Force the section to be given a file position.  */
	  bfd_set_section_contents (abfd, mdebug_sec, (PTR) NULL,
				    (file_ptr) 0, (bfd_size_type) 0);
	  BFD_ASSERT (abfd->output_has_begun);
	}
      if (! bfd_ecoff_write_accumulated_debug (mdebug_handle, abfd, &debug,
					       swap, info,
					       mdebug_sec->filepos))
	return false;

      bfd_ecoff_debug_free (mdebug_handle, abfd, &debug, swap, info);
    }

  /* Handle all the link order information for the sections.  */
  for (o = abfd->sections;
       o != (asection *) NULL;
       o = o->next)
    {
      for (p = o->link_order_head;
	   p != (struct bfd_link_order *) NULL;
	   p = p->next)
	{
	  switch (p->type)
	    {
	    case bfd_section_reloc_link_order:
	    case bfd_symbol_reloc_link_order:
	      if (! _bfd_generic_reloc_link_order (abfd, info, o, p))
		return false;
	      break;
	    default:
	      if (! _bfd_default_link_order (abfd, info, o, p))
		return false;
	      break;
	    }
	}
    }

  return true;
}

/* MIPS ELF uses two common sections.  One is the usual one, and the
   other is for small objects.  All the small objects are kept
   together, and then referenced via the gp pointer, which yields
   faster assembler code.  This is what we use for the small common
   section.  This approach is copied from ecoff.c.  */
static asection mips_elf_scom_section;
static asymbol mips_elf_scom_symbol;
static asymbol *mips_elf_scom_symbol_ptr;

/* MIPS ELF also uses an acommon section, which represents an
   allocated common symbol which may be overridden by a 	
   definition in a shared library.  */
static asection mips_elf_acom_section;
static asymbol mips_elf_acom_symbol;
static asymbol *mips_elf_acom_symbol_ptr;

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
      /* This section is used in a dynamically linked executable file.
	 It is an allocated common section.  The dynamic linker can
	 either resolve these symbols to something in a shared
	 library, or it can just leave them here.  For our purposes,
	 we can consider these symbols to be in a new section.  */
      if (mips_elf_acom_section.name == NULL)
	{
	  /* Initialize the acommon section.  */
	  mips_elf_acom_section.name = ".acommon";
	  mips_elf_acom_section.flags = SEC_NO_FLAGS;
	  mips_elf_acom_section.output_section = &mips_elf_acom_section;
	  mips_elf_acom_section.symbol = &mips_elf_acom_symbol;
	  mips_elf_acom_section.symbol_ptr_ptr = &mips_elf_acom_symbol_ptr;
	  mips_elf_acom_symbol.name = ".acommon";
	  mips_elf_acom_symbol.flags = BSF_SECTION_SYM;
	  mips_elf_acom_symbol.section = &mips_elf_acom_section;
	  mips_elf_acom_symbol_ptr = &mips_elf_acom_symbol;
	}
      asym->section = &mips_elf_acom_section;
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

/* ECOFF swapping routines.  These are used when dealing with the
   .mdebug section, which is in the ECOFF debugging format.  */
static const struct ecoff_debug_swap mips_elf_ecoff_debug_swap =
{
  /* Symbol table magic number.  */
  magicSym,
  /* Alignment of debugging information.  E.g., 4.  */
  4,
  /* Sizes of external symbolic information.  */
  sizeof (struct hdr_ext),
  sizeof (struct dnr_ext),
  sizeof (struct pdr_ext),
  sizeof (struct sym_ext),
  sizeof (struct opt_ext),
  sizeof (struct fdr_ext),
  sizeof (struct rfd_ext),
  sizeof (struct ext_ext),
  /* Functions to swap in external symbolic data.  */
  ecoff_swap_hdr_in,
  ecoff_swap_dnr_in,
  ecoff_swap_pdr_in,
  ecoff_swap_sym_in,
  ecoff_swap_opt_in,
  ecoff_swap_fdr_in,
  ecoff_swap_rfd_in,
  ecoff_swap_ext_in,
  /* Functions to swap out external symbolic data.  */
  ecoff_swap_hdr_out,
  ecoff_swap_dnr_out,
  ecoff_swap_pdr_out,
  ecoff_swap_sym_out,
  ecoff_swap_opt_out,
  ecoff_swap_fdr_out,
  ecoff_swap_rfd_out,
  ecoff_swap_ext_out
};

#define TARGET_LITTLE_SYM		bfd_elf32_littlemips_vec
#define TARGET_LITTLE_NAME		"elf32-littlemips"
#define TARGET_BIG_SYM			bfd_elf32_bigmips_vec
#define TARGET_BIG_NAME			"elf32-bigmips"
#define ELF_ARCH			bfd_arch_mips
#define ELF_MACHINE_CODE		EM_MIPS
#define ELF_MAXPAGESIZE			0x10000
#define elf_backend_collect		true
#define elf_info_to_howto		0
#define elf_info_to_howto_rel		mips_info_to_howto_rel
#define elf_backend_sym_is_global	mips_elf_sym_is_global
#define elf_backend_object_p		mips_elf_object_p
#define elf_backend_section_from_shdr	mips_elf_section_from_shdr
#define elf_backend_fake_sections	mips_elf_fake_sections
#define elf_backend_section_from_bfd_section \
					mips_elf_section_from_bfd_section
#define elf_backend_section_processing	mips_elf_section_processing
#define elf_backend_symbol_processing	mips_elf_symbol_processing
#define elf_backend_final_write_processing \
					mips_elf_final_write_processing
#define elf_backend_ecoff_debug_swap	&mips_elf_ecoff_debug_swap

#define bfd_elf32_bfd_link_add_symbols	_bfd_generic_link_add_symbols_collect
#define bfd_elf32_bfd_final_link	mips_elf_final_link

#include "elf32-target.h"
