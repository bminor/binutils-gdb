/* Assembler interface for targets using CGEN. -*- C -*-
   CGEN: Cpu tools GENerator

THIS FILE IS USED TO GENERATE fr30-asm.c.

Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

This file is part of the GNU Binutils and GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "sysdep.h"
#include <ctype.h>
#include <stdio.h>
#include "ansidecl.h"
#include "bfd.h"
#include "symcat.h"
#include "fr30-opc.h"
#include "opintl.h"

#undef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#undef max
#define max(a,b) ((a) > (b) ? (a) : (b))

#undef INLINE
#ifdef __GNUC__
#define INLINE __inline__
#else
#define INLINE
#endif

/* Used by the ifield rtx function.  */
#define FLD(f) (fields->f)

static const char * insert_normal
     PARAMS ((CGEN_OPCODE_DESC, long, unsigned int, unsigned int, unsigned int,
	      unsigned int, unsigned int, unsigned int, CGEN_INSN_BYTES_PTR));
static const char * parse_insn_normal
     PARAMS ((CGEN_OPCODE_DESC, const CGEN_INSN *,
	      const char **, CGEN_FIELDS *));
static const char * insert_insn_normal
     PARAMS ((CGEN_OPCODE_DESC, const CGEN_INSN *,
	      CGEN_FIELDS *, CGEN_INSN_BYTES_PTR, bfd_vma));

/* -- assembler routines inserted here */
/* -- asm.c */
/* Handle register lists for LDMx and STMx  */

static const char *
parse_low_register_list (od, strp, opindex, valuep)
     CGEN_OPCODE_DESC od;
     const char **strp;
     int opindex;
     unsigned long *valuep;
{
  *valuep = 0;
  while (**strp && **strp != ')')
    {
      ++*strp;
    }

  if (!*strp)
    return "Register list is not valid";

  return NULL;
}

static const char *
parse_hi_register_list (od, strp, opindex, valuep)
     CGEN_OPCODE_DESC od;
     const char **strp;
     int opindex;
     unsigned long *valuep;
{
  return parse_low_register_list (od, strp, opindex, valuep);
}

/* -- */

/* Main entry point for operand parsing.

   This function is basically just a big switch statement.  Earlier versions
   used tables to look up the function to use, but
   - if the table contains both assembler and disassembler functions then
     the disassembler contains much of the assembler and vice-versa,
   - there's a lot of inlining possibilities as things grow,
   - using a switch statement avoids the function call overhead.

   This function could be moved into `parse_insn_normal', but keeping it
   separate makes clear the interface between `parse_insn_normal' and each of
   the handlers.
*/

