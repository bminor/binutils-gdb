
/* m88k.h -- Assembler for the Motorola 88000
   Contributed by Devon Bowen of Buffalo University
   and Torbjorn Granlund of the Swedish Institute of Computer Science.
   Copyright (C) 1989, 1990, 1991 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler.

GAS is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define NEED_FX_R_TYPE

/* different type of relocation available in the m88k */

enum reloc_type
{
  RELOC_LO16,			/* lo16(sym) */
  RELOC_HI16,			/* hi16(sym) */
  RELOC_PC16,			/* bb0, bb1, bcnd */
  RELOC_PC26,			/* br, bsr */
  RELOC_32,			/* jump tables, etc */
  RELOC_IW16,			/* global access through linker regs 28 */
  NO_RELOC
};

struct reloc_info_m88k
{
  unsigned long int r_address;
  unsigned int r_symbolnum:24;
  unsigned int r_extern:1;
  unsigned int r_pad:3;
  enum reloc_type r_type:4;
  long int r_addend;
};

#define relocation_info reloc_info_m88k

#define LOCAL_LABEL(name) (name[0] =='@' \
                         && ( name [1] == 'L' || name [1] == '.' ))

#ifndef BFD_ASSEMBLER
#define md_convert_frag(h,f)		{as_fatal ("m88k convert_frag\n");}
#else
#define md_convert_frag(b,s,f)		{as_fatal ("m88k convert_frag\n");}
#endif
