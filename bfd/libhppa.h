/* HP PA-RISC SOM object file format:  definitions internal to BFD.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

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

#ifndef _HPPA_H
#define _HPPA_H

#define BYTES_IN_WORD 4

#ifndef INLINE
#ifdef __GNUC__
#define INLINE inline
#else
#define INLINE
#endif /* GNU C? */
#endif /* INLINE */

/* HP PA-RISC relocation types */

enum hppa_reloc_field_selector_type
  {
    R_HPPA_FSEL = 0x0,
    R_HPPA_LSSEL = 0x1,
    R_HPPA_RSSEL = 0x2,
    R_HPPA_LSEL = 0x3,
    R_HPPA_RSEL = 0x4,
    R_HPPA_LDSEL = 0x5,
    R_HPPA_RDSEL = 0x6,
    R_HPPA_LRSEL = 0x7,
    R_HPPA_RRSEL = 0x8,
    R_HPPA_PSEL = 0x9,
    R_HPPA_LPSEL = 0xa,
    R_HPPA_RPSEL = 0xb,
    R_HPPA_TSEL = 0xc,
    R_HPPA_LTSEL = 0xd,
    R_HPPA_RTSEL = 0xe
  };

/* for compatibility */
enum hppa_reloc_field_selector_type_alt
  {
    e_fsel = R_HPPA_FSEL,
    e_lssel = R_HPPA_LSSEL,
    e_rssel = R_HPPA_RSSEL,
    e_lsel = R_HPPA_LSEL,
    e_rsel = R_HPPA_RSEL,
    e_ldsel = R_HPPA_LDSEL,
    e_rdsel = R_HPPA_RDSEL,
    e_lrsel = R_HPPA_LRSEL,
    e_rrsel = R_HPPA_RRSEL,
    e_psel = R_HPPA_PSEL,
    e_lpsel = R_HPPA_LPSEL,
    e_rpsel = R_HPPA_RPSEL,
    e_tsel = R_HPPA_TSEL,
    e_ltsel = R_HPPA_LTSEL,
    e_rtsel = R_HPPA_RTSEL
  };

enum hppa_reloc_expr_type
  {
    R_HPPA_E_ONE = 0,
    R_HPPA_E_TWO = 1,
    R_HPPA_E_PCREL = 2,
    R_HPPA_E_CON = 3,
    R_HPPA_E_PLABEL = 7,
    R_HPPA_E_ABS = 18
  };

/* for compatibility */
enum hppa_reloc_expr_type_alt
  {
    e_one = R_HPPA_E_ONE,
    e_two = R_HPPA_E_TWO,
    e_pcrel = R_HPPA_E_PCREL,
    e_con = R_HPPA_E_CON,
    e_plabel = R_HPPA_E_PLABEL,
    e_abs = R_HPPA_E_ABS
  };


/* Some functions to manipulate PA instructions.  */
static INLINE unsigned int
assemble_3 (x)
     unsigned int x;
{
  return (((x & 1) << 2) | ((x & 6) >> 1)) & 7;
}

static INLINE void
dis_assemble_3 (x, r)
     unsigned int x;
     unsigned int *r;
{
  *r = (((x & 4) >> 2) | ((x & 3) << 1)) & 7;
}

static INLINE unsigned int
assemble_12 (x, y)
     unsigned int x, y;
{
  return (((y & 1) << 11) | ((x & 1) << 10) | ((x & 0x7fe) >> 1)) & 0xfff;
}

static INLINE void
dis_assemble_12 (as12, x, y)
     unsigned int as12;
     unsigned int *x, *y;
{
  *y = (as12 & 0x800) >> 11;
  *x = ((as12 & 0x3ff) << 1) | ((as12 & 0x400) >> 10);
}

static INLINE unsigned long
assemble_17 (x, y, z)
     unsigned int x, y, z;
{
  unsigned long temp;

  temp = ((z & 1) << 16) |
    ((x & 0x1f) << 11) |
    ((y & 1) << 10) |
    ((y & 0x7fe) >> 1);
  return temp & 0x1ffff;
}

static INLINE void
dis_assemble_17 (as17, x, y, z)
     unsigned int as17;
     unsigned int *x, *y, *z;
{

  *z = (as17 & 0x10000) >> 16;
  *x = (as17 & 0x0f800) >> 11;
  *y = (((as17 & 0x00400) >> 10) | ((as17 & 0x3ff) << 1)) & 0x7ff;
}