const char *
fr30_cgen_parse_operand (od, opindex, strp, fields)
     CGEN_OPCODE_DESC od;
     int opindex;
     const char ** strp;
     CGEN_FIELDS * fields;
{
  const char * errmsg;

  switch (opindex)
    {
    case FR30_OPERAND_RI :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_gr, & fields->f_Ri);
      break;
    case FR30_OPERAND_RJ :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_gr, & fields->f_Rj);
      break;
    case FR30_OPERAND_RIC :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_gr, & fields->f_Ric);
      break;
    case FR30_OPERAND_RJC :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_gr, & fields->f_Rjc);
      break;
    case FR30_OPERAND_CRI :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_cr, & fields->f_CRi);
      break;
    case FR30_OPERAND_CRJ :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_cr, & fields->f_CRj);
      break;
    case FR30_OPERAND_RS1 :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_dr, & fields->f_Rs1);
      break;
    case FR30_OPERAND_RS2 :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_dr, & fields->f_Rs2);
      break;
    case FR30_OPERAND_R13 :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_r13, & fields->f_nil);
      break;
    case FR30_OPERAND_R14 :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_r14, & fields->f_nil);
      break;
    case FR30_OPERAND_R15 :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_r15, & fields->f_nil);
      break;
    case FR30_OPERAND_PS :
      errmsg = cgen_parse_keyword (od, strp, & fr30_cgen_opval_h_ps, & fields->f_nil);
      break;
    case FR30_OPERAND_U4 :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_U4, &fields->f_u4);
      break;
    case FR30_OPERAND_U4C :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_U4C, &fields->f_u4c);
      break;
    case FR30_OPERAND_M4 :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_M4, &fields->f_m4);
      break;
    case FR30_OPERAND_U8 :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_U8, &fields->f_u8);
      break;
    case FR30_OPERAND_I8 :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_I8, &fields->f_i8);
      break;
    case FR30_OPERAND_UDISP6 :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_UDISP6, &fields->f_udisp6);
      break;
    case FR30_OPERAND_DISP8 :
      errmsg = cgen_parse_signed_integer (od, strp, FR30_OPERAND_DISP8, &fields->f_disp8);
      break;
    case FR30_OPERAND_DISP9 :
      errmsg = cgen_parse_signed_integer (od, strp, FR30_OPERAND_DISP9, &fields->f_disp9);
      break;
    case FR30_OPERAND_DISP10 :
      errmsg = cgen_parse_signed_integer (od, strp, FR30_OPERAND_DISP10, &fields->f_disp10);
      break;
    case FR30_OPERAND_S10 :
      errmsg = cgen_parse_signed_integer (od, strp, FR30_OPERAND_S10, &fields->f_s10);
      break;
    case FR30_OPERAND_U10 :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_U10, &fields->f_u10);
      break;
    case FR30_OPERAND_I32 :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_I32, &fields->f_i32);
      break;
    case FR30_OPERAND_I20 :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_I20, &fields->f_i20);
      break;
    case FR30_OPERAND_LABEL9 :
      {
        bfd_vma value;
        errmsg = cgen_parse_address (od, strp, FR30_OPERAND_LABEL9, 0, NULL,  & value);
        fields->f_rel9 = value;
      }
      break;
    case FR30_OPERAND_DIR8 :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_DIR8, &fields->f_dir8);
      break;
    case FR30_OPERAND_DIR9 :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_DIR9, &fields->f_dir9);
      break;
    case FR30_OPERAND_DIR10 :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_DIR10, &fields->f_dir10);
      break;
    case FR30_OPERAND_LABEL12 :
      {
        bfd_vma value;
        errmsg = cgen_parse_address (od, strp, FR30_OPERAND_LABEL12, 0, NULL,  & value);
        fields->f_rel12 = value;
      }
      break;
    case FR30_OPERAND_REGLIST_LOW :
      errmsg = parse_low_register_list (od, strp, FR30_OPERAND_REGLIST_LOW, &fields->f_reglist_low);
      break;
    case FR30_OPERAND_REGLIST_HI :
      errmsg = parse_hi_register_list (od, strp, FR30_OPERAND_REGLIST_HI, &fields->f_reglist_hi);
      break;
    case FR30_OPERAND_CC :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_CC, &fields->f_cc);
      break;
    case FR30_OPERAND_CCC :
      errmsg = cgen_parse_unsigned_integer (od, strp, FR30_OPERAND_CCC, &fields->f_ccc);
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while parsing.\n"), opindex);
      abort ();
  }

  return errmsg;
}

/* Main entry point for operand insertion.

   This function is basically just a big switch statement.  Earlier versions
   used tables to look up the function to use, but
   - if the table contains both assembler and disassembler functions then
     the disassembler contains much of the assembler and vice-versa,
   - there's a lot of inlining possibilities as things grow,
   - using a switch statement avoids the function call overhead.

   This function could be moved into `parse_insn_normal', but keeping it
   separate makes clear the interface between `parse_insn_normal' and each of
   the handlers.  It's also needed by GAS to insert operands that couldn't be
   resolved during parsing.
*/

