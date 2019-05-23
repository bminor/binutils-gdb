/* Linux bpf specific support for 64-bit ELF
   Copyright (C) 2019 Free Software Foundation, Inc.
   Contributed by Oracle Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/bpf.h"
#include "libiberty.h"

/* In case we're on a 32-bit machine, construct a 64-bit "-1" value.  */
#define MINUS_ONE (~ (bfd_vma) 0)

#define BASEADDR(SEC)	((SEC)->output_section->vma + (SEC)->output_offset)

/* Handler for PC-relative relocations, which must be handled in
   64-bit words.  */

static bfd_reloc_status_type
bpf_elf_insn_disp_reloc (bfd *abfd,
                         arelent *reloc_entry,
                         asymbol *symbol,
                         void *data,
                         asection *input_section,
                         bfd *output_bfd,
                         char **error_message ATTRIBUTE_UNUSED)
{
  bfd_signed_vma relocation;
  bfd_signed_vma addend;
  reloc_howto_type *howto = reloc_entry->howto;

  /* This part is from bfd_elf_generic_reloc.  */
  if (output_bfd != NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (! reloc_entry->howto->partial_inplace
	  || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  /* This works because partial_inplace is FALSE.  */
  if (output_bfd != NULL)
    return bfd_reloc_continue;

  if (reloc_entry->address > bfd_get_section_limit (abfd, input_section))
    return bfd_reloc_outofrange;

  relocation = (symbol->value
		+ symbol->section->output_section->vma
		+ symbol->section->output_offset);
  /* Make it PC relative.  */
  relocation -= (input_section->output_section->vma
                 + input_section->output_offset);
  relocation -= reloc_entry->address;
  /* Make it 64-bit words.  */
  relocation = relocation / 8;

  /* Get the addend from the instruction and apply it.  */
  switch (howto->bitsize)
    {
    default:
      abort ();
      break;
    case 16:
      addend = bfd_get_16 (abfd, (bfd_byte *) data + reloc_entry->address + 2);
      break;
    case 32:
      addend = bfd_get_32 (abfd, (bfd_byte *) data + reloc_entry->address + 4);
      break;
    }

  if ((addend & (((~howto->src_mask) >> 1) & howto->src_mask)) != 0)
    addend -= (((~howto->src_mask) >> 1) & howto->src_mask) << 1;
  relocation += addend;

  /* Write out the relocated value.  */
  switch (howto->bitsize)
    {
    default:
      abort ();
      break;
    case 16:
      bfd_put_16 (abfd, relocation, (bfd_byte *) data + reloc_entry->address + 2);
      break;
    case 32:
      bfd_put_32 (abfd, relocation, (bfd_byte *) data + reloc_entry->address + 4);
      break;
    }

  /* Check for overflow.  */
  if (howto->complain_on_overflow == complain_overflow_signed)
    {
      bfd_signed_vma reloc_signed_max = (1 << (howto->bitsize - 1)) - 1;
      bfd_signed_vma reloc_signed_min = ~reloc_signed_max;
      
      if (relocation > reloc_signed_max || relocation < reloc_signed_min)
        return bfd_reloc_overflow;
    }
  else
    abort();

  return bfd_reloc_ok;
}

/* Relocation tables.  */
static reloc_howto_type bpf_elf_howto_table [] =
{
  /* This reloc does nothing.  */
  HOWTO (R_BPF_NONE,		/* type */
	 0,			/* rightshift */
	 3,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_NONE",		/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 64-immediate in LDDW instruction.  */
  HOWTO (R_BPF_INSN_64,		/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_INSN_64",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 MINUS_ONE,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* 32-immediate in LDDW instruction.  */
  HOWTO (R_BPF_INSN_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_INSN_32",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* 16-bit offsets in instructions.  */
  HOWTO (R_BPF_INSN_16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_INSN_16",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* 16-bit PC-relative address in jump instructions.  */
  HOWTO (R_BPF_INSN_DISP16,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 TRUE,			/* pc_relative */
	 32,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bpf_elf_insn_disp_reloc, /* special_function */
	 "R_BPF_INSN_DISP16",   /* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  HOWTO (R_BPF_DATA_8_PCREL,
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_8_PCREL",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xff,			/* dst_mask */
	 TRUE),			/* pcrel_offset */

  HOWTO (R_BPF_DATA_16_PCREL,
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_16_PCREL",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  HOWTO (R_BPF_DATA_32_PCREL,
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_32_PCREL",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  HOWTO (R_BPF_DATA_8,
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_unsigned, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_DATA_8",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xff,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  HOWTO (R_BPF_DATA_16,
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_unsigned, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_DATA_16",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 32-bit PC-relative address in call instructions.  */
  HOWTO (R_BPF_INSN_DISP32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bpf_elf_insn_disp_reloc, /* special_function */
	 "R_BPF_INSN_DISP32",   /* name */
	 FALSE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* 32-bit data.  */
  HOWTO (R_BPF_DATA_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_DATA_32",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* 64-bit data.  */
  HOWTO (R_BPF_DATA_64,		/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_DATA_64",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 MINUS_ONE,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  HOWTO (R_BPF_DATA_64_PCREL,
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_BPF_64_PCREL",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 MINUS_ONE,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
};
#undef AHOW

/* Map BFD reloc types to bpf ELF reloc types.  */

static reloc_howto_type *
bpf_reloc_type_lookup (bfd * abfd ATTRIBUTE_UNUSED,
                        bfd_reloc_code_real_type code)
{
  /* Note that the bpf_elf_howto_table is indxed by the R_
     constants.  Thus, the order that the howto records appear in the
     table *must* match the order of the relocation types defined in
     include/elf/bpf.h.  */

  switch (code)
    {
    case BFD_RELOC_NONE:
      return &bpf_elf_howto_table[ (int) R_BPF_NONE];

    case BFD_RELOC_8_PCREL:
      return &bpf_elf_howto_table[ (int) R_BPF_DATA_8_PCREL];
    case BFD_RELOC_16_PCREL:
      return &bpf_elf_howto_table[ (int) R_BPF_DATA_16_PCREL];
    case BFD_RELOC_32_PCREL:
      return &bpf_elf_howto_table[ (int) R_BPF_DATA_32_PCREL];
    case BFD_RELOC_64_PCREL:
      return &bpf_elf_howto_table[ (int) R_BPF_DATA_64_PCREL];

    case BFD_RELOC_8:
      return &bpf_elf_howto_table[ (int) R_BPF_DATA_8];
    case BFD_RELOC_16:
      return &bpf_elf_howto_table[ (int) R_BPF_DATA_16];
    case BFD_RELOC_32:
      return &bpf_elf_howto_table[ (int) R_BPF_DATA_32];
    case BFD_RELOC_64:
      return &bpf_elf_howto_table[ (int) R_BPF_DATA_64];

    case BFD_RELOC_BPF_64:
      return &bpf_elf_howto_table[ (int) R_BPF_INSN_64];
    case BFD_RELOC_BPF_32:
      return &bpf_elf_howto_table[ (int) R_BPF_INSN_32];
    case BFD_RELOC_BPF_16:
      return &bpf_elf_howto_table[ (int) R_BPF_INSN_16];
    case BFD_RELOC_BPF_DISP16:
      return &bpf_elf_howto_table[ (int) R_BPF_INSN_DISP16];
    case BFD_RELOC_BPF_DISP32:
      return &bpf_elf_howto_table[ (int) R_BPF_INSN_DISP32];

    default:
      /* Pacify gcc -Wall.  */
      return NULL;
    }
  return NULL;
}

/* Map BFD reloc names to bpf ELF reloc names.  */

static reloc_howto_type *
bpf_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED, const char *r_name)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (bpf_elf_howto_table); i++)
    if (bpf_elf_howto_table[i].name != NULL
	&& strcasecmp (bpf_elf_howto_table[i].name, r_name) == 0)
      return &bpf_elf_howto_table[i];

  return NULL;
}

/* Set the howto pointer for a bpf reloc.  */

static bfd_boolean
bpf_info_to_howto (bfd *abfd, arelent *bfd_reloc,
                    Elf_Internal_Rela *elf_reloc)
{
  unsigned int r_type;

  r_type = ELF64_R_TYPE (elf_reloc->r_info);
  if (r_type >= (unsigned int) R_BPF_max)
    {
      /* xgettext:c-format */
      _bfd_error_handler (_("%pB: unsupported relocation type %#x"),
                          abfd, r_type);
      bfd_set_error (bfd_error_bad_value);
      return FALSE;
    }

  bfd_reloc->howto = &bpf_elf_howto_table [r_type];
  return TRUE;
}

/* Merge backend specific data from an object file to the output
   object file when linking.  */

static bfd_boolean
elf64_bpf_merge_private_bfd_data (bfd *ibfd, struct bfd_link_info *info)
{
  /* Check if we have the same endianness.  */
  if (! _bfd_generic_verify_endian_match (ibfd, info))
    return FALSE;

  return TRUE;
}

/* The macros below configure the architecture.  */

#define TARGET_LITTLE_SYM bpf_elf64_le_vec
#define TARGET_LITTLE_NAME "elf64-bpfle"

#define TARGET_BIG_SYM bpf_elf64_be_vec
#define TARGET_BIG_NAME "elf64-bpfbe"

#define ELF_ARCH bfd_arch_bpf
#define ELF_MACHINE_CODE EM_BPF

#define ELF_MAXPAGESIZE 0x100000

#define elf_info_to_howto_rel bpf_info_to_howto
#define elf_info_to_howto bpf_info_to_howto

#define elf_backend_may_use_rel_p		1
#define elf_backend_may_use_rela_p		0
#define elf_backend_default_use_rela_p		0

#define elf_backend_can_gc_sections		0

#define elf_symbol_leading_char			'_'
#define bfd_elf64_bfd_reloc_type_lookup		bpf_reloc_type_lookup
#define bfd_elf64_bfd_reloc_name_lookup		bpf_reloc_name_lookup

#define bfd_elf64_bfd_merge_private_bfd_data elf64_bpf_merge_private_bfd_data

#include "elf64-target.h"
