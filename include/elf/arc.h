/* ARC ELF support for BFD.
   Copyright (C) 1995 Free Software Foundation, Inc.

   Copied from mips.h.

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

/* This file holds definitions specific to the ARC ELF ABI.  */

/* Processor specific flags for the ELF header e_flags field.  */

/* File contains position independent code.  */
#define EF_ARC_PIC		0x00000001

/* Four bit ARC architecture field.  */
#define EF_MIPS_ARCH		0xf0000000

/* Base ARC.  */
#define E_ARC_ARCH_BASE		0x00000000

/* Host ARC.  */
#define E_ARC_ARCH_HOST		0x10000000

/* Graphics ARC.  */
#define E_ARC_ARCH_GRAPHICS	0x20000000

/* Audio ARC.  */
#define E_ARC_ARCH_AUDIO	0x30000000

/* Processor specific section types.  */

/* Section contains the global pointer table.  */
#define SHT_ARC_GPTAB		0x70000000

/* Processor specific section flags.  */

/* This section must be in the global data area.  */
#define SHF_ARC_GPREL		0x10000000

/* Processor specific program header types.  */

/* Register usage information.  Identifies one .reginfo section.  */
#define PT_ARC_REGINFO		0x70000000

/* Processor specific dynamic array tags.  */

/* 32 bit version number for runtime linker interface.  */
#define DT_ARC_RLD_VERSION	0x70000001

/* Time stamp.  */
#define DT_ARC_TIME_STAMP	0x70000002

/* Index of version string in string table.  */
#define DT_ARC_IVERSION		0x70000003

/* 32 bits of flags.  */
#define DT_ARC_FLAGS		0x70000004