const char *
fr30_cgen_insert_operand (od, opindex, fields, buffer, pc)
     CGEN_OPCODE_DESC od;
     int opindex;
     CGEN_FIELDS * fields;
     CGEN_INSN_BYTES_PTR buffer;
     bfd_vma pc;
{
  const char * errmsg;
  unsigned int total_length = CGEN_FIELDS_BITSIZE (fields);

  switch (opindex)
    {
    case FR30_OPERAND_RI :
      errmsg = insert_normal (od, fields->f_Ri, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 12, 4, 16, total_length, buffer);
      break;
    case FR30_OPERAND_RJ :
      errmsg = insert_normal (od, fields->f_Rj, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 4, 16, total_length, buffer);
      break;
    case FR30_OPERAND_RIC :
      errmsg = insert_normal (od, fields->f_Ric, 0|(1<<CGEN_OPERAND_UNSIGNED), 16, 12, 4, 16, total_length, buffer);
      break;
    case FR30_OPERAND_RJC :
      errmsg = insert_normal (od, fields->f_Rjc, 0|(1<<CGEN_OPERAND_UNSIGNED), 16, 8, 4, 16, total_length, buffer);
      break;
    case FR30_OPERAND_CRI :
      errmsg = insert_normal (od, fields->f_CRi, 0|(1<<CGEN_OPERAND_UNSIGNED), 16, 12, 4, 16, total_length, buffer);
      break;
    case FR30_OPERAND_CRJ :
      errmsg = insert_normal (od, fields->f_CRj, 0|(1<<CGEN_OPERAND_UNSIGNED), 16, 8, 4, 16, total_length, buffer);
      break;
    case FR30_OPERAND_RS1 :
      errmsg = insert_normal (od, fields->f_Rs1, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 4, 16, total_length, buffer);
      break;
    case FR30_OPERAND_RS2 :
      errmsg = insert_normal (od, fields->f_Rs2, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 12, 4, 16, total_length, buffer);
      break;
    case FR30_OPERAND_R13 :
      errmsg = insert_normal (od, fields->f_nil, 0, 0, 0, 0, 0, total_length, buffer);
      break;
    case FR30_OPERAND_R14 :
      errmsg = insert_normal (od, fields->f_nil, 0, 0, 0, 0, 0, total_length, buffer);
      break;
    case FR30_OPERAND_R15 :
      errmsg = insert_normal (od, fields->f_nil, 0, 0, 0, 0, 0, total_length, buffer);
      break;
    case FR30_OPERAND_PS :
      errmsg = insert_normal (od, fields->f_nil, 0, 0, 0, 0, 0, total_length, buffer);
      break;
    case FR30_OPERAND_U4 :
      errmsg = insert_normal (od, fields->f_u4, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 4, 16, total_length, buffer);
      break;
    case FR30_OPERAND_U4C :
      errmsg = insert_normal (od, fields->f_u4c, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 0, 12, 4, 16, total_length, buffer);
      break;
    case FR30_OPERAND_M4 :
      {
        long value = fields->f_m4;
        value = ((value) & (15));
        errmsg = insert_normal (od, value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 4, 16, total_length, buffer);
      }
      break;
    case FR30_OPERAND_U8 :
      errmsg = insert_normal (od, fields->f_u8, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 8, 16, total_length, buffer);
      break;
    case FR30_OPERAND_I8 :
      errmsg = insert_normal (od, fields->f_i8, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 0, 4, 8, 16, total_length, buffer);
      break;
    case FR30_OPERAND_UDISP6 :
      {
        long value = fields->f_udisp6;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (od, value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 4, 16, total_length, buffer);
      }
      break;
    case FR30_OPERAND_DISP8 :
      errmsg = insert_normal (od, fields->f_disp8, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGNED), 0, 4, 8, 16, total_length, buffer);
      break;
    case FR30_OPERAND_DISP9 :
      {
        long value = fields->f_disp9;
        value = ((int) (value) >> (1));
        errmsg = insert_normal (od, value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGNED), 0, 4, 8, 16, total_length, buffer);
      }
      break;
    case FR30_OPERAND_DISP10 :
      {
        long value = fields->f_disp10;
        value = ((int) (value) >> (2));
        errmsg = insert_normal (od, value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGNED), 0, 4, 8, 16, total_length, buffer);
      }
      break;
    case FR30_OPERAND_S10 :
      {
        long value = fields->f_s10;
        value = ((int) (value) >> (2));
        errmsg = insert_normal (od, value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGNED), 0, 8, 8, 16, total_length, buffer);
      }
      break;
    case FR30_OPERAND_U10 :
      {
        long value = fields->f_u10;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (od, value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 8, 16, total_length, buffer);
      }
      break;
    case FR30_OPERAND_I32 :
      errmsg = insert_normal (od, fields->f_i32, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGN_OPT)|(1<<CGEN_OPERAND_UNSIGNED), 16, 0, 32, 32, total_length, buffer);
      break;
    case FR30_OPERAND_I20 :
      {
do {
  FLD (f_i20_4) = ((unsigned int) (FLD (f_i20)) >> (16));
  FLD (f_i20_16) = ((FLD (f_i20)) & (65535));
} while (0);
        errmsg = insert_normal (od, fields->f_i20_4, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED)|(1<<CGEN_OPERAND_VIRTUAL), 0, 8, 4, 16, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (od, fields->f_i20_16, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED)|(1<<CGEN_OPERAND_VIRTUAL), 16, 0, 16, 16, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case FR30_OPERAND_LABEL9 :
      {
        long value = fields->f_rel9;
        value = ((int) (((value) - (((pc) + (2))))) >> (1));
        errmsg = insert_normal (od, value, 0|(1<<CGEN_OPERAND_PCREL_ADDR)|(1<<CGEN_OPERAND_SIGNED), 0, 8, 8, 16, total_length, buffer);
      }
      break;
    case FR30_OPERAND_DIR8 :
      errmsg = insert_normal (od, fields->f_dir8, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 8, 16, total_length, buffer);
      break;
    case FR30_OPERAND_DIR9 :
      {
        long value = fields->f_dir9;
        value = ((unsigned int) (value) >> (1));
        errmsg = insert_normal (od, value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 8, 16, total_length, buffer);
      }
      break;
    case FR30_OPERAND_DIR10 :
      {
        long value = fields->f_dir10;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (od, value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 8, 16, total_length, buffer);
      }
      break;
    case FR30_OPERAND_LABEL12 :
      {
        long value = fields->f_rel12;
        value = ((int) (((value) - (((pc) & (-2))))) >> (1));
        errmsg = insert_normal (od, value, 0|(1<<CGEN_OPERAND_PCREL_ADDR)|(1<<CGEN_OPERAND_SIGNED), 0, 5, 11, 16, total_length, buffer);
      }
      break;
    case FR30_OPERAND_REGLIST_LOW :
      errmsg = insert_normal (od, fields->f_reglist_low, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 8, 16, total_length, buffer);
      break;
    case FR30_OPERAND_REGLIST_HI :
      errmsg = insert_normal (od, fields->f_reglist_hi, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 8, 16, total_length, buffer);
      break;
    case FR30_OPERAND_CC :
      errmsg = insert_normal (od, fields->f_cc, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 4, 4, 16, total_length, buffer);
      break;
    case FR30_OPERAND_CCC :
      errmsg = insert_normal (od, fields->f_ccc, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 16, 0, 8, 16, total_length, buffer);
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while building insn.\n"),
	       opindex);
      abort ();
  }

  return errmsg;
}

