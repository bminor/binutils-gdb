/* LoongArch-specific support for ELF.
   Copyright (C) 2021-2022 Free Software Foundation, Inc.
   Contributed by Loongson Ltd.

   Based on RISC-V target.

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
   along with this program; see the file COPYING3.  If not,
   see <http://www.gnu.org/licenses/>.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/loongarch.h"
#include "elfxx-loongarch.h"

#define ALL_ONES (~ (bfd_vma) 0)

/* This does not include any relocation information, but should be
   good enough for GDB or objdump to read the file.  */

static reloc_howto_type howto_table[] =
{
#define LOONGARCH_HOWTO(r_name)						 \
  HOWTO (R_LARCH_##r_name, 0, 2, 32, false, 0, complain_overflow_signed, \
	 bfd_elf_generic_reloc, "R_LARCH_" #r_name, false, 0, 0xffffffff, false)

  /* No relocation.  */
  HOWTO (R_LARCH_NONE,			/* type (0).  */
	 0,				/* rightshift */
	 3,				/* size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_NONE",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 32 bit relocation.  */
  HOWTO (R_LARCH_32,			/* type (1).  */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_32",			/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 64 bit relocation.  */
  HOWTO (R_LARCH_64,			/* type (2).  */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_64",			/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ALL_ONES,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_LARCH_RELATIVE,		/* type (3).  */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_RELATIVE",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_LARCH_COPY,			/* type (4).  */
	 0,				/* rightshift */
	 0,				/* this one is variable size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_COPY",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_LARCH_JUMP_SLOT,		/* type (5).  */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_JUMP_SLOT",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* Dynamic TLS relocations.  */
  HOWTO (R_LARCH_TLS_DTPMOD32,		/* type (6).  */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_TLS_DTPMOD32",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_LARCH_TLS_DTPMOD64,		/* type (7).  */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_TLS_DTPMOD64",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ALL_ONES,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_LARCH_TLS_DTPREL32,		/* type (8). */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_TLS_DTPREL32",	/* name */
	 true,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_LARCH_TLS_DTPREL64,		/* type (9).  */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_TLS_DTPREL64",	/* name */
	 true,				/* partial_inplace */
	 0,				/* src_mask */
	 ALL_ONES,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_LARCH_TLS_TPREL32,		/* type (10).  */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_TLS_TPREL32",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_LARCH_TLS_TPREL64,		/* type (11).  */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_TLS_TPREL64",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ALL_ONES,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_LARCH_IRELATIVE,		/* type (12).  */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_LARCH_IRELATIVE",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  EMPTY_HOWTO(13),
  EMPTY_HOWTO(14),
  EMPTY_HOWTO(15),
  EMPTY_HOWTO(16),
  EMPTY_HOWTO(17),
  EMPTY_HOWTO(18),
  EMPTY_HOWTO(19),

  HOWTO (R_LARCH_MARK_LA,			/* type (20).  */
	 0,				   	/* rightshift.  */
	 3,				   	/* size.  */
	 0,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_MARK_LA",			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask.  */
	 0,					/* dst_mask.  */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_MARK_PCREL,			/* type (21).  */
	 0,				   	/* rightshift.  */
	 3,				   	/* size.  */
	 0,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_MARK_PCREL",			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask.  */
	 0,					/* dst_mask.  */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SOP_PUSH_PCREL,	      	/* type (22).  */
	 2,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 32,				  	/* bitsize.  */
	 true /* FIXME: somewhat use this.  */,	/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SOP_PUSH_PCREL",	    	/* name.  */
	 false,			       		/* partial_inplace.  */
	 0x03ffffff,			  	/* src_mask.  */
	 0x03ffffff,			  	/* dst_mask.  */
	 false),			      	/* pcrel_offset.  */

  /* type 23-37.  */
  LOONGARCH_HOWTO (SOP_PUSH_ABSOLUTE),
  LOONGARCH_HOWTO (SOP_PUSH_DUP),
  LOONGARCH_HOWTO (SOP_PUSH_GPREL),
  LOONGARCH_HOWTO (SOP_PUSH_TLS_TPREL),
  LOONGARCH_HOWTO (SOP_PUSH_TLS_GOT),
  LOONGARCH_HOWTO (SOP_PUSH_TLS_GD),
  LOONGARCH_HOWTO (SOP_PUSH_PLT_PCREL),
  LOONGARCH_HOWTO (SOP_ASSERT),
  LOONGARCH_HOWTO (SOP_NOT),
  LOONGARCH_HOWTO (SOP_SUB),
  LOONGARCH_HOWTO (SOP_SL),
  LOONGARCH_HOWTO (SOP_SR),
  LOONGARCH_HOWTO (SOP_ADD),
  LOONGARCH_HOWTO (SOP_AND),
  LOONGARCH_HOWTO (SOP_IF_ELSE),

  HOWTO (R_LARCH_SOP_POP_32_S_10_5,	      	/* type (38).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 5,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 10,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SOP_POP_32_S_10_5",	    	/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0x7c00,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SOP_POP_32_U_10_12,	      	/* type (39).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 12,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 10,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SOP_POP_32_U_10_12",	    	/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0x3ffc00,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SOP_POP_32_S_10_12,	      	/* type (40).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 12,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 10,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SOP_POP_32_S_10_12",	    	/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0x3ffc00,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SOP_POP_32_S_10_16,	      	/* type (41).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 16,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 10,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SOP_POP_32_S_10_16",	    	/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0x3fffc00,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SOP_POP_32_S_10_16_S2,	      	/* type (42).  */
	 2,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 16,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 10,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SOP_POP_32_S_10_16_S2",    	/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0x3fffc00,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SOP_POP_32_S_5_20,	      	/* type (43).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 20,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 5,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SOP_POP_32_S_5_20",    	/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0x1fffe0,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SOP_POP_32_S_0_5_10_16_S2,    	/* type (44).  */
	 2,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 21,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SOP_POP_32_S_0_5_10_16_S2",  	/* name.  */
	 false,			       		/* partial_inplace.  */
	 0xfc0003e0,				/* src_mask */
	 0xfc0003e0,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SOP_POP_32_S_0_10_10_16_S2,   	/* type (45).  */
	 2,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 26,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SOP_POP_32_S_0_10_10_16_S2", 	/* name.  */
	 false,			       		/* partial_inplace.  */
	 0xfc000000,				/* src_mask */
	 0xfc000000,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SOP_POP_32_U,	      		/* type (46).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 32,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SOP_POP_32_S_U",    		/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0,					/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_ADD8,	      			/* type (47).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 8,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_ADD8",    			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0xffffffff,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_ADD16,	      			/* type (48).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 16,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_ADD16",    			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0xffffffff,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_ADD24,	      			/* type (49).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 24,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_ADD24",    			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0xffffffff,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_ADD32,	      			/* type (50).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 32,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_ADD32",    			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0xffffffff,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_ADD64,	      			/* type (51).  */
	 0,				   	/* rightshift.  */
	 4,				   	/* size.  */
	 64,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_ADD64",    			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 ALL_ONES,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SUB8,	      			/* type (52).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 8,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SUB8",    			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0xffffffff,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SUB16,	      			/* type (53).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 16,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SUB16",    			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0xffffffff,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SUB24,	      			/* type (54).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 24,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SUB24",    			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0xffffffff,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SUB32,	      			/* type (55).  */
	 0,				   	/* rightshift.  */
	 2,				   	/* size.  */
	 32,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SUB32",    			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 0xffffffff,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

  HOWTO (R_LARCH_SUB64,	      			/* type (56).  */
	 0,				   	/* rightshift.  */
	 4,				   	/* size.  */
	 64,				  	/* bitsize.  */
	 false,					/* pc_relative.  */
	 0,				   	/* bitpos.  */
	 complain_overflow_signed,	    	/* complain_on_overflow.  */
	 bfd_elf_generic_reloc,	       		/* special_function.  */
	 "R_LARCH_SUB64",    			/* name.  */
	 false,			       		/* partial_inplace.  */
	 0,					/* src_mask */
	 ALL_ONES,				/* dst_mask */
	 false),			      	/* pcrel_offset.  */

};

struct elf_reloc_map
{
  bfd_reloc_code_real_type bfd_val;
  enum elf_loongarch_reloc_type elf_val;
};

static const struct elf_reloc_map larch_reloc_map[] =
{
  { BFD_RELOC_NONE, R_LARCH_NONE },
  { BFD_RELOC_32, R_LARCH_32 },
  { BFD_RELOC_64, R_LARCH_64 },

#define LOONGARCH_reloc_map(r_name)		   	\
  {						  	\
    BFD_RELOC_LARCH_##r_name, R_LARCH_##r_name		\
  }
  LOONGARCH_reloc_map (TLS_DTPMOD32),
  LOONGARCH_reloc_map (TLS_DTPMOD64),
  LOONGARCH_reloc_map (TLS_DTPREL32),
  LOONGARCH_reloc_map (TLS_DTPREL64),
  LOONGARCH_reloc_map (TLS_TPREL32),
  LOONGARCH_reloc_map (TLS_TPREL64),

  LOONGARCH_reloc_map (MARK_LA),
  LOONGARCH_reloc_map (MARK_PCREL),
  LOONGARCH_reloc_map (SOP_PUSH_PCREL),
  LOONGARCH_reloc_map (SOP_PUSH_ABSOLUTE),
  LOONGARCH_reloc_map (SOP_PUSH_DUP),
  LOONGARCH_reloc_map (SOP_PUSH_GPREL),
  LOONGARCH_reloc_map (SOP_PUSH_TLS_TPREL),
  LOONGARCH_reloc_map (SOP_PUSH_TLS_GOT),
  LOONGARCH_reloc_map (SOP_PUSH_TLS_GD),
  LOONGARCH_reloc_map (SOP_PUSH_PLT_PCREL),
  LOONGARCH_reloc_map (SOP_ASSERT),
  LOONGARCH_reloc_map (SOP_NOT),
  LOONGARCH_reloc_map (SOP_SUB),
  LOONGARCH_reloc_map (SOP_SL),
  LOONGARCH_reloc_map (SOP_SR),
  LOONGARCH_reloc_map (SOP_ADD),
  LOONGARCH_reloc_map (SOP_AND),
  LOONGARCH_reloc_map (SOP_IF_ELSE),
  LOONGARCH_reloc_map (SOP_POP_32_S_10_5),
  LOONGARCH_reloc_map (SOP_POP_32_U_10_12),
  LOONGARCH_reloc_map (SOP_POP_32_S_10_12),
  LOONGARCH_reloc_map (SOP_POP_32_S_10_16),
  LOONGARCH_reloc_map (SOP_POP_32_S_10_16_S2),
  LOONGARCH_reloc_map (SOP_POP_32_S_5_20),
  LOONGARCH_reloc_map (SOP_POP_32_S_0_5_10_16_S2),
  LOONGARCH_reloc_map (SOP_POP_32_S_0_10_10_16_S2),
  LOONGARCH_reloc_map (SOP_POP_32_U),
  LOONGARCH_reloc_map (ADD8),
  LOONGARCH_reloc_map (ADD16),
  LOONGARCH_reloc_map (ADD24),
  LOONGARCH_reloc_map (ADD32),
  LOONGARCH_reloc_map (ADD64),
  LOONGARCH_reloc_map (SUB8),
  LOONGARCH_reloc_map (SUB16),
  LOONGARCH_reloc_map (SUB24),
  LOONGARCH_reloc_map (SUB32),
  LOONGARCH_reloc_map (SUB64),
};

reloc_howto_type *
loongarch_elf_rtype_to_howto (bfd *abfd, unsigned int r_type)
{
  size_t i;
  for (i = 0; i < ARRAY_SIZE (howto_table); i++)
    if (howto_table[i].type == r_type)
      return &howto_table[i];

  (*_bfd_error_handler) (_("%pB: unsupported relocation type %#x"),
			 abfd, r_type);
  bfd_set_error (bfd_error_bad_value);
  return NULL;
}

reloc_howto_type *
loongarch_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			     bfd_reloc_code_real_type code)
{
  unsigned int i;
  for (i = 0; i < ARRAY_SIZE (larch_reloc_map); i++)
    if (larch_reloc_map[i].bfd_val == code)
      return loongarch_elf_rtype_to_howto (abfd,
					   (int) larch_reloc_map[i].elf_val);

  return NULL;
}

reloc_howto_type *
loongarch_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED, const char *r_name)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (howto_table); i++)
    if (howto_table[i].name && strcasecmp (howto_table[i].name, r_name) == 0)
      return &howto_table[i];

  return NULL;
}