static INLINE unsigned long
assemble_21 (x)
     unsigned int x;
{
  unsigned long temp;

  temp = ((x & 1) << 20) |
    ((x & 0xffe) << 8) |
    ((x & 0xc000) >> 7) |
    ((x & 0x1f0000) >> 14) |
    ((x & 0x003000) >> 12);
  return temp & 0x1fffff;
}

static INLINE void
dis_assemble_21 (as21, x)
     unsigned int as21, *x;
{
  unsigned long temp;


  temp = (as21 & 0x100000) >> 20;
  temp |= (as21 & 0x0ffe00) >> 8;
  temp |= (as21 & 0x000180) << 7;
  temp |= (as21 & 0x00007c) << 14;
  temp |= (as21 & 0x000003) << 12;
  *x = temp;
}

static INLINE unsigned long
sign_ext (x, len)
     unsigned int x, len;
{
  return (x << (32 - len)) >> (32 - len);
}

static INLINE unsigned int
ones (n)
     int n;
{
  unsigned int len_ones;
  int i;

  i = 0;
  len_ones = 0;
  while (i < n)
    {
      len_ones = (len_ones << 1) | 1;
      i++;
    }

  return len_ones;
}

static INLINE void
sign_unext (x, len, result)
     unsigned int x, len;
     unsigned int *result;
{
  unsigned int len_ones;

  len_ones = ones (len);

  *result = x & len_ones;
}

static INLINE unsigned long
low_sign_ext (x, len)
     unsigned int x, len;
{
  unsigned int temp1, temp2;
  unsigned int len_ones;

  len_ones = ones (len);

  temp1 = (x & 1) << (len - 1);
  temp2 = ((x & 0xfffffffe) & len_ones) >> 1;
  return sign_ext ((temp1 | temp2), len);
}

static INLINE void
low_sign_unext (x, len, result)
     unsigned int x, len;
     unsigned int *result;
{
  unsigned int temp;
  unsigned int sign;
  unsigned int rest;
  unsigned int one_bit_at_len;
  unsigned int len_ones;

  len_ones = ones (len);
  one_bit_at_len = 1 << (len - 1);

  sign_unext (x, len, &temp);
  sign = temp & one_bit_at_len;
  sign >>= (len - 1);

  rest = temp & (len_ones ^ one_bit_at_len);
  rest <<= 1;

  *result = rest | sign;
}

/* Handle field selectors for PA instructions.  */

static INLINE unsigned long
hppa_field_adjust (value, constant_value, r_field)
     unsigned long value;
     unsigned long constant_value;
     unsigned short r_field;
{
  unsigned long init_value = value;
  value += constant_value;
  switch (r_field)
    {
    case e_fsel:		/* F  : no change                      */
      break;

    case e_lssel:		/* LS : if (bit 21) then add 0x800
				   arithmetic shift right 11 bits */
      if (value & 0x00000400)
	value += 0x800;
      value = (value & 0xfffff800) >> 11;
      break;

    case e_rssel:		/* RS : Sign extend from bit 21        */
      if (value & 0x00000400)
	value |= 0xfffff800;
      else
	value &= 0x7ff;
      break;

    case e_lsel:		/* L  : Arithmetic shift right 11 bits */
      value = (value & 0xfffff800) >> 11;
      break;

    case e_rsel:		/* R  : Set bits 0-20 to zero          */
      value = value & 0x7ff;
      break;

    case e_ldsel:		/* LD : Add 0x800, arithmetic shift
				   right 11 bits                  */
      value += 0x800;
      value = (value & 0xfffff800) >> 11;
      break;

    case e_rdsel:		/* RD : Set bits 0-20 to one           */
      value |= 0xfffff800;
      break;

    case e_lrsel:		/* LR : L with "rounded" constant      */
      value = value + ((constant_value + 0x1000) & 0xffffe000);
      value = (value & 0xfffff800) >> 11;
      break;

    case e_rrsel:		/* RR : R with "rounded" constant      */
      value = value + ((constant_value + 0x1000) & 0xffffe000);
      value = (value & 0x7ff) + constant_value - ((constant_value + 0x1000) & 0xffffe000);
      break;

    default:
      abort ();
    }
  return value;

}
#endif /* _HPPA_H */