cgen_parse_fn * const fr30_cgen_parse_handlers[] = 
{
  0, /* default */
  parse_insn_normal,
};

cgen_insert_fn * const fr30_cgen_insert_handlers[] = 
{
  0, /* default */
  insert_insn_normal,
};

void
fr30_cgen_init_asm (od)
     CGEN_OPCODE_DESC od;
{
}


#if ! CGEN_INT_INSN_P

/* Subroutine of insert_normal.  */

static INLINE void
insert_1 (od, value, start, length, word_length, bufp)
     CGEN_OPCODE_DESC od;
     unsigned long value;
     int start,length,word_length;
     unsigned char *bufp;
{
  unsigned long x,mask;
  int shift;
  int big_p = CGEN_OPCODE_INSN_ENDIAN (od) == CGEN_ENDIAN_BIG;

  switch (word_length)
    {
    case 8:
      x = *bufp;
      break;
    case 16:
      if (big_p)
	x = bfd_getb16 (bufp);
      else
	x = bfd_getl16 (bufp);
      break;
    case 24:
      /* ??? This may need reworking as these cases don't necessarily
	 want the first byte and the last two bytes handled like this.  */
      if (big_p)
	x = (bufp[0] << 16) | bfd_getb16 (bufp + 1);
      else
	x = bfd_getl16 (bufp) | (bufp[2] << 16);
      break;
    case 32:
      if (big_p)
	x = bfd_getb32 (bufp);
      else
	x = bfd_getl32 (bufp);
      break;
    default :
      abort ();
    }

  /* Written this way to avoid undefined behaviour.  */
  mask = (((1L << (length - 1)) - 1) << 1) | 1;
  if (CGEN_INSN_LSB0_P)
    shift = (start + 1) - length;
  else
    shift = (word_length - (start + length));
  x = (x & ~(mask << shift)) | ((value & mask) << shift);

  switch (word_length)
    {
    case 8:
      *bufp = x;
      break;
    case 16:
      if (big_p)
	bfd_putb16 (x, bufp);
      else
	bfd_putl16 (x, bufp);
      break;
    case 24:
      /* ??? This may need reworking as these cases don't necessarily
	 want the first byte and the last two bytes handled like this.  */
      if (big_p)
	{
	  bufp[0] = x >> 16;
	  bfd_putb16 (x, bufp + 1);
	}
      else
	{
	  bfd_putl16 (x, bufp);
	  bufp[2] = x >> 16;
	}
      break;
    case 32:
      if (big_p)
	bfd_putb32 (x, bufp);
      else
	bfd_putl32 (x, bufp);
      break;
    default :
      abort ();
    }
}

