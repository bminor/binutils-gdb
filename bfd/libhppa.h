/* HP PA-RISC SOM object file format:  definitions internal to BFD.
   Copyright (C) 1990, 91, 92, 93, 94, 95, 96, 98, 99, 2000
   Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _HPPA_H
#define _HPPA_H

#define BYTES_IN_WORD 4
#define PA_PAGESIZE 0x1000

#ifndef INLINE
#ifdef __GNUC__
#define INLINE inline
#else
#define INLINE
#endif /* GNU C? */
#endif /* INLINE */

/* The PA instruction set variants.  */
enum pa_arch {pa10 = 10, pa11 = 11, pa20 = 20, pa20w = 25};

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
    R_HPPA_NSEL  = 0x9,
    R_HPPA_NLSEL  = 0xa,
    R_HPPA_NLRSEL  = 0xb,
    R_HPPA_PSEL = 0xc,
    R_HPPA_LPSEL = 0xd,
    R_HPPA_RPSEL = 0xe,
    R_HPPA_TSEL = 0xf,
    R_HPPA_LTSEL = 0x10,
    R_HPPA_RTSEL = 0x11,
    R_HPPA_LTPSEL = 0x12,
    R_HPPA_RTPSEL = 0x13
  };

/* /usr/include/reloc.h defines these to constants.  We want to use
   them in enums, so #undef them before we start using them.  We might
   be able to fix this another way by simply managing not to include
   /usr/include/reloc.h, but currently GDB picks up these defines
   somewhere.  */
#undef e_fsel
#undef e_lssel
#undef e_rssel
#undef e_lsel
#undef e_rsel
#undef e_ldsel
#undef e_rdsel
#undef e_lrsel
#undef e_rrsel
#undef e_nsel
#undef e_nlsel
#undef e_nlrsel
#undef e_psel
#undef e_lpsel
#undef e_rpsel
#undef e_tsel
#undef e_ltsel
#undef e_rtsel
#undef e_one
#undef e_two
#undef e_pcrel
#undef e_con
#undef e_plabel
#undef e_abs

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
    e_nsel = R_HPPA_NSEL,
    e_nlsel = R_HPPA_NLSEL,
    e_nlrsel = R_HPPA_NLRSEL,
    e_psel = R_HPPA_PSEL,
    e_lpsel = R_HPPA_LPSEL,
    e_rpsel = R_HPPA_RPSEL,
    e_tsel = R_HPPA_TSEL,
    e_ltsel = R_HPPA_LTSEL,
    e_rtsel = R_HPPA_RTSEL,
    e_ltpsel = R_HPPA_LTPSEL,
    e_rtpsel = R_HPPA_RTPSEL
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


/* Relocations for function calls must be accompanied by parameter
   relocation bits.  These bits describe exactly where the caller has
   placed the function's arguments and where it expects to find a return
   value.

   Both ELF and SOM encode this information within the addend field
   of the call relocation.  (Note this could break very badly if one
   was to make a call like bl foo + 0x12345678).

   The high order 10 bits contain parameter relocation information,
   the low order 22 bits contain the constant offset.  */

#define HPPA_R_ARG_RELOC(a)	\
  (((a) >> 22) & 0x3ff)
#define HPPA_R_CONSTANT(a)	\
  ((((bfd_signed_vma)(a)) << (BFD_ARCH_SIZE-22)) >> (BFD_ARCH_SIZE-22))
#define HPPA_R_ADDEND(r, c)	\
  (((r) << 22) + ((c) & 0x3fffff))
#define HPPA_WIDE	       (0) /* PSW W-bit, need to check! FIXME */

/* These macros get bit fields using HP's numbering (MSB = 0),
 * but note that "MASK" assumes that the LSB bits are what's
 * wanted.
 */
#ifndef GET_FIELD
#define GET_FIELD(X, FROM, TO) \
  ((X) >> (31 - (TO)) & ((1 << ((TO) - (FROM) + 1)) - 1))
