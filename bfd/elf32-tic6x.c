/* 32-bit ELF support for TI C6X
   Copyright 2010
   Free Software Foundation, Inc.

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
#include "libiberty.h"
#include "elf-bfd.h"
#include "elf/tic6x.h"

static reloc_howto_type elf32_tic6x_howto_table[] =
{
  HOWTO (R_C6000_NONE,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_NONE",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_ABS32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_ABS32",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_ABS16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_ABS16",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_ABS8,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_ABS8",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x000000ff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_PCR_S21,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 21,			/* bitsize */
	 TRUE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_signed,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_PCR_S21",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0fffff80,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
  HOWTO (R_C6000_PCR_S12,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 12,			/* bitsize */
	 TRUE,			/* pc_relative */
	 16,			/* bitpos */
	 complain_overflow_signed,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_PCR_S12",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0fff0000,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
  HOWTO (R_C6000_PCR_S10,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 10,			/* bitsize */
	 TRUE,			/* pc_relative */
	 13,			/* bitpos */
	 complain_overflow_signed,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_PCR_S10",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fe000,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
  HOWTO (R_C6000_PCR_S7,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 7,			/* bitsize */
	 TRUE,			/* pc_relative */
	 16,			/* bitpos */
	 complain_overflow_signed,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_PCR_S7",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007f0000,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
  HOWTO (R_C6000_ABS_S16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_signed,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_ABS_S16",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_ABS_L16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_ABS_L16",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_ABS_H16,	/* type */
	 16,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_ABS_H16",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_U15_B,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 15,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_unsigned,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_U15_B",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff00,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_U15_H,	/* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 15,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_unsigned,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_U15_H",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff00,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_U15_W,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 15,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_unsigned,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_U15_W",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff00,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_S16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_signed,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_S16",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_L16_B,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_L16_B",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_L16_H,	/* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_L16_H",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_L16_W,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_L16_W",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_H16_B,	/* type */
	 16,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_H16_B",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_H16_H,	/* type */
	 17,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_H16_H",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_H16_W,	/* type */
	 18,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_H16_W",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_GOT_U15_W,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 15,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_unsigned,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_GOT_U15_W",/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff00,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_GOT_L16_W,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_GOT_L16_W",/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_SBR_GOT_H16_W,	/* type */
	 18,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_SBR_GOT_H16_W",/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff80,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_DSBT_INDEX,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 15,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_unsigned,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_DSBT_INDEX",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x007fff00,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_PREL31,	/* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 31,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_PREL31",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x7fffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_COPY,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_COPY",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  EMPTY_HOWTO (27),
  EMPTY_HOWTO (28),
  EMPTY_HOWTO (29),
  EMPTY_HOWTO (30),
  EMPTY_HOWTO (31),
  EMPTY_HOWTO (32),
  EMPTY_HOWTO (33),
  EMPTY_HOWTO (34),
  EMPTY_HOWTO (35),
  EMPTY_HOWTO (36),
  EMPTY_HOWTO (37),
  EMPTY_HOWTO (38),
  EMPTY_HOWTO (39),
  EMPTY_HOWTO (40),
  EMPTY_HOWTO (41),
  EMPTY_HOWTO (42),
  EMPTY_HOWTO (43),
  EMPTY_HOWTO (44),
  EMPTY_HOWTO (45),
  EMPTY_HOWTO (46),
  EMPTY_HOWTO (47),
  EMPTY_HOWTO (48),
  EMPTY_HOWTO (49),
  EMPTY_HOWTO (50),
  EMPTY_HOWTO (51),
  EMPTY_HOWTO (52),
  EMPTY_HOWTO (53),
  EMPTY_HOWTO (54),
  EMPTY_HOWTO (55),
  EMPTY_HOWTO (56),
  EMPTY_HOWTO (57),
  EMPTY_HOWTO (58),
  EMPTY_HOWTO (59),
  EMPTY_HOWTO (60),
  EMPTY_HOWTO (61),
  EMPTY_HOWTO (62),
  EMPTY_HOWTO (63),
  EMPTY_HOWTO (64),
  EMPTY_HOWTO (65),
  EMPTY_HOWTO (66),
  EMPTY_HOWTO (67),
  EMPTY_HOWTO (68),
  EMPTY_HOWTO (69),
  EMPTY_HOWTO (70),
  EMPTY_HOWTO (71),
  EMPTY_HOWTO (72),
  EMPTY_HOWTO (73),
  EMPTY_HOWTO (74),
  EMPTY_HOWTO (75),
  EMPTY_HOWTO (76),
  EMPTY_HOWTO (77),
  EMPTY_HOWTO (78),
  EMPTY_HOWTO (79),
  EMPTY_HOWTO (80),
  EMPTY_HOWTO (81),
  EMPTY_HOWTO (82),
  EMPTY_HOWTO (83),
  EMPTY_HOWTO (84),
  EMPTY_HOWTO (85),
  EMPTY_HOWTO (86),
  EMPTY_HOWTO (87),
  EMPTY_HOWTO (88),
  EMPTY_HOWTO (89),
  EMPTY_HOWTO (90),
  EMPTY_HOWTO (91),
  EMPTY_HOWTO (92),
  EMPTY_HOWTO (93),
  EMPTY_HOWTO (94),
  EMPTY_HOWTO (95),
  EMPTY_HOWTO (96),
  EMPTY_HOWTO (97),
  EMPTY_HOWTO (98),
  EMPTY_HOWTO (99),
  EMPTY_HOWTO (100),
  EMPTY_HOWTO (101),
  EMPTY_HOWTO (102),
  EMPTY_HOWTO (103),
  EMPTY_HOWTO (104),
  EMPTY_HOWTO (105),
  EMPTY_HOWTO (106),
  EMPTY_HOWTO (107),
  EMPTY_HOWTO (108),
  EMPTY_HOWTO (109),
  EMPTY_HOWTO (110),
  EMPTY_HOWTO (111),
  EMPTY_HOWTO (112),
  EMPTY_HOWTO (113),
  EMPTY_HOWTO (114),
  EMPTY_HOWTO (115),
  EMPTY_HOWTO (116),
  EMPTY_HOWTO (117),
  EMPTY_HOWTO (118),
  EMPTY_HOWTO (119),
  EMPTY_HOWTO (120),
  EMPTY_HOWTO (121),
  EMPTY_HOWTO (122),
  EMPTY_HOWTO (123),
  EMPTY_HOWTO (124),
  EMPTY_HOWTO (125),
  EMPTY_HOWTO (126),
  EMPTY_HOWTO (127),
  EMPTY_HOWTO (128),
  EMPTY_HOWTO (129),
  EMPTY_HOWTO (130),
  EMPTY_HOWTO (131),
  EMPTY_HOWTO (132),
  EMPTY_HOWTO (133),
  EMPTY_HOWTO (134),
  EMPTY_HOWTO (135),
  EMPTY_HOWTO (136),
  EMPTY_HOWTO (137),
  EMPTY_HOWTO (138),
  EMPTY_HOWTO (139),
  EMPTY_HOWTO (140),
  EMPTY_HOWTO (141),
  EMPTY_HOWTO (142),
  EMPTY_HOWTO (143),
  EMPTY_HOWTO (144),
  EMPTY_HOWTO (145),
  EMPTY_HOWTO (146),
  EMPTY_HOWTO (147),
  EMPTY_HOWTO (148),
  EMPTY_HOWTO (149),
  EMPTY_HOWTO (150),
  EMPTY_HOWTO (151),
  EMPTY_HOWTO (152),
  EMPTY_HOWTO (153),
  EMPTY_HOWTO (154),
  EMPTY_HOWTO (155),
  EMPTY_HOWTO (156),
  EMPTY_HOWTO (157),
  EMPTY_HOWTO (158),
  EMPTY_HOWTO (159),
  EMPTY_HOWTO (160),
  EMPTY_HOWTO (161),
  EMPTY_HOWTO (162),
  EMPTY_HOWTO (163),
  EMPTY_HOWTO (164),
  EMPTY_HOWTO (165),
  EMPTY_HOWTO (166),
  EMPTY_HOWTO (167),
  EMPTY_HOWTO (168),
  EMPTY_HOWTO (169),
  EMPTY_HOWTO (170),
  EMPTY_HOWTO (171),
  EMPTY_HOWTO (172),
  EMPTY_HOWTO (173),
  EMPTY_HOWTO (174),
  EMPTY_HOWTO (175),
  EMPTY_HOWTO (176),
  EMPTY_HOWTO (177),
  EMPTY_HOWTO (178),
  EMPTY_HOWTO (179),
  EMPTY_HOWTO (180),
  EMPTY_HOWTO (181),
  EMPTY_HOWTO (182),
  EMPTY_HOWTO (183),
  EMPTY_HOWTO (184),
  EMPTY_HOWTO (185),
  EMPTY_HOWTO (186),
  EMPTY_HOWTO (187),
  EMPTY_HOWTO (188),
  EMPTY_HOWTO (189),
  EMPTY_HOWTO (190),
  EMPTY_HOWTO (191),
  EMPTY_HOWTO (192),
  EMPTY_HOWTO (193),
  EMPTY_HOWTO (194),
  EMPTY_HOWTO (195),
  EMPTY_HOWTO (196),
  EMPTY_HOWTO (197),
  EMPTY_HOWTO (198),
  EMPTY_HOWTO (199),
  EMPTY_HOWTO (200),
  EMPTY_HOWTO (201),
  EMPTY_HOWTO (202),
  EMPTY_HOWTO (203),
  EMPTY_HOWTO (204),
  EMPTY_HOWTO (205),
  EMPTY_HOWTO (206),
  EMPTY_HOWTO (207),
  EMPTY_HOWTO (208),
  EMPTY_HOWTO (209),
  EMPTY_HOWTO (210),
  EMPTY_HOWTO (211),
  EMPTY_HOWTO (212),
  EMPTY_HOWTO (213),
  EMPTY_HOWTO (214),
  EMPTY_HOWTO (215),
  EMPTY_HOWTO (216),
  EMPTY_HOWTO (217),
  EMPTY_HOWTO (218),
  EMPTY_HOWTO (219),
  EMPTY_HOWTO (220),
  EMPTY_HOWTO (221),
  EMPTY_HOWTO (222),
  EMPTY_HOWTO (223),
  EMPTY_HOWTO (224),
  EMPTY_HOWTO (225),
  EMPTY_HOWTO (226),
  EMPTY_HOWTO (227),
  EMPTY_HOWTO (228),
  EMPTY_HOWTO (229),
  EMPTY_HOWTO (230),
  EMPTY_HOWTO (231),
  EMPTY_HOWTO (232),
  EMPTY_HOWTO (233),
  EMPTY_HOWTO (234),
  EMPTY_HOWTO (235),
  EMPTY_HOWTO (236),
  EMPTY_HOWTO (237),
  EMPTY_HOWTO (238),
  EMPTY_HOWTO (239),
  EMPTY_HOWTO (240),
  EMPTY_HOWTO (241),
  EMPTY_HOWTO (242),
  EMPTY_HOWTO (243),
  EMPTY_HOWTO (244),
  EMPTY_HOWTO (245),
  EMPTY_HOWTO (246),
  EMPTY_HOWTO (247),
  EMPTY_HOWTO (248),
  EMPTY_HOWTO (249),
  EMPTY_HOWTO (250),
  EMPTY_HOWTO (251),
  EMPTY_HOWTO (252),
  HOWTO (R_C6000_ALIGN,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_ALIGN",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_FPHEAD,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_FPHEAD",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */
  HOWTO (R_C6000_NOCMP,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_C6000_NOCMP",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE)			/* pcrel_offset */
};

/* Map BFD relocations to ELF relocations.  */

typedef struct
{
  bfd_reloc_code_real_type bfd_reloc_val;
  enum elf_tic6x_reloc_type elf_reloc_val;
} tic6x_reloc_map;

static const tic6x_reloc_map elf32_tic6x_reloc_map[] =
  {
    { BFD_RELOC_NONE, R_C6000_NONE },
    { BFD_RELOC_32, R_C6000_ABS32 },
    { BFD_RELOC_16, R_C6000_ABS16 },
    { BFD_RELOC_8, R_C6000_ABS8 },
    { BFD_RELOC_C6000_PCR_S21, R_C6000_PCR_S21 },
    { BFD_RELOC_C6000_PCR_S12, R_C6000_PCR_S12 },
    { BFD_RELOC_C6000_PCR_S10, R_C6000_PCR_S10 },
    { BFD_RELOC_C6000_PCR_S7, R_C6000_PCR_S7 },
    { BFD_RELOC_C6000_ABS_S16, R_C6000_ABS_S16 },
    { BFD_RELOC_C6000_ABS_L16, R_C6000_ABS_L16 },
    { BFD_RELOC_C6000_ABS_H16, R_C6000_ABS_H16 },
    { BFD_RELOC_C6000_SBR_U15_B, R_C6000_SBR_U15_B },
    { BFD_RELOC_C6000_SBR_U15_H, R_C6000_SBR_U15_H },
    { BFD_RELOC_C6000_SBR_U15_W, R_C6000_SBR_U15_W },
    { BFD_RELOC_C6000_SBR_S16, R_C6000_SBR_S16 },
    { BFD_RELOC_C6000_SBR_L16_B, R_C6000_SBR_L16_B },
    { BFD_RELOC_C6000_SBR_L16_H, R_C6000_SBR_L16_H },
    { BFD_RELOC_C6000_SBR_L16_W, R_C6000_SBR_L16_W },
    { BFD_RELOC_C6000_SBR_H16_B, R_C6000_SBR_H16_B },
    { BFD_RELOC_C6000_SBR_H16_H, R_C6000_SBR_H16_H },
    { BFD_RELOC_C6000_SBR_H16_W, R_C6000_SBR_H16_W },
    { BFD_RELOC_C6000_SBR_GOT_U15_W, R_C6000_SBR_GOT_U15_W },
    { BFD_RELOC_C6000_SBR_GOT_L16_W, R_C6000_SBR_GOT_L16_W },
    { BFD_RELOC_C6000_SBR_GOT_H16_W, R_C6000_SBR_GOT_H16_W },
    { BFD_RELOC_C6000_DSBT_INDEX, R_C6000_DSBT_INDEX },
    { BFD_RELOC_C6000_PREL31, R_C6000_PREL31 },
    { BFD_RELOC_C6000_COPY, R_C6000_COPY },
    { BFD_RELOC_C6000_ALIGN, R_C6000_ALIGN },
    { BFD_RELOC_C6000_FPHEAD, R_C6000_FPHEAD },
    { BFD_RELOC_C6000_NOCMP, R_C6000_NOCMP }
  };

static reloc_howto_type *
elf32_tic6x_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			       bfd_reloc_code_real_type code)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (elf32_tic6x_reloc_map); i++)
    if (elf32_tic6x_reloc_map[i].bfd_reloc_val == code)
      return &elf32_tic6x_howto_table[elf32_tic6x_reloc_map[i].elf_reloc_val];

  return NULL;
}

static reloc_howto_type *
elf32_tic6x_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED, const char *r_name)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (elf32_tic6x_howto_table); i++)
    if (elf32_tic6x_howto_table[i].name != NULL
	&& strcasecmp (elf32_tic6x_howto_table[i].name, r_name) == 0)
      return &elf32_tic6x_howto_table[i];

  return NULL;
}