#endif /* ! CGEN_INT_INSN_P */

/* Default insertion routine.

   ATTRS is a mask of the boolean attributes.
   WORD_OFFSET is the offset in bits from the start of the insn of the value.
   WORD_LENGTH is the length of the word in bits in which the value resides.
   START is the starting bit number in the word, architecture origin.
   LENGTH is the length of VALUE in bits.
   TOTAL_LENGTH is the total length of the insn in bits.

   The result is an error message or NULL if success.  */

/* ??? This duplicates functionality with bfd's howto table and
   bfd_install_relocation.  */
/* ??? This doesn't handle bfd_vma's.  Create another function when
   necessary.  */

static const char *
insert_normal (od, value, attrs, word_offset, start, length, word_length,
	       total_length, buffer)
     CGEN_OPCODE_DESC od;
     long value;
     unsigned int attrs;
     unsigned int word_offset, start, length, word_length, total_length;
     CGEN_INSN_BYTES_PTR buffer;
{
  static char errbuf[100];
  /* Written this way to avoid undefined behaviour.  */
  unsigned long mask = (((1L << (length - 1)) - 1) << 1) | 1;

  /* If LENGTH is zero, this operand doesn't contribute to the value.  */
  if (length == 0)
    return NULL;

  if (CGEN_INT_INSN_P
      && word_offset != 0)
    abort ();

  if (word_length > 32)
    abort ();

  /* For architectures with insns smaller than the insn-base-bitsize,
     word_length may be too big.  */
#if CGEN_MIN_INSN_BITSIZE < CGEN_BASE_INSN_BITSIZE
  if (word_offset == 0
      && word_length > total_length)
    word_length = total_length;
#endif

  /* Ensure VALUE will fit.  */
  if ((attrs & CGEN_ATTR_MASK (CGEN_OPERAND_UNSIGNED)) != 0)
    {
      unsigned long maxval = mask;
      if ((unsigned long) value > maxval)
	{
	  /* xgettext:c-format */
	  sprintf (errbuf,
		   _("operand out of range (%lu not between 0 and %lu)"),
		   value, maxval);
	  return errbuf;
	}
    }
  else
    {
      long minval = - (1L << (length - 1));
      long maxval = (1L << (length - 1)) - 1;
      if (value < minval || value > maxval)
	{
	  sprintf
	    /* xgettext:c-format */
	    (errbuf, _("operand out of range (%ld not between %ld and %ld)"),
	     value, minval, maxval);
	  return errbuf;
	}
    }

#if CGEN_INT_INSN_P

  {
    int shift;

    if (CGEN_INSN_LSB0_P)
      shift = (start + 1) - length;
    else
      shift = word_length - (start + length);
    *buffer = (*buffer & ~(mask << shift)) | ((value & mask) << shift);
  }

#else /* ! CGEN_INT_INSN_P */

  {
    unsigned char *bufp = (unsigned char *) buffer + word_offset / 8;

    insert_1 (od, value, start, length, word_length, bufp);
  }

#endif /* ! CGEN_INT_INSN_P */

  return NULL;
}

/* Default insn parser.

   The syntax string is scanned and operands are parsed and stored in FIELDS.
   Relocs are queued as we go via other callbacks.

   ??? Note that this is currently an all-or-nothing parser.  If we fail to
   parse the instruction, we return 0 and the caller will start over from
   the beginning.  Backtracking will be necessary in parsing subexpressions,
   but that can be handled there.  Not handling backtracking here may get
   expensive in the case of the m68k.  Deal with later.

   Returns NULL for success, an error message for failure.
*/