#endif
#define GET_BIT(X, WHICH) \
  GET_FIELD (X, WHICH, WHICH)

#define MASK(SIZE) \
  (~((-1) << SIZE))

#define CATENATE(X, XSIZE, Y, YSIZE) \
  (((X & MASK (XSIZE)) << YSIZE) | (Y & MASK (YSIZE)))

#define ELEVEN(X) \
  CATENATE (GET_BIT (X, 10), 1, GET_FIELD (X, 0, 9), 10)

/* Some functions to manipulate PA instructions.  */

/* NOTE: these use the HP convention that f{0} is the _left_ most
 *       bit (MSB) of f; they sometimes have to impose an assumption
 *       about the size of a field; and as far as I can tell, most
 *       aren't used.
 */

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
/* Declare the functions with the unused attribute to avoid warnings.  */
static INLINE unsigned int sign_extend (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int low_sign_extend (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int assemble_3 (unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int assemble_6 (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int assemble_12 (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int assemble_16 (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int assemble_16a (unsigned int, unsigned int,
					 unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int assemble_17 (unsigned int, unsigned int,
					unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int assemble_21 (unsigned int)
     __attribute ((__unused__));

static INLINE unsigned int sign_unext (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int low_sign_unext (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int re_assemble_3 (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int re_assemble_12 (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int re_assemble_16 (unsigned int, unsigned int, int)
     __attribute__ ((__unused__));
static INLINE unsigned int re_assemble_17 (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int re_assemble_22 (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE unsigned int re_assemble_21 (unsigned int, unsigned int)
     __attribute__ ((__unused__));
static INLINE bfd_signed_vma hppa_field_adjust (bfd_signed_vma, bfd_signed_vma,
						enum hppa_reloc_field_selector_type_alt)
     __attribute__ ((__unused__));
static INLINE int bfd_hppa_insn2fmt (unsigned int)
     __attribute__ ((__unused__));
static INLINE  unsigned int hppa_rebuild_insn (bfd *, unsigned int,
					       unsigned int, unsigned int)
     __attribute__ ((__unused__));
#endif /* gcc 2.7 or higher */


/* The *sign_extend and assemble_* functions are used to assemble
   various bitfields taken from an instruction and return the
   resulting immediate value.  They correspond to functions by the
   same name in HP's PA-RISC 2.0 Architecture Reference Manual.  */

static INLINE unsigned int
sign_extend (x, len)
     unsigned int x, len;
{
  unsigned int signbit = (1 << (len - 1));
  unsigned int mask = (signbit << 1) - 1;
  return ((x & mask) ^ signbit) - signbit;
}

static INLINE unsigned int
low_sign_extend (x, len)
     unsigned int x, len;
{
  return (x >> 1) - ((x & 1) << (len - 1));
}

static INLINE unsigned int
assemble_3 (x)
     unsigned int x;
{
  return CATENATE (GET_BIT (x, 2), 1, GET_FIELD (x, 0, 1), 2);
}

static INLINE unsigned int
assemble_6 (x, y)
     unsigned int x, y;
{
  return (((x & 1) << 5) + (32 - (y & 0x1f)));
}

static INLINE unsigned int
assemble_12 (x, y)
     unsigned int x, y;
{
  return CATENATE (CATENATE (y, 1, GET_BIT (x, 10), 1), 2,
 		   GET_FIELD (x, 0, 9), 9);
}

static INLINE unsigned int
assemble_16 (x, y)
     unsigned int x, y;
{
  /* Depends on PSW W-bit !*/
  unsigned int temp;

  if (HPPA_WIDE)
    temp = CATENATE (CATENATE (GET_BIT (y, 13), 1,
			       (GET_BIT (y, 13) ^ GET_BIT (x, 0)), 1), 2,
		     CATENATE ((GET_BIT (y, 13) ^ GET_BIT (x, 1)), 1,
			       GET_FIELD (y, 0, 12), 13), 14);
  else
    temp = CATENATE (CATENATE (GET_BIT (y, 13), 1, GET_BIT (y, 13), 1), 2,
		     CATENATE (GET_BIT (y, 13), 1, GET_FIELD (y, 0, 12), 13), 14);

  return sign_extend (temp, 16);
}

static INLINE unsigned int
assemble_16a (x, y, z)
     unsigned int x, y, z;
{
  /* Depends on PSW W-bit !*/
  unsigned int temp;

  if (HPPA_WIDE)
    temp = CATENATE (CATENATE (z, 1, (z ^ GET_BIT (x, 0)), 1), 2,
		     CATENATE ((z ^ GET_BIT (x, 1)), 1, y, 11), 12);
  else
    temp = CATENATE (CATENATE (z, 1, z, 1), 2, CATENATE (z, 1, y, 11), 12);

  return sign_extend ((temp << 2), 16);
}

static INLINE unsigned int
assemble_17 (x, y, z)
     unsigned int x, y, z;
{
  unsigned int temp;

  temp = CATENATE (CATENATE (z, 1, x, 5), 6,
		   CATENATE (GET_BIT (y, 10), 1, GET_FIELD (y, 0, 9), 10), 11);

  return temp;
}

static INLINE unsigned int
assemble_21 (x)
     unsigned int x;
{
  unsigned int temp;

  temp = ((  (x & 0x000001) << 20)
	  | ((x & 0x000ffe) << 8)
	  | ((x & 0x003000) >> 12)
	  | ((x & 0x00c000) >> 7)
	  | ((x & 0x1f0000) >> 14));
  return temp;
}

static INLINE unsigned int
assemble_22 (a,b,c,d)
     unsigned int a,b,c,d;
{
  unsigned int temp;

  temp = CATENATE (CATENATE (d, 1, a, 5), 6,
		   CATENATE (b, 5, ELEVEN (c), 11), 16);

  return sign_extend (temp, 22);
}


/* The re_assemble_* functions splice together an opcode and an
   immediate value.  pa-risc uses all sorts of weird bitfields in the
   instruction to hold the value.  */

static INLINE unsigned int
sign_unext (x, len)
     unsigned int x, len;
{
  unsigned int len_ones;

  len_ones = ((unsigned int) 1 << len) - 1;

  return x & len_ones;
}

static INLINE unsigned int
low_sign_unext (x, len)
     unsigned int x, len;
{
  unsigned int temp;
  unsigned int sign;

  sign = (x >> (len-1)) & 1;

  temp = sign_unext (x, len-1);

  return (temp << 1) | sign;
}

static INLINE unsigned int
re_assemble_3 (insn, as3)
     unsigned int insn;
     unsigned int as3;
{
  return (insn
	  | ((as3 & 4) << (13-2))
	  | ((as3 & 3) << (13+1)));
}

static INLINE unsigned int
re_assemble_12 (insn, as12)
     unsigned int insn;
     unsigned int as12;
{
  return (insn
	  | ((as12 & 0x800) >> 11)
	  | ((as12 & 0x400) >> (10 - 2))
	  | ((as12 & 0x3ff) << (1 + 2)));
}

static INLINE unsigned int
re_assemble_16 (insn, as16, wide)
     unsigned int insn;
     unsigned int as16;
     int wide;
{
  unsigned int s, t;

  if (wide)
    {
      /* Unusual 16-bit encoding.  */
      t = (as16 << 1) & 0xffff;
      s = (as16 & 0x8000);
      return insn | (t ^ s ^ (s >> 1)) | (s >> 15);
    }
  else
    {
      /* Standard 14-bit encoding.  */
      t = (as16 << 1) & 0x3fff;
      s = (as16 & 0x2000);
      return insn | t | (s >> 13);
    }
}

static INLINE unsigned int
re_assemble_17 (insn, as17)
     unsigned int insn;
     unsigned int as17;
{
  return (insn
	  | ((as17 & 0x10000) >> 16)
	  | ((as17 & 0x0f800) << (16 - 11))
	  | ((as17 & 0x00400) >> (10 - 2))
	  | ((as17 & 0x003ff) << (1 + 2)));
}

static INLINE unsigned int
re_assemble_21 (insn, as21)
     unsigned int insn;
     unsigned int as21;
{
  return (insn
	  | ((as21 & 0x100000) >> 20)
	  | ((as21 & 0x0ffe00) >> 8)
	  | ((as21 & 0x000180) << 7)
	  | ((as21 & 0x00007c) << 14)
	  | ((as21 & 0x000003) << 12));
}

static INLINE unsigned int
re_assemble_22 (insn, as22)
     unsigned int insn;
     unsigned int as22;
{
  return (insn
	  | ((as22 & 0x200000) >> 21)
	  | ((as22 & 0x1f0000) << (21 - 16))
	  | ((as22 & 0x00f800) << (16 - 11))
	  | ((as22 & 0x000400) >> (10 - 2))
	  | ((as22 & 0x0003ff) << (1 + 2)));
}


/* Handle field selectors for PA instructions.
   The L and R (and LS, RS etc.) selectors are used in pairs to form a
   full 32 bit address.  eg.

   LDIL	L'start,%r1		; put left part into r1
   LDW	R'start(%r1),%r2	; add r1 and right part to form address

   This function returns sign extended values in all cases.
*/

static INLINE bfd_signed_vma
hppa_field_adjust (sym_val, addend, r_field)
     bfd_signed_vma sym_val;
     bfd_signed_vma addend;
     enum hppa_reloc_field_selector_type_alt r_field;
{
  bfd_signed_vma value;

  value = sym_val + addend;
  switch (r_field)
    {
    case e_fsel:
    case e_nsel:
      /* F: No change.  */
      break;

    case e_lsel:
    case e_nlsel:
      /* L:  Select top 21 bits.  */
      value = value >> 11;
      break;

    case e_rsel:
      /* R:  Select bottom 11 bits.  */
      value = value & 0x7ff;
      break;

    case e_lssel:
      /* LS:  Round to nearest multiple of 2048 then select top 21 bits.  */
      value = value + 0x400;
      value = value >> 11;
      break;

    case e_rssel:
      /* RS:  Select bottom 11 bits for LS.
	 We need to return a value such that 2048 * LS'x + RS'x == x.
	 ie. RS'x = x - ((x + 0x400) & -0x800)
	 this is just a sign extension from bit 21.  */
      value = ((value & 0x7ff) ^ 0x400) - 0x400;
      break;

    case e_ldsel:
      /* LD:  Round to next multiple of 2048 then select top 21 bits.
	 Yes, if we are already on a multiple of 2048, we go up to the
	 next one.  RD in this case will be -2048.  */
      value = value + 0x800;
      value = value >> 11;
      break;

    case e_rdsel:
      /* RD:  Set bits 0-20 to one.  */
      value = value | -0x800;
      break;

    case e_lrsel:
    case e_nlrsel:
      /* LR:  L with rounding of the addend to nearest 8k.  */
      value = sym_val + ((addend + 0x1000) & -0x2000);
      value = value >> 11;
      break;

    case e_rrsel:
      /* RR:  R with rounding of the addend to nearest 8k.
	 We need to return a value such that 2048 * LR'x + RR'x == x
	 ie. RR'x = s+a - (s + (((a + 0x1000) & -0x2000) & -0x800))
	 .	  = s+a - ((s & -0x800) + ((a + 0x1000) & -0x2000))
	 .	  = (s & 0x7ff) + a - ((a + 0x1000) & -0x2000)  */
      value = (sym_val & 0x7ff) + (((addend & 0x1fff) ^ 0x1000) - 0x1000);
      break;

    default:
      abort ();
    }
  return value;
}

/* PA-RISC OPCODES */
#define get_opcode(insn)	(((insn) >> 26) & 0x3f)

/* FIXME: this list is incomplete.  It should also be an enumerated
   type rather than #defines.  */

#define LDO	0x0d
#define LDB	0x10
#define LDH	0x11
#define LDW	0x12
#define LDWM	0x13
#define STB	0x18
#define STH	0x19
#define STW	0x1a
#define STWM	0x1b
#define COMICLR	0x24
#define SUBI	0x25
#define SUBIO	0x25
#define ADDIT	0x2c
#define ADDITO	0x2c
#define ADDI	0x2d
#define ADDIO	0x2d
#define LDIL	0x08
#define ADDIL	0x0a

#define MOVB	0x32
#define MOVIB	0x33
#define COMBT	0x20
#define COMBF	0x22
#define COMIBT	0x21
#define COMIBF	0x23
#define ADDBT	0x28
#define ADDBF	0x2a
#define ADDIBT	0x29
#define ADDIBF	0x2b
#define BVB	0x30
#define BB	0x31

#define BL	0x3a
#define BLE	0x39
#define BE	0x38

#define CMPBDT	0x27
#define CMPBDF	0x2f
#define CMPIBD	0x3b
#define LDD	0x14
#define STD	0x1c
#define LDWL	0x17
#define STWL	0x1f
#define FLDW    0x16
#define FSTW    0x1e

/* Given a machine instruction, return its format.

   FIXME:  opcodes which do not map to a known format
   should return an error of some sort.  */

static INLINE int
bfd_hppa_insn2fmt (insn)
     unsigned int insn;
{
  unsigned char op = get_opcode (insn);

  switch (op)
    {
    case ADDI:
    case ADDIT:
    case SUBI:
      return 11;

    case MOVB:
    case MOVIB:
    case COMBT:
    case COMBF:
    case COMIBT:
    case COMIBF:
    case ADDBT:
    case ADDBF:
    case ADDIBT:
    case ADDIBF:
    case BVB:
    case BB:
    case CMPBDT:
    case CMPBDF:
    case CMPIBD:
      return 12;

    case LDO:
    case LDB:
    case LDH:
    case LDW:
    case LDWM:
    case STB:
    case STH:
    case STW:
    case STWM:
      return 14;

    case LDWL:
    case STWL:
    case FLDW:
    case FSTW:
      /* This is a hack.  Unfortunately, format 11 is already taken
	 and we're using integers rather than an enum, so it's hard
	 to describe the 11a format.  */
      return -11;

    case LDD:
    case STD:
      return 10;

    case BL:
    case BE:
    case BLE:
      if ((insn & 0x00008000) != 0)
	return 22;
      return 17;

    case LDIL:
    case ADDIL:
      return 21;

    default:
      break;
    }
  return 32;
}


/* Insert VALUE into INSN using R_FORMAT to determine exactly what
   bits to change.  */

static INLINE unsigned int
hppa_rebuild_insn (abfd, insn, value, r_format)
     bfd *abfd ATTRIBUTE_UNUSED;
     unsigned int insn;
     unsigned int value;
     unsigned int r_format;
{
  switch (r_format)
    {
    case 11: return (insn & ~ 0x7ff) | low_sign_unext (value, 11);
    case 12: return re_assemble_12 (insn & ~ 0x1ffd, value);
    case 14: return (insn & ~ 0x3fff) | low_sign_unext (value, 14);
    case 17: return re_assemble_17 (insn & ~ 0x1f1ffd, value);
    case 21: return re_assemble_21 (insn & ~ 0x1fffff, value);
    case 32: return value;

    default:
      abort ();
    }
  return insn;
}

#endif /* _HPPA_H */