static void
elf32_tic6x_info_to_howto (bfd *abfd ATTRIBUTE_UNUSED, arelent *bfd_reloc,
			   Elf_Internal_Rela *elf_reloc)
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (elf_reloc->r_info);
  if (r_type >= ARRAY_SIZE (elf32_tic6x_howto_table))
    bfd_reloc->howto = NULL;
  else
    bfd_reloc->howto = &elf32_tic6x_howto_table[r_type];
}

static bfd_boolean
elf32_tic6x_relocate_section (bfd *output_bfd,
			      struct bfd_link_info *info,
			      bfd *input_bfd,
			      asection *input_section,
			      bfd_byte *contents,
			      Elf_Internal_Rela *relocs,
			      Elf_Internal_Sym *local_syms,
			      asection **local_sections)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;
  bfd_boolean ok = TRUE;

  symtab_hdr = & elf_symtab_hdr (input_bfd);
  sym_hashes = elf_sym_hashes (input_bfd);

  relend = relocs + input_section->reloc_count;

  for (rel = relocs; rel < relend; rel ++)
    {
      int r_type;
      unsigned long r_symndx;
      arelent bfd_reloc;
      reloc_howto_type *howto;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_boolean unresolved_reloc;
      bfd_reloc_status_type r;
      struct bfd_link_hash_entry *sbh;

      r_type = ELF32_R_TYPE (rel->r_info);
      r_symndx = ELF32_R_SYM (rel->r_info);

      elf32_tic6x_info_to_howto (input_bfd, &bfd_reloc, rel);
      howto = bfd_reloc.howto;
      if (howto == NULL)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return FALSE;
	}

      h = NULL;
      sym = NULL;
      sec = NULL;
      unresolved_reloc = FALSE;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);
	}
      else
	{
	  bfd_boolean warned;

	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes,
				   h, sec, relocation,
				   unresolved_reloc, warned);
	}

      if (sec != NULL && elf_discarded_section (sec))
	{
	  /* For relocs against symbols from removed linkonce sections,
	     or sections discarded by a linker script, we just want the
	     section contents zeroed.  Avoid any special processing.  */
	  _bfd_clear_contents (howto, input_bfd, contents + rel->r_offset);
	  rel->r_info = 0;
	  rel->r_addend = 0;
	  continue;
	}

      if (info->relocatable)
	continue;

      switch (r_type)
	{
	case R_C6000_NONE:
	case R_C6000_ALIGN:
	case R_C6000_FPHEAD:
	case R_C6000_NOCMP:
	  /* No action needed.  */
	  continue;

	case R_C6000_PCR_S21:
	case R_C6000_PCR_S12:
	case R_C6000_PCR_S10:
	case R_C6000_PCR_S7:
	  /* Generic PC-relative handling produces a value relative to
	     the exact location of the relocation.  Adjust it to be
	     relative to the start of the fetch packet instead.  */
	  relocation += (input_section->output_section->vma
			 + input_section->output_offset
			 + rel->r_offset) & 0x1f;
	  /* Fall through.  */
	case R_C6000_ABS32:
	case R_C6000_ABS16:
	case R_C6000_ABS8:
	case R_C6000_ABS_S16:
	case R_C6000_ABS_L16:
	case R_C6000_ABS_H16:
	  /* Generic logic OK.  */
	  break;

	case R_C6000_SBR_U15_B:
	case R_C6000_SBR_U15_H:
	case R_C6000_SBR_U15_W:
	case R_C6000_SBR_S16:
	case R_C6000_SBR_L16_B:
	case R_C6000_SBR_L16_H:
	case R_C6000_SBR_L16_W:
	case R_C6000_SBR_H16_B:
	case R_C6000_SBR_H16_H:
	case R_C6000_SBR_H16_W:
	  sbh = bfd_link_hash_lookup (info->hash, "__c6xabi_DSBT_BASE",
				      FALSE, FALSE, TRUE);
	  if (sbh != NULL
	      && (sbh->type == bfd_link_hash_defined
		  || sbh->type == bfd_link_hash_defweak))
	    relocation -= (sbh->u.def.value
			   + sbh->u.def.section->output_section->vma
			   + sbh->u.def.section->output_offset);
	  else
	    {
	      (*_bfd_error_handler) (_("%B: SB-relative relocation but "
				       "__c6xabi_DSBT_BASE not defined"),
				     input_bfd);
	      ok = FALSE;
	      continue;
	    }
	  break;

	case R_C6000_SBR_GOT_U15_W:
	case R_C6000_SBR_GOT_L16_W:
	case R_C6000_SBR_GOT_H16_W:
	case R_C6000_DSBT_INDEX:
	case R_C6000_PREL31:
	  /* Shared libraries and exception handling support not
	     implemented.  */
	  (*_bfd_error_handler) (_("%B: relocation type %d not implemented"),
				 input_bfd, r_type);
	  ok = FALSE;
	  continue;

	case R_C6000_COPY:
	  /* Invalid in relocatable object.  */
	default:
	  /* Unknown relocation.  */
	  (*_bfd_error_handler) (_("%B: invalid relocation type %d"),
				 input_bfd, r_type);
	  ok = FALSE;
	  continue;
	}

      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
				    contents, rel->r_offset,
				    relocation, rel->r_addend);

      if (r == bfd_reloc_ok
	  && howto->complain_on_overflow == complain_overflow_bitfield)
	{
	  /* Generic overflow handling accepts cases the ABI says
	     should be rejected for R_C6000_ABS16 and
	     R_C6000_ABS8.  */
	  bfd_vma value = (relocation + rel->r_addend) & 0xffffffff;
	  bfd_vma sbit = 1 << (howto->bitsize - 1);
	  bfd_vma sbits = (-(bfd_vma) sbit) & 0xffffffff;
	  bfd_vma value_sbits = value & sbits;

	  if (value_sbits != 0
	      && value_sbits != sbit
	      && value_sbits != sbits)
	    r = bfd_reloc_overflow;
	}

      if (r != bfd_reloc_ok)
	{
	  const char *name;
	  const char *error_message;

	  if (h != NULL)
	    name = h->root.root.string;
	  else
	    {
	      name = bfd_elf_string_from_elf_section (input_bfd,
						      symtab_hdr->sh_link,
						      sym->st_name);
	      if (name == NULL)
		return FALSE;
	      if (*name == '\0')
		name = bfd_section_name (input_bfd, sec);
	    }

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      /* If the overflowing reloc was to an undefined symbol,
		 we have already printed one error message and there
		 is no point complaining again.  */
	      if ((! h ||
		   h->root.type != bfd_link_hash_undefined)
		  && (!((*info->callbacks->reloc_overflow)
			(info, (h ? &h->root : NULL), name, howto->name,
			 (bfd_vma) 0, input_bfd, input_section,
			 rel->r_offset))))
		  return FALSE;
	      break;

	    case bfd_reloc_undefined:
	      if (!((*info->callbacks->undefined_symbol)
		    (info, name, input_bfd, input_section,
		     rel->r_offset, TRUE)))
		return FALSE;
	      break;

	    case bfd_reloc_outofrange:
	      error_message = _("out of range");
	      goto common_error;

	    case bfd_reloc_notsupported:
	      error_message = _("unsupported relocation");
	      goto common_error;

	    case bfd_reloc_dangerous:
	      error_message = _("dangerous relocation");
	      goto common_error;

	    default:
	      error_message = _("unknown error");
	      /* Fall through.  */

	    common_error:
	      BFD_ASSERT (error_message != NULL);
	      if (!((*info->callbacks->reloc_dangerous)
		    (info, error_message, input_bfd, input_section,
		     rel->r_offset)))
		return FALSE;
	      break;
	    }
	}
    }

  return ok;
}


#define TARGET_LITTLE_SYM	bfd_elf32_tic6x_le_vec
#define TARGET_LITTLE_NAME	"elf32-tic6x-le"
#define TARGET_BIG_SYM		bfd_elf32_tic6x_be_vec
#define TARGET_BIG_NAME		"elf32-tic6x-be"
#define ELF_ARCH		bfd_arch_tic6x
#define ELF_MACHINE_CODE	EM_TI_C6000
#define ELF_MAXPAGESIZE		1
#define bfd_elf32_bfd_reloc_type_lookup elf32_tic6x_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup elf32_tic6x_reloc_name_lookup
#define elf_backend_can_gc_sections	1
#define elf_backend_default_use_rela_p	1
#define elf_backend_may_use_rel_p	1
#define elf_backend_may_use_rela_p	1
#define elf_backend_rela_normal		1
#define elf_backend_relocate_section	elf32_tic6x_relocate_section
#define elf_info_to_howto		elf32_tic6x_info_to_howto
#define elf_info_to_howto_rel		_bfd_elf_no_info_to_howto

#include "elf32-target.h"