static const char *
parse_insn_normal (od, insn, strp, fields)
     CGEN_OPCODE_DESC od;
     const CGEN_INSN * insn;
     const char ** strp;
     CGEN_FIELDS * fields;
{
  const CGEN_SYNTAX * syntax = CGEN_INSN_SYNTAX (insn);
  const char * str = *strp;
  const char * errmsg;
  const char * p;
  const unsigned char * syn;
#ifdef CGEN_MNEMONIC_OPERANDS
  /* FIXME: wip */
  int past_opcode_p;
#endif

  /* For now we assume the mnemonic is first (there are no leading operands).
     We can parse it without needing to set up operand parsing.
     GAS's input scrubber will ensure mnemonics are lowercase, but we may
     not be called from GAS.  */
  p = CGEN_INSN_MNEMONIC (insn);
  while (*p && tolower (*p) == tolower (*str))
    ++p, ++str;
  
  if (* p || (* str && !isspace (* str)))
    return _("unrecognized instruction");

  CGEN_INIT_PARSE (od);
  cgen_init_parse_operand (od);
#ifdef CGEN_MNEMONIC_OPERANDS
  past_opcode_p = 0;
#endif

  /* We don't check for (*str != '\0') here because we want to parse
     any trailing fake arguments in the syntax string.  */
  syn = CGEN_SYNTAX_STRING (syntax);

  /* Mnemonics come first for now, ensure valid string.  */
  if (! CGEN_SYNTAX_MNEMONIC_P (* syn))
    abort ();

  ++syn;

  while (* syn != 0)
    {
      /* Non operand chars must match exactly.  */
      if (CGEN_SYNTAX_CHAR_P (* syn))
	{
	  if (*str == CGEN_SYNTAX_CHAR (* syn))
	    {
#ifdef CGEN_MNEMONIC_OPERANDS
	      if (* syn == ' ')
		past_opcode_p = 1;
#endif
	      ++ syn;
	      ++ str;
	    }
	  else
	    {
	      /* Syntax char didn't match.  Can't be this insn.  */
	      /* FIXME: would like to return something like
		 "expected char `c'" */
	      return _("syntax error");
	    }
	  continue;
	}

      /* We have an operand of some sort.  */
      errmsg = fr30_cgen_parse_operand (od, CGEN_SYNTAX_FIELD (*syn),
					  &str, fields);
      if (errmsg)
	return errmsg;

      /* Done with this operand, continue with next one.  */
      ++ syn;
    }

  /* If we're at the end of the syntax string, we're done.  */
  if (* syn == '\0')
    {
      /* FIXME: For the moment we assume a valid `str' can only contain
	 blanks now.  IE: We needn't try again with a longer version of
	 the insn and it is assumed that longer versions of insns appear
	 before shorter ones (eg: lsr r2,r3,1 vs lsr r2,r3).  */
      while (isspace (* str))
	++ str;

      if (* str != '\0')
	return _("junk at end of line"); /* FIXME: would like to include `str' */

      return NULL;
    }

  /* We couldn't parse it.  */
  return _("unrecognized instruction");
}

/* Default insn builder (insert handler).
   The instruction is recorded in CGEN_INT_INSN_P byte order
   (meaning that if CGEN_INT_INSN_P BUFFER is an int * and thus the value is
   recorded in host byte order, otherwise BUFFER is an array of bytes and the
   value is recorded in target byte order).
   The result is an error message or NULL if success.  */

static const char *
insert_insn_normal (od, insn, fields, buffer, pc)
     CGEN_OPCODE_DESC od;
     const CGEN_INSN * insn;
     CGEN_FIELDS * fields;
     CGEN_INSN_BYTES_PTR buffer;
     bfd_vma pc;
{
  const CGEN_SYNTAX * syntax = CGEN_INSN_SYNTAX (insn);
  unsigned long value;
  const unsigned char * syn;

  CGEN_INIT_INSERT (od);
  value = CGEN_INSN_BASE_VALUE (insn);

  /* If we're recording insns as numbers (rather than a string of bytes),
     target byte order handling is deferred until later.  */

#if CGEN_INT_INSN_P

  *buffer = value;

#else

  cgen_put_insn_value (od, buffer, min (CGEN_BASE_INSN_BITSIZE,
					CGEN_FIELDS_BITSIZE (fields)),
		       value);

#endif /* ! CGEN_INT_INSN_P */

  /* ??? It would be better to scan the format's fields.
     Still need to be able to insert a value based on the operand though;
     e.g. storing a branch displacement that got resolved later.
     Needs more thought first.  */

  for (syn = CGEN_SYNTAX_STRING (syntax); * syn != '\0'; ++ syn)
    {
      const char *errmsg;

      if (CGEN_SYNTAX_CHAR_P (* syn))
	continue;

      errmsg = fr30_cgen_insert_operand (od, CGEN_SYNTAX_FIELD (*syn),
					   fields, buffer, pc);
      if (errmsg)
	return errmsg;
    }

  return NULL;
}

