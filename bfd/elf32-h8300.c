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
	 NULL,			/* special_function */
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
	 NULL,			/* special_function */
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
	 NULL,			/* special_function */
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
	 NULL,			/* special_function */
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
	 NULL,			/* special_function */
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
	 NULL,			/* special_function */
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
	 NULL,			/* special_function */
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
	 NULL,			/* special_function */
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
	 NULL,			/* special_function */
	 "R_H8_DIR32",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */
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
     Elf32_Internal_Rela *elf_reloc ATTRIBUTE_UNUSED;
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

#define TARGET_BIG_SYM			bfd_elf32_h8300_vec
#define TARGET_BIG_NAME			"elf32-h8300"
#define ELF_ARCH			bfd_arch_h8300
#define ELF_MACHINE_CODE		EM_H8_300
#define ELF_MAXPAGESIZE			0x1
#define bfd_elf32_bfd_reloc_type_lookup elf32_h8_reloc_type_lookup
#define elf_info_to_howto		elf32_h8_info_to_howto
#define elf_info_to_howto_rel		elf32_h8_info_to_howto_rel

/* ??? when elf_backend_relocate_section is not defined, elf32-target.h
   defaults to using _bfd_generic_link_hash_table_create, but
   elflink.h:bfd_elf32_size_dynamic_sections uses
   dynobj = elf_hash_table (info)->dynobj;
   and thus requires an elf hash table.  */
#define bfd_elf32_bfd_link_hash_table_create _bfd_elf_link_hash_table_create

#include "elf32-target.h"