/* Main entry point.
   This routine is called for each instruction to be assembled.
   STR points to the insn to be assembled.
   We assume all necessary tables have been initialized.
   The assembled instruction, less any fixups, is stored in BUF.
   Remember that if CGEN_INT_INSN_P then BUF is an int and thus the value
   still needs to be converted to target byte order, otherwise BUF is an array
   of bytes in target byte order.
   The result is a pointer to the insn's entry in the opcode table,
   or NULL if an error occured (an error message will have already been
   printed).

   Note that when processing (non-alias) macro-insns,
   this function recurses.  */

const CGEN_INSN *
fr30_cgen_assemble_insn (od, str, fields, buf, errmsg)
     CGEN_OPCODE_DESC od;
     const char * str;
     CGEN_FIELDS * fields;
     CGEN_INSN_BYTES_PTR buf;
     char ** errmsg;
{
  const char * start;
  CGEN_INSN_LIST * ilist;

  /* Skip leading white space.  */
  while (isspace (* str))
    ++ str;

  /* The instructions are stored in hashed lists.
     Get the first in the list.  */
  ilist = CGEN_ASM_LOOKUP_INSN (od, str);

  /* Keep looking until we find a match.  */

  start = str;
  for ( ; ilist != NULL ; ilist = CGEN_ASM_NEXT_INSN (ilist))
    {
      const CGEN_INSN *insn = ilist->insn;

#if 0 /* not needed as unsupported opcodes shouldn't be in the hash lists */
      /* Is this insn supported by the selected cpu?  */
      if (! fr30_cgen_insn_supported (od, insn))
	continue;
#endif

      /* If the RELAX attribute is set, this is an insn that shouldn't be
	 chosen immediately.  Instead, it is used during assembler/linker
	 relaxation if possible.  */
      if (CGEN_INSN_ATTR (insn, CGEN_INSN_RELAX) != 0)
	continue;

      str = start;

      /* Allow parse/insert handlers to obtain length of insn.  */
      CGEN_FIELDS_BITSIZE (fields) = CGEN_INSN_BITSIZE (insn);

      if (! CGEN_PARSE_FN (insn) (od, insn, & str, fields))
	{
	  /* ??? 0 is passed for `pc' */
	  if (CGEN_INSERT_FN (insn) (od, insn, fields, buf, (bfd_vma) 0) != NULL)
	    continue;
	  /* It is up to the caller to actually output the insn and any
	     queued relocs.  */
	  return insn;
	}

      /* Try the next entry.  */
    }

  /* FIXME: We can return a better error message than this.
     Need to track why it failed and pick the right one.  */
  {
    static char errbuf[100];
    if (strlen (start) > 50)
      /* xgettext:c-format */
      sprintf (errbuf, _("bad instruction `%.50s...'"), start);
    else 
      /* xgettext:c-format */
      sprintf (errbuf, _("bad instruction `%.50s'"), start);
      
    *errmsg = errbuf;
    return NULL;
  }
}

#if 0 /* This calls back to GAS which we can't do without care.  */

/* Record each member of OPVALS in the assembler's symbol table.
   This lets GAS parse registers for us.
   ??? Interesting idea but not currently used.  */

/* Record each member of OPVALS in the assembler's symbol table.
   FIXME: Not currently used.  */

void
fr30_cgen_asm_hash_keywords (od, opvals)
     CGEN_OPCODE_DESC od;
     CGEN_KEYWORD * opvals;
{
  CGEN_KEYWORD_SEARCH search = cgen_keyword_search_init (opvals, NULL);
  const CGEN_KEYWORD_ENTRY * ke;

  while ((ke = cgen_keyword_search_next (& search)) != NULL)
    {
#if 0 /* Unnecessary, should be done in the search routine.  */
      if (! fr30_cgen_opval_supported (ke))
	continue;
#endif
      cgen_asm_record_register (od, ke->name, ke->value);
    }
}

#endif /* 0 */
